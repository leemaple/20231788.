#!/usr/bin/env python3
"""One check-only engineering child; study execution is a separate admission."""
import argparse
from collections import Counter
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import signal
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
ENGINE_PATH = ROOT / "coordination/c9-native/run_checks.py"
spec = importlib.util.spec_from_file_location("c9_native_lifecycle", ENGINE_PATH)
engine = importlib.util.module_from_spec(spec)
spec.loader.exec_module(engine)
CONTRACT_PATH = Path(__file__).parent / "contract.json"

def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def require(condition, message):
    if not condition:
        raise RuntimeError(message)

def verify_source():
    contract = json.loads(CONTRACT_PATH.read_text())
    for item in contract["source_files"]:
        require(digest(ROOT/item["path"]) == item["sha256"], "source differs: "+item["path"])
    engine.verify_source()

def validate_outputs(directory, code, stdout, stderr, commit):
    require(code == 0, "check child exit failure")
    require(stdout.read_text() == "C9_COST_CHECK_PASS\n" and stderr.read_text() == "", "check marker/stream")
    require({p.name for p in directory.iterdir()} == {
        "started.json", "completed.json", "records.jsonl", "check_N64_D3-shape.json",
        "check_N64_D3-public-inputs.bin"}, "check output member set")
    started = json.loads((directory/"started.json").read_text())
    completed = json.loads((directory/"completed.json").read_text())
    require(started == {"source_commit":commit, "mode":"--check", "admission_sha256":"none"}, "started binding")
    require(completed == {"source_commit":commit, "mode":"--check", "passed":True,
        "sample_rows":0, "check_rows":4, "comparator_negative_controls":2,
        "classification":"engineering only", "repeated_performance":False}, "completion binding")
    rows = [json.loads(line) for line in (directory/"records.jsonl").read_text().splitlines()]
    names = ("Reference", "SharedStaged", "TailDigit", "TailWide")
    require(len(rows) == 4 and Counter(r.get("backend") for r in rows) == Counter(names), "check backend set")
    for row in rows:
        require(row == {"kind":"check", "profile":"check_N64_D3", "backend":row["backend"],
            "executed":row["backend"], "fallback":"None", "exact_output_and_input_state":True}, "check route/result")
    shape = json.loads((directory/"check_N64_D3-shape.json").read_text())
    require(shape["N"] == 64 and shape["depth"] == 3 and shape["slots"] == 32 and
        shape["surviving_towers"] == 2 and len(shape["full_Q"]) == 4 and
        len(shape["pair_moduli"]) == 3 and shape["P"] and
        shape["security_requested"] == "HEStd_NotSet" and shape["repeated_receipt"] is False,
        "check shape differs")
    require((directory/"check_N64_D3-public-inputs.bin").stat().st_size == 12288, "public input size")

def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("binary", type=Path)
    parser.add_argument("evidence", type=Path)
    args = parser.parse_args(argv)
    for sig in engine.CANCELLATION_SIGNALS:
        signal.signal(sig, engine.note_cancellation)
    verify_source()
    args.evidence.mkdir(parents=True, exist_ok=True)
    receipt = args.evidence/"check-controller-receipt.json"
    with receipt.open("x") as marker:
        marker.write('{"status":"started","passed":false}\n')
    start = time.monotonic()
    result = {"classification":"engineering only", "run_id":os.environ.get("GITHUB_RUN_ID"),
        "run_attempt":os.environ.get("GITHUB_RUN_ATTEMPT"), "project_commit":os.environ.get("GITHUB_SHA"),
        "controller_sha256":digest(Path(__file__)), "lifecycle_engine_sha256":digest(ENGINE_PATH),
        "contract_sha256":digest(CONTRACT_PATH), "passed":False, "children":engine.CHILDREN}
    output, error = args.evidence/"check.stdout.txt", args.evidence/"check.stderr.txt"
    directory = args.evidence/"check"
    try:
        engine.require_not_cancelled()
        result["binary_sha256"] = digest(args.binary)
        result["exit_code"] = engine.invoke([str(args.binary.resolve()), "--check", str(directory.resolve())], 120, output, error)
        validate_outputs(directory, result["exit_code"], output, error, result["project_commit"])
        result["output_files"] = [{"path":str(p.relative_to(directory)), "bytes":p.stat().st_size,
            "sha256":digest(p)} for p in sorted(directory.iterdir())]
        engine.require_not_cancelled()
        result["passed"] = True
    except BaseException as error_value:
        result["passed"] = False
        result["error"] = repr(error_value)
    finally:
        signal.pthread_sigmask(signal.SIG_BLOCK, engine.CANCELLATION_SIGNALS)
        for sig in signal.sigpending() & engine.CANCELLATION_SIGNALS:
            engine.note_cancellation(sig, None)
        if engine.CANCELLED:
            result["passed"] = False
            result["cancellation"] = list(engine.CANCELLED)
        if any(c["pid"] is not None and not c["reaped"] for c in engine.CHILDREN):
            result["passed"] = False
            result["cleanup_incomplete"] = True
        for key, path in (("stdout_sha256", output), ("stderr_sha256", error)):
            if path.is_file(): result[key] = digest(path)
        result["status"] = "passed" if result["passed"] else "failed"
        result["engineering_wall_seconds"] = time.monotonic()-start
        result["cancellation_boundary"] = "signals blocked and pending inspected before final receipt write"
        receipt.write_text(json.dumps(result,indent=2)+"\n")
    return 0 if result["passed"] else 1

if __name__ == "__main__":
    sys.exit(main())
