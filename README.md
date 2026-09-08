# OpenFHE 2023/1788 clean-room implementation

This branch is a greenfield implementation of the `t=2` Double-CKKS multiplication method from IACR ePrint 2023/1788 for official pristine OpenFHE 1.5.0.

**新增参考文档任务（2026-09-08，进行中）：** 本分支 `codex/parameter-atlas-20260908` 专门研究 CKKS 参数、全部相关随机数路径、论文步骤与改动联动。完整主文档由网页版 Pro 最高可见思考档编写，尚未返回；[任务与当前状态](coordination/parameter-atlas-20260908/STATUS.md)记录了可恢复的同一对话。已有[项目参数映射](coordination/parameter-atlas-20260908/PROJECT_PARAMETER_MAP.md)、[随机性源码核对](coordination/parameter-atlas-20260908/ROOT_RANDOMNESS_MAP.md)、[论文原页核对](coordination/parameter-atlas-20260908/ROOT_PAPER_CROSSCHECK.md)和[实际检查记录](coordination/parameter-atlas-20260908/ROOT_EXECUTION_LEDGER.md)，均是审查材料，不能冒充已完成的主文档。运行源码仍与 `a4b815a733efe81897325e2a8e4c826a4ebfa439` 相同，没有启动新的加密实验；先完成文档，再选择诊断。下方已完成的外部审查指历史 annulus 任务，不是本次文档任务。

**最新交付与用户检查入口（2026-09-08）：[中文检查指南](CHECK_AND_HANDOFF.zh-CN.md)。** 最新增量在 `codex/s100-annulus125-20260908` 分支，不等于仓库默认分支。用户已重新授权全面复核及必要远端实验；不联系作者的决定仍有效。一次独立命名、调整输入范围的 S100 完整八平方实验通过，生产乘法代码未因此修改；原近单位圆 S100 精度 FAIL 保留，不能声称论文表 3 已完整复现。

最新实测与边界：[S100 单次条件输入实验说明](coordination/s100-annulus125-20260908/RESULT.zh-CN.md)。此前 S116 固定快照说明：[实现做了什么、复现命令、结果与限制](REPRODUCE.zh-CN.md)。下方历史 delivery/default-branch 记录描述上一轮 S116 交付，不能覆盖最新指南的分支、源码和验收边界。

The destination repository's previous implementation and every related local code tree are quarantined and are not inputs. Development begins with paper-derived specifications and red-first independent-oracle tests.

## Live project state

- Current scientific source: `03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b`, [single Linux run34184869227](https://github.com/leemaple/20231788./actions/runs/34184869227). One public-key payload, eight squares, all16384 complex slots; final maximum complex-modulus error `1.462314103884e-25`, below `2^-80`. This holds for one predeclared annulus125 input vector and one actual key/noise sample, not all S100 inputs or keys. Raw data and process/scale/hash evidence are committed under `coordination/s100-annulus125-20260908/experiment-evidence/`; the [independent result review](coordination/s100-annulus125-20260908/INDEPENDENT_RESULT_REVIEW.md) records limits.
- Original near-unit S100 remains FAIL; S116's earlier two-platform PASS remains a separately changed-parameter result. Exact Table3 provenance/statistics/performance and deployment security remain unresolved. The full reproduction objective is not complete. No repeated sample or restored high-frequency engineering timer follows this PASS.
- Independent browser Pro final review is complete (48m44s displayed); [root disposition and retained return](coordination/annulus-independent-pro-review-20260908/RETURN_DISPOSITION.md) include independent scalar replay and a separate mathematical review. The new sample has approximately82.50 absolute bits but73.30 worst relative bits. No new production arithmetic repair is justified by this review. [Active coordination checkpoint](coordination/s100-fresh-error-repair-01/STATUS.md) records the unfinished provenance/security boundary; no external review is still thinking.

## Historical S116 delivery state (2026-09-07)

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

The additional `s100_annulus125_eight_square_test` target is optional (`OPENFHE_2023_1788_ENABLE_S100_ANNULUS125=ON`, default OFF). Its one predeclared live run is already complete. Do not rerun, move, or recreate `s100-annulus125-once-20260908`; the user check guide provides a pure scalar replay of committed data without another encrypted sample. The older test entries below retain their distinct historical scopes.

Use pristine OpenFHE **1.5.0**, commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`, native64/backend4. The [hosted workflow](.github/workflows/dcp-rcb.yml) records dependency setup, compiler versions, exact source provenance and commands for Ubuntu/GCC and Windows/MINGW64. Put sustained compilation and cryptographic tests on those hosts, not the shared Mac.

The explicitly built `paper_full_eight_square_contract_test` target currently registers two distinct CTest entry points:

| CTest name | What it runs | Current evidence |
| --- | --- | --- |
| `paper_full_eight_square_contract` | Original-profile eight-square numerical contract | Actual Linux/Windows numerical FAIL; retain it unchanged |
| `experimental_precision116_profile_seam` | Experimental-profile single-operation structural/integration contract | Actual Linux/Windows PASS; no numerical-result comparison |

Both have a 1200-second CTest limit, serial execution and `OMP_NUM_THREADS=2`. The target is `EXCLUDE_FROM_ALL`: a default build alone does not build it. An unrestricted `ctest` is not the accepted 60-test regression selection and may invoke the expensive original experiment. Follow the exact workflow exclusions and anchored named selection instead.

The new target `experimental_precision116_eight_square_test` and CTest `experimental_precision116_eight_square_contract` are registered only when `OPENFHE_2023_1788_ENABLE_EXPERIMENTAL_PRECISION116_EIGHT_SQUARE=ON` (default OFF). The reviewed workflow uses a separate option-enabled build after the unchanged legacy suites, and selects this new test once on the dedicated observation ref. It also has a 1200-second CTest limit, serial execution and OMP2. See the [final integration review](coordination/precision116-eight-square-return-01/FINAL_INTEGRATION_REVIEW.md).

Do not manually dispatch the working branch or rerun an observation as a shortcut: the new full-eight steps are guarded by the exact observation ref, while other refs may select the old endpoint path. No rerun is necessary merely to reproduce the already retained single-operation result.
