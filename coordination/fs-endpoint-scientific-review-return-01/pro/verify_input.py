#!/usr/bin/env python3
"""Strict read-only archive verification and safe extraction of this input packet."""
import argparse, hashlib, json, pathlib, stat, zipfile

def require(condition, message='integrity condition'):
 if not condition: raise ValueError(message)

def main():
 p=argparse.ArgumentParser();p.add_argument('archive',type=pathlib.Path);p.add_argument('--extract',type=pathlib.Path);a=p.parse_args()
 data=a.archive.read_bytes(); sha=hashlib.sha256(data).hexdigest()
 require(len(data)==9896298, 'archive size mismatch')
 require(sha=='ee98c075f62e23cf99f0a06ce694b49290f1f5dbe275932988954d41aa919d23', 'archive SHA mismatch')
 with zipfile.ZipFile(a.archive) as z:
  ns=z.namelist(); require(len(ns)==218 and len(set(ns))==218, 'member closure/duplicates')
  for i in z.infolist():
   q=pathlib.PurePosixPath(i.filename); mode=i.external_attr>>16
   require(q.as_posix()==i.filename and not q.is_absolute() and '..' not in q.parts and '\\' not in i.filename and ':' not in i.filename, 'integrity condition')
   require(not i.is_dir() and stat.S_IFMT(mode) in (0,stat.S_IFREG), 'nonregular member')
   require(not i.flag_bits & 1, 'encrypted member')
  require(z.testzip() is None, 'CRC error')
  md=z.read('MANIFEST.json'); m=json.loads(md)
  require(m['manifest_self_excluded'] is True, 'integrity condition')
  entries=m['files']; paths=[x['path'] for x in entries]
  require(len(entries)==217 and len(set(paths))==217, 'integrity condition')
  require(set(paths)==set(ns)-{'MANIFEST.json'}, 'manifest closure')
  for e in entries:
   b=z.read(e['path']);require(len(b)==e['bytes'], e['path']);require(hashlib.sha256(b).hexdigest()==e['sha256'], e['path'])
  out={'result':'PASS','archive_bytes':len(data),'archive_sha256':sha,'regular_members':len(ns),'manifest_payloads':len(entries),'manifest_sha256':hashlib.sha256(md).hexdigest(),'crc':'PASS','all_payload_hashes':'PASS','safe_paths_and_modes':'PASS','tested_source':m['tested_source'],'accepted_evidence_checkpoint':m['accepted_evidence_checkpoint'],'official_pin':m['official_pin'],'git_provenance_recreated':False,'supplied_secret_scan_independently_rerun':False}
  if a.extract:
   a.extract.mkdir(parents=True,exist_ok=True)
   for name in ns:
    dst=a.extract/name; dst.parent.mkdir(parents=True,exist_ok=True)
    require(not dst.exists(), f'already exists: {dst}')
    dst.write_bytes(z.read(name))
   out['extracted_to']=str(a.extract)
  print(json.dumps(out,indent=2))
if __name__=='__main__':main()
