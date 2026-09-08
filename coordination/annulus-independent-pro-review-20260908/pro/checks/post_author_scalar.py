#!/usr/bin/env python3
"""Post-freeze scalar consequences, not encrypted or FFT execution."""
from __future__ import annotations
import argparse,json,math
from pathlib import Path
from decimal import Decimal as D,localcontext
from fractions import Fraction as F
from independent_scalar import Q

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--results',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
 new=json.loads((a.results/'independent_230.json').read_text());old=json.loads((a.results/'old_s100_230.json').read_text())
 with localcontext() as c:
  c.prec=100
  domain=new['domain']; gains=[D(256)*D(domain[k])**255 for k in ['minimum_radius','maximum_radius']]
  bound=39*(32768+1+128);root=math.prod(Q)
  assert F(318,100)*12>38 and F(320,100)*F(1201,100)<39
  assert 2*((1<<164)+bound)<root
  result={'scope':'post-author scalar checks; online pinned source inspected separately; no PRNG sampling',
    'new_absolute_condition_gain_min':str(gains[0]),'new_absolute_condition_gain_max':str(gains[1]),
    'relative_condition_number_for_power256_nonzero_x':256,
    'unpaired_comparison_limit':'new run samples different keys/noise; measured bit gain is not an exact matched-noise causal effect',
    'gaussian_support_scalar_consequence':{'pin':'df495ba2e91739a6dc8f1de254fc5a41155ce504',
      'official_source_location':'EXTERNAL_REFERENCE_NOTES.md S1/S2','successful_inversion_support_abs':39,
      'support_rounding_margin':'38 < 3.18*12 <= sigma*M <= 3.20*12.01 < 39 under supplied sigma3.19F',
      'aggregate_coefficient_bound':bound,'root_Q_bits':root.bit_length(),
      'conditional_encoded_norm_bound':'2^164','conditional_no_wrap_integer_inequality':True,
      'fresh_old_diagnostic_only':'requires the actual m norm asserted by that diagnostic source and actual successful record',
      'annulus_sample_m_bound_recorded':False,'new_annulus_lift_certified':False,
      'all_circuit_lifts_certified':False,'parameter_security_certified':False},
    'S116_complex_upper_bound_from_retained_component_receipts':{},'new_encrypted_runs':0}
  for host,comp in [('linux',D('2.5905123324714e-26')),('windows',D('3.4805603371614e-26'))]:
   upper=comp*D(2).sqrt();T=D(2)**-80
   assert upper<T
   result['S116_complex_upper_bound_from_retained_component_receipts'][host]={'component_value':str(comp),'upper_bound':str(upper),'upper_bound_over_T':str(upper/T),'full_slot_raw_independently_replayed':False}
 a.output.write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n');print(json.dumps(result,ensure_ascii=False,indent=2))
 return 0
if __name__=='__main__':raise SystemExit(main())
