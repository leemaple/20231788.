# Final corrected-source and CI integration review

## Disposition

No remaining source-level integration blocker was found in the corrected test/CMake/workflow slice. The previously reported evaluator-boundary finding is closed by the active test change. The distinct option-enabled build is a valid alternative to changing the legacy selection regexes: the original build keeps the option OFF and therefore keeps its 62-test registry and existing 57/60 selections unchanged; only the later fresh build registers and explicitly builds the new test.

This is a bounded static/source disposition. Compilation, CTest, cryptography, hosted execution, numerical PASS/FAIL, runtime cleanup, and security remain pending.

## Identity correction

The earlier `INTEGRATION_REVIEW.md` phrase “using GPT-5” was not backed by an attested runtime/backend identifier and is withdrawn. The accurate identity for this review responsibility is the separate Codex task context `endpoint_interop_workflow`; its exact model/backend identity is **unknown/unattested**. This remains independent context, not provider diversity.

## Corrected evaluator boundary

The active test differs from the archived Pro postimage only in the accepted boundary correction. `Evaluate(plan, input)` no longer calls `ScaleOracle`, `CheckReturned`, `InspectAncestry`, `EmitReturned`, or `xp::CheckCipher`; it performs the one DCP, the loop containing eight `Mult2(pair,pair)` operations, operand/input preservation, required nonterminal rejection probes, and the one successful terminal `RCBWithReceipt`. It receives only the immutable public plan and read-only ciphertext and has no secret, client, encryption/decryption, callback, numeric observation, sparse polynomial, or scale/receipt-oracle path.

The removed checks were retained after `Evaluate` returns: the client-side caller validates the terminal root ciphertext, initial Input state/root receipt, every round's returned state and ancestry, emits round0-through-round8 receipts, and later binds/decrypts the terminal result. The existing terminal-chain assertion still binds `evaluation.result.GetReceipt()` to the final returned receipt and all 32 distinct receipt nodes. `PreserveCipher` remains an intentional structural immutability check inside evaluation; it uses only public ciphertext state and frozen-basis validation, not a secret/client/numerical oracle.

Observed active test SHA-256: `478954a08580d3d342a47ad1e54900f4dce455af1964a3d3e2d231cf61b748d2`. The archived Pro test remains `ff82f162b90a33793506de3bb883b4e12cbc95bea736adbd683d837cca31cea7`; the bounded diff contains only the check moves/removals described above.

## CMake and hosted wiring

- Active CMake SHA-256 `9cba634cedde1e8d96aff8561f5b4ed545b52fac8f1ce523e481a7833e1936f8` exactly matches the reviewed Pro postimage. The option defaults OFF; when enabled it adds one `EXCLUDE_FROM_ALL` target and one serial, OMP2, 1200-second CTest.
- Workflow SHA-256 `6095c87e3c79de388a73fbf816a5bfa772a6175e08631d219eab7f8d12672a05` adds only `codex/precision116-eight-square-observation-20260907` as the new activation branch. The current working branch remains untriggered.
- On each host, the unchanged option-OFF configure/build and legacy 57/60 suites run first. The original paper target is not built on the new ref. A fresh, non-preexisting build directory is then configured with the option ON, the exact `experimental_precision116_eight_square_test` target is built with `--parallel 2`, and exactly `^experimental_precision116_eight_square_contract$` is selected with `--no-tests=error`, verbose failure output, OMP2, and a 25-minute step timeout over CTest's 1200-second limit.
- Linux uses `build-precision116-eight-square` and the proven OpenFHE prefix. Windows uses a fresh suffix of the run/attempt-specific native `PROJECT_BUILD`, converts prefix/build paths under MINGW64, and restores the installed `bin`/`lib` runtime PATH before CTest.
- The new configure/build/run steps use exact-ref positive guards and ordinary success propagation. They do not run on prior refs, the working ref, or after a preceding failure. On the new ref, the prior RED assertion, one-operation profile seam, old paper build, endpoint observer self-test, live endpoint runner, always-selector, and artifact upload all remain disabled, including failure paths.
- Because the new CTest exists only in the second option-ON build, no new exclusion is needed in the option-OFF legacy build. This avoids the unbuilt-test hazard identified in the earlier review while preserving old selectors byte-for-byte.

## Static evidence actually obtained

- `git diff --check`: exit 0.
- `bash -n` over all six new host run blocks: exit 0.
- `python3 -B coordination/precision116-eight-square-return-01/check_evaluator_boundary.py`: 2/2 PASS.
- `python3 -B coordination/precision116-eight-square-return-01/check_ci_wiring.py`: 5/5 PASS.
- Manual comparison of the active test against the archived Pro postimage confirmed only the accepted evaluator-boundary delta; active CMake matches its archived postimage.

The Python checks are bounded lexical/YAML-condition regressions, not C++ type checking or GitHub Actions execution. No CMake configure, compiler, OpenFHE operation, CTest, CI dispatch, push, or numerical replay was performed in this review.
