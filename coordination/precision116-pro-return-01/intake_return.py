"""Verify and retain the exact terminal Pro draft; do not execute its contents."""
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
NAME = "experimental-precision116-profile-seam-red-green-draft-20260907.zip"
SOURCE = Path("/Users/lifeng/Downloads") / NAME
DEST = ROOT / "artifacts/handoffs/precision116-pro-return-01" / NAME
UNPACKED = Path(__file__).parent / "pro"
EXPECTED_SHA = "e516676dee09e3fb114fdd9536a0b580221eb2211ac8b56a144f44722560588b"
EXPECTED = {
    "DESIGN.md", "EXECUTION_LEDGER.md", "GREEN.patch", "RED.patch", "TEST_PLAN.md", "说明.md",
    "GREEN/include/openfhe_2023_1788/repeated_mult2.h", "GREEN/src/double_ckks.cpp",
    "GREEN/src/high_precision_client_io.cpp", "GREEN/src/repeated_mult2.cpp",
    "RED/CMakeLists.txt", "RED/tests/experimental_precision116_profile_seam.h",
    "RED/tests/paper_full_eight_square_contract_test.cpp", "checks/verify_draft.py",
    "results/environment.json", "results/static_checks.json", "MANIFEST.json",
}

# Reuse the fully root-reviewed local path/content and Gitleaks checks, not Pro code.
spec = importlib.util.spec_from_file_location(
    "local_packet_checks", ROOT / "coordination/precision116-pro-handoff-01/build_packet.py")
checks = importlib.util.module_from_spec(spec)
spec.loader.exec_module(checks)
require = checks.require

require(SOURCE.is_file() and not SOURCE.is_symlink(), "download is absent or nonregular")
raw = SOURCE.read_bytes()
require(len(raw) == 108234 and hashlib.sha256(raw).hexdigest() == EXPECTED_SHA,
        "download identity mismatch")
require(not DEST.exists() and not UNPACKED.exists(), "refusing duplicate/overwrite intake")
with zipfile.ZipFile(io.BytesIO(raw)) as archive:
    infos = archive.infolist()
    names = [info.filename for info in infos]
    checks.validate_names(names)
    require(set(names) == EXPECTED and len(names) == 17, "unexpected return inventory")
    require(archive.testzip() is None, "CRC failure")
    require(sum(info.file_size for info in infos) == 388322, "decoded byte count changed")
    for info in infos:
        require(not info.flag_bits & 1 and stat.S_ISREG(info.external_attr >> 16),
                "encrypted/nonregular return member")
    decoded = {name: archive.read(name) for name in names}

manifest = json.loads(decoded["MANIFEST.json"])
require(manifest["format"] == "self-excluding-sha256-v1"
        and manifest["self_excluded"] == "MANIFEST.json", "unexpected manifest schema")
rows = manifest["files"]
require(len(rows) == 16 and {row["path"] for row in rows} == EXPECTED - {"MANIFEST.json"},
        "manifest closure failure")
for row in rows:
    content = decoded[row["path"]]
    require(len(content) == row["bytes"] and checks.sha256(content) == row["sha256"],
            "manifest member identity mismatch")
payloads = {name: {"bytes": content} for name, content in decoded.items()}
targeted = checks.targeted_content_scan(payloads, "decoded Pro return")
scanner = checks.gitleaks_scan(payloads, "all decoded Pro return members")

DEST.parent.mkdir(parents=True, exist_ok=True)
with DEST.open("xb") as output:
    output.write(raw)
UNPACKED.mkdir()
for name, content in decoded.items():
    target = UNPACKED / name
    target.parent.mkdir(parents=True, exist_ok=True)
    with target.open("xb") as output:
        output.write(content)
    require(target.read_bytes() == content, "retained member bytes changed")
require(DEST.read_bytes() == raw, "retained archive bytes changed")
print(json.dumps({
    "source": str(SOURCE), "archive": str(DEST), "archive_bytes": len(raw),
    "archive_sha256": EXPECTED_SHA, "unpacked": str(UNPACKED),
    "members": len(decoded), "decoded_bytes": sum(map(len, decoded.values())),
    "manifest_sha256": checks.sha256(decoded["MANIFEST.json"]),
    "manifest": manifest, "targeted_scan": targeted, "gitleaks_scan": scanner,
    "crc_path_regular_member_closure_and_retained_byte_equality": "PASS",
    "pro_code_executed": False, "patches_applied": False,
}, indent=2, ensure_ascii=False))
