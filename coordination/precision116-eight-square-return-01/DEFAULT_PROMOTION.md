# Default delivery — observed 2026-09-07 04:08 Asia/Shanghai

The remote default branch `cleanroom/reimplement-mult2-20260831` was advanced by a normal fast-forward push from `8c06b480ab553aa30ba1c86b4e091a89432bce11` to `1552ebe04ba1406e6f735bd7eeb19413aca3f740`. `git ls-remote` confirmed the exact new SHA. No reset, force push or repeated numerical observation was used.

Scientific acceptance remains tied to `2b8b349edf5575556347082c1b725f6696c743b6`, run34055816234/attempt1. Source, headers, tests and CMake are byte-identical at the delivered descendant. `c799826` retains the actual two-host logs, status, review and acceptance; `1552ebe` adds the independently reviewed default-only CI exclusions. Both were pushed to the working branch before default delivery.

## Preserved local reporting worktree

The canonical reporting worktree `/Users/lifeng/Documents/20231788-openfhe-cleanroom-20260831` contains pre-existing modified reports, untracked daily-report directories and a modified reporting skill reference. All36 report/skill files (5230288bytes) were hashed before attempting a local fast-forward; see `CANONICAL_PRESERVATION_BEFORE.json`.

The local `git merge --ff-only` safely refused because of the existing reporting-reference edit. Although that file's working bytes have the same Git blob as the incoming version (`a720c5852a52360d08357797fc61b8cdce924933`), root did not stage, stash, reset or overwrite it to bypass Git's guard. After the refusal, all36 file sizes/hashes, canonical HEAD `8c06b48...` and empty index were verified unchanged. Root instead pushed the already reviewed descendant directly to the remote default from the clean engineering worktree.

The canonical local branch therefore intentionally remains behind the remote default. It remains the protected report-output directory, not the current engineering checkout. Continue implementation/audit in `/Users/lifeng/Documents/20231788-openfhe-precision116-eight-square-20260907`; do not overwrite the canonical user's changes merely to synchronize its branch.

## Default CI still pending

The default push created[run34057018442](https://github.com/leemaple/20231788./actions/runs/34057018442) at `2026-09-06T20:07:46Z`, exact source`1552ebe...`, initially `in_progress`. This run should build the normal library/API contracts and run the existing60 unique regressions on each host, without compiling/running either expensive full-eight experiment or old endpoint publisher. It verifies final delivery wiring, not another scientific sample.

Retain actual final run/attempt/job evidence and verify the disabled steps before closing delivery. The full goal remains ACTIVE while this check and the final completion audit are pending.
