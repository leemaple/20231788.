#!/usr/bin/env python3
"""Exact public S100 local error bounds, using only integers and Fraction.

Optional --input-root checks constants against the supplied fixed source.
No Gaussian sampling, OpenFHE calls, transforms, or primality experiments.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import re
import sys
from fractions import Fraction
from math import gcd, prod
from pathlib import Path

if hasattr(sys, 'set_int_max_str_digits'):
    sys.set_int_max_str_digits(100000)

PAIRS = (
 (1125899904679937,26113207984), (1125899903827969,150640639383),
 (1152921504598720513,100545759574150), (1152921504597016577,31693996050849),
 (1152921504595968001,88651361085495), (1152921504595640321,9679305630873),
 (1152921504593412097,24428769072221), (1152921504592822273,18776242964106),
 (1152921504592429057,5821397352863), (1152921504589938689,33888991361320),
 (1099510054913,121567553))
P_PAIR = (1152921504606584833,4443670208963)
N, H, EMAX = 32768, 128, 39
SRC_SHA = '6781d01ff9b4012e7c6d14d16f482def0821c54f4e81e2de80bd8db9d1aea41b'


def need(ok: bool, why: str) -> None:
    if not ok:
        raise AssertionError(why)


def ceil_fraction(x: Fraction) -> int:
    return -(-x.numerator // x.denominator)


def floorlog2(x: Fraction) -> int:
    need(x > 0, 'log domain')
    k = x.numerator.bit_length() - x.denominator.bit_length()
    threshold = Fraction(2**k) if k >= 0 else Fraction(1, 2**(-k))
    return k if x >= threshold else k - 1


def sci(x: Fraction, digits: int = 7) -> str:
    """Truncated scientific display, never used for assertions."""
    if x == 0:
        return '0'
    sign = '-' if x < 0 else ''
    x = abs(x)
    k = len(str(x.numerator)) - len(str(x.denominator))
    ten = Fraction(10**k) if k >= 0 else Fraction(1, 10**(-k))
    if x < ten:
        k -= 1
        ten /= 10
    coeff = x / ten
    mantissa = coeff.numerator * (10 ** (digits - 1)) // coeff.denominator
    text = str(mantissa).rjust(digits, '0')
    return f'{sign}{text[0]}.{text[1:]}e{k:+d}'


def quantity(x: Fraction) -> dict:
    return {'numerator': str(x.numerator), 'denominator': str(x.denominator),
            'display_truncated': sci(x), 'floor_log2': floorlog2(x) if x > 0 else None}


def calculate(source_root: Path | None) -> dict:
    source_match = None
    if source_root is not None:
        path = source_root / 'project/src/repeated_mult2.cpp'
        data = path.read_bytes()
        need(hashlib.sha256(data).hexdigest() == SRC_SHA, 'wrong frozen source')
        text = data.decode('utf-8')
        block = text.split('kPaperQ{{', 1)[1].split('}};', 1)[0]
        pairs = tuple((int(a), int(b)) for a, b in re.findall(r'\{(\d+)ULL,(\d+)ULL\}', block))
        need(pairs == PAIRS, 'source constants differ')
        p = re.search(r'kPaperP\{(\d+)ULL,(\d+)ULL\}', text)
        need(p is not None and tuple(map(int, p.groups())) == P_PAIR, 'P differs')
        source_match = True
    all_pairs = (*PAIRS, P_PAIR)
    need(all(gcd(x[0], y[0]) == 1 for i, x in enumerate(all_pairs) for y in all_pairs[i + 1:]), 'gcd')
    for q, root in all_pairs:
        need(q % (2 * N) == 1, 'root modulus congruence')
        need(pow(root, N, q) == q - 1 and pow(root, 2 * N, q) == 1, 'root order check')
    # A rational envelope handles binary evaluation of 12.00610553538285 and 3.19F:
    # M < 12.007 and 3.19F < 3.20. Therefore ceil(M*sigma) <= 39.
    support_one = ceil_fraction(Fraction(12007, 1000))
    support_nominal_envelope = ceil_fraction(Fraction(12007, 1000) * Fraction(16, 5))
    need(support_one == 13 and support_nominal_envelope == 39, 'support envelope')
    P, d = P_PAIR[0], PAIRS[-1][0]
    T = Fraction(1, 2**80)
    S = Fraction(2**100)
    rows = []
    for f in range(8):
        qs = [v[0] for v in PAIRS[:10-f]]
        Q, m = prod(qs), qs[-1]
        BK = Fraction(N * EMAX * sum(q - 1 for q in qs) + (1 + H) * (P - 1), P)
        B2 = 2 * BK
        next_S = S * S / (d * m)
        relin_local = d * N * B2 / (S * S)
        rs_local = Fraction(N * (H + 1) * (m - 1), 2 * m) / next_S
        need(B2 < Fraction(Q, 2), 'local error itself could wrap')
        need(relin_local < T / (2**40), 'unexpected key-noise bound')
        # The coarse deterministic rescale bound alone does not certify E80.
        need(rs_local > T, 'rescale comparison changed')
        rows.append({'family': f, 'active_towers': len(qs), 'raised_towers': len(qs)+1,
                     'Q': str(Q), 'm': str(m), 'B2_is_less_than_Q_half': True, 'exact_S': quantity(S), 'exact_S_next': quantity(next_S),
                     'BK_coefficient': quantity(BK), 'B2_coefficient': quantity(B2),
                     'normalized_local_relin_bound': quantity(relin_local),
                     'normalized_local_relin_bound_over_T': quantity(relin_local / T),
                     'normalized_local_RS_bound': quantity(rs_local),
                     'normalized_local_RS_bound_over_T': quantity(rs_local / T),
                     'unit_amplitude_tensor_scale_over_Qhalf': quantity((S*S/d) / Fraction(Q,2))})
        S = next_S
    low_can = Fraction(N * (H + 1) * (d - 1), 2)
    initial_low_product = low_can * low_can / (Fraction(2**100)**2)
    need(initial_low_product > T, 'low-part conservative comparison changed')
    return {'schema': 'relin2-public-bounds-v1', 'status': 'PASS', 'source_hash': SRC_SHA,
            'source_constants_match': source_match, 'arithmetic': 'integer/Fraction only',
            'N': N, 'h': H, 'd': str(d), 'P': str(P), 'T': quantity(T),
            'sampler_support_envelope': {'sigma_1': 13, 'sigma_nominal_3_19f': 39,
                                         'envelope_used': EMAX, 'sampler_executed': False},
            'root_checks': {'pairs_checked': len(all_pairs), 'modular_power_only': True,
                            'FFT_or_NTT_executed': False, 'primality_reproved': False},
            'families': rows,
            'initial_DCP_low_product_bound_over_T': quantity(initial_low_product / T),
            'scope_limit': 'Local Relin and RS bounds; not an eight-step total precision or nonwrap certificate.'}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input-root', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--table', type=Path)
    args = parser.parse_args()
    result = calculate(args.input_root)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    lines = ['family\tactive_Q_towers\tB2_coefficient_bound\tlocal_Relin_bound\tlocal_Relin_over_T\tlocal_RS_over_T\tunit_tensor_scale_over_Qhalf']
    for row in result['families']:
        lines.append('\t'.join([str(row['family']), str(row['active_towers']),
            row['B2_coefficient']['display_truncated'],
            row['normalized_local_relin_bound']['display_truncated'],
            row['normalized_local_relin_bound_over_T']['display_truncated'],
            row['normalized_local_RS_bound_over_T']['display_truncated'],
            row['unit_amplitude_tensor_scale_over_Qhalf']['display_truncated']]))
    text = '\n'.join(lines) + '\n'
    if args.table:
        args.table.write_text(text, encoding='utf-8')
    print('PASS: 8 exact S100 family bounds; 12 modular root-order checks; no transforms.')
    print(text, end='')
    print('Initial DCP low-product conservative bound / T =', result['initial_DCP_low_product_bound_over_T']['display_truncated'])


if __name__ == '__main__':
    main()
