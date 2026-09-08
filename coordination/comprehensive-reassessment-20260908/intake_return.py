"""Verify and retain the exact Pro return without executing returned code."""
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = Path('/Users/lifeng/Downloads/comprehensive-reassessment-3c02988-return.zip')
EXPECTED_SHA = 'b9792ca2ba8b466c0a082917b7f41e740b450ab22dc38d81e85522eee6abd15b'
helper = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
if hashlib.sha256(helper.read_bytes()).hexdigest() != 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814':
    raise RuntimeError('reviewed helper changed')
spec = importlib.util.spec_from_file_location('root_packet_checks', helper)
checks = importlib.util.module_from_spec(spec)
spec.loader.exec_module(checks)
require = checks.require
require(SOURCE.is_file() and not SOURCE.is_symlink(), 'download not regular')
raw = SOURCE.read_bytes()
require(len(raw) == 99809 and checks.sha256(raw) == EXPECTED_SHA, 'download identity')
with zipfile.ZipFile(io.BytesIO(raw)) as archive:
    infos = archive.infolist()
    names = [item.filename for item in infos]
    checks.validate_names(names)
    require(len(names) == 25, 'unexpected member count')
    require(sum(item.file_size for item in infos) == 242760, 'expanded size')
    for item in infos:
        require(not item.flag_bits & 1 and stat.S_ISREG(item.external_attr >> 16), 'encrypted/nonregular member')
    require(archive.testzip() is None, 'CRC mismatch')
    decoded = {name: archive.read(name) for name in names}
manifest = json.loads(decoded['MANIFEST.json'])
require(manifest['task'] == 'COMPREHENSIVE-REASSESSMENT-20260908', 'task identity')
require(manifest['source_commit'] == '3c02988fb5655dc6ea48f4f7d559e62d9d4a9d37', 'source identity')
require(manifest['input']['sha256'] == '08760af7def640afa06caf939c9ff7a5805d4cc6c567b49f24024cbf2f7c9158', 'input identity')
require(manifest['self_excluding'] is True and manifest['manifest_excluded_path'] == 'MANIFEST.json', 'self exclusion')
rows = manifest['files']
require(len(rows) == 24 and {row['path'] for row in rows} == set(names) - {'MANIFEST.json'}, 'manifest closure')
for row in rows:
    data = decoded[row['path']]
    require(len(data) == row['bytes'] and checks.sha256(data) == row['sha256'], 'member identity')
payloads = {name: {'bytes': data} for name, data in decoded.items()}
targeted = checks.targeted_content_scan(payloads, 'comprehensive reassessment return')
scan = checks.gitleaks_scan(payloads, 'comprehensive reassessment return')
destination = ROOT / 'artifacts/handoffs/comprehensive-reassessment-return-20260908' / SOURCE.name
unpacked = HERE / 'pro'
receipt = HERE / 'RETURN_INTAKE.json'
for path in (destination, unpacked, receipt):
    require(not path.exists() and not path.is_symlink(), 'refusing overwrite/repeat intake')
destination.parent.mkdir(parents=True, exist_ok=True)
require(not destination.parent.is_symlink(), 'symlink destination')
with destination.open('xb') as output:
    output.write(raw)
unpacked.mkdir()
for name, data in decoded.items():
    path = unpacked / name
    path.parent.mkdir(parents=True, exist_ok=True)
    require(not path.parent.is_symlink(), 'symlink unpacked parent')
    with path.open('xb') as output:
        output.write(data)
    require(path.read_bytes() == data, 'retained bytes changed')
result = {'archive': str(destination), 'bytes': len(raw), 'sha256': EXPECTED_SHA,
          'members': names, 'expanded_bytes': 242760,
          'manifest_sha256': checks.sha256(decoded['MANIFEST.json']),
          'targeted_scan': targeted, 'gitleaks_scan': scan,
          'closure_crc_paths_hashes': 'PASS', 'external_code_executed': False,
          'conversation': 'https://chatgpt.com/c/6a9f6a3f-1520-83ec-8c62-4d0231c2196c',
          'ui': '6 / Pro; highest visible power 5 of 5 before submission; backend unattested',
          'terminal_observed': '2026-09-08 10:56:30 Asia/Shanghai; Worked for 63m39s',
          'download_clicked_once': '2026-09-08T02:57:18.749Z'}
with receipt.open('x') as output:
    json.dump(result, output, indent=2)
    output.write('\n')
print(json.dumps(result, indent=2))
