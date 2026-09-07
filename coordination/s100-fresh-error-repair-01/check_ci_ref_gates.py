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
WIRING_BASE_HEAD = "a9299749ea1e8388adc23aa0ec1dba3c287e54ff"
CONTROLS_BASE_HEAD = "724a43cec317d08c6699fd89671b10cc4cd3e9cb"
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
DIAGNOSTIC_STEP_NAMES = (
    "Configure S100 fresh-error diagnostic build",
    "Build S100 fresh-error diagnostic",
    "Reject unexpected S100 RED build success",
    "Run S100 encoding inspection contract once",
    "Run S100 fresh-error diagnostic once",
)


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


def condition_enabled(
        step, ref, prior_success=True, selected_outcome="success", s100_scope=""):
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
            r"(github\.ref|steps\.endpoint-select\.outcome|inputs\.s100_scope)"
            r"\s*(==|!=)\s*'([^']*)'",
            term,
        )
        if match is None:
            raise ValueError(f"condition outside bounded grammar: {term!r}")
        subject, operator, literal = match.groups()
        actual = {
            "github.ref": ref,
            "steps.endpoint-select.outcome": selected_outcome,
            "inputs.s100_scope": s100_scope,
        }[subject]
        values.append(actual == literal if operator == "==" else actual != literal)
    return all(values)


def load_at(commit):
    text = subprocess.check_output(
        ["git", "-C", str(ROOT), "show", f"{commit}:{WORKFLOW}"], text=True
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


BASELINE = load_at(BASE_HEAD)
WIRING_BASELINE = load_at(WIRING_BASE_HEAD)
CONTROLS_BASELINE = load_at(CONTROLS_BASE_HEAD)
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


class S100DiagnosticWiringContract(unittest.TestCase):
    def diagnostic_steps(self):
        steps = CURRENT["jobs"]["linux-gcc"]["steps"]
        found = {step.get("name"): step for step in steps
                 if step.get("name") in DIAGNOSTIC_STEP_NAMES}
        self.assertEqual(set(found), set(DIAGNOSTIC_STEP_NAMES))
        self.assertEqual(sum(step.get("name") in DIAGNOSTIC_STEP_NAMES for step in steps), 5)
        return steps, found

    def test_existing_workflow_is_preserved_around_linux_additions_and_windows_guard(self):
        self.assertEqual(CURRENT["true"]["push"]["branches"],
                         WIRING_BASELINE["true"]["push"]["branches"])

        current_linux = CURRENT["jobs"]["linux-gcc"]
        before_linux = WIRING_BASELINE["jobs"]["linux-gcc"]
        self.assertEqual({k: v for k, v in current_linux.items() if k != "steps"},
                         {k: v for k, v in before_linux.items() if k != "steps"})
        retained = [step for step in current_linux["steps"]
                    if step.get("name") not in DIAGNOSTIC_STEP_NAMES]
        self.assertEqual(retained, before_linux["steps"])

        current_windows = CURRENT["jobs"]["windows-mingw64"]
        before_windows = WIRING_BASELINE["jobs"]["windows-mingw64"]
        self.assertEqual({k: v for k, v in current_windows.items() if k != "if"},
                         before_windows)
        self.assertEqual(
            condition_terms(current_windows),
            [f"github.ref != '{ref}'" for ref in NEW_REFS],
        )
        self.assertFalse(any(step.get("name") in DIAGNOSTIC_STEP_NAMES
                             for step in current_windows["steps"]))

    def test_separate_opt_in_build_follows_default_library_and_suite(self):
        steps, found = self.diagnostic_steps()
        names = [step["name"] for step in steps]
        configure = found[DIAGNOSTIC_STEP_NAMES[0]]
        build = found[DIAGNOSTIC_STEP_NAMES[1]]
        both_refs = (
            f"github.ref == '{NEW_REFS[1]}' || "
            f"github.ref == '{NEW_REFS[0]}'"
        )
        self.assertEqual(configure["if"], both_refs)
        self.assertEqual(build["if"], both_refs)
        self.assertLess(names.index("Build warning-clean project"), names.index(configure["name"]))
        self.assertLess(names.index("Run complete 60-test three-track suite"),
                        names.index(configure["name"]))
        self.assertLess(names.index(configure["name"]), names.index(build["name"]))
        self.assertIn("cmake -S . -B build-s100-fresh", configure["run"])
        self.assertIn("-DOPENFHE_2023_1788_ENABLE_S100_FRESH_ERROR_DIAGNOSTIC=ON",
                      configure["run"])
        self.assertIn("cmake --build build-s100-fresh", build["run"])
        self.assertIn("--target s100_fresh_error_diagnostic_test --parallel 2", build["run"])
        self.assertNotIn("continue-on-error", build)

    def test_red_build_failure_is_not_masked_and_success_is_rejected(self):
        _, found = self.diagnostic_steps()
        build = found[DIAGNOSTIC_STEP_NAMES[1]]
        reject = found[DIAGNOSTIC_STEP_NAMES[2]]
        self.assertEqual(build["id"], "s100-fresh-build")
        self.assertEqual(
            condition_terms(reject),
            ["always()", f"github.ref == '{NEW_REFS[1]}'",
             "steps.s100-fresh-build.outcome == 'success'"],
        )
        self.assertIn("exit 1", reject["run"])
        self.assertNotIn("continue-on-error", reject)

    def test_green_runs_exact_controls_then_fresh_once_with_bounds(self):
        steps, found = self.diagnostic_steps()
        controls = found[DIAGNOSTIC_STEP_NAMES[3]]
        fresh = found[DIAGNOSTIC_STEP_NAMES[4]]
        self.assertLess(steps.index(controls), steps.index(fresh))
        for step, test_name in (
            (controls, "s100_encoding_inspection_contract"),
            (fresh, "s100_fresh_error_diagnostic"),
        ):
            self.assertTrue(condition_enabled(step, NEW_REFS[0],
                                              s100_scope="fresh"))
            self.assertEqual(step["timeout-minutes"], 20)
            self.assertEqual(step["env"]["OMP_NUM_THREADS"], 2)
            self.assertEqual(step["run"].count("ctest --test-dir build-s100-fresh"), 1)
            self.assertIn("--no-tests=error", step["run"])
            self.assertIn(f"-R '^{test_name}$'", step["run"])
            self.assertNotRegex(step["run"], r"--repeat|until-pass|set \+e|\|\|")

        retained_text = json.dumps(WIRING_BASELINE["jobs"]["linux-gcc"]["steps"])
        self.assertNotIn("s100_encoding_inspection_contract", retained_text)
        self.assertNotIn("s100_fresh_error_diagnostic", retained_text)


class S100ControlsOnlyDispatchContract(unittest.TestCase):
    @staticmethod
    def named_step(workflow, name):
        return next(step for step in workflow["jobs"]["linux-gcc"]["steps"]
                    if step.get("name") == name)

    def test_one_optional_choice_input_defaults_to_fresh(self):
        dispatch = CURRENT["true"]["workflow_dispatch"]
        self.assertEqual(set(dispatch), {"inputs"})
        self.assertEqual(set(dispatch["inputs"]), {"s100_scope"})
        scope = dispatch["inputs"]["s100_scope"]
        self.assertEqual(scope["type"], "choice")
        self.assertEqual(scope["default"], "fresh")
        self.assertEqual(scope["options"], ["fresh", "controls-only"])
        self.assertFalse(scope["required"])

    def test_only_dispatch_input_and_fresh_step_condition_change(self):
        normalized = deepcopy(CURRENT)
        normalized["true"]["workflow_dispatch"] = \
            CONTROLS_BASELINE["true"]["workflow_dispatch"]
        fresh = self.named_step(normalized, "Run S100 fresh-error diagnostic once")
        before = self.named_step(CONTROLS_BASELINE,
                                 "Run S100 fresh-error diagnostic once")
        fresh["if"] = before["if"]
        self.assertEqual(normalized, CONTROLS_BASELINE)

    def test_controls_stays_enabled_and_only_fresh_honors_controls_only(self):
        controls = self.named_step(CURRENT,
                                   "Run S100 encoding inspection contract once")
        old_controls = self.named_step(
            CONTROLS_BASELINE, "Run S100 encoding inspection contract once")
        fresh = self.named_step(CURRENT, "Run S100 fresh-error diagnostic once")
        old_fresh = self.named_step(
            CONTROLS_BASELINE, "Run S100 fresh-error diagnostic once")
        self.assertEqual(controls, old_controls)
        self.assertEqual(
            condition_terms(fresh),
            condition_terms(old_fresh) + ["inputs.s100_scope != 'controls-only'"],
        )

        green = NEW_REFS[0]
        for scope in ("", "fresh"):
            self.assertTrue(condition_enabled(controls, green, s100_scope=scope))
            self.assertTrue(condition_enabled(fresh, green, s100_scope=scope))
        self.assertTrue(condition_enabled(controls, green,
                                          s100_scope="controls-only"))
        self.assertFalse(condition_enabled(fresh, green,
                                           s100_scope="controls-only"))

    def test_other_refs_and_default_failure_semantics_are_unchanged(self):
        fresh = self.named_step(CURRENT, "Run S100 fresh-error diagnostic once")
        old_fresh = self.named_step(
            CONTROLS_BASELINE, "Run S100 fresh-error diagnostic once")
        refs = ["refs/heads/" + branch
                for branch in CONTROLS_BASELINE["true"]["push"]["branches"]]
        refs.extend((NEW_REFS[1], "refs/heads/workflow-dispatch-unlisted-ref"))
        for ref in refs:
            if ref == NEW_REFS[0]:
                continue
            for prior_success in (False, True):
                for scope in ("", "fresh", "controls-only"):
                    self.assertEqual(
                        condition_enabled(fresh, ref, prior_success,
                                          s100_scope=scope),
                        condition_enabled(old_fresh, ref, prior_success),
                        (ref, prior_success, scope),
                    )
        self.assertFalse(condition_enabled(fresh, NEW_REFS[0], False,
                                           s100_scope="fresh"))


if __name__ == "__main__":
    unittest.main(verbosity=2)
