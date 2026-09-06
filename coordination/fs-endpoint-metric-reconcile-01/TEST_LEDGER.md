# Endpoint replay-metric reconciliation test ledger

Date: 2026-09-06. Requested model/routing was Sol/high; backend identity is
requested-unverified. No model, ranking, quota, or external-agent probe was
performed.

## Boundary and TDD receipt

The public seam and its limits were written before the first test. The focused
zero-residual import test then failed because `reconcile_replay` did not exist:
1 test, 1 error, exit 1, 0.009 seconds. After the bounded implementation it
passed: 1 test, exit 0, 0.086 seconds. Exact commands and pre-run hashes are in
`evidence/01-zero-metric-import-red.json` and
`evidence/02-zero-metric-green.json`.

The remaining negative cases are discriminating source-level contract tests,
not separately claimed as one-test RED receipts. Their first combined execution
exposed undersized synthetic mutations; those fixtures were corrected to cross
the exact applicable allowance without changing the implementation. No failing
implementation receipt is claimed for those already-green behaviors.

## Final local receipt

The final bundled Python 3.12 command ran the preserved six primary-binding
tests and seven new replay-reconciliation tests. All 13 passed in 1.782 seconds
with exit 0. The new tests cover the zero tie, all ReplayMetric selected tuples,
bounds/environment binding, all eight primary tuple positions and same-slot
copy consistency, exact E0 bytes, signed I8, distinct A8 argmax inside/outside
the combined bound, and raw-discrepancy precedence over an excessive quantum.
The exact receipt and hashes are in
`evidence/03-replay-and-binding-green.json`.

## Deliberate limits

The synthetic metric fixtures invoke public `replay_row` only for selected
rows and use the public bounded analytic summarizer. They do not run the full
16,384-row replay, C++, crypto, CTest, CI, or a hosted cross-language chain.
`ReplayResult` has no provenance field, so the finalizer must call
`replay_sidecar` on the exact parsed sidecar immediately before this seam.
Primary has no exact C field; exact C binding remains sidecar-derived allowance
binding plus the separate capture/writer provenance gate. Assurance remains
CONDITIONAL, original E80 remains unchanged, and A remains NOT_ADOPTED.

## Independent corrective pass

An independent source-first review found that the metric formulas matched the
adopted boundary but replay exception translations discarded the newly typed
reason, ReplayMetric tuple negatives changed only `E0.real`, and no metric test
passed actual reader dataclasses. It also found that the existing distinct-A8
and nonzero tuple fixtures did not discriminate removing the magnitude or
per-component quantum terms. The raw-priority fixture changed magnitude quantum
without updating its duplicate `magnitude_quantum` field.

The public typed-reason tracer was authored first. Against the unchanged metric
source and the pre-typed replay dependency it failed as intended because the
observed reason was `REPLAY` instead of `FORMAT`: 1 test, 1 failure, exit 1,
0.100 seconds. After copying the exact already accepted typed dependencies and
changing only replay-exception translation to preserve `.reason`, it passed:
1 test, exit 0, 0.105 seconds. Exact timestamps, commands and hashes are in
`evidence/04-typed-reason-red.json` and `05-typed-reason-green.json`.

Coverage-only edits then expanded every ReplayMetric to all eight signed tuple
positions, made the raw-priority duplicate magnitude quantum coherent, and
passed actual zero-valued primary/sidecar reader dataclasses through the public
metric seam. Each focused assertion passed on its first execution; these are
not claimed as RED/GREEN implementation cycles. The raw-priority fixture remains
a deliberately assembled `SimpleNamespace` seam fixture, not a complete parser
round-trip, although its magnitude, exact value, magnitude quantum and tuple
quantum are now mutually coherent. See `evidence/06-review-coverage-green.json`.

The typed sidecar reader and replay files in this worktree are dependency-only
copies with accepted hashes
`40d00c340f40504722e000d450a6883c01fadf99ebb05678018c01b7267008a0`
and `4f44b8012171fc79d5edcae8455f270b161dd354997be0b7bcc96d15b9574d46`.
Root already owns those sources and must not import them again from this slice.
No full scalar replay, C++, crypto, CTest, CI, commit or push was performed.

The final bounded Python 3.12.14 regression ran the primary reader, typed
sidecar reader, typed replay, binding, and metric-reconciliation suites: 69
tests in 5.907 seconds, 69 successful outcomes with the existing hosted full
16,384-row replay test counted as one SKIP, exit 0. This run did not execute the
full scalar replay. Exact command and source/test hashes are in
`evidence/07-corrective-regression-green.json`.
