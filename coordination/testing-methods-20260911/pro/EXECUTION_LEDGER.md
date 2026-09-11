# 执行账与未执行项

日期：2026-09-11。工作仅基于当前附件和指定的公开测试方法；没有调用其他对话、私人仓库、隔离区旧实现或外部 agent。以下是实际动作，不是待执行计划。

## 已执行

| 动作 | 实际结果与限制 |
|---|---|
| Files 内容检索 | 针对附件先后两次检索无结果；ZIP 未提供可用解析内容，随后使用开发者给定的已挂载路径做归档处理。未把“无结果”当成无内容。 |
| 输入 ZIP 与解包 | 2630139 bytes、486 普通文件、SHA-256 匹配；无重复文件名、路径越界或 symlink；CRC 检查通过。未运行解包代码。 |
| 输入根 manifest | 485 行的路径覆盖/bytes/SHA-256 全匹配；仅 MANIFEST 自排除；147 条顶层 origin.git_blob 的 SHA-1 重算匹配。官方文件均被根 SHA-256 行绑定，不夸称本轮重新从远端下载全部官方 blobs。 |
| 定向源码阅读 | 读取 root REVIEW_NOTES/reference、现有 API、关键初始化/变换/PKE 源码及相关测试/历史审计。不是声称逐行审阅 486 个文件。位置见 SOURCES.md。 |
| 论文 | 核验附件 PDF SHA；本地解析并渲染、目视核对第 4、12 页。在线 landing page 可读；PDF 抓取和 web screenshot 失败，未用网络文件替换附件。无 OCR。 |
| 指定 GitHub testing SKILL | 在线读取固定 commits 的方法性内容；不安装、不执行 skill，不接受其泛化试验量替代本 TASK。 |
| 标量与源码检查 | `tools/verify_static.py` 实际退出 0；仅 stdlib 哈希、CRC、整数、Decimal、AST/文本检查。结果为 STATIC_CHECKS.json。未执行直接求值/FFT/NTT，即使它们是测试 oracle。 |
| Patch 检查 | 对当前 CMake 的临时副本实际执行 git apply --check、git apply，再 cmp 完整 C++ 文件；均成功。验证 CMake 只在末尾追加，未改生产 src/include。结果为 PATCH_CHECKS.json。 |

可复核的标量命令（只是文件名不同也需指定实际 ZIP 路径）：

```bash
python -B -I tools/verify_static.py /path/to/testing-methods-33722b9.zip --output STATIC_CHECKS.json
```

本地实际命令使用 `/mnt/data/testing-methods-33722b9.zip`。标量检查计算的是固定公开根的几个模幂、q 的互素性、σ 的支持界、明文非绕回和两个偏移端点的不等式，**不是加解密，不是运行 NTT**。检查脚本并不是编译器，也不能证明 C++ 可编译。

实际 patch 检查的本地命令：

```bash
mkdir -p /mnt/data/initial-phase-patch-check/tests
cp /mnt/data/testing_methods_input/project/CMakeLists.txt /mnt/data/initial-phase-patch-check/
git -C /mnt/data/initial-phase-patch-check apply --check /mnt/data/testing-methods-diagnosis-20260911/initial-phase.patch
git -C /mnt/data/initial-phase-patch-check apply /mnt/data/testing-methods-diagnosis-20260911/initial-phase.patch
cmp /mnt/data/initial-phase-patch-check/tests/initial_phase_exact_contract_test.cpp /mnt/data/testing-methods-diagnosis-20260911/tests/initial_phase_exact_contract_test.cpp
```

## 明确未执行

没有 C++ 语法/链接测试、OpenFHE 编译、加密、解密、采样、NTT/FFT、eval-key 生成、Mult2、远程 CTest、GitHub dispatch、Mac 构建、负对照运行、种子扫描或基准测试。没有重新跑 ten-anchor ideal propagation checker 或 public coefficient certificate。没有新的 Gitleaks 扫描；输入的密钥扫描通过属于用户/供件声明，不能冒充本轮独立扫描结果。

C++ 的基线 PASS、两个负对照被拒绝、超时时间是否足够、Windows/MinGW 链接与具体动态库身份 **全部待 root 观察**。交付前的静态自查不是独立评审者的签字。测试预期日志均明确标为示意，不能入库为执行证据。

## 与历史证据隔离

原 S100 run34039088536/sourceed5fd192a89d6d4728ad295e87cf06a3f4abc832 的双平台 FAIL 保留。S116 run34055816234、annulus run34184869227、当前 p 证书 run34275429052 的结果使用所附审计和 TASK 的冻结事实，不称本轮现场重放。输入树没有修改；补丁只在单独临时副本验证。

## 输出身份

MANIFEST.json 为交付中每个其他普通文件记录 bytes/SHA-256，并明确自排除，以避免自哈希循环。MANIFEST 自身由最终 ZIP 校验和覆盖；ZIP 的 bytes/SHA-256 另在交付链接旁及外部 SHA256.txt 给出。完整纯文本 fallback 是对 ZIP 内文本文件的串接，保留文件边界，不是另一份运行证据。
