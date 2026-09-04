#!/usr/bin/env python3
from __future__ import annotations

from itertools import product
from math import gcd


def source_switch(v: int, p: int, r: int) -> int:
    """Exact mathematical model of the cited NativeVectorT::SwitchModulus branch."""
    assert 0 <= v < p and p > 0 and r > 0
    half_q = p >> 1
    diff = abs(p - r)
    if r > p:
        # AddEqFast; source performs no final reduction.
        return v + (diff if v > half_q else 0)
    # ModSubEq reduces both operands modulo r and returns canonical difference.
    return (v - (diff if v > half_q else 0)) % r


def centered_lift(v: int, p: int) -> int:
    half_q = p // 2
    return v if v <= half_q else v - p


def negacyclic(a: tuple[int, ...], b: tuple[int, ...]) -> tuple[int, ...]:
    assert len(a) == len(b)
    n = len(a)
    out = [0] * n
    for i, av in enumerate(a):
        for j, bv in enumerate(b):
            k = i + j
            if k < n:
                out[k] += av * bv
            else:
                out[k - n] -= av * bv
    return tuple(out)


def add(a: tuple[int, ...], b: tuple[int, ...]) -> tuple[int, ...]:
    return tuple(x + y for x, y in zip(a, b))


def inf_norm(a: tuple[int, ...]) -> int:
    return max(abs(x) for x in a)


def l1_norm(a: tuple[int, ...]) -> int:
    return sum(abs(x) for x in a)


def main() -> None:
    # Independent exhaustive scalar witness, not the supplied JS witness.
    moduli = list(range(3, 102, 2))
    scalar_cases = 0
    increasing_cases = 0
    decreasing_cases = 0
    equal_cases = 0
    lazy_distinctions = 0
    for p in moduli:
        for r in moduli:
            for v in range(p):
                actual = source_switch(v, p, r)
                d = centered_lift(v, p)
                expected = d % r
                assert actual == expected, (p, r, v, actual, expected)
                assert 0 <= actual < r
                scalar_cases += 1
                if r > p:
                    increasing_cases += 1
                    # The raw AddEqFast result itself is representable below r.
                    assert actual <= r - 1
                elif r < p:
                    decreasing_cases += 1
                else:
                    equal_cases += 1
                if v % r != expected:
                    lazy_distinctions += 1

            k = p // 2
            assert centered_lift(0, p) == 0
            assert centered_lift(k, p) == k
            assert centered_lift(k + 1, p) == -k
            assert centered_lift(p - 1, p) == -1

    # Exact current fixture arithmetic copied from the verified prior derivation,
    # then independently recomputed here.
    active = [
        34359736577,
        1073744257,
        1073738753,
        1073742721,
        1073739649,
        1073742209,
        1073741441,
    ]
    q_div = 1073741953
    assert all(q % 2 == 1 for q in active + [q_div])
    assert max(active + [q_div]).bit_length() == 35
    assert all(gcd(active[i], active[j]) == 1
               for i in range(len(active)) for j in range(i + 1, len(active)))
    assert all(gcd(q_div, q) == 1 for q in active)
    q_l_product = 1
    for q in active:
        q_l_product *= q
    assert q_l_product == 52656049226897061758347970843194892279389197066160739197584863617
    radii = [q // 2 for q in active]

    # Independent N=2 toy: exact l1 bound and the prior N*infinity bound.
    n = 2
    rho0 = (1, 0)
    rho1 = (0, 1)
    d0_radius = 1
    d1_radius = 2
    b_l1 = d0_radius * l1_norm(rho0) + d1_radius * l1_norm(rho1)
    b_inf = n * (d0_radius * inf_norm(rho0) + d1_radius * inf_norm(rho1))
    observed = 0
    for d0 in product(range(-d0_radius, d0_radius + 1), repeat=n):
        for d1 in product(range(-d1_radius, d1_radius + 1), repeat=n):
            z = add(negacyclic(d0, rho0), negacyclic(d1, rho1))
            observed = max(observed, inf_norm(z))
            assert inf_norm(z) <= b_l1 <= b_inf
    assert observed == 3 and b_l1 == 3 and b_inf == 6

    # Factor N is needed for the infinity-norm conversion in general.
    rho = (1, 1)
    d_radius = 1
    observed_single = 0
    for d in product(range(-d_radius, d_radius + 1), repeat=n):
        observed_single = max(observed_single, inf_norm(negacyclic(d, rho)))
    assert observed_single == 2
    assert d_radius * inf_norm(rho) == 1 < observed_single
    assert d_radius * l1_norm(rho) == n * d_radius * inf_norm(rho) == 2

    # Modular triangle acceptance is not an integer-lift/no-wrap proof.
    q = 101
    centered_80 = ((80 + q // 2) % q) - q // 2
    assert centered_80 == -21 and abs(centered_80) <= 80

    print("independent_static_checks=PASS")
    print(f"scalar_switch_cases={scalar_cases}")
    print(f"increasing_cases={increasing_cases}")
    print(f"decreasing_cases={decreasing_cases}")
    print(f"equal_modulus_cases={equal_cases}")
    print(f"lazy_unsigned_distinctions={lazy_distinctions}")
    print("strict_halfway_for_all_odd_p=PASS")
    print("zero_lift_for_all_tested_p_r=PASS")
    print(f"fixture_active_rows={len(active)}")
    print(f"fixture_max_modulus_bits={max(active + [q_div]).bit_length()}")
    print(f"fixture_Q_l={q_l_product}")
    print(f"fixture_q_div={q_div}")
    print("fixture_pairwise_coprime=PASS")
    print("fixture_q_div_unit_each_active_tower=PASS")
    print("fixture_digit_radii=" + ",".join(str(x) for x in radii))
    print(f"toy_l1_bound={b_l1}")
    print(f"toy_N_inf_bound={b_inf}")
    print(f"toy_observed_max={observed}")
    print("infinity_bound_requires_N_witness=PASS")
    print("Q101_center_40_plus_40=-21")
    print("modular_triangle_does_not_imply_no_wrap=PASS")


if __name__ == "__main__":
    main()
