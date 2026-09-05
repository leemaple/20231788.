# h128 Cycle-A RED preflight — 2026-09-05

Status: static integration gates accepted; actual hosted RED is pending.
Owner: Codex. User delegated routine seam and engineering choices.

## Exact inputs and reviewed scope

- Task input: `9d21c3a5aea79c31745aca790712a9fd8c7743b2`.
- Actual integration parent: `345a88393aadb81d4cdb12a8bf05831d96643636`;
  clean branch `codex/paper-h128-keypair-01`, synchronized with origin before edits.
- Pristine OpenFHE: `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Returned ZIP: 96,840 bytes; SHA-256
  `ccf2ecad2b6db7d0c6306dcedcd64b21e6e57aa677fa33e0eab83b964aa5df5a`.
- Only original `0001-red-paper-h128-client-keypair.patch` is integrated here,
  plus the two-build-file overlay below. Original archive and patches remain
  immutable under `artifacts/handoffs/h128-return-01/`.
- Static review used a separate detached replay from the exact task input.
  Its four local synthetic commits (`ffc7ba3`, `eafc525`, `bad64b3`, `ed039fa`)
  were neither pushed nor executed. They are NOT RED/GREEN runtime evidence.

## Standards

Independent reviewer `h128_candidate_standards` reviewed all six candidate
engineering paths and all four synthetic stages against the project workflow,
engineering rules and documented task. Initial result: zero concrete standards
violations and zero actionable heuristic findings. Client-only official
sampling/zero-encryption composition, no context/cache mutation, narrow public
rejection checks and detached malformed fixtures respect the agreed boundaries.
The pinned public scheme stream/RTTI coupling is explicit, not an unverified
cross-version compatibility claim. Compilation and runtime were not performed.

Root subsequently identified the CI evidence-order issue below; the same
independent reviewer confirmed the issue and minimum correction. This is a
build/CI integration correction, not a changed key-generation contract.

## Spec

Independent reviewer `h128_candidate_spec` reviewed `9d21c3a...ed039fa` against
`PAPER_H128_KEYPAIR_PRO_IMPLEMENTATION_01.md`, `TEST_SEAMS.md` and Candidate B.
Result: no concrete finding blocks original Cycle-A RED. Final candidate uses
one official h-aware sample, fresh matching private/public objects, official
`EncryptZeroCore`, full-Q elements and unchanged context/cache ownership.
Negative fixture safety/order and frozen patch scopes were checked. No claim
of compiled compatibility, cryptographic success, precision or completion follows.

## Independent provenance and frozen-profile gate

Reviewer `h128_profile_provenance` independently compared every supplied payload
to the actual Git object, including recomputed Git blob IDs:

- 39 project files / 921,481 bytes exactly match the task-input commit.
- 51 official files / 1,020,323 bytes exactly match the pristine OpenFHE pin.
- Official checkout status is clean; quarantined implementations were not used.

Source-to-profile mapping covers parameter generation, explicit constructor
forwarding, native/backend selection, Q search endpoints, singleton HYBRID
partitions, P and ordered QP, minimum primitive roots and ordinary FIXEDMANUAL
scale. Independently checked five search endpoint mappings, 213 contiguous
skipped candidate positions, binary32 sigma and eight algebraic square vectors.
Previous separate exact-integer prime/root certificates remain recorded in
`ROOT_RETURN_ARITHMETIC_01.json`; they were not mislabelled as OpenFHE execution.
The Q bit lengths are [50, 40, 41]: official FirstPrime(40,512) starts above 2^40.

Frozen SHA-256 identities, unchanged by integration:

- Profile: `2f1c5c8975eb53652c0e1c9ed97c6e44700eba7c61e800a424223b16937b2013`.
- Cycle-A test: `71dbdf81bb2c5a208535f90262c3746fea08bcc0e1d45ddb5570f61ff9e781b6`.

## Reviewed CI-only integration overlay

Observed issue: the return includes the new target in the default ALL build.
Its deliberately absent public header would fail before old-suite evidence.
Mult2/Add/Sub API targets also follow the new focus/full suite, so either RED
would skip those three old API checks.

Correction in CMake and both existing workflow jobs only:

1. Mark the new executable EXCLUDE_FROM_ALL; keep linking, warning flags and
   its single CTest registration unchanged.
2. Build the existing default project and all five explicit API targets first.
3. Preserve the old first-Mult2 and Pair-focused tests, then explicitly run the
   legacy suite excluding only the exact new h128 CTest name (57 bindings).
4. Explicitly build the h128 target, run its exact-name focus, then full 58.

No continue-on-error, expected-failure masking or forced-success wrapper is
introduced. The Windows shell, path translation and runtime library PATH are
preserved. Native64/backend4, exact pristine pin, Debug and warnings-as-errors
remain required on both hosts. Original remaining patches apply without an
overlapping build-file hunk; `git apply --check` for 0002 succeeded.
Final integrated build files therefore intentionally do not byte-match the
return's complete/project build files. Test/profile identities still do.

## Runtime gate and next boundary

Expected, not yet observed: both hosted jobs pass old 57/API checks, then fail
at explicit h128 target compilation because the adapter header is absent.
New focused/full tests should be skipped after that genuine failure.
Only after retaining exact-SHA dual-host logs may Codex apply 0002 (A GREEN).
Do not bundle 0002–0004 into this RED or weaken frozen literals/tolerances.

This N=256 key-consistency diagnostic does not establish 80-bit precision,
N=32768, shared-secret basis families, eight paper squarings, security or 1000
trials. No Mac build, NTT, cryptography, benchmark, CI dispatch/rerun or default
branch merge was performed by this preflight.
