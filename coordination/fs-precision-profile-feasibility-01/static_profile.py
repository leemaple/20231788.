"""Exact scalar certificate for one experimental profile; no crypto or security claim."""
from fractions import Fraction
import json
from math import log2, prod
from pathlib import Path


# Retained exact original-profile consumed moduli/roots and reserved P.
CONSUMED = (
    (1152921504598720513,100545759574150),
    (1152921504597016577,31693996050849),
    (1152921504595968001,88651361085495),
    (1152921504595640321,9679305630873),
    (1152921504593412097,24428769072221),
    (1152921504592822273,18776242964106),
    (1152921504592429057,5821397352863),
    (1152921504589938689,33888991361320),
)
P = (1152921504606584833,4443670208963)
FROZEN_NEW = (
    (288230191468118017,43136605093011213,5),
    (288230165698314241,82872750907637397,7),
    (72057589742960641,50608680790172261,11),
)


def require(ok, message):
    if not ok:
        raise ValueError(message)


def certify(profile):
    """Validate candidate scalar data and return only static, conditional claims."""
    require(profile['profile_id'] == 'experimental-s116-d56-b58-v1', 'profile identity')
    require(profile['scale_bits'] == 116 and profile['base_metadata_bits'] == 58,
            'initial scale and FIXEDMANUAL metadata must match the explicit profile')
    fresh = 1 << profile['scale_bits']
    require(fresh == (1 << profile['base_metadata_bits'])**2, 'metadata coherence')
    new = profile['new_primes']
    require(len(new) == 3, 'two base primes and one divisor required')
    for record, bits in zip(new, (58,58,56)):
        q, root, witness = record['modulus'], record['root'], record['witness']
        require(all(type(v) is int for v in (q,root,witness)), 'integer prime certificate')
        require(q.bit_length() == bits and (q-1) % (1 << 32) == 0, 'prime shape')
        odd = (q-1) >> 32
        require(odd % 2 == 1 and 0 < odd < (1 << 32), 'Proth shape')
        require(1 < witness < q and pow(witness,(q-1)//2,q) == q-1,
                'primality witness')
        # For each prime divisor p of q the displayed -1 forces v2(ord_p(a))=32,
        # hence p >= 2^32+1. Since q < 2^64, it cannot have two such factors.
        # This certifies primality, not merely a probable-prime test.
        require(0 < root < q and pow(root,32768,q) == q-1 and pow(root,65536,q) == 1,
                'primitive NTT root')
    ordered = [(r['modulus'],r['root']) for r in new[:2]] + list(CONSUMED)
    ordered.append((new[2]['modulus'],new[2]['root']))
    q = [v[0] for v in ordered]
    require(len(set(q+[P[0]])) == 12, 'Q/P collision')
    for modulus,root in ordered+[P]:
        require(modulus.bit_length() <= 60 and modulus % 65536 == 1,
                'native64 tower size or NTT congruence')
        require(pow(root,32768,modulus) == modulus-1 and pow(root,65536,modulus) == 1,
                'retained primitive root')
    require(tuple((r['modulus'],r['root'],r['witness']) for r in new) == FROZEN_NEW,
            'frozen candidate identity')
    families = []
    active = list(q)
    for _ in range(8):
        families.append({'Q':list(active),'consumed_modulus':active[-2]})
        del active[-2]
    require(active == q[:2]+[q[-1]], 'terminal base/divisor preservation')
    scales = [Fraction(fresh)]
    for family in families:
        m = family['consumed_modulus']
        scales.append(scales[-1]**2 / (q[-1]*m))
    drift = all(Fraction(99,100) < s/fresh < Fraction(101,100) for s in scales)
    require(drift, 'scale drift exceeds one percent')
    terminal_ratio = Fraction(q[0]*q[1])/scales[8]
    require(Fraction(99,100) < terminal_ratio < 1, 'terminal capacity ratio')
    rho, target = Fraction(127,128), Fraction(1,2**80)
    prospective_capacity = []
    for stage,scale in enumerate(scales):
        active_q = prod(q[:10-stage])
        # Component error <=T implies complex modulus error <=sqrt(2)T<2T.
        # This is conditional, not a proof
        # of real intermediate tensor/relin lifts or an observation of E80.
        margin = Fraction(active_q,2)/scale - (rho**(2**stage)+2*target)
        prospective_capacity.append(margin > 0)
    require(all(prospective_capacity), 'prospective recombined coefficient capacity')
    return {
        'static_status':'PASS', 'profile_id':profile['profile_id'],
        'scale0':{'numerator':str(fresh),'denominator':'1'},
        'scales':[{'numerator_hex':hex(s.numerator),'denominator_hex':hex(s.denominator)} for s in scales],
        'ordered_Q':[{'modulus':m,'root':r} for m,r in ordered],
        'reserved_P':{'modulus':P[0],'root':P[1]},
        'new_prime_certificates':'DETERMINISTIC_PROTH_WITNESSES_PASS',
        'retained_primality':'INHERITED_PINNED_PROFILE_NOT_REPROVED',
        'native_tower_bits':[m.bit_length() for m in q],
        'family_q_counts':[len(family['Q']) for family in families],
        'families':families,
        'scale_drift_less_than_one_percent':drift,
        'scale8_over_scale0_approx':float(scales[8]/fresh),
        'terminal_q_over_scale_between_99_100_and_1':True,
        'terminal_q_over_scale_approx':float(terminal_ratio),
        'prospective_recombined_capacity':prospective_capacity,
        'prospective_capacity_assumption':'every canonical slot, both components, at every recombined endpoint stage r=0..8 has error <=2^-80; not observed; 2T complex allowance; ten anchors do not establish this premise',
        'min_consumed_over_divisor_approx':float(Fraction(min(q[2:10]),q[-1])),
        'root_QP_bits_approx':log2(prod(q)*P[0]),
        'security_status':'UNRESOLVED', 'E80_status':'NOT_TESTED',
        'adoption_status':'NOT_ADOPTED', 'intermediate_nonwrap':'NOT_PROVED',
        'crypto_executed':False,
    }


if __name__ == '__main__':
    candidate = json.loads(Path(__file__).with_name('candidate.json').read_text())
    print(json.dumps(certify(candidate),indent=2))
