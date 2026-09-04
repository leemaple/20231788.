# Default-branch consolidation — 2026-09-04

## Scope and exact inputs

This is a non-fast-forward consolidation of two clean-room histories, not new algorithm work or acceptance of the pending repeated-operation/I/O proposals.

- Default parent: `1f765462d8cfb20c36fc10a95913c630eec8fa35` (`cleanroom/reimplement-mult2-20260831`).
- Reviewed integration parent: `4938b4458e3de17cd4bf48230878c1dea3aa1dfd` (`codex/integration-01`).
- Common ancestor: `ff0f56b68c526da87717b798c9f519a0c540a02a`; `git rev-list --left-right --count` returned `149 231`. Neither reset nor force push is appropriate.
- Isolated consolidation branch: `codex/default-mainline-integration-20260904`; worktree `/Users/lifeng/Documents/20231788-openfhe-default-mainline-integration-20260904`.

Both remote input refs were checked against these exact SHAs before starting. Independent read-only review confirmed that the default-only engineering change is the manually dispatched historical `.github/workflows/fb862a3-review-evidence.yml`; it does not change current source, headers, tests or CMake. It is preserved but is not dispatched by this consolidation. The live `dcp-rcb.yml` workflow comes unchanged from the accepted integration.

## Conflict and preservation decisions

`git merge-tree --write-tree --name-only` reported only a README content conflict. An ordinary `git merge --no-ff --no-commit` in the new worktree reproduced that conflict. The README's two stale progress summaries are replaced with the exact accepted first-Mult2 evidence and explicit pending full-paper work. Original history and both sides' linked coordination evidence remain available.

All six default-parent tracked report paths are to remain byte-identical. The canonical worktree is not used for the merge; its existing modified `reports/delivery-log.md` and untracked `reports/daily/2026-09-03/` and `reports/daily/2026-09-04/` are neither copied into this commit nor edited, staged, stashed or discarded. No quarantined implementation or modified OpenFHE source is an input.

## Existing acceptance and fresh verification gate

The reviewed source/test/build configuration is that tested at `7c982519dfedacf5505dbd0f1ca6579ee91da2fd` in [GitHub Actions run 33882911345](https://github.com/leemaple/20231788./actions/runs/33882911345): Linux 57/57 and Windows 57/57, with focused first-Mult2 precision and pair-composition evidence. The `7c982519..4938b445` delta consists only of six evidence/acceptance files. See [Integration 05](INTEGRATION_PAIR_COMPOSITION_05.md) and [hosted audit](INTEGRATION_PAIR_COMPOSITION_HOSTED_AUDIT.md).

Before promotion: compare the merged index's source, headers, tests, CMake and active workflow byte-for-byte with `4938b445`; compare all tracked reports with `1f76546`; review the README resolution; commit/push the isolated candidate; run the existing workflow on that exact candidate SHA on GitHub Actions and retain its actual results. No new tests or test seams are introduced. No Mac compilation or cryptographic runtime is performed.

At creation of this record the consolidation commit, its fresh CI run and default-branch promotion are **pending**, not completed. Subsequent evidence must append exact SHA/run/verification results. The existing full-paper blockers and deferred guard decisions are not closed by branch consolidation.

## Pre-commit checks observed

- Root and independent Codex reviewer: the 699-path union of both parents is preserved; the candidate index contains 700 paths. Only README and this new ledger do not match a parent blob; the other 698 paths match at least one parent exactly. There are no unresolved index entries.
- `git diff --cached --exit-code 4938b445 -- src include tests CMakeLists.txt .gitignore .github/workflows/dcp-rcb.yml`: exit 0. `git diff --cached --exit-code 1f76546 -- reports .github/workflows/fb862a3-review-evidence.yml`: exit 0.
- README and this ledger passed scoped whitespace checks; all 16 local Markdown links checked by the independent reviewer exist. The whole imported history emits whitespace warnings in retained raw logs and patches; those archival bytes are intentionally not reformatted, and no clean whole-history whitespace result is claimed.
- The candidate branch is absent from the existing push trigger allowlist. Therefore push alone will not start its regression: use the existing `workflow_dispatch` and verify the returned run's actual source SHA. This does not dispatch the historical `fb862a3` workflow.
- The canonical worktree still has only its pre-existing report changes. No source/test implementation or unconfirmed test seam was added in this consolidation.
