"""A few deterministic scalar model checks; never imports project or OpenFHE."""
from fractions import Fraction
import hashlib
import json
from pathlib import Path


def centered(x, modulus):
    return (x + modulus // 2) % modulus - modulus // 2


def main():
    results = {}
    q, d, s = 101, 13, 2
    high = (7, 3, 5)
    low = (2, -4, 3)
    err_hi, err_lo = (2, -1), (-1, 2)
    # Deliberately artificial scalar relinearizers, not HYBRID or ciphertexts.
    rh = (d * (high[0] + high[2] * s*s) + err_hi[0], d * high[1] + err_hi[1])
    rl = (low[0] + low[2] * s*s + err_lo[0], low[1] + err_lo[1])
    v = tuple(centered(x, d) for x in rh)
    u = tuple((x-r)//d for x,r in zip(rh,v))
    assert all((d*a+b-c) % q == 0 for a,b,c in zip(u,v,rh))
    out = tuple(d*a+b+c for a,b,c in zip(u,v,rl))
    target = d * sum(x*s**i for i,x in enumerate(high)) + sum(x*s**i for i,x in enumerate(low))
    error = err_hi[0]+s*err_hi[1]+err_lo[0]+s*err_lo[1]
    assert (out[0]+s*out[1]-target-error) % q == 0
    # Dropping the first low error coordinate must not pass the same identity.
    wrong_error = error-err_lo[0]
    assert (out[0]+s*out[1]-target-wrong_error) % q != 0
    results['direct_recombination'] = {'model_only': True, 'expected_phase_error_mod_q': error % q, 'missing_first_coordinate_rejected': True}

    # Odd denominator has no half-integer ties for these integer numerators.
    def rounding_pair(x):
        return tuple((2*x*k+7)//14 for k in (3,4))
    f1, f2 = rounding_pair(1), rounding_pair(2)
    carry = tuple(y-2*x for x,y in zip(f1,f2))
    assert carry == (1,-1) and carry[0] != 0
    results['nonadditivity'] = {'model_only': True, 'F(2)-2F(1)': carry, 'second_coordinate_only_false': True}

    expected, delta = 50, 3
    observed = centered(expected+delta,q)
    assert observed == -48 and observed-expected == delta-q
    results['wrap_distinction'] = {'model_only': True, 'modulus': q, 'target': expected, 'perturbation': delta, 'observed_center': observed, 'actual_difference': observed-expected}

    # Public upper bounds only; conditional formula from the root draft.
    n, h, e, digits = 32768, 128, 39, 11
    p = 1152921504606584833
    max_q_minus_one = 2**60-1
    coefficient_bound = Fraction(n*e*digits*max_q_minus_one+(1+h)*(p-1),p)
    embedding_bound = n*coefficient_bound
    results['conditional_upper_bound'] = {'assumptions_not_certified': True, 'E': e, 'digits_upper': digits, 'max_q_minus_one_upper': max_q_minus_one,
        'coefficient_bound_numerator': coefficient_bound.numerator, 'coefficient_bound_denominator': coefficient_bound.denominator,
        'embedding_bound_numerator': embedding_bound.numerator, 'embedding_bound_denominator': embedding_bound.denominator}
    results['scope'] = 'Scalar identities and evaluation of a conditional formula only. No FHE, NTT, sampler, historical endpoint replay or precision PASS.'
    results['script_sha256'] = hashlib.sha256(Path(__file__).read_bytes()).hexdigest()
    path = Path(__file__).with_name('ROOT_ALGEBRA_CHECK.json')
    with path.open('x') as handle:
        json.dump(results, handle, indent=2)
        handle.write('\n')
    print(json.dumps(results, indent=2))


if __name__ == '__main__':
    main()
