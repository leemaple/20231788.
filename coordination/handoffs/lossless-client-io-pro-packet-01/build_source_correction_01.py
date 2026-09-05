#!/usr/bin/env python3
"""Build the bounded lossless-client-I/O source-closure correction packet."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import stat
import subprocess
import sys
from typing import Dict, Iterable, List, NamedTuple, Tuple
import zipfile


ORIGINAL_ARCHIVE_NAME = "lossless-client-io-implementation-01-4ccc8fd.zip"
ORIGINAL_ARCHIVE_BYTES = 1_294_234
ORIGINAL_ARCHIVE_SHA256 = "67cea2db1565550c7d96816d076a5d56be45e82f5175e9578e31ddbb50289f89"
ORIGINAL_ROOT = "lossless-client-io-implementation-01-4ccc8fd"
ORIGINAL_MEMBERS = 113
ORIGINAL_PAYLOADS = 111

DESIGN_ARCHIVE_NAME = "lossless-client-io-design-b64a980.zip"
DESIGN_ARCHIVE_BYTES = 1_531_734
DESIGN_ARCHIVE_SHA256 = "efd18ebf2f753624251b1ad60da08d8e31c431ef91495fe93e8951d6cd3f24cc"

OPENFHE_REMOTE = "https://github.com/openfheorg/openfhe-development.git"
OPENFHE_PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
IMPLEMENTATION_BASE = "4ccc8fd2e7617625d27e58a53eb3489e99466ed4"
TASK_OVERLAY = "a6937904887d17dffcfcf8a2367b9b4244c52961"
TESTED_ENGINEERING = "4ecbd972429884489918d9f82dfc3fe9f702ef4a"

TASK_PATH = "TASK.md"
TASK_BYTES = 23_771
TASK_SHA256 = "707d366dcd4880450ac09ba4c1eb6195daf64def65333c305c20a099f8eadb1f"
FROZEN_CONTRACT_PATH = (
    "project/coordination/returns/lossless-client-io-pro-b64a980/"
    "PROPOSED_FIRST_TDD_CONTRACT.md"
)
FROZEN_CONTRACT_BYTES = 17_687
FROZEN_CONTRACT_SHA256 = "d85823ccb318833b01e526ea9e2e9930645d5cb09ccd9a446b0ec3d862f6c6af"

CORRECTION_DOCUMENTS = ("SOURCE_CORRECTION_01.md", "SOURCE_CORRECTION_01.json")
ORIGINAL_MANIFEST_PATH = "MANIFEST.tsv"
ORIGINAL_MANIFEST_SIDECAR_PATH = "MANIFEST.sha256"
ORIGINAL_PROVENANCE_PREFIX = "provenance/original-input"
OUTPUT_ARCHIVE_NAME = "lossless-client-io-implementation-01-4ccc8fd-source-correction-01.zip"
OUTPUT_BUILD_RECORD_NAME = "SOURCE_SELECTION_BUILD.json"
FIXED_ZIP_TIME = (2026, 9, 5, 0, 0, 0)

FORBIDDEN_PARTS = {
    ".git",
    ".env",
    ".cache",
    "__pycache__",
    "node_modules",
    "build",
    "browser-state",
    "cookies",
    "credentials",
}


class Source(NamedTuple):
    legacy_path: str
    canonical_path: str
    bytes: int
    sha256: str
    git_blob_sha1: str

    @property
    def corrected_path(self) -> str:
        return f"official-full/{self.canonical_path}"


SOURCES = (
    Source(
        "official-openfhe/dftransform.cpp",
        "src/core/lib/math/dftransform.cpp",
        10_368,
        "091220ee6b29ca1b0efbe8afa41a90098507720f59abcd0db1ed0a6e70fc26f3",
        "ea5b3b2dcc2b9024da0487d3de285c241a8ec8a4",
    ),
    Source(
        "official-openfhe/dftransform.h",
        "src/core/include/math/dftransform.h",
        4_719,
        "7492b8d882a421aa3fa28c2dc2d24548bc914d83e04a719e89c0ff7176dde900",
        "d4ed3000c11618102aa5415c8561809d3aad0273",
    ),
    Source(
        "official-openfhe/ckkspackedencoding.h",
        "src/pke/include/encoding/ckkspackedencoding.h",
        11_651,
        "983981653104dc21680653f3b4e1108b51d0e8a39e40a0d8337f7d6502d0fe6a",
        "956a97ff6f214abb2d690c77bceca0ec746f753a",
    ),
    Source(
        "official-openfhe/plaintext.h",
        "src/pke/include/encoding/plaintext.h",
        13_953,
        "8736ff85fa9366cc06a30d43a40e8dcaa55a240c9f2c881c880eb79f3cc340cb",
        "19d32771478cd5365248d5a6630e28b7501b47fb",
    ),
    Source(
        "official-openfhe/plaintextfactory.h",
        "src/pke/include/encoding/plaintextfactory.h",
        6_826,
        "16f788d3e4cc32e8831bcbe7009f99a05e6ef6f0496c19e6e119e821f98b61a7",
        "a28c475d70d786b0f63ced2bcbcd116642b9a703",
    ),
)

EXPLICITLY_NOT_ADDED = {
    "official-full/src/pke/include/schemebase/base-leveledshe.h",
    "official-full/src/pke/lib/keyswitch/keyswitch-bv.cpp",
}


def fail(message: str) -> None:
    raise RuntimeError(message)


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def git_blob_sha1(data: bytes) -> str:
    prefix = f"blob {len(data)}\0".encode("ascii")
    return hashlib.sha1(prefix + data).hexdigest()


def validate_relative_path(value: str, *, expected_root: str | None = None) -> PurePosixPath:
    if not value or "\x00" in value or "\\" in value:
        fail(f"unsafe ZIP path spelling: {value!r}")
    path = PurePosixPath(value)
    if path.is_absolute() or any(part in {"", ".", ".."} for part in path.parts):
        fail(f"unsafe ZIP path: {value!r}")
    if expected_root is not None and (not path.parts or path.parts[0] != expected_root):
        fail(f"ZIP member is outside the one-root layout: {value!r}")
    for part in path.parts:
        if part.lower() in FORBIDDEN_PARTS:
            fail(f"forbidden state/dependency path in packet: {value!r}")
    return path


def validate_zip_info(info: zipfile.ZipInfo, *, expected_root: str | None = None) -> None:
    validate_relative_path(info.filename, expected_root=expected_root)
    if info.is_dir():
        fail(f"directory entry is not permitted: {info.filename}")
    mode = info.external_attr >> 16
    if info.create_system != 3 or not stat.S_ISREG(mode) or stat.S_IMODE(mode) != 0o644:
        fail(f"member is not a Unix regular 0644 file: {info.filename} mode={oct(mode)}")


def verify_archive_identity(path: Path, expected_name: str, expected_bytes: int, expected_sha: str) -> None:
    if not path.is_file():
        fail(f"required archive is absent: {path}")
    if path.name != expected_name:
        fail(f"archive basename mismatch: expected {expected_name}, got {path.name}")
    if path.stat().st_size != expected_bytes:
        fail(f"archive byte-size mismatch for {path}")
    if sha256_file(path) != expected_sha:
        fail(f"archive SHA-256 mismatch for {path}")


def parse_manifest(data: bytes, expected_records: int) -> List[Tuple[str, int, str]]:
    try:
        text = data.decode("utf-8")
    except UnicodeDecodeError as exc:
        fail(f"manifest is not UTF-8: {exc}")
    if not text.endswith("\n"):
        fail("manifest must end with a newline")
    lines = text.splitlines()
    if not lines or lines[0] != "sha256\tbytes\tpath":
        fail("manifest header mismatch")
    rows: List[Tuple[str, int, str]] = []
    seen = set()
    for line in lines[1:]:
        fields = line.split("\t", 2)
        if len(fields) != 3:
            fail(f"malformed manifest row: {line!r}")
        digest, size_text, name = fields
        if len(digest) != 64 or any(ch not in "0123456789abcdef" for ch in digest):
            fail(f"malformed manifest digest for {name!r}")
        try:
            size = int(size_text)
        except ValueError as exc:
            fail(f"malformed manifest size for {name!r}: {exc}")
        validate_relative_path(name)
        if name in seen or name in {ORIGINAL_MANIFEST_PATH, ORIGINAL_MANIFEST_SIDECAR_PATH}:
            fail(f"duplicate or recursive manifest path: {name!r}")
        seen.add(name)
        rows.append((name, size, digest))
    if len(rows) != expected_records:
        fail(f"manifest row count mismatch: expected {expected_records}, got {len(rows)}")
    return rows


def verify_original_archive(path: Path) -> Tuple[Dict[str, bytes], bytes, bytes]:
    verify_archive_identity(path, ORIGINAL_ARCHIVE_NAME, ORIGINAL_ARCHIVE_BYTES, ORIGINAL_ARCHIVE_SHA256)
    with zipfile.ZipFile(path, "r") as archive:
        infos = archive.infolist()
        if len(infos) != ORIGINAL_MEMBERS:
            fail(f"original member count mismatch: {len(infos)}")
        names = [info.filename for info in infos]
        if len(set(names)) != len(names):
            fail("original archive has duplicate member names")
        for info in infos:
            validate_zip_info(info, expected_root=ORIGINAL_ROOT)
        if archive.testzip() is not None:
            fail("original archive CRC validation failed")

        prefix = f"{ORIGINAL_ROOT}/"
        manifest_name = prefix + ORIGINAL_MANIFEST_PATH
        sidecar_name = prefix + ORIGINAL_MANIFEST_SIDECAR_PATH
        manifest_data = archive.read(manifest_name)
        sidecar_data = archive.read(sidecar_name)
        expected_sidecar = f"{sha256(manifest_data)}  {ORIGINAL_MANIFEST_PATH}\n".encode("ascii")
        if sidecar_data != expected_sidecar:
            fail("original MANIFEST.sha256 does not authenticate MANIFEST.tsv")

        rows = parse_manifest(manifest_data, ORIGINAL_PAYLOADS)
        expected_names = {prefix + name for name, _, _ in rows} | {manifest_name, sidecar_name}
        if set(names) != expected_names:
            fail("original manifest/member closure mismatch")

        payloads: Dict[str, bytes] = {}
        for name, expected_size, expected_sha in rows:
            data = archive.read(prefix + name)
            if len(data) != expected_size or sha256(data) != expected_sha:
                fail(f"original payload mismatch: {name}")
            payloads[name] = data

    if len(payloads) != ORIGINAL_PAYLOADS:
        fail("original payload count changed unexpectedly")
    if len([name for name in payloads if name.startswith("project/")]) != 47:
        fail("original project payload count is not 47")
    if len([name for name in payloads if name.startswith("paper/")]) != 2:
        fail("original paper payload count is not 2")
    if len([name for name in payloads if name.startswith("official-full/")]) != 59:
        fail("original official-full payload count is not 59")
    if len(payloads[TASK_PATH]) != TASK_BYTES or sha256(payloads[TASK_PATH]) != TASK_SHA256:
        fail("TASK.md identity mismatch")
    frozen = payloads[FROZEN_CONTRACT_PATH]
    if len(frozen) != FROZEN_CONTRACT_BYTES or sha256(frozen) != FROZEN_CONTRACT_SHA256:
        fail("frozen first-TDD contract identity mismatch")

    provenance = json.loads(payloads["SOURCE_PROVENANCE.json"])
    if (
        provenance.get("implementationBaseCommit") != IMPLEMENTATION_BASE
        or provenance.get("taskOverlayCommit") != TASK_OVERLAY
        or provenance.get("testedEngineeringCommit") != TESTED_ENGINEERING
        or provenance.get("openfheCommit") != OPENFHE_PIN
        or provenance.get("officialSource", {}).get("regularFiles") != 59
    ):
        fail("original source provenance authority mismatch")
    return payloads, manifest_data, sidecar_data


def verify_design_archive(path: Path) -> Dict[str, bytes]:
    verify_archive_identity(path, DESIGN_ARCHIVE_NAME, DESIGN_ARCHIVE_BYTES, DESIGN_ARCHIVE_SHA256)
    wanted = {source.legacy_path: source for source in SOURCES}
    with zipfile.ZipFile(path, "r") as archive:
        infos = archive.infolist()
        names = [info.filename for info in infos]
        if len(infos) != 172 or len(set(names)) != len(names):
            fail("legacy design archive member/uniqueness check failed")
        for info in infos:
            validate_zip_info(info)
        if archive.testzip() is not None:
            fail("legacy design archive CRC validation failed")
        result: Dict[str, bytes] = {}
        for legacy_path, source in wanted.items():
            if legacy_path not in names:
                fail(f"legacy source member is absent: {legacy_path}")
            data = archive.read(legacy_path)
            if (
                len(data) != source.bytes
                or sha256(data) != source.sha256
                or git_blob_sha1(data) != source.git_blob_sha1
            ):
                fail(f"legacy source identity mismatch: {legacy_path}")
            result[legacy_path] = data
    return result


def run_git(command: List[str], *, cwd: Path | None = None, binary: bool = False) -> bytes | str:
    environment = os.environ.copy()
    environment["GIT_TERMINAL_PROMPT"] = "0"
    process = subprocess.run(
        command,
        cwd=cwd,
        env=environment,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    if process.returncode != 0:
        stderr = process.stderr.decode("utf-8", errors="replace").strip()
        fail(f"git command failed ({' '.join(command)}): {stderr}")
    if binary:
        return process.stdout
    return process.stdout.decode("utf-8").strip()


def read_fresh_official_sources(scratch: Path, legacy: Dict[str, bytes]) -> Dict[str, bytes]:
    repository = scratch / "fresh-openfhe"
    repository.mkdir(parents=True)
    run_git(["git", "init", "--quiet"], cwd=repository)
    run_git(["git", "remote", "add", "origin", OPENFHE_REMOTE], cwd=repository)
    run_git(
        [
            "git",
            "-c",
            "protocol.version=2",
            "fetch",
            "--quiet",
            "--no-tags",
            "--depth=1",
            "--filter=blob:none",
            "origin",
            OPENFHE_PIN,
        ],
        cwd=repository,
    )
    fetched = run_git(["git", "rev-parse", "FETCH_HEAD^{commit}"], cwd=repository)
    if fetched != OPENFHE_PIN:
        fail(f"fresh official fetch resolved to {fetched}, expected {OPENFHE_PIN}")

    result: Dict[str, bytes] = {}
    for source in SOURCES:
        data = run_git(
            ["git", "show", f"{OPENFHE_PIN}:{source.canonical_path}"],
            cwd=repository,
            binary=True,
        )
        assert isinstance(data, bytes)
        if (
            len(data) != source.bytes
            or sha256(data) != source.sha256
            or git_blob_sha1(data) != source.git_blob_sha1
        ):
            fail(f"fresh official source identity mismatch: {source.canonical_path}")
        if data != legacy[source.legacy_path]:
            fail(f"fresh/legacy byte inequality: {source.canonical_path}")
        result[source.corrected_path] = data
    return result


def write_regular(path: Path, data: bytes) -> None:
    if path.exists():
        fail(f"refusing to overwrite staging path: {path}")
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)
    path.chmod(0o644)


def staging_files(root: Path) -> List[Path]:
    files: List[Path] = []
    for path in sorted(root.rglob("*")):
        if path.is_symlink():
            fail(f"staging symlink is forbidden: {path}")
        if path.is_file():
            if stat.S_IMODE(path.stat().st_mode) != 0o644:
                fail(f"staging file mode is not 0644: {path}")
            files.append(path)
    return files


def create_manifest(root: Path) -> Tuple[bytes, bytes, int]:
    rows = []
    for path in staging_files(root):
        relative = path.relative_to(root).as_posix()
        if relative in {ORIGINAL_MANIFEST_PATH, ORIGINAL_MANIFEST_SIDECAR_PATH}:
            continue
        validate_relative_path(relative)
        data = path.read_bytes()
        rows.append((relative, len(data), sha256(data)))
    rows.sort(key=lambda row: row[0])
    if len(rows) != 120:
        fail(f"corrected manifest payload count mismatch: {len(rows)}")
    manifest = "sha256\tbytes\tpath\n" + "".join(
        f"{digest}\t{size}\t{name}\n" for name, size, digest in rows
    )
    manifest_data = manifest.encode("utf-8")
    sidecar_data = f"{sha256(manifest_data)}  {ORIGINAL_MANIFEST_PATH}\n".encode("ascii")
    return manifest_data, sidecar_data, len(rows)


def add_zip_member(archive: zipfile.ZipFile, name: str, data: bytes) -> None:
    validate_relative_path(name, expected_root=ORIGINAL_ROOT)
    info = zipfile.ZipInfo(name, FIXED_ZIP_TIME)
    info.create_system = 3
    info.compress_type = zipfile.ZIP_STORED
    info.external_attr = (stat.S_IFREG | 0o644) << 16
    archive.writestr(info, data)


def build_zip(staging_root: Path, output_archive: Path) -> None:
    if output_archive.exists():
        fail(f"refusing to overwrite output archive: {output_archive}")
    with zipfile.ZipFile(output_archive, "w", compression=zipfile.ZIP_STORED, allowZip64=True) as archive:
        for path in staging_files(staging_root):
            relative = path.relative_to(staging_root).as_posix()
            add_zip_member(archive, f"{ORIGINAL_ROOT}/{relative}", path.read_bytes())


def verify_corrected_zip(output_archive: Path, staging_root: Path) -> None:
    expected_files = staging_files(staging_root)
    expected = {
        f"{ORIGINAL_ROOT}/{path.relative_to(staging_root).as_posix()}": path.read_bytes()
        for path in expected_files
    }
    with zipfile.ZipFile(output_archive, "r") as archive:
        infos = archive.infolist()
        names = [info.filename for info in infos]
        if len(infos) != 122 or len(set(names)) != len(names) or set(names) != set(expected):
            fail("corrected ZIP member/uniqueness/closure mismatch")
        for info in infos:
            validate_zip_info(info, expected_root=ORIGINAL_ROOT)
            if info.compress_type != zipfile.ZIP_STORED:
                fail(f"corrected ZIP is not deterministically stored: {info.filename}")
        if archive.testzip() is not None:
            fail("corrected ZIP CRC validation failed")
        for name, data in expected.items():
            if archive.read(name) != data:
                fail(f"corrected ZIP/staging byte mismatch: {name}")

        prefix = f"{ORIGINAL_ROOT}/"
        manifest_data = archive.read(prefix + ORIGINAL_MANIFEST_PATH)
        sidecar_data = archive.read(prefix + ORIGINAL_MANIFEST_SIDECAR_PATH)
        if sidecar_data != f"{sha256(manifest_data)}  {ORIGINAL_MANIFEST_PATH}\n".encode("ascii"):
            fail("corrected manifest sidecar mismatch")
        rows = parse_manifest(manifest_data, 120)
        manifest_names = {prefix + name for name, _, _ in rows} | {
            prefix + ORIGINAL_MANIFEST_PATH,
            prefix + ORIGINAL_MANIFEST_SIDECAR_PATH,
        }
        if set(names) != manifest_names:
            fail("corrected manifest does not close over ZIP members")
        for name, expected_size, expected_sha in rows:
            data = archive.read(prefix + name)
            if len(data) != expected_size or sha256(data) != expected_sha:
                fail(f"corrected manifest payload mismatch: {name}")


def verify_static_correction_documents(script_dir: Path) -> Dict[str, bytes]:
    documents: Dict[str, bytes] = {}
    for name in CORRECTION_DOCUMENTS:
        path = script_dir / name
        if not path.is_file() or path.is_symlink():
            fail(f"correction document is absent or unsafe: {path}")
        documents[name] = path.read_bytes()
    correction = json.loads(documents["SOURCE_CORRECTION_01.json"])
    if correction.get("unchangedAuthority", {}).get("openfheCommit") != OPENFHE_PIN:
        fail("correction JSON OpenFHE pin mismatch")
    additions = correction.get("additions", [])
    expected = [source.corrected_path for source in SOURCES]
    if [entry.get("correctedPacketPath") for entry in additions] != expected:
        fail("correction JSON source list/order mismatch")
    return documents


def prepare_output_directory(path: Path) -> None:
    path.mkdir(parents=True, exist_ok=True)
    if any(path.iterdir()):
        fail(f"output directory must be empty: {path}")


def build(args: argparse.Namespace) -> None:
    output_dir = args.output_dir.resolve()
    original_archive = args.original_zip.resolve()
    design_archive = args.design_zip.resolve()
    script_dir = Path(__file__).resolve().parent
    prepare_output_directory(output_dir)

    payloads, original_manifest, original_manifest_sidecar = verify_original_archive(original_archive)
    legacy_sources = verify_design_archive(design_archive)
    documents = verify_static_correction_documents(script_dir)

    scratch = output_dir / "scratch"
    scratch.mkdir()
    official_sources = read_fresh_official_sources(scratch, legacy_sources)

    staging_root = output_dir / "staging" / ORIGINAL_ROOT
    staging_root.mkdir(parents=True)
    for relative, data in payloads.items():
        write_regular(staging_root / relative, data)
    write_regular(
        staging_root / ORIGINAL_PROVENANCE_PREFIX / ORIGINAL_MANIFEST_PATH,
        original_manifest,
    )
    write_regular(
        staging_root / ORIGINAL_PROVENANCE_PREFIX / ORIGINAL_MANIFEST_SIDECAR_PATH,
        original_manifest_sidecar,
    )
    for relative, data in documents.items():
        write_regular(staging_root / relative, data)
    for relative, data in official_sources.items():
        write_regular(staging_root / relative, data)

    old_official = {name for name in payloads if name.startswith("official-full/")}
    new_official = {
        path.relative_to(staging_root).as_posix()
        for path in staging_files(staging_root)
        if path.relative_to(staging_root).as_posix().startswith("official-full/")
    }
    expected_additions = {source.corrected_path for source in SOURCES}
    if new_official - old_official != expected_additions or len(new_official) != 64:
        fail("corrected official source closure is not exactly 59 + 5")
    if EXPLICITLY_NOT_ADDED & new_official:
        fail("an explicitly out-of-scope legacy source was added")
    for relative, original_data in payloads.items():
        if (staging_root / relative).read_bytes() != original_data:
            fail(f"original payload was changed in staging: {relative}")

    manifest_data, manifest_sidecar_data, manifest_records = create_manifest(staging_root)
    write_regular(staging_root / ORIGINAL_MANIFEST_PATH, manifest_data)
    write_regular(staging_root / ORIGINAL_MANIFEST_SIDECAR_PATH, manifest_sidecar_data)

    output_archive = output_dir / OUTPUT_ARCHIVE_NAME
    build_zip(staging_root, output_archive)
    verify_corrected_zip(output_archive, staging_root)

    output_sha = sha256_file(output_archive)
    outer_sidecar = output_dir / f"{OUTPUT_ARCHIVE_NAME}.sha256"
    outer_sidecar.write_text(f"{output_sha}  {OUTPUT_ARCHIVE_NAME}\n", encoding="ascii")
    outer_sidecar.chmod(0o644)

    build_record = {
        "schemaVersion": 1,
        "status": "source-correction-package-built-static-only",
        "outputArchive": {
            "path": str(output_archive),
            "bytes": output_archive.stat().st_size,
            "sha256": output_sha,
            "sidecarPath": str(outer_sidecar),
        },
        "stagingRoot": str(staging_root),
        "manifest": {
            "path": str(staging_root / ORIGINAL_MANIFEST_PATH),
            "records": manifest_records,
            "bytes": len(manifest_data),
            "sha256": sha256(manifest_data),
            "sidecarPath": str(staging_root / ORIGINAL_MANIFEST_SIDECAR_PATH),
        },
        "originalInput": {
            "path": str(original_archive),
            "bytes": ORIGINAL_ARCHIVE_BYTES,
            "sha256": ORIGINAL_ARCHIVE_SHA256,
            "preservedPayloads": ORIGINAL_PAYLOADS,
            "manifestCopyPath": str(
                staging_root / ORIGINAL_PROVENANCE_PREFIX / ORIGINAL_MANIFEST_PATH
            ),
            "manifestSidecarCopyPath": str(
                staging_root / ORIGINAL_PROVENANCE_PREFIX / ORIGINAL_MANIFEST_SIDECAR_PATH
            ),
        },
        "officialSourceSelection": {
            "remote": OPENFHE_REMOTE,
            "commit": OPENFHE_PIN,
            "readMethod": "fresh repository git show",
            "legacyDesignArchivePath": str(design_archive),
            "legacyDesignArchiveSha256": DESIGN_ARCHIVE_SHA256,
            "correctedOfficialFullCount": 64,
            "additions": [
                {
                    "legacyPath": source.legacy_path,
                    "canonicalPath": source.canonical_path,
                    "packetPath": source.corrected_path,
                    "bytes": source.bytes,
                    "sha256": source.sha256,
                    "gitBlobSha1": source.git_blob_sha1,
                    "freshEqualsLegacy": True,
                }
                for source in SOURCES
            ],
        },
        "checks": [
            "original archive exact size/SHA/CRC/path/mode/member closure",
            "all original 111 payload bytes preserved",
            "original manifest and sidecar preserved under provenance/original-input",
            "fresh official fetch resolved exactly to the pinned commit",
            "each git-show source matches expected size/SHA/Git blob and legacy archive bytes",
            "exactly five canonical official-full additions and no other legacy-only sources",
            "new manifest, one-root paths, regular 0644 modes, ZIP CRC and staging equality",
        ],
        "notRun": [
            "build",
            "cryptographic execution",
            "CTest",
            "benchmark",
            "CI",
            "browser/external agent",
            "upload",
            "commit/push",
            "gitleaks (reserved for independent root review)",
        ],
    }
    build_record_path = output_dir / OUTPUT_BUILD_RECORD_NAME
    build_record_path.write_text(
        json.dumps(build_record, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    build_record_path.chmod(0o644)
    print(f"archive={output_archive}")
    print(f"archive_bytes={output_archive.stat().st_size}")
    print(f"archive_sha256={output_sha}")
    print(f"sidecar={outer_sidecar}")
    print(f"staging={staging_root}")
    print(f"manifest={staging_root / ORIGINAL_MANIFEST_PATH}")
    print(f"manifest_records={manifest_records}")
    print(f"build_record={build_record_path}")


def parse_args(argv: Iterable[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path("/private/tmp/io-source-correction-20260905.oeqUa5/build-01"),
    )
    parser.add_argument(
        "--original-zip",
        type=Path,
        default=Path("/private/tmp/lossless-client-io-implementation-01-4ccc8fd.zip"),
    )
    parser.add_argument(
        "--design-zip",
        type=Path,
        default=Path(
            "/private/tmp/lossless-io-pro-handoff.AXGaam/"
            "lossless-client-io-design-b64a980.zip"
        ),
    )
    return parser.parse_args(list(argv))


if __name__ == "__main__":
    build(parse_args(sys.argv[1:]))
