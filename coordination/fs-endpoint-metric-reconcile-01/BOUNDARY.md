# Endpoint replay-metric reconciliation boundary

Status: public seam fixed before the first test on 2026-09-06.

## Public interface

```python
reconcile_replay(primary, sidecar, replay_result) -> None
```

The function first calls the existing accepted `validate_primary_binding` and
then reconciles metrics from the same complete primary, parsed sidecar, and
full-row scalar `ReplayResult`. It raises typed `ReconcileError`; it performs no
file parsing, full replay, production import, crypto, transform, compression,
status writing, publication, or shell execution.

It derives the maximum E0/E8 serialization exponents by scanning the already
validated sidecar rows and independently calls `derive_replay_bounds` from the
actual sidecar C/scales. It requires exact agreement with every supplied bounds
field, row count 16384, Decimal precision 256, and `ROUND_HALF_EVEN`. Every used
combined live/replay/serialization allowance must be at most `2^-128`. Raw
finite discrepancy above `2^-120` is an integrity failure before allowance
ceiling classification.

Every `ReplayMetric` must identify a valid row/component, retain the exact four
signed replay vectors returned by public `replay_row` at that row, select its
own reported absolute value, obey the zero tie convention, and dominate the
components replayed at every primary-selected slot. These selected-row calls do
not rerun the full sidecar and therefore do not independently establish I8/A8/R
globality or tie order over unselected rows; that fact comes from the required
direct `replay_sidecar` return.

For every primary maximum, all eight retained signed tuple components are
checked. E0/E8 component text must equal the corresponding sidecar bytes
exactly. I8/A8 signed component discrepancies use that component's reported
decimal quantum plus the corresponding live and replay allowances. Multiple
primary maxima selecting one slot must retain identical full tuple text.
Primary and replay E0/E8 serialized global maxima agree exactly; displayed I8
and A8 maxima use live plus replay plus displayed-magnitude quantum. Different
argmax positions are permitted only when this maximum discrepancy is bounded.
No extra acceptance allowance or threshold is invented for `r_identity`.

This data-level seam cannot authenticate `ReplayResult`, which has no identity
or digest, and primary does not contain exact C. The finalizer must call
`replay_sidecar` directly on this same parsed sidecar and pass that return here;
capture/writer and hosted provenance remain separate gates. Local tests use
bounded synthetic rows and analytic summaries only. Full hosted 16,384-row
replay/cross-language integration remains pending, assurance is CONDITIONAL,
E80 is unchanged, and A is NOT_ADOPTED.

## Corrective typed-reason and coverage boundary

Replay exceptions are translated to `ReconcileError` with their actual typed
`.reason`; the reconciliation layer does not infer a reason from the exception
class or message. Exact dependency-only copies of the already accepted typed
sidecar reader and replay modules are present in this worktree solely so this
slice can execute against that interface. Root must not reimport those copies
from this return.

A zero-residual test passes actual `PrimaryLog` and `Sidecar` reader dataclasses
through `reconcile_replay`, while still constructing the analytic zero
`ReplayResult` without running the full 16,384-row scalar replay. ReplayMetric
negative coverage independently changes all eight signed tuple components for
each of the five replay metrics.

Both required reporting-quantum terms remain explicit in the source. A single
deterministic selected-row check found that the adopted conservative replay
bounds dominate the relevant quanta by roughly 46 decimal orders in the
existing nonzero fixture; therefore the existing public fixtures do not make
acceptance depend on either quantum term. No invalid model, arbitrary random
search, private helper test, or fabricated parser-valid evidence was introduced
solely to obtain mutation discrimination. This is an explicit source-reviewed
but not test-mutation-discriminated limit.
