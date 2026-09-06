"""Strict, bounded reader for the primary CTest 61 endpoint evidence stream."""

from dataclasses import dataclass
from fractions import Fraction
import math
import re


MAX_LOG_BYTES = 16 * 1024 * 1024
MAX_PRIMARY_LINE_BYTES = 32768
OPENFHE_PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
DIVISOR = 1099510054913
Q = (
    1125899904679937, 1125899903827969,
    1152921504598720513, 1152921504597016577,
    1152921504595968001, 1152921504595640321,
    1152921504593412097, 1152921504592822273,
    1152921504592429057, 1152921504589938689,
    1099510054913,
)
CHECK_IDS = (
    "control.constant.512", "control.constant.768", "control.x.512",
    "control.x.768", "control.xNminus1.512", "control.xNminus1.768",
    "control.sparse.512", "control.sparse.768", "fresh.cross", "terminal.cross",
    "fresh.horner.512", "fresh.horner.768", "terminal.horner.512",
    "terminal.horner.768", "fresh.producer.512", "fresh.producer.768",
    "terminal.producer.512", "terminal.producer.768", "residual.E0.cross",
    "residual.E8.cross", "residual.I8.cross", "residual.A8.cross",
    "identity.512", "identity.768",
)
MAX_IDS = ("E0", "E8", "I8", "A8")
NUMERIC_GATE_LABELS = (
    "fresh full-slot 2^-80 gate",
    "fresh retained sub-binary64 witness",
    "fresh independent anchor 2^-80 gate",
    "round_1 independent anchor 2^-80 gate",
    "round_2 independent anchor 2^-80 gate",
    "round_3 independent anchor 2^-80 gate",
    "round_4 independent anchor 2^-80 gate",
    "round_5 independent anchor 2^-80 gate",
    "round_6 independent anchor 2^-80 gate",
    "round_7 independent anchor 2^-80 gate",
    "round_8 independent anchor 2^-80 gate",
    "final full-slot 2^-80 gate",
    "final retained sub-binary64 witness",
    "final independent anchor 2^-80 gate",
)
HORNER_SLOTS = frozenset((0, 1, 256, 257, 512, 513, 768, 769, 1023, 16383))
UNRESOLVED_REASONS = frozenset(("MODEL_UNSUPPORTED", "ESTIMATOR_CEILING", "CONDITIONING"))
FATAL_REASONS = frozenset(("NONFINITE", "INTEGRITY", "IDENTITY", "FORMAT", "REPLAY", "IO_ERROR"))
TWO_NEG_120 = Fraction(1, 1 << 120)
TWO_NEG_128 = Fraction(1, 1 << 128)
_SHA = re.compile(r"[0-9a-f]{40}\Z")
_UINT = re.compile(r"(?:0|[1-9][0-9]*)\Z")
_CANONICAL = re.compile(r"([+-])([1-9])\.([0-9]{109})e([+-])([0-9]{5})\Z")
ZERO = "+0." + "0" * 109 + "e+00000"


@dataclass(frozen=True)
class PrimaryIdentity:
    source_commit: str
    host: str
    github_run_id: str
    github_run_attempt: str


@dataclass(frozen=True)
class PrimaryObservation:
    numeric_gate_failures: int | None
    e80_disposition: str
    boost_version: int | None


EMPTY_OBSERVATION = PrimaryObservation(None, "NOT_OBSERVED", None)


@dataclass(frozen=True)
class FailureRecord:
    reason: str
    detail: str
    line_number: int
    typed: bool = True


@dataclass(frozen=True)
class ScaleReceipt:
    operation: int
    numerator: int
    denominator: int


@dataclass(frozen=True)
class CheckReceipt:
    check_id: str
    distance: Fraction
    allowance: Fraction
    argmax_slot: int
    argmax_component: str


@dataclass(frozen=True)
class CanonicalValue:
    text: str
    value: Fraction
    quantum: Fraction


@dataclass(frozen=True)
class MaximumRecord:
    residual_id: str
    magnitude: CanonicalValue
    magnitude_exact: Fraction
    magnitude_quantum: Fraction
    allowance: Fraction
    interval_lower: Fraction
    interval_upper: Fraction
    argmax_slot: int
    argmax_component: str
    tuple_values: tuple


@dataclass(frozen=True)
class EndpointPrimary:
    scope: str
    boost_version: int
    scales: tuple
    checks: tuple
    maxima: tuple


@dataclass(frozen=True)
class PrimaryLog:
    evidence_state: str
    reason: str
    identity: PrimaryIdentity
    ctest_exit_code: int
    timed_out: bool
    legacy_begin_seen: bool
    cleanup_seen: bool
    numeric_gate_labels: tuple
    numeric_gate_failures: object
    e80_disposition: str
    legacy_scales: tuple
    endpoint: object
    original_result: object
    original_reason: object
    first_failure: object


class PrimaryLogError(ValueError):
    def __init__(self, reason, detail, first_failure=None, observation=None):
        super().__init__(f"{reason}: {detail}")
        self.reason = reason
        self.detail = detail
        self.first_failure = first_failure
        self.observation = EMPTY_OBSERVATION if observation is None else observation


def _fail(reason, detail, first_failure=None, observation=None):
    raise PrimaryLogError(reason, detail, first_failure, observation)


def _parse_uint(text, name, *, positive=False, maximum=None, first_failure=None,
                observation=None):
    if not _UINT.fullmatch(text):
        _fail("FORMAT", f"{name} is not a canonical unsigned integer", first_failure,
              observation)
    value = 0
    for start in range(0, len(text), 9):
        chunk = text[start:start + 9]
        value = value * (10 ** len(chunk)) + int(chunk)
    if positive and value == 0:
        _fail("FORMAT", f"{name} must be positive", first_failure, observation)
    if maximum is not None and value > maximum:
        _fail("FORMAT", f"{name} exceeds {maximum}", first_failure, observation)
    return value


def _fields(line, record, names, first_failure=None, observation=None):
    pieces = line.split("\t")
    if pieces[0] != record or len(pieces) != len(names) + 1:
        _fail("FORMAT", f"malformed {record} field count", first_failure, observation)
    values = []
    for piece, name in zip(pieces[1:], names):
        prefix = name + "="
        if not piece.startswith(prefix) or len(piece) == len(prefix):
            _fail("FORMAT", f"malformed {record} field {name}", first_failure,
                  observation)
        values.append(piece[len(prefix):])
    return values


def _rational(n_text, d_text, name, first_failure=None, observation=None):
    n = _parse_uint(n_text, name + " numerator", first_failure=first_failure,
                    observation=observation)
    d = _parse_uint(d_text, name + " denominator", positive=True,
                    first_failure=first_failure, observation=observation)
    if math.gcd(n, d) != 1:
        _fail("INTEGRITY", f"{name} is not reduced", first_failure, observation)
    return Fraction(n, d)


def _require_dyadic(value, name, first_failure=None, observation=None):
    if value.denominator & (value.denominator - 1):
        _fail("INTEGRITY", f"{name} must be an exact represented dyadic", first_failure,
              observation)


def _power10(exponent):
    if exponent >= 0:
        return Fraction(10 ** exponent, 1)
    return Fraction(1, 10 ** (-exponent))


def _canonical(text, name, first_failure=None, observation=None):
    if text == ZERO:
        return CanonicalValue(text, Fraction(0), Fraction(0))
    match = _CANONICAL.fullmatch(text)
    if not match or (match.group(4) == "-" and match.group(5) == "00000"):
        _fail("FORMAT", f"{name} is not canonical decimal", first_failure, observation)
    exponent = int(match.group(5)) * (-1 if match.group(4) == "-" else 1)
    significand = int(match.group(2) + match.group(3))
    if match.group(1) == "-":
        significand = -significand
    value = Fraction(significand) * _power10(exponent - 109)
    quantum = Fraction(5) * _power10(exponent - 110)
    return CanonicalValue(text, value, quantum)


def _decimal_exponent(value):
    assert value > 0
    exponent = int((value.numerator.bit_length() - value.denominator.bit_length()) * 0.30103)
    while value < _power10(exponent):
        exponent -= 1
    while value >= _power10(exponent + 1):
        exponent += 1
    return exponent


def _canonical_fraction(value, first_failure=None, observation=None):
    if value == 0:
        return ZERO
    sign = "-" if value < 0 else "+"
    value = abs(value)
    exponent = _decimal_exponent(value)
    scaled = value * _power10(109 - exponent)
    quotient, remainder = divmod(scaled.numerator, scaled.denominator)
    twice = remainder * 2
    if twice > scaled.denominator or (twice == scaled.denominator and quotient % 2):
        quotient += 1
    if quotient == 10 ** 110:
        quotient //= 10
        exponent += 1
    if not -99999 <= exponent <= 99999:
        _fail("FORMAT", "canonical decimal exponent is out of range", first_failure,
              observation)
    digits = str(quotient)
    if len(digits) != 110:
        _fail("INTEGRITY", "canonical decimal significand width is invalid", first_failure,
              observation)
    exp_sign = "+" if exponent >= 0 else "-"
    return f"{sign}{digits[0]}.{digits[1:]}e{exp_sign}{abs(exponent):05d}"


def _exact_scales():
    result = [(1 << 100, 1)]
    for operation in range(1, 9):
        prior_n, prior_d = result[-1]
        numerator = prior_n * prior_n
        denominator = prior_d * prior_d * DIVISOR * Q[10 - operation]
        common = math.gcd(numerator, denominator)
        result.append((numerator // common, denominator // common))
    return tuple(result)


def _power_two(exponent):
    return Fraction(1 << exponent, 1) if exponent >= 0 else Fraction(1, 1 << -exponent)


def _is_power_two(value):
    if value == 0:
        return True
    return (value.numerator & (value.numerator - 1) == 0 and
            value.denominator & (value.denominator - 1) == 0)


def _validate_primary_allowance_consistency(checks, maxima, first_failure,
                                            observation=None):
    power768 = _power_two(270 - 768)
    subtraction768 = _power_two(264 - 768)
    residual768 = tuple(maximum.allowance for maximum in maxima)
    fresh768 = residual768[0] - subtraction768
    terminal768 = residual768[1] - power768 - subtraction768
    propagated768 = residual768[2] - power768 - subtraction768
    if min(fresh768, terminal768, propagated768) < 0:
        _fail("INTEGRITY", "MAX allowances cannot recover a nonnegative allowance model",
              first_failure, observation)
    if not _is_power_two(fresh768) or not _is_power_two(terminal768):
        _fail("INTEGRITY", "MAX allowances do not recover power-of-two norm bounds",
              first_failure, observation)
    if propagated768 != power768 + (1 << 263) * fresh768:
        _fail("INTEGRITY", "MAX allowances disagree with propagated allowance model",
              first_failure, observation)
    if residual768[3] != terminal768 + propagated768 + subtraction768:
        _fail("INTEGRITY", "MAX A8 allowance disagrees with recovered allowance model",
              first_failure, observation)

    factor = 1 << 256
    fresh512 = fresh768 * factor
    terminal512 = terminal768 * factor
    power512 = power768 * factor
    subtraction512 = subtraction768 * factor
    propagated512 = power512 + (1 << 263) * fresh512
    residual512 = (
        fresh512 + subtraction512,
        terminal512 + power512 + subtraction512,
        propagated512 + power512 + subtraction512,
        terminal512 + propagated512 + subtraction512,
    )
    identity512 = sum(residual512[1:]) + 2 * subtraction512
    identity768 = sum(residual768[1:]) + 2 * subtraction768

    expected = []
    for exponent in (0, 0, 0, 3):
        direct = _power_two(exponent + 10 - 768)
        expected.extend((_power_two(exponent + 12 - 512) + direct,
                         _power_two(exponent + 12 - 768) + direct))
    expected.extend((fresh512 + fresh768, terminal512 + terminal768,
                     fresh512 + fresh512 * (1 << 12),
                     fresh768 + fresh512 * (1 << 12),
                     terminal512 + terminal512 * (1 << 12),
                     terminal768 + terminal512 * (1 << 12)))
    transport = _power_two(-300)
    expected.extend((fresh512 + transport, fresh768 + transport,
                     terminal512 + transport, terminal768 + transport))
    expected.extend(left + right for left, right in zip(residual512, residual768))
    expected.extend((identity512, identity768))
    if len(expected) != len(CHECK_IDS):
        _fail("INTEGRITY", "internal recovered allowance count", first_failure,
              observation)
    for check, allowance in zip(checks, expected):
        if check.allowance != allowance:
            _fail("INTEGRITY", f"{check.check_id} allowance model mismatch", first_failure,
                  observation)


def _validate_identity(identity):
    if type(identity) is not PrimaryIdentity:
        _fail("FORMAT", "expected_identity must be PrimaryIdentity")
    if not _SHA.fullmatch(identity.source_commit):
        _fail("FORMAT", "expected source_commit must be a lowercase 40-hex SHA")
    if identity.host not in ("linux", "windows"):
        _fail("FORMAT", "expected host must be linux or windows")
    _parse_uint(identity.github_run_id, "expected github_run_id", positive=True)
    _parse_uint(identity.github_run_attempt, "expected github_run_attempt", positive=True)


def _parse_legacy_begin(line, expected, first_failure):
    exact = (
        "BEGIN test=paper_full_eight_square_contract source=" + expected.source_commit +
        " openfhe_pin=" + OPENFHE_PIN +
        " native=64 backend=4 N=32768 M=65536 slots=16384 gap=1 h=128 nominal=50"
        " P=1152921504606584833 P_root=4443670208963 chain_count=1"
    )
    if line != exact:
        _fail("IDENTITY", "legacy BEGIN is not the exact frozen identity", first_failure)


def _parse_receipt(line, expected_operation, scales, first_failure):
    pattern = re.compile(
        r"RECEIPT operation=([^ ]+) family=([^ ]+) local_level=([^ ]+) towers=([^ ]+) "
        r"recorded_exp2=([^ ]+) degree=([^ ]+) exact_n=([^ ]+) exact_d=([^ ]+) terminal=([^ ]+)\Z")
    match = pattern.fullmatch(line)
    if not match:
        _fail("FORMAT", "malformed legacy RECEIPT", first_failure)
    operation = _parse_uint(match.group(1), "receipt operation", maximum=8, first_failure=first_failure)
    if operation != expected_operation:
        _fail("REPLAY", "legacy RECEIPT is duplicate or reordered", first_failure)
    family = _parse_uint(match.group(2), "receipt family", maximum=7, first_failure=first_failure)
    local = _parse_uint(match.group(3), "receipt local_level", first_failure=first_failure)
    towers = _parse_uint(match.group(4), "receipt towers", first_failure=first_failure)
    exp2 = _parse_uint(match.group(5), "receipt recorded_exp2", first_failure=first_failure)
    degree = _parse_uint(match.group(6), "receipt degree", first_failure=first_failure)
    terminal = _parse_uint(match.group(9), "receipt terminal", maximum=1, first_failure=first_failure)
    expected_family = 7 if operation == 8 else operation
    if (family, local, towers, exp2, degree, terminal) != (
            expected_family, 2 if operation == 8 else 1, 10 - operation, 100, 2,
            1 if operation == 8 else 0):
        _fail("INTEGRITY", f"legacy RECEIPT {operation} metadata mismatch", first_failure)
    numerator = _parse_uint(match.group(7), "receipt exact_n", positive=True, first_failure=first_failure)
    denominator = _parse_uint(match.group(8), "receipt exact_d", positive=True, first_failure=first_failure)
    if math.gcd(numerator, denominator) != 1 or (numerator, denominator) != scales[operation]:
        _fail("INTEGRITY", f"legacy RECEIPT {operation} scale mismatch", first_failure)
    return ScaleReceipt(operation, numerator, denominator)


def _parse_check(line, expected_id, first_failure, observation=None):
    names = ("id", "result", "distance_num", "distance_den", "allowance_num",
             "allowance_den", "argmax_slot", "argmax_component")
    check_id, result, dn, dd, an, ad, slot_text, component = _fields(
        line, "FS_ENDPOINT_CHECK", names, first_failure, observation)
    if check_id != expected_id:
        _fail("REPLAY", "endpoint CHECK is duplicate, foreign, or reordered", first_failure,
              observation)
    if result != "PASS":
        _fail("INTEGRITY", f"endpoint CHECK {check_id} is not PASS", first_failure,
              observation)
    distance = _rational(dn, dd, "check distance", first_failure, observation)
    _require_dyadic(distance, "check distance", first_failure, observation)
    allowance = _rational(an, ad, "check allowance", first_failure, observation)
    slot = _parse_uint(slot_text, "check argmax_slot", maximum=16383,
                       first_failure=first_failure, observation=observation)
    if component not in ("real", "imag"):
        _fail("FORMAT", "check argmax_component is invalid", first_failure, observation)
    if distance == 0 and (slot != 0 or component != "real"):
        _fail("INTEGRITY", "zero check distance must select slot 0 real", first_failure,
              observation)
    if ".horner." in check_id and slot not in HORNER_SLOTS:
        _fail("INTEGRITY", "Horner check selected a non-probe slot", first_failure,
              observation)
    if distance > TWO_NEG_120 or allowance > TWO_NEG_128 or distance + allowance > TWO_NEG_120:
        _fail("INTEGRITY", f"endpoint CHECK {check_id} exceeds its classifier",
              first_failure, observation)
    if ".producer." not in check_id and distance > allowance:
        _fail("INTEGRITY", f"endpoint CHECK {check_id} exceeds its allowance", first_failure,
              observation)
    return CheckReceipt(check_id, distance, allowance, slot, component)


def _parse_maximum(line, expected_id, first_failure, observation=None):
    names = [
        "id", "magnitude", "magnitude_exact_num", "magnitude_exact_den",
        "magnitude_quantum_num", "magnitude_quantum_den", "allowance_num",
        "allowance_den", "interval_lower_num", "interval_lower_den",
        "interval_upper_num", "interval_upper_den", "argmax_slot", "argmax_component",
    ]
    for residual in MAX_IDS:
        for component in ("real", "imag"):
            field = residual + "." + component
            names.extend((field, field + "_q_num", field + "_q_den"))
    values = _fields(line, "FS_ENDPOINT_MAX", names, first_failure, observation)
    if values[0] != expected_id:
        _fail("REPLAY", "endpoint MAX is duplicate, foreign, or reordered", first_failure,
              observation)
    magnitude = _canonical(values[1], "maximum magnitude", first_failure, observation)
    if magnitude.value < 0:
        _fail("INTEGRITY", "maximum magnitude is negative", first_failure, observation)
    magnitude_exact = _rational(values[2], values[3], "magnitude_exact", first_failure,
                                observation)
    _require_dyadic(magnitude_exact, "magnitude_exact", first_failure, observation)
    magnitude_quantum = _rational(values[4], values[5], "magnitude_quantum",
                                   first_failure, observation)
    allowance = _rational(values[6], values[7], "maximum allowance", first_failure,
                          observation)
    lower = _rational(values[8], values[9], "interval_lower", first_failure, observation)
    upper = _rational(values[10], values[11], "interval_upper", first_failure, observation)
    slot = _parse_uint(values[12], "maximum argmax_slot", maximum=16383,
                       first_failure=first_failure, observation=observation)
    component = values[13]
    if component not in ("real", "imag"):
        _fail("FORMAT", "maximum argmax_component is invalid", first_failure, observation)
    if magnitude.text != _canonical_fraction(magnitude_exact, first_failure, observation):
        _fail("INTEGRITY", "maximum decimal does not round from magnitude_exact",
              first_failure, observation)
    if magnitude_quantum != magnitude.quantum:
        _fail("INTEGRITY", "maximum magnitude quantum mismatch", first_failure, observation)
    if allowance > TWO_NEG_128:
        _fail("INTEGRITY", "maximum allowance exceeds 2^-128", first_failure, observation)
    if lower != max(Fraction(0), magnitude_exact - allowance) or upper != magnitude_exact + allowance:
        _fail("INTEGRITY", "maximum interval mismatch", first_failure, observation)
    tuple_values = []
    offset = 14
    for residual in MAX_IDS:
        for tuple_component in ("real", "imag"):
            canonical = _canonical(values[offset], residual + "." + tuple_component,
                                   first_failure, observation)
            quantum = _rational(values[offset + 1], values[offset + 2],
                                residual + "." + tuple_component + " quantum", first_failure,
                                observation)
            if quantum != canonical.quantum:
                _fail("INTEGRITY", f"{residual}.{tuple_component} quantum mismatch",
                      first_failure, observation)
            tuple_values.append((residual, tuple_component, canonical))
            offset += 3
    selected = next(v for r, c, v in tuple_values if r == expected_id and c == component)
    if abs(selected.value) != magnitude.value:
        _fail("INTEGRITY", "selected tuple component does not match maximum magnitude",
              first_failure, observation)
    if magnitude.value == 0 and (slot != 0 or component != "real"):
        _fail("INTEGRITY", "zero maximum must select slot 0 real", first_failure,
              observation)
    return MaximumRecord(expected_id, magnitude, magnitude_exact, magnitude_quantum,
                         allowance, lower, upper, slot, component, tuple(tuple_values))


def parse_primary_log(log_bytes, expected_identity, *, ctest_exit_code, timed_out=False,
                      expected_scope="live-single-chain"):
    """Parse the bounded primary CTest 61 stream into immutable typed evidence."""
    _validate_identity(expected_identity)
    if type(log_bytes) is not bytes:
        _fail("FORMAT", "log_bytes must be exact bytes")
    if len(log_bytes) > MAX_LOG_BYTES:
        _fail("FORMAT", "primary log exceeds 16 MiB")
    if type(ctest_exit_code) is not int or not 0 <= ctest_exit_code <= 255:
        _fail("FORMAT", "ctest_exit_code must be an integer in 0..255")
    if type(timed_out) is not bool:
        _fail("FORMAT", "timed_out must be bool")
    if type(expected_scope) is not str or expected_scope not in ("live-single-chain", "synthetic"):
        _fail("FORMAT", "expected_scope must be live-single-chain or synthetic")

    scales = _exact_scales()
    begin = False
    cleanup = False
    legacy_scales = []
    labels = []
    declared_count = None
    endpoint_begin = False
    endpoint_scales = []
    checks = []
    maxima = []
    endpoint_complete = None
    original_result = None
    original_reason = None
    first_failure = None
    boost_version = None
    endpoint_scope = None
    observation = EMPTY_OBSERVATION

    physical_lines = log_bytes.split(b"\n")
    for line_number, body in enumerate(physical_lines, 1):
        raw = body + (b"\n" if line_number < len(physical_lines) else b"")
        if not raw.startswith(b"61: "):
            continue
        if len(raw) > MAX_PRIMARY_LINE_BYTES:
            _fail("FORMAT", "selected primary line exceeds 32768 bytes", first_failure,
                  observation)
        if expected_identity.host == "windows" and raw.endswith(b"\r\n"):
            payload = raw[4:-2]
        else:
            payload = raw[4:-1] if raw.endswith(b"\n") else raw[4:]
        if b"\r" in payload or b"\0" in payload:
            _fail("FORMAT", "selected primary line contains CR or NUL", first_failure,
                  observation)
        try:
            line = payload.decode("ascii")
        except UnicodeDecodeError:
            _fail("FORMAT", "selected primary line is not ASCII", first_failure,
                  observation)
        if line.startswith("FS_ENDPOINT_FAILURE"):
            if original_result is not None:
                _fail("REPLAY", "typed failure follows legacy COMPLETE", first_failure,
                      observation)
            match = re.fullmatch(r"FS_ENDPOINT_FAILURE reason=([^ ]+) detail=(.+)", line)
            if not match:
                _fail("FORMAT", "malformed FS_ENDPOINT_FAILURE", first_failure, observation)
            if first_failure is not None:
                _fail("REPLAY", "duplicate FS_ENDPOINT_FAILURE", first_failure, observation)
            reason = match.group(1)
            if reason not in UNRESOLVED_REASONS | FATAL_REASONS:
                _fail("FORMAT", "unknown FS_ENDPOINT_FAILURE reason", observation=observation)
            first_failure = FailureRecord(reason, match.group(2), line_number)
            continue
        if line.startswith("BEGIN "):
            if original_result is not None:
                _fail("REPLAY", "legacy BEGIN follows legacy COMPLETE", first_failure,
                      observation)
            if begin:
                _fail("REPLAY", "duplicate legacy BEGIN", first_failure, observation)
            _parse_legacy_begin(line, expected_identity, first_failure)
            begin = True
            continue
        if line.startswith("RECEIPT "):
            if not begin or cleanup or endpoint_begin or original_result is not None:
                _fail("REPLAY", "legacy RECEIPT is out of order", first_failure, observation)
            legacy_scales.append(_parse_receipt(line, len(legacy_scales), scales, first_failure))
            continue
        if line.startswith("OBS numeric_gate=FAIL label="):
            if not begin or cleanup or original_result is not None:
                _fail("REPLAY", "numeric gate observation is out of order", first_failure,
                      observation)
            label = line[len("OBS numeric_gate=FAIL label="):]
            if label not in NUMERIC_GATE_LABELS:
                _fail("FORMAT", "numeric gate label is not frozen", first_failure,
                      observation)
            position = NUMERIC_GATE_LABELS.index(label)
            if labels and position <= NUMERIC_GATE_LABELS.index(labels[-1]):
                _fail("REPLAY", "numeric gate label is duplicated or reordered", first_failure,
                      observation)
            labels.append(label)
            continue
        if line.startswith("OBS lifecycle=paper_owner_cleanup"):
            if cleanup or len(legacy_scales) != 9 or original_result is not None:
                _fail("REPLAY", "cleanup is duplicate or out of order", first_failure,
                      observation)
            if line != "OBS lifecycle=paper_owner_cleanup owned_absent=8 unrelated_unchanged=2 result=PASS":
                _fail("INTEGRITY", "owner cleanup record mismatch", first_failure,
                      observation)
            cleanup = True
            continue
        if line.startswith("OBS numeric_gate_failures="):
            if not cleanup or declared_count is not None or original_result is not None:
                _fail("REPLAY", "numeric failure count is duplicate or out of order",
                      first_failure, observation)
            declared_count = _parse_uint(line.split("=", 1)[1], "numeric_gate_failures",
                                         first_failure=first_failure)
            if declared_count != len(labels):
                _fail("INTEGRITY", "numeric failure count does not match observations",
                      first_failure, observation)
            continue
        if line.startswith("FS_ENDPOINT_BEGIN"):
            if endpoint_begin or declared_count is None or original_result is not None:
                _fail("REPLAY", "endpoint BEGIN is duplicate or out of order", first_failure,
                      observation)
            values = _fields(line, "FS_ENDPOINT_BEGIN",
                             ("schema", "scope", "source_commit", "host", "github_run_id",
                              "github_run_attempt", "boost_version"), first_failure)
            if values[0] != "fs-residual-endpoint-primary-v1-r1":
                _fail("FORMAT", "endpoint BEGIN schema mismatch", first_failure, observation)
            if values[1] != expected_scope:
                _fail("IDENTITY", "endpoint BEGIN scope mismatch", first_failure, observation)
            if values[2:6] != [expected_identity.source_commit, expected_identity.host,
                               expected_identity.github_run_id,
                               expected_identity.github_run_attempt]:
                _fail("IDENTITY", "endpoint BEGIN identity mismatch", first_failure, observation)
            boost_version = _parse_uint(values[6], "boost_version", positive=True,
                                        first_failure=first_failure)
            endpoint_scope = values[1]
            endpoint_begin = True
            observation = PrimaryObservation(None, "NOT_OBSERVED", boost_version)
            continue
        if line.startswith("FS_ENDPOINT_SCALE"):
            if not endpoint_begin or checks or maxima or endpoint_complete or original_result is not None:
                _fail("REPLAY", "endpoint SCALE is out of order", first_failure, observation)
            index, nt, dt = _fields(line, "FS_ENDPOINT_SCALE",
                                    ("index", "numerator", "denominator"), first_failure,
                                    observation)
            parsed_index = _parse_uint(index, "scale index", maximum=8,
                                       first_failure=first_failure, observation=observation)
            if parsed_index != len(endpoint_scales):
                _fail("REPLAY", "endpoint SCALE is duplicate or reordered", first_failure,
                      observation)
            numerator = _parse_uint(nt, "scale numerator", positive=True,
                                    first_failure=first_failure, observation=observation)
            denominator = _parse_uint(dt, "scale denominator", positive=True,
                                      first_failure=first_failure, observation=observation)
            if math.gcd(numerator, denominator) != 1 or (numerator, denominator) != scales[parsed_index]:
                _fail("INTEGRITY", f"endpoint SCALE {parsed_index} mismatch", first_failure,
                      observation)
            endpoint_scales.append(ScaleReceipt(parsed_index, numerator, denominator))
            continue
        if line.startswith("FS_ENDPOINT_CHECK"):
            if len(endpoint_scales) != 9 or maxima or endpoint_complete or original_result is not None:
                _fail("REPLAY", "endpoint CHECK is out of order", first_failure, observation)
            if len(checks) >= len(CHECK_IDS):
                _fail("REPLAY", "duplicate endpoint CHECK", first_failure, observation)
            checks.append(_parse_check(line, CHECK_IDS[len(checks)], first_failure,
                                       observation))
            continue
        if line.startswith("FS_ENDPOINT_MAX"):
            if len(checks) != 24 or endpoint_complete or original_result is not None:
                _fail("REPLAY", "endpoint MAX is out of order", first_failure, observation)
            if len(maxima) >= len(MAX_IDS):
                _fail("REPLAY", "duplicate endpoint MAX", first_failure, observation)
            maxima.append(_parse_maximum(line, MAX_IDS[len(maxima)], first_failure,
                                         observation))
            if len(maxima) == len(MAX_IDS):
                _validate_primary_allowance_consistency(checks, maxima, first_failure,
                                                        observation)
            continue
        if line.startswith("FS_ENDPOINT_COMPLETE"):
            if endpoint_complete is not None or len(maxima) != 4 or original_result is not None:
                _fail("REPLAY", "endpoint COMPLETE is duplicate or out of order", first_failure,
                      observation)
            values = _fields(line, "FS_ENDPOINT_COMPLETE",
                             ("result", "assurance", "row_count", "check_count",
                              "numeric_gate_failures", "E80_disposition", "A_disposition",
                              "owner_cleanup_confirmed"), first_failure, observation)
            if values[0:4] != ["PASS", "CONDITIONAL", "16384", "24"] or values[6:] != ["NOT_ADOPTED", "true"]:
                _fail("INTEGRITY", "endpoint COMPLETE fixed fields mismatch", first_failure,
                      observation)
            endpoint_count = _parse_uint(values[4], "endpoint numeric_gate_failures",
                                         first_failure=first_failure, observation=observation)
            disposition = "PASS" if endpoint_count == 0 else "FAIL"
            if endpoint_count != declared_count or values[5] != disposition:
                _fail("INTEGRITY", "endpoint E80 summary mismatch", first_failure,
                      observation)
            endpoint_complete = (endpoint_count, disposition)
            continue
        if line.startswith("COMPLETE test=paper_full_eight_square_contract"):
            if original_result is not None:
                _fail("REPLAY", "duplicate legacy COMPLETE", first_failure, observation)
            pass_line = (
                "COMPLETE test=paper_full_eight_square_contract result=PASS source=" +
                expected_identity.source_commit + " openfhe_pin=" + OPENFHE_PIN +
                " chain_count=1 squares=8 full_slots=16384 anchors=10 error_gate=2^-80"
                " codec_gate=2^-120 gaussian_global_guarantee=false")
            fail_prefix = (
                "COMPLETE test=paper_full_eight_square_contract result=FAIL source=" +
                expected_identity.source_commit + " openfhe_pin=" + OPENFHE_PIN + " reason=")
            if line == pass_line:
                original_result, original_reason = "PASS", None
            elif line.startswith(fail_prefix) and len(line) > len(fail_prefix):
                original_result, original_reason = "FAIL", line[len(fail_prefix):]
            else:
                _fail("IDENTITY", "legacy COMPLETE identity or grammar mismatch", first_failure,
                      observation)
            if declared_count is not None:
                expected_result = "PASS" if declared_count == 0 else "FAIL"
                expected_reason = None if declared_count == 0 else (
                    f"paper contract: accumulated numeric acceptance failures: {declared_count}")
                if original_result == expected_result and original_reason == expected_reason:
                    observation = PrimaryObservation(
                        declared_count, expected_result, observation.boost_version)
            continue
        if line.startswith("FS_ENDPOINT_"):
            _fail("FORMAT", "unknown FS_ENDPOINT primary record", first_failure, observation)

    legacy_numeric_complete = False
    if declared_count is not None and original_result is not None:
        expected_result = "PASS" if declared_count == 0 else "FAIL"
        expected_reason = None if declared_count == 0 else (
            f"paper contract: accumulated numeric acceptance failures: {declared_count}")
        legacy_numeric_complete = (original_result == expected_result and
                                   original_reason == expected_reason)
    record_set_complete = (
        begin and cleanup and len(legacy_scales) == 9 and declared_count is not None and
        endpoint_begin and len(endpoint_scales) == 9 and len(checks) == 24 and
        len(maxima) == 4 and endpoint_complete is not None and original_result is not None)
    endpoint = None
    if endpoint_begin:
        endpoint = EndpointPrimary(endpoint_scope, boost_version, tuple(endpoint_scales),
                                   tuple(checks), tuple(maxima))
    retained_count = declared_count if legacy_numeric_complete else None
    retained_e80 = ("PASS" if retained_count == 0 else "FAIL") if retained_count is not None else "NOT_OBSERVED"
    if record_set_complete:
        if first_failure is not None:
            _fail("INTEGRITY", "typed endpoint failure coexists with complete evidence",
                  first_failure, observation)
        if not legacy_numeric_complete:
            _fail("INTEGRITY", "legacy COMPLETE disagrees with numeric observations",
                  observation=observation)
        if timed_out:
            return PrimaryLog("FATAL", "TIMEOUT", expected_identity, ctest_exit_code, True,
                              True, True, tuple(labels), retained_count, retained_e80,
                              tuple(legacy_scales), endpoint, original_result, original_reason, None)
        if declared_count == 0 and ctest_exit_code != 0:
            return PrimaryLog("FATAL", "CTEST_FATAL", expected_identity, ctest_exit_code, False,
                              True, True, tuple(labels), retained_count, retained_e80,
                              tuple(legacy_scales), endpoint, original_result, original_reason, None)
        if declared_count > 0 and ctest_exit_code == 0:
            return PrimaryLog("FATAL", "INTEGRITY", expected_identity, ctest_exit_code, False,
                              True, True, tuple(labels), retained_count, retained_e80,
                              tuple(legacy_scales), endpoint, original_result, original_reason, None)
        return PrimaryLog("COMPLETE", "NONE", expected_identity, ctest_exit_code, False,
                          True, True, tuple(labels), declared_count, endpoint_complete[1],
                          tuple(legacy_scales), endpoint, original_result, original_reason, None)

    if first_failure is not None:
        reason = first_failure.reason
        state = "UNRESOLVED" if reason in UNRESOLVED_REASONS else "FATAL"
    elif timed_out:
        reason, state = "TIMEOUT", "FATAL"
    elif legacy_numeric_complete and declared_count == 0 and ctest_exit_code != 0:
        reason, state = "CTEST_FATAL", "FATAL"
    elif legacy_numeric_complete and declared_count > 0 and ctest_exit_code == 0:
        reason, state = "INTEGRITY", "FATAL"
    elif legacy_numeric_complete:
        reason, state = "NO_CANONICAL", "MISSING"
    elif original_result == "FAIL":
        reason, state = "CTEST_FATAL", "FATAL"
    else:
        reason, state = "NO_CANONICAL", "MISSING"
    return PrimaryLog(state, reason, expected_identity, ctest_exit_code, timed_out, begin, cleanup,
                      tuple(labels), retained_count, retained_e80, tuple(legacy_scales), endpoint,
                      original_result, original_reason, first_failure)
