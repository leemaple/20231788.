#!/usr/bin/env python3
"""Synthetic event and static source gate for PUBLIC-S100-ECD-CELL-01.

This standard-library-only checker never imports or executes the mathematical
candidate.  Static source inspection cannot attest a future GitHub event or
hosted runner; the workflow repeats the event check before invoking the harness.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
WORKFLOW = ROOT / ".github/workflows/public-s100-ecd-cell.yml"
TAG = "public-s100-ecd-cell-once-20260909"
REF = f"refs/tags/{TAG}"
JOB_CONDITION = (
    "${{ github.event_name == 'push' && "
    f"github.ref == '{REF}' && "
    "github.event.created == true && github.event.deleted == false && "
    "github.event.forced == false && github.run_attempt == 1 }}"
)


def event_admitted(*, event_name: str, ref: str, run_attempt: int,
                   event: dict) -> tuple[bool, str]:
    """Admit only the first unforced creation event for the exact frozen tag."""
    checks = (
        (event_name == "push", "event is not push"),
        (ref == REF, "workflow ref is not the exact frozen tag"),
        (event.get("ref") == REF, "payload ref is not the exact frozen tag"),
        (event.get("created") is True, "created flag is not true"),
        (event.get("deleted") is False, "deleted flag is not false"),
        (event.get("forced") is False, "forced flag is not false"),
        (run_attempt == 1, "rerun attempt is not permitted"),
    )
    for accepted, reason in checks:
        if not accepted:
            return False, reason
    return True, "fresh exact tag creation on attempt one"


def assert_in(source: str, token: str) -> None:
    if token not in source:
        raise AssertionError(f"missing CI control: {token}")


def verify_workflow_source() -> None:
    source = WORKFLOW.read_text(encoding="utf-8")
    required = (
        "name: Public S100 Ecd-cell one-shot adjudication",
        "on:\n  push:\n    tags:\n      - public-s100-ecd-cell-once-20260909",
        "permissions:\n  contents: read",
        "cancel-in-progress: false",
        f"if: {JOB_CONDITION}",
        "runs-on: ubuntu-24.04",
        "timeout-minutes: 45",
        "actions/checkout@11bd71901bbe5b1630ceea73d27597364c9af683",
        "actions/setup-python@a26af69be951a213d495a4c3e4e4022e16d87065",
        "actions/upload-artifact@ea165f8d65b6e75b540449e92b4886f43607fa02",
        "python-version: '3.12'",
        "persist-credentials: false",
        "GITHUB_EVENT_PATH",
        "check_ci_gate.py ci",
        "${RUNNER_TEMP}/public-s100-ecd-cell-${GITHUB_RUN_ID}-${GITHUB_RUN_ATTEMPT}",
        "umask 077",
        "stat -c '%a'",
        "git rev-parse HEAD",
        "git rev-parse \"${GITHUB_REF}^{commit}\"",
        "python --version",
        "platform.platform()",
        "coordination/reproduction-adjudication-20260909/pro/MANIFEST.sha256.json",
        "integration-sha256.txt",
        "pro-manifest-sha256.txt",
        "python -B coordination/public-s100-ecd-cell-20260909/test_harness_contract.py",
        "harness-contract-tests.log",
        "python -B -I coordination/public-s100-ecd-cell-20260909/run_once.py",
        "--reviewed --output-directory \"$EVIDENCE_DIR/execution\"",
        "2>&1 | tee \"$EVIDENCE_DIR/harness.log\"",
        "pipeline_status=(\"${PIPESTATUS[@]}\")",
        "harness-tee.exit",
        "if test \"$tee_exit\" -ne 0",
        "if: ${{ always() }}",
        "path: ${{ runner.temp }}/public-s100-ecd-cell-${{ github.run_id }}-${{ github.run_attempt }}",
        "if-no-files-found: error",
    )
    for token in required:
        assert_in(source, token)

    forbidden = (
        "workflow_dispatch", "schedule:", "release:", "branches:",
        "continue-on-error", "cancel-in-progress: true", "actions/cache@",
        "openfheorg/", "cmake ", "ctest ", "--repeat", "pip install",
        "apt-get", "sudo ", "docker ", "git clone", "printenv", "env |",
    )
    for token in forbidden:
        if token in source:
            raise AssertionError(f"forbidden CI behavior: {token}")
    if source.count(TAG) != 2:
        raise AssertionError("exact tag must appear only in the trigger and job gate")
    harness = (
        "python -B -I coordination/public-s100-ecd-cell-20260909/run_once.py"
    )
    if source.count(harness) != 1:
        raise AssertionError("reviewed harness must be invoked exactly once")
    scalar_harness_test = (
        "python -B coordination/public-s100-ecd-cell-20260909/test_harness_contract.py"
    )
    if source.count(scalar_harness_test) != 1:
        raise AssertionError("root harness scalar contracts must run exactly once")
    if "set -euo pipefail" not in source:
        raise AssertionError("shell pipeline failure propagation is required")

    for path in sorted((ROOT / ".github/workflows").glob("*")):
        if path == WORKFLOW or not path.is_file():
            continue
        if TAG in path.read_text(encoding="utf-8"):
            raise AssertionError(f"another workflow names the one-shot tag: {path}")


def ci_command(args: argparse.Namespace) -> int:
    event = json.loads(args.event_path.read_text(encoding="utf-8"))
    allowed, reason = event_admitted(
        event_name=args.event_name,
        ref=args.ref,
        run_attempt=args.run_attempt,
        event=event,
    )
    print(json.dumps({"allowed": allowed, "reason": reason,
                      "full_transforms": 0, "crypto_calls": 0}, sort_keys=True))
    return 0 if allowed else 1


def main() -> int:
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest="command", required=True)
    subparsers.add_parser("source")
    ci = subparsers.add_parser("ci")
    ci.add_argument("--event-path", type=Path, required=True)
    ci.add_argument("--event-name", required=True)
    ci.add_argument("--ref", required=True)
    ci.add_argument("--run-attempt", type=int, required=True)
    args = parser.parse_args()
    if args.command == "source":
        verify_workflow_source()
        print(json.dumps({"status": "PASS_SOURCE_ONLY", "full_transforms": 0,
                          "crypto_calls": 0}, sort_keys=True))
        return 0
    return ci_command(args)


if __name__ == "__main__":
    raise SystemExit(main())
