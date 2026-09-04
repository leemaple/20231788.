# Default-mainline consolidation: independent hosted audit

## Exact observed run

Candidate source: `4ecbd972429884489918d9f82dfc3fe9f702ef4a`.
[Run 33892550947](https://github.com/leemaple/20231788./actions/runs/33892550947),
attempt 1, workflow_dispatch, branch codex/default-mainline-integration-20260904:
completed/success, updated 2026-09-04T16:07:19Z. Both job metadata and actual
in-band PROJECT_SOURCE_COMMIT / GITHUB_RUN_ID / GITHUB_RUN_ATTEMPT match.
OpenFHE pin remains df495ba2e91739a6dc8f1de254fc5a41155ce504.

| Host / job | Focused precision | Focused Pair | Full suite | Finished UTC |
| --- | --- | --- | --- | --- |
| Linux / 101087474806 | 1/1, 0.24 s | 2/2, 0.17 s | 57/57, 1.32 s | 16:01:37 |
| Windows MinGW64 / 101087474933 | 1/1, 0.26 s | 2/2, 0.19 s | 57/57, 2.49 s | 16:07:18 |

Checked every actual test name, index/order and executable/selector COMMAND
against frozen CMake: 60 invocations per host (1 + 2 + 57).
All five explicit Relin2/RS2/Mult2/Add/Sub API commands and successful compiler
completion outputs are present. All declared workflow steps occur in order.
Linux has 24 actual steps: only its two cache-conditional upstream build steps
are skipped; all others succeed. Windows has 19 actual steps, all successful,
including a pristine upstream build. Both project warning-clean builds passed;
no project compiler warning/error diagnostic was found.

The source/header/tests/CMake/.gitignore/active dcp-rcb workflow diff from
4938b4458e3de17cd4bf48230878c1dea3aa1dfd to this candidate is empty (exit 0).
Previous run33882911345 evidence was used for frozen configuration/provenance,
not substituted for this new run.

## Numerical checks and counting boundary

| Record category | Per host | Both hosts | Execution explanation |
| --- | ---: | ---: | --- |
| First-Mult2 precision | 8 | 16 | four trials, focused plus full |
| DCP/RCB precursor | 4 | 8 | four trials, full only |
| Pair composition | 4 | 8 | Add/Sub selectors, focused plus full |
| Fixed-key BV | 2 | 4 | real/complex selectors, full only |
| Other ordinary HYBRID | 2 | 4 | real/complex selectors, full only |

These are certificate occurrences, not unique plaintext vectors or proven
globally distinct keys. Trial labels and fixed inputs recur. Source
tests/precision_first_mult2_contract_test.cpp:1015-1019 invokes KeyGen per trial;
this does not prove key uniqueness from the logs. The 16/8/8/4 categories match
the previous two-host audit; precursor records are counted separately, never
added to first-Mult2 samples.

All 16 first-Mult2 slot/product-delta decimals were parsed directly as exact
BigInt rationals and compared with 2^-80. Exact q_div*q_l scale denominators
and frozen N64 diagnostic configuration/headroom fields match. Worst slot:
1.6828004945448615964660069790707135652506e-27; worst delta:
1.8430716944924484173432219625222389687872e-27 (Linux full trial 1).

All 16 execution certificates satisfy independently recomputed triangle,
nonwrap, coefficient-denominator and bound inequalities. The eight Pair
occurrences retain N64/p30/HYBRID, fixed_key_bv_bound_available=false,
PER_PATH_CONDITIONAL and UNPROVED; both emitted decoded-error measures are
below exact 1/1000. They are not the p50 precision experiment.

All four BV records pass recomputation from the already accepted ordered
primes and newly emitted row norms: Bpath, Bpair=2*Bpath, high/low/pair error
domination, conservative nonwrap/bound, and final integer-lift inequality.
Bpath real/complex: Linux 2680059273216 / 3745211088896; Windows
2680059445248 / 3882650050560. All four retain ZERO raised-high digit,
PASSED centered-digit probe, conditional status and universal UNPROVED;
combined-additivity observations remain false, not suppressed.

## Retained raw evidence and procedure

Files below are under artifacts/tdd/default-mainline-consolidation/33892550947/.
The two logs are complete job-log HTTP response bodies saved directly by the
download client: no cropping, ANSI/CR removal or whitespace normalization.
Only in-memory parsing normalizes presentation characters.

| File | Bytes | SHA-256 |
| --- | ---: | --- |
| linux-101087474806.log | 122231 | b85960e160746c9ca9afbaa3f04ea784e6233e89f67b66c0678c32c3f17ca054 |
| windows-101087474933.log | 205145 | f44e1a03962b185ec1a06e4e121497b0516f203119403ab528cd96c25bcd0eda |
| audit-index.json | 35903 | 08353697b90838a9c0f88d02a6739049e106d45a894ca8e6be445683f72b7685 |

The JSON contains projected run/job metadata, every step outcome, raw-log line
indices, exact-gate results and classifications. Linux's Node20 action advisory
and Windows's pre-project MSYS2 installation warning are preserved; neither is
misreported as a project compiler warning.

Both raw .log files exist at the exact paths above but match .gitignore:11
(*.log), so ordinary rg --files/status omit them. Archival staging must include
these two exact files explicitly (for example, scoped git add -f by the owner);
this auditor did not stage them or change the ignore rule.

Observation began 16:01:24Z with a 16:13:24Z deadline; terminal success was
observed by 16:08:20Z. Commands: gh run watch 33892550947 --repo
leemaple/20231788. --interval 50 --exit-status; GET run/jobs metadata and exact
job /logs endpoints; git show/diff; bounded Node BigInt/read-only log parsing.
The watch exited 0. Initial Linux-oriented parser assertions rejected Windows
CTest summary wording and Ninja link markers. Direct raw-line inspection
corrected those parser assumptions; no CI failure, log edit or test change
occurred. No compiler or cryptographic runtime ran on the Mac.

This independently verifies the isolated candidate's new hosted regression.
It does not establish default-branch promotion, repeated multiplication,
production lossless I/O, h128/paper-scale statistics, security, or full completion.
No dispatch, retry, cancel, source/test edit, stage, commit or push was performed.
