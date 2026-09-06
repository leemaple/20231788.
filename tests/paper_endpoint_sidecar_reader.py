"""Strict standalone reader for the v1-r1 endpoint canonical TSV.

This validates the sidecar itself. It does not perform Decimal replay, reconcile
the primary CTest log, compress/publish evidence, or validate status JSON. The
caller/final packer owns a trusted publication directory and parent-path race
protection; this bounded reader rejects a direct leaf symlink.
"""
from dataclasses import dataclass
from fractions import Fraction
from pathlib import Path
import re
from typing import Mapping, Tuple


MAX_BYTES = 16 * 1024 * 1024
MAX_LINE_BYTES = 32768
ROW_COUNT = 16384
# Each integer is confined by the already-enforced physical line envelope. The
# field cap is therefore derived from that envelope, not from an extra schema
# limit that could reject a legal rational.
INTEGER_DIGITS = MAX_LINE_BYTES - 1
HEADER = b"#fs-residual-endpoint-01.v1-r1"
ROW_HEADER = b"slot\tE0.real\tE0.imag\tE8.real\tE8.imag"
BASELINE = "9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e"
PRODUCTION = "b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89"
OPENFHE = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
ZERO = "+0." + "0" * 109 + "e+00000"
_UINT = re.compile(r"0|[1-9][0-9]*", re.ASCII)
_HEX40 = re.compile(r"[0-9a-f]{40}", re.ASCII)
_DECIMAL = re.compile(r"[+-][1-9]\.[0-9]{109}e[+-][0-9]{5}", re.ASCII)

META_KEYS = (
    "scope", "source_commit", "baseline_tested_source", "production_source",
    "openfhe_pin", "host", "github_run_id", "github_run_attempt", "test_name",
    "chain_count", "n", "m", "slots", "gap", "input_formula",
    "primary_precision_bits", "check_precision_bits", "significant_digits",
    "scale0_numerator", "scale0_denominator", "scale8_numerator",
    "scale8_denominator", "coefficient_l1_fresh", "coefficient_l1_terminal",
    "fresh_max_l1_numerator", "fresh_max_l1_denominator",
    "terminal_max_l1_numerator", "terminal_max_l1_denominator", "model",
    "assurance", "boost_version", "root_policy", "norm", "observer_tolerance",
    "estimator_ceiling", "original_error_gate", "rounding", "row_count",
    "check_count", "numeric_gate_failures", "E80_disposition", "A_disposition",
    "observer_disposition",
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
HORNER_ANCHORS = frozenset((0, 1, 256, 257, 512, 513, 768, 769, 1023, 16383))


class SidecarError(ValueError):
    pass


def _require(condition, message):
    if not condition:
        raise SidecarError(message)


@dataclass(frozen=True)
class CanonicalValue:
    text: str
    signed_significand: int
    decimal_exponent: int


@dataclass(frozen=True)
class CheckReceipt:
    check_id: str
    distance: Fraction
    allowance: Fraction
    argmax_slot: int
    argmax_component: str


@dataclass(frozen=True)
class SlotRow:
    slot: int
    values: Tuple[CanonicalValue, CanonicalValue, CanonicalValue, CanonicalValue]


@dataclass(frozen=True)
class Sidecar:
    meta: Mapping[str, object]
    checks: Tuple[CheckReceipt, ...]
    rows: Tuple[SlotRow, ...]


def _uint(text, *, positive=False):
    _require(isinstance(text, str) and 0 < len(text) <= INTEGER_DIGITS,
             "integer length")
    _require(_UINT.fullmatch(text) is not None, "integer grammar")
    value = 0
    for offset in range(0, len(text), 9):
        chunk = text[offset:offset + 9]
        value = value * 10 ** len(chunk) + int(chunk)
    _require(not positive or value > 0, "positive integer required")
    return value


def _rational(numerator, denominator):
    n, d = _uint(numerator), _uint(denominator, positive=True)
    value = Fraction(n, d)
    _require(value.numerator == n and value.denominator == d, "unreduced rational")
    return value


def _canonical(text):
    _require(len(text) == 119 and text.isascii(), "canonical decimal byte grammar")
    if text == ZERO:
        return CanonicalValue(text, 0, 0)
    _require(_DECIMAL.fullmatch(text) is not None and not text.endswith("e-00000"),
             "canonical decimal grammar")
    significand = int(text[1] + text[3:112])
    if text[0] == "-":
        significand = -significand
    return CanonicalValue(text, significand, int(text[113:]))


def _pow2(exponent):
    return Fraction(1 << exponent, 1) if exponent >= 0 else Fraction(1, 1 << -exponent)


def _scale8():
    q = (1125899904679937, 1125899903827969, 1152921504598720513,
         1152921504597016577, 1152921504595968001, 1152921504595640321,
         1152921504593412097, 1152921504592822273, 1152921504592429057,
         1152921504589938689, 1099510054913)
    result = Fraction(1 << 100)
    for index in range(1, 9):
        result = result * result / (q[-1] * q[10 - index])
    return result


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


def _endpoint(bits, fresh_k, terminal_k):
    u = _pow2(-bits)
    df, dt = _pow2(12) * u * fresh_k, _pow2(12) * u * terminal_k
    p, r = _pow2(270) * u, _pow2(264) * u
    q = p + _pow2(263) * df
    e0, e8 = df + r, dt + p + r
    i8, a8 = q + p + r, dt + q + r
    return {"D_fresh": df, "D_terminal": dt,
            "H_fresh": _pow2(24) * u * fresh_k,
            "H_terminal": _pow2(24) * u * terminal_k,
            "E0": e0, "E8": e8, "I8": i8, "A8": a8,
            "identity": e8 + i8 + a8 + 2 * r}


def _allowances(fresh_k, terminal_k):
    a512, a768 = _endpoint(512, fresh_k, terminal_k), _endpoint(768, fresh_k, terminal_k)
    result = {}
    for name, k in (("constant", Fraction(1)), ("x", Fraction(1)),
                    ("xNminus1", Fraction(1)), ("sparse", Fraction(8))):
        for bits in (512, 768):
            result[f"control.{name}.{bits}"] = (_pow2(12 - bits) + _pow2(10 - 768)) * k
    result.update({
        "fresh.cross": a512["D_fresh"] + a768["D_fresh"],
        "terminal.cross": a512["D_terminal"] + a768["D_terminal"],
        "fresh.horner.512": a512["D_fresh"] + a512["H_fresh"],
        "fresh.horner.768": a768["D_fresh"] + a512["H_fresh"],
        "terminal.horner.512": a512["D_terminal"] + a512["H_terminal"],
        "terminal.horner.768": a768["D_terminal"] + a512["H_terminal"],
        "fresh.producer.512": a512["D_fresh"] + _pow2(-300),
        "fresh.producer.768": a768["D_fresh"] + _pow2(-300),
        "terminal.producer.512": a512["D_terminal"] + _pow2(-300),
        "terminal.producer.768": a768["D_terminal"] + _pow2(-300),
    })
    for key in ("E0", "E8", "I8", "A8"):
        result[f"residual.{key}.cross"] = a512[key] + a768[key]
    result["identity.512"], result["identity.768"] = a512["identity"], a768["identity"]
    return result


def _read_bounded(path):
    path = Path(path)
    _require(not path.is_symlink() and path.is_file(), "sidecar must be a regular non-symlink")
    size = path.stat().st_size
    _require(0 < size <= MAX_BYTES, "sidecar byte limit")
    with path.open("rb") as stream:
        data = stream.read(MAX_BYTES + 1)
        trailing = stream.read(1)
    _require(len(data) <= MAX_BYTES and not trailing, "sidecar byte limit")
    _require(len(data) == size, "sidecar changed while reading")
    _require(data.endswith(b"\n") and b"\r" not in data and b"\0" not in data,
             "ASCII LF envelope")
    _require(data.isascii() and not data.startswith(b"\xef\xbb\xbf"), "ASCII sidecar required")
    start = 0
    while start < len(data):
        newline = data.find(b"\n", start, min(len(data), start + MAX_LINE_BYTES))
        _require(newline >= 0 and newline > start, "blank or oversized line")
        start = newline + 1
    return data


def read_sidecar(path, *, expected_scope, expected_source_commit, expected_host,
                 expected_run_id, expected_run_attempt):
    _require(expected_scope in ("synthetic", "live-single-chain"), "invalid expected scope")
    _require(isinstance(expected_source_commit, str) and
             _HEX40.fullmatch(expected_source_commit) is not None,
             "expected source identity")
    _require(expected_host in ("linux", "windows"), "expected host identity")
    expected_run_id = str(_uint(str(expected_run_id), positive=True))
    expected_run_attempt = str(_uint(str(expected_run_attempt), positive=True))
    lines = _read_bounded(path).split(b"\n")[:-1]
    expected_lines = 1 + len(META_KEYS) + len(CHECK_IDS) + 1 + ROW_COUNT
    _require(len(lines) == expected_lines and lines[0] == HEADER, "sidecar line count/header")

    raw_meta = {}
    for offset, key in enumerate(META_KEYS, 1):
        fields = lines[offset].decode("ascii").split("\t")
        _require(len(fields) == 3 and fields[0] == "meta" and fields[1] == key,
                 "ordered metadata schema")
        raw_meta[key] = fields[2]

    fixed = {
        "scope": expected_scope, "source_commit": expected_source_commit,
        "baseline_tested_source": BASELINE, "production_source": PRODUCTION,
        "openfhe_pin": OPENFHE, "host": expected_host,
        "github_run_id": expected_run_id, "github_run_attempt": expected_run_attempt,
        "test_name": "paper_full_eight_square_contract", "chain_count": "1",
        "n": "32768", "m": "65536", "slots": "16384", "gap": "1",
        "input_formula": "frozen-four-phase-exact-dyadic-v1",
        "primary_precision_bits": "768", "check_precision_bits": "512",
        "significant_digits": "110", "model": "conditional-binary-nearest-direct-trig8u-v1",
        "assurance": "CONDITIONAL", "root_policy": "direct-own-precision-v1",
        "norm": "max-real-imag-component", "observer_tolerance": "2^-120",
        "estimator_ceiling": "2^-128", "original_error_gate": "2^-80",
        "rounding": "decimal-nearest-ties-even", "row_count": "16384",
        "check_count": "24", "A_disposition": "NOT_ADOPTED",
        "observer_disposition": "PASS",
    }
    for key, value in fixed.items():
        _require(raw_meta[key] == value, "metadata identity/value mismatch: " + key)
    _require(_HEX40.fullmatch(raw_meta["source_commit"]) is not None, "source commit grammar")

    integer_keys = ("chain_count", "n", "m", "slots", "gap",
                    "primary_precision_bits", "check_precision_bits", "significant_digits",
                    "row_count", "check_count",
                    "scale0_numerator", "scale0_denominator", "scale8_numerator",
                    "scale8_denominator", "coefficient_l1_fresh", "coefficient_l1_terminal",
                    "fresh_max_l1_numerator", "fresh_max_l1_denominator",
                    "terminal_max_l1_numerator", "terminal_max_l1_denominator",
                    "boost_version", "numeric_gate_failures")
    values = {key: _uint(raw_meta[key], positive=key in {
        "scale0_numerator", "scale0_denominator", "scale8_numerator", "scale8_denominator",
        "fresh_max_l1_denominator", "terminal_max_l1_denominator", "boost_version"})
              for key in integer_keys}
    scale0 = _rational(raw_meta["scale0_numerator"], raw_meta["scale0_denominator"])
    scale8 = _rational(raw_meta["scale8_numerator"], raw_meta["scale8_denominator"])
    _require(scale0 == Fraction(1 << 100) and scale8 == _scale8(), "frozen scale mismatch")
    fresh_max = _rational(raw_meta["fresh_max_l1_numerator"], raw_meta["fresh_max_l1_denominator"])
    terminal_max = _rational(raw_meta["terminal_max_l1_numerator"], raw_meta["terminal_max_l1_denominator"])
    _require(fresh_max <= Fraction(5, 4) and terminal_max <= Fraction(5, 4),
             "endpoint radius guard")
    _require(raw_meta["E80_disposition"] == ("PASS" if values["numeric_gate_failures"] == 0 else "FAIL"),
             "numeric failure/E80 mismatch")

    fresh_k = _ceiling_power_of_two(values["coefficient_l1_fresh"], scale0)
    terminal_k = _ceiling_power_of_two(values["coefficient_l1_terminal"], scale8)
    allowances = _allowances(fresh_k, terminal_k)
    checks = []
    check_start = 1 + len(META_KEYS)
    for index, check_id in enumerate(CHECK_IDS):
        fields = lines[check_start + index].decode("ascii").split("\t")
        _require(len(fields) == 9 and fields[:3] == ["check", check_id, "PASS"],
                 "ordered check schema")
        distance, allowance = _rational(fields[3], fields[4]), _rational(fields[5], fields[6])
        _require(allowance == allowances[check_id], "rederived allowance mismatch")
        _require(allowance <= _pow2(-128), "estimator ceiling exceeded")
        producer = ".producer." in check_id
        decision = "FAIL" if distance > _pow2(-120) else (
            "FAIL" if not producer and distance > allowance else
            ("PASS" if distance + allowance <= _pow2(-120) else "UNRESOLVED"))
        _require(decision == "PASS", "check classification is not PASS")
        slot = _uint(fields[7])
        _require(slot < ROW_COUNT and fields[8] in ("real", "imag"), "check argmax")
        if ".horner." in check_id:
            _require(slot in HORNER_ANCHORS, "Horner argmax is not a frozen anchor")
        if distance == 0:
            _require(slot == 0 and fields[8] == "real", "zero-distance argmax tie policy")
        checks.append(CheckReceipt(check_id, distance, allowance, slot, fields[8]))

    row_header_index = check_start + len(CHECK_IDS)
    _require(lines[row_header_index] == ROW_HEADER, "slot header")
    rows = []
    for slot in range(ROW_COUNT):
        fields = lines[row_header_index + 1 + slot].decode("ascii").split("\t")
        _require(len(fields) == 5 and _uint(fields[0]) == slot and fields[0] == str(slot),
                 "ordered slot row")
        rows.append(SlotRow(slot, tuple(_canonical(value) for value in fields[1:])))

    meta = dict(raw_meta)
    meta.update(values)
    meta["scale0"], meta["scale8"] = scale0, scale8
    meta["fresh_max_l1"], meta["terminal_max_l1"] = fresh_max, terminal_max
    return Sidecar(meta, tuple(checks), tuple(rows))
