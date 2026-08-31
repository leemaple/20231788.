# Git checkpoint and recovery policy

Recorded: 2026-08-31 (Asia/Shanghai)

This policy keeps every useful project state recoverable and visible on GitHub without mixing unreviewed agent output into the clean-room integration branch.

## Required workflow

1. Check the current branch, upstream, worktree status, and repository baseline before changing files.
2. Work in a dedicated branch/worktree when a change is produced by an external agent or is otherwise independent.
3. Make a small, coherent commit as soon as a checkpoint is reviewable. Do not leave completed work only in a working tree.
4. Run the proportionate test or validation for that checkpoint and record the exact command and outcome in the commit message, task evidence, or review ledger.
5. Push the commit immediately to its matching remote branch and verify that the remote object ID equals the local object ID.
6. Record the branch, commit, author/source, validation status, and integration decision in the project ledger.
7. Merge or cherry-pick into `cleanroom/reimplement-mult2-20260831` only after provenance, test, and review gates pass; then push and verify the integration branch immediately.

## Recovery and safety rules

- Never force-push or rewrite a shared branch.
- Never delete a recovery branch or tag until its accepted replacement is pushed and verified.
- Never mix ChatGPT Pro, Windows Z code/Zima, or experimental changes directly into the integration worktree.
- Preserve user changes and unrelated work; stop if branch ownership or provenance is ambiguous.
- Keep credentials, `.env` files, cookies, browser state, caches, databases, build output, and runtime state out of Git and handoff archives.
- Before uploading a source archive, run a secret scan and record its source commit, byte size, SHA-256, archive test, and exclusions.
- A push is complete only when `git ls-remote` (or an equivalent GitHub check) confirms the expected remote commit.

## Branch roles

- `cleanroom/reimplement-mult2-20260831`: reviewed integration and coordination checkpoints; current default branch.
- `agent/chatgpt-pro-01`: isolated ChatGPT Pro output and review fixes.
- `agent/windows-zcode-mult2`: isolated Windows Z code/Zima output and review fixes.
- Additional independent experiments receive separate `agent/*` or `experiment/*` branches.

Implementation checkpoints must include retained red/green TDD evidence before they can be described as tested or accepted.
