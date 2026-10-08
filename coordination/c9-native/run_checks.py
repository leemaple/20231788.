#!/usr/bin/env python3
"""One-shot functional engineering only; durations are not performance results."""
import argparse
from collections import Counter
import hashlib
import json
import os
from pathlib import Path
import re
import signal
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
CONTRACT = json.loads((Path(__file__).parent / "contract.json").read_text())
CANCELLATION_SIGNALS = {signal.SIGINT, signal.SIGTERM}
CANCELLED = []
CHILDREN = []


def note_cancellation(signum, _frame):
    # A flag avoids asynchronous exceptions between Popen and ownership capture.
    name = signal.Signals(signum).name
    if name not in CANCELLED:
        CANCELLED.append(name)


def require_not_cancelled():
    if CANCELLED:
        raise RuntimeError("controlled cancellation: " + ",".join(CANCELLED))


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def verify_source():
    for item in CONTRACT["source_files"]:
        if digest(ROOT / item["path"]) != item["sha256"]:
            raise RuntimeError("source identity differs: " + item["path"])


def controlled_red(source):
    """Reproduce the real v1 trace bug, retaining the new counterexample test."""
    path = source / "src/mult2_backends.cpp"
    if digest(path) != CONTRACT["trace_green_sha256"]:
        raise RuntimeError("unexpected defect-control starting source")
    text = path.read_text()
    reset = ('    if (trace) {\n'
             '        *trace = {backend, Mult2Backend::Reference, Mult2Fallback::None, false};\n'
             '    }\n')
    validation = ('    Require(backend == Mult2Backend::Reference || backend == Mult2Backend::SharedStaged ||\n'
                  '                backend == Mult2Backend::TailDigit || backend == Mult2Backend::TailWide,\n'
                  '            "unknown Mult2 backend");\n')
    if text.count(reset + validation) != 1:
        raise RuntimeError("trace defect-control patch not unique")
    text = text.replace(reset + validation, validation + reset, 1)
    if hashlib.sha256(text.encode()).hexdigest() != CONTRACT["trace_red_sha256"]:
        raise RuntimeError("red material does not reproduce frozen v1 source")
    path.write_text(text)
    print(json.dumps({"classification": "retrospective defect control, not test-first source history",
                      "green": CONTRACT["trace_green_sha256"], "red": digest(path)}))


def invoke(command, timeout, stdout, stderr):
    # Polling bounds ordinary cancellation latency; the timeout remains a wall limit.
    require_not_cancelled()
    state = {"command": command, "pid": None, "reaped": False}
    CHILDREN.append(state)
    with stdout.open("xb") as out, stderr.open("xb") as err:
        process = subprocess.Popen(command, stdout=out, stderr=err, start_new_session=True)
        state["pid"] = process.pid
        deadline = time.monotonic() + timeout
        try:
            while True:
                require_not_cancelled()
                remaining = deadline - time.monotonic()
                if remaining <= 0:
                    raise subprocess.TimeoutExpired(command, timeout)
                try:
                    code = process.wait(timeout=min(0.25, remaining))
                except subprocess.TimeoutExpired:
                    continue
                state.update({"exit_code": code, "reaped": True})
                require_not_cancelled()
                return code
        except BaseException as error:
            state["error"] = repr(error)
            try:
                os.killpg(process.pid, signal.SIGKILL)
                state["group_cleanup"] = "SIGKILL sent"
            except ProcessLookupError:
                state["group_cleanup"] = "group already absent"
            except OSError as cleanup_error:
                state["group_cleanup_error"] = repr(cleanup_error)
            try:
                state.update({"exit_code": process.wait(timeout=10), "reaped": True})
            except BaseException as cleanup_error:
                state["reap_error"] = repr(cleanup_error)
            raise


def native(phase, build, evidence):
    binary = build / "mult2_backends_test"
    output = evidence / (phase + ".stdout.txt")
    error = evidence / (phase + ".stderr.txt")
    code = invoke([str(binary)], 120, output, error)
    lines = output.read_text().splitlines()
    stderr = error.read_text()
    if phase == "red":
        expected = "C9_NATIVE_FUNCTIONAL_FAIL: unknown backend retained a previous successful trace\n"
        passed = code == 1 and stderr == expected and "C9_NATIVE_FUNCTIONAL_PASS" not in lines
    else:
        passed = code == 0 and stderr == "" and lines[-1:] == ["C9_NATIVE_FUNCTIONAL_PASS"]
        routes = [line.split(",") for line in lines if line.startswith("route,")]
        expected_routes = Counter()
        for label, count in (("legacy", 4), ("repeated_nonterminal", 1), ("repeated_terminal", 1)):
            for requested in ("Reference", "SharedStaged", "TailDigit", "TailWide"):
                fallback = phase == "fallback" and requested == "TailWide"
                expected_routes[(label, requested, "Reference" if fallback else requested,
                                 "WideUnavailable" if fallback else "None")] = count
        actual_routes = Counter(tuple(row[1:]) for row in routes)
        passed = passed and actual_routes == expected_routes
        expected_kernel = Counter()
        for label, count in (("actual_relinearize_outputs", 4),
                             ("controlled_original_basis", 1), ("controlled_swapped_tails", 1)):
            for backend in ("SharedStaged", "TailDigit", "TailWide"):
                if phase != "fallback" or backend != "TailWide":
                    expected_kernel[(label, backend, "all_coefficients_pass")] = count
        actual_kernel = Counter(tuple(line.split(",")[1:]) for line in lines if line.startswith("kernel,"))
        passed = passed and actual_kernel == expected_kernel
        expected_rejections = Counter({name: count for name, count in CONTRACT["negative_cases"].items()})
        actual_rejections = Counter(line.split(",")[1] for line in lines if line.startswith("reject,"))
        passed = passed and actual_rejections == expected_rejections
        passed = passed and sum(line.startswith("profile,") for line in lines) == 4
        passed = passed and sum(line.startswith("controlled_order,") for line in lines) == 2
    return {"binary_sha256": digest(binary), "exit_code": code, "passed": passed,
            "stdout_sha256": digest(output), "stderr_sha256": digest(error)}


def regressions(build, evidence):
    selection = evidence / "regression-selection.json"
    selection_error = evidence / "regression-selection.stderr.txt"
    pattern = CONTRACT["regression_pattern"]
    code = invoke(["ctest", "--test-dir", str(build), "-N", "--show-only=json-v1", "-R", pattern],
                  30, selection, selection_error)
    if code != 0 or selection_error.read_text():
        raise RuntimeError("CTest selection failed")
    names = [item["name"] for item in json.loads(selection.read_text())["tests"]]
    if sorted(names) != sorted(CONTRACT["regression_names"]):
        raise RuntimeError("CTest selection differs from frozen 51 cases")
    output = evidence / "regressions.stdout.txt"
    error = evidence / "regressions.stderr.txt"
    code = invoke(["ctest", "--test-dir", str(build), "--output-on-failure", "--timeout", "120",
                   "-j", "1", "-R", pattern], 900, output, error)
    out = output.read_text()
    # Empty or different selections must not count as an all-green regression.
    passed = code == 0 and re.search(r"100% tests passed, 0 tests failed out of 51\b", out) is not None
    return {"selected_tests": names, "exit_code": code, "passed": passed,
            "stdout_sha256": digest(output), "stderr_sha256": digest(error)}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("phase", choices=("red-source", "red", "green", "fallback", "regressions"))
    parser.add_argument("path", type=Path)
    parser.add_argument("evidence", type=Path)
    args = parser.parse_args()
    for signum in CANCELLATION_SIGNALS:
        signal.signal(signum, note_cancellation)
    verify_source()
    args.evidence.mkdir(parents=True, exist_ok=True)
    receipt = args.evidence / (args.phase + "-receipt.json")
    # Reserving the filename prevents unrecorded reruns, including failed attempts.
    with receipt.open("x") as marker:
        marker.write('{"status":"started","passed":false}\n')
    start = time.monotonic()
    result = {"classification": "engineering only, no scientific timing evidence",
              "phase": args.phase, "run_id": os.environ.get("GITHUB_RUN_ID"),
              "run_attempt": os.environ.get("GITHUB_RUN_ATTEMPT"),
              "project_commit": os.environ.get("GITHUB_SHA"), "platform": sys.platform,
              "contract_sha256": digest(Path(__file__).parent / "contract.json"),
              "controller_sha256": digest(Path(__file__)), "passed": False, "children": CHILDREN}
    try:
        require_not_cancelled()
        if args.phase == "red-source":
            controlled_red(args.path.resolve())
            result["passed"] = True
        elif args.phase == "regressions":
            result.update(regressions(args.path.resolve(), args.evidence))
        else:
            result.update(native(args.phase, args.path.resolve(), args.evidence))
        require_not_cancelled()
        if not result["passed"]:
            raise RuntimeError("unexpected functional outcome; retained logs are authoritative")
    except BaseException as error:
        result["passed"] = False
        result["error"] = repr(error)
    finally:
        # Explicit cancellation decision boundary. Signals pending at this point
        # fail the phase; later signals stay blocked through this short write/exit.
        # SIGKILL, host loss, or an unwritable disk cannot promise a final receipt.
        signal.pthread_sigmask(signal.SIG_BLOCK, CANCELLATION_SIGNALS)
        for signum in signal.sigpending() & CANCELLATION_SIGNALS:
            note_cancellation(signum, None)
        if CANCELLED:
            result["passed"] = False
            result["cancellation"] = list(CANCELLED)
        if any(child["pid"] is not None and not child["reaped"] for child in CHILDREN):
            result["passed"] = False
            result["cleanup_incomplete"] = True
        result["status"] = "passed" if result["passed"] else "failed"
        result["cancellation_boundary"] = "signals blocked and pending inspected before final receipt write"
        result["engineering_wall_seconds"] = time.monotonic() - start
        receipt.write_text(json.dumps(result, indent=2) + "\n")
    return 0 if result["passed"] else 1


if __name__ == "__main__":
    sys.exit(main())
