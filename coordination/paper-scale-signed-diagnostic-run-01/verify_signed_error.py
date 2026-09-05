#!/usr/bin/env python3
"""Read-only, bounded scalar verification of the retained signed diagnostics.

No codec, transform, FHE, compiler, network or filesystem writes. JSON goes to
stdout; save artifacts separately through the authorized apply_patch workflow.
"""
from decimal import Decimal as D, getcontext
from fractions import Fraction as F
from pathlib import Path
import hashlib
import json
import re
import sys

getcontext().prec = 160
if hasattr(sys, "set_int_max_str_digits"):
    sys.set_int_max_str_digits(20000)
SOURCE = "9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e"
PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
DEFAULT_HASH = "44eb5102695106bbe80a693ce4028b4160923f521d683c2d6af293bfc76040fa"
log = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(__file__).with_name("LINUX_RAW.log")
expected_hash = sys.argv[2] if len(sys.argv) > 2 else DEFAULT_HASH
raw = log.read_bytes()
digest = hashlib.sha256(raw).hexdigest()
assert digest == expected_hash, "unexpected input hash"
anchors = (0, 1, 256, 257, 512, 513, 768, 769, 1023, 16383)
q = (1125899904679937, 1125899903827969, 1152921504598720513,
     1152921504597016577, 1152921504595968001, 1152921504595640321,
     1152921504593412097, 1152921504592822273, 1152921504592429057,
     1152921504589938689, 1099510054913)
gate = D(2) ** -80
codec_gate = D(2) ** -120
signed_tolerance = D("1e-120")
fields, field_lines, receipts, failures = {}, {}, {}, []
active = False
end = None
numeric_failures = None
cleanup = False
for line_number, line in enumerate(raw.decode("utf-8").splitlines(), 1):
    if "61: BEGIN test=paper_full_eight_square_contract " in line:
        assert not active
        assert "source=" + SOURCE in line and "openfhe_pin=" + PIN in line
        assert "chain_count=1" in line
        active = True
        begin = line_number
    if not active:
        continue
    match = re.search(r"61: OBS field=(\S+) value=([-+0-9.eE]+)", line)
    if match:
        name, value = match.groups()
        assert name not in fields, "duplicate original field: " + name
        fields[name] = D(value)
        assert fields[name].is_finite()
        field_lines[name] = line_number
    match = re.search(r"61: RECEIPT (.*)", line)
    if match:
        data = dict(re.findall(r"(\w+)=([^ ]+)", match[1]))
        operation = int(data["operation"])
        assert operation not in receipts
        receipts[operation] = data
    match = re.search(r"61: OBS numeric_gate=FAIL label=(.*)", line)
    if match:
        failures.append(match[1])
    match = re.search(r"61: OBS numeric_gate_failures=(\d+)", line)
    if match:
        assert numeric_failures is None
        numeric_failures = int(match[1])
    if "61: OBS lifecycle=paper_owner_cleanup owned_absent=8 unrelated_unchanged=2 result=PASS" in line:
        cleanup = True
    if "61: COMPLETE test=paper_full_eight_square_contract " in line:
        assert "result=FAIL" in line and "source=" + SOURCE in line
        assert "accumulated numeric acceptance failures:" in line
        end = line_number
        completion = line.split("61: ", 1)[1]
        break  # Never process CTest's replay as another observation.
assert end and numeric_failures == len(failures) and cleanup
assert set(receipts) == set(range(9))


def mul(z, w):
    return z[0] * w[0] - z[1] * w[1], z[0] * w[1] + z[1] * w[0]


def add(z, w):
    return z[0] + w[0], z[1] + w[1]


def sub(z, w):
    return z[0] - w[0], z[1] - w[1]


def norm(z):
    return max(abs(z[0]), abs(z[1]))


def decimal_fraction(x):
    return D(x.numerator) / D(x.denominator)


def input_fraction(slot):
    t = slot // 2
    a = F(1015, 1024) - F(t % 16, 65536) + F(slot, 2 ** 75)
    b = F(1 + (t // 16) % 8, 1024) * (-1 if (t // 512) % 2 else 1)
    return ((a, b), (-b, a), (-a, -b), (b, -a))[(t // 128) % 4]


def pair(prefix):
    return fields[prefix + ".real"], fields[prefix + ".imag"]


def close_printed(observed, recomputed):
    assert abs(observed - recomputed) <= max(abs(recomputed) * D("1e-44"), D("1e-180"))


checks = {name: D(0) for name in ("fresh_w0_minus_zE", "I", "A", "L", "E_minus_I_minus_A")}


def check_identity(name, observed, recomputed, tolerance=signed_tolerance):
    error = norm(sub(observed, recomputed))
    checks[name] = max(checks[name], error)
    assert error <= tolerance, (name, str(error))


scale = F(2 ** 100)
scale_summaries = []
for r in range(9):
    if r:
        scale = scale * scale / (q[-1] * q[10-r])
    row = receipts[r]
    assert int(row["exact_n"]) == scale.numerator and int(row["exact_d"]) == scale.denominator
    assert int(row["family"]) == min(r, 7)
    assert int(row["local_level"]) == (2 if r == 8 else 1)
    assert int(row["towers"]) == 10-r and int(row["recorded_exp2"]) == 100
    assert int(row["terminal"]) == (1 if r == 8 else 0)
    scale_summaries.append({"round": r, "exact_scale_matches": True,
                            "numerator_bits": scale.numerator.bit_length(),
                            "denominator_bits": scale.denominator.bit_length(),
                            "S_over_2pow100": decimal_fraction(scale/F(2**100))})

exact_z = {a: input_fraction(a) for a in anchors}
fresh_power, previous, fresh_norms = {}, {}, {}
for a in anchors:
    z = tuple(map(decimal_fraction, exact_z[a]))
    e0 = pair(f"diag.fresh.anchor_{a}.E")
    w0_printed = pair(f"diag.fresh.anchor_{a}.w0")
    # z+E carries ~100 significant digits of the tiny error, more accurate for
    # cancellation checks than w0's ~100 digits of its near-unit value.
    w0 = add(z, e0)
    check_identity("fresh_w0_minus_zE", w0_printed, w0, D("1e-100"))
    fresh_power[a] = previous[a] = w0
    fresh_norms[a] = norm(e0)
    close_printed(fields[f"fresh.anchor_{a}_error"], norm(e0))
close_printed(fields["fresh.anchor_max_component_error"], max(fresh_norms.values()))

rounds = []
expected_failures = []
if fields["fresh.full_max_component_error"] > gate:
    expected_failures.append("fresh full-slot 2^-80 gate")
for phase in ("fresh", "final"):
    assert fields[f"{phase}.codec_cross_precision"] <= codec_gate
    assert fields[f"{phase}.anchor_vs_production"] <= gate


def witness_pass(phase):
    actual = fields[f"{phase}.actual_witness_real_difference"]
    expected = fields[f"{phase}.expected_witness_real_difference"]
    return actual > D(2)**-76 and abs(actual-expected) <= 2*gate


if not witness_pass("fresh"):
    expected_failures.append("fresh retained sub-binary64 witness")
if max(fresh_norms.values()) > gate:
    expected_failures.append("fresh independent anchor 2^-80 gate")
for r in range(1, 9):
    samples = []
    for a in anchors:
        exact_z[a] = mul(exact_z[a], exact_z[a])
        z = tuple(map(decimal_fraction, exact_z[a]))
        fresh_power[a] = mul(fresh_power[a], fresh_power[a])
        data = {kind: pair(f"diag.round_{r}.anchor_{a}.{kind}") for kind in "EIAL"}
        actual = add(z, data["E"])
        check_identity("I", data["I"], sub(fresh_power[a], z))
        check_identity("A", data["A"], sub(actual, fresh_power[a]))
        check_identity("L", data["L"], sub(actual, mul(previous[a], previous[a])))
        check_identity("E_minus_I_minus_A", data["E"], add(data["I"], data["A"]))
        previous[a] = actual
        values = {kind: norm(data[kind]) for kind in "EIAL"}
        close_printed(fields[f"round_{r}.anchor_{a}_error"], values["E"])
        samples.append({"anchor": a, **values, "I_over_A": values["I"]/values["A"],
                        "A_over_I": values["A"]/values["I"],
                        "E_over_gate": values["E"]/gate})
    maxima = {kind: max(samples, key=lambda row: row[kind]) for kind in "EIAL"}
    for kind in "IAL":
        close_printed(fields[f"diag.round_{r}.{kind}_anchor_max_component"], maxima[kind][kind])
    close_printed(fields[f"round_{r}.anchor_max_component_error"], maxima["E"]["E"])
    if maxima["E"]["E"] > gate:
        expected_failures.append(f"round_{r} independent anchor 2^-80 gate")
    rounds.append({"round": r,
                   "maxima": {kind: {"value": row[kind], "anchor": row["anchor"]} for kind, row in maxima.items()},
                   "ratio_of_anchor_maxima_I_over_A": maxima["I"]["I"]/maxima["A"]["A"],
                   "same_anchor_I_over_A_min": min(row["I_over_A"] for row in samples),
                   "same_anchor_I_over_A_max": max(row["I_over_A"] for row in samples),
                   "samples": samples})

for a in anchors:
    close_printed(fields[f"final.anchor_{a}_error"], fields[f"round_8.anchor_{a}_error"])
close_printed(fields["final.anchor_max_component_error"], fields["round_8.anchor_max_component_error"])
if fields["final.full_max_component_error"] > gate:
    expected_failures.append("final full-slot 2^-80 gate")
if not witness_pass("final"):
    expected_failures.append("final retained sub-binary64 witness")
if fields["final.anchor_max_component_error"] > gate:
    expected_failures.append("final independent anchor 2^-80 gate")
assert failures == expected_failures
assert fields["final.wrong_nominal100_error"] > D(2)**-30
exact_delta = decimal_fraction(exact_z[1][0] - exact_z[0][0])
close_printed(fields["final.expected_witness_real_difference"], exact_delta)
assert F(1,2**71) < exact_z[1][0]-exact_z[0][0] < F(1,2**70)

coefficients = {stage: fields[f"diag.{stage}.coefficient_max_over_scale"]
                for stage in ("fresh", *(f"round_{r}" for r in range(1,9)), "final")}
assert all(value >= 0 for value in coefficients.values())
relevant = {name: value for name, value in fields.items() if name.startswith(("fresh.", "final."))}
radius_upper = ((D(1015)/1024+D(16383)/D(2)**75)**2+(D(8)/1024)**2).sqrt()
budget_scenarios = []
for k in (2,4,8,16,32,64,128,256):
    sensitivity = D(2).sqrt()*k*radius_upper**(k-1)
    budget_scenarios.append({"power": k, "component_sensitivity_upper": sensitivity,
                             "first_order_sufficient_fresh_budget_if_added_error_zero": gate/sensitivity})
summary = {
    "log": log.name, "bytes": len(raw), "sha256": digest, "source": SOURCE, "openfhe_pin": PIN,
    "first_stream_lines": [begin, end], "completion": completion,
    "fresh_signed_complex_pairs": 20, "round_signed_complex_pairs": 320,
    "unique_numeric_fields": len(fields), "receipts_verified": 9,
    "independent_scalar_method": "Exact Fraction dyadic z and eight squares; Decimal160 signed log identities. No coefficient or transform execution.",
    "identity_absolute_tolerance": signed_tolerance, "identity_max_disagreements": checks,
    "scale_receipts": scale_summaries, "rounds": rounds,
    "coefficient_max_over_scale": coefficients,
    "ordinary_rounding_Horner_estimate_max_16N2BoverS2powminus512": D(16)*32768**2*max(coefficients.values())*D(2)**-512,
    "fresh_final_observations": relevant,
    "fresh_witness_pass": witness_pass("fresh"), "final_witness_pass": witness_pass("final"),
    "fresh_witness_difference_error": abs(fields["fresh.actual_witness_real_difference"]-fields["fresh.expected_witness_real_difference"]),
    "final_witness_difference_error": abs(fields["final.actual_witness_real_difference"]-fields["final.expected_witness_real_difference"]),
    "numeric_failure_labels": failures, "numeric_failures": numeric_failures, "cleanup_marker": cleanup,
    "final_full_over_gate": fields["final.full_max_component_error"]/gate,
    "final_anchor_over_gate": fields["final.anchor_max_component_error"]/gate,
    "analytic_budget_scenarios": budget_scenarios,
    "analytic_budget_limit": "First-order conservative sufficient budgets, not necessary bounds, proposed new thresholds, or measured noise guarantees. Nonlinear remainder and added arithmetic need budget too.",
    "scope_limits": ["No ciphertext coefficients retained in this log: Horner correctness/normalization is not independently re-executed.",
                     "I/A/L attribution covers ten anchors, not every slot; full-slot end-to-end error is a separate observed aggregate.",
                     "Fresh residual includes both encoding and encryption. Added/local residuals do not isolate individual evaluator primitives.",
                     "Conditioning estimate assumes ordinary rounding and accurate binary512 roots; not a certified transcendental bound.",
                     "A numeric FAIL remains a failed frozen contract, regardless of small A or L."]}
print(json.dumps(summary, default=lambda value: format(value, ".50E") if isinstance(value, D) else str(value), indent=2))
