#!/usr/bin/env python3
"""Byte/provenance consistency, NOT remote attestation. Reads packet and hashes; no external access/build."""
from __future__ import annotations
import argparse,datetime,hashlib,json,stat,zipfile
from pathlib import Path,PurePosixPath
from decimal import Decimal as D,localcontext
from independent_scalar import COMMIT,PIN,Q,scale_closed,require
ARCHIVE_SHA='c30b6e84a53712792cfda4ec17da7b434ee745aebad3949916a90fa6d528a0b1'
MANIFEST_SHA='6cbd05178b788ead26ea12c1032c6fa8f22cad77e65488886a8d1b8f74fbbcdc'

def sha(b):return hashlib.sha256(b).hexdigest()
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--zip',required=True,type=Path);ap.add_argument('--packet',required=True,type=Path);ap.add_argument('--results',required=True,type=Path);ap.add_argument('--output',required=True,type=Path);a=ap.parse_args()
 archive=a.zip.read_bytes();require(len(archive)==13743164 and sha(archive)==ARCHIVE_SHA,'archive identity')
 checked=[];blobs=0
 with zipfile.ZipFile(a.zip) as z:
  names=z.namelist();require(len(names)==310 and len(set(names))==310,'310 unique members');require(z.testzip() is None,'CRC')
  for item in z.infolist():
   p=PurePosixPath(item.filename);mode=item.external_attr>>16
   require(not item.is_dir() and not p.is_absolute() and '..' not in p.parts and not stat.S_ISLNK(mode),'regular safe path')
  mb=z.read('MANIFEST.json');require(sha(mb)==MANIFEST_SHA,'input manifest identity');m=json.loads(mb)
  require(m['manifest_self_excluded'] and m['source_commit']==COMMIT and m['task_evidence_commit']=='695a951a7379d355cf9d167cd14c9a17d9d25199','manifest binding')
  require({x['path'] for x in m['files']}==set(names)-{'MANIFEST.json'},'self-excluding exact membership')
  for entry in m['files']:
   b=z.read(entry['path']);require(len(b)==entry['bytes'] and sha(b)==entry['sha256'],'member identity '+entry['path'])
   require((a.packet/entry['path']).read_bytes()==b,'extracted bytes unchanged')
   origin=entry.get('origin',{});blob=origin.get('git_blob')
   if blob:
    require(hashlib.sha1(f'blob {len(b)}\0'.encode()+b).hexdigest()==blob,'git blob framing');blobs+=1
   checked.append({'path':entry['path'],'bytes':len(b),'sha256':sha(b),'git_blob_verified':bool(blob)})
 p=a.packet/'project/coordination/s100-annulus125-20260908';s=p/'experiment-evidence/sample';v=json.loads((s/'verification.json').read_text())
 names5=['program-start.json','program-end.json','raw.tsv','stdout.txt','stderr.txt']
 five={n:sha((s/n).read_bytes()) for n in names5};require(five==v['evidence_sha256'],'five evidence hashes')
 checksums={}
 for file in ['EXPERIMENT_FILE_SHA256.txt','COMPILE_FILE_SHA256.txt']:
  count=0
  for line in (p/file).read_text().splitlines():
   digest,path=line.split(maxsplit=1);require(sha((a.packet/'project'/path).read_bytes())==digest,'file SHA inventory');count+=1
  checksums[file]=count
 start=json.loads((s/'program-start.json').read_text());end=json.loads((s/'program-end.json').read_text())
 run=json.loads((p/'EXPERIMENT_RUN_FINAL.json').read_text());job=run['jobs'][0]
 require(start['source_commit']==end['source_commit']==run['headSha']==COMMIT,'source binding')
 require(run['databaseId']==34184869227 and run['attempt']==1 and job['databaseId']==101931070901 and run['event']=='push','run/job/event binding')
 require(run['status']=='completed' and run['conclusion']=='success' and end['returncode']==0 and end['timed_out'] is False,'process success')
 require(len(run['jobs'])==1 and len([t for t in job['steps'] if t['name']=='Invoke the frozen encrypted sample exactly once' and t['conclusion']=='success'])==1,'one payload step')
 expected='/home/runner/work/20231788./20231788./'
 require(start['argv']==[expected+'.ci/s100-annulus125-once-build-34184869227-1/s100_annulus125_eight_square_test','--output',expected+'artifacts/s100-annulus125-once-34184869227-1/sample/raw.tsv'],'untouched remote paths')
 require(start['timeout_seconds']==1200 and (s/'stderr.txt').read_bytes()==b'','timeout/stderr')
 require((s/'stdout.txt').read_bytes()==b's100-annulus125-e80-v1 status=COMPLETE result=PASS chain_count=1 squares=8 slots=16384 security=UNRESOLVED\n','stdout banner')
 elapsed=(datetime.datetime.fromisoformat(end['ended_utc'])-datetime.datetime.fromisoformat(start['started_utc'])).total_seconds()
 comparisons={}
 with localcontext() as c:
  c.prec=260
  ra=json.loads((a.results/'independent_180.json').read_text());rb=json.loads((a.results/'independent_230.json').read_text())
  for n in ra['maxima']:
   aa=ra['maxima'][n];bb=rb['maxima'][n];remote=v['replays'][1]['maxima'][n]
   require(aa['slot']==bb['slot']==remote['slot'],'max slot consistency')
   cross=abs(D(aa['complex_modulus'])-D(bb['complex_modulus']));remote_diff=abs(D(bb['complex_modulus'])-D(remote['complex_modulus']))
   require(cross<D(2)**-300 and remote_diff<D(2)**-300,'independent comparison')
   comparisons[n]={'cross_precision_abs_difference':str(cross),'independent_vs_supplied_verifier_difference':str(remote_diff)}
  ratio=D(scale_closed(8).numerator)/D(scale_closed(8).denominator)/D(2**100)
  scale={'S8_over_S0':str(ratio),'relative_drift':str(ratio-1),'using_nominal_scale_would_be_wrong':True}
 out={'scope':'local byte and declared-record consistency, not authenticated remote execution', 'checked_at_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
  'archive_bytes':len(archive),'archive_sha256':sha(archive),'input_manifest_sha256':sha(mb),'regular_members':len(names),'manifest_payloads':len(checked),'CRC':'PASS','payload_hashes':'PASS','git_blob_hashes_checked':blobs,
  'input_source_commit':COMMIT,'task_evidence_commit':m['task_evidence_commit'],'openfhe_pin':PIN,'checksums':checksums,'five_evidence_hashes':five,
  'source_process_binding':'PASS_WITH_PACKET_PROVENANCE_ASSUMPTION','process_elapsed_seconds':elapsed,'pure_multiplication_timing':False,'run_id':run['databaseId'],'job_id':job['databaseId'],
  'recorded_openfhe_cache_hit':True,'binary_available_for_rebuild_or_execution':False,'run_attempt':run['attempt'],'cross_checks':comparisons,'exact_scale':scale,
  'not_attested':['commit tree membership beyond supplied blob/origin mapping','cache binary derivation from pristine source','global absence of past/future tag recreation','runner authenticity','observer numerical proof'],
  'members':checked}
 a.output.write_text(json.dumps(out,ensure_ascii=False,indent=2)+'\n');print(json.dumps({k:v for k,v in out.items() if k not in ['members','cross_checks','exact_scale']},ensure_ascii=False,indent=2))
if __name__=='__main__':main()
