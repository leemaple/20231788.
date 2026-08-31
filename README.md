# OpenFHE 2023/1788 clean-room implementation

This branch is a greenfield implementation of the `t=2` Double-CKKS multiplication method from IACR ePrint 2023/1788 for official pristine OpenFHE 1.5.0.

The destination repository's previous implementation and every related local code tree are quarantined and are not inputs. Development begins with paper-derived specifications and red-first independent-oracle tests.

## Live project state

- Default integration branch: [`cleanroom/reimplement-mult2-20260831`](https://github.com/leemaple/20231788./tree/cleanroom/reimplement-mult2-20260831)
- Current implementation slice: `agent/codex-tensor2-01` extends the accepted DCP/RCB base with the bounded `t=2` Tensor2 public seam, distinct three-component result type, and independent negacyclic-oracle tests. It is not merged into the default branch.
- Exact Tensor2 base evidence: GitHub Actions run `33411494861` passed the pristine OpenFHE 1.5.0 DCP/RCB base at source commit `87c84b879c13b55cf15d6559d3317853228fdc05` on Linux/GCC and Windows 2022/MSYS2 MinGW64. That retained run is baseline evidence only; final Tensor2 same-commit Actions verification is pending downstream application.
- First Tensor2 production green: Actions run `33427271692` passed the strict build and all 6/6 CTest entries on Linux at implementation commit `1408d46217e97a1c14d43d49b64791da22f652da`; its intermediate Windows job was intentionally cancelled. Merge requires a later run whose `headSha` is the final source/test/workflow commit and whose Linux and Windows jobs both succeed.
- External review: ChatGPT Pro returned the ordered Tensor2 candidate with verdict `ready to apply`; Codex independently hardened its pre-arithmetic validation test and reviewed the resulting source. ChatGPT Pro has not yet reviewed the final post-hardening commit, and the preserved Windows ZCode/Zima task has not reviewed it. The DCP/RCB-only exact closure is `MERGEABLE`, conditional on that separate Windows review.
- Still outside this bounded slice: exact-current external closure and implementation of Relin2, RS2, Mult2, and pair addition/subtraction.
- Continuity: an active long-running Codex Goal advances the project across turns; the daily 07:00 Asia/Shanghai automation independently produces and delivers the PDF status report.
- Progress evidence: [`coordination/CONVERSATIONS.md`](coordination/CONVERSATIONS.md)
- Paper/OpenFHE API review: [`coordination/CODEX_API_REVIEW.md`](coordination/CODEX_API_REVIEW.md)
- Confirmed TDD seams: [`coordination/TEST_SEAMS.md`](coordination/TEST_SEAMS.md)
- Independent oracle and red/green evidence plan: [`coordination/INDEPENDENT_ORACLE_PLAN.md`](coordination/INDEPENDENT_ORACLE_PLAN.md)
- Tensor2 bounded API/scale contract: [`coordination/TENSOR2_DESIGN.md`](coordination/TENSOR2_DESIGN.md)
- Shared ZCode quota and allocation log: [`coordination/ZCODE_QUOTA.md`](coordination/ZCODE_QUOTA.md)
- Integration gates: [`coordination/INTEGRATION_REVIEW_CHECKLIST.md`](coordination/INTEGRATION_REVIEW_CHECKLIST.md)
- Git checkpoint policy: [`coordination/GIT_CHECKPOINT_POLICY.md`](coordination/GIT_CHECKPOINT_POLICY.md)

All coherent project changes are committed in small checkpoints and pushed immediately. Agent work stays on isolated branches until reviewed; shared history is never force-pushed. Red/green records are retained under [`artifacts/tdd/dcp-rcb`](artifacts/tdd/dcp-rcb) and [`artifacts/tdd/tensor2`](artifacts/tdd/tensor2).
