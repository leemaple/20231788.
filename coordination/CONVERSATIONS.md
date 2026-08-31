# External agent ledger

| Agent | Task | URL or task ID | Brief | Package SHA-256 | Status | Latest point | Output |
|---|---|---|---|---|---|---|---|
| ChatGPT Pro | Bounded DCP/RCB clean-room slice | https://chatgpt.com/c/6a952b95-0ed0-83ec-a38b-e415758ef2a5 | `coordination/tasks/chatgpt-pro-dcp-rcb-01.md` | `82b40b8084f35d2d0785f2ca05aa8e77842fd00a1c575d23cd7c49ddb8408243` | delivered; candidate unverified | Submitted at 2026-08-31 17:43 CST and completed after 21m50s without interruption. At about 18:26 CST the candidate ZIP was downloaded, integrity-tested, and secret-scanned. ChatGPT Pro explicitly reports that its green build did not reach project compilation because the supplied source archive omitted the cereal submodule; it makes no compile, CTest, warning-clean, Windows, precision, performance, or security pass claim. | Output evidence: `coordination/handoffs/chatgpt-pro-dcp-rcb-01-output.md`; candidate ZIP SHA-256 `c0d868b3f615b5288c8ed2790a034bdcfc66cfa8156fe9873a0b74e7ed04a108` |
| Windows Z code/Zima | Independent clean-room implementation | Windows ZCode `Untitled session`, started 2026-08-31 14:45 CST | `coordination/tasks/windows-zcode-01.md` | clean-room task brief + paper + skill attached | existing task preserved; post-reset continuation pending | At 2026-08-31 18:31 CST the existing UU remote-control window became visible again. The same ZCode task showed 1h31m of work, `DESIGN.md`, 17 working-tree changes, and its latest diagnosis: global `Format`, a BigInteger/NativeInteger comparison, and C++20 designated initializers in a C++17 build. Its stale quota notice reached the displayed reset time, but the remote composer did not yet accept a follow-up during the first post-reset observation. No duplicate task was created; continue only this session after quota propagation and keep its work isolated before any push. | Windows clean-room folder; no Windows branch was present on the remote at 18:33 CST |

Fable5 is terminal-only and reserved for a concrete unresolved disagreement after Codex, Windows Z code/Zima, and ChatGPT Pro review.

## Codex checkpoint

At 2026-08-31 17:10 CST, Codex had independently completed and pushed the public seam record, paper/OpenFHE API review, dual paper-versus-OpenFHE scale lifecycle, centered-DCP source proof, and independent oracle/red-green evidence plan. No implementation, build, or passing-test claim has been made. Remote default branch and local HEAD were verified after each coherent commit.

At 2026-08-31 17:17 CST, the logged-in official BigModel Coding Plan page showed the shared ZCode five-hour quota 100% used (reset 18:31), weekly quota 94% used (reset 2026-09-02 10:00), and MCP monthly quota 9% used (reset 2026-09-25 10:00). New ZCode dispatches and quota retries are paused; the existing Windows task is preserved. After 18:31 Codex must re-read the page before resuming at most one critical-path ZCode task while weekly usage remains above 90%. Evidence and policy: `coordination/ZCODE_QUOTA.md`.

At 2026-08-31 17:24 CST, the ChatGPT Pro recovery also ended in an explicit delivery timeout with no code or downloadable artifact. Codex will preserve that evidence, replace the monolithic response request with independently deliverable vertical slices, and continue the first DCP/RCB slice without claiming external-agent output.

At 2026-08-31 18:26 CST, the bounded DCP/RCB recovery in the same saved ChatGPT Pro conversation completed and produced a 36,865-byte candidate archive. Its hash, scan results, contents, and explicit unverified status are retained in `coordination/handoffs/chatgpt-pro-dcp-rcb-01-output.md`. The output is review input only until Codex compares it with the independent implementation and GitHub Actions/Windows produce accepted green evidence.

At 2026-08-31 18:31 CST, Codex manually refreshed the official BigModel usage page. The shared five-hour quota recovered to 0% used, while weekly usage remained 94%. Only the existing critical-path Windows ZCode task may resume; all new, duplicate, or parallel ZCode work remains paused. Evidence and policy: `coordination/ZCODE_QUOTA.md`.

## Scheduled reporting

- Automation ID: `2023-1788-openfhe-07-00-pdf`
- Kind: heartbeat attached to the project coordination task
- Schedule: daily at 07:00 Asia/Shanghai
- Output: visually verified Markdown/PDF under `reports/daily/YYYY-MM-DD/`, followed by Telegram Saved Messages delivery and `reports/delivery-log.md` evidence
- Guardrails: reporting-only writes, no Mac compilation, no source changes, no CI dispatch/rerun, and no interruption or duplicate submission to external agents

## Continuous execution

- Activated: 2026-08-31 16:20 CST
- Mechanism: active long-running Codex Goal in the same project task, separate from the daily reporting heartbeat
- Stopping condition: greenfield implementation, retained red/green oracle evidence, Windows or GitHub Actions verification, tri-party review resolution, and complete pushed provenance/report evidence
- Persistence rule: continue choosing the next safe in-scope action without waiting for a user message; pause only for a decision or authority that would materially change scope or risk
- Checkpoint rule: every coherent change is committed, pushed, and verified against the remote object ID according to `coordination/GIT_CHECKPOINT_POLICY.md`
