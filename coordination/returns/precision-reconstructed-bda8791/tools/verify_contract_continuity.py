#!/usr/bin/env python3
"""Reapply the reconstructed precision red/green series on the exact supplied baseline."""

from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

CONTRACT = Path("tests/precision_dcp_rcb_contract_test.cpp")
HEADER = Path("tests/precision_dcp_rcb_fixture.h")
FIXTURE = Path("tests/precision_dcp_rcb_fixture.cpp")
CMAKE = Path("CMakeLists.txt")
FROZEN = (CMAKE, CONTRACT, HEADER)
EXPECTED_CHANGED = tuple(sorted(str(path) for path in (*FROZEN, FIXTURE)))
TEST_NAME = "precision_dcp_rcb_high_precision_contract"
REQUIRED_CONTRACT_SNIPPETS = (
    "constexpr std::uint32_t kAcceptanceBits = 80;",
    "constexpr std::size_t kMinimumWrapHeadroomBits = 128;",
    "constexpr std::size_t kFreshKeyTrials = 4;",
    "BigFloat(1) / Pow2Float(70)",
    "BigFloat(1) / Pow2Float(73)",
    "deltaError <= tolerance",
    "maximumError <= tolerance",
    "module.DCP(input)",
    "module.RCB(pair)",
    "CheckBinary64NegativeControl(context, expected)",
    "CheckCanonicalOracleWitnesses()",
    "parameters.SetRingDim(kRingDimension)",
    "parameters.SetBatchSize(kBatchSize)",
    "parameters.SetScalingModSize(kScalingModSize)",
    "parameters.SetFirstModSize(kFirstModSize)",
    "parameters.SetScalingTechnique(lbcrypto::FIXEDMANUAL)",
    "parameters.SetKeySwitchTechnique(lbcrypto::HYBRID)",
    "parameters.SetCKKSDataType(lbcrypto::COMPLEX)",
)
FORBIDDEN_SUBSTRINGS = (
    "GetCKKSPackedValue",
    "context->Decrypt(",
    "catch (...)",
    "catch (const std::exception",
)


def require(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(*args: str, cwd: Path | None = None) -> str:
    process = subprocess.run(
        args,
        cwd=cwd,
        check=False,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    if process.returncode != 0:
        raise RuntimeError(
            f"command failed ({process.returncode}): {' '.join(args)}\n{process.stdout}"
        )
    return process.stdout


def snapshot(root: Path) -> dict[str, str]:
    return {
        str(path.relative_to(root)): digest(path)
        for path in sorted(root.rglob("*"))
        if path.is_file() and ".git" not in path.parts
    }


def changed(before: dict[str, str], after: dict[str, str]) -> list[str]:
    return [
        path
        for path in sorted(set(before) | set(after))
        if before.get(path) != after.get(path)
    ]


def read_sha256_file(path: Path) -> dict[str, str]:
    entries: dict[str, str] = {}
    for line_number, raw_line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        if not raw_line.strip():
            continue
        parts = raw_line.split(maxsplit=1)
        require(len(parts) == 2, f"malformed SHA-256 line {line_number}: {raw_line!r}")
        sha256, relative = parts
        relative = relative.lstrip("*")
        require(len(sha256) == 64, f"malformed SHA-256 at line {line_number}")
        require(relative not in entries, f"duplicate manifest path: {relative}")
        entries[relative] = sha256
    return entries


def check_exact_baseline(baseline: Path, expected_manifest: Path) -> dict[str, str]:
    expected = read_sha256_file(expected_manifest)
    actual = snapshot(baseline)
    require(actual == expected, "baseline tree is not the exact 30-file selected snapshot")
    return actual


def check_contract(root: Path, stage: str) -> dict[str, str]:
    contract_text = (root / CONTRACT).read_text(encoding="utf-8")
    cmake_text = (root / CMAKE).read_text(encoding="utf-8")
    fixture_text = (root / FIXTURE).read_text(encoding="utf-8")
    require(TEST_NAME in contract_text and TEST_NAME in cmake_text,
            f"{stage}: exact test name is absent")
    missing = [item for item in REQUIRED_CONTRACT_SNIPPETS if item not in contract_text]
    require(not missing, f"{stage}: frozen contract snippets missing: {missing}")
    return {str(path): digest(root / path) for path in FROZEN}


def compare_final_copies(work: Path, final_root: Path) -> None:
    for relative in (*FROZEN, FIXTURE):
        delivered = final_root / relative
        require(delivered.is_file() and delivered.stat().st_size > 0,
                f"missing/nonempty final changed file: {delivered}")
        require(digest(delivered) == digest(work / relative),
                f"final changed file differs from sequential green state: {relative}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("baseline_project", type=Path)
    parser.add_argument(
        "delivery_root",
        nargs="?",
        type=Path,
        default=Path(__file__).resolve().parent.parent,
    )
    args = parser.parse_args()

    baseline = args.baseline_project.resolve()
    delivery = args.delivery_root.resolve()
    patches = delivery / "patches"
    final_root = delivery / "final-changed-files" / "project"
    baseline_manifest = delivery / "evidence" / "BASELINE_PROJECT.sha256"
    red_patch = patches / "0001-red-freeze-dcp-rcb-high-precision-contract.patch"
    green_patch = patches / "0002-green-replace-only-precision-fixture.patch"
    aggregate_patch = patches / "candidate-final.patch"

    require(baseline.is_dir(), f"baseline project not found: {baseline}")
    for path in (baseline_manifest, red_patch, green_patch, aggregate_patch):
        require(path.is_file() and path.stat().st_size > 0, f"required input missing/empty: {path}")

    baseline_state = check_exact_baseline(baseline, baseline_manifest)

    with tempfile.TemporaryDirectory(prefix="precision-contract-") as temporary:
        temporary_root = Path(temporary)
        sequential = temporary_root / "sequential"
        aggregate = temporary_root / "aggregate"
        shutil.copytree(baseline, sequential)
        shutil.copytree(baseline, aggregate)

        run("git", "apply", "--check", str(red_patch), cwd=sequential)
        run("git", "apply", str(red_patch), cwd=sequential)
        red_state = snapshot(sequential)
        red_changed = changed(baseline_state, red_state)
        require(red_changed == list(EXPECTED_CHANGED),
                f"red changed unexpected paths: {red_changed}")
        red_frozen = check_contract(sequential, "red")
        red_fixture_hash = digest(sequential / FIXTURE)
        red_fixture_text = (sequential / FIXTURE).read_text(encoding="utf-8")
        require("convert_to<double>()" in red_fixture_text,
                "red fixture does not explicitly collapse to binary64")
        require("MakeCKKSPackedPlaintext" in red_fixture_text,
                "red fixture does not use standard packed encoding")
        require("Intentionally incomplete RED fixture" in red_fixture_text,
                "red fixture lacks its non-defect limitation label")

        run("git", "apply", "--check", str(green_patch), cwd=sequential)
        run("git", "apply", str(green_patch), cwd=sequential)
        green_state = snapshot(sequential)
        green_changed_from_red = changed(red_state, green_state)
        require(green_changed_from_red == [str(FIXTURE)],
                f"green must change only the fixture: {green_changed_from_red}")
        green_frozen = check_contract(sequential, "green")
        require(red_frozen == green_frozen,
                "CMake/header/contract hashes changed between red and green")
        green_fixture_hash = digest(sequential / FIXTURE)
        require(red_fixture_hash != green_fixture_hash,
                "red and green fixture hashes unexpectedly match")

        final_contract = (sequential / CONTRACT).read_text(encoding="utf-8")
        final_fixture = (sequential / FIXTURE).read_text(encoding="utf-8")
        for forbidden in FORBIDDEN_SUBSTRINGS:
            require(forbidden not in final_contract and forbidden not in final_fixture,
                    f"forbidden stale-cache/decrypt/blanket-catch text found: {forbidden}")
        require("GetElement<lbcrypto::DCRTPoly>()" in final_fixture,
                "green fixture lacks public plaintext DCRT access")
        require("RoundHalfAwayFromZero" in final_fixture and
                "if (value >= 0)" in final_fixture and
                "else" in final_fixture,
                "green fixture lacks explicit positive/negative rounding branches")
        require("DirectCanonicalEvaluate" not in final_fixture,
                "fixture duplicates the independently discriminating direct oracle")
        require("GetCKKSPackedValue" not in final_fixture,
                "fixture consumes its stale packed-value cache")

        run("git", "apply", "--check", str(aggregate_patch), cwd=aggregate)
        run("git", "apply", str(aggregate_patch), cwd=aggregate)
        aggregate_state = snapshot(aggregate)
        require(changed(baseline_state, aggregate_state) == list(EXPECTED_CHANGED),
                "aggregate patch changed an unexpected path")
        require(aggregate_state == green_state,
                "aggregate patch output differs from sequential red+green output")
        compare_final_copies(sequential, final_root)

        print("RECONSTRUCTION_STATUS=RECONSTRUCTED")
        print("BASELINE_FILE_COUNT=30")
        print("BASELINE_EXACT_HASH_SET=PASS")
        print("RED_PATCH_APPLY=PASS")
        print("GREEN_PATCH_APPLY=PASS")
        print("AGGREGATE_PATCH_APPLY=PASS")
        print("RED_CHANGED=" + ",".join(red_changed))
        print("GREEN_CHANGED_FROM_RED=" + ",".join(green_changed_from_red))
        print("AGGREGATE_EQUALS_SEQUENTIAL_GREEN=PASS")
        print("FINAL_COPIES_EQUAL_SEQUENTIAL_GREEN=PASS")
        print("TEST_NAME=" + TEST_NAME)
        for relative in FROZEN:
            print(f"FROZEN_SHA256 {red_frozen[str(relative)]}  {relative}")
        print(f"RED_FIXTURE_SHA256 {red_fixture_hash}  {FIXTURE}")
        print(f"GREEN_FIXTURE_SHA256 {green_fixture_hash}  {FIXTURE}")
        print("STALE_CACHE_GETTER=ABSENT")
        print("PRODUCTION_DECRYPT_CALL=ABSENT")
        print("BLANKET_CATCH=ABSENT")
        print("DIRECT_EXPECTED_ORACLE_IN_FIXTURE=ABSENT")
        print("CONTRACT_CONTINUITY=PASS")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (RuntimeError, OSError, ValueError) as exception:
        print(f"VERIFY_CONTRACT_CONTINUITY=FAIL: {exception}", file=sys.stderr)
        raise SystemExit(1)
