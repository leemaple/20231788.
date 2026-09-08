"""Independent, conditional canonical-bound draft; no transforms or crypto.

The induction explicitly tests coefficient nonwrap before identifying each
bounded integer representative with a centered phase. It never assumes that
coefficient centering contracts canonical norm.
"""
import hashlib
import json
from fractions import Fraction as F
from math import prod
from pathlib import Path
import re


def ceil(x):
    return -(-x.numerator//x.denominator)


def main():
    here=Path(__file__).resolve().parent
    root=here.parents[1]
    source=(root/'src/repeated_mult2.cpp').read_bytes()
    oracle=(root/'tests/paper_full_eight_square_oracle.h').read_bytes()
    assert hashlib.sha256(source).hexdigest()=='6781d01ff9b4012e7c6d14d16f482def0821c54f4e81e2de80bd8db9d1aea41b'
    assert hashlib.sha256(oracle).hexdigest()=='08d6c4dfe6761b84d893d90c120418a29d8a2b96aa05304e95728d72e4c3e203'
    block=source.decode().split('kPaperQ{{',1)[1].split('}};',1)[0]
    qs=[int(a) for a,_ in re.findall(r'\{(\d+)ULL,(\d+)ULL\}',block)]
    p=int(re.search(r'kPaperP\{(\d+)ULL',source.decode()).group(1))
    n,h,d,s0=32768,128,qs[-1],2**100
    assert len(qs)==11
    # Actual frozen formula's public radius; no message substitution or slots FFT.
    a_max=F(1015,1024)+F(16383,2**75)
    b_max=F(1,128)
    assert a_max*a_max+b_max*b_max < F(127,128)**2
    b_fresh=39*(n+h+1)
    encoding_error_hypothesis=F(1,512)
    radius=F(255,256)
    assert F(127,128)+encoding_error_hypothesis+F(n*b_fresh,s0)<radius
    rd=F((1+h)*(d-1),2)
    high=ceil((s0*radius+n*rd)/d)
    low=ceil(n*rd)
    assert 2*(d*high+low)<prod(qs[:-1])
    rows=[]
    for k in range(8):
        active=qs[:10-k]
        q,m=prod(active),active[-1]
        bk=F(39*n*sum(v-1 for v in active)+(1+h)*(p-1),p)
        rm=F((1+h)*(m-1),2*m)
        tensor=d*high*high+2*high*low
        nw_lhs=tensor+2*n*bk+m*n*rm
        high_next=ceil(F(high*high,m)+(n*bk+n*rd)/(d*m)+n*rm)
        low_next=ceil((2*high*low+n*bk+n*rd)/m+(1+d)*n*rm)
        next_q=q//m
        margins={
            'input_members':q-2*max(high,low),
            'input_recombined':q-2*(d*high+low),
            'recombined_NW':F(q)-2*nw_lhs,
            'output_members':next_q-2*max(high_next,low_next),
            'output_recombined':next_q-2*(d*high_next+low_next),
        }
        assert all(v>0 for v in margins.values()), (k+1,margins)
        ratio=F(2*(d*high_next+low_next),next_q)
        # Human display is an upward rational bound; exact comparison is above.
        rows.append({'round':k+1,'Q_bits':q.bit_length(),
            'high_can_bound':str(high),'low_can_bound':str(low),
            'next_high_can_bound':str(high_next),'next_low_can_bound':str(low_next),
            'all_five_margin_tests_positive':True,
            'output_recombined_over_halfQ_upper_millionths':ceil(ratio*10**6)})
        high,low=high_next,low_next
    assert F(2*(d*high+low),prod(qs[:2]))<F(3,4)
    # Exact N=4, Q=5 counterexample to canonical contraction by centering.
    # For a+bX+cX²+dX³ at primitive eighth roots,
    # max |f|² = A+sqrt(2)*|B|, A=sum(coeff²), B=a(b-d)+c(b+d).
    def ab(v):
        a,b,c,dv=v
        return sum(x*x for x in v),a*(b-dv)+c*(b+dv)
    original=[3,2,-1,1]
    centered=[(x+2)%5-2 for x in original]
    assert ab(original)==(15,0) and ab(centered)==(10,-5)
    assert 2*5**2>(15-10)**2  # 10+5sqrt(2)>15, no floating sqrt.
    result={'schema':'conditional-canonical-nonwrap-draft-v1','execution_status':'PASS',
        'adoption_status':'DRAFT_PENDING_INDEPENDENT_EIGHT_STEP_REVIEW',
        'script_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        'source_sha256':hashlib.sha256(source).hexdigest(),'oracle_sha256':hashlib.sha256(oracle).hexdigest(),
        'assumptions':['adopted exact ring, honest key, coherent finite support and Relin2/DCP/RS source contracts',
            'actual integer fresh phase can-norm <= (255/256)*2^100; sufficient from E_enc<=1/512 plus frozen input radius and worst-case PKE support'],
        'E_enc_bound_verified_here':False,'historical_phase_bound_verified_here':False,
        'all_eight_sufficient_induction_checks_pass':True,
        'centering_canonical_contraction_counterexample':{'original':original,'centered':centered,'original_max_norm_squared':'15','centered_max_norm_squared':'10+5*sqrt(2)'},
        'rows':rows,
        'scope':'Pure integer/Fraction model. Not a production test, historical wrap observation, E80 certificate, encoding-error proof or changed-input experiment.'}
    with (here/'ROOT_CANONICAL_MARGIN_DRAFT.json').open('x') as out:
        json.dump(result,out,indent=2)
        out.write('\n')
    print(json.dumps({k:v for k,v in result.items() if k!='rows'},indent=2))
    print([(r['round'],r['output_recombined_over_halfQ_upper_millionths']) for r in rows])


if __name__=='__main__':
    main()
