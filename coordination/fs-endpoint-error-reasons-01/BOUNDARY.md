# Endpoint Python typed-reason boundary

Status: sealed before the first test/source edit on 2026-09-06.

## Public compatibility seam

`SidecarError`, `ReplayError`, and `ReplayUnresolved` remain subclasses of their
current exception bases and remain constructible with one positional message:

```python
SidecarError(message, *, reason="FORMAT")
ReplayError(message, *, reason="REPLAY")
ReplayUnresolved(message, *, reason="CONDITIONING")
```

Each instance exposes the stable machine token as `.reason`; `str(error)` stays
the original human message so existing `assertRaisesRegex` callers remain
valid. Internal raises use an explicit keyword reason whenever the default is
not the correct classification. No finalizer may infer a reason by parsing the
message.

## Mapping

Sidecar reader:

| Failure | Reason |
|---|---|
| malformed caller identity scalar, byte/line/integer/decimal/rational/schema/order grammar, non-regular or symlink leaf | `FORMAT` |
| validly formed artifact scope/source/host/run/attempt differs from the trusted expected identity | `IDENTITY` |
| fixed model/metadata/scale/count/allowance/check/argmax semantic contradiction | `INTEGRITY` |
| endpoint one-norm radius exceeds the adopted conditioning guard | `CONDITIONING` |
| a derived comparison allowance exceeds `2^-128` | `ESTIMATOR_CEILING` |
| translated `stat`/open/read filesystem operation fails | `IO_ERROR` |

An artifact check record that says PASS but recomputes to FAIL or threshold
overlap is an `INTEGRITY` contradiction. This reader does not independently
discover an unsupported arithmetic implementation model.

Scalar replay:

| Failure | Reason |
|---|---|
| canonical decimal or direct public argument grammar | `FORMAT` |
| an allegedly validated sidecar has inconsistent metadata/row/order/shape | `INTEGRITY` |
| fresh-value disk/radius precondition is exceeded | `CONDITIONING` |
| serialization or derived replay allowance exceeds `2^-128` | `ESTIMATOR_CEILING` |
| Decimal replay produces a nonfinite represented value | `NONFINITE` |
| Decimal arithmetic/conversion or replay identity computation fails despite valid inputs | `REPLAY` |

The module catches only the existing precise `DecimalException` translation.
Unknown Python/programming exceptions are not caught or relabeled.

## Scope exclusions

This slice does not change grammar, numeric formulas, precision, thresholds,
row counts, full-replay opt-in, primary/status/publication/reconciliation code,
or exception message text except where minimal context is required for an I/O
translation. It does not make a finalizer or status record.

Pre-edit source identities:

```text
paper_endpoint_sidecar_reader.py b24a7d9828a7cc6d086537afd7bec2a04018c3ce2509157d7416e2ff151ebf30
paper_endpoint_sidecar_replay.py b0a879a90ed0e730c53eb569f7412841be5ba88bb8aef162b6a652e6d08367ef
```
