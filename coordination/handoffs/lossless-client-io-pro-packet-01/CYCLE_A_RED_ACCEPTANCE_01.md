# Cycle-A RED accepted on both hosted platforms

2026-09-05, Asia/Shanghai. **ACCEPT_EXPECTED_RED**, not a GREEN result.

Exact engineering source: `12d8fae78cc0d0fed5038cf21cdbd2173fe1f1ef`;
branch `codex/lossless-io-implementation-01`;
[run 33948543866](https://github.com/leemaple/20231788./actions/runs/33948543866),
push, attempt 1, created `2026-09-05T05:57:52Z`.
Both jobs completed/failure: Linux `101258898386` at `06:03:12Z`,
Windows `101258898314` at `06:06:45Z`.

| Gate | Linux | Windows |
| --- | --- | --- |
| Pristine OpenFHE pin/native64/backend4 | Actual build/install PASS | Actual build/install PASS |
| Warning-clean project and five explicit APIs | PASS | PASS |
| Old first-Mult2 focus | 1/1; 0.26 s | 1/1; 0.27 s |
| Old Pair Add/Sub focus | 2/2; 0.20 s | 2/2; 0.19 s |
| Legacy suite | 57/57; 1.40 s | 57/57; 2.58 s |
| New I/O target compilation | Expected missing header; exit 2 | Expected missing header; exit 1 |
| New focus/full58 | SKIPPED/SKIPPED | SKIPPED/SKIPPED |

The unique first compiler error on each host is
`precision_client_io_first_mult2_contract_test.cpp:2:10: fatal error: openfhe_2023_1788/high_precision_client_io.h: No such file or directory`.
No production header/source exists at this commit. This is the intended
missing public behavior, after all old gates succeed, not infrastructure or
an unrelated new-test syntax error. Later new-test syntax/API compatibility
is still uncompiled behind that missing include and remains to be verified.

Two independent Codex reviewers retrieved and audited each complete hosted
job log and actual metadata. Root independently checked both complete retained
logs, all 60 executed starts/commands/pass rows per host (old focus1 + Pair2 +
legacy57), hashes, source/pin/backend markers and the unique expected fatal.
The live CTest JSON's exact old57 names/order/commands were also checked by
each host auditor against the fixed source. Root independently regenerated
the unchanged 3,869-byte name<TAB>command<LF> ledger:
`3527832e2d46591c46a93d3cb96d5469a9362ec4ca1ba39c8ed0587964e77f8b`.
Job API confirms the sole failed step and both downstream skips.

Root's first command parser used Unix shlex before normalizing Windows
backslashes, causing a Windows path-comparison rejection. Normalizing the
observed path separators before tokenization corrected the verifier; all 60
commands then matched. No project, expectation or threshold changed.

## Retained complete evidence

- Linux raw/retained log: 277,436 bytes, 3,285 lines;
  SHA-256 `4bbfe7af8819d622d68ccea87017d5609139cf1f51f71ea0893b0bd767ee55f4`.
  All 204 trailing-whitespace lines remain unchanged.
- Windows raw: 286,717 bytes, 3,521 CRLFs;
  SHA-256 `80695dc71489a48b46e02d20ac1adc3b979772fd5e60f56b6f210fca826e8efd`.
- Windows checked-in LF-only normalization: 283,196 bytes;
  SHA-256 `9a29144507323db4fba8d1e18a65008828976c64779d7330aa7fac3aa4569658`.
  Raw UTF-8 and API responses remain losslessly preserved in ignored
  `artifacts/handoffs/io-cycle-a-red-01/windows-api-capture.json`.
  Root verified raw hash and exact CRLF-only transformation.

Read the two named JOB logs, two host VERIFICATION JSONs,
CYCLE_A_RED_ROOT_VERIFICATION_01.json and CYCLE_A_RED_RUN_FINAL_01.json.
Toolchain deprecation/MSYS installation notices are retained, not incorrectly
described as C++ warnings. There were no compiler warning diagnostics.
The new test's backend static_assert itself has not compiled yet.

## Next authorized slice and claim boundary

Commit and push this evidence with `[skip ci]` before authoring A GREEN.
A GREEN may add only the minimal production client-I/O header/source and
CMake target_sources required by the already frozen A tracer, rational
ownership/lifetime and malformed-key tests. Do not change either new test
file, workflow, old source/tests or precision/vector contracts. B clone
mutation and shared-Params drift coverage/revalidation remain the next
separate RED/GREEN pair.

No new I/O runtime, numerical error, 32 rejection execution, transform or
receipt acceptance has happened yet. This does not prove production I/O,
the paper experiment or the full goal complete. No Mac compilation or
cryptographic experiment was run. Healthy hosted execution was not rerun.

## Prepublication secret-scan disposition

Root gitleaks8.30.1 scanned the complete handoff directory (800,107 bytes),
with ambient configuration unset, inline allows ignored, /dev/null ignore
file and decode depth5. It reported two generic-api-key candidates at Windows
VERIFICATION JSON lines22/23: raw_job_api_sha256 and raw_run_api_sha256.
Root independently recomputed each from the exact retained public GitHub API
response UTF-8 bytes, checked the response run/job identities and obtained
exact equality with both digest fields. Both are ordinary SHA-256 evidence
digests, not API credentials. Thus there are two reviewed false positives,
zero unresolved findings; this is not claimed as a zero-finding scanner run.
No scanner allowlist or evidence field was changed to suppress them. A first
attempt to emit a redacted report to /dev/stdout was rejected as unwritable;
the subsequent redacted verbose scan provided both exact rule/line locations.

Source diff check remained clean. Staged evidence diff check excludes only
the two exact raw-log paths so preserved whitespace is not silently rewritten.
