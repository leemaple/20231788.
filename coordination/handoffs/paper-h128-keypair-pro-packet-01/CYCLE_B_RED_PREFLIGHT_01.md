# h128 Cycle-B RED preflight — 2026-09-05

Cycle-A dual-host GREEN evidence was committed and pushed first at
`094d320d9404cd67e03be3d45711f2f723f93f96`. Its tested implementation is
`8aac5b7cf6530a9a2da14e8a4bdd5b65ab3c869f`, run 33944191280.

Only original `0003-red-paper-h128-client-keypair-guards.patch` is now applied.
Original patch: 15,914 bytes, SHA-256
`250cb3e06da4981a4a12feb73ec85a7d0b2453fda39d7655750a351c17f2b0b5`.
Engineering diff: one test file, 239 added lines, no deletions. Resulting test:
30,390 bytes, SHA-256
`c2698109d0a45621f6c705bcdfd2d0da3ae748db9802db1287de8517077fb81f`.
It adds detached negative fixtures, public rejection assertions and client-owned
tag/cache lifecycle checks. Original A inputs, literals and tolerances remain
unchanged. Production source/header, profile, CMake and workflow remain at A
GREEN; original 0004 B GREEN has not been applied.

## Required hosted RED (not yet observed)

On both Linux and Windows, require exact source/pin provenance, successful
warning-clean build and all five API targets, existing focused checks and legacy
57/57, successful new target compilation, then actual focused CTest execution
failing with `unsupported profile was accepted`. Earlier fixture, compilation,
loading, timeout or infrastructure failures do not count as this RED.

The first named case is noiseScale=2 with otherwise valid detached Q/sigma/PKE
and reconstructed precomputation. A does not reject that scalar; official
EncryptZeroCore can return (a*s + 2*e, -a), reaching the expected assertion.
This is a source-supported expectation, not a runtime result. Other rejection
cases and lifecycle assertions are not expected to execute in this RED.

The valid-path stdout marker is useful when present, but is not mandatory:
it ends with newline without an explicit flush and an uncaught exception can
discard buffered stdout. If absent, preserve the exact observed semantic
diagnostic and label prior ValidPath completion as a control-flow inference;
do not claim the marker was observed or modify the frozen test just to flush it.
Independent reviewer h128_candidate_standards confirmed this acceptance rule.

No Mac build/cryptography, CI rerun, threshold change, default-branch merge or
paper-scale/80-bit/security claim is part of this step. After exact dual RED is
retained and accepted, only original 0004 may enter the next GREEN cycle.

Actual-worktree independent gate by h128_candidate_spec: PASS; exact test hash,
239 additions/no deletions, frozen A code and other engineering paths verified.
Root diff checks passed. Gitleaks 8.30.1 at 12:41 Asia/Shanghai scanned the tests
(712,222 bytes) and handoff evidence (1,119,367 bytes), both with no leaks.
