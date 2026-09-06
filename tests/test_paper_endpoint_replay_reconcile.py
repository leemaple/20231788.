"""Bounded selected-row metric reconciliation tests; no full sidecar replay."""
from fractions import Fraction
from types import SimpleNamespace
from dataclasses import replace
import unittest

import paper_endpoint_reconcile as reconcile
from paper_endpoint_sidecar_replay import (
    ReplayMetric,
    ReplayResult,
    derive_replay_bounds,
    replay_row,
    summarize_scalars,
)
from test_paper_endpoint_reconcile import bound_pair


ZERO = "+0." + "0" * 109 + "e+00000"
RESIDUALS = ("E0", "E8", "I8", "A8")


def power10(exponent):
    return Fraction(10 ** exponent) if exponent >= 0 else Fraction(1, 10 ** -exponent)


def canonical_parts(value):
    if value == 0:
        return ZERO, 0, 0, Fraction(0), Fraction(0)
    sign = "-" if value < 0 else "+"
    absolute = abs(value)
    exponent = 0
    while absolute < power10(exponent):
        exponent -= 1
    while absolute >= power10(exponent + 1):
        exponent += 1
    scaled = absolute * power10(109 - exponent)
    quotient, remainder = divmod(scaled.numerator, scaled.denominator)
    if remainder * 2 > scaled.denominator or (
            remainder * 2 == scaled.denominator and quotient % 2):
        quotient += 1
    if quotient == 10 ** 110:
        quotient //= 10
        exponent += 1
    digits = str(quotient)
    assert len(digits) == 110
    signed = -quotient if sign == "-" else quotient
    exp_sign = "+" if exponent >= 0 else "-"
    text = f"{sign}{digits[0]}.{digits[1:]}e{exp_sign}{abs(exponent):05d}"
    parsed = Fraction(signed) * power10(exponent - 109)
    quantum = Fraction(5) * power10(exponent - 110)
    return text, signed, exponent, parsed, quantum


def sidecar_value(value):
    text, signed, exponent, _, _ = canonical_parts(value)
    return SimpleNamespace(text=text, signed_significand=signed,
                           decimal_exponent=exponent)


def primary_value(value):
    text, _, _, parsed, quantum = canonical_parts(value)
    return SimpleNamespace(text=text, value=parsed, quantum=quantum)


def zero_fixture():
    primary, sidecar = bound_pair()
    sidecar_zero = SimpleNamespace(text=ZERO, signed_significand=0,
                                   decimal_exponent=0)
    rows = tuple(SimpleNamespace(slot=slot, values=(sidecar_zero,) * 4)
                 for slot in range(16384))
    sidecar.rows = rows
    bounds = derive_replay_bounds(
        sidecar.meta, maximum_e0_exponent=None, maximum_e8_exponent=None)
    primary_zero = SimpleNamespace(text=ZERO, value=Fraction(0), quantum=Fraction(0))
    tuple_values = tuple((residual, component, primary_zero)
                         for residual in RESIDUALS for component in ("real", "imag"))
    primary.endpoint.maxima = tuple(
        SimpleNamespace(
            residual_id=residual,
            magnitude=primary_zero,
            magnitude_exact=Fraction(0),
            magnitude_quantum=Fraction(0),
            allowance=getattr(bounds, "live_" + residual.lower()),
            argmax_slot=0,
            argmax_component="real",
            tuple_values=tuple_values,
        )
        for residual in RESIDUALS
    )
    scalar = replay_row(0, rows[0])
    signed = (scalar.e0, scalar.e8, scalar.i8, scalar.a8)
    zero_metric = ReplayMetric(Fraction(0), 0, "real", signed)
    replay_result = ReplayResult(
        16384, bounds, zero_metric, zero_metric, zero_metric, zero_metric,
        ReplayMetric(Fraction(0), 0, "real", signed),
    )
    return primary, sidecar, replay_result


def metric_fixture(row_values):
    """Use public one-row replay only for explicitly selected synthetic rows."""
    primary, sidecar = bound_pair()
    zero = sidecar_value(Fraction(0))
    rows = [SimpleNamespace(slot=slot, values=(zero,) * 4) for slot in range(16384)]
    records = []
    if 0 not in row_values:
        records.append((0, replay_row(0, rows[0])))
    maximum_exponents = [None, None]
    for slot, values in sorted(row_values.items()):
        row = SimpleNamespace(slot=slot, values=tuple(sidecar_value(value) for value in values))
        rows[slot] = row
        records.append((slot, replay_row(slot, row)))
        for index, value in enumerate(values):
            if value == 0:
                continue
            group = 0 if index < 2 else 1
            exponent = canonical_parts(value)[2]
            current = maximum_exponents[group]
            maximum_exponents[group] = exponent if current is None else max(current, exponent)
    sidecar.rows = tuple(rows)
    summary = summarize_scalars(tuple(records))
    bounds = derive_replay_bounds(
        sidecar.meta, maximum_e0_exponent=maximum_exponents[0],
        maximum_e8_exponent=maximum_exponents[1])
    replay_result = ReplayResult(
        16384, bounds, summary.e0, summary.e8, summary.i8, summary.a8,
        summary.r_identity)
    metric_map = dict(E0=summary.e0, E8=summary.e8, I8=summary.i8, A8=summary.a8)
    maxima = []
    for residual in RESIDUALS:
        metric = metric_map[residual]
        scalar = replay_row(metric.argmax_slot, sidecar.rows[metric.argmax_slot])
        vectors = (scalar.e0, scalar.e8, scalar.i8, scalar.a8)
        tuples = tuple(
            (name, component, primary_value(vectors[index][component_index]))
            for index, name in enumerate(RESIDUALS)
            for component_index, component in enumerate(("real", "imag"))
        )
        magnitude = primary_value(metric.maximum)
        maxima.append(SimpleNamespace(
            residual_id=residual, magnitude=magnitude,
            magnitude_exact=metric.maximum, magnitude_quantum=magnitude.quantum,
            allowance=getattr(bounds, "live_" + residual.lower()),
            argmax_slot=metric.argmax_slot, argmax_component=metric.argmax_component,
            tuple_values=tuples))
    primary.endpoint.maxima = tuple(maxima)
    return primary, sidecar, replay_result


def replace_primary_tuple_at_slot(primary, slot, residual, component, value):
    for maximum in primary.endpoint.maxima:
        if maximum.argmax_slot != slot:
            continue
        tuples = list(maximum.tuple_values)
        index = next(index for index, item in enumerate(tuples)
                     if item[:2] == (residual, component))
        tuples[index] = residual, component, value
        maximum.tuple_values = tuple(tuples)


def primary_maximum_at(primary, sidecar, bounds, residual, slot, component):
    scalar = replay_row(slot, sidecar.rows[slot])
    vectors = (scalar.e0, scalar.e8, scalar.i8, scalar.a8)
    tuples = tuple(
        (name, name_component, primary_value(vectors[index][component_index]))
        for index, name in enumerate(RESIDUALS)
        for component_index, name_component in enumerate(("real", "imag"))
    )
    selected = vectors[RESIDUALS.index(residual)][0 if component == "real" else 1]
    magnitude = primary_value(abs(selected))
    return SimpleNamespace(
        residual_id=residual, magnitude=magnitude, magnitude_exact=abs(selected),
        magnitude_quantum=magnitude.quantum,
        allowance=getattr(bounds, "live_" + residual.lower()),
        argmax_slot=slot, argmax_component=component, tuple_values=tuples)


class ReplayReconcileTests(unittest.TestCase):
    def test_zero_residual_selected_row_and_analytic_summary_reconcile(self):
        primary, sidecar, replay_result = zero_fixture()
        self.assertIsNone(reconcile.reconcile_replay(primary, sidecar, replay_result))

    def test_bounds_environment_and_each_replay_metric_are_independently_bound(self):
        mutations = (
            lambda result: replace(result, row_count=1),
            lambda result: replace(result, decimal_precision=255),
            lambda result: replace(result, decimal_rounding="ROUND_DOWN"),
            lambda result: replace(
                result, bounds=replace(result.bounds,
                                       serialization_e0=Fraction(1, 10 ** 200))),
        )
        for mutate in mutations:
            primary, sidecar, replay_result = zero_fixture()
            with self.assertRaises(reconcile.ReconcileError):
                reconcile.reconcile_replay(primary, sidecar, mutate(replay_result))
        for field in ("e0", "e8", "i8", "a8", "r_identity"):
            for residual_index, residual in enumerate(RESIDUALS):
                for component_index, component in enumerate(("real", "imag")):
                    primary, sidecar, replay_result = zero_fixture()
                    metric = getattr(replay_result, field)
                    signed = list(metric.signed_tuple)
                    vector = list(signed[residual_index])
                    vector[component_index] = Fraction(1, 1 << 500)
                    signed[residual_index] = tuple(vector)
                    forged = replace(metric, signed_tuple=tuple(signed))
                    with self.subTest(
                            metric=field,
                            signed_field=f"{residual}.{component}"), \
                            self.assertRaises(reconcile.ReconcileError):
                        reconcile.reconcile_replay(
                            primary, sidecar,
                            replace(replay_result, **{field: forged}))

    def test_selected_row_replay_failure_preserves_typed_reason(self):
        primary, sidecar, replay_result = zero_fixture()
        malformed = SimpleNamespace(
            text="not-a-canonical-decimal", signed_significand=0,
            decimal_exponent=-99999)
        rows = list(sidecar.rows)
        rows[0] = SimpleNamespace(
            slot=0, values=(malformed, *rows[0].values[1:]))
        sidecar.rows = tuple(rows)
        replay_result = replace(
            replay_result,
            bounds=derive_replay_bounds(
                sidecar.meta, maximum_e0_exponent=-99999,
                maximum_e8_exponent=None))

        with self.assertRaises(reconcile.ReconcileError) as caught:
            reconcile.reconcile_replay(primary, sidecar, replay_result)
        self.assertEqual(caught.exception.reason, "FORMAT")

    def test_actual_reader_dataclasses_reconcile_zero_metrics(self):
        from pathlib import Path
        import tempfile

        import paper_endpoint_primary_reader as primary_reader
        import paper_endpoint_sidecar_reader as sidecar_reader
        import test_paper_endpoint_primary_reader as primary_fixture
        import test_paper_endpoint_sidecar_reader as sidecar_fixture

        identity = primary_reader.PrimaryIdentity("a" * 40, "linux", "17", "2")
        sidecar_bytes = sidecar_fixture.valid_bytes().replace(
            b"meta\tnumeric_gate_failures\t7\n",
            b"meta\tnumeric_gate_failures\t2\n")
        primary_bytes = primary_fixture.ctest_log(
            primary_fixture.complete_lines(
                2, scope="synthetic", fresh768=Fraction(1, 1 << 854),
                terminal768=Fraction(1, 1 << 854)))
        primary_bytes = primary_bytes.replace(
            primary_fixture.SOURCE.encode("ascii"), b"a" * 40).replace(
                b"github_run_id=101\t", b"github_run_id=17\t")

        with tempfile.TemporaryDirectory(
                prefix="fs-endpoint-synthetic-metric-reader-") as temporary:
            sidecar_path = Path(temporary).resolve() / "synthetic.tsv"
            sidecar_path.write_bytes(sidecar_bytes)
            sidecar = sidecar_reader.read_sidecar(
                sidecar_path, expected_scope="synthetic",
                expected_source_commit="a" * 40, expected_host="linux",
                expected_run_id="17", expected_run_attempt="2")
            primary = primary_reader.parse_primary_log(
                primary_bytes, identity, ctest_exit_code=8,
                expected_scope="synthetic")

        bounds = derive_replay_bounds(
            sidecar.meta, maximum_e0_exponent=None,
            maximum_e8_exponent=None)
        scalar = replay_row(0, sidecar.rows[0])
        signed = (scalar.e0, scalar.e8, scalar.i8, scalar.a8)
        zero_metric = ReplayMetric(Fraction(0), 0, "real", signed)
        replay_result = ReplayResult(
            16384, bounds, zero_metric, zero_metric, zero_metric, zero_metric,
            ReplayMetric(Fraction(0), 0, "real", signed))

        self.assertIsNone(
            reconcile.reconcile_replay(primary, sidecar, replay_result))

    def test_one_last_digit_e0_change_fails_exact_byte_binding(self):
        base = Fraction(1, 10 ** 200)
        primary, sidecar, replay_result = metric_fixture({7: (base, 0, 0, 0)})
        alternate = primary_value(base + Fraction(1, 10 ** 309))
        slot = primary.endpoint.maxima[0].argmax_slot
        replace_primary_tuple_at_slot(primary, slot, "E0", "real", alternate)
        primary.endpoint.maxima[0].magnitude = primary_value(abs(alternate.value))
        primary.endpoint.maxima[0].magnitude_exact = abs(alternate.value)
        with self.assertRaises(reconcile.ReconcileError) as caught:
            reconcile.reconcile_replay(primary, sidecar, replay_result)
        self.assertEqual(caught.exception.reason, "INTEGRITY")
        self.assertIn("byte mismatch", caught.exception.detail)

    def test_all_eight_primary_tuple_fields_and_same_slot_copy_must_agree(self):
        for residual in RESIDUALS:
            for component in ("real", "imag"):
                primary, sidecar, replay_result = zero_fixture()
                # This remains far below 2^-120 but is larger than every
                # applicable conditional reconciliation allowance.
                changed = primary_value(Fraction(1, 10 ** 100))
                replace_primary_tuple_at_slot(primary, 0, residual, component, changed)
                with self.subTest(field=f"{residual}.{component}"), \
                        self.assertRaises(reconcile.ReconcileError):
                    reconcile.reconcile_replay(primary, sidecar, replay_result)

        primary, sidecar, replay_result = zero_fixture()
        tuples = list(primary.endpoint.maxima[1].tuple_values)
        tuples[4] = "I8", "real", primary_value(Fraction(1, 10 ** 200))
        primary.endpoint.maxima[1].tuple_values = tuple(tuples)
        with self.assertRaises(reconcile.ReconcileError) as caught:
            reconcile.reconcile_replay(primary, sidecar, replay_result)
        self.assertIn("one selected slot", caught.exception.detail)

    def test_equal_absolute_i8_value_with_opposite_sign_is_not_accepted(self):
        primary, sidecar, replay_result = metric_fixture(
            {7: (Fraction(1, 10 ** 40), 0, 0, 0)})
        maximum = primary.endpoint.maxima[2]
        key = ("I8", maximum.argmax_component)
        selected = next(value for residual, component, value in maximum.tuple_values
                        if (residual, component) == key)
        opposite = primary_value(-selected.value)
        replace_primary_tuple_at_slot(primary, maximum.argmax_slot, *key, opposite)
        with self.assertRaises(reconcile.ReconcileError) as caught:
            reconcile.reconcile_replay(primary, sidecar, replay_result)
        self.assertEqual(caught.exception.reason, "INTEGRITY")

    def test_distinct_a8_argmax_is_allowed_only_within_combined_allowance(self):
        base = Fraction(1, 10 ** 40)
        cases = ((Fraction(1, 10 ** 110), True), (Fraction(1, 10 ** 100), False))
        for gap, accepted in cases:
            primary, sidecar, replay_result = metric_fixture(
                {7: (0, 0, base, 0), 8: (0, 0, base - gap, 0)})
            primary.endpoint.maxima = (*primary.endpoint.maxima[:3],
                primary_maximum_at(primary, sidecar, replay_result.bounds,
                                   "A8", 8, "real"))
            if accepted:
                self.assertIsNone(reconcile.reconcile_replay(primary, sidecar, replay_result))
            else:
                with self.assertRaises(reconcile.ReconcileError) as caught:
                    reconcile.reconcile_replay(primary, sidecar, replay_result)
                self.assertEqual(caught.exception.reason, "INTEGRITY")

    def test_raw_excess_precedes_an_excess_component_quantum(self):
        primary, sidecar, replay_result = zero_fixture()
        # Its q=5e-38 exceeds 2^-128, while the raw discrepancy itself is
        # already above 2^-120 and therefore has first-cause precedence.
        excessive = primary_value(Fraction(10 ** 72))
        replace_primary_tuple_at_slot(primary, 0, "I8", "real", excessive)
        primary.endpoint.maxima[2].magnitude = primary_value(abs(excessive.value))
        primary.endpoint.maxima[2].magnitude_exact = abs(excessive.value)
        primary.endpoint.maxima[2].magnitude_quantum = excessive.quantum
        with self.assertRaises(reconcile.ReconcileError) as caught:
            reconcile.reconcile_replay(primary, sidecar, replay_result)
        self.assertEqual(caught.exception.reason, "INTEGRITY")
        self.assertIn("2^-120", caught.exception.detail)


if __name__ == "__main__":
    unittest.main()
