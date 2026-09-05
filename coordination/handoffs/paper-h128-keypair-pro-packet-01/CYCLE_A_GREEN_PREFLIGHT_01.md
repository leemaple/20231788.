# h128 Cycle-A GREEN candidate — 2026-09-05

Actual RED acceptance was committed and pushed as
`3c9bfc208d55f32437f8650bc7700e20edfc5a27` before this implementation was applied.
See `CYCLE_A_RED_ACCEPTANCE_01.md` for run 33943456483's dual-host evidence.

Applied only original `0002-green-paper-h128-client-keypair.patch` from the
verified Pro return; its SHA-256 remains
`37ec6422ccfb3d267611a443aa0ffa5209d8dd3bcb0fa74afd0b7c64fda659e2`.
The implementation is the minimal valid-path cycle, not Cycle-B validation.

Observed static integration:

- New header/source exactly match original A GREEN, independently replayed in
  synthetic `eafc525`; source SHA-256
  `c8e32de43973f37c9edbae214d7d720869a564bb4336b39a0a3791f8875007c3`.
- The sole CMake change from actual A RED is one `target_sources` line.
- The actual RED test, profile, workflow and all 58 bindings are unchanged.
  In particular the reviewed EXCLUDE_FROM_ALL/explicit-new-build CI ordering
  remains in force on both hosts.
- Original 0003/0004 are not applied; no new tests are added during GREEN.
- Root fully inspected the 94-line implementation and 21-line header. It uses
  the official h-aware DCRT sampler once with 128, fresh SK/PK, one private
  element assignment and official public scheme EncryptZeroCore; no evaluator
  change, context mutation or cache clearing.
- Earlier independent Standards/Spec and pinned-source reviews remain in
  `CYCLE_A_RED_PREFLIGHT_01.md`. The current integration is checked against
  their exact A GREEN stage rather than importing final B GREEN prematurely.
- Independent `h128_candidate_standards` also checked the actual uncommitted
  three engineering paths against eafc525 and a21216f: byte identities,
  one-line CMake delta, frozen files and absence of Cycle B all pass. No obvious
  new pinned-API/compile-facing defect was identified; runtime remains pending.

Full structural rejection and lifecycle validation belong to the separate B
RED/GREEN cycle. Do not treat header preconditions as guards already executed
in this minimal A implementation. Likewise do not claim an h128 runtime pass
before the new target/focus/full suite actually completes on both hosts.

Required hosted result for this next exact source: Debug warning-clean build,
five API builds, prior focused tests, legacy 57/57, new h128 focus 1/1 and full
58/58, pinned native64/backend4. Source-supported compile/runtime repairs may
not alter the frozen test, profile, oracle or tolerance. No Mac builds, crypto,
benchmarks, default-branch merge, speculative validation or refactoring.
