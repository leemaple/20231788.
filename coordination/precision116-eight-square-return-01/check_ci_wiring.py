"""Bounded workflow-source tests; no configure, compiler, CTest or crypto execution."""
import hashlib
from pathlib import Path
import re
import runpy
import subprocess
import unittest

ROOT = Path(__file__).resolve().parents[2]
HELPER = ROOT / "coordination/precision116-pro-handoff-01/check_ci_wiring.py"
if hashlib.sha256(HELPER.read_bytes()).hexdigest() != "4d077514f526bbd1a93bcaf1ce9a24c46deb9f8a3bb9a69435dda836592da049":
    raise RuntimeError("reviewed YAML/condition helper changed")
HELPERS = runpy.run_path(str(HELPER), run_name="source_helpers_only")
parse_yaml, enabled = HELPERS["parse_yaml"], HELPERS["enabled"]
WORKFLOW = ".github/workflows/dcp-rcb.yml"
BASE = "2a6be7a1ec4528718df47d7c5d6366b3904b3036"
REF = "refs/heads/codex/precision116-eight-square-observation-20260907"
WORKING = "codex/precision116-eight-square-20260907"
NAME = "experimental_precision116_eight_square_contract"
OPTION = "OPENFHE_2023_1788_ENABLE_EXPERIMENTAL_PRECISION116_EIGHT_SQUARE"
EXTRA = ("Configure opt-in precision116 eight-square test",
         "Build opt-in precision116 eight-square test",
         "Run experimental precision116 eight-square observation once")
BASELINE = parse_yaml(subprocess.check_output(
    ["git", "-C", str(ROOT), "show", f"{BASE}:{WORKFLOW}"], text=True))
CURRENT = parse_yaml((ROOT / WORKFLOW).read_text())


class EightSquareWiring(unittest.TestCase):
    def test_one_new_activation_ref_and_working_ref_stays_untriggered(self):
        old = BASELINE["true"]["push"]["branches"]
        new = CURRENT["true"]["push"]["branches"]
        self.assertEqual(new, old + [REF.removeprefix("refs/heads/")])
        self.assertNotIn(WORKING, new)

    def test_separate_opt_in_build_after_unchanged_legacy_suite(self):
        for job in CURRENT["jobs"].values():
            steps = job["steps"]
            names = [s["name"] for s in steps]
            config, build, run = [next(s for s in steps if s["name"] == n) for n in EXTRA]
            self.assertLess(names.index("Run complete 60-test three-track suite"), names.index(EXTRA[0]))
            self.assertLess(names.index(EXTRA[0]), names.index(EXTRA[1]))
            self.assertLess(names.index(EXTRA[1]), names.index(EXTRA[2]))
            self.assertIn(f"-D{OPTION}=ON", config["run"])
            self.assertIn("precision116-eight-square", config["run"])
            self.assertIn("--target experimental_precision116_eight_square_test --parallel 2", build["run"])
            for step in (config, build, run):
                self.assertTrue(enabled(step, REF))
                self.assertFalse(enabled(step, REF, success=False))
                self.assertFalse(enabled(step, "refs/heads/" + WORKING))
                self.assertNotIn("continue-on-error", step)

    def test_exact_nonempty_single_ctest_and_timeout_headroom(self):
        for key, job in CURRENT["jobs"].items():
            run = next(s for s in job["steps"] if s["name"] == EXTRA[2])
            self.assertEqual(run["timeout-minutes"], 25)
            self.assertEqual(run["env"]["OMP_NUM_THREADS"], 2)
            self.assertEqual(run["run"].count("ctest --test-dir"), 1)
            self.assertIn("--no-tests=error", run["run"])
            self.assertIn(f"-R '^{NAME}$'", run["run"])
            self.assertIn("--verbose --output-on-failure", run["run"])
            self.assertNotRegex(run["run"], r"--repeat|until-pass|set \+e|\|\|")
            if key == "windows-mingw64":
                self.assertEqual(run["shell"], "msys2 {0}")
                self.assertEqual(run["working-directory"], "C:/openfhe-2023-1788/cleanroom")
                self.assertIn('export PATH="$prefix/bin:$prefix/lib:$PATH"', run["run"])

    def test_no_old_paper_build_or_endpoint_or_one_op_on_new_ref(self):
        excluded = ("Build paper full eight-square contract", "Run and finalize paper endpoint once",
                    "Select exact endpoint upload", "Upload exact endpoint evidence",
                    "Run draft endpoint observer self-test once", "Run experimental precision116 profile seam once",
                    "Reject unexpected precision116 RED build success")
        for job in CURRENT["jobs"].values():
            for step in job["steps"]:
                if step["name"] in excluded:
                    for success in (False, True):
                        for selected in ("success", "failure", "skipped"):
                            self.assertFalse(enabled(step, REF, success, selected), step["name"])

    def test_all_existing_steps_and_old_branch_behavior_preserved(self):
        for key, job in CURRENT["jobs"].items():
            old = BASELINE["jobs"][key]
            self.assertEqual({k: v for k, v in job.items() if k != "steps"},
                             {k: v for k, v in old.items() if k != "steps"})
            retained = [s for s in job["steps"] if s["name"] not in EXTRA]
            self.assertEqual([s["name"] for s in retained], [s["name"] for s in old["steps"]])
            for step, before in zip(retained, old["steps"]):
                self.assertEqual({k: v for k, v in step.items() if k != "if"},
                                 {k: v for k, v in before.items() if k != "if"})
                if step.get("if") == before.get("if"):
                    continue  # Exact equality already covers unrelated cache predicates.
                for branch in BASELINE["true"]["push"]["branches"]:
                    for success in (False, True):
                        for selected in ("success", "failure", "skipped"):
                            self.assertEqual(enabled(step, "refs/heads/" + branch, success, selected),
                                             enabled(before, "refs/heads/" + branch, success, selected))
            for step in job["steps"]:
                if step["name"] in EXTRA:
                    for branch in BASELINE["true"]["push"]["branches"]:
                        self.assertFalse(enabled(step, "refs/heads/" + branch))


if __name__ == "__main__":
    unittest.main(verbosity=2)
