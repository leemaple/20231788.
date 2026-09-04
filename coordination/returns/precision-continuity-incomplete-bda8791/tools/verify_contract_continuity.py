#!/usr/bin/env python3
"""Verify that the red/green series preserves one literal public-behavior contract."""

from __future__ import annotations

import argparse
import hashlib
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


CONTRACT = Path("tests/precision_dcp_rcb_contract_test.cpp")
HEADER = Path("tests/precision_dcp_rcb_fixture.h")
FIXTURE = Path("tests/precision_dcp_rcb_fixture.cpp")
CMAKE = Path("CMakeLists.txt")
FROZEN = (CONTRACT, HEADER, CMAKE)
TEST_NAME = "precision_dcp_rcb_high_precision_contract"
REQUIRED_CONTRACT_SNIPPETS = (
    "constexpr std::uint32_t kAcceptanceBits = 80;",
    "BigFloat(1) / Pow2Float(70)",
    "BigFloat(1) / Pow2Float(73)",
    "deltaError <= tolerance",
    "maximumError <= tolerance",
    "module.DCP(input)",
    "module.RCB(pair)",
    "CheckBinary64NegativeControl(context, expected)",
    "CheckCanonicalOracleWitnesses()",
    "kMinimumWrapHeadroomBits = 128",
)
FORBIDDEN_IN_FINAL_TEST = (
    "GetCKKSPackedValue",
    "context->Decrypt(",
    "catch (",
)


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
        raise RuntimeError(f"command failed ({process.returncode}): {' '.join(args)}\n{process.stdout}")
    return process.stdout


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def snapshot(root: Path) -> dict[str, str]:
    return {
        str(path.relative_to(root)): digest(path)
        for path in sorted(root.rglob("*"))
        if path.is_file() and ".git" not in path.parts
    }


def changed(before: dict[str, str], after: dict[str, str]) -> list[str]:
    keys = sorted(set(before) | set(after))
    return [key for key in keys if before.get(key) != after.get(key)]


def require(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("baseline_project", type=Path)
    parser.add_argument("patch_directory", type=Path)
    args = parser.parse_args()

    baseline = args.baseline_project.resolve()
    patch_directory = args.patch_directory.resolve()
    red_patch = patch_directory / "0001-red-freeze-dcp-rcb-high-precision-contract.patch"
    green_patch = patch_directory / "0002-green-replace-only-precision-fixture.patch"
    require(baseline.is_dir(), f"baseline project not found: {baseline}")
    require(red_patch.is_file(), f"red patch not found: {red_patch}")
    require(green_patch.is_file(), f"green patch not found: {green_patch}")

    with tempfile.TemporaryDirectory(prefix="precision-contract-") as temporary:
        work = Path(temporary) / "project"
        shutil.copytree(baseline, work)
        baseline_state = snapshot(work)

        run("git", "apply", "--check", str(red_patch), cwd=work)
        run("git", "apply", str(red_patch), cwd=work)
        red_state = snapshot(work)
        red_changed = changed(baseline_state, red_state)
        expected_red_changed = sorted(
            str(path)
            for path in (CMAKE, CONTRACT, HEADER, FIXTURE)
        )
        require(red_changed == expected_red_changed,
                f"red changed unexpected paths: {red_changed}")

        red_hashes = {str(path): digest(work / path) for path in FROZEN}
        red_fixture_hash = digest(work / FIXTURE)
        contract_text = (work / CONTRACT).read_text(encoding="utf-8")
        cmake_text = (work / CMAKE).read_text(encoding="utf-8")
        fixture_text = (work / FIXTURE).read_text(encoding="utf-8")
        require(TEST_NAME in contract_text and TEST_NAME in cmake_text,
                "red state does not register/use the exact frozen test name")
        require(all(snippet in contract_text for snippet in REQUIRED_CONTRACT_SNIPPETS),
                "red state is missing a frozen contract element")
        require("convert_to<double>()" in fixture_text and
                "MakeCKKSPackedPlaintext" in fixture_text,
                "red fixture is not the declared incomplete binary64 fixture")

        run("git", "apply", "--check", str(green_patch), cwd=work)
        run("git", "apply", str(green_patch), cwd=work)
        green_state = snapshot(work)
        green_changed_from_red = changed(red_state, green_state)
        require(green_changed_from_red == [str(FIXTURE)],
                f"green must change only the fixture, changed: {green_changed_from_red}")

        green_hashes = {str(path): digest(work / path) for path in FROZEN}
        require(red_hashes == green_hashes,
                f"frozen files changed between red and green: {red_hashes} != {green_hashes}")
        green_fixture_hash = digest(work / FIXTURE)
        require(red_fixture_hash != green_fixture_hash,
                "fixture implementation did not change between red and green")

        final_contract = (work / CONTRACT).read_text(encoding="utf-8")
        final_fixture = (work / FIXTURE).read_text(encoding="utf-8")
        for forbidden in FORBIDDEN_IN_FINAL_TEST:
            require(forbidden not in final_contract and forbidden not in final_fixture,
                    f"forbidden stale-cache/standard-decrypt call found: {forbidden}")
        require("GetElement<lbcrypto::DCRTPoly>()" in final_fixture,
                "green fixture does not use public plaintext DCRT access")
        require("RoundHalfAwayFromZero" in final_fixture and
                "if (value >= 0)" in final_fixture,
                "green fixture lacks explicit portable rounding branches")
        require("DirectCanonicalEvaluate" not in final_fixture,
                "green fixture improperly duplicates the direct expected-value oracle")
        require(TEST_NAME in final_contract and TEST_NAME in (work / CMAKE).read_text(encoding="utf-8"),
                "green state lost or renamed the frozen test")
        require(all(snippet in final_contract for snippet in REQUIRED_CONTRACT_SNIPPETS),
                "green state relaxed or removed a frozen contract element")

        print("PATCH_APPLY=PASS")
        print("RED_CHANGED=" + ",".join(red_changed))
        print("GREEN_CHANGED_FROM_RED=" + ",".join(green_changed_from_red))
        print("TEST_NAME=" + TEST_NAME)
        for path in FROZEN:
            print(f"FROZEN_SHA256 {red_hashes[str(path)]}  {path}")
        print(f"RED_FIXTURE_SHA256 {red_fixture_hash}  {FIXTURE}")
        print(f"GREEN_FIXTURE_SHA256 {green_fixture_hash}  {FIXTURE}")
        print("FORBIDDEN_CACHE_GETTER=ABSENT")
        print("STANDARD_DECRYPT_CALL=ABSENT")
        print("BLANKET_CATCH=ABSENT")
        print("GREEN_ONLY_FIXTURE=PASS")
        print("CONTRACT_CONTINUITY=PASS")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (RuntimeError, OSError, ValueError) as exception:
        print(f"VERIFY_CONTRACT_CONTINUITY=FAIL: {exception}", file=sys.stderr)
        raise SystemExit(1)
