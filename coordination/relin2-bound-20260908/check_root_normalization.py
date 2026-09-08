"""Evaluate a PROVISIONAL local Relin bound; no OpenFHE, sampling or FFT/NTT."""
from fractions import Fraction
import hashlib
import json
from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[2]
SOURCE_SHA256 = "6781d01ff9b4012e7c6d14d16f482def0821c54f4e81e2de80bd8db9d1aea41b"


def dyadic_enclosure(value):
    """Return k with 2^(-k-1) < positive value <= 2^(-k), exactly."""
    assert 0 < value < 1
    k = value.denominator.bit_length() - value.numerator.bit_length()
    while value > Fraction(1, 2**k):
        k -= 1
    while value <= Fraction(1, 2**(k + 1)):
        k += 1
    assert Fraction(1, 2**(k + 1)) < value <= Fraction(1, 2**k)
    return k


def main():
    source_bytes = (ROOT / "src/repeated_mult2.cpp").read_bytes()
    assert hashlib.sha256(source_bytes).hexdigest() == SOURCE_SHA256
    source = source_bytes.decode("utf-8")
    block = re.search(r"constexpr std::array<PaperPrime,11> kPaperQ\{\{(.*?)\}\};", source, re.S)
    assert block is not None
    pairs = re.findall(r"\{(\d+)ULL,(\d+)ULL\}", block.group(1))
    assert len(pairs) == 11
    q = [int(modulus) for modulus, _ in pairs]
    p_match = re.search(r"constexpr PaperPrime kPaperP\{(\d+)ULL,\d+ULL\};", source)
    assert p_match is not None
    p = int(p_match.group(1))
    n, h, e = 32768, 128, 39
    d = q[-1]
    active = q[:-1]
    scale = Fraction(2**100)
    rows = []

    def ordinary_embedding_bound(primes):
        # Conditional formula B in ROOT_ALGEBRA_DRAFT.md, not an observed error.
        coefficient = Fraction(n * e * sum(prime - 1 for prime in primes)
                               + (1 + h) * (p - 1), p)
        return n * coefficient

    for step in range(1, 9):
        m = active[-1]
        high_bound = ordinary_embedding_bound(active + [d])
        low_bound = ordinary_embedding_bound(active)
        relin_bound = high_bound + low_bound
        next_scale = scale * scale / (d * m)
        # RCB(RS2(pair)) = Rescale(RCB(pair)) modulo its output basis.
        # Only the local Relin summand is represented; rescale rounding is separate.
        normalized = relin_bound / (m * next_scale)
        assert normalized == relin_bound * d / (scale * scale)
        assert normalized != relin_bound / (d * m * next_scale)
        bits = dyadic_enclosure(normalized)
        rows.append({"step": step, "active_Q_towers": len(active),
                     "raised_Q_towers": len(active) + 1, "rescale_prime": m,
                     "conditional_local_relin_bound_decimal_approx": format(float(normalized), ".12e"),
                     "exact_dyadic_enclosure": {"strict_lower": f"2^-{bits + 1}", "upper": f"2^-{bits}"},
                     "two_normalizations_equal": True, "extra_divisor_rejected": True})
        scale = next_scale
        active = active[:-1]
    assert len(active) == 2
    output = {
        "status": "PROVISIONAL_CONDITIONAL_FORMULA_EVALUATION_NOT_FHE",
        "source": "src/repeated_mult2.cpp", "source_sha256": SOURCE_SHA256,
        "script_sha256": hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        "N": n, "h": h, "E": e, "Div": d, "P": p, "initial_scale": "2^100",
        "assumptions": ["Ordinary HYBRID bound in root draft is valid: alpha=1, single P, noiseScale=1, coherent arithmetic and secret lifts, key-error coefficient bound E=39.",
                        "Compatible representatives/nonwrap justify separating this numerical local Relin term; no such runtime witness was obtained here."],
        "excluded": ["Fresh error and its amplification", "Tensor2 low-times-low omission", "RS2 rounding error", "Cross-step propagation", "Wrap events", "Full-chain precision or security claims"],
        "rows": rows,
    }
    target = Path(__file__).with_name("ROOT_NORMALIZATION_CHECK.json")
    with target.open("x") as handle:
        json.dump(output, handle, ensure_ascii=False, indent=2)
        handle.write("\n")
    print(json.dumps(output, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
