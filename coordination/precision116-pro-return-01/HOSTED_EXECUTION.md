# Hosted precision116 execution

## RED dispatched once

- Source: `70c37679f4760c6dc2bebc35bdfd741c238f315b`.
- Ref: `codex/precision116-profile-seam-red-20260907` (also pushed to the safe working branch).
- Run: https://github.com/leemaple/20231788./actions/runs/34050734415
- Created: `2026-09-06T18:07:48Z`.
- Initial observed state: IN_PROGRESS, not RED accepted. Windows job `101533759804` was installing its official toolchain; Linux job `101533759903` was building pristine OpenFHE. No project compiler diagnostic was available yet.
- No dispatch, rerun, cancellation, or second activation-ref push. Production GREEN is not applied.

## Subsequent GREEN orchestration headroom

The unchanged new CTest timeout is 1200 seconds. Its outer workflow step previously also expired at 20 minutes, leaving no shutdown/reporting headroom. The working branch now increases only that GREEN-only step to 25 minutes on both hosts; the already running RED source and all cryptographic assertions remain unchanged.

Test-first evidence: changed the bounded CI checker expectation to 25, then ran it against the old workflow: 5 tests, 4 PASS / 1 FAIL, 0.019 seconds, with exact assertion `20 != 25`, exit 1. Changed only the two outer timeout values and reran: 5/5 PASS (exact duration retained in the command output). `git diff --check` passed. These are workflow source checks, not a cryptographic RED/GREEN result.

Current scientific claims remain original E80 FAIL, experimental operation NOT RUN, full-eight experimental E80 NOT TESTED, and security UNRESOLVED. Inspect the exact run's actual compiler diagnostics before accepting RED or applying GREEN.

## Actual Linux RED accepted before GREEN application

Linux job `101533759903` is terminal FAILURE at `Build paper full eight-square contract`. Its pristine OpenFHE build and project prerequisites succeeded. The retained raw job log is `RED_LINUX_JOB.log`, 464450 bytes, SHA-256 `f246a0a13f2422313d120602074e4372abc2f73cda27bceee122965cb3496a0a`. Provenance in that log binds source `70c37679f4760c6dc2bebc35bdfd741c238f315b`, run `34050734415`, attempt 1, OpenFHE pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`, GCC 13.3.0 and CMake 3.31.6.

At `2026-09-06T18:12:51.2182979Z`, the first C++ diagnostic is at new header line 285: `CreateExperimentalPrecision116Setup` is not a member of `openfhe_2023_1788`. The explicit target build exits 2 at `18:12:57.3759621Z`; this is the intended missing-factory RED, not the unexpected-success guard or an environment failure.

The compiler also reports undeclared `savedLow` and two initializer-list deduction errors after the missing-factory error. Source inspection shows these expressions depend on types derived from the failed setup expression; they are provisionally cascading diagnostics, not established independent defects. Leave all RED test assertions unchanged and require actual GREEN compilation to resolve them.

Before this expected compile failure, the log establishes all five Relin2/RS2/Mult2/Add/Sub API targets built and the complete original 60-test checkpoint passed in 1.97 seconds. Earlier selections also passed 1, 2, 57, 1 and 2 tests respectively (123 invocations, 60 unique names across the selected checkpoints). This does not include the original full-eight test or the new operation test.

Root accepts the actual Linux compile RED as the test-before-implementation gate required by TASK section 5. Windows is still running the same unchanged RED source and will be inspected separately; its outcome is not inferred from Linux. GREEN may now be applied on the working branch without altering the ongoing RED run. Experimental runtime and precision acceptance remain pending.

## GREEN dispatched once

- Source: `2759fa90840946ef42957c7ba71ebea47e0e4995`, pushed and `ls-remote` verified on the safe working branch and `codex/precision116-profile-seam-green-20260907`.
- Run: https://github.com/leemaple/20231788./actions/runs/34051183115
- Created: `2026-09-06T18:16:24Z`; first observed IN_PROGRESS. This is not a GREEN pass.
- The accepted Linux RED preceded GREEN application and publication. Windows RED remains live on its separate immutable source and has not been cancelled or restarted.
- All four production files exactly match the reviewed Pro complete copies; tests/CMake remain byte-identical to RED. No assertion was weakened to obtain a pass.
- Before committing, the raw Linux log passed strict Gitleaks (464450 bytes), and the complete staged change passed strict Gitleaks (500100 bytes), both zero findings. Active-source/docs whitespace checks passed; only the exact raw external `.log` was excluded from whitespace checking to preserve downloaded evidence bytes.

Next: inspect the same RED Windows job and the exact GREEN run on both hosts. Require warning-clean compilation, original 60-test/five-API checkpoints and the explicitly named single-operation CTest. Do not dispatch/rerun/cancel or push either activation ref again merely because observation takes time. After actual runtime acceptance, obtain independent final review and proceed to the separate original-input eight-square precision slice; the full goal remains incomplete.

## Windows RED accepted; both RED jobs terminal

Run `34050734415` is now terminal FAILURE, with both jobs failing only at the intended explicit paper-target build. Its final API state is retained in `RED_RUN_STATUS.json`; no rerun is needed.

Windows job `101533759804` confirms source `70c37679f4760c6dc2bebc35bdfd741c238f315b`, attempt 1, pristine OpenFHE pin, native64/backend4, MSYS2 GCC 16.2.0 and CMake 4.4.2. `RED_WINDOWS_JOB.log` retains its exact downloaded bytes (including CRLF), 477517 bytes, SHA-256 `f68d34853bd442d5df363e5a42c6e1c967dacd6b84a58de3ff77f031ff4236d6`.

At `2026-09-06T18:18:47.2682087Z`, its first compiler error is the same missing `CreateExperimentalPrecision116Setup` at header line 285. `savedLow` and receipt initializer-list errors follow, again provisionally cascading pending GREEN compilation. Ninja exits 1 at `18:18:57.7701579Z`. The unexpected-success guard and experimental operation do not run.

Windows had already linked all five API contracts and passed the original 60-test suite in 3.75 seconds, plus earlier 1/2/57/1/2 selections (123 invocations, 60 unique tests total). Both platforms therefore supply genuine missing-factory RED evidence after successful prerequisites. Neither supplies experimental numerical evidence at this stage. The separate GREEN run `34051183115` remains in progress; inspect that run, not this completed baseline.

## Linux GREEN: actual single-operation PASS

Linux GREEN job `101534979303` is terminal SUCCESS at source `2759fa90840946ef42957c7ba71ebea47e0e4995`, run `34051183115`, attempt 1. Exact downloaded `GREEN_LINUX_JOB.log`: 467486 bytes, SHA-256 `9fcb0c7c8edfb9147c9885895990ddd698458c27281d111cd97781c23b2ff630`.

The actual paper target compiled successfully with the unchanged RED assertions. The missing-factory and its subsequent deduction diagnostics are therefore resolved on GCC, without a test edit. The log confirms five API targets built, all prior selected checkpoints passed (123 invocations / 60 unique tests; final 60-test checkpoint 2.50 seconds), then exactly one `experimental_precision116_profile_seam` CTest selected with `--no-tests=error`, OMP=2, and timeout 1200.

At `2026-09-06T18:23:59.1996825Z`, it emitted `EXPERIMENTAL_PRECISION116_PROFILE_SEAM result=PASS profile=experimental-s116-d56-b58-v1 squares=1 full_eight_square_E80=NOT_TESTED security=UNRESOLVED`. CTest reports 1/1 passed in 10.85 seconds. This exercises actual candidate construction, public encryption, DCP, one Mult2, exact scale/metadata/basis/receipt checks and ownership cleanup as specified by the preserved test; it contains no numerical eight-square measurement.

Windows GREEN job `101534979053` is still in progress; do not infer portability or its test result from Linux. Once both platforms complete, finish independent semantic/runtime review and move to the next separate full-eight precision slice. The original paper-profile E80 failure, experimental full-eight NOT TESTED, and unresolved security remain unchanged.

## Windows GREEN: PASS; run terminal SUCCESS

Run `34051183115` is now terminal SUCCESS on both hosts at exact tested source `2759fa90840946ef42957c7ba71ebea47e0e4995`; final API state is retained in `GREEN_RUN_STATUS.json`. Do not poll or rerun the completed RED/GREEN jobs.

Windows job `101534979053` also compiled the unchanged test and implementation, linked all five API contracts, and passed 123 prior invocations / 60 unique regression tests (final 60-test checkpoint 3.59 seconds). Then its sole experimental CTest emitted the same profile-specific one-operation PASS at `2026-09-06T18:28:37.1062839Z`; CTest reports 11.75 seconds for the test, 11.76 seconds total. Exact `GREEN_WINDOWS_JOB.log`: 479437 bytes, SHA-256 `30b3b31b0245dfddfdb164d04d1d85647163a4b82ebf1baaefa3b3f678924d4e`.

Both observed platforms therefore resolve the earlier cascading compile diagnostics without any test change and execute the specified one-operation seam. Runtime acceptance is bounded to that seam and these samples. Independent final runtime/semantic review is now assigned to the existing separate Codex reviewer (`FINAL_RUNTIME_REVIEW.md`, pending at this update); an independent Pro semantic/next-numerical task is still to be prepared and actually submitted with complete context. Neither full-eight numerical accuracy nor security is established. No additional trial batch is requested.

## Independent runtime audit received

Root has read the complete `FINAL_RUNTIME_REVIEW.md` (SHA-256 `ea7906455dfadf9ae195538fba2eadb2a0fa47c3fcc7f154b6c1dbd0f3888785`) from a separate Codex context. It independently reconciles source, frozen RED bytes, four GREEN postimages, raw job logs and final run status, and finds no blocker to the scoped dual-host one-operation integration acceptance. Root accepts that bounded disposition.

In particular, the one-operation test does not decrypt its result or compare it with a one-square plaintext oracle. It proves execution and specified structural invariants, not even first-square numerical precision. Experimental terminal RCB/binding, operations 2–8, full-slot errors and E80 remain unexecuted. The next task brief preserves those real remaining obligations; independent Pro semantic review and the new numerical draft have not yet been submitted.
