"""Cross-artifact checks for independently parsed endpoint evidence.

This data-level seam does not establish parser/replay provenance. The finalizer
must pass records produced from the same actual input files by the independent
readers and replay. No production imports, crypto, transforms or file writes.
"""
from fractions import Fraction

from paper_endpoint_sidecar_replay import (
    ReplayError, ReplayUnresolved, derive_replay_bounds,
)


RESIDUALS = ("E0", "E8", "I8", "A8")
ROW_COUNT = 16384


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
        raise ReconcileError("ESTIMATOR_CEILING", str(error)) from error
    except ReplayError as error:
        raise ReconcileError("INTEGRITY", str(error)) from error
    _require(len(endpoint.maxima) == 4, "INTEGRITY", "complete maximum receipt count")
    for name, maximum in zip(RESIDUALS, endpoint.maxima):
        _require(maximum.residual_id == name and
                 maximum.allowance == getattr(bounds, "live_" + name.lower()),
                 "INTEGRITY", f"{name} maximum does not bind actual C/scale allowance")
