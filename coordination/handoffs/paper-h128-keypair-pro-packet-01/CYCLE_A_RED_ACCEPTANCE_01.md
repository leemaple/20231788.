# h128 Cycle-A RED accepted — 2026-09-05

Observed: genuine missing-adapter RED is confirmed independently on both hosts.
This acceptance is saved before any Cycle-A GREEN implementation is applied.

- Tested source: `a21216f0a8f854f478129d02fd32f496bd80f71c`.
- [Run 33943456483](https://github.com/leemaple/20231788./actions/runs/33943456483),
  attempt 1, automatic push event, completed/failure.
- Official OpenFHE: `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Linux job `101245135874`, completed 04:07:08 UTC.
- Windows MinGW64 job `101245136018`, completed 04:09:05 UTC.

## Required evidence before the intentional failure

| Check | Linux | Windows MinGW64 |
|---|---|---|
| Exact source/pin, native64/backend4, actual official build/install | PASS | PASS |
| Debug warning-clean project build | PASS | PASS |
| Five explicit Relin2/RS2/Mult2/Add/Sub API builds | PASS | PASS |
| Existing first-Mult2 focus | 1/1, 0.29 s | 1/1, 0.28 s |
| Existing Pair Add/Sub focus | 2/2, 0.17 s | 2/2, 0.14 s |
| Legacy regression | 57/57, 1.30 s | 57/57, 1.84 s |
| New h128 target build | Expected failure, exit 2 | Expected failure, exit 1 |
| New focused test / full 58 | SKIPPED / SKIPPED | SKIPPED / SKIPPED |

On each host the only failed step is `Build fixed-Q h128 client keypair contract`.
The actual diagnostic is at `paper_h128_client_keypair_contract_test.cpp:2:10`:

```text
fatal error: openfhe_2023_1788/paper_h128_client_keypair.h: No such file or directory
```

This is the required missing production seam, not a fixture typo, unexpected
source regression, runtime failure or passing new test. No h128 keypair was
created or exercised in this RED run.

## Independent verification and retained evidence

Root read complete job metadata/logs and programmatically verified each host's
57 actual test indices, names, normalized commands and ordering against the
tested SHA's CMake Git blob, plus required successful steps, sole expected
failure, explicit source/run/attempt provenance and skipped new/full steps.
The per-host VERIFICATION JSON files retain those 57-entry comparisons.

`h128_candidate_spec` independently read the Linux API log/metadata and completed
the same 57-entry comparison. Its exact raw hash matches Root's retained file.
`h128_profile_provenance` independently read the Windows API log/metadata and
completed the 57-entry comparison. A later overly strict PASS-line regex in
that independent audit failed; the reviewer disclosed it rather than claiming
that parser passed. Root's independent full summary/step/binding validation
passed. The raw CI result itself is unambiguous and the parser issue is not a
project failure.

Complete Linux log: 204,956 bytes, SHA-256
`03ba83d8697a7bbbeaf6c4095b9beca34ed293de2e2c0368049a922002092b70`.
Windows raw captured text: 211,198 bytes, SHA-256
`9432bcbfde9048a34e4167e8ea0e3ace412f54a6171e7c0b283a54d47afceaa8`.
Windows checked-in log is explicitly LF-normalized: 209,130 bytes, SHA-256
`bc00e95ef930399ac28c6c79cb0106b874a02c9188126041d140a4a424618e0a`.
The original CRLF text is losslessly recoverable from the ignored JSON capture
listed in `CYCLE_A_RED_LOG_IDENTITIES_01.json`; normalized text equality was
verified after normalizing both sides' encodings to bytes. No second CI run was
needed for log normalization. Gitleaks 8.30.1 found no secrets in either log.

The captured logs contain original runner trailing whitespace. An unqualified
staged diff check reports that whitespace; it is intentionally preserved to
retain evidence hashes. The subsequent documentation diff check excludes only
these two named log files, not any source, test, workflow or other document.

## Next authorized boundary

Apply only original `0002-green-paper-h128-client-keypair.patch`, keeping the
frozen A test/profile and reviewed CI ordering exactly unchanged. Require both
hosts' warning-clean/API/focus/full 58 success before Cycle B. Keep genuine
B RED and B GREEN separate, with their own hosted evidence. No default merge.

This is a TDD evidence gate, not a successful h128 implementation or paper
result. All builds/tests above ran on hosted Linux/Windows; none on the Mac.
Other Pro terminal outputs were only read and preserved in the separate
`PRO_TERMINAL_OBSERVATIONS_20260905_1206.json`; their artifacts remain unaccepted.
