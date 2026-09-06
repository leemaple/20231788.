# REPRODUCE.zh-CN.md bounded review

## Outcome

No actionable factual error, misleading assurance claim, or broken reproduction command was found in `REPRODUCE.zh-CN.md` (6656 bytes; reviewed SHA-256 `1346a07ed54e24b687550878ee15732e389ebe861ca47445de70defad0eb743e`).

The guide correctly separates the paper's t=2 method from the changed `experimental-s116-d56-b58-v1` parameters, preserves the original-profile failure, and limits the two-host result to one chain per host rather than a performance, arbitrary-input, repeated-trial, security, or production claim. Its Linux/Windows errors and 20.54/21.89-second CTest durations agree with the retained raw logs and runtime review. The approximately 1/32 and 1/24 error-limit fractions are consistent with the recorded exact values. The stated native64/backend4 OpenFHE pin, GCC and Boost versions, 60-test regression boundary, all-slot endpoint checks, intermediate ten-anchor limitation, unresolved security, and lack of refresh/retry are accurate.

The shell block is coherent with CMake and the hosted wiring: the clone URL intentionally contains the repository's trailing dot plus `.git`; delivery commit `1552ebe04ba1406e6f735bd7eeb19413aca3f740` exists; its `src/`, `include/`, `tests/`, and `CMakeLists.txt` are unchanged from tested source `2b8b349edf5575556347082c1b725f6696c743b6`; the option, target, anchored CTest name, OMP2 setting, and 1200-second CTest property are exact. The block explicitly begins after a fresh pinned OpenFHE installation, while the linked workflow supplies the full platform-specific dependency build and Windows path conversion. The warning against unfiltered CTest is warranted because the numerical target is `EXCLUDE_FROM_ALL` and opt-in.

The integration guidance matches the public API and compiled example: `RunCandidate` owns setup/encryption/binding/final decryption; `Evaluate` receives only the immutable public plan and read-only ciphertext and performs DCP, eight `Mult2` calls, and terminal `RCBWithReceipt`; the independent secret/CRT/Horner oracle remains client-side test code. `ClientReal` and `PositiveRationalScale` support the stated high-precision/exact-scale boundary.

The default branch is `cleanroom/reimplement-mult2-20260831`, is listed in the workflow trigger, retains the ordinary regression/API steps, and excludes both archived expensive numerical paths. Thus the guide's statement that a normal default-branch green run does not replace the cited scientific run is accurate.

## Review boundary

This was source/documentation reconciliation only. I did not execute the guide's commands, configure CMake, compile, run cryptography or CTest, poll/download CI, or repeat numerical analysis. Review responsibility is the separate Codex task context `endpoint_interop_workflow`; its exact model/backend identity is unknown/unattested.
