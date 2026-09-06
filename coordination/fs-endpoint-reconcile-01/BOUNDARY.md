# Endpoint primary / sidecar / scalar replay reconciliation

Root-owned implementation at the accepted writer checkpoint. The user delegated
ordinary technical choices; this fixes the TDD seam before tests without asking
the user to design an internal interface. Read the adopted ENDPOINT_SPEC sections
5–8 and the existing independent reader/replay contracts before this slice.

## Interface

- `validate_primary_binding(primary, sidecar) -> None` binds already independently
  parsed complete records before the expensive hosted replay. It compares scope,
  source/host/run/attempt, observed count/E80/Boost, all nine scale receipts and
  all fields of the 24 comparison receipts, and derives live maximum allowances
  from the sidecar's actual C/scales. Self-consistent primary allowances alone do
  not prove that binding.
- Planned next slice, NOT YET IMPLEMENTED: `reconcile_replay(primary, sidecar, replay_result) -> None` verifies that same
  binding, the full-row replay result's bounds/maxima and all signed tuples at
  every primary maximizer. It independently replays those at-most-four selected
  rows through the existing public `replay_row` function. The finalizer must
  produce `replay_result` by calling `replay_sidecar` on this exact parsed sidecar;
  this data-level interface does not authenticate arbitrary Python objects.
- `ReconcileError(reason, detail)` provides a typed failure for the finalizer.
  A concrete earlier primary failure is handled by the finalizer before this
  complete-only seam, never overwritten with missing evidence.

Readers own byte grammar and internal record validation. This module owns
cross-artifact agreement, not a duplicate parser, FHE/FFT/NTT, compression, status
files, shell execution or publication. It may import only the independent scalar
replay helper, not production code. Exact comparisons use Fraction; allowances
remain CONDITIONAL. E80 is retained unchanged; A remains NOT_ADOPTED.

All combined live/replay/serialization allowances used must be <=2^-128.
Signed components, not just absolute maxima, must agree within their justified
allowance. Different I8/A8 argmax indices are not a failure when the represented
maximum magnitudes are indistinguishable within those bounds. E0/E8 canonical
components have exact byte correspondence to the same serialized source.

Local tests use small explicit synthetic scalar fixtures / known-answer data at
this public interface. Their data-level receipts are not claims that a parser or
full replay ran. A full 16384-row parser/replay/cross-language integration remains
hosted-only and must run before the next encrypted chain. No internal-method
mocking or Mac full replay is permitted. Retain each actual RED/GREEN command,
source/test hash and output contemporaneously; do not invent authoring history.
