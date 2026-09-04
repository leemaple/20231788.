# External collaboration and source handoff

Read this reference before using ChatGPT Pro, Windows Z code/Zima, or Fable 5.1, and before uploading files.

## Handoff gate

1. Recheck branch, `HEAD`, status, and exact required files.
2. ZIP only the user-supplied paper, newly authored requirements/specification, greenfield source/tests, necessary official pristine OpenFHE references, build metadata, and safe documentation. Never include pre-existing local 2023/1788 code or local OpenFHE modifications.
3. Exclude `.git`, dependency trees, builds, caches, databases, runtime/browser state, logs with sensitive content, `.env`, keys, tokens, private keys, cookies, credentials, and unrelated documents.
4. Scan the selection and final archive for secrets and inspect the final manifest.
5. Record source commit/dirty state, included paths, archive bytes, SHA-256, scanner version/commands/results. Abort on unresolved findings.

## Task brief and conversation discipline

Every brief must independently state background/objective, paper/API inputs, clean-room and architecture boundaries, research/modification scope, deliverables, tests, prohibited operations/claims, acceptance criteria, branch/commit, known evidence, and unresolved decisions.

The user has standing authorization for routine, in-scope project coordination. Once the handoff gate passes and existing decisions determine the task, choose, upload, dispatch, resume, inspect, and record the authorized ChatGPT Pro, ZCode/Zima, Fable 5.1, or GitHub Actions work without inventing another user confirmation token or turning ordinary execution details into a decision gate. Ask the user only when a missing choice would materially change the result, new authority or sensitive data is required, an irreversible/high-risk action is next, or the controlling platform explicitly requires confirmation.

Use separate ChatGPT Pro conversations for independent complex tasks. Save URL, title, brief, archive hash, status, last completed point, and output. Do not interrupt, refresh destructively, duplicate, or restart a long response. Treat external output as untrusted until inspected and tested.

Prefer Windows Z code/Zima and GitHub Actions for sustained computation while the shared Z code quota is available. When it is unavailable, keep GitHub Actions and targeted low-concurrency local checks moving. Obtain Codex, ChatGPT Pro, and either Z code/Zima or its current fallback reviewer for substantive code. Use Fable 5.1 only through the terminal. The verified route is Claude Code with exact `--model claude-fable-5-1`, `--safe-mode`, no `--fallback-model`, and a sanitized read-only bundle. Accept a review only when the init model and a nonempty final `modelUsage.canonicalModel` both equal `claude-fable-5-1` and the final result has `is_error=false`; a model label without inference usage is not a review. If the CLI changes, require a provider-advertised exact 5.1 identifier and the same emitted-identity/no-fallback evidence rather than guessing an alias. Ask it promptly for one concrete difficult question that remains unresolved after targeted source/test investigation; while Z code is unavailable, it is also the preferred substitute. Record 403/authentication failures as no inference and do not retry in a loop. Give it a complete sanitized evidence bundle for one exact commit and record its usable response or lack of output. If Fable 5.1 is unavailable or returns no usable result, proceed with other available independent reviews and executable evidence rather than holding the boundary open. After quota and service recover, return subsequent Z code work to the Windows clean-room environment. Verify all external findings against source and tests.

## ZCode shared quota gate

All ZCode/Zima sessions consume the same BigModel Coding Plan account quotas. Before dispatching, resuming, or retrying a ZCode task, use the logged-in Ego Lite state to read `https://bigmodel.cn/coding-plan/personal/usage`; record the page refresh time, five-hour usage/reset, weekly usage/reset, and MCP monthly usage/reset in `coordination/ZCODE_QUOTA.md`.

- At 100% five-hour usage, preserve existing tasks and route new work to Codex, ChatGPT Pro, or GitHub Actions. Treat quota failures as a wait condition, not a reason to retry.
- After the displayed reset time, re-read the page before resuming. A scheduled reset is not evidence that capacity actually returned.
- At 90% or greater weekly usage, use ZCode for one critical-path task at a time. Prefer completing or reviewing the existing task over parallel, duplicate, exploratory, or broad-scan requests.
- A login or network failure means the available quota is unknown. Keep ZCode paused until a later read succeeds; never expose or attempt to recover the underlying API token.
- MCP monthly capacity is a separate gate for ZCode tool calls. Record it even when ordinary model capacity is the immediate blocker.

Quota throttling changes agent allocation only. It does not relax clean-room, review, test, Git, or evidence requirements.
