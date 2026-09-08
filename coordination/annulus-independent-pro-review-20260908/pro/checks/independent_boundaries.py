#!/usr/bin/env python3
"""Independent exact/static counterexamples and finite scalar tests. NEVER an encrypted experiment."""
from __future__ import annotations
import argparse,ast,hashlib,importlib.util,json,sys,tempfile
from pathlib import Path
from fractions import Fraction as F
from decimal import Decimal as D,localcontext
from independent_scalar import COMMIT,require,parse,replay,scale_closed,Q

def nearest(x:F)->int:
    a,b=divmod(x.numerator,x.denominator);return a+(2*b>x.denominator)
def center(x:int,q:int)->int:
    r=x%q;return r-q if 2*r>q else r

def run(packet:Path,results:Path)->dict:
    out={'scope':'synthetic/static/exact scalar challenges, not extra encrypted samples','new_encrypted_runs':0,'tests':{}}
    tests=out['tests']
    # Our own witness: all q,d,base primes 1 mod 2N; NOT the numbers in supplied scalar_reassessment.py.
    N,h,d,q,Q0,H,L,er=2,1,13,101,1009,10,0,0;mod=q*Q0
    hi=H*H;lo=2*H*L;rs_hi=nearest(F(hi,q));target=nearest(F(d*hi+lo,q));rs_lo=target-d*rs_hi
    printed=F((d*H+L)**2,q);normalized=printed/d
    hypothesis=N*(d*H+L)**2+er+h;bound=F(N*L*L,d*q)+F(er+h,q)+F(h+1,2)
    require(F(hypothesis)<F(mod,2),'literal theorem size condition')
    require(abs(target-printed)>bound and abs(target-normalized)<=bound,'missing divisor counterexample')
    tests['paper_thm_4_8_literal_target']={'disposition':'COUNTEREXAMPLE_CONFIRMED','paper_page':8,'N':N,'h':h,'d':d,'q':q,'Q':mod,'high_plaintext':H,'low_plaintext':L,
       'relinearization':'ideal exact rlk=(P*s^2,0) allowed by algebra; no cryptographic claim or generation; all c2=0 in witness',
       'size_lhs':hypothesis,'size_rhs':str(F(mod,2)),'rescaled_high':rs_hi,'rescaled_low':rs_lo,'recombined':target,
       'printed_target':str(printed),'missing_divisor_corrected_target':str(normalized),'printed_error':str(abs(target-printed)),
       'normalized_error':str(abs(target-normalized)),'claimed_bound':str(bound),'production_defect_demonstrated':False}
    tests['paper_tensor_sign']={'printed_inner_product':1-2+1,'plus_convention_product':(1+1)**2,'counterexample':(1-2+1)!=(1+1)**2,'production_uses_plus_cross_terms':True}
    def r(c):return nearest(F(3*c,7)),nearest(F(4*c,7))
    a=r(1);b=r(2);carry=(2*a[0]-b[0],2*a[1]-b[1]);require(carry==(-1,1),'two coordinate carry')
    tests['paper_lemma4_4_intermediate_identity']={'P':7,'rlk':[3,4],'s':1,'component_carry':carry,'assertion_refuted':'rounding carry is always (0,e)','whole_lemma_refuted':False}
    require(F(3,4)**2+F(3,4)**2>1,'complex norm trap')
    tests['component_pass_complex_fail']={'error_over_T':['3/4','3/4'],'component_pass':True,'squared_complex_modulus_over_T2':'9/8','complex_pass':False}
    # maxima have equal size but opposite directions; max subtraction destroys all error.
    E=(F(1),F(0));I=(F(-1),F(0));A=(E[0]-I[0],E[1]-I[1]);require(A==(2,0),'max subtraction trap')
    tests['maxima_subtraction_is_invalid']={'maxE':1,'maxI':1,'wrong_difference':0,'correct_residual_modulus':2}
    require(center(55,101)==-46 and 101-2*abs(center(55,101))==9,'centered headroom example')
    tests['centered_headroom_not_integer_lift']={'unreduced':55,'modulus':101,'centered':-46,'positive_centered_headroom':9,'lift_changed':True}
    # Exhaustive scalar DCP and correlated RS checks in another small basis.
    Qs,ds,qs=221,5,13;count=0
    for x in range(Qs*ds):
       lo=center(x,ds);hi=((x-lo)//ds)%Qs
       require((hi*ds+lo-x)%Qs==0,'DCP/RCB modulo Q')
       require(hi==((x%Qs-lo)*pow(ds,-1,Qs))%Qs,'DCP residue quotient')
       count+=1
    for hi in range(Qs):
      for lo in range(Qs):
       rh=nearest(F(center(hi,Qs),qs));expected=nearest(F(center(ds*hi+lo,Qs),qs));rl=(expected-ds*rh)%(Qs//qs)
       require((ds*rh+rl-expected)%(Qs//qs)==0,'correlated rescale identity')
    tests['small_modular_identities']={'DCP_cases':count,'correlated_RS_cases':Qs*Qs,'scope':'coordinate identities only, no encrypted/integer-lift proof'}
    # Exact positive scalar Lipschitz bound, no numerical/observer model used in this proof.
    T=F(1,2**80);radius=F(125,128);K=256*(radius+T)**255
    require(K+F(1,4)<F(171,200),'conditional annulus budget')
    with localcontext() as c:
       c.prec=80;ks=D(K.numerator)/D(K.denominator)
       tests['conditional_annulus_propagation']={'proved_by_exact_rational_comparison':True,'K_rounded_for_display':str(ks),'K_plus_quarter_upper_bound':'171/200 = 0.855',
        'assumptions':['|x| < 125/128','|E0| <= 2^-80','same-slot |A8| <= 2^-82'],
        'conclusion':'|E8| < 0.855 * 2^-80 for these quantities; assumes observer accuracy to apply to true ciphertext semantics',
        'does_not_prove':'E0/A8 bounds for every other encryption, integer lifts, or FFT accuracy'}
       new=json.loads((results/'independent_230.json').read_text());old=json.loads((results/'old_s100_230.json').read_text())
       tests['actual_maxima_are_not_a_residual']={'max_E8_minus_max_I8':str(D(new['maxima']['E8_obs']['complex_modulus'])-D(new['maxima']['I8_obs']['complex_modulus'])),
        'actual_A8_max':new['maxima']['A8_obs']['complex_modulus'],'A8_slot':new['maxima']['A8_obs']['slot']}
       tests['zero_added_error_does_not_fix_old_samples']={}
       for host in ['linux','windows']:
        x=old[host]['maxima'];ival=D(x['I8']['complex_modulus']);aval=D(x['A8']['complex_modulus']);limit=D(T.numerator)/D(T.denominator)
        require(ival>limit and ival-aval>limit,'old failure persists at dominant inherited slot')
        tests['zero_added_error_does_not_fix_old_samples'][host]={'I8_max_over_T':str(ival/limit),
          'reverse_triangle_lower_bound_over_T':str((ival-aval)/limit),'A8_zero_still_fails':True,
          'scope':'conditional on retained observed fresh error; not an impossibility theorem for all schemes'}
    rawpath=packet/'project/coordination/s100-annulus125-20260908/experiment-evidence/sample/raw.tsv';raw=rawpath.read_bytes();digest=hashlib.sha256(raw).hexdigest()
    cases={'missing_last_LF':raw[:-1],'wrong_source':raw.replace(COMMIT.encode(),b'0'*40,1),
           'renamed_column':raw.replace(b'E8_prod.real\t',b'E8_fake.real\t',1),
           'missing_slot':b'\n'.join(line for line in raw.split(b'\n') if not line.startswith(b'1234\t')),
           'trailing_record':raw+b'extra\trow\n'}
    rejected={}
    for name,b in cases.items():
      try:parse(b,COMMIT)
      except (ValueError,UnicodeError) as e:rejected[name]=str(e)
      else:raise AssertionError('accepted malformed '+name)
    try:replay(raw,180,1)
    except ValueError as e:rejected['actual_PASS_with_exit_1']=str(e)
    else:raise AssertionError('accepted wrong exit')
    tests['in_memory_negative_records']=rejected
    # Only compile the real pure predicate AST, do not import/run the rest of the workflow checker.
    p=packet/'project/coordination/s100-annulus125-20260908/check_one_shot_gate.py';tree=ast.parse(p.read_text())
    fn=next(x for x in tree.body if isinstance(x,ast.FunctionDef) and x.name=='event_allowed')
    ns={'TAG_REF':'refs/tags/s100-annulus125-once-20260908'};exec(compile(ast.Module(body=[fn],type_ignores=[]),str(p),'exec'),ns)
    event={'ref':ns['TAG_REF'],'created':True,'deleted':False,'forced':False}
    first=ns['event_allowed'](event_name='push',ref=ns['TAG_REF'],run_attempt=1,event=event)
    rebuilt=ns['event_allowed'](event_name='push',ref=ns['TAG_REF'],run_attempt=1,event=event)
    rerun=ns['event_allowed'](event_name='push',ref=ns['TAG_REF'],run_attempt=2,event=event)
    require(first[0] and rebuilt[0] and not rerun[0],'one-shot history counterexample')
    tests['event_gate_cannot_prove_first_ever_tag']={'predicate_source_sha256':hashlib.sha256(p.read_bytes()).hexdigest(),
       'first_create_accepted':first[0],'indistinguishable_recreation_new_run_attempt1_accepted':rebuilt[0],'same_run_attempt2_accepted':rerun[0],
       'scope':'pure predicate synthetic inputs; NOT a claim tag was actually recreated, no GitHub access'}
    # Untouched remote records must be rejected as local finalizer inputs. No path rewriting.
    hdir=p.parent;sys.path.insert(0,str(hdir));import finalize_once
    with tempfile.TemporaryDirectory(prefix='annulus-review-unchanged-copy-') as td:
      local=Path(td)
      for name in ['program-start.json','program-end.json','raw.tsv','stdout.txt','stderr.txt']:(local/name).write_bytes((rawpath.parent/name).read_bytes())
      try:finalize_once.finalize(local,COMMIT)
      except ValueError as e:
       require(str(e)=='wrong process entry/output','expected remote path failure')
       tests['untouched_remote_absolute_path']={'result':'EXPECTED_REJECTION','exception':str(e),'rewritten_evidence':False,'verification_created':(local/'verification.json').exists()}
      else:raise AssertionError('unexpectedly accepted foreign path')
    require(rawpath.read_bytes()==raw and hashlib.sha256(rawpath.read_bytes()).hexdigest()==digest,'original raw unchanged')
    out['original_raw_unchanged_sha256']=digest;out['status']='PASS_SCALAR_STATIC_ONLY';out['test_groups']=len(tests);return out

def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--packet',required=True,type=Path);ap.add_argument('--results',required=True,type=Path);ap.add_argument('--output',required=True,type=Path);ap.add_argument('--literal-paper-red',action='store_true');a=ap.parse_args()
 out=run(a.packet,a.results);a.output.write_text(json.dumps(out,ensure_ascii=False,indent=2)+'\n')
 if a.literal_paper_red:
  print('EXPECTED_RED: literal Theorem 4.8 target fails its own bound; no production code has been changed.');return 1
 print(json.dumps({'status':out['status'],'test_groups':out['test_groups'],'no_new_encrypted_runs':True,'output':str(a.output)}));return 0
if __name__=='__main__':raise SystemExit(main())
