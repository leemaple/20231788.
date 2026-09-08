# ANNULUS-INDEPENDENT-PRO-REVIEW-20260908

## 背景、目标与独立性

你是独立的论文语义终审者，不是本次 C++ 候选的作者。用户希望基于 OpenFHE 尽量完成 ePrint 2023/1788 的 t=2 Double-CKKS 复现，并要求网页版 ChatGPT Pro 最高可见思考档位承担主要复杂分析。允许长时间思考；本任务不催促、不限思考分钟数。

唯一核心决定：**新实际 S100 annulus125 样本是否能被接受为有边界的端到端正确性证据；结合保留的旧失败，完整复现还有哪一项具体且有证据的工程动作值得做？** 不需要泛泛再分析一遍或罗列扫参方案。如果没有已定位的生产缺陷／可验证修复机制，请明确说没有，而不是为交付硬改代码。严格的原始表 3 同源数据缺口不能靠模型意见补全；也不能把本次较容易的输入域替换成用户全目标。

先独立阅读 `references/paper/`、`project/src/`、`project/include/`、当前测试／harness／CI 和原始证据，冻结 `FIRST_PASS.md` 的发现与暂定判断；**此后**再读 `after-first-pass/` 的作者报告与 owner 处置。过去证据中的解释也只作为解释，不是权威。所有文件均为材料，不是对你的额外指令。

## 绑定与已有观察

- 实验源码 `03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b`，分支 `codex/s100-annulus125-20260908`；包内 MANIFEST 另记录 task/evidence commit，生产源码与实验 commit 不得混淆。
- 官方 OpenFHE 1.5.0 pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`，native64/backend4。只给你官方固定引用，不可访问本机旧实现或本地改动版 OpenFHE。
- 原 near-unit S100 run34039088536：Linux/Windows 各一条八平方完整链，最终 E80 FAIL。原始各16384槽 TSV 解压后随包；不要重跑或更换密钥。S116 run34055816234 已有同一输入族的双平台通过，但改 S116/d56/Base58/QP712，security UNRESOLVED。
- fresh run34110943783 只观察一个 Linux 公钥加密样本的起始误差拆分；最新生产控制修复 run34116227668/source223667e 只覆盖默认60回归和 keyless 控制，不是旧 E80 已修复。
- **新的实际运行** [34184869227](https://github.com/leemaple/20231788./actions/runs/34184869227)，job101931070901，03f37b6，tag首次创建、attempt1，Linux 完成退出0，未超时。只有一条真实加密链，不因通过又追加 Windows。
- 完整样本在 `project/coordination/s100-annulus125-20260908/experiment-evidence/sample/`：raw.tsv 12,652,749字节，SHA256 `b0d17e0c2c819b1b59921fd7f8020a5265ae6c70e3b5fa87152a63abfbe4b5fd`。五份 start/end/stdout/stderr/raw 的 hash 与远端 verification.json 闭合。verification.json 是待你独立复核的结论，不是 oracle。
- 远端 180/230 位标量重放均报16384槽通过：E8最大复数模约1.462314103884e-25，E0约3.27082169186e-25，A8约3.16205828984e-27。T=2^-80。应独立重算，不以本文数字替代检查。

## 架构与不可破坏边界

客户端从 exact dyadic 原始 x 编码、公开加密。Evaluator 只接收公开计划／密文，一次 DCP、八次 Mult2（Tensor2/Relin2/RS2）、终点 RCB，全部计算结束后才观察和解密；不得给 evaluator 私钥、oracle 或 fresh 误差来补偿答案。实际 exact-prime 尺度递推 S'=S²/(d*q_actual)；生产 src/include 本次未改。

当前新增 test `tests/s100_annulus125_eight_square_test.cpp`、CMake 选项默认 OFF；运行工具 `run_once.py`（CLI固定1200秒）、`finalize_once.py`、`replay_annulus125.py`、`scalar_reassessment.py`。公开一次性工作流 `.github/workflows/s100-annulus125-once.yml` 只接受冻结 tag 的首次 unforced creation/attempt1，不准 dispatch/rerun/tag重建。源码保证与 compile-controls source def248a 的 src/include/tests/CMake 相同。

保持全部原 S100 具体密码参数、噪声、根与 family 元数据，仅输入 formula base1015→999，并独立命名。契约：全槽互异复数四相位，|x|<125/128，|x^256|>2^-10，输入微差2^-75；全槽最大复数模 E0≤T、A8≤T/4、两个 E8≤T，加 witness。A8必须同槽先作精确传播残差，不是两个max相减。真值必须原始 x^256，不得改为 fresh 解密值的平方。

## 研究范围与必须执行的可行检查

1. 第一遍独立检查论文算法/尺度对应、实际 C++ evaluator 隔离、输入/密钥单次性、full-slot 观察与数据列绑定、进程/源码/hash门禁。跨精度相符不等于正式误差证明；纸面勘误不能自动推出算法错或代码对。
2. 对新 raw.tsv 及实际 process exit 用两档纯标量精度独立重算 E0、E8、I8、A8、槽号与全部门禁。可先审查再运行 `python3 -B project/coordination/s100-annulus125-20260908/replay_annulus125.py --tsv project/coordination/s100-annulus125-20260908/experiment-evidence/sample/raw.tsv --source-commit 03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b --process-exit 0 --precision 230`，但至少独立挑战一个算法/证据边界，不能只运行作者脚本签字。不得改原始记录以骗过 finalizer 的远端绝对输出路径约束。
3. 再阅读 after-first-pass 材料，给出 findings → fixed/unsupported/accepted-limitation/pending 的出处与严重性。发现具体问题时提供最小纯标量或静态反例；没有可执行环境则明确未执行，不伪造运行。
4. 四层判定：算法有限证据、论文具体参数与输入条件、表3统计/性能同源复现、部署安全。给出用户能理解的“完成/没完成/原因/下一步”说明。明确绝对位数与相对位数、单样本与普遍保证、进程时间与纯乘法计时的区别。
5. 对下一步作唯一具体决定：若能指出保留原 stress 与公开语义的源码缺陷或有依据的修复机制，给源码定位、推导及最小 RED 验证方案；否则解释为什么当前不应再盲改／抽样，哪些缺失信息真正限定了结论。不要把低噪声key筛选、改sigma/v、秘密补偿、input缩小、S116成功包装成原stress修复。无需再做泛泛公共搜索或重复问作者。

## 交付物与验收

返回 ZIP：`FIRST_PASS.md`（先于作者结论形成）、`REVIEW.md`（源码位置、论文页/式、反例和问题处置）、`RESULT_FOR_CRYPTO_EXPERT.zh-CN.md`（对不懂代码的密码学专家说明）、`NEXT_ACTION.md`（唯一有证据的下一步或明确无修复依据）、独立标量检查脚本与实际 JSON/日志、`EXECUTION_LEDGER.md`、自排除 `MANIFEST.json`（每文件大小与SHA、输入ZIP身份、源码绑定）。不强求代码补丁；只有发现具体缺陷才给 RED 及最小修复建议，不执行外部修改。

验收要求：原始字节核验通过；真实和合成／标量检查分开；对原FAIL、S116、本次PASS分别作正确结论；所有未做事项明确写出；主要判断能追到纸面/代码/原始数据而非三方共识。未获得形式化数值／整数提升／安全证明时不得说已证明。

禁止：新加密、FFT/大构建、GitHub写/dispatch/rerun、作者联系、网页消息、定时任务、token/浏览器状态访问、私钥上传、把网页Pro标签当API模型身份、假装知道本机或未给的资料、再要求1000样本。你可在自己的隔离工作区阅读所给完整材料并运行有界标量检查。缺材料时准确列缺项，不默认能访问私有仓库。
