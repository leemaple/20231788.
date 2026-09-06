"""Independent stdlib scalar replay for validated endpoint sidecars.

This module performs no parsing of files, project imports, transforms, crypto,
primary-log reconciliation, compression, status writing, or publication.
"""
from dataclasses import dataclass
from decimal import (Context, Decimal, DecimalException, InvalidOperation,
                     ROUND_HALF_EVEN, localcontext)
from fractions import Fraction
import re
from typing import Mapping, Tuple


PRECISION = 256
ROW_COUNT = 16384
CONTEXT = Context(prec=PRECISION, rounding=ROUND_HALF_EVEN,
                  Emin=-25_600_000, Emax=25_600_000, capitals=0, clamp=0)
ZERO = "+0." + "0" * 109 + "e+00000"
_CANONICAL = re.compile(r"[+-][1-9]\.[0-9]{109}e[+-][0-9]{5}", re.ASCII)
ComplexFraction = Tuple[Fraction, Fraction]


class ReplayError(ValueError):
    pass


class ReplayUnresolved(ReplayError):
    pass


@dataclass(frozen=True)
class ScalarReplay:
    e0: ComplexFraction
    e8: ComplexFraction
    fresh: ComplexFraction
    z_power: ComplexFraction
    fresh_power: ComplexFraction
    terminal: ComplexFraction
    i8: ComplexFraction
    a8: ComplexFraction
    identity: ComplexFraction


@dataclass(frozen=True)
class _DecimalReplay:
    fresh: tuple
    z_power: tuple
    fresh_power: tuple
    terminal: tuple
    i8: tuple
    a8: tuple
    identity: tuple


@dataclass(frozen=True)
class ReplayBounds:
    serialization_e0: Fraction
    serialization_e8: Fraction
    live_e0: Fraction
    live_e8: Fraction
    live_i8: Fraction
    live_a8: Fraction
    replay_e0: Fraction
    replay_e8: Fraction
    replay_i8: Fraction
    replay_a8: Fraction


@dataclass(frozen=True)
class ReplayMetric:
    maximum: Fraction
    argmax_slot: int
    argmax_component: str
    signed_tuple: Tuple[ComplexFraction, ComplexFraction,
                        ComplexFraction, ComplexFraction]


@dataclass(frozen=True)
class ReplaySummary:
    row_count: int
    e0: ReplayMetric
    e8: ReplayMetric
    i8: ReplayMetric
    a8: ReplayMetric
    r_identity: ReplayMetric


@dataclass(frozen=True)
class ReplayResult:
    row_count: int
    bounds: ReplayBounds
    e0: ReplayMetric
    e8: ReplayMetric
    i8: ReplayMetric
    a8: ReplayMetric
    r_identity: ReplayMetric
    decimal_precision: int = PRECISION
    decimal_rounding: str = "ROUND_HALF_EVEN"


def _require(condition, message):
    if not condition:
        raise ReplayError(message)


def _fraction(value):
    if isinstance(value, Fraction):
        return value
    _require(value.is_finite(), "nonfinite Decimal replay value")
    sign, digits, exponent = value.as_tuple()
    coefficient = 0
    for digit in digits:
        coefficient = coefficient * 10 + digit
    if sign:
        coefficient = -coefficient
    if exponent >= 0:
        return Fraction(coefficient * 10 ** exponent)
    return Fraction(coefficient, 10 ** -exponent)


def _canonical_decimal(text):
    _require(isinstance(text, str) and len(text) == 119 and text.isascii(),
             "canonical decimal byte grammar")
    if text == ZERO:
        return Decimal(0)
    _require(_CANONICAL.fullmatch(text) is not None and not text.endswith("e-00000"),
             "canonical decimal grammar")
    try:
        value = Decimal(text)
    except InvalidOperation as error:
        raise ReplayError("invalid canonical decimal") from error
    _require(value.is_finite() and not value.is_zero(), "canonical nonzero semantics")
    return value


def _decimal_from_exact(value):
    with localcontext(CONTEXT):
        result = Decimal(value.numerator) / Decimal(value.denominator)
    _require(result.is_finite() and _fraction(result) == value,
             "exact input is not terminating within Decimal precision")
    return result


def _square(value):
    real, imag = value
    return real * real - imag * imag, real * imag + imag * real


def _square_eight(value):
    result = value
    for _ in range(8):
        result = _square(result)
    return result


def _scalar_kernel(z_decimal, e0_decimal, e8_decimal):
    """The single Decimal256 arithmetic graph used by both public replays."""
    with localcontext(CONTEXT):
        fresh = (z_decimal[0] + e0_decimal[0],
                 z_decimal[1] + e0_decimal[1])
        if not _within_disk(_complex_fraction(fresh), Fraction(3, 2)):
            raise ReplayUnresolved("represented Decimal fresh value exceeds 3/2 disk")
        z_power = _square_eight(z_decimal)
        fresh_power = _square_eight(fresh)
        terminal = (z_power[0] + e8_decimal[0],
                    z_power[1] + e8_decimal[1])
        i8 = (fresh_power[0] - z_power[0],
              fresh_power[1] - z_power[1])
        a8 = (terminal[0] - fresh_power[0],
              terminal[1] - fresh_power[1])
        identity = (e8_decimal[0] - i8[0] - a8[0],
                    e8_decimal[1] - i8[1] - a8[1])
        values = (fresh, z_power, fresh_power, terminal, i8, a8, identity)
        _require(all(part.is_finite() for value in values for part in value),
                 "nonfinite Decimal replay arithmetic")
        return _DecimalReplay(*values)


def _complex_fraction(value):
    return _fraction(value[0]), _fraction(value[1])


def _magnitude(value):
    # Decimal.__abs__ applies the ambient context; copy_abs preserves all
    # digits when comparisons happen outside the replay's Decimal256 context.
    return value.copy_abs() if isinstance(value, Decimal) else abs(value)


def _within_disk(value, radius):
    return _magnitude(value[0]) + _magnitude(value[1]) <= radius


def _pow2(exponent):
    return Fraction(1 << exponent) if exponent >= 0 else Fraction(1, 1 << -exponent)


def _ceiling_power_of_two(coefficient_l1, scale):
    if coefficient_l1 == 0:
        return Fraction(0)
    numerator = coefficient_l1 * scale.denominator
    denominator = scale.numerator
    exponent = numerator.bit_length() - denominator.bit_length()
    if exponent >= 0:
        if numerator > denominator << exponent:
            exponent += 1
    elif numerator << -exponent > denominator:
        exponent += 1
    return _pow2(exponent)


def _serialization_bound(exponent):
    if exponent is None:
        return Fraction(0)
    power = exponent - 109
    if power >= 0:
        return Fraction(10 ** power)
    return Fraction(1, 10 ** -power)


def _derive_bounds(meta, t0, t8):
    _require(isinstance(meta, Mapping), "replay metadata mapping")
    values = []
    for key in ("coefficient_l1_fresh", "coefficient_l1_terminal"):
        value = meta.get(key)
        _require(isinstance(value, int) and not isinstance(value, bool) and value >= 0,
                 "replay coefficient metadata")
        values.append(value)
    scales = []
    for key in ("scale0", "scale8"):
        value = meta.get(key)
        _require(isinstance(value, Fraction) and value > 0, "replay scale metadata")
        scales.append(value)

    fresh_k = _ceiling_power_of_two(values[0], scales[0])
    terminal_k = _ceiling_power_of_two(values[1], scales[1])
    d_fresh, d_terminal = _pow2(12 - 768) * fresh_k, _pow2(12 - 768) * terminal_k
    p, r = _pow2(270 - 768), _pow2(264 - 768)
    q = p + _pow2(263) * d_fresh
    live_e0 = d_fresh + r
    live_e8 = d_terminal + p + r
    live_i8 = q + p + r
    live_a8 = d_terminal + q + r

    u_dec = Fraction(1, 10 ** 255)
    p_dec, r_dec = (1 << 270) * u_dec, (1 << 264) * u_dec
    amplification = 256 * Fraction(3, 2) ** 255
    _require(amplification < _pow2(158), "replay amplification proof")
    u0 = d_fresh + r + t0 + r_dec
    replay_e0 = live_e0 + t0
    replay_e8 = live_e8 + t8
    replay_i8 = amplification * u0 + 2 * p_dec + r_dec
    replay_a8 = live_e8 + t8 + amplification * u0 + 2 * p_dec + 2 * r_dec
    bounds = ReplayBounds(t0, t8, live_e0, live_e8, live_i8, live_a8,
                          replay_e0, replay_e8, replay_i8, replay_a8)
    budget = _pow2(-128)
    if any(value > budget for value in bounds.__dict__.values()):
        raise ReplayUnresolved("scalar replay allowance exceeds 2^-128 budget")
    return bounds


def derive_replay_bounds(meta, *, maximum_e0_exponent, maximum_e8_exponent):
    """Derive exact replay bounds from validated metadata and worst exponents."""
    for exponent in (maximum_e0_exponent, maximum_e8_exponent):
        _require(exponent is None or
                 (isinstance(exponent, int) and not isinstance(exponent, bool) and
                  -99999 <= exponent <= 99999),
                 "canonical serialization exponent")
    # This avoids constructing an enormous positive power that is already many
    # orders of magnitude beyond the exact estimator budget.
    if ((maximum_e0_exponent is not None and maximum_e0_exponent >= 109) or
            (maximum_e8_exponent is not None and maximum_e8_exponent >= 109)):
        raise ReplayUnresolved("scalar replay serialization budget exceeded")
    return _derive_bounds(
        meta, _serialization_bound(maximum_e0_exponent),
        _serialization_bound(maximum_e8_exponent))


def _preflight(sidecar):
    rows = getattr(sidecar, "rows", None)
    _require(isinstance(rows, tuple) and len(rows) == ROW_COUNT,
             "scalar replay requires exactly 16384 rows")
    maximum_e0 = maximum_e8 = None
    for slot, row in enumerate(rows):
        _require(getattr(row, "slot", None) == slot, "ordered scalar replay rows")
        values = getattr(row, "values", None)
        _require(isinstance(values, tuple) and len(values) == 4, "scalar replay row shape")
        for index, value in enumerate(values):
            text = getattr(value, "text", None)
            decimal_value = _canonical_decimal(text)
            if decimal_value.is_zero():
                continue
            exponent = int(text[113:])
            if index < 2:
                maximum_e0 = exponent if maximum_e0 is None else max(maximum_e0, exponent)
            else:
                maximum_e8 = exponent if maximum_e8 is None else max(maximum_e8, exponent)
    bounds = derive_replay_bounds(
        getattr(sidecar, "meta", None), maximum_e0_exponent=maximum_e0,
        maximum_e8_exponent=maximum_e8)
    return rows, bounds


def frozen_input(slot):
    """Return the exact signed dyadic z for one frozen slot."""
    _require(isinstance(slot, int) and not isinstance(slot, bool) and 0 <= slot < ROW_COUNT,
             "frozen slot range")
    half_slot = slot // 2
    real = (Fraction(1015, 1024) - Fraction(half_slot % 16, 65536) +
            Fraction(slot, 1 << 75))
    imag = Fraction(1 + ((half_slot // 16) % 8), 1024)
    if (half_slot // 512) % 2:
        imag = -imag
    phase = (half_slot // 128) % 4
    if phase == 0:
        return real, imag
    if phase == 1:
        return -imag, real
    if phase == 2:
        return -real, -imag
    return imag, -real


def _update_maximum(accumulator, name, slot, vectors):
    current = accumulator[name]
    for component_index, component in enumerate(("real", "imag")):
        value = vectors[name][component_index]
        magnitude = _magnitude(value)
        if current is None or magnitude > current[0]:
            signed_tuple = tuple(vectors[key] for key in ("E0", "E8", "I8", "A8"))
            current = magnitude, slot, component, signed_tuple
    accumulator[name] = current


def _finish_metric(value):
    maximum, slot, component, signed_tuple = value
    return ReplayMetric(_fraction(maximum), slot, component,
                        tuple(tuple(_fraction(part) for part in complex_value)
                              for complex_value in signed_tuple))


def summarize_scalars(records):
    """Summarize an explicitly bounded ordered synthetic scalar sequence."""
    _require(isinstance(records, tuple) and len(records) > 0,
             "bounded scalar summary records")
    maxima = {name: None for name in ("E0", "E8", "I8", "A8", "R")}
    previous_slot = -1
    for slot, scalar in records:
        _require(isinstance(slot, int) and not isinstance(slot, bool) and
                 previous_slot < slot < ROW_COUNT,
                 "ordered bounded scalar summary slots")
        _require(isinstance(scalar, ScalarReplay), "bounded scalar replay record")
        previous_slot = slot
        vectors = {"E0": scalar.e0, "E8": scalar.e8, "I8": scalar.i8,
                   "A8": scalar.a8, "R": scalar.identity}
        for name in maxima:
            _update_maximum(maxima, name, slot, vectors)
    return ReplaySummary(
        len(records), _finish_metric(maxima["E0"]), _finish_metric(maxima["E8"]),
        _finish_metric(maxima["I8"]), _finish_metric(maxima["A8"]),
        _finish_metric(maxima["R"]))


def replay_complex(*, z, e0_text, e8_text):
    """Replay one scalar using the spec's exact eight-squaring graph."""
    _require(len(z) == 2 and all(isinstance(part, Fraction) for part in z),
             "exact z shape")
    _require(len(e0_text) == 2 and len(e8_text) == 2, "residual shape")
    e0_decimal = tuple(_canonical_decimal(text) for text in e0_text)
    e8_decimal = tuple(_canonical_decimal(text) for text in e8_text)
    e0_exact, e8_exact = _complex_fraction(e0_decimal), _complex_fraction(e8_decimal)
    exact_fresh = z[0] + e0_exact[0], z[1] + e0_exact[1]
    if not _within_disk(exact_fresh, Fraction(3, 2)):
        raise ReplayUnresolved("exact reconstructed fresh value exceeds 3/2 disk")

    z_decimal = _decimal_from_exact(z[0]), _decimal_from_exact(z[1])
    result = _scalar_kernel(z_decimal, e0_decimal, e8_decimal)

    return ScalarReplay(
        e0_exact, e8_exact, _complex_fraction(result.fresh),
        _complex_fraction(result.z_power), _complex_fraction(result.fresh_power),
        _complex_fraction(result.terminal), _complex_fraction(result.i8),
        _complex_fraction(result.a8), _complex_fraction(result.identity))


def replay_row(slot, row):
    """Replay one parsed row selected by a future primary receipt's slot."""
    _require(getattr(row, "slot", None) == slot, "selected replay row identity")
    values = getattr(row, "values", None)
    _require(isinstance(values, tuple) and len(values) == 4,
             "selected replay row shape")
    return replay_complex(
        z=frozen_input(slot),
        e0_text=(values[0].text, values[1].text),
        e8_text=(values[2].text, values[3].text))


def replay_sidecar(sidecar):
    """Replay all rows of an already validated, structurally compatible sidecar."""
    rows, bounds = _preflight(sidecar)
    maxima = {name: None for name in ("E0", "E8", "I8", "A8", "R")}
    quarter = Decimal("0.25")
    try:
        for slot, row in enumerate(rows):
            z = frozen_input(slot)
            _require(_magnitude(z[0]) + _magnitude(z[1]) < 1,
                     "frozen input one-norm invariant")
            e0 = tuple(_canonical_decimal(value.text) for value in row.values[:2])
            e8 = tuple(_canonical_decimal(value.text) for value in row.values[2:])

            # The fast path proves the exact Fraction disk guard by triangle
            # inequality without materializing enormous denominators for tiny
            # canonical residuals. Only an inconclusive row takes the exact
            # one-row fallback.
            if not (_magnitude(e0[0]) <= quarter and
                    _magnitude(e0[1]) <= quarter):
                e0_exact = _complex_fraction(e0)
                exact_fresh = z[0] + e0_exact[0], z[1] + e0_exact[1]
                if not _within_disk(exact_fresh, Fraction(3, 2)):
                    raise ReplayUnresolved(
                        "exact reconstructed fresh value exceeds 3/2 disk")

            z_decimal = _decimal_from_exact(z[0]), _decimal_from_exact(z[1])
            result = _scalar_kernel(z_decimal, e0, e8)
            vectors = {"E0": e0, "E8": e8, "I8": result.i8,
                       "A8": result.a8, "R": result.identity}
            for name in maxima:
                _update_maximum(maxima, name, slot, vectors)
    except ReplayError:
        raise
    except DecimalException as error:
        raise ReplayError("invalid Decimal replay arithmetic") from error

    return ReplayResult(
        ROW_COUNT, bounds, _finish_metric(maxima["E0"]),
        _finish_metric(maxima["E8"]), _finish_metric(maxima["I8"]),
        _finish_metric(maxima["A8"]), _finish_metric(maxima["R"]))
