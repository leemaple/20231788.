"""Verify and retain one exact external review return, without executing its code."""
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = Path('/Users/lifeng/Downloads/annulus-independent-review-03f37b6.zip')
EXPECTED_SHA = 'badeeff902d1bdb36ae89e17ef33d845895eaebb17fd7d81cb87157cc97fd289'
helper = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
if hashlib.sha256(helper.read_bytes()).hexdigest() != 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814':
    raise RuntimeError('reviewed helper changed')
spec = importlib.util.spec_from_file_location('root_packet_checks', helper)
checks = importlib.util.module_from_spec(spec)
spec.loader.exec_module(checks)
require = checks.require
checks.ALLOWED_LOGS |= {'logs/' + name + '.log' for name in (
    'boundary_challenges', 'delivery_quality', 'independent_180', 'independent_230',
    'literal_paper_red', 'old_s100_180', 'old_s100_230', 'packet_and_process',
    'post_author_scalar', 'receiver_current_green', 'receiver_historical_red', 'supplied_replay_230')}
require(SOURCE.is_file() and not SOURCE.is_symlink(), 'download not regular')
raw = SOURCE.read_bytes()
require(len(raw) == 139928 and checks.sha256(raw) == EXPECTED_SHA, 'download identity')
with zipfile.ZipFile(io.BytesIO(raw)) as archive:
    infos = archive.infolist()
    names = [item.filename for item in infos]
    checks.validate_names(names)
    require(len(names) == 44, 'unexpected member count')
    require(sum(item.file_size for item in infos) == 361832, 'expanded size')
    for item in infos:
        require(not item.flag_bits & 1 and stat.S_ISREG(item.external_attr >> 16), 'encrypted/nonregular member')
    require(archive.testzip() is None, 'CRC mismatch')
    decoded = {name: archive.read(name) for name in names}
manifest = json.loads(decoded['MANIFEST.json'])
require(manifest['schema'] == 'annulus-independent-review-delivery-v1', 'schema identity')
require(manifest['source_binding']['experiment_source_commit'] == '03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b', 'source identity')
require(manifest['source_binding']['task_evidence_commit'] == '695a951a7379d355cf9d167cd14c9a17d9d25199', 'evidence identity')
require(manifest['source_binding']['official_openfhe_pin'] == 'df495ba2e91739a6dc8f1de254fc5a41155ce504', 'official pin')
require(manifest['input_archive']['sha256'] == 'c30b6e84a53712792cfda4ec17da7b434ee745aebad3949916a90fa6d528a0b1', 'input identity')
require(manifest['manifest_self_excluded'] is True, 'self exclusion')
rows = manifest['files']
require(len(rows) == 43 and {row['path'] for row in rows} == set(names) - {'MANIFEST.json'}, 'manifest closure')
for row in rows:
    data = decoded[row['path']]
    require(len(data) == row['bytes'] and checks.sha256(data) == row['sha256'], 'member identity')
require(manifest['first_pass']['sha256'] == checks.sha256(decoded['FIRST_PASS.md']), 'first pass byte binding')
payloads = {name: {'bytes': data} for name, data in decoded.items()}
targeted = checks.targeted_content_scan(payloads, 'independent annulus semantic review return')
scan = checks.gitleaks_scan(payloads, 'independent annulus semantic review return')
destination = ROOT / 'artifacts/handoffs/annulus-independent-review-return-20260908' / SOURCE.name
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
          'members': names, 'expanded_bytes': 361832,
          'manifest_sha256': checks.sha256(decoded['MANIFEST.json']),
          'targeted_scan': targeted, 'gitleaks_scan': scan,
          'closure_crc_paths_hashes': 'PASS', 'external_code_executed': False,
          'conversation': 'https://chatgpt.com/c/6a9f8aa6-eb78-83ec-aeb9-879aa5bf1b2e',
          'ui': '6 / Pro; highest visible power5of5 before submission; backend unattested',
          'terminal_observed': '2026-09-08 13:00:00.747 Asia/Shanghai; Worked for48m44s',
          'download_clicked_once': '2026-09-08T05:00:39.662Z'}
with receipt.open('x') as output:
    json.dump(result, output, indent=2)
    output.write('\n')
print(json.dumps(result, indent=2))
