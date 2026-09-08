"""Finite arithmetic for the explicitly conditional initial range certificate.

No Boost/FFT/NTT execution, encryption, sampler or historical sample read.
The assumed scalar contract is stated in the output, not measured by this check.
"""
import hashlib
import json
from fractions import Fraction as F
from math import prod
from pathlib import Path
import re


def main():
    here = Path(__file__).resolve().parent
    source = (here.parents[1]/'src/repeated_mult2.cpp').read_bytes()
    assert hashlib.sha256(source).hexdigest() == '6781d01ff9b4012e7c6d14d16f482def0821c54f4e81e2de80bd8db9d1aea41b'
    block = source.decode().split('kPaperQ{{',1)[1].split('}};',1)[0]
    qs = [int(a) for a,_ in re.findall(r'\{(\d+)ULL,(\d+)ULL\}',block)]
    n,h,d = 32768,128,qs[-1]
    q,f = prod(qs[:-1]),prod(qs)
    assert len(qs)==11 and all(v>2**49 for v in qs[:-1])
    eps = F(1,10**159)
    threshold = F(1,2**410)
    # For 410 relative errors <=eps, binomial coefficients are <=410^j,
    # so (1+eps)^410 <= 1/(1-410eps). One multiplication may round down.
    assert 410*eps < 1
    round_factor = 1/((1-410*eps)*(1-eps))
    assert round_factor < 4
    scalar_cap = 4*threshold/eps + F(1,2)
    m_bound = 2**123
    assert scalar_cap < m_bound
    b_fresh = 39*(n+h+1)
    rd = (1+h)*(d-1)//2
    assert b_fresh==1282983 and b_fresh<2**21 and rd<2**47
    assert 2*(m_bound+b_fresh+rd)<2**125<q<f
    margins = {
        'full_fresh_phase':f-2*(m_bound+b_fresh),
        'high_DCP_member':f-2*(m_bound+b_fresh+rd),
        'recombined_first_pair':q-2*(m_bound+b_fresh),
        'low_DCP_member':q-2*rd,
    }
    assert all(v>0 for v in margins.values())
    # Negative fixture: a full-F centered guard alone allows post-DCP wrap.
    counter_m = (q+1)//2
    assert 2*counter_m<f and not (2*counter_m<q)
    assert counter_m*eps > threshold  # StableRound's extra gate rejects it.
    # Type distinction: do not replace the internal160-digit epsilon by100.
    assert 2**100*eps < threshold < F(2**100,10**99)
    result = {
        'schema':'conditional-initial-margin-check-v1','execution_status':'PASS',
        'source_sha256':hashlib.sha256(source).hexdigest(),
        'script_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        'assumptions':['Primary epsilon=10^-159; normal scalar multiplication/division relative error <=epsilon; finite comparison and nearest rounding semantics',
            'honest frozen successful public-key/PKE consumers with coherent Gaussian support39 and ternary v',
            'adopted exact ring and DCP primitive semantics'],
        'not_executed':['Boost','encoding','FFT','NTT','sampling','encryption','historical ciphertext inspection'],
        'M_enc_upper_bound':str(m_bound),'B_fresh':b_fresh,'R_d':str(rd),
        'Q':str(q),'F':str(f),
        'margins':{k:{'positive':v>0,'numerator':str(v),'bit_length':v.bit_length()} for k,v in margins.items()},
        'full_F_guard_only_counterexample_rejected':True,
        'Primary160_vs_Client100_distinction_checked':True,
        'conclusion':'All four initial coefficient margins follow under the stated contracts; no ideal inverse-transform accuracy, eight-step nonwrap or E80 result is established.',
    }
    with (here/'ROOT_INITIAL_MARGINS.json').open('x') as out:
        json.dump(result,out,indent=2)
        out.write('\n')
    print(json.dumps({k:v for k,v in result.items() if k not in ('margins','Q','F')},indent=2))
    print({k:v['bit_length'] for k,v in result['margins'].items()})


if __name__=='__main__':
    main()
