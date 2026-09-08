#!/usr/bin/env python3
"""Independent replay of retained old S100 TSVs; no C++/FFT/encryption. Both original component norm and complex norm."""
from __future__ import annotations
import argparse,json,hashlib,time
from pathlib import Path
from decimal import Decimal as D, localcontext
from fractions import Fraction
from independent_scalar import integer_input,scale_closed,cm,cs,ca,cd,n2,num,require

def inspect(path:Path,precision:int)->dict:
    raw=path.read_bytes();lines=raw.decode('ascii').splitlines()
    require(raw.endswith(b'\n') and b'\r' not in raw,'raw framing')
    require(lines[0]=='#fs-residual-endpoint-01.v1-r1','old magic')
    meta={};checks=[];rows=[];header=False
    for line in lines[1:]:
        t=line.split('\t')
        if t[0]=='meta':
            require(not header and len(t)==3 and t[1] not in meta,'duplicate metadata');meta[t[1]]=t[2]
        elif t[0]=='check':
            require(not header and len(t)==9,'check shape');checks.append(t)
        elif t[0]=='slot':
            require(not header and line=='slot\tE0.real\tE0.imag\tE8.real\tE8.imag','columns');header=True
        else:
            require(header and len(t)==5 and t[0]==str(len(rows)),'old slot row');rows.append(tuple(num(x) for x in t[1:]))
    statuspath=path.with_suffix('.status.json');status=json.loads(statuspath.read_text())
    require(len(raw)==status['canonical_bytes'] and hashlib.sha256(raw).hexdigest()==status['canonical_sha256'],'old raw hash')
    for key,value in {'openfhe_pin':'df495ba2e91739a6dc8f1de254fc5a41155ce504','source_commit':'ed5fd192a89d6d4728ad295e87cf06a3f4abc832','github_run_id':'34039088536','github_run_attempt':'1','slots':'16384','input_formula':'frozen-four-phase-exact-dyadic-v1','norm':'max-real-imag-component','chain_count':'1'}.items():require(meta[key]==value,'old binding '+key)
    require(len(rows)==16384 and len(checks)==24 and len({x[1] for x in checks})==24,'complete rows/checks')
    for k in [0,8]:
        s=scale_closed(k);require(int(meta[f'scale{k}_numerator'])==s.numerator and int(meta[f'scale{k}_denominator'])==s.denominator,'old exact scale')
    require(meta['E80_disposition']=='FAIL' and status['ctest_exit_code']==8 and status['evidence_state']=='COMPLETE','retained old failure')
    with localcontext() as ctx:
        ctx.prec=precision;T=D(2)**-80
        names=('E0','E8','I8','A8'); maxima={x:[D(-1),-1] for x in names};comp={x:[D(-1),-1] for x in names};fails={x:{'complex':0,'component':0} for x in names}
        cross=D(0); radii=[];relative=[D(-1),-1]
        for s,row in enumerate(rows):
            x=tuple(D(i)/D(2**75) for i in integer_input(s,1015));e0=row[:2];e8=row[2:]
            u=ca(x,e0);v=x;factor=(D(1),D(0))
            for _ in range(8):factor=cm(factor,ca(u,v));u=cs(u);v=cs(v)
            i8=cm(e0,factor);a8=cd(e8,i8);direct=cd(u,v)
            cross=max(cross,abs(direct[0]-i8[0]),abs(direct[1]-i8[1]))
            for name,z in zip(names,(e0,e8,i8,a8)):
                sq=n2(z);c=max(abs(z[0]),abs(z[1]))
                if sq>maxima[name][0]:maxima[name]=[sq,s]
                if c>comp[name][0]:comp[name]=[c,s]
                fails[name]['complex']+=sq>T*T;fails[name]['component']+=c>T
            rr=n2(e8)/n2(v)
            if rr>relative[0]:relative=[rr,s]
            radii.append(n2(x))
        require(cross<D(10)**(-precision+5),'factor identity check')
        report={}
        for name in names:
            mod=+maxima[name][0].sqrt();c=comp[name][0]
            report[name]={'complex_modulus':str(mod),'complex_slot':maxima[name][1],'component_maximum':str(c),'component_slot':comp[name][1],
                         'absolute_complex_bits':str(-mod.ln()/D(2).ln()),'complex_ratio_to_T':str(mod/T),'component_ratio_to_T':str(c/T)}
        # The reverse triangle inequality does NOT prove exact contributions; this is only a necessary cancellation bound.
        defect_free_endpoint_failure=report['I8']
        return {'scope':'retained real old sample, newly replayed pure scalar arithmetic','platform':meta['host'],'precision_decimal_digits':precision,
                'raw_sha256':hashlib.sha256(raw).hexdigest(),'raw_bytes':len(raw),'source_commit':meta['source_commit'],'original_production_source':meta['production_source'],
                'row_count':len(rows),'retained_E80':'FAIL','recorded_ctest_exit':status['ctest_exit_code'],'observer_claim':'CONDITIONAL PASS, not re-proved here',
                'maxima':report,'slots_above_T':fails,'independent_factor_identity_difference':str(cross),
                'original_gate_norm':'max-real-imag-component (preserved, not retrospectively replaced)',
                'domain':{'minimum_radius':str(min(radii).sqrt()),'maximum_radius':str(max(radii).sqrt()),
                          'min_ideal_output_modulus':str(min(radii)**128),'max_ideal_output_modulus':str(max(radii)**128),
                          'min_local_absolute_gain':str(D(256)*min(radii)**127*min(radii).sqrt()),'max_local_absolute_gain':str(D(256)*max(radii)**127*max(radii).sqrt())},
                'worst_relative_error':{'slot':relative[1],'error':str(relative[0].sqrt()),'bits':str(-relative[0].sqrt().ln()/D(2).ln())},
                'all_reported_observer_checks_PASS':all(x[2]=='PASS' for x in checks),
                'no_new_encryption':True,'formal_error_proof':False}

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--packet',required=True,type=Path);ap.add_argument('--precision',required=True,type=int);ap.add_argument('--output',required=True,type=Path);a=ap.parse_args()
    require(a.precision in [180,230],'bounded two precisions');start=time.monotonic()
    out={p:inspect(next((a.packet/'evidence/coordination/fs-endpoint-live-run-01'/p).glob('*.tsv')),a.precision) for p in ['linux','windows']}
    out['elapsed_scalar_seconds']=time.monotonic()-start;a.output.write_text(json.dumps(out,indent=2)+'\n')
    print(json.dumps({p:{'retained_E80':out[p]['retained_E80'],'maxima':{k:{'slot':v['complex_slot'],'complex':v['complex_modulus'][:26],'component':v['component_maximum'][:26]} for k,v in out[p]['maxima'].items()},'slots_above_T':out[p]['slots_above_T']} for p in ['linux','windows']},indent=2))
if __name__=='__main__':main()
