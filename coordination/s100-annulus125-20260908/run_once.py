#!/usr/bin/env python3
"""One direct future annulus binary invocation with immutable local receipts.

This helper is deliberately not wired into the initial compile/controls workflow.
It catches only ``TimeoutExpired``, the one boundary it can translate without
hiding an unexpected harness or filesystem failure.  A result directory is
exclusive: its existence permanently prevents this helper from starting there
again.
"""

from __future__ import annotations

import argparse
from datetime import datetime, timezone
import json
import os
from pathlib import Path
import re
import subprocess


CONTRACT = "s100-annulus125-e80-v1"


def utc_now() -> str:
    return datetime.now(timezone.utc).isoformat()


def write_json_exclusive(path: Path, value: dict) -> None:
    with path.open("x", encoding="utf-8") as output:
        json.dump(value, output, sort_keys=True)
        output.write("\n")
        output.flush()
        os.fsync(output.fileno())


def run_once(executable: Path, result_directory: Path, source_commit: str,
             timeout_seconds: float = 1200) -> int:
    if re.fullmatch(r"[0-9a-f]{40}", source_commit) is None:
        raise ValueError("source commit must be exactly 40 lowercase hex characters")
    if timeout_seconds <= 0:
        raise ValueError("timeout must be positive")

    executable = executable.resolve(strict=True)
    if not executable.is_file() or not os.access(executable, os.X_OK):
        raise ValueError("executable must be an executable regular file")

    os.umask(0o077)
    # mkdir is intentionally exclusive; no resume, overwrite, or retry path exists.
    result_directory.mkdir(mode=0o700, parents=False, exist_ok=False)
    result_directory = result_directory.resolve(strict=True)
    raw_tsv = result_directory / "raw.tsv"
    stdout_path = result_directory / "stdout.txt"
    stderr_path = result_directory / "stderr.txt"
    argv = [str(executable), "--output", str(raw_tsv)]

    write_json_exclusive(result_directory / "program-start.json", {
        "argv": argv,
        "contract": CONTRACT,
        "entry": "DIRECT_BINARY_ONCE_NOT_CTEST",
        "source_commit": source_commit,
        "started_utc": utc_now(),
        "timeout_seconds": timeout_seconds,
    })

    timed_out = False
    returncode = None
    with stdout_path.open("xb") as stdout, stderr_path.open("xb") as stderr:
        try:
            completed = subprocess.run(
                argv,
                stdout=stdout,
                stderr=stderr,
                timeout=timeout_seconds,
                check=False,
                env={**os.environ, "OMP_NUM_THREADS": "2"},
            )
            returncode = completed.returncode
        except subprocess.TimeoutExpired:
            timed_out = True

    write_json_exclusive(result_directory / "program-end.json", {
        "contract": CONTRACT,
        "ended_utc": utc_now(),
        "entry": "DIRECT_BINARY_ONCE_NOT_CTEST",
        # A timed-out process has no CompletedProcess return code.  Keep null
        # rather than fabricating 124; 124 is only this wrapper's exit status.
        "returncode": returncode,
        "source_commit": source_commit,
        "timed_out": timed_out,
    })
    return 124 if timed_out else int(returncode)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--executable", type=Path, required=True)
    parser.add_argument("--result-directory", type=Path, required=True)
    parser.add_argument("--source-commit", required=True)
    parser.add_argument("--timeout-seconds", type=float, default=1200)
    args = parser.parse_args()
    return run_once(
        args.executable,
        args.result_directory,
        args.source_commit,
        args.timeout_seconds,
    )


if __name__ == "__main__":
    raise SystemExit(main())
