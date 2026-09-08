#!/usr/bin/env python3
"""Verify the original adjudication ZIP, without running/extracting any payload."""
from __future__ import annotations
import argparse,hashlib,json,stat,zipfile
from pathlib import Path,PurePosixPath


def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('archive',type=Path)
    ap.add_argument('--output',type=Path,required=True)
    args=ap.parse_args()
    def need(v,message):
        if not v:raise ValueError(message)
    def sha(raw):return hashlib.sha256(raw).hexdigest()
    raw=args.archive.read_bytes()
    need(len(raw)==5400318,'archive byte length')
    need(sha(raw)=='7e3ea9fee4a04d5535cc47aab42a71e38367db18c70b804a609551345affcbe4','archive digest')
    with zipfile.ZipFile(args.archive) as z:
        infos=z.infolist(); names=[item.filename for item in infos]
        need(len(names)==760 and len(names)==len(set(names)),'member count/uniqueness')
        for item in infos:
            p=PurePosixPath(item.filename); mode=stat.S_IFMT(item.external_attr>>16)
            need(not item.is_dir() and not p.is_absolute() and '..' not in p.parts
                 and '\\' not in item.filename and str(p)==item.filename
                 and mode in (0,stat.S_IFREG),'unsafe/nonregular ZIP member')
        need(z.testzip() is None,'CRC')
        mraw=z.read('MANIFEST.json')
        need(sha(mraw)=='776fd05dc43adf11ca9a912487ae8bf5c1269dfe69149addd8263cbe34b8edaf','manifest digest')
        m=json.loads(mraw); rows=m['files']
        need(len(rows)==759 and len({x['path'] for x in rows})==759,'manifest uniqueness')
        need({x['path'] for x in rows}|{'MANIFEST.json'}==set(names),'manifest completeness')
        blobs=0
        for row in rows:
            data=z.read(row['path'])
            need(len(data)==row['bytes'] and sha(data)==row['sha256'],'payload mismatch: '+row['path'])
            origin=row['origin']
            while isinstance(origin,dict):
                if 'git_blob' in origin:
                    expected=hashlib.sha1(b'blob '+str(len(data)).encode()+b'\0'+data).hexdigest()
                    need(expected==origin['git_blob'],'Git blob mismatch: '+row['path'])
                    blobs+=1;break
                if 'row' in origin:origin=origin['row'].get('origin',{})
                else:break
        result={'status':'PASS','archive_sha256':sha(raw),'archive_bytes':len(raw),
            'members':len(infos),'payloads':len(rows),'expanded_bytes':sum(i.file_size for i in infos),
            'manifest_sha256':sha(mraw),'all_payload_sha256_and_bytes':'PASS','CRC':'PASS',
            'regular_safe_unique_members':'PASS','Git_blob_checks':blobs,
            'official_files':sum(row['path'].startswith('official/') for row in rows),
            'source_commit':m['source_commit'],'task_commit':m['task_commit'],
            'numerical_execution':'NONE','secret_scanning':'NOT_PERFORMED_BY_THIS_CHECKER'}
    with args.output.open('x',encoding='utf-8') as dst:
        json.dump(result,dst,indent=2);dst.write('\n')
    print(json.dumps(result))

if __name__=='__main__':main()
