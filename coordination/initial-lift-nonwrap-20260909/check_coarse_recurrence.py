"""One conditional C25 bound calculation; no FHE, sampling or transforms.

This probes the strength of a sufficient bound, not a historical ciphertext.
Even the optimistic initial fresh coefficient bound M0<=2^100 is a hypothesis.
"""
import hashlib
import json
from fractions import Fraction as F
from math import prod
from pathlib import Path
import re


def ceil(x):
    return -(-x.numerator // x.denominator)


def main():
    here = Path(__file__).resolve().parent
    source = (here.parents[1] / 'src/repeated_mult2.cpp').read_bytes()
    assert hashlib.sha256(source).hexdigest() == '6781d01ff9b4012e7c6d14d16f482def0821c54f4e81e2de80bd8db9d1aea41b'
    block = source.decode().split('kPaperQ{{', 1)[1].split('}};', 1)[0]
    qs = [int(a) for a, _ in re.findall(r'\{(\d+)ULL,(\d+)ULL\}', block)]
    p = int(re.search(r'kPaperP\{(\d+)ULL', source.decode()).group(1))
    assert len(qs) == 11
    n, h, d = 32768, 128, qs[-1]
    rd = F((1+h)*(d-1), 2)
    initial_m = 2**100
    high, low = ceil((initial_m+rd)/d), ceil(rd)
    initial_gate = d*high+low < F(prod(qs[:-1]), 2)
    rows = []
    for k in range(8):
        active = qs[:10-k]
        q, m = prod(active), active[-1]
        bk = F(39*n*sum(v-1 for v in active)+(1+h)*(p-1), p)
        rm = F((1+h)*(m-1), 2*m)
        tensor_bound = n*(d*high*high+2*high*low)
        nw_lhs = tensor_bound + 2*bk + m*rm
        raw_gate = nw_lhs < F(q, 2)
        high_next = ceil(F(n*high*high, m)+(bk+rd)/(d*m)+rm)
        low_next = ceil((2*n*high*low+bk+rd)/m+(1+d)*rm)
        rows.append({'round':k+1,'Q_bits':q.bit_length(),
            'high_bound_bits':high.bit_length(),'low_bound_bits':low.bit_length(),
            'tensor_bound_bits':tensor_bound.bit_length(),
            'raw_sufficient_NW_comparison':raw_gate,
            'sufficient_chain_certified_under_initial_hypothesis':initial_gate and all(r['raw_sufficient_NW_comparison'] for r in rows) and raw_gate,
            'high_bound':str(high),'low_bound':str(low),
            'next_high_bound':str(high_next),'next_low_bound':str(low_next)})
        high, low = high_next, low_next
    assert initial_gate
    assert [r['raw_sufficient_NW_comparison'] for r in rows] == [True]*4+[False]*4
    result = {'schema':'conditional-coarse-c25-probe-v1','execution_status':'PASS',
        'source_sha256':hashlib.sha256(source).hexdigest(),
        'script_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        'initial_hypothesis':'fresh centered phase coefficient norm M0 <= 2^100; not established for an actual sample here',
        'initial_DCP_representative_interface':'H0 <= (M0+Rd)/d, L0<=Rd with compatible initial lifts',
        'arithmetic':'Fraction bounds rounded upward to integer H/L after each step',
        'first_uncertified_round':5,
        'conclusion':'This sufficient coefficient bound is already inconclusive at round5 under the stated optimistic initial hypothesis. It does not prove a wrap, numerical FAIL, or production defect.',
        'rows':rows}
    with (here/'ROOT_COARSE_RECURRENCE.json').open('x') as out:
        json.dump(result,out,indent=2)
        out.write('\n')
    print(json.dumps({k:v for k,v in result.items() if k!='rows'},indent=2))
    for row in rows:
        print({k:row[k] for k in ('round','Q_bits','high_bound_bits','low_bound_bits','tensor_bound_bits','raw_sufficient_NW_comparison')})


if __name__ == '__main__':
    main()
