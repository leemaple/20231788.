"""Retain a bounded, verified Pro decision archive; execute no returned code."""
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).parent
SOURCE = Path('/Users/lifeng/Downloads/S100-CONDITION-DECISION-01-2c14d7f-zh.zip')
EXPECTED_SHA = '6edfbe9c65f6e88dd10561661d07f55d50d00aab829c075f73e3b4c497bbeded'
EXPECTED = {'DECISION.md', 'NEXT_ACTION.md', 'MANIFEST.json'}
helper = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
if hashlib.sha256(helper.read_bytes()).hexdigest() != 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814':
    raise RuntimeError('reviewed helper changed')
spec = importlib.util.spec_from_file_location('root_packet_checks', helper)
checks = importlib.util.module_from_spec(spec)
spec.loader.exec_module(checks)
require = checks.require
require(SOURCE.is_file() and not SOURCE.is_symlink(), 'download not regular')
raw = SOURCE.read_bytes()
require(len(raw) == 28223 and checks.sha256(raw) == EXPECTED_SHA, 'download identity')
with zipfile.ZipFile(io.BytesIO(raw)) as archive:
    infos = archive.infolist()
    names = [item.filename for item in infos]
    checks.validate_names(names)
    require(len(names) == 3 and set(names) == EXPECTED, 'unexpected inventory')
    require(sum(item.file_size for item in infos) == 65866, 'expanded size')
    for item in infos:
        require(not item.flag_bits & 1 and stat.S_ISREG(item.external_attr >> 16), 'encrypted/nonregular member')
    require(archive.testzip() is None, 'CRC mismatch')
    decoded = {name: archive.read(name) for name in names}
manifest = json.loads(decoded['MANIFEST.json'])
require(manifest['task'] == 'S100-CONDITION-DECISION-01', 'task identity')
require(manifest['source_commit'] == '2c14d7f394ec385029716d00f9c03fad974ba88b', 'source identity')
require(manifest['input_archive']['verification']['archive_sha256'] == '3bbb738d56a17f76807546f5f7fc4a1f4c83e889164e1dab71f0409d190e417b', 'input identity')
rows = manifest['files']
require(len(rows) == 2 and {row['path'] for row in rows} == EXPECTED - {'MANIFEST.json'}, 'manifest closure')
for row in rows:
    data = decoded[row['path']]
    require(len(data) == row['bytes'] and checks.sha256(data) == row['sha256'], 'member identity')
payloads = {name: {'bytes': data} for name, data in decoded.items()}
targeted = checks.targeted_content_scan(payloads, 'scientific condition decision return')
scan = checks.gitleaks_scan(payloads, 'scientific condition decision return')
destination = ROOT / 'artifacts/handoffs/s100-condition-decision-return-01' / SOURCE.name
unpacked = HERE / 'pro'
receipt = HERE / 'RETURN_INTAKE.json'
for path in (destination, unpacked, receipt):
    require(not path.exists() and not path.is_symlink(), 'refusing overwrite/repeat intake')
destination.parent.mkdir(parents=True, exist_ok=True)
with destination.open('xb') as output:
    output.write(raw)
unpacked.mkdir()
for name, data in decoded.items():
    with (unpacked / name).open('xb') as output:
        output.write(data)
    require((unpacked / name).read_bytes() == data, 'retained bytes changed')
result = {'archive': str(destination), 'bytes': len(raw), 'sha256': EXPECTED_SHA,
          'members': names, 'expanded_bytes': 65866,
          'manifest_sha256': checks.sha256(decoded['MANIFEST.json']),
          'targeted_scan': targeted, 'gitleaks_scan': scan,
          'closure_crc_paths_hashes': 'PASS', 'external_code_executed': False}
with receipt.open('x') as output:
    json.dump(result, output, indent=2)
    output.write('\n')
print(json.dumps(result, indent=2))
