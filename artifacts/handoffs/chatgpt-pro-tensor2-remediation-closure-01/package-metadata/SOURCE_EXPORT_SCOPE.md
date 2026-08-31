# Source export scope — Tensor2 remediation closure

Prepared: 2026-09-01 Asia/Shanghai

## Included

- `cleanroom-project/` is a direct `git archive` of exact commit
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`, tree
  `759d5195739684748d5a9664edabe3fa719e1acf`. It contains every tracked file
  at that commit, including the raw intermediate P2 hosted evidence.
- `PROJECT_DIFF.patch` and `COMMIT_HISTORY.txt` are generated from accepted
  DCP/RCB base `87c84b...` through exact current head `fb862a3...`.
- `REMEDIATION_DIFF.patch` and `REMEDIATION_COMMIT_HISTORY.txt` are generated
  from the previously reviewed Tensor2 head `55f3b43...` through exact current
  head `fb862a3...`.
- `openfhe-1.5.0/`, `references/`, and `prior-tensor2-delivery/` are copied
  unchanged from the previously hash-bound and twice-scanned exact review
  package. They contain pristine official OpenFHE at `df495ba...`, the
  user-supplied paper, and the preceding clean-room Tensor2 delivery.
- `prior-closure-review/` contains the exact preceding ChatGPT Pro result ZIP,
  all six extracted files, the preceding task, and preceding input binding.
- `ci/` and `tdd/p3-diagnostic-red/` contain the raw final-green and P3-red
  run/jobs API responses and both job logs.
- `reviews/` contains the scale proof, the prior accepted two-axis review, and
  the final independent PASS/PASS review of this remediation.

## Excluded

- All `.git` metadata and unrelated branches/worktrees. The clean package can
  support auditing exported histories/diffs and execution timestamps, but it
  cannot independently prove Git object ancestry or absence of history
  rewriting.
- Every former, known-wrong, local, private, or author proof-of-concept
  implementation.
- `.env` files, API keys, tokens, private keys, cookies, login databases, and
  all other credentials.
- `node_modules`, generated build trees, CMake object directories, caches,
  databases, runtime state, and browser profiles/state.
- Relin2, RS2, full Mult2, pair Add/Sub, repeated multiplication, precision,
  performance, and network-security work.

The package is only for the bounded algorithm/OpenFHE/test/evidence review in
`FINAL_REVIEW_TASK.md`.
