#!/usr/bin/env python3
"""UNEXECUTED runner candidate: one independent interval inverse, retained PUBLIC p.

No OpenFHE, Boost, native FFT, keys, encryption, samplers, or new production
encoding. This module performs no work on import. Full/tiny transforms require
an explicit review flag. Do NOT execute in the adjudication turn.
"""
from __future__ import annotations
import argparse
from collections import Counter
from fractions import Fraction
import hashlib
import json
import math
from pathlib import Path
import platform
import sys
from interval_core import (BITS, UNIT, Interval, ZERO, ONE, ComplexInterval,
    cadd, csub, cmul, pi_interval, sincos_small)
from rounding_contract import (N, SCALE, BASE, PAYLOAD_SHA256, STREAM_SHA256,
    require, load_bound_fixture, original_slot, closed_form_two_coefficients,
    classify_cell, cell_margin, sha256_file)


def conjugate(z: ComplexInterval) -> ComplexInterval:
    return z[0],-z[1]


def as_interval(z: tuple[Fraction,Fraction]) -> ComplexInterval:
    return tuple(Interval.rational(v.numerator,v.denominator) for v in z)


def inverse_coefficients(z: list[ComplexInterval], scale: int) -> tuple[list[ComplexInterval],dict]:
    """Inverse of p -> (p(xi^(5^s))/scale)_s, real coefficient embedding.

    y_k = z_s at k=(5^s-1)/2 and conjugate(z_s) at N-1-k.
    a_j = scale/N * xi^(-j) * sum_k y_k exp(-2*pi*i*j*k/N).
    Uses ordinary negative-exponent radix-2 DIT, NOT production SpecialFFTInv.
    """
    n=2*len(z)
    require(type(scale) is int and scale>0,'positive integer scale required')
    require(n>=4 and n<=N and not (n&(n-1)), 'bounded power-of-two dimension required')
    values: list[ComplexInterval | None]=[None]*n
    exponent=1
    for value in z:
        require(exponent%2 == 1,'non-odd root exponent')
        k=(exponent-1)//2
        for index,item in ((k,value),(n-1-k,conjugate(value))):
            require(values[index] is None,'root orbit collision')
            values[index]=item
        exponent=5*exponent%(2*n)
    require(exponent==1 and all(v is not None for v in values),'incomplete Hermitian root orbit')
    data=list(values)
    seed=sincos_small(pi_interval().divide_positive_integer(n))
    roots=[(ONE,ZERO)]
    for _ in range(1,n):
        roots.append(cmul(roots[-1],seed))
    index=0
    for i in range(1,n):
        bit=n>>1
        while index&bit:
            index^=bit
            bit>>=1
        index^=bit
        if i<index:
            data[i],data[index]=data[index],data[i]
    length=2
    butterflies=0
    while length<=n:
        for start in range(0,n,length):
            for j in range(length//2):
                root=conjugate(roots[2*j*(n//length)])
                left=data[start+j]
                right=cmul(data[start+j+length//2],root)
                data[start+j]=cadd(left,right)
                data[start+j+length//2]=csub(left,right)
                butterflies+=1
        length*=2
    normalization=Interval.rational(scale,n)
    result=[]
    for j,value in enumerate(data):
        twisted=cmul(value,conjugate(roots[j]))
        result.append((twisted[0]*normalization,twisted[1]*normalization))
    require(butterflies == n//2*(n.bit_length()-1),'butterfly count mismatch')
    return result,{'n':n,'grid_bits':BITS,'inverse_transforms':1,'butterflies':butterflies,
                   'root_multiplications':n-1,'root_direction':'negative_in_inverse',
                   'projection':'powers_of_five_with_Hermitian_completion'}


def tiny_controls() -> dict:
    """Four analytic cases, no round-trip oracle. All transform calls remote only."""
    tests=[]
    # Constants and X^(N/2) have spectra known without any transform.
    specifications=[(4,[as_interval((Fraction(3,2),Fraction(-2))) for _ in range(2)],
                         [Fraction(3,2),Fraction(0),Fraction(-2),Fraction(0)],'N4_CONSTANT_PLUS_MIDDLE'),
                    (8,[as_interval((Fraction(-3,4),Fraction(5,8))) for _ in range(4)],
                         [Fraction(-3,4)]+[Fraction(0)]*3+[Fraction(5,8)]+[Fraction(0)]*3,'N8_CONSTANT_PLUS_MIDDLE')]
    # sqrt(2)/2 is enclosed with INTEGER sqrt, not the trigonometric seed.
    floor=math.isqrt(UNIT*UNIT//2)
    require(2*floor*floor<=UNIT*UNIT<2*(floor+1)*(floor+1),'sqrt2 cell')
    a=Interval(floor,floor+1)
    specifications.extend([
        (4,[(a,a),(-a,-a)],[Fraction(0),Fraction(1),Fraction(0),Fraction(0)],'N4_X'),
        (4,[(-a,a),(a,-a)],[Fraction(0),Fraction(0),Fraction(0),Fraction(1)],'N4_X_CUBED')])
    counts=[]
    for n,z,expected,label in specifications:
        out,counters=inverse_coefficients(z,1)
        require(len(out)==n,'control dimension mismatch')
        for j,truth in enumerate(expected):
            require(out[j][0].contains(truth) and out[j][1].contains(Fraction(0)),
                    label+': analytic coefficient not enclosed')
            require(out[j][0].hi-out[j][0].lo < (UNIT>>100),label+': control enclosure too wide')
        counts.append(counters)
        tests.append(label)
    # Negative is downstream semantic validation, not a file-hash rejection.
    require(classify_cell(out[3][0],-1)=='REFUTED','negative X^3 coefficient sentinel')
    tests.append('NEGATIVE_SIGN_ON_X_CUBED_REJECTED')
    return {'schema':'ecd-rounding-tiny-controls-v1','status':'PASS','cases':tests,
            'tiny_inverse_transforms':4,'counters':counts,'full_transforms':0,'FHE_calls':0}


def certify_once(output: Path) -> tuple[dict,int]:
    _,coefficients=load_bound_fixture()
    # No new production p: these are the exact INPUT dyadics for an independent observer.
    z=[as_interval(original_slot(s)) for s in range(N//2)]
    intervals,counters=inverse_coefficients(z,SCALE)
    require(len(intervals)==N and all(im.lo<=0<=im.hi for _,im in intervals),
            'ORACLE_INVALID: nonreal inverse not enclosing zero')
    exact0,exactmiddle=closed_form_two_coefficients()
    require(intervals[0][0].contains(Fraction(exact0)) and
            intervals[N//2][0].contains(Fraction(exactmiddle)),
            'ORACLE_INVALID: closed-form coefficient control')
    require(classify_cell(intervals[0][0],coefficients[0]+1)=='REFUTED' and
            classify_cell(intervals[0][0],coefficients[0]-1)=='REFUTED',
            'ORACLE_INVALID: source-bound semantic negative')
    counts=Counter()
    first={}
    minimum=None
    minimum_index=None
    table=output/'coefficient_cells.tsv'
    with table.open('x',encoding='utf-8',newline='') as out:
        out.write('index\tactual_p\treal_lower_num\treal_upper_num\timag_lower_num\timag_upper_num\tdenominator\tcell_status\tstrict_margin_num\n')
        for j,((re,im),p) in enumerate(zip(intervals,coefficients)):
            status=classify_cell(re,p)
            margin=cell_margin(re,p)
            counts[status]+=1
            first.setdefault(status,j)
            if minimum is None or margin<minimum:
                minimum,minimum_index=margin,j
            out.write(f'{j}\t{p}\t{re.lo}\t{re.hi}\t{im.lo}\t{im.hi}\t{UNIT}\t{status}\t{margin}\n')
    require(sum(counts.values())==N,'result coverage mismatch')
    if counts['REFUTED']:
        status,exit_code='ECD_ROUNDING_REFUTED',3
    elif counts['INCONCLUSIVE'] or minimum<=0:
        status,exit_code='INCONCLUSIVE_INTERVAL',4
    else:
        status,exit_code='ECD_ROUNDING_CERTIFIED',0
    return {'schema':'public-s100-ecd-rounding-v1','status':status,'exit_code':exit_code,
            'public_payload_sha256':PAYLOAD_SHA256,'coefficient_stream_sha256':STREAM_SHA256,
            'counts':{key:counts[key] for key in ('CERTIFIED','REFUTED','INCONCLUSIVE')},
            'first_indices':first,'strict_minimum_margin':{'numerator':str(minimum),
               'denominator':str(UNIT),'index':minimum_index},
            'coefficient_cells_sha256':sha256_file(table),'counters':counters,
            'cell_convention':'(p-1/2,p+1/2]; GREEN additionally requires strictly positive margin',
            'mean_controls':'PASS','semantic_plus_minus_one_negatives':'PASS',
            'original_S100_E80':'UNCHANGED_FAIL','production_encoding_calls':0,
            'FHE_calls':0,'sampling_calls':0,'new_ciphertext_chains':0,
            'observer_full_slot_input_constructions':1,
            'scope':'ONLY this retained public p against the exact frozen original input; not historical p, FHE or security'},exit_code


def main() -> int:
    parser=argparse.ArgumentParser()
    modes=parser.add_mutually_exclusive_group(required=True)
    modes.add_argument('--controls',action='store_true')
    modes.add_argument('--certify',action='store_true')
    parser.add_argument('--allow-reviewed-transform',action='store_true')
    parser.add_argument('--output-dir',type=Path,required=True)
    args=parser.parse_args()
    require(args.allow_reviewed_transform,'SEPARATE_REVIEW_REQUIRED: no transform authorization flag')
    require(sys.version_info[:2]==(3,12),'frozen runner requires Python 3.12.x')
    output=args.output_dir
    output.mkdir(parents=False,exist_ok=False,mode=0o700)
    # Persistent marker intentionally remains on failure. No automatic retry.
    (output/'STARTED.json').write_text(json.dumps({'mode':'controls' if args.controls else 'certify',
        'python':sys.version,'platform':platform.platform(),
        'candidate_sha256':sha256_file(Path(__file__)),
        'source_bindings_sha256':sha256_file(BASE/'source/SOURCE_BINDINGS.json')},indent=2)+'\n')
    try:
        if args.controls:
            result,code=tiny_controls(),0
        else:
            result,code=certify_once(output)
        result['python']=sys.version
        result['candidate_sha256']=sha256_file(Path(__file__))
        result['interval_core_sha256']=sha256_file(BASE/'candidate/interval_core.py')
        with (output/'RESULT.json').open('x') as out:
            json.dump(result,out,indent=2);out.write('\n')
        print(json.dumps({'status':result['status'],'exit_code':code}))
        return code
    except Exception as error:
        result={'schema':'public-s100-ecd-oracle-failure-v1','status':'ORACLE_OR_INFRA_INVALID',
                'error_type':type(error).__name__,'error':str(error),'original_S100_E80':'UNCHANGED_FAIL'}
        with (output/'FAILURE.json').open('x') as out:
            json.dump(result,out,indent=2);out.write('\n')
        print(json.dumps(result),file=sys.stderr)
        return 2

if __name__=='__main__':
    try:
        raise SystemExit(main())
    except Exception as error:
        print(f'PRETRANSFORM_REJECTED: {type(error).__name__}: {error}',file=sys.stderr)
        raise SystemExit(2)
