"""Bounded source checks for the precision116 hosted gate; no CI or crypto run."""
import ast
import json
from pathlib import Path
import re
import subprocess
import unittest


ROOT = Path(__file__).resolve().parents[2]
WORKFLOW = ".github/workflows/dcp-rcb.yml"
BASE = "dbbbee0d20d8a7ae3c138e42f633414db621a173"
RED = "refs/heads/codex/precision116-profile-seam-red-20260907"
GREEN = "refs/heads/codex/precision116-profile-seam-green-20260907"
NEW = "experimental_precision116_profile_seam"
PAPER = "paper_full_eight_square_contract"


def parse_yaml(text):
    # System Ruby's standard YAML parser is available; no dependency installation.
    result = subprocess.run(
        ["ruby", "-r", "yaml", "-r", "json", "-e",
         "puts JSON.generate(YAML.load(STDIN.read))"],
        input=text, text=True, capture_output=True, check=True,
    )
    return json.loads(result.stdout)


BASELINE = parse_yaml(subprocess.check_output(
    ["git", "-C", str(ROOT), "show", f"{BASE}:{WORKFLOW}"], text=True))
CURRENT = parse_yaml((ROOT / WORKFLOW).read_text())


def enabled(step, ref, success=True, selected="success"):
    expression = step.get("if", "true").strip().removeprefix("${{").removesuffix("}}").strip()
    if "always()" not in expression and not success:
        return False
    expression = expression.replace("always()", "True").replace("true", "True")
    expression = expression.replace("github.ref", repr(ref))
    expression = expression.replace("steps.endpoint-select.outcome", repr(selected))
    expression = expression.replace("&&", " and ").replace("||", " or ")
    tree = ast.parse(expression, mode="eval")
    allowed = (ast.Expression, ast.BoolOp, ast.And, ast.Or, ast.Compare,
               ast.Eq, ast.NotEq, ast.Constant, ast.Load)
    if not all(isinstance(node, allowed) for node in ast.walk(tree)):
        raise ValueError("condition outside the bounded boolean evaluator")
    return eval(compile(tree, "<workflow-condition>", "eval"), {"__builtins__": {}})


class WiringContract(unittest.TestCase):
    def test_only_two_new_trigger_refs(self):
        old = BASELINE["true"]["push"]["branches"]
        new = CURRENT["true"]["push"]["branches"]
        self.assertEqual(new, old + [RED.removeprefix("refs/heads/"), GREEN.removeprefix("refs/heads/")])

    def test_legacy_selection_excludes_unbuilt_experimental_test(self):
        names = re.findall(r"add_test\(NAME\s+(\S+)", (ROOT / "CMakeLists.txt").read_text())
        original_names = [name for name in names if name != NEW]
        self.assertEqual(len(original_names), 61)
        for key, job in CURRENT["jobs"].items():
            old_steps = {s["name"]: s for s in BASELINE["jobs"][key]["steps"]}
            for step in job["steps"]:
                if step["name"] not in ("Run legacy 57-test checkpoint", "Run complete 60-test three-track suite"):
                    continue
                patterns = re.findall(r"-E '([^']+)'", step["run"])
                old_patterns = re.findall(r"-E '([^']+)'", old_steps[step["name"]]["run"])
                self.assertEqual(len(patterns), 2)
                for pattern, old_pattern in zip(patterns, old_patterns):
                    self.assertIsNotNone(re.search(pattern, NEW))
                    self.assertEqual([n for n in original_names if not re.search(pattern, n)],
                                     [n for n in original_names if not re.search(old_pattern, n)])

    def test_no_endpoint_activity_on_experimental_refs_even_after_failure(self):
        endpoint_names = ("Run draft endpoint observer self-test once", "Run and finalize paper endpoint once",
                          "Select exact endpoint upload", "Upload exact endpoint evidence")
        for job in CURRENT["jobs"].values():
            for step in job["steps"]:
                if step["name"] in endpoint_names:
                    for ref in (RED, GREEN):
                        for success in (False, True):
                            for selected in ("success", "failure", "skipped"):
                                self.assertFalse(enabled(step, ref, success, selected), (step["name"], ref, success))

    def test_new_mode_is_green_only_and_rejects_empty_selection(self):
        for job in CURRENT["jobs"].values():
            steps = job["steps"]
            mode = next(s for s in steps if s["name"] == "Run experimental precision116 profile seam once")
            self.assertIn("--no-tests=error", mode["run"])
            self.assertIn("-R '^experimental_precision116_profile_seam$'", mode["run"])
            self.assertEqual(mode["env"]["OMP_NUM_THREADS"], 2)
            self.assertEqual(mode["timeout-minutes"], 25)
            self.assertTrue(enabled(mode, GREEN))
            self.assertFalse(enabled(mode, RED))
            self.assertFalse(enabled(mode, GREEN, success=False))
            reject = next(s for s in steps if s["name"] == "Reject unexpected precision116 RED build success")
            self.assertIn("exit 1", reject["run"])
            self.assertTrue(enabled(reject, RED))
            self.assertFalse(enabled(reject, GREEN))
            build = next(s for s in steps if s["name"] == "Build paper full eight-square contract")
            self.assertLess(steps.index(build), steps.index(reject))
            self.assertLess(steps.index(reject), steps.index(mode))

    def test_existing_steps_and_branches_preserved(self):
        extra = {"Reject unexpected precision116 RED build success", "Run experimental precision116 profile seam once"}
        for key, job in CURRENT["jobs"].items():
            original = BASELINE["jobs"][key]
            self.assertEqual({k: v for k, v in job.items() if k != "steps"},
                             {k: v for k, v in original.items() if k != "steps"})
            retained = [s for s in job["steps"] if s["name"] not in extra]
            self.assertEqual([s["name"] for s in retained], [s["name"] for s in original["steps"]])
            for step, old in zip(retained, original["steps"]):
                for field in set(step) | set(old):
                    if field == "if" or (field == "run" and "-E '" in old.get("run", "")):
                        continue
                    self.assertEqual(step.get(field), old.get(field), (step["name"], field))
                if step.get("if") == old.get("if"):
                    continue
                for branch in BASELINE["true"]["push"]["branches"]:
                    for success in (False, True):
                        for selected in ("success", "failure", "skipped"):
                            self.assertEqual(enabled(step, "refs/heads/" + branch, success, selected),
                                             enabled(old, "refs/heads/" + branch, success, selected))


if __name__ == "__main__":
    unittest.main(verbosity=2)
