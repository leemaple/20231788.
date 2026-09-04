#!/usr/bin/env python3
"""Exact integer witnesses for the BV fixed-key-bound review.

No OpenFHE code or supplied executable is imported or executed.
"""
from decimal import Decimal, getcontext
from itertools import product
from math import gcd, prod

MODULI = [
    34359736577,
    1073744257,
    1073738753,
    1073742721,
    1073739649,
    1073742209,
    1073741441,
]
Q_DIV = 1073741953
N = 64


def center(value: int, modulus: int) -> int:
    value %= modulus
    return value - modulus if value > modulus // 2 else value


def negacyclic2(left: tuple[int, int], right: tuple[int, int]) -> tuple[int, int]:
    return (
        left[0] * right[0] - left[1] * right[1],
        left[0] * right[1] + left[1] * right[0],
    )


def main() -> None:
    getcontext().prec = 60
    q_l_product = prod(MODULI)
    q_l = MODULI[-1]
    print(f"active_moduli={','.join(map(str, MODULI))}")
    print(f"q_div={Q_DIV}")
    print(f"q_l={q_l}")
    print(f"Q_l={q_l_product}")
    print(f"Q_l_bits={q_l_product.bit_length()}")
    print(f"q_div_times_q_l={Q_DIV * q_l}")
    ratio = Decimal(2**60) / Decimal(Q_DIV * q_l)
    print(f"two_to_60_over_q_div_q_l={ratio}")

    pairwise_coprime = all(
        gcd(MODULI[i], MODULI[j]) == 1
        for i in range(len(MODULI))
        for j in range(i + 1, len(MODULI))
    )
    print(f"active_moduli_pairwise_coprime={pairwise_coprime}")

    digit_bounds = [q // 2 for q in MODULI]
    for index, (modulus, bound) in enumerate(zip(MODULI, digit_bounds)):
        print(
            f"row={index} q_i={modulus} floor_q_i_over_2={bound} "
            f"N_times_bound={N * bound} gcd_q_div_q_i={gcd(Q_DIV, modulus)}"
        )
    print(f"sum_centered_digit_bounds={sum(digit_bounds)}")
    print(f"N_times_sum_centered_digit_bounds={N * sum(digit_bounds)}")

    # Exact N=2 witness. Each row residual has infinity norm one.
    # B = N * (D_0*R_0 + D_1*R_1) = 2*(1+2) = 6.
    rho0 = (1, -1)
    rho1 = (-1, 1)
    maximum = -1
    maximizer = None
    maximum_value = None
    for d0 in product(range(-1, 2), repeat=2):
        for d1 in product(range(-2, 3), repeat=2):
            p0 = negacyclic2(d0, rho0)
            p1 = negacyclic2(d1, rho1)
            value = (p0[0] + p1[0], p0[1] + p1[1])
            norm = max(abs(value[0]), abs(value[1]))
            if norm > maximum:
                maximum = norm
                maximizer = (d0, d1)
                maximum_value = value
    print(f"toy_bound=6")
    print(f"toy_exhaustive_max={maximum}")
    print(f"toy_maximizer_d0={maximizer[0]} toy_maximizer_d1={maximizer[1]}")
    print(f"toy_maximizer_error={maximum_value}")
    print(f"toy_bound_without_N=3")
    print(f"toy_missing_N_fails={maximum > 3}")

    # The q_i/2 bound is not source-closed unless SwitchModulus uses a
    # centered source lift. For q_i=3 and residue 2, a canonical lift is 2,
    # whereas the centered lift is -1.
    print(f"lift_gap_source_q=3 residue=2 centered={center(2, 3)} canonical=2")
    print("lift_gap_centered_bound=1 canonical_violates_bound=true")

    # A centered modular triangle inequality can pass after wrap.
    wrapped = center(40 + 40, 101)
    print(f"wrap_witness_Q=101 left=40 right=40 centered_sum={wrapped}")
    print(f"wrap_triangle_passes={abs(wrapped) <= 40 + 40}")


if __name__ == "__main__":
    main()
