#!/usr/bin/env python3
"""Build the bounded, exact-source EXPERIMENTAL-PRECISION116 Pro packet.

The script packages Git blobs from one fixed clean-room commit plus one
root-authored TASK.md and fixed workflow instructions.  Scientific references
are decoded from previously verified archives; neither prior ZIP is embedded.
It performs two fail-closed Gitleaks scans and only publishes the ZIP after
decoded-member, manifest, path, CRC, and exclusion checks pass.

This builder intentionally performs no build, test, crypto, numerical replay,
network access, upload, Git mutation, or browser operation.
"""

import hashlib
import io
import json
import os
from pathlib import Path, PurePosixPath
import re
import stat
import subprocess
import tempfile
import zipfile


ROOT = Path("/Users/lifeng/Documents/20231788-openfhe-precision116-seam-20260907")
EVIDENCE_ROOT = Path(
    "/Users/lifeng/Documents/20231788-openfhe-paper-scale-implementation-20260905"
)
HERE = ROOT / "coordination/precision116-pro-handoff-01"
TASK_PATH = "coordination/precision116-pro-handoff-01/TASK.md"
TASK_PREFLIGHT_PATH = "coordination/precision116-pro-handoff-01/TASK_PREFLIGHT.md"
SOURCE_COMMIT = "dbbbee0d20d8a7ae3c138e42f633414db621a173"
BRANCH = "codex/precision116-profile-seam-20260907"
OFFICIAL_PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
OUTPUT_DIR = ROOT / "artifacts/handoffs/precision116-pro-handoff-01"
OUTPUT = OUTPUT_DIR / "experimental-precision116-profile-seam-dbbbee0d.zip"

SCIENTIFIC_INPUT = EVIDENCE_ROOT / (
    "artifacts/handoffs/fs-endpoint-scientific-review-01/"
    "fs-endpoint-scientific-review-ed5fd192.zip"
)
SCIENTIFIC_INPUT_BYTES = 9_896_298
SCIENTIFIC_INPUT_SHA256 = (
    "ee98c075f62e23cf99f0a06ce694b49290f1f5dbe275932988954d41aa919d23"
)
SCIENTIFIC_INPUT_MANIFEST_SHA256 = (
    "bb8f9059770eb80b23056e899089f93d703e8605f888408fb27d687f6cf1ff47"
)

SCIENTIFIC_RETURN = EVIDENCE_ROOT / (
    "artifacts/handoffs/fs-endpoint-scientific-review-return-01/"
    "fs-endpoint-scientific-review-return-ed5fd192.zip"
)
SCIENTIFIC_RETURN_BYTES = 8_839_463
SCIENTIFIC_RETURN_SHA256 = (
    "ebd28c13a04746935b089b6af3dd6952c54361e45996b6074254594218940284"
)
SCIENTIFIC_RETURN_MANIFEST_SHA256 = (
    "f08aeed1469a181fc207c3efe0b2551d0a52a3b8aa87bbc799fd26ad1691c437"
)

WORKFLOW_ROOT = Path(
    "/Users/lifeng/Documents/2023.1788.pdf 协同实现 08312026/"
    ".agents/skills/openfhe-2023-1788-workflow"
)
WORKFLOW_INPUTS = {
    "SKILL.md": "7b3ab6fd4845534e5963f8728a96319a7afcfe1f7ca55d0d776d9016826f0ad6",
    "references/model-routing.md": "5859d907e05042dcaddeb1b6c06cc7295c3ae037816daeae4f0e1bba2fe98eed",
    "references/external-collaboration.md": "d16988a87b4d0e145b124cc9464e2309caa3bb9a5191495a54fc3e5c52ec4079",
    "references/engineering.md": "470fdc3e81bdccdd2a7cb27f8179d331fafab5ed099a7ef4a65f052077b23c38",
}

STATIC_PREFIX = "coordination/fs-precision-profile-feasibility-01/"
ROOT_REVIEW = [
    "coordination/fs-endpoint-scientific-review-return-01/ACCEPTANCE.md",
    "coordination/fs-endpoint-scientific-review-return-01/MATH_REVIEW.md",
    "coordination/fs-endpoint-scientific-review-return-01/ROOT_INTAKE.json",
    "coordination/fs-endpoint-scientific-review-return-01/WORKFLOW_REVIEW.md",
]
PRO_CORE = [
    "coordination/fs-endpoint-scientific-review-return-01/pro/DECISION.md",
    "coordination/fs-endpoint-scientific-review-return-01/pro/EXECUTION_LEDGER.md",
    "coordination/fs-endpoint-scientific-review-return-01/pro/FINDINGS.md",
    "coordination/fs-endpoint-scientific-review-return-01/pro/NEXT_STEP.md",
    "coordination/fs-endpoint-scientific-review-return-01/pro/PROPOSED_PRECISION_BOUNDARY_V1.json",
]
ACCEPTANCE_CONTEXT = [
    "coordination/fs-endpoint-live-run-01/ACCEPTANCE.md",
    "coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md",
]
MODEL_CONTEXT = ["coordination/MODEL_BENCHMARK_EVIDENCE_20260905.md"]
BOOST_PATHS = [
    "references/boost-1.83.0/include/boost/multiprecision/cpp_bin_float.hpp",
    "references/boost-1.83.0/include/boost/multiprecision/cpp_int/bitwise.hpp",
    "references/boost-1.83.0/include/boost/multiprecision/detail/functions/trig.hpp",
    "references/boost-1.83.0/include/boost/multiprecision/fwd.hpp",
]
PAPER_PATHS = [
    "references/paper/PAPER-2023-1788.pdf",
    "references/paper/PAPER-2023-1788.txt",
]
ALLOWED_UNTRACKED = {
    "coordination/precision116-pro-handoff-01/build_packet.py",
}
ALLOWED_DOCUMENTATION_CHANGES = {
    TASK_PATH,
    TASK_PREFLIGHT_PATH,
    "coordination/precision116-pro-handoff-01/build_packet.py",
}

GITLEAKS = Path("/opt/homebrew/bin/gitleaks")
GITLEAKS_VERSION = "8.30.1"
GITLEAKS_ENV = {
    "GOMAXPROCS": "2",
    "LANG": "C",
    "LC_ALL": "C",
    "PATH": "/usr/bin:/bin:/usr/sbin:/sbin",
}

FORBIDDEN_PARTS = {
    ".git", ".env", ".ssh", ".aws", ".codex", ".cache", "__pycache__",
    "node_modules", "cmakefiles", "browser-state", "browser_state", "cookies",
    "sessions", "credentials", "build", "_build", "dist", "venv", ".venv",
}
FORBIDDEN_BASENAMES = {
    ".env", "credentials.json", "cookies.json", "cookies.sqlite", "login data",
    "token.json", "auth.json", "id_rsa", "id_ed25519",
}
FORBIDDEN_SUFFIXES = {
    ".zip", ".7z", ".tar", ".tgz", ".pem", ".key", ".p12", ".pfx",
    ".kdbx", ".jks", ".sqlite", ".db",
}
ALLOWED_LOGS = {
    "project/tests/fixtures/endpoint_ctest_timeout_linux.log",
    "project/tests/fixtures/endpoint_ctest_timeout_windows.log",
}
SECRET_PATTERNS = {
    "private_key_block": re.compile(
        rb"-----BEGIN (?:RSA |EC |DSA |OPENSSH )?PRIVATE KEY-----"
    ),
    "aws_access_key": re.compile(rb"AKIA[0-9A-Z]{16}"),
    "github_token": re.compile(rb"(?:github_pat_|gh[pousr]_[A-Za-z0-9_]{20,})"),
    "slack_token": re.compile(rb"xox[baprs]-[A-Za-z0-9-]{20,}"),
}


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def sha256(blob):
    return hashlib.sha256(blob).hexdigest()


def git(*args):
    return subprocess.check_output(["git", "-C", str(ROOT), *args])


def git_show(commit, path):
    return git("show", f"{commit}:{path}")


def safe_name(name):
    path = PurePosixPath(name)
    require(name and path.as_posix() == name and not path.is_absolute(), f"unsafe path: {name!r}")
    require(".." not in path.parts and "\\" not in name and ":" not in name,
            f"unsafe path syntax: {name!r}")
    require(not name.endswith("/"), f"directory member forbidden: {name!r}")
    folded = {part.casefold() for part in path.parts}
    require(not (folded & FORBIDDEN_PARTS), f"forbidden state/build path: {name}")
    require(path.name.casefold() not in FORBIDDEN_BASENAMES,
            f"forbidden credential filename: {name}")
    require(path.suffix.casefold() not in FORBIDDEN_SUFFIXES,
            f"forbidden archive/credential/state suffix: {name}")
    require(path.suffix.casefold() != ".log" or name in ALLOWED_LOGS,
            f"non-fixture log excluded: {name}")


def validate_names(names):
    require(len(names) == len(set(names)), "duplicate member path")
    require(len(names) == len({name.casefold() for name in names}),
            "case-colliding member path")
    for name in names:
        safe_name(name)


def is_nested_zip(blob):
    return zipfile.is_zipfile(io.BytesIO(blob))


def targeted_content_scan(payloads, phase):
    checked_bytes = 0
    for name, item in sorted(payloads.items()):
        blob = item["bytes"]
        checked_bytes += len(blob)
        require(not is_nested_zip(blob), f"nested ZIP content forbidden: {name}")
        for label, pattern in SECRET_PATTERNS.items():
            require(pattern.search(blob) is None, f"{phase} targeted {label} finding: {name}")
    return {
        "phase": phase,
        "files": len(payloads),
        "bytes": checked_bytes,
        "patterns": sorted(SECRET_PATTERNS),
        "findings": [],
    }


def framed(payloads):
    blocks = []
    for name, item in sorted(payloads.items()):
        blob = item["bytes"]
        encoded_name = name.encode("utf-8")
        blocks.append(
            b"FILE " + str(len(encoded_name)).encode() + b" " + str(len(blob)).encode()
            + b"\n" + encoded_name + b"\n" + blob + b"\nEND_FILE\n"
        )
    return b"".join(blocks)


def gitleaks_scan(payloads, phase):
    require(GITLEAKS.is_file() and GITLEAKS.resolve().is_file(), "pinned Gitleaks binary unavailable")
    scan_bytes = framed(payloads)
    version_command = [str(GITLEAKS), "version"]
    version = subprocess.check_output(
        version_command, cwd="/private/tmp", env=GITLEAKS_ENV
    ).decode().strip()
    require(version == GITLEAKS_VERSION, f"unexpected Gitleaks version: {version!r}")
    command = [
        str(GITLEAKS), "stdin", "--ignore-gitleaks-allow",
        "--gitleaks-ignore-path", "/dev/null", "--max-decode-depth", "5",
        "--max-archive-depth", "1", "--redact", "--no-banner", "--no-color",
        "--report-format", "json", "--report-path", "-",
    ]
    completed = subprocess.run(
        command, input=scan_bytes, capture_output=True,
        cwd="/private/tmp", env=GITLEAKS_ENV, check=False,
    )
    require(
        completed.returncode == 0,
        f"{phase} Gitleaks exit {completed.returncode}: "
        f"{completed.stdout.decode('utf-8', 'replace')}",
    )
    findings = json.loads(completed.stdout.decode("utf-8"))
    require(findings == [], f"{phase} Gitleaks findings: {findings}")
    return {
        "phase": phase,
        "version_command": version_command,
        "command": command,
        "environment_keys": sorted(GITLEAKS_ENV),
        "tool_version": version,
        "exit_code": completed.returncode,
        "findings": findings,
        "framed_bytes": len(scan_bytes),
        "framed_sha256": sha256(scan_bytes),
    }


def add(payloads, name, blob, origin):
    safe_name(name)
    require(name not in payloads, f"duplicate selected output path: {name}")
    payloads[name] = {"bytes": blob, "origin": origin}


def git_entry(commit, path):
    lines = git("ls-tree", commit, "--", path).decode().splitlines()
    require(len(lines) == 1, f"Git path missing or ambiguous: {path}")
    metadata, listed_path = lines[0].split("\t", 1)
    mode, object_type, oid = metadata.split()
    require(object_type == "blob" and mode in {"100644", "100755"} and listed_path == path,
            f"non-regular or mismatched Git path: {path}")
    blob = git_show(commit, path)
    return blob, mode, oid


def add_git(payloads, path, output_path=None, kind="cleanroom_git_blob"):
    blob, mode, oid = git_entry(SOURCE_COMMIT, path)
    add(payloads, output_path or "project/" + path, blob, {
        "kind": kind,
        "commit": SOURCE_COMMIT,
        "path": path,
        "mode": mode,
        "git_blob": oid,
    })


def decode_manifest_archive(path, expected_bytes, expected_sha, expected_manifest_sha,
                            expected_members, self_field):
    require(path.is_file() and not path.is_symlink(), f"archive absent/non-regular: {path}")
    raw = path.read_bytes()
    require(len(raw) == expected_bytes and sha256(raw) == expected_sha,
            f"outer archive identity changed: {path}")
    with zipfile.ZipFile(io.BytesIO(raw)) as archive:
        require(archive.testzip() is None, f"archive CRC failure: {path}")
        infos = archive.infolist()
        require(len(infos) == expected_members, f"archive member count changed: {path}")
        names = [info.filename for info in infos]
        require(len(names) == len(set(names)) == len({name.casefold() for name in names}),
                f"archive duplicate/case collision: {path}")
        for info in infos:
            member = PurePosixPath(info.filename)
            require(
                info.filename and member.as_posix() == info.filename
                and not member.is_absolute() and ".." not in member.parts
                and "\\" not in info.filename and ":" not in info.filename,
                f"unsafe inherited member: {info.filename!r}",
            )
            require(not (info.flag_bits & 1), f"encrypted inherited member: {info.filename}")
            require(stat.S_ISREG(info.external_attr >> 16),
                    f"non-regular inherited member: {info.filename}")
        decoded = {name: archive.read(name) for name in names}
    require("MANIFEST.json" in decoded, f"archive manifest missing: {path}")
    require(sha256(decoded["MANIFEST.json"]) == expected_manifest_sha,
            f"archive manifest identity changed: {path}")
    manifest = json.loads(decoded["MANIFEST.json"].decode("utf-8"))
    require(manifest.get(self_field) is True, f"archive manifest is not self-excluding: {path}")
    rows = manifest.get("files")
    require(isinstance(rows, list) and len(rows) == len(decoded) - 1,
            f"archive manifest count/shape mismatch: {path}")
    require({row.get("path") for row in rows} == set(decoded) - {"MANIFEST.json"},
            f"archive manifest closure mismatch: {path}")
    row_map = {}
    for row in rows:
        member = row.get("path")
        require(member not in row_map and isinstance(row.get("origin"), dict),
                f"invalid archive manifest row: {member!r}")
        blob = decoded[member]
        require(row.get("bytes") == len(blob) and row.get("sha256") == sha256(blob),
                f"archive member byte/hash mismatch: {member}")
        row_map[member] = row
    return decoded, row_map


def verify_final_archive(path, expected):
    with zipfile.ZipFile(path) as archive:
        require(archive.testzip() is None, "final ZIP CRC failure")
        infos = archive.infolist()
        names = [info.filename for info in infos]
        validate_names(names)
        for info in infos:
            require(not (info.flag_bits & 1), f"encrypted final member: {info.filename}")
            require(stat.S_ISREG(info.external_attr >> 16),
                    f"non-regular final member: {info.filename}")
        decoded = {name: archive.read(name) for name in names}
    require(decoded == expected, "decoded final archive bytes differ from selected bytes")
    manifest = json.loads(decoded["MANIFEST.json"].decode("utf-8"))
    require(manifest.get("manifest_self_excluded") is True,
            "final manifest is not self-excluding")
    rows = manifest.get("files")
    require(isinstance(rows, list) and len(rows) == len(decoded) - 1,
            "final manifest count mismatch")
    require({row.get("path") for row in rows} == set(decoded) - {"MANIFEST.json"},
            "final manifest payload closure mismatch")
    for row in rows:
        blob = decoded[row["path"]]
        require(row.get("bytes") == len(blob) and row.get("sha256") == sha256(blob)
                and isinstance(row.get("origin"), dict),
                f"final manifest row mismatch: {row.get('path')}")
    return decoded


def status_snapshot():
    require(git("diff", "--quiet") == b"", "tracked unstaged changes are forbidden")
    require(git("diff", "--cached", "--quiet") == b"", "staged changes are forbidden")
    untracked = set(
        git("ls-files", "--others", "--exclude-standard", "-z")
        .decode().rstrip("\0").split("\0")
    )
    untracked.discard("")
    require(untracked <= ALLOWED_UNTRACKED,
            f"unrelated untracked files present: {sorted(untracked - ALLOWED_UNTRACKED)}")
    return {
        "tracked_index_and_worktree": "clean",
        "allowed_untracked_paths": sorted(untracked),
        "porcelain_v1": git("status", "--porcelain=v1", "--untracked-files=all").decode().splitlines(),
    }


def main():
    require(ROOT.resolve() == Path(__file__).resolve().parents[2], "builder is in the wrong worktree")
    require(git("branch", "--show-current").decode().strip() == BRANCH, "branch changed")
    require(git("cat-file", "-t", SOURCE_COMMIT).strip() == b"commit", "source commit missing")
    documentation_head = git("rev-parse", "HEAD").decode().strip()
    require(git("merge-base", "--is-ancestor", SOURCE_COMMIT, documentation_head) == b"",
            "packaging HEAD does not descend from the fixed engineering source")
    changed_since_source = set(
        git("diff", "--name-only", SOURCE_COMMIT, documentation_head).decode().splitlines()
    )
    require(changed_since_source <= ALLOWED_DOCUMENTATION_CHANGES,
            "packaging HEAD has changes outside the fixed handoff-document allowlist")
    require(TASK_PATH in changed_since_source and TASK_PREFLIGHT_PATH in changed_since_source,
            "packaging HEAD must commit TASK.md and TASK_PREFLIGHT.md")
    initial_status = status_snapshot()
    require(not OUTPUT.exists(), f"refusing to overwrite output: {OUTPUT}")

    task, task_mode, task_oid = git_entry(documentation_head, TASK_PATH)
    preflight, preflight_mode, preflight_oid = git_entry(
        documentation_head, TASK_PREFLIGHT_PATH
    )
    require(0 < len(task) <= 131_072 and b"\x00" not in task, "TASK.md size/content invalid")
    require(0 < len(preflight) <= 65_536 and b"\x00" not in preflight,
            "TASK_PREFLIGHT.md size/content invalid")
    task_text = task.decode("utf-8")
    preflight_text = preflight.decode("utf-8")
    for marker in (
        "EXPERIMENTAL-PRECISION116-PROFILE-SEAM-01",
        "CreateExperimentalPrecision116Setup",
        "experimental-s116-d56-b58-v1",
        SOURCE_COMMIT,
        BRANCH,
        OFFICIAL_PIN,
        "Current original paper-table profile still **fails**",
        "full_eight_square_E80=NOT_TESTED",
        "security=UNRESOLVED",
    ):
        require(marker in task_text, f"TASK.md missing fixed marker: {marker}")
    for marker in ("Independent brief preflight", SOURCE_COMMIT,
                   "No edits, compilation, cryptography"):
        require(marker in preflight_text, f"TASK_PREFLIGHT.md missing fixed marker: {marker}")

    project_paths = sorted(
        git("ls-tree", "-r", "--name-only", SOURCE_COMMIT, "--", "src", "include", "tests")
        .decode().splitlines()
        + ["CMakeLists.txt", ".github/workflows/dcp-rcb.yml"]
    )
    require(len(project_paths) == 84 and len(project_paths) == len(set(project_paths)),
            "current project source/build inventory is not the fixed 84-file set")
    static_paths = sorted(
        git("ls-tree", "-r", "--name-only", SOURCE_COMMIT, "--", STATIC_PREFIX)
        .decode().splitlines()
    )
    require(len(static_paths) == 12 and all(path.startswith(STATIC_PREFIX) for path in static_paths),
            "static profile inventory is not the fixed 12-file set")

    scientific, scientific_rows = decode_manifest_archive(
        SCIENTIFIC_INPUT, SCIENTIFIC_INPUT_BYTES, SCIENTIFIC_INPUT_SHA256,
        SCIENTIFIC_INPUT_MANIFEST_SHA256, 218, "manifest_self_excluded",
    )
    require(
        json.loads(scientific["MANIFEST.json"])["official_pin"] == OFFICIAL_PIN,
        "scientific input official pin changed",
    )
    official_paths = sorted(
        name for name in scientific_rows if name.startswith("references/official-full/")
    )
    require(len(official_paths) == 77, "scientific input official closure is not 77 files")
    require(sorted(name for name in scientific_rows if name.startswith("references/boost-1.83.0/"))
            == BOOST_PATHS, "scientific input Boost closure changed")
    require(sorted(name for name in scientific_rows if name.startswith("references/paper/"))
            == PAPER_PATHS, "scientific input paper set changed")

    returned, return_rows = decode_manifest_archive(
        SCIENTIFIC_RETURN, SCIENTIFIC_RETURN_BYTES, SCIENTIFIC_RETURN_SHA256,
        SCIENTIFIC_RETURN_MANIFEST_SHA256, 100, "self_excluded",
    )
    require(json.loads(returned["MANIFEST.json"])["input_archive_sha256"]
            == SCIENTIFIC_INPUT_SHA256, "scientific return is not bound to the selected input")

    payloads = {}
    add(payloads, "TASK.md", task, {
        "kind": "root_authored_task_git_blob",
        "commit": documentation_head,
        "path": TASK_PATH,
        "mode": task_mode,
        "git_blob": task_oid,
    })
    add(payloads, "TASK_PREFLIGHT.md", preflight, {
        "kind": "independent_task_preflight_git_blob",
        "commit": documentation_head,
        "path": TASK_PREFLIGHT_PATH,
        "mode": preflight_mode,
        "git_blob": preflight_oid,
    })
    for path in project_paths:
        add_git(payloads, path)
    for path in static_paths:
        add_git(payloads, path, "project/" + path, "accepted_static_profile_git_blob")
    for path in ROOT_REVIEW:
        add_git(payloads, path, "context/qualified-scientific-review/root/" + Path(path).name,
                "qualified_root_review_git_blob")
    for path in PRO_CORE:
        member = Path(path).name
        blob = git_show(SOURCE_COMMIT, path)
        require(returned[member] == blob,
                f"committed Pro core document differs from verified return: {member}")
        add_git(payloads, path, "context/qualified-scientific-review/pro/" + member,
                "pro_return_core_git_blob_equal_to_verified_return")
        payloads["context/qualified-scientific-review/pro/" + member]["origin"].update({
            "return_archive_sha256": SCIENTIFIC_RETURN_SHA256,
            "return_manifest_row": return_rows[member],
        })
    for path in ACCEPTANCE_CONTEXT:
        add_git(payloads, path, "requirements/" + path.removeprefix("coordination/"),
                "current_acceptance_requirement_git_blob")
    for path in MODEL_CONTEXT:
        add_git(payloads, path, "requirements/" + path.removeprefix("coordination/"),
                "current_model_routing_evidence_git_blob")

    for relative, expected_hash in WORKFLOW_INPUTS.items():
        source = WORKFLOW_ROOT / relative
        require(source.is_file() and not source.is_symlink(), f"workflow input absent/non-regular: {relative}")
        blob = source.read_bytes()
        require(sha256(blob) == expected_hash, f"workflow input identity changed: {relative}")
        add(payloads, "instructions/openfhe-2023-1788-workflow/" + relative, blob, {
            "kind": "fixed_local_project_workflow_instruction",
            "path": relative,
            "sha256": expected_hash,
        })

    for path in official_paths + BOOST_PATHS + PAPER_PATHS:
        add(payloads, path, scientific[path], {
            "kind": "verified_scientific_input_reference",
            "input_archive": {
                "bytes": SCIENTIFIC_INPUT_BYTES,
                "sha256": SCIENTIFIC_INPUT_SHA256,
                "manifest_sha256": SCIENTIFIC_INPUT_MANIFEST_SHA256,
            },
            "input_manifest_row": scientific_rows[path],
        })

    expected_payload_count = 197
    require(len(payloads) == expected_payload_count,
            f"selection count changed: {len(payloads)} != {expected_payload_count}")
    validate_names(list(payloads))
    targeted_selection = targeted_content_scan(payloads, "selected decoded payloads")
    selection_scan = gitleaks_scan(payloads, "selected decoded payloads before manifest")

    manifest = {
        "schema": "experimental-precision116-profile-seam-input-v1",
        "manifest_self_excluded": True,
        "branch": BRANCH,
        "source_commit": SOURCE_COMMIT,
        "packaging_documentation_head": documentation_head,
        "source_tree_status": initial_status,
        "official_pin": OFFICIAL_PIN,
        "no_prior_or_quarantined_project_implementation": True,
        "prior_archives_embedded": False,
        "large_live_logs_or_full_slot_captures_included": False,
        "selection_counts": {
            "root_authored_task": 1,
            "independent_task_preflight": 1,
            "project_source_test_build": len(project_paths),
            "static_profile": len(static_paths),
            "qualified_root_review": len(ROOT_REVIEW),
            "pro_return_core": len(PRO_CORE),
            "acceptance_context": len(ACCEPTANCE_CONTEXT),
            "model_context": len(MODEL_CONTEXT),
            "workflow_instructions": len(WORKFLOW_INPUTS),
            "official_openfhe": len(official_paths),
            "boost": len(BOOST_PATHS),
            "paper": len(PAPER_PATHS),
            "total_payloads": len(payloads),
        },
        "reference_archives": {
            "scientific_input": {
                "bytes": SCIENTIFIC_INPUT_BYTES,
                "sha256": SCIENTIFIC_INPUT_SHA256,
                "manifest_sha256": SCIENTIFIC_INPUT_MANIFEST_SHA256,
                "embedded": False,
            },
            "scientific_return": {
                "bytes": SCIENTIFIC_RETURN_BYTES,
                "sha256": SCIENTIFIC_RETURN_SHA256,
                "manifest_sha256": SCIENTIFIC_RETURN_MANIFEST_SHA256,
                "embedded": False,
            },
        },
        "excluded_classes": [
            "prior/quarantined implementation", "nested archives", "dependency trees",
            "build outputs", "caches/databases", "browser/session state", "credentials",
            "full-slot gzip captures", "live job logs", "historical diagnostic logs",
        ],
        "targeted_selection_scan": targeted_selection,
        "selection_gitleaks_scan": selection_scan,
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
    final = {name: item["bytes"] for name, item in payloads.items()}
    final["MANIFEST.json"] = manifest_bytes
    validate_names(list(final))

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    require(OUTPUT_DIR.is_dir() and not OUTPUT_DIR.is_symlink(), "unsafe output directory")
    temporary = tempfile.NamedTemporaryFile(
        prefix=".precision116-packet-", suffix=".tmp", dir=OUTPUT_DIR, delete=False
    )
    temporary_path = Path(temporary.name)
    temporary.close()
    try:
        with zipfile.ZipFile(
            temporary_path, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=6
        ) as archive:
            for name, blob in sorted(final.items()):
                info = zipfile.ZipInfo(name, date_time=(2026, 9, 7, 0, 0, 0))
                info.create_system = 3
                info.external_attr = (stat.S_IFREG | 0o644) << 16
                info.compress_type = zipfile.ZIP_DEFLATED
                archive.writestr(info, blob)
        decoded = verify_final_archive(temporary_path, final)
        decoded_payloads = {
            name: {"bytes": blob, "origin": "decoded-final-archive"}
            for name, blob in decoded.items()
        }
        targeted_final = targeted_content_scan(decoded_payloads, "decoded final archive")
        final_scan = gitleaks_scan(decoded_payloads, "all decoded final archive members")
        require(status_snapshot() == initial_status, "source/status changed during packaging")
        require(git("rev-parse", "HEAD").decode().strip() == documentation_head,
                "packaging HEAD changed during packaging")
        archive_bytes = temporary_path.read_bytes()
        os.link(temporary_path, OUTPUT)
    finally:
        if temporary_path.exists():
            temporary_path.unlink()

    require(OUTPUT.read_bytes() == archive_bytes, "published ZIP byte equality failure")
    receipt = {
        "archive_absolute_path": str(OUTPUT),
        "archive_bytes": len(archive_bytes),
        "archive_sha256": sha256(archive_bytes),
        "regular_members": len(final),
        "payload_members": len(payloads),
        "manifest_sha256": sha256(manifest_bytes),
        "final_archive_validation": {
            "crc": "PASS",
            "regular_unencrypted_members": "PASS",
            "safe_unique_paths": "PASS",
            "decoded_byte_equality": "PASS",
            "manifest_payload_closure": "PASS",
            "nested_archives_absent": "PASS",
        },
        "source_commit": SOURCE_COMMIT,
        "packaging_documentation_head": documentation_head,
        "source_tree_status": initial_status,
        "targeted_selection_scan": targeted_selection,
        "selection_gitleaks_scan": selection_scan,
        "targeted_decoded_final_scan": targeted_final,
        "decoded_final_gitleaks_scan": final_scan,
        "included_paths": sorted(final),
    }
    print(json.dumps(receipt, indent=2, sort_keys=True))


if __name__ == "__main__":
    main()
