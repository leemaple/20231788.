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

Source page: `https://bigmodel.cn/coding-plan/personal/usage`

Latest read-only check reused the agent-owned project coordination task space,
numeric ID 53, without taking over the user-owned quota-monitor space. The
official page reported refresh time `2026.09.01 11:10`; the scratch quota tab
was closed after recording the values.

The existing daily 07:00 project-report heartbeat was updated at 2026-08-31 17:24 CST to include a read-only quota snapshot and allocation recommendation. The app permits only one heartbeat on this task, so the active continuous project Goal—not a duplicate automation—owns later rechecks. The latest reading above came from the freshly refreshed official page.

At approximately 2026-09-01 07:15 CST, the user authorized direct Windows
experiments or one terminal-only Fable5 review as a substitute for the pending
ZCode review. The bounded allocation and consumption rule are recorded in
`coordination/REVIEW_ALLOCATION.md`. This does not create or resume a ZCode
task and does not change the quota values above.
