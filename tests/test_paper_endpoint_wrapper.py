"""Actual shell control-flow tests with disposable external process fixtures."""
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest


WRAPPER = Path(__file__).with_name("run_paper_endpoint_once.sh")
FAKE_TOOL = r'''
import json, os, pathlib, sys
root = pathlib.Path(os.environ["SYNTHETIC_ENDPOINT_ROOT"])
kind = pathlib.Path(sys.argv[0]).name
if kind == "python-fixture":
    if sys.argv[1:2] == ["-c"]:
        if os.environ["SYNTHETIC_LAUNCH_FAILURE"] == "127":
            (root / "bin/ctest-fixture").unlink()
        sys.version_info = (3, int(os.environ["SYNTHETIC_PYTHON_MINOR"]), 0)
        exec(compile(sys.argv[2], "external-python-version-probe", "exec", optimize=1))
        sys.exit(0)
    assert sys.argv[1] == "-B"
    assert pathlib.Path(sys.argv[2]).name == "paper_endpoint_finalizer.py"
    assert sys.argv[3] == "finalize"
    args = dict(zip(sys.argv[4::2], sys.argv[5::2]))
    assert args["--source-commit"] == "a" * 40
    assert args["--scope"] == "synthetic"
    assert args["--host"] == "linux"
    assert args["--github-run-id"] == "42"
    assert args["--github-run-attempt"] == "1"
    assert args["--canonical-parent"] == str(root / "scratch/canonical")
    assert args["--published-parent"] == str(root / "scratch/published")
    assert args["--primary-log"] == str(root / "scratch/primary.ctest.log")
    with (root / "finalizer.calls").open("a") as stream:
        stream.write(json.dumps(args) + "\n")
    sys.exit(int(os.environ["SYNTHETIC_FINALIZER_STATUS"]))
assert kind == "ctest-fixture"
assert sys.argv[1:] == ["--test-dir", str(root / "build"), "--verbose",
                        "--output-on-failure", "-R",
                        "^paper_full_eight_square_contract$"]
assert os.environ["PAPER_ENDPOINT_HOST"] == "linux"
assert os.environ["PAPER_ENDPOINT_RUN_ID"] == "42"
assert os.environ["PAPER_ENDPOINT_RUN_ATTEMPT"] == "1"
assert os.environ["PAPER_ENDPOINT_CANONICAL_PARENT"] == str(root / "scratch/canonical")
with (root / "ctest.calls").open("a") as stream:
    stream.write("one\n")
print("61: synthetic external CTest fixture", flush=True)
if os.environ["SYNTHETIC_CTEST_STATUS"] == "signal-kill":
    import signal
    os.kill(os.getpid(), signal.SIGKILL)
sys.exit(int(os.environ["SYNTHETIC_CTEST_STATUS"]))
'''


class EndpointWrapperTests(unittest.TestCase):
    def run_fixture(self, ctest_status, finalizer_status, *, capture_status=0,
                    scratch_extra=False, missing_finalizer=False, python_minor=12,
                    launch_failure=0):
        self.assertTrue(WRAPPER.is_file(), "one-shot wrapper is not implemented")
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-wrapper-") as temp:
            root = Path(temp).resolve()
            scripts = root / "project/tests"
            scripts.mkdir(parents=True)
            script = scripts / WRAPPER.name
            script.write_bytes(WRAPPER.read_bytes())
            if not missing_finalizer:
                (scripts / "paper_endpoint_finalizer.py").write_text(
                    "# Synthetic placeholder; external Python fixture owns this boundary.\n")
            scratch = root / "scratch"
            scratch.mkdir()
            (root / "build").mkdir()
            if scratch_extra:
                (scratch / "preexisting.txt").write_bytes(b"preserve")
            bin_dir = root / "bin"
            bin_dir.mkdir()
            for name in ("python-fixture", "ctest-fixture"):
                path = bin_dir / name
                if name == "ctest-fixture" and launch_failure == 126:
                    path.mkdir()
                    continue
                path.write_text("#!" + sys.executable + "\n" + FAKE_TOOL)
                path.chmod(0o700)
            if capture_status:
                path = bin_dir / "tee"
                path.write_text("#!" + sys.executable + "\nimport sys\nsys.stdin.buffer.read()\n"
                                + "sys.exit(" + str(capture_status) + ")\n")
                path.chmod(0o700)
            env = dict(os.environ, SYNTHETIC_ENDPOINT_ROOT=str(root),
                       SYNTHETIC_CTEST_STATUS=str(ctest_status),
                       SYNTHETIC_FINALIZER_STATUS=str(finalizer_status),
                       SYNTHETIC_PYTHON_MINOR=str(python_minor),
                       SYNTHETIC_LAUNCH_FAILURE=str(launch_failure),
                       GITHUB_SHA="a" * 40, GITHUB_RUN_ID="42", GITHUB_RUN_ATTEMPT="1")
            env["PATH"] = str(bin_dir) + os.pathsep + env["PATH"]
            result = subprocess.run([
                "/bin/bash", str(script), "--scope", "synthetic", "--host", "linux",
                "--build-dir-shell", str(root / "build"),
                "--scratch-shell", str(scratch), "--scratch-native", str(scratch),
                "--python", str(bin_dir / "python-fixture"),
                "--ctest", str(bin_dir / "ctest-fixture")],
                cwd=root, env=env, capture_output=True, timeout=10)
            ctest_calls = (root / "ctest.calls").read_text().splitlines() if (
                root / "ctest.calls").exists() else []
            calls = (root / "finalizer.calls").read_text().splitlines() if (
                root / "finalizer.calls").exists() else []
            preserved = (scratch / "preexisting.txt").read_bytes() if scratch_extra else None
            return result, ctest_calls, [json.loads(x) for x in calls], preserved

    def test_success_runs_exact_ctest_and_finalizer_once(self):
        result, ctest_calls, calls, _ = self.run_fixture(0, 0)
        self.assertEqual(result.returncode, 0, result.stderr.decode())
        self.assertEqual(ctest_calls, ["one"])
        self.assertEqual(len(calls), 1)
        self.assertEqual(calls[0]["--ctest-exit-code"], "0")
        self.assertEqual(calls[0]["--capture-exit-code"], "0")

    def test_failure_status_precedence_and_single_finalization(self):
        for ctest_status, capture_status, finalizer_status, expected in (
                (8, 0, 0, 8), (8, 0, 8, 8), (137, 0, 137, 137),
                (0, 23, 1, 23), (8, 23, 1, 8), (8, 0, 1, 8), (0, 0, 1, 1)):
            with self.subTest(ctest=ctest_status, capture=capture_status,
                              finalizer=finalizer_status):
                result, ctest_calls, calls, _ = self.run_fixture(
                    ctest_status, finalizer_status, capture_status=capture_status)
                self.assertEqual(result.returncode, expected, result.stderr.decode())
                self.assertEqual(ctest_calls, ["one"])
                self.assertEqual(len(calls), 1)
                self.assertEqual(calls[0]["--ctest-exit-code"], str(ctest_status))
                self.assertEqual(calls[0]["--capture-exit-code"], str(capture_status))

    def test_actual_child_signal_retains_bash_shell_status(self):
        result, ctest_calls, calls, _ = self.run_fixture("signal-kill", 1)
        self.assertEqual(result.returncode, 137, result.stderr.decode())
        self.assertEqual(ctest_calls, ["one"])
        self.assertEqual(len(calls), 1)
        self.assertEqual(calls[0]["--ctest-exit-code"], "137")

    def test_preconditions_stop_before_ctest_and_preserve_existing_scratch(self):
        for arguments in (dict(scratch_extra=True), dict(missing_finalizer=True)):
            with self.subTest(arguments=arguments):
                result, ctest_calls, calls, preserved = self.run_fixture(0, 0, **arguments)
                self.assertNotEqual(result.returncode, 0)
                self.assertEqual(ctest_calls, [])
                self.assertEqual(calls, [])
                if arguments.get("scratch_extra"):
                    self.assertEqual(preserved, b"preserve")

    def test_optimized_python_probe_rejects_wrong_version_before_ctest(self):
        result, ctest_calls, calls, _ = self.run_fixture(0, 0, python_minor=11)
        self.assertNotEqual(result.returncode, 0)
        self.assertEqual(ctest_calls, [])
        self.assertEqual(calls, [])

    def test_actual_launch_failures_still_finalize_once_with_enclosing_status(self):
        for code in (126, 127):
            with self.subTest(launch_failure=code):
                result, ctest_calls, calls, _ = self.run_fixture(0, 1, launch_failure=code)
                self.assertEqual(result.returncode, code, result.stderr.decode())
                self.assertEqual(ctest_calls, [])
                self.assertEqual(len(calls), 1)
                self.assertEqual(calls[0]["--ctest-exit-code"], str(code))


if __name__ == "__main__":
    unittest.main()
