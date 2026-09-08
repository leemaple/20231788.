#!/usr/bin/env python3
"""Independent scalar checks; no OpenFHE, FFT, encryption, or project-code import.
Use --literal-paper-red to expose the printed Theorem 4.8 contradiction (exit 1).
The ordinary mode verifies the counterexamples and the proposed named domain.
"""
from __future__ import annotations
import argparse
import json
from decimal import Decimal, localcontext, ROUND_CEILING
from fractions import Fraction as F


def require(ok: bool, what: str) -> None:
    if not ok:
        raise RuntimeError(what)


def nearest_ties_down(x: F) -> int:
    n, r = divmod(x.numerator, x.denominator)
    return n + (2*r > x.denominator)


def center(x: int, modulus: int) -> int:
    y = x % modulus
    return y - modulus if 2*y > modulus else y


def dyadic_input(s: int, a_base: int) -> tuple[int, int]:
    """Exact numerator pair over 2**75. Independently transcribed public formula."""
    t = s // 2
    a = (a_base << 65) - ((t % 16) << 59) + s
    b = (1 + (t // 16) % 8) << 65
    if (t // 512) % 2:
        b = -b
    return ((a,b),(-b,a),(-a,-b),(b,-a))[(t // 128) % 4]


def theorem_counterexample() -> dict:
    # N=2, s=1, d=13, q=5, Q=5*1009. All primes are 1 mod 2N.
    # Both input ciphertexts are (26,0); DCP=(high=(2,0),low=(0,0)).
    # Exact relinearization is permitted: choose P=5, rlk=(P,0), s=1.
    # It gives a global E_relin=0 modulo Q, not an observed error used as a bound.
    N, h, d, q, Q, H, L = 2, 1, 13, 5, 5045, 2, 0
    tensor_hi, tensor_lo = H*H, 2*H*L
    rs_hi = nearest_ties_down(F(tensor_hi, q))
    out = nearest_ties_down(F(d*tensor_hi+tensor_lo, q))
    rs_lo = out-d*rs_hi
    printed = F((d*H+L)**2, q)
    corrected = printed/d
    bound = F(N*L*L, d*q)+F(h, q)+F(h+1, 2)
    lhs = N*(d*H+L)**2+h  # E_relin=0: tensor's s**2 component is zero.
    require(F(lhs) < F(Q,2), 'counterexample must satisfy printed size hypothesis')
    require(abs(F(out)-printed) > bound, 'printed formula must be falsified')
    require(abs(F(out)-corrected) <= bound, 'normalized formula must survive witness')
    return dict(N=N, h=h, d=d, q=q, Q=Q, P=5, rlk=[5,0], input_plaintext=26,
                rs_high=rs_hi, rs_low=rs_lo, recombined=out,
                size_lhs=lhs, size_rhs=str(F(Q,2)), printed_target=str(printed),
                normalized_target=str(corrected), printed_error=str(abs(out-printed)),
                normalized_error=str(abs(out-corrected)), bound=str(bound))


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument('--literal-paper-red', action='store_true')
    args = ap.parse_args()
    result = {'scope':'scalar/specification only; no encrypted execution',
              'theorem_4_8':theorem_counterexample()}
    if args.literal_paper_red:
        print(json.dumps(result, indent=2))
        print('EXPECTED_RED: literal printed 1/q target violates its stated bound')
        return 1
    tests = []
    tests.append('thm4.8_missing_divisor_exact_counterexample')
    require(nearest_ties_down(F(3,2))==1 and nearest_ties_down(F(-3,2))==-2,
            'signed ties down')
    tests.append('signed_rounding_ties_down')
    # Conventional c0+c1*s: b=a=b'=a'=s=1.
    require(1-2+1 != (1+1)**2 and 1+2+1 == (1+1)**2,'Tensor sign')
    tests.append('printed_tensor_sign_counterexample')
    def relin(c2: int) -> tuple[int,int]:
        return nearest_ties_down(F(2*c2,5)), nearest_ties_down(F(3*c2,5))
    a,b,c = relin(3),relin(1),relin(4)
    carry = tuple(a[i]+b[i]-c[i] for i in range(2))
    require(carry == (-1,1), 'two-coordinate carry')
    result['lemma4_4_proof_step'] = {'difference':carry,
        'scope':'refutes the (0,e) intermediate identity; not the whole E_relin bound'}
    tests.append('lemma4.4_two_coordinate_rounding_carry')
    # Exhaustive small ring-coordinate DCP / correlated RS identities, no noise.
    Q,d,q = 85,13,5
    for x in range(d*Q):
        quotient = ((x-center(x,d))//d) % Q
        residue_formula = ((x % Q-center(x,d))*pow(d,-1,Q)) % Q
        require(quotient==residue_formula,'DCP residue arithmetic')
        require((d*quotient+center(x,d)-x) % Q==0,'DCP/RCB mod Q')
    tests.append('exhaustive_1105_dcp_coordinates')
    for h in range(Q):
        for lo in range(Q):
            rh = nearest_ties_down(F(center(h,Q),q))
            target = nearest_ties_down(F(center(d*h+lo,Q),q))
            rl = (target-d*rh) % (Q//q)
            require((d*rh+rl-target)%(Q//q)==0,'RS correlated reconstruction')
    tests.append('exhaustive_7225_correlated_rs_coordinates')
    require(F(3,4)**2+F(3,4)**2>1,'component-pass complex-fail witness')
    tests.append('component_vs_complex_norm_counterexample')
    require(center(55,101)==-46 and 101-2*abs(center(55,101))>0,
            'centered headroom cannot certify original lift')
    tests.append('headroom_is_not_nonwrap_certificate')
    domains={}
    for name,base in [('retained_near_unit',1015),('s100-annulus125-e80-v1',999)]:
        values=[dyadic_input(s,base) for s in range(16384)]
        norms=[a*a+b*b for a,b in values]
        require(len(set(values))==16384,'all slots distinct')
        require(max(norms)<2**150,'original and new families both in unit disk')
        domains[name]={'count':len(values),'unique':len(set(values)),
            'max_norm2_numerator':str(max(norms)),
            'min_norm2_numerator':str(min(norms)), 'norm2_denominator':str(2**150)}
        if base==999:
            require(max(norms)<(125<<68)**2,'new radius must be <125/128')
            require(min(norms)**256 * 2**20 > 2**(150*256),
                    'every true output norm must exceed 2^-10')
    result['domains']=domains
    tests+=['both_input_families_in_unit_disk','16384_exact_unique_dyadics_each',
            'annulus_radius_exact_integer_bound','annulus_nonvanishing_output_exact_bound']
    # Directed rounding on positive terms supplies a rigorous scalar upper bound.
    with localcontext() as c:
        c.prec=120; c.rounding=ROUND_CEILING
        T=Decimal(2)**-80
        r=Decimal(125)/128
        v=r+T
        p=Decimal(1)
        for _ in range(255): p=p*v
        K=Decimal(256)*p
        total=K+Decimal(1)/4
        require(total<Decimal('0.855'),'Lipschitz budget < 0.855 T')
        result['conditional_scalar_bound']={'T':str(T),'radius':str(r),
            'K_upper':str(K),'K_plus_one_quarter_upper':str(total),
            'assumptions':'complex E0<=T; complex A8<=T/4; observer errors separately controlled',
            'arithmetic':'120 decimal digits; every positive multiplication ROUND_CEILING'}
    tests.append('directed_256_power_conditioning_budget')
    # Pin-specific finite inversion sampler: sigma=3.19F is below 300 and
    # its table size ceil(sigma*12.00610553538285) is 39. This checks the
    # consequence of that inspected source, not the sampler probabilities.
    q_values=[1125899904679937,1125899903827969,1152921504598720513,
        1152921504597016577,1152921504595968001,1152921504595640321,
        1152921504593412097,1152921504592822273,1152921504592429057,
        1152921504589938689,1099510054913]
    Qroot=1
    for v in q_values: Qroot*=v
    noise_bound=39*(32768+1+128)
    require(noise_bound==1282983,'finite PKE coefficient bound')
    require(2*((1<<164)+noise_bound)<Qroot,'conditional fresh unique lift')
    result['conditional_fresh_lift']={'root_Q':str(Qroot),'root_Q_bits':Qroot.bit_length(),
        'sampler_abs_bound':39,'aggregate_coefficient_bound':noise_bound,
        'message_coefficient_bound':str(1<<164),
        'assumptions':'pinned successful Peikert samples; dense |v_j|<=1; h128; noiseScale1; |m_j|<=2^164',
        'conclusion':'fresh center(m+noise)=m+noise; not an intermediate circuit lift certificate'}
    tests.append('finite_sampler_conditional_fresh_nonwrap')
    # Independent closed-form scale vs recursive exact rationals, all eight q's.
    scale=F(1<<100); scale_rows=[str(scale)]
    for k in range(1,9):
        scale=scale*scale/(q_values[-1]*q_values[10-k])
        den=1
        for j in range(1,k+1): den*=(q_values[-1]*q_values[10-j])**(2**(k-j))
        require(scale==F(1<<(100*2**k),den),'actual-prime exact scale')
        require(scale>F(1<<100),'prime-near-power mismatch is not zero')
        scale_rows.append({'round':k,'numerator_bits':scale.numerator.bit_length(),
                           'denominator_bits':scale.denominator.bit_length()})
    result['exact_scale_recurrence']=scale_rows
    tests.append('eight_exact_prime_scale_receipts')
    result['passed_checks']=tests
    result['count']=len(tests)
    result['status']='PASS_SCALAR_ONLY'
    print(json.dumps(result, indent=2, ensure_ascii=False))
    return 0

if __name__=='__main__':
    raise SystemExit(main())
