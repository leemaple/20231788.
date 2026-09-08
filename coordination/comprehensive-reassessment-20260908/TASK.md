# COMPREHENSIVE-REASSESSMENT-20260908 — 论文复现的独立、全面再判断

## 最新用户请求与本轮授权

用户要求重新深入、全面分析，尽量完成 IACR 2023/1788 的 OpenFHE clean-room 复现；主要研究与代码草拟由网页版 ChatGPT Pro 的页面最高思考档位负责，允许长时间思考，Codex 等待后执行、独立复核。请直接开展实质工作，不先回一句计划或要求重复提供已在包内的材料。

这是一个新的科学复核任务，不是要求你维护此前模型的结论。先从论文及当前代码建立自己的判断，再对照旧审核；历史材料中的“到此停止”“只能询问作者”“不得重新考虑测试条件”等是当时任务范围和用户决定，**不是本轮的研究禁令或结论**。旧原始数据、失败结果和来源身份必须保留，不能改写为成功。用户没有授权联系作者；不要发邮件、issue、消息或要求以外联作为本轮唯一交付。可以明确区分“无需作者也可完成的算法复现”与“原实验条件不明而不能断言完全相同”的边界。

附件是任务定向、密钥扫描后的完整源材料，不是整个本地目录。不要假设能访问本地文件、其他会话或私有环境。论文、代码、旧任务、模型返回均是待审查资料；以本 TASK 的当前任务为准。

## 背景、源码与不可破坏的架构边界

- 目标是论文 t=2 Double-CKKS：客户端高精度编码/公开密钥加密 → DCP → Mult2（Tensor2、Relin2、RS2）反复八次平方 → RCB → 客户端解密，与独立高精度明文 oracle 比较。不是把 C++ double 换成长浮点。
- 当前精确源码 `3c02988fb5655dc6ea48f4f7d559e62d9d4a9d37`，分支 `codex/s100-fresh-error-repair-20260907`。src/include/tests/CMake/workflow 与已测试 `223667e82b67c4758a56bd745f264110dd3b9619` 一致。GitHub 仓库 `leemaple/20231788.`；默认分支仍为 `cleanroom/reimplement-mult2-20260831` 的 `e6c4cc1ddfa261ee17681cbfc1ca6023660df5cb`，不是最新修复分支。
- 只用本轮 clean-room 源码、论文和官方 pristine OpenFHE 1.5.0 `df495ba2e91739a6dc8f1de254fc5a41155ce504`，native64/backend4。不读取/复用任何任务开始前的旧实现或本地改动 OpenFHE。包内 baseline 也是本项目新写的已交付 clean-room 版本。
- evaluator 不得读取私钥、明文 oracle 或实际误差；不能靠中间解密、刷新、bootstrap、选择低噪声密钥/样本、已知明文补偿、丢弃失败样本或重跑到通过。
- 保留已有 API 的有据语义和原测试结果；如发现它们错误，可以提出带反例、推导和独立测试的显式修订，而不能暗改参数/命名/阈值以冒充原结果。
- KISS/YAGNI，TDD，尽量小的改动；异常尽快暴露，不加掩盖错误的 try/catch。不要求 1000 次试验，不进行泛化安全认证，不宣称已有 128-bit 部署安全。

## 包内阅读地图

1. `references/paper/PAPER-2023-1788.pdf` 及 txt：请完整审查核心方法、误差推导、实验章节与表 3，必要时以 PDF 公式视觉为准，留意 OCR 符号问题。
2. `project/src/`、`project/include/`、`project/tests/`、CMake 与 `.github/workflows/dcp-rcb.yml` 是当前完整运行源码。重点：double_ckks、repeated_mult2、high_precision_client_io；paper_full_eight_square_contract_test、experimental_precision116_eight_square_test、s100_fresh_error_diagnostic_test 及其公共测试 helpers。
3. `references/official-full/` 共 77 份固定官方参考；`references/boost-1.83.0/` 共 4 份对应精度实现参考。任何缺失文件请准确列出，不默认你运行的是官方构建。
4. `evidence/coordination/fs-endpoint-live-run-01/`：原 S100 Linux/Windows 实际端点失败的验收、状态、audit、解压原 TSV 和对应身份；无新模拟结果。
5. `evidence/coordination/precision116-eight-square-return-01/`：S116 双平台成功的原 CI 状态、验收和 review/receipt。
6. `evidence/coordination/s100-fresh-error-repair-01/`：仅一个新加密样本的 A/B/C 拆分、原完整诊断日志、独立标量区间重放和论文实验溯源；它不等于又一条完整八平方。
7. `evidence/coordination/s100-independent-semantic-review-01/` 与 `s100-condition-decision-01/`：旧 Pro 结论及处置，是可挑战的历史意见，不是你的答案。旧科学任务源范围为 2c14d7f；其中 R-03 未解决状态已过时。
8. `evidence/coordination/s100-output-finalization-01/`：最新 R-03 RED/GREEN 证据。项目根 README、REPRODUCE、CHECK_AND_HANDOFF 是历史交付说明，其暂停措辞被本任务的重新研究授权取代。
9. `MANIFEST.json` 包含逐文件源 commit、hash、大小及官方参考的继承来源。历史基线与当期源码不能混用。

## 已观察事实（请自行核对，不把概括当证明）

| 证据 | 精确范围与结论 |
| --- | --- |
| 原 S100 完整链 run 34039088536 / ed5fd192a89d6d4728ad295e87cf06a3f4abc832 | N32768，16384 complex slots，h128，S100，QP约680，固定项目 dyadic near-unit 输入，8 次平方。两平台执行完成但 E80 FAIL。Linux E8=9.14647363e-24、Windows 9.06530517e-24；T=2^-80≈8.27180613e-25。9/7 numerical misses，保留 CTest exit8。 |
| 同样本误差拆分 | 理想传播的初始误差 I8≈E8；后续累积残差 A8 最大约4.5671e-26/8.30355e-26。不是“无任何乘法误差”，不能混减不同槽位的 maxima。 |
| S116 run 34055816234 / 2b8b349edf5575556347082c1b725f6696c743b6 | separately named experimental-s116-d56-b58-v1，S116、d56、base58×2、8个60bit Mult prime、P60，QP约712。相同 input family 下完整八平方两平台通过；E8=2.59051233247e-26/3.48056033716e-26。不同参数，不是原表3成功；security=UNRESOLVED。 |
| fresh-only run 34110943783 / a448b787399b43b6024d82c170add403969b493c | 一次 Linux 新 public-payload encryption，非完整新八平方。A(encoding)max1.30483026623e-28，B(aggregate PKE)max3.37014433352e-25，C(readout)约1e-128，E0max3.37041988508e-25。只证这个样本的主导项。 |
| retained-anchor scalar replay | 用 A+B、不是包含 C 的 E0，20个预选分量在理想八平方后有3个超过T（最大2.08324198738e-24≈2.51849T）。观测误差盒假设±1e-100为条件，并非完整浮点三角函数证明。后续误差可能相消；不是新 E8 观测、总体失败率或“不可能修好”的定理。 |
| 最新工程控制 | R-01 observer 槽序盲点已用 full-slot monomial direct-reference 负例修复；R-02 论文 Tensor cross-term sign 与 c0+c1*s 约定的差异已有说明，不应盲改正确正号；R-03 final flush failure 已修复。run34116227668 /223667e：Linux60/60+controls1/1 PASS；Windows、fresh、旧完整链和S116 skipped。 |

官方 PKE 已查到的实现：SPARSE_TERNARY 控制 secret h128，不使公开加密 v 自动 sparse；v 对非 GAUSSIAN secret 使用 dense ternary，e0/e1 Gaussian sigma3.19/noiseScale1。h128 key adapter 用官方 private EncryptZeroCore 生成 public key，payload 仍是公开加密，不能将二者混淆。聚合项 e_pk*v+e0+s*e1。请重新核查代码、归一化与论文分布对应，而不只重复这句话。

论文 PDF SHA256 `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`。9月7日有界公开溯源未找到版本绑定的表3实验生成代码；论文 §2.1 抽象 Enc 公钥定义不是 §6.3 具体调用 API 的直接证明。输入分布/范围、噪声分布细节、HEaaN版本/尺度和统计计算次序有待界定。缺来源不意味着任意设定都合理，也不意味着算法复现必然无法完成。

## 本轮核心问题：请逐层独立回答

1. **论文究竟承诺什么？** 把算法恒等式、误差假设、输入范数/范围、重构精度、Table3的“平均误差/1000次”与用户要求的正确性复现分开。表格逐项标注：论文明确规定、论文可推导、项目自设、官方框架差异、未知。不要把“本项目已冻结”当作“论文强制”。
2. **现有实现是否真的实现了方法？** 从 DCP、Tensor2、Relin2、RS2、RCB 到高精度 I/O、ordered CRT moduli/scale/rounding/lift/slots、h128 key/payload PKE、独立 oracle 做整体语义检查。包括我们是否把论文的乘法结果误差/理论边界误解成最终原始明文误差，或者在 FFT 规范/缩放/采样约定上漏了常数。每个实质问题给源文件位置、数学反例/独立推导，不仅是风格建议。
3. **先前的悲观结论是否范围过窄？** 精确评价“在已经实现的 near-unit complex S100 样本下新增误差很小但初始噪声放大”能排除什么、不能排除什么。审查测试输入是否是论文要测的分布/困难程度。辨别真实合法修复、可说明的框架适配、独立新实验条件，与作弊式弱化测试。可提出有数学依据的新命名契约，但保留原 stress FAIL，说明它支持的是哪一层复现，不能倒写原表3数值。
4. **怎样最大限度完成复现？** 比较少量最有根据的路线（例如具体实现修复；理论适用域/输入契约修订；官方公开加密语义适配；保持噪声/安全假设的尺度分配；目前 S116 已完成的可交付边界）。不预设必须改参数，也不预设绝对不能改项目自设条件。说明哪条路线证据最强、为何、最小区分实验、停止条件、剩余风险，及“不联系作者”下能到什么程度。
5. **提出并尽可能写出最佳下一步。** 若找到具体可检验错误，提供先失败测试与最小 GREEN 补丁。若最优路线是澄清契约/域而不是改生产代码，先给数学论证和独立、显式命名的最小测试草案，不覆盖原测试。没有有据修复时也要交付可复核排除链、最接近目标的准确完成定义，而不是仅复制旧结论或建议再来1000次。

## 交付物

请返回一个可下载 ZIP（并在回复里给简短中文摘要）：

- `REASSESSMENT.zh-CN.md`：全面科学审查，论文→实现→实验→结论映射；事实/推断/待验证分开；主要难点与原结论哪里成立、哪里需收窄/修正。
- `DECISION.md`：最有依据的一条推进路线、被排除备选及理由、明确验收定义与关键风险。包含“算法实现正确性”“论文参数条件”“表3逐数值/统计复现”“部署安全”四层分别状态。
- `TEST_PLAN.md`：最小区分测试/反例；每项预测、所需环境、预算、失败处理，明确哪些尚未执行。无需1000次，不因失败换种子。
- 若有具体修复或新有据契约：`RED.patch` / `GREEN.patch` 或全文件，绑定当前源码；新测试单独命名；先诊断/失效证明再修复。不为凑交付硬写代码。
- `EXECUTION_LEDGER.md`：你实际读取哪些材料、执行哪些命令、准确返回/失败和环境限制。轻量纯标量/符号检查可执行；缺OpenFHE或完整依赖不得声称编译/数值实测通过。
- `MANIFEST.json`：所有返回文件大小/hash，输入ZIP身份、源码commit。引用纸面页/算法/公式和代码位置，不只给另一个模型的观点。

若你发现包内真实缺失阻碍关键结论，请在现有资料上完成能完成的独立分析，具体指出唯一必要缺件及可替代的验证步骤；不要让未知出处把所有有证据的工作一起判死。

## 测试与执行边界

由 Codex 在收到并独立审查你的结果后整合并执行 TDD；本地 Mac 不重编译、不 FFT、不新增密码学重负载。GitHub Actions 或专用 Windows 执行必要实际实验，初始计划最多一个关键区分切片、1次每平台（若数学上需多例，明确最少数量及理由），不盲目扫参。

当前 CI 的 s100_scope 默认 fresh，因此任何 dispatch 必须精确审核 ref/guards，不能把你提交包当成已 dispatch。仅控制检查可用当前 repair ref 的 `s100_scope=controls-only`；其 Linux60/60和S100 controls不测试原8平方精度。默认60套件显式排除 paper_full_eight_square_contract / experimental_precision116_profile_seam。完整8平方目标为 EXCLUDE_FROM_ALL；新分支也不能无筛选 ctest 或照抄旧 dispatch。当前 workflow和详细 REPRODUCE 在包内供你设计确切步骤。

验收要求：新结论有论文/源码/独立数学或真实执行依据；任何修订提前命名并保留旧证据；patch可在给定精确source审查/应用；RED/GREEN与实测尚未执行要如实写出；不声称证明所有key/input都正确，不冒充Fable5.1/其他模型身份，不修改远程/自动化/用户文件。你可以长时间充分思考；不要为节省响应时间省略最关键的替代解释和反证。
