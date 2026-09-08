#!/usr/bin/env python3
"""Read-only ZIP integrity and supplied Git-blob identity checks; no extraction/execution.
Usage: python check_input_identity.py INPUT_ZIP OUTPUT_JSON
A matching Git blob authenticates bytes against the supplied manifest, not an
independent fetch/proof that the blob belongs to the remote commit tree.
"""
from __future__ import annotations
import argparse, hashlib, json, pathlib, platform, stat, zipfile
EXPECTED_SHA='abc4df778754a2d3713f1442fbfe34d69af274f3638e65d4d362f35aa93fdd68'
PROJECT='a4b815a733efe81897325e2a8e4c826a4ebfa439'
OFFICIAL='df495ba2e91739a6dc8f1de254fc5a41155ce504'
def main()->None:
    ap=argparse.ArgumentParser();ap.add_argument('archive',type=pathlib.Path);ap.add_argument('output',type=pathlib.Path);a=ap.parse_args()
    raw=a.archive.read_bytes();fail=[];blob_count=0;official=0
    if len(raw)!=2273139 or hashlib.sha256(raw).hexdigest()!=EXPECTED_SHA:fail.append('archive identity')
    with zipfile.ZipFile(a.archive) as z:
        info=z.infolist();names=[x.filename for x in info]
        if len(info)!=454 or len(set(names))!=454:fail.append('member count/uniqueness')
        for i in info:
            p=pathlib.PurePosixPath(i.filename);mode=i.external_attr>>16
            if p.is_absolute() or '..' in p.parts or '\\' in i.filename or i.is_dir() or stat.S_ISLNK(mode):fail.append('unsafe/nonregular:'+i.filename)
        crc=z.testzip()
        if crc:fail.append('CRC:'+crc)
        mraw=z.read('MANIFEST.json');m=json.loads(mraw)
        if m.get('source_commit')!=PROJECT or m.get('official_pin')!=OFFICIAL:fail.append('declared pin')
        unlisted=sorted(set(names)-{e['path'] for e in m['files']}-{'MANIFEST.json'})
        if unlisted:fail.append('unlisted members')
        for e in m['files']:
            b=z.read(e['path']);o=e.get('origin',{})
            if len(b)!=e['bytes'] or hashlib.sha256(b).hexdigest()!=e['sha256']:fail.append('payload:'+e['path'])
            if 'git_blob' in o:
                blob_count+=1
                if hashlib.sha1(b'blob '+str(len(b)).encode()+b'\0'+b).hexdigest()!=o['git_blob']:fail.append('git_blob:'+e['path'])
            if e['path'].startswith('official/'):
                official+=1
                if o.get('commit')!=OFFICIAL:fail.append('official pin:'+e['path'])
            if e['path'].startswith('project/') and o.get('commit')!=PROJECT:fail.append('project pin:'+e['path'])
        report={'check_type':'read-only zip/hash/declared pins/supplied Git blob IDs, not remote-tree acquisition','archive_bytes':len(raw),'archive_sha256':hashlib.sha256(raw).hexdigest(),'members':len(info),'expanded_bytes':sum(i.file_size for i in info),'payloads':len(m['files']),'manifest_sha256':hashlib.sha256(mraw).hexdigest(),'git_blob_checked':blob_count,'official_files':official,'source_commit':PROJECT,'official_pin':OFFICIAL,'crc_failure':crc,'unlisted':unlisted,'failures':fail,'environment':{'python':platform.python_version(),'platform':platform.platform()}}
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n');print(json.dumps(report,ensure_ascii=False,indent=2))
    if fail:raise SystemExit(1)
if __name__=='__main__':main()
