#!/usr/bin/env python3
"""Build the fixed-source FS residual endpoint GREEN Pro input packet.

This script performs only bounded Git-object, hashing, ZIP and Gitleaks work.
It never executes project code or a nested/returned checker.  It creates the
final ZIP by an exclusive hard link only after all decoded-member checks pass.
"""

import argparse
import hashlib
import io
import json
import os
from pathlib import Path, PurePosixPath
import stat
import subprocess
import tempfile
import zipfile


ROOT = Path("/Users/lifeng/Documents/20231788-openfhe-paper-scale-implementation-20260905")
BASE = ROOT / "artifacts/handoffs/fs-residual-endpoint-green-01"
BRANCH = "codex/paper-scale-implementation-20260905"
DOCUMENTATION_HEAD = "29e12670150f083be396686f0f4b92136758956f"
TESTED_RED_SOURCE = "2fe655d493dcde5f05aa1515f41ca6823bba30bd"
PRODUCTION_SOURCE = "b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89"
OFFICIAL_PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
OFFICIAL_REPO = Path("/private/tmp/h128-pro-packet.Lo406g/official-source")
PRIOR_PACKET = ROOT / (
    "artifacts/handoffs/paper-scale-precision-adjudication-01/"
    "paper-scale-precision-adjudication-9f6c8eae.zip"
)
PRIOR_PACKET_BYTES = 2046500
PRIOR_PACKET_SHA256 = "1584a5b7362c9568d3f8f7fa8acfea9f28a4a934cabe2dcdd283aa6f02e9b7da"
PRIOR_MANIFEST_SHA256 = "d19303d2d6fd5364f14c3076ed2d3f306a6e90da4d11d257deab0e6b1e7e2c98"
GITLEAKS = Path("/opt/homebrew/bin/gitleaks")
GITLEAKS_VERSION = "8.30.1"
DEFAULT_OUTPUT = BASE / "fs-residual-endpoint-green-2fe655d4.zip"

REQUIREMENTS = [
    "coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md",
    "coordination/TEST_SEAMS.md",
    "coordination/paper-scale-integration-01/INPUT_DOMAIN_AUDIT_01.md",
    "coordination/paper-scale-integration-01/NOMINAL_SCALE_AUDIT_01.md",
    "coordination/paper-scale-integration-01/PRODUCTION_CONTRACT_01.md",
]

ENDPOINT_CONTEXT = [
    "coordination/fs-residual-endpoint-red-return-01/pro/ENDPOINT_SPEC.md",
    "coordination/fs-residual-endpoint-red-return-01/pro/SOURCE_MAP.md",
    "coordination/fs-residual-endpoint-red-return-01/pro/SOURCE_REFERENCES.json",
    "coordination/fs-residual-endpoint-red-return-01/pro/TEST_PLAN.md",
    "coordination/fs-residual-endpoint-red-return-01/ROOT_DECISION.md",
    "coordination/fs-residual-endpoint-red-return-01/ROOT_INTEGRATION_VERIFICATION.json",
    "coordination/fs-residual-endpoint-red-return-01/ROOT_SOURCE_BINDING.json",
    "coordination/fs-residual-endpoint-red-return-01/SPEC_REVIEW.md",
    "coordination/fs-residual-endpoint-red-return-01/STANDARDS_REVIEW.md",
]

RED_EVIDENCE = [
    "coordination/fs-residual-endpoint-red-run-01/ACCEPTANCE.md",
    "coordination/fs-residual-endpoint-red-run-01/LINUX_RAW.log",
    "coordination/fs-residual-endpoint-red-run-01/OLD_CHECKPOINT_AUDIT.md",
    "coordination/fs-residual-endpoint-red-run-01/OLD_CHECKPOINT_VERIFICATION.json",
    "coordination/fs-residual-endpoint-red-run-01/PREFINAL_SELECTION_SCAN.json",
    "coordination/fs-residual-endpoint-red-run-01/RAW_PREFLIGHT.json",
    "coordination/fs-residual-endpoint-red-run-01/ROOT_LINK_AUDIT.md",
    "coordination/fs-residual-endpoint-red-run-01/ROOT_LINK_VERIFICATION.json",
    "coordination/fs-residual-endpoint-red-run-01/ROOT_OLD_REEXECUTION.json",
    "coordination/fs-residual-endpoint-red-run-01/RUN_TERMINAL.json",
    "coordination/fs-residual-endpoint-red-run-01/WINDOWS_LF.log",
    "coordination/fs-residual-endpoint-red-run-01/verify_link_red.py",
    "coordination/fs-residual-endpoint-red-run-01/verify_old_checkpoint.py",
]

SCIENTIFIC_DISPOSITION = [
    "coordination/paper-scale-precision-adjudication-return-01/pro/DECISION.md",
    "coordination/paper-scale-precision-adjudication-return-01/pro/NEXT_TEST_SPEC.md",
    "coordination/paper-scale-precision-adjudication-return-01/pro/SOURCE_MAP.md",
    "coordination/paper-scale-precision-adjudication-return-01/pro/ACCEPTANCE_ADJUDICATION_DRAFT.md",
    "coordination/paper-scale-precision-adjudication-return-01/ROOT_DECISION.md",
    "coordination/paper-scale-precision-adjudication-return-01/ASTRA_ADJUDICATION_REVIEW.md",
    "coordination/paper-scale-precision-adjudication-return-01/SOL_ADJUDICATION_REVIEW.md",
    "coordination/paper-scale-precision-adjudication-return-01/OBSERVER_ALLOWANCE_PROPOSAL.md",
    "coordination/paper-scale-precision-adjudication-return-01/ENDPOINT_EVIDENCE_PROPOSAL.md",
]

HISTORICAL_SIGNED_EVIDENCE = [
    "evidence/signed-diagnostic-run/ACCEPTANCE.md",
    "evidence/signed-diagnostic-run/LINUX_AUDIT.md",
    "evidence/signed-diagnostic-run/LINUX_RAW.log",
    "evidence/signed-diagnostic-run/LINUX_SIGNED_ERROR.json",
    "evidence/signed-diagnostic-run/LINUX_VERIFICATION.json",
    "evidence/signed-diagnostic-run/RAW_PREFLIGHT.json",
    "evidence/signed-diagnostic-run/ROOT_REEXECUTION.json",
    "evidence/signed-diagnostic-run/RUN_TERMINAL_01.json",
    "evidence/signed-diagnostic-run/SIGNED_ERROR_AUDIT.md",
    "evidence/signed-diagnostic-run/WINDOWS_AUDIT.md",
    "evidence/signed-diagnostic-run/WINDOWS_LF.log",
    "evidence/signed-diagnostic-run/WINDOWS_SIGNED_ERROR.json",
    "evidence/signed-diagnostic-run/WINDOWS_VERIFICATION.json",
    "evidence/signed-diagnostic-run/verify_signed_error.py",
    "evidence/signed-diagnostic-run/verify_signed_run.py",
]

BOOST_PATHS = [
    "boost-1.83.0/include/boost/multiprecision/cpp_bin_float.hpp",
    "boost-1.83.0/include/boost/multiprecision/cpp_int/bitwise.hpp",
    "boost-1.83.0/include/boost/multiprecision/detail/functions/trig.hpp",
    "boost-1.83.0/include/boost/multiprecision/fwd.hpp",
]

PAPER_PATHS = ["paper/PAPER-2023-1788.pdf", "paper/PAPER-2023-1788.txt"]
PAPER_ORIGINALS = {
    "paper/PAPER-2023-1788.pdf": {
        "path": Path("/Users/lifeng/.zcode/workspace/default/2023.1788.pdf"),
        "bytes": 759375,
        "sha256": "61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac",
    },
    "paper/PAPER-2023-1788.txt": {
        "path": Path("/Users/lifeng/.zcode/workspace/default/2023.1788.txt"),
        "bytes": 90235,
        "sha256": "60dd871a2769fddfe7ce7b2562d031d7c8d819a679eff3c2b6ebf3d7ea5769ae",
    },
}

FORBIDDEN_PARTS = {
    ".git", ".env", ".ssh", ".aws", ".codex", "node_modules", "__pycache__",
    "browser-state", "browser_state", "cookies", "sessions", "credentials",
}
FORBIDDEN_BASENAMES = {
    ".env", "credentials.json", "cookies.json", "cookies.sqlite", "login data",
    "token.json", "auth.json", "id_rsa", "id_ed25519",
}
FORBIDDEN_SUFFIXES = {".pem", ".key", ".p12", ".pfx", ".kdbx", ".jks"}


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def git(repo, *args):
    return subprocess.check_output(["git", "-C", str(repo), *args])


def git_show(repo, commit, path):
    return git(repo, "show", f"{commit}:{path}")


def safe_name(name, credential_policy):
    path = PurePosixPath(name)
    require(name and path.as_posix() == name and not path.is_absolute(), f"unsafe path: {name!r}")
    require(".." not in path.parts and "\\" not in name and ":" not in name,
            f"unsafe path syntax: {name!r}")
    require(not name.endswith("/"), f"directory member forbidden: {name!r}")
    if credential_policy:
        folded_parts = {part.casefold() for part in path.parts}
        basename = path.name.casefold()
        require(not (folded_parts & FORBIDDEN_PARTS), f"forbidden state/credential path: {name}")
        require(basename not in FORBIDDEN_BASENAMES, f"forbidden credential filename: {name}")
        require(path.suffix.casefold() not in FORBIDDEN_SUFFIXES, f"forbidden credential suffix: {name}")
        require(path.suffix.casefold() != ".zip", f"nested archive forbidden: {name}")


def validate_name_set(names, credential_policy):
    require(len(names) == len(set(names)), "duplicate member path")
    require(len(names) == len({name.casefold() for name in names}), "case-colliding member path")
    for name in names:
        safe_name(name, credential_policy)


def decode_manifest_archive(raw):
    with zipfile.ZipFile(io.BytesIO(raw)) as archive:
        require(archive.testzip() is None, "prior packet CRC failure")
        infos = archive.infolist()
        names = [info.filename for info in infos]
        validate_name_set(names, credential_policy=False)
        for info in infos:
            require(not (info.flag_bits & 1), f"encrypted prior member: {info.filename}")
            require(stat.S_ISREG(info.external_attr >> 16), f"non-regular prior member: {info.filename}")
        decoded = {name: archive.read(name) for name in names}
    require("MANIFEST.json" in decoded, "prior packet manifest missing")
    require(sha256(decoded["MANIFEST.json"]) == PRIOR_MANIFEST_SHA256,
            "prior packet manifest identity changed")
    manifest = json.loads(decoded["MANIFEST.json"].decode("utf-8"))
    require(manifest.get("manifest_self_excluded") is True, "prior manifest is not self-excluding")
    rows = manifest.get("files")
    require(isinstance(rows, list) and len(rows) == len(decoded) - 1,
            "prior manifest count/shape mismatch")
    require({row.get("path") for row in rows} == set(decoded) - {"MANIFEST.json"},
            "prior manifest payload closure mismatch")
    row_map = {}
    for row in rows:
        path = row["path"]
        require(path not in row_map and "origin" in row, f"bad prior manifest row: {path}")
        blob = decoded[path]
        require(row["bytes"] == len(blob) and row["sha256"] == sha256(blob),
                f"prior manifest byte/hash mismatch: {path}")
        row_map[path] = row
    return decoded, row_map


def framed(payloads):
    blocks = []
    for name, item in sorted(payloads.items()):
        blob = item["bytes"]
        blocks.append(b"FILE " + str(len(name.encode("utf-8"))).encode() + b" "
                      + str(len(blob)).encode() + b"\n" + name.encode("utf-8") + b"\n"
                      + blob + b"\nEND_FILE\n")
    return b"".join(blocks)


def scan(payloads, phase):
    scan_bytes = framed(payloads)
    env = os.environ.copy()
    env.pop("GITLEAKS_CONFIG", None)
    env.pop("GITLEAKS_CONFIG_TOML", None)
    env["GOMAXPROCS"] = "2"
    version_command = [str(GITLEAKS), "version"]
    version = subprocess.check_output(version_command, cwd="/private/tmp", env=env).decode().strip()
    require(version == GITLEAKS_VERSION, f"unexpected Gitleaks version: {version!r}")
    command = [
        str(GITLEAKS), "stdin", "--ignore-gitleaks-allow",
        "--gitleaks-ignore-path", "/dev/null", "--max-decode-depth", "5",
        "--max-archive-depth", "1", "--redact", "--no-banner", "--no-color",
        "--report-format", "json", "--report-path", "-",
    ]
    completed = subprocess.run(command, input=scan_bytes, capture_output=True,
                               cwd="/private/tmp", env=env)
    require(completed.returncode == 0,
            f"{phase} Gitleaks exit {completed.returncode}: {completed.stdout.decode('utf-8', 'replace')}")
    findings = json.loads(completed.stdout.decode("utf-8"))
    require(findings == [], f"{phase} Gitleaks findings: {findings}")
    return {
        "phase": phase,
        "version_command": version_command,
        "command": command,
        "tool_version": version,
        "exit_code": completed.returncode,
        "findings": findings,
        "framed_bytes": len(scan_bytes),
        "framed_sha256": sha256(scan_bytes),
    }


def add(payloads, name, blob, origin):
    safe_name(name, credential_policy=True)
    require(name not in payloads, f"duplicate selected output path: {name}")
    payloads[name] = {"bytes": blob, "origin": origin}


def add_git_context(payloads, source_path, output_prefix):
    blob = git_show(ROOT, DOCUMENTATION_HEAD, source_path)
    add(payloads, output_prefix + source_path, blob, {
        "kind": "current_cleanroom_context",
        "commit": DOCUMENTATION_HEAD,
        "path": source_path,
        "git_blob": git(ROOT, "rev-parse", f"{DOCUMENTATION_HEAD}:{source_path}").decode().strip(),
    })


def verify_decoded_final(path, expected):
    with zipfile.ZipFile(path) as archive:
        require(archive.testzip() is None, "final ZIP CRC failure")
        infos = archive.infolist()
        names = [info.filename for info in infos]
        validate_name_set(names, credential_policy=True)
        for info in infos:
            require(not (info.flag_bits & 1), f"encrypted final member: {info.filename}")
            require(stat.S_ISREG(info.external_attr >> 16), f"non-regular final member: {info.filename}")
        decoded = {name: archive.read(name) for name in names}
    require(decoded == expected, "decoded final archive bytes differ from selection")
    manifest = json.loads(decoded["MANIFEST.json"].decode("utf-8"))
    require(manifest.get("manifest_self_excluded") is True, "final manifest is not self-excluding")
    rows = manifest["files"]
    require({row["path"] for row in rows} == set(decoded) - {"MANIFEST.json"},
            "final manifest payload closure mismatch")
    require(len(rows) == len(decoded) - 1, "final manifest row count mismatch")
    for row in rows:
        blob = decoded[row["path"]]
        require(row["bytes"] == len(blob) and row["sha256"] == sha256(blob)
                and "origin" in row, f"final manifest row mismatch: {row['path']}")
    return decoded


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--task", required=True, type=Path,
                        help="root-authored, final GREEN task brief")
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    args = parser.parse_args()

    output = args.output.resolve()
    require(output.parent == BASE.resolve(), "output must remain in the authorized ignored directory")
    require(not output.exists(), f"refusing to overwrite output: {output}")
    require(git(ROOT, "rev-parse", "HEAD").decode().strip() == DOCUMENTATION_HEAD,
            "documentation HEAD changed")
    require(git(ROOT, "branch", "--show-current").decode().strip() == BRANCH,
            "branch changed")
    require(git(ROOT, "status", "--porcelain=v1") == b"", "repository is not clean")
    require(git(ROOT, "cat-file", "-t", TESTED_RED_SOURCE).strip() == b"commit",
            "tested RED source commit missing")
    require(git(ROOT, "cat-file", "-t", PRODUCTION_SOURCE).strip() == b"commit",
            "production source commit missing")
    require(git(ROOT, "diff", "--name-only", PRODUCTION_SOURCE, DOCUMENTATION_HEAD,
                "--", "src", "include") == b"", "production src/include changed after production source")

    task_path = args.task.resolve()
    require(task_path.is_file() and not task_path.is_symlink(), "TASK path is not a regular non-symlink file")
    task = task_path.read_bytes()
    require(0 < len(task) <= 131072 and b"\x00" not in task, "TASK size/content invalid")
    task_text = task.decode("utf-8")
    for marker in ("FS-RESIDUAL-ENDPOINT-01", "GREEN", DOCUMENTATION_HEAD,
                   TESTED_RED_SOURCE, PRODUCTION_SOURCE, OFFICIAL_PIN):
        require(marker in task_text, f"TASK missing fixed marker: {marker}")

    project_paths = sorted(
        git(ROOT, "ls-tree", "-r", "--name-only", DOCUMENTATION_HEAD,
            "--", "src", "include", "tests").decode().splitlines()
        + ["CMakeLists.txt", ".github/workflows/dcp-rcb.yml"]
    )
    red_project_paths = sorted(
        git(ROOT, "ls-tree", "-r", "--name-only", TESTED_RED_SOURCE,
            "--", "src", "include", "tests").decode().splitlines()
        + ["CMakeLists.txt", ".github/workflows/dcp-rcb.yml"]
    )
    require(len(project_paths) == 39 and project_paths == red_project_paths,
            "current/tested RED project path inventory is not the fixed 39-file set")

    payloads = {}
    add(payloads, "TASK.md", task, {
        "kind": "root_authored_task",
        "source_path": str(task_path),
        "source_sha256": sha256(task),
    })
    for source_path in project_paths:
        current_blob = git_show(ROOT, DOCUMENTATION_HEAD, source_path)
        red_blob = git_show(ROOT, TESTED_RED_SOURCE, source_path)
        current_oid = git(ROOT, "rev-parse", f"{DOCUMENTATION_HEAD}:{source_path}").decode().strip()
        red_oid = git(ROOT, "rev-parse", f"{TESTED_RED_SOURCE}:{source_path}").decode().strip()
        require(current_blob == red_blob and current_oid == red_oid,
                f"project path differs between documentation HEAD and tested RED: {source_path}")
        add(payloads, "project/" + source_path, current_blob, {
            "kind": "cleanroom_git_equal_at_documentation_and_tested_red",
            "path": source_path,
            "documentation_head": DOCUMENTATION_HEAD,
            "documentation_blob": current_oid,
            "tested_red_source": TESTED_RED_SOURCE,
            "tested_red_blob": red_oid,
        })

    for source_path in REQUIREMENTS:
        current = git_show(ROOT, DOCUMENTATION_HEAD, source_path)
        require(current == git_show(ROOT, TESTED_RED_SOURCE, source_path),
                f"frozen requirement differs after tested RED: {source_path}")
        add(payloads, "requirements/" + source_path.removeprefix("coordination/"), current, {
            "kind": "frozen_requirement_git",
            "path": source_path,
            "documentation_head": DOCUMENTATION_HEAD,
            "tested_red_source": TESTED_RED_SOURCE,
            "git_blob": git(ROOT, "rev-parse", f"{DOCUMENTATION_HEAD}:{source_path}").decode().strip(),
        })

    for source_path in ENDPOINT_CONTEXT:
        add_git_context(payloads, source_path, "context/endpoint-red-return/")
    for source_path in RED_EVIDENCE:
        add_git_context(payloads, source_path, "evidence/current-endpoint-red-run/")
    for source_path in SCIENTIFIC_DISPOSITION:
        add_git_context(payloads, source_path, "context/scientific-disposition/")

    prior_raw = PRIOR_PACKET.read_bytes()
    require(len(prior_raw) == PRIOR_PACKET_BYTES and sha256(prior_raw) == PRIOR_PACKET_SHA256,
            "verified prior packet outer identity changed")
    prior, prior_rows = decode_manifest_archive(prior_raw)

    official_paths = sorted(name for name in prior_rows if name.startswith("official-full/"))
    require(len(official_paths) == 77, f"prior official reference count changed: {len(official_paths)}")
    require(git(OFFICIAL_REPO, "rev-parse", "HEAD").decode().strip() == OFFICIAL_PIN,
            "approved official repository HEAD changed")
    require(git(OFFICIAL_REPO, "status", "--porcelain=v1") == b"",
            "approved official repository is dirty")
    for archive_path in official_paths:
        source_path = archive_path.removeprefix("official-full/")
        listing = git(OFFICIAL_REPO, "ls-tree", OFFICIAL_PIN, "--", source_path).decode().splitlines()
        require(len(listing) == 1, f"official pin path absent/ambiguous: {source_path}")
        metadata, listed_path = listing[0].split("\t", 1)
        mode, object_type, oid = metadata.split()
        require(object_type == "blob" and listed_path == source_path,
                f"official ls-tree mismatch: {source_path}")
        official_blob = git_show(OFFICIAL_REPO, OFFICIAL_PIN, source_path)
        require(prior[archive_path] == official_blob, f"prior official bytes mismatch Git pin: {source_path}")
        prior_origin = prior_rows[archive_path]["origin"]
        add(payloads, "references/" + archive_path, official_blob, {
            "kind": "official_git_verified_in_prior_packet",
            "commit": OFFICIAL_PIN,
            "path": source_path,
            "mode": mode,
            "git_blob": oid,
            "prior_packet": {"bytes": PRIOR_PACKET_BYTES, "sha256": PRIOR_PACKET_SHA256},
            "prior_manifest_origin": prior_origin,
        })

    require(sorted(name for name in prior_rows if name.startswith("boost-1.83.0/")) == BOOST_PATHS,
            "prior Boost reference path set changed")
    for archive_path in BOOST_PATHS:
        row = prior_rows[archive_path]
        inherited = row["origin"]
        prior_origin = inherited.get("prior_origin", {})
        require(prior_origin.get("kind") == "official_public_source"
                and prior_origin.get("tag") == "boost-1.83.0"
                and prior_origin.get("url", "").startswith("https://raw.githubusercontent.com/boostorg/"),
                f"Boost origin/version not preserved: {archive_path}")
        add(payloads, "references/" + archive_path, prior[archive_path], {
            "kind": "verified_inherited_boost_reference",
            "version": "1.83.0",
            "prior_packet": {"bytes": PRIOR_PACKET_BYTES, "sha256": PRIOR_PACKET_SHA256},
            "prior_manifest_row": row,
        })

    for archive_path in PAPER_PATHS:
        cfg = PAPER_ORIGINALS[archive_path]
        original = cfg["path"].read_bytes()
        require(len(original) == cfg["bytes"] and sha256(original) == cfg["sha256"],
                f"user paper original identity changed: {cfg['path']}")
        require(prior[archive_path] == original, f"prior paper bytes differ from user original: {archive_path}")
        add(payloads, "references/" + archive_path, original, {
            "kind": "user_paper_original_verified_in_prior_packet",
            "source_path": str(cfg["path"]),
            "prior_packet": {"bytes": PRIOR_PACKET_BYTES, "sha256": PRIOR_PACKET_SHA256},
            "prior_manifest_row": prior_rows[archive_path],
        })

    prior_signed = sorted(name for name in prior_rows if name.startswith("evidence/signed-diagnostic-run/"))
    require(prior_signed == HISTORICAL_SIGNED_EVIDENCE,
            "prior signed-diagnostic evidence path set changed")
    for archive_path in HISTORICAL_SIGNED_EVIDENCE:
        add(payloads, "evidence/historical-original-e80/" + archive_path.removeprefix("evidence/signed-diagnostic-run/"),
            prior[archive_path], {
                "kind": "historical_original_e80_evidence_preserved_byte_for_byte",
                "prior_packet": {"bytes": PRIOR_PACKET_BYTES, "sha256": PRIOR_PACKET_SHA256},
                "prior_manifest_row": prior_rows[archive_path],
            })

    expected_payload_count = 174
    require(len(payloads) == expected_payload_count,
            f"selection count changed: {len(payloads)} != {expected_payload_count}")
    validate_name_set(list(payloads), credential_policy=True)
    selection_scan = scan(payloads, "selected decoded payloads before manifest")

    manifest = {
        "schema": "fs-residual-endpoint-green-input-v1",
        "manifest_self_excluded": True,
        "branch": BRANCH,
        "documentation_head": DOCUMENTATION_HEAD,
        "tested_red_source": TESTED_RED_SOURCE,
        "production_source": PRODUCTION_SOURCE,
        "official_pin": OFFICIAL_PIN,
        "source_tree_status": "clean",
        "project_file_count": len(project_paths),
        "project_paths_equal_at_documentation_and_tested_red": project_paths,
        "selection_counts": {
            "task": 1,
            "project": 39,
            "frozen_requirements": len(REQUIREMENTS),
            "endpoint_context": len(ENDPOINT_CONTEXT),
            "current_endpoint_red_evidence": len(RED_EVIDENCE),
            "scientific_disposition": len(SCIENTIFIC_DISPOSITION),
            "historical_original_e80_evidence": len(HISTORICAL_SIGNED_EVIDENCE),
            "official_git_references": len(official_paths),
            "boost_references": len(BOOST_PATHS),
            "paper_originals": len(PAPER_PATHS),
            "total_payloads": len(payloads),
        },
        "prior_reference_packet": {
            "path": str(PRIOR_PACKET),
            "bytes": PRIOR_PACKET_BYTES,
            "sha256": PRIOR_PACKET_SHA256,
            "manifest_sha256": PRIOR_MANIFEST_SHA256,
            "old_project_tree_included": False,
            "prior_archive_included": False,
        },
        "selection_scan": selection_scan,
        "files": [
            {
                "path": name,
                "bytes": len(item["bytes"]),
                "sha256": sha256(item["bytes"]),
                "origin": item["origin"],
            }
            for name, item in sorted(payloads.items())
        ],
    }
    manifest_bytes = (json.dumps(manifest, indent=2, sort_keys=True) + "\n").encode("utf-8")
    final_bytes = {name: item["bytes"] for name, item in payloads.items()}
    final_bytes["MANIFEST.json"] = manifest_bytes

    BASE.mkdir(parents=False, exist_ok=True)
    temporary = tempfile.NamedTemporaryFile(prefix=".green-packet-", suffix=".tmp", dir=BASE, delete=False)
    temporary_path = Path(temporary.name)
    temporary.close()
    try:
        with zipfile.ZipFile(temporary_path, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
            for name, blob in sorted(final_bytes.items()):
                info = zipfile.ZipInfo(name, date_time=(2026, 9, 6, 0, 0, 0))
                info.create_system = 3
                info.external_attr = (stat.S_IFREG | 0o644) << 16
                info.compress_type = zipfile.ZIP_DEFLATED
                archive.writestr(info, blob)
        decoded = verify_decoded_final(temporary_path, final_bytes)
        decoded_scan_payloads = {
            name: {"bytes": blob, "origin": "decoded-final"} for name, blob in decoded.items()
        }
        final_scan = scan(decoded_scan_payloads, "all decoded final archive members")
        require(git(ROOT, "status", "--porcelain=v1") == b"", "repository changed during packaging")
        archive_bytes = temporary_path.read_bytes()
        os.link(temporary_path, output)
    finally:
        if temporary_path.exists():
            temporary_path.unlink()

    receipt = {
        "archive_absolute_path": str(output),
        "archive_bytes": len(archive_bytes),
        "archive_sha256": sha256(archive_bytes),
        "regular_members": len(final_bytes),
        "payload_members": len(payloads),
        "manifest_sha256": sha256(manifest_bytes),
        "selection_scan": selection_scan,
        "decoded_final_archive_scan": final_scan,
        "manifest": manifest,
    }
    print(json.dumps(receipt, indent=2, sort_keys=True))


if __name__ == "__main__":
    main()
