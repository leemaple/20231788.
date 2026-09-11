"""Verify and retain this one exact Pro return; never execute returned files."""
import hashlib
import json
from pathlib import Path, PurePosixPath
import re
import stat
import zipfile


ROOT = Path(__file__).resolve().parent
ARCHIVE = Path('/Users/lifeng/Downloads/testing-methods-diagnosis-20260911.zip')
SHA = '60f08159f81178ced012244e1da7d1e76a21045c27a33994756c19e17f666903'
EXPECTED = {
    'CMakeLists.append.txt', 'COVERAGE.md', 'DIAGNOSIS.md',
    'EXECUTION_LEDGER.md', 'MANIFEST.json', 'MUTATION_PLAN.md',
    'PATCH_CHECKS.json', 'SOURCES.md', 'STATIC_CHECKS.json', 'TEST_PLAN.md',
    'initial-phase.patch', 'tests/initial_phase_exact_contract_test.cpp',
    'tools/verify_static.py',
}


def require(ok, label):
    if not ok:
        raise SystemExit(label)


def digest(data):
    return hashlib.sha256(data).hexdigest()


require(ARCHIVE.is_file() and not ARCHIVE.is_symlink(), 'archive must be regular')
raw = ARCHIVE.read_bytes()
require(len(raw) == 42319 and digest(raw) == SHA, 'archive identity mismatch')
with zipfile.ZipFile(ARCHIVE) as archive:
    entries = archive.infolist()
    names = [e.filename for e in entries]
    require(len(names) == len(set(names)) == len(EXPECTED), 'member count/duplicate')
    require(set(names) == EXPECTED, 'unexpected members')
    require(archive.testzip() is None, 'CRC mismatch')
    data = {}
    for entry in entries:
        name = entry.filename
        path = PurePosixPath(name)
        mode = entry.external_attr >> 16
        require(not path.is_absolute() and '..' not in path.parts and '\\' not in name,
                'unsafe path')
        require(not entry.is_dir() and not (entry.flag_bits & 1), 'non-file/encryption')
        require(stat.S_IFMT(mode) in (0, stat.S_IFREG), 'non-regular member')
        require(entry.file_size < 100000, 'member too large')
        data[name] = archive.read(entry)
        text = data[name].decode('utf-8')
        require('\0' not in text, 'unexpected NUL')
        require(not re.search(r'-----BEGIN (?:RSA |EC |OPENSSH )?PRIVATE KEY-----', text),
                'private key material')
        require(not re.search(r'\b(?:ghp_|github_pat_|AKIA)[A-Za-z0-9_]{16,}', text),
                'credential-shaped material')

manifest = json.loads(data['MANIFEST.json'])
require(manifest['source_commit'] == '33722b9c9ba9d32b36e672ee7cdaa3c3fb2a95d1',
        'wrong source base')
require(manifest['official_pin'] == 'df495ba2e91739a6dc8f1de254fc5a41155ce504',
        'wrong official pin')
require(manifest['input_zip']['sha256'] ==
        '572cf0db6765dfbb1c8ee658d2425fc8e27933a540adf7b1d1145310e50de809',
        'wrong handoff')
records = manifest['files']
require(len(records) == 12 and len({r['path'] for r in records}) == 12,
        'manifest record count')
require({r['path'] for r in records} == EXPECTED - {'MANIFEST.json'},
        'manifest closure')
for record in records:
    payload = data[record['path']]
    require(len(payload) == record['bytes'] and digest(payload) == record['sha256'],
            'manifest identity mismatch: ' + record['path'])

destination = ROOT / 'pro'
require(not destination.exists() and not destination.is_symlink(), 'refuse overwrite')
destination.mkdir(mode=0o700)
for name, payload in data.items():
    target = destination / name
    target.parent.mkdir(parents=True, exist_ok=True)
    with target.open('xb') as out:
        out.write(payload)

receipt = {
    'archive': str(ARCHIVE), 'archive_bytes': len(raw), 'archive_sha256': SHA,
    'regular_members': len(entries), 'crc_paths_manifest_hashes': 'PASS',
    'targeted_decoded_scan': 'PASS', 'gitleaks': 'PENDING',
    'destination': str(destination), 'returned_code_executed': False,
    'input_zip_sha256': manifest['input_zip']['sha256'],
    'source_commit': manifest['source_commit'], 'official_pin': manifest['official_pin'],
    'files': [{'path': name, 'bytes': len(payload), 'sha256': digest(payload)}
              for name, payload in sorted(data.items())],
}
with (ROOT / 'RETURN_INTAKE.json').open('x') as out:
    json.dump(receipt, out, indent=2)
    out.write('\n')
print(json.dumps({k: v for k, v in receipt.items() if k != 'files'}, indent=2))
