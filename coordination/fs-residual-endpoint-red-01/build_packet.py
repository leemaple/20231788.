#!/usr/bin/env python3
"""Build a fixed-source, nested-context RED handoff; no project code execution."""
import hashlib
import io
import json
import os
from pathlib import Path, PurePosixPath
import stat
import subprocess
import zipfile

ROOT = Path('/Users/lifeng/Documents/20231788-openfhe-paper-scale-implementation-20260905')
BASE = ROOT / 'artifacts/handoffs/fs-residual-endpoint-red-01'
HEAD = 'f1c33b7fdcc12741f40b96164c87a35f90090345'
SOURCE = '9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e'
DEST = BASE / 'fs-residual-endpoint-red-9f6c8eae-v2.zip'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def git(*args):
    return subprocess.check_output(['git', *args], cwd=ROOT)


def decode(raw, manifest_path):
    with zipfile.ZipFile(io.BytesIO(raw)) as archive:
        infos = archive.infolist()
        names = [i.filename for i in infos]
        assert len(names) == len(set(names)) == len({n.casefold() for n in names})
        for item in infos:
            name = item.filename
            path = PurePosixPath(name)
            assert not path.is_absolute() and '..' not in path.parts
            assert '\\' not in name and ':' not in name and path.as_posix() == name
            assert not item.flag_bits & 1 and stat.S_ISREG(item.external_attr >> 16)
            assert not any(p in {'.git', 'node_modules', '__pycache__', '.env'} for p in path.parts)
            assert path.suffix not in {'.pem', '.key', '.p12', '.pfx'}
        assert archive.testzip() is None
        data = {name: archive.read(name) for name in names}
    manifest = json.loads(data[manifest_path])
    parent = PurePosixPath(manifest_path).parent
    rows = manifest['files']
    assert manifest['manifest_self_excluded'] is True
    assert {str(parent/r['path']) for r in rows} == set(data)-{manifest_path}
    assert len(rows) == len(data)-1
    for row in rows:
        blob = data[str(parent/row['path'])]
        assert len(blob) == row['bytes'] and sha(blob) == row['sha256']
    return data


def scan(data):
    blocks = []
    for name, blob in sorted(data.items()):
        blocks.append(b'FILE '+name.encode()+b'\n'+blob+b'\n')
        if name.startswith('input/') and name.endswith('.zip'):
            nested = decode(blob, 'MANIFEST.json')
        elif name.startswith('review/') and name.endswith('.zip'):
            nested = decode(blob, 'precision_adjudication_decision_v1/MANIFEST.json')
        else:
            nested = {}
        for inner, value in sorted(nested.items()):
            blocks.append(b'NESTED '+name.encode()+b'/'+inner.encode()+b'\n'+value+b'\n')
    payload = b''.join(blocks)
    env = os.environ.copy()
    for name in ['GITLEAKS_CONFIG', 'GITLEAKS_CONFIG_TOML']:
        env.pop(name, None)
    env['GOMAXPROCS'] = '2'
    version_command = ['/opt/homebrew/bin/gitleaks', 'version']
    version = subprocess.check_output(version_command, cwd='/private/tmp', env=env).decode().strip()
    assert version == '8.30.1', version
    command = ['/opt/homebrew/bin/gitleaks', 'stdin', '--ignore-gitleaks-allow',
               '--gitleaks-ignore-path', '/dev/null', '--max-decode-depth', '5',
               '--max-archive-depth', '1', '--redact', '--no-banner', '--no-color',
               '--report-format', 'json', '--report-path', '-']
    run = subprocess.run(command, input=payload, capture_output=True, cwd='/private/tmp', env=env)
    assert run.returncode == 0, run.stdout.decode()
    assert json.loads(run.stdout) == []
    return {'command': command, 'version_command': version_command,
            'tool_version': version, 'exit': 0, 'findings': [],
            'framed_bytes': len(payload), 'framed_sha256': sha(payload)}


assert git('rev-parse', 'HEAD').decode().strip() == HEAD
assert git('status', '--porcelain=v1') == b''
assert not DEST.exists()
original_path = ROOT/'artifacts/handoffs/paper-scale-precision-adjudication-01/paper-scale-precision-adjudication-9f6c8eae.zip'
original = original_path.read_bytes()
assert len(original) == 2046500 and sha(original) == '1584a5b7362c9568d3f8f7fa8acfea9f28a4a934cabe2dcdd283aa6f02e9b7da'
original_data = decode(original, 'MANIFEST.json')
assert len(original_data) == 154
assert sha(original_data['MANIFEST.json']) == 'd19303d2d6fd5364f14c3076ed2d3f306a6e90da4d11d257deab0e6b1e7e2c98'
project_paths = []
for name, blob in original_data.items():
    if name.startswith('project/'):
        path = name[len('project/'):]
        assert blob == git('show', SOURCE+':'+path) == git('show', HEAD+':'+path), path
        project_paths.append(path)
assert len(project_paths) == 44
assert not git('diff', 'b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89', HEAD, '--', 'src', 'include')
return_prefix = 'coordination/paper-scale-precision-adjudication-return-01/'
returned = git('show', HEAD+':'+return_prefix+'paper-scale-precision-adjudication-decision-v1.zip')
assert len(returned) == 60905 and sha(returned) == 'f20f89a67e233cd9bc39553dd20b923dbc663c2939dc79dd0fafd756ec58fba5'
assert len(decode(returned, 'precision_adjudication_decision_v1/MANIFEST.json')) == 9
data = {'TASK.md': (BASE/'TASK.md').read_bytes(),
        'input/paper-scale-precision-adjudication-9f6c8eae.zip': original,
        'review/paper-scale-precision-adjudication-decision-v1.zip': returned}
for name in ['ROOT_DECISION.md', 'ASTRA_ADJUDICATION_REVIEW.md',
             'SOL_ADJUDICATION_REVIEW.md', 'OBSERVER_ALLOWANCE_PROPOSAL.md',
             'ENDPOINT_EVIDENCE_PROPOSAL.md', 'ROOT_REEXECUTION.json',
             'RECEIVE_PREFLIGHT.json', 'RETURN_RECEIPT.json']:
    data['context/'+name] = git('show', HEAD+':'+return_prefix+name)
selection_scan = scan(data)
manifest = {'schema': 'fs-residual-endpoint-red-input-v1', 'manifest_self_excluded': True,
            'branch': 'codex/paper-scale-implementation-20260905',
            'documentation_head': HEAD, 'tested_source': SOURCE, 'dirty_state': 'clean',
            'nested_project_files_compared_to_both_commits': project_paths,
            'nested_archives': {'input/paper-scale-precision-adjudication-9f6c8eae.zip': 154,
                                'review/paper-scale-precision-adjudication-decision-v1.zip': 9},
            'files': [{'path': p, 'bytes': len(b), 'sha256': sha(b)} for p,b in sorted(data.items())]}
data['MANIFEST.json'] = (json.dumps(manifest, indent=2)+'\n').encode()
with zipfile.ZipFile(DEST, 'x', compression=zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
    for name, blob in sorted(data.items()):
        info = zipfile.ZipInfo(name, date_time=(2026, 9, 6, 0, 0, 0))
        info.create_system = 3
        info.external_attr = (stat.S_IFREG | 0o644) << 16
        info.compress_type = zipfile.ZIP_DEFLATED
        archive.writestr(info, blob)
archive_bytes = DEST.read_bytes()
decoded = decode(archive_bytes, 'MANIFEST.json')
assert decoded == data
archive_scan = scan(decoded)
assert git('status', '--porcelain=v1') == b''
print(json.dumps({'archive_absolute_path': str(DEST), 'archive_bytes': len(archive_bytes),
                  'archive_sha256': sha(archive_bytes), 'regular_members': len(data),
                  'manifest_sha256': sha(data['MANIFEST.json']), 'manifest': manifest,
                  'selection_scan': selection_scan, 'archive_scan': archive_scan}, indent=2))
