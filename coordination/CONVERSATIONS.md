# External agent ledger

| Agent | Task | URL or task ID | Brief | Package SHA-256 | Status | Latest point | Output |
|---|---|---|---|---|---|---|---|
| ChatGPT Pro | Clean-room design and implementation | https://chatgpt.com/c/6a952b95-0ed0-83ec-a38b-e415758ef2a5 | `coordination/tasks/chatgpt-pro-01.md` | `03d972fadb603fab937ab3772987cbc4f5c99741ac7bf0194455791c494f181c` | submitted; generating | Full 5,195-character brief and the verified ZIP were submitted at 2026-08-31 15:25 CST; the page shows active generation. Do not stop, refresh, or resend. | Branch `agent/chatgpt-pro-01`; worktree `/Users/lifeng/Documents/20231788-openfhe-chatgpt-pro-01` |
| Windows Z code/Zima | Independent clean-room implementation | Windows ZCode `Untitled session`, started 2026-08-31 14:45 CST | `coordination/tasks/windows-zcode-01.md` | clean-room task brief + paper + skill attached | working | At 2026-08-31 15:44 CST it had independently verified official OpenFHE 1.5.0 commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`, derived the paper operations, and was writing `DESIGN.md`. It reported candidate mappings for level key switching and exact integer multiplication, selected manual scaling for predictable behavior, and delegated sustained compilation to a newly installing Windows MSYS2/MinGW64 toolchain. These findings remain unreviewed. Do not interrupt. | Windows clean-room folder |

Fable5 is terminal-only and reserved for a concrete unresolved disagreement after Codex, Windows Z code/Zima, and ChatGPT Pro review.

## Scheduled reporting

- Automation ID: `2023-1788-openfhe-07-00-pdf`
- Kind: heartbeat attached to the project coordination task
- Schedule: daily at 07:00 Asia/Shanghai
- Output: visually verified Markdown/PDF under `reports/daily/YYYY-MM-DD/`, followed by Telegram Saved Messages delivery and `reports/delivery-log.md` evidence
- Guardrails: reporting-only writes, no Mac compilation, no source changes, no CI dispatch/rerun, and no interruption or duplicate submission to external agents
