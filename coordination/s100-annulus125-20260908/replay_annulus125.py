#!/usr/bin/env python3
"""Strict scalar receiver for the UNEXECUTED C++ candidate's future TSV.
No encryption, FFT, project imports, or fabricated live result. --self-test uses
in-memory synthetic zero-error rows ONLY, never labels them as a real run.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import re
import sys
from decimal import Decimal as D, localcontext
from fractions import Fraction
from pathlib import Path
from scalar_reassessment import dyadic_input

# Eight squarings produce ~7707-digit exact scale integers; keep a finite cap.
if hasattr(sys, 'set_int_max_str_digits'): sys.set_int_max_str_digits(10000)

PIN='df495ba2e91739a6dc8f1de254fc5a41155ce504'
CONTRACT='s100-annulus125-e80-v1'
HEADER='slot\tE0_obs.real\tE0_obs.imag\tE8_obs.real\tE8_obs.imag\tE8_prod.real\tE8_prod.imag'
Q=[1125899904679937,1125899903827969,1152921504598720513,
   1152921504597016577,1152921504595968001,1152921504595640321,
   1152921504593412097,1152921504592822273,1152921504592429057,
   1152921504589938689,1099510054913]

def need(ok: bool, label: str) -> None:
    if not ok: raise ValueError(label)

def number(s: str) -> D:
    x=D(s); need(x.is_finite(),'nonfinite decimal'); return x

def mul(a: tuple[D,D],b: tuple[D,D]) -> tuple[D,D]:
    return a[0]*b[0]-a[1]*b[1], a[0]*b[1]+a[1]*b[0]

def inherited(x: tuple[D,D],delta: tuple[D,D]) -> tuple[D,D]:
    for _ in range(8):
        xd=mul(x,delta); dd=mul(delta,delta)
        delta=(2*xd[0]+dd[0],2*xd[1]+dd[1]); x=mul(x,x)
    return delta

def replay(text: str, commit: str, process_exit: int, precision: int=180) -> dict:
    need(re.fullmatch('[0-9a-f]{40}',commit) is not None,'invalid expected commit')
    need(text.endswith('\n') and '\r' not in text and '\x00' not in text,
         'invalid LF-terminated TSV framing')
    with localcontext() as ctx:
        ctx.prec=precision
        lines=text.splitlines(); need(bool(lines) and lines[0]=='#'+CONTRACT,'wrong contract')
        T=D(2)**-80; eps=D(2)**-300
        meta={}; scales={}; agreements={}; declared={}; gates={}; rows=[]; status=None; header=False
        for line in lines[1:]:
            need(status is None,'trailing data after final status')
            p=line.split('\t'); tag=p[0]
            if tag=='meta':
                need(len(p)==3 and p[1] not in meta,'duplicate/malformed metadata'); meta[p[1]]=p[2]
            elif tag=='scale':
                need(len(p)==4 and int(p[1]) not in scales,'duplicate scale')
                need(int(p[3])>0,'scale denominator'); scales[int(p[1])]=Fraction(int(p[2]),int(p[3]))
            elif tag=='agreement':
                need(len(p)==3 and p[1] not in agreements,'duplicate agreement'); agreements[p[1]]=number(p[2])
            elif tag=='slot':
                need(line==HEADER and not header,'slot header'); header=True
            elif tag=='max':
                need(len(p)==4 and p[1] not in declared,'duplicate maximum'); declared[p[1]]=(int(p[2]),number(p[3]))
            elif tag=='gate':
                need(len(p)==3 and p[1] not in gates and p[2] in ('0','1'),'gate shape'); gates[p[1]]=p[2]=='1'
            elif tag=='status':
                need(len(p)==3 and p[1]=='COMPLETE' and p[2] in ('PASS','FAIL'),'completion status'); status=p[2]
            else:
                need(header and tag.isdigit() and len(p)==7 and not declared,'unexpected row')
                need(int(tag)==len(rows),'missing, duplicate, or permuted row'); rows.append(tuple(number(v) for v in p[1:]))
        expected={'source_commit':commit,'openfhe_pin':PIN,'input_formula':'four-phase-999-dyadic-v1',
            'norm':'max-complex-modulus','assurance':'CONDITIONAL_OBSERVER_NOT_FORMAL',
            'security':'UNRESOLVED','legacy_S100_stress':'FAIL_RETAINED',
            'chain_count':'1','squares':'8','slots':'16384'}
        need(meta==expected,'metadata mismatch'); need(status is not None and len(rows)==16384,'incomplete rows')
        need(set(agreements)=={'fresh','terminal'} and all(0<=v<=D(2)**-120 for v in agreements.values()),'observer agreement')
        expected_scales={0:Fraction(2**100)}
        for k in range(1,9): expected_scales[k]=expected_scales[k-1]**2/(Q[-1]*Q[10-k])
        need(scales==expected_scales,'actual-prime scale mismatch')
        names=['E0_obs','E8_obs','E8_prod','I8_obs','A8_obs']; maxima={k:(0,D(0)) for k in names}
        terminal_component_disagreement=D(0)
        for s,row in enumerate(rows):
            a,b=dyadic_input(s,999); x=(D(a)/2**75,D(b)/2**75)
            e0=row[:2]; e8=row[2:4]; prod=row[4:]; i8=inherited(x,e0); a8=(e8[0]-i8[0],e8[1]-i8[1])
            terminal_component_disagreement=max(terminal_component_disagreement,
                abs(prod[0]-e8[0]),abs(prod[1]-e8[1]))
            for key,z in zip(names,(e0,e8,prod,i8,a8)):
                n=z[0]*z[0]+z[1]*z[1]
                if n>maxima[key][1]: maxima[key]=(s,n)
        need(set(declared)==set(names),'maximum names')
        for key,(s,n) in maxima.items():
            ds,dv=declared[key]; need(ds==s and abs(dv-n.sqrt())<eps,'maximum mismatch: '+key)
        for key,bound in [('E0_obs',T),('A8_obs',T/4),('E8_obs',T),('E8_prod',T)]:
            need(abs(maxima[key][1].sqrt()-bound)>eps,'numerically ambiguous threshold: '+key)
        witness_error=(rows[1][4]-rows[0][4],rows[1][5]-rows[0][5])
        witness_norm2=sum(v*v for v in witness_error)
        need(abs(witness_norm2.sqrt()-2*T)>eps,'ambiguous witness threshold')
        actual={'E0_le_T':maxima['E0_obs'][1]<=T*T,'A8_le_T_over_4':maxima['A8_obs'][1]<=T*T/16,
                'E8_obs_le_T':maxima['E8_obs'][1]<=T*T,'E8_prod_le_T':maxima['E8_prod'][1]<=T*T,
                'witness':witness_norm2<=4*T*T}
        need(gates==actual,'recorded gates disagree with independent replay')
        # Match C++ ComponentDistance, not the scientific complex-norm gates.
        # Its agreement is a max over producer, cross-precision and Horner,
        # so row disagreement is bounded by it but need not equal it. eps=2^-300
        # covers decimal serialization subtraction, far below the 2^-120 gate.
        need(terminal_component_disagreement<=D(2)**-120 and
             terminal_component_disagreement<=agreements['terminal']+eps,
             'terminal producer/observer disagreement')
        decision='PASS' if all(actual.values()) else 'FAIL'
        need(status==decision and process_exit==(0 if decision=='PASS' else 1),'process/footer disagreement')
        return {'contract':CONTRACT,'source_commit':commit,'status':decision,'scope':'scalar replay of supplied TSV; not proof of runtime provenance',
                'decimal_precision':precision,'rows':len(rows),'gates':actual,
                'terminal_component_disagreement':str(terminal_component_disagreement),
                'fresh_agreement_scope':'source-bound runtime check; no E0_prod columns to replay',
                'maxima':{k:{'slot':s,'complex_modulus':str(n.sqrt())} for k,(s,n) in maxima.items()}}

def synthetic() -> str:
    commit='0'*40
    out=['#'+CONTRACT]
    for k,v in {'source_commit':commit,'openfhe_pin':PIN,'input_formula':'four-phase-999-dyadic-v1',
        'norm':'max-complex-modulus','assurance':'CONDITIONAL_OBSERVER_NOT_FORMAL','security':'UNRESOLVED',
        'legacy_S100_stress':'FAIL_RETAINED','chain_count':'1','squares':'8','slots':'16384'}.items(): out.append(f'meta\t{k}\t{v}')
    scale=Fraction(2**100)
    for k in range(9):
        if k: scale=scale**2/(Q[-1]*Q[10-k])
        out.append(f'scale\t{k}\t{scale.numerator}\t{scale.denominator}')
    out+=['agreement\tfresh\t0','agreement\tterminal\t0',HEADER]
    out += [str(s)+'\t0\t0\t0\t0\t0\t0' for s in range(16384)]
    out += ['max\t'+k+'\t0\t0' for k in ('E0_obs','E8_obs','E8_prod','I8_obs','A8_obs')]
    out += ['gate\t'+k+'\t1' for k in ('E0_le_T','A8_le_T_over_4','E8_obs_le_T','E8_prod_le_T','witness')]
    out+=['status\tCOMPLETE\tPASS']; return '\n'.join(out)+'\n'

def self_test() -> dict:
    text=synthetic(); result=replay(text,'0'*40,0); need(result['status']=='PASS','synthetic positive')
    # Keep this negative's maxima correct, so only the complex gate catches it.
    with localcontext() as ctx:
        ctx.prec=180
        z=D('6.2e-25'); complex_max=(2*z*z).sqrt()
    complex_case=text.replace('1024\t0\t0\t0\t0\t0\t0\n',
        '1024\t0\t0\t0\t0\t6.2e-25\t6.2e-25\n').replace(
        'max\tE8_prod\t0\t0\n',f'max\tE8_prod\t1024\t{complex_max}\n')
    variants={
        'missing_slot':text.replace('1024\t0\t0\t0\t0\t0\t0\n',''),
        'component_pass_complex_fail':complex_case,
        'wrong_norm':text.replace('max-complex-modulus','max-component'),
        'duplicate_footer':text+'status\tCOMPLETE\tPASS\n',
        'truncated':text.replace('status\tCOMPLETE\tPASS\n','')}
    rejected=[]
    for label,mutated in variants.items():
        try: replay(mutated,'0'*40,0)
        except ValueError as exc:
            if label=='component_pass_complex_fail':
                need(str(exc)=='recorded gates disagree with independent replay',
                     'complex negative must reach the independent gate, not fail metadata/maxima')
            rejected.append(label)
        else: raise RuntimeError('negative not rejected: '+label)
    for label,c,e in [('wrong_source','1'*40,0),('nonzero_exit','0'*40,1)]:
        try: replay(text,c,e)
        except ValueError: rejected.append(label)
        else: raise RuntimeError('negative not rejected: '+label)
    return {'status':'PASS_SYNTHETIC_PARSER_ONLY','positive':1,'negative_rejections':rejected,
            'live_encrypted_runs':0,'not_a_ciphertext_result':True}

def main() -> int:
    ap=argparse.ArgumentParser(); ap.add_argument('--self-test',action='store_true')
    ap.add_argument('--tsv',type=Path); ap.add_argument('--source-commit'); ap.add_argument('--process-exit',type=int)
    ap.add_argument('--precision',type=int,default=180); args=ap.parse_args()
    if args.self_test: result=self_test()
    else:
        need(args.tsv is not None and args.source_commit is not None and args.process_exit is not None,'required TSV/source/process exit')
        need(args.precision>=150,'minimum 150 decimal digits')
        raw=args.tsv.read_bytes(); result=replay(raw.decode('utf-8'),args.source_commit,args.process_exit,args.precision)
        result['input_sha256']=hashlib.sha256(raw).hexdigest()
    print(json.dumps(result,indent=2)); return 1 if result['status']=='FAIL' else 0
if __name__=='__main__': raise SystemExit(main())
