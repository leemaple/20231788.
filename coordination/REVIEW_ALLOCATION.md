# Review and compute allocation

## 2026-09-01 post-report authorization

At approximately 07:15 Asia/Shanghai, the user explicitly authorized either of
these bounded substitutes while shared ZCode capacity is constrained:

- Codex may operate the Windows computer directly for project experiments; or
- one terminal-only Fable5 invocation may replace the pending ZCode review.

The current allocation decision is:

1. Keep GitHub Actions and, when useful, direct Windows operation as the build
   and experiment path. Do not consume shared ZCode quota for a duplicate task.
2. Reserve exactly one Fable5 invocation for an independent algorithm and code
   review of the first exact Relin2 commit that has passed the full Linux and
   Windows gate. Calling it before the implementation and evidence are stable
   would spend the one-time review on a moving target.
3. Invoke Fable5 from the terminal only. Provide the exact commit, paper/API
   contract, relevant source/tests, retained red-green evidence, CI identities,
   explicit acceptance questions, and prohibition on unsupported claims.
4. Record the prompt/artifact identities, invocation time, result, and whether
   the one-time authorization was consumed. Until that record exists, the
   authorization remains unused.

This is a task-specific user override, not a permanent relaxation of the
project workflow's ordinary three-party escalation rule.

## 2026-09-01 19:16 quota restoration

The official BigModel usage page now reports 0% five-hour use and 39% weekly
use, with the weekly reset still displayed as 2026-09-02 10:00. The reason for
the counter change is unknown; the current official reading is sufficient for
an operational allocation change but not for a claim about provider billing.

Normal three-party review allocation is restored for the next stable Relin2
candidate: Codex plus the preserved Windows ZCode/Zima session plus ChatGPT Pro
must review the same exact commit after its hosted Linux/Windows gate. Do not
resume the stale Windows implementation as a source input and do not create a
duplicate ZCode task. The one terminal-only Fable5 authorization remains unused
and is now held for a concrete unresolved dispute rather than preallocated as a
quota substitute.

## 2026-09-01 21:26 Fable5 substitution

The user explicitly superseded the 19:16 allocation for the current review:
use terminal Fable5 in place of ZCode, return to ZCode after its service/capacity
recovers, and do not let ZCode block the critical path.

The already submitted Windows task remains preserved exactly as dispatched. It
showed only the unchanged `已工作 1 秒` shell after more than fifteen minutes and
never produced a verdict, error, or completed artifact. Do not interrupt,
retry, resend, duplicate, reclaim, or treat that blank state as evidence.

Exactly one Fable5 provider process is now allocated to an independent review
of accepted DCP/RCB/Tensor2 commit `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.
Its task, packet, sandbox, and receipt must pass the project gates before
launch. The allowance is consumed at process start, even if transport or output
is later lost; no retry, resume, follow-up, or second Fable invocation is
permitted. ZCode returns to the normal collaboration pool only after a fresh
capacity/service check shows it is usable.

## 2026-09-01 22:46 Fable5 terminal outcome

All pre-launch gates passed and the one authorized provider-capable process
started exactly once. It terminated naturally after one second with CLI exit 1
and no `stream-json` event. The sole stderr was the local sandbox error
`EPERM: operation not permitted, mkdir '/tmp/claude-501'`; the wrapper's
combined post-identity comparison exited 0, though it did not emit separate
per-item post hashes. No session ID, tool call, source path, model answer,
provider duration/cost, or verdict was emitted.

Provider acceptance cannot be proven, so the authorization is recorded exactly
as `consumption-unknown (operationally exhausted)`. Do not retry, resume,
follow up, create a second Fable process, or represent this as an independent
review. Continue the critical path with Codex, the existing ChatGPT Pro work,
and GitHub Actions/direct Windows experiments. Keep the existing ZCode task
preserved and outside the critical path; restore ZCode collaboration only after
a fresh capacity/service check. Exact evidence is in
`coordination/handoffs/fable5-fb862a3-review-01-receipt/`.

## 2026-09-02 00:14 nonblocking substitution

The user prospectively superseded the one-process restriction above. While
ZCode is unavailable, use terminal Fable5 as its review substitute and keep the
project moving. The failed `fb862a3` invocation remains an immutable historical
record and must not be described as a review; any later Fable5 use is a new,
exact-boundary task with its own sanitized packet, preflight, process identity,
and receipt rather than a retry or continuation of that failed process.

Fable5 availability is advisory, not a completion lock. If a new invocation
cannot start or produces no usable result, record the exact outcome and proceed
with Codex, ChatGPT Pro when available, the existing independent reviewers,
GitHub Actions/direct Windows, and executable evidence. Do not resend the same
request merely because a response is slow.

Re-read the official BigModel usage page after the displayed 2026-09-02 10:00
weekly reset before assigning new ZCode work. Once service and quota are both
usable, return subsequent ZCode review/build work to its Windows clean-room
task. Do not retroactively reopen already evidenced boundaries solely because
an external reviewer was unavailable.

## 2026-09-02 04:09 Relin2 validation Fable5 outcome

The fresh exact-boundary Relin2 review packet never reached a model. One
pre-provider wrapper attempt exposed and then closed an inherited-`TRAPEXIT`
temporary-file bug. Two later provider-capable processes both exited during
local CLI startup with zero raw stdout, zero stream events, zero session/model
identity, and no usage or answer: the first rejected the empty MCP JSON shape;
after that one-line correction and a fresh three-axis PASS, the final process
was denied while creating `/tmp/claude-501` by the read-only sandbox.

Both provider-capable attempts completed their 65-entry receipt manifests,
post-provider exact-token scans, Gitleaks scans, and identity gates. The final
guard remains bound to `receipt-attempt-13408`; no further process is permitted
for this task and no Fable5 review is claimed. Exact identities and outcomes are
in `coordination/handoffs/fable5-relin2-validation-84df651-review-01/RECEIPT.md`.
Per the nonblocking policy, Codex continues Relin2 TDD with existing independent
reviewers and Linux/Windows executable evidence. ZCode remains outside the
critical path until a fresh official service/quota check supports restoring it.

## 2026-09-02 Fable 5.1 and daily-report update

The user superseded the prospective Fable model choice: new difficult-question
escalations and ZCode-substitute reviews must use the latest Fable 5.1, not the
older Fable 5 model. Invoke it only from Terminal, prefer the CLI's `fable`
latest-model alias or a provider-advertised exact 5.1 identifier, disable model
fallback, and retain the emitted model identity before accepting any verdict.
Historical Fable 5 tasks and receipts remain immutable evidence and are not
renamed or retroactively claimed as Fable 5.1 reviews. Fable availability stays
nonblocking.

The user also moved the daily standalone PDF report and Telegram Saved Messages
delivery from 07:00 to 07:30 Asia/Shanghai. The project reporting skill and
automation own the new schedule.

## 2026-09-02 10:03 ZCode quota restoration

The official BigModel page refreshed at 10:03 and reported 1% five-hour use,
1% weekly use, and 10% MCP monthly use. The next resets displayed were 15:01,
2026-09-09 10:00, and 2026-09-25 10:00 respectively. Quota is therefore no
longer the reason to exclude ZCode.

For the next stable Relin2 boundary, one bounded independent Windows
ZCode/Zima review may be dispatched after the exact source commit and hosted
Linux/Windows evidence are fixed. It must use a fresh clean-room folder and
sanitized exact-commit packet, not the historical implementation session or
its uncommitted changes. GitHub Actions remains the reproducible Windows build
and CTest authority. A slow or stalled ZCode task stays off the critical path.

The user's new escalation rule remains separate: a concrete difficult question
that ordinary source review and tests do not resolve is sent promptly to the
latest Fable 5.1 through Terminal, with fallback disabled and model identity
verified. Routine boundaries do not spend a Fable escalation merely because
ZCode quota is available or unavailable.
