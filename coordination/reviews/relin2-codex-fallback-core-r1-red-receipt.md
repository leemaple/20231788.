# Relin2 connected-core R1 TDD red receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the connected Relin2 core contract is red exactly as intended on
Linux and Windows; no Relin2 arithmetic green claim is made**.

## Frozen source boundary

- branch: `agent/codex-relin2-01`;
- commit: `f90a04d199e96a3247a2607aa3e1f80ad55be8cc`;
- parent: `1e59e8b36d5119ceb2b463922f1053e03a029bd4`;
- tree: `7edbfad070201f68a60d1b53f6c72bbb99939eb3`;
- subject: `test: define Relin2 connected core red contract`.

The commit changes only `CMakeLists.txt`, `tests/dcp_rcb_test.cpp`,
`tests/relin2_test.cpp`, and `tests/tensor2_test.cpp`: 1,554 insertions and one
deletion. It changes no production source or public header. Local HEAD, the
upstream-tracking ref, and the live GitHub branch ref were verified equal after
the non-force push; the implementation worktree was clean.

## Test-first contract

Ten unique selectors extend the suite from 26 to 36 tests. The first eight
exercise production `Relin2` and require, in order, immediate deep
Tensor/cache/evaluation-key invariance, rejection of the terminal scaffold,
an independent exact-integer `(u, v+w)` oracle, complete `ReadyForRS2`
lifecycle/scale/metadata state, and public RCB exactness. The last two require
exact fail-fast validation of malformed first-recombined RCB and Tensor2
fields.

The fixtures include controlled nonzero witnesses, representative public
inputs, HYBRID, BV with zero digit size, BV with nonzero digit size, and real
public `[s^2 -> s, s^3 -> s]` evaluation keys. One later-key case retains the
valid second key; the other changes only index one to null and requires it to
be ignored. These semantics and the exact `26 -> 36 -> 37` sequencing were
previously resolved by two clean-room terminal reviews whose emitted model was
exactly `claude-fable-5-1`, with fallback disabled.

Before commit, three independent read-only Spec/API, TDD, and
Delivery/compile reviews returned `PASS` against the frozen source bytes.

## Hosted red observation

The formal push-triggered workflow was:

- run: `33638053832`, attempt 1;
- URL: `https://github.com/leemaple/20231788./actions/runs/33638053832`;
- Linux job: `100273799877`;
- Windows/MSYS2 MinGW64 job: `100273799654`;
- pristine OpenFHE commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

Both jobs passed the warning-clean default project build and compile-only
Relin2 public-API build. Both reported exactly `26/36`: all inherited tests
passed and only the ten new tests failed. Tests 27 through 34 observed the
exact existing `DoubleCKKS: Relin2 is not implemented` scaffold and then
rejected it; tests 35 and 36 reported that their required new fail-fast
predicates are absent. Linux took 0.32 seconds and Windows 0.67 seconds.

Supplementary workflow-dispatch run `33637147696` executed the same exact
commit before the formal fast-forward and independently produced the same
build-success/`26/36` matrix. It is corroboration only; the push run controls.

## Evidence integrity

- evidence branch: `evidence/relin2-hosted-f90a04d`;
- evidence commit: `947a04ec0d50ac1959c3f973aa3731ce509d55fa`;
- evidence tree: `4ad6808e764370e14d68c65ad3df477f8f0a7344`;
- evidence directory: `artifacts/hosted/relin2/f90a04d/`;
- manifest SHA-256:
  `80258f95214fe08dfd79fa563b1a1ba6fc4a7342324435e704bb2635c81d9bcc`.

The directory contains 24 non-manifest files plus `MANIFEST.sha256`. The
formal logs ZIP has 30 unique, unencrypted, path-safe members and passes
`unzip -t`; aggregate Linux and Windows logs are byte-identical to the
separately retained logs. GitHub reported zero uploaded artifacts. Raw,
expanded, and final evidence passed Gitleaks 8.30.1 with redaction and an
independent targeted credential filename/content scan. Three independent
post-run Spec, TDD, and Delivery/evidence reviews returned `PASS` against the
frozen evidence bytes.

## Decision and next step

This R1 red boundary is accepted and closed. The next source change is a
production-only green implementation that must make these exact 36 tests pass
without weakening, renaming, reordering, or changing their expected results.
R2 lifecycle, RS2, pair Add/Sub, Mult2, precision, performance,
serialization, and security estimates remain outside this receipt. A genuine
paper/OpenFHE/scale/lifecycle ambiguity is escalated promptly to exact terminal
Fable 5.1; ordinary implementation and hosted CI do not wait for external
reviewers.
