#!/usr/bin/env python3
"""Source and synthetic-process checks for the isolated annulus CI path.

These checks parse workflow source and exercise ``run_once.py`` only with tiny
fake executables.  They do not configure CMake, compile C++, run CTest, perform
FFT work, create keys, encrypt, or establish GitHub runner behavior.
"""

from __future__ import annotations

import argparse
import importlib.util
import json
import os
from pathlib import Path
import stat
import subprocess
import sys
import tempfile
import textwrap
import unittest


ROOT = Path(__file__).resolve().parents[2]
WORKFLOW = ROOT / ".github/workflows/s100-annulus125.yml"
RUNNER = Path(__file__).with_name("run_once.py")
BRANCH_REF = "refs/heads/codex/s100-annulus125-20260908"
WORKFLOW_REPO_PATH = ".github/workflows/s100-annulus125.yml"
COMPILE_MODE = "compile-controls-only"
EXPECTED_PATHS = {
    WORKFLOW_REPO_PATH,
    "CMakeLists.txt",
    "include/openfhe_2023_1788/**",
    "src/**",
    "tests/**",
    "coordination/s100-annulus125-20260908/run_once.py",
    "coordination/s100-annulus125-20260908/check_execution_guards.py",
    "coordination/s100-annulus125-20260908/replay_annulus125.py",
    "coordination/s100-annulus125-20260908/check_receiver_framing.py",
    "coordination/s100-annulus125-20260908/scalar_reassessment.py",
    "coordination/comprehensive-reassessment-20260908/check_receiver_agreement.py",
}


def parse_yaml(path: Path) -> dict:
    """Use bounded system Ruby/Psych parsing; no project dependency is added."""
    result = subprocess.run(
        [
            "ruby", "-r", "yaml", "-r", "json", "-e",
            "puts JSON.generate(YAML.safe_load(STDIN.read, aliases: false))",
        ],
        input=path.read_text(encoding="utf-8"),
        text=True,
        capture_output=True,
        check=True,
    )
    return json.loads(result.stdout)


def workflow_mode(workflow: dict) -> str:
    mode = workflow.get("env", {}).get("S100_EXPERIMENT_MODE")
    if mode != COMPILE_MODE:
        raise ValueError(f"invalid S100_EXPERIMENT_MODE: {mode!r}")
    return mode


def verify_source_contract(workflow: dict) -> None:
    trigger = workflow.get("true")  # YAML 1.1 parses the key ``on`` as true.
    if set(trigger or {}) != {"push"}:
        raise AssertionError("only the push trigger is permitted")
    push = trigger["push"]
    if push.get("branches") != ["codex/s100-annulus125-20260908"]:
        raise AssertionError("push branch allowlist must contain only the S100 branch")
    if set(push.get("paths", [])) != EXPECTED_PATHS:
        raise AssertionError("workflow paths must equal the reviewed source/harness allowlist")
    if workflow.get("permissions") != {"contents": "read"}:
        raise AssertionError("workflow permissions must be contents: read only")
    if "${{ runner." in json.dumps(workflow.get("env", {}), sort_keys=True):
        raise AssertionError("top-level env cannot use the runner context")
    if workflow_mode(workflow) != COMPILE_MODE:
        raise AssertionError("the checked-in workflow must remain compile-controls-only")
    jobs = workflow.get("jobs", {})
    if set(jobs) != {"compile-controls"}:
        raise AssertionError("only the isolated compile-controls job is permitted")
    encoded = json.dumps(workflow, sort_keys=True)
    required = (
        "df495ba2e91739a6dc8f1de254fc5a41155ce504",
        "OPENFHE_2023_1788_ENABLE_S100_ANNULUS125=ON",
        "s100_annulus125_eight_square_test",
        "s100_annulus125_eight_square_v1",
        "--show-only=json-v1",
        "--controls",
        "always()",
    )
    for token in required:
        if token not in encoded:
            raise AssertionError(f"missing workflow control: {token}")
    forbidden = (
        "workflow_dispatch", "--output", "--repeat", "continue-on-error",
        "DIRECT_BINARY_ONCE_NOT_CTEST", "--precision 180", "--precision 230",
    )
    for token in forbidden:
        if token in encoded:
            raise AssertionError(f"forbidden workflow behavior: {token}")
    steps = jobs["compile-controls"].get("steps", [])
    run_text = "\n".join(str(step.get("run", "")) for step in steps)
    if run_text.count("ctest --test-dir") != 1 or run_text.count("--controls") != 1:
        raise AssertionError("exactly one CTest show-only and one controls invocation required")
    if "run_once.py" in run_text or "replay_annulus125.py --tsv" in run_text:
        raise AssertionError("future encrypted runner/replay must remain unwired")
    uploads = [step for step in steps if "actions/upload-artifact@" in str(step.get("uses", ""))]
    if len(uploads) != 1 or uploads[0].get("if") != "${{ always() }}":
        raise AssertionError("one always-run evidence archival step is required")


def load_runner_module():
    spec = importlib.util.spec_from_file_location("annulus_run_once", RUNNER)
    if spec is None or spec.loader is None:
        raise RuntimeError("cannot load run_once.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class WorkflowSourceContract(unittest.TestCase):
    def test_exact_source_contract(self):
        verify_source_contract(parse_yaml(WORKFLOW))


class SyntheticRunOnceContract(unittest.TestCase):
    def setUp(self):
        self.runner = load_runner_module()
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)

    def tearDown(self):
        self.temp.cleanup()

    def fake(self, body: str) -> Path:
        path = self.root / "fake.py"
        path.write_text("#!/usr/bin/env python3\n" + textwrap.dedent(body), encoding="utf-8")
        path.chmod(path.stat().st_mode | stat.S_IXUSR)
        return path

    def test_nonzero_exit_and_raw_streams_are_preserved_without_retry(self):
        marker = self.root / "invocations"
        exe = self.fake(f"""
            import pathlib, sys
            pathlib.Path({str(marker)!r}).open('a').write('1')
            pathlib.Path(sys.argv[2]).write_text('raw-fail\\n', encoding='utf-8')
            print('stdout-fail', flush=True)
            print('stderr-fail', file=sys.stderr, flush=True)
            raise SystemExit(7)
        """)
        result = self.root / "result"
        code = self.runner.run_once(exe, result, "a" * 40, 2.0)
        self.assertEqual(code, 7)
        receipt = json.loads((result / "program-end.json").read_text())
        self.assertEqual(receipt["returncode"], 7)
        self.assertFalse(receipt["timed_out"])
        self.assertEqual((result / "raw.tsv").read_text(), "raw-fail\n")
        self.assertEqual((result / "stdout.txt").read_text(), "stdout-fail\n")
        self.assertEqual((result / "stderr.txt").read_text(), "stderr-fail\n")
        self.assertEqual(stat.S_IMODE(result.stat().st_mode), 0o700)
        with self.assertRaises(FileExistsError):
            self.runner.run_once(exe, result, "a" * 40, 2.0)
        self.assertEqual(marker.read_text(), "1")

    def test_timeout_records_no_fabricated_subprocess_returncode(self):
        exe = self.fake("""
            import pathlib, sys, time
            pathlib.Path(sys.argv[2]).write_text('partial\\n', encoding='utf-8')
            print('before-timeout', flush=True)
            time.sleep(5)
        """)
        result = self.root / "timeout-result"
        code = self.runner.run_once(exe, result, "b" * 40, 1.0)
        self.assertEqual(code, 124)
        receipt = json.loads((result / "program-end.json").read_text())
        self.assertIsNone(receipt["returncode"])
        self.assertTrue(receipt["timed_out"])
        self.assertEqual((result / "raw.tsv").read_text(), "partial\n")
        self.assertIn("before-timeout", (result / "stdout.txt").read_text())

    def test_cli_rejects_timeout_override_before_starting_a_process(self):
        marker = self.root / "launched"
        exe = self.fake(f"import pathlib\npathlib.Path({str(marker)!r}).write_text('launched')\n")
        result = self.root / "override-result"
        completed = subprocess.run(
            [sys.executable, "-B", str(RUNNER), "--executable", str(exe),
             "--result-directory", str(result), "--source-commit", "c" * 40,
             "--timeout-seconds", "600"], capture_output=True, text=True, check=False)
        self.assertEqual(completed.returncode, 2, "the frozen CLI must reject a timeout knob")
        self.assertFalse(marker.exists())
        self.assertFalse(result.exists())


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("command", choices=("self-test",))
    parser.parse_args()
    suite = unittest.defaultTestLoader.loadTestsFromModule(sys.modules[__name__])
    return 0 if unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful() else 1


if __name__ == "__main__":
    raise SystemExit(main())
