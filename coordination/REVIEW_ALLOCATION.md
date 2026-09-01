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
