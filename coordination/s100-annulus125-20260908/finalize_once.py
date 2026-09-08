#!/usr/bin/env python3
"""Validate one completed runner sample; replay identical bytes at 180/230 digits.

No process launch, FFT, key generation or encryption. Unexpected/invalid evidence
fails loudly without a verification artifact; a valid numerical FAIL is retained.
This is an evidence consistency check, not runtime or transcendental attestation.
"""
import argparse
from datetime import datetime
from decimal import Decimal, localcontext
import hashlib
import json
from pathlib import Path
import re
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
import replay_annulus125 as receiver
from run_once import write_json_exclusive


def need(ok, reason):
    if not ok:
        raise ValueError(reason)


def banner(status):
    return (f"{receiver.CONTRACT} status=COMPLETE result={status} "
            "chain_count=1 squares=8 slots=16384 security=UNRESOLVED\n")


def finalize(directory, source_commit):
    directory = directory.resolve(strict=True)
    output = directory / "verification.json"
    if output.exists() or output.is_symlink():
        raise FileExistsError("verification already exists; refuse overwrite")
    need(re.fullmatch(r"[0-9a-f]{40}", source_commit) is not None, "invalid source commit")
    names = ("program-start.json", "program-end.json", "raw.tsv", "stdout.txt", "stderr.txt")
    for name in names:
        path = directory / name
        need(path.is_file() and not path.is_symlink(), "missing/nonregular evidence: " + name)
    start = json.loads((directory / "program-start.json").read_text())
    end = json.loads((directory / "program-end.json").read_text())
    for receipt in (start, end):
        need(receipt.get("source_commit") == source_commit and
             receipt.get("contract") == receiver.CONTRACT and
             receipt.get("entry") == "DIRECT_BINARY_ONCE_NOT_CTEST", "receipt source/contract")
    argv = start.get("argv")
    need(isinstance(argv, list) and len(argv) == 3 and all(isinstance(x, str) for x in argv),
         "invalid process argv")
    need(Path(argv[0]).is_absolute() and Path(argv[0]).name == "s100_annulus125_eight_square_test" and
         argv[1] == "--output" and Path(argv[2]) == directory / "raw.tsv", "wrong process entry/output")
    need(start.get("timeout_seconds") == 1200, "wrong frozen timeout")
    started = datetime.fromisoformat(start["started_utc"])
    ended = datetime.fromisoformat(end["ended_utc"])
    need(started.utcoffset() is not None and ended.utcoffset() is not None and ended >= started,
         "invalid process timestamps")
    code = end.get("returncode")
    need(end.get("timed_out") is False and type(code) is int and code in (0, 1),
         "incomplete/abnormal process; not a numerical result")
    decision = "PASS" if code == 0 else "FAIL"
    need((directory / "stderr.txt").read_bytes() == b"", "unexpected process stderr")
    need((directory / "stdout.txt").read_bytes() == banner(decision).encode(), "process stdout mismatch")
    path = directory / "raw.tsv"
    need(0 < path.stat().st_size <= 32 * 1024 * 1024, "invalid TSV size")
    raw = path.read_bytes()
    text = raw.decode("utf-8")
    replays = [receiver.replay(text, source_commit, code, digits) for digits in (180, 230)]
    a, b = replays
    need(a["status"] == b["status"] == decision and a["gates"] == b["gates"],
         "cross-precision decision disagreement")
    with localcontext() as context:
        context.prec = 250
        for name, maximum in a["maxima"].items():
            other = b["maxima"][name]
            need(maximum["slot"] == other["slot"] and
                 abs(Decimal(maximum["complex_modulus"]) - Decimal(other["complex_modulus"])) < Decimal(2) ** -300,
                 "cross-precision maximum disagreement: " + name)
    result = {"contract": receiver.CONTRACT, "source_commit": source_commit,
              "status": decision, "process_exit": code, "raw_bytes": len(raw),
              "raw_sha256": hashlib.sha256(raw).hexdigest(), "replays": replays,
              "scope": "one supplied process record; scalar replay, not independent runtime attestation",
              "assurance": "CONDITIONAL_OBSERVER_NOT_FORMAL", "legacy_S100_stress": "FAIL_RETAINED",
              "security": "UNRESOLVED"}
    write_json_exclusive(output, result)
    print(json.dumps({"status": decision, "raw_sha256": result["raw_sha256"],
                      "decimal_precisions": [180, 230], "new_encrypted_runs": 0}))
    return code


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--directory", type=Path, required=True)
    parser.add_argument("--source-commit", required=True)
    args = parser.parse_args()
    return finalize(args.directory, args.source_commit)


if __name__ == "__main__":
    raise SystemExit(main())
