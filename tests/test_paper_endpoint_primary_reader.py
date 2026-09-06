import unittest
from fractions import Fraction

import paper_endpoint_primary_reader as reader


SOURCE = "1234567890abcdef1234567890abcdef12345678"
OPENFHE = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
IDENTITY = reader.PrimaryIdentity(SOURCE, "linux", "101", "2")
Q = (
    1125899904679937, 1125899903827969,
    1152921504598720513, 1152921504597016577,
    1152921504595968001, 1152921504595640321,
    1152921504593412097, 1152921504592822273,
    1152921504592429057, 1152921504589938689,
    1099510054913,
)
DIVISOR = 1099510054913
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
RESIDUALS = ("E0", "E8", "I8", "A8")
ZERO = "+0." + "0" * 109 + "e+00000"
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


def power_two(exponent):
    return Fraction(1 << exponent, 1) if exponent >= 0 else Fraction(1, 1 << -exponent)


def model_bounds(bits, fresh768=Fraction(0), terminal768=Fraction(0)):
    factor = 1 << (768 - bits)
    fresh = fresh768 * factor
    terminal = terminal768 * factor
    power = power_two(270 - bits)
    subtraction = power_two(264 - bits)
    propagated = power + (1 << 263) * fresh
    residuals = (fresh + subtraction, terminal + power + subtraction,
                 propagated + power + subtraction,
                 terminal + propagated + subtraction)
    identity = residuals[1] + residuals[2] + residuals[3] + 2 * subtraction
    return fresh, terminal, residuals, identity


def model_allowances(fresh768=Fraction(0), terminal768=Fraction(0)):
    result = []
    for exponent in (0, 0, 0, 3):
        direct = power_two(exponent + 10 - 768)
        result.extend((power_two(exponent + 12 - 512) + direct,
                       power_two(exponent + 12 - 768) + direct))
    fresh512, terminal512, residual512, identity512 = model_bounds(
        512, fresh768, terminal768)
    _, _, residual768, identity768 = model_bounds(768, fresh768, terminal768)
    result.extend((fresh512 + fresh768, terminal512 + terminal768,
                   fresh512 + fresh512 * (1 << 12),
                   fresh768 + fresh512 * (1 << 12),
                   terminal512 + terminal512 * (1 << 12),
                   terminal768 + terminal512 * (1 << 12)))
    transport = power_two(-300)
    result.extend((fresh512 + transport, fresh768 + transport,
                   terminal512 + transport, terminal768 + transport))
    result.extend(left + right for left, right in zip(residual512, residual768))
    result.extend((identity512, identity768))
    assert len(result) == 24
    return tuple(result), residual768


def decimal_integer(value):
    if value == 0:
        return "0"
    chunks = []
    while value:
        value, chunk = divmod(value, 1_000_000_000)
        chunks.append(chunk)
    return str(chunks[-1]) + "".join(f"{chunk:09d}" for chunk in reversed(chunks[:-1]))


def exact_scales():
    scales = [(1 << 100, 1)]
    for operation in range(1, 9):
        prior_n, prior_d = scales[-1]
        numerator = prior_n * prior_n
        denominator = prior_d * prior_d * DIVISOR * Q[10 - operation]
        common = __import__("math").gcd(numerator, denominator)
        scales.append((numerator // common, denominator // common))
    return tuple(scales)


def maximum_line(residual, allowance=None):
    if allowance is None:
        allowance = model_bounds(768)[2][RESIDUALS.index(residual)]
    fields = [
        "FS_ENDPOINT_MAX", f"id={residual}", f"magnitude={ZERO}",
        "magnitude_exact_num=0", "magnitude_exact_den=1",
        "magnitude_quantum_num=0", "magnitude_quantum_den=1",
        f"allowance_num={allowance.numerator}", f"allowance_den={allowance.denominator}",
        "interval_lower_num=0", "interval_lower_den=1",
        f"interval_upper_num={allowance.numerator}",
        f"interval_upper_den={allowance.denominator}",
        "argmax_slot=0", "argmax_component=real",
    ]
    for name in RESIDUALS:
        for component in ("real", "imag"):
            field = f"{name}.{component}"
            fields.extend((f"{field}={ZERO}", f"{field}_q_num=0", f"{field}_q_den=1"))
    assert len(fields) == 39
    return "\t".join(fields)


def nonzero_maximum_line(residual, exact, canonical, quantum, component, signed, slot):
    allowance = model_bounds(768)[2][RESIDUALS.index(residual)]
    lower = exact - allowance
    upper = exact + allowance
    fields = [
        "FS_ENDPOINT_MAX", f"id={residual}", f"magnitude={canonical}",
        f"magnitude_exact_num={decimal_integer(exact.numerator)}",
        f"magnitude_exact_den={decimal_integer(exact.denominator)}",
        f"magnitude_quantum_num={decimal_integer(quantum.numerator)}",
        f"magnitude_quantum_den={decimal_integer(quantum.denominator)}",
        f"allowance_num={allowance.numerator}", f"allowance_den={allowance.denominator}",
        f"interval_lower_num={decimal_integer(lower.numerator)}",
        f"interval_lower_den={decimal_integer(lower.denominator)}",
        f"interval_upper_num={decimal_integer(upper.numerator)}",
        f"interval_upper_den={decimal_integer(upper.denominator)}",
        f"argmax_slot={slot}", f"argmax_component={component}",
    ]
    for name in RESIDUALS:
        for tuple_component in ("real", "imag"):
            field = f"{name}.{tuple_component}"
            value = signed if (name, tuple_component) == (residual, component) else ZERO
            q = quantum if value != ZERO else Fraction(0)
            fields.extend((f"{field}={value}", f"{field}_q_num={q.numerator}",
                           f"{field}_q_den={q.denominator}"))
    assert len(fields) == 39
    return "\t".join(fields)


def nonzero_carry_maximum_line(residual):
    canonical = "+1." + "0" * 109 + "e+00111"
    signed = "-1." + "0" * 109 + "e+00111"
    return nonzero_maximum_line(
        residual, Fraction(10 ** 111 - 5), canonical, Fraction(50), "imag", signed, 7,
    )


def replace_line(lines, prefix, replacement):
    result = list(lines)
    index = next(index for index, line in enumerate(result) if line.startswith(prefix))
    result[index] = replacement(result[index]) if callable(replacement) else replacement
    return result


def complete_lines(numeric_failures, *, scope="live-single-chain", host="linux",
                   fresh768=Fraction(0), terminal768=Fraction(0)):
    if not 0 <= numeric_failures <= len(NUMERIC_GATE_LABELS):
        raise ValueError("fixture numeric failure count")
    scales = exact_scales()
    lines = [
        ("BEGIN test=paper_full_eight_square_contract source=" + SOURCE +
         " openfhe_pin=" + OPENFHE +
         " native=64 backend=4 N=32768 M=65536 slots=16384 gap=1 h=128 nominal=50"+
         " P=1152921504606584833 P_root=4443670208963 chain_count=1"),
    ]
    for operation, (numerator, denominator) in enumerate(scales):
        family = 7 if operation == 8 else operation
        local_level = 2 if operation == 8 else 1
        terminal = 1 if operation == 8 else 0
        lines.append(
            f"RECEIPT operation={operation} family={family} local_level={local_level} "
            f"towers={10 - operation} recorded_exp2=100 degree=2 "
            f"exact_n={decimal_integer(numerator)} exact_d={decimal_integer(denominator)} "
            f"terminal={terminal}"
        )
    for label in NUMERIC_GATE_LABELS[:numeric_failures]:
        lines.append(f"OBS numeric_gate=FAIL label={label}")
    lines.extend((
        "OBS lifecycle=paper_owner_cleanup owned_absent=8 unrelated_unchanged=2 result=PASS",
        f"OBS numeric_gate_failures={numeric_failures}",
        ("FS_ENDPOINT_BEGIN\tschema=fs-residual-endpoint-primary-v1-r1"
         f"\tscope={scope}\tsource_commit=" + SOURCE +
         f"\thost={host}\tgithub_run_id=101\tgithub_run_attempt=2\tboost_version=108300"),
    ))
    for index, (numerator, denominator) in enumerate(scales):
        lines.append(
            f"FS_ENDPOINT_SCALE\tindex={index}\tnumerator={decimal_integer(numerator)}"
            f"\tdenominator={decimal_integer(denominator)}"
        )
    for check_id, allowance in zip(CHECK_IDS, model_allowances(fresh768, terminal768)[0]):
        lines.append(
            f"FS_ENDPOINT_CHECK\tid={check_id}\tresult=PASS\tdistance_num=0"
            f"\tdistance_den=1\tallowance_num={allowance.numerator}"
            f"\tallowance_den={allowance.denominator}"
            "\targmax_slot=0\targmax_component=real"
        )
    residual_allowances = model_allowances(fresh768, terminal768)[1]
    lines.extend(maximum_line(residual, allowance)
                 for residual, allowance in zip(RESIDUALS, residual_allowances))
    disposition = "PASS" if numeric_failures == 0 else "FAIL"
    lines.append(
        "FS_ENDPOINT_COMPLETE\tresult=PASS\tassurance=CONDITIONAL\trow_count=16384"
        f"\tcheck_count=24\tnumeric_gate_failures={numeric_failures}"
        f"\tE80_disposition={disposition}\tA_disposition=NOT_ADOPTED"
        "\towner_cleanup_confirmed=true"
    )
    if numeric_failures == 0:
        lines.append(
            "COMPLETE test=paper_full_eight_square_contract result=PASS source=" + SOURCE +
            " openfhe_pin=" + OPENFHE + " chain_count=1 squares=8 full_slots=16384"
            " anchors=10 error_gate=2^-80 codec_gate=2^-120"
            " gaussian_global_guarantee=false"
        )
    else:
        lines.append(
            "COMPLETE test=paper_full_eight_square_contract result=FAIL source=" + SOURCE +
            " openfhe_pin=" + OPENFHE +
            f" reason=paper contract: accumulated numeric acceptance failures: {numeric_failures}"
        )
    return lines


def ctest_log(lines, *, unprefixed_replay=False, line_ending=b"\n"):
    primary = b"".join(("61: " + line).encode("ascii") + line_ending for line in lines)
    if unprefixed_replay:
        primary += line_ending.join(line.encode("ascii") for line in lines) + line_ending
    return primary


class PrimaryReaderTests(unittest.TestCase):
    def test_complete_nonzero_e80_failure_remains_complete_evidence(self):
        parsed = reader.parse_primary_log(
            ctest_log(complete_lines(2), unprefixed_replay=True), IDENTITY,
            ctest_exit_code=8,
        )
        self.assertEqual(parsed.evidence_state, "COMPLETE")
        self.assertEqual(parsed.reason, "NONE")
        self.assertEqual(parsed.numeric_gate_failures, 2)
        self.assertEqual(parsed.numeric_gate_labels,
                         NUMERIC_GATE_LABELS[:2])
        self.assertEqual(parsed.e80_disposition, "FAIL")
        self.assertEqual(parsed.original_result, "FAIL")
        self.assertEqual(parsed.ctest_exit_code, 8)
        self.assertEqual(tuple(scale.operation for scale in parsed.legacy_scales), tuple(range(9)))
        self.assertEqual(len(parsed.endpoint.checks), 24)
        self.assertEqual(len(parsed.endpoint.maxima), 4)

    def test_complete_zero_numeric_failures_is_the_only_successful_status(self):
        parsed = reader.parse_primary_log(
            ctest_log(complete_lines(0)), IDENTITY, ctest_exit_code=0,
        )
        self.assertEqual(parsed.evidence_state, "COMPLETE")
        self.assertEqual(parsed.numeric_gate_failures, 0)
        self.assertEqual(parsed.numeric_gate_labels, ())
        self.assertEqual(parsed.e80_disposition, "PASS")
        self.assertEqual(parsed.original_result, "PASS")

    def test_typed_nonfinite_precedes_generic_failure_without_begin(self):
        lines = [
            "FS_ENDPOINT_FAILURE reason=NONFINITE detail=E8.real was not finite",
            ("COMPLETE test=paper_full_eight_square_contract result=FAIL source=" + SOURCE +
             " openfhe_pin=" + OPENFHE + " reason=endpoint evidence failed"),
        ]
        parsed = reader.parse_primary_log(ctest_log(lines), IDENTITY, ctest_exit_code=1)
        self.assertEqual((parsed.evidence_state, parsed.reason), ("FATAL", "NONFINITE"))
        self.assertEqual(parsed.first_failure.detail, "E8.real was not finite")
        self.assertIsNone(parsed.numeric_gate_failures)
        self.assertEqual(parsed.e80_disposition, "NOT_OBSERVED")

    def test_untyped_legacy_failure_is_fatal_without_inventing_endpoint_context(self):
        lines = [
            ("COMPLETE test=paper_full_eight_square_contract result=FAIL source=" + SOURCE +
             " openfhe_pin=" + OPENFHE + " reason=unexpected standard exception"),
        ]
        parsed = reader.parse_primary_log(ctest_log(lines), IDENTITY, ctest_exit_code=1)
        self.assertEqual((parsed.evidence_state, parsed.reason), ("FATAL", "CTEST_FATAL"))
        self.assertIsNone(parsed.first_failure)
        self.assertEqual(parsed.original_reason, "unexpected standard exception")

    def test_no_primary_record_may_follow_legacy_complete(self):
        complete = complete_lines(0)
        lines = [
            complete[0],
            ("COMPLETE test=paper_full_eight_square_contract result=FAIL source=" + SOURCE +
             " openfhe_pin=" + OPENFHE + " reason=stopped early"),
            complete[1],
        ]
        with self.assertRaisesRegex(reader.PrimaryLogError, "REPLAY"):
            reader.parse_primary_log(ctest_log(lines), IDENTITY, ctest_exit_code=1)

    def test_typed_model_failure_is_unresolved_and_timeout_does_not_replace_it(self):
        lines = [
            "FS_ENDPOINT_FAILURE reason=MODEL_UNSUPPORTED detail=runtime model is unavailable",
            ("COMPLETE test=paper_full_eight_square_contract result=FAIL source=" + SOURCE +
             " openfhe_pin=" + OPENFHE + " reason=runtime model is unavailable"),
        ]
        parsed = reader.parse_primary_log(
            ctest_log(lines), IDENTITY, ctest_exit_code=124, timed_out=True,
        )
        self.assertEqual((parsed.evidence_state, parsed.reason),
                         ("UNRESOLVED", "MODEL_UNSUPPORTED"))

    def test_later_malformed_line_error_carries_the_first_typed_failure(self):
        log = ctest_log([
            "FS_ENDPOINT_FAILURE reason=NONFINITE detail=first reliable cause",
        ]) + b"61: " + b"x" * 32764 + b"\n"
        with self.assertRaises(reader.PrimaryLogError) as caught:
            reader.parse_primary_log(log, IDENTITY, ctest_exit_code=1)
        self.assertEqual(caught.exception.reason, "FORMAT")
        self.assertEqual(caught.exception.first_failure.reason, "NONFINITE")

    def test_timeout_and_unexplained_absence_are_distinct_incomplete_states(self):
        timeout = reader.parse_primary_log(b"", IDENTITY, ctest_exit_code=124, timed_out=True)
        missing = reader.parse_primary_log(b"", IDENTITY, ctest_exit_code=8)
        self.assertEqual((timeout.evidence_state, timeout.reason), ("FATAL", "TIMEOUT"))
        self.assertEqual(timeout.e80_disposition, "NOT_OBSERVED")
        self.assertEqual((missing.evidence_state, missing.reason), ("MISSING", "NO_CANONICAL"))

    def test_prefixed_duplicate_is_replay_but_unprefixed_replay_is_ignored(self):
        lines = complete_lines(0)
        parsed = reader.parse_primary_log(
            ctest_log(lines, unprefixed_replay=True), IDENTITY, ctest_exit_code=0,
        )
        self.assertEqual(parsed.evidence_state, "COMPLETE")
        with self.assertRaisesRegex(reader.PrimaryLogError, "REPLAY"):
            reader.parse_primary_log(ctest_log([lines[0], *lines]), IDENTITY,
                                     ctest_exit_code=0)

    def test_numeric_failures_are_an_ordered_subset_of_frozen_gate_labels(self):
        arbitrary = replace_line(
            complete_lines(1), "OBS numeric_gate=FAIL label=",
            "OBS numeric_gate=FAIL label=synthetic finite gate",
        )
        reversed_labels = complete_lines(2)
        first = next(index for index, line in enumerate(reversed_labels)
                     if line.startswith("OBS numeric_gate=FAIL label="))
        reversed_labels[first], reversed_labels[first + 1] = (
            reversed_labels[first + 1], reversed_labels[first])
        for lines in (arbitrary, reversed_labels):
            with self.assertRaisesRegex(reader.PrimaryLogError, "numeric gate label"):
                reader.parse_primary_log(ctest_log(lines), IDENTITY, ctest_exit_code=8)

    def test_unknown_selected_endpoint_record_is_rejected(self):
        with self.assertRaisesRegex(reader.PrimaryLogError, "unknown FS_ENDPOINT"):
            reader.parse_primary_log(
                ctest_log(["FS_ENDPOINT_UNKNOWN\tvalue=1"]), IDENTITY,
                ctest_exit_code=8,
            )

    def test_foreign_endpoint_identity_is_rejected(self):
        lines = replace_line(
            complete_lines(0), "FS_ENDPOINT_BEGIN",
            lambda line: line.replace("\thost=linux\t", "\thost=windows\t"),
        )
        with self.assertRaisesRegex(reader.PrimaryLogError, "IDENTITY"):
            reader.parse_primary_log(ctest_log(lines), IDENTITY, ctest_exit_code=0)

    def test_scope_defaults_live_and_explicit_synthetic_is_strictly_matched(self):
        synthetic = ctest_log(complete_lines(0, scope="synthetic"))
        with self.assertRaisesRegex(reader.PrimaryLogError, "scope"):
            reader.parse_primary_log(synthetic, IDENTITY, ctest_exit_code=0)
        parsed = reader.parse_primary_log(
            synthetic, IDENTITY, ctest_exit_code=0, expected_scope="synthetic",
        )
        self.assertEqual(parsed.endpoint.scope, "synthetic")
        with self.assertRaisesRegex(reader.PrimaryLogError, "scope"):
            reader.parse_primary_log(
                ctest_log(complete_lines(0)), IDENTITY, ctest_exit_code=0,
                expected_scope="synthetic",
            )

    def test_foreign_check_id_and_reordered_maximum_are_rejected(self):
        foreign = replace_line(
            complete_lines(0), "FS_ENDPOINT_CHECK\tid=control.constant.512\t",
            lambda line: line.replace("id=control.constant.512", "id=foreign.check"),
        )
        reordered = list(complete_lines(0))
        first = next(index for index, line in enumerate(reordered)
                     if line.startswith("FS_ENDPOINT_MAX\tid=E0\t"))
        reordered[first], reordered[first + 1] = reordered[first + 1], reordered[first]
        for lines in (foreign, reordered):
            with self.assertRaisesRegex(reader.PrimaryLogError, "REPLAY"):
                reader.parse_primary_log(ctest_log(lines), IDENTITY, ctest_exit_code=0)

    def test_all_nine_scales_are_independently_checked(self):
        for index in range(9):
            lines = replace_line(
                complete_lines(0), f"FS_ENDPOINT_SCALE\tindex={index}\t",
                lambda line: line.replace("\tnumerator=", "\tnumerator=2", 1),
            )
            with self.subTest(index=index), self.assertRaisesRegex(
                    reader.PrimaryLogError, "INTEGRITY"):
                reader.parse_primary_log(ctest_log(lines), IDENTITY, ctest_exit_code=0)

    def test_nonzero_signed_tuple_uses_literal_half_even_carry_and_interval(self):
        lines = replace_line(complete_lines(0), "FS_ENDPOINT_MAX\tid=E0\t",
                             nonzero_carry_maximum_line("E0"))
        parsed = reader.parse_primary_log(ctest_log(lines), IDENTITY, ctest_exit_code=0)
        maximum = parsed.endpoint.maxima[0]
        self.assertEqual(maximum.magnitude.text, "+1." + "0" * 109 + "e+00111")
        self.assertEqual(maximum.magnitude_exact, Fraction(10 ** 111 - 5))
        self.assertEqual(maximum.magnitude_quantum, Fraction(50))
        selected = next(value for residual, component, value in maximum.tuple_values
                        if (residual, component) == ("E0", "imag"))
        self.assertTrue(selected.text.startswith("-1."))
        self.assertEqual(maximum.interval_upper,
                         maximum.magnitude_exact + maximum.allowance)

    def test_all_four_maxima_retain_nonzero_signed_real_and_imaginary_tuples(self):
        cases = (
            ("E0", Fraction(10 ** 111 - 5), "+1." + "0" * 109 + "e+00111",
             Fraction(50), "imag", "-1." + "0" * 109 + "e+00111", 7),
            ("E8", Fraction(1, 2), "+5." + "0" * 109 + "e-00001",
             Fraction(1, 2 * 10 ** 110), "real", "-5." + "0" * 109 + "e-00001", 8),
            ("I8", Fraction(1), "+1." + "0" * 109 + "e+00000",
             Fraction(1, 2 * 10 ** 109), "imag", "+1." + "0" * 109 + "e+00000", 9),
            ("A8", Fraction(3, 2), "+1." + "5" + "0" * 108 + "e+00000",
             Fraction(1, 2 * 10 ** 109), "real", "-1." + "5" + "0" * 108 + "e+00000", 10),
        )
        lines = complete_lines(0)
        for case in cases:
            lines = replace_line(lines, f"FS_ENDPOINT_MAX\tid={case[0]}\t",
                                 nonzero_maximum_line(*case))
        parsed = reader.parse_primary_log(ctest_log(lines), IDENTITY, ctest_exit_code=0)
        self.assertEqual(tuple(maximum.argmax_component for maximum in parsed.endpoint.maxima),
                         ("imag", "real", "imag", "real"))
        self.assertEqual(tuple(len(maximum.tuple_values) for maximum in parsed.endpoint.maxima),
                         (8, 8, 8, 8))
        for maximum in parsed.endpoint.maxima:
            selected = next(value for residual, component, value in maximum.tuple_values
                            if (residual, component) ==
                            (maximum.residual_id, maximum.argmax_component))
            self.assertEqual(abs(selected.value), maximum.magnitude.value)

    def test_exact_measured_rationals_must_be_dyadic(self):
        non_dyadic_check = replace_line(
            complete_lines(0), "FS_ENDPOINT_CHECK\tid=fresh.producer.512\t",
            lambda line: line.replace("distance_num=0\tdistance_den=1",
                                      f"distance_num=1\tdistance_den={3 * (1 << 301)}"),
        )
        non_dyadic_maximum = replace_line(
            complete_lines(0), "FS_ENDPOINT_MAX\tid=E0\t",
            nonzero_maximum_line(
                "E0", Fraction(1, 10), "+1." + "0" * 109 + "e-00001",
                Fraction(1, 2 * 10 ** 110), "real",
                "+1." + "0" * 109 + "e-00001", 7,
            ),
        )
        for lines in (non_dyadic_check, non_dyadic_maximum):
            with self.assertRaisesRegex(reader.PrimaryLogError, "dyadic"):
                reader.parse_primary_log(ctest_log(lines), IDENTITY, ctest_exit_code=0)

    def test_classifier_distinguishes_producer_from_two_bounded_paths(self):
        distance = Fraction(1, 1 << 200)
        producer = replace_line(
            complete_lines(0), "FS_ENDPOINT_CHECK\tid=fresh.producer.512\t",
            lambda line: line.replace("distance_num=0\tdistance_den=1",
                                      f"distance_num=1\tdistance_den={1 << 200}"),
        )
        parsed = reader.parse_primary_log(ctest_log(producer), IDENTITY, ctest_exit_code=0)
        producer_check = next(check for check in parsed.endpoint.checks
                              if check.check_id == "fresh.producer.512")
        self.assertEqual(producer_check.distance, distance)
        self.assertGreater(producer_check.distance, producer_check.allowance)

        two_paths = replace_line(
            complete_lines(0), "FS_ENDPOINT_CHECK\tid=fresh.cross\t",
            lambda line: line.replace("distance_num=0\tdistance_den=1",
                                      f"distance_num=1\tdistance_den={1 << 200}"),
        )
        with self.assertRaisesRegex(reader.PrimaryLogError, "INTEGRITY"):
            reader.parse_primary_log(ctest_log(two_paths), IDENTITY, ctest_exit_code=0)

    def test_check_allowance_must_match_the_model_recovered_from_maxima(self):
        lines = replace_line(
            complete_lines(0), "FS_ENDPOINT_CHECK\tid=fresh.cross\t",
            lambda line: line.replace("allowance_num=0\tallowance_den=1",
                                      f"allowance_num=1\tallowance_den={1 << 300}"),
        )
        with self.assertRaisesRegex(reader.PrimaryLogError, "allowance model"):
            reader.parse_primary_log(ctest_log(lines), IDENTITY, ctest_exit_code=0)

    def test_self_consistent_alternate_k_is_deferred_to_sidecar_binding(self):
        fresh768 = power_two(-800)
        terminal768 = power_two(-810)
        parsed = reader.parse_primary_log(
            ctest_log(complete_lines(0, fresh768=fresh768, terminal768=terminal768)),
            IDENTITY, ctest_exit_code=0,
        )
        self.assertEqual(parsed.evidence_state, "COMPLETE")
        self.assertEqual(parsed.endpoint.maxima[0].allowance,
                         fresh768 + power_two(264 - 768))

    def test_malformed_maximum_quantum_and_selected_tuple_are_rejected(self):
        good = replace_line(complete_lines(0), "FS_ENDPOINT_MAX\tid=E0\t",
                            nonzero_carry_maximum_line("E0"))
        bad_quantum = replace_line(
            good, "FS_ENDPOINT_MAX\tid=E0\t",
            lambda line: line.replace("magnitude_quantum_num=50", "magnitude_quantum_num=3"),
        )
        bad_tuple = replace_line(
            good, "FS_ENDPOINT_MAX\tid=E0\t",
            lambda line: line.replace("E0.imag=-1.", "E0.imag=+1.")
                                      .replace("E0.imag=+1." + "0" * 109 + "e+00111",
                                               "E0.imag=+2." + "0" * 109 + "e+00111"),
        )
        for lines in (bad_quantum, bad_tuple):
            with self.assertRaisesRegex(reader.PrimaryLogError, "INTEGRITY"):
                reader.parse_primary_log(ctest_log(lines), IDENTITY, ctest_exit_code=0)

    def test_each_of_the_eight_tuple_quanta_is_checked(self):
        for residual in RESIDUALS:
            for component in ("real", "imag"):
                field = f"{residual}.{component}_q_num=0"
                lines = replace_line(
                    complete_lines(0), "FS_ENDPOINT_MAX\tid=E0\t",
                    lambda line, field=field: line.replace(field, field[:-1] + "1"),
                )
                with self.subTest(field=field), self.assertRaisesRegex(
                        reader.PrimaryLogError, "quantum mismatch"):
                    reader.parse_primary_log(ctest_log(lines), IDENTITY, ctest_exit_code=0)

    def test_missing_or_duplicate_complete_is_not_accepted(self):
        complete = complete_lines(0)
        missing = [line for line in complete if not line.startswith("FS_ENDPOINT_COMPLETE")]
        parsed = reader.parse_primary_log(ctest_log(missing), IDENTITY, ctest_exit_code=0)
        self.assertEqual((parsed.evidence_state, parsed.reason), ("MISSING", "NO_CANONICAL"))
        endpoint_complete = next(line for line in complete if line.startswith("FS_ENDPOINT_COMPLETE"))
        duplicate = replace_line(
            complete, "COMPLETE test=", lambda line: endpoint_complete + "\n61: " + line,
        )
        with self.assertRaisesRegex(reader.PrimaryLogError, "REPLAY"):
            reader.parse_primary_log(ctest_log(duplicate), IDENTITY, ctest_exit_code=0)

    def test_missing_cleanup_or_inconsistent_observed_count_is_rejected(self):
        complete = complete_lines(0)
        missing_cleanup = [line for line in complete
                           if not line.startswith("OBS lifecycle=paper_owner_cleanup")]
        wrong_count = replace_line(
            complete, "OBS numeric_gate_failures=", "OBS numeric_gate_failures=1",
        )
        for lines in (missing_cleanup, wrong_count):
            with self.assertRaises(reader.PrimaryLogError):
                reader.parse_primary_log(ctest_log(lines), IDENTITY, ctest_exit_code=0)

    def test_complete_legacy_e80_survives_eventual_shell_status_failure(self):
        late_failure = reader.parse_primary_log(
            ctest_log(complete_lines(0)), IDENTITY, ctest_exit_code=8,
        )
        swallowed_failure = reader.parse_primary_log(
            ctest_log(complete_lines(9)), IDENTITY, ctest_exit_code=0,
        )
        timeout = reader.parse_primary_log(
            ctest_log(complete_lines(0)), IDENTITY, ctest_exit_code=124,
            timed_out=True,
        )
        self.assertEqual((late_failure.evidence_state, late_failure.reason),
                         ("FATAL", "CTEST_FATAL"))
        self.assertEqual((late_failure.numeric_gate_failures,
                          late_failure.e80_disposition), (0, "PASS"))
        self.assertEqual((swallowed_failure.evidence_state, swallowed_failure.reason),
                         ("FATAL", "INTEGRITY"))
        self.assertEqual((swallowed_failure.numeric_gate_failures,
                          swallowed_failure.e80_disposition), (9, "FAIL"))
        self.assertEqual((timeout.evidence_state, timeout.reason,
                          timeout.numeric_gate_failures, timeout.e80_disposition),
                         ("FATAL", "TIMEOUT", 0, "PASS"))

    def test_missing_endpoint_retains_only_a_complete_legacy_e80_record(self):
        complete = complete_lines(2)
        legacy_only = [line for line in complete if not line.startswith("FS_ENDPOINT_")]
        parsed = reader.parse_primary_log(ctest_log(legacy_only), IDENTITY, ctest_exit_code=8)
        self.assertEqual((parsed.evidence_state, parsed.reason), ("MISSING", "NO_CANONICAL"))
        self.assertEqual((parsed.numeric_gate_failures, parsed.e80_disposition), (2, "FAIL"))

        count_index = next(index for index, line in enumerate(complete)
                           if line.startswith("OBS numeric_gate_failures="))
        partial = complete[:count_index + 1] + [
            "FS_ENDPOINT_FAILURE reason=NONFINITE detail=failed after old count",
            ("COMPLETE test=paper_full_eight_square_contract result=FAIL source=" + SOURCE +
             " openfhe_pin=" + OPENFHE + " reason=failed after old count"),
        ]
        failed = reader.parse_primary_log(ctest_log(partial), IDENTITY, ctest_exit_code=1)
        self.assertEqual((failed.evidence_state, failed.reason), ("FATAL", "NONFINITE"))
        self.assertIsNone(failed.numeric_gate_failures)
        self.assertEqual(failed.e80_disposition, "NOT_OBSERVED")

    def test_bool_is_not_an_exit_status(self):
        with self.assertRaisesRegex(reader.PrimaryLogError, "ctest_exit_code"):
            reader.parse_primary_log(ctest_log(complete_lines(0)), IDENTITY,
                                     ctest_exit_code=False)

    def test_selected_primary_line_limit_is_enforced_before_record_dispatch(self):
        oversized = b"61: " + b"x" * 32764 + b"\n"
        with self.assertRaisesRegex(reader.PrimaryLogError, "32768"):
            reader.parse_primary_log(oversized, IDENTITY, ctest_exit_code=8)

    def test_windows_primary_transport_accepts_a_full_crlf_stream(self):
        windows_identity = reader.PrimaryIdentity(SOURCE, "windows", "101", "2")
        parsed = reader.parse_primary_log(
            ctest_log(complete_lines(0, host="windows"), line_ending=b"\r\n"),
            windows_identity,
            ctest_exit_code=0,
        )
        self.assertEqual(parsed.evidence_state, "COMPLETE")
        self.assertEqual(parsed.identity.host, "windows")

    def test_transport_cr_is_rejected_unless_it_is_one_windows_line_terminator(self):
        windows_identity = reader.PrimaryIdentity(SOURCE, "windows", "101", "2")
        windows_log = ctest_log(
            complete_lines(0, host="windows"), line_ending=b"\r\n")
        cases = (
            (windows_log.replace(b"61: BEGIN", b"61: BE\rGIN", 1), windows_identity),
            (windows_log.replace(b"\r\n", b"\r\r\n", 1), windows_identity),
            (windows_log[:-1], windows_identity),
            (ctest_log(complete_lines(0), line_ending=b"\r\n"), IDENTITY),
        )
        for log_bytes, identity in cases:
            with self.subTest(host=identity.host), self.assertRaisesRegex(
                    reader.PrimaryLogError, "CR"):
                reader.parse_primary_log(log_bytes, identity, ctest_exit_code=0)

    def test_windows_line_limit_counts_the_original_crlf_bytes(self):
        windows_identity = reader.PrimaryIdentity(SOURCE, "windows", "101", "2")
        head = b"FS_ENDPOINT_FAILURE reason=NONFINITE detail="
        exact = b"61: " + head + b"x" * (32768 - 4 - len(head) - 2) + b"\r\n"
        parsed = reader.parse_primary_log(exact, windows_identity, ctest_exit_code=1)
        self.assertEqual((parsed.evidence_state, parsed.reason), ("FATAL", "NONFINITE"))
        with self.assertRaisesRegex(reader.PrimaryLogError, "32768"):
            reader.parse_primary_log(
                exact[:-2] + b"x\r\n", windows_identity, ctest_exit_code=1)


if __name__ == "__main__":
    unittest.main(verbosity=2)
