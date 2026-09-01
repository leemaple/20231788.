# OpenFHE 2023/1788 clean-room implementation

This branch is a greenfield implementation of the `t=2` Double-CKKS multiplication method from IACR ePrint 2023/1788 for official pristine OpenFHE 1.5.0.

The destination repository's previous implementation and every related local code tree are quarantined and are not inputs. Development begins with paper-derived specifications and red-first independent-oracle tests.

## Live project state

- Active/default branch: [`cleanroom/reimplement-mult2-20260831`](https://github.com/leemaple/20231788./tree/cleanroom/reimplement-mult2-20260831)
- Accepted DCP/RCB/Tensor2 baseline: isolated branch [`agent/codex-relin2-01`](https://github.com/leemaple/20231788./tree/agent/codex-relin2-01), exact clean head `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`, tree `759d5195739684748d5a9664edabe3fa719e1acf`. The retained exact-head run [33436252725](https://github.com/leemaple/20231788./actions/runs/33436252725) passed Linux and Windows 6/6; the manual registration-evidence run [33513310345](https://github.com/leemaple/20231788./actions/runs/33513310345) independently verified the same candidate and complete six-test CTest registration on both platforms.
- Relin2 next slice: no Relin2 source has been applied to the accepted branch. The first external delivery remains rejected; its sequential remediation chain and downstream paper/OpenFHE/TDD gates are retained while the latest ChatGPT Pro remediation task is allowed to finish naturally without interruption or duplicate submission.
- External review allocation: the submitted Windows ZCode/Zima exact-commit task remains preserved but outside the critical path until a fresh capacity/service check shows recovery. The one authorized terminal Fable5 substitute was launched exactly once at 2026-09-01 22:46 CST and exited before any model output because the read-only sandbox denied `/tmp/claude-501`; no verdict is claimed and the allowance is operationally exhausted. Codex, ChatGPT Pro, GitHub Actions, and direct Windows experiments continue without waiting on ZCode.
- Continuity: an active long-running Codex Goal advances the project across turns; the daily 07:00 Asia/Shanghai automation independently produces and delivers the PDF status report.
- Progress evidence: [`coordination/CONVERSATIONS.md`](coordination/CONVERSATIONS.md)
- Paper/OpenFHE API review: [`coordination/CODEX_API_REVIEW.md`](coordination/CODEX_API_REVIEW.md)
- Tensor2 dual-scale derivation: [`coordination/reviews/tensor2-scale-derivation.md`](coordination/reviews/tensor2-scale-derivation.md)
- Confirmed TDD seams: [`coordination/TEST_SEAMS.md`](coordination/TEST_SEAMS.md)
- Independent oracle and red/green evidence plan: [`coordination/INDEPENDENT_ORACLE_PLAN.md`](coordination/INDEPENDENT_ORACLE_PLAN.md)
- Shared ZCode quota and allocation log: [`coordination/ZCODE_QUOTA.md`](coordination/ZCODE_QUOTA.md)
- Integration gates: [`coordination/INTEGRATION_REVIEW_CHECKLIST.md`](coordination/INTEGRATION_REVIEW_CHECKLIST.md)
- Git checkpoint policy: [`coordination/GIT_CHECKPOINT_POLICY.md`](coordination/GIT_CHECKPOINT_POLICY.md)

All coherent project changes are committed in small checkpoints and pushed immediately. Agent work stays on isolated branches until reviewed; shared history is never force-pushed.
