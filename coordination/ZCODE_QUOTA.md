# Shared ZCode quota log

All Windows and other ZCode/Zima sessions use the same BigModel Coding Plan account. Readings come from the logged-in official usage page and are operational evidence, not credentials.

| Observed page refresh (Asia/Shanghai) | Five-hour quota | Five-hour reset | Weekly quota | Weekly reset | MCP monthly quota | MCP reset | Allocation decision |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 2026-08-31 17:17 | 100% used / 0% remaining | 2026-08-31 18:31 | 94% used / 6% remaining | 2026-09-02 10:00 | 9% used / 91% remaining | 2026-09-25 10:00 | Pause every new ZCode dispatch and quota retry. Preserve the existing Windows task. Recheck after 18:31; if five-hour capacity actually returns, resume only one critical-path ZCode task because weekly usage remains above 90%. |

Source page: `https://bigmodel.cn/coding-plan/personal/usage`

Browser task-space name: `zcode-quota-check` (reuse by name; numeric IDs are not stable).

The existing daily 07:00 project-report heartbeat was updated at 2026-08-31 17:24 CST to include a read-only quota snapshot and allocation recommendation. The app permits only one heartbeat on this task, so the active continuous project Goal—not a duplicate automation—owns the one-time post-18:31 recheck.
