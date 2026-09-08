#!/usr/bin/env python3
"""Source and synthetic-event checks for the one-shot tag workflow.

This script does not contact GitHub, create a tag, compile, or run cryptography.
Its source checks cannot prove hosted-runner scheduling or webhook authenticity;
the workflow repeats the same bounded event facts before any build or payload.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import sys
import unittest

import check_execution_guards


ROOT = Path(__file__).resolve().parents[2]
WORKFLOW = ROOT / ".github/workflows/s100-annulus125-once.yml"
TAG = "s100-annulus125-once-20260908"
TAG_REF = f"refs/tags/{TAG}"
SCIENCE_BASE = "def248a04b7212088e41a72e2239496dcd3e7027"
OPENFHE_PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
JOB_CONDITION = (
    "${{ github.event_name == 'push' && "
    f"github.ref == '{TAG_REF}' && "
    "github.event.created == true && github.event.deleted == false && "
    "github.event.forced == false && github.run_attempt == 1 }}"
)


def trigger(workflow: dict):
    """Accept YAML 1.1's boolean ``on`` key and explicitly quoted ``'on'``."""
    return workflow.get("true", workflow.get(True, workflow.get("on")))


def event_allowed(*, event_name: str, ref: str, run_attempt: int,
                  event: dict) -> tuple[bool, str]:
    checks = (
        (event_name == "push", "event is not push"),
        (ref == TAG_REF, "workflow ref is not the exact tag"),
        (event.get("ref") == TAG_REF, "event ref is not the exact tag"),
        (event.get("created") is True, "tag creation flag is not true"),
        (event.get("deleted") is False, "deleted flag is not false"),
        (event.get("forced") is False, "forced flag is not false"),
        (run_attempt == 1, "rerun attempt is not permitted"),
    )
    for accepted, reason in checks:
        if not accepted:
            return False, reason
    return True, "fresh exact tag creation on first run attempt"


def verify_other_workflows_do_not_match_tag() -> None:
    for path in sorted((ROOT / ".github/workflows").glob("*.yml")):
        if path == WORKFLOW:
            continue
        parsed = check_execution_guards.parse_yaml(path)
        events = trigger(parsed)
        if not events or "push" not in events:
            continue
        push = events["push"] or {}
        if not push.get("branches"):
            raise AssertionError(f"existing push workflow lacks a branch filter: {path}")
        if TAG in push.get("tags", []):
            raise AssertionError(f"existing workflow also matches the one-shot tag: {path}")


def verify_workflow_source(workflow: dict) -> None:
    events = trigger(workflow)
    if events != {"push": {"tags": [TAG]}}:
        raise AssertionError("trigger must be only push of the exact one-shot tag")
    if workflow.get("permissions") != {"contents": "read"}:
        raise AssertionError("permissions must be contents: read only")
    if workflow.get("env", {}).get("SCIENCE_BASE") != SCIENCE_BASE:
        raise AssertionError("science-source baseline is not frozen")
    if workflow.get("env", {}).get("OPENFHE_COMMIT") != OPENFHE_PIN:
        raise AssertionError("OpenFHE pin is not frozen")

    jobs = workflow.get("jobs", {})
    if set(jobs) != {"s100-annulus125-once"}:
        raise AssertionError("workflow must have exactly one isolated job")
    job = jobs["s100-annulus125-once"]
    if job.get("if") != JOB_CONDITION:
        raise AssertionError("job condition does not repeat the complete one-shot gate")
    steps = job.get("steps", [])
    run_text = "\n".join(str(step.get("run", "")) for step in steps)
    encoded = json.dumps(workflow, sort_keys=True)

    required = (
        "fetch-depth", "persist-credentials", "check_one_shot_gate.py ci",
        "git diff --quiet", SCIENCE_BASE, OPENFHE_PIN,
        "-DNATIVE_SIZE=64", "-DMATHBACKEND=4", "--parallel 2",
        "-DOPENFHE_2023_1788_ENABLE_S100_ANNULUS125=ON",
        "--target s100_annulus125_eight_square_test",
        "check_finalizer.py", "sha256sum",
        "run_once.py", "--result-directory", "--source-commit", "finalize_once.py",
        "actions/upload-artifact@", "always()",
    )
    for token in required:
        if token not in encoded:
            raise AssertionError(f"missing one-shot workflow control: {token}")

    forbidden = (
        "workflow_dispatch", "release", "branches", "--controls", "ctest ",
        "continue-on-error", "--repeat", "1000", "windows-",
        "dcp-rcb", "paper_full_eight_square_contract",
    )
    for token in forbidden:
        if token in encoded:
            raise AssertionError(f"forbidden one-shot workflow behavior: {token}")
    if run_text.count("run_once.py") != 1:
        raise AssertionError("run_once.py must be invoked exactly once")
    for argument in ('--result-directory "$S100_EVIDENCE/sample"',
                     '--source-commit "$GITHUB_SHA"'):
        if argument not in run_text:
            raise AssertionError(f"wrong run-once argument: {argument}")
    if "--timeout-seconds" in run_text:
        raise AssertionError("workflow must use run_once.py's frozen 1200-second default")
    if any(token in run_text for token in ("while ", "for ", "until ", "|| true", "||:")):
        raise AssertionError("retry/loop/error-masking shell constructs are forbidden")

    run_steps = [step for step in steps if step.get("id") == "run-once"]
    if len(run_steps) != 1 or run_steps[0].get("continue-on-error") is not None:
        raise AssertionError("one real-failure run-once step is required")
    finalizers = [step for step in steps if "finalize_once.py" in str(step.get("run", ""))]
    if len(finalizers) != 1 or finalizers[0].get("if") != \
            "${{ always() && steps.run-once.outcome != 'skipped' }}":
        raise AssertionError("finalizer must run after every attempted process")
    uploads = [step for step in steps
               if "actions/upload-artifact@" in str(step.get("uses", ""))]
    if len(uploads) != 1 or uploads[0].get("if") != "${{ always() }}":
        raise AssertionError("exact evidence archival must always be attempted")
    if uploads[0].get("with", {}).get("path") != "${{ env.S100_EVIDENCE }}":
        raise AssertionError("artifact upload must target only the one-shot evidence directory")
    synthetic = next(i for i, step in enumerate(steps)
                     if "check_finalizer.py" in str(step.get("run", "")))
    binary_hash = next(i for i, step in enumerate(steps)
                       if "sha256sum" in str(step.get("run", "")))
    attempted = next(i for i, step in enumerate(steps) if step.get("id") == "run-once")
    if not (synthetic < attempted and binary_hash < attempted):
        raise AssertionError("synthetic finalizer gate and binary hash must precede the sample")
    verify_other_workflows_do_not_match_tag()


def good_event() -> dict:
    return {"ref": TAG_REF, "created": True, "deleted": False, "forced": False}


class SyntheticEventGate(unittest.TestCase):
    def test_only_fresh_exact_tag_attempt_one_is_allowed(self):
        allowed, reason = event_allowed(
            event_name="push", ref=TAG_REF, run_attempt=1, event=good_event())
        self.assertTrue(allowed, reason)

    def test_branch_wrong_tag_rerun_and_mutated_flags_are_rejected(self):
        cases = {
            "branch": {"ref": "refs/heads/codex/s100-annulus125-20260908"},
            "wrong tag": {"ref": "refs/tags/s100-annulus125-once-wrong"},
            "rerun": {"run_attempt": 2},
            "forced": {"event": {**good_event(), "forced": True}},
            "deleted": {"event": {**good_event(), "deleted": True}},
            "not created": {"event": {**good_event(), "created": False}},
            "not push": {"event_name": "workflow_dispatch"},
        }
        base = {"event_name": "push", "ref": TAG_REF,
                "run_attempt": 1, "event": good_event()}
        for label, mutation in cases.items():
            inputs = dict(base)
            inputs.update(mutation)
            self.assertFalse(event_allowed(**inputs)[0], label)

    def test_workflow_source_contract(self):
        verify_workflow_source(check_execution_guards.parse_yaml(WORKFLOW))


def ci_check(args: argparse.Namespace) -> int:
    event = json.loads(args.event_path.read_text(encoding="utf-8"))
    allowed, reason = event_allowed(
        event_name=args.event_name,
        ref=args.ref,
        run_attempt=args.run_attempt,
        event=event,
    )
    result = {"allowed": allowed, "reason": reason, "encrypted_runs": 0}
    print(json.dumps(result, sort_keys=True))
    return 0 if allowed else 1


def main() -> int:
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest="command", required=True)
    sub.add_parser("self-test")
    ci = sub.add_parser("ci")
    ci.add_argument("--event-path", type=Path, required=True)
    ci.add_argument("--event-name", required=True)
    ci.add_argument("--ref", required=True)
    ci.add_argument("--run-attempt", type=int, required=True)
    args = parser.parse_args()
    if args.command == "ci":
        return ci_check(args)
    suite = unittest.defaultTestLoader.loadTestsFromModule(sys.modules[__name__])
    return 0 if unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful() else 1


if __name__ == "__main__":
    raise SystemExit(main())
