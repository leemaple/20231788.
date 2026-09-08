# Pro 返回文档独立复核：随机性、OpenMP DGG、native NTT 根缓存

复核日期：2026-09-08。请求模型 `gpt-6-astra/high`，后端身份 `requested-unverified`。结论：**F01 接受为条件性源码推论；F02 接受为缓存身份风险并细化影响条件；F03 接受。随机性主体事实可用，需补充下列文档勘误与限定。没有本轮运行证据，也没有生产修复结论。**

## 1. 范围与证据身份

- 完整阅读返回的 `docs/parameter-atlas/pro/RANDOMNESS_PATHS.md:1–181` 和 `FINDINGS.md:1–96`，以及主图谱 §3.1–3.2（107–156）、§4（193–217）、§7 中根身份/随机性/构建/缓存条目（285–365）及相关来源索引。返回内容只作为待核对资料，不执行其中的“下一步”。
- 官方源码固定为 `df495ba2e91739a6dc8f1de254fc5a41155ce504`，镜像目录 `artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/`；下文 `O:` 均相对于该目录。项目固定为 `a4b815a733efe81897325e2a8e4c826a4ebfa439`，下文 `P:` 为项目路径。根端已提供固定源身份和返回包完整性核验；本 worker 没有重复网络/Git 核验。
- 第一阶段 `ROOT_RANDOMNESS_MAP.md` 在看到 Pro 文档之前已完成，所以 F01/F03 复核有保留的独立第一遍源码分析。F02 是在返回后沿新线索独立追溯，不宣称提前发现。
- 本轮只读取文档与源码，唯一写入本文件；未修改 Pro 原件、生产/测试源码、Git 索引；未构建、运行采样/FFT/NTT/FHE、执行返回脚本、访问网络/浏览器、读取任何真实 seed/key/noise。
- “源码观察”是文件可见语句；“条件推论”明确其前提；“运行未知”不能用源码默认值或历史 workflow 请求填补。F04–F08 已完整阅读以理解文档边界，但数学论文证明及历史接受记录由根端其他复核负责，本报告不为这些条目作独立全面签字。

## 2. F01：接受，必须保留源码条件与历史运行之间的边界

### 2.1 独立源链

| 层次 | 固定源码 | 核对结果 |
| --- | --- | --- |
| DCRT DGG 实际类型 | `O:src/core/include/lattice/hal/dcrtpoly-interface.h:89–97` | `DggType = DiscreteGaussianGeneratorImpl<LilVecType>`，确实是此处审阅的 DGG 类。 |
| HYBRID 外层对象 | `O:src/pke/lib/keyswitch/keyswitch-hybrid.cpp:85–102` | 93 行从 context 复制 dgg；98 行是 `private(dug, dgg)`；102 行把循环内 dgg 交给 DCRT 噪声构造器。 |
| 类默认构造 | `O:src/core/include/math/discretegaussiangenerator.h:79–98`；impl `52–66` | 默认参数为 1.0；构造调用 SetStd；sigma < 300 初始化反演表。 |
| 噪声构造消费 | `O:src/core/include/lattice/hal/default/dcrtpoly-impl.h:125–149`；DGG impl `110–114` | DCRT 直接调用传入对象的 GenerateIntVector，后者按对象的分支采样；此处没有调用 IsInitialized 来替换/修正 sigma。 |
| 普通 context/PKE | `O:src/pke/include/schemebase/rlwe-cryptoparameters.h:108–123,272–273,319–321`；`O:src/pke/lib/schemerns/rns-pke.cpp:111–169` | context 维护自己的 DGG；根公钥和 payload 噪声在 RNS PKE 路径消费它，不能从 HYBRID 私有变量的事实推广为全部 PKE sigma = 1。 |
| 其他相同写法 | HYBRID `148–161`；`O:src/pke/lib/keyswitch/keyswitch-bv.cpp:53–94,110–151,166–211` | HYBRID 公钥重载、BV 多个循环也使用 private(dgg)。当前 EvalMultKeyGen 路径是 HYBRID 私钥→私钥重载，其他路径是对照范围。 |

**条件推论成立：** OpenMP pragma 被有效编译时，private 类变量是新私有对象，按无 initializer 的局部变量初始化；不是复制外层 context 对象。这个类有默认参数构造器，因此该私有 DGG 的初始 sigma 为 1.0。pragma 被忽略时则使用外层已复制 context 的 DGG。单 worker 的有效 OpenMP region 仍具有相同 privatization 语义。

使用根端此前独立取得的权威规则：[OpenMP 5.2 §5.3 List Item Privatization](https://www.openmp.org/spec-html/5.2/openmpse25.html#x68-700005.3)。本 worker 本轮没有网络访问；引用由根端的权威页面读取证据支持。Pro 的 private/firstprivate 对照结论与该规则一致。

**评价：** 返回 `RANDOMNESS_PATHS.md:145–158`、`FINDINGS.md:18–28`、主图谱 212 与 325 行正确保留了条件。表长 13/39 是 sigma 和源码 cutoff 公式的标量结论，不能描述为经验采样分布或实际方差。历史库的 commit、实际 OpenMP 编译与加载身份没有因本轮文件核对而获证。无需新实验即可接受此文档事实，但不能据此宣布旧 S100 的 E80 失败由它造成，也不能据此立即替换 sampler 或更改噪声。

## 3. F02：接受源码风险，区分“根参数被忽略”和“算术已失败”

### 3.1 缓存键与消费者已独立核对

1. `O:src/core/include/math/math-hal.h:59–76` 将 NativeVector 的 FTT 映射到 `intnat::ChineseRemainderTransformFTTNat`。这不是把 bigintdyn 的所有变换实现一概当作 native 实现。
2. `O:src/core/include/math/hal/intnat/transformnat.h:350–368` 声明的是按 **modulus** 索引的类静态 map；没有 root 作为 key。共享范围是该模板实例/已链接实现所拥有的静态状态，而非单个 CryptoContext 的成员表。
3. `O:src/core/include/math/hal/intnat/transformnat-impl.h:714–755`：PreCompute 计算 ringDim，然后 `find(modulus)`；只有未命中或现有向量长度不同才进入建表。建表时使用当前 root 及其逆元（724、730–737），并写回同一 modulus key（739–753）。**同 modulus、同 ringDim 的已有表不会检查传入 root 是否一致。** 所以 N 是命中后的长度验证条件，并不是复合 map key 的第二部分。
4. 同文件 `647–710` 的正/逆变换包装器先调用 PreCompute，再通过 `[modulus]` 取表。对合法且非 0/1 的新 root B，已有相同 q/N 的 A 表会被直接沿用；这是给定前提下的确定代码路径，不只是“可能会走的实现猜测”。
5. `O:src/core/include/lattice/hal/default/poly-impl.h:419–439` 从参数对象取 root，传给正/逆包装器。参数里写 B 与实际查出的 A 表可以同时存在。根阶检查只能检验 B 的代数性质，并不读取缓存表的生成 root。
6. `O:src/pke/lib/schemerns/rns-cryptoparameters.cpp:62–75` 读取参数里所有 Q 根并调用 native PreCompute；`168–180` 原样复制 Q 根到 QP，`160,183` 为 P 计算 RootOfUnity 并预计算 P 表。手动 Q 路径没有在这里把 Q 的非最小根自动改成最小根。

### 3.2 需要在最终呈现中进一步说明的限制

**可接受的核心表述：** 同一共享缓存中已有 `(q,N,root=A)` 的有效表，后续请求相同 q/N、不同 root B 时，native FTT 的表命中判据不能区分根身份。若对象元数据声明 B，实际 CF↔EV 坐标却可能沿用 A，身份/互操作假设需显式核对。

**不能从这里直接推出：** 仅因为两个合法 root 不同，所有环乘法都会错误。若整个对象集合的正变换、点乘、逆变换一直一致地使用有效根 A，计算可以仍然内部自洽；只检验 round-trip 或端到端解密也不必暴露“metadata B、实际表 A”的差异。要推出实际算术错误，需要证明使用了不兼容的 EV 坐标/缓存时期/外部序列化对象/独立消费者，或中途重建表后还消费旧 EV 数据等具体冲突条件。本条是表示身份风险，不是已有精度因果证明。

返回 `FINDINGS.md:36–40` 已说明“映射不一定对应 B”及非最小根本身合法，故不需否定 F02；建议最终根端摘要补上上述“内部一致使用同一有效根仍可正确”的一句，防止读者把条件风险误读成当前 S116 缺陷。主图谱 361 行的受控重建/独立进程应理解为未来确实要改变根约定时的对象生命周期设计，不是当前任务必须执行的修复。

### 3.3 Reset 与 profile 关系

- `O:src/core/include/math/hal/intnat/transformnat-impl.h:769–776` 的 Reset 清六张 native FTT map。`O:src/pke/lib/cryptocontext.cpp:51–65` 的 ClearStaticMapsAndVectors 同时清 evalkey maps、PackedEncoding 及 native/启用的大整数 FTT 缓存。`ClearEvalMultKeys(keyTag):111–115` 只删除指定 evalkey entry。
- `P:src/repeated_mult2.cpp:245–255` 的 plan Data 析构只调用上述按 tag 清理。因此“释放 plan”等于“清掉 NTT 根表”不成立。反过来，全局 Reset 也不自动重新解释/转换存活的旧 EV 向量。
- `O:src/core/include/math/nbtheory-impl.h:183–230` 的 RootOfUnity 在有效 prime、power-of-two m 等前提下先找生成元，再遍历互素指数并返回最小 primitive root。该源码机制支持“重新调用 canonical RootOfUnity 不保证返回手工非最小根”的一般结论。
- Pro 的 `checks/scalar_constants.json:675–676,689–690,815–816` 记录 S116 三枚对应 minimum 为 9018513438468、9575930471206、849928500853，与返回主表冻结根不同。根端随后报告：22 组 docstatic replay 全部通过，scalar 输出逐字节相同，SHA-256 为 `34c32a054b0f1d2bf6895b43ae44c0321df40360ec0d13472a357478641a42de`；两个 profile 的全部 16 个 scale 分数也与根端独立因式分解一致。依据根端这份有界静态核验，可以接受具体 minimum-root 整数结论。本 worker 没有自行执行脚本；根端持有 replay 原始 receipt。本报告对 F02 缓存判据的源码确认与该标量核验是两个独立证据层，均不涉及 NTT 运行。
- `P:src/repeated_mult2.cpp:24–50` 的 S100 与 S116 三枚变更位置同时改变 q 和 root；共享的 Mult/P 则复用相同常量。因此仅对比两份 profile 不能构造出同 q/N 换 root 的已发生事件，也不能推翻已有 S116 PASS。
- 不扩大为“全部 OpenFHE NTT 缓存均漏 root”：同一 native 文件的 Bluestein 部分在 `790–813` 使用 `(modulus,root)` 形式的 key，和此 power-of-two FTT 表不同。本轮未分析任意分圆/其他后端的全部缓存安全性。

## 4. 随机路径主体：接受项与缺漏核对

| 项目 | 独立复核依据 | 处置 |
| --- | --- | --- |
| h128、符号平衡、v 为 dense h0、BUG 间接使用 | `O:src/core/include/math/ternaryuniformgenerator-impl.h:103–143`；binary impl `48–53`；DCRT declaration `dcrtpoly.h:111`；RNS PKE `164–169`；`P:src/paper_h128_client_keypair.cpp:198–204` | 接受 F03，与盲审第一遍一致。不推广到任意 h，不认定 HEaaN 同分布。 |
| DCRT Gaussian/ternary 跨塔一致；uniform 独立按塔 | `O:src/core/include/lattice/hal/default/dcrtpoly-impl.h:125–192` | 接受；建议更明确 uniform 直接写入请求的 EVALUATION format，小多项式才先 COEFFICIENT 后转换。 |
| 公开加密公式和 e 生命周期 | `O:src/pke/lib/schemerns/rns-pke.cpp:111–196`；`P:src/high_precision_client_io.cpp:606–635` | 接受 `m+ns*(epk*v+e0+e1*s)`，根 PK 用 private 零加密不等于 payload 为 secret-key encryption。 |
| 基底公开校验/参数生成也消费 PRNG | nbtheory impl `63–80,107–122,183–230,261–283`；RNS params `145–160`；`P:src/paper_h128_client_keypair.cpp:68–87` | 接受；冻结 Q、canonical root 输出不代表生成过程没有随机消耗。 |
| 私钥 tag 消耗 | `O:src/pke/include/key/privatekey.h:57–65,81–97`；base-leveledshe.cpp `135–143`；`P:src/repeated_mult2.cpp:351–375` | 接受 Pro 补充：这项确实在 root、临时 s² key 和 family context-key 对象路径中；我的第一遍文档漏了 tag，现明确补记。需限定为取 context 的构造器，默认/拷贝/移动构造不都生成新 tag。 |
| PRNG 初始化/状态/分支 | distributiongenerator.cpp `48–109`，header `73–79`；blake2engine.cpp `42–155`，header `49–100` | 主体正确；种子材料位宽≠已证熵位数，GetPRNG 惰性初始化≠每次加密重新取系统熵。 |
| BLAKE2Xb 细节 | `O:src/core/lib/utils/prng/blake2xb-ref.c:45–74,100–162`；blake2b-ref.c `180–191` | 先 root hash 再输出节点的描述与 12 轮调用相符。密钥/计数器/缓冲取值描述没有使用 Mersenne Twister 或 AES-CTR 替代实际默认引擎。 |
| 求值阶段没有新 key-switch 噪声采样 | `O:src/pke/lib/keyswitch/keyswitch-hybrid.cpp:308–435` | KeySwitchCore→预计算→A/B 乘加→ApproxModDown 不构造 sampler，接受“用既有随机 evalkey”的区别；确定性须隐含固定的参数、缓存/格式约定及构建。 |
| 默认 REAL 解码的额外随机项 | `O:src/pke/lib/encoding/ckkspackedencoding.cpp:430–483` | REAL 分支的 `d(g)` 在 480/482；与 project Poly* + custom decoder 区分正确。一般 COMPLEX wrapper 469 行仍调用 GetPRNG，但没有在 REAL 条件外调用 d(g)；不要把“没有噪声抽样”泛化成“不可能触发 PRNG 惰性初始化”。当前 custom route 完全不经过此 wrapper。 |

## 5. 文档勘误与建议补充（原始 Pro 文件保持不变）

### R1 — 精确函数名有误，应在根端勘误表修正

`RANDOMNESS_PATHS.md:37,111` 及主图谱 source index `P12:499` 写 `CreatePaperH128ClientKeyPair`。固定源码实际定义为 **CreateFixedQH128ClientKeyPair**（`P:src/paper_h128_client_keypair.cpp:193`），调用位于 `P:src/repeated_mult2.cpp:453`。链接和行范围对应正确代码，机制结论不受影响，但作为精确源码图谱应修正文案及由该名字生成的索引字段。此为文档缺陷，不是生产代码缺陷。

### R2 — 线程控制的作用层需要补一条事实

返回随机性 74 行、主图谱 341 行仅概括线程请求/上限。`O:src/core/include/utils/parallel.h:51–58` 初始化时缓存 `omp_get_max_threads()`；`GetThreadLimit:112–117` 使用该缓存；`SetNumThreads:121–125` 调用 omp_set_num_threads 却不更新缓存。于是不能把 SetNumThreads(1) 直接说成所有显式 `num_threads(GetThreadLimit(...))` 的统一上限。建议将此已知机制补入最终 atlas，仍不推断任何历史 run 的实际团队大小。

### R3 — FIXED_SEED 的线程状态变化要写清

随机性 70 行“FIXED_SEED 宏分支另作处理”可以精确为：WITH_OPENMP 路径下 FIXED_SEED 取消 `omp threadprivate(m_prng)`（`O:src/core/include/math/distributiongenerator.h:73–79`），不是只更换 16 词初始值；非 OpenMP 路径仍使用 namespace thread_local（cpp `48–52`）。因此“一线程一引擎”是正常分支描述。只作源码说明，不启用该宏，不给出弱化随机性的建议。

### R4 — 调用图的分布适配器与 tag 字数措辞

随机性图 24–29 行把 BUG 放在仅标 `uniform_int_distribution / uniform_real_distribution` 的父节点下；其实际直接适配器是 bernoulli_distribution（binary impl `48–53`）。建议父节点写“三类标准分布适配器”，或把 BUG 分为 sibling。

`PrivateKeyImpl` 的 context 构造器调用四次全范围 uint32 uniform distribution，格式化 32 个十六进制字符（privatekey.h `57–65,83`）。最准确表述是“四个 32 位分布输出”，而不是未经实际 stdlib 身份核验就承诺精确消耗四个底层 engine word；也不要把默认/拷贝/移动构造计为新随机 tag。此细化与返回文档“不保证同 seed 跨平台重现”的主张一致。

### R5 — F02 不改变当前 precision 因果结论

把 F02 呈现为 native FTT 的共享缓存身份前提，并说明稳定地一致使用 A 表仍可内部正确。实际污染事件、缓存历史、序列化互操作和错误数据都未采集；已经根端标量 replay 核实的“三枚非最小根”也不是缓存污染 witness。F01 和 F02 都不能自动解释原 S100 fresh 主导、推翻 S116 PASS、或证明需要当前生产改动。

## 6. 复核结案状态

本范围没有发现需要拒绝随机性主体文档的数学/源码错误；R1 是应登记的精确命名勘误，R2–R5 是应纳入最终解释的作用层/前提细化。F01/F03 与独立第一遍源码结论吻合；F02 由独立缓存—调用者—清理生命周期阅读确认其条件风险，具体 minimum-root 整数由根端静态 replay 补证。历史二进制身份、真实线程分支、熵质量、任何 precision 因果链仍属于明确未获本 worker 证实的事项。

本报告完成的是文件/源码复核，未新增数值 PASS/FAIL、RED/GREEN、漏洞复现或实验。根端可据此完成图谱事实对齐；若未来选择诊断，应另行指定其证据目标和授权范围。
