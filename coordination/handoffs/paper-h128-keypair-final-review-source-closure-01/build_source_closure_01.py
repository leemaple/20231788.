#!/usr/bin/env python3
"""Mechanical, offline source-closure packet build. No engineering, crypto or CI."""
import argparse
import hashlib
import io
import json
import os
from pathlib import Path, PurePosixPath
import re
import stat
import subprocess
import unicodedata
import zipfile

REPO = Path("/Users/lifeng/Documents/20231788-openfhe-paper-h128-keypair-20260905")
OFFICIAL = Path("/private/tmp/h128-pro-packet.Lo406g/official-source")
PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
TESTED = "1192200f558c69c0967e8306ed1a8bddf786ca34"
ORIGINAL = REPO / "artifacts/handoffs/h128-final-review-01/build-03/paper-h128-final-review-1192200.zip"
OLD_SHA = "c4b8012a1f690d40c5571b24ae7828f414a5d05c6715a45fec0f3cb3e6710305"
RETURN = REPO / "artifacts/handoffs/h128-final-review-return-01/paper-h128-final-review-1192200-return.zip"
RETURN_SHA = "31e3bea98c2468a31be2eab1f3688f238e68be393dc6a8ecad2a1d5692a6a416"
TASK = REPO / "coordination/tasks/PAPER_H128_KEYPAIR_FINAL_REVIEW_SOURCE_CLOSURE_01.md"
ALLOWED = REPO / "artifacts/handoffs/h128-final-review-source-closure-01"
ROOT = "paper-h128-final-review-source-closure-1192200"
ADDITIONS = [
    ("src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp", "IC-01"),
    ("src/pke/include/schemerns/rns-leveledshe.h", "IC-02"),
    ("src/pke/lib/schemerns/rns-leveledshe.cpp", "IC-02"),
    ("src/pke/include/schemebase/decrypt-result.h", "IC-01 direct return-type closure; not a new P1"),
]
ROOT_RECONCILIATION = {
    "provenance": "root coordination message; packager did not rerun the returned verifier",
    "method": "reviewer verifier read completely, isolated Python -I offline execution, complete sorted-object comparison",
    "output_sha256": "e3edb0cf78b5a60ab74583b8818f5883afb55bba020eb7dbdb3dcbc2a2193df1",
    "supplied_mechanical_checks_object_diff_exit": 0,
    "patch_replays": 10, "CTest_invocations": 34, "numbered_test_starts": 718,
    "jobs": 8, "bindings": 58, "Git_blob_checks": 259, "Lucas_certificates": 39,
    "skipped_composites": 213, "searches": 5, "roots": 4,
    "manual_source_anchors": {
        "result": "PASS_REFERENCES_WITH_3_EXPLICIT_INPUT_CLOSURE_GAPS",
        "total_anchors": 111, "unique_anchors": 86, "input_files": 38,
        "reading_map_records": 10, "manual_groups": 8, "manual_anchors": 29,
        "errors": 0,
        "audit_sha256": "ec5e29e83d49584a007f1bfb443e7517c8f1d42b4eeef767973210f25958b27c",
        "boundary": "Independent source-reference identity/range checking, not a new semantic review or model authentication",
    },
    "new_crypto_build_NTT_or_service_authentication": False,
}


def require(ok, message):
    if not ok:
        raise ValueError(message)


def run(args, **kwargs):
    return subprocess.check_output(args, **kwargs)


def digest(data):
    return hashlib.sha256(data).hexdigest()


def identity(data):
    return {"bytes": len(data), "sha256": digest(data)}


def json_bytes(value):
    return (json.dumps(value, indent=2, ensure_ascii=False) + "\n").encode()


def safe(path):
    require(path and not path.startswith("/") and "\\" not in path and ":" not in path and "\0" not in path, "unsafe path")
    parts = path.split("/")
    require(all(p not in ("", ".", "..") for p in parts), "unsafe path segment")
    require(str(PurePosixPath(path)) == path, "non-canonical path")
    forbidden = {".git", "node_modules", "__pycache__", ".cache", ".ci", "build", "dist", "coverage",
                 "browser-state", "browser_state", "cookies", "credentials"}
    require(not any(p.casefold() in forbidden for p in parts), "excluded runtime/credential path")
    name = parts[-1].casefold()
    require(not (name.startswith(".env") or name in {"id_rsa", "id_dsa", "id_ecdsa", "id_ed25519", "cookies.txt", "cookies.json", ".gitleaks.toml", ".gitleaksignore"}
                 or name.endswith((".pem", ".key", ".p12", ".pfx", ".sqlite", ".sqlite3", ".db", ".db3", ".dylib", ".so", ".o", ".a", ".exe", ".dll"))),
            "excluded file")


def read_archive(path, expected_sha, expected_bytes, outer, count):
    require(path.is_file() and not path.is_symlink(), "archive not regular local input")
    data = path.read_bytes()
    require(identity(data) == {"bytes": expected_bytes, "sha256": expected_sha}, "archive identity " + path.name)
    sidecar = Path(str(path) + ".sha256").read_bytes()
    require(sidecar == (expected_sha + "  " + path.name + "\n").encode(), "sidecar identity")
    with zipfile.ZipFile(io.BytesIO(data)) as z:
        rows = z.infolist()
        names = [r.filename for r in rows]
        require(len(rows) == count and len(set(names)) == count, "archive member count/raw duplicates")
        require(len({unicodedata.normalize("NFC", n).casefold() for n in names}) == count, "NFC-casefold conflict")
        require(sum(r.file_size for r in rows) < 32 * 1024 * 1024, "expanded-size bound")
        for r in rows:
            safe(r.filename)
            require(r.filename.startswith(outer + "/") and not r.is_dir() and stat.S_ISREG(r.external_attr >> 16)
                    and not r.flag_bits & 1, "nonregular/encrypted/outside-root archive member")
        require(z.testzip() is None, "archive CRC")
        files = {r.filename[len(outer) + 1:]: z.read(r) for r in rows}
    return files, {"path": str(path), **identity(data), "members": count,
                   "expanded_bytes": sum(map(len, files.values())), "sidecar": identity(sidecar)}


def verify_manifest(files, count):
    manifest = json.loads(files["MANIFEST.json"])
    rows = manifest["files"]
    require(manifest["self_exclusion"] == ["MANIFEST.json"], "self-exclusion")
    require(len(rows) == count and len({r["path"] for r in rows}) == count, "manifest count/duplicates")
    require({r["path"] for r in rows} == set(files) - {"MANIFEST.json"}, "manifest not closed")
    for row in rows:
        safe(row["path"])
        b = files[row["path"]]
        require(identity(b) == {k: row[k] for k in ("bytes", "sha256")}, "manifest identity " + row["path"])
        if row.get("git_blob"):
            blob = hashlib.sha1(b"blob " + str(len(b)).encode() + b"\0" + b).hexdigest()
            require(blob == row["git_blob"], "Git blob byte identity " + row["path"])
    return {r["path"]: r for r in rows}


def pinned_source(path):
    object_name = PIN + ":" + path
    data = run(["git", "-C", str(OFFICIAL), "show", object_name])
    blob = run(["git", "-C", str(OFFICIAL), "rev-parse", object_name], text=True).strip()
    require(run(["git", "-C", str(OFFICIAL), "cat-file", "-t", object_name], text=True).strip() == "blob", "not blob")
    require(hashlib.sha1(b"blob " + str(len(data)).encode() + b"\0" + data).hexdigest() == blob, "new official blob mismatch")
    return data, {"source_commit": PIN, "source_path": path, "git_blob": blob, **identity(data)}


def targeted_scan(files):
    # Credential shapes, not mere mentions of public API names or keys in C++.
    rules = {
        "private-key-material": rb"-----BEGIN (?:RSA |EC |OPENSSH |DSA |ENCRYPTED )?PRIVATE KEY-----",
        "github-token": rb"\b(?:gh[pousr]_[A-Za-z0-9]{30,}|github_pat_[A-Za-z0-9_]{40,})\b",
        "aws-access-key": rb"\b(?:AKIA|ASIA)[0-9A-Z]{16}\b",
        "openai-key": rb"\bsk-(?:proj-|svcacct-)?[A-Za-z0-9_-]{32,}\b",
        "jwt": rb"\beyJ[A-Za-z0-9_-]{12,}\.[A-Za-z0-9_-]{12,}\.[A-Za-z0-9_-]{12,}\b",
    }
    findings = []
    for name, data in files.items():
        safe(name)
        for rule, pattern in rules.items():
            if re.search(pattern, data):
                findings.append({"path": name, "rule": rule})
    require(not findings, "targeted credential scan failed; values intentionally not printed")
    return {"result": "PASS", "files": len(files), "rules": sorted(rules),
            "filename_runtime_and_credential_exclusions": "PASS", "findings": []}


def scanner(path, report, capture):
    env = dict(os.environ)
    for key in ("GITLEAKS_CONFIG", "GITLEAKS_CONFIG_TOML"):
        env.pop(key, None)
    args = ["/opt/homebrew/bin/gitleaks", "dir", str(path), "--redact", "--no-banner", "--no-color",
            "--ignore-gitleaks-allow", "--gitleaks-ignore-path", os.devnull,
            "--max-decode-depth", "5", "--max-archive-depth", "1",
            "--report-format", "json", "--report-path", str(report)]
    result = subprocess.run(args, cwd=capture.parent, env=env, capture_output=True, timeout=120)
    capture.write_bytes(result.stdout + result.stderr)
    require(result.returncode in (0, 1), "gitleaks execution failure")
    parsed = json.loads(report.read_bytes())
    return {"command": args, "exit_code": result.returncode, "findings": len(parsed or []),
            "redacted_report": {"path": str(report), **identity(report.read_bytes())},
            "capture": {"path": str(capture), **identity(capture.read_bytes())},
            "ambient_configuration": "GITLEAKS_CONFIG/TOML removed; no packaged .gitleaks config; ignore path /dev/null; inline allowances disabled",
            "version": run(["/opt/homebrew/bin/gitleaks", "version"], text=True).strip()}


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--output", type=Path, default=ALLOWED / "build-01")
    args = ap.parse_args()
    out = args.output.resolve()
    require(out.parent == ALLOWED.resolve() and out.name.startswith("build-") and not out.exists(), "output must be a new direct build-* directory")
    require(run(["git", "-C", str(OFFICIAL), "rev-parse", "HEAD"], text=True).strip() == PIN, "official checkout pin")
    require(run(["git", "-C", str(OFFICIAL), "status", "--porcelain"], text=True) == "", "official checkout dirty")
    require(run(["git", "-C", str(REPO), "branch", "--show-current"], text=True).strip() == "codex/paper-h128-keypair-01", "project branch")
    before_head = run(["git", "-C", str(REPO), "rev-parse", "HEAD"], text=True).strip()
    old, old_id = read_archive(ORIGINAL, OLD_SHA, 2202028, "paper-h128-final-review-1192200", 315)
    require(old_id["expanded_bytes"] == 7760741, "old expanded identity")
    old_rows = verify_manifest(old, 314)
    returned, return_id = read_archive(RETURN, RETURN_SHA, 47452, "paper-h128-final-review-1192200-return", 5)
    require(set(returned) == {"REVIEW.md", "FINDINGS.md", "EVIDENCE_CHECKS.json", "MANIFEST.json", "verify_review.py"}, "review return paths")
    verify_manifest(returned, 4)
    require(json.loads(returned["EVIDENCE_CHECKS.json"])["disposition"] == "BLOCKED_INPUT_CLOSURE", "actual return disposition")
    files, origins = {}, {}
    def add(path, data, origin, **metadata):
        safe(path)
        require(path not in files, "duplicate output member " + path)
        files[path] = data
        origins[path] = {"path": path, **identity(data), "origin": origin, **metadata}
    mapping = []
    for path, data in sorted(old.items()):
        dest = "original-input/" + path if path in {"TASK.md", "MANIFEST.json"} else path
        add(dest, data, "byte-identical original build-03 archive member",
            original_member=path, original_archive_sha256=OLD_SHA,
            historical_record=old_rows.get(path))
        mapping.append({"original_path": path, "new_path": dest, **identity(data),
                        "original_manifest_payload": path != "MANIFEST.json",
                        "byte_identical": True, "path_unchanged": path == dest})
    additions = []
    for path, finding in ADDITIONS:
        dest = "official/" + path
        require(dest not in old, "supplement not actually missing")
        data, info = pinned_source(path)
        add(dest, data, "exact official Git object", **info)
        additions.append({"path": dest, "closes": finding, **info})
    for path, data in sorted(returned.items()):
        add("review-return-original/" + path, data, "untrusted original completed Pro review return; bytes unchanged",
            original_member=path, original_return_zip_sha256=RETURN_SHA)
    task = TASK.read_bytes()
    require(old["TASK.md"] in task, "new task does not contain complete original task byte sequence")
    require(b"1000 repetitions are cancelled" in task, "current acceptance scope override absent")
    add("TASK.md", task, "current self-contained source-closure task; historical sections explicitly superseded")
    add("tools/build_source_closure_01.py", Path(__file__).read_bytes(), "this mechanical packaging script; not an instruction to reviewer to execute")
    add("ORIGINAL_PAYLOAD_MAPPING.json", json_bytes({
        "schema": "h128-original-member-lossless-mapping-v1",
        "original_archive": old_id, "original_payloads": 314, "original_members_including_manifest": 315,
        "mapping": mapping, "result": "PASS: all 315 original members preserved byte-for-byte; all 314 payloads accounted for"
    }), "mechanical one-to-one original archive mapping")
    add("SOURCE_CLOSURE_PROVENANCE.json", json_bytes({
        "schema": "h128-source-closure-provenance-v1", "source": TESTED,
        "packaging_checkout_observed": before_head, "official_commit": PIN,
        "original_archive": old_id, "original_return_archive": return_id,
        "new_official_sources": additions, "original_official_files": 74, "current_official_files": 78,
        "original_payloads_preserved": 314, "original_total_members_preserved": 315,
        "relocated_original_members": {"TASK.md": "original-input/TASK.md", "MANIFEST.json": "original-input/MANIFEST.json"},
        "old_root_PROVENANCE_and_README": "unchanged historical bytes; current provenance is this file",
        "current_TASK_source": str(TASK), "current_TASK_identity": identity(task),
        "current_scope": "source closure only; no engineering/test/log changes; no experiments",
        "latest_user_acceptance_update": "1000 repetitions cancelled; appropriate correctness oracles and key/boundary/dual-platform/paper-consecutive correctness gates remain; no new performance gate",
        "root_reported_original_return_mechanical_reconciliation": ROOT_RECONCILIATION,
        "manual_source_review": "Original reference identity/range audit passed; corrected-source Pro semantic review remains pending",
        "hash_boundary": "Current MANIFEST excludes only itself; archive size/SHA and manifest hash belong to external preflight/sidecar, not this inner payload",
        "execution": {"build_crypto_NTT": "NOT RUN", "CI_network_external_agents": "NOT USED",
                      "commit_push_merge": "NOT USED", "returned_verifier_executed_by_packager": False}
    }), "new source-closure provenance; old identities explicitly historical")
    require(sum(p.startswith("official/") for p in files) == 78, "official source count")
    require(sum(p.startswith("current/evidence/") and p.endswith("_JOB_01.log") for p in files) == 8, "eight complete logs retained")
    for row in mapping:
        require(files[row["new_path"]] == old[row["original_path"]], "original member changed")
    add("MANIFEST.json", json_bytes({"schema": "closed-review-manifest-v1", "self_exclusion": ["MANIFEST.json"],
                                     "files": [origins[k] for k in sorted(origins)]}), "current non-self closed manifest")
    require(len(files) == 329, "unexpected current member count")
    verify_manifest(files, 328)
    folded = [unicodedata.normalize("NFC", p).casefold() for p in files]
    require(len(set(folded)) == len(files), "current path collision")
    targeted_selection = targeted_scan(files)
    out.mkdir(parents=True, exist_ok=False)
    stage = out / ROOT
    stage.mkdir()
    for path, data in sorted(files.items()):
        target = stage / path
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(data)
    archive = out / (ROOT + ".zip")
    with zipfile.ZipFile(archive, "x", zipfile.ZIP_DEFLATED, compresslevel=9) as z:
        for path, data in sorted(files.items()):
            item = zipfile.ZipInfo(ROOT + "/" + path, (2026, 9, 5, 0, 0, 0))
            item.create_system = 3
            item.external_attr = (stat.S_IFREG | 0o644) << 16
            z.writestr(item, data, compress_type=zipfile.ZIP_DEFLATED, compresslevel=9)
    archive_data = archive.read_bytes()
    sidecar = Path(str(archive) + ".sha256")
    sidecar.write_text(digest(archive_data) + "  " + archive.name + "\n", encoding="ascii")
    reread, new_id = read_archive(archive, digest(archive_data), len(archive_data), ROOT, 329)
    require(reread == files, "final archive content differs from expanded selection")
    verify_manifest(reread, 328)
    targeted_archive = targeted_scan(reread)
    scan_selection = scanner(stage, out / "gitleaks-selection.json", out / "gitleaks-selection.log")
    scan_archive = scanner(archive, out / "gitleaks-archive.json", out / "gitleaks-archive.log")
    scan_ok = all(x["exit_code"] == 0 and x["findings"] == 0 for x in (scan_selection, scan_archive))
    # Verify all immutable inputs after packaging; never modify the old ZIP or task.
    require(identity(ORIGINAL.read_bytes()) == {"bytes": 2202028, "sha256": OLD_SHA}, "original input changed during build")
    require(identity(RETURN.read_bytes()) == {"bytes": 47452, "sha256": RETURN_SHA}, "original return changed during build")
    result = {
        "schema": "h128-source-closure-preflight-v1",
        "result": "PASS" if scan_ok else "BLOCKED_SECRET_SCAN_REVIEW",
        "archive": new_id, "archive_sha256": digest(archive_data),
        "current_manifest": identity(files["MANIFEST.json"]), "current_manifest_payloads": 328,
        "current_TASK": identity(task), "builder": identity(Path(__file__).read_bytes()),
        "original_archive": old_id, "original_return_archive": return_id,
        "original_member_mapping": identity(files["ORIGINAL_PAYLOAD_MAPPING.json"]),
        "original_members_preserved": 315, "original_payloads_preserved": 314,
        "original_paths_unchanged": 313, "original_paths_relocated": 2,
        "all_eight_full_logs_retained": True, "official_files": 78, "added_official": additions,
        "path_safety": "PASS: regular unencrypted unique raw/NFC-casefold names, one root, no traversal, forbidden artifacts absent, CRC",
        "manifest_closure": "PASS: exact membership, bytes/SHA; non-self only",
        "archive_selection_byte_equality": "PASS",
        "targeted_selection": targeted_selection, "targeted_archive": targeted_archive,
        "gitleaks_selection": scan_selection, "gitleaks_archive": scan_archive,
        "packaging_checkout_before": before_head,
        "packaging_checkout_after": run(["git", "-C", str(REPO), "rev-parse", "HEAD"], text=True).strip(),
        "root_reported_original_return_mechanical_reconciliation": ROOT_RECONCILIATION,
        "remaining_gates": ["Root independent final ZIP integrity/security check",
                            "Original manual-reference audit passed; this does not predetermine the corrected-source semantic verdict",
                            "Corrected complete packet upload and actual source-closure review, with no claim of existing acceptance"],
        "no_engineering_changes_or_new_build_crypto_NTT_CI_external_contact": True,
        "original_archives_preserved": True,
    }
    (out / "BUILD_RESULT.json").write_bytes(json_bytes(result))
    print(json.dumps(result, indent=2))
    require(scan_ok, "secret scanner findings need explicit independent review; packet not upload-approved")


if __name__ == "__main__":
    main()
