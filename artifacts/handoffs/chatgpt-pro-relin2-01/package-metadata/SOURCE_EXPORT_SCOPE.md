# Relin2 source-export scope

Prepared: 2026-09-01 Asia/Shanghai

## Included

- `cleanroom-project/`: byte-for-byte `git archive` of exact project tree
  `759d5195739684748d5a9664edabe3fa719e1acf` at source/test head
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.
- `IMPLEMENTATION_TASK.md`: the complete independently audited Relin2 task.
- `references/`: the user-supplied paper PDF and extracted text.
- `openfhe-1.5.0/`: the supplied pristine official OpenFHE 1.5.0 source export
  bound to commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- `reviews/`: the accepted Relin2 paper/OpenFHE preflight, final task audit, and
  exact-current Tensor2 two-axis closure-fix review.
- `accepted-tensor2/`: the verified ChatGPT Pro `MERGEABLE` closure ZIP, all four
  extracted returned documents, and Codex's independent collection record.
- `ci/33436252725/`: raw exact-current run/jobs JSON and Linux/Windows job logs.
- `source-provenance/`: full accepted-DCP/RCB-base-to-current binary diff and
  exported commit history.
- root identity, CI, scope, handoff, and per-file SHA-256 records.

## Excluded

The archive deliberately excludes:

- all `.git` object databases, worktree administration, hooks, and credentials;
- `node_modules`, compiler/build/install output, CMake caches, object files,
  package-manager caches, and generated runtime state;
- browser profiles, cookies, login databases, sessions, downloads history, and
  remote-control state;
- `.env`, API keys, tokens, private keys, credential stores, and unrelated user
  data;
- every former, local, private, author, quarantined, or known-wrong 2023/1788
  implementation.

## Proof boundary

The exact current source bytes and exported diff/history can be verified. Since
`.git` is excluded, the package does not independently prove Git object
ancestry, absence of history rewriting, or the remote server's retained object
graph. The exact remote branch SHA was checked before packaging and is recorded
as a coordination fact, not converted into a claim that the archive contains
Git metadata.

The Tensor2 result is accepted for the Relin2 base by exact Linux/Windows CI,
Codex review, parallel Standards/Spec review, and ChatGPT Pro closure. Its
separate Windows ZCode/Zima same-commit review remains pending because the
shared ZCode quota is nearly exhausted; this does not become a false completed
claim in the package.

