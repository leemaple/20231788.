#!/usr/bin/env python3
"""Verify this return's regular-member closure and self-excluding manifest."""
import argparse
import hashlib
import json
import pathlib
import stat
import zipfile

def require(condition, message):
    if not condition:
        raise ValueError(message)

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('archive',type=pathlib.Path)
    args=p.parse_args()
    raw=args.archive.read_bytes()
    with zipfile.ZipFile(args.archive) as z:
        names=z.namelist()
        require(len(set(names))==len(names),'duplicate members')
        for info in z.infolist():
            name=pathlib.PurePosixPath(info.filename)
            require(name.as_posix()==info.filename and not name.is_absolute()
                    and '..' not in name.parts and '\\' not in info.filename
                    and ':' not in info.filename,'unsafe member path')
            require(not info.is_dir() and stat.S_IFMT(info.external_attr>>16)
                    in (0,stat.S_IFREG),'nonregular member')
            require(not (info.flag_bits&1),'encrypted member')
        require(z.testzip() is None,'CRC failure')
        m=json.loads(z.read('MANIFEST.json'))
        require(m['self_excluded']==['MANIFEST.json'],'self exclusion')
        entries=m['files'];paths=[e['path'] for e in entries]
        require(len(paths)==len(set(paths)),'duplicate manifest entry')
        require(set(paths)==set(names)-{'MANIFEST.json'},'manifest closure')
        for e in entries:
            b=z.read(e['path'])
            require(len(b)==e['bytes'],'member size: '+e['path'])
            require(hashlib.sha256(b).hexdigest()==e['sha256'],'member digest: '+e['path'])
            origin=e.get('origin',{})
            if origin.get('kind')=='exact_input_copy':
                require(e['sha256']==origin['input_sha256'],'input copy changed')
    print(json.dumps({'result':'RETURN_INTEGRITY_PASS','regular_members':len(names),
                      'manifest_payloads':len(entries),'archive_bytes':len(raw),
                      'archive_sha256':hashlib.sha256(raw).hexdigest()},indent=2))
if __name__=='__main__': main()
