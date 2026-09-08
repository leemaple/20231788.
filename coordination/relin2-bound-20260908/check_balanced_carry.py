"""Public symbolic h128 carry witness with 64 plus / 64 minus signs.

No sampling, key generation, FHE, transforms or historical secret material.
Checks 256 coefficient classes of one formal polynomial, not 256 experiments.
"""
import hashlib
import json
from math import prod
from pathlib import Path
import re


def main():
    here = Path(__file__).resolve().parent
    source = (here.parents[1] / 'src/repeated_mult2.cpp').read_bytes()
    assert hashlib.sha256(source).hexdigest() == '6781d01ff9b4012e7c6d14d16f482def0821c54f4e81e2de80bd8db9d1aea41b'
    block = re.search(r'kPaperQ\{\{(.*?)\}\};', source.decode(), re.S).group(1)
    full_primes = [int(q) for q, _ in re.findall(r'\{(\d+)ULL,(\d+)ULL\}', block)]
    p = 1152921504606584833
    n, h = 32768, 128
    # Negative fixture: Pro M13's all-plus formal secret fails this source gate.
    assert not (h // 2 - 1 <= h <= h // 2 + 1)
    signs = [1] * 64 + [-1] * 64
    assert sum(v == 1 for v in signs) == sum(v == -1 for v in signs) == 64
    assert 63 <= sum(v == 1 for v in signs) <= 65
    assert sum(abs(v) for v in signs) == h and sum(signs) == 0
    q, f = prod(full_primes[:-1]), prod(full_primes)
    selectors = [(f // qi) * pow(f // qi, -1, qi) for qi in full_primes]
    assert len(full_primes) == 11 and all(qi > 2 for qi in full_primes)
    a = (p + 1) // 2
    rows = []
    for k in range(256):
        sj = 2 * sum(signs[:min(k+1, h)]) - sum(signs)
        s2 = sum(signs[i] * signs[k-i] for i in range(h) if 0 <= k-i < h)
        z0 = 0
        for j, theta in enumerate(selectors):
            as_coefficient = a * sj if j == 0 else 0
            b_full = (p * theta * s2 - as_coefficient) % (f * p)
            assert (b_full + as_coefficient - p * theta * s2) % (f * p) == 0
            if j < len(full_primes) - 1:
                z0 += b_full % (q * p)
        z1 = a
        delta0 = (2*z0) // p - 2 * (z0 // p)
        delta1 = (2*z1) // p - 2 * (z1 // p)
        assert delta0 == int(k < 127) and delta1 == 1
        rows.append({'coefficient_class': k, 'delta0': delta0, 'delta1': delta1,
                     's_times_J': sj, 'decoded_carry': delta0 + sj})
    peak = max(abs(row['decoded_carry']) for row in rows)
    assert peak == 129 and rows[63]['decoded_carry'] == 129
    assert rows[-1] == {'coefficient_class': 255, 'delta0': 0, 'delta1': 1, 's_times_J': 0, 'decoded_carry': 0}
    result = {
        'status': 'PASS', 'scope': 'One public symbolic coefficient-class witness; not a generated key or production FHE RED.',
        'N': n, 'h': h, 'positive_count': 64, 'negative_count': 64,
        'pro_all_positive_fixture_rejected_by_actual_sign_count_constraint': True,
        'balanced_fixture_meets_sign_count_and_hamming_constraints': True,
        'all_digit_carries_zero_for_third_inputs_1_and_1': True,
        'coefficient_classes': len(rows), 'tail': 'For every k>=255: s²=0, sJ=0, delta0=0, delta1=1.',
        'decoded_carry_coefficient_norm': peak, 'overstrong_h_only_bound_rejected': peak > h,
        'script_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        'disclaimer': 'Meeting deterministic sign constraints is not a claim that this exact key is sampled by the PRNG, nor a distribution/probability or historical-run assertion.',
        'rows': rows,
    }
    with (here / 'BALANCED_CARRY_CHECK.json').open('x') as output:
        json.dump(result, output, indent=2)
        output.write('\n')
    print(json.dumps({key: value for key, value in result.items() if key != 'rows'}, indent=2))


if __name__ == '__main__':
    main()
