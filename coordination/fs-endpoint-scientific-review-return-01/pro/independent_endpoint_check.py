#!/usr/bin/env python3
"""Independent scalar review of the two frozen FS endpoint captures.

Standard library only. No project imports, Decimal arithmetic decisions, FFT,
C++ or encryption. Polynomial error is propagated as d' = d*(2*w+d), w'=w*w,
using outward-rounded integer intervals on a 10^-200 grid. Endpoint uncertainty
is CONDITIONAL on the supplied live observer model, not a certification of it.
Run: python3 -B independent_endpoint_check.py INPUT_OR_EVIDENCE_ROOT HOST
"""
from __future__ import annotations
import argparse
import gzip
import hashlib
import json
import pathlib
import re
import sys
import time
from decimal import Decimal, localcontext
from fractions import Fraction as F

if hasattr(sys, 'set_int_max_str_digits'):
    sys.set_int_max_str_digits(30000)  # fixed scale-8 rationals have >4300 digits
SOURCE = 'ed5fd192a89d6d4728ad295e87cf06a3f4abc832'
PRODUCTION = 'b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89'
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
RUN = '34039088536'
GRID = 10**200
T = F(1, 2**80)
Q = (1125899904679937,1125899903827969,1152921504598720513,
     1152921504597016577,1152921504595968001,1152921504595640321,
     1152921504593412097,1152921504592822273,1152921504592429057,
     1152921504589938689,1099510054913)
FROZEN = {
 'linux': {'canonical_sha256':'919f7deee4d6acbb2c068d62c2f657cd91b4068a458e1a69a8f9767a4f87a6cd',
           'log_sha256':'ba76bd3b955fb6bd5e022dcd1d1d6aeb6a8ac657ea8188cef75ab48c0d9d016c','misses':9},
 'windows':{'canonical_sha256':'b21d50a97a81c7e83f4fea3bd2262a77587e21a14772c1070a20c71d4f19512e',
            'log_sha256':'92201a4bb4a628a330df2d11f2bbb1ce885d651e4ddee946fcdc9284d7754384','misses':7}}
CANONICAL = re.compile(r'[+-][0-9]\.[0-9]{109}e[+-][0-9]{5}\Z')
I = tuple[int,int]
C = tuple[I,I]

def require(ok: bool, message: str) -> None:
    if not ok:
        raise ValueError(message)

def sha(b: bytes) -> str:
    return hashlib.sha256(b).hexdigest()

def p2(e: int) -> F:
    return F(2**e) if e >= 0 else F(1,2**(-e))

def ceiling2(f: F) -> F:
    if f == 0: return F(0)
    require(f > 0, 'negative coefficient bound')
    e = f.numerator.bit_length() - f.denominator.bit_length()
    v = p2(e)
    return 2*v if v < f else v

def floor_grid(f: F) -> int:
    return f.numerator*GRID//f.denominator

def ceil_grid(f: F) -> int:
    return -((-f.numerator*GRID)//f.denominator)

def interval(x: F, radius: F=F(0)) -> I:
    require(radius >= 0, 'negative radius')
    return floor_grid(x-radius), ceil_grid(x+radius)

def add(a: I,b: I) -> I: return a[0]+b[0], a[1]+b[1]
def sub(a: I,b: I) -> I: return a[0]-b[1], a[1]-b[0]
def twice(a: I) -> I: return 2*a[0],2*a[1]
def mul(a: I,b: I) -> I:
    v=(a[0]*b[0],a[0]*b[1],a[1]*b[0],a[1]*b[1])
    return min(v)//GRID, -((-max(v))//GRID)
def cmul(a: C,b: C) -> C:
    return sub(mul(a[0],b[0]),mul(a[1],b[1])),add(mul(a[0],b[1]),mul(a[1],b[0]))
def cadd(a: C,b: C) -> C: return add(a[0],b[0]),add(a[1],b[1])
def csub(a: C,b: C) -> C: return sub(a[0],b[0]),sub(a[1],b[1])
def abs_bounds(a: I) -> I:
    return (0 if a[0] <= 0 <= a[1] else min(abs(a[0]),abs(a[1])), max(abs(a[0]),abs(a[1])))
def overlaps(a: I,b: I) -> bool: return max(a[0],b[0]) <= min(a[1],b[1])

def exact_input(slot: int) -> tuple[F,F]:
    group = slot//2
    a = F(1015,1024)-F(group%16,65536)+F(slot,2**75)
    b = F(1+(group//16)%8,1024)*(-1 if (group//512)%2 else 1)
    return ((a,b),(-b,a),(-a,-b),(b,-a))[(group//128)%4]

def fields(token: str) -> tuple[F,F]:
    require(CANONICAL.fullmatch(token) is not None,'noncanonical serialized scalar')
    value=F(token)
    if value == 0:
        require(token == '+'+'0.'+'0'*109+'e+00000', 'unique canonical zero')
        return F(0),F(0)
    require(token[1] != '0', 'nonzero leading significant digit')
    exponent=int(token.split('e')[1])-109
    quantum=(F(10**exponent) if exponent >= 0 else F(1,10**(-exponent)))/2
    return value,quantum

def ratio(d: dict, name: str) -> F:
    return F(int(d[name+'_num']),int(d[name+'_den']))

def pretty(f: F) -> str:
    # Presentation only; every decision above/below uses exact integer/Fraction.
    with localcontext() as ctx:
        ctx.prec=42
        return format(Decimal(f.numerator)/Decimal(f.denominator),'.32E')

def describe(a: I) -> dict:
    return {'lower':pretty(F(a[0],GRID)), 'upper':pretty(F(a[1],GRID)),
            'lower_grid_integer':str(a[0]),'upper_grid_integer':str(a[1])}

def derive_bounds(meta: dict) -> tuple[dict,list[F]]:
    scales=[F(2**100)]
    for stage in range(1,9):
        scales.append(scales[-1]**2 / (Q[-1]*Q[10-stage]))
    require(scales[0] == F(int(meta['scale0_numerator']),int(meta['scale0_denominator'])), 'scale0')
    require(scales[8] == F(int(meta['scale8_numerator']),int(meta['scale8_denominator'])), 'scale8')
    k0=ceiling2(F(int(meta['coefficient_l1_fresh']))/scales[0])
    k8=ceiling2(F(int(meta['coefficient_l1_terminal']))/scales[8])
    d0,d8=p2(12-768)*k0,p2(12-768)*k8
    p,r=p2(270-768),p2(264-768)
    q=p+p2(263)*d0
    bounds={'E0':d0+r,'E8':d8+p+r,'I8':q+p+r,'A8':d8+q+r}
    require(all(0 <= b <= p2(-128) for b in bounds.values()),'allowance ceiling')
    return bounds,scales

def self_tests() -> dict:
    count=0
    vals=[F(-7,5),F(-1,3),F(0),F(2,7),F(11,9)]
    for a in vals:
        for b in vals:
            for op,exact in ((add,a+b),(sub,a-b),(mul,a*b)):
                got=op(interval(a),interval(b))
                require(F(got[0],GRID) <= exact <= F(got[1],GRID),'integer interval unit test')
                count+=1
    # The recurrence is checked against exact rational complex powers (not the
    # endpoint replay's subtraction algorithm) on a small, independent fixture.
    z=(F(3,8),F(-1,16));e=(F(1,1024),F(-1,2048))
    def emul(a,b):return a[0]*b[0]-a[1]*b[1],a[0]*b[1]+a[1]*b[0]
    w=tuple(map(interval,z));d=tuple(map(interval,e))
    x=(z[0]+e[0],z[1]+e[1]);v=z
    for _ in range(8):
        d=cmul(d,cadd((twice(w[0]),twice(w[1])),d));w=cmul(w,w)
        x=emul(x,x);v=emul(v,v)
    for j in (0,1):
        require(F(d[j][0],GRID) <= x[j]-v[j] <= F(d[j][1],GRID),'exact polynomial selftest')
        count+=1
    require(abs_bounds((-4,3)) == (0,4),'interval abs crossing zero');count+=1
    return {'result':'PASS','assertions':count}

def run(root: pathlib.Path,host: str) -> dict:
    start=time.perf_counter();base=root/'project/coordination/fs-endpoint-live-run-01'
    status_paths=list((base/host).glob('*.status.json'));require(len(status_paths)==1,'one status')
    status_bytes=status_paths[0].read_bytes();status=json.loads(status_bytes)
    for key,value in {'source_commit':SOURCE,'production_source':PRODUCTION,'openfhe_pin':PIN,
                      'host':host,'github_run_id':RUN,'github_run_attempt':'1',
                      'evidence_state':'COMPLETE','reason':'NONE','ctest_exit_code':8,
                      'E80_disposition':'FAIL','A_disposition':'NOT_ADOPTED',
                      'observer_disposition':'PASS','packer_disposition':'PASS',
                      'chain_count':1,'row_count':16384,
                      'numeric_gate_failures':FROZEN[host]['misses']}.items():
        require(status.get(key)==value,'status identity/disposition: '+key)
    require(pathlib.PurePosixPath(status['gzip_filename']).name==status['gzip_filename'],'gzip filename')
    compressed=(base/host/status['gzip_filename']).read_bytes()
    require(len(compressed)==status['gzip_bytes'] and sha(compressed)==status['gzip_sha256'],'gzip binding')
    canonical=gzip.decompress(compressed)
    require(len(canonical)==status['canonical_bytes'],'canonical size')
    require(sha(canonical)==status['canonical_sha256']==FROZEN[host]['canonical_sha256'],'frozen canonical binding')
    lines=canonical.decode('ascii').splitlines()
    require(lines[0]=='#fs-residual-endpoint-01.v1-r1','schema')
    meta={};controls=[];header=None
    for n,line in enumerate(lines[1:],1):
        v=line.split('\t')
        if v[0]=='meta':
            require(len(v)==3 and v[1] not in meta,'unique metadata');meta[v[1]]=v[2]
        elif v[0]=='check':controls.append(v)
        elif line=='slot\tE0.real\tE0.imag\tE8.real\tE8.imag':header=n;break
        else:raise ValueError('unexpected canonical record')
    for key in ('source_commit','production_source','openfhe_pin','host','github_run_id',
                'github_run_attempt','numeric_gate_failures','E80_disposition','A_disposition'):
        require(meta[key]==str(status[key]),'metadata/status binding: '+key)
    for key,value in {'n':'32768','m':'65536','slots':'16384','gap':'1','row_count':'16384',
                     'primary_precision_bits':'768','check_precision_bits':'512',
                     'significant_digits':'110','assurance':'CONDITIONAL',
                     'model':'conditional-binary-nearest-direct-trig8u-v1',
                     'input_formula':'frozen-four-phase-exact-dyadic-v1'}.items():
        require(meta[key]==value,'metadata profile: '+key)
    require(len(controls)==24 and len({v[1] for v in controls})==24,'24 controls')
    for v in controls:
        require(len(v)==9 and v[2]=='PASS','control schema/verdict')
        distance,allowance=F(int(v[3]),int(v[4])),F(int(v[5]),int(v[6]))
        require(0 <= distance <= allowance and distance+allowance <= p2(-120),'retained control allowance')
    require(header is not None and len(lines)-header-1==16384,'full row closure')
    bounds,scales=derive_bounds(meta)
    raw_log=(base/(host.upper()+'_JOB.log')).read_bytes()
    require(sha(raw_log)==FROZEN[host]['log_sha256'],'retained log identity')
    logs=[]
    for line_number,line in enumerate(raw_log.decode('utf-8-sig').splitlines(),1):
        line=re.sub(r'^2026-09-06T[0-9:.]+Z ','',line)
        if line.startswith('61: '):logs.append((line_number,line[4:]))
    maxima={};numeric=[];scale_records=[];begins=ends=0
    for number,line in logs:
        if line.startswith('FS_ENDPOINT_MAX\t'):
            d=dict(x.split('=',1) for x in line.split()[1:]);d['_line']=number
            require(d['id'] not in maxima,'duplicate maximum');maxima[d['id']]=d
        if line.startswith('OBS numeric_gate=FAIL '):numeric.append({'line':number,'text':line})
        if line.startswith('FS_ENDPOINT_SCALE\t'):scale_records.append(line)
        if line.startswith('FS_ENDPOINT_BEGIN\t'):begins+=1
        if line.startswith('FS_ENDPOINT_COMPLETE\t'):ends+=1
    require(begins==ends==1 and set(maxima)==set(bounds),'one real endpoint sequence')
    require(len(numeric)==FROZEN[host]['misses'],'original failure count')
    # Derive all scales independently and reconcile log records without accepting metadata-only scales.
    require(len(scale_records)==9,'nine scales')
    for i,line in enumerate(scale_records):
        rec=dict(t.split('=',1) for t in line.split()[1:])
        require(int(rec['index'])==i,'scale order')
        require(F(int(rec['numerator']),int(rec['denominator']))==scales[i],'exact stage scale')
    selected={0,1}|{int(d['argmax_slot']) for d in maxima.values()}
    global_bounds={k:[0,0] for k in bounds};argmax={};saved={};input_norm_upper=0
    for slot,line in enumerate(lines[header+1:]):
        cells=line.split('\t');require(len(cells)==5 and cells[0]==str(slot),'row order/shape')
        fractions=[fields(v) for v in cells[1:]]
        e0=tuple(interval(x,q+bounds['E0']) for x,q in fractions[:2])
        e8=tuple(interval(x,q+bounds['E8']) for x,q in fractions[2:])
        z=exact_input(slot);w=tuple(map(interval,z));delta=e0
        l1=abs(z[0])+abs(z[1]);require(l1<1,'exact input conditioning')
        fresh=cadd(w,e0)
        require(sum(abs_bounds(j)[1] for j in fresh) <= floor_grid(F(3,2)),'fresh conditioning disk')
        input_norm_upper=max(input_norm_upper,sum(abs_bounds(j)[1] for j in fresh))
        for _ in range(8):
            delta=cmul(delta,cadd((twice(w[0]),twice(w[1])),delta))
            w=cmul(w,w)
        values={'E0':e0,'E8':e8,'I8':delta,'A8':csub(e8,delta)}
        # Identity enclosure is algebraic, not independent evidence of crypto correctness.
        identity=csub(csub(e8,delta),values['A8'])
        require(all(a<=0<=b for a,b in identity),'decomposition enclosure')
        for name,value in values.items():
            for component,item in enumerate(value):
                lo,hi=abs_bounds(item);g=global_bounds[name]
                if lo>g[0]:g[0]=lo;argmax[name]=(slot,('real','imag')[component])
                g[1]=max(g[1],hi)
        if slot in selected:
            saved[slot]={'values':values,'tokens':cells[1:],'z256':w,'row_line':header+2+slot}
    reconciliation=[]
    for name,d in maxima.items():
        require(ratio(d,'allowance')==bounds[name],'independently derived live allowance')
        exact=ratio(d,'magnitude_exact');quantum=ratio(d,'magnitude_quantum')
        require(abs(F(d['magnitude'])-exact)<=quantum,'printed maximum quantization')
        require(ratio(d,'interval_lower')==max(F(0),exact-bounds[name]),'lower interval binding')
        require(ratio(d,'interval_upper')==exact+bounds[name],'upper interval binding')
        require(overlaps(tuple(global_bounds[name]),interval(exact,bounds[name])),'maximum enclosure reconciliation')
        require(argmax[name]==(int(d['argmax_slot']),d['argmax_component']),'maximum arg attribution')
        row=saved[int(d['argmax_slot'])]
        for metric in bounds:
            for component,part in enumerate(('real','imag')):
                token=metric+'.'+part
                point,q=fields(d[token]);require(ratio(d,token+'_q')==q,'tuple quantum')
                require(overlaps(row['values'][metric][component],interval(point,q+bounds[metric])), 'same-slot '+token)
        require(row['tokens']==[d[k] for k in ('E0.real','E0.imag','E8.real','E8.imag')],'exact selected serialization')
        reconciliation.append({'metric':name,'retained_log_line':d['_line'],'row_line':row['row_line'],'result':'PASS'})
    d=maxima['E8'];slot=int(d['argmax_slot']);component=('real','imag').index(d['argmax_component'])
    value=saved[slot]['values'];iv=value['I8'][component];av=value['A8'][component]
    require(iv[1]<0<av[0],'signed same-component I negative, A positive')
    reverse=abs_bounds(iv)[0]-abs_bounds(av)[1]
    require(F(reverse,GRID)>T,'conditional same-component reverse-triangle lower bound > E80')
    # Separate retained-live tuple budget, retaining BI, BA and serialized quantization.
    ip,iq=fields(d['I8.'+d['argmax_component']]);ap,aq=fields(d['A8.'+d['argmax_component']])
    retained_reverse=abs(ip)-iq-bounds['I8']-abs(ap)-aq-bounds['A8']
    require(retained_reverse>T,'all retained allowance reverse lower bound')
    require(F(global_bounds['I8'][0],GRID)>T and F(global_bounds['A8'][1],GRID)<T,'global I/A characterization')
    witness=sub(saved[1]['values']['E8'][0],saved[0]['values']['E8'][0])
    expected=sub(saved[1]['z256'][0],saved[0]['z256'][0]);actual=add(expected,witness)
    require(F(actual[0],GRID)>p2(-76),'witness distinctness')
    witness_abs=abs_bounds(witness)
    # Retain 2^-120 per endpoint for transfer to the producer's decoded witness.
    transfer=2*p2(-120)
    if host=='linux':require(F(witness_abs[0],GRID)-transfer>2*T,'Linux extra witness failure')
    else:require(F(witness_abs[1],GRID)+transfer<=2*T,'Windows witness accuracy pass')
    result={'result':'INDEPENDENT_FULL_SLOT_CONDITIONAL_AUDIT_PASS','host':host,
            'source_commit':SOURCE,'production_source':PRODUCTION,'openfhe_pin':PIN,
            'rows':16384,'components':32768,'integer_grid_denominator':str(GRID),
            'recurrence':'delta_next=delta*(2*w+delta); w_next=w*w; 8 steps',
            'crypto_or_fft_executed':False,'rigorous_live_observer_certification':False,
            'conditional_model':meta['model'],'E80_disposition':'FAIL','A_disposition':'NOT_ADOPTED',
            'numeric_gate_failures':len(numeric),'original_numeric_failure_records':numeric,
            'bindings':{'gzip_sha256':sha(compressed),'canonical_sha256':sha(canonical),
                        'status_sha256':sha(status_bytes),'retained_log_sha256':sha(raw_log)},
            'live_allowances':{k:{'exact_fraction':str(v),'decimal':pretty(v)} for k,v in bounds.items()},
            'nine_exact_scales_match':True,'controls_checked':24,
            'maxima':{k:{**describe(tuple(v)),'argmax_slot':argmax[k][0],
                         'argmax_component':argmax[k][1],
                         'midpoint_over_E80':pretty(F(sum(v),2*GRID)/T)} for k,v in global_bounds.items()},
            'same_component':{'slot':slot,'component':d['argmax_component'],
                              'I8':describe(iv),'A8':describe(av),
                              'independent_reverse_lower':pretty(F(reverse,GRID)),
                              'retained_allowance_reverse_lower_exact':str(retained_reverse),
                              'retained_allowance_reverse_lower':pretty(retained_reverse),
                              'both_reverse_lower_bounds_exceed_E80':True},
            'witness':{'signed_difference_error':describe(witness),
                       'expected_difference':describe(expected),'actual_difference':describe(actual),
                       'distinctness_pass':True,'accuracy_pass':host=='windows',
                       'limit_exact':str(2*T),'producer_transfer_allowance_exact':str(transfer)},
            'selected_tuple_reconciliation':reconciliation,
            'elapsed_seconds':round(time.perf_counter()-start,3)}
    return result

def main() -> None:
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('root',type=pathlib.Path,nargs='?')
    parser.add_argument('host',choices=tuple(FROZEN),nargs='?')
    parser.add_argument('--self-test',action='store_true')
    args=parser.parse_args()
    tests=self_tests()
    if args.self_test and args.root is None:
        print(json.dumps(tests,indent=2));return
    require(args.root is not None and args.host is not None,'root and host required')
    result=run(args.root.resolve(),args.host);result['arithmetic_self_tests']=tests
    print(json.dumps(result,indent=2))

if __name__=='__main__':
    main()
