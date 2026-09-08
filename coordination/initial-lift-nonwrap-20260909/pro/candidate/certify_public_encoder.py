#!/usr/bin/env python3
"""UNRUN transform candidate for root review, INITIAL-LIFT-NONWRAP-01.

All numerical endpoints are integers on a 2^-224 grid. The CLI computes one
interval forward canonical transform of PUBLIC pre-residue coefficients, not
OpenFHE/NTT/decryption. Import has no transforms, sampling or I/O side effects.
Only the scalar arithmetic / parser helpers were tested in the author turn.
"""
from __future__ import annotations
import argparse
from dataclasses import dataclass
from fractions import Fraction
import hashlib
import json
import math
from pathlib import Path
import re
import sys

BITS = 224
UNIT = 1 << BITS
N = 32768
SCALE = 1 << 100
BASE_COMMIT = 'a4b815a733efe81897325e2a8e4c826a4ebfa439'
Q = [1125899904679937,1125899903827969,1152921504598720513,
     1152921504597016577,1152921504595968001,1152921504595640321,
     1152921504593412097,1152921504592822273,1152921504592429057,
     1152921504589938689,1099510054913]
ROOTS = [26113207984,150640639383,100545759574150,31693996050849,
         88651361085495,9679305630873,24428769072221,18776242964106,
         5821397352863,33888991361320,121567553]
KEYS = {'schema','profile','base_source_commit','build_source_commit','boost_version',
        'compiler','n','slots','gap','cyclotomic_order','scale_num','scale_den',
        'full_moduli','full_roots','encoding_calls','crypto_calls','coefficients'}


def ceil_div(a: int, b: int) -> int:
    if b <= 0: raise ValueError('positive divisor required')
    return -((-a)//b)


@dataclass(frozen=True)
class Interval:
    lo: int
    hi: int

    def __post_init__(self):
        if type(self.lo) is not int or type(self.hi) is not int or self.lo > self.hi:
            raise ValueError('invalid interval endpoints')

    @classmethod
    def rational(cls, p: int, q: int=1) -> 'Interval':
        if q <= 0: raise ValueError('positive denominator required')
        return cls((p*UNIT)//q, ceil_div(p*UNIT,q))

    def __add__(self, other: 'Interval') -> 'Interval':
        return Interval(self.lo+other.lo,self.hi+other.hi)

    def __neg__(self) -> 'Interval':
        return Interval(-self.hi,-self.lo)

    def __sub__(self, other: 'Interval') -> 'Interval':
        return self+-other

    def __mul__(self, other: 'Interval') -> 'Interval':
        products=(self.lo*other.lo,self.lo*other.hi,self.hi*other.lo,self.hi*other.hi)
        return Interval(min(products)//UNIT,ceil_div(max(products),UNIT))

    def divide_positive_integer(self, divisor: int) -> 'Interval':
        if divisor <= 0: raise ValueError('positive divisor required')
        return Interval(self.lo//divisor,ceil_div(self.hi,divisor))

    def square(self) -> 'Interval':
        upper=max(self.lo*self.lo,self.hi*self.hi)
        lower=0 if self.lo<=0<=self.hi else min(self.lo*self.lo,self.hi*self.hi)
        return Interval(lower//UNIT,ceil_div(upper,UNIT))

    def expanded(self, radius: Fraction) -> 'Interval':
        if radius < 0: raise ValueError('nonnegative radius required')
        r=ceil_div(radius.numerator*UNIT,radius.denominator)
        return Interval(self.lo-r,self.hi+r)

    def contains(self, x: Fraction) -> bool:
        return self.lo*x.denominator <= x.numerator*UNIT <= self.hi*x.denominator

    def json(self) -> dict:
        return {'lower_numerator':str(self.lo),'upper_numerator':str(self.hi),
                'denominator':str(UNIT)}


ZERO=Interval(0,0)
ONE=Interval(UNIT,UNIT)
# Complex intervals are (real interval, imaginary interval).
ComplexInterval=tuple[Interval,Interval]


def cadd(a: ComplexInterval,b: ComplexInterval) -> ComplexInterval:
    return a[0]+b[0],a[1]+b[1]


def csub(a: ComplexInterval,b: ComplexInterval) -> ComplexInterval:
    return a[0]-b[0],a[1]-b[1]


def cmul(a: ComplexInterval,b: ComplexInterval) -> ComplexInterval:
    return a[0]*b[0]-a[1]*b[1],a[0]*b[1]+a[1]*b[0]


def atan_reciprocal_bounds(denominator: int) -> tuple[Fraction,Fraction]:
    if denominator < 2: raise ValueError('alternating convergence domain')
    terms=96
    partial=sum((Fraction((-1)**j,(2*j+1)*denominator**(2*j+1))
                 for j in range(terms)),Fraction(0))
    adjacent=partial+Fraction((-1)**terms,(2*terms+1)*denominator**(2*terms+1))
    return min(partial,adjacent),max(partial,adjacent)


def pi_interval() -> Interval:
    a,b=atan_reciprocal_bounds(5)
    c,d=atan_reciprocal_bounds(239)
    lo,hi=16*a-4*d,16*b-4*c
    lower=Interval.rational(lo.numerator,lo.denominator)
    upper=Interval.rational(hi.numerator,hi.denominator)
    return Interval(lower.lo,upper.hi)


def sincos_small(x: Interval) -> ComplexInterval:
    """Returns (cos(x),sin(x)); |x|<=1. Taylor remainder is explicitly added."""
    bound=Fraction(max(abs(x.lo),abs(x.hi)),UNIT)
    if bound>1: raise ValueError('Taylor argument outside fixed certified domain')
    powers=[ONE]
    for _ in range(63):powers.append(powers[-1]*x)
    cosine=ZERO;sine=ZERO
    for k in range(32):
        cosine=cosine+powers[2*k]*Interval.rational((-1)**k,math.factorial(2*k))
        sine=sine+powers[2*k+1]*Interval.rational((-1)**k,math.factorial(2*k+1))
    return (cosine.expanded(bound**64/math.factorial(64)),
            sine.expanded(bound**65/math.factorial(65)))


def canonical_bounds(coefficients: list[int], scale: int) -> dict:
    """ROOT-ONLY TRANSFORM. Includes all N odd 2N-th roots (conjugates too)."""
    n=len(coefficients)
    if n<4 or n>N or n&(n-1) or scale<=0:
        raise ValueError('fixed bounded power-of-two transform domain')
    seed=sincos_small(pi_interval().divide_positive_integer(n))
    roots=[(ONE,ZERO)]
    for _ in range(1,n): roots.append(cmul(roots[-1],seed))
    data=[cmul((Interval.rational(c,scale),ZERO),roots[j])
          for j,c in enumerate(coefficients)]
    # Bit-reversal, then positive-exponent radix-2 DIT. This yields
    # sum_j (p_j/S) exp(pi*i*j/n) exp(2*pi*i*j*k/n).
    j=0
    for i in range(1,n):
        bit=n>>1
        while j&bit:j^=bit;bit>>=1
        j^=bit
        if i<j:data[i],data[j]=data[j],data[i]
    length=2;butterflies=0
    while length<=n:
        half=length//2;stride=2*(n//length)
        for start in range(0,n,length):
            for j in range(half):
                u=data[start+j]
                t=cmul(data[start+j+half],roots[j*stride])
                data[start+j]=cadd(u,t)
                data[start+j+half]=csub(u,t)
                butterflies+=1
        length*=2
    squares=[z[0].square()+z[1].square() for z in data]
    low_index=max(range(n),key=lambda i:squares[i].lo)
    high_index=max(range(n),key=lambda i:squares[i].hi)
    enclosure=Interval(squares[low_index].lo,squares[high_index].hi)
    cap_squared=Fraction(127,128)**2
    if enclosure.hi*cap_squared.denominator <= cap_squared.numerator*UNIT:
        status='ENCODER_CAP_CERTIFIED'
    elif enclosure.lo*cap_squared.denominator > cap_squared.numerator*UNIT:
        status='ENCODER_CAP_REFUTED'
    else:status='INCONCLUSIVE_INTERVAL'
    return {'status':status,'maximum_squared_modulus':enclosure.json(),
            'cap_squared':{'numerator':str(cap_squared.numerator),'denominator':str(cap_squared.denominator)},
            'max_lower_root_index':low_index,'max_upper_root_index':high_index,
            'maximum_component_interval_width_numerator':str(max(max(z[0].hi-z[0].lo,z[1].hi-z[1].lo) for z in data)),
            'root_seed':{'real':seed[0].json(),'imag':seed[1].json()},
            'n':n,'all_odd_roots_including_conjugates':True,
            'butterflies':butterflies,'root_multiplications':n-1,
            'forward_canonical_transforms':1,'grid_bits':BITS}


def integer_string(value: object) -> int:
    if not isinstance(value,str) or len(value)>200 or not re.fullmatch(r'0|-?[1-9][0-9]*',value):
        raise ValueError('noncanonical or oversized integer string')
    return int(value)


def validate_payload(data: dict) -> list[int]:
    if not isinstance(data,dict) or set(data)!=KEYS:
        raise ValueError('exact public-only schema required; unknown/missing fields rejected')
    fixed={'schema':'initial-lift-public-encoding-v1','profile':'original-s100-near-unit-v1',
           'base_source_commit':BASE_COMMIT,'n':N,'slots':N//2,'gap':1,'cyclotomic_order':2*N,
           'scale_num':str(SCALE),'scale_den':'1','full_moduli':list(map(str,Q)),
           'full_roots':list(map(str,ROOTS)),'encoding_calls':1,'crypto_calls':0}
    for key,expected in fixed.items():
        if data[key]!=expected or type(data[key]) is not type(expected):
            raise ValueError('wrong frozen metadata: '+key)
    if not isinstance(data['build_source_commit'],str) or not re.fullmatch('[0-9a-f]{40}',data['build_source_commit']):
        raise ValueError('actual build source commit must be recorded (not unknown)')
    for key in ('compiler','boost_version'):
        if not isinstance(data[key],str) or not 1<=len(data[key])<=200 or any(ord(c)<32 for c in data[key]):
            raise ValueError('invalid nonsecret build metadata')
    if type(data['coefficients']) is not list or len(data['coefficients'])!=N:
        raise ValueError('exactly N public coefficients required')
    coefficients=[integer_string(x) for x in data['coefficients']]
    modulus=math.prod(Q)
    if any(2*abs(c)>=modulus for c in coefficients):
        raise ValueError('encoder centered coefficient guard violated')
    return coefficients


def no_duplicates(pairs):
    out={}
    for key,value in pairs:
        if key in out: raise ValueError('duplicate JSON field')
        out[key]=value
    return out


def main() -> int:
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--public-encoding',required=True,type=Path)
    parser.add_argument('--out',required=True,type=Path)
    parser.add_argument('--allow-transform-after-root-review',action='store_true')
    args=parser.parse_args()
    if not args.allow_transform_after_root_review:
        parser.error('transform execution is reserved for the reviewed root successor')
    if args.out.exists():raise ValueError('refusing to overwrite aggregate certificate')
    raw=args.public_encoding.read_bytes()
    if len(raw)>12_000_000 or not raw.endswith(b'\n') or b'\r' in raw:
        raise ValueError('bounded LF-terminated complete public JSON required')
    payload=json.loads(raw.decode('utf-8'),object_pairs_hook=no_duplicates)
    coefficients=validate_payload(payload)
    result=canonical_bounds(coefficients,SCALE)
    result.update(schema='initial-lift-public-cap-result-v1',
        public_payload_sha256=hashlib.sha256(raw).hexdigest(),
        coefficient_stream_sha256=hashlib.sha256(''.join(str(c)+'\n' for c in coefficients).encode('ascii')).hexdigest(),
        maximum_absolute_coefficient=str(max(map(abs,coefficients))),
        base_source_commit=BASE_COMMIT,build_source_commit=payload['build_source_commit'],
        boost_version=payload['boost_version'],compiler=payload['compiler'],
        source_build_origin='REQUIRES_SEPARATE_ROOT_RECEIPT; JSON labels are not attestations',
        historical_secret_pairing=None,historical_E80_status='UNCHANGED_FAIL',
        FHE_encryption_decryption_sampling_calls=0,
        theorem_scope='This one exact public polynomial; all honest finite-support keys only after source/path adoption')
    with args.out.open('x',encoding='utf-8',newline='\n') as stream:
        json.dump(result,stream,ensure_ascii=False,indent=2);stream.write('\n')
    print(json.dumps({'status':result['status'],'forward_canonical_transforms':1,'crypto_calls':0}))
    return {'ENCODER_CAP_CERTIFIED':0,'ENCODER_CAP_REFUTED':2,'INCONCLUSIVE_INTERVAL':3}[result['status']]


if __name__=='__main__':
    try:sys.exit(main())
    except (ValueError,OSError,KeyError,TypeError,json.JSONDecodeError) as error:
        print('PUBLIC_CAP_REJECTED: '+str(error),file=sys.stderr);sys.exit(4)
