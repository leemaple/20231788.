# Mult2 clean-room review candidate

本交付实现论文 2023/1788 定义 4.7 的首次双分量乘法：

```text
Mult2(left, right) = RS2(Relin2(Tensor2(left, right)))
```

对象是附件中 `project/` 的精确源码快照。生产代码只增加一个公开 `const` API 声明和一个三算子组合实现；没有改动 DCP、Tensor2、Relin2、RS2 或 RCB 的算法体，没有引入 Add/Sub、自动刷新、第二次乘法、泛型回调、序列化、包装兼容层或 OpenFHE 分叉。

## 交付结论

| 门禁 | 状态 | 说明 |
|---|---|---|
| 输入 ZIP 大小与 SHA-256 | PASS（已执行） | 1,576,818 bytes；`5b83855378345120e65a4495eb034b2c2e5d406b1c6b03053491f1929bde7f81` |
| ZIP 成员与 SOURCE-MANIFEST | PASS（已执行） | 43 个成员、31 个普通文件；清单列出的 30 个对象全部逐字节匹配，无缺失、额外普通文件或哈希偏差 |
| 补丁顺序与干净重放 | PASS（已执行） | 8 个补丁按 `patches/series` 重放；最终文件逐字节一致，Git tree 均为 `66bad80110c866a4616194a74eb3b6fa55b096af` |
| 生产改动范围 | PASS（已执行的静态审计） | 仅 2 个生产文件、5 行新增；组合式唯一；无新增生产 `try/catch` |
| CMake/CTest 图与 workflow 结构 | PASS（配置图/静态审计） | 合成包元数据下声明 49 个 CTest；Linux/MinGW 的 4 个显式 API 目标和分支触发均已接入 |
| 候选代码真实编译、链接、CTest | **NOT EXECUTED** | 当前 Linux 容器没有 OpenFHE 1.5.0 安装，隔离获取依赖又因 DNS 失败；不得将静态检查解释成编译通过 |
| Linux GCC / Windows MinGW64 候选 CI | **NOT RUN / UNVERIFIED** | 包内绿色日志只证明旧基线 39 项测试，不覆盖本次 Mult2 候选 |
| 定理 4.8 通用上界 | **UNPROVED** | 测试公开单次执行的经验 `E_Relin` 证书；没有把测量值冒充保守、通用的 `E_Relin` |

因此，本包是**可重放、待独立集成和执行的审查候选**，不是已通过 CI 的成品，也不构成生产安全或精度声明。

## 目录

```text
README.md                         本文件；交付入口与精确应用顺序
SOURCE_FACT_AUDIT.md              论文、OpenFHE、基线与候选源码事实审计
THEOREM_SCALE_DECISION.md         定理 4.8 分歧、反例、双尺度与解码结论
EXECUTION_STATUS.md               已执行、未执行和未验证证据边界
UPSTREAM_ASSESSMENT.md            是否需要改上游算子/公共契约
INPUT_OUTPUT_HASHES.md            输入、补丁、源码树与交付身份
patches/                          8 个 `git format-patch` 补丁和 `series`
final-project/                    应用全部补丁后的完整项目文件
verification/                     重放、差异、CMake 图、workflow 与环境记录
source-records/                   本轮 TASK.md 与 SOURCE-MANIFEST.json 的原样副本
verify_delivery.py                交付内文件大小/SHA-256/补丁序列校验器
SHA256SUMS                        除自身和 DELIVERY-MANIFEST.json 外的文件摘要
DELIVERY-MANIFEST.json            除自身外的全文件清单
```

## 精确补丁顺序

补丁必须按 `patches/series` 顺序应用，不能压缩或交换红/绿顺序。

| # | 补丁 | 作用与预期状态 |
|---:|---|---|
| 1 | `0001-test-red-add-missing-Mult2-public-API-contract.patch` | **API compile-red**：编译契约引用精确 `const` 成员指针，但基线类尚无 `Mult2`。该红态已静态保留；本环境未真实编译。 |
| 2 | `0002-feat-green-scaffold-const-Mult2-API.patch` | **API scaffold-green**：加入精确声明与显式 `logic_error` 桩，使 API 面完整而行为仍未实现。编译状态本环境未执行。 |
| 3 | `0003-test-red-require-first-Mult2-end-to-end-composition.patch` | **behavior-red**：加入真实加密→DCP→Mult2→RCB→解密路径，并在实现前冻结逻辑尺度校正后的绝对误差阈值 `1e-3`；合理预期在 Mult2 桩处运行失败，而非坏夹具/构建错误。本环境未运行。 |
| 4 | `0004-feat-green-compose-Mult2-from-Tensor2-Relin2-and-RS2.patch` | **minimal behavior-green**：仅以 `return RS2(Relin2(Tensor2(left, right)));` 替换桩，不复制任何算法。 |
| 5 | `0005-test-add-independent-Mult2-end-to-end-arithmetic-oracle.patch` | 独立 `cpp_int` CRT、中心化系数解密、学校式负循环卷积、HYBRID/BV×REAL/COMPLEX 四条端到端证据路径。 |
| 6 | `0006-test-harden-Mult2-validation-and-terminal-fail-fast.patch` | 终态输入在清除评估密钥后必须先因生命周期失败；另测 context/key/scale/basis 失配及输入/缓存不变。 |
| 7 | `0007-test-make-Mult2-output-oracle-independent-of-RCB.patch` | 系数 oracle 直接独立解密并重组公开结果 pair，不再把生产 RCB 当作系数期望；RCB 仅保留在必需的公开槽位解码路径。 |
| 8 | `0008-ci-exercise-Mult2-branch-and-all-public-API-targets.patch` | 单独加入 `codex/mult2-01` 触发和 Linux/MinGW 四个显式 API 目标。 |

本地审查历史中的提交 ID 仅是本次构造账本；用户应用 `git am` 后的 commit ID 会受 committer 时间影响，**应以最终 Git tree 和文件哈希判等**。

## 应用补丁

在包含真实 Git 对象的用户仓库中，从任务指定基线开始：

```bash
git switch -c codex/mult2-01 a3a6a171b60c4829f674f0dc4f5b35d658d47868
DELIVERY=/absolute/path/to/mult2-pro-a3a6a17-delivery
while IFS= read -r patch; do
  git am "$DELIVERY/patches/$patch"
done < "$DELIVERY/patches/series"
```

应用后应得到与 `final-project/` 相同的文件内容。可用以下方式比较树内容；不要要求 commit ID 相同：

```bash
git status --short
git write-tree
# 期望最终 tree：66bad80110c866a4616194a74eb3b6fa55b096af
```

## 必须执行的构建与测试命令

`<pristine-install>` 必须指向从 OpenFHE 1.5.0 固定提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504` 构建安装的未修改前缀：

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=<pristine-install>
cmake --build build --parallel 2
cmake --build build --target tensor2_api_contract_test --parallel 2
cmake --build build --target relin2_api_contract_test --parallel 2
cmake --build build --target rs2_api_contract_test --parallel 2
cmake --build build --target mult2_api_contract_test --parallel 2
ctest --test-dir build --output-on-failure
```

项目 CMake 对非 MSVC 工具链启用 `-Wall -Wextra -Wpedantic -Werror`。workflow 已为 Ubuntu GCC 和 Windows MSYS2 MinGW64 保留相同告警门禁。

## 新增的 10 个 CTest

```text
mult2_composition_contract
mult2_e2e_hybrid_real
mult2_e2e_hybrid_complex
mult2_e2e_bv_real
mult2_e2e_bv_complex
mult2_terminal_before_key
mult2_mismatched_context
mult2_mismatched_key
mult2_tampered_scale
mult2_tampered_basis
```

端到端 oracle 固定记录：`N=64`、batch `16`、`p=30`、first modulus `35` bits、depth `7`、输入 degree `2`、level `0`、`UNIFORM_TERNARY`、`maxRelinSkDeg=2`；`HEStd_NotSet` 只用于功能夹具，不代表生产安全。REAL/COMPLEX 与 HYBRID/BV 均有独立进程用例。

## 交付自校验

在解压目录内运行：

```bash
python3 verify_delivery.py
```

成功时会同时验证 `DELIVERY-MANIFEST.json`、`SHA256SUMS`、`patches/series`、补丁数量和所有文件字节；它不编译 C++，也不验证定理。
