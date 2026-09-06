"""Exact, bounded building blocks for FS-RESIDUAL-ENDPOINT-01.

PARTIAL implementation: this module is NOT the endpoint reader/packer.  It does
not validate the complete sidecar, run scalar replay, parse CTest, publish an
artifact, or attest a chain.  No production or third-party code is imported.
"""
from __future__ import annotations

from fractions import Fraction
import hashlib
import json
import re
import struct
import zlib
from typing import Any, Literal

CANONICAL_BYTES = 119
SIGNIFICANT_DIGITS = 110
MAX_CANONICAL_BYTES = 16 * 1024 * 1024
MAX_LINE_BYTES = 32768
MAX_GZIP_BYTES = MAX_CANONICAL_BYTES + 131072
MAX_DECIMAL_EXPONENT = 99999
MAX_BINARY_EXPONENT = 400000
MAX_INTEGER_DIGITS = 10000
GZIP_HEADER = bytes.fromhex("1f8b08000000000002ff")
ZERO = "+0." + "0" * 109 + "e+00000"
TOLERANCE = Fraction(1, 1 << 120)
ESTIMATOR_CEILING = Fraction(1, 1 << 128)
PRODUCER_TRANSPORT = Fraction(1, 1 << 300)
_BASELINE = "9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e"
_PRODUCTION = "b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89"
_OPENFHE = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
_NONZERO = re.compile(r"[+-][1-9]\.[0-9]{109}e[+-][0-9]{5}", re.ASCII)
_UNSIGNED = re.compile(r"0|[1-9][0-9]*", re.ASCII)
_HASH = re.compile(r"[0-9a-f]{64}", re.ASCII)
Decision = Literal["PASS", "FAIL", "UNRESOLVED"]


class EvidenceError(ValueError):
    """A detectable primitive validation error; not a final evidence status."""


def _require(condition: bool, message: str) -> None:
    if not condition:
        raise EvidenceError(message)


def parse_uint(text: str, *, positive: bool = False,
               max_digits: int = MAX_INTEGER_DIGITS) -> int:
    """Parse a bounded canonical ASCII integer without changing Python globals."""
    _require(type(text) is str and 0 < len(text) <= max_digits,
             "integer type/length")
    _require(_UNSIGNED.fullmatch(text) is not None, "integer grammar")
    result = 0
    for offset in range(0, len(text), 9):
        chunk = text[offset:offset + 9]
        result = result * 10 ** len(chunk) + int(chunk)
    _require(not positive or result > 0, "integer must be positive")
    return result


def integer_text(value: int, *, max_digits: int = MAX_INTEGER_DIGITS) -> str:
    _require(type(value) is int and value >= 0, "unsigned integer required")
    if value == 0:
        return "0"
    # Reject before repeated divisions; this upper bound never rejects a value
    # whose decimal expansion fits.  The exact length check follows.
    _require(value.bit_length() <= max_digits * 4, "integer range")
    chunks: list[int] = []
    while value:
        value, remainder = divmod(value, 1_000_000_000)
        chunks.append(remainder)
    text = str(chunks[-1]) + "".join(f"{chunk:09d}" for chunk in reversed(chunks[:-1]))
    _require(len(text) <= max_digits, "integer digit limit")
    return text


def reduced_nonnegative(numerator: int, denominator: int) -> Fraction:
    """Do not silently normalize a malformed on-disk rational receipt."""
    _require(type(numerator) is int and type(denominator) is int,
             "rational integer types")
    _require(numerator >= 0 and denominator > 0, "rational signs")
    result = Fraction(numerator, denominator)
    _require(result.numerator == numerator and result.denominator == denominator,
             "rational is not reduced")
    return result


def power_two(exponent: int) -> Fraction:
    _require(type(exponent) is int and abs(exponent) <= MAX_BINARY_EXPONENT,
             "binary exponent range")
    return Fraction(1 << exponent, 1) if exponent >= 0 else Fraction(1, 1 << -exponent)


def scaled_one_norm_exponent(coefficient_l1: int, scale_n: int,
                             scale_d: int) -> int | None:
    """Exact log2 ceiling of C/S, with a separate zero sentinel."""
    _require(type(coefficient_l1) is int and coefficient_l1 >= 0,
             "C must be a nonnegative integer")
    scale = reduced_nonnegative(scale_n, scale_d)
    _require(scale > 0, "scale must be positive")
    if coefficient_l1 == 0:
        return None
    numerator, denominator = coefficient_l1 * scale_d, scale_n
    exponent = numerator.bit_length() - denominator.bit_length()
    if exponent >= 0:
        if numerator > denominator << exponent:
            exponent += 1
    elif numerator << -exponent > denominator:
        exponent += 1
    _require(abs(exponent) <= MAX_BINARY_EXPONENT, "K exponent range")
    return exponent


def classify(distance: Fraction, allowance: Fraction, *,
             kind: str, model_supported: bool) -> Decision:
    """Exact adopted precedence, including raw disagreement before support."""
    if (type(distance) is not Fraction or type(allowance) is not Fraction
            or distance < 0 or allowance < 0
            or kind not in ("two-bounded-paths", "producer")
            or type(model_supported) is not bool):
        return "FAIL"
    if distance > TOLERANCE:
        return "FAIL"
    if not model_supported or allowance > ESTIMATOR_CEILING:
        return "UNRESOLVED"
    if kind == "two-bounded-paths" and distance > allowance:
        return "FAIL"
    return "PASS" if distance + allowance <= TOLERANCE else "UNRESOLVED"


def endpoint_allowances(bits: int, fresh_k: int | None,
                        terminal_k: int | None) -> dict[str, Fraction]:
    """Calculate prescribed values, not their applicability/assurance.

    Calling this function does not check Boost arithmetic, roots, Horner
    conversion, endpoint conditioning, or the 24 comparison receipts.
    """
    _require(type(bits) is int and bits in (512, 768), "observer precision")
    kf = Fraction(0) if fresh_k is None else power_two(fresh_k)
    kt = Fraction(0) if terminal_k is None else power_two(terminal_k)
    u = power_two(-bits)
    df, dt = power_two(12) * u * kf, power_two(12) * u * kt
    hf, ht = power_two(24) * u * kf, power_two(24) * u * kt
    p, r = power_two(270) * u, power_two(264) * u
    q = p + power_two(263) * df
    e0, e8, i8, a8 = df + r, dt + p + r, q + p + r, dt + q + r
    return {"u": u, "D_fresh": df, "D_terminal": dt,
            "H_fresh": hf, "H_terminal": ht, "P": p, "Q": q, "R": r,
            "E0": e0, "E8": e8, "I8": i8, "A8": a8,
            "identity": e8 + i8 + a8 + 2 * r}


def replay_allowances(fresh_k: int | None, terminal_k: int | None,
                      e0_quantum: Fraction, e8_quantum: Fraction) -> dict[str, Fraction]:
    """Exact prescribed replay budgets; this function performs no replay."""
    for quantum in (e0_quantum, e8_quantum):
        _require(type(quantum) is Fraction and quantum >= 0, "transport quantum")
    binary = endpoint_allowances(768, fresh_k, terminal_k)
    lipschitz = 256 * Fraction(3, 2) ** 255
    _require(lipschitz < power_two(158), "fixed replay inequality")
    decimal_u = Fraction(1, 10 ** 255)
    pdec, rdec = power_two(270) * decimal_u, power_two(264) * decimal_u
    u0 = binary["D_fresh"] + binary["R"] + e0_quantum + rdec
    return {"E0": binary["E0"] + e0_quantum,
            "E8": binary["E8"] + e8_quantum,
            "I8": lipschitz * u0 + 2 * pdec + rdec,
            "A8": binary["E8"] + e8_quantum + lipschitz * u0 + 2 * pdec + 2 * rdec}


def is_canonical_decimal(text: object) -> bool:
    if type(text) is not str or len(text) != CANONICAL_BYTES or not text.isascii():
        return False
    if text == ZERO:
        return True
    return _NONZERO.fullmatch(text) is not None and not text.endswith("e-00000")


def _ten(exponent: int) -> Fraction:
    _require(type(exponent) is int and abs(exponent) <= MAX_DECIMAL_EXPONENT + 110,
             "decimal exponent range")
    return Fraction(10 ** exponent) if exponent >= 0 else Fraction(1, 10 ** -exponent)


def parse_canonical_decimal(text: str) -> tuple[Fraction, Fraction]:
    """Return the exact printed rational and its decimal rounding quantum."""
    _require(is_canonical_decimal(text), "canonical decimal grammar")
    if text == ZERO:
        return Fraction(0), Fraction(0)
    exponent = int(text[113:])
    digits = int(text[1] + text[3:112])  # Exactly 110 ASCII digits.
    value = digits * _ten(exponent - 109)
    if text[0] == "-":
        value = -value
    return value, _ten(exponent - 109) / 2


def canonical_decimal(value: Fraction) -> str:
    """Integer-only half-even rounding; no rounded floating subtraction."""
    _require(type(value) is Fraction, "exact Fraction input required")
    if not value:
        return ZERO
    numerator, denominator = abs(value.numerator), value.denominator
    _require(max(numerator.bit_length(), denominator.bit_length()) <= MAX_BINARY_EXPONENT,
             "represented input range")
    # Rational approximation is only a starting index.  Exact comparisons
    # below determine floor(log10(abs(value))); no floating logarithm is used.
    exponent = ((numerator.bit_length() - denominator.bit_length()) * 1233) // 4096
    while Fraction(numerator, denominator) < _ten(exponent):
        exponent -= 1
    while Fraction(numerator, denominator) >= _ten(exponent + 1):
        exponent += 1
    _require(abs(exponent) <= MAX_DECIMAL_EXPONENT, "serialization exponent overflow")
    shift = 109 - exponent
    if shift >= 0:
        numerator *= 10 ** shift
    else:
        denominator *= 10 ** -shift
    significand, remainder = divmod(numerator, denominator)
    doubled = remainder * 2
    if doubled > denominator or (doubled == denominator and significand & 1):
        significand += 1
    if significand == 10 ** 110:
        significand //= 10
        exponent += 1
    _require(abs(exponent) <= MAX_DECIMAL_EXPONENT, "rounded exponent overflow")
    digits = str(significand)
    _require(len(digits) == 110 and digits[0] != "0", "significand/underflow")
    result = ("-" if value < 0 else "+") + digits[0] + "." + digits[1:]
    result += "e" + ("-" if exponent < 0 else "+") + f"{abs(exponent):05d}"
    _require(is_canonical_decimal(result), "internal decimal formatting error")
    return result


def validate_ascii_lines(data: bytes, *, max_bytes: int = MAX_CANONICAL_BYTES,
                         max_line: int = MAX_LINE_BYTES) -> None:
    """Envelope only, NOT the endpoint row/metadata schema."""
    _require(type(data) is bytes and 0 < len(data) <= max_bytes, "file byte limit")
    _require(data.endswith(b"\n") and b"\r" not in data and b"\x00" not in data,
             "ASCII LF envelope")
    _require(data.isascii(), "non-ASCII/BOM")
    start = 0
    while start < len(data):
        end = data.find(b"\n", start, min(len(data), start + max_line))
        _require(end >= 0, "line byte limit or missing LF")
        _require(end > start, "empty line")
        start = end + 1


def gzip_bytes(data: bytes) -> bytes:
    """Build one deterministic-header member; data still needs schema validation."""
    _require(type(data) is bytes and len(data) <= MAX_CANONICAL_BYTES, "canonical byte limit")
    compressor = zlib.compressobj(level=9, method=zlib.DEFLATED, wbits=-15)
    payload = compressor.compress(data) + compressor.flush()
    result = GZIP_HEADER + payload + struct.pack("<II", zlib.crc32(data), len(data))
    _require(len(result) <= MAX_GZIP_BYTES, "gzip byte limit")
    return result


def validate_gzip(blob: bytes, *, canonical_bytes: int | None = None,
                  canonical_sha256: str | None = None,
                  gzip_bytes_count: int | None = None,
                  gzip_sha256: str | None = None) -> bytes:
    """Validate one fixed-header gzip member and optional external identities."""
    _require(type(blob) is bytes and 18 <= len(blob) <= MAX_GZIP_BYTES, "gzip size")
    _require(blob[:10] == GZIP_HEADER, "gzip fixed header")
    _check_external(blob, gzip_bytes_count, gzip_sha256, "gzip")
    decompressor = zlib.decompressobj(-15)
    try:
        decoded = decompressor.decompress(blob[10:-8], MAX_CANONICAL_BYTES + 1)
    except zlib.error as error:
        raise EvidenceError("invalid DEFLATE stream") from error
    _require(len(decoded) <= MAX_CANONICAL_BYTES, "decompressed byte limit")
    _require(decompressor.eof and not decompressor.unused_data
             and not decompressor.unconsumed_tail, "truncated/extra/trailing gzip stream")
    crc, size = struct.unpack("<II", blob[-8:])
    _require(crc == zlib.crc32(decoded), "gzip CRC")
    _require(size == len(decoded), "gzip ISIZE")
    _check_external(decoded, canonical_bytes, canonical_sha256, "canonical")
    return decoded


def _check_external(data: bytes, count: int | None, digest: str | None,
                    label: str) -> None:
    if count is not None:
        _require(type(count) is int and count >= 0 and count == len(data),
                 label + " external bytes")
    if digest is not None:
        _require(type(digest) is str and _HASH.fullmatch(digest) is not None,
                 label + " external hash grammar")
        _require(hashlib.sha256(data).hexdigest() == digest, label + " external SHA256")


def strict_status_json(data: bytes, *, exact_keys: frozenset[str]) -> dict[str, Any]:
    """Validate the JSON envelope only; no complete status-semantic attestation.

    Callers must separately validate every schema value and its relationships.
    Booleans/floats are forbidden globally, including nested values.
    """
    validate_ascii_lines(data, max_bytes=32768, max_line=32768)
    _require(data.count(b"\n") == 1, "status must be one line")

    def pairs(items: list[tuple[str, Any]]) -> dict[str, Any]:
        result: dict[str, Any] = {}
        for key, value in items:
            _require(key not in result, "duplicate JSON key")
            result[key] = value
        return result

    def forbidden(_: str) -> Any:
        raise EvidenceError("JSON float/nonfinite token")

    try:
        result = json.loads(data, object_pairs_hook=pairs, parse_float=forbidden,
                            parse_constant=forbidden)
    except (ValueError, UnicodeError, RecursionError) as error:
        raise EvidenceError("invalid strict JSON") from error
    _require(type(result) is dict and frozenset(result) == exact_keys, "status key set")
    _require(not any(key in result for key in ("status_bytes", "status_sha256",
                 "status_hash", "status_size")), "status self identity forbidden")

    def check(value: Any, depth: int = 0) -> None:
        _require(depth <= 8, "JSON nesting limit")
        if type(value) is dict:
            for key, item in value.items():
                _require(type(key) is str and key.isascii(), "JSON key ASCII")
                check(item, depth + 1)
        elif type(value) is list:
            for item in value:
                check(item, depth + 1)
        else:
            _require(value is None or type(value) in (int, str), "JSON scalar type")
            if type(value) is str:
                _require(value.isascii(), "JSON value ASCII")
    check(result)
    canonical = (json.dumps(result, sort_keys=True, separators=(",", ":"),
                            ensure_ascii=True, allow_nan=False) + "\n").encode("ascii")
    _require(canonical == data, "status must be sorted compact canonical ASCII LF")
    return result
