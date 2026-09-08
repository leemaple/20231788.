#!/usr/bin/env python3
"""INITIAL-LIFT-NONWRAP-01: bounded integer/Fraction checks; no transforms,
no random source, no compilation, and no execution of supplied project code.
The conditional spectral certificate does NOT assert its encoding premise.
"""
from __future__ import annotations
import argparse
import hashlib
import itertools
import json
import math
from pathlib import Path
import re
import sys
from fractions import Fraction as F

if hasattr(sys, 'set_int_max_str_digits'):
    sys.set_int_max_str_digits(200000)
N, HWT, SUPPORT, BITS = 32768, 128, 39, 192
SOURCE_SHA = '6781d01ff9b4012e7c6d14d16f482def0821c54f4e81e2de80bd8db9d1aea41b'
D = 1099510054913
P = 1152921504606584833
Q_PRIMES = [1125899904679937,1125899903827969,1152921504598720513,
1152921504597016577,1152921504595968001,1152921504595640321,
1152921504593412097,1152921504592822273,1152921504592429057,1152921504589938689]
ROOTS = [26113207984,150640639383,100545759574150,31693996050849,
88651361085495,9679305630873,24428769072221,18776242964106,5821397352863,33888991361320]
S0 = 1 << 100
RD = F((1+HWT)*(D-1), 2)
EC = SUPPORT*(N+1+HWT)
ES = N*EC


def ceilf(x: F) -> int:
    return -((-x.numerator)//x.denominator)


def up(x: F) -> F:
    """Exact upward rounding to a multiple of 2^-BITS."""
    if x < 0:
        raise ValueError('upper-bound arithmetic expects nonnegative input')
    y=F(ceilf(x*(1<<BITS)),1<<BITS)
    assert x <= y < x+F(1,1<<BITS)
    return y


def floor_log2(x: F) -> int | None:
    if x <= 0:
        return None
    e=x.numerator.bit_length()-x.denominator.bit_length()
    if e >= 0:
        if x.numerator < x.denominator << e: e-=1
    elif x.numerator << (-e) < x.denominator: e-=1
    assert F(2)**e <= x < F(2)**(e+1)
    return e


def sci_interval(x: F, digits: int=12) -> dict:
    """Integer-only decimal display; the exact Fraction is authoritative."""
    if x == 0: return {'lower':'0','upper':'0'}
    if x < 0:
        t=sci_interval(-x,digits)
        return {'lower':'-'+t['upper'],'upper':'-'+t['lower']}
    e=len(str(x.numerator))-len(str(x.denominator))
    ten=F(10)**e
    if x < ten: e-=1;ten/=10
    while x >= 10*ten: e+=1;ten*=10
    scaled=x/ten*(10**digits)
    lo=scaled.numerator//scaled.denominator
    hi=ceilf(scaled)
    def text(a): return f'{a//10**digits}.{a%10**digits:0{digits}d}e{e:+d}'
    return {'lower':text(lo),'upper':text(hi)}


def rat(x, exact=True):
    x=F(x)
    r={'floor_log2':floor_log2(x),'decimal_enclosure':sci_interval(x)}
    if exact: r.update(numerator=str(x.numerator),denominator=str(x.denominator))
    return r


def scale_description(step:int,S:F)->dict:
    # Exact compact representation: numerator 2^(100*2^step), odd denominator.
    description=rat(S,False)
    factors=[{'base':str(D),'exponent':(1<<step)-1}]
    factors += [{'base':str(Q_PRIMES[10-j]),'exponent':1<<(step-j)} for j in range(1,step+1)]
    denominator=math.prod(int(t['base'])**t['exponent'] for t in factors)
    assert S==F(1<<(100*(1<<step)),denominator)
    description.update(exact_numerator_power_of_two=100*(1<<step),exact_denominator_factors=factors)
    return description


def center(x:int,q:int)->int:
    if q<=1 or q%2==0: raise ValueError('odd modulus required')
    r=x%q
    return r-q if 2*r>q else r


def bk(active):
    return F(N*SUPPORT*sum(q-1 for q in active)+(1+HWT)*(P-1),P)


def bindings(root:Path):
    src=root/'project/src/repeated_mult2.cpp'
    data=src.read_bytes()
    assert hashlib.sha256(data).hexdigest()==SOURCE_SHA
    text=data.decode()
    table=text.split('kPaperQ{{',1)[1].split('}};',1)[0]
    pairs=[tuple(map(int,m)) for m in re.findall(r'\{(\d+)ULL,(\d+)ULL\}',table)]
    assert pairs == list(zip(Q_PRIMES+[D],ROOTS+[121567553]))
    assert 'kPaperP{1152921504606584833ULL,4443670208963ULL}' in text
    for q,r in pairs+[(P,4443670208963)]:
        assert (q-1)%(2*N)==0 and pow(r,N,q)==q-1 and pow(r,2*N,q)==1
    for a,b in itertools.combinations(Q_PRIMES+[D,P],2): assert math.gcd(a,b)==1
    oracle=(root/'project/tests/paper_full_eight_square_oracle.h').read_text()
    for needle in ['Real(1015)/1024-Real(t%16)/65536+Real(s)*Pow2(-75)',
                   'Real(1+(t/16)%8)/1024', 'if ((t/512)%2)', 'switch ((t/128)%4)']:
        assert needle in oracle
    enc=root/'project/src/high_precision_client_io.cpp'
    return {'production_source_sha256':SOURCE_SHA,
            'encoder_sha256':hashlib.sha256(enc.read_bytes()).hexdigest(),
            'q_root_pairs_checked':12,'pairwise_coprime':True,
            'modular_exponentiation_only':True,'primality_reproved':False,
            'input_formula_text_matches':True}


def public_input():
    # Public scalar formula only. No inverse/forward embedding is evaluated.
    maximum=F(0); arg=None
    for s in range(N//2):
        t=s//2
        a=F(1015,1024)-F(t%16,65536)+F(s,1<<75)
        b=F(1+(t//16)%8,1024)
        x=a*a+b*b
        if x>maximum: maximum,arg=x,s
    uniform=(F(1015,1024)+F(N//2-1,1<<75))**2+F(1,128)**2
    target=F(127,128)
    assert maximum<=uniform<target*target
    # Outward square-root enclosure by one integer square root.
    den=1<<BITS
    scaled=ceilf(maximum*den*den)
    num=math.isqrt(scaled)
    if num*num<scaled:num+=1
    r=F(num,den)
    assert maximum<=r*r
    return {'scalar_slots_examined':N//2,'first_maximum_slot':arg,
            'maximum_modulus_squared':rat(maximum),
            'uniform_modulus_squared_bound':rat(uniform),
            'maximum_modulus_upper':rat(r),'encoding_cap_K':rat(target),
            'ideal_input_is_below_K':True,
            'gap_K_minus_ideal_upper':rat(target-r),
            'actual_encoder_canonical_norm_computed':False}


def coarse_c25(K:F):
    """A conditional seed M_c <= K*S0. True PKE and DCP support are added.
    Upward integer rounding controls bit-size. No spectral contraction assumed.
    The untruncated bounds remain valid after a failure, but certify no wrap there.
    """
    H=ceilf((K*S0+EC+RD)/D); L=ceilf(RD)
    out=[]
    for k in range(8):
        active=Q_PRIMES[:10-k];Q=math.prod(active);mu=active[-1]
        B=bk(active);Rm=F((1+HWT)*(mu-1),2*mu)
        term_hi=N*D*H*H;term_cross=2*N*H*L
        tensor=term_hi+term_cross
        nw=F(tensor)+2*B+mu*Rm
        Hn=ceilf(F(N*H*H,mu)+(B+RD)/(D*mu)+Rm)
        Ln=ceilf((2*N*H*L+B+RD)/mu+(1+D)*Rm)
        out.append({'step':k+1,'Q_bits':Q.bit_length(),'Q_next_bits':(Q//mu).bit_length(),
                    'H_before':str(H),'L_before':str(L),
                    'dominant_N_d_H_squared':rat(term_hi,False),
                    'cross_2NHL':rat(term_cross,False),
                    'NW_over_Qhalf':rat(2*nw/Q,False),
                    'NW_pass':2*nw<Q,
                    'H_next':str(Hn),'L_next':str(Ln),
                    'RCB_next_bound_over_Qnext_half':rat(F(2*(D*Hn+Ln),Q//mu),False),
                    'RCB_next_pass':2*(D*Hn+Ln)<Q//mu})
        H,L=Hn,Ln
    first=next((r['step'] for r in out if not r['NW_pass']),None)
    return {'seed_encoding_coefficient_bound':rat(K*S0),'seed_is_conditional':True,
            'initial_H':out[0]['H_before'],'initial_L':out[0]['L_before'],
            'first_sufficient_NW_failure_step':first,'actual_wrap_conclusion':None,'steps':out}


def spectral(K:F):
    """Compatible global lifts, no intermediate centering, exact upward bounds.
    K is a PREMISE on the actual deterministic encoder, NOT a measured value.
    """
    a=up(K+F(ES+N*RD,S0));b=up(F(N*RD,S0));z=up(K+F(ES,S0))
    S=F(S0); rows=[]; initial={'a':rat(a),'b':rat(b),'z':rat(z)}
    Q0=math.prod(Q_PRIMES);F0=D*Q0
    initial_checks={
        'actual_encoding_phase_mod_F':2*K*S0<F0,
        'fresh_phase_mod_F':2*(K*S0+ES)<F0,
        'DCP_high_mod_Q0':2*S0*a/D<Q0,
        'DCP_low_mod_Q0':2*S0*b<Q0,
        'DCP_RCB_mod_Q0':2*S0*(a+b)<Q0,
    }
    for k in range(8):
        active=Q_PRIMES[:10-k];Q=math.prod(active);mu=active[-1];Qn=Q//mu
        B=bk(active);Rm=F((1+HWT)*(mu-1),2*mu);Sn=S*S/(D*mu)
        U=N*(B+RD)/(mu*Sn);V=N*Rm/Sn
        ht=(S*a/D)**2;lt=2*S*S*a*b/D
        raised=D*ht+N*B
        x=ht+N*(B+RD)/D;y=lt+N*(B+RD)
        t=D*ht+lt+2*N*B
        hn=x/mu+N*Rm;ln=y/mu+(1+D)*N*Rm
        cn=t/mu+N*Rm
        an=up(a*a+U+D*V)
        bn=up(2*a*b+U+(1+D)*V)
        # A separate recombined bound retains the one-rescale cancellation.
        local=up(b*b+2*N*B/(mu*Sn)+V)
        zn=up(z*z+local)
        checks={
            'input_high':2*S*a/D<Q,
            'input_low':2*S*b<Q,
            'input_RCB':2*S*(a+b)<Q,
            'tensor_high_mod_Q':2*ht<Q,
            'tensor_low_mod_Q':2*lt<Q,
            'raised_relin_high_mod_dQ':2*raised<D*Q,
            'Relin2_high_mod_Q':2*x<Q,
            'Relin2_low_mod_Q':2*y<Q,
            'Relin2_RCB_mod_Q':2*t<Q,
            'C24_joint_NW_spectral_sufficient':2*(t+mu*N*Rm)<Q,
            'RS_high_mod_Qnext':2*hn<Qn,
            'RS_low_mod_Qnext':2*ln<Qn,
            'RS_RCB_mod_Qnext':2*cn<Qn,
            'recursive_pair_RCB_mod_Qnext':2*Sn*(an+bn)<Qn,
            'recursive_combined_mod_Qnext':2*Sn*zn<Qn,
        }
        # Check recurrence formula and its exact scale normalization.
        assert Sn*an/D>=hn and Sn*bn>=ln
        assert D*mu*Sn==S*S
        rows.append({'step':k+1,'active_Q':str(Q),'mu':str(mu),'next_Q':str(Qn),
             'S_before':scale_description(k,S),'S_after':scale_description(k+1,Sn),
             'BK_coefficient':rat(B),'Rm_coefficient':rat(Rm),
             'alpha_additive_U':rat(U,False),'rescale_V':rat(V,False),
             'a_before':rat(a),'b_before':rat(b),'z_before':rat(z),
             'a_after':rat(an),'b_after':rat(bn),'z_after':rat(zn),
             'local_error_bound':rat(local),'local_error_bound_over_E80':rat(local*(1<<80),False),
             'joint_NW_over_Qhalf':rat(2*(t+mu*N*Rm)/Q,False),
             'pair_RCB_next_over_Qnext_half':rat(2*Sn*(an+bn)/Qn,False),
             'combined_next_over_Qnext_half':rat(2*Sn*zn/Qn,False),
             'checks':checks,'all_phase_checks_pass':all(checks.values())})
        S,a,b,z=Sn,an,bn,zn
    first=next((r['step'] for r in rows if not r['all_phase_checks_pass']),None)
    return {'schema':'conditional-spectral-lift-certificate-v1','K':rat(K),
        'premise_actual_encoding_canonical_bound_verified':False,
        'rounding_bits':BITS,'rounding':'ceil(x*2^192)/2^192 after each nonnegative bound update',
        'initial_bounds':initial,'initial_checks':initial_checks,
        'all_phase_checks_pass_given_premise':all(initial_checks.values()) and first is None,
        'first_phase_bound_failure_step':first,'actual_historical_wraps':None,
        'final_normalized_M_bound':rat(z),'final_scale':scale_description(8,S),
        'final_Q_half_div_S':rat(F(math.prod(Q_PRIMES[:2]),2)/S),
        'steps':rows}


def toy_checks():
    results=[]
    def record(name,**kw):results.append({'name':name,'status':'PASS',**kw})
    # DCP coordinate lift invariance: finite deterministic grid, not encryption.
    count=0
    for d,Q in [(3,35),(5,77),(7,55)]:
        F0=d*Q
        for c0,c1,s in itertools.product(range(-13,14,3),range(-9,10,3),[-1,0,1]):
            phase=c0+s*c1
            r0,r1=center(c0,d),center(c1,d);rho=r0+s*r1
            assert (phase-rho)%d==0
            high=(phase-rho)//d
            for g0,g1 in [(0,0),(1,-1),(-2,3)]:
                h2=(c0+F0*g0-r0+s*(c1+F0*g1-r1))//d
                assert (h2-high)%Q==0 and (d*h2+rho-phase)%Q==0
            count+=1
    record('DCP_full_basis_lift_invariance',cases=count)
    # Missing Q before division by d: congruence alone does not license division.
    d,Q,p=3,35,40;rho=center(p,d)
    good=(p-rho)//d;wrong=F(center(p,Q)-rho,d)
    assert good==13 and wrong==F(4,3) and wrong.denominator!=1
    record('wrong_Q_center_before_DCP_counterexample',d=d,Q=Q,p=p,rho=rho,
           correct_high=good,wrong_high='4/3')
    # Rescale's old-Q multiple becomes Q/m, NOT zero as an integer.
    Q,mu,p=35,7,20;rem=center(p,mu)
    unwrapped=F(p-rem,mu);observed=center(int(unwrapped),Q//mu)
    assert unwrapped==3 and observed==-2 and unwrapped-observed==Q//mu
    record('missing_Qprime_wrap_counterexample',Q=Q,mu=mu,lift=p,remainder=rem,
           unwrapped=3,centered=-2,missing_term=5)
    # Compatible RS2 lifts with actual component remainder terms.
    count=0
    d,Q,mu,s=3,385,7,1;F0=d*Q;Qn=Q//mu
    for h,l,nuH,nuL,a1,b1 in itertools.product([-9,2,17],[-7,0,5],[-3,1],[0,4],[-4,8],[-5,6]):
        A1=a1;A0=d*h*h+nuH-s*A1+F0
        r0,r1=center(A0,d),center(A1,d);rho=r0+s*r1
        H0=(A0-r0)//d;H1=(A1-r1)//d
        X=F(h*h)+F(nuH-rho,d)
        assert X.denominator==1
        L1=b1;L0=2*h*l+nuL-s*L1
        Y=2*h*l+nuL+rho
        C0=d*H0+L0+r0;C1=d*H1+L1+r1
        epsH=-F(center(H0,mu)+s*center(H1,mu),mu)
        epsC=-F(center(C0,mu)+s*center(C1,mu),mu)
        hn=X/mu+epsH;ln=F(Y,mu)+epsC-d*epsH
        assert hn.denominator==ln.denominator==1
        obsH=((H0-center(H0,mu))//mu+s*((H1-center(H1,mu))//mu))%Qn
        obsC=((C0-center(C0,mu))//mu+s*((C1-center(C1,mu))//mu))%Qn
        assert (int(hn)-obsH)%Qn==0 and (d*int(hn)+int(ln)-obsC)%Qn==0
        M=d*h+l
        assert d*hn+ln==F(M*M-l*l,d*mu)+F(nuH+nuL,mu)+epsC
        count+=1
    record('RS2_compatible_integer_lifts_and_normalization',cases=count)
    # Exact N=4 analytic canonical norm, no transform evaluated.
    # max |sigma(a)|^2 = sum a_i^2 + sqrt(2)*abs(a0*(a1-a3)+a2*(a1+a3)).
    p=[3,1,1,2];c=[center(x,5) for x in p]
    assert c==[-2,1,1,2]
    def form(x):return sum(a*a for a in x),abs(x[0]*(x[1]-x[3])+x[2]*(x[1]+x[3]))
    assert form(p)==(15,0) and form(c)==(10,5)
    assert max(map(abs,c))<max(map(abs,p)) and 10+5*F(7,5)>15
    assert F(7,5)**2<2  # rigorous lower enclosure for sqrt(2)
    record('centering_is_not_canonical_contraction',before=p,after=c,
           norm_squared_before='15',norm_squared_after='10+5*sqrt(2)',
           certified_increase=True)
    assert form([1,1,1,1])==(4,2)
    record('coefficient_norm_is_not_canonical_norm',coefficient_max=1,
           canonical_max_squared='4+2*sqrt(2)',valid_conversion='coefficient <= canonical <= N*coefficient')
    # Failure of a sufficient upper bound is not a measured wrap.
    assert 2*5<11 and center(5,11)==5
    assert not 2*6<11 and center(1,11)==1 and center(6,11)==-5
    record('strict_half_modulus_boundary_and_nonconverse',q=11,
           passing_witness=5,actual_wrap_witness=6,loose_bound=6,nonwrap_value_despite_loose_bound=1)
    # Stable disagreement/margin guards do not bound a common bias.
    approximate=F(1,8);ideal=F(9,8)
    round_down_half=lambda x: x.numerator//x.denominator+(1 if x-(x.numerator//x.denominator)>F(1,2) else 0)
    assert round_down_half(approximate)==0 and round_down_half(ideal)==1
    assert F(3,8)>F(1,1<<400)
    record('two_precision_agreement_is_not_ideal_inverse_proof',primary='1/8',check='1/8',
           possible_unconstrained_ideal='9/8',common_bias_not_excluded_by_guard=True,
           production_defect_claim=False)
    # NWT is deliberately a support bound, not a probability/security result.
    sigma_upper=F(319001,100000);multiplier_upper=F(12007,1000)
    assert sigma_upper*multiplier_upper<39 and multiplier_upper<13
    assert EC==1282983 and ES==42040786944
    record('finite_noise_support_and_PKE_envelope',sigma_nominal_upper=str(sigma_upper),
           cutoff_multiplier_upper=str(multiplier_upper),Peikert_support_envelope=39,
           PKE_coefficient_bound=EC,PKE_canonical_bound=ES)
    return results


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input-dir',required=True,type=Path)
    parser.add_argument('--out-dir',required=True,type=Path)
    args=parser.parse_args()
    args.out_dir.mkdir(parents=True,exist_ok=True)
    binding=bindings(args.input_dir)
    inp=public_input();K=F(127,128)
    coarse=coarse_c25(K);assert coarse['first_sufficient_NW_failure_step']==5
    good=spectral(K);assert good['all_phase_checks_pass_given_premise']
    bad=spectral(F(1));assert bad['first_phase_bound_failure_step']==8
    models=toy_checks();assert all(r['status']=='PASS' for r in models)
    Q0=math.prod(Q_PRIMES);F0=D*Q0
    Mguard=F(F0-1,2)
    guard={'scope':'Only the exact centered-CRT guard; deliberately does not turn floating unit/stability guards into an ideal inverse theorem.',
           'M_coefficient_upper':rat(Mguard),
           'true_unwrapped_H0_coefficient_upper':rat((Mguard+EC+RD)/D),
           'true_L0_coefficient_upper':rat(RD),
           'fresh_margin_sufficient':2*(Mguard+EC)<F0,
           'fresh_margin_excess_over_Fhalf':rat(Mguard+EC-F(F0,2)),
           'recombined_margin_sufficient':2*(Mguard+EC)<Q0,
           'recombined_bound_over_Q0half':rat(2*(Mguard+EC)/Q0,False),
           'actual_wrap':None,'epsilon_guard_stronger_range_translation_not_used':True}
    ideal_r=F(int(inp['maximum_modulus_upper']['numerator']),int(inp['maximum_modulus_upper']['denominator']))
    finalS=F(S0)
    for mu in reversed(Q_PRIMES[2:]):finalS=finalS*finalS/(D*mu)
    ideal_final=finalS*ideal_r**256+F(1,2)
    assert 2*ideal_final<math.prod(Q_PRIMES[:2])
    summary={'schema':'initial-lift-nonwrap-bounded-checks-v1','status':'PASS',
       'input_binding':binding,'N':N,'h':HWT,'d':str(D),'P':str(P),
       'PKE_noise_coefficient_bound':str(EC),'PKE_noise_canonical_bound':str(ES),
       'Rd_coefficient':rat(RD),'Rd_canonical':rat(N*RD),
       'public_near_unit_input':inp,'exact_center_guard_only':guard,
       'conditional_C25_first_failure':coarse['first_sufficient_NW_failure_step'],
       'conditional_refined_all_phase_checks':good['all_phase_checks_pass_given_premise'],
       'K1_negative_control_first_failure':bad['first_phase_bound_failure_step'],
       'ideal_final_encoding_coeff_bound_over_Qhalf':rat(2*ideal_final/math.prod(Q_PRIMES[:2]),False),
       'ideal_final_encoding_nonwrap_proved_under_ideal_nearest_encoding_definition':True,
       'actual_encoder_premise_verified':False,'historical_S100_E80':'FAIL (supplied evidence; not re-run)',
       'historical_S116':'changed parameters PASS (not re-run)',
       'historical_annulus125':'changed inputs and independent fresh sample PASS (not re-run)',
       'model_groups':models,'FHE_or_sampling_or_FFT_NTT_executed':False}
    for name,obj in [('results.json',summary),('conditional_spectral_certificate.json',good),
                     ('conditional_coarse_c25.json',coarse),('K1_negative_control.json',bad)]:
        (args.out_dir/name).write_text(json.dumps(obj,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    table=['step\tQ_bits\tcoarse_NW_ratio_log2_floor\tcoarse_NW_pass\trefined_NW_ratio_log2_floor\trefined_all_phase_checks\tcombined_normalized_upper\tL_normalized_upper']
    for c,r in zip(coarse['steps'],good['steps']):
        table.append('\t'.join(map(str,[c['step'],c['Q_bits'],c['NW_over_Qhalf']['floor_log2'],c['NW_pass'],
                r['joint_NW_over_Qhalf']['floor_log2'],r['all_phase_checks_pass'],
                r['z_after']['decimal_enclosure']['upper'],r['b_after']['decimal_enclosure']['upper']])))
    (args.out_dir/'summary.tsv').write_text('\n'.join(table)+'\n',encoding='utf-8')
    print(json.dumps({'status':'PASS','model_groups':len(models),'scalar_input_values':N//2,
        'first_coarse_sufficient_failure':coarse['first_sufficient_NW_failure_step'],
        'conditional_refined_8_step_certificate':True,'actual_encoding_bound_verified':False,
        'K1_negative_control_first_failure':bad['first_phase_bound_failure_step'],
        'outputs':5},ensure_ascii=False))

if __name__=='__main__':main()
