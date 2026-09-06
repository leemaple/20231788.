from fractions import Fraction
import os
from pathlib import Path
import tempfile
import unittest

import paper_endpoint_sidecar_reader as reader


SOURCE = "a" * 40
BASELINE = "9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e"
PRODUCTION = "b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89"
OPENFHE = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
ZERO = "+0." + "0" * 109 + "e+00000"
CHECK_IDS = (
    "control.constant.512", "control.constant.768", "control.x.512",
    "control.x.768", "control.xNminus1.512", "control.xNminus1.768",
    "control.sparse.512", "control.sparse.768", "fresh.cross",
    "terminal.cross", "fresh.horner.512", "fresh.horner.768",
    "terminal.horner.512", "terminal.horner.768", "fresh.producer.512",
    "fresh.producer.768", "terminal.producer.512", "terminal.producer.768",
    "residual.E0.cross", "residual.E8.cross", "residual.I8.cross",
    "residual.A8.cross", "identity.512", "identity.768",
)


def pow2(exponent):
    return Fraction(1 << exponent, 1) if exponent >= 0 else Fraction(1, 1 << -exponent)


def int_text(value):
    if value == 0:
        return "0"
    chunks = []
    while value:
        value, remainder = divmod(value, 1_000_000_000)
        chunks.append(remainder)
    return str(chunks[-1]) + "".join(f"{chunk:09d}" for chunk in reversed(chunks[:-1]))


def scale8():
    q = (1125899904679937, 1125899903827969, 1152921504598720513,
         1152921504597016577, 1152921504595968001, 1152921504595640321,
         1152921504593412097, 1152921504592822273, 1152921504592429057,
         1152921504589938689, 1099510054913)
    numerator = 1 << (100 * 256)
    denominator = 1
    for index in range(1, 9):
        denominator *= (q[-1] * q[10 - index]) ** (1 << (8 - index))
    return Fraction(numerator, denominator)


def expected_allowances():
    # Independently reduced fixture algebra. C=3 gives K=2^-98 at both
    # frozen scales, hence D={2^-598,2^-854}, H_512=2^-586,
    # P={2^-242,2^-498}, R={2^-248,2^-504}, and 2^263 D={2^-335,2^-591}.
    return {
        "control.constant.512": pow2(-500) + pow2(-758),
        "control.constant.768": pow2(-756) + pow2(-758),
        "control.x.512": pow2(-500) + pow2(-758),
        "control.x.768": pow2(-756) + pow2(-758),
        "control.xNminus1.512": pow2(-500) + pow2(-758),
        "control.xNminus1.768": pow2(-756) + pow2(-758),
        "control.sparse.512": pow2(-497) + pow2(-755),
        "control.sparse.768": pow2(-753) + pow2(-755),
        "fresh.cross": pow2(-598) + pow2(-854),
        "terminal.cross": pow2(-598) + pow2(-854),
        "fresh.horner.512": pow2(-598) + pow2(-586),
        "fresh.horner.768": pow2(-854) + pow2(-586),
        "terminal.horner.512": pow2(-598) + pow2(-586),
        "terminal.horner.768": pow2(-854) + pow2(-586),
        "fresh.producer.512": pow2(-598) + pow2(-300),
        "fresh.producer.768": pow2(-854) + pow2(-300),
        "terminal.producer.512": pow2(-598) + pow2(-300),
        "terminal.producer.768": pow2(-854) + pow2(-300),
        "residual.E0.cross": (pow2(-598) + pow2(-248) +
                              pow2(-854) + pow2(-504)),
        "residual.E8.cross": (pow2(-598) + pow2(-242) + pow2(-248) +
                              pow2(-854) + pow2(-498) + pow2(-504)),
        "residual.I8.cross": (2 * pow2(-242) + pow2(-335) + pow2(-248) +
                              2 * pow2(-498) + pow2(-591) + pow2(-504)),
        "residual.A8.cross": (pow2(-598) + pow2(-242) + pow2(-335) + pow2(-248) +
                              pow2(-854) + pow2(-498) + pow2(-591) + pow2(-504)),
        "identity.512": (2 * pow2(-598) + 4 * pow2(-242) +
                         2 * pow2(-335) + 5 * pow2(-248)),
        "identity.768": (2 * pow2(-854) + 4 * pow2(-498) +
                         2 * pow2(-591) + 5 * pow2(-504)),
    }


def valid_bytes(scope="synthetic"):
    s8 = scale8()
    meta = (
        ("scope", scope), ("source_commit", SOURCE),
        ("baseline_tested_source", BASELINE), ("production_source", PRODUCTION),
        ("openfhe_pin", OPENFHE), ("host", "linux"), ("github_run_id", "17"),
        ("github_run_attempt", "2"), ("test_name", "paper_full_eight_square_contract"),
        ("chain_count", "1"), ("n", "32768"), ("m", "65536"),
        ("slots", "16384"), ("gap", "1"),
        ("input_formula", "frozen-four-phase-exact-dyadic-v1"),
        ("primary_precision_bits", "768"), ("check_precision_bits", "512"),
        ("significant_digits", "110"), ("scale0_numerator", str(1 << 100)),
        ("scale0_denominator", "1"), ("scale8_numerator", int_text(s8.numerator)),
        ("scale8_denominator", int_text(s8.denominator)),
        ("coefficient_l1_fresh", "3"), ("coefficient_l1_terminal", "3"),
        ("fresh_max_l1_numerator", "0"), ("fresh_max_l1_denominator", "1"),
        ("terminal_max_l1_numerator", "0"), ("terminal_max_l1_denominator", "1"),
        ("model", "conditional-binary-nearest-direct-trig8u-v1"),
        ("assurance", "CONDITIONAL"), ("boost_version", "108300"),
        ("root_policy", "direct-own-precision-v1"),
        ("norm", "max-real-imag-component"), ("observer_tolerance", "2^-120"),
        ("estimator_ceiling", "2^-128"), ("original_error_gate", "2^-80"),
        ("rounding", "decimal-nearest-ties-even"), ("row_count", "16384"),
        ("check_count", "24"), ("numeric_gate_failures", "7"),
        ("E80_disposition", "FAIL"), ("A_disposition", "NOT_ADOPTED"),
        ("observer_disposition", "PASS"),
    )
    lines = ["#fs-residual-endpoint-01.v1-r1"]
    lines.extend("meta\t" + key + "\t" + value for key, value in meta)
    allowances = expected_allowances()
    for check_id in CHECK_IDS:
        bound = allowances[check_id]
        lines.append(f"check\t{check_id}\tPASS\t0\t1\t{bound.numerator}\t{bound.denominator}\t0\treal")
    lines.append("slot\tE0.real\tE0.imag\tE8.real\tE8.imag")
    lines.extend(f"{slot}\t{ZERO}\t{ZERO}\t{ZERO}\t{ZERO}" for slot in range(16384))
    return ("\n".join(lines) + "\n").encode("ascii")


class SidecarReaderTests(unittest.TestCase):
    def read(self, data, **overrides):
        expected = dict(expected_scope="synthetic", expected_source_commit=SOURCE,
                        expected_host="linux", expected_run_id="17", expected_run_attempt="2")
        expected.update(overrides)
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-") as directory:
            path = Path(directory) / "candidate.tsv"
            path.write_bytes(data)
            return reader.read_sidecar(path, **expected)

    def test_reads_complete_16384_row_synthetic_sidecar(self):
        parsed = self.read(valid_bytes())
        self.assertEqual(parsed.meta["row_count"], 16384)
        self.assertEqual(len(parsed.checks), 24)
        self.assertEqual(len(parsed.rows), 16384)
        self.assertEqual(parsed.rows[-1].slot, 16383)

    def test_rederives_nonzero_fractional_scale_allowances(self):
        parsed = self.read(valid_bytes())
        checks = {check.check_id: check for check in parsed.checks}
        # C/S is fractional at each endpoint and rounds upward to K=2^-98.
        # The two summands here are independently reduced D_512 and D_768.
        self.assertEqual(checks["fresh.cross"].allowance,
                         pow2(-598) + pow2(-854))
        self.assertEqual(checks["terminal.cross"].allowance,
                         pow2(-598) + pow2(-854))

    def test_rejects_endpoint_radius_above_five_fourths(self):
        data = valid_bytes().replace(
            b"meta\tfresh_max_l1_numerator\t0\n", b"meta\tfresh_max_l1_numerator\t2\n", 1)
        with self.assertRaises(reader.SidecarError):
            self.read(data)

    def test_live_identity_is_bound_by_caller(self):
        with self.assertRaises(reader.SidecarError):
            self.read(valid_bytes(), expected_scope="live-single-chain")
        with self.assertRaises(reader.SidecarError):
            self.read(valid_bytes(), expected_source_commit="b" * 40)
        with self.assertRaises(reader.SidecarError):
            self.read(valid_bytes(), expected_source_commit=b"a" * 40)
        with self.assertRaises(reader.SidecarError):
            self.read(valid_bytes(), expected_run_id="18")

    def test_rejects_metadata_order_scale_and_numeric_inconsistency(self):
        valid = valid_bytes()
        scope = b"meta\tscope\tsynthetic\n"
        source = b"meta\tsource_commit\t" + SOURCE.encode() + b"\n"
        mutations = (
            valid.replace(scope + source, source + scope, 1),
            valid.replace(b"meta\tscale8_numerator\t", b"meta\tscale8_numerator\t1", 1),
            valid.replace(b"meta\tE80_disposition\tFAIL\n", b"meta\tE80_disposition\tPASS\n", 1),
            valid.replace(b"meta\tcheck_count\t24\n", b"meta\tunknown_count\t24\n", 1),
        )
        for data in mutations:
            with self.subTest(prefix=data[:80]), self.assertRaises(reader.SidecarError):
                self.read(data)

    def test_rejects_wrong_allowance_and_failed_check(self):
        lines = valid_bytes().decode("ascii").splitlines()
        first = next(index for index, line in enumerate(lines)
                     if line.startswith("check\tcontrol.constant.512\t"))
        fields = lines[first].split("\t")
        fields[5] = str(int(fields[5]) + 1)
        wrong_bound = lines[:]
        wrong_bound[first] = "\t".join(fields)
        failed_distance = lines[:]
        fields = lines[first].split("\t")
        fields[3:5] = ["1", "1"]
        failed_distance[first] = "\t".join(fields)
        unreduced = lines[:]
        fields = lines[first].split("\t")
        fields[3:5] = ["0", "2"]
        unreduced[first] = "\t".join(fields)
        for changed in (wrong_bound, failed_distance, unreduced):
            with self.assertRaises(reader.SidecarError):
                self.read(("\n".join(changed) + "\n").encode("ascii"))

    def test_rejects_non_anchor_horner_argmax_and_noncanonical_zero_tie(self):
        lines = valid_bytes().decode("ascii").splitlines()
        horner = next(index for index, line in enumerate(lines)
                      if line.startswith("check\tfresh.horner.512\t"))
        fields = lines[horner].split("\t")
        fields[3:5] = fields[5:7]
        fields[7] = "2"
        non_anchor = lines[:]
        non_anchor[horner] = "\t".join(fields)

        control = next(index for index, line in enumerate(lines)
                       if line.startswith("check\tcontrol.constant.512\t"))
        fields = lines[control].split("\t")
        fields[7:9] = ["1", "imag"]
        bad_zero_tie = lines[:]
        bad_zero_tie[control] = "\t".join(fields)
        for changed in (non_anchor, bad_zero_tie):
            with self.assertRaises(reader.SidecarError):
                self.read(("\n".join(changed) + "\n").encode("ascii"))

    def test_accepts_line_bounded_rational_over_ten_thousand_digits(self):
        lines = valid_bytes().decode("ascii").splitlines()
        control = next(index for index, line in enumerate(lines)
                       if line.startswith("check\tcontrol.constant.512\t"))
        fields = lines[control].split("\t")
        denominator = int_text(1 << 40000)
        self.assertGreater(len(denominator), 10000)
        fields[3:5] = ["1", denominator]
        lines[control] = "\t".join(fields)
        parsed = self.read(("\n".join(lines) + "\n").encode("ascii"))
        self.assertEqual(parsed.checks[0].distance, Fraction(1, 1 << 40000))

    def test_rejects_direct_symlink(self):
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-") as directory:
            target = Path(directory) / "target.tsv"
            link = Path(directory) / "candidate.tsv"
            target.write_bytes(valid_bytes())
            os.symlink(target, link)
            with self.assertRaisesRegex(reader.SidecarError, "non-symlink"):
                reader.read_sidecar(link, expected_scope="synthetic",
                    expected_source_commit=SOURCE, expected_host="linux",
                    expected_run_id="17", expected_run_attempt="2")

    def test_rejects_row_order_count_and_decimal_grammar(self):
        valid = valid_bytes()
        lines = valid.decode("ascii").splitlines()
        last = lines[-1].split("\t")
        last[0] = "16382"
        reordered = lines[:]
        reordered[-1] = "\t".join(last)
        bad_zero = ("-0." + "0" * 109 + "e+00000").encode("ascii")
        mutations = (
            ("\n".join(reordered) + "\n").encode("ascii"),
            b"\n".join(valid.split(b"\n")[:-2]) + b"\n",
            valid + valid.split(b"\n")[-2] + b"\n",
            valid.replace(ZERO.encode("ascii"), bad_zero, 1),
        )
        for data in mutations:
            with self.assertRaises(reader.SidecarError):
                self.read(data)

    def test_rejects_before_unbounded_line_or_file_allocation(self):
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-") as directory:
            path = Path(directory) / "oversized.tsv"
            path.write_bytes(b"x" * (16 * 1024 * 1024 + 1))
            with self.assertRaisesRegex(reader.SidecarError, "byte limit"):
                reader.read_sidecar(path, expected_scope="synthetic",
                    expected_source_commit=SOURCE, expected_host="linux",
                    expected_run_id="17", expected_run_attempt="2")
            path.write_bytes(b"x" * 32768 + b"\n")
            with self.assertRaisesRegex(reader.SidecarError, "oversized line"):
                reader.read_sidecar(path, expected_scope="synthetic",
                    expected_source_commit=SOURCE, expected_host="linux",
                    expected_run_id="17", expected_run_attempt="2")


if __name__ == "__main__":
    unittest.main(verbosity=2)
