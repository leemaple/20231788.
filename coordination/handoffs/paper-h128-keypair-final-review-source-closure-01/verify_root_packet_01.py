#!/usr/bin/env python3
"""Independent read-only final ZIP and actual Git-object verification."""
import hashlib
import json
from pathlib import Path, PurePosixPath
import stat
import subprocess
import unicodedata
import zipfile

REPO = Path('/Users/lifeng/Documents/20231788-openfhe-paper-h128-keypair-20260905')
OFFICIAL = Path('/private/tmp/h128-pro-packet.Lo406g/official-source')
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
DIR = REPO / 'artifacts/handoffs/h128-final-review-source-closure-01/build-02'
CURRENT = DIR / 'paper-h128-final-review-source-closure-1192200.zip'
OLD = REPO / 'artifacts/handoffs/h128-final-review-01/build-03/paper-h128-final-review-1192200.zip'
RETURN = REPO / 'artifacts/handoffs/h128-final-review-return-01/paper-h128-final-review-1192200-return.zip'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def read(path, digest, size, count):
    assert not path.is_symlink() and path.is_file()
    data = path.read_bytes()
    assert len(data) == size and sha(data) == digest
    assert Path(str(path) + '.sha256').read_bytes() == (digest + '  ' + path.name + '\n').encode()
    with zipfile.ZipFile(path) as archive:
        entries = archive.infolist()
        names = [e.filename for e in entries]
        assert len(entries) == count == len(set(names))
        assert len({unicodedata.normalize('NFC', n).casefold() for n in names}) == count
        assert sum(e.file_size for e in entries) < 32 * 1024 * 1024
        prefix = names[0].split('/')[0] + '/'
        for entry in entries:
            name = entry.filename
            assert name.startswith(prefix) and not name.startswith('/')
            assert '\\' not in name and ':' not in name and '\0' not in name
            assert all(p not in ('', '.', '..') for p in name.split('/'))
            assert PurePosixPath(name).as_posix() == name
            assert stat.S_ISREG(entry.external_attr >> 16) and not entry.flag_bits & 1
            assert not entry.is_dir()
        assert archive.testzip() is None
        files = {e.filename[len(prefix):]: archive.read(e) for e in entries}
    manifest = json.loads(files['MANIFEST.json'])
    assert manifest['self_exclusion'] == ['MANIFEST.json']
    records = manifest['files']
    assert len(records) == count - 1 == len({r['path'] for r in records})
    assert {r['path'] for r in records} == set(files) - {'MANIFEST.json'}
    for row in records:
        content = files[row['path']]
        assert len(content) == row['bytes'] and sha(content) == row['sha256']
    return files, records


old, old_rows = read(OLD, 'c4b8012a1f690d40c5571b24ae7828f414a5d05c6715a45fec0f3cb3e6710305', 2202028, 315)
returned, _ = read(RETURN, '31e3bea98c2468a31be2eab1f3688f238e68be393dc6a8ecad2a1d5692a6a416', 47452, 5)
current, current_rows = read(CURRENT, '8882265a6344f519b9a00adfab0e64436882b65c2b1f40a26d18c34cec0cf18f', 2328140, 329)
mapping = json.loads(current['ORIGINAL_PAYLOAD_MAPPING.json'])['mapping']
assert len(mapping) == 315 and len({r['new_path'] for r in mapping}) == 315
assert {r['original_path'] for r in mapping} == set(old)
for row in mapping:
    original = row['original_path']
    expected_path = 'original-input/' + original if original in ('TASK.md', 'MANIFEST.json') else original
    assert row['new_path'] == expected_path
    assert current[expected_path] == old[original]
    assert row['bytes'] == len(old[original]) and row['sha256'] == sha(old[original])
    assert row['original_manifest_payload'] == (original != 'MANIFEST.json')
for path, content in returned.items():
    assert current['review-return-original/' + path] == content

new_provenance = json.loads(current['SOURCE_CLOSURE_PROVENANCE.json'])
additions = new_provenance['new_official_sources']
assert len(additions) == 4 and new_provenance['official_commit'] == PIN
expected_additions = {
    'src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp',
    'src/pke/include/schemerns/rns-leveledshe.h',
    'src/pke/lib/schemerns/rns-leveledshe.cpp',
    'src/pke/include/schemebase/decrypt-result.h',
}
assert {r['source_path'] for r in additions} == expected_additions
verified_objects = []
for row, content in [(r, old[r['path']]) for r in old_rows if r.get('git_blob')] + [(r, current[r['path']]) for r in additions]:
    commit, source_path = row['source_commit'], row['source_path']
    repo = OFFICIAL if commit == PIN else REPO
    object_name = commit + ':' + source_path
    blob = subprocess.check_output(['git', '-C', str(repo), 'rev-parse', object_name], text=True).strip()
    actual = subprocess.check_output(['git', '-C', str(repo), 'cat-file', 'blob', object_name])
    expected_blob = hashlib.sha1(b'blob ' + str(len(content)).encode() + b'\0' + content).hexdigest()
    assert actual == content and blob == expected_blob == row['git_blob']
    verified_objects.append({'source_commit': commit, 'source_path': source_path, 'git_blob': blob, 'bytes': len(content), 'sha256': sha(content)})
assert len(verified_objects) == 263
assert sum(p.startswith('official/') for p in current) == 78
assert sum(p.startswith('current/evidence/') and p.endswith('_JOB_01.log') for p in current) == 8
assert current['TASK.md'] == (REPO / 'coordination/tasks/PAPER_H128_KEYPAIR_FINAL_REVIEW_SOURCE_CLOSURE_01.md').read_bytes()
assert old['TASK.md'] in current['TASK.md'] and b'1000 repetitions are cancelled' in current['TASK.md']
assert current['tools/build_source_closure_01.py'] == (REPO / 'coordination/handoffs/paper-h128-keypair-final-review-source-closure-01/build_source_closure_01.py').read_bytes()
expanded = DIR / CURRENT.stem
disk = {p.relative_to(expanded).as_posix(): p.read_bytes() for p in expanded.rglob('*') if p.is_file()}
assert disk == current
assert all(not p.is_symlink() for p in expanded.rglob('*'))

result = {
    'schema': 'h128-source-closure-root-independent-packet-check-v1',
    'result': 'PASS', 'archive': str(CURRENT), 'bytes': CURRENT.stat().st_size,
    'sha256': sha(CURRENT.read_bytes()), 'regular_members': len(current),
    'payloads': len(current_rows), 'expanded_bytes': sum(map(len, current.values())),
    'original_members_all_preserved': 315, 'original_payloads_all_preserved': 314,
    'original_return_members_all_preserved': 5, 'official_files': 78,
    'actual_git_commit_path_blob_checks': len(verified_objects),
    'complete_original_logs': 8, 'task_and_builder_match_disk': True,
    'current_task_supersedes_1000_trial_requirement': True,
    'archive_and_selection_exact_byte_equality': True,
    'safe_paths_crc_modes_manifest_sidecar': 'PASS',
    'verified_git_objects': verified_objects,
    'scope': 'Read-only byte/provenance checks; no source/code execution, crypto, build, CI, upload or semantic acceptance',
}
print(json.dumps(result, indent=2) + '\n', end='')
