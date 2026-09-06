"""Retain the exact terminal draft without executing external code or patching source."""
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
NAME = "experimental-precision116-eight-square-01-return-2759fa90.zip"
SOURCE = Path("/Users/lifeng/Downloads") / NAME
DEST = ROOT / "artifacts/handoffs/precision116-eight-square-return-01" / NAME
UNPACKED = Path(__file__).parent / "pro"
SHA = "5849c48518791fe52dff44ae6c28bd13ebcf7328c3fd02ae9af23596b7e05b45"
EXPECTED = {
    "COMMANDS.md", "DESIGN_AND_ORACLE.md", "EXECUTION_LEDGER.md",
    "INDEPENDENT_REVIEW.md", "MANIFEST.json", "README.md",
    "checks/CONFIGURE_ATTEMPT.txt", "checks/RETURN_SCREENING.txt",
    "checks/STATIC_CHECKS.txt", "files/CMakeLists.txt",
    "files/tests/experimental_precision116_eight_square_test.cpp",
    "patches/precision116-eight-square.patch",
}

helper = ROOT / "coordination/precision116-pro-handoff-01/build_packet.py"
if hashlib.sha256(helper.read_bytes()).hexdigest() != "f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814":
    raise RuntimeError("reviewed local helper identity changed")
spec = importlib.util.spec_from_file_location("root_packet_checks", helper)
checks = importlib.util.module_from_spec(spec)
spec.loader.exec_module(checks)
require = checks.require

require(SOURCE.is_file() and not SOURCE.is_symlink(), "absent/nonregular download")
raw = SOURCE.read_bytes()
require(len(raw) == 48984 and checks.sha256(raw) == SHA, "download identity changed")
require(not DEST.exists() and not DEST.is_symlink(), "archive already retained")
require(not UNPACKED.exists() and not UNPACKED.is_symlink(), "refusing repeat extraction")
with zipfile.ZipFile(io.BytesIO(raw)) as archive:
    infos = archive.infolist()
    names = [info.filename for info in infos]
    checks.validate_names(names)
    require(len(names) == 12 and set(names) == EXPECTED, "unexpected archive inventory")
    require(sum(info.file_size for info in infos) == 146986, "decoded size changed")
    for info in infos:
        require(not info.flag_bits & 1 and stat.S_ISREG(info.external_attr >> 16),
                "encrypted or nonregular member")
    require(archive.testzip() is None, "CRC mismatch")
    decoded = {name: archive.read(name) for name in names}

manifest = json.loads(decoded["MANIFEST.json"])
require(manifest["format_version"] == 1 and manifest["self_excluding"] is True,
        "manifest format changed")
require(manifest["task"] == "EXPERIMENTAL-PRECISION116-EIGHT-SQUARE-01"
        and manifest["source_commit"] == "2759fa90840946ef42957c7ba71ebea47e0e4995"
        and manifest["profile"] == "experimental-s116-d56-b58-v1", "source/task binding changed")
rows = manifest["files"]
require(manifest["payload_count"] == len(rows) == 11
        and {row["path"] for row in rows} == EXPECTED - {"MANIFEST.json"},
        "manifest closure changed")
for row in rows:
    content = decoded[row["path"]]
    require(len(content) == row["bytes"] and checks.sha256(content) == row["sha256"],
            "manifest member identity mismatch")
payloads = {name: {"bytes": content} for name, content in decoded.items()}
targeted = checks.targeted_content_scan(payloads, "decoded eight-square Pro return")
scan = checks.gitleaks_scan(payloads, "all decoded eight-square Pro return members")

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
    "archive_sha256": SHA, "unpacked": str(UNPACKED), "members": len(decoded),
    "decoded_bytes": sum(map(len, decoded.values())),
    "manifest_sha256": checks.sha256(decoded["MANIFEST.json"]), "manifest": manifest,
    "targeted_scan": targeted, "gitleaks_scan": scan,
    "crc_paths_regular_members_closure_and_retained_byte_equality": "PASS",
    "external_code_executed": False, "patch_applied": False,
}, indent=2))
