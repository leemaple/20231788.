"""Cross-artifact checks for independently parsed endpoint evidence.

This data-level seam does not establish parser/replay provenance. The finalizer
must pass records produced from the same actual input files by the independent
readers and replay. No production imports, crypto, transforms or file writes.
"""
from fractions import Fraction

from paper_endpoint_sidecar_replay import (
    ReplayError, ReplayUnresolved, derive_replay_bounds, replay_row,
)


RESIDUALS = ("E0", "E8", "I8", "A8")
ROW_COUNT = 16384
T_OBS = Fraction(1, 1 << 120)
T_BUDGET = Fraction(1, 1 << 128)
ZERO = "+0." + "0" * 109 + "e+00000"


class ReconcileError(ValueError):
    def __init__(self, reason, detail):
        super().__init__(f"{reason}: {detail}")
        self.reason = reason
        self.detail = detail


def _require(condition, reason, detail):
    if not condition:
        raise ReconcileError(reason, detail)


def validate_primary_binding(primary, sidecar):
    """Bind complete independently parsed records before expensive replay."""
    _require(primary.evidence_state == "COMPLETE", "INTEGRITY",
             "incomplete primary cannot authorize complete evidence")
    for key in ("source_commit", "host", "github_run_id", "github_run_attempt"):
        _require(getattr(primary.identity, key) == sidecar.meta[key],
                 "IDENTITY", f"primary/sidecar {key} mismatch")
    endpoint = primary.endpoint
    _require(endpoint.scope == sidecar.meta["scope"], "IDENTITY", "scope mismatch")
    for actual, key in ((endpoint.boost_version, "boost_version"),
                        (primary.numeric_gate_failures, "numeric_gate_failures"),
                        (primary.e80_disposition, "E80_disposition")):
        _require(actual == sidecar.meta[key], "INTEGRITY", f"observed {key} mismatch")
    _require(len(sidecar.rows) == ROW_COUNT and sidecar.meta["row_count"] == ROW_COUNT,
             "INTEGRITY", "complete sidecar row count")
    _require(len(endpoint.scales) == len(primary.legacy_scales) == 9,
             "INTEGRITY", "complete scale receipt count")
    for index, (observed, legacy) in enumerate(zip(endpoint.scales, primary.legacy_scales)):
        fields = ("operation", "numerator", "denominator")
        _require(observed.operation == index and
                 all(getattr(observed, key) == getattr(legacy, key) for key in fields),
                 "INTEGRITY", f"primary scale receipt {index} mismatch")
        if index in (0, 8):
            _require(Fraction(observed.numerator, observed.denominator) ==
                     sidecar.meta[f"scale{index}"], "INTEGRITY",
                     f"sidecar scale {index} mismatch")
    _require(len(endpoint.checks) == len(sidecar.checks) == 24,
             "INTEGRITY", "complete comparison receipt count")
    fields = ("check_id", "distance", "allowance", "argmax_slot", "argmax_component")
    for index, (observed, retained) in enumerate(zip(endpoint.checks, sidecar.checks)):
        _require(all(getattr(observed, key) == getattr(retained, key) for key in fields),
                 "INTEGRITY", f"comparison receipt {index} mismatch")
    try:
        bounds = derive_replay_bounds(sidecar.meta, maximum_e0_exponent=None,
                                      maximum_e8_exponent=None)
    except ReplayUnresolved as error:
        raise ReconcileError(error.reason, str(error)) from error
    except ReplayError as error:
        raise ReconcileError(error.reason, str(error)) from error
    _require(len(endpoint.maxima) == 4, "INTEGRITY", "complete maximum receipt count")
    for name, maximum in zip(RESIDUALS, endpoint.maxima):
        _require(maximum.residual_id == name and
                 maximum.allowance == getattr(bounds, "live_" + name.lower()),
                 "INTEGRITY", f"{name} maximum does not bind actual C/scale allowance")


def _sidecar_fraction(value):
    significand = value.signed_significand
    exponent = value.decimal_exponent - 109
    if exponent >= 0:
        return Fraction(significand * 10 ** exponent)
    return Fraction(significand, 10 ** -exponent)


def _bounded_difference(left, right, allowance, detail):
    distance = abs(left - right)
    _require(distance <= T_OBS, "INTEGRITY", detail + " exceeds 2^-120")
    _require(allowance <= T_BUDGET, "ESTIMATOR_CEILING",
             detail + " allowance exceeds 2^-128")
    _require(distance <= allowance, "INTEGRITY", detail + " exceeds allowance")
    _require(distance + allowance <= T_OBS, "ESTIMATOR_CEILING",
             detail + " overlaps 2^-120")


def _scalar_vectors(scalar):
    return dict(E0=scalar.e0, E8=scalar.e8, I8=scalar.i8,
                A8=scalar.a8, R=scalar.identity)


def _selected_scalar(sidecar, slot, cache):
    _require(type(slot) is int and 0 <= slot < ROW_COUNT,
             "INTEGRITY", "selected replay slot is invalid")
    if slot not in cache:
        try:
            cache[slot] = replay_row(slot, sidecar.rows[slot])
        except ReplayUnresolved as error:
            raise ReconcileError(error.reason, str(error)) from error
        except ReplayError as error:
            raise ReconcileError(error.reason, str(error)) from error
    return cache[slot]


def reconcile_replay(primary, sidecar, replay_result):
    """Reconcile full replay summaries and every primary-selected signed tuple."""
    validate_primary_binding(primary, sidecar)
    _require(getattr(replay_result, "row_count", None) == ROW_COUNT,
             "INTEGRITY", "replay row count mismatch")
    _require(getattr(replay_result, "decimal_precision", None) == 256 and
             getattr(replay_result, "decimal_rounding", None) == "ROUND_HALF_EVEN",
             "INTEGRITY", "replay Decimal environment mismatch")

    maximum_exponents = [None, None]
    serialized_maxima = [Fraction(0), Fraction(0)]
    for slot, row in enumerate(sidecar.rows):
        _require(getattr(row, "slot", None) == slot, "INTEGRITY",
                 "sidecar row identity changed before reconciliation")
        values = getattr(row, "values", None)
        _require(isinstance(values, tuple) and len(values) == 4,
                 "INTEGRITY", "sidecar row shape changed before reconciliation")
        for index, value in enumerate(values):
            group = 0 if index < 2 else 1
            exact = _sidecar_fraction(value)
            serialized_maxima[group] = max(serialized_maxima[group], abs(exact))
            if value.text != ZERO:
                exponent = value.decimal_exponent
                current = maximum_exponents[group]
                maximum_exponents[group] = exponent if current is None else max(current, exponent)
    try:
        expected_bounds = derive_replay_bounds(
            sidecar.meta, maximum_e0_exponent=maximum_exponents[0],
            maximum_e8_exponent=maximum_exponents[1])
    except ReplayUnresolved as error:
        raise ReconcileError(error.reason, str(error)) from error
    except ReplayError as error:
        raise ReconcileError(error.reason, str(error)) from error
    actual_bounds = getattr(replay_result, "bounds", None)
    _require(actual_bounds == expected_bounds, "INTEGRITY", "replay bounds mismatch")
    _require(all(isinstance(value, Fraction) and 0 <= value <= T_BUDGET
                 for value in expected_bounds.__dict__.values()),
             "ESTIMATOR_CEILING", "replay bound exceeds 2^-128")

    metrics = {name: getattr(replay_result, name.lower()) for name in RESIDUALS}
    metrics["R"] = getattr(replay_result, "r_identity")
    replay_cache = {}
    for name, metric in metrics.items():
        maximum = getattr(metric, "maximum", None)
        slot = getattr(metric, "argmax_slot", None)
        component = getattr(metric, "argmax_component", None)
        _require(isinstance(maximum, Fraction) and maximum >= 0 and
                 component in ("real", "imag"), "INTEGRITY",
                 f"{name} replay metric shape")
        scalar = _selected_scalar(sidecar, slot, replay_cache)
        vectors = _scalar_vectors(scalar)
        expected_tuple = tuple(vectors[key] for key in RESIDUALS)
        _require(metric.signed_tuple == expected_tuple, "INTEGRITY",
                 f"{name} replay signed tuple mismatch")
        component_index = 0 if component == "real" else 1
        _require(abs(vectors[name][component_index]) == maximum, "INTEGRITY",
                 f"{name} replay selected maximum mismatch")
        if maximum == 0:
            _require(slot == 0 and component == "real", "INTEGRITY",
                     f"{name} replay zero tie mismatch")

    primary_tuples = {}
    for maximum in primary.endpoint.maxima:
        slot = maximum.argmax_slot
        scalar = _selected_scalar(sidecar, slot, replay_cache)
        vectors = _scalar_vectors(scalar)
        expected_fields = tuple((name, component) for name in RESIDUALS
                                for component in ("real", "imag"))
        actual_fields = tuple((name, component) for name, component, _ in
                              maximum.tuple_values)
        _require(actual_fields == expected_fields, "INTEGRITY",
                 f"{maximum.residual_id} primary tuple shape")
        tuple_text = tuple(value.text for _, _, value in maximum.tuple_values)
        if slot in primary_tuples:
            _require(primary_tuples[slot] == tuple_text, "INTEGRITY",
                     "primary tuples disagree at one selected slot")
        else:
            primary_tuples[slot] = tuple_text
        tuple_map = {(name, component): value for name, component, value in
                     maximum.tuple_values}
        row = sidecar.rows[slot]
        for index, key in enumerate((("E0", "real"), ("E0", "imag"),
                                     ("E8", "real"), ("E8", "imag"))):
            _require(tuple_map[key].text == row.values[index].text, "INTEGRITY",
                     f"{maximum.residual_id} {key[0]}.{key[1]} byte mismatch")
        for name in ("I8", "A8"):
            for component_index, component in enumerate(("real", "imag")):
                observed = tuple_map[(name, component)]
                allowance = (getattr(expected_bounds, "live_" + name.lower()) +
                             getattr(expected_bounds, "replay_" + name.lower()) +
                             observed.quantum)
                _bounded_difference(observed.value, vectors[name][component_index], allowance,
                                    f"{maximum.residual_id} {name}.{component}")
        for name, metric in metrics.items():
            _require(all(metric.maximum >= abs(component) for component in vectors[name]),
                     "INTEGRITY", f"{name} replay maximum misses a primary-selected row")

    for index, name in enumerate(("E0", "E8")):
        primary_maximum = primary.endpoint.maxima[index]
        _require(primary_maximum.magnitude.value == serialized_maxima[index] and
                 metrics[name].maximum == serialized_maxima[index],
                 "INTEGRITY", f"{name} serialized global maximum mismatch")
        selected = next(value for residual, component, value in primary_maximum.tuple_values
                        if (residual, component) ==
                        (name, primary_maximum.argmax_component))
        expected_text = ZERO if selected.text == ZERO else "+" + selected.text[1:]
        _require(primary_maximum.magnitude.text == expected_text, "INTEGRITY",
                 f"{name} displayed maximum byte mismatch")
    for name in ("I8", "A8"):
        primary_maximum = primary.endpoint.maxima[RESIDUALS.index(name)]
        allowance = (getattr(expected_bounds, "live_" + name.lower()) +
                     getattr(expected_bounds, "replay_" + name.lower()) +
                     primary_maximum.magnitude.quantum)
        _bounded_difference(primary_maximum.magnitude.value, metrics[name].maximum,
                            allowance, f"{name} displayed global maximum")
