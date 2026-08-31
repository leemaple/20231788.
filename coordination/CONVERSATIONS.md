# External agent ledger

| Agent | Task | URL or task ID | Brief | Package SHA-256 | Status | Latest point | Output |
|---|---|---|---|---|---|---|---|
| ChatGPT Pro | Bounded DCP/RCB clean-room slice | https://chatgpt.com/c/6a952b95-0ed0-83ec-a38b-e415758ef2a5 | `coordination/tasks/chatgpt-pro-dcp-rcb-01.md` | `82b40b8084f35d2d0785f2ca05aa8e77842fd00a1c575d23cd7c49ddb8408243` | verified package prepared; submission pending | The two monolithic responses timed out without a code artifact. A new complete package at clean-room commit `7db43014a9c80d64e5f03f6666a710a3d7ca7e55` narrows recovery to ordered red-test and implementation patches for DCP/RCB only. Do not use the generic Retry control; submit the bounded task as a new message in the saved conversation. | Branch `agent/chatgpt-pro-01`; worktree `/Users/lifeng/Documents/20231788-openfhe-chatgpt-pro-01`; timeout evidence plus `coordination/handoffs/chatgpt-pro-dcp-rcb-01.md` |
| Windows Z code/Zima | Independent clean-room implementation | Windows ZCode `Untitled session`, started 2026-08-31 14:45 CST | `coordination/tasks/windows-zcode-01.md` | clean-room task brief + paper + skill attached | working; latest screen temporarily unavailable | At 2026-08-31 15:44 CST it had independently verified official OpenFHE 1.5.0 commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`, derived the paper operations, and was writing `DESIGN.md`. At 17:10 CST the Windows device still appeared online, but the Mac UU client continued to report error 2003 and exposed no fresh Windows screen. Do not infer that the Windows task stopped; retry read-only observation later without interrupting it. | Windows clean-room folder |

Fable5 is terminal-only and reserved for a concrete unresolved disagreement after Codex, Windows Z code/Zima, and ChatGPT Pro review.

## Codex checkpoint

At 2026-08-31 17:10 CST, Codex had independently completed and pushed the public seam record, paper/OpenFHE API review, dual paper-versus-OpenFHE scale lifecycle, centered-DCP source proof, and independent oracle/red-green evidence plan. No implementation, build, or passing-test claim has been made. Remote default branch and local HEAD were verified after each coherent commit.

At 2026-08-31 17:17 CST, the logged-in official BigModel Coding Plan page showed the shared ZCode five-hour quota 100% used (reset 18:31), weekly quota 94% used (reset 2026-09-02 10:00), and MCP monthly quota 9% used (reset 2026-09-25 10:00). New ZCode dispatches and quota retries are paused; the existing Windows task is preserved. After 18:31 Codex must re-read the page before resuming at most one critical-path ZCode task while weekly usage remains above 90%. Evidence and policy: `coordination/ZCODE_QUOTA.md`.

At 2026-08-31 17:24 CST, the ChatGPT Pro recovery also ended in an explicit delivery timeout with no code or downloadable artifact. Codex will preserve that evidence, replace the monolithic response request with independently deliverable vertical slices, and continue the first DCP/RCB slice without claiming external-agent output.

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
