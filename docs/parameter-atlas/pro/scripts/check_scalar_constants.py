#!/usr/bin/env python3
"""Standard-library-only static/integer/Fraction checks; no OpenFHE import or sampling.
Usage: python check_scalar_constants.py INPUT_ROOT OUTPUT_JSON
The Miller–Rabin screen uses fixed public bases; report it as a screen, not a
certificate. Three S116 primes separately receive explicit Proth certificates.
"""
from __future__ import annotations
import argparse, fractions, hashlib, json, math, pathlib, re, struct, sys
from functools import reduce
from operator import mul
sys.set_int_max_str_digits(100000)
F=fractions.Fraction
BASES=(2,325,9375,28178,450775,9780504,1795265022)
def mr(n:int)->bool:
    if n<2:return False
    for p in (2,3,5,7,11,13,17,19,23,29,31,37):
        if n%p==0:return n==p
    d=n-1;s=0
    while d%2==0:d//=2;s+=1
    for a in BASES:
        a%=n
        if a==0:continue
        x=pow(a,d,n)
        if x in (1,n-1):continue
        for _ in range(s-1):
            x=x*x%n
            if x==n-1:break
        else:return False
    return True

def lg(f:F)->float:return math.log2(f.numerator)-math.log2(f.denominator)
def fr(f:F)->dict:return {'numerator':str(f.numerator),'denominator':str(f.denominator)}
def main()->None:
    ap=argparse.ArgumentParser();ap.add_argument('input_root',type=pathlib.Path);ap.add_argument('output',type=pathlib.Path);a=ap.parse_args()
    text=(a.input_root/'project/src/repeated_mult2.cpp').read_text()
    parse=lambda s:[tuple(map(int,x)) for x in re.findall(r'\{(\d+)ULL,(\d+)ULL\}',s)]
    q100=parse(text.split('kPaperQ{{',1)[1].split('}};',1)[0]);p=parse(text.split('constexpr PaperPrime kPaperP',1)[1].split(';',1)[0])[0]
    explicit=parse(text.split('kExperimentalPrecision116Profile{{{',1)[1].split('}},kPaperP',1)[0]);assert len(q100)==11 and len(explicit)==3
    q116=explicit[:2]+q100[2:10]+explicit[2:]
    out={'scope':'static constants and pure scalar checks ONLY; no build/FFT/FHE/sampling','fixed_miller_rabin_bases':list(BASES),'profiles':{},'failures':[]}
    for name,primes,bits in [('S100',q100,100),('S116',q116,116)]:
        allp=primes+[p]; qs=[q for q,r in primes]; delta=F(1<<bits); d=qs[-1]
        entries=[]
        for i,(q,r) in enumerate(allp):
            # Enumerating modular powers is NOT an NTT and touches no ciphertext.
            rr=r*r%q; z=r; minimum=r
            for _ in range(1,32768):z=z*rr%q;minimum=min(minimum,z)
            e={'index':i,'role':('Base'+str(i) if i<2 else 'Mult'+str(i-2) if i<10 else 'Div' if i==10 else 'P'),'q':str(q),'root':str(r),'bits':q.bit_length(),'log2_approx':math.log2(q),'congruent_1_mod_65536':q%65536==1,'fixed_base_MR_screen':mr(q),'root_power_N_is_minus_1':pow(r,32768,q)==q-1,'root_power_2N_is_1':pow(r,65536,q)==1,'minimum_primitive_root':str(minimum),'frozen_root_is_minimum':minimum==r}
            assert all(e[k] for k in ['congruent_1_mod_65536','fixed_base_MR_screen','root_power_N_is_minus_1','root_power_2N_is_1'])
            entries.append(e)
        assert all(math.gcd(x[0],y[0])==1 for i,x in enumerate(allp) for y in allp[i+1:])
        Q=math.prod(qs);QP=Q*p[0]
        steps=[]
        for j in range(1,9):
            previous=delta;tensor=previous*previous/d;drop=qs[10-j];delta=tensor/drop
            closed=F(1<<(bits*(1<<j)), math.prod((d*qs[10-k])**(1<<(j-k)) for k in range(1,j+1)))
            assert delta==closed
            family=qs[:12-j-1]+[d] # j1: first10+Div; j8:first3+Div
            active_before=qs[:11-j];active_after=qs[:10-j]
            assert family[:-1]==active_before and active_before[-1]==drop
            steps.append({'round':j,'family_index':j-1,'family_Q':list(map(str,family)),'active_Q_before':list(map(str,active_before)),'active_Q_after':list(map(str,active_after)),'d':str(d),'dropped_mult':str(drop),'scale_input':fr(previous),'scale_tensor':fr(tensor),'scale_output':fr(delta),'closed_form_equals_recursive':True,'log2_output_approx':lg(delta),'log2_output_over_nominal_approx':lg(delta/F(1<<bits)),'recorded_input_output_scale':str(1<<bits),'recorded_tensor_scale':str(1<<(3*(bits//2))),'local_level_input_tensor_relin':1,'local_level_output':2,'noise_degree_input_tensor_relin_output':[2,3,3,2]})
        out['profiles'][name]={'N':32768,'slots':16384,'initial_scale':str(1<<bits),'metadata_bits':bits//2,'ordered_primes':entries,'Q':str(Q),'QP':str(QP),'Q_bit_length':Q.bit_length(),'QP_bit_length':QP.bit_length(),'Q_log2_approx':math.log2(Q),'QP_log2_approx':math.log2(QP),'root_basis_count':11,'terminal_basis':list(map(str,qs[:2])),'terminal_logical_scale':fr(delta),'steps':steps}
    cert=[]
    for (q,_),w in zip(explicit,[5,7,11]):
        k=q-1;n=0
        while k%2==0:k//=2;n+=1
        passed=k%2==1 and k<(1<<n) and pow(w,(q-1)//2,q)==q-1
        assert passed
        cert.append({'q':str(q),'k':str(k),'n':n,'witness':w,'k_odd_and_lt_2pow_n':True,'witness_power_is_minus1':True,'proth_certificate':True})
    out['s116_proth_certificates']=cert
    # Frozen public primes are copied in independent fixture headers; do not run them.
    fixtures={}
    for path in ['project/tests/paper_full_eight_square_oracle.h','project/tests/experimental_precision116_profile_seam.h']:
        s=(a.input_root/path).read_text();expected=q100 if 'oracle' in path else q116
        present=[str(q) in s and str(r) in s for q,r in expected]
        assert all(present)
        fixtures[path]={'all_11_q_root_literal_pairs_present':all(present),'sha256':hashlib.sha256(s.encode()).hexdigest()}
    out['literal_fixture_checks']=fixtures
    # Source-derived P candidate, fixed tests only; no upstream prime finder/PRNG.
    candidate=(1<<60)-((1<<60)%65536)+1
    while candidate >= (1<<60):candidate-=65536
    rejected=[]
    forbidden={q for q,_ in q100+q116}
    while candidate in forbidden or not mr(candidate):rejected.append(str(candidate));candidate-=65536
    assert candidate==p[0]
    out['auxiliary_prime_scalar_derivation']={'candidate':str(candidate),'rejected_candidates':rejected,'matches_frozen_P':True}
    sig=struct.unpack('<f',struct.pack('<f',3.19))[0]
    out['sigma']={'context_float32_widened':repr(sig),'float32_hex':struct.pack('>f',3.19).hex(),'peikert_tail_ceil_for_context':math.ceil(12.00610553538285*sig),'default_private_dgg_sigma':1.0,'peikert_tail_ceil_for_private':math.ceil(12.00610553538285)}
    # Only input-domain scalar bounds; no encoding, no polynomial arithmetic or sampling.
    inp={}
    for name,base in [('original',1015),('annulus125',999)]:
        radii=[]
        for s in range(16384):
            t=s//2;ar=F(base,1024)-F(t%16,65536)+F(s,1<<75);br=F(1+(t//16)%8,1024)
            radii.append(ar*ar+br*br)
        lo,hi=min(radii),max(radii)
        inp[name]={'base_numerator':base,'max_radius_squared':fr(hi),'min_radius_squared':fr(lo),'all_strictly_inside_unit_circle':hi<1,'all_strictly_inside_125_over_128':hi<F(125,128)**2,'output_min_magnitude_gt_2pow_minus10':lo**256>F(1,1<<20),'max_radius_squared_slot':radii.index(hi),'min_radius_squared_slot':radii.index(lo),'min_radius_approx':math.sqrt(float(lo)),'max_radius_approx':math.sqrt(float(hi))}
    assert inp['original']['all_strictly_inside_unit_circle'] and inp['annulus125']['all_strictly_inside_125_over_128'] and inp['annulus125']['output_min_magnitude_gt_2pow_minus10']
    out['input_domain_scalar_checks']=inp
    # Pure integer illustration of PDF Thm4.8 printed missing d; NOT an HE run.
    N,d,q,Q,h,E=2,13,17,17*12289,1,0
    mh=q;ml=0;initial=d*mh;tensorH=mh*mh
    Hout=tensorH//q;Wout=(d*tensorH)//q;Lout=Wout-d*Hout;rcb=d*Hout+Lout
    printed=F(initial*initial,q);corrected=F(initial*initial,d*q);bound=F(E+h,q)+F(h+1,2)
    out['paper_theorem_4_8_scalar_normalization']={'not_FHE_run':True,'purpose':'algebraic constant-polynomial consistency check; not a production cryptographic counterexample','N':N,'d':d,'q_l':q,'Q_l':Q,'h':h,'E_Relin_assumed':E,'secret_polynomial':'1','high_ciphertext_components':[q,0],'low_ciphertext_components':[0,0],'nonwrap_hypothesis':N*(mh*d+ml)**2+E+h<F(Q,2),'RCB_output_from_Def4_1_4_3_4_5':rcb,'printed_Theorem4_8_target':fr(printed),'composition_target_with_extra_d':fr(corrected),'absolute_printed_gap':fr(abs(rcb-printed)),'printed_bound':fr(bound),'corrected_gap_is_zero':rcb==corrected,'printed_gap_exceeds_printed_bound':abs(rcb-printed)>bound}
    assert out['paper_theorem_4_8_scalar_normalization']['nonwrap_hypothesis'] and out['paper_theorem_4_8_scalar_normalization']['printed_gap_exceeds_printed_bound']
    a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(out,ensure_ascii=False,indent=2)+'\n')
    print(json.dumps({'profiles':{k:{'QP_bits':v['QP_bit_length'],'nonminimum_roots':[x['role'] for x in v['ordered_primes'] if not x['frozen_root_is_minimum']],'scale_log2_round8':v['steps'][-1]['log2_output_approx']} for k,v in out['profiles'].items()},'proth_certificates':len(cert),'input_bounds':inp,'paper_printed_formula_discrepancy':True,'failures':out['failures']},ensure_ascii=False,indent=2))
if __name__=='__main__':main()
