#!/usr/bin/env python3
"""Build one exact-source precision-adjudication ZIP; never compile or run crypto."""

import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import stat
import subprocess
import zlib
import zipfile


ROOT = Path("/Users/lifeng/Documents/20231788-openfhe-paper-scale-implementation-20260905")
HERE = ROOT / "artifacts/handoffs/paper-scale-precision-adjudication-01"
BRANCH = "codex/paper-scale-implementation-20260905"
SOURCE = "9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e"
PRODUCTION = "b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89"
PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
OLD_PACKET = ROOT / "artifacts/handoffs/paper-scale-precision-diagnosis-01/paper-scale-precision-b1b024e3.zip"
OLD_PACKET_BYTES = 1_480_623
OLD_PACKET_SHA256 = "28fa7ee1297af78ac4c2848b85d1fce1efdd81bbd1a13d15b0e75076c2719336"
TASK_DRAFT = HERE / "TASK_DRAFT.md"
TASK_DRAFT_SHA256 = "6338316b50995982b508b9109bcb8503829907fbbfc80bffffc1d4f699361902"
OUT = HERE / "paper-scale-precision-adjudication-9f6c8eae.zip"
GITLEAKS = "/opt/homebrew/bin/gitleaks"

FROZEN_REQUIREMENTS = (
    "coordination/TEST_SEAMS.md",
    "coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md",
    "coordination/paper-scale-integration-01/PRODUCTION_CONTRACT_01.md",
    "coordination/paper-scale-integration-01/NOMINAL_SCALE_AUDIT_01.md",
    "coordination/paper-scale-integration-01/INPUT_DOMAIN_AUDIT_01.md",
    "coordination/paper-scale-integration-01/ORACLE_ADVERSARIAL_AUDIT_01.md",
)

SIGNED_EVIDENCE = (
    "LINUX_RAW.log",
    "WINDOWS_LF.log",
    "RUN_TERMINAL_01.json",
    "RAW_PREFLIGHT.json",
    "LINUX_SIGNED_ERROR.json",
    "WINDOWS_SIGNED_ERROR.json",
    "LINUX_VERIFICATION.json",
    "WINDOWS_VERIFICATION.json",
    "verify_signed_error.py",
    "verify_signed_run.py",
    "LINUX_AUDIT.md",
    "WINDOWS_AUDIT.md",
    "SIGNED_ERROR_AUDIT.md",
    "ROOT_REEXECUTION.json",
    "ACCEPTANCE.md",
)

PRECISION_RETURN = (
    "PRO_DIAGNOSIS.md",
    "PRO_EXECUTION_LEDGER.md",
    "ROOT_VALIDATION.json",
    "RETURN_PREFLIGHT.json",
    "INTEGRATION_VERIFICATION.json",
)

DENIED_SEGMENTS = {
    ".git", "node_modules", "__pycache__", ".env", "credentials", "cookies",
    "browser-state", "browser_state", "session", "sessions",
}
DENIED_BASENAMES = {
    ".env", "id_rsa", "id_ed25519", "credentials.json", "cookies.json",
    "known_hosts", "authorized_keys",
}
DENIED_SUFFIXES = {".key", ".pem", ".p12", ".pfx", ".jks", ".keystore"}
SECRET_PATTERNS = (
    rb"-----BEGIN (?:RSA |EC |OPENSSH |DSA )?PRIVATE KEY-----",
    rb"gh[pousr]_[A-Za-z0-9]{30,}",
    rb"github_pat_[A-Za-z0-9_]{60,}",
    rb"sk-(?:proj-|ant-)?[A-Za-z0-9_-]{32,}",
    rb"AKIA[0-9A-Z]{16}",
    rb"xox[baprs]-[A-Za-z0-9-]{20,}",
)


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def git(*args):
    return subprocess.check_output(["git", "-C", str(ROOT), *args])


def git_show(commit, path):
    return git("show", f"{commit}:{path}")


def safe_regular_path(path):
    if not path or path.startswith("/") or "\\" in path or "\x00" in path:
        return False
    pure = PurePosixPath(path)
    if str(pure) != path or any(part in ("", ".", "..") for part in pure.parts):
        return False
    lowered = [part.casefold() for part in pure.parts]
    basename = lowered[-1]
    return (not any(part in DENIED_SEGMENTS for part in lowered)
            and basename not in DENIED_BASENAMES
            and Path(basename).suffix not in DENIED_SUFFIXES)


def zip_member_is_regular(info):
    mode = info.external_attr >> 16
    return (not info.is_dir() and not (info.flag_bits & 1)
            and (mode == 0 or stat.S_ISREG(mode)))


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--documentation-head", required=True,
                    help="clean, published documentation commit that supplies all adjudication evidence")
args = parser.parse_args()
documentation_head = args.documentation_head.lower()

if not __debug__:
    raise RuntimeError("Safety checks require ordinary Python execution without -O")
require(re.fullmatch(r"[0-9a-f]{40}", documentation_head),
        "--documentation-head must be one full lowercase commit SHA")
require(git("rev-parse", "--verify", "HEAD^{commit}").decode().strip() == documentation_head,
        "repository HEAD does not equal --documentation-head")
require(git("branch", "--show-current").decode().strip() == BRANCH, "wrong source branch")
require(git("status", "--porcelain=v1", "--untracked-files=all") == b"", "repository is not clean")
require(git("cat-file", "-t", SOURCE).strip() == b"commit", "tested source commit unavailable")
require(git("cat-file", "-t", PRODUCTION).strip() == b"commit", "production source commit unavailable")
require(not OUT.exists(), f"refusing to overwrite existing packet: {OUT}")
require(TASK_DRAFT.is_file(), "new task draft missing")
task_bytes = TASK_DRAFT.read_bytes()
require(sha256(task_bytes) == TASK_DRAFT_SHA256, "new task draft identity changed")

# HEAD may add only documentation/evidence after the exact tested source.
protected = ("src", "include", "tests", "CMakeLists.txt", ".github/workflows/dcp-rcb.yml")
require(git("diff", "--name-only", SOURCE, documentation_head, "--", *protected) == b"",
        "documentation HEAD changes tested code/build/workflow inputs")
require(git("diff", "--name-only", PRODUCTION, SOURCE, "--", "src", "include") == b"",
        "tested production src/include differ from b1b024e")

payload = {}
rows = []
casefold_paths = set()


def add(path, data, origin):
    require(safe_regular_path(path), f"unsafe archive path: {path!r}")
    require(path not in payload and path.casefold() not in casefold_paths,
            f"duplicate/case-colliding archive path: {path}")
    require(isinstance(data, bytes), f"non-byte payload: {path}")
    require(isinstance(origin, dict) and origin, f"missing origin: {path}")
    require(not any(re.search(pattern, data) for pattern in SECRET_PATTERNS),
            f"targeted credential signature in {path}")
    payload[path] = data
    casefold_paths.add(path.casefold())
    rows.append({"path": path, "bytes": len(data), "sha256": sha256(data), "origin": origin})


engineering = git("ls-tree", "-r", "--name-only", SOURCE,
                  "src", "include", "tests", "CMakeLists.txt", ".github/workflows/dcp-rcb.yml").decode().splitlines()
require(len(engineering) == 38 and len(engineering) == len(set(engineering)),
        "tested source must expose exactly 38 unique engineering files")
for path in engineering:
    add("project/" + path, git_show(SOURCE, path),
        {"kind": "cleanroom_git", "commit": SOURCE, "path": path})
for path in FROZEN_REQUIREMENTS:
    add("project/" + path, git_show(SOURCE, path),
        {"kind": "frozen_requirement_git", "commit": SOURCE, "path": path})
add("TASK.md", task_bytes,
    {"kind": "new_task_brief", "source_path": str(TASK_DRAFT.relative_to(ROOT)),
     "source_sha256": TASK_DRAFT_SHA256, "documentation_checkpoint": documentation_head,
     "packaged_as": "TASK.md", "supersedes": "old packet TASK.md"})

# The prior diagnosis packet is an immutable, verified source for non-project
# paper, official-source, Boost and first-run evidence. Its old TASK is excluded.
old_bytes = OLD_PACKET.read_bytes()
require(len(old_bytes) == OLD_PACKET_BYTES and sha256(old_bytes) == OLD_PACKET_SHA256,
        "prior diagnosis packet byte identity changed")
with zipfile.ZipFile(OLD_PACKET) as archive:
    infos = archive.infolist()
    require(archive.testzip() is None, "prior diagnosis packet CRC failure")
    require(len(infos) == len({info.filename.casefold() for info in infos}),
            "prior diagnosis packet has duplicate/case-colliding paths")
    require(all(safe_regular_path(info.filename) and zip_member_is_regular(info) for info in infos),
            "prior diagnosis packet has unsafe/non-regular/encrypted member")
    old_decoded = {info.filename: archive.read(info.filename) for info in infos}
    for info in infos:
        require((zlib.crc32(old_decoded[info.filename]) & 0xffffffff) == info.CRC,
                f"prior diagnosis packet CRC mismatch: {info.filename}")
    old_manifest = json.loads(old_decoded["MANIFEST.json"])
    require(old_manifest["manifest_self_excluded"] is True
            and old_manifest["source"] == PRODUCTION and old_manifest["official_pin"] == PIN,
            "prior diagnosis manifest identity mismatch")
    old_rows = old_manifest["files"]
    require(len(old_rows) == 133 and {row["path"] for row in old_rows} == set(old_decoded) - {"MANIFEST.json"},
            "prior diagnosis manifest coverage mismatch")
    for row in old_rows:
        data = old_decoded[row["path"]]
        require(len(data) == row["bytes"] and sha256(data) == row["sha256"] and row.get("origin"),
                f"prior diagnosis manifest row mismatch: {row['path']}")

    inherited = sorted(path for path in old_decoded
                       if path.startswith(("official-full/", "paper/", "boost-1.83.0/", "evidence/")))
    require(sum(path.startswith("official-full/") for path in inherited) == 77,
            "prior official-source inventory changed")
    require(sum(path.startswith("paper/") for path in inherited) == 2,
            "prior paper inventory changed")
    require(sum(path.startswith("boost-1.83.0/") for path in inherited) == 4,
            "prior Boost inventory changed")
    require(sum(path.startswith("evidence/") for path in inherited) == 5,
            "prior first-run evidence inventory changed")
    prior_nonproject = {path for path in old_decoded if not path.startswith("project/")}
    require(prior_nonproject == set(inherited) | {"TASK.md", "MANIFEST.json"},
            "prior packet has an unclassified non-project payload")
    old_by_path = {row["path"]: row for row in old_rows}
    for path in inherited:
        prior = old_by_path[path]
        add(path, old_decoded[path],
            {"kind": "verified_inherited_payload",
             "input_archive": {"path": str(OLD_PACKET.relative_to(ROOT)),
                               "bytes": OLD_PACKET_BYTES, "sha256": OLD_PACKET_SHA256},
             "prior_origin": prior["origin"], "prior_bytes": prior["bytes"],
             "prior_sha256": prior["sha256"]})

# New dual-host signed diagnostic evidence and the integrated Pro diagnosis are
# read only from the explicitly supplied clean documentation checkpoint.
signed_root = "coordination/paper-scale-signed-diagnostic-run-01"
for name in SIGNED_EVIDENCE:
    source_path = f"{signed_root}/{name}"
    add(f"evidence/signed-diagnostic-run/{name}", git_show(documentation_head, source_path),
        {"kind": "tracked_signed_diagnostic_evidence", "commit": documentation_head,
         "path": source_path, "tested_source": SOURCE})

return_root = "coordination/paper-scale-precision-return-01"
for name in PRECISION_RETURN:
    source_path = f"{return_root}/{name}"
    add(f"review/precision-diagnosis-return/{name}", git_show(documentation_head, source_path),
        {"kind": "tracked_precision_diagnosis_review", "commit": documentation_head,
         "path": source_path, "production_source": PRODUCTION, "tested_source": SOURCE})

require(len(payload) == 38 + 6 + 1 + 77 + 2 + 4 + 5 + len(SIGNED_EVIDENCE) + len(PRECISION_RETURN),
        "unexpected payload count before manifest")
rows.sort(key=lambda row: row["path"])
manifest = {
    "schema": "paper-precision-adjudication-v1",
    "tested_source": SOURCE,
    "production_source": PRODUCTION,
    "documentation_checkpoint": documentation_head,
    "source_branch": BRANCH,
    "source_tree_status": "clean",
    "official_pin": PIN,
    "prior_input_archive": {"path": str(OLD_PACKET.relative_to(ROOT)),
                            "bytes": OLD_PACKET_BYTES, "sha256": OLD_PACKET_SHA256},
    "task_source": {"path": str(TASK_DRAFT.relative_to(ROOT)), "sha256": TASK_DRAFT_SHA256},
    "manifest_self_excluded": True,
    "files": rows,
}
payload["MANIFEST.json"] = (json.dumps(manifest, indent=2, sort_keys=True) + "\n").encode("utf-8")

scan_env = dict(os.environ, GOMAXPROCS="2")
scan_env.pop("GITLEAKS_CONFIG", None)
scan_env.pop("GITLEAKS_CONFIG_TOML", None)


def scan(decoded_payloads):
    framed = b"".join(path.encode("utf-8") + b"\0" + str(len(data)).encode("ascii") + b"\0" + data + b"\n"
                       for path, data in sorted(decoded_payloads.items()))
    result = subprocess.run(
        [GITLEAKS, "stdin", "--ignore-gitleaks-allow", "--gitleaks-ignore-path", "/dev/null",
         "--max-decode-depth", "5", "--max-archive-depth", "1", "--redact", "--no-banner",
         "--no-color", "--report-format", "json", "--report-path", "-"],
        input=framed, env=scan_env, capture_output=True, check=False)
    require(result.returncode == 0,
            "strict gitleaks stdin scan failed; inspect redacted output without packaging")
    findings = json.loads(result.stdout)
    require(findings == [], "strict gitleaks scan returned findings")
    return {"status": "PASS", "findings": 0, "decoded_payload_count": len(decoded_payloads),
            "framed_bytes": len(framed), "framed_sha256": sha256(framed)}


# Scan every decoded payload, including the manifest, before creating any ZIP.
scanner_version = subprocess.check_output([GITLEAKS, "version"], text=True).strip()
selection_scan = scan(payload)
OUT.parent.mkdir(parents=True, exist_ok=True)
with zipfile.ZipFile(OUT, "x", compression=zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
    for path, data in sorted(payload.items()):
        info = zipfile.ZipInfo(path, date_time=(2026, 9, 6, 0, 0, 0))
        info.compress_type = zipfile.ZIP_DEFLATED
        info.external_attr = 0o100644 << 16
        archive.writestr(info, data)

with zipfile.ZipFile(OUT) as archive:
    members = archive.infolist()
    require(archive.testzip() is None, "new packet CRC failure")
    require(len(members) == len(payload) == len({info.filename.casefold() for info in members}),
            "new packet path count/uniqueness mismatch")
    require(all(safe_regular_path(info.filename) and zip_member_is_regular(info) for info in members),
            "new packet has unsafe/non-regular/encrypted member")
    decoded = {info.filename: archive.read(info.filename) for info in members}
    require(decoded == payload, "new packet decoded content differs from selected payload")
    for info in members:
        require((zlib.crc32(decoded[info.filename]) & 0xffffffff) == info.CRC,
                f"new packet CRC mismatch: {info.filename}")
    decoded_manifest = json.loads(decoded["MANIFEST.json"])
    require(decoded_manifest == manifest, "new packet manifest roundtrip mismatch")
    require({row["path"] for row in decoded_manifest["files"]} == set(decoded) - {"MANIFEST.json"},
            "new packet manifest does not cover every non-self payload")
    for row in decoded_manifest["files"]:
        data = decoded[row["path"]]
        require(len(data) == row["bytes"] and sha256(data) == row["sha256"] and row["origin"],
                f"new packet manifest row mismatch: {row['path']}")

final_payload_scan = scan(decoded)
zip_bytes = OUT.read_bytes()
print(json.dumps({
    "status": "VERIFIED_NOT_UPLOADED",
    "tested_source": SOURCE,
    "production_source": PRODUCTION,
    "documentation_checkpoint": documentation_head,
    "source_branch": BRANCH,
    "source_tree_status": "clean before output creation",
    "zip": str(OUT),
    "zip_bytes": len(zip_bytes),
    "zip_sha256": sha256(zip_bytes),
    "member_count": len(payload),
    "manifest_payload_count": len(rows),
    "manifest_sha256": sha256(payload["MANIFEST.json"]),
    "engineering_files": len(engineering),
    "official_files": sum(path.startswith("official-full/") for path in payload),
    "prior_input_archive": {"path": str(OLD_PACKET), "bytes": OLD_PACKET_BYTES,
                            "sha256": OLD_PACKET_SHA256},
    "signed_evidence_files": len(SIGNED_EVIDENCE),
    "precision_return_files": len(PRECISION_RETURN),
    "selection_scan": selection_scan,
    "final_archive_payload_scan": final_payload_scan,
    "scanner": scanner_version,
    "safe_unique_regular_paths": True,
    "crc_and_manifest_roundtrip": "PASS",
    "network_used": False,
    "local_build_or_crypto_used": False,
    "uploaded": False,
}, indent=2, sort_keys=True))
