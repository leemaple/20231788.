"""Verify the immutable independent Pro review; never execute returned content."""
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).parent
SOURCE = Path('/Users/lifeng/Downloads/S100-INDEPENDENT-SEMANTIC-REVIEW-01-a448b78-zh.zip')
EXPECTED_SHA = '753f64b15e68371444bfcc5bd035c41bdf0f06ce9abccefd307355cdd82ae6d1'
EXPECTED = {'REVIEW.md', 'CLAIM_BOUNDARY.md', 'NEXT_STEP.md', 'MANIFEST.json'}
helper = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
if hashlib.sha256(helper.read_bytes()).hexdigest() != 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814':
    raise RuntimeError('reviewed helper changed')
spec = importlib.util.spec_from_file_location('root_packet_checks', helper)
checks = importlib.util.module_from_spec(spec)
spec.loader.exec_module(checks)
require = checks.require
require(SOURCE.is_file() and not SOURCE.is_symlink(), 'download not regular')
raw = SOURCE.read_bytes()
require(len(raw) == 34056 and checks.sha256(raw) == EXPECTED_SHA, 'download identity')
with zipfile.ZipFile(io.BytesIO(raw)) as archive:
    infos = archive.infolist()
    names = [item.filename for item in infos]
    checks.validate_names(names)
    require(len(names) == 4 and set(names) == EXPECTED, 'unexpected inventory')
    require(sum(item.file_size for item in infos) == 65684, 'expanded size')
    for item in infos:
        require(not item.flag_bits & 1 and stat.S_ISREG(item.external_attr >> 16), 'encrypted/nonregular member')
    require(archive.testzip() is None, 'CRC mismatch')
    decoded = {name: archive.read(name) for name in names}
manifest = json.loads(decoded['MANIFEST.json'])
require(manifest['task'] == 'S100-INDEPENDENT-SEMANTIC-REVIEW-01', 'task identity')
require(manifest['source_commit_as_supplied'] == 'a448b787399b43b6024d82c170add403969b493c', 'source identity')
require(manifest['input_archive']['sha256'] == '2ca65c697e57eec29e29afa89a45172855aeb4f70a367c1260c7fbff710bf401', 'input identity')
rows = manifest['files']
require(len(rows) == 3 and {row['path'] for row in rows} == EXPECTED - {'MANIFEST.json'}, 'manifest closure')
for row in rows:
    data = decoded[row['path']]
    require(len(data) == row['bytes'] and checks.sha256(data) == row['sha256'], 'member identity')
payloads = {name: {'bytes': data} for name, data in decoded.items()}
targeted = checks.targeted_content_scan(payloads, 'independent semantic review return')
scan = checks.gitleaks_scan(payloads, 'independent semantic review return')
destination = ROOT / 'artifacts/handoffs/s100-independent-semantic-review-return-01' / SOURCE.name
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
          'members': names, 'expanded_bytes': 65684,
          'manifest_sha256': checks.sha256(decoded['MANIFEST.json']),
          'targeted_scan': targeted, 'gitleaks_scan': scan,
          'closure_crc_paths_hashes': 'PASS', 'external_code_executed': False}
with receipt.open('x') as output:
    json.dump(result, output, indent=2)
    output.write('\n')
print(json.dumps(result, indent=2))
