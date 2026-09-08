#!/usr/bin/env python3
"""Recheck the supplied archive without extracting or executing its contents."""
from __future__ import annotations
import argparse
import hashlib
import json
import stat
import zipfile
from pathlib import Path, PurePosixPath

EXPECTED = {
 'bytes': 3319606,
 'sha256': '1aba188016d93ca8b38ec72f0f2592c6b45de562c455cb33e6d303d95ec6dc4c',
 'manifest_sha256': '34b3a05c62c48a55dba5bdcdc8fde0e1f397cb831dce35a95f53cc97129422b3',
 'task_commit': 'def8ccfde3c67bf00d7dc6a009451704b1b045b4',
 'source_commit': 'a4b815a733efe81897325e2a8e4c826a4ebfa439',
 'official_pin': 'df495ba2e91739a6dc8f1de254fc5a41155ce504',
 'evidence_commit': 'bbd4e73af74d1b072e3beb588cf9c7c4de3117cc',
 'paper_sha256': '61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac'}


def need(ok: bool, label: str) -> None:
    if not ok:
        raise ValueError(label)


def verify(path: Path) -> dict:
    raw = path.read_bytes()
    need(len(raw) == EXPECTED['bytes'], 'archive bytes')
    need(hashlib.sha256(raw).hexdigest() == EXPECTED['sha256'], 'archive hash')
    rows, blobs, official_blobs = [], 0, 0
    with zipfile.ZipFile(path) as z:
        infos = z.infolist()
        names = [i.filename for i in infos]
        need(len(names) == len(set(names)) == 500, 'unique member count')
        for i in infos:
            p = PurePosixPath(i.filename)
            mode = i.external_attr >> 16
            need(not i.is_dir() and not p.is_absolute() and '..' not in p.parts
                 and '\\' not in i.filename and stat.S_IFMT(mode) in (0, stat.S_IFREG), 'unsafe member')
        need(z.testzip() is None, 'CRC')
        manifest_bytes = z.read('MANIFEST.json')
        need(hashlib.sha256(manifest_bytes).hexdigest() == EXPECTED['manifest_sha256'], 'manifest hash')
        m = json.loads(manifest_bytes)
        for field in ('task_commit', 'source_commit', 'official_pin', 'evidence_commit'):
            need(m[field] == EXPECTED[field], field)
        need(len(m['files']) == 499, 'payload count')
        need(set(names) == {r['path'] for r in m['files']} | {'MANIFEST.json'}, 'member closure')
        for row in m['files']:
            data = z.read(row['path'])
            sha = hashlib.sha256(data).hexdigest()
            need(len(data) == row['bytes'] and sha == row['sha256'], 'payload: ' + row['path'])
            origin = row.get('origin', {})
            while 'row' in origin:
                origin = origin['row'].get('origin', {})
            blob = origin.get('git_blob')
            if blob:
                got = hashlib.sha1(b'blob ' + str(len(data)).encode('ascii') + b'\0' + data).hexdigest()
                need(got == blob, 'git blob: ' + row['path'])
                blobs += 1
                official_blobs += int(row['path'].startswith('official/'))
            rows.append({'path': row['path'], 'bytes': len(data), 'sha256': sha,
                         'git_blob_recomputed': blob is not None, 'git_blob': blob})
        need(hashlib.sha256(z.read('references/paper/PAPER-2023-1788.pdf')).hexdigest() == EXPECTED['paper_sha256'], 'paper')
        uncompressed = sum(i.file_size for i in infos)
    return {'schema': 'input-archive-verification-v1', 'status': 'PASS', 'expected_and_matched': EXPECTED,
            'members': 500, 'payloads_verified': 499, 'total_uncompressed_bytes': uncompressed,
            'git_blobs_recomputed': blobs, 'official_git_blobs_recomputed': official_blobs,
            'CRC_ok': True, 'member_closure_ok': True,
            'limitation': 'Offline equality with supplied receipt/blob identities, not a new network commit-membership proof.',
            'rows': rows}


def main() -> None:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('archive', type=Path)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    result = verify(a.archive)
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(result, ensure_ascii=False, indent=2)+'\n', encoding='utf-8')
    print('PASS:', result['members'], 'members;', result['payloads_verified'], 'payload hashes;',
          result['git_blobs_recomputed'], 'Git blobs;', result['official_git_blobs_recomputed'], 'official blobs;',
          result['total_uncompressed_bytes'], 'expanded bytes; CRC/member closure PASS')

if __name__ == '__main__':
    main()
