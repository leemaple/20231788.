# OpenFHE 2023/1788 clean-room implementation

This branch is a greenfield implementation of the `t=2` Double-CKKS multiplication method from IACR ePrint 2023/1788 for official pristine OpenFHE 1.5.0.

The destination repository's previous implementation and every related local code tree are quarantined and are not inputs. Development begins with paper-derived specifications and red-first independent-oracle tests.

## Live project state

- Active/default branch: [`cleanroom/reimplement-mult2-20260831`](https://github.com/leemaple/20231788./tree/cleanroom/reimplement-mult2-20260831)
- Current phase: clean-room specification, agent handoff, and integration gates are recorded; no implementation code has been accepted yet.
- External work: ChatGPT Pro and Windows Z code/Zima are working independently. Their output is accepted only after provenance capture, TDD evidence, and review.
- Continuity: an active long-running Codex Goal advances the project across turns; the daily 07:00 Asia/Shanghai automation independently produces and delivers the PDF status report.
- Progress evidence: [`coordination/CONVERSATIONS.md`](coordination/CONVERSATIONS.md)
- Paper/OpenFHE API review: [`coordination/CODEX_API_REVIEW.md`](coordination/CODEX_API_REVIEW.md)
- Confirmed TDD seams: [`coordination/TEST_SEAMS.md`](coordination/TEST_SEAMS.md)
- Independent oracle and red/green evidence plan: [`coordination/INDEPENDENT_ORACLE_PLAN.md`](coordination/INDEPENDENT_ORACLE_PLAN.md)
- Integration gates: [`coordination/INTEGRATION_REVIEW_CHECKLIST.md`](coordination/INTEGRATION_REVIEW_CHECKLIST.md)
- Git checkpoint policy: [`coordination/GIT_CHECKPOINT_POLICY.md`](coordination/GIT_CHECKPOINT_POLICY.md)

All coherent project changes are committed in small checkpoints and pushed immediately. Agent work stays on isolated branches until reviewed; shared history is never force-pushed.
