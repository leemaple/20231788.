# Integration BV fixed-key hosted audit

## Exact new run

Observed source `33ad0aa8959dba3cef5c321f1cee9d5f4d84de2a`, branch
`codex/integration-01`; [run 33877197208](https://github.com/leemaple/20231788./actions/runs/33877197208),
attempt **1**. Both job APIs independently match that run/head/attempt and
report completed/success:

| Host | Job | Compiler | Actual full CTest | Total |
|---|---|---|---|---|
| Linux | 101036891213 | GNU 13.3.0 | 53/53 | 0.77 s |
| Windows MinGW64 | 101036891058 | GNU 16.2.0 | 53/53 | 1.77 s |

Completion timestamps: Linux `2026-09-04T13:19:13Z`, Windows
`2026-09-04T13:24:44Z`. This is NEW integration evidence; it neither replaces
nor relabels source `5b5a415` / run `33866620400` or earlier 53-test records.

## Independent checks

All 53 actual test names, indices, order and executable/selector bindings match
the frozen CMake file. Frozen CMake and oracle bytes match the working tree.
Default warning-clean builds succeeded; CMake applies
`-Wall -Wextra -Wpedantic -Werror` to all 16 project targets (library plus
15 executables). This Windows run is MinGW64/GCC, not the MSVC `/W4 /WX` branch.
Relin2, RS2, Mult2, Add and Sub API commands, completion markers and successful
job steps are present on both hosts.

All eight execution certificates pass independent BigInt recomputation of
the high/low triangle, execution non-wrap and coefficient inequalities.
All **four BV fixed-key certificates pass**: B_path from logged row norms and
accepted ordered primes; B_pair=2*B_path; high/low/pair error limits;
`2*(N*A^2+B_pair)<Q`; conservative numerator/denominator and
`2*coefficient_error_numerator<=bound_numerator`; final integer-lift formula
and strict comparison. Here `A=M_high*q_div+M_low`.

| Host/case | B_path | Coefficient error / fixed bound, % (truncated) |
|---|---:|---:|
| Linux REAL | 4947801911296 | 3.570272 |
| Linux COMPLEX | 2748778729472 | 3.153569 |
| Windows REAL | 2783138656256 | 4.154450 |
| Windows COMPLEX | 3848290398208 | 2.677783 |

The ordered-prime provenance and corrected source route are recorded in
`coordination/BV_FIXED_KEY_FINAL_REVIEW_38C28C2.md:88`; the complete array and
all measured fields/checks are retained in the new JSONs. This audit checked
the table product/divisors against this run; it did not search primes or
independently recover secret-key residuals. All four BV combined-call
additivity observations remain **false**, not an accepting gate. Centered-digit
probe labels remain PASSED and universal/all-key labels remain UNPROVED.

## Retained bytes and commands

Paths are relative to this repository. SHA256 values cover actual disk bytes.

| File | Bytes | SHA256 |
|---|---:|---|
| `artifacts/tdd/integration-bv-fixed-key/linux.txt` | 53487 | `7bbc5bc0b0ad2d1201f3f38ff5d4e92ecdd62f663f20f94b8130bb79fc4d361f` |
| `artifacts/tdd/integration-bv-fixed-key/windows.txt` | 56723 | `54ff3c58bcfea38c1ea958dbd9aff726198977ceb891fffc7179337913474f72` |
| `coordination/evidence/integration-bv-fixed-key/linux-33ad0aa.json` | 31607 | `9bcb340723f0a5c587e2a44e062af43cc76fefd58f2aeee752055c49167345a8` |
| `coordination/evidence/integration-bv-fixed-key/windows-33ad0aa.json` | 29871 | `8a51e87b949b4d54062f0cf49c53966acb54f0c8f9007a1569d808799382d714` |

Read-only retrieval: `gh api --method GET repos/leemaple/20231788./actions/jobs/ID`
and the same path suffixed `/logs`, for each exact job ID above.
Source inspection used `git show 33ad0aa8959dba3cef5c321f1cee9d5f4d84de2a:CMakeLists.txt`
and the corresponding oracle path; bounded Node/BigInt parsing checked the
identities. `wc -c` and `shasum -a 256` verified retained payloads.

Logs start at the project configure command's group. Linux ends before the
first Post job cleanup. Windows has no such step, so ends before Cleaning up
orphan processes. ANSI, CR and trailing horizontal whitespace were normalized;
no internal lines were removed. The nonfatal Linux Node-20 deprecation advisory
is retained; no compiler warning/error or failed test was found. Metadata
omits account and runner identity fields. Initial audit-parser assumptions
about 15 warning targets and a Windows post-job marker were corrected before
archival; they were not hosted failures.

## Boundary

Only logs, metadata and this note were added through apply_patch; no source,
configuration, staging, commit, push, CI mutation or local OpenFHE/CTest run.
Fixed-key/conditional N64, p30, `1e-3` functional diagnostics do not establish
all-key/tail bounds, high precision, paper-scale parameters, repeated use,
security, performance or full-project completion.
