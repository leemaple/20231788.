# OpenFHE 2023/1788 clean-room implementation

This branch is a greenfield implementation of the `t=2` Double-CKKS multiplication method from IACR ePrint 2023/1788 for official pristine OpenFHE 1.5.0.

The destination repository's previous implementation and every related local code tree are quarantined and are not inputs. Development begins with paper-derived specifications and red-first independent-oracle tests.

## Live project state

- Default integration branch: [`cleanroom/reimplement-mult2-20260831`](https://github.com/leemaple/20231788./tree/cleanroom/reimplement-mult2-20260831)
- Current implementation slice: [`agent/codex-dcp-rcb-01`](https://github.com/leemaple/20231788./tree/agent/codex-dcp-rcb-01) contains the isolated DCP/RCB implementation and independent-oracle tests. It is not merged into the default branch.
- Verified on Linux and Windows: [GitHub Actions 33399245184](https://github.com/leemaple/20231788./actions/runs/33399245184) builds pinned pristine OpenFHE 1.5.0 and then compiles/runs the complete DCP/RCB test executable with strict GCC warnings on both Linux and the officially supported Windows/MinGW64 path.
- Still pending: final same-commit Windows Z code/Zima and ChatGPT Pro review, plus implementation of Tensor2, Relin2, RS2, Mult2, and pair addition/subtraction.
- External review: ChatGPT Pro completed its independent pre-hardening DCP/RCB review and the accepted findings have been addressed test-first. The preserved Windows Z code/Zima task has not yet reviewed the final green commit; no ZCode acceptance is claimed.
- Continuity: an active long-running Codex Goal advances the project across turns; the daily 07:00 Asia/Shanghai automation independently produces and delivers the PDF status report.
- Progress evidence: [`coordination/CONVERSATIONS.md`](coordination/CONVERSATIONS.md)
- Paper/OpenFHE API review: [`coordination/CODEX_API_REVIEW.md`](coordination/CODEX_API_REVIEW.md)
- Confirmed TDD seams: [`coordination/TEST_SEAMS.md`](coordination/TEST_SEAMS.md)
- Independent oracle and red/green evidence plan: [`coordination/INDEPENDENT_ORACLE_PLAN.md`](coordination/INDEPENDENT_ORACLE_PLAN.md)
- Shared ZCode quota and allocation log: [`coordination/ZCODE_QUOTA.md`](coordination/ZCODE_QUOTA.md)
- Integration gates: [`coordination/INTEGRATION_REVIEW_CHECKLIST.md`](coordination/INTEGRATION_REVIEW_CHECKLIST.md)
- Git checkpoint policy: [`coordination/GIT_CHECKPOINT_POLICY.md`](coordination/GIT_CHECKPOINT_POLICY.md)

All coherent project changes are committed in small checkpoints and pushed immediately. Agent work stays on isolated branches until reviewed; shared history is never force-pushed. Red/green records for the current slice are retained under [`artifacts/tdd/dcp-rcb`](artifacts/tdd/dcp-rcb).
