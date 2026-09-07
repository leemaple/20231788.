"""Validate and retain this exact Pro return; never execute external code."""
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).parent
SOURCE = Path('/Users/lifeng/Downloads/s100-fresh-error-repair-01-delivery.zip')
EXPECTED_SHA = '5ea38bfa16c18b187bbf808604b2b5d2c74ae79b26eb3e8e23c4968ad1004637'
EXPECTED = {'01-red.patch', '02-green.patch', 'DESIGN.md', 'MANIFEST.json',
            'STATIC_CHECKS.json', 'modified/CMakeLists.txt',
            'modified/include/openfhe_2023_1788/high_precision_client_io.h',
            'modified/src/high_precision_client_io.cpp',
            'modified/tests/s100_fresh_error_diagnostic_test.cpp'}

helper = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
if hashlib.sha256(helper.read_bytes()).hexdigest() != 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814':
    raise RuntimeError('reviewed helper changed')
spec = importlib.util.spec_from_file_location('root_packet_checks', helper)
checks = importlib.util.module_from_spec(spec)
spec.loader.exec_module(checks)
require = checks.require
require(SOURCE.is_file() and not SOURCE.is_symlink(), 'download not regular')
raw = SOURCE.read_bytes()
require(len(raw) == 58724 and checks.sha256(raw) == EXPECTED_SHA, 'download identity')
with zipfile.ZipFile(io.BytesIO(raw)) as archive:
    infos = archive.infolist()
    names = [item.filename for item in infos]
    checks.validate_names(names)
    require(len(names) == 9 and set(names) == EXPECTED, 'unexpected inventory')
    require(sum(item.file_size for item in infos) == 195060, 'expanded size')
    for item in infos:
        require(not item.flag_bits & 1 and stat.S_ISREG(item.external_attr >> 16), 'encrypted/nonregular member')
    require(archive.testzip() is None, 'CRC mismatch')
    decoded = {name: archive.read(name) for name in names}
manifest = json.loads(decoded['MANIFEST.json'])
require(checks.sha256(decoded['MANIFEST.json']) == '2096026faf6cc952469b6aa7bd65baba13b8583d9f8e5409d7b65d89e9656552', 'manifest identity')
require(manifest['task_id'] == 'S100-FRESH-ERROR-REPAIR-01' and manifest['input']['declared_source_commit'] == 'e6c4cc1ddfa261ee17681cbfc1ca6023660df5cb', 'task/source identity')
rows = manifest['files']
require(len(rows) == 8 and {row['path'] for row in rows} == EXPECTED - {'MANIFEST.json'}, 'manifest closure')
for row in rows:
    data = decoded[row['path']]
    require(len(data) == row['bytes'] and checks.sha256(data) == row['sha256'], 'member identity')
payloads = {name: {'bytes': data} for name, data in decoded.items()}
targeted = checks.targeted_content_scan(payloads, 'decoded S100 Pro return')
scan = checks.gitleaks_scan(payloads, 'decoded S100 Pro return')
destination = ROOT / 'artifacts/handoffs/s100-fresh-error-repair-return-01' / SOURCE.name
unpacked = HERE / 'pro'
receipt = HERE / 'RETURN_INTAKE.json'
for path in (destination, unpacked, receipt):
    require(not path.exists() and not path.is_symlink(), 'refusing overwrite/repeat intake')
destination.parent.mkdir(parents=True, exist_ok=True)
with destination.open('xb') as output:
    output.write(raw)
unpacked.mkdir()
for name, data in decoded.items():
    path = unpacked / name
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('xb') as output:
        output.write(data)
    require(path.read_bytes() == data, 'retained bytes changed')
result = {'source': str(SOURCE), 'archive': str(destination), 'bytes': len(raw),
          'sha256': EXPECTED_SHA, 'members': names, 'expanded_bytes': 195060,
          'manifest_sha256': checks.sha256(decoded['MANIFEST.json']),
          'targeted_scan': targeted, 'gitleaks_scan': scan,
          'closure_crc_paths_hashes': 'PASS', 'external_code_executed': False,
          'patches_applied': False}
with receipt.open('x') as output:
    json.dump(result, output, indent=2)
    output.write('\n')
print(json.dumps(result, indent=2))
