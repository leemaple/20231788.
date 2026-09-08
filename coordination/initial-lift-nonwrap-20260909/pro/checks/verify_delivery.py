#!/usr/bin/env python3
"""Verify the self-excluding return manifest, exact path set, bytes and hashes.
Does not execute payload files. Choose an extracted root or the final ZIP.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path, PurePosixPath
import stat
import sys
import zipfile

def need(ok:bool,message:str)->None:
    if not ok:raise ValueError(message)

def safe(s:str)->bool:
    p=PurePosixPath(s)
    return bool(s) and str(p)==s and not p.is_absolute() and '..' not in p.parts and '\\' not in s and ':' not in s

def verify(data:dict[str,bytes])->dict:
    need('MANIFEST.json' in data,'missing root manifest')
    m=json.loads(data['MANIFEST.json'])
    need(m['schema']=='initial-lift-nonwrap-return-manifest-v1','unknown manifest schema')
    need(m['self_excluded']==['MANIFEST.json'],'wrong self-exclusion')
    rows=m['files'];names=[r['path'] for r in rows]
    need(len(names)==len(set(names))==len({n.casefold() for n in names}),'duplicate or case-colliding manifest paths')
    need(all(safe(n) and n!='MANIFEST.json' for n in names),'unsafe/self-included manifest path')
    need(set(data)==set(names)|{'MANIFEST.json'},'actual file set differs from manifest')
    total=0
    for r in rows:
        b=data[r['path']]
        need(len(b)==r['bytes'],'size mismatch '+r['path'])
        need(hashlib.sha256(b).hexdigest()==r['sha256'],'hash mismatch '+r['path']);total+=len(b)
    need(m['payload_count']==len(rows) and m['payload_bytes']==total,'manifest count/size summary mismatch')
    need(m['input_binding']['archive_sha256']=='0cde0f999e13445e2faec46a4bf34dbcf6e47131c53afcf9e57375051eed7bed','input identity changed')
    return {'status':'PASS','payload_count':len(rows),'regular_members':len(data),'payload_bytes':total,
        'expanded_bytes':sum(map(len,data.values())),
        'manifest_sha256':hashlib.sha256(data['MANIFEST.json']).hexdigest(),
        'all_payload_hashes_pass':True,'exact_member_set':True,'self_exclusion_verified':True}

def main()->int:
    p=argparse.ArgumentParser(description=__doc__);g=p.add_mutually_exclusive_group(required=True)
    g.add_argument('--root',type=Path);g.add_argument('--zip',type=Path);a=p.parse_args()
    if a.zip:
        with zipfile.ZipFile(a.zip) as z:
            infos=z.infolist();names=[i.filename for i in infos]
            need(len(names)==len(set(names)),'duplicate ZIP entries')
            for i in infos:
                mode=(i.external_attr>>16)&0xFFFF
                need(safe(i.filename) and not i.is_dir() and stat.S_IFMT(mode) in (0,stat.S_IFREG),'unsafe/nonregular ZIP entry')
            need(z.testzip() is None,'CRC failure')
            result=verify({n:z.read(n) for n in names});result['zip_CRC_pass']=True
    else:
        entries=list(a.root.rglob('*'));need(not any(f.is_symlink() for f in entries),'symlink in return root')
        result=verify({f.relative_to(a.root).as_posix():f.read_bytes() for f in entries if f.is_file()})
    print(json.dumps(result,ensure_ascii=False));return 0

if __name__=='__main__':
    try:sys.exit(main())
    except (ValueError,OSError,KeyError,zipfile.BadZipFile) as e:
        print('DELIVERY_VERIFICATION_FAILED: '+str(e),file=sys.stderr);sys.exit(1)
