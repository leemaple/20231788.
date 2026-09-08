# 2023/1788 双精度乘法：实现与复现说明

## 当前状态限定（COMPLETION-CONTRACT-01，2026-09-09）

本页以下操作属于历史 S116 快照。当前供件 a7f54de 的验收合同见同批 `COMPLETION_CONTRACT.zh-CN.md` 及更新后的交付指南。原 S100 Linux／Windows E80 均 FAIL；S116 两平台 PASS 与 annulus 单 Linux PASS 不得替代它。当前编码 p 已获严格 Ecd 证书，但不是旧密文重新通过。算法有条件正确性判断、原冻结数值验收、表3原作者统计及安全性分别报告；总体冻结数值项尚未全通过。作者实验设置未知不是所有工程判断的自动门槛。没有本轮新增编译或密文运行。


本页保留的是 S116 历史固定快照的复现方法，不是最新分支检查指南。查看最新 GitHub 入口、S100 诊断增量及当前交付限制，请先读 [交付与自行检查指南](CHECK_AND_HANDOFF.zh-CN.md)。下文双平台八平方 PASS 均指实验 S116，不指原 S100。

## 做成了什么

这里实现的是论文的 **t=2 Double-CKKS 方法**，基于官方 OpenFHE 1.5.0。它用高、低两部分密文表示一个数，在加密状态下完成乘法，再合成为普通密文；不是简单地把程序里的 `double` 换成更长的浮点类型。

已实现的主要步骤是：DCP 分解、Tensor2 乘法展开、Relin2 重线性化、RS2 缩减模数、Mult2 组合乘法、RCB 重组合，以及配套的 Add/Sub。客户端负责高精度输入、加密和最终解密；计算端只有公开参数、评估密钥和密文，不拿私钥，不在中间解密或刷新。

最重要的验证：同一条包含 16384 个复数槽位的加密输入，连续平方八次，得到每个原始值的 256 次方。最后把解密结果和独立高精度明文计算比较，检查全部槽位的实部、虚部误差。在另行命名的 S116 改参实验中，Linux、Windows 均通过该实验沿用的绝对分量误差上限 `2^-80 ≈ 8.27×10^-25`。

| 平台 | 最大最终分量误差 | 占误差上限 | 八次平方验证程序耗时 |
| --- | --- | --- | --- |
| Linux | 约 2.59×10^-26 | 约 1/32 | 20.54 秒 |
| Windows | 约 3.48×10^-26 | 约 1/24 | 21.89 秒 |

耗时包含验证过程，不能当作单次乘法性能。完整原始日志、源码身份、独立审核和限制见[验收说明](coordination/precision116-eight-square-return-01/ACCEPTANCE.md)。本轮没有千次实验：每个平台一条完整加密链，另有原来的 60 项回归和编译接口检查。

## 必须分清的限制

通过的是另行命名的实验参数 `experimental-s116-d56-b58-v1`：初始逻辑缩放为 `2^116`，调整了三个素数。原论文表格参数的那次执行未达到我们的误差门槛，失败记录仍保留。因此这是**论文算法在调整参数后的正确性实现与验证，不是原表格参数的完全成功复刻**。

目前验证了 N=32768、稀疏 h=128 根私钥、固定原始输入族、八次无刷新平方和相应参数族。不能把这一结果推广为任意输入、任意深度或所有随机密钥均满足同样误差。中间各轮检查十个独立锚点，不能据此声称已证明所有中间槽位不发生环绕。模数暴露约 712 bits，**安全级别尚未认证，不能声称 128-bit 安全或直接用于生产保护敏感数据**。

## 不重跑也能核查结果

- 科学验证：[run 34055816234，attempt 1](https://github.com/leemaple/20231788./actions/runs/34055816234)，源码 `2b8b349edf5575556347082c1b725f6696c743b6`。
- 交付源码：`1552ebe04ba1406e6f735bd7eeb19413aca3f740`。它的 `src/`、`include/`、`tests/`、CMake 与上述科学验证源码完全一致；后续差别是验收文档和默认分支的 CI 触发条件。
- [双平台原始日志及哈希](coordination/precision116-eight-square-return-01/HOSTED_EXECUTION.md)、[独立运行复核](coordination/precision116-eight-square-return-01/RUNTIME_REVIEW.md)。两次随机密钥/噪声抽样不同，复跑不应期待误差小数逐位相同。

## 确实需要重新执行时

在 Linux 或 Windows 专用环境运行，不在共享 Mac 上重编译。使用新目录和官方原版依赖，绝不复用以前的相关实现或改动版 OpenFHE。

要求 C++17、CMake、Boost 头文件；Windows 使用 MINGW64，而不是混用 MSVC/UCRT64 库。已经验证的工具链是 Linux GCC 13.3.0/Boost 1.83、Windows MINGW64 GCC 16.2.0/Boost 1.92。其他组合不在本轮实测声明内。

先从官方 `openfheorg/openfhe-development` 的提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`（1.5.0）及其递归子模块构建全新依赖。配置必须包含 `NATIVE_SIZE=64`、`MATHBACKEND=4`、`WITH_OPENMP=ON`，不能以版本号相同代替源码身份。完整依赖配置、安装和 Windows 路径转换命令已保存在[实际 CI 工作流](.github/workflows/dcp-rcb.yml)与原始日志中。

以下 Bash 命令用于**已安装上述全新依赖**的 Linux/MINGW64 环境。把路径占位符替换为真实安装目录；Windows 路径使用 MINGW64 的 `/c/...` 形式。仓库网址的两个点是有意的：一个属于仓库名称，另一个属于 `.git` 后缀。

```bash
git clone https://github.com/leemaple/20231788..git 20231788-cleanroom
cd 20231788-cleanroom
git checkout --detach 1552ebe04ba1406e6f735bd7eeb19413aca3f740
export TASK_OPENFHE_PREFIX=/absolute/path/to/pristine-openfhe-1.5.0
export PATH="$TASK_OPENFHE_PREFIX/bin:$TASK_OPENFHE_PREFIX/lib:$PATH"
cmake -S . -B build-eight-square \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$TASK_OPENFHE_PREFIX" \
  -DOPENFHE_2023_1788_ENABLE_EXPERIMENTAL_PRECISION116_EIGHT_SQUARE=ON
cmake --build build-eight-square \
  --target experimental_precision116_eight_square_test --parallel 2
OMP_NUM_THREADS=2 ctest --test-dir build-eight-square \
  --verbose --output-on-failure --no-tests=error \
  -R '^experimental_precision116_eight_square_contract$'
```

这些命令对应已经执行的配置/目标/测试选择；本说明未另行启动实验。测试上限 1200 秒。不要不加筛选直接运行 `ctest`：旧参数实验是独立记录，且部分目标默认不编译。正常默认分支 CI 运行常规回归，不自动重复两种昂贵实验；因此默认分支出现绿勾不能代替上述科学验证记录。

判断一次执行时，必须同时看 `event=COMPLETE`、`result=PASS`、`numeric_gate_failures=0`、`cleanup=PASS` 和 CTest 通过。`ABORT`、编译失败、超时或 oracle 无效不能当作“精度失败”，更不能当作成功。有限数值 FAIL 需要定位原因，不通过换密钥反复重跑挑选成功结果。

## 接入代码时从哪里看

库目标是 `openfhe_2023_1788`。公开接口在[double_ckks.h](include/openfhe_2023_1788/double_ckks.h)、[repeated_mult2.h](include/openfhe_2023_1788/repeated_mult2.h) 和[high_precision_client_io.h](include/openfhe_2023_1788/high_precision_client_io.h)。

最完整的已编译示例就是[八次平方测试](tests/experimental_precision116_eight_square_test.cpp)：`RunCandidate` 展示客户端的 setup/Encrypt/BindRepeatedRcb/Decrypt；`Evaluate` 展示仅接收公开 plan 和密文的 DCP → 八次 Mult2 → RCBWithReceipt。独立 oracle 代码只属于测试客户端，不应复制到计算端。

高精度输入使用 `ClientReal` 和精确有理缩放，不先转成机器 `double`；否则细小差异会在加密前丢失。不要自行修改公开 plan 返回的 OpenFHE context、密钥标签、塔顺序或缩放元数据。参数路由与精确尺度变化由实现维护，错误状态应直接暴露，而不是捕获后继续计算。
