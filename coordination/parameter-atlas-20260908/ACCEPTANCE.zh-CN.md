# 参数/随机性参考文档：根端验收

2026-09-08 Asia/Shanghai。结论：**文档任务完成，可作为后续实现与排错的固定基线参考；不是整个论文复现完成，也不是生产代码无错证明。**

## 交付与协作

阅读入口：`docs/parameter-atlas/README.zh-CN.md`；详细阅读版：`docs/parameter-atlas/reference/OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md`；必须合读 `docs/parameter-atlas/REVIEW_NOTES.zh-CN.md`。

网页版 ChatGPT Pro 是主作者，页面显示 6 / Pro，提交前核验最高可见 Power 5/5，后端实际身份未获独立鉴证。完整源码/论文/任务 ZIP 提交一次，21:12:54 CST；22:38:35 CST 观察到终端答复，页面报告 Worked for 85m11s。未停止、刷新原页、催促、重复提交或换模型。对话永久引用：https://chatgpt.com/c/6aa009d7-cca4-83ec-9aab-1edac246bb44 。

两份独立第一遍源码地图在看到 Pro 主稿前形成，返回后分别复核参数与随机性/缓存问题。独立 worker 的模型字段只记录 requested-unverified，不把选择器当后台身份保证。ZCode/Fable 未参与本轮文档审查，不冒称其签字。Codex 主控完整阅读核心文档与检查脚本、核对原页/源码、实际运行轻量检查并裁定勘误。

运行基线 `a4b815a733efe81897325e2a8e4c826a4ebfa439`；官方固定依赖 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。新文档在独立 `codex/parameter-atlas-20260908` worktree，旧隔离区实现未使用。

## 覆盖与核验结果

| 要求 | 已交付/核验 | 不扩大为 |
| --- | --- | --- |
| 相关参数完整入口 | 33 个 CCParams 字段、32 setter、8 个 CKKS 禁用 setter；共 132 参数记录及 2 个派生状态检索别名 | 全 OpenFHE 所有方案/所有源码的审计 |
| 精确参数 | 两套 Q/P/root、基顺序、8 family、活动塔数、真实/记录尺度 | 自动通过 HEStd 或实现性能保证 |
| 完整随机路径 | 系统熵包装→BLAKE2Xb→DUG/BUG/TUG/DGG→同根密钥/PKE/evalkey，以及参数校验/tag 消耗 | 实际运行已取样、已证明熵质量或固定跨平台随机流 |
| 所有步骤 | 13 步状态/基/表示/尺度/秘密边界表；DCP/Tensor2/Relin2/RS2/RCB 展开 | 仅靠文档证明所有密文输入正确 |
| 改动影响 | 22 类变更矩阵、4 个具体例子；失效对象、测试、oracle、证据 | 本轮已实施这些改动 |
| 论文与项目映射 | S100/S116/annulus 分开；论文公式问题注明原页及历史地位 | 三种实验条件等价或原 S100 已修复 |
| 来源/文件 | 原输入 454 文件；返回 33 普通文件；所有 payload hash/CRC/路径核验；解码秘密扫描无发现 | 扫描绝对无遗漏或所有代码逐行读完 |
| 文档静态检查 | 原稿根端 replay 22/22，通过后修订阅读版再检查 22/22；127 引用 hash/行界/固定 URL 路径核对 | 新 FHE 数值 PASS |
| 公开整数复核 | scalar 输出与 Pro 逐字节同 SHA；两套参数/合计 16 轮精确 Fraction 与根端独立因子分解一致 | 编译器、sampler、FFT/NTT 或已有二进制分支已测试 |

原稿在 `docs/parameter-atlas/pro/` 原样保存，原 ZIP 已归档；阅读版有独立 MANIFEST。所有作者检查结果仍注明作者原稿自检；根端原稿 replay、初次阅读版、最终修订版检查分别有不覆盖的文件。

## 审查事项闭环

1. **精确函数名笔误：已修正。** 阅读版及关联 JSON/TSV 的 12 处由 `CreatePaperH128ClientKeyPair` 改为源码实际 `CreateFixedQH128ClientKeyPair`。原始返回未改。
2. **线程请求遗漏：已补齐。** 当前 CMake/dcp 源码也为 S100/S116 请求 2 线程；运行有效值保持未知，不混写。
3. **familyCount/familyQCount 缺专用检索项：已补别名。** 保持 132 条主体记录不重复制造独立参数，增加精确派生公式及现有条目/源锚点映射。
4. **随机路径作用层：已细化。** Bernoulli 适配器、FIXED_SEED 取消 threadprivate、ParallelControls 缓存上限、tag 的 distribution 输出与 engine 消耗分开说明。
5. **OpenMP private DGG：保留条件推论。** 源码/标准支持，历史二进制分支与 S100 精度因果未证。不认定需要立即改源码。
6. **native NTT 根缓存：保留条件风险并收窄结论。** 不比较 root 的缓存命中判据已核；一致使用同一合法根仍可内部正确。尚无同 q/N 混用不兼容表示的当前失败证据。
7. **论文印刷/中间推导问题：不改历史地位。** 与正确定义/尺度分开注明，不把局部不一致当新生产 RED，未补出未验证的完整误差证明。

## 后续交接

现在已有足够参考材料来选择诊断，不因未公开 HEaaN 配置阻塞文档。首选下一切片是 Codex 核对历史 runner 依赖构建 provenance/有效宏，再由 Pro 对涉及的采样/误差假设作定点复核；缓存项先看同进程 q/root 来源与对象生命周期。需要编译/实验时使用 Windows 或 GitHub，限最小区分性检查，不做 1000 次、不挑样本、不改门槛。

本轮没有启动后继实验、修改任何运行源码/测试/CMake/CI、dispatch/rerun CI、merge 默认分支、联系论文作者、新建定时任务或发送 PDF/Telegram。旧高频任务不因文档完成恢复。后继诊断的执行/结果需单独记录，不能从本验收推定已做。
