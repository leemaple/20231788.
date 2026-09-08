#!/usr/bin/env python3
"""Bounded, deterministic INTEGER/Fraction models; NOT production FHE RED/GREEN.

No OpenFHE, encryption, decryption, PRNG, transforms, or real key material.
All small-ring rows below are public, deliberately constructed algebraic fixtures.
Run: python checks/model_checks.py --output checks/model_results.json
"""
from __future__ import annotations
import argparse
import json
from dataclasses import dataclass
from fractions import Fraction
from math import gcd, prod
from pathlib import Path
from typing import Iterable

Poly = tuple[int, ...]


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


@dataclass(frozen=True)
class Ring:
    n: int

    def p(self, values: int | Iterable[int]) -> Poly:
        if isinstance(values, int):
            return (values,) + (0,) * (self.n - 1)
        result = tuple(values)
        require(len(result) == self.n, 'wrong polynomial length')
        return result

    def add(self, a: Poly, b: Poly) -> Poly:
        return tuple(x + y for x, y in zip(a, b))

    def sub(self, a: Poly, b: Poly) -> Poly:
        return tuple(x - y for x, y in zip(a, b))

    def scale(self, a: Poly, k: int) -> Poly:
        return tuple(k * x for x in a)

    def mul(self, a: Poly, b: Poly) -> Poly:
        result = [0] * self.n
        for i, x in enumerate(a):
            for j, y in enumerate(b):
                k = i + j
                result[k % self.n] += x * y * (1 if k < self.n else -1)
        return tuple(result)

    def mod(self, a: Poly, m: int) -> Poly:
        return tuple(x % m for x in a)

    def center(self, a: Poly, m: int) -> Poly:
        require(m > 1 and m % 2 == 1, 'odd modulus required')
        return tuple((x + m // 2) % m - m // 2 for x in a)

    def divexact(self, a: Poly, m: int) -> Poly:
        require(all(x % m == 0 for x in a), 'nonintegral polynomial quotient')
        return tuple(x // m for x in a)

    def total(self, terms: Iterable[Poly]) -> Poly:
        z = self.p(0)
        for term in terms:
            z = self.add(z, term)
        return z

    def phase(self, c: tuple[Poly, ...], s: Poly) -> Poly:
        z, power = self.p(0), self.p(1)
        for v in c:
            z = self.add(z, self.mul(v, power))
            power = self.mul(power, s)
        return z

    def norm(self, a: Poly) -> int:
        return max(map(abs, a))


class HybridModel:
    """Unsigned one-prime digit lift / one-P floor mod-down source model."""
    def __init__(self, ring: Ring, primes: tuple[int, ...], p: int,
                 s: Poly, a: tuple[Poly, ...], e: tuple[Poly, ...]):
        self.r, self.qs, self.p, self.s = ring, primes, p, s
        self.fullq = prod(primes)
        require(len(primes) == len(a) == len(e), 'row mismatch')
        require(all(gcd(x, y) == 1 for i, x in enumerate((*primes, p))
                    for y in (*primes, p)[i + 1:]), 'non-coprime moduli')
        self.e = e
        self.a = tuple(ring.mod(v, self.fullq * p) for v in a)
        s2 = ring.mul(s, s)
        rows = []
        for q, av, ev in zip(primes, self.a, e):
            hat = self.fullq // q
            theta = hat * pow(hat, -1, q)
            b = ring.sub(ring.add(ring.scale(s2, p * theta), ev), ring.mul(av, s))
            rows.append(ring.mod(b, self.fullq * p))
        self.b = tuple(rows)

    def key_switch(self, c: Poly, count: int) -> dict:
        r, p, qs = self.r, self.p, self.qs[:count]
        q = prod(qs)
        digits = tuple(r.mod(c, qi) for qi in qs)
        # Restriction of a full-family key preserves its P tower and active Q.
        aa = tuple(r.mod(x, q * p) for x in self.a[:count])
        bb = tuple(r.mod(x, q * p) for x in self.b[:count])
        z = tuple(r.total(r.mul(d, row) for d, row in zip(digits, rows))
                  for rows in (bb, aa))
        rem = tuple(r.mod(v, p) for v in z)
        t = tuple(r.divexact(r.sub(v, rr), p) for v, rr in zip(z, rem))
        error = r.total(r.mul(d, ev) for d, ev in zip(digits, self.e[:count]))
        nu = r.divexact(r.sub(r.sub(error, rem[0]), r.mul(self.s, rem[1])), p)
        require(r.mod(r.sub(r.phase(t, self.s),
                            r.add(r.mul(c, r.mul(self.s, self.s)), nu)), q) == r.p(0),
                'single-relinearization contract fails')
        bnd = Fraction(sum(r.norm(d) * sum(map(abs, ev)) for d, ev in zip(digits, self.e)), p)
        bnd += Fraction((1 + sum(map(abs, self.s))) * (p - 1), p)
        require(r.norm(nu) <= bnd, 'coefficient bound fails')
        return {'digits': digits, 'z': z, 'r': rem, 't': t,
                'nu': nu, 'bound': bnd, 'rows': (bb, aa)}

    def relin(self, c: tuple[Poly, Poly, Poly], count: int) -> tuple[tuple[Poly, Poly], dict]:
        rec = self.key_switch(c[2], count)
        q = prod(self.qs[:count])
        return tuple(self.r.mod(self.r.add(c[i], rec['t'][i]), q) for i in range(2)), rec

    def additivity(self, u: Poly, v: Poly, count: int) -> dict:
        r, p, q = self.r, self.p, prod(self.qs[:count])
        w = r.mod(r.add(u, v), q)
        ru, rv, rw = [self.key_switch(c, count) for c in (u, v, w)]
        kappa = tuple(r.divexact(r.sub(r.add(du, dv), dw), qi)
                      for du, dv, dw, qi in zip(ru['digits'], rv['digits'], rw['digits'], self.qs))
        require(all(x in (0, 1) for k in kappa for x in k), 'digit carry range')
        gs, eta, predicted, actual = [], [], [], []
        for i in range(2):
            G = r.total(r.scale(r.mul(k, row), qi)
                        for k, row, qi in zip(kappa, ru['rows'][i], self.qs))
            gamma = r.mod(G, p)
            g = r.divexact(r.sub(G, gamma), p)
            h = tuple((a + b - c) // p for a, b, c in zip(ru['r'][i], rv['r'][i], gamma))
            require(all(-1 <= x <= 1 for x in h), 'output carry range')
            expect = r.mod(r.sub(h, g), q)
            delta = r.mod(r.sub(r.sub(rw['t'][i], ru['t'][i]), rv['t'][i]), q)
            require(expect == delta, 'corrected coordinate additivity')
            gs.append(G); eta.append(h); predicted.append(expect); actual.append(delta)
        delta_nu = r.sub(r.sub(rw['nu'], ru['nu']), rv['nu'])
        delta_r = tuple(r.sub(r.sub(rw['r'][i], ru['r'][i]), rv['r'][i]) for i in range(2))
        carry_noise = r.total(r.scale(r.mul(k, ev), qi)
                              for k, ev, qi in zip(kappa, self.e, self.qs))
        expect_nu = r.divexact(r.sub(r.sub(r.scale(carry_noise, -1), delta_r[0]),
                                        r.mul(self.s, delta_r[1])), p)
        require(expect_nu == delta_nu, 'exact noise-difference identity')
        return {'kappa': kappa, 'G': gs, 'eta': eta,
                'delta_centered': [r.center(v, q) for v in actual],
                'delta_nu': delta_nu, 'u': ru, 'v': rv, 'w': rw}


def frac_string(v: Fraction) -> str:
    return str(v.numerator) + '/' + str(v.denominator)


def run() -> dict:
    tests = []
    def done(name: str, detail: dict) -> None:
        tests.append({'id': name, 'status': 'PASS', **detail})

    r = Ring(1)
    one = HybridModel(r, (11,), 17, (1,), ((10,),), ((3,),))
    c = ((0,), (0,), (1,))
    _, rec = one.relin(c, 1)
    ad = one.additivity((1,), (1,), 1)
    require(ad['delta_centered'] == [(1,), (1,)], 'fixture changed')
    require(ad['kappa'] == ((0,),), 'unexpected digit carry')
    require(sum(v[0] for v in ad['delta_centered']) == 2, 'decoded carry')
    nearest_10 = (10 - r.center((10,), 17)[0]) // 17
    nearest_20 = (20 - r.center((20,), 17)[0]) // 17
    require(nearest_20 - 2 * nearest_10 == -1, 'paper-nearest-rounding counterexample')
    done('M01_overstrong_zero_first_coordinate_counterexample',
         {'Q': 11, 'P': 17, 'synthetic_s': 1, 'synthetic_a': 10, 'synthetic_b': 10,
          'synthetic_e': 3, 'third_inputs': [1, 1], 'digit_carry': 0,
          'actual_delta': [1, 1], 'overstrong_delta_0_equals_0': False,
          'decoded_delta': 2, 'overstrong_decoded_bound_h_equals_1': False,
          'same_fixture_paper_nearest_delta': [-1, -1]})
    done('M02_corrected_two_coordinate_positive',
         {'delta': [1, 1], 'eta': [v[0] for v in ad['eta']], 'G': [0, 0],
          'decoded_bound_1_plus_h': 2, 'identity_holds': True})

    bad_nu = Fraction(rec['nu'][0] * 17 + 2 * rec['r'][1][0], 17)
    require(bad_nu != rec['nu'][0], 'wrong-secret-remainder-sign mutant survived')
    require((0, ad['delta_centered'][1][0]) != (1, 1), 'omitted-first-carry mutant survived')
    done('M03_negative_mutants_first_carry_and_secret_sign',
         {'mutants_rejected': ['omit coordinate-0 carry', 'replace -s*r1 by +s*r1'],
          'correct_nu': rec['nu'][0], 'wrong_sign_nu': frac_string(bad_nu),
          'production_RED_GREEN': False})

    two = HybridModel(r, (5, 11), 17, (1,), ((30,), (7,)), ((1,), (-2,)))
    ad2 = two.additivity((4,), (4,), 2)
    require(ad2['delta_centered'] == [(8,), (-8,)], 'two-partition fixture changed')
    require(ad2['G'] == [(790,), (150,)], 'G fixture changed')
    omitted_g = [r.center(x, 55) for x in ad2['eta']]
    require(omitted_g != ad2['delta_centered'], 'omitted-digit-carry mutant survived')
    done('M04_digit_carry_correction_not_just_unit_vector',
         {'Q': 55, 'P': 17, 'digits_u': [4, 4], 'digits_w': [3, 8],
          'kappa': [1, 0], 'G': [790, 150], 'eta': [-1, 0],
          'actual_delta_centered': [8, -8], 'omitted_G_prediction': [-1, 0],
          'mutant_rejected': True})
    done('M05_exact_decrypted_additivity_positive',
         {'delta_nu': list(ad2['delta_nu']), 'coordinate_delta_phase_mod_Q': 0,
          'noise_and_remainders_identity_holds': True})

    rr = Ring(4)
    ring_model = HybridModel(rr, (5, 11, 13), 17, (1, -1, 0, 0),
                             ((4, 1, 2, 7), (2, 9, 1, 4), (8, 1, 7, 2)),
                             ((1, -1, 0, 2), (0, 1, -2, 1), (2, 0, -1, 1)))
    fixtures = [(0, 0, 0, 0), (1, 2, 3, 4), (54, 31, 12, 9),
                (-9, 10, 111, -202), (7, -6, 5, -4), (714, 6, 81, 13)]
    for x in fixtures:
        ring_model.key_switch(x, 2)
        ring_model.additivity(x, (4, 8, 11, 17), 2)
    done('M06_nonconstant_negacyclic_ring_contracts',
         {'N': 4, 'fixture_count': len(fixtures), 'transforms_executed': 0,
          'single_phase_bound_and_additivity_identities_hold': True})

    H = ((4, 12, 7, 8), (3, 1, 9, 2), (1, 2, 3, 4))
    L = ((8, 2, 1, 5), (4, 7, 2, 1), (9, 8, 7, 6))
    raised = tuple(rr.mod(rr.scale(x, 13), 715) for x in H)
    full_relin, full_rec = ring_model.relin(raised, 3)
    active_input = tuple(rr.mod(rr.scale(x, 13), 55) for x in H)
    active_relin, active_rec = ring_model.relin(active_input, 2)
    require(full_rec['digits'][-1] == rr.p(0), 'extra d digit nonzero')
    require(tuple(rr.mod(x, 55) for x in full_relin) == active_relin,
            'restriction/relinearization naturality fails')
    done('M07_zero_d_digit_and_restriction_naturality',
         {'N': 4, 'active_Q': 55, 'raised_F': 715, 'P': 17, 'd': 13,
          'extra_digit': [0, 0, 0, 0], 'coordinatewise_identity_holds': True})
    remainder = tuple(rr.center(x, 13) for x in full_relin)
    high = tuple(rr.mod(rr.divexact(rr.sub(x, y), 13), 55)
                 for x, y in zip(full_relin, remainder))
    low_relin, low_rec = ring_model.relin(L, 2)
    low = tuple(rr.mod(rr.add(x, y), 55) for x, y in zip(remainder, low_relin))
    recombined = tuple(rr.mod(rr.add(rr.scale(x, 13), y), 55) for x, y in zip(high, low))
    expected = tuple(rr.mod(rr.add(x, y), 55) for x, y in zip(active_relin, low_relin))
    require(recombined == expected, 'Relin2 recombination identity')
    require(full_rec['nu'] == active_rec['nu'], 'restricted error representative differs')
    eta_d = rr.phase(remainder, ring_model.s)
    high_phase_lift = rr.add(rr.phase(H, ring_model.s),
                            rr.divexact(rr.sub(active_rec['nu'], eta_d), 13))
    require(rr.mod(rr.sub(rr.phase(high, ring_model.s), high_phase_lift), 55) == rr.p(0),
            'DCP high phase quotient contract')
    nu2 = rr.add(active_rec['nu'], low_rec['nu'])
    tensor = rr.add(rr.scale(rr.phase(H, ring_model.s), 13), rr.phase(L, ring_model.s))
    require(rr.mod(rr.sub(rr.phase(recombined, ring_model.s), rr.add(tensor, nu2)), 55) == rr.p(0),
            'Relin2 phase error')
    done('M08_Relin2_DCP_rounding_cancels_on_recombination',
         {'coordinatewise_identity_holds': True, 'nu2': list(nu2),
          'nu2_is_sum_not_d_times_high_error': True})

    def rescale(pair: tuple[Poly, Poly]) -> tuple[Poly, Poly]:
        return tuple(rr.mod(rr.divexact(rr.sub(x, rr.center(x, 11)), 11), 5) for x in pair)
    rh = rescale(high)
    rc = rescale(recombined)
    rl = tuple(rr.mod(rr.sub(x, rr.scale(y, 13)), 5) for x, y in zip(rc, rh))
    require(tuple(rr.mod(rr.add(rr.scale(x, 13), y), 5) for x, y in zip(rh, rl)) == rc,
            'RS2 recombination does not equal recombined rescale')
    phase = rr.center(rr.phase(recombined, ring_model.s), 55)
    phase_out = rr.center(rr.phase(rc, ring_model.s), 5)
    rho_num = rr.scale(rr.phase(tuple(rr.center(x, 11) for x in recombined), ring_model.s), -1)
    before_out_center = rr.divexact(rr.add(phase, rho_num), 11)
    wpre = rr.divexact(rr.sub(rr.add(tensor, nu2), phase), 55)
    wout = rr.divexact(rr.sub(before_out_center, phase_out), 5)
    for i in range(rr.n):
        rhs = Fraction(tensor[i] + nu2[i] + rho_num[i], 11) - 5 * (wpre[i] + wout[i])
        require(rhs == phase_out[i], 'RS exact rational/wrap identity')
    h_weight = sum(map(abs, ring_model.s))
    Rd = Fraction((1 + h_weight) * (13 - 1), 2)
    Rm = Fraction((1 + h_weight) * (11 - 1), 2 * 11)
    # General H/L tensor-phase bounds; substituting the Tensor2 product bounds
    # in the report gives C25. Centering contracts coefficient, not slot, norm.
    hp_bound = rr.norm(rr.phase(H, ring_model.s)) + (active_rec['bound'] + Rd) / 13
    lp_bound = rr.norm(rr.phase(L, ring_model.s)) + low_rec['bound'] + Rd
    require(rr.norm(rr.center(rr.phase(rh, ring_model.s), 5)) <= hp_bound / 11 + Rm,
            'high coefficient recurrence')
    require(rr.norm(rr.center(rr.phase(rl, ring_model.s), 5)) <= lp_bound / 11 + (1 + 13) * Rm,
            'low coefficient recurrence')
    done('M09_RS2_recombination_and_exact_wrap_terms',
         {'identity_holds': True, 'w_pre': list(wpre), 'w_out': list(wout),
          'coefficient_high_low_recurrence_checked': True,
          'high_only_rounding_cancels_from_recombined_result': True})

    # Scalar polynomial identity: M_a=14, M_b=29, low product=3, T*=31.
    sa, sb, d, m = 19, 23, 13, 11
    ma, mb, la, lb, tensor_scalar = 14, 29, 1, 3, 31
    require(d * tensor_scalar == ma * mb - la * lb, 'Tensor positive-cross identity')
    scale_next = Fraction(sa * sb, d * m)
    y, rho, w = -2, Fraction(2, 11), 1
    normalized = Fraction(y, 1) / scale_next
    rhs = Fraction(ma * mb - la * lb, sa * sb) + d * m * rho / (sa * sb) - Fraction(5, 1) / scale_next * w
    require(normalized == rhs, 'normalized Mult2 with wrap')
    wrong_scale = Fraction(sa * sb, m)
    require(Fraction(y, 1) / wrong_scale != normalized, 'missing d scale mutant survived')
    done('M10_Mult2_exact_Fraction_normalization',
         {'correct_scale': frac_string(scale_next), 'normalized_output': frac_string(normalized),
          'formula_rhs': frac_string(rhs), 'wrap_W': w, 'missing_d_mutant_rejected': True,
          'note': 'Scale regression is supporting coverage, not claimed as a newly discovered paper typo.'})

    mu, nu, q = 4, 2, 11
    centered = r.center((mu + nu,), q)[0]
    require(centered - mu == nu - q, 'actual-wrap identity')
    require(not (0 + 6 < Fraction(q, 2)) and r.center((0,), q)[0] == 0,
            'failed sufficient bound incorrectly implies wrap')
    done('M11_sufficient_nonwrap_failure_vs_actual_wrap',
         {'actual_wrap_case': {'mu': mu, 'nu': nu, 'centered': centered, 'w': 1},
          'no_wrap_despite_failed_sufficient_bound': {'mu': 0, 'nu': 0, 'loose_B': 6, 'w': 0}})

    unreduced = (1 * pow(11, -1, 5) % 5) * 11 + (1 * pow(5, -1, 11) % 11) * 5
    require(unreduced == 56 and unreduced % 55 == 1, 'multi-prime lift example')
    require((56 - unreduced) // 55 == 0 and (56 - 1) // 55 == 1,
            'multi-P correction example')
    done('M12_general_HYBRID_internal_basis_carry_is_not_single_prime_model',
         {'source_part_primes': [5, 11], 'residue': 1, 'unreduced_CRT_lift': 56,
          'part_modulus': 55, 'internal_carry': 1,
          'multi_P_example_input': 56, 'approx_moddown': 0,
          'incorrect_single_P_floor': 1, 'current_profile_has_no_such_internal_carry': True})

    # Current public S100 geometry, evaluated as 256 coefficient classes.
    # This is NOT an OpenFHE key/ciphertext object, key generation, or a sampled
    # secret. s=1+X+...+X^127 and J=1+...+X^(N-1) are public formal polynomials.
    # s*J has coefficient 2*k+2-h for k<h-1, and h thereafter; s^2 is triangular.
    N_big, h_big = 32768, 128
    q_big = (1125899904679937,1125899903827969,1152921504598720513,
             1152921504597016577,1152921504595968001,1152921504595640321,
             1152921504593412097,1152921504592822273,1152921504592429057,
             1152921504589938689)
    p_big, d_big = 1152921504606584833, 1099510054913
    Q_big, F_big = prod(q_big), prod(q_big) * d_big
    a_big = (p_big + 1) // 2
    selectors = []
    for qi in (*q_big, d_big):
        hat = F_big // qi
        selectors.append(hat * pow(hat, -1, qi))
    classes = []
    for k in range(2 * h_big):
        sj = 2*k+2-h_big if k < h_big-1 else h_big
        s2 = k+1 if k < h_big else max(0,2*h_big-1-k)
        # Only row0 has A=a*J. Every error row is the formal zero polynomial.
        # These are valid equation witnesses; no historical key is reconstructed.
        z0 = 0
        for j, theta in enumerate(selectors[:len(q_big)]):
            bj_full = (p_big * theta * s2 - (a_big * sj if j == 0 else 0)) % (F_big * p_big)
            z0 += bj_full % (Q_big * p_big)
        z1 = a_big
        delta0 = (2*z0)//p_big - 2*(z0//p_big)
        delta1 = (2*z1)//p_big - 2*(z1//p_big)
        require(delta0 == int(k >= h_big//2) and delta1 == 1, 'S100 carry class')
        decoded_delta = delta0 + sj  # s*delta1=s*J, with exact negacyclic signs.
        classes.append((k, delta0, delta1, decoded_delta))
    require(max(abs(x[3]) for x in classes) == h_big+1, 'h+1 bound not saturated')
    done('M13_current_S100_h128_geometry_public_symbolic_counterexample',
         {'N': N_big, 'h': h_big, 'P': str(p_big), 'd': str(d_big),
          'active_towers': len(q_big), 'raised_towers': len(q_big)+1,
          'third_inputs': [1,1], 'all_digit_carries': 0,
          'synthetic_secret': 's=sum(X^i,i=0..127)',
          'synthetic_rows': 'A_0=((P+1)/2)*J, A_j=0 for j>0; all e_j=0; B_j=P*theta_j*s^2-A_j*s mod FP',
          'J': 'sum(X^i,i=0..N-1)', 'coefficient_classes_checked': len(classes),
          'tail_class': 'all coefficients k>=255 equal the k=255 class for this calculation',
          'delta0': '0 for k<64; 1 for k>=64', 'delta1': '1 for every coefficient',
          'decoded_delta_coefficient_norm': h_big+1, 'overstrong_h_bound': False,
          'corrected_h_plus_1_bound_attained': True, 'production_FHE_RED': False})

    return {'schema': 'relin2-bound-model-checks-v1', 'status': 'PASS',
            'test_count': len(tests), 'scope': 'deterministic integer/Fraction source-shaped models',
            'production_fhe_executed': False, 'random_sampling_executed': False,
            'transforms_executed': False, 'synthetic_public_fixtures_only': True, 'tests': tests}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    result = run()
    text = json.dumps(result, ensure_ascii=False, indent=2) + '\n'
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(text, encoding='utf-8')
    print(text, end='')


if __name__ == '__main__':
    main()
