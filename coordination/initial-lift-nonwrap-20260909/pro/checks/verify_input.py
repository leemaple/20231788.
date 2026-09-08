#!/usr/bin/env python3
"""Read-only identity, CRC, content, ancestry-row and Git-blob verification.
No extraction, project code execution, network, transform or crypto operation.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path, PurePosixPath
import stat
import sys
import zipfile

ARCHIVE_SHA='0cde0f999e13445e2faec46a4bf34dbcf6e47131c53afcf9e57375051eed7bed'
MANIFEST_SHA='96a45b3de562d23d30090e2048cb66d2c0412318a691bc204988fad736e305a7'
PINS={'source_commit':'a4b815a733efe81897325e2a8e4c826a4ebfa439',
      'official_pin':'df495ba2e91739a6dc8f1de254fc5a41155ce504',
      'evidence_commit':'b86105b33294a84ce76d65f585eb16d25ae07156',
      'task_commit':'6d42e6ea2d807cd7fdb9fc1433349694a6f06b48'}

def need(ok: bool, message: str) -> None:
    if not ok:raise ValueError(message)

def sha(data:bytes)->str:return hashlib.sha256(data).hexdigest()

def safe(name:str)->bool:
    p=PurePosixPath(name)
    return bool(name) and not p.is_absolute() and '\\' not in name and ':' not in name and '..' not in p.parts and str(p)==name

def main()->int:
    a=argparse.ArgumentParser(description=__doc__)
    a.add_argument('--zip',required=True,type=Path)
    a.add_argument('--extracted',type=Path)
    a.add_argument('--out',required=True,type=Path)
    args=a.parse_args()
    raw=args.zip.read_bytes()
    need(len(raw)==3626371 and sha(raw)==ARCHIVE_SHA,'archive identity mismatch')
    if args.extracted:
        eroot=args.extracted.resolve()
        need(not args.out.resolve().is_relative_to(eroot),'report may not be written inside immutable input')
    rows_checked=blobs=origins=0
    with zipfile.ZipFile(args.zip) as z:
        infos=z.infolist();names=[i.filename for i in infos]
        need(len(infos)==537 and len(set(names))==537,'member count/duplicate mismatch')
        need(len({n.casefold() for n in names})==537,'case-colliding member paths')
        for i in infos:
            mode=(i.external_attr>>16)&0xFFFF
            need(safe(i.filename) and not i.is_dir() and not stat.S_ISLNK(mode),'unsafe/nonregular member')
            need(stat.S_IFMT(mode) in (0,stat.S_IFREG),'unexpected archive file mode')
        need(z.testzip() is None,'CRC failed')
        mb=z.read('MANIFEST.json');need(sha(mb)==MANIFEST_SHA,'manifest identity mismatch')
        m=json.loads(mb)
        for k,v in PINS.items():need(m[k]==v,'manifest pin mismatch '+k)
        need(m['manifest_self_excluded'] is True,'manifest self-exclusion flag')
        need(len(m['files'])==536,'payload count')
        need(len({r['path'] for r in m['files']})==536,'duplicate manifest path')
        need(set(names)=={'MANIFEST.json'}|{r['path'] for r in m['files']},'manifest/member set mismatch')
        for r in m['files']:
            b=z.read(r['path']);need(len(b)==r['bytes'] and sha(b)==r['sha256'],'payload mismatch '+r['path'])
            rows_checked+=1
            origin=r.get('origin')
            while origin:
                if origin.get('git_blob'):
                    bh=hashlib.sha1(b'blob '+str(len(b)).encode()+b'\0'+b).hexdigest()
                    need(bh==origin['git_blob'],'Git blob mismatch '+r['path']);blobs+=1
                old=origin.get('row')
                if old:
                    need(len(b)==old['bytes'] and sha(b)==old['sha256'],'nested origin row mismatch '+r['path']);origins+=1
                origin=old.get('origin') if old else None
        if args.extracted:
            entries=list(args.extracted.rglob('*'))
            need(not any(p.is_symlink() for p in entries),'symlink in extracted input')
            actual={p.relative_to(args.extracted).as_posix() for p in entries if p.is_file()}
            need(actual==set(names),'extracted input path set changed')
            for name in names:need((args.extracted/name).read_bytes()==z.read(name),'extracted input bytes changed '+name)
    need(blobs==527,'Git blob count differs')
    result={'status':'PASS','archive_bytes':len(raw),'archive_sha256':ARCHIVE_SHA,
      'manifest_sha256':MANIFEST_SHA,'members':537,'payloads':rows_checked,'git_blob_checks':blobs,
      'nested_origin_rows_checked':origins,'pin_labels_match':PINS,
      'CRC_all_pass':True,'extracted_bytes_unchanged':True if args.extracted else None,
      'read_only_input_verification':True,'remote_commit_ancestry_verified':False,
      'historical_binary_identity_verified':False}
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8',newline='\n')
    print(json.dumps(result,ensure_ascii=False));return 0

if __name__=='__main__':
    try:sys.exit(main())
    except (ValueError,OSError,KeyError,zipfile.BadZipFile) as e:
        print('INPUT_VERIFICATION_FAILED: '+str(e),file=sys.stderr);sys.exit(1)
