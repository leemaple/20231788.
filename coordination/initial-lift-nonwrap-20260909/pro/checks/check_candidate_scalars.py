#!/usr/bin/env python3
"""Executed author tests: interval scalar primitives/parser ONLY. No transforms."""
from pathlib import Path
from fractions import Fraction as F
import copy
import argparse
import importlib.util
import json
import math
import sys
root=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--out',type=Path,default=root/'checks/results/candidate_scalar_checks.json')
args=parser.parse_args()
p=root/'candidate/certify_public_encoder.py'
spec=importlib.util.spec_from_file_location('candidate_scalar_only',p)
m=importlib.util.module_from_spec(spec);sys.modules[spec.name]=m;spec.loader.exec_module(m)
count=0
# Endpoint enclosure checks; 25 * 25 interval combinations, not FFT butterflies.
intervals=[(F(a,7),F(a,7)+F(b,9)) for a in range(-2,3) for b in range(5)]
for al,ah in intervals:
    A=m.Interval(m.Interval.rational(al.numerator,al.denominator).lo,
                 m.Interval.rational(ah.numerator,ah.denominator).hi)
    assert A.square().contains(al*al) and A.square().contains(ah*ah)
    for bl,bh in intervals:
        B=m.Interval(m.Interval.rational(bl.numerator,bl.denominator).lo,
                     m.Interval.rational(bh.numerator,bh.denominator).hi)
        for x in (al,ah):
            for y in (bl,bh):
                assert (A+B).contains(x+y) and (A-B).contains(x-y) and (A*B).contains(x*y)
                count+=3
pi=m.pi_interval()
assert 3*m.UNIT<pi.lo<pi.hi<F(22,7)*m.UNIT
assert pi.hi-pi.lo<=2
cosine,sine=m.sincos_small(m.Interval.rational(1,8))
assert F(1,8)-F(1,8)**3/6<=F(sine.lo,m.UNIT)<=F(sine.hi,m.UNIT)<=F(1,8)
assert 1-F(1,8)**2/2<=F(cosine.lo,m.UNIT)<=F(cosine.hi,m.UNIT)<=1
assert m.sincos_small(m.ZERO)==(m.ONE,m.ZERO)
# Scalar-only check: seed truncation at the smallest future transform N=4.
# 16 terms were statically too weak for that TDD width; 32 suffice here.
worst_seed_angle=F(22,28)
assert worst_seed_angle**64/math.factorial(64)<F(1,1<<256)
assert worst_seed_angle**65/math.factorial(65)<F(1,1<<256)
valid={'schema':'initial-lift-public-encoding-v1','profile':'original-s100-near-unit-v1',
       'base_source_commit':m.BASE_COMMIT,'build_source_commit':m.BASE_COMMIT,
       'boost_version':'synthetic-parser-model','compiler':'synthetic-parser-model',
       'n':m.N,'slots':m.N//2,'gap':1,'cyclotomic_order':2*m.N,
       'scale_num':str(m.SCALE),'scale_den':'1','full_moduli':list(map(str,m.Q)),
       'full_roots':list(map(str,m.ROOTS)),'encoding_calls':1,'crypto_calls':0,
       'coefficients':['0']*m.N}
# This is schema-positive synthetic public data only, never an encoder certificate.
assert m.validate_payload(valid)==[0]*m.N
negatives=[]
def reject(label,mutate):
    x=copy.deepcopy(valid);mutate(x)
    try:m.validate_payload(x)
    except ValueError:negatives.append(label);return
    raise AssertionError('accepted negative model: '+label)
reject('wrong_input_profile',lambda x:x.__setitem__('profile','annulus125'))
reject('wrong_scale',lambda x:x.__setitem__('scale_num',str(1<<116)))
reject('wrong_basis',lambda x:x['full_moduli'].__setitem__(0,'17'))
reject('wrong_root',lambda x:x['full_roots'].__setitem__(0,'1'))
reject('short_coefficients',lambda x:x['coefficients'].pop())
reject('noncanonical_signed_zero',lambda x:x['coefficients'].__setitem__(0,'-0'))
reject('unknown_build',lambda x:x.__setitem__('build_source_commit','unknown'))
reject('unexpected_secret_field',lambda x:x.__setitem__('private_key','synthetic-rejected-field'))
reject('changed_sampling_count',lambda x:x.__setitem__('crypto_calls',1))
reject('bool_substituted_for_int',lambda x:x.__setitem__('encoding_calls',True))
reject('missing_field',lambda x:x.pop('scale_den'))
import math
reject('outside_centered_encoding_range',lambda x:x['coefficients'].__setitem__(0,str(math.prod(m.Q))))
try:m.no_duplicates([('schema',1),('schema',2)])
except ValueError:negatives.append('duplicate_json_key')
else:raise AssertionError('duplicate keys accepted')
result={'status':'PASS','scope':'scalar interval/parser helpers only; synthetic zero coefficients are NOT source encoding',
        'interval_endpoint_assertions':count,'interval_pairs':625,'parser_negative_models':negatives,
        'pi_Machin_scalar_enclosure':pi.json(),'sincos_one_scalar_argument_checked':True,
        'Taylor_nonzero_terms':32,'N4_Taylor_remainder_less_than_2_pow_minus_256':True,
        'canonical_bounds_called':False,'FFT_NTT_calls':0,'FHE_sampling_calls':0,
        'CXX_compiled_or_run':False,'future_transform_tests_run':False}
args.out.parent.mkdir(parents=True,exist_ok=True)
args.out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8',newline='\n')
print(json.dumps({k:result[k] for k in ('status','interval_endpoint_assertions','canonical_bounds_called','FFT_NTT_calls','CXX_compiled_or_run')}))
