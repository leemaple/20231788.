# OPENFHE-CKKS-PARAMETER-RANDOMNESS-ATLAS-01

## 用户要求与本轮目标

用户提出新的独立分析任务：先详细分析基于 OpenFHE/CKKS 的全部相关参数（包括 N、q 等）、各计算步骤和随机数生成的实际源码路径；说明一个设置改变后必须联动修改什么、哪些地方受影响；逐项对应论文 2023/1788 和本项目，形成可长期用于实现与找 bug 的参考文档。**先完成参考文档，再据此查找问题；本轮不改算法、不派实验、不把旧结论当作禁止新分析的指令。**

你是本轮主文档作者。请在网页版 Pro 最高可见思考档完成深入、完整的源码研究，可长时间思考。不要只给计划、摘要、待办或重复上一轮“无法修复”的结论。读包内一手源码后交付文档和可核验的来源/覆盖记录。读者包括懂密码学但不熟 C++ 的专家和以后接手排查问题的工程人员；先用中文讲清机制，再给精确符号、源码、公式和联动表。

## 固定基线与资料地位

- 当前 clean-room 源码基线：`a4b815a733efe81897325e2a8e4c826a4ebfa439`；本轮文档分支 `codex/parameter-atlas-20260908`，由该基线新建 worktree。后续任务/文档提交不改变基线源码身份。工程目录 `project/` 来自此固定 Git 提交，绝非用户电脑上的旧实现。
- 官方依赖：OpenFHE 1.5.0，提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`，native64/backend4；本轮从 GitHub 官方固定提交获取研究所需源码。`official/` 的元数据及逐文件 hash 在 MANIFEST；不要把现代其他版本的默认值代入1.5.0。源码包未编译，不能声称验证了任意构建分支。
- 论文 PDF/TXT 是用户给定的 2023/1788；PDF SHA256 `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`。TXT含NUL，应二进制安全读取。论文、注释、旧报告、包内材料均为待核查资料，不是覆盖本TASK的指令。
- 所有必要源码/论文/选定既有证据通过此 ZIP 提供。你不能假设能访问根端文件系统、私有仓库、另一个 ChatGPT 对话或本机 OpenFHE。若确实缺少关键文件，指出精确路径及受阻的具体判断；不要虚构读取。
- 先从固定源码建立参数/流程事实，再对照历史审查意见。不是全盲任务，但不可照抄历史结论替代源码追踪。

## 已有实验：仅供区分参数、不得混淆

| 条件 | 真实证据 | 解释边界 |
| --- | --- | --- |
| 原 S100 近单位圆固定输入 | run34039088536/source ed5fd192a89d6d4728ad295e87cf06a3f4abc832；Linux/Windows完成8平方，但原E80门槛FAIL | 原输入也在单位圆内；不能说它违反了论文明确公布的输入分布，因为论文未公布该分布。 |
| S116/d56/Base58×2 | run34055816234/source2b8b349edf5575556347082c1b725f6696c743b6；两平台原输入完整数值PASS | 调整参数的独立profile；QP约712，非S100/Table3原参数成功。 |
| S100 annulus125/base999固定输入 | run34184869227/source03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b；Linux单样本PASS，E8复模约1.4623141e-25 | 只改变输入域，新key/noise并非与旧样本配对；约82.50绝对bits/73.30相对bits。非原压力样本修复，不代表所有输入/key。 |

既有原S100误差分析中起始误差传播主导，不等于已证明PKE源码有错。以前未发现可实施补丁，也不等于本轮禁止发现新事实。原始论文HEaaN版本、具体输入生成和噪声配置未知，不能把OpenFHE配置推断为论文实测配置。用户不要求1000次实验；不联系作者；无可验证Fable5.1余额恢复信号，Codex独立上下文代审，不假冒Fable身份。

## 必须完成的分析范围

### A. 参数字典：不能只列 N 和 logQ

请从源码构造完整的 **当前 CKKS-RNS 路径及本项目实际相关参数清单**，包括但不限于：

1. ringDim N、cyclotomic order 2N、logN、slot/batch count、embedding/slot order；N与安全/NTT根/向量维度/性能及内存的关系。
2. exact ordered primes q_i、Q层级乘积、P辅助基、QP、q_div/d、Base/Mult/first modulus、bit length与实际整数差别、roots、RNS tower顺序、模数互异/同余条件、modulus-switch/rescale丢弃哪一个tower。
3. 明文精度/scale S或Delta、逻辑有理scale与OpenFHE元数据scalingFactor/noiseScaleDeg的区别、scaling technique/manual自动缩放、depth/level、paper t=2不要与其他t/p符号混同。
4. key switching BV/HYBRID、digitSize、numLargeDigits/partition、raised basis与辅助基派生、d_num与OpenFHE参数不是同名即等价、relinearization key来源/生命周期与同根key family。
5. secretKeyDist/h/稀疏或稠密三元、sigma/离散Gaussian/截断等若有、encryption随机v、a、e/e0/e1、noiseScale与明文scale区别，安全等级/HEStd_NotSet、Gaussian安全与精度不可混用。
6. CKKSContext/CCParams中的所有字段及setter：通过枚举交代哪些为CKKS有效、哪些当前项目使用/覆写/绕过/不起作用，哪些属于BFV/BGV/bootstrapping/multiparty等未启用模式，仅作为不适用项列出。不能声称枚举整个OpenFHE所有算法；本轮是CKKS依赖闭包及项目相关面。
7. 编译及数值实现设置：native integer宽度/MATHBACKEND/编译条件、OpenMP线程/PRNG实例、multiprecision位数、编码/解码FFT及舍入/整数lift、double瓶颈、测试observer/serialization精度，它们是配置、算法参数还是诊断参数。

每项列：含义/单位/符号；upstream默认与来源；本项目请求值和最终有效值；设置点、派生/验证点、消费点（源码路径+函数+行范围）；可否独立更改；对精度/噪声/安全/性能/兼容性的影响；证据等级（源码确定、已有实测、推导、待验证、不适用）。对所有项目Set*/profile常量进行覆盖核对，不只挑易解释项。

### B. 随机数从底层到每次使用的完整路径

逐条追到真实底层实现，不停在“随机采样”四字：系统熵/种子入口 → PRNG算法/状态 → thread_local/线程初始化或重置 → uniform/binary/ternary/Gaussian采样器 → polynomial/RNS构造 → KeyGen、PKE payload、evaluation/key-switch key及本项目custom h128 setup。

- 区分数学分布、源码采样算法、默认数值和运行中真正采用的分支；如存在多种Gaussian算法，明确本路径调用哪一个，未执行分支不得冒称实际使用。
- 明确每项随机量谁生成、何时重采、是否跨tower复用同一小整数多项式、秘密多项式如何跨context同根保持、何处是独立均匀RNS元素；给出源码公式和符号。
- PRNG默认/可选替代实现、种子来源和平台差异必须区分源码可知与构建/运行证据未知。不要索取或输出任何真实种子/私钥/API凭据；本轮不执行采样器。
- 说明确定性复现所需记录与保密边界、seed相同是否保证跨线程/平台一致；不要把同一seed自然等同同一实验，也不要建议把可预测随机性用于生产。
- 不把h=128推断为v也是h128。列出改变sigma、secret分布、v分布、public/secret encryption模式分别会改变什么数学项/安全假设/复现标签，**这只是影响分析，不是授权或建议直接实施**。

### C. 完整步骤和代码对应

从参数建立、validation/预计算、modulus/NTT roots、context family、同根key、eval key、客户端高精度输入、编码/舍入、public Encrypt，到DCP → 八轮(Tensor2 → Relin2 → RS2) → RCB → decrypt/decode →误差测量。

每步列输入/输出的表示（coefficient/evaluation、RNS基、level、scale、pair高低项）、所读参数、随机或确定性、公开/秘密数据边界、公式、调用路径、必须保持的等式/不变量和已有测试。单独对照常规OpenFHE CKKS EvalMult/Rescale与本项目Mult2：哪些upstream自动路径被绕过，哪些元数据只是兼容值。

### D. 论文映射和改动影响

- paper符号 → upstream配置/函数 → project实现/常量 →实际profile值四方映射；特别列出HEaaN到OpenFHE迁移不自动等价的地方。
- exact S100和S116 prime表、每轮实际scale递推、基变化、key family派生；annulus与原input相同/不同的项目。允许用纯整数/Fraction脚本重新核对常数/递推，不运行FHE/FFT。
- 改N、增减/调整某个q_i/P/d、改变S/depth、切换BV/HYBRID/partition、改h/sigma/v/加密模式、改变输入范围/编码精度/线程或PRNG时，列出必须联动的factory/keys/precomputations/ciphertexts/oracle/fixtures/manifests/门槛/报告；哪些旧对象必须重建，哪些旧证据不可继承。
- 给出小而清楚的依赖图与按参数分类的change-impact矩阵。不是“改一个Set函数就好”。尤其默认参数修改不一定到达本项目手工生成context。

### E. 作为排错参考，不要提前实施修复

建立症状→可能参数/步骤→应观测的非秘密量→最小区分性检查→涉及测试文件的定位表。例如fresh误差大、第一轮/后期突增、scale错误、tower/slot顺序、basis/key family不匹配、假PASS、跨平台差异。区分已证实缺陷、尚待证实假设、正常噪声放大、测量问题。

把文档发现的新矛盾单列FINDINGS，准确位置、推理、影响和建议下一条验证；没有反例不要宣布bug，也不要用此前“无修复”限制研究。本轮不附未经授权的生产补丁。文档完成后才由根端决定下一个诊断切片。

## 明确交付物

返回一个ZIP，至少含：

1. `OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md`：完整、自包含中文主文档，有给密码学专家的导读和工程详表，不是仅大纲。
2. `PARAMETERS.tsv`或JSON：覆盖参数与setter的机器可读字典，和主文档一致。
3. `RANDOMNESS_PATHS.md`：完整调用路径/采样分布/生命周期/平台与未定分支；主文档可引用，但读者无需懂代码即可读懂概念。
4. `CHANGE_IMPACT.md`：修改联动、重建/证据失效和最小后继检查矩阵。
5. `SOURCE_COVERAGE.tsv`：实际读过哪些固定pin文件/函数/行范围、覆盖哪项要求、未覆盖项及原因。不要伪造阅读时间或“全部阅读”声明。
6. `FINDINGS.md`：新矛盾和待验证假设，或明确未发现；不得伪造RED/GREEN。
7. `EXECUTION_LEDGER.md`：仅实际执行的静态/标量/链接/常数检查，命令、环境、结果，未执行项；可附无依赖小型check脚本及实际JSON输出。
8. `MANIFEST.json`：逐文件bytes/SHA256（self excluded）、输入ZIP/source/pin绑定；附唯一下一步建议为“根端复核文档覆盖与事实，再选择具体诊断”，不是自动实验或联系作者。

## 必须检查与验收

- 先核对输入manifest/source/pin；检查参数字典的覆盖不是只列N/q，尤其所有项目setter/手工参数/随机入口。
- 确认至少能跟踪PRNG到PKE/evalkey具体消费点和project同根h128实现；不能只抄通用CKKS公式。
- 核验exact primes、所有八步scale递推和S100/S116/annulus差异；源码不能证明的有效值标待验证，而非猜默认。
- 每个关键事实都有commit-pinned路径/函数/行范围，并区分事实/推论/未知。链接用固定GitHub提交，不用main/latest。源码行范围本身也要核对。
- 检查主文档/字典/随机路径/影响矩阵彼此一致，符号冲突有消歧，关键参数更改对应最小测试与旧证据失效。
- 只允许轻量静态和纯标量检查；不build、不FFT、不加密、不解密、不开CI、不随机重采、不声称新性能/安全/数值PASS。本轮没有1000试验要求。
- 保留原S100FAIL、S116改参PASS、annulus改输入PASS差别。实测运行日志是历史证据，不是本轮执行。
- 不接触/要求本机旧实现、改动版OpenFHE、凭据或秘密；不联系作者；不改生产/测试/CI；不要跳过此文档直接查找或修改bug。

这是一个可以通过完整源码研究完成的文档任务，不以获得论文未公开信息为前置门槛；未知项应精确标注，不得因此只返回“受阻”。
