# First hosted production run

Production source `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89` was pushed to `codex/paper-scale-implementation-20260905`. This is the exact six-file Pro candidate plus its verified original ZIP, intake evidence and separate initial source reviews. Its parent is `901a9256dd3b495aefd93e26e49d3a5aa161fbb5`; the frozen test and hosted wiring remain from `448e9d3067796b64656f0746f4be5c4153b1271d`.

The existing workflow automatically created [run33971779479](https://github.com/leemaple/20231788./actions/runs/33971779479) at 2026-09-05T14:25:40Z, event `push`, attempt1, with the exact production SHA. The first list immediately after push had not yet exposed this run; a subsequent read-only list/view resolved the unique matching run. No dispatch, rerun, cancellation or default-branch merge was used.

Initial authoritative observation from `gh run view 33971779479 --repo leemaple/20231788. --json databaseId,headSha,event,attempt,createdAt,updatedAt,status,conclusion,url,jobs`:

- Linux job101321455160: in progress, exact source/upstream checkout and provenance steps completed, upstream configure completed, Build and install OpenFHE active. A cache-step success is not a cache hit: this run was actually building pristine OpenFHE.
- Windows job101321455226: in progress, exact no-dot-path checkout completed, official toolchain installation active.
- Project builds, all60 regression results and the paper chain were still pending at this observation. No new-source compile or precision PASS is claimed here.

Next: follow these same run/job IDs to terminal evidence; preserve the first actual failed output if any; reconcile source SHA, five API builds, the original60 inventory/execution bindings and the focused full-paper61st test. Do not select new inputs/keys, weaken frozen limits or add 1,000 trials. Final independently briefed Pro semantic review remains due once actual source/execution is stable.

The candidate's exact11-file staged secret scan is retained in `STAGED_SCAN.json`: 279,189 framed bytes, SHA256 `2619b7f8188fa6e1d199b3750e3e41526720c1ff0d061e4b04c194e7d2cd1e46`, strict gitleaks zero findings and staged diff check without exclusions. This later evidence checkpoint is documentation only, not another engineering change or test invocation.
