# External agent ledger

| Agent | Task | URL or task ID | Brief | Package SHA-256 | Status | Latest point | Output |
|---|---|---|---|---|---|---|---|
| ChatGPT Pro | Clean-room design and implementation | https://chatgpt.com/c/6a952b95-0ed0-83ec-a38b-e415758ef2a5 | `coordination/tasks/chatgpt-pro-01.md` | `03d972fadb603fab937ab3772987cbc4f5c99741ac7bf0194455791c494f181c` | recovery submitted; generating | Read-only check at 2026-08-31 17:10 CST still showed active Extra High generation and `Stop answering`; no final assistant message or download existed. Its visible unreviewed candidate remains to place `q_div` as the full context's last tower, support exactly the first Mult2, and make the unsupported second multiplication an executable refresh-boundary rejection because the next required basis is no longer an evaluation-key prefix. Do not interrupt, refresh, retry, or resend while generation remains active. | Branch `agent/chatgpt-pro-01`; worktree `/Users/lifeng/Documents/20231788-openfhe-chatgpt-pro-01`; timeout/recovery evidence `coordination/handoffs/chatgpt-pro-01-timeout-20260831.md` |
| Windows Z code/Zima | Independent clean-room implementation | Windows ZCode `Untitled session`, started 2026-08-31 14:45 CST | `coordination/tasks/windows-zcode-01.md` | clean-room task brief + paper + skill attached | working; latest screen temporarily unavailable | At 2026-08-31 15:44 CST it had independently verified official OpenFHE 1.5.0 commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`, derived the paper operations, and was writing `DESIGN.md`. At 17:10 CST the Windows device still appeared online, but the Mac UU client continued to report error 2003 and exposed no fresh Windows screen. Do not infer that the Windows task stopped; retry read-only observation later without interrupting it. | Windows clean-room folder |

Fable5 is terminal-only and reserved for a concrete unresolved disagreement after Codex, Windows Z code/Zima, and ChatGPT Pro review.

## Codex checkpoint

At 2026-08-31 17:10 CST, Codex had independently completed and pushed the public seam record, paper/OpenFHE API review, dual paper-versus-OpenFHE scale lifecycle, centered-DCP source proof, and independent oracle/red-green evidence plan. No implementation, build, or passing-test claim has been made. Remote default branch and local HEAD were verified after each coherent commit.

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
