#!/usr/bin/env python3
"""Verify the first-Mult2 precision proposal against the exact supplied project tree.

This tool performs byte/static checks only. It does not configure or execute OpenFHE.
Run with PYTHONDONTWRITEBYTECODE=1 to avoid cache output.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
from typing import Iterable

EXPECTED_SOURCE_COMMIT = "c9ee28d0370eeee1ec7a1965402ed0b5e91f425e"
EXPECTED_PATCH_SHA256 = "acf043d3a04876e71ddbf17c14f8f579c1a7cb86a032d881243e27e010b6a105"
EXPECTED_CMAKE_SHA256 = "48647eb2caa5cc481d8923c4d97f6836528f88cd74c8339b2cd31d71b9cb686e"
EXPECTED_TEST_SHA256 = "60a7c27c7f88420b1c55899bbe4d4b3ad54cbbf6a06da90d2fdfb0a0646770db"
EXPECTED_CONTRACT_SHA256 = "7d6b7f5e1c820cf49641dc1606fa64984a52267e2f04305c2ad5bd5981d2036b"
EXPECTED_BASELINE_HASHES = {
    "CMakeLists.txt": "ab5a9873f90f5ab7d292dca4e54684e1242102ac469e0810f71508b26e39c91b",
    ".github/workflows/dcp-rcb.yml": "e3e1d23250b73f70747a3681975e3da870d1f337a3e2f54e674bb05a8900f951",
    "include/openfhe_2023_1788/double_ckks.h": "3d089be507d7ea64e64b5ed3eb97b25ec249675c4da19e8e90d517151654bdfb",
    "src/double_ckks.cpp": "3ed23f2261cd7d8efea7a76a5724bfe44f6f20c521c20aedccd4fec69e6e916a",
    "tests/precision_dcp_rcb_contract_test.cpp": "ad677414499c3e98e7f798ed940d587cb35c6cc791c7b0f81166ca1e6917f854",
    "tests/precision_dcp_rcb_fixture.h": "4b7b1c4f2670f5dc93e8d28f1ad585a47bb9cf4b81130bb45200a3af82e6b554",
    "tests/precision_dcp_rcb_fixture.cpp": "4bcd633c3fb4b6ad4fa5d2088908e7992ccd50dec0d3b826fe976e590a3aa596",
}
REQUIRED = [
    "README.md",
    "PRECURSOR_REVIEW.md",
    "FIRST_MULT2_TEST_DESIGN.md",
    "SOURCE_CLAIM_TEST_LEDGER.md",
    "HOSTED_COMMANDS_AND_EXPECTED_REGISTRATION.md",
    "NEXT_PAPER_PRECISION_GATES.md",
    "EXECUTION_LEDGER.md",
    "SOURCE-AND-DELIVERY-RECORD.json",
    "patches/0001-add-first-mult2-high-precision-contract.patch",
    "final-changed-files/project/CMakeLists.txt",
    "final-changed-files/project/tests/precision_first_mult2_contract_test.cpp",
    "evidence/INPUT_INTEGRITY.txt",
    "evidence/PRECURSOR_RUNTIME_AUDIT.txt",
    "evidence/PRECURSOR_NAMESPACE_DIFF.txt",
    "evidence/CURRENT_54_CTEST_BINDINGS.tsv",
    "evidence/CANDIDATE_55_CTEST_BINDINGS.tsv",
    "evidence/CTEST_CONTINUITY.txt",
    "evidence/PATCH_APPLY_AND_FINAL_EQUALITY.txt",
    "evidence/STATIC_SOURCE_GUARDS.txt",
    "evidence/FROZEN_CONTRACT.json",
    "evidence/FROZEN_CONTRACT.sha256",
    "evidence/STANDALONE_FIRST_MULT2_MATH.txt",
    "evidence/STANDALONE_BUILD_RECORD.txt",
    "evidence/CMAKE_ENVIRONMENT_PROBE.txt",
    "evidence/SOURCE_HASHES.tsv",
    "evidence/FINAL_CHANGED_FILES.sha256",
    "evidence/REQUIRED_MEMBER_CHECK.txt",
    "tools/standalone_first_mult2_contract_math.cpp",
    "tools/verify_delivery.py",
    "OUTPUT_TREE.txt",
    "MANIFEST.sizes-sha256.tsv",
    "MANIFEST.sha256",
]
FORBIDDEN_DIR_NAMES = {".git", "__pycache__", "CMakeFiles", "build", "_deps"}
FORBIDDEN_SUFFIXES = {
    ".pyc", ".pyo", ".o", ".obj", ".a", ".lib", ".so", ".dylib",
    ".dll", ".exe", ".class", ".jar", ".zip", ".tar", ".gz", ".7z",
}


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def regular_files(root: Path) -> list[str]:
    return sorted(
        path.relative_to(root).as_posix()
        for path in root.rglob("*")
        if path.is_file() and not path.is_symlink()
    )


def parse_tests(cmake_text: str) -> list[tuple[str, str]]:
    pattern = re.compile(
        r"add_test\s*\(\s*NAME\s+([^\s\)]+)\s+COMMAND\s+(.+?)\)",
        re.DOTALL,
    )
    result: list[tuple[str, str]] = []
    for match in pattern.finditer(cmake_text):
        command = " ".join(match.group(2).split())
        result.append((match.group(1), command))
    return result


def run(command: list[str], cwd: Path, env: dict[str, str] | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command,
        cwd=cwd,
        env=env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )


def copy_baseline(source: Path, destination: Path) -> None:
    shutil.copytree(
        source,
        destination,
        ignore=shutil.ignore_patterns(".git", "__pycache__", "*.pyc", "build", "CMakeFiles"),
    )


def verify_manifest(root: Path, failures: list[str], messages: list[str]) -> None:
    manifest_path = root / "MANIFEST.sha256"
    listed: dict[str, str] = {}
    for number, raw in enumerate(manifest_path.read_text(encoding="utf-8").splitlines(), 1):
        if not raw.strip():
            continue
        match = re.fullmatch(r"([0-9a-f]{64})  (.+)", raw)
        if not match:
            failures.append(f"MANIFEST.sha256 malformed line {number}")
            continue
        digest, rel = match.groups()
        if rel in listed:
            failures.append(f"MANIFEST.sha256 duplicate path: {rel}")
        listed[rel] = digest
    expected = set(regular_files(root)) - {"MANIFEST.sha256"}
    if set(listed) != expected:
        failures.append(
            "MANIFEST.sha256 path closure mismatch: "
            f"missing={sorted(expected-set(listed))}, extra={sorted(set(listed)-expected)}"
        )
    for rel, expected_digest in listed.items():
        path = root / rel
        if path.is_file() and sha256(path) != expected_digest:
            failures.append(f"MANIFEST.sha256 hash mismatch: {rel}")
    messages.append(f"MANIFEST_MEMBER_COUNT={len(listed)}")

    table_path = root / "MANIFEST.sizes-sha256.tsv"
    rows = table_path.read_text(encoding="utf-8").splitlines()
    if not rows or rows[0] != "bytes\tsha256\tpath":
        failures.append("MANIFEST.sizes-sha256.tsv header mismatch")
        return
    table: dict[str, tuple[int, str]] = {}
    for number, raw in enumerate(rows[1:], 2):
        parts = raw.split("\t")
        if len(parts) != 3 or not parts[0].isdigit() or not re.fullmatch(r"[0-9a-f]{64}", parts[1]):
            failures.append(f"MANIFEST.sizes-sha256.tsv malformed line {number}")
            continue
        size, digest, rel = int(parts[0]), parts[1], parts[2]
        if rel in table:
            failures.append(f"MANIFEST.sizes-sha256.tsv duplicate path: {rel}")
        table[rel] = (size, digest)
    table_expected = set(regular_files(root)) - {"MANIFEST.sha256", "MANIFEST.sizes-sha256.tsv"}
    if set(table) != table_expected:
        failures.append(
            "MANIFEST.sizes-sha256.tsv path closure mismatch: "
            f"missing={sorted(table_expected-set(table))}, extra={sorted(set(table)-table_expected)}"
        )
    for rel, (expected_size, expected_digest) in table.items():
        path = root / rel
        if path.is_file():
            if path.stat().st_size != expected_size:
                failures.append(f"size-table size mismatch: {rel}")
            if sha256(path) != expected_digest:
                failures.append(f"size-table hash mismatch: {rel}")
    messages.append(f"SIZE_SHA_TABLE_COUNT={len(table)}")


def verify_output_tree(root: Path, failures: list[str], messages: list[str]) -> None:
    listed = root.joinpath("OUTPUT_TREE.txt").read_text(encoding="utf-8").splitlines()
    actual: list[str] = []
    for path in sorted(root.rglob("*"), key=lambda p: p.relative_to(root).as_posix()):
        rel = path.relative_to(root).as_posix()
        if path.is_symlink():
            actual.append(f"L\t{rel}")
        elif path.is_dir():
            actual.append(f"D\t{rel}/")
        elif path.is_file():
            actual.append(f"F\t{rel}")
    if listed != actual:
        failures.append("OUTPUT_TREE.txt does not exactly describe the extracted tree")
    messages.append(f"OUTPUT_TREE_ENTRY_COUNT={len(listed)}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--baseline-project", required=True, type=Path)
    parser.add_argument("--delivery-root", required=True, type=Path)
    args = parser.parse_args()
    baseline = args.baseline_project.resolve()
    delivery = args.delivery_root.resolve()
    failures: list[str] = []
    messages: list[str] = []

    if not baseline.is_dir():
        failures.append(f"baseline project missing: {baseline}")
    if not delivery.is_dir():
        failures.append(f"delivery root missing: {delivery}")
    if failures:
        print("\n".join(f"FAIL={item}" for item in failures))
        return 1

    for rel in REQUIRED:
        path = delivery / rel
        if not path.is_file() or path.stat().st_size == 0:
            failures.append(f"required member missing or empty: {rel}")
    messages.append(f"REQUIRED_MEMBER_COUNT={len(REQUIRED)}")

    files = regular_files(delivery)
    symlinks = [p.relative_to(delivery).as_posix() for p in delivery.rglob("*") if p.is_symlink()]
    if symlinks:
        failures.append(f"symlinks present: {symlinks}")
    forbidden: list[str] = []
    for path in delivery.rglob("*"):
        rel = path.relative_to(delivery).as_posix()
        if any(part in FORBIDDEN_DIR_NAMES for part in path.relative_to(delivery).parts):
            forbidden.append(rel)
        elif path.is_file() and path.suffix.lower() in FORBIDDEN_SUFFIXES:
            forbidden.append(rel)
    if forbidden:
        failures.append(f"forbidden delivery members: {sorted(set(forbidden))}")
    messages.append(f"REGULAR_FILE_COUNT={len(files)}")
    messages.append(f"FORBIDDEN_MEMBER_COUNT={len(set(forbidden))}")

    for rel, expected in EXPECTED_BASELINE_HASHES.items():
        path = baseline / rel
        if not path.is_file() or sha256(path) != expected:
            failures.append(f"baseline identity mismatch: {rel}")
    messages.append(f"BASELINE_SELECTED_COMMIT={EXPECTED_SOURCE_COMMIT}")

    patch = delivery / "patches/0001-add-first-mult2-high-precision-contract.patch"
    if patch.is_file() and sha256(patch) != EXPECTED_PATCH_SHA256:
        failures.append("candidate patch hash mismatch")
    final_cmake = delivery / "final-changed-files/project/CMakeLists.txt"
    final_test = delivery / "final-changed-files/project/tests/precision_first_mult2_contract_test.cpp"
    if final_cmake.is_file() and sha256(final_cmake) != EXPECTED_CMAKE_SHA256:
        failures.append("delivered final CMake hash mismatch")
    if final_test.is_file() and sha256(final_test) != EXPECTED_TEST_SHA256:
        failures.append("delivered final test hash mismatch")

    if patch.is_file():
        with tempfile.TemporaryDirectory(prefix="first-mult2-verify-") as temp_name:
            temp = Path(temp_name) / "project"
            copy_baseline(baseline, temp)
            commands = [
                ["git", "init", "-q"],
                ["git", "config", "user.email", "verify@example.invalid"],
                ["git", "config", "user.name", "Static verifier"],
                ["git", "config", "core.autocrlf", "false"],
                ["git", "add", "-A"],
                ["git", "commit", "-q", "-m", "baseline"],
                ["git", "apply", "--check", str(patch)],
                ["git", "apply", "--index", str(patch)],
            ]
            for command in commands:
                result = run(command, temp)
                if result.returncode != 0:
                    failures.append(f"command failed: {' '.join(command)} :: {result.stdout.strip()}")
                    break
            else:
                changed_result = run(["git", "diff", "--cached", "--name-only"], temp)
                changed = [line for line in changed_result.stdout.splitlines() if line]
                expected_changed = ["CMakeLists.txt", "tests/precision_first_mult2_contract_test.cpp"]
                if sorted(changed) != sorted(expected_changed):
                    failures.append(f"patch changed-path closure mismatch: {changed}")
                if (temp / "CMakeLists.txt").read_bytes() != final_cmake.read_bytes():
                    failures.append("patched CMake does not equal delivered final copy")
                if (temp / "tests/precision_first_mult2_contract_test.cpp").read_bytes() != final_test.read_bytes():
                    failures.append("patched test does not equal delivered final copy")
                baseline_tests = parse_tests((baseline / "CMakeLists.txt").read_text(encoding="utf-8"))
                candidate_tests = parse_tests((temp / "CMakeLists.txt").read_text(encoding="utf-8"))
                added = [("precision_first_mult2_high_precision_contract", "precision_first_mult2_contract_test")]
                filtered = [entry for entry in candidate_tests if entry not in added]
                if len(baseline_tests) != 54:
                    failures.append(f"baseline CTest count is {len(baseline_tests)}, expected 54")
                if len(candidate_tests) != 55:
                    failures.append(f"candidate CTest count is {len(candidate_tests)}, expected 55")
                if filtered != baseline_tests:
                    failures.append("original CTest name/command sequence changed")
                if [entry for entry in candidate_tests if entry not in baseline_tests] != added:
                    failures.append("candidate does not add exactly the frozen CTest binding")
                messages.append(f"PATCH_CHANGED_PATH_COUNT={len(changed)}")
                messages.append(f"BASELINE_CTEST_COUNT={len(baseline_tests)}")
                messages.append(f"CANDIDATE_CTEST_COUNT={len(candidate_tests)}")

    if final_test.is_file():
        source = final_test.read_text(encoding="utf-8")
        source_checks = {
            "no stale packed getter": "GetCKKSPackedValue" not in source,
            "no production decrypt call": not re.search(r"\b(?:context|cc|cryptoContext)\s*(?:->|\.)\s*Decrypt\s*\(", source),
            "no serialization": not re.search(r"\b(?:Serialize|Deserialize|Serial::)\b", source),
            "no crypto seeding": not re.search(r"\b(?:SetSeed|PseudoRandom|PRNG|seed)\b", source, re.I),
            "only narrow catch": source.count("catch (") == 1 and "catch (const TestFailure& error)" in source,
            "four fresh keys": "kFreshKeyTrials = 4" in source and "context->KeyGen()" in source,
            "frozen threshold": "kAcceptanceBits = 80" in source,
            "exact scale": "kProductScaleNumeratorBits = 200" in source and "exactProductScaleDenominator = qDiv * qL" in source,
            "public Mult2": "module.Mult2(leftPair, rightPair)" in source,
            "public RCB": "module.RCB(result)" in source,
            "frozen products": "FrozenExpectedProducts()" in source and "CheckHostProductsAgainstFrozen" in source,
            "direct canonical witnesses": "X2WitnessTable()" in source and "X^32 canonical phase witness" in source,
            "result immutable across RCB": "resultBeforeRcb" in source and "CheckPairUnchanged(result, resultBeforeRcb" in source,
        }
        for label, ok in source_checks.items():
            if not ok:
                failures.append(f"source guard failed: {label}")
        messages.append(f"SOURCE_GUARD_COUNT={len(source_checks)}")

    contract_path = delivery / "evidence/FROZEN_CONTRACT.json"
    if contract_path.is_file():
        if sha256(contract_path) != EXPECTED_CONTRACT_SHA256:
            failures.append("frozen contract hash mismatch")
        try:
            contract = json.loads(contract_path.read_text(encoding="utf-8"))
        except json.JSONDecodeError as exc:
            failures.append(f"frozen contract JSON invalid: {exc}")
        else:
            expected_fields = {
                "status": "FROZEN_BEFORE_HOSTED_EXECUTION",
                "testName": "precision_first_mult2_high_precision_contract",
                "exactFinalLogicalScale": "2^200/(q_div*q_l)",
            }
            for key, expected in expected_fields.items():
                if contract.get(key) != expected:
                    failures.append(f"frozen contract field mismatch: {key}")
            if len(contract.get("leftSlots", [])) != 16 or len(contract.get("rightSlots", [])) != 16 or len(contract.get("frozenExpectedProducts", [])) != 16:
                failures.append("frozen contract vectors are not all length 16")

    if all((delivery / name).is_file() for name in ("MANIFEST.sha256", "MANIFEST.sizes-sha256.tsv")):
        verify_manifest(delivery, failures, messages)
    if (delivery / "OUTPUT_TREE.txt").is_file():
        verify_output_tree(delivery, failures, messages)

    status = "PASS" if not failures else "FAIL"
    print(f"DELIVERY_VERIFICATION={status}")
    for message in messages:
        print(message)
    for failure in failures:
        print(f"FAILURE={failure}")
    print("OPENFHE_CONFIGURE_BUILD_RUNTIME=NOT_EXECUTED_BY_THIS_TOOL")
    return 0 if not failures else 1


if __name__ == "__main__":
    sys.exit(main())
