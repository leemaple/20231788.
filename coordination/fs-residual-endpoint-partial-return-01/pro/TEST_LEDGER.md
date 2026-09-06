# Test ledger — actual versus NOT RUN

## Overall disposition

**PARTIAL_NOT_GREEN.** Primitive arithmetic/format/compression/JSON unit tests do
not establish observer correctness, live evidence integrity, or E80 acceptance.

## Input verification (executed)

Input archive: `fs-residual-endpoint-green-2fe655d4.zip`; 2145068 bytes;
SHA-256 `7c70ed57b3a54eba16115d6dd600310d967f5077f669fb82c0875f76b63024c1`. CRC, safe unique regular member paths, all 174 payload sizes
and SHA-256 hashes, manifest closure/self-exclusion and the 39-file engineering
inventory were checked with the Python standard library. Exact per-path Git-blob
matches and recorded manifest field names are in `evidence/INPUT_VERIFICATION.json`.
No remote repository was queried.

## Actual test-first primitive runs

Environment: `Linux-6.18.35-x86_64-with-glibc2.41`.
Python executable: `/opt/pyvenv/bin/python`.
Python: `3.13.5 (main, Jul 15 2026, 20:25:40) [GCC 14.2.0]`.
zlib compile/runtime: `1.3.1` / `1.3.1`.

Both runs used:

```text
/opt/pyvenv/bin/python -B -m unittest -v test_endpoint_evidence_primitives
```

`PYTHONPATH` was empty and `PYTHONDONTWRITEBYTECODE=1`. The baseline namespace
contained only the test file; the next namespace contained the byte-identical
test file and the implementation. Each subprocess had a 40-second execution cap.
They are disposable local scalar/file test namespaces, not chain evidence.

| Run | Actual exit | Actual unittest count | Meaning |
| --- | ---: | ---: | --- |
| Python primitive RED | 1 | 1 | Missing new module observed: True |
| Python primitive implementation | 0 | 23 | PASS for this primitive slice only |

The test source defines 23 test methods, with deterministic subcases.
Do not count subcases or repeated invocations as additional unique test bodies.
Exact commands, working directories, UTC start times, code identities, exit codes,
stdout/stderr byte counts and hashes are in `evidence/PYTHON_RUNS.json`. The raw
stdout/stderr and `.exit` records are retained beside it. A missing-module import
failure is an expected API-absence baseline, not a numerical failing assertion.
No older transient run is substituted for these retained executions.

## Executed source checks

Both Python files were parsed with `ast.parse`. Both ordered new-file patches
were independently reconstructed from their added hunk lines and matched against
the complete delivered file bytes. No original engineering file was altered.
The output archive itself was reopened and every returned payload/manifest entry
was checked during packaging. No C++ compile or source execution was used.

## NOT RUN / NOT IMPLEMENTED

All seven C++ helpers, the four sparse all-slot controls at both precisions,
endpoint FFTs, direct768 comparison, C++ canonical validator and malformed-input
tests, actual Horner conversion checks, 24-check receipts, all-slot signed
metrics, scalar Decimal replay, schema-specific filesystem/wrapper tests, CTest
parsing, owned directory/atomic publication and workflow integration are not
implemented in this return and were not run. No OpenFHE, codec, FFT, NTT,
encryption, decrypt, build configuration, C++ compilation, library install,
benchmark, hosted test, CI dispatch, push or independent agent call occurred.

## Future commands (not executed; blocked on complete integration)

The currently retained build/link RED is not repaired by this package. Do not
interpret the following as ready-to-run instructions for this partial patch:

```text
cmake --build <existing-build-directory> --target paper_full_eight_square_contract_test --parallel 2
<existing-build-directory>/paper_full_eight_square_contract_test --endpoint-observer-self-test
ctest --test-dir <existing-build-directory> --show-only=json-v1
ctest --test-dir <existing-build-directory> --verbose --output-on-failure -R '^paper_full_eight_square_contract$'
```

The complete-task workflow must retain old60/API gates before paper build,
execute the self-test once, then required synthetic evidence checks, then the
existing no-argument paper CTest exactly once per host with the existing timeout,
serial setting and OMP_NUM_THREADS=2. The missing wrapper must capture the actual
CTest shell status, finalize once even on nonzero and preserve that nonzero.
One Linux and one Windows chain are the maximum next authorized observation
following completed integration/review, not an execution claim or pass promise.
There must be no automatic repeat after unsupported, compile, integrity or
timeout failure. New-source identity must not be mislabeled as 2fe or 9f.

## Scientific status separation

Historical supplied E80: **FAIL retained**. This work does not rerun, repair,
weaken or replace it. New endpoint observer: **NOT IMPLEMENTED / NOT RUN**.
Primitive test outcome: **PASS**, only within the boundaries above.
No full-slot inherited-error attribution, interval root certificate, production
correctness conclusion, or adoption of A is made.
