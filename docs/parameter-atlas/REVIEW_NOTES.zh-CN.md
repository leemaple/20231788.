# 根端复核、勘误与使用边界

2026-09-08。本文是 Codex 对网页版 ChatGPT Pro 主稿的复核层，不是主稿作者的自我背书。运行源码仍固定为 `a4b815a733efe81897325e2a8e4c826a4ebfa439`，官方 OpenFHE 固定为 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

## 阅读版本

- `reference/` 是经根端勘误的阅读版，入口为[完整中文图谱](reference/OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md)。其 `MANIFEST.json` 绑定发布文件，不能拿原稿清单来验证修改后的文件。
- `pro/` 是 33 文件原始返回，逐字节保留，原 ZIP 的 SHA-256 为 `623abe4affa88d2e53ba67b77a0648d477177c7df668508fdb66edbcb6c9d535`。原作者执行记录、原检查器失败记录和结果未改写。
- [中文导读](README.zh-CN.md)解释论文做了什么、我们怎样实现、已有结果及下一步。要定位具体参数和代码时再进入完整图谱与机器字典。

## 已核验的范围

根端完整阅读主图谱、随机路径、变更影响、发现登记、覆盖说明、执行账及三个可移植检查脚本；对照独立参数/随机性源码地图，并核对论文原 PDF 的第 4、5、8、13 页。没有声称对 331 个官方文件全部逐行审计。

原 ZIP 的 33 个普通文件、大小、SHA-256、CRC、路径与清单均通过核验；解码后的 Gitleaks 8.30.1 和针对性扫描均无发现。完整输入包含 454 个文件，官方源码来自本轮新取的固定官方提交，不复用隔离区实现。

根端读完脚本后实际执行了三个轻量检查：输入身份、公开整数/Fraction 常数、文档一致性。22 组文档检查通过；公开整数输出与 Pro 的结果逐字节相同。两套 profile 的 Q/P/root 和合计 16 个轮次的精确尺度，与根端先前独立因子分解计算一致。127 个引用的本地源字节、物理行界、固定提交 URL 路径已核对；没有逐个 HTTP 请求来声称所有网页在线可达。

这些 PASS 只代表上述文档、源码索引和标量检查，不是新加密实验、性能测量、安全认证或无 bug 证明。实际命令和输出见[根端复核回执](../../coordination/parameter-atlas-20260908/root-replay/RECONCILIATION.json)。

## 发布版勘误

### 1. 修正精确函数名，不修改实现

原稿若干处写成 `CreatePaperH128ClientKeyPair`，实际标识符是 **`CreateFixedQH128ClientKeyPair`**。文件、行范围以及 h128/公钥构造机制的解释正确，错误在函数标签。阅读版在正文、随机路径、参数字典、源码引用字典及覆盖表中同步替换，原稿保持不变。

固定依据：[src/paper_h128_client_keypair.cpp 第 193 行](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/paper_h128_client_keypair.cpp#L193)，以及 `src/repeated_mult2.cpp:453` 的调用。

### 2. NTT 缓存风险不是已发生的算术错误

源码确认：native NTT 的 `PreCompute` 以 modulus 查表，并检查表长是否等于 ringDim，没有比较请求 root；正逆变换读取同一套 modulus 索引表。[固定源码](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/hal/intnat/transformnat-impl.h#L648-L755)

但如果所有参与对象始终一致地使用缓存中同一个有效根 A，正逆变换和环运算仍可能内部自洽，即使对象标注的是另一个根 B。要推出错误，还需要证明混用了不一致的 EV 坐标、不同缓存时期的对象，或依赖 B 的独立消费者。故这项发现是**根身份与生命周期风险**，不是“非最小根一定错误”，也不是当前 S100 失败或 S116 通过失效的证据。

公开整数复核确认 S116 三枚新素数的冻结根有效但非最小根；S100 冻结根为最小根。S100/S116 的这三枚不同根对应的素数也不同，并不能仅凭切换这两套 profile 推出同 q 的缓存冲突。

### 3. OpenMP 的局部 σ 结论必须带构建前提

HYBRID/BV 的 `private(dgg)` 与 DGG 默认构造参数 1.0，确实构成“有效 OpenMP 私有对象不同于外层 context σ”的源码条件性推论。根端另查了 OpenMP 官方私有化规则，与独立审查一致；详见[规则核对](../../coordination/parameter-atlas-20260908/OPENMP_PARAMETER_SEMANTICS.md)。

这不表示已从历史二进制测得 σ=1，更不表示所有 public PKE 噪声都为 1。下一步若选择此项，应先查实际依赖构建身份与宏；不能先降低噪声、改 sampler 或重复抽样追求通过。

### 4. 线程设置、分布适配器与 tag 消耗的进一步限定

以下补充适用于主图谱的 `build.threads`、`rng.fixed_seed`、`rng.binary`、`key.tags` 及随机性文档相应段落；不能用概括性调用图覆盖这些精确条件。

- `ParallelControls` 构造时缓存 `omp_get_max_threads()`；`GetThreadLimit(n)` 使用这个缓存，`SetNumThreads(n)` 只调用 `omp_set_num_threads`，不更新缓存。因此它不是所有显式 `num_threads(GetThreadLimit(...))` 的统一动态上限。[固定源码](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/utils/parallel.h#L51-L125)
- `WITH_OPENMP + FIXED_SEED` 会取消 `m_prng` 的 `threadprivate`，不是只改变种子数组。非 OpenMP 分支仍是 `thread_local`。正常分支的“一线程一引擎”不能不加前提地套到 FIXED_SEED。[固定源码](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/distributiongenerator.h#L73-L79)
- BUG 实际直接使用 `std::bernoulli_distribution(0.5)`；调用图的分布适配器应理解为还包含 Bernoulli，不仅是 uniform integer/real。h128 TUG 确实间接调用 BUG。[固定源码](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/binaryuniformgenerator-impl.h#L48-L53)
- tag 的准确表述是：接收 context 的私钥构造器获取**四个 32 位 uniform-distribution 输出**，形成 32 个十六进制字符；不是所有默认/拷贝/移动构造都会新采 tag，也不在未核标准库实现时承诺恰好消耗四个底层 engine word。[固定源码](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/key/privatekey.h#L57-L97)

上述细化来自[独立随机性审查](../../coordination/parameter-atlas-20260908/ROOT_PRO_RANDOMNESS_REVIEW.md)，根端已读原源码复核；没有测得历史真实线程数或随机流。

### 5. 补齐当前线程请求与参数检索别名

当前固定源码的 `CMakeLists.txt:266–301,326–354` 及 `.github/workflows/dcp-rcb.yml:202–266`，不仅为 annulus，也为原 S100 和 S116 请求 `OMP_NUM_THREADS=2`。已在阅读版正文和 `PARAMETERS.json` 的 `project_request` 中补齐；`project_effective` 仍明确历史实际 team 大小/编译分支待验证。请求并不证明真的只有两个线程，也不证明已有库是按所请求的配置编译。[固定测试配置](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/CMakeLists.txt#L266-L354)

机器字典仍保留 132 条独立记录，另补 `root_lookup_aliases.familyCount` 和 `familyQCount`：分别为 8、`11-f`，活动输入和 RS 后为 `10-f`、`9-f`（f=0…7）。这些事实原本已经在正文及 family/partition 条目中，补检索别名是为了方便找参数，不把派生状态伪装成可任意单独修改的设置。详见[独立参数审查](../../coordination/parameter-atlas-20260908/ROOT_PRO_PARAMETER_REVIEW.md)与[独立参数表](../../coordination/parameter-atlas-20260908/ROOT_PARAMETER_DICTIONARY.tsv)。

## 仍然未知的边界与下一步

论文未公开的 HEaaN 版本、精确采样/输入配置不能从 OpenFHE 默认值补全。原 S100 原门槛失败、S116 改参通过、annulus 改输入且新 key/noise 的单样本通过，保持各自标签。

文档完成后的首选诊断是**先核历史 runner 使用的 OpenFHE 构建来源与有效配置**，再判断 OpenMP 发现是否进入实际路径；缓存项先做同进程 q/root 来源和生命周期检查。只在得到能区分原因的具体问题后，安排 Windows/GitHub 上的最小实验。此文档轮没有新增任何编译、采样、加密、FFT/NTT、CI 或生产源码修改，也不恢复高频定时任务。
