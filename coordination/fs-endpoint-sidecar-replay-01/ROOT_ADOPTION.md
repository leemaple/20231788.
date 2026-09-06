# Bounded scalar replay intake — not full diagnostic acceptance

The root integrated the two replay source/test files and their 15 author ledger
and evidence files from the new clean-room sidecar worktree on 2026-09-06.
Source and destination hashes are identical:

- replay: `b0a879a90ed0e730c53eb569f7412841be5ba88bb8aef162b6a652e6d08367ef`
- test: `c7e2ff12299e8955808a34a133b1ea9015699f3a1cb9efe5caa27b2ce9af0196`

The root read the actual replay implementation and tests. A separate Codex
review context, `/root/partial_primitives_standards`, reviewed the source/spec
boundary and retained RED/GREEN evidence without rerunning or editing it.
Provider diversity is not claimed. The following findings were resolved:

1. Retain all four complete complex signed tuples at each maximum, not just
   four scalars from the maximizing component; expose selected primary-slot
   replay rather than require identical primary/replay argmax.
2. Use one actual Decimal256 scalar kernel in both the bounded and full-row
   paths, with a worked nonzero-imaginary fixture and exact allowance seam.
3. Preserve represented Decimal values exactly during maximum selection and
   the quarter-disk guard. Ambient-context `abs(Decimal)` previously rounded
   at the default precision 28. A public near-tie at significant digit 29
   actually failed on the old source; `copy_abs()` fixed the cause. The full
   hosted fixture also distinguishes its maximum only at this digit.

Root independently ran only the lightweight replay discovery using bundled
CPython 3.12.14. Actual result: 11 discovered, 10 PASS, one explicit hosted SKIP,
0.031 seconds, exit 0. Four source/test hashes before and after were unchanged.
The command, environment override, raw output and timestamps are retained in
`ROOT_LIGHT_REPLAY_GREEN.json`. The sealed reader suite was not rerun.

This accepts a bounded, independently reviewed source slice with actual light
tests, not a full 16,384-slot replay or an integrated packer. The full nonzero
fixture requires `RUN_FULL_ENDPOINT_REPLAY=1` on a hosted runner; it was not run
on this Mac. No FFT, NTT, cryptography, C++ build or CI was invoked for this
Python check. The original E80 failure remains unchanged.

Still required: actual primary stdout parsing, all-nine-scale and source/run
identity reconciliation, live metric and full signed tuple comparisons with
their exact reporting quanta and combined allowances, complete hosted replay,
strict gzip/status publication, single-CTest failure preservation and exact
artifact selection. The module intentionally does not implement those parts.

The separate C++ compile-only run 34007548849 is not a test of these Python
files. Its source is 8d7e6f072a399e572de5c4762d8399cbed0bc15b; Linux exposed
a new Boost fixed512-to-allocator768 conversion compile error. That error is
being diagnosed separately; no compile success or self-test pass is inferred.
