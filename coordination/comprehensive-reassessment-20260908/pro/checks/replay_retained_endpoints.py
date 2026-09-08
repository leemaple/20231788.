#!/usr/bin/env python3
"""Scalar-only replay of the two retained TSVs; no project imports/FFT/decrypt.
TSV errors are treated as supplied decimal observations, NOT newly certified truth.
Run separately at two precisions to expose precision-sensitive scalar arithmetic.
"""
from __future__ import annotations
import argparse, hashlib, json
from decimal import Decimal as D, localcontext
from pathlib import Path
from scalar_reassessment import dyadic_input, require


def eighth_power(z: tuple[D,D]) -> tuple[D,D]:
    a,b=z
    for _ in range(8):
        a,b=a*a-b*b,2*a*b
    return a,b


def sub(a: tuple[D,D], b: tuple[D,D]) -> tuple[D,D]:
    return a[0]-b[0],a[1]-b[1]


def replay(root: Path, host: str, digits: int) -> dict:
    folder=root/'evidence/coordination/fs-endpoint-live-run-01'/host
    matches=list(folder.glob('*.tsv')); require(len(matches)==1,'one retained TSV')
    path=matches[0]
    status_path=next(folder.glob('*.status.json'))
    status=json.loads(status_path.read_text())
    digest=hashlib.sha256(path.read_bytes()).hexdigest()
    require(status['canonical_sha256']==digest,'retained canonical SHA')
    require(status['E80_disposition']=='FAIL' and status['ctest_exit_code']==8,
            'preserve actual FAIL')
    records=[]; metadata={}
    for line in path.read_text().splitlines():
        fields=line.split('\t')
        if fields[0]=='meta': metadata[fields[1]]=fields[2]
        elif fields[0].isdigit():
            require(len(fields)==5,'five TSV columns')
            records.append((int(fields[0]),tuple(D(v) for v in fields[1:])))
    require(len(records)==16384,'complete full-slot record count')
    with localcontext() as c:
        c.prec=digits
        denom=D(2)**75; T=D(2)**-80
        comp={name:(D(-1),-1,-1) for name in ('E0','E8','I8','A8')}
        norm2={name:(D(-1),-1) for name in comp}
        E8_exceed_component=0; E8_exceed_complex=0
        endpoint_argmax=None
        for expected,(slot,v) in enumerate(records):
            require(slot==expected,'ordered unique slots')
            x=tuple(D(n)/denom for n in dyadic_input(slot,1015))
            e0=(v[0],v[1]); e8=(v[2],v[3])
            y0=x[0]+e0[0],x[1]+e0[1]
            ideal=eighth_power(x); freshprop=eighth_power(y0)
            inherited=sub(freshprop,ideal); added=sub(e8,inherited)
            values={'E0':e0,'E8':e8,'I8':inherited,'A8':added}
            for name,z in values.items():
                for k in range(2):
                    if z[k].copy_abs()>comp[name][0]:
                        comp[name]=(z[k].copy_abs(),slot,k)
                        if name=='E8':
                            endpoint_argmax={'slot':slot,'component':('real','imag')[k],
                                'E8':str(e8[k]),'I8':str(inherited[k]),'A8':str(added[k])}
                n=z[0]*z[0]+z[1]*z[1]
                if n>norm2[name][0]: norm2[name]=(n,slot)
            E8_exceed_component+=sum(z.copy_abs()>T for z in e8)
            E8_exceed_complex+=(e8[0]*e8[0]+e8[1]*e8[1]>T*T)
        return {'host':host,'precision_decimal_digits':digits,
            'input_tsv':str(path.relative_to(root)),'input_sha256':digest,
            'input_status_sha256':hashlib.sha256(status_path.read_bytes()).hexdigest(),
            'source_commit':metadata['source_commit'],'run':metadata['github_run_id'],
            'rows':len(records),'ctest_exit_code':status['ctest_exit_code'],
            'legacy_numeric_gate_failures':status['numeric_gate_failures'],
            'norm_component_max':{k:{'value':str(v[0]),'slot':v[1],
                'component':('real','imag')[v[2]],'ratio_to_T':str(v[0]/T)} for k,v in comp.items()},
            'norm_complex_max':{k:{'value':str(v[0].sqrt()),'slot':v[1],
                'ratio_to_T':str(v[0].sqrt()/T)} for k,v in norm2.items()},
            'same_component_at_E8_component_argmax':endpoint_argmax,
            'E8_exceeding_components':E8_exceed_component,
            'E8_exceeding_complex_slots':E8_exceed_complex,
            'assurance':'scalar replay conditional on supplied observer TSV; no new ciphertext run; counts are within these samples, not population rates'}


def main() -> int:
    ap=argparse.ArgumentParser();ap.add_argument('input_root',type=Path)
    ap.add_argument('--digits',type=int,default=150);args=ap.parse_args()
    require(args.digits>=120,'at least 120 decimal digits')
    print(json.dumps([replay(args.input_root,h,args.digits) for h in ('linux','windows')],
                     ensure_ascii=False,indent=2))
    return 0

if __name__=='__main__':raise SystemExit(main())
