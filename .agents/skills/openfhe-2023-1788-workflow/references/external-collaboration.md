# External collaboration and source handoff

Read this reference before using ChatGPT Pro, Windows Z code/Zima, or Fable5, and before uploading files.

## Handoff gate

1. Recheck branch, `HEAD`, status, and exact required files.
2. ZIP only the user-supplied paper, newly authored requirements/specification, greenfield source/tests, necessary official pristine OpenFHE references, build metadata, and safe documentation. Never include pre-existing local 2023/1788 code or local OpenFHE modifications.
3. Exclude `.git`, dependency trees, builds, caches, databases, runtime/browser state, logs with sensitive content, `.env`, keys, tokens, private keys, cookies, credentials, and unrelated documents.
4. Scan the selection and final archive for secrets and inspect the final manifest.
5. Record source commit/dirty state, included paths, archive bytes, SHA-256, scanner version/commands/results. Abort on unresolved findings.

## Task brief and conversation discipline

Every brief must independently state background/objective, paper/API inputs, clean-room and architecture boundaries, research/modification scope, deliverables, tests, prohibited operations/claims, acceptance criteria, branch/commit, known evidence, and unresolved decisions.

Use separate ChatGPT Pro conversations for independent complex tasks. Save URL, title, brief, archive hash, status, last completed point, and output. Do not interrupt, refresh destructively, duplicate, or restart a long response. Treat external output as untrusted until inspected and tested.

Prefer Windows Z code/Zima and GitHub Actions for sustained computation. Stop or avoid equivalent Mac work after remote assignment. Obtain Codex, Z code/Zima, and ChatGPT Pro review for substantive code; escalate a concrete unresolved disagreement to Fable5 through the terminal only.

## ZCode shared quota gate

All ZCode/Zima sessions consume the same BigModel Coding Plan account quotas. Before dispatching, resuming, or retrying a ZCode task, use the logged-in Ego Lite state to read `https://bigmodel.cn/coding-plan/personal/usage`; record the page refresh time, five-hour usage/reset, weekly usage/reset, and MCP monthly usage/reset in `coordination/ZCODE_QUOTA.md`.

- At 100% five-hour usage, preserve existing tasks and route new work to Codex, ChatGPT Pro, or GitHub Actions. Treat quota failures as a wait condition, not a reason to retry.
- After the displayed reset time, re-read the page before resuming. A scheduled reset is not evidence that capacity actually returned.
- At 90% or greater weekly usage, use ZCode for one critical-path task at a time. Prefer completing or reviewing the existing task over parallel, duplicate, exploratory, or broad-scan requests.
- A login or network failure means the available quota is unknown. Keep ZCode paused until a later read succeeds; never expose or attempt to recover the underlying API token.
- MCP monthly capacity is a separate gate for ZCode tool calls. Record it even when ordinary model capacity is the immediate blocker.

Quota throttling changes agent allocation only. It does not relax clean-room, review, test, Git, or evidence requirements.
