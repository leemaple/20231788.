#!/usr/bin/env python3
"""Static workflow and synthetic GitHub-event gate for the public encoder cap.

This checker does not contact GitHub, compile, encode, or transform.  Parsing the
workflow source cannot attest hosted-runner scheduling or the authenticity of a
future event; the workflow repeats the event check before any expensive action.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import subprocess


ROOT = Path(__file__).resolve().parents[2]
WORKFLOW = ROOT / ".github/workflows/public-s100-encoder-cap.yml"
RED_TAG = "public-s100-encoder-cap-red-20260909"
GREEN_TAG = "public-s100-encoder-cap-once-20260909"
RED_REF = f"refs/tags/{RED_TAG}"
GREEN_REF = f"refs/tags/{GREEN_TAG}"
OPENFHE_PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
PRODUCTION_BASE = "a4b815a733efe81897325e2a8e4c826a4ebfa439"
JOB_CONDITION = (
    "${{ github.event_name == 'push' && "
    f"(github.ref == '{RED_REF}' || github.ref == '{GREEN_REF}') && "
    "github.event.created == true && github.event.deleted == false && "
    "github.event.forced == false && github.run_attempt == 1 }}"
)


def parse_yaml(path: Path) -> dict:
    result = subprocess.run(
        ["ruby", "-r", "yaml", "-r", "json", "-e",
         "puts JSON.generate(YAML.safe_load(STDIN.read, aliases: false))"],
        input=path.read_text(encoding="utf-8"), text=True,
        capture_output=True, check=True,
    )
    return json.loads(result.stdout)


def trigger(workflow: dict):
    return workflow.get("true", workflow.get(True, workflow.get("on")))


def event_stage(*, event_name: str, ref: str, run_attempt: int,
                event: dict) -> tuple[str | None, str]:
    if event_name != "push":
        return None, "event is not push"
    stage = {RED_REF: "red", GREEN_REF: "green"}.get(ref)
    if stage is None:
        return None, "workflow ref is not an exact approved tag"
    if event.get("ref") != ref:
        return None, "event ref and workflow ref disagree"
    checks = (
        (event.get("created") is True, "creation flag is not true"),
        (event.get("deleted") is False, "deleted flag is not false"),
        (event.get("forced") is False, "forced flag is not false"),
        (run_attempt == 1, "rerun attempt is not permitted"),
    )
    for accepted, reason in checks:
        if not accepted:
            return None, reason
    return stage, f"fresh exact {stage} tag creation on attempt one"


def verify_other_workflows_do_not_match() -> None:
    for path in sorted((ROOT / ".github/workflows").glob("*.yml")):
        if path == WORKFLOW:
            continue
        events = trigger(parse_yaml(path))
        if not events or "push" not in events:
            continue
        push = events["push"] or {}
        tags = push.get("tags", [])
        if RED_TAG in tags or GREEN_TAG in tags:
            raise AssertionError(f"another workflow explicitly matches a cap tag: {path}")
        if not push.get("branches") and not tags:
            raise AssertionError(f"another workflow has an unfiltered push trigger: {path}")


def verify_workflow_source(workflow: dict) -> None:
    if trigger(workflow) != {"push": {"tags": [RED_TAG, GREEN_TAG]}}:
        raise AssertionError("workflow must trigger only on the two exact tags")
    if workflow.get("permissions") != {"contents": "read"}:
        raise AssertionError("workflow permissions must be contents: read only")
    environment = workflow.get("env", {})
    if environment.get("OPENFHE_COMMIT") != OPENFHE_PIN:
        raise AssertionError("official OpenFHE pin is not frozen")
    if environment.get("PRODUCTION_BASE") != PRODUCTION_BASE:
        raise AssertionError("production source baseline is not frozen")
    if "${{ runner." in json.dumps(environment, sort_keys=True):
        raise AssertionError("top-level env cannot use the runner context")

    jobs = workflow.get("jobs", {})
    if set(jobs) != {"public-encoder-cap"}:
        raise AssertionError("exactly one isolated job is required")
    job = jobs["public-encoder-cap"]
    if job.get("if") != JOB_CONDITION:
        raise AssertionError("job-level creation/attempt/tag gate is incomplete")
    steps = job.get("steps", [])
    encoded = json.dumps(workflow, sort_keys=True)
    run_text = "\n".join(str(step.get("run", "")) for step in steps)

    required = (
        "actions/checkout@11bd71901bbe5b1630ceea73d27597364c9af683",
        "actions/setup-python@a26af69be951a213d495a4c3e4e4022e16d87065",
        "actions/upload-artifact@ea165f8d65b6e75b540449e92b4886f43607fa02",
        "persist-credentials", "check_ci_gate.py ci", "GITHUB_EVENT_PATH",
        "tests/public_encoder_scalar_contract_test.py",
        "tests/public_encoder_transform_contract_test.py",
        "-DWITH_OPENMP=ON", "-DWITH_REDUCED_NOISE=OFF",
        "-DNATIVE_SIZE=64", "-DMATHBACKEND=4", "--parallel 2",
        "OPENFHE1788_PUBLIC_ENCODER_DIAGNOSTIC=ON",
        "--target public_s100_encoding_dump", "InspectFixedS100PublicEncoding",
        "undefined reference", "--metadata", "--api-negative", "sha256sum",
        "PUBLIC_ENCODER", "PUBLIC_JSON", "CAP_JSON",
        "--allow-transform-after-root-review", "encode_exit", "certify_exit", "always()",
    )
    for token in required:
        if token not in encoded:
            raise AssertionError(f"missing CI control: {token}")

    forbidden = (
        "workflow_dispatch", "release:", "schedule:", "branches:",
        "actions/cache@", "restore-keys", "ctest ", "continue-on-error",
        "--repeat", "windows-", "dcp-rcb", "env |", "printenv",
    )
    for token in forbidden:
        if token in encoded:
            raise AssertionError(f"forbidden CI behavior: {token}")
    if run_text.count('"$PUBLIC_ENCODER" "$PUBLIC_JSON"') != 1:
        raise AssertionError("the original public encoder must be invoked exactly once")
    if run_text.count("--allow-transform-after-root-review") != 1:
        raise AssertionError("the full interval certifier must be invoked exactly once")

    by_id = {step.get("id"): (index, step) for index, step in enumerate(steps)
             if step.get("id")}
    for identifier in ("gate", "red-link", "green-build", "encode", "certify"):
        if identifier not in by_id:
            raise AssertionError(f"missing step id: {identifier}")
    red_run = str(by_id["red-link"][1].get("run", ""))
    if "if cmake --build" not in red_run or "build_exit" not in red_run:
        raise AssertionError("RED link failure must be captured and discriminated")
    same_line = (
        "(undefined reference|undefined symbol).*InspectFixedS100PublicEncoding|"
        "InspectFixedS100PublicEncoding.*(undefined reference|undefined symbol)"
    )
    if same_line not in red_run:
        raise AssertionError("RED must bind the missing symbol and linker error on one line")
    green_order = [by_id[name][0] for name in ("green-build", "encode", "certify")]
    if green_order != sorted(green_order):
        raise AssertionError("GREEN build/encode/certify order is invalid")
    scalar_index = next(i for i, step in enumerate(steps)
                        if "python -B tests/public_encoder_scalar_contract_test.py" in
                        str(step.get("run", "")))
    transform_index = next(i for i, step in enumerate(steps)
                           if "python -B tests/public_encoder_transform_contract_test.py" in
                           str(step.get("run", "")))
    metadata_index = next(i for i, step in enumerate(steps)
                          if "--metadata" in str(step.get("run", "")))
    negative_index = next(i for i, step in enumerate(steps)
                          if "--api-negative" in str(step.get("run", "")))
    provenance_index = next(i for i, step in enumerate(steps)
                            if "boost-header-sha256" in str(step.get("run", "")))
    encode_index = by_id["encode"][0]
    if not (scalar_index < by_id["green-build"][0] < metadata_index < negative_index
            < transform_index < provenance_index < encode_index < by_id["certify"][0]):
        raise AssertionError("GREEN preflight/provenance/expensive-operation order is invalid")

    uploads = [step for step in steps
               if "actions/upload-artifact@" in str(step.get("uses", ""))]
    if len(uploads) != 1 or uploads[0].get("if") != "${{ always() }}":
        raise AssertionError("one always-run narrow artifact upload is required")
    if uploads[0].get("with", {}).get("path") != "${{ env.EVIDENCE_DIR }}":
        raise AssertionError("artifact upload must target only the evidence directory")
    verify_other_workflows_do_not_match()


def ci_command(args: argparse.Namespace) -> int:
    event = json.loads(args.event_path.read_text(encoding="utf-8"))
    stage, reason = event_stage(event_name=args.event_name, ref=args.ref,
                                run_attempt=args.run_attempt, event=event)
    print(json.dumps({"allowed": stage is not None, "stage": stage,
                      "reason": reason, "encoding_calls": 0,
                      "forward_transforms": 0}, sort_keys=True))
    if stage is not None and args.github_output:
        with args.github_output.open("a", encoding="utf-8") as output:
            output.write(f"stage={stage}\n")
    return 0 if stage is not None else 1


def main() -> int:
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest="command", required=True)
    sub.add_parser("source")
    ci = sub.add_parser("ci")
    ci.add_argument("--event-path", type=Path, required=True)
    ci.add_argument("--event-name", required=True)
    ci.add_argument("--ref", required=True)
    ci.add_argument("--run-attempt", type=int, required=True)
    ci.add_argument("--github-output", type=Path)
    args = parser.parse_args()
    if args.command == "source":
        verify_workflow_source(parse_yaml(WORKFLOW))
        print(json.dumps({"status": "PASS_SOURCE_ONLY", "encoding_calls": 0,
                          "forward_transforms": 0}, sort_keys=True))
        return 0
    return ci_command(args)


if __name__ == "__main__":
    raise SystemExit(main())
