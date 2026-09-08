#!/usr/bin/env python3
"""Only deterministic bounded scalar arithmetic, parsing and hashing. NO transforms."""
from __future__ import annotations
import argparse
import ast
from fractions import Fraction
import hashlib
import json
from pathlib import Path
import sys

BASE=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(BASE/'candidate'))
import interval_core as ic
from rounding_contract import (N, SCALE, UNIT, Interval, load_bound_fixture,
    original_slot, closed_form_two_coefficients, classify_cell, cell_margin, require)


def forbidden(*args, **kwargs):
    raise RuntimeError('TRIGONOMETRY_OR_TRANSFORM_FORBIDDEN_IN_SCALAR_REVIEW')


# Import is side-effect-free; the only transcendental helpers are disabled here.
ic.pi_interval=forbidden
ic.sincos_small=forbidden
ic.atan_reciprocal_bounds=forbidden


def main() -> int:
    parser=argparse.ArgumentParser()
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    if args.output.exists():
        raise ValueError('refusing to overwrite a scalar result')
    checks=[]
    def check(label: str, condition: bool):
        require(condition,label)
        checks.append({'label':label,'result':'PASS'})
    data,p=load_bound_fixture()
    check('FROZEN_PUBLIC_PAYLOAD_AND_SOURCE_BINDINGS',len(p)==32768)
    cap=json.loads((BASE/'fixtures/public_s100_cap.json').read_text())
    ep=cap['maximum_squared_modulus']
    lo=Fraction(int(ep['lower_numerator']),int(ep['denominator']))
    hi=Fraction(int(ep['upper_numerator']),int(ep['denominator']))
    radius=Fraction(495621,500000)
    cap_radius=Fraction(127,128)
    check('RETAINED_CAP_BINDING',cap['public_payload_sha256']==
          hashlib.sha256((BASE/'fixtures/public_s100_encoding.json').read_bytes()).hexdigest())
    check('CAP_ENDPOINT_ORDER',0<=lo<=hi)
    check('CAP_STRICT_REFINED_RADIUS',hi < radius**2 < cap_radius**2)
    check('CAP_GRID_224',int(ep['denominator']) == 1<<224)
    # Closed forms; no iteration across all slots and no inverse calculation.
    ideal0,idealhalf=closed_form_two_coefficients()
    check('CLOSED_FORM_CONSTANT_IS_NEGATIVE_2_TO_32',ideal0 == -(1<<32))
    check('CLOSED_FORM_MIDPOINT_IS_NEGATIVE_2_TO_32',idealhalf == -(1<<32))
    check('ACTUAL_P0_MATCHES_EXACT_MEAN',p[0] == ideal0)
    check('ACTUAL_P_N_OVER_2_MATCHES_EXACT_IMAGINARY_MEAN',p[N//2] == idealhalf)
    point=Interval.rational(ideal0)
    check('ACTUAL_CONSTANT_ROUNDING_CELL_CERTIFIED',classify_cell(point,p[0])=='CERTIFIED')
    # Semantic negatives are AFTER fixture binding, not hash-failure substitutes.
    check('NEGATIVE_CONSTANT_PLUS_ONE_REJECTED',classify_cell(point,p[0]+1)=='REFUTED')
    check('NEGATIVE_CONSTANT_MINUS_ONE_REJECTED',classify_cell(point,p[0]-1)=='REFUTED')
    check('PLUS_ONE_MUTANT_STILL_MEETS_CAP_BY_TRIANGLE',radius+Fraction(1,SCALE)<cap_radius)
    for k in (-3,-1,0,2,1<<32):
        check(f'CELL_OPEN_LEFT_REJECTS_{k}',classify_cell(Interval.rational(2*k-1,2),k)=='REFUTED')
        check(f'CELL_CLOSED_RIGHT_ACCEPTS_{k}',classify_cell(Interval.rational(2*k+1,2),k)=='CERTIFIED')
        check(f'CELL_STRADDLING_LEFT_INCONCLUSIVE_{k}',
              classify_cell(Interval(k*UNIT-UNIT//2-1,k*UNIT-UNIT//2+1),k)=='INCONCLUSIVE')
        check(f'CELL_ENDPOINT_ACCEPT_IS_NOT_STRICT_GREEN_{k}',
              cell_margin(Interval.rational(2*k+1,2),k)==0)
    check('NEGATIVE_HALF_TIE_DOWN',classify_cell(Interval.rational(-3,2),-2)=='CERTIFIED')
    check('NEGATIVE_HALF_TIE_NOT_UP',classify_cell(Interval.rational(-3,2),-1)=='REFUTED')
    check('UNSIGNED_ROUNDING_MUTANT_REJECTED',classify_cell(point,abs(ideal0))=='REFUTED')
    check('SCALE_DOUBLE_MUTANT_REJECTED',classify_cell(point,2*ideal0)=='REFUTED')
    # A few exact input points guard the transcription, not an all-slot run.
    check('EXACT_SLOT0',original_slot(0)==(Fraction(1015,1024),Fraction(1,1024)))
    check('EXACT_SLOT256_ROTATION',original_slot(256)==(-Fraction(1,1024),Fraction(1015,1024)+Fraction(256,1<<75)))
    check('EXACT_SLOT1024_SIGN',original_slot(1024)==(Fraction(1015,1024)+Fraction(1024,1<<75),-Fraction(1,1024)))
    delta=Fraction(N,2*SCALE)
    threshold=Fraction(1,1<<80)
    ratio=256*(radius+delta)**255*delta/threshold
    check('CONDITIONAL_ECD_ERROR_IS_TWO_TO_MINUS_86',delta==Fraction(1,1<<86))
    check('CONDITIONAL_EIGHT_SQUARE_ENCODING_ONLY_BELOW_HALF_E80',ratio<Fraction(1,2))
    endpoints=[]
    for host in ('linux','windows'):
        audit=json.loads((BASE/f'evidence/retained_{host}_audit.json').read_text())
        e8=next(row for row in audit['maxima'] if row['id']=='E8')
        lower,upper=Fraction(e8['lower']),Fraction(e8['upper'])
        check(f'RETAINED_{host}_E8_INTERVAL_EXCEEDS_E80',threshold<lower<=upper)
        check(f'RETAINED_{host}_ONE_COMPLETED_FAIL_CHAIN',
              audit['status']['chain_count']==1 and audit['status']['ctest_exit_code']==8
              and audit['numerical_result']=='E80_FAIL')
        signed=e8['signed_tuple']['E8.'+e8['component']]
        check(f'RETAINED_{host}_SAME_COMPONENT_MAGNITUDE',abs(Fraction(signed))==Fraction(e8['magnitude']))
        factor=lower/threshold
        unit_endpoint=10**12
        floor_endpoint=factor.numerator*unit_endpoint//factor.denominator
        endpoints.append({'host':host,'slot':e8['slot'],'component':e8['component'],
            'serialized_E8_magnitude':e8['magnitude'],
            'E8_certified_lower_over_T_lower_bound':f'{floor_endpoint}/{unit_endpoint}',
            'scope':'rechecked supplied audit rationals, NOT raw sidecar/full numerical replay'})
    # Exact 12-place enclosing decimal endpoints; not a floating point verdict.
    unit=10**12
    ratiofloor=ratio.numerator*unit//ratio.denominator
    check('ENCODING_ONLY_BOUND_BRACKET',Fraction(ratiofloor,unit)<=ratio<Fraction(ratiofloor+1,unit))
    # Basic outward integer interval operations, including signed corners.
    x=Interval.rational(-7,3);y=Interval.rational(11,5)
    check('INTERVAL_SIGNED_PRODUCT', (x*y).contains(Fraction(-77,15)))
    check('INTERVAL_SIGNED_DIVISION',x.divide_positive_integer(7).contains(Fraction(-1,3)))
    check('INTERVAL_CROSS_ZERO_SQUARE',Interval(-2*UNIT,3*UNIT).square()==Interval(0,9*UNIT))
    files=[]
    for path in sorted(list((BASE/'candidate').glob('*.py'))+list((BASE/'tools').glob('*.py'))):
        ast.parse(path.read_text(),filename=str(path))
        files.append({'path':str(path.relative_to(BASE)),
                      'sha256':hashlib.sha256(path.read_bytes()).hexdigest()})
    check('PYTHON_AST_PARSE_NO_BYTECODE',True)
    result={'schema':'reproduction-adjudication-scalars-v1','status':'PASS',
        'checks_count':len(checks),'checks':checks,'files_ast_parsed':files,'retained_endpoint_checks':endpoints,
        'actual_two_coefficients':{'0':str(p[0]),str(N//2):str(p[N//2])},
        'exact_unrounded_two_coefficients':{'0':str(ideal0),str(N//2):str(idealhalf)},
        'conditional_encoding_only_E8_over_E80_upper_bound_bracket':{
            'lower_numerator':str(ratiofloor),'upper_numerator':str(ratiofloor+1),
            'denominator':str(unit),'strict_upper':True,
            'condition':'ALL coefficients of this p are correctly nearest-rounded Ecd; NOT yet established'},
        'scope':'2 coefficient identities, scalar semantic negatives and conditional scalar bound; NOT a full encoder certificate',
        'FHE_calls':0,'encryption_calls':0,'decryption_calls':0,'sampler_calls':0,
        'FFT_NTT_transform_calls':0,'full_slot_input_generation_calls':0,
        'new_original_eight_square_chains':0,'original_S100_E80':'UNCHANGED_FAIL'}
    args.output.parent.mkdir(parents=True,exist_ok=True)
    with args.output.open('x',encoding='utf-8') as out:
        json.dump(result,out,ensure_ascii=False,indent=2);out.write('\n')
    print(json.dumps({'status':'PASS','checks':len(checks),'p0':p[0],'p16384':p[N//2],
        'conditional_E8_bound_over_T':f'[{ratiofloor}/{unit},{ratiofloor+1}/{unit})',
        'transform_calls':0,'FHE_calls':0},ensure_ascii=False))
    return 0

if __name__=='__main__':
    try:
        raise SystemExit(main())
    except Exception as error:
        print(f'SCALAR_CHECK_FAILED: {type(error).__name__}: {error}',file=sys.stderr)
        raise SystemExit(2)
