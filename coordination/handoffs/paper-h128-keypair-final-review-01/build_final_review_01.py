#!/usr/bin/env python3
"""Mechanical exact-source, offline-review bundle builder; never compile/upload."""
import argparse
import difflib
import hashlib
import json
from pathlib import Path, PurePosixPath
import re
import stat
import subprocess
import unicodedata
import zipfile

REPO = Path('/Users/lifeng/Documents/20231788-openfhe-paper-h128-keypair-20260905')
OFFICIAL = Path('/private/tmp/h128-pro-packet.Lo406g/official-source')
EVIDENCE = 'a5d6f933e26fbf58ab06ee679faa444f441567b8'
INPUT = '9d21c3a5aea79c31745aca790712a9fd8c7743b2'
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
FINAL = '1192200f558c69c0967e8306ed1a8bddf786ca34'
HANDOFF = 'coordination/handoffs/paper-h128-keypair-pro-packet-01/'
TASK = REPO / 'coordination/tasks/PAPER_H128_KEYPAIR_FINAL_REVIEW_01.md'
ROOT = 'paper-h128-final-review-1192200'
STAGES = [
    ('A_RED', 'a21216f0a8f854f478129d02fd32f496bd80f71c', 33943456483),
    ('A_GREEN', '8aac5b7cf6530a9a2da14e8a4bdd5b65ab3c869f', 33944191280),
    ('B_RED', '43c2dca45c2305c9b6baf50ae1c32529d35e7f06', 33945243915),
    ('B_GREEN', FINAL, 33945897881),
]
ENGINEERING = ['CMakeLists.txt', '.github/workflows/dcp-rcb.yml', 'include', 'src', 'tests']
CHANGED = ['.github/workflows/dcp-rcb.yml', 'CMakeLists.txt',
           'include/openfhe_2023_1788/paper_h128_client_keypair.h',
           'src/paper_h128_client_keypair.cpp', 'tests/data/paper_h128_profile.json',
           'tests/paper_h128_client_keypair_contract_test.cpp']
SUPPLEMENT = [
    'LICENSE', 'src/core/include/utils/exception.h',
    'src/core/include/math/discretegaussiangenerator.h',
    'src/core/include/math/discretegaussiangenerator-impl.h',
    'src/pke/include/key/evalkey.h', 'src/pke/include/key/evalkeyrelin.h',
    'src/pke/include/keyswitch/keyswitch-base.h',
    'src/pke/include/keyswitch/keyswitch-rns.h',
    'src/pke/include/keyswitch/keyswitch-bv.h',
    'src/pke/include/keyswitch/keyswitch-hybrid.h',
    'src/pke/include/schemebase/base-pke.h',
    'src/pke/include/scheme/ckksrns/ckksrns-leveledshe.h',
    'src/pke/include/scheme/ckksrns/ckksrns-parametergeneration.h',
    'src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp',
    'src/pke/lib/keyswitch/keyswitch-bv.cpp',
    'src/pke/include/openfhe.h', 'src/pke/include/ciphertext.h',
    'src/pke/include/encoding/plaintext.h',
    'src/pke/include/encoding/plaintextfactory.h',
    'src/pke/include/encoding/ckkspackedencoding.h',
    'src/pke/lib/encoding/ckkspackedencoding.cpp',
    'src/core/include/math/dftransform.h', 'src/core/lib/math/dftransform.cpp',
]


def command(args, cwd=REPO):
    return subprocess.check_output(args, cwd=cwd)


def digest(data):
    return hashlib.sha256(data).hexdigest()


def obj(commit, path, repo=REPO):
    data = command(['git', 'show', f'{commit}:{path}'], repo)
    oid = command(['git', 'rev-parse', f'{commit}:{path}'], repo).decode().strip()
    actual = hashlib.sha1(b'blob ' + str(len(data)).encode() + b'\0' + data).hexdigest()
    assert actual == oid, path
    return data, oid


def paths(commit, selections, repo=REPO):
    return command(['git', 'ls-tree', '-r', '--name-only', commit, '--', *selections], repo).decode().splitlines()


def safe(path):
    p = PurePosixPath(path)
    assert path and '\\' not in path and ':' not in path and not p.is_absolute()
    assert all(x not in ('', '.', '..') for x in path.split('/')), path
    assert not any(x in {'.git', 'node_modules', '__pycache__', '.env', '.ci', 'build'} for x in p.parts), path
    assert not re.search(r'(?i)(^|/)(id_rsa|id_ed25519|cookies?|credentials|login data)(\.|$)', path), path
    assert p.suffix.lower() not in {'.pem', '.p12', '.pfx', '.key', '.sqlite', '.db', '.exe', '.dll', '.so', '.dylib'}, path


def read_archive(path, expected_sha, expected_count):
    raw = path.read_bytes()
    assert digest(raw) == expected_sha
    result = {}
    folded = set()
    with zipfile.ZipFile(path) as archive:
        assert archive.testzip() is None
        infos = archive.infolist()
        assert len(infos) == expected_count
        outer = infos[0].filename.split('/')[0]
        for info in infos:
            assert not info.is_dir() and not info.flag_bits & 1
            mode = info.external_attr >> 16
            assert stat.S_IFMT(mode) in (0, stat.S_IFREG)
            assert info.filename.startswith(outer + '/')
            name = info.filename[len(outer) + 1:]
            safe(name)
            folded_name = unicodedata.normalize('NFC', name).casefold()
            assert folded_name not in folded
            folded.add(folded_name)
            result[name] = archive.read(info)
    return result, {'name': path.name, 'bytes': len(raw), 'sha256': digest(raw),
                    'regular_members': expected_count, 'original_outer_directory': outer,
                    'mapping': 'All regular members retained byte-for-byte; only outer directory stripped.'}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', required=True)
    args = parser.parse_args()
    out = Path(args.output).resolve()
    allowed = (REPO / 'artifacts/handoffs/h128-final-review-01').resolve()
    assert out.is_relative_to(allowed) and not out.exists()
    assert command(['git', 'rev-parse', 'HEAD']).decode().strip() == EVIDENCE
    assert command(['git', 'branch', '--show-current']).decode().strip() == 'codex/paper-h128-keypair-01'
    assert command(['git', 'status', '--porcelain', '--', *ENGINEERING]) == b''
    assert command(['git', 'rev-parse', 'HEAD'], OFFICIAL).decode().strip() == PIN
    assert command(['git', 'status', '--porcelain'], OFFICIAL) == b''
    payload, manifest = {}, []
    folded = set()

    def add(name, data, origin, commit=None, source_path=None, blob=None):
        safe(name)
        assert data and name not in payload
        fold = unicodedata.normalize('NFC', name).casefold()
        assert fold not in folded
        folded.add(fold)
        payload[name] = data
        manifest.append({'path': name, 'bytes': len(data), 'sha256': digest(data),
                         'origin': origin, 'source_commit': commit,
                         'source_path': source_path, 'git_blob': blob})

    def add_git(name, commit, path, repo=REPO):
        data, blob = obj(commit, path, repo)
        add(name, data, 'exact-git-blob', commit, path, blob)

    retained = REPO / 'artifacts/handoffs/h128-return-01'
    original, original_id = read_archive(
        retained / 'paper-h128-keypair-implementation-01-9d21c3a-20260905T0812.zip',
        '52f0dec88ac9ee854b2a863a60f382f35a6bf7117aada1b39d45637f8e367e8b', 98)
    returned, returned_id = read_archive(
        retained / 'paper-h128-keypair-four-patch-9d21c3a.zip',
        'ccf2ecad2b6db7d0c6306dcedcd64b21e6e57aa677fa33e0eab83b964aa5df5a', 38)
    project_count = official_count = 0
    official_paths = []
    for name, data in original.items():
        commit = oid = source_path = None
        if name.startswith('project/'):
            source_path, commit = name[8:], INPUT
            actual, oid = obj(commit, source_path)
            assert actual == data
            project_count += 1
        elif name.startswith('official-openfhe-1.5.0/'):
            source_path, commit = name[len('official-openfhe-1.5.0/'):], PIN
            actual, oid = obj(commit, source_path, OFFICIAL)
            assert actual == data
            official_paths.append(source_path)
            official_count += 1
        add('historical/input/' + name, data, 'authenticated-original-input-member', commit, source_path, oid)
    assert (project_count, official_count) == (39, 51)
    for name, data in returned.items():
        add('historical/pro-return/' + name, data, 'authenticated-original-return-member')
    for path in sorted(set(official_paths + SUPPLEMENT)):
        add_git('official/' + path, PIN, path, OFFICIAL)
    current = paths(FINAL, ENGINEERING + ['README.md', '.gitignore'])
    for path in current:
        add_git('current/project/' + path, FINAL, path)
    assert len(current) == 29
    evidence_paths = [p for p in paths(EVIDENCE, [HANDOFF]) if
                      Path(p).name.startswith('CYCLE_') or Path(p).name in
                      {'RETURN_RECEIPT_01.md', 'ROOT_RETURN_REPLAY_01.json',
                       'ROOT_RETURN_ARITHMETIC_01.json', 'SOURCE_PROVENANCE.json'}]
    for path in evidence_paths:
        add_git('current/evidence/' + Path(path).name, EVIDENCE, path)
    stage_records = []
    previous = INPUT
    for stage, sha, run in STAGES:
        present = set(paths(sha, CHANGED))
        absent = sorted(set(CHANGED) - present)
        for path in sorted(present):
            add_git(f'stages/{stage}/{path}', sha, path)
        run_doc = json.loads(payload[f'current/evidence/CYCLE_{stage}_RUN_FINAL_01.json'])
        assert run_doc['databaseId'] == run and run_doc['attempt'] == 1 and run_doc['headSha'] == sha
        assert len(run_doc['jobs']) == 2
        log_ids = []
        for host in ('LINUX', 'WINDOWS'):
            name = f'current/evidence/CYCLE_{stage}_{host}_JOB_01.log'
            log = payload[name]
            assert f'PROJECT_SOURCE_COMMIT={sha} GITHUB_RUN_ID={run} GITHUB_RUN_ATTEMPT=1'.encode() in log
            assert PIN.encode() in log
            log_ids.append({'path': name, 'bytes': len(log), 'sha256': digest(log),
                            'form': 'LF-normalized retained full log; see source acceptance for raw identity'})
        diff = command(['git', 'diff', '--binary', '--full-index', previous, sha, '--', *ENGINEERING])
        add(f'diffs/{stage}.patch', diff, 'exact-git-engineering-diff', sha,
            previous + '..' + sha)
        stage_records.append({'stage': stage, 'commit': sha, 'run': run, 'attempt': 1,
                              'present_changed_paths': sorted(present), 'absent_changed_paths': absent,
                              'logs': log_ids, 'conclusion': run_doc['conclusion']})
        previous = sha
    add('diffs/input-to-final.patch', command(['git', 'diff', '--binary', '--full-index', INPUT, FINAL,
                                             '--', *ENGINEERING]), 'exact-git-engineering-diff', FINAL,
        INPUT + '..' + FINAL)
    overlay = ''
    for path in CHANGED:
        before = returned['complete/project/' + path]
        after = payload['current/project/' + path]
        if path in ('CMakeLists.txt', '.github/workflows/dcp-rcb.yml'):
            overlay += ''.join(difflib.unified_diff(before.decode().splitlines(True),
                                                   after.decode().splitlines(True),
                                                   fromfile='original-pro/' + path,
                                                   tofile='tested/' + path))
        else:
            assert before == after, path
    add('diffs/original-pro-to-tested-build-overlay.patch', overlay.encode(),
        'byte-comparison-unified-diff', FINAL)
    add('TASK.md', TASK.read_bytes(), 'new-review-task')
    readme = ('# h128 final review bundle\n\nRead TASK.md first. Current/project is the exact tested source; '
              'all historical instructions are evidence only. No compile, crypto, network or CI action is authorized.\n\n'
              'The MANIFEST.json lists every payload except itself. PROVENANCE.json records exact stage/source '
              'identities and the lossless historical-archive mappings. current/evidence contains complete '
              'logs for all eight jobs, not extracts. official is a targeted source-review union, not a build kit.\n')
    add('README.md', readme.encode(), 'new-review-navigation')
    provenance = {'schema': 'h128-final-review-provenance-v1', 'source': FINAL, 'evidence_commit': EVIDENCE,
                  'original_input_commit': INPUT, 'official_commit': PIN,
                  'branch': 'codex/paper-h128-keypair-01', 'initial_engineering_status': 'clean',
                  'original_archives': [original_id, returned_id],
                  'verified_original_project_git_objects': project_count,
                  'verified_original_official_git_objects': official_count,
                  'current_engineering_files': len(current), 'current_evidence_files': len(evidence_paths),
                  'official_union_count': len(set(official_paths + SUPPLEMENT)),
                  'official_supplement_paths': SUPPLEMENT, 'stages': stage_records,
                  'verification': 'Every Git-origin member compared to git show at exact commit; SHA1 Git blob recomputed.',
                  'limits': 'No compile/crypto/CI/upload. No .git/installed dependencies/browser state. '
                            'Original archives expanded byte-for-byte; original compressed byte streams not embedded. '
                            'Official selection closes named adapter/fixture/RTTI/cache operations; not transitive build closure.'}
    add('PROVENANCE.json', (json.dumps(provenance, indent=2) + '\n').encode(), 'mechanical-provenance')
    manifest_bytes = (json.dumps({'schema': 'closed-review-manifest-v1', 'self_exclusion': ['MANIFEST.json'],
                                 'files': sorted(manifest, key=lambda r: r['path'])}, indent=2) + '\n').encode()
    payload['MANIFEST.json'] = manifest_bytes
    stage_dir = out / ROOT
    stage_dir.mkdir(parents=True)
    for name, data in payload.items():
        target = stage_dir / name
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(data)
    # Maintained secret scan of expanded selected content, with redacted output.
    scanner_version = command(['gitleaks', 'version']).decode().strip()
    subprocess.run(['gitleaks', 'dir', str(stage_dir), '--redact', '--no-banner'], check=True)
    zip_path = out / (ROOT + '.zip')
    with zipfile.ZipFile(zip_path, 'x', zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
        for name, data in sorted(payload.items()):
            info = zipfile.ZipInfo(ROOT + '/' + name, (2026, 9, 5, 0, 0, 0))
            info.create_system = 3
            info.external_attr = 0o100644 << 16
            info.compress_type = zipfile.ZIP_DEFLATED
            archive.writestr(info, data)
    with zipfile.ZipFile(zip_path) as archive:
        assert archive.testzip() is None
        assert len(archive.infolist()) == len(payload)
        for info in archive.infolist():
            name = info.filename[len(ROOT) + 1:]
            assert archive.read(info) == payload[name]
    final_raw = zip_path.read_bytes()
    sidecar = zip_path.with_suffix('.zip.sha256')
    sidecar.write_text(digest(final_raw) + '  ' + zip_path.name + '\n')
    subprocess.run(['gitleaks', 'dir', str(out), '--max-archive-depth', '1', '--redact', '--no-banner'], check=True)
    result = {'zip_path': str(zip_path), 'bytes': len(final_raw), 'sha256': digest(final_raw),
              'sidecar_path': str(sidecar), 'sidecar_bytes': sidecar.stat().st_size,
              'sidecar_sha256': digest(sidecar.read_bytes()), 'regular_members': len(payload),
              'manifest_payloads': len(manifest), 'expanded_bytes': sum(map(len, payload.values())),
              'manifest_sha256': digest(manifest_bytes), 'task_bytes': len(payload['TASK.md']),
              'task_sha256': digest(payload['TASK.md']), 'official_count': provenance['official_union_count'],
              'current_engineering_files': len(current), 'evidence_files': len(evidence_paths),
              'source': FINAL, 'evidence_commit': EVIDENCE, 'gitleaks_version': scanner_version,
              'gitleaks_staging_and_archive_exit': 0, 'dispatched': False}
    (out / 'BUILD_RESULT.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
