"""Source-level contract for the S100 diagnostic-ref CI exclusions.

This parses the checked-in workflow but does not configure, compile, run CTest,
dispatch Actions, or exercise cryptographic code.
"""

from copy import deepcopy
import json
from pathlib import Path
import re
import subprocess
import unittest


ROOT = Path(__file__).resolve().parents[2]
WORKFLOW = ".github/workflows/dcp-rcb.yml"
BASE_HEAD = "155b40fbb312d3f000e0bc6d72685c11185720b2"
NEW_REFS = (
    "refs/heads/codex/s100-fresh-error-repair-20260907",
    "refs/heads/codex/s100-fresh-error-red-20260907",
)
STEP_NAMES = (
    "Build paper full eight-square contract",
    "Run and finalize paper endpoint once",
    "Select exact endpoint upload",
    "Upload exact endpoint evidence",
)
JOB_NAMES = ("linux-gcc", "windows-mingw64")


def parse_yaml(text):
    """Parse YAML through the bounded system Ruby/Psych dependency."""
    result = subprocess.run(
        [
            "ruby",
            "-r",
            "yaml",
            "-r",
            "json",
            "-e",
            "puts JSON.generate(YAML.safe_load(STDIN.read, aliases: false))",
        ],
        input=text,
        text=True,
        capture_output=True,
        check=True,
    )
    return json.loads(result.stdout)


def condition_terms(step):
    expression = step.get("if", "true").strip()
    if expression.startswith("${{") and expression.endswith("}}"):
        expression = expression[3:-2].strip()
    return [term.strip() for term in expression.split("&&")]


def condition_enabled(step, ref, prior_success=True, selected_outcome="success"):
    """Evaluate only the small conjunction grammar used by the four steps."""
    terms = condition_terms(step)
    if "always()" not in terms and not prior_success:
        return False

    values = []
    for term in terms:
        if term in ("true", "always()"):
            values.append(True)
            continue
        match = re.fullmatch(
            r"(github\.ref|steps\.endpoint-select\.outcome)\s*(==|!=)\s*'([^']*)'",
            term,
        )
        if match is None:
            raise ValueError(f"condition outside bounded grammar: {term!r}")
        subject, operator, literal = match.groups()
        actual = ref if subject == "github.ref" else selected_outcome
        values.append(actual == literal if operator == "==" else actual != literal)
    return all(values)


def load_baseline():
    text = subprocess.check_output(
        ["git", "-C", str(ROOT), "show", f"{BASE_HEAD}:{WORKFLOW}"], text=True
    )
    return parse_yaml(text)


def target_steps(workflow):
    found = {}
    for job_name in JOB_NAMES:
        job = workflow["jobs"][job_name]
        for step in job["steps"]:
            name = step.get("name")
            if name in STEP_NAMES:
                key = (job_name, name)
                if key in found:
                    raise AssertionError(f"duplicate target step: {key}")
                found[key] = step
    expected = {(job, name) for job in JOB_NAMES for name in STEP_NAMES}
    if set(found) != expected:
        raise AssertionError(
            f"expected all eight target steps; missing={sorted(expected - set(found))}, "
            f"extra={sorted(set(found) - expected)}"
        )
    return found


def assert_gate_contract(testcase, baseline, current):
    before = target_steps(baseline)
    after = target_steps(current)
    exclusions = [f"github.ref != '{ref}'" for ref in NEW_REFS]

    for key in sorted(after):
        testcase.assertEqual(
            condition_terms(after[key]),
            condition_terms(before[key]) + exclusions,
            f"{key} must retain its old condition and append both exact-ref exclusions",
        )

    # No diagnostic push activation: these refs remain workflow_dispatch-only.
    testcase.assertEqual(current["true"]["push"]["branches"],
                         baseline["true"]["push"]["branches"])


BASELINE = load_baseline()
CURRENT = parse_yaml((ROOT / WORKFLOW).read_text())


class S100RefGateContract(unittest.TestCase):
    def test_all_eight_steps_append_both_exact_ref_exclusions(self):
        assert_gate_contract(self, BASELINE, CURRENT)

    def test_new_refs_skip_all_eight_steps_for_every_status_combination(self):
        for key, step in target_steps(CURRENT).items():
            for ref in NEW_REFS:
                for prior_success in (False, True):
                    for outcome in ("success", "failure", "skipped"):
                        self.assertFalse(
                            condition_enabled(step, ref, prior_success, outcome),
                            (key, ref, prior_success, outcome),
                        )

    def test_existing_ref_and_status_truth_table_is_unchanged(self):
        before = target_steps(BASELINE)
        after = target_steps(CURRENT)
        refs = [
            "refs/heads/" + branch
            for branch in BASELINE["true"]["push"]["branches"]
        ]
        refs.append("refs/heads/workflow-dispatch-unlisted-ref")
        for key in sorted(after):
            for ref in refs:
                for prior_success in (False, True):
                    for outcome in ("success", "failure", "skipped"):
                        self.assertEqual(
                            condition_enabled(after[key], ref, prior_success, outcome),
                            condition_enabled(before[key], ref, prior_success, outcome),
                            (key, ref, prior_success, outcome),
                        )

    def test_each_single_gate_omission_is_detected(self):
        for key in sorted(target_steps(CURRENT)):
            for ref in NEW_REFS:
                mutant = deepcopy(CURRENT)
                step = target_steps(mutant)[key]
                omitted = f" && github.ref != '{ref}'"
                self.assertIn(omitted, step["if"], (key, ref))
                step["if"] = step["if"].replace(omitted, "", 1)
                with self.assertRaises(AssertionError, msg=(key, ref)):
                    assert_gate_contract(self, BASELINE, mutant)


if __name__ == "__main__":
    unittest.main(verbosity=2)
