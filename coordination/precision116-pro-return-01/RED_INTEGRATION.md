# Precision116 RED integration

## Observed before hosted execution

- Parent: `4647c5bb23f6b0f84900b3009e7d1f52c2f59ddf` on `codex/precision116-profile-seam-20260907`.
- Independent RED and GREEN source reviews found no actionable source defect. Both explicitly remain uncompiled and unexecuted; GREEN acceptance is conditional on actual RED evidence.
- Applied only the three RED paths using `apply_patch`. `cmp` verified all three active files byte-for-byte against the retained Pro RED complete copies. The new header is 419 lines; CMake adds eight lines and main adds ten.
- `git diff --exit-code -- src include` succeeded: production remains unchanged and the new factory remains absent.
- `git diff --check` succeeded. The bounded CI wiring suite passed 5/5 in 0.018 seconds; this is not C++ or cryptographic evidence.
- No local compiler, CTest, OpenFHE construction, or cryptographic experiment was run.

## Hosted execution gate

Push this RED source exactly once to `codex/precision116-profile-seam-red-20260907`; retain its SHA and resulting workflow run identity. Accept RED only if actual compilation of `paper_full_eight_square_contract_test` reports the missing `CreateExperimentalPrecision116Setup` factory. Dependency/setup failure or the unexpected-build-success guard is not the intended RED.

The normal 60-test regression and API gates retain their prior selections. This RED ref cannot execute the experimental operation or the original endpoint chain. Do not apply GREEN until the hosted diagnostic is inspected. Before GREEN execution, give the outer CI step 25 minutes of headroom around the unchanged 1200-second CTest timeout.

Scientific status is unchanged: original-profile full-chain E80 FAIL; experimental one-operation NOT RUN; experimental full-eight E80 NOT TESTED; security UNRESOLVED.
