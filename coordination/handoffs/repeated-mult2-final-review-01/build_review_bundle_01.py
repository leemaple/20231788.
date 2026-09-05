#!/usr/bin/env python3
"""Build a bounded read-only review packet; never build, test, upload, or dispatch."""
import argparse
import csv
import difflib
import hashlib
import io
import json
from pathlib import Path, PurePosixPath
import re
import stat
import subprocess
import zipfile

REPO = Path('/Users/lifeng/Documents/20231788-openfhe-repeated-semantic-20260905')
OFFICIAL = Path('/private/tmp/h128-pro-packet.Lo406g/official-source')
SOURCE = 'd09f15f535f0dbf22ef89b33255e947166cc392a'
EVIDENCE = '019588513452c5e153d891cf7d787555a7a0c013'
RED = '7399db55b799a166aee9b72b8f89bcded373b540'
BASE = '80d771c52df10bce1c60992b5e0edb4e64f145ca'
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
INPUT = Path('/private/tmp/repeated-mult2-semantic-implementation-01-80d771c.zip')
RETURN = Path('/Users/lifeng/Downloads/repeated-mult2-semantic-candidate-80d771c.zip')
INPUT_SHA = '764baddb20d81c1168745ac31eb043d0d94cf1ba6b406d0194f9245a994196a2'
RETURN_SHA = '77d32a3d28b528722efa59633feb7225cb813e68092023fbd462f0d4d318fec5'
HANDOFF = 'coordination/handoffs/repeated-mult2-semantic-pro-packet-01/'
RETURN_TREE = 'coordination/returns/repeated-mult2-semantic-candidate-80d771c/'
ROOT = 'repeated-mult2-final-review-d09f15f'
ENGINEERING = ['CMakeLists.txt', 'include/openfhe_2023_1788/double_ckks.h',
               'include/openfhe_2023_1788/repeated_mult2.h', 'src/double_ckks.cpp', 'src/repeated_mult2.cpp']
EVIDENCE_NAMES = '''SOURCE_PROVENANCE.json RETURN_RECEIPT_01.md ROOT_OFFLINE_VERIFICATION_01.json
CODEX_PRE_HOSTED_REVIEW_01.md INTEGRATED_RED_GATE_01.md REPEATED_PRE_GREEN_GUARD_01.md
REPEATED_RED_RUN_7399db5.json REPEATED_RED_FAILURE_7399db5.txt REPEATED_RED_CHECKPOINT_7399db5.txt
HOSTED_RED_RESULT_01.md HOSTED_RED_ACCEPTANCE_01.json CORRECTED_GREEN_PREFLIGHT_01.json
CORRECTED_GREEN_INTEGRATION_01.md DUAL_HOST_GREEN_RESULT_01.md GREEN_RUN_d09f15f.json
GREEN_LINUX_VERIFICATION_d09f15f.json GREEN_WINDOWS_VERIFICATION_d09f15f.json
GREEN_LINUX_TEST_EXCERPT_d09f15f.txt GREEN_STANDARDS_REVIEW_01.md GREEN_REVIEW_DISPOSITION_01.md
verify_hosted_green_01.py'''.split()
BLOCKED = {'.git', 'node_modules', 'build', 'out', '.cache', '__pycache__', '.ssh',
           'cookies', 'credentials', 'local storage', 'session storage', 'browser', 'browser-state'}
SECRET = re.compile(rb'-----BEGIN (?:RSA |EC |OPENSSH |DSA )?PRIVATE KEY-----|'
                    rb'gh[pousr]_[A-Za-z0-9]{30,}|github_pat_[A-Za-z0-9_]{40,}|'
                    rb'AKIA[0-9A-Z]{16}|sk-(?:proj-)?[A-Za-z0-9_-]{40,}')
payload, origins, folded = {}, {}, set()

def require(condition, message):
    if not condition:
        raise ValueError(message)

def sha(data):
    return hashlib.sha256(data).hexdigest()

def git(*args, repo=REPO):
    return subprocess.check_output(['git', '-C', str(repo), *args])

def safe(name):
    parts = PurePosixPath(name).parts
    require(bool(parts) and not name.startswith('/') and '\\' not in name and ':' not in name,
            'Unsafe member path: ' + name)
    require(all(p not in ('', '.', '..') and not any(ord(c) < 32 for c in p) for p in name.split('/')),
            'Unsafe member component: ' + name)
    require(not any(p.casefold() in BLOCKED or p.casefold().startswith('.env') for p in parts),
            'Excluded member class: ' + name)
    require(PurePosixPath(name).suffix.casefold() not in {'.zip', '.db', '.sqlite', '.sqlite3', '.pem', '.p12', '.pfx'},
            'Excluded member suffix: ' + name)

def add(name, data, origin):
    safe(name)
    require(name.casefold() not in folded, 'Duplicate/casefold member: ' + name)
    require(len(data) <= 5_000_000 and not SECRET.search(data), 'Size or targeted credential gate: ' + name)
    folded.add(name.casefold())
    payload[name], origins[name] = data, origin

def add_git(name, commit, path, repo=REPO):
    mode = git('ls-tree', commit, '--', path, repo=repo).split(maxsplit=1)[0]
    require(mode == b'100644', 'Non-regular/nonstandard Git blob mode: ' + path)
    data = git('show', commit + ':' + path, repo=repo)
    add(name, data, {'kind': 'exact-git-blob', 'commit': commit, 'path': path,
                    'blob_sha1': git('rev-parse', commit + ':' + path, repo=repo).decode().strip()})

def read_archive(path, expected, root, destination, count):
    require(path.is_file() and not path.is_symlink(), 'Missing/nonregular archive: ' + str(path))
    raw = path.read_bytes()
    require(sha(raw) == expected, 'Original archive SHA mismatch')
    files, seen, total = {}, set(), 0
    with zipfile.ZipFile(io.BytesIO(raw)) as archive:
        require(len(archive.infolist()) <= 200, 'Too many original archive entries')
        for entry in archive.infolist():
            require(entry.filename.startswith(root + '/'), 'Original archive root mismatch')
            name = entry.filename.rstrip('/')
            safe(name)
            require(name.casefold() not in seen and not entry.flag_bits & 1, 'Duplicate/encrypted original member')
            seen.add(name.casefold())
            mode = stat.S_IFMT(entry.external_attr >> 16)
            require(mode in (0, stat.S_IFDIR if entry.is_dir() else stat.S_IFREG), 'Special original member')
            total += entry.file_size
            require(entry.file_size <= 5_000_000 and total <= 10_000_000, 'Original archive size ceiling')
            if not entry.is_dir():
                relative = entry.filename[len(root) + 1:]
                files[relative] = archive.read(entry)  # Verifies each member CRC.
        require(archive.testzip() is None and len(files) == count, 'CRC/member count mismatch')
    rows = list(csv.DictReader(io.StringIO(files['MANIFEST.tsv'].decode()), delimiter='\t'))
    declared = [row['path'] for row in rows]
    require(len(set(declared)) == len(declared), 'Duplicate original manifest entry')
    require(set(declared) == set(files) - {'MANIFEST.tsv', 'MANIFEST.sha256'}, 'Original manifest not closed')
    require(files['MANIFEST.sha256'].decode().split() == [sha(files['MANIFEST.tsv']), 'MANIFEST.tsv'],
            'Original manifest sidecar mismatch')
    for row in rows:
        data = files[row['path']]
        require(len(data) == int(row['bytes']) and sha(data) == row['sha256'], 'Original manifest content mismatch')
    for name, data in files.items():
        add(destination + name, data, {'kind': 'unchanged-original-archive-member',
            'archive_sha256': expected, 'member': root + '/' + name, 'historical_only': True})
    return files

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--task', type=Path, required=True)
parser.add_argument('--readme', type=Path, required=True)
parser.add_argument('--output-dir', type=Path, required=True)
args = parser.parse_args()
require(git('rev-parse', 'HEAD').decode().strip() == EVIDENCE, 'Evidence HEAD changed; re-review builder')
require(git('branch', '--show-current').decode().strip() == 'codex/repeated-mult2-semantic-01', 'Wrong branch')
require(not git('status', '--porcelain', '--untracked-files=no'), 'Tracked working changes must be preserved/reviewed')
require(git('rev-parse', 'HEAD', repo=OFFICIAL).decode().strip() == PIN and
        not git('status', '--porcelain', repo=OFFICIAL), 'Official checkout not exact clean pin')
original = read_archive(INPUT, INPUT_SHA, 'repeated-mult2-semantic-implementation-01-80d771c', 'historical/input/', 129)
returned = read_archive(RETURN, RETURN_SHA, 'repeated-mult2-semantic-candidate-80d771c', 'historical/pro-return/', 33)
for name, data in original.items():
    if name.startswith('project/'):
        require(data == git('show', BASE + ':' + name[len('project/'):]), 'Historical base blob mismatch: ' + name)
for name, data in returned.items():
    require(data == git('show', EVIDENCE + ':' + RETURN_TREE + name), 'Retained return blob mismatch: ' + name)
selection = ['CMakeLists.txt', '.github/workflows/dcp-rcb.yml', 'include', 'src', 'tests', 'README.md',
             '.gitignore', 'LICENSE', 'LICENSE.md', 'LICENSE.txt', 'NOTICE']
project_paths = git('ls-tree', '-r', '--name-only', SOURCE, '--', *selection).decode().splitlines()
for path in project_paths:
    add_git('current/project/' + path, SOURCE, path)
spec_paths = {'TASK.md': 'coordination/tasks/REPEATED_MULT2_SEMANTIC_PRO_IMPLEMENTATION_01.md',
              'TEST_SEAMS.md': 'coordination/TEST_SEAMS.md',
              'REPEATED_MULT2_PHASE_CONTRACT_RESEARCH.md': 'coordination/REPEATED_MULT2_PHASE_CONTRACT_RESEARCH.md'}
for name, path in spec_paths.items():
    add_git('current/spec/' + name, EVIDENCE, path)
for suffix in ['SKILL.md', 'references/engineering.md', 'references/external-collaboration.md']:
    add_git('current/spec/project-workflow/' + suffix, EVIDENCE, '.agents/skills/openfhe-2023-1788-workflow/' + suffix)
for name in EVIDENCE_NAMES:
    add_git('current/evidence/' + name, EVIDENCE, HANDOFF + name)
official_locations = {}
for path in ['src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h',
             'src/pke/include/scheme/gen-cryptocontext-params.h',
             'src/pke/include/scheme/gen-cryptocontext-params-defaults.h', 'src/core/include/utils/exception.h']:
    old = 'official/extracted/official-full/' + path
    if old in original:
        require(original[old] == git('show', PIN + ':' + path, repo=OFFICIAL), 'Official anchor mismatch')
        official_locations[path] = 'historical/input/' + old
    else:
        official_locations[path] = 'current/official-supplement/' + path
        add_git(official_locations[path], PIN, path, repo=OFFICIAL)
add('diffs/red-to-tested-green.patch', git('diff', '--no-ext-diff', RED + '...' + SOURCE, '--', *ENGINEERING),
    {'kind': 'generated-git-diff', 'from': RED, 'to': SOURCE, 'paths': ENGINEERING})
cpp = 'src/repeated_mult2.cpp'
delta = ''.join(difflib.unified_diff(returned['complete/project/' + cpp].decode().splitlines(True),
    payload['current/project/' + cpp].decode().splitlines(True), fromfile='original-Pro/' + cpp, tofile='tested-GREEN/' + cpp))
add('diffs/original-pro-to-tested-repeated_mult2.patch', delta.encode(),
    {'kind': 'generated-diff', 'from_archive_sha256': RETURN_SHA, 'to_commit': SOURCE})
for name, path in [('TASK.md', args.task), ('README.md', args.readme)]:
    require(path.is_file() and not path.is_symlink() and path.suffix == '.md', 'Invalid explicit documentation overlay')
    add(name, path.read_bytes(), {'kind': 'explicit-current-review-overlay', 'source_name': path.name,
                                 'source_commit': 'NOT_A_TESTED_SOURCE_BLOB'})
provenance = {'tested_source': SOURCE, 'evidence_commit': EVIDENCE, 'red_source': RED, 'historical_base': BASE,
    'official_pin': PIN, 'official_four_setter_source_locations': official_locations,
    'worktree_status': git('status', '--porcelain').decode(), 'original_input_sha256': INPUT_SHA,
    'original_return_sha256': RETURN_SHA, 'project_paths': project_paths,
    'project_license_files_found': [p for p in project_paths if p.startswith(('LICENSE', 'NOTICE'))],
    'claim_boundary': 'Packaging only. No build, runtime, review completion, upload, or dispatch.',
    'targeted_scan': 'filename exclusions and credential signatures passed; independent gitleaks still required'}
add('PROVENANCE.json', (json.dumps(provenance, indent=2) + '\n').encode(), {'kind': 'generated-packaging-provenance'})
manifest = {'schema': 'exact-source-review-packet/v1', 'self_exclusions': ['MANIFEST.json'],
    'files': [{'path': p, 'bytes': len(payload[p]), 'sha256': sha(payload[p]), 'origin': origins[p]} for p in sorted(payload)]}
add('MANIFEST.json', (json.dumps(manifest, indent=2) + '\n').encode(), {'kind': 'self-excluded-manifest'})
require(len(payload) <= 300 and sum(map(len, payload.values())) <= 15_000_000, 'Output size/member ceiling')
require(args.output_dir.is_absolute() and not args.output_dir.exists(), 'Output must be a fresh absolute directory')
args.output_dir.mkdir(mode=0o700)
stage = args.output_dir / ROOT
stage.mkdir(mode=0o700)
for name, data in payload.items():
    target = stage / name
    target.parent.mkdir(parents=True, exist_ok=True)
    with target.open('xb') as stream:
        stream.write(data)
    target.chmod(0o600)
archive_path = args.output_dir / (ROOT + '.zip')
with zipfile.ZipFile(archive_path, 'x', compression=zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
    for name in sorted(payload):
        entry = zipfile.ZipInfo(ROOT + '/' + name, date_time=(2026, 9, 5, 0, 0, 0))
        entry.create_system, entry.external_attr = 3, (stat.S_IFREG | 0o644) << 16
        archive.writestr(entry, payload[name], compress_type=zipfile.ZIP_DEFLATED, compresslevel=6)
with zipfile.ZipFile(archive_path) as archive:
    require(archive.testzip() is None and len(archive.infolist()) == len(payload), 'Output CRC/count mismatch')
    require(all(archive.read(ROOT + '/' + p) == data for p, data in payload.items()), 'Output byte mismatch')
digest = sha(archive_path.read_bytes())
with archive_path.with_suffix('.zip.sha256').open('x') as stream:
    stream.write(digest + '  ' + archive_path.name + '\n')
print(json.dumps({'archive': str(archive_path), 'stage': str(stage), 'bytes': archive_path.stat().st_size,
                  'sha256': digest, 'regular_members': len(payload), 'gitleaks': 'NOT RUN; root gate before upload'}, indent=2))
