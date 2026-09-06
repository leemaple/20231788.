# Hosted CTest timeout protocol RED boundary

Status: test-only RED design frozen 2026-09-06. This slice has not been run on
GitHub Actions and does not implement the missing timeout handoff.

## Purpose

This is the disposable status-only transport observation required by
`coordination/fs-endpoint-wrapper-01/INTEGRATION_CONTRACT.md` section 8. It
checks the actual CTest #61 prefix/timeout transport, the existing one-shot
wrapper, Windows MSYS/native paths, always-run selection, and exact artifact
transport without running OpenFHE, crypto, or the 16,384-row replay.

It does not relax the adopted specification's prohibition on mixing synthetic
files into live evidence. Every invocation uses `scope=synthetic`, a fresh
private scratch root (descriptive on Linux and deliberately shortened to
`RUNNER_TEMP/fsew` on Windows), and an artifact named
`fs-endpoint-synthetic-ctest-protocol-<host>-<run>-<attempt>`. Only the one
validated incomplete status path is uploaded. The primary CTest log, generated
CMake project, selector manifest, canonical directory, and marker are never
uploaded and are not numeric evidence.

## Exact seam

The workflow is push-only and matches exactly
`codex/endpoint-ctest-protocol-red-20260906`. Each Ubuntu 24.04 and Windows 2022
job is limited to ten minutes and performs:

1. validate GitHub-hosted identity, host, Python 3.12, and the exact checked-out
   `GITHUB_SHA` before allocating a fixture;
2. generate a temporary CMake project with 60 inert tests followed by the sole
   selected test `paper_full_eight_square_contract`;
3. configure without building and invoke the real
   `tests/run_paper_endpoint_once.sh` exactly once with CTest's timeout fixed at
   two seconds;
4. retain the real nonzero wrapper status, inspect a bounded escaped rendering
   of the actual captured CTest bytes, and never interpret those untrusted bytes
   as commands;
5. under `if: always()`, run the real finalizer selector, validate its exact
   one-line status-only manifest, then upload and download that one path;
6. only after download, require the decoded reason to be `TIMEOUT` and require
   downloaded bytes to equal the selected status.

The emitter writes one exclusive marker, emits one flushed sentinel, and then
sleeps long enough for CTest to terminate it. The inspector requires exactly
one `61: CTEST_PROTOCOL_SENTINEL` occurrence but does not predict a CTest
timeout diagnostic grammar. It reports bounded `repr`-style physical lines so
the first hosted RED preserves the real grammar and line endings.

## Expected RED and failure classification

The current wrapper does not pass an observed timeout flag to the current
finalizer CLI. Therefore the wrapper is expected to return the real nonzero
CTest shell status and the current status is expected to decode as
`CTEST_FATAL`, while the final verifier requires `TIMEOUT`. This mismatch is the
intended RED; it is not an expected-failure test and the job remains failed.

- wrapper step nonzero: expected consequence of the real CTest timeout;
- inspector, selector, pre-upload validation, upload, or download failure:
  protocol/setup/transport failure, not the intended RED;
- final `TIMEOUT` assertion failure after successful download: intended missing
  timeout-integration RED;
- wrapper zero, missing/duplicate emitter marker, wrong #61 prefix, or any
  non-status artifact: protocol failure.

The upload occurs before the strict `TIMEOUT` assertion, so the intended
verifier failure cannot suppress the already validated status-only artifact.
An `always()` condition schedules recovery steps but never changes the earlier
wrapper failure into success.

## Platform boundary

Linux uses the action checkout. Windows follows the existing workflow's manual
checkout into `C:/openfhe-endpoint-ctest-protocol/cleanroom`, then uses the
existing `msys2 {0}` MINGW64 shell. MSYS tools receive `/c/...` spellings;
native Python/finalizer receive `cygpath -am` spellings. `RUNNER_TEMP` and
`GITHUB_OUTPUT` are converted only inside workflow run steps. The repository's
historical trailing-dot name is never used as a Windows workspace path.

The Windows scratch name is short because the publication transaction repeats
the long identity stem in its private staging filename. Before any fixture or
scratch allocation, native Python constructs the exact deepest
`published/<stem>/.staging/<stem>.candidate.status.json` spelling from the
actual runner temp and identity and requires at most 247 characters, with no
`\\?\` long-path prefix. This keeps a conservative traditional `MAX_PATH`
boundary rather than depending on host long-path policy.

The pinned MSYS2 action defaults `path-type` to `minimal`, which intentionally
does not inherit tool paths added by setup-python. This workflow explicitly
uses `path-type: inherit`, then compares `command -v python` after `cygpath -am`
to setup-python's exact `python-path` output before running the fixture. A
different interpreter is a setup/transport failure.

## Owned and excluded files

Owned additions are only:

- `tests/paper_endpoint_ctest_protocol_gate.py`
- `.github/workflows/endpoint-ctest-protocol-gate.yml`
- this file
- `coordination/fs-endpoint-ctest-protocol-01/AUTHOR_NOTE.md`

No production source, existing test, CMake file, finalizer/reader/wrapper,
`dcp-rcb.yml`, canonical payload, crypto, live chain, workflow dispatch, commit,
or push is in scope. A later GREEN may wire the observed timeout fact into the
finalizer exactly once; it is not part of this RED slice.
