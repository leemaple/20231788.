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

## Actual merge and hosted closure — 2026-09-05

Merge `4ecbd972429884489918d9f82dfc3fe9f702ef4a` was committed with the exact
two parents above and pushed to the isolated candidate branch; remote SHA
matched. The existing workflow was dispatched exactly once at
2026-09-04T15:58:50Z, yielding
[run33892550947](https://github.com/leemaple/20231788./actions/runs/33892550947),
attempt1, exact head `4ecbd972`. It completed successfully at
2026-09-04T16:07:19Z (Sep5 00:07:19 Asia/Shanghai).

- Linux101087474806: precision1/1 (0.24s), Pair2/2 (0.17s), full57/57 (1.32s).
- Windows101087474933: precision1/1 (0.26s), Pair2/2 (0.19s), full57/57 (2.49s).
- The independent [hosted audit](DEFAULT_MAINLINE_HOSTED_AUDIT.md) verified
  all actual names/commands, workflow stages, source IDs and certificates.
  Root independently re-read authoritative run state, verified both raw-log
  hashes/source markers, 120 total CTest occurrences, all16 exact rational
  first-Mult2 error records, 8 distinct-from-first precursor occurrences,
  8 Pair occurrences and 4 fixed-key BV records. Root's bounded BigInt checks
  rederived decimal thresholds, divisor products, triangle/nonwrap/error
  inequalities and BV row-norm bounds; all passed. Counts are execution
  occurrences, not claims of globally unique inputs or keys.
- Linux reused the pristine upstream install cache; its two upstream build
  steps were skipped. Windows built pristine OpenFHE. Both built/tested the
  project on the hosted runner. No build or cryptographic run occurred on Mac.
- Raw logs and the audit index remain unchanged under
  `artifacts/tdd/default-mainline-consolidation/33892550947/`. The two precise
  `.log` paths are deliberately tracked despite the existing generic ignore
  rule; no ignore rule or archived whitespace is changed.

This acceptance follow-up changes only README/ledger/evidence. Default
promotion is the remaining Git operation: recheck the remote old head and
canonical report hashes, fast-forward to this descendant, verify preservation,
then normal push. No force push, report staging, CI rerun or unreviewed source
is authorized by this record. Actual promotion outcome must be recorded after
execution rather than inferred from this plan.

## Actual default promotion — 2026-09-05 00:26 Asia/Shanghai

The canonical default branch `cleanroom/reimplement-mult2-20260831` was
fast-forwarded from `1f765462d8cfb20c36fc10a95913c630eec8fa35` to
`7b41e1cdadd407736b757f662fddceccd2d67c99` using `git merge --ff-only`.
An ordinary explicit-ref push succeeded; `git ls-remote` then returned the
same full SHA for the remote default branch. Observation completed by
2026-09-04T16:26:55Z. No reset, force push, stash, report staging or CI dispatch
was used.

Before promotion, root and an independent Codex reviewer verified that:

- the old default is an ancestor of the candidate, whose direct parent is
  the exact tested merge `4ecbd972429884489918d9f82dfc3fe9f702ef4a`;
- candidate source, headers, tests, CMake, all workflows and `.gitignore`
  are byte-identical to that tested merge;
- all tracked report paths are unchanged between the old and new default;
- the candidate worktree is clean and both original hosted logs are tracked
  at their recorded hashes.

Root enumerated and hashed every existing file under the two untracked daily
report directories plus the modified delivery log before and after the
fast-forward. All six byte counts and SHA-256 values were identical:

| Existing report path | Bytes | Unchanged SHA-256 |
| --- | ---: | --- |
| `reports/daily/2026-09-03/2023-1788-openfhe-daily-2026-09-03.md` | 14143 | `04b464f89e5577735fa5cc54cc98c2aa6bb5f5781575bb2df9597ea3f0698a90` |
| `reports/daily/2026-09-03/2023-1788-openfhe-daily-2026-09-03.pdf` | 303692 | `963af65db78d262ca74821ef4e4887db37abf2bfe2d6d3a2d913603686f141ac` |
| `reports/daily/2026-09-04/2023-1788-openfhe-daily-2026-09-04.md` | 15689 | `1a268a740b34b1e048187117b90a2b85cb62f08fcd10d2db27d389a70923eccd` |
| `reports/daily/2026-09-04/2023-1788-openfhe-daily-2026-09-04.pdf` | 256360 | `3ca7a95189aa15c3c2a17769455fbb7ee2951dab1fb0aa3e50deb9b6432dc647` |
| `reports/daily/2026-09-04/build_report.py` | 5220 | `2ed7fd852dcbc48ca3a9ebfb27326ce2bee01b5837a823bdf5020d9c87f49ef6` |
| `reports/delivery-log.md` | 2099 | `1c5e2c57160fabb68e47f4b666be6eb79708eb8075c54fb826ca196ed144c799` |

The index was empty after the fast-forward. Those existing report changes
remain unstaged/untracked; they are not part of this promotion or its factual
follow-up. The default now carries the hosted-tested first-Mult2 baseline and
retained evidence, not the unimplemented repeated-Mult2, high-precision I/O or
h128 proposals. The full paper goal remains incomplete.
