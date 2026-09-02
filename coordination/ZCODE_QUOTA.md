# Shared ZCode quota log

All Windows and other ZCode/Zima sessions use the same BigModel Coding Plan account. Readings come from the logged-in official usage page and are operational evidence, not credentials.

| Observed page refresh (Asia/Shanghai) | Five-hour quota | Five-hour reset | Weekly quota | Weekly reset | MCP monthly quota | MCP reset | Allocation decision |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 2026-08-31 17:17 | 100% used / 0% remaining | 2026-08-31 18:31 | 94% used / 6% remaining | 2026-09-02 10:00 | 9% used / 91% remaining | 2026-09-25 10:00 | Pause every new ZCode dispatch and quota retry. Preserve the existing Windows task. Recheck after 18:31; if five-hour capacity actually returns, resume only one critical-path ZCode task because weekly usage remains above 90%. |
| 2026-08-31 18:31 | 0% used / 100% remaining | Not displayed after reset | 94% used / 6% remaining | 2026-09-02 10:00 | 9% used / 91% remaining | 2026-09-25 10:00 | Five-hour capacity is confirmed restored. Resume only the existing critical-path Windows ZCode task; do not create, duplicate, retry, or parallelize any other ZCode task while weekly usage remains at or above 90%. Re-read this page before any later allocation change. |
| 2026-08-31 21:09 | 6% used / 94% remaining | 2026-08-31 23:35 | 95% used / 5% remaining | 2026-09-02 10:00 | 9% used / 91% remaining | 2026-09-25 10:00 | Weekly capacity has tightened further. Preserve only the existing Windows task; create no new ZCode task, retry, or parallel agent. Use GitHub Actions for the independent Windows build/test gate while this shared quota remains constrained. |
| 2026-08-31 22:31 | 9% used / 91% remaining | 2026-08-31 23:35 | 95% used / 5% remaining | 2026-09-02 10:00 | 9% used / 91% remaining | 2026-09-25 10:00 | The five-hour pool remains usable, but the weekly pool remains critical. Keep GitHub Actions as the Windows build/test runner and preserve only the existing Windows ZCode session; do not create, duplicate, retry, or parallelize ZCode work. Recheck after the five-hour reset only if the existing task can actually resume. |
| 2026-09-01 06:39 | 1% used / 99% remaining | 2026-09-01 10:59 | 96% used / 4% remaining | 2026-09-02 10:00 | 9% used / 91% remaining | 2026-09-25 10:00 | The short-window pool has recovered, but the shared weekly pool is now more constrained. Do not create or duplicate any ZCode task. Preserve at most the existing Windows session for a critical same-commit review; continue to use GitHub Actions for Windows builds/tests and re-read the official page after the weekly reset before restoring normal ZCode allocation. |
| 2026-09-01 08:40 | 1% used / 99% remaining | 2026-09-01 10:59 | 96% used / 4% remaining | 2026-09-02 10:00 | 9% used / 91% remaining | 2026-09-25 10:00 | The official page is unchanged. Keep every new or duplicate ZCode task paused; use GitHub Actions/direct Windows only after the revised Relin2 static gate passes. Preserve the one Fable5 substitution for the first exact cross-platform-green Relin2 commit, and re-read after the weekly reset before restoring normal ZCode collaboration. |
| 2026-09-01 11:10 | 1% used / 99% remaining | 2026-09-01 16:04 | 98% used / 2% remaining | 2026-09-02 10:00 | 10% used / 90% remaining | 2026-09-25 10:00 | The shared weekly pool is now effectively exhausted. Dispatch no ZCode work before the weekly reset. Keep Windows experiments on GitHub Actions or direct Windows only after the candidate passes the static gate, and reserve the single terminal-only Fable5 substitution for the first exact same-SHA Linux/Windows-green Relin2 review. |
| 2026-09-01 17:48 | 0% used / 100% remaining | Not displayed | 100% used / 0% remaining | 2026-09-02 10:00 | 10% used / 90% remaining | 2026-09-25 10:00 | The weekly pool is fully exhausted despite a recovered five-hour window. Dispatch no ZCode work before the weekly reset. Continue Codex/ChatGPT Pro review and use GitHub Actions or direct Windows only after the candidate passes static gates. Re-read the official page after 2026-09-02 10:00 before restoring ZCode allocation. |
| 2026-09-01 19:16 | 0% used / 100% remaining | Not displayed | 39% used / 61% remaining | 2026-09-02 10:00 | 4% used / 96% remaining | 2026-09-25 10:00 | The official page now reports substantial weekly capacity despite the still-displayed 2026-09-02 reset time. Treat the current page as the operational authority without inferring why the counter fell. Restore one critical-path ZCode allocation, but do not create duplicate work: use the preserved Windows session for the first exact Relin2 same-commit review after static acceptance and keep GitHub Actions/direct Windows for builds. Preserve Fable5 for a concrete unresolved three-party dispute. |
| 2026-09-02 10:03 | 1% used / 99% remaining | 2026-09-02 15:01 | 1% used / 99% remaining | 2026-09-09 10:00 | 10% used / 90% remaining | 2026-09-25 10:00 | The weekly reset is confirmed and ordinary model capacity is usable. Restore one bounded ZCode/Zima review for the next exact, stable Relin2 boundary; keep GitHub Actions as the Windows build gate, never reuse the quarantined old implementation, and do not let a stalled ZCode session block Codex. Difficult unresolved questions still escalate to terminal-only Fable 5.1 with fallback disabled. |
| 2026-09-02 21:12 | 1% used / 99% remaining | 2026-09-03 01:27 | 31% used / 69% remaining | 2026-09-09 10:00 | 13% used / 87% remaining | 2026-09-25 10:00 | Capacity remains usable. Dispatch at most one bounded, asynchronous ZCode/Zima review for the exact committed Relin2 R1 boundary; GitHub Actions remains authoritative for Windows build/CTest, a slow or stalled ZCode task does not block implementation, and difficult algorithm/API decisions go promptly to terminal-only Fable 5.1 with fallback disabled. One weekly reset ticket was shown as unused and is not consumed. |

Source page: `https://bigmodel.cn/coding-plan/personal/usage`

The latest read-only check used isolated Ego Lite task space 109 and the
official page refresh time `2026.09.02 21:12`. The page showed the values and
reset boundaries recorded in the final row above, plus one unused weekly reset
ticket. After readback, task space 109 was closed; no account token,
credential, or browser-state data was read or retained.

The existing daily 07:00 project-report heartbeat was updated at 2026-08-31 17:24 CST to include a read-only quota snapshot and allocation recommendation. The app permits only one heartbeat on this task, so the active continuous project Goal—not a duplicate automation—owns later rechecks. The latest reading above came from the freshly refreshed official page.

At approximately 2026-09-01 07:15 CST, the user authorized direct Windows
experiments or one terminal-only Fable5 review as a substitute for the pending
ZCode review. The bounded allocation and consumption rule are recorded in
`coordination/REVIEW_ALLOCATION.md`. This does not create or resume a ZCode
task and does not change the quota values above.

At 2026-09-01 22:46 CST, the one Fable5 process ended before any model output
and is operationally exhausted without a retry. This changes no BigModel quota
reading. Per the user's latest direction, ZCode remains preserved and outside
the critical path until a fresh official page/service check shows recovery;
Codex, ChatGPT Pro, GitHub Actions, and direct Windows experiments continue in
the meantime.

At 2026-09-02 00:14 CST, the user broadened the prospective fallback rule:
terminal Fable5 replaces ZCode while ZCode is unavailable, but neither service
may block the project. The failed historical Fable process remains closed; any
new Fable review must be a fresh exact-boundary task with its own evidence.
Re-read this official page after the displayed 10:00 weekly reset and restore
subsequent ZCode allocation only after quota and service are both observed
usable. This policy update is not a new quota reading.

At 2026-09-02, the user changed future fallback escalation from Fable 5 to the
latest Fable 5.1 and moved the daily PDF/Telegram report from 07:00 to 07:30
Asia/Shanghai. This supersedes the older prospective model and heartbeat-time
wording above without changing any historical receipt or quota reading.

At 2026-09-02 10:03 CST, the displayed weekly reset was confirmed: five-hour
and weekly usage were each 1%. ZCode/Zima therefore returns to the bounded
collaboration pool for subsequent exact-boundary review, while GitHub Actions
continues to own reproducible Windows builds. Quota recovery does not authorize
reuse of the historical Windows implementation and does not turn ZCode into a
critical-path blocker. Fable 5.1 remains the prompt escalation for a concrete
hard question that the ordinary reviewers cannot settle.

At 2026-09-02 21:12 CST, a fresh official-page read confirmed the shared pool
remained available: five-hour use 1%, weekly use 31%, and MCP monthly use 13%.
The exact Relin2 R1 test boundary may therefore receive one asynchronous
Windows ZCode review. This later reading does not broaden the clean-room scope,
replace hosted Windows evidence, or delay the production green step.
