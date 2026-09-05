# First signed-diagnostic hosted run

Source `9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e` was committed and pushed to
`codex/paper-scale-implementation-20260905`; remote HEAD was read back exactly.
The unique automatic [run 33978202814](https://github.com/leemaple/20231788./actions/runs/33978202814)
was created at 2026-09-05T16:32:47Z, event push, attempt 1.
Linux job 101338538686 and Windows job 101338538587 are in progress at this
snapshot; paper compilation/numerical results are pending. No CI was dispatched,
rerun or cancelled. No local watch session was started for this run.

This commit changes only the paper diagnostics/portability test seam and its
review evidence. Production remains byte-identical to b1b024e; full acceptance
is not claimed. The original 2^-80 / 2^-120 gates, one chain per host and existing
60 regressions remain unchanged. See ACCEPTANCE.md for the exact review scope.

The final engineering staging gate included 12 files, 513,739 decoded/framed
bytes, SHA-256 `40d5eabd5a246e255875d30ef0e3f82a90a8edb8b1e2477cb963562ec6b0ebcd`;
strict gitleaks 8.30.1 returned 0 with no findings and diff --check passed.
STAGED_SCAN.json records the preceding 11-file scan before adding that receipt.

Next: inspect this exact run's terminal metadata, download each terminal job log
once, and reconcile source, ordered live inventories, all old test invocations,
parameter/receipt identities and the one original paper stream. CTest failed
output replay is not a new experiment. Retain signed E/I/A/L, coefficient/scale
conditioning, every numeric failure, final binder/full-slot/witness and cleanup
observations actually reached. A diagnostic run with any retained numeric miss
is still FAIL; no changed keys/inputs/retry-until-green or hidden threshold change.

A later documentation-only commit does not change the CI source SHA above.
