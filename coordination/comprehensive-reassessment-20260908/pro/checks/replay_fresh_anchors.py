#!/usr/bin/env python3
"""Independent directed-interval scalar replay of retained A+B (never C).
Condition: EACH A+B real/imaginary value is enclosed by center +/- 1e-100.
This condition is NOT a proof about Boost transcendental routines.
"""
from __future__ import annotations
import argparse, hashlib, json, re
from pathlib import Path
from decimal import Decimal as D, Context, ROUND_FLOOR, ROUND_CEILING, localcontext
from scalar_reassessment import require, dyadic_input
LO=Context(prec=240,rounding=ROUND_FLOOR)
HI=Context(prec=240,rounding=ROUND_CEILING)
I=tuple[D,D]
Z=tuple[I,I]

def add(x:I,y:I)->I:return LO.add(x[0],y[0]),HI.add(x[1],y[1])
def neg(x:I)->I:return x[1].copy_negate(),x[0].copy_negate()
def sub(x:I,y:I)->I:return add(x,neg(y))
def mul(x:I,y:I)->I:
    return min(LO.multiply(a,b) for a in x for b in y),max(HI.multiply(a,b) for a in x for b in y)
def cmul(x:Z,y:Z)->Z:
    return sub(mul(x[0],y[0]),mul(x[1],y[1])),add(mul(x[0],y[1]),mul(x[1],y[0]))
def cadd(x:Z,y:Z)->Z:return add(x[0],y[0]),add(x[1],y[1])
def point(x:D)->I:return x,x

def main()->int:
    ap=argparse.ArgumentParser();ap.add_argument('input_root',type=Path);a=ap.parse_args()
    p=a.input_root/'evidence/coordination/s100-fresh-error-repair-01/remote-green2-diagnostic-steps.log'
    records={}
    for line in p.read_text().splitlines():
        if 'S100 tuple=anchor ' not in line:continue
        row=dict(re.findall(r'(\w+)=([^\s]+)',line))
        slot=int(row['slot']);comp=row['component']
        require((slot,comp) not in records,'duplicate anchor')
        records[slot,comp]=row
    anchors=[0,1,256,257,512,513,768,769,1023,16383]
    require(set(records)=={(s,k) for s in anchors for k in ('real','imag')},'exact 20 components')
    rows=[];exceeded=0
    with localcontext() as c:
        c.prec=240
        T=D(2)**-80;eps=D('1e-100');den=D(2)**75
        for s in anchors:
            x0=tuple(D(n)/den for n in dyadic_input(s,1015))
            for k,name in enumerate(('real','imag')):
                require(D(records[s,name]['z'])==x0[k],'literal input independent dyadic identity')
            x=(point(x0[0]),point(x0[1]))
            errors=[D(records[s,k]['encoding'])+D(records[s,k]['encryption']) for k in ('real','imag')]
            delta=tuple((LO.subtract(e,eps),HI.add(e,eps)) for e in errors)
            # delta' = 2*x*delta + delta**2, x'=x**2: different from (x+e)**256 subtraction.
            for _ in range(8):
                xd=cmul(x,delta);dd=cmul(delta,delta)
                delta=cadd(cadd(xd,xd),dd);x=cmul(x,x)
            for k,name in enumerate(('real','imag')):
                lower,upper=delta[k]
                magnitude_lower=min(abs(lower),abs(upper)) if lower*upper>0 else D(0)
                magnitude_upper=max(abs(lower),abs(upper))
                verdict='ABOVE_T' if magnitude_lower>T else ('BELOW_T' if magnitude_upper<T else 'UNDECIDED')
                exceeded+=(verdict=='ABOVE_T')
                rows.append({'slot':s,'component':name,'lower':str(lower),'upper':str(upper),
                             'ratio_lower':str(magnitude_lower/T),'ratio_upper':str(magnitude_upper/T),
                             'classification':verdict})
        require(exceeded==3 and all(v['classification']!='UNDECIDED' for v in rows),'retained result not reproduced')
        result={'source_log':str(p.relative_to(a.input_root)),
            'sha256':hashlib.sha256(p.read_bytes()).hexdigest(),
            'scope':'one retained fresh sample; conditional ideal scalar propagation; no observed new E8',
            'assumption':'A+B component error box +/-1e-100; no C propagation',
            'method':'240-digit outward-rounded complex interval delta recurrence',
            'components':20,'above_T':exceeded,'rows':rows}
    print(json.dumps(result,indent=2));return 0
if __name__=='__main__':raise SystemExit(main())
