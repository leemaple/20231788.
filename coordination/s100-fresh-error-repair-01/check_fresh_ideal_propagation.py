#!/usr/bin/env python3
"""Bounded scalar replay: ten retained complex anchors, eight ideal squarings.

No OpenFHE, FFT, ciphertext, key, full-slot data, or external dependency is used.
The output bounds exact powers for each printed A+B component perturbed by at
most 1e-100. C is readout-only; E0 is retained solely for consistency checks.
This enclosure is an analysis robustness check, not a change to
any cryptographic test tolerance. Read FRESH_IDEAL_PROPAGATION.md for scope.
"""

from decimal import Decimal as D, localcontext, ROUND_FLOOR, ROUND_CEILING
import hashlib
from pathlib import Path
import re


LOG_SHA256 = "cbd1d44bb8bdfbf7f9ff74bc94e950ce8a7602cd5e8bb43d98da6f403b2a874d"
SOURCE = "a448b787399b43b6024d82c170add403969b493c"
ANCHORS = (0, 1, 256, 257, 512, 513, 768, 769, 1023, 16383)
PRECISION = 250
RADIUS = D("1e-100")


def rounded(operation, mode):
    with localcontext() as ctx:
        ctx.prec = PRECISION
        ctx.rounding = mode
        return operation()


def point(value):
    return (value, value)


def add(a, b):
    return (rounded(lambda: a[0] + b[0], ROUND_FLOOR),
            rounded(lambda: a[1] + b[1], ROUND_CEILING))


def subtract(a, b):
    return (rounded(lambda: a[0] - b[1], ROUND_FLOOR),
            rounded(lambda: a[1] - b[0], ROUND_CEILING))


def multiply(a, b):
    lower = [rounded(lambda: x * y, ROUND_FLOOR) for x in a for y in b]
    upper = [rounded(lambda: x * y, ROUND_CEILING) for x in a for y in b]
    return (min(lower), max(upper))


def square_complex(z):
    re, im = z
    return (subtract(multiply(re, re), multiply(im, im)),
            multiply(point(D(2)), multiply(re, im)))


def eight_squares(z):
    for _ in range(8):
        z = square_complex(z)
    return z


def absolute_bounds(value):
    low, high = value
    assert low <= high
    if low >= 0:
        return value
    if high <= 0:
        return (high.copy_negate(), low.copy_negate())
    return (D(0), max(low.copy_abs(), high.copy_abs()))


def frozen_input(slot):
    # Exact dyadic arithmetic; all these terminating decimals fit in 250 digits.
    with localcontext() as ctx:
        ctx.prec = PRECISION
        t = slot // 2
        a = D(1015) / 1024 - D(t % 16) / 65536 + D(slot) * D(2) ** -75
        b = D(1 + (t // 16) % 8) / 1024
        if (t // 512) % 2:
            b = -b
        return ((a, b), (-b, a), (-a, -b), (b, -a))[(t // 128) % 4]


def read_anchors():
    raw = Path(__file__).with_name("remote-green2-diagnostic-steps.log").read_bytes()
    assert hashlib.sha256(raw).hexdigest() == LOG_SHA256, "wrong source log"
    text = raw.decode("utf-8")
    assert f"mode=--fresh source={SOURCE}" in text
    assert "status=COMPLETE mode=fresh public_encryptions=1" in text
    rows = {}
    for line in text.splitlines():
        if "S100 tuple=anchor " not in line:
            continue
        fields = dict(re.findall(r"(\w+)=([^\s]+)", line))
        key = (int(fields["slot"]), fields["component"])
        assert key not in rows, "duplicate anchor"
        rows[key] = {name: D(fields[name]) for name in
                     ("z", "encoding", "encryption", "decoding", "E0")}
        assert all(value.is_finite() for value in rows[key].values()), "nonfinite tuple"
    assert set(rows) == {(s, c) for s in ANCHORS for c in ("real", "imag")}
    return rows


def main():
    rows = read_anchors()
    with localcontext() as ctx:
        ctx.prec = PRECISION
        threshold = D(2) ** -80
    print(f"source={SOURCE} log_sha256={LOG_SHA256}")
    print("checker_sha256=" + hashlib.sha256(Path(__file__).read_bytes()).hexdigest())
    print("perturbation=A+B excluded_readout=C historical_E0=CONSISTENCY_ONLY")
    print(f"anchors=10 components=20 squarings=8 decimal_precision={PRECISION}")
    print(f"input_component_radius={RADIUS} threshold={threshold}")
    exceeding = []
    largest = (D(0), None, None)
    maximum_interval_width = D(0)
    maximum_readout = D(0)
    maximum_e0_phase_gap = D(0)
    maximum_tuple_residual = D(0)
    for slot in ANCHORS:
        logged = [rows[(slot, c)] for c in ("real", "imag")]
        z = frozen_input(slot)
        assert tuple(r["z"] for r in logged) == z, "frozen dyadic input mismatch"
        ideal = eight_squares(tuple(point(v) for v in z))
        observed = []
        for row in logged:
            # Directed addition prevents Python's default Decimal precision 28
            # from truncating A+B before the interval calculation begins.
            phase_error = add(point(row["encoding"]), point(row["encryption"]))
            e0_gap = subtract(phase_error, point(row["E0"]))
            residual = add(e0_gap, point(row["decoding"]))
            maximum_readout = max(maximum_readout, row["decoding"].copy_abs())
            maximum_e0_phase_gap = max(maximum_e0_phase_gap, absolute_bounds(e0_gap)[1])
            maximum_tuple_residual = max(maximum_tuple_residual, absolute_bounds(residual)[1])
            error_box = (subtract(phase_error, point(RADIUS))[0],
                         add(phase_error, point(RADIUS))[1])
            observed.append(add(point(row["z"]), error_box))
        propagated = eight_squares(tuple(observed))
        for component, name in enumerate(("real", "imag")):
            signed = subtract(propagated[component], ideal[component])
            width = rounded(lambda: signed[1] - signed[0], ROUND_CEILING)
            maximum_interval_width = max(maximum_interval_width, width)
            low, high = absolute_bounds(signed)
            ratio_low = rounded(lambda: low / threshold, ROUND_FLOOR)
            ratio_high = rounded(lambda: high / threshold, ROUND_CEILING)
            status = "EXCEEDS" if low > threshold else "BELOW" if high <= threshold else "UNRESOLVED"
            if status == "EXCEEDS":
                exceeding.append((slot, name))
            if low > largest[0]:
                largest = (low, slot, name)
            # 24 displayed digits summarize the internally directed enclosure;
            # explicit rational coarse bounds below avoid interpreting these
            # nearest-formatted summaries as outward-rounded endpoints.
            print(f"slot={slot} component={name} signed_low={signed[0]:.23E} "
                  f"signed_high={signed[1]:.23E} ratio_low={ratio_low:.23E} "
                  f"ratio_high={ratio_high:.23E} status={status}")
            if slot == 0:
                print(f"prespecified_slot0_witness component={name} status={status} "
                      f"absolute_upper_bound={high:.23E} ratio_upper={ratio_high:.23E}")
            if status == "EXCEEDS":
                # These deliberately coarser decimal bounds are rounded OUTWARD.
                quantum = D("1e-35")
                coarse_low = low.quantize(quantum, rounding=ROUND_FLOOR)
                coarse_high = high.quantize(quantum, rounding=ROUND_CEILING)
                print(f"certified_component_abs_interval=[{coarse_low},{coarse_high}]")
    print(f"exceeding_components={exceeding}")
    print(f"largest_anchor_lower_bound_slot={largest[1]} component={largest[2]}")
    print(f"largest_anchor_absolute_lower_bound={largest[0]:.23E}")
    print(f"maximum_signed_interval_width={maximum_interval_width:.23E}")
    assert maximum_e0_phase_gap <= RADIUS, "historical E0 approximation exceeds analysis radius"
    assert maximum_tuple_residual <= RADIUS, "logged A+B+C/E0 inconsistency exceeds analysis radius"
    print(f"maximum_anchor_readout_abs={maximum_readout:.23E}")
    print(f"maximum_anchor_E0_minus_phase_abs_upper={maximum_e0_phase_gap:.23E}")
    print(f"maximum_anchor_ABC_minus_E0_abs_upper={maximum_tuple_residual:.23E}")
    required_cancellation = rounded(lambda: largest[0] - threshold, ROUND_FLOOR)
    print("largest_anchor_required_opposing_error_lower_bound=" +
          str(required_cancellation.quantize(D("1e-35"), rounding=ROUND_FLOOR)))


if __name__ == "__main__":
    main()
