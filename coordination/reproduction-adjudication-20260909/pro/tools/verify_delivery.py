#!/usr/bin/env python3
"""Verify this delivery, not any inherited manifest. No experiment execution."""
from pathlib import Path, PurePosixPath
import hashlib,json,sys
root=Path(__file__).resolve().parents[1]
manifest=root/'MANIFEST.sha256.json'
try:
    obj=json.loads(manifest.read_text())
    rows=obj['files']
    names=[row['path'] for row in rows]
    if len(names)!=len(set(names)):
        raise ValueError('duplicate manifest path')
    for row in rows:
        name=row['path'];p=PurePosixPath(name)
        if p.is_absolute() or '..' in p.parts or '\\' in name or str(p)!=name:
            raise ValueError('unsafe path')
        path=root/name
        if path.is_symlink() or not path.is_file():
            raise ValueError('nonregular manifest payload')
        raw=path.read_bytes()
        if len(raw)!=row['bytes'] or hashlib.sha256(raw).hexdigest()!=row['sha256']:
            raise ValueError('payload mismatch: '+name)
    actual={str(p.relative_to(root)) for p in root.rglob('*') if p.is_file()}
    if actual!=set(names)|{'MANIFEST.sha256.json'}:
        raise ValueError('unmanifested/missing file; keep runtime output OUTSIDE the delivery')
    print(json.dumps({'status':'PASS','verified_payloads':len(rows),'manifest_sha256':hashlib.sha256(manifest.read_bytes()).hexdigest()}))
except Exception as error:
    print(f'VERIFY_FAILED: {error}',file=sys.stderr)
    raise SystemExit(2)
