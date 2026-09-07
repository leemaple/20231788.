# OpenFHE 2023/1788 clean-room implementation

This branch is a greenfield implementation of the `t=2` Double-CKKS multiplication method from IACR ePrint 2023/1788 for official pristine OpenFHE 1.5.0.

**最新交付与用户检查入口（2026-09-07）：[中文检查指南](CHECK_AND_HANDOFF.zh-CN.md)。** 最新增量在 `codex/s100-fresh-error-repair-20260907` 分支；仓库默认分支仍是上一版。原 S100 精度 FAIL 保留，现有测试／审核未发现明确未处理的生产实现缺陷，但不构成绝对正确性证明。用户已决定不联系作者、暂不继续追加实验。

此前 S116 固定快照说明：[实现做了什么、复现命令、结果与限制](REPRODUCE.zh-CN.md)。下方 delivery/default-branch 记录描述上一轮 S116 交付，最新源码身份、诊断增量和当前停止边界以中文检查指南为准。

The destination repository's previous implementation and every related local code tree are quarantined and are not inputs. Development begins with paper-derived specifications and red-first independent-oracle tests.

## Live project state

- Status checkpoint: **2026-09-07 Asia/Shanghai**. The constructive t=2 algorithm implementation is **delivered with qualified experimental-profile correctness**, not exact Table3 reproduction or a security-certified library. The [delivery record](IMPLEMENTATION_DELIVERY.md) maps requirements to evidence. The remote default branch has the implementation; the protected local reporting checkout intentionally retains its existing edits.
- Implemented paths include DCP/RCB, Tensor2, Relin2, RS2, Mult2, pair Add/Sub, high-precision client I/O, repeated multiplication and same-root h=128 setup. The original paper-scale path has actually executed one encryption followed by eight squarings on both hosts at `N=32768` / 16384 slots. **Its execution completed, but its frozen numerical acceptance failed.** See the [full-slot acceptance and retained evidence](coordination/fs-endpoint-live-run-01/ACCEPTANCE.md).
- Original-profile result: [run 34039088536](https://github.com/leemaple/20231788./actions/runs/34039088536), source `ed5fd192a89d6d4728ad295e87cf06a3f4abc832`, measured final maximum real/imaginary component error approximately `9.15e-24` on Linux and `9.07e-24` on Windows, about **11 times** the unchanged `2^-80` limit. Completing diagnostics does not turn this FAIL into PASS. The [subsequent scientific review disposition](coordination/fs-endpoint-scientific-review-return-01/ACCEPTANCE.md) records the conditional fresh-error propagation finding and its limitations.
- Experimental precision-margin profile: `experimental-s116-d56-b58-v1`, exposed separately through `CreateExperimentalPrecision116Setup()`. [Run 34051183115](https://github.com/leemaple/20231788./actions/runs/34051183115), exact source `2759fa90840946ef42957c7ba71ebea47e0e4995`, passed the existing **60 unique regression tests**, five explicitly built API contracts, and one new structural/integration test on **each** host. The new test took 10.85 s on Linux / 11.75 s on Windows. It exercises encryption, DCP and one Mult2 but **does not decrypt/compare the result against a numerical oracle**. See [execution evidence](coordination/precision116-pro-return-01/HOSTED_EXECUTION.md) and [independent runtime review](coordination/precision116-pro-return-01/FINAL_RUNTIME_REVIEW.md).
- The experimental profile changes the original scale/modulus parameters; it is **not an exact Table 3 parameter replication**. Its eight-square numerical test now **PASSES on Linux and Windows** at exact source `2b8b349edf5575556347082c1b725f6696c743b6`, [run34055816234/attempt1](https://github.com/leemaple/20231788./actions/runs/34055816234). Final maximum component errors are approximately `2.59e-26` and `3.48e-26`, below the unchanged `2^-80` limit (`8.27e-25`). See the [qualified correctness acceptance](coordination/precision116-eight-square-return-01/ACCEPTANCE.md) and [independent runtime review](coordination/precision116-eight-square-return-01/RUNTIME_REVIEW.md). Security remains **UNRESOLVED**; it must not inherit a 128-bit claim.
- Final default delivery regression: [run34057018442/attempt1](https://github.com/leemaple/20231788./actions/runs/34057018442), source`1552ebe04ba1406e6f735bd7eeb19413aca3f740`, passed60/60 unique regressions and the five explicit API builds on each host. All archived numerical/endpoint steps were skipped as intended. No implementation/test task remains pending in the [accepted correctness scope](coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md); no extra trial is scheduled merely to repeat passing evidence.
- External collaboration: ChatGPT Pro supplies design/code proposals; Codex verifies and integrates; ZCode provides independent review when the shared quota permits. Escalation uses terminal Fable **5.1** only when its actual model identity is verified. An unavailable reviewer is not a reason to stop other authorized work. Long-running Pro tasks are not interrupted or resubmitted.
- Continuity: the daily **07:30 Asia/Shanghai** PDF report to Telegram Saved Messages remains scheduled. Completed engineering and experiments should not be restarted by stale historical checkpoints. Historical quota and conversation entries below are timestamped evidence, not current availability claims.
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

## Test entry points and execution limits

Use pristine OpenFHE **1.5.0**, commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`, native64/backend4. The [hosted workflow](.github/workflows/dcp-rcb.yml) records dependency setup, compiler versions, exact source provenance and commands for Ubuntu/GCC and Windows/MINGW64. Put sustained compilation and cryptographic tests on those hosts, not the shared Mac.

The explicitly built `paper_full_eight_square_contract_test` target currently registers two distinct CTest entry points:

| CTest name | What it runs | Current evidence |
| --- | --- | --- |
| `paper_full_eight_square_contract` | Original-profile eight-square numerical contract | Actual Linux/Windows numerical FAIL; retain it unchanged |
| `experimental_precision116_profile_seam` | Experimental-profile single-operation structural/integration contract | Actual Linux/Windows PASS; no numerical-result comparison |

Both have a 1200-second CTest limit, serial execution and `OMP_NUM_THREADS=2`. The target is `EXCLUDE_FROM_ALL`: a default build alone does not build it. An unrestricted `ctest` is not the accepted 60-test regression selection and may invoke the expensive original experiment. Follow the exact workflow exclusions and anchored named selection instead.

The new target `experimental_precision116_eight_square_test` and CTest `experimental_precision116_eight_square_contract` are registered only when `OPENFHE_2023_1788_ENABLE_EXPERIMENTAL_PRECISION116_EIGHT_SQUARE=ON` (default OFF). The reviewed workflow uses a separate option-enabled build after the unchanged legacy suites, and selects this new test once on the dedicated observation ref. It also has a 1200-second CTest limit, serial execution and OMP2. See the [final integration review](coordination/precision116-eight-square-return-01/FINAL_INTEGRATION_REVIEW.md).

Do not manually dispatch the working branch or rerun an observation as a shortcut: the new full-eight steps are guarded by the exact observation ref, while other refs may select the old endpoint path. No rerun is necessary merely to reproduce the already retained single-operation result.
