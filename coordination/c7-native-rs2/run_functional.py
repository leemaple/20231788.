#!/usr/bin/env python3
"""Bounded engineering checks. Durations are not benchmark measurements."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import time


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("phase", choices=("red", "green"))
    parser.add_argument("build", type=Path)
    parser.add_argument("evidence", type=Path)
    args = parser.parse_args()
    args.evidence.mkdir(parents=True, exist_ok=True)
    receipt = args.evidence / (args.phase + "-receipt.json")
    if receipt.exists():
        raise RuntimeError("phase receipt exists; refusing an unrecorded retry")
    binary = args.build.resolve() / "rs2_test"
    cases = ["valid_arithmetic_state_immutability"] if args.phase == "red" else [
        "wrong_lifecycle", "valid_arithmetic_state_immutability",
        "valid_wide_arithmetic_state_immutability", "mixed_tower_format",
        "untouched_public_pipeline", "declared_basis_mismatch", "terminal_rejections",
    ]
    result = {
        "classification": "engineering; no scientific performance evidence",
        "phase": args.phase, "binary_sha256": hashlib.sha256(binary.read_bytes()).hexdigest(),
        "run_id": os.environ.get("GITHUB_RUN_ID"),
        "run_attempt": os.environ.get("GITHUB_RUN_ATTEMPT"),
        "project_commit": os.environ.get("C7_TEST_COMMIT"), "tests": [], "passed": False,
    }
    try:
        for backend in ("reference", "reordered", "fused"):
            for case in cases:
                stem = f"{args.phase}-{backend}-{case}"
                start = time.monotonic()
                with (args.evidence / (stem + ".stdout.txt")).open("xb") as stdout, (
                    args.evidence / (stem + ".stderr.txt")).open("xb") as stderr:
                    process = subprocess.run([str(binary), case, backend], stdout=stdout,
                                             stderr=stderr, timeout=120, check=False)
                out = (args.evidence / (stem + ".stdout.txt")).read_text()
                err = (args.evidence / (stem + ".stderr.txt")).read_text()
                expected_red = args.phase == "red" and backend != "reference"
                expected_message = "RS2 test failure: DoubleCKKS: requested RS2 backend is not implemented\n"
                passed = (process.returncode == 1 and err == expected_message and not out) if expected_red else (
                    process.returncode == 0 and out == f"RS2 case passed: {case}\n" and not err)
                result["tests"].append({"backend": backend, "case": case,
                    "returncode": process.returncode, "expected_red": expected_red,
                    "passed": passed, "engineering_wall_seconds": time.monotonic() - start})
                print(stem, "PASS" if passed else "FAIL", flush=True)
                if not passed:
                    raise RuntimeError(f"unexpected outcome: {stem}; inspect retained output")
        result["passed"] = True
    except Exception as error:
        result["error"] = repr(error)
        raise
    finally:
        receipt.write_text(json.dumps(result, indent=2) + "\n")


if __name__ == "__main__":
    main()
