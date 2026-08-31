# OpenFHE 2023/1788 clean-room implementation

This branch is a greenfield implementation of the `t=2` Double-CKKS multiplication method from IACR ePrint 2023/1788 for official pristine OpenFHE 1.5.0.

The destination repository's previous implementation and every related local code tree are quarantined and are not inputs. Development begins with paper-derived specifications and red-first independent-oracle tests.

## Live project state

- Default integration branch: [`cleanroom/reimplement-mult2-20260831`](https://github.com/leemaple/20231788./tree/cleanroom/reimplement-mult2-20260831)
- Current implementation slice: [`agent/codex-dcp-rcb-01`](https://github.com/leemaple/20231788./tree/agent/codex-dcp-rcb-01) contains the isolated DCP/RCB implementation and independent-oracle tests. It is not merged into the default branch.
- Verified on Linux: the strict GCC build and complete DCP/RCB test executable pass against pinned pristine OpenFHE 1.5.0; the latest green run is [GitHub Actions 33394619792](https://github.com/leemaple/20231788./actions/runs/33394619792).
- Still pending: independent Windows/MSVC execution and implementation of Tensor2, Relin2, RS2, Mult2, and pair addition/subtraction.
- External review: ChatGPT Pro completed its independent DCP/RCB review and the accepted findings have been addressed test-first. The existing Windows Z code/Zima task remains active; no Windows result is claimed yet.
- Continuity: an active long-running Codex Goal advances the project across turns; the daily 07:00 Asia/Shanghai automation independently produces and delivers the PDF status report.
- Progress evidence: [`coordination/CONVERSATIONS.md`](coordination/CONVERSATIONS.md)
- Paper/OpenFHE API review: [`coordination/CODEX_API_REVIEW.md`](coordination/CODEX_API_REVIEW.md)
- Confirmed TDD seams: [`coordination/TEST_SEAMS.md`](coordination/TEST_SEAMS.md)
- Independent oracle and red/green evidence plan: [`coordination/INDEPENDENT_ORACLE_PLAN.md`](coordination/INDEPENDENT_ORACLE_PLAN.md)
- Shared ZCode quota and allocation log: [`coordination/ZCODE_QUOTA.md`](coordination/ZCODE_QUOTA.md)
- Integration gates: [`coordination/INTEGRATION_REVIEW_CHECKLIST.md`](coordination/INTEGRATION_REVIEW_CHECKLIST.md)
- Git checkpoint policy: [`coordination/GIT_CHECKPOINT_POLICY.md`](coordination/GIT_CHECKPOINT_POLICY.md)

All coherent project changes are committed in small checkpoints and pushed immediately. Agent work stays on isolated branches until reviewed; shared history is never force-pushed. Red/green records for the current slice are retained under [`artifacts/tdd/dcp-rcb`](artifacts/tdd/dcp-rcb).
