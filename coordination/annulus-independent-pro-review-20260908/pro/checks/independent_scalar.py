#!/usr/bin/env python3
"""Independent, stdlib-only, no project imports; scalar replay, NOT a new FHE run.

Treats TSV decimal columns as observations bound to the supplied C++ source.
Does not attest ciphertext generation, FFT correctness or observer error bounds.
Independent propagated error: (u-v)*product_j(u^(2^j)+v^(2^j)), j=0..7.
Cross-checks against subtracting separately squared endpoints; never max(E8)-max(I8).
"""
from __future__ import annotations
import argparse, hashlib, json, re, sys, time
from decimal import Decimal as D, localcontext
from fractions import Fraction
from pathlib import Path

if hasattr(sys,'set_int_max_str_digits'): sys.set_int_max_str_digits(12000)
COMMIT='03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b'
PIN='df495ba2e91739a6dc8f1de254fc5a41155ce504'
Q=(1125899904679937,1125899903827969,1152921504598720513,1152921504597016577,
   1152921504595968001,1152921504595640321,1152921504593412097,1152921504592822273,
   1152921504592429057,1152921504589938689,1099510054913)
NAMES=('E0_obs','E8_obs','E8_prod','I8_obs','A8_obs')
GATE_NAMES=('E0_le_T','A8_le_T_over_4','E8_obs_le_T','E8_prod_le_T','witness')
HEADER='slot\tE0_obs.real\tE0_obs.imag\tE8_obs.real\tE8_obs.imag\tE8_prod.real\tE8_prod.imag'

def require(c:bool,msg:str)->None:
    if not c: raise ValueError(msg)
def num(s:str)->D:
    require(len(s)<=300 and re.fullmatch(r'[+-]?\d+(?:\.\d*)?(?:[eE][+-]?\d+)?',s) is not None,'decimal lexical bound')
    d=D(s);require(d.is_finite(),'nonfinite');return d

def cm(a,b): return a[0]*b[0]-a[1]*b[1],a[0]*b[1]+a[1]*b[0]
def cs(a): return a[0]*a[0]-a[1]*a[1],D(2)*a[0]*a[1]
def ca(a,b): return a[0]+b[0],a[1]+b[1]
def cd(a,b): return a[0]-b[0],a[1]-b[1]
def n2(a): return a[0]*a[0]+a[1]*a[1]
def power(a,k=8):
    for _ in range(k): a=cs(a)
    return a

def integer_input(s:int,base:int=999):
    t=s//2
    a=(base<<65)-((t%16)<<59)+s
    b=(1+(t//16)%8)<<65
    if (t//512)%2: b=-b
    phase=(t//128)%4
    return ((a,b),(-b,a),(-a,-b),(b,-a))[phase]

def scale_closed(k:int)->Fraction:
    den=Q[-1]**(2**k-1)
    for j in range(1,k+1): den*=Q[10-j]**(2**(k-j))
    return Fraction(2**(100*2**k),den)

def parse(raw:bytes,expected_commit:str):
    require(len(raw)<32*1024*1024 and raw.endswith(b'\n') and b'\r' not in raw and b'\0' not in raw,'framing')
    lines=raw.decode('ascii').splitlines();p=0
    def take():
        nonlocal p
        require(p<len(lines),'truncated');s=lines[p];p+=1;return s
    require(take()=='#s100-annulus125-e80-v1','contract')
    expected=[('source_commit',expected_commit),('openfhe_pin',PIN),('input_formula','four-phase-999-dyadic-v1'),
              ('norm','max-complex-modulus'),('assurance','CONDITIONAL_OBSERVER_NOT_FORMAL'),('security','UNRESOLVED'),
              ('legacy_S100_stress','FAIL_RETAINED'),('chain_count','1'),('squares','8'),('slots','16384')]
    for k,v in expected:require(take()==f'meta\t{k}\t{v}','metadata order/value: '+k)
    for k in range(9):
        x=take().split('\t'); require(len(x)==4 and x[:2]==['scale',str(k)],'scale framing')
        n,d=int(x[2]),int(x[3]);e=scale_closed(k)
        require(n==e.numerator and d==e.denominator,'canonical exact-prime scale: '+str(k))
    agreement={}
    for k in ('fresh','terminal'):
        x=take().split('\t');require(len(x)==3 and x[:2]==['agreement',k],'agreement framing');agreement[k]=num(x[2])
    require(take()==HEADER,'column binding')
    rows=[]
    for s in range(16384):
        x=take().split('\t');require(len(x)==7 and x[0]==str(s),'slot ordering/count: '+str(s));rows.append(tuple(num(v) for v in x[1:]))
    declared={}
    for k in NAMES:
        x=take().split('\t'); require(len(x)==4 and x[:2]==['max',k],'maximum framing')
        declared[k]=(int(x[2]),num(x[3]))
    gates={}
    for k in GATE_NAMES:
        x=take().split('\t');require(len(x)==3 and x[:2]==['gate',k] and x[2] in ('0','1'),'gate framing');gates[k]=x[2]=='1'
    x=take().split('\t');require(x in (['status','COMPLETE','PASS'],['status','COMPLETE','FAIL']),'footer')
    require(p==len(lines),'trailing data')
    return rows,agreement,declared,gates,x[2]

def replay(raw:bytes,precision:int,process_exit:int,commit=COMMIT)->dict:
    require(150<=precision<=400,'bounded precision 150..400 decimal digits')
    require(type(process_exit) is int and process_exit in (0,1),'normal numerical process exit')
    rows,agreement,declared,gates,footer=parse(raw,commit)
    with localcontext() as ctx:
        ctx.prec=precision
        T=D(2)**-80;tol=D(2)**-300
        maxima={k:(-1,D(-1)) for k in NAMES}; counts={k:0 for k in GATE_NAMES[:-1]}
        seen=set();minimum=None;maximum=None;phase_counts=[0]*4
        max_cross=D(0);factor_direct=D(0);max_relative=D(0);relative_slot=-1
        nearest_max_gap={}; second={k:D(-1) for k in NAMES}
        extrema=[]
        for s,row in enumerate(rows):
            ai,bi=integer_input(s);require((ai,bi) not in seen,'duplicate exact input');seen.add((ai,bi));phase_counts[((s//2)//128)%4]+=1
            radi=ai*ai+bi*bi
            if minimum is None or radi<minimum[1]:minimum=(s,radi)
            if maximum is None or radi>maximum[1]:maximum=(s,radi)
            x=(D(ai)/D(2**75),D(bi)/D(2**75));e0=row[0:2];e8=row[2:4];p8=row[4:6]
            u=ca(x,e0);v=x;factor=(D(1),D(0))
            for _ in range(8):
                factor=cm(factor,ca(u,v));u=cs(u);v=cs(v)
            i8=cm(e0,factor);direct=cd(u,v)
            factor_direct=max(factor_direct,abs(direct[0]-i8[0]),abs(direct[1]-i8[1]))
            a8=cd(e8,i8)
            for k,z in zip(NAMES,(e0,e8,p8,i8,a8)):
                q=n2(z)
                if q>maxima[k][1]: second[k]=maxima[k][1];maxima[k]=(s,q)
                elif q>second[k]:second[k]=q
            r=n2(e8)/n2(v)
            if r>max_relative:max_relative=r;relative_slot=s
            max_cross=max(max_cross,abs(p8[0]-e8[0]),abs(p8[1]-e8[1]))
            for gate,z,bound in zip(GATE_NAMES[:-1],(e0,a8,e8,p8),(T,T/4,T,T)):
                if n2(z)>bound*bound:counts[gate]+=1
        require(maximum[1]*128**2 < 125**2*2**150,'all-slot exact radius')
        require(minimum[1]**128 > 2**(150*128-10),'nonvanishing endpoint |x^256|>2^-10')
        p0=integer_input(0);p1=integer_input(1);require((p1[0]-p0[0])**2+(p1[1]-p0[1])**2==1,'exact input witness')
        x0=tuple(D(t)/D(2**75) for t in p0);x1=tuple(D(t)/D(2**75) for t in p1)
        witness_ideal=cd(power(x1),power(x0));witness_error=cd(rows[1][4:6],rows[0][4:6]);witness_actual=ca(witness_ideal,witness_error)
        require(n2(witness_ideal)>16*T*T,'witness ideal >4T')
        actual={k:counts[k]==0 for k in counts};actual['witness']=n2(witness_error)<=4*T*T
        require(actual==gates,'declared gate disagreement')
        status='PASS' if all(actual.values()) else 'FAIL'
        require(footer==status and process_exit==(0 if status=='PASS' else 1),'process/footer mismatch')
        require(all(D(0)<=v<=D(2)**-120 for v in agreement.values()),'observer agreement runtime declaration')
        require(max_cross<=D(2)**-120 and max_cross<=agreement['terminal']+tol,'terminal row agreement')
        require(factor_direct<D(10)**(-precision+5),'independent propagation identities disagree')
        report={}
        for k,(s,q) in maxima.items():
            mod=q.sqrt(); ds,dv=declared[k]
            require(s==ds and abs(mod-dv)<tol,'declared max/slot mismatch: '+k)
            report[k]={'slot':s,'complex_modulus':str(mod),'absolute_bits':str(-mod.ln()/D(2).ln()) if mod else 'Infinity',
                       'ratio_to_T':str(mod/T),'declared_difference':str(abs(mod-dv)),
                       'runner_up_modulus_gap':str(mod-second[k].sqrt()) if second[k]>=0 else None}
        for k,bound in [('E0_obs',T),('A8_obs',T/4),('E8_obs',T),('E8_prod',T)]:
            require(abs(maxima[k][1].sqrt()-bound)>tol,'near ambiguous gate')
        return {'status':status,'scope':'new independent scalar replay of RETAINED observations; no new encryption',
                'precision_decimal_digits':precision,'new_encrypted_runs':0,'raw_bytes':len(raw),
                'raw_sha256':hashlib.sha256(raw).hexdigest(),'source_commit':commit,'process_exit_from_end_receipt':process_exit,
                'slots':len(rows),'gates':actual,'failing_slots_per_gate':counts,'maxima':report,
                'domain':{'unique_exact_inputs':len(seen),'four_phase_counts':phase_counts,'exact_radius_gate':True,'exact_nonzero_endpoint_gate':True,
                          'minimum_radius_slot':minimum[0],'maximum_radius_slot':maximum[0],
                          'minimum_radius':str((D(minimum[1])/D(2**150)).sqrt()),'maximum_radius':str((D(maximum[1])/D(2**150)).sqrt()),
                          'minimum_ideal_output_modulus':str((D(minimum[1])/D(2**150))**128),
                          'maximum_ideal_output_modulus':str((D(maximum[1])/D(2**150))**128)},
                'witness':{'input_difference':'2^-75','ideal_difference_modulus':str(n2(witness_ideal).sqrt()),
                           'ideal_difference_over_T':str(n2(witness_ideal).sqrt()/T),'actual_difference_modulus':str(n2(witness_actual).sqrt()),
                           'error_modulus':str(n2(witness_error).sqrt()),'gate':actual['witness']},
                'worst_relative_error':{'slot':relative_slot,'complex_relative_error':str(max_relative.sqrt()),
                                        'relative_bits':str(-max_relative.sqrt().ln()/D(2).ln())},
                'independent_propagation_identity_max_component_difference':str(factor_direct),
                'terminal_max_component_disagreement':str(max_cross),'runtime_observer_agreement':{k:str(v) for k,v in agreement.items()},
                'unreplayable_from_tsv':['fresh producer-vs-observer full-slot agreement','512-vs-768 transform error','ten Horner anchors','integer phase and ciphertext lineage'],
                'formal_numerical_proof':False}

def main()->int:
    a=argparse.ArgumentParser(description=__doc__);a.add_argument('--tsv',type=Path,required=True);a.add_argument('--end',type=Path,required=True)
    a.add_argument('--precision',type=int,required=True);a.add_argument('--output',type=Path,required=True);args=a.parse_args()
    started=time.monotonic();end=json.loads(args.end.read_text());require(end['source_commit']==COMMIT and end['timed_out'] is False,'process source/timeout')
    result=replay(args.tsv.read_bytes(),args.precision,end['returncode']);result['local_scalar_elapsed_seconds']=time.monotonic()-started
    args.output.write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n')
    print(json.dumps({'output':str(args.output),'status':result['status'],'digits':args.precision,'rows':result['slots'],
                      'elapsed_scalar_seconds':result['local_scalar_elapsed_seconds'],'maxima':{k:(v['slot'],v['complex_modulus'][:28]) for k,v in result['maxima'].items()}},indent=2))
    return 0
if __name__=='__main__':raise SystemExit(main())
