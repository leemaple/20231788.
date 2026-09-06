"""Synthetic data-level seam tests; not a full hosted replay receipt."""
from types import SimpleNamespace
from fractions import Fraction
from copy import copy
from pathlib import Path
import tempfile
import unittest

import paper_endpoint_reconcile as reconcile


def pow2(exponent):
    return Fraction(2) ** exponent


def bound_pair():
    """Analytic zero-residual fixture, not a generated cryptographic receipt."""
    q = (1125899904679937, 1125899903827969, 1152921504598720513,
         1152921504597016577, 1152921504595968001, 1152921504595640321,
         1152921504593412097, 1152921504592822273, 1152921504592429057,
         1152921504589938689, 1099510054913)
    scales = []
    for i in range(9):
        denominator = 1
        for j in range(1, i + 1):
            denominator *= (q[-1] * q[10 - j]) ** (2 ** (i - j))
        value = Fraction(2 ** (100 * 2 ** i), denominator)
        scales.append(SimpleNamespace(operation=i, numerator=value.numerator,
                                      denominator=value.denominator))
    ids = tuple(f"control.{name}.{bits}" for name in
                ("constant", "x", "xNminus1", "sparse") for bits in (512, 768))
    ids += ("fresh.cross", "terminal.cross", "fresh.horner.512", "fresh.horner.768",
            "terminal.horner.512", "terminal.horner.768", "fresh.producer.512",
            "fresh.producer.768", "terminal.producer.512", "terminal.producer.768",
            "residual.E0.cross", "residual.E8.cross", "residual.I8.cross",
            "residual.A8.cross", "identity.512", "identity.768")
    allowances = [(pow2(12 - bits) + pow2(-758)) * k
                  for k in (1, 1, 1, 8) for bits in (512, 768)]
    allowances += [Fraction(0)] * 6 + [pow2(-300)] * 4
    live = (pow2(-504), pow2(-498) + pow2(-504),
            2 * pow2(-498) + pow2(-504), pow2(-498) + pow2(-504))
    allowances += [value * (2 ** 256 + 1) for value in live]
    identity = 4 * pow2(-498) + 5 * pow2(-504)
    allowances += [identity * 2 ** 256, identity]
    checks = tuple(SimpleNamespace(check_id=name, distance=Fraction(0),
                                   allowance=allowance, argmax_slot=0,
                                   argmax_component="real")
                   for name, allowance in zip(ids, allowances))
    maxima = tuple(SimpleNamespace(residual_id=name, allowance=allowance)
                   for name, allowance in zip(("E0", "E8", "I8", "A8"), live))
    endpoint = SimpleNamespace(scope="synthetic", boost_version=108300,
                               scales=tuple(scales), checks=checks, maxima=maxima)
    primary = SimpleNamespace(
        evidence_state="COMPLETE", reason="NONE", ctest_exit_code=8,
        cleanup_seen=True, legacy_begin_seen=True, timed_out=False,
        identity=SimpleNamespace(source_commit="a" * 40, host="linux",
                                 github_run_id="101", github_run_attempt="2"),
        numeric_gate_failures=2, numeric_gate_labels=("fresh full-slot 2^-80 gate",
                                                   "fresh retained sub-binary64 witness"),
        e80_disposition="FAIL", original_result="FAIL", endpoint=endpoint,
        legacy_scales=tuple(scales))
    meta = dict(scope="synthetic", source_commit="a" * 40, host="linux",
                github_run_id="101", github_run_attempt="2", boost_version=108300,
                numeric_gate_failures=2, E80_disposition="FAIL", A_disposition="NOT_ADOPTED",
                row_count=16384, chain_count=1, coefficient_l1_fresh=0,
                coefficient_l1_terminal=0, scale0=Fraction(2 ** 100),
                scale8=Fraction(scales[8].numerator, scales[8].denominator))
    sidecar = SimpleNamespace(meta=meta, checks=checks, rows=(None,) * 16384)
    return primary, sidecar


class ReconcileTests(unittest.TestCase):
    def test_all_four_foreign_identity_fields_are_rejected(self):
        for field, value in (("source_commit", "b" * 40), ("host", "windows"),
                             ("github_run_id", "102"), ("github_run_attempt", "3")):
            with self.subTest(identity=field):
                primary, sidecar = bound_pair()
                sidecar.meta[field] = value
                with self.assertRaises(reconcile.ReconcileError) as caught:
                    reconcile.validate_primary_binding(primary, sidecar)
                self.assertEqual(caught.exception.reason, "IDENTITY")

    def test_complete_failing_e80_binds_without_becoming_success(self):
        primary, sidecar = bound_pair()
        self.assertIsNone(reconcile.validate_primary_binding(primary, sidecar))
        self.assertEqual((primary.ctest_exit_code, primary.e80_disposition), (8, "FAIL"))

    def test_scope_observed_facts_and_all_scale_receipts_must_agree(self):
        for key, replacement in (("scope", "live-single-chain"), ("boost_version", 109200),
                                 ("numeric_gate_failures", 3), ("E80_disposition", "PASS"),
                                 ("scale0", Fraction(1)), ("scale8", Fraction(1))):
            with self.subTest(key=key):
                primary, sidecar = bound_pair()
                sidecar.meta[key] = replacement
                with self.assertRaises(reconcile.ReconcileError):
                    reconcile.validate_primary_binding(primary, sidecar)
        for index in range(9):
            with self.subTest(scale=index):
                primary, sidecar = bound_pair()
                modified = list(primary.legacy_scales)
                modified[index] = copy(modified[index])
                modified[index].numerator += 1
                primary.legacy_scales = tuple(modified)
                with self.assertRaises(reconcile.ReconcileError):
                    reconcile.validate_primary_binding(primary, sidecar)

    def test_all_comparison_fields_and_actual_c_derived_max_allowances_bind(self):
        for index in range(24):
            for field, value in (("check_id", "foreign"), ("distance", Fraction(1)),
                                 ("allowance", Fraction(1)), ("argmax_slot", 7),
                                 ("argmax_component", "imag")):
                with self.subTest(check=index, field=field):
                    primary, sidecar = bound_pair()
                    checks = list(sidecar.checks)
                    checks[index] = copy(checks[index])
                    setattr(checks[index], field, value)
                    sidecar.checks = tuple(checks)
                    with self.assertRaises(reconcile.ReconcileError):
                        reconcile.validate_primary_binding(primary, sidecar)
        for index in range(4):
            with self.subTest(maximum=index):
                primary, sidecar = bound_pair()
                primary.endpoint.maxima[index].allowance += pow2(-800)
                with self.assertRaises(reconcile.ReconcileError):
                    reconcile.validate_primary_binding(primary, sidecar)
        for key in ("coefficient_l1_fresh", "coefficient_l1_terminal"):
            with self.subTest(coefficient=key):
                primary, sidecar = bound_pair()
                sidecar.meta[key] = 1
                with self.assertRaises(reconcile.ReconcileError):
                    reconcile.validate_primary_binding(primary, sidecar)

    def test_incomplete_and_wrong_complete_record_shapes_are_rejected(self):
        primary, sidecar = bound_pair()
        primary.evidence_state = "FATAL"
        with self.assertRaises(reconcile.ReconcileError):
            reconcile.validate_primary_binding(primary, sidecar)
        for field in ("checks", "scales", "maxima"):
            primary, sidecar = bound_pair()
            setattr(primary.endpoint, field, getattr(primary.endpoint, field)[:-1])
            with self.subTest(field=field), self.assertRaises(reconcile.ReconcileError):
                reconcile.validate_primary_binding(primary, sidecar)
        primary, sidecar = bound_pair()
        primary.endpoint.maxima[0].residual_id = "A8"
        with self.assertRaises(reconcile.ReconcileError):
            reconcile.validate_primary_binding(primary, sidecar)

    def test_actual_readers_bind_then_reject_individually_valid_disagreement(self):
        # These are independent test-data builders, not internal SUT helpers.
        # This exercises the actual parser dataclasses with full synthetic TSV
        # shape; it does not perform the hosted-only scalar full replay.
        import paper_endpoint_primary_reader as primary_reader
        import paper_endpoint_sidecar_reader as sidecar_reader
        import test_paper_endpoint_primary_reader as primary_fixture
        import test_paper_endpoint_sidecar_reader as sidecar_fixture

        identity = primary_reader.PrimaryIdentity("a" * 40, "linux", "17", "2")
        canonical = sidecar_fixture.valid_bytes().replace(
            b"meta\tnumeric_gate_failures\t7\n", b"meta\tnumeric_gate_failures\t2\n")

        def primary_bytes(fresh768=pow2(-854)):
            return primary_fixture.ctest_log(primary_fixture.complete_lines(
                2, scope="synthetic", fresh768=fresh768, terminal768=pow2(-854)
            )).replace(primary_fixture.SOURCE.encode("ascii"), b"a" * 40).replace(
                b"github_run_id=101\t", b"github_run_id=17\t")

        def parse_primary(data):
            return primary_reader.parse_primary_log(
                data, identity, ctest_exit_code=8, expected_scope="synthetic")

        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-binding-") as temporary:
            path = Path(temporary).resolve() / "synthetic.tsv"
            path.write_bytes(canonical)
            sidecar = sidecar_reader.read_sidecar(
                path, expected_scope="synthetic", expected_source_commit="a" * 40,
                expected_host="linux", expected_run_id="17", expected_run_attempt="2")
            primary = parse_primary(primary_bytes())
            self.assertIsNone(reconcile.validate_primary_binding(primary, sidecar))
            self.assertEqual((primary.ctest_exit_code, primary.e80_disposition), (8, "FAIL"))

            differing_distance = primary_bytes().replace(
                b"id=fresh.producer.512\tresult=PASS\tdistance_num=0\tdistance_den=1\t",
                b"id=fresh.producer.512\tresult=PASS\tdistance_num=1\tdistance_den=" +
                str(2 ** 400).encode("ascii") + b"\t")
            for data in (differing_distance, primary_bytes(pow2(-853))):
                independently_complete = parse_primary(data)
                self.assertEqual(independently_complete.evidence_state, "COMPLETE")
                with self.assertRaises(reconcile.ReconcileError):
                    reconcile.validate_primary_binding(independently_complete, sidecar)


if __name__ == "__main__":
    unittest.main()
