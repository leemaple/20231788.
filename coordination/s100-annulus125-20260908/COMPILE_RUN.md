# First compile-controls run — active checkpoint

TERMINAL UPDATE: run34183786327 completedSUCCESS2026-09-08 11:33:05CST,
job101927970054 duration1m55s. Root inspected the downloaded9-file artifact:
warning-clean candidate build57s; exact CTest show-only registration; keyless controls13s,
stdout `s100-annulus125-e80-v1 controls=PASS encrypted_runs=0`, stderr empty.
Source/harness3/3PASS1.823s, receiver synthetic controlsPASS. No default/legacy/ciphertext
test executed. OpenFHE install cache hit, source pin clean. GCC13.3.0, Python3.12.14,
CMake3.31.6, Boost1.83.0.1ubuntu2. Full run JSON, file hashes and raw logs retained locally
beside this file. Artifact id10039833753,5801bytes, GitHub-reported ZIP digest
`7bd5ef055c3b86f9782c67ecd430b9f75f8b14cb8135e59d703f20c732bc99f1`;
the downloaded extracted files were separately hashed, not confused with ZIP bytes.
The watch exited0; do not poll this completed run or rerun it.
Node20-targeted pinned actions were reported forced onto Node24 by GitHub; this warning
did not fail the job and is not a scientific correctness issue. No action migration now.

Observed 2026-09-08 11:31:26 Asia/Shanghai:

- Exact source `def248a04b7212088e41a72e2239496dcd3e7027`, committed and pushed.
- Branch `codex/s100-annulus125-20260908`.
- Run [34183786327](https://github.com/leemaple/20231788./actions/runs/34183786327),
  workflow `S100 annulus125 compile and controls`, status `in_progress`, no conclusion yet.
- Triggered once by the workflow/source push, not a manual rerun or scheduler.
- Root independently read all four CI/harness files, resolved top-level context,
  owner-only build and hidden-artifact concerns, and completed the dependency allowlist.
  Final local source/fake-process check:3/3PASS,1.665s. `git diff --check` PASS.
- Production src/include and old dcp-rcb.yml unchanged from1ccb219 (and tested runtime223667e).
- No local CMake/build/FFT/crypto. No new encrypted payload anywhere in this phase.
- C++ assertions and keyless controls are still pending runtime evidence at this checkpoint.

Continue from this SAME run; do not dispatch or rerun because it is taking time.
After terminal, retain exact run/job metadata and the scoped compile/controls artifact,
including failures, and inspect raw compiler/control/provenance logs before concluding.
If a build/static issue occurs, diagnose/fix that explicit issue with new source; no sample
has yet launched. Only after these checks and source review may root add the separate
predeclared single-payload path. `run_once.py` is still unwired; the current workflow can
only launch fake-process tests and the keyless --controls binary entry.

The next docs-only checkpoint push must not start another CI run (paths filter).
Original near-unit S100 FAIL, Table3 provenance/statistics/performance and security limits
remain unchanged. Goal active/incomplete. No new timer, default merge, author contact or PDF resend.
