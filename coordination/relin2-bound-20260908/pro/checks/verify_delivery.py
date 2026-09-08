#!/usr/bin/env python3
"""Validate the self-excluding output manifest and cross-file proof identifiers."""
from __future__ import annotations
import argparse, csv, hashlib, json
from pathlib import Path, PurePosixPath


def need(ok: bool, why: str) -> None:
    if not ok:
        raise ValueError(why)


def verify(root: Path) -> dict:
    manifest = json.loads((root / 'MANIFEST.json').read_text(encoding='utf-8'))
    rows = manifest['files']
    paths = [row['path'] for row in rows]
    need(len(paths) == len(set(paths)) and 'MANIFEST.json' not in paths, 'self-excluding unique manifest')
    for row in rows:
        rel = PurePosixPath(row['path'])
        need(not rel.is_absolute() and '..' not in rel.parts, 'unsafe manifest path')
        p = root / row['path']
        need(p.is_file() and not p.is_symlink(), 'missing/nonregular payload')
        data = p.read_bytes()
        need(len(data) == row['bytes'] and hashlib.sha256(data).hexdigest() == row['sha256'], 'hash: '+str(rel))
    actual = {p.relative_to(root).as_posix() for p in root.rglob('*') if p.is_file()}
    need(actual == set(paths) | {'MANIFEST.json'}, 'output member closure')
    claims = json.loads((root/'CLAIMS.json').read_text(encoding='utf-8'))
    amap = {a['id'] for a in claims['assumption_registry']}
    with (root/'SOURCE_MAP.tsv').open(encoding='utf-8',newline='') as f:
        sources = list(csv.DictReader(f,delimiter='\t'))
    smap = {s['source_id'] for s in sources}
    input_rows = json.loads((root/'evidence/INPUT_VERIFICATION.json').read_text(encoding='utf-8'))['rows']
    input_hashes = {r['path']:r['sha256'] for r in input_rows}
    need(all(input_hashes.get(s['path']) == s['sha256'] for s in sources), 'source-map/input hash mismatch')
    cmap = {c['id'] for c in claims['claims']}
    need(len(cmap)==30 and len(smap)==len(sources)==50, 'claim/source counts')
    body = (root/'RELIN2_BOUND.zh-CN.md').read_text(encoding='utf-8')
    for c in claims['claims']:
        need(set(c['assumptions'])<=amap and set(c['basis'])<=smap, 'broken claim link')
        need(c['id'] in body, 'claim not present in report')
    for s in sources:
        need(set(s['claims'].split(';'))<=cmap, 'source points to missing claim')
    results = json.loads((root/'checks/model_results.json').read_text(encoding='utf-8'))
    need(results['status']=='PASS' and results['test_count']==13 and len(results['tests'])==13, 'model status')
    need(all(t['status']=='PASS' for t in results['tests']), 'model test failed')
    nums=json.loads((root/'checks/public_bounds.json').read_text(encoding='utf-8'))
    need(nums['status']=='PASS' and len(nums['families'])==8 and nums['source_constants_match'] is True, 'public bounds')
    return {'status':'PASS','payloads':len(rows),'members_including_manifest':len(rows)+1,
            'claims':len(cmap),'source_rows':len(sources),'model_tests':13,'public_families':8}


def main() -> None:
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('root',type=Path,nargs='?',default=Path('.'))
    a=p.parse_args();print(json.dumps(verify(a.root),ensure_ascii=False,indent=2))

if __name__=='__main__':
    main()
