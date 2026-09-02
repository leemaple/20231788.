# Relin2 connected-core R1 runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the Relin2 connected-core R1 test contract is red exactly as
intended on Linux and Windows**. Both warning-clean project builds and both
compile-only Relin2 public-API builds pass. All 26 inherited tests pass, and
only the ten newly added R1 tests fail. This is test-first evidence, not a
Relin2 arithmetic implementation or a green result.

## Source boundary

- source branch/commit: `agent/codex-relin2-01` /
  `f90a04d199e96a3247a2607aa3e1f80ad55be8cc`;
- parent: `1e59e8b36d5119ceb2b463922f1053e03a029bd4`;
- tree: `7edbfad070201f68a60d1b53f6c72bbb99939eb3`;
- local HEAD, upstream-tracking ref, and live GitHub ref matched after the
  non-force push.

The commit changes only `CMakeLists.txt`, `tests/dcp_rcb_test.cpp`,
`tests/relin2_test.cpp`, and `tests/tensor2_test.cpp`: 1,554 insertions and one
deletion. No production source or public header changed. The ten selectors are
new and unique, and the complete CMake/`ResolveTest` selector sets agree.

The connected tests require three independent fail-closed oracles for each of
the first eight success cases: exact integer `(u, v+w)` arithmetic, complete
`ReadyForRS2` lifecycle/scale/metadata state, and public RCB exactness. They
also snapshot Tensor, ciphertext, metadata, evaluation-key cache, key-vector,
key object, and A/B polynomial state. The later-key cases use real public
`[s^2 -> s, s^3 -> s]` generation; one retains the valid later key and the
other makes only index one null. The last two tests demand exact fail-fast
validation of malformed recombined fields before any later operation.

Three independent read-only Spec/API, TDD, and Delivery/compile reviews
returned `PASS` against the frozen test bytes before commit. The exact
26-to-36-to-37 ordering and later-key semantics were previously resolved by
two clean-room terminal reviews whose machine-emitted model identity was
`claude-fable-5-1`; no fallback output was accepted.

## Supplementary draft observation

Workflow-dispatch run `33637147696`, attempt 1, executed the same exact commit
before it was moved to the formal implementation branch. Linux job
`100270763453` and Windows job `100270763683` both passed the warning-clean
project and Relin2 public-API builds. CTest then reported exactly `26/36`, with
the same ten expected failures described below. Linux reported 0.30 seconds
and Windows 0.69 seconds.

This run is retained as corroboration only. It does not replace the formal
push-triggered observation.

## Formal hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33638053832`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33638053832`;
- exact head SHA: `f90a04d199e96a3247a2607aa3e1f80ad55be8cc`;
- terminal run state: `completed/failure`, where failure is the required TDD
  red state;
- Linux job: `100273799877`;
- Windows/MSYS2 MinGW64 job: `100273799654`.

Both jobs used pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default
project builds and both compile-only Relin2 public-API builds succeeded.
Tests 1 through 26 all passed. CTest then reported `72% tests passed, 10 tests
failed out of 36` on both platforms, in 0.32 seconds on Linux and 0.67 seconds
on Windows.

The exact expected failure set was:

1. `relin2_valid_arithmetic_state_immutability` — terminal scaffold;
2. `relin2_controlled_witnesses_and_boundaries` — terminal scaffold;
3. `relin2_representative_public_input` — terminal scaffold;
4. `relin2_key_extra_later_valid` — terminal scaffold;
5. `relin2_key_malformed_later_ignored` — terminal scaffold;
6. `relin2_hybrid_valid_shapes` — terminal scaffold;
7. `relin2_bv_zero_digit_valid_shapes` — terminal scaffold;
8. `relin2_bv_nonzero_digit_valid_shapes` — terminal scaffold;
9. `relin2_first_recombined_rcb_validation` — required predicate did not yet
   fail fast;
10. `relin2_first_recombined_tensor2_validation` — required predicate did not
    yet fail fast.

For items 1 through 8 the test observed the exact existing
`DoubleCKKS: Relin2 is not implemented` terminal scaffold and then rejected
that outcome. Items 9 and 10 rejected normal return because their new
pre-arithmetic guards do not yet exist. No inherited test, configure step,
warning-clean build, API-contract build, dependency provenance check, or
unrelated job step failed.

The GitHub Node.js deprecation annotation concerns the hosted Action runtime,
not this C++ project. The MSYS2 installer notice is likewise not a project
compiler warning; the warning-clean build steps succeeded.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both
formal job records and aggregate logs, the complete formal logs ZIP, exact
workflow, source identity, and the supplementary draft run plus both draft job
records/logs. The formal ZIP has 30 unique, unencrypted, path-safe members and
passes `unzip -t`; its two aggregate logs are byte-identical to the separately
retained formal logs. GitHub reported zero uploaded artifacts.

The raw download set and a fresh expansion of the ZIP passed Gitleaks 8.30.1
with redaction and independent targeted sensitive-filename/content scans. No
matching secret text is retained. A final same-directory scan binds the
completed receipt set before `MANIFEST.sha256` is generated.

This exact R1 red boundary is accepted. The next production-only green step
must make these same 36 tests pass without weakening, deleting, reordering, or
renaming the new tests and without changing their expected results. A new
OpenFHE API or algorithm ambiguity is escalated immediately to terminal Fable
5.1 with fallback disabled and emitted model identity verified; routine
mechanical implementation and CI work continue without waiting for ZCode.
