#!/usr/bin/env python3
"""Post-first-pass regression of immutable historical/current receivers.

Only in-memory mutations of the retained TSV. They are SYNTHETIC, not real runs.
Original bytes and remote absolute path receipts are never rewritten.
"""
from __future__ import annotations
import argparse,hashlib,importlib.util,json,sys
from pathlib import Path
from independent_scalar import COMMIT,integer_input

def main()->int:
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--packet',type=Path,required=True)
    ap.add_argument('--target',choices=['historical','current'],required=True)
    ap.add_argument('--output',type=Path,required=True)
    a=ap.parse_args()
    base=a.packet/'project/coordination/s100-annulus125-20260908'
    old=a.packet/'after-first-pass/coordination/comprehensive-reassessment-20260908/pro/checks'
    path=(old if a.target=='historical' else base)/'replay_annulus125.py'
    # Both audited receiver revisions import exactly the same helper bytes.
    assert (base/'scalar_reassessment.py').read_bytes()==(old/'scalar_reassessment.py').read_bytes()
    sys.path.insert(0,str(base))
    spec=importlib.util.spec_from_file_location('reviewed_receiver_target',path)
    mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod)
    p=base/'experiment-evidence/sample/raw.tsv';raw=p.read_bytes();text=raw.decode('ascii')
    assert mod.replay(text,COMMIT,0,180)['status']=='PASS'
    lines=text.splitlines()
    for i,line in enumerate(lines):
        fields=line.split('\t')
        if fields[0]=='1024':
            assert len(fields)==7;fields[5:7]=['4e-25','0'];lines[i]='\t'.join(fields)
        if line.startswith('max\tE8_prod\t'):
            lines[i]='max\tE8_prod\t1024\t4e-25'
    variants={'inconsistent_terminal_producer':'\n'.join(lines)+'\n',
              'missing_final_LF':text[:-1], 'CRLF':text.replace('\n','\r\n')}
    outcomes={}
    for name,mutant in variants.items():
        try:
            result=mod.replay(mutant,COMMIT,0,180)
        except ValueError as exc:
            outcomes[name]={'accepted':False,'error':str(exc)}
        else:
            outcomes[name]={'accepted':True,'returned_status':result['status']}
        outcomes[name]['synthetic_bytes_sha256']=hashlib.sha256(mutant.encode()).hexdigest()
    assert p.read_bytes()==raw
    norms={ai*ai+bi*bi for ai,bi in (integer_input(s) for s in range(16384))}
    out={'scope':'post-freeze synthetic receiver regression; no new ciphertexts',
         'target':a.target,'target_relative_path':str(path.relative_to(a.packet)),
         'target_sha256':hashlib.sha256(path.read_bytes()).hexdigest(),
         'unchanged_helper_sha256':hashlib.sha256((base/'scalar_reassessment.py').read_bytes()).hexdigest(),
         'original_raw_sha256':hashlib.sha256(raw).hexdigest(),'real_record_positive_PASS':True,
         'mutants_are_not_actual_evidence':True,'mutant_tests':outcomes,'new_encrypted_runs':0,
         'additional_exact_domain_check':{'distinct_input_squared_moduli':len(norms),
           'implies_distinct_ideal_outputs':len(norms)==16384,
           'reason':'positive |x|^2 all distinct; |x^256|=(|x|^2)^128 is strictly increasing'},
         'status':'EXPECTED_RED_HISTORICAL_FALSE_ACCEPT' if any(o['accepted'] for o in outcomes.values()) else 'GREEN_ALL_MUTANTS_REJECTED'}
    a.output.write_text(json.dumps(out,ensure_ascii=False,indent=2)+'\n')
    print(json.dumps(out,ensure_ascii=False,indent=2))
    return 1 if any(o['accepted'] for o in outcomes.values()) else 0
if __name__=='__main__':raise SystemExit(main())
