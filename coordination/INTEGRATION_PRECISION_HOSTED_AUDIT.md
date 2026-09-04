# Integration precision — independent hosted audit

## Exact new observation

Source `23a5f3a5455134ecb617f7332937b6ef10c93e1b`, branch
`codex/integration-01`; [run 33880254416](https://github.com/leemaple/20231788./actions/runs/33880254416),
attempt **1**, completed/success. Both job APIs and each retained in-band
`PROJECT_SOURCE_COMMIT` / `GITHUB_RUN_ID` / `GITHUB_RUN_ATTEMPT` line match.

| Host | Actual job | Focused first-Mult2 | Full suite |
|---|---|---|---|
| Linux GCC | 101046903332 | 1/1, 0.34 s | 55/55, 1.25 s |
| Windows MinGW64 | 101046903472 | 1/1, 0.23 s | 55/55, 2.49 s |

All actual 55 names, indices and executable/selector COMMANDs match frozen
CMake in order; the focused command independently matches the first-Mult2
registration. Default warning-clean builds and Relin2/RS2/Mult2/Add/Sub API
steps succeed, with their actual commands and completion markers retained.
CMake enables `-Wall -Wextra -Wpedantic -Werror` on all 18 targets. This is
GCC/MinGW64 evidence, not the unexecuted MSVC warning branch.

## Exact precision and BV checks

Each host emits **four focused plus four full first-Mult2 records**, trials
0–3 per phase. All **16** pass independent rational comparisons against
`2^-80 = 1/1208925819614629174706176`. Scientific-decimal strings are parsed
directly into BigInt numerator/denominator; no binary64 comparison is used.
Four precursor records per host are separately classified and excluded from
the first-Mult2 count.

Across all 16 records, exact ordering gives:

- Worst slot error: `1.4088305861352399284724768998723593326671e-27`
  (Linux, focused, trial 2).
- Worst product-delta error: `8.3694990687530140705216742335760923715162e-28`
  (Linux, full, trial 2).

Printed `q_div*q_l` denominators, frozen context/scale/gate identities and
headroom fields also match. These checks verify emitted observations, not a
new local canonical evaluation or cryptographic execution.

All **four new BV fixed-key certificates** pass independent B_path/B_pair,
high/low/pair limits, conservative coefficient, pre-RS non-wrap and final
integer-lift recomputation. All eight ordinary execution certificates also
pass their integer inequalities. Ordered primes use the accepted corrected
source provenance in `BV_FIXED_KEY_FINAL_REVIEW_38C28C2.md`; this audit checks
their product/divisors and performs no prime search or key-residual recovery.
The four BV combined-call additivity observations remain false; universal
and all-key claims remain UNPROVED. Complete measured fields are in the JSONs.

## Retained payloads

| Repository-relative path | Bytes | SHA256 |
|---|---:|---|
| `artifacts/tdd/integration-precision/linux.txt` | 67703 | `756326a0f2847f977f79d6cd678e0495cdae7f573b6c3a6d73e43474ffef87ca` |
| `artifacts/tdd/integration-precision/windows.txt` | 71581 | `e78db8023440e3c89c8d7136e8cad2c73815e43ef6f8fdffde5f1b6997a647f9` |
| `coordination/evidence/integration-precision/linux-23a5f3a.json` | 52486 | `99d567db42fdc69f8395fea566afaff6598be3ec459773e7ab41c88c663af7c2` |
| `coordination/evidence/integration-precision/windows-23a5f3a.json` | 50771 | `5e67a242f4bc2de98aa949b5ef55e5d93624d784e627c2f4c54b776c521d1bfc` |

Read-only commands: `gh api --method GET repos/leemaple/20231788./actions/runs/33880254416`
and `/jobs`; `gh api --method GET repos/leemaple/20231788./actions/jobs/ID`
and `/logs` for the exact IDs above; `gh run watch 33880254416 --repo leemaple/20231788. --interval 50 --exit-status`.
Frozen source checks use `git show`; bounded Node/BigInt parsing,
`wc -c` and `shasum -a 256` verify retained evidence.

Observation began `13:51:52Z`; both completed by the `13:58:37Z` watch result,
before the `14:06:52Z` 15-minute limit. Linux was archived while Windows ran.
No timeout, hosted failure or compiler diagnostic occurred. One local audit
script-generation quoting error was corrected without any CI mutation.

Logs start at the project-configure group and stop before Linux Post job
cleanup or Windows orphan cleanup. ANSI/CR/trailing whitespace are normalized;
internal lines are retained, including Linux's nonfatal Node-20 advisory.
Account/runner identity fields are omitted from metadata.

## Limits

These are new post-merge results, not substituted historical 55/55 evidence.
Only five requested evidence files were added/updated through apply_patch;
no source/configuration, staging, commit, push, rerun or Mac OpenFHE execution.
N64/p50/2^100 test-only precision and p30 BV diagnostics do not establish
shipping client I/O, repeated multiplication, paper parameters, statistics,
performance, security or full-project completion. The deferred stale-cache
guard remains deferred as recorded in `INTEGRATION_PRECISION_04.md`.
