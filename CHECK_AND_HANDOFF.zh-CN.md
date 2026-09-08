# 交付与自行检查指南（2026-09-08 更新）

## 最新增补：请先看这里

用户在 9 月 8 日重新授权全面复核和必要的远端实验，取代下文 9 月 7 日“暂不继续”的工程停止安排；不联系作者的决定仍有效。

**最新分支是 [codex/s100-annulus125-20260908](https://github.com/leemaple/20231788./tree/codex/s100-annulus125-20260908)**，不等于仓库默认分支。9 月 8 日新增的独立命名 S100 输入实验已通过，生产乘法代码未改变，原近单位圆 S100 失败仍保留。请读 [最新实验说明与轻量检查命令](coordination/s100-annulus125-20260908/RESULT.zh-CN.md)。实际测试源码固定为 `03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b`，后续文档／证据提交不改变该运行身份。

13:08 更新：网页版 Pro 最高可见思考档的独立终审已结束，页面显示用时 48 分 44 秒。完整交付已下载、扫描、保存，Codex 又用独立脚本重算全部 16,384 槽（约 1.23 秒，不是新增加密实验），与 Pro 结果一致。新样本误差约 `1.46×10^-25`，约 **82.5 位绝对精度、73.3 位相对精度**；“误差低于 2^-80”不等于“保留 80 位有效数字”。[终审结论与核验记录](coordination/annulus-independent-pro-review-20260908/RETURN_DISPOSITION.md) 明确保留旧 S100 失败，并未找到支持继续修改生产乘法的新缺陷。表 3 的同源输入、噪声和统计／计时口径仍缺材料，不能声称完整复现或承诺确切完成时间。无需为了这次终审再跑 Windows／GitHub 实验。

下面正文保留为 **9 月 7 日历史交付说明**；其中“最新”“停止”和旧分支指当时，不是新的运行指令。当前最省事的核验是检查已保存数据，不需要重新派发实验。

## 先说结论

论文的 t=2 Double-CKKS 算法已基于官方 OpenFHE 实现，完整加密计算流程已经实际跑通。目前测试和独立审核没有定位出尚未处理的明确生产代码缺陷，**可以保留为当前工程版本，暂不继续修改**。这不等于证明代码绝对无错，也不等于论文表 3 已完全复现。

按用户最新决定：不联系作者，不追加实验，不再围绕外联授权反复询问。保留已有代码和全部成功／失败证据。原 S100 精度不足仍如实记录，不用调整参数后的成功替换它。

## 1. GitHub 上哪个才是最新代码？

仓库名最后确实有一个英文点：`leemaple/20231788.`。

- **本次交付分支（请直接打开）：** [codex/s100-fresh-error-repair-20260907](https://github.com/leemaple/20231788./tree/codex/s100-fresh-error-repair-20260907)。本指南也在这个分支。
- 本指南编写前核对的代码与证据快照：[`3f71e0dca78e56899d2b8df98e716a81a761ccb8`](https://github.com/leemaple/20231788./tree/3f71e0dca78e56899d2b8df98e716a81a761ccb8)。后续本次交付提交只更新说明文档，不改代码、测试或 CI。
- 最新实际 CI 验证的源码：`223667e82b67c4758a56bd745f264110dd3b9619`。上述快照与它的 `src/`、`include/`、`tests/`、`CMakeLists.txt`、`.github/workflows/` 一致。
- 核验时 GitHub **默认分支**是 `cleanroom/reimplement-mult2-20260831`，停在 `e6c4cc1ddfa261ee17681cbfc1ca6023660df5cb`。它有之前交付的实现，但没有这一轮全部增量。**直接看仓库首页可能看到旧版；“已经推送”不等于“已经合并到默认分支”。** 本次没有合并、改默认分支或覆盖历史。

网页下载时，先确认左上方分支名正确，再点 **Code → Download ZIP**。使用 Git 则在一个全新、不存在的目录中运行：

```bash
git clone --branch codex/s100-fresh-error-repair-20260907 --single-branch https://github.com/leemaple/20231788..git 20231788-check-20260907
cd 20231788-check-20260907
git status --short --branch
git rev-parse HEAD
```

网址中的 `20231788..git` 有两个点是正确的。不要在旧实现目录上覆盖或执行 `reset --hard`。如果要检查固定快照，可在这个新克隆目录运行：

```bash
git checkout --detach 3f71e0dca78e56899d2b8df98e716a81a761ccb8
```

detached HEAD 只表示固定查看一个提交，不是报错；普通浏览和检查不需要新建开发分支。

## 2. 最省事的检查：先看已经完成的测试，不必重跑

| 要确认什么 | 打开哪里 | 实际结果与边界 |
| --- | --- | --- |
| 最新增量没有破坏既有功能，日志失败不会被当作成功 | [最新 CI：34116227668](https://github.com/leemaple/20231788./actions/runs/34116227668) | Linux 60/60 回归通过；控制测试 1/1 通过，13.64 秒。包括正常输出、刷新失败拒绝、槽序置换反例。Windows 本轮跳过，不是本轮双平台通过。 |
| 原参数下完整流程是否跑过 | [原 S100：34039088536](https://github.com/leemaple/20231788./actions/runs/34039088536) | Linux、Windows 均完成八次平方，但最终误差约 9.15×10^-24／9.07×10^-24，超过 2^-80≈8.27×10^-25。这个红色结果是保留的数值失败，不要删除。 |
| 调整参数后的方法是否实际有效 | [实验 S116：34055816234](https://github.com/leemaple/20231788./actions/runs/34055816234) | 两平台八次平方均通过，误差约 2.59×10^-26／3.48×10^-26。参数改变，因此不能叫作原表 3 参数成功复现。 |
| 初始误差主要来自哪里 | [S100 单样本诊断：34110943783](https://github.com/leemaple/20231788./actions/runs/34110943783) | 该样本中公钥加密聚合误差主导；不是新一次完整八平方精度通过，也不是总体噪声分布认证。 |

打开最新 CI 后，检查页面提交是 `223667e`，展开 `linux-gcc`：

1. `Run complete 60-test three-track suite` 应显示 `100% tests passed, 0 tests failed out of 60`。
2. `Run S100 encoding inspection contract once` 应显示 `output_stream_controls`、`observer_order_control=COMPLETE`、`status=COMPLETE mode=controls`，随后 CTest 通过。
3. `Run S100 fresh-error diagnostic once`、旧完整端点步骤和 `windows-mingw64` 为 **Skipped**，是本次有意限定范围，不是漏报成功。
4. 同时看源码提交、日志末尾和任务最终状态。单独一个 `COMPLETE` 字样或绿色徽章，不能证明论文精度通过。

最新一次 CI 的 Linux 总耗时为 5 分 14 秒，包括构建和测试，不是算法性能。结果和完整相关步骤日志已存入 [GREEN_RESULT.md](coordination/s100-output-finalization-01/GREEN_RESULT.md) 与 [GREEN_EVIDENCE.txt](coordination/s100-output-finalization-01/GREEN_EVIDENCE.txt)，避免只能依赖 GitHub 日志的保留期。

## 3. 如果你想自己重跑一次（可选，不要求）

推荐 GitHub Actions，不占用本机的编译资源；需要有仓库 Actions 的执行权限。

1. 打开 [OpenFHE 2023/1788 TDD 工作流](https://github.com/leemaple/20231788./actions/workflows/dcp-rcb.yml)。
2. 点击 **Run workflow**，分支明确选择 `codex/s100-fresh-error-repair-20260907`。
3. 把 `s100_scope` 设为 **controls-only**（默认值是 `fresh`，不要漏改）。
4. 只提交一次，等它完成，按上一节检查结果和实际 source SHA。新一次运行会绑定你点击时的分支提交，不会继续显示旧的 `223667e`。

已安装并登录 GitHub CLI 的用户也可使用下面这一种方式，**不要网页和命令各提交一次**：

```bash
gh workflow run dcp-rcb.yml --repo leemaple/20231788. --ref codex/s100-fresh-error-repair-20260907 -f s100_scope=controls-only
```

这会运行常规 60 项回归和专门控制测试；不新增 fresh 诊断样本、不重跑原 S100 或 S116 八平方。不能把这个绿勾解释成原 S100 已达标。不选历史 RED 分支，不在不认识的分支上直接 dispatch，不反复换随机密钥试到通过。

如确有需要在专用 Linux／Windows 环境检查 S116 的历史完整数值结果，参照 [原有详细复现说明](REPRODUCE.zh-CN.md) 中的固定提交、官方依赖与命名测试。那份说明的旧提交是为了重现其对应实验，不是当前分支最新源码。本次未执行新的重跑，也未验证其他编译器组合。

## 4. 看代码时的入口

论文方法不是把 C++ `double` 改成更长的浮点数，而是让一对高、低部分密文一起承载更高精度：客户端加密 → DCP 分解 → 八次 Mult2（内部 Tensor2、Relin2、RS2）→ RCB 合并 → 客户端解密，与独立高精度明文答案比较。计算端不持有私钥，不靠中间解密来计算。

- 算法接口与实现：[double_ckks.h](include/openfhe_2023_1788/double_ckks.h)、[double_ckks.cpp](src/double_ckks.cpp)。
- 输入编码、加解密：[high_precision_client_io.h](include/openfhe_2023_1788/high_precision_client_io.h)、[high_precision_client_io.cpp](src/high_precision_client_io.cpp)。
- 多轮计算与参数入口：[repeated_mult2.h](include/openfhe_2023_1788/repeated_mult2.h)、[repeated_mult2.cpp](src/repeated_mult2.cpp)。
- 完整使用示例：[experimental_precision116_eight_square_test.cpp](tests/experimental_precision116_eight_square_test.cpp)。先看 `RunCandidate` 的客户端，再看 `Evaluate` 的计算端；测试 oracle 不是服务端代码。
- 本轮检查工具：[s100_fresh_error_diagnostic_test.cpp](tests/s100_fresh_error_diagnostic_test.cpp)。用于拆分编码／加密／解码误差，不是乘法算法本身。

有高精度输入时不要先转成机器 `double`；不要把测试私钥或真实误差注入计算端。官方依赖固定 OpenFHE 1.5.0 提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`、native64/backend4。不要复用旧实现或本地改动版依赖，也不要无筛选地运行 `ctest`，它可能选中默认不构建的昂贵实验目标。

## 5. 怎样判断“先这样可以了”

可以采用的表述是：**“论文算法已实现，现有测试和审核未发现明确未解决的实现缺陷；调整参数后有双平台完整数值通过证据，原 S100 精度限制仍保留。”**

不应写成“代码已证明完全正确”“表 3 已完整复现”“所有输入／密钥都满足同一精度”或“已达到 128-bit 安全”。目前不适合直接作为保护真实敏感数据的生产密码库；尤其 S116 的安全强度尚未认证。

最新 [科学审核处置](coordination/s100-condition-decision-01/RETURN_DISPOSITION.md) 解释了为什么目前不再盲改乘法：已有样本的起始误差经过理想八平方仍会使部分分量超限，提高后续计算精度本身不保证消除它；这并不证明所有算法不可能成功。论文原始输入和噪声等条件未充分公开。遵照用户决定，到此保留版本，不外联、不继续追逐同一精度数字。历史台账中的“待授权问询”是当时状态，不再是当前待办。

文档校验记录：12 个本地文件链接存在，3 个 Bash 代码块通过 `bash -n` 语法检查；未执行其中的克隆／测试／派发命令。已检查上述两份源码快照之间的运行相关文件差异为空，并实时核对远程分支提交及最新 CI 终态。独立只读文档审核未发现实质问题；文档审核不替代数值测试。
