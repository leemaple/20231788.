# OpenFHE 2023/1788 clean-room implementation

This branch is a greenfield implementation of the `t=2` Double-CKKS multiplication method from IACR ePrint 2023/1788 for official pristine OpenFHE 1.5.0.

The destination repository's previous implementation and every related local code tree are quarantined and are not inputs. Development begins with paper-derived specifications and red-first independent-oracle tests.

## Live project state

- Status checkpoint: 2026-09-04 Asia/Shanghai. This is a **partial implementation**, not a completed reproduction of the paper.
- Default branch: [`cleanroom/reimplement-mult2-20260831`](https://github.com/leemaple/20231788./tree/cleanroom/reimplement-mult2-20260831). This consolidation joins its coordination/report history with the reviewed implementation from [`codex/integration-01`](https://github.com/leemaple/20231788./tree/codex/integration-01), commit `4938b4458e3de17cd4bf48230878c1dea3aa1dfd`; promotion evidence is recorded in [`coordination/DEFAULT_MAINLINE_CONSOLIDATION.md`](coordination/DEFAULT_MAINLINE_CONSOLIDATION.md).
- Implemented bounded seams: DCP/RCB, Tensor2, Relin2, RS2, first Mult2, and pair Add/Sub, including pair inputs to first Mult2. Scope and limitations remain those of the [integration acceptance](coordination/INTEGRATION_PAIR_COMPOSITION_05.md) and [hosted audit](coordination/INTEGRATION_PAIR_COMPOSITION_HOSTED_AUDIT.md).
- Latest accepted integration regression: [run 33882911345](https://github.com/leemaple/20231788./actions/runs/33882911345), exact tested commit `7c982519dfedacf5505dbd0f1ca6579ee91da2fd`, passed **57/57 tests on Linux and 57/57 on Windows**, plus warning-clean builds and public API checks. Commit `4938b445` adds evidence only. This is prior integration evidence, not a claim that the new consolidation commit has already run.
- Precision evidence: the checked small-parameter first-Mult2 exact-rational contracts pass the `2^-80` error threshold. This does not establish eight repeated squarings, all-key correctness, production high-precision I/O, or the paper's full parameter/1000-run result.
- Remaining critical work: semantic second-to-eighth Mult2, production lossless client I/O, paper-compatible h=128 setup and full `N=32768`/100-bit-scale repeated-squaring experiments. Drafts on the [repeated-operation](https://github.com/leemaple/20231788./tree/codex/repeated-mult2-01) and [client-I/O](https://github.com/leemaple/20231788./tree/codex/lossless-io-01) branches are not accepted implementations.
- External collaboration: ChatGPT Pro supplies design/code proposals; Codex verifies and integrates; ZCode provides independent review when the shared quota permits. Escalation uses terminal Fable **5.1** only when its actual model identity is verified. An unavailable reviewer is not a reason to stop other authorized work. Long-running Pro tasks are not interrupted or resubmitted.
- Continuity: an active Codex Goal advances the project across turns; the separate daily **07:30 Asia/Shanghai** report is delivered as a PDF to Telegram Saved Messages. Historical quota and conversation entries below are timestamped evidence, not current availability claims.
- Progress evidence: [`coordination/CONVERSATIONS.md`](coordination/CONVERSATIONS.md)
- Paper/OpenFHE API review: [`coordination/CODEX_API_REVIEW.md`](coordination/CODEX_API_REVIEW.md)
- Tensor2 dual-scale derivation: [`coordination/reviews/tensor2-scale-derivation.md`](coordination/reviews/tensor2-scale-derivation.md)
- Confirmed TDD seams: [`coordination/TEST_SEAMS.md`](coordination/TEST_SEAMS.md)
- Independent oracle and red/green evidence plan: [`coordination/INDEPENDENT_ORACLE_PLAN.md`](coordination/INDEPENDENT_ORACLE_PLAN.md)
- Tensor2 bounded API/scale contract: [`coordination/TENSOR2_DESIGN.md`](coordination/TENSOR2_DESIGN.md)
- Shared ZCode quota and allocation log: [`coordination/ZCODE_QUOTA.md`](coordination/ZCODE_QUOTA.md)
- Integration gates: [`coordination/INTEGRATION_REVIEW_CHECKLIST.md`](coordination/INTEGRATION_REVIEW_CHECKLIST.md)
- Git checkpoint policy: [`coordination/GIT_CHECKPOINT_POLICY.md`](coordination/GIT_CHECKPOINT_POLICY.md)

All coherent project changes are committed in small checkpoints and pushed immediately. Agent work stays on isolated branches until reviewed; shared history is never force-pushed. Red/green records are retained under [`artifacts/tdd/dcp-rcb`](artifacts/tdd/dcp-rcb) and [`artifacts/tdd/tensor2`](artifacts/tdd/tensor2).
