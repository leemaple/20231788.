# One-shot wrapper draft evidence — 2026-09-06

Root-owned subprocess orchestration draft; independent source review is queued
to /root/endpoint_canonical_writer after its nonoverlapping typed-error work.
Do not call the full wrapper/finalizer pipeline accepted yet.

Source SHA-256:
1c83afb0c5f332048d2e40df0292b933a3490097ba660d74a054b31798015033.
Tests SHA-256:
eb0377f96f146ab7ef0184781d00c8dcb94dead3761836982d9c385148ee7ae3.

Actual bounded bundled Python3.12/-B results:
- 01_missing_wrapper_red.json: absent wrapper, 1 actual failure, exit1.
- 02_first_green.json: bash -n succeeds; first external subprocess tracer
  1 PASS, 0.841s, exit0.
- 03_status_and_signal_coverage.json: bash -n succeeds; 4 PASS, 5.445s.
  Includes seven status-precedence subcases, actual SIGKILL of only a disposable
  fake CTest child (Bash137 retained), missing finalizer/nonempty scratch
  preconditions before CTest, and unchanged preexisting scratch file. These
  additional cases were first-run green, not a claimed additional RED.

The test harness copies the actual script into a disposable synthetic project.
Its fake external Python executable checks the finalize invocation but is not
the real finalizer; the Python version preflight is a separate external call.
No original CTest/crypto/compiled program/full16K replay is executed. Missing
real finalizer in the project currently stops this draft before CTest.

Windows cygpath/native Python, hosted CTest prefix/timeout grammar, real
finalization, upload scheduling and live C++ writer call are NOT VERIFIED.
Original E80 FAIL and A NOT_ADOPTED remain unchanged; process-control success
cannot discharge them. No new workflow trigger or CI run was created.
