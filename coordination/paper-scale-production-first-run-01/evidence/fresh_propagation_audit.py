#!/usr/bin/env python3
"""Bounded scalar audit of the retained first Windows paper-test transcript.

Faithfully consolidates the two interactive Decimal calculations used in the
independent review; re-executed after saving. No transforms, crypto, or writes.
Analytical bounds are evaluated with Decimal(80), not interval arithmetic.
"""
from decimal import Decimal as D, getcontext
from pathlib import Path
import re

getcontext().prec = 80
SOURCE = "b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89"
PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
LOG = Path(__file__).with_name("WINDOWS_LF.log")
ANCHORS = (0, 1, 256, 257, 512, 513, 768, 769, 1023, 16383)
GATE = D(2) ** -80


def mul(z, w):
    return z[0] * w[0] - z[1] * w[1], z[0] * w[1] + z[1] * w[0]


def power(z, exponent):
    out = (D(1), D(0))
    while exponent:
        if exponent & 1:
            out = mul(out, z)
        z = mul(z, z)
        exponent //= 2
    return out


def frozen_input(slot):
    t = slot // 2
    a = D(1015) / 1024 - D(t % 16) / 65536 + D(slot) / (D(2) ** 75)
    b = D(1 + (t // 16) % 8) / 1024
    if (t // 512) % 2:
        b = -b
    return ((a, b), (-b, a), (-a, -b), (b, -a))[(t // 128) % 4]


def propagated_interval(slot, exponent, error):
    z = frozen_input(slot)
    # Since |z| + |epsilon| < 1, the binomial/Taylor remainder is at most
    # k*(k-1)/2 * |epsilon|^2 <= k*(k-1)*E^2.
    assert (z[0] ** 2 + z[1] ** 2).sqrt() + D(2).sqrt() * error < 1
    c = tuple(D(exponent) * value for value in power(z, exponent - 1))
    induced = abs(c[0]) + abs(c[1])
    lower = (c[0] ** 2 + c[1] ** 2) / induced * error
    upper = induced * error
    remainder = D(exponent * (exponent - 1)) * error * error
    return lower - remainder, upper + remainder, remainder


rows = {}
fresh_full = None
active = False
finished = False
with LOG.open(encoding="utf-8") as stream:
    for line_number, line in enumerate(stream, 1):
        if "61: BEGIN test=paper_full_eight_square_contract " in line:
            assert not active
            assert "source=" + SOURCE in line and "openfhe_pin=" + PIN in line
            active = True
            begin_line = line_number
        if not active:
            continue
        match = re.search(
            r"61: OBS field=(fresh|round_[1-4])\.anchor_(\d+)_error value=([0-9.e+-]+)",
            line,
        )
        if match:
            key = match[1], int(match[2])
            assert key not in rows
            rows[key] = D(match[3])
        match = re.search(r"61: OBS field=fresh.full_max_component_error value=([0-9.e+-]+)", line)
        if match:
            assert fresh_full is None
            fresh_full = D(match[1])
        if "61: COMPLETE test=paper_full_eight_square_contract " in line:
            assert "result=FAIL" in line and "round_4 independent anchor 2^-80 gate" in line
            end_line = line_number
            finished = True
            break  # Exclude CTest's subsequent replay of the same execution.

assert finished and fresh_full is not None
assert set(rows) == {(stage, slot) for stage in ("fresh", "round_1", "round_2", "round_3", "round_4") for slot in ANCHORS}
print(f"source={SOURCE} pin={PIN} original_lines={begin_line}..{end_line} anchor_observations={len(rows)}")
print(f"gate={GATE:.25E}")
for slot in ANCHORS:
    error = rows["fresh", slot]
    minimum_added = []
    for round_index in range(1, 5):
        lower, upper, remainder = propagated_interval(slot, 2 ** round_index, error)
        observed = rows[f"round_{round_index}", slot]
        # Reverse triangle inequality: distance outside this interval is a
        # lower bound on accumulated departure from pure-fresh propagation.
        excess = max(D(0), observed - upper, lower - observed)
        minimum_added.append(excess)
        if slot == 512:
            print(f"slot=512 round={round_index} pure_lower={lower:.25E} pure_upper={upper:.25E} remainder_bound={remainder:.8E} observed={observed:.25E}")
    print(f"slot={slot} minimum_added_by_round=" + ",".join(f"{value:.12E}" if value else "0" for value in minimum_added))

lower, upper, remainder = propagated_interval(512, 256, rows["fresh", 512])
print(f"slot512_counterfactual_terminal lower={lower:.25E} upper={upper:.25E} lower_gate_ratio={lower/GATE:.18F} upper_gate_ratio={upper/GATE:.18F} remainder_bound={remainder:.8E}")
encoding_bound = D(32768) / (D(2) ** 101)
print(f"nearest_coefficient_rounding_component_bound={encoding_bound:.25E}")
print(f"fresh_full_beyond_rounding_bound={fresh_full-encoding_bound:.25E}")
print(f"round4_max_gate_ratio={rows['round_4', 512]/GATE:.18F}")
print(f"heuristic_dense_v_complex_rms={D('3.19')*32768*(D(2)/3).sqrt()/(D(2)**100):.25E}")
print(f"heuristic_dense_v_component_rms={D('3.19')*32768/(D(3).sqrt()*(D(2)**100)):.25E}")
