#!/usr/bin/env python3
"""Read-only result/source/claim cross-check, plus replay of author-only scripts.
Never runs candidate transform functions, candidate transform TDD, or C++.
"""
from __future__ import annotations
import argparse
import ast
import csv
from datetime import datetime, timezone
from fractions import Fraction as F
import hashlib
import json
from pathlib import Path
import subprocess
import sys
if hasattr(sys,'set_int_max_str_digits'):sys.set_int_max_str_digits(200000)

def need(value,message):
    if not value:raise ValueError(message)

def main():
    a=argparse.ArgumentParser(description=__doc__)
    a.add_argument('--input-dir',required=True,type=Path)
    a.add_argument('--replay-dir',required=True,type=Path)
    a.add_argument('--out',required=True,type=Path)
    args=a.parse_args();root=Path(__file__).resolve().parents[1]
    need(not args.replay_dir.exists(),'use a fresh external replay directory')
    need(not args.replay_dir.resolve().is_relative_to(args.input_dir.resolve()),'replay may not be inside input')
    args.replay_dir.mkdir(parents=True)
    calls=[]
    commands=[
       ['checks/run_checks.py','--input-dir',str(root/'checks/bound_source'),'--out-dir',str(args.replay_dir/'results')],
       ['checks/check_candidate_scalars.py','--out',str(args.replay_dir/'results/candidate_scalar_checks.json')],
       ['checks/check_candidate_static.py','--input-dir',str(root/'checks/bound_source'),'--out',str(args.replay_dir/'results/candidate_static_checks.json')]
    ]
    for number,c in enumerate(commands,1):
        cmd=[sys.executable,'-B',str(root/c[0]),*c[1:]]
        began=datetime.now(timezone.utc).isoformat()
        process=subprocess.run(cmd,capture_output=True,text=False,timeout=45,check=False)
        (args.replay_dir/f'command{number}_stdout.txt').write_bytes(process.stdout)
        (args.replay_dir/f'command{number}_stderr.txt').write_bytes(process.stderr)
        calls.append({'command':cmd,'exit':process.returncode,'started_utc':began,'ended_utc':datetime.now(timezone.utc).isoformat(),
          'stdout_sha256':hashlib.sha256(process.stdout).hexdigest(),'stderr_sha256':hashlib.sha256(process.stderr).hexdigest()})
        need(process.returncode==0,'bounded replay failed '+c[0])
    result_files=sorted(p.name for p in (root/'checks/results').iterdir() if p.is_file())
    need(len(result_files)==7,'expected seven result files including summary')
    for name in result_files:
        need((root/'checks/results'/name).read_bytes()==(args.replay_dir/'results'/name).read_bytes(),'non-deterministic result '+name)
    claims=json.loads((root/'CLAIMS.json').read_text())
    ids={c['id'] for c in claims['claims']};assumptions={a['id'] for a in claims['assumptions']}
    need(len(ids)==20 and len(assumptions)==6,'claim/assumption count')
    graph={c['id']:c['depends_on'] for c in claims['claims']}
    seen=set();stack=set()
    def visit(i):
        need(i in ids,'missing claim dependency')
        need(i not in stack,'cyclic claim dependency')
        if i in seen:return
        stack.add(i)
        for j in graph[i]:visit(j)
        stack.remove(i);seen.add(i)
    for i in ids:visit(i)
    source_rows=list(csv.DictReader((root/'SOURCE_MAP.tsv').open(encoding='utf-8'),delimiter='\t'))
    source_ids={r['source_id'] for r in source_rows}
    need(len(source_rows)==51 and len(source_ids)==51,'source map row count')
    requests=[json.loads(line) for line in (root/'evidence/READ_REQUESTS.jsonl').read_text().splitlines()]
    pdf=json.loads((root/'evidence/PDF_READ_RECORD.json').read_text())
    for r in source_rows:
        b=(args.input_dir/r['input_path']).read_bytes()
        need(hashlib.sha256(b).hexdigest()==r['sha256'],'source hash '+r['source_id'])
        for c in r['claims'].split(','):need(c in ids,'unknown source-map claim '+c)
        if r['physical_pdf_page']:
            need(int(r['physical_pdf_page']) in pdf['physical_pages_visually_inspected'],'unrecorded PDF page')
        else:
            start,end=int(r['start_line']),int(r['end_line'])
            need(1<=start<=end<=len(b.decode('utf-8').splitlines()),'source range exceeds file')
            covered=set()
            for req in requests:
                if req['path']==r['input_path'] and req['sha256']==r['sha256']:
                    covered.update(range(req['start_line'],req['end_line']+1))
            need(set(range(start,end+1))<=covered,'source range not in requested read coverage '+r['source_id'])
    for c in claims['claims']:
        need(set(c['assumptions'])<=assumptions,'unknown assumption')
        need(set(c['source_ids'])<=source_ids,'unknown source citation')
        for reference in c['check_evidence']:
            name,*fragments=reference.split('#',1);path=root/name;need(path.is_file(),'missing evidence '+name)
            if fragments:
                obj=json.loads(path.read_text())
                for part in fragments[0].strip('/').split('/'):
                    obj=obj[int(part)] if isinstance(obj,list) else obj[part]
    snap=json.loads((root/'evidence/SNAPSHOT_ORIGINS.json').read_text())
    for s in snap:
        data=(root/s['output_path']).read_bytes()
        need(data==(args.input_dir/s['input_path']).read_bytes(),'snapshot changed')
        need(hashlib.sha256(data).hexdigest()==s['sha256'],'snapshot identity')
    rationals=0
    def scan(x):
        nonlocal rationals
        if isinstance(x,dict):
            if 'numerator' in x and 'denominator' in x and 'decimal_enclosure' in x:
                v=F(int(x['numerator']),int(x['denominator']));d=x['decimal_enclosure']
                need(F(d['lower'])<=v<=F(d['upper']),'decimal display not outward');rationals+=1
                if v>0 and 'floor_log2' in x:
                    e=x['floor_log2'];need(F(2)**e<=v<F(2)**(e+1),'log2 bound mismatch')
            for v in x.values():scan(v)
        elif isinstance(x,list):
            for v in x:scan(v)
    for name in result_files:
        if name.endswith('.json'):scan(json.loads((root/'checks/results'/name).read_text()))
    results=json.loads((root/'checks/results/results.json').read_text())
    need(results['conditional_C25_first_failure']==5,'wrong first coarse failure')
    need(results['conditional_refined_all_phase_checks'] is True,'conditional certificate not passing')
    need(results['actual_encoder_premise_verified'] is False,'actual encoder premise mislabelled')
    need(results['K1_negative_control_first_failure']==8,'wrong negative control')
    need(all(v is None for v in claims['unknown_values'].values()),'unknown values became invented observations')
    pycount=0
    for p in root.rglob('*.py'):
        ast.parse(p.read_text(encoding='utf-8'),filename=str(p));pycount+=1
    need(not any(root.rglob('__pycache__')),'bytecode cache entered payload')
    report={'status':'PASS','author_only_replay_commands':calls,'byte_identical_result_files':result_files,
        'claim_dependency_graph_acyclic':True,'claims':20,'assumptions':6,'source_rows_hash_range_and_read_request_coverage':51,
        'original_byte_snapshots_verified':len(snap),'outward_decimal_rational_records':rationals,
        'Python_files_AST_parsed_not_compiled':pycount,'actual_encoder_cap_still_unknown':True,
        'candidate_transform_TDD_executed':False,'CXX_compiled_or_run':False,'FFT_NTT_or_sampling_calls':0,
        'input_project_writes':0}
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8',newline='\n')
    print(json.dumps({k:v for k,v in report.items() if k!='author_only_replay_commands'},ensure_ascii=False))

if __name__=='__main__':main()
