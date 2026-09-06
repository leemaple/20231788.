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
