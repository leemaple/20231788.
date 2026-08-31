# Shared ZCode quota log

All Windows and other ZCode/Zima sessions use the same BigModel Coding Plan account. Readings come from the logged-in official usage page and are operational evidence, not credentials.

| Observed page refresh (Asia/Shanghai) | Five-hour quota | Five-hour reset | Weekly quota | Weekly reset | MCP monthly quota | MCP reset | Allocation decision |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 2026-08-31 17:17 | 100% used / 0% remaining | 2026-08-31 18:31 | 94% used / 6% remaining | 2026-09-02 10:00 | 9% used / 91% remaining | 2026-09-25 10:00 | Pause every new ZCode dispatch and quota retry. Preserve the existing Windows task. Recheck after 18:31; if five-hour capacity actually returns, resume only one critical-path ZCode task because weekly usage remains above 90%. |
| 2026-08-31 18:31 | 0% used / 100% remaining | Not displayed after reset | 94% used / 6% remaining | 2026-09-02 10:00 | 9% used / 91% remaining | 2026-09-25 10:00 | Five-hour capacity is confirmed restored. Resume only the existing critical-path Windows ZCode task; do not create, duplicate, retry, or parallelize any other ZCode task while weekly usage remains at or above 90%. Re-read this page before any later allocation change. |
| 2026-08-31 21:09 | 6% used / 94% remaining | 2026-08-31 23:35 | 95% used / 5% remaining | 2026-09-02 10:00 | 9% used / 91% remaining | 2026-09-25 10:00 | Weekly capacity has tightened further. Preserve only the existing Windows task; create no new ZCode task, retry, or parallel agent. Use GitHub Actions for the independent Windows build/test gate while this shared quota remains constrained. |
| 2026-08-31 22:31 | 9% used / 91% remaining | 2026-08-31 23:35 | 95% used / 5% remaining | 2026-09-02 10:00 | 9% used / 91% remaining | 2026-09-25 10:00 | The five-hour pool remains usable, but the weekly pool remains critical. Keep GitHub Actions as the Windows build/test runner and preserve only the existing Windows ZCode session; do not create, duplicate, retry, or parallelize ZCode work. Recheck after the five-hour reset only if the existing task can actually resume. |
| 2026-09-01 06:39 | 1% used / 99% remaining | 2026-09-01 10:59 | 96% used / 4% remaining | 2026-09-02 10:00 | 9% used / 91% remaining | 2026-09-25 10:00 | The short-window pool has recovered, but the shared weekly pool is now more constrained. Do not create or duplicate any ZCode task. Preserve at most the existing Windows session for a critical same-commit review; continue to use GitHub Actions for Windows builds/tests and re-read the official page after the weekly reset before restoring normal ZCode allocation. |

Source page: `https://bigmodel.cn/coding-plan/personal/usage`

Latest read-only browser task space: `zcode-quota-monitor-20260901`, numeric ID
86. It was closed after the official-page values were recorded; future reads
may create a fresh short-lived monitor space because numeric IDs are not stable.

The existing daily 07:00 project-report heartbeat was updated at 2026-08-31 17:24 CST to include a read-only quota snapshot and allocation recommendation. The app permits only one heartbeat on this task, so the active continuous project Goal—not a duplicate automation—owns later rechecks. The latest reading above came from the freshly refreshed official page.

At approximately 2026-09-01 07:15 CST, the user authorized direct Windows
experiments or one terminal-only Fable5 review as a substitute for the pending
ZCode review. The bounded allocation and consumption rule are recorded in
`coordination/REVIEW_ALLOCATION.md`. This does not create or resume a ZCode
task and does not change the quota values above.
