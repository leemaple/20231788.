# Cycle-A frozen tracer: dual-platform GREEN acceptance, P2 still OPEN

2026-09-05, Asia/Shanghai. Verdict:
**ACCEPT_FROZEN_CYCLE_A_TRACER_DUAL_PLATFORM_GREEN_WITH_OPEN_P2**.

This accepts actual execution of the frozen low-N first-operation
production-I/O tracer on both hosts. It does **not** accept the entire I/O
module, every requirement of its specification, or the paper-scale system.
The firstModSize=55 constructor-validation P2 remains OPEN; Cycle B has not
been implemented or executed.

## Exact execution identity and TDD boundary

- Project source: `084ffa0af3cb21623151df0c826736ca84954140`.
- Official pristine OpenFHE: `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Branch: `codex/lossless-io-implementation-01`.
- Automatic **push** run [33950923304](https://github.com/leemaple/20231788./actions/runs/33950923304), attempt 1.
- Linux job [101265404189](https://github.com/leemaple/20231788./actions/runs/33950923304/job/101265404189);
  Windows job [101265404098](https://github.com/leemaple/20231788./actions/runs/33950923304/job/101265404098).
- Final retained service metadata reports the run, both jobs and every job
  step completed successfully. The earlier Linux run snapshot was still
  in progress; the [final run record](CYCLE_A_GREEN_RUN_FINAL_01.json), not
  that earlier snapshot, supports the whole-run conclusion.
- Actual RED source `12d8fae78cc0d0fed5038cf21cdbd2173fe1f1ef` was accepted
  and published at `1e1b6339d3c24de6b675958f4de279fe29846fab` before
  production authoring. This GREEN does not retroactively manufacture RED.
  Candidate parent was `46568f52d0e47540a9093221e3b5a64904036a50`.

The tested source added only the production header/source and one CMake
target_sources line relative to its parent. All 21 test/helper files, the
workflow and the old double_ckks public header/source remain byte-identical
to actual RED. In particular, the frozen tracer SHA-256 is
`6b3260a35db8715fff2a3d3a4b24137b7d8c12b6101db422c476b0236ed0ea16`;
the independent oracle SHA-256 is
`9f7d8222ef6520bc845ab1b81fe735f5f7a46a48d5de2c2b984c63148e2c42af`.
The exact tested workflow SHA-256 is
`6ef6c424d7b412bc28f94510a1ba70143378b5a3976e1eebb4b68536033e03fa`.
Later documentation commits do not inherit this CI run as their source SHA.

## Observed builds and five test groups

Times below are CTest's reported total real time, not job-step timestamps,
individual test durations, or cryptographic benchmarks. Each column was
checked against its own complete log and verification record.

| Gate, in execution order | Linux | Windows |
| --- | --- | --- |
| Focused existing first-Mult2 precision | 1/1; 0.26 s | 1/1; 0.23 s |
| Focused existing Pair Add/Sub inputs | 2/2; 0.19 s | 2/2; 0.20 s |
| Original suite excluding new I/O test | 57/57; 1.41 s | 57/57; 2.75 s |
| Focused new production-I/O contract | 1/1; 0.18 s | 1/1; 0.23 s |
| Complete suite including new I/O test | 58/58; 1.57 s | 58/58; 2.72 s |

The five explicit targets `relin2_api_contract_test`,
`rs2_api_contract_test`, `mult2_api_contract_test`,
`add_api_contract_test` and `sub_api_contract_test` all built on both
hosts before the new I/O target. Default builds and the explicit new target
also succeeded. The exact candidate CMake enforces
`-Wall -Wextra -Wpedantic -Werror`; there are no project/API/new-target
compiler warnings or errors in either log. Linux has a runner Node.js
deprecation notice; Windows has an MSYS2 setup warning to terminate other
MSYS2 programs before updating. Neither is hidden or classified as a
project compiler warning.

| Environment/provenance | Linux | Windows |
| --- | --- | --- |
| Runner | Ubuntu 24.04 | Windows Server 2022, MSYS2 MINGW64 |
| Compiler | Ubuntu GCC 13.3.0 | MSYS2 GCC 16.2.0 Rev3 |
| CMake | 3.31.6 | 4.4.2 |
| Actual native/backend | 64 / 4 | 64 / 4 |
| OpenFHE build | Release, cache miss, built/installed | Release, built/installed |
| Project build | Debug | Debug |
| Job interval, UTC | 06:50:41–06:56:15 | 06:50:42–06:58:33 |
| Job duration from service seconds | 334 s | 471 s |

CTest's actual JSON lists and verbose start/command/pass records were
reconciled, not only the counts: 119 executions per host
(`1+2+57+1+58`), 238 total. Original 57 names, order, executable basenames
and ordered arguments are unchanged; only
`precision_client_io_first_mult2_contract` is appended as #58.
The legacy baselines used by the two audits,
`7b8a9d2164037f9d30e94e3b96d245d27f32c61b` and
`4ccc8fd2e7617625d27e58a53eb3489e99466ed4`, have the same 57 bindings.
The canonical `name<TAB>executable and ordered arguments<LF>` map is
3,869 bytes, SHA-256
`3527832e2d46591c46a93d3cb96d5469a9362ec4ca1ba39c8ed0587964e77f8b`.
Exact commands and individual test durations are retained in the two JSON
verifications and raw logs.

## Current-tracer numerical and negative-case evidence

Both hosts executed the new tracer twice: focused and full-suite invocation.
Each invocation created one fresh matching keypair and called EvalMultKeyGen
once. Each checked all 16 complex slots for fresh left/right and the
first-Mult2 product, the frozen distinguishing input/product deltas, public
decryption versus independent secret/schoolbook/CRT/projected-Horner
evaluation, owned output lifetime, state/receipt identity and immutability.
The 32 malformed-key cases per invocation comprise 22 public-key and
10 private-key checks, with exact diagnostic and unchanged-state checks.
Thus there are 64 rejection observations per host, not 64 distinct cases.

Actual source/profile identity is recorded in each success marker:
N=64, S=16, gap=2, depth=7, scaling/first bits 50/55, HYBRID,
FIXEDMANUAL, COMPLEX, EXEC_EVALUATION, FIXED_NOISE_DECRYPT,
STANDARD, HPS, PRE=NOT_SET, noiseScale=1, UNIFORM_TERNARY and
HEStd_NotSet. The successful configured profile does not prove rejection
of a different initial first modulus.

The exact output scale is reduced
`2^200 / 1267650600226646386227681786497`, with
`qDiv=1125899906843009` and `qL=1125899906840833`;
the denominator equals `qDiv*qL`. It is not a reconstructed native-double
approximation. Centered headroom was strictly positive in all four markers.

The following are independent per-host maxima across that host's two
new-tracer invocations. Public and oracle error maxima agree at the printed
digits; that agreement alone is not the basis for claiming oracle
independence. Full per-invocation values, including exact headroom integers,
remain in the JSON records.

| Metric | Linux maximum | Windows maximum |
| --- | --- | --- |
| Fresh all-slot error, public and oracle | 3.60292430090438514963485800816107694467976955e-28 | 3.87483692756146437466295607301791638924661396e-28 |
| Product all-slot error, public and oracle | 8.46261698485907647704174802844342560803305919e-28 | 5.58163623295204212136320375822349375453206218e-28 |
| Input delta error, public and oracle | 6.89416816665887862811066503883884170974752132e-29 | 1.70896113575895622415603243682418341402644224e-28 |
| Product delta error, public and oracle | 2.70170551632643163514201370718312497215589678e-28 | 3.11563396936620759122583947065978765932589402e-28 |
| Public/projected-Horner maximum component disagreement | 1.032506025503259163268e-107 | 1.03250602371991406843e-107 |
| Maximum reported cross-precision disagreement | 9.69978346078924437794422022852937830568511348e-166 | 9.70274900059717989426179792676752223682614831e-166 |

Every reported slot/delta error passes the unchanged `2^-80` bound;
both disagreement measures pass `2^-120`. Linux markers are at raw log
lines 3247 and 5218; Windows markers are at retained LF log lines 3531 and
5509. These are aggregate maxima, not a claim that 16 individual slot
values were printed. Source inspection establishes that the bounded
nonfinite checks, complete all-slot/delta checks and all 32 rejections
precede each success marker.

Root independently reparsed all 238 start/command/pass records and checked
all four numerical records using exact Fraction comparisons. The separate
Linux and Windows audits and this receipt's cross-check used retained
source/log evidence; no local cryptographic execution was performed to
produce these verifications.

## Evidence identity and raw/LF boundary

| Retained evidence | Bytes | SHA-256 |
| --- | ---: | --- |
| [Linux complete raw log](CYCLE_A_GREEN_LINUX_JOB_01.log) | 413578 | 9ad9c0ec5e175809595c2cd050b086f151c45c825fddaff510ef617fc0ff0fbc |
| [Linux verification](CYCLE_A_GREEN_LINUX_VERIFICATION_01.json) | 45566 | 812dd05c6cdccfea8af965f727e1df83afaa65683d4f6e26bc5e74d37ba506d9 |
| [Windows retained LF log](CYCLE_A_GREEN_WINDOWS_JOB_01.log) | 418541 | 96770cb59e065e18ce15dbe10ab5df184c8eab91358a80b8b11021693cf39dbc |
| [Windows verification](CYCLE_A_GREEN_WINDOWS_VERIFICATION_01.json) | 137399 | 0dd6c6665b4b1a66acb62c2d34ea5daa6261feec2424d526490639b476bf242c |
| [Final run/job metadata](CYCLE_A_GREEN_RUN_FINAL_01.json) | 13927 | a87d1abac41f12159faebd440f1c3f921ed27c6c21680a642c0b7313e4711743 |

Linux: raw API output is retained byte-for-byte, including the UTF-8 BOM,
5,287 LF-terminated lines and 390 lines with trailing whitespace. An
independent second raw-API read, used only for hashing, matched its size and
SHA-256. The analysis-only timestamp/ANSI removal is not the retained raw
log: it yields 259,944 bytes, SHA-256
`b8022c8ee8d8e6b1461e01310aab101e283a34bb88ca0fc76bb6e6cf0f76dc19`.

Windows: the retained connector capture at
`artifacts/handoffs/io-cycle-a-green-01/windows-api-capture.json`
is 456,166 bytes, SHA-256
`4d4b3a02ebc9436517b2bddb75713923fcfab120588322e8dce77f8eac8035f0`.
Its decoded raw-log string, exactly re-encoded as UTF-8, is 424,056 bytes,
SHA-256
`aaf9b73e62526aa393bdeeb972fe8432d82b4f60abc87a2e892771ff44535c18`.
It contains a BOM and exactly 5,515 CRLF pairs, with no bare CR.
Replacing only CRLF with LF produces the retained 418,541-byte log exactly;
its BOM and 410 trailing-whitespace lines are preserved. This is a verified
decoded-string-to-LF identity, **not** a claim that this independent audit
re-downloaded or hashed Windows HTTP transport bytes.

Both verification JSON files were fully parsed and their relevant recorded
claims reconciled with exact source and complete retained logs. The Windows
cross-check includes all 119 runtime records, both complete actual CTest
JSON lists, 105 official compilation records, five API links and every
new-tracer numeric field. The Linux log retrieval session and independent
hash session both reached exit 0 with all chunks retained. No external
agent was interrupted and no archived verification script was executed
for this receipt.

## Open boundaries and mandatory next order

The [candidate preflight](CYCLE_A_GREEN_CANDIDATE_PREFLIGHT_01.md) records
one Spec P2: initial context binding does not enforce firstModSize=55.
A fresh firstModSize=56 context is a source-supported acceptance-hole
candidate, **not yet an observed runtime counterexample**. This is an
initial-constructor/profile guard, distinct from later shared-Params drift.
A passing 50/55 positive tracer does not close the issue or waive the spec.

The next sequence is:

1. Root reviews and commits/pushes these completed A-tracer evidence files.
   This receipt does not claim that publication has already occurred.
2. Add a separate public-constructor firstMod55 negative test; capture its
   actual expected RED, implement the narrow guard, then capture GREEN and
   relevant dual-host regressions. Keep the existing vectors, oracle,
   thresholds and 32-key matrix frozen.
3. Then implement and evidence Cycle B clone-isolation/live-Params-drift
   RED/GREEN; retain separate lifecycle/ownership boundaries.
4. Continue remaining independent review and integration correctness gates.
   Do not describe the whole module as accepted before substantive findings
   and required later stages close.

The [current correctness scope](../../CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md)
supersedes historical 1000-trial requirements. Do not start or wait for a
1000-run batch, performance ranking or statistical benchmark reproduction.
This changes no frozen test or threshold and does not remove the later
bounded paper-parameter eight-no-refresh-operation/same-root-h128 gate.

This receipt makes no claim of complete I/O, complete Spec acceptance,
h128 keys, N32768/full packing, eight operations, universal paper-wide
accuracy, performance benefit or cryptographic-security certification.
Only this new acceptance Markdown was written for this receipt; no source,
test, CI, previous evidence, commit or remote state was changed.
