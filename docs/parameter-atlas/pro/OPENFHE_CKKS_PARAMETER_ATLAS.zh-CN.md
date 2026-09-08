# OpenFHE / CKKS 参数、随机数与 Mult² 实现参考图谱

**任务：OPENFHE-CKKS-PARAMETER-RANDOMNESS-ATLAS-01 · 中文源码参考文档 · 2026-09-08**

## 0. 阅读入口、身份与结论边界

本图谱研究的是附件中冻结的 clean-room 实现，不是用户电脑上的旧实现，也不是 OpenFHE 当前最新版。工程基线为 `a4b815a733efe81897325e2a8e4c826a4ebfa439`；上游为 OpenFHE **1.5.0 / `df495ba2e91739a6dc8f1de254fc5a41155ce504`**，目标 native64/backend4。文档分支名仅作为来源记录，不表示本轮访问或写入了远程分支。输入 ZIP SHA-256 为 `abc4df778754a2d3713f1442fbfe34d69af274f3638e65d4d362f35aa93fdd68`。

本轮已做输入字节/清单/Git blob 标识核验、固定源码定点阅读、参数与 setter 词法清点、论文原页目视核对，以及纯整数/Fraction 检查。**没有构建、运行 OpenFHE、FFT/NTT、采样、加密、解密、算法测试或 CI。** 文中的“源码确定”不等于“历史二进制已经确认走这个分支”；“历史 PASS”也不是本轮新 PASS。详细执行账在 `EXECUTION_LEDGER.md`，逐文件范围在 `SOURCE_COVERAGE.tsv`。

给懂密码学、不熟 C++ 的读者：先读第 1、3、5、6、8 节。排查代码时：先确认第 3 节的精确 profile，再沿第 6 节定位步骤；查随机性时转 `RANDOMNESS_PATHS.md`；变更前查 `CHANGE_IMPACT.md`。`PARAMETERS.json` 是机器字典，`PROJECT_SETTER_OCCURRENCES.json` 是逐出现位置的覆盖账，不能把测试里的负控 Set 调用当作有效配置。

最重要的机制区别是：**一个密文同时有 RNS 基、真实归一化 scale、C++ 对象身份/元数据三套状态。三者不能互相代替。** 当前论文路径手工构造 context，常规 `CCParams→GenCryptoContext` 的默认值并不自动到达它；求值结束时的真 scale 是有理数，不能从 `GetScalingFactor()` 单独恢复。

证据记号：**S**＝固定源码；**I**＝本轮纯整数/标量检查；**H**＝包内既有运行回执；**D**＝注明前提的数学/语言语义推导；**U**＝构建、运行或论文未公开信息；**NA**＝本路径未启用。所有代码引用均在文末给出固定提交、路径、函数和物理行范围。原 PDF 的 SHA-256 为 `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`；TXT 含 4 个 NUL，读取时没有把 NUL 当 EOF。

## 1. 用一套不混淆的符号理解系统

### 1.1 环、槽和两种“变换”

取 $R=\mathbb Z[X]/(X^N+1)$，$N=32768=2^{15}$，cyclotomic order 为 $2N=65536$。一个环元素有 **N 个系数**；共轭配对后有 **N/2=16384 个复数槽**。CKKS 的槽嵌入以 $\zeta=\exp(2\pi i/(2N))$ 的奇数次幂为点，当前打包约定选择按 5 的幂生成的顺序；observer 用等价 bin $(5^j-1)/2\bmod N$ 处理正向、未归一化的 twisted DFT。不能把自然编号 j 直接当成 NTT 下标。

有限域 NTT 与复数编码 FFT 是不同的变换：每枚 $q_i$ 各有一个模 $q_i$ 的 $2N$ 阶根 $r_i$，它用于环乘法；复数 $\zeta$ 用于编码与解释槽。修改任意一方的根/符号/排序，都必须检查对应转换的一致性，而不是用“根都有效”代替兼容性检查。[P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [P24](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p24) [T14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t14) [N03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n03) [N04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n04)

N 增大一般会增加安全估计允许的模数预算，同时使向量更长；一次有 L 个塔的环运算至少处理 $LN$ 个字，NTT 型乘法的结构性代价为 $O(LN\log N)$。密文的系数存储随“密文分量数 × 塔数 × N”增长，HYBRID key 还乘上 digit 数与 QP 塔数。这些是结构复杂度推导，不是本轮性能数据。`HEStd_NotSet` 表示没有自动安全查表背书；既不能据此宣布不安全，也不能把论文的安全声明直接移植过来。[U07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u07) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [B01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b01)

### 1.2 符号消歧

| 本文记号 | 含义 | 容易误读的同名项 |
| --- | --- | --- |
| $q_i$ | 数组中一枚精确有序 RNS 素数 | `scalingModSize` 只是普通生成器目标 bit 数 |
| $\mathcal Q_j$ | family j 的完整有序 Q 基，包含末尾 d | 它不是把原 Q 连续丢最后 j 塔的普通前缀 |
| $Q_j$ | 该基中所有素数的整数乘积 | `logQ≈620` 不是一个真实模数 |
| $\overline{\mathcal Q}_j$ | pair 活动基，即 family Q 去掉末尾 d | DCP/后继重入后的 H/L 都在此基 |
| $P$ | HYBRID 辅助基的乘积；当前一枚 60-bit prime | 不是 plaintext modulus，不是 paper 的精度 p |
| $d=q_{\mathrm{div}}$ | 分割高低项的固定除数；40 或 56 bit | 不等于 BV 的 digitSize，也不等于 d_num |
| $\Delta$ | 真正把整数编码映射回复数的正有理 scale | 不是 `scalingFactor` 的兼容记录值 |
| $b$ | `EncodingParams`/metadata 的位数，50 或 58 | 不是每轮 Mult prime 的 60 bit |
| $n_s$ | 源码 `noiseScale` 整数噪声乘子，当前 1 | 不等于明文 scale $\Delta$ |
| $h$ | 根秘密小整数多项式的非零项数，128 | 不是加密临时量 v 的非零项数 |
| $\sigma$ | 离散高斯参数 | 不等于统计安全 λ、HEStd 计算安全级别 |
| $t=2$ | 论文高/低两项表示的元组长度 | HYBRID 代码局部 t=0 是 BGV 修正开关；不是同一个 t |
| $d_{num}$ | 论文 gadget 维度 | root 的 OpenFHE numPartQ 也为 11，但算法定义仍须分别核对 |

来源：[P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) [U09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u09) [U12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u12) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [R18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r18) [A02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-a02)

## 2. 参数是怎样真正进入 context 的

### 2.1 普通常规路径与当前论文路径

普通路径为 `CCParams<CryptoContextCKKSRNS>` → CKKS 参数验证 → `genCryptoContextCKKSRNSInternal` → `ParamsGenCKKSRNS` → CRT/NTT/encoding 预计算 → context。native64 默认是 `FLEXIBLEAUTOEXT`、first=60、scaling=50、secret=`UNIFORM_TERNARY`、type=`REAL`、depth=1、numLargeDigits=0。普通 numLargeDigits=0 按 depth 派生：depth>3 为 3，depth>0 为 2，其余为 1。自动 Q 生成器会选择满足根同余条件的实际素数；`FirstPrime(bits,order)` 与 `LastPrime` 的选取方向不同，目标 bit 数和整数值必须分开记录。[U01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u01) [U04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u04) [U05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u05) [U06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u06) [U07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u07) [U08](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u08) [U22](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u22)

当前 S100/S116 路径为 `Create…Setup` → `MakeFamily`：直接建立给定 `(order,q[],root[])` 的 DCRT 参数、`EncodingParams(b,16384)` 和 `CryptoParametersCKKSRNS`，显式写入安全/分布/scale/深度等字段，调用预计算，安装 HYBRID scheme，启用 PKE、KEYSWITCH、LEVELEDSHE，再取工厂返回的 context 并验证。**不调用普通 Q 参数生成器，也不让其替换冻结的 Q/roots。** P 仍由预计算派生，然后与冻结期望 P/根核对。[P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [P08](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p08)

因此，改变 `SetScalingModSize(…)` 的例程或修改上游默认 `standardDeviation`，并不意味着 paper factory 的 b、Mult primes 或硬编码 3.19f 会改变。反过来，直接 `SetElementParams`、`SetEncodingParams`、`SetStdLevel` 等也不是“自动重建已有密钥/密文/预计算”的事务接口。字段值与已创建对象可能脱节，当前 factory 的验证就是为了拒绝这种脱节。[U14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u14) [U15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u15) [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [P23](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p23)

### 2.2 CCParams 的完整 33 字段与 32 setter 清单

下表默认值只对指定 pin 的 **native64 分支**成立。scheme 由构造器决定，无通用 SetScheme；其余 32 个 setter 均有字典条目。CKKS 特化中 8 个 setter 被禁用，不因底层类仍保存同名字段就变成合法 CKKS 用户接口。全部默认/字段/禁用证据：[U01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u01) [U02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u02) [U03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u03) [U04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u04)；当前有效值证据：[P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03)。

| 字段 | C++类型；符号/单位 | setter | native64默认 | S100有效/位置 | S116有效/位置 | 解释与约束 |
| --- | --- | --- | --- | --- | --- | --- |
| scheme | SCHEME；SCHEME枚举；算法身份，无量纲 | 构造器 | `CKKSRNS_SCHEME` | CKKSRNS_SCHEME | CKKSRNS_SCHEME | CCParams 的 CKKS 构造函数隐式设置；paper 工厂直接指定 CKKS scheme。 |
| ptModulus | PlaintextModulus；p_enc；当前底层CKKS将其用作scale指数位数；不是BFV/BGV明文环模数 | SetPlaintextModulus | `0` | CCParams不使用；底层EncodingParams=50 | CCParams不使用；底层EncodingParams=58 | CKKS SetPlaintextModulus 禁用；不能把这里的50/58理解成q或2^50/2^58。 |
| digitSize | uint32_t；w；BV digit的二进制位数(bit)，0为特殊默认；当前HYBRID不用它作partition数 | SetDigitSize | `0` | 0 | 0 | HYBRID下不控制partition数；legacy BV tests覆盖0及正数窗口。 |
| standardDeviation | float；σ；离散误差在整数系数域的标准差参数，单位为整数系数 | SetStandardDeviation | `3.19f` | 3.19f，即3.190000057220459 | 同S100 | root公钥与payload用context DGG；OpenMP私有evalkey DGG的条件例外见F01。 |
| secretKeyDist | SecretKeyDist；SecretKeyDist枚举；秘密分布族，无量纲；与手工h分开 | SetSecretKeyDist | `UNIFORM_TERNARY` | SPARSE_TERNARY，但手工h128 | 同S100 | 普通KeyGen的稀疏模式为h192；本项目绕过普通秘密生成，v仍走稠密三元。 |
| maxRelinSkDeg | uint32_t；最大secret幂次数，整数阶数；2对应s²→s | SetMaxRelinSkDeg | `2` | 2 | 2 | 本路径EvalMultKeyGen仅s²→s的一行evalkey；不是乘法深度或重线性化次数。 |
| ksTech | KeySwitchTechnique；KeySwitchTechnique枚举：BV/HYBRID | SetKeySwitchTechnique | `HYBRID` | HYBRID | HYBRID | MakeFamily同时设置参数和scheme实现；两者必须一致。 |
| scalTech | ScalingTechnique；ScalingTechnique枚举：manual/auto等；非数值Δ | SetScalingTechnique | `FLEXIBLEAUTOEXT` | FIXEDMANUAL | FIXEDMANUAL | 必须绕开自动对齐/额外rescale；仍复用真实ModReduce算术。 |
| firstModSize | uint32_t；首个生成Q塔的请求位数(bit)；手工表不经此生成器 | SetFirstModSize | `60` | 未调用普通Q生成器；Base0实际50位 | 未调用普通Q生成器；Base0实际58位 | 不是用SetFirstModSize产生两枚Base；冻结数组优先。 |
| batchSize | uint32_t；编码复数槽个数；正整数计数或0自动值 | SetBatchSize | `0` | 16384 | 16384 | 等于N/2；客户端和fixture严格要求全槽。诊断N64/16的gap为2。 |
| numLargeDigits | uint32_t；HYBRID大digit/partition个数；整数计数或0自动值 | SetNumLargeDigits | `0` | 每family=当前完整Q塔数11→4 | 同S100 | 普通默认0会按depth派生3/2/1；paper不走此默认，alpha=1。 |
| multiplicativeDepth | uint32_t；请求可消费level数；非当前密文level，也非本任务轮数 | SetMultiplicativeDepth | `1` | root10；family10→3 | 同S100 | 八个平方不等于CCParams depth8：11塔包含2Base+8Mult+Div。 |
| scalingModSize | uint32_t；普通生成器的scale/模数请求位数(bit)；手工profile另表 | SetScalingModSize | `50` | 未生成Q；EncodingParams/元数据50 | 未生成Q；EncodingParams/元数据58 | 实际每轮Mult均60位，不能用这个字段替代q_l。 |
| securityLevel | SecurityLevel；SecurityLevel枚举；HEStd_NotSet不提供安全位数证明 | SetSecurityLevel | `HEStd_128_classic` | HEStd_NotSet | HEStd_NotSet | 禁用自动安全认证不等于证明不安全，也不等于128位安全；需独立估计。 |
| ringDim | uint32_t；N；环系数个数，正整数维数或0自动值 | SetRingDim | `0` | 32768 | 32768 | 工厂直接构造order65536；改变N会使roots、slots、keys与安全估计失效。 |
| evalAddCount | uint32_t；预期加法次数；计数，当前CKKS setter禁用 | SetEvalAddCount | `0` | 未启用/CKKS setter禁用 | 同S100 | 不是当前电路中真实执行的加法次数。 |
| keySwitchCount | uint32_t；预期key-switch次数；计数，当前CKKS setter禁用 | SetKeySwitchCount | `0` | 未启用/CKKS setter禁用 | 同S100 | 实际evalkey消费由Relin2调用决定。 |
| encryptionTechnique | EncryptionTechnique；EncryptionTechnique枚举；当前STANDARD占位，不是公钥/私钥Encrypt开关 | SetEncryptionTechnique | `STANDARD` | STANDARD | STANDARD | CKKS CCParams setter禁用；底层手工构造保留STANDARD，GetParamsPK=Q。 |
| multiplicationTechnique | MultiplicationTechnique；MultiplicationTechnique枚举；HPS等BFV/RNS路径选择，当前占位 | SetMultiplicationTechnique | `HPS` | HPS兼容字段 | HPS兼容字段 | 不表示本项目使用BFV HPS乘法；CKKS setter禁用。 |
| PRENumHops | uint32_t；代理重加密跳数；整数计数，当前未启用 | SetPRENumHops | `0` | 未启用/CKKS setter禁用 | 同S100 | 不要与密文复制保留的HopLevel混为当前计算层级。 |
| PREMode | ProxyReEncryptionMode；ProxyReEncryptionMode枚举；当前NOT_SET | SetPREMode | `NOT_SET` | NOT_SET | NOT_SET | 切换会影响GetParamsPK/辅助基及安全模型；当前无PRE。 |
| multipartyMode | MultipartyMode；MultipartyMode枚举；当前未启用多方协议 | SetMultipartyMode | `FIXED_NOISE_MULTIPARTY` | FIXED_NOISE_MULTIPARTY占位 | 同S100 | CCParams setter禁用且当前单钥单客户端；不代表执行了多方协议。 |
| executionMode | ExecutionMode；ExecutionMode枚举；EXEC_EVALUATION/噪声估计阶段 | SetExecutionMode | `EXEC_EVALUATION` | EXEC_EVALUATION | EXEC_EVALUATION | COMPLEX配EXEC_NOISE_ESTIMATION被普通验证拒绝。 |
| decryptionNoiseMode | DecryptionNoiseMode；DecryptionNoiseMode枚举；FIXED或FLOODING | SetDecryptionNoiseMode | `FIXED_NOISE_DECRYPT` | FIXED_NOISE_DECRYPT | FIXED_NOISE_DECRYPT | 此Poly*路径不加flooding；不等于解密结果没有原加密噪声。 |
| noiseEstimate | double；噪声幅度的log2估计(bit指数)；普通flooding公式中进入2的指数，不是σ本身 | SetNoiseEstimate | `0.0` | 0.0，明确覆盖 | 0.0，明确覆盖 | 当前fixed分支不使用；不是实测误差E0/E8。 |
| desiredPrecision | double；目标明文精度位数(bit)，不是十进制digits | SetDesiredPrecision | `25.0` | paper手工工厂未消费此CCParams字段 | 同S100 | 默认25不限制本项目2^100/2^116的自定义编码；不是算法有效精度保证。 |
| statisticalSecurity | uint32_t；统计安全参数位数(bit)；不同于HEStd计算安全级别 | SetStatisticalSecurity | `30` | 30，占位并验证 | 30，占位并验证 | 不是HEStd计算安全128；普通验证对非默认值有额外限制。 |
| numAdversarialQueries | uint32_t；对抗解密查询数；整数计数，不是slots或实验样本量 | SetNumAdversarialQueries | `1` | 1，占位并验证 | 1，占位并验证 | 不是本实验采样数量；不得把16384slots代入。 |
| thresholdNumOfParties | uint32_t；阈值协议参与方数量；整数计数 | SetThresholdNumOfParties | `1` | 1，占位 | 1，占位 | CCParams setter禁用；当前没有multiparty对象。 |
| interactiveBootCompressionLevel | CompressionLevel；CompressionLevel枚举；交互式bootstrap压缩策略 | SetInteractiveBootCompressionLevel | `SLACK` | SLACK，占位 | SLACK，占位 | 未启用bootstrap；不影响当前terminal RCB。 |
| compositeDegree | uint32_t；每组composite-scale素数个数；整数计数 | SetCompositeDegree | `1` | 1 | 1 | 当前每次RS只丢一枚Mult；改成2会改变level/noise及多个tower语义。 |
| registerWordSize | uint32_t；配置寄存器字长(bit)；不能改变已编译NativeInteger类型 | SetRegisterWordSize | `NATIVEINT (native64=64)` | 64 | 64 | 与编译NATIVEINT=64分别记录；不是cpp_dec_float精度。 |
| ckksDataType | CKKSDataType；CKKSDataType枚举：REAL/COMPLEX | SetCKKSDataType | `REAL` | COMPLEX | COMPLEX | 普通默认REAL；当前四相复数输入要求COMPLEX，且绕过普通REAL解码噪声。 |

“占位/不适用”不是遗漏：BFV/BGV 乘法技术、PRE、多方/交互式 bootstrap、scheme-switch、噪声估计阶段不在当前启用链内。CCParams 中 `desiredPrecision=25` 不代表本项目仅有 25 bit 精度；这里没有执行依赖该字段的噪声淹没配置。`statisticalSecurity=30`、`numAdversarialQueries=1` 也不是样本次数或 RLWE 的 128-bit 计算安全声明。[U05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u05) [U06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u06) [U17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u17) [U18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u18) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03)

### 2.3 CryptoContext本身的字段与setter，不与CCParams混为一表

`CryptoContextImpl` 自有4个实例状态：m_params、m_scheme、m_schemeId、m_keyGenLevel；前两者默认空、schemeId默认INVALID_SCHEME、keyGenLevel默认0。另有按keytag索引的静态evalMult与evalAutomorphism map；DEBUG_KEY条件下还有privateKey成员，当前project用编译期#error禁止它。完整词法核对结果在 `CONTEXT_API_SURFACE.json`。[U27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u27) [U28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u28) [P30](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p30)

其Set命名方法共7个：内部SetKSTechniqueInScheme（从params同步scheme）；SetPrivateKey（仅DEBUG_KEY，当前禁止）；SetKeyGenLevel（保存future-use字段，未作family选择器）；SetCKKSBootCorrectionFactor；SetParamsFromCKKSCryptocontext（向SchSwchParams导出首模数/N/scaling/batch）；SetBinCCForSchemeSwitch；SetSwkFC。后4项属于未启用bootstrap或scheme-switch表面。Enable是两个重载，委托scheme安装功能；Insert/Clear evalkey与序列化属于对象生命周期，不是独立的精度或随机参数。[U16](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u16) [U17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u17) [U18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u18) [U29](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u29) [U26](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u26)

## 3. 精确 Q/P、roots、基变化与 scale

### 3.1 精确常数表

下表 **index 是源码数组顺序，不可排序后替换**。S100 的 Base 为 50×2，Mult 为 60×8，Div 为 40；S116 为 Base58×2、相同 Mult60×8、Div56。P 相同。`root` 是项目冻结值，不是“给定 q 后可随意选择的等价元数据”。[P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [T01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t01) [T05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t05)

| profile | index | role | 精确 q / P | bit_length | 冻结 root |
| --- | --- | --- | --- | --- | --- |
| S100 | 0 | Base0 | 1125899904679937 | 50 | 26113207984 |
| S100 | 1 | Base1 | 1125899903827969 | 50 | 150640639383 |
| S100 | 2 | Mult0 | 1152921504598720513 | 60 | 100545759574150 |
| S100 | 3 | Mult1 | 1152921504597016577 | 60 | 31693996050849 |
| S100 | 4 | Mult2 | 1152921504595968001 | 60 | 88651361085495 |
| S100 | 5 | Mult3 | 1152921504595640321 | 60 | 9679305630873 |
| S100 | 6 | Mult4 | 1152921504593412097 | 60 | 24428769072221 |
| S100 | 7 | Mult5 | 1152921504592822273 | 60 | 18776242964106 |
| S100 | 8 | Mult6 | 1152921504592429057 | 60 | 5821397352863 |
| S100 | 9 | Mult7 | 1152921504589938689 | 60 | 33888991361320 |
| S100 | 10 | Div | 1099510054913 | 40 | 121567553 |
| S100 | 11 | P | 1152921504606584833 | 60 | 4443670208963 |
| S116 | 0 | Base0 | 288230191468118017 | 58 | 43136605093011213 |
| S116 | 1 | Base1 | 288230165698314241 | 58 | 82872750907637397 |
| S116 | 2 | Mult0 | 1152921504598720513 | 60 | 100545759574150 |
| S116 | 3 | Mult1 | 1152921504597016577 | 60 | 31693996050849 |
| S116 | 4 | Mult2 | 1152921504595968001 | 60 | 88651361085495 |
| S116 | 5 | Mult3 | 1152921504595640321 | 60 | 9679305630873 |
| S116 | 6 | Mult4 | 1152921504593412097 | 60 | 24428769072221 |
| S116 | 7 | Mult5 | 1152921504592822273 | 60 | 18776242964106 |
| S116 | 8 | Mult6 | 1152921504592429057 | 60 | 5821397352863 |
| S116 | 9 | Mult7 | 1152921504589938689 | 60 | 33888991361320 |
| S116 | 10 | Div | 72057589742960641 | 56 | 50608680790172261 |
| S116 | 11 | P | 1152921504606584833 | 60 | 4443670208963 |

本轮 I 检查：所有 q/P 两两互素、满足 $q\equiv1\pmod{65536}$、冻结根满足 $r^N=-1$ 和 $r^{2N}=1$；使用固定公开基数的 Miller–Rabin 标量筛查；S116 三枚新素数另核对了 Proth 证书。结果与独立 fixture 中的常数逐项匹配。所有计算和精确大整数在 `checks/scalar_constants.json`，没有调用上游的随机素数/根生成器。

S100 所有冻结根都是其素数下最小的有效 $2N$ 阶根；S116 的 Base0/Base1/Div 冻结根有效但**不是**最小根。上游 `RootOfUnity` 会选到最小根；这不构成当前工厂错误，因为工厂显式传冻结根。但它意味着“重新调用 RootOfUnity 恢复同一 root”对这三枚值不成立。NTT 缓存又仅以 modulus/长度判断是否已有表，详见 FINDINGS F02。[U20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u20) [N04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n04) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03)

| profile | 完整 Q 的 bit_length / 近似 log2 | QP 的 bit_length / 近似 log2 | DCP 后塔数 | 最终基 |
| --- | --- | --- | --- | --- |
| S100 | 620 / 619.9999979294503 | 680 / 679.9999979294499 | 10 | Base0, Base1；乘积约100bit |
| S116 | 652 / 651.9999979360838 | 712 / 711.9999979360836 | 10 | Base0, Base1；乘积约116bit |

`bit_length(Q)` 是整数二进制长度；`log2(Q)` 是实数；把每枚位数相加得到的是名义预算。三者在这里接近但不可替代。P 参与 key switching 的公开模数和 key 存储，安全估计不能只看活动 pair 的 Q。S116 的 QP≈712 明确不同于论文 Table 3 的 680。[A02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-a02) [U09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u09) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17)

### 3.2 八个 family 的共同根秘密与塔序

以根数组 $q_0,\ldots,q_9,d$ 表示。family j（j=0,…,7）的完整 Q 基为
$$\mathcal Q_j=(q_0,\ldots,q_{9-j},d),\qquad \overline{\mathcal Q}_j=(q_0,\ldots,q_{9-j}).$$
它们的 Q 塔数依次 11,10,9,8,7,6,5,4；pair 在一次 RS 前为 10→3 塔，RS 后为 9→2 塔。每次 RS 丢活动基最后一枚 **Mult**：q9、q8、…、q2。d 只在最初 DCP 中从密文活动基剥离，在每个 family 的完整 context 中始终保留最后位置，供 Relin2 的 d 倍高项提升使用。**不应把“d 在 full-Q 的最后”简化为“每轮 RS 误丢 d”。**[P04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p04) [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) [P15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p15) [P19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p19) [P20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p20)

根秘密只生成一个；派生 family 的秘密通过匹配 `(q,root,order)` 投影同一根秘密的对应塔，不重新选择小整数秘密。每个 family 再各自产生 s²→s 的评估密钥。当前 numPartQ 等于该 family 完整 Q 塔数，$\alpha=\lceil |Q|/numPartQ\rceil=1$，每 family 的 evalkey A/B 数组长度分别为 11→4；全部八个 family 合计各 60 个多项式，但它们的 QP 基长度不同。不能把根评估密钥随便改 tag 后当作所有 family 的 key。[P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [U09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u09) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r20)

### 3.3 真实 scale 的八步精确递推

初始 $\Delta_0=2^{100}$ 或 $2^{116}$。第 r 轮（1…8），$m_r=q_{10-r}$：
$$\Delta_r=\frac{\Delta_{r-1}^2}{d\,m_r},\quad
\Delta_{r,Tensor}=\Delta_{r,Relin}=\frac{\Delta_{r-1}^2}{d}.$$
闭式为
$$\Delta_r=\frac{2^{s2^r}}{\prod_{k=1}^{r}(d\,q_{10-k})^{2^{r-k}}},\quad s\in\{100,116\}.$$
源码用 `cpp_int` 分子/分母存储并约分；八步递推与闭式本轮已逐步核对相等。下表小数只为阅读；精确分数才是归一化依据。[P04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p04) [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) [P28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p28)

| 轮r | family j | full-Q塔数 | pair塔数前→后 | 丢弃Mult整数 | log2 Δr S100 | log2 Δr S116 |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | 0 | 11 | 10→9 | 1152921504589938689 | 100.000002063813 | 116.000000086012 |
| 2 | 1 | 10 | 9→8 | 1152921504592429057 | 100.000006191436 | 116.000000258034 |
| 3 | 2 | 9 | 8→7 | 1152921504592822273 | 100.000014446682 | 116.000000602078 |
| 4 | 3 | 8 | 7→6 | 1152921504593412097 | 100.000030957173 | 116.000001290163 |
| 5 | 4 | 7 | 6→5 | 1152921504595640321 | 100.000063978152 | 116.000002666332 |
| 6 | 5 | 6 | 5→4 | 1152921504595968001 | 100.000130020109 | 116.000005418668 |
| 7 | 6 | 5 | 4→3 | 1152921504597016577 | 100.000262104024 | 116.000010923341 |
| 8 | 7 | 4 | 3→2 | 1152921504598720513 | 100.000526271848 | 116.000021932683 |

由于实际 d 和 Mult 小于相应的 2 的幂，终端 scale 略大于名义值，而不是正好 $2^{100}$/$2^{116}$。八平方会把前面各轮的相对 scale 偏差反复平方传播。把终端输出一律除以名义二次幂，会制造测量偏差；反之把 metadata 改成某个好看的 double，也不会自动改变底层整数算术。

### 3.4 元数据账不是 scale 权威

| 阶段 | 真实 scale | `scalingFactor` 记录 | `noiseScaleDeg` | family 局部 level |
| --- | --- | --- | --- | --- |
| fresh 根密文 | $2^{2b}$ | $2^{2b}$ | 2 | 0 |
| DCP / 每轮 input | 当前 $\Delta_{r-1}$ | $2^{2b}$ | 2 | 1 |
| Tensor2 | $\Delta_{r-1}^2/d$ | $2^{3b}$ | 3 | 1 |
| Relin2 | 同 Tensor | $2^{3b}$ | 3 | 1 |
| RS2 | $\Delta_{r-1}^2/(dm_r)$ | $2^{2b}$ | 2 | 2 |
| 重入下一 family | 同刚才 RS2 | $2^{2b}$ | 2 | 1 |
| terminal RCB 包装到根 context | $\Delta_8$ | $2^{2b}$ | 2 | 根 context 的 9 |

source 的 EvalMultNoRelin 原始乘积会先带出 degree4/scale乘积，随后 Tensor2 手工更新为表中的兼容状态。这个 metadata 更新不是一次额外的真除模，也不是把 d 当 $2^b$；真 d 已在双项算法的语义中出现。FIXEDMANUAL 下 `GetScalingFactorReal()` 和 `GetModReduceFactor()` 的近似值为 $2^b$，实际模降仍使用真实最后一个 q 的预计算逆元。[P17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p17) [P20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p20) [P28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p28) [U11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u11) [U12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u12) [N05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n05)

## 4. 随机性：一次公开加密到底引入了什么

### 4.1 实际根 key 与 payload 公式

令 $s$ 为根秘密、$a$ 为均匀 RNS 元素、$e_{pk}$ 为小高斯多项式、$n_s=1$。本项目 custom root 公钥由 secret-key `EncryptZeroCore` 重载产生：
$$pk=(a s+n_s e_{pk},\ -a).$$
这与普通 OpenFHE `KeyGenInternal` 的 $(n_s e-a s,a)$ 是符号等价的合法约定，但**不能在追踪实际随机量时混用同一个 a 的符号**。根秘密来自 TUG 的 h=128 分支，不是普通 SPARSE_TERNARY KeyGen 的 h=192。[P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) [R13](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r13) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14)

真正 payload 用 public Encrypt。当前 secret 分布枚举不是 GAUSSIAN，所以 v 由 **h 参数为 0 的稠密三元**采样，而不是 h128；另生成新 e0/e1：
$$c_0=pk_0v+n_s e_0+m,\qquad c_1=pk_1v+n_s e_1,$$
$$c_0+c_1s=m+n_s(e_{pk}v+e_0+e_1s).$$
`e_pk` 随公钥长寿命保存，v/e0/e1 每次 payload 新采。每个小多项式先在整数系数上采一份，再跨所有塔取模；均匀 a 才是每塔独立均匀向量。因此按塔重新采小噪声不只是改变随机种子，而是破坏了现有“小整数 RLWE 噪声”的数学对象。[R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14)

### 4.2 从种子到底层算法的摘要

默认入口是 `PseudoRandomNumberGenerator::GetPRNG`，每线程惰性取得引擎；默认算法为用 512-bit seed 作 key、64-bit counter 作输入的 BLAKE2Xb，输出缓冲 1024 个 32-bit word。seed 源码组合了 time/thread/address 派生流和 `std::random_device` 的 16 个 word；后者连续三次异常失败即抛出，不会在异常失败后静默用时间种子继续。确切 OS 熵 API 由实际 C++ 标准库决定，包内源码不能证明它是哪个系统调用。FIXED_SEED 与替代引擎是可选构建/初始化分支，不能仅凭默认源码宣称历史运行已经排除它们。[R01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r01) [R02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r02) [R03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r03) [R04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r04) [R05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r05) [R06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r06)

PKE 的 context DGG 以 float `3.19f` 创建；转 double 后是 **3.190000057220459**。当前小 σ 路径使用有限表 Peikert 反演，而非 Karney：权重 $\exp(-x^2/(2\sigma^2))$，表长 $\lceil12.00610553538285\sigma\rceil=39$。稀疏 TUG 除恰好 h 个非零支持外，还拒绝正项数不在 h/2±1 的样本，所以 h128 根秘密的正项数只可能是 63、64、65。参数 h 与实际分布约束应一起记录。[R07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r07) [R10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r10) [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12)

**条件性新发现 F01：** HYBRID evalkey 生成的 OpenMP 循环声明 `private(dug,dgg)`。标准语义会默认构造线程私有 DGG；该类默认 σ=1，而不是复制 context 的 σ。若 pragma 有效，则循环内 σ 条件值是 1、表长13；若 pragma 被忽略，则使用外部复制的 context DGG。这里没有历史二进制分支鉴证，也没有新采样来证明运行采用哪一种。BV 有同类 private 写法，不能把切换 BV 当成已经验证的解决方案。完整前提、影响与未决处见 `FINDINGS.md`。[R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r19) [R10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r10) [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) [R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12)

### 4.3 “同 seed”不是完整实验身份

公开素数的 Miller–Rabin 验证、P 的预计算、根搜索、PrivateKey 的随机 tag 都会消费同一 PRNG 系统。测试在真实 paper setup 前还可能创建小 N 外来 context。线程惰性初始化时间、调度、标准库分布实现和拒绝次数都会改变后续消耗序列。即使有相同的一个 seed，也不能据此保证跨线程/平台、不同 factory 调用顺序得到同一秘密、同一 v/e 或同一密文。随机性与保密记录规范详见 `RANDOMNESS_PATHS.md`；本轮不生成或导出任何真实种子、私钥或私有噪声。[U19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u19) [U20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u20) [U21](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u21) [U23](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u23) [R22](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r22) [R23](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r23) [T08](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t08)

## 5. 高精度数值边界：不把 binary64 当作任意精度

当前客户端公开类型 `ClientReal` 是 **100 十进制位**，不是 100 个二进制 bit。内部编码/解码分别在 160 和 220 十进制位重算复数根和变换；明文 oracle 常用 512 binary bit，端点 observer 为 512/768 binary bit。这些精度分别属于输入 API、算法边界和诊断，不是 N、q、σ 一类的 RLWE 参数。[P29](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p29) [P22](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p22) [T01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t01) [T12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t12)

编码按 OpenFHEPackedStride 的复数嵌入反变换，然后乘精确 scale 并舍入为整数。实现不依赖 binary64 的预缓存根来保留高精度输入。StableRound 比较两种工作精度，危险半整数区间取 `max(16×crossDifference,2^-400)`，另有足够有效位的 2^-410 防护；两次舍入必须得到同一整数。目标约定为最近整数、精确半整数向下，但靠近半整数的输入实际被拒绝，所以不能声称所有 tie 都已经由运行覆盖。整数写入 DCRT 前还拒绝 $2|m_i|\ge Q$。[P24](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p24) [P25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p25)

高精度客户端把大整数通过 decimal string 与 backend4 BigInteger 桥接并核对往返，然后构造 DCRT 明文，调用公开 PKE。它**绕过了**普通 `MakeCKKSPackedPlaintext→CKKSPackedEncoding::Encode` 的 double complex FFT / native64 llround。不是把普通 double 编码的结果事后装进 multiprecision 就得到了高精度。[P25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p25) [P27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p27) [N06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n06) [N07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n07)

解密仍调用 scheme 的 `Decrypt(...,Poly*)`，而不是直接调用 DecryptCore 从而无视解密模式；当前 `FIXED_NOISE_DECRYPT` 不额外淹没噪声。CRT lift 取中心代表，再按绑定的精确有理 scale 正向复数解码。terminal 只接受 owning plan 的终端 receipt；生产解码还比较 160/220 结果并要求分量差不超过 2^-120。普通上游 REAL Decode 中可能添加的连续正态噪声是另一条路径，当前 custom COMPLEX 解码不执行它。[P26](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p26) [P28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p28) [R16](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r16) [N08](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n08) [N09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n09)

`centeredHeadroom=Q/2−max|centered coefficient|` 只是“已选中心代表”的余量。它本身不能证明某个更大的未模约简理想整数没有 wrap；任何非 wrap 结论仍须有独立的量值界或直接证据。类似地，两种数值计算相近不自动保证两者都正确：observer 使用独立根生成、精确整数转换、正向顺序控制和额外稀疏参考，就是为减少共享错误。[P28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p28) [T13](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t13) [T14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t14) [T15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t15) [T16](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t16) [T17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t17)

## 6. 完整执行步骤与不变量

下表描述固定源码的执行逻辑，不是本轮运行。CF=coefficient，EV=evaluation/NTT；在多项式乘法/求值侧主要保存 EV，在取整/中心 lift 时临时切 CF。公开/秘密边界指可公开给求值端的内容，不等于可以公开客户端解密残差或捕获数据。

| 步骤 | 输入→输出 | 表示/基/scale | 随机还是确定 | 边界/不变量 | 来源 | 既有测试定位 |
| --- | --- | --- | --- | --- | --- | --- |
| 0 profile/factory | 公开常数 → 八个context/P/QP表 | Q_j完整基；roots/α/σ/technique | P和MR/root验证可能消耗PRNG；Q冻结 | 无secret；返回instance须live验证 | [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [U09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u09) [U11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u11) | paper_h128_client_keypair_contract_test / experimental_precision116_profile_seam |
| 1 根secret/public key | 新h128 s → rootSK/rootPK | s与PK完整根Q，EV；n_s=1 | s,a,epk随机；h正项数条件 | s仅客户端；PK可给求值端 | [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) [R07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r07) [R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) | paper_h128_client_keypair_contract_test |
| 2 family/evalkeys | 根secret投影 → 同根局部SK；s²→s key | Q_j→QP_j；α1；每family11→4digits | 投影确定；tag/a_i/e_i随机 | 只发布evalkey；不向plan存secret | [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r20) [R22](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r22) | repeated_mult2_semantic_two_square_test / paper_h128_client_keypair_contract_test |
| 3 高精度输入/编码 | 原始复数 → 取整m∈R_Q | CF整数再EV；Δ0=2^100/116 | 编码确定；160/220dec同舍入 | 客户端明文；2\|m_i\|<Q；phase/stride一致 | [P24](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p24) [P25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p25) [P29](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p29) | precision_client_io_first_mult2_contract_test |
| 4 public Encrypt | m,PK → fresh c=(c0,c1) | Q_root，EV，level0/degree2/metadataΔ0 | v dense TUG；e0/e1新DGG | 求值端仅ct；聚合误差epkv+e0+e1s | [P27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p27) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) | paper_full_eight_square_contract_test / s100_annulus125_eight_square_test |
| 5 DCP | fresh ct → pair(H,L) | 活动Q去d，EV；level1，degree2 | 确定；每分量centered÷d | RCB(DCP(c))=c mod活动Q | [P15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p15) [N01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n01) | dcp_rcb_test / repeated_mult2_semantic_two_square_test |
| 6 Tensor2×8 | pair平方 → 高/低各3分量 | 相同活动Q/EV；Δ²/d；metadata2^(3b) | 确定；三次EvalMultNoRelin；省略LL | 只用公开ct；s²项等待relin | [P17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p17) | paper_full_eight_square_contract_test / experimental_precision116_eight_square_test |
| 7 Relin2×8 | 3分量pair+evalkey → 2分量pair | 高项临时恢复d零塔；内部QP；输出活动Q | 本次求值确定；噪声来自预生成key/基转换 | 不访问secret；验证key family/tag/基 | [P18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p18) [P19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p19) [R18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r18) [R21](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r21) | dcp_rcb_test / repeated_mult2_semantic_two_square_test |
| 8 RS2×8 | H,L → H′,L′ | 活动基丢当前最后Mult；level2；Δ/(Mult) | 确定；两次真实centered除模 | RCB输出=RS(RCB输入) | [P20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p20) [N01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n01) [N05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n05) | paper_full_eight_square_contract_test / experimental_precision116_eight_square_test |
| 9 中间重入×7 | 刚RS的pair → 下一family wrapper | 相同剩余coeff/EV；换context/tag，局部level1 | 确定；不重新采样、不再DCP | root secret一致；receipt父子与owning plan一致 | [P09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p09) [P30](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p30) | repeated_mult2_semantic_two_square_test / paper_full_eight_square_contract_test |
| 10 RCB | 最后pair → terminal root-wrapper ct | 仅2枚Base；root level9；Δ8 exact | 确定；dH+L | 不能把receipt误当加密真实性证明 | [P21](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p21) [P28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p28) [P30](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p30) | experimental_precision116_eight_square_test / s100_annulus125_eight_square_test |
| 11 Decrypt/Decode | terminal ct,rootSK → centered Poly → complex | CF CRT lift，dual-decimal transform，÷Δ8 | 当前FIXED不新采flooding；计算确定 | 仅客户端；不泄露raw secret/noise | [P28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p28) [R15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r15) [R16](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r16) | precision_client_io_first_mult2_contract_test / paper_endpoint_observer tests |
| 12 误差/observer | 原z、实际ztilde、精确scale → E/I/A/decision | 512/768binary或512Horner；明确norm | 确定；不是把fresh值改成原oracle | 控制输入域、全部slots、三值判决、独立anchors | [T02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t02) [T03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t03) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09) [T12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t12) [T17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t17) | paper_full_eight_square_contract_test / s100_annulus125_eight_square_test |

### 6.1 DCP 的正确理解

对原 ciphertext 的每个分量 $c$，DCP 计算近似中心除法 $H=\lfloor c/d\rceil$，再令 $L=[c-dH]_{\bar Q}$。在活动基中有严格模等式
$$[dH+L]_{\bar Q}=[c]_{\bar Q}.$$
该等式不声称恢复原完整 Q 上被去掉的 d 塔。解密多项式的高低项还含有 carry：论文写作 $H(s)=\widehat m+I$、$L(s)=\bar m-dI$，因此“低项”并不一定是单独很小的明文噪声。给 low 任意重新编码、清零或重新加密，都会改变原算法。[P15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p15) [A03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-a03)

### 6.2 Tensor2、Relin2 与 RS2 的代数

设两个 pair 为 (H1,L1)、(H2,L2)。Tensor2 的三分量乘积是
$$H_T=H_1H_2,\quad L_T=H_1L_2+L_1H_2,$$
$$dH_T+L_T=\frac{(dH_1+L_1)(dH_2+L_2)-L_1L_2}{d}.$$
故有意略去低低项不是偶然漏乘；其误差和 $\Delta^2/d$ scale 必须共同解释。这里的乘法先是 ciphertext 张量，得到 s² 分量。[P17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p17)

Relin2 不能只把 H 和 L 各自随便 key-switch 完事：它先把 dH 置入 family 的完整 Q 基——活动塔中是 dH，**d 塔严格为零**——再普通 relinearize；DCP 拆出新的高项和 remainder；低项 relinearize 后与 remainder 相加。HYBRID 的 P 基是在内部 key switching 过程中另行使用，不能与这次“恢复 d 塔的零倍数提升”混为 generic ModUp。[P18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p18) [P19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p19) [R18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r18)

RS2 对当前活动基最后 Mult $m$ 执行
$$H'=\operatorname{RS}_m(H),\qquad W'=\operatorname{RS}_m(dH+L),\qquad L'=W'-dH'.$$
因此 $dH'+L'=\operatorname{RS}_m(dH+L)$ 严格成立于剩余基。代码的 `DropLastElementAndScale` 按最后塔的中心余数及逆元实现真实除法；不是简单删除塔，也不是只改 metadata。[P20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p20) [N01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n01)

### 6.3 与常规 OpenFHE EvalMult/Rescale 的差别

| 问题 | 常规 CKKS 路径 | 当前 Mult² 路径 |
| --- | --- | --- |
| 明文入口 | double 编码/普通 Plaintext | dual-decimal 编码，直接 DCRT public Encrypt |
| 自动对齐 | 由 scaling technique 的 AdjustForMult 等决定 | factory 锁 FIXEDMANUAL；严格相同 basis/metadata 输入 |
| 中间表示 | 一条普通 ciphertext | 高/低 pair；Tensor 时每项三分量 |
| 主乘法 | 全部张量项和普通重线性化 | 有意缺 L1L2；Relin2 的 dH 零塔提升+carry处理 |
| 模降 | 每次内部按末尾塔/技术选择处理 | RS2 对 H 和 RCB(H,L) 各做一次真模降 |
| 真 scale 权威 | 库 technique 所维护的规则 | plan 的 ExactScale + receipt；metadata只兼容 |
| 换 context | 常规 levels 通常同一个参数链前缀 | 每轮切换到保留 d 的另一个同根 family |
| keys | 同 context 的 evalkey map | 每 family 的独立 QP key，但共用根秘密 |
| 解码/误差 | 常规 Decode 的double限制和模式 | client Poly* +精确fraction；独立observer/anchors |

来源：[U24](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u24) [U25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u25) [N05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n05) [N06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n06) [N09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n09) [P09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p09) [P17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p17) [P19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p19) [P20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p20) [P28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p28)。其中 U24/U25 说明普通路径，而不是声称本轮执行了它。

## 7. 手工参数、编译设置与诊断配置工程表

下表是 CCParams 以外相关表面的逐项入口，补足“只列 N/q”的不足。每一项的设置、派生/校验、消费点及对精度/噪声/安全/性能/兼容性的分项说明都在 `PARAMETERS.json`；这里保留可读的有效值与联动约束。对 prime 的逐项字典另有 24 条记录（S100/S116 各 11 枚 Q+1 枚 P）。

| 字典ID | 含义 | 符号/单位 | S100/original | S116 | 联动与限制 | 来源 |
| --- | --- | --- | --- | --- | --- | --- |
| ring.order | 分圆阶M=2N；不是槽数 | M=65536 | 65536 | 65536 | N、φ和所有root必须一起变；q≡1 modM。 | [P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [N04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n04) |
| ring.logN | N的二进制指数 | log2 N | 15 | 15 | logN不是整数位宽；用于复杂度/索引。 | [P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) |
| ring.polynomial | 环、系数长度与乘法关系 | R=Z[X]/(X^N+1) | N=32768；negacyclic | 同S100 | x^N=-1；稀疏oracle卷积跨N符号翻转。 | [N03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n03) [T03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t03) |
| ring.embedding | canonical embedding及复数槽序 | ζ=e^(2πi/(2N)); slot_j evaluates at ζ^(5^j) | 16384个复数槽；正号 | 同S100 | NTT bit-reversed顺序不是明文slot顺序；最终observer bin=(5^j mod2N−1)/2。 | [P24](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p24) [T14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t14) |
| ring.gap | 稀疏打包系数间隔 | gap=N/(2·slots) | 1 | 1 | N64/slots16诊断gap2，不能用于paper槽次序证明。 | [P23](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p23) [P25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p25) |
| basis.orderedQ | 有序完整RNS基，不只乘积 | [(q_i,root_i,2N)] | Base50×2,Mult60×8,Div40 | Base58×2,Mult60×8,Div56 | 顺序决定DropLast；按(q,root,order)而不是位置猜测投影。 | [P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) |
| basis.Q | 完整Q及活跃Q_l | 整数乘积 | root620bit；实际log2见scalar JSON | root652bit；实际log2见scalar JSON | Q没有浮点舍入；bit_length不是精确log2；terminal只剩两Base。 | [P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [P04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p04) [N01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n01) |
| basis.P | HYBRID辅助RNS基 | 整数P | 1152921504606584833，60bit单塔 | 同S100 | P不是明文p；只供key switching升/降基，不属于pair活跃Q。 | [U09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u09) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r18) |
| basis.QP | 最大evalkey模数暴露 | Q·P | 680bit；log2≈679.999997929450 | 712bit；log2≈711.999997936084 | 只有数字对齐不是128位安全证书。 | [P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [U09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u09) [H02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h02) |
| basis.div | DCP分母/低项重组基数 | d=q_div | 1099510054913 | 72057589742960641 | 始终是family完整Q最后一塔；不等于当前RS应丢的Mult。 | [P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [P15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p15) [P20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p20) |
| basis.drop_order | 八轮真实丢弃顺序 | Mult7→Mult0 | q9,q8,q7,q6,q5,q4,q3,q2 | 同S100 | family先保留Div以便下一轮Relin2升基，pair始终无Div。 | [P04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p04) [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) [P20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p20) |
| basis.root_identity | NTT根身份而不只是阶 | root^(N)=-1 modq | 全部冻结根为最小primitive root | 三枚新素数root有效但不是最小root | 手工非最小root本身合法；同q同N换root可能命中旧NTT cache。 | [U20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u20) [N04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n04) [P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) |
| basis.auxBits | 生成P素数的名义位长 | auxBits | 60 | 60 | 每partition一塔且maxQbit60，故P一塔；不能单独换P常量。 | [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [U09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u09) |
| basis.extraBits | FLEXIBLEAUTOEXT额外末层模数配置 | extraBits | 0 | 0 | manual factory显式0；普通默认FLEXIBLEAUTOEXT不是本路径。 | [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [U07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u07) |
| basis.partition_width | 每个HYBRID partition含Q塔数量 | alpha=ceil(nQ/numPartQ) | 1 | 1 | 根11分区，family数减少时分区同步11→4；末分区完整性验证。 | [U09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u09) [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) |
| basis.partition_count | 每family完整Q分区数 | numPartQ | 11,10,9,8,7,6,5,4 | 同S100 | 不等于“8个family”；每family一组s²→s key含该数量A/B多项式。 | [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) |
| basis.complement_tables | 互补升基/降基、逆元及Barrett预计算 | CRT / NTT table identities | 由每family exact Q/P派生 | 同机制，3枚q不同 | 不是修改ElementParams指针后自动有效；必须重建P^-1、PHat、QHat、partition补基。 | [U09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u09) [U10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u10) [N02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n02) |
| basis.paramsPK | 公钥使用的RNS基 | GetParamsPK | Q full11，不是QP | 同S100 | HYBRID并不自然要求公钥QP；evalkey才用QP。 | [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [P11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p11) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) |
| scale.logical_initial | 客户端逻辑有理scale | Δ0 | 2^100/1 | 2^116/1 | 输入先在160/220十进制精度编码成大整数，公共接口从100十进制数起。 | [P25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p25) [P27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p27) [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) |
| scale.logical_recurrence | 八轮精确scale | Δ_r=Δ_(r−1)^2/(d·m_r) | 完整整数分子/分母见scalar JSON | 同公式；d与初值不同 | 每步与独立闭式核对；不能以2^100/2^116常量解码final。 | [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) [T01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t01) [T06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t06) |
| scale.high_low_units | pair高低项与重组的量纲 | RCB=dH+L | combined Δ；高项约Δ/d | 同S100 | 低项是补偿和相关噪声，不是独立低精度明文；直接分开解读会误判。 | [P15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p15) [P17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p17) [P21](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p21) |
| scale.recorded | OpenFHE ciphertext scalingFactor的兼容标记 | double | input/RS:2^100；Tensor/Relin:2^150 | input/RS:2^116；Tensor/Relin:2^174 | exact power of2可由double表示但不含真实素数漂移；修改字段不执行rescale。 | [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) [P17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p17) [N05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n05) |
| scale.noiseDegree | 缩放次数/阶数元数据，不是实际噪声大小 | noiseScaleDeg | fresh2；pair2；Tensor3；Relin3；RS2 | 同S100 | Tensor原始2+2=4后手工改3；不是证明误差有3位或只有3个噪声项。 | [P27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p27) [P17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p17) [P20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p20) |
| scale.integer_factor | 非CKKS整数缩放兼容字段 | scalingFactorInt | 1 | 1 | 本项目逐层要求1；不承担有理scale分母。 | [P14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p14) [P27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p27) |
| scale.level | 相对所属context已丢塔数 | level | fresh0，pair1，RS2；reentry回1；terminal root9 | 同S100 | level复位是换wrapper的局部索引，不是bootstrap或补回模数。 | [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) [P09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p09) [P21](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p21) |
| key.h | 根秘密Hamming weight | h | 128 | 128 | custom h128只采一个根秘密；family投影保持同一整数多项式。 | [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [R13](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r13) |
| key.sign_condition | HWT三元样本正负个数条件 | nplus | 63/64/65个+1；其余非零为−1 | 同S100 | 选择不同支撑后给独立符号，若正数计数不合格重采整个vector；不是任意固定HWT三元分布。 | [R07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r07) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) |
| key.root_projection | 跨context同根秘密投影 | integer s coefficients | 按(q,root,2N)匹配保留tower；不重采s | 同S100 | 每family私钥仅用于生成自己的evalkey；重贴tag不能建立同根。 | [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [P09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p09) |
| key.evalkey_family | s²→s key集合及生命周期 | 8 families；各1 row | A/B各sum(11..4)=60个QP多项式 | 同S100 | 每family fresh a,e；析构只清本plan拥有tags，不全局清NTT。 | [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [U26](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u26) |
| key.tags | 随机根tag与派生family tag | 128bit root tag | 4×32bit PRNG词；后续root+family编号 | 同S100 | private key临时构造即消耗tag随机字，随后覆盖tag也不退回PRNG。 | [R22](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r22) [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [U26](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u26) |
| key.encryption_api | public与secret encryption的重载选择 | PKE mode | payload public Encrypt；root公钥生成用private EncryptZeroCore | 同S100 | 改用secret payload去掉epk*v及e1*s项，是不同噪声与实验条件，未授权。 | [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) [P27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p27) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) |
| noise.ns | 整数噪声乘子，不是明文scale | ns=GetNoiseScale | 1 | 1 | 使PKE中的epk,e0,e1按1倍进入；HYBRID局部t因此取0。 | [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [R18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r18) |
| noise.v | public payload临时掩码分布 | v∈{-1,0,1}^N | 稠密iid三元，h参数0 | 同S100 | 每次public Encrypt重采；不能从root h128推出v h128。 | [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12) [R07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r07) |
| noise.errors | PKE Gaussian项生命周期 | epk / e0 / e1 | σ=float3.19f，Peikert；同一小多项式跨塔 | 同S100 | epk随公钥固定，e0/e1每次payload新采；evalkey errors与此分开。 | [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) [R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) |
| noise.eval_sigma | evalkey Gaussian的有效σ条件分支 | σ_eval | OpenMP private active:1；pragma ignored:context σ | 同S100 | F01：读取context σ并不能证明私有线程DGG仍为3.19；本轮无binary attestation。 | [R10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r10) [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r19) |
| noise.gaussian_algorithm | 实际Gaussian采样器选择 | KARNEY_THRESHOLD | 当前σ<300走Peikert；不是Karney | 同S100 | 另有带mean/n拒绝采样重载和generic sampler；当前DCRT构造不调用。 | [R10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r10) [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) [R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12) |
| noise.gaussian_tail | Peikert成功返回的整数支持界 | T=ceil(12.00610553538285σ) | context T39；private-default T13 | 同S100 | 截断及浮点实现不是无限支撑理想Gaussian；概率和安全证明不可直接等同。 | [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) |
| noise.assurance | assuranceMeasure / 参数生成理论界常数 | α | 36.0f | 36.0f | 不是σ、N或128位安全；manual固定但不自动评估所有实际误差。 | [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [U05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u05) |
| noise.flood_sigma | 解密flooding sampler的参数 | σ_flood | 0.0存储，fixed解密不使用 | 同S100 | 修改此值不影响已经存在的ciphertext噪声；未启用分支不计入当前随机链。 | [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [R16](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r16) [U14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u14) |
| rng.algorithm | 默认PRNG算法 | Blake2Xb | 源码默认；实际替代引擎未取证 | 同S100 | 16uint32 seed作为key；counter作为消息；底层BLAKE2b12round。 | [R01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r01) [R03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r03) [R05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r05) [R06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r06) |
| rng.seed | 种子构造与保密边界 | 512bit seed material | random_device16词+time/thread/address预混合 | 同S100 | 没有输出真实种子；OS熵后端与runtime是否替换未知，失败三次后抛异常。 | [R03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r03) [R04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r04) |
| rng.state | 每引擎counter与输出缓冲 | 64bit counter;1024×uint32 buffer | 每线程惰性初始化/持续消耗 | 同S100 | buffer4096B；state reset不等于重建context；公开prime MR和tags也消耗。 | [R01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r01) [R02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r02) [R03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r03) [R04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r04) |
| rng.fixed_seed | FIXED_SEED编译分支 | compile macro | 是否编译进历史binary未独立确认 | 同S100 | 测试专用，不建议用于生产；固定seed并不消除调度/stdlib/字节序差异。 | [R02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r02) [R03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r03) |
| rng.custom_engine | 可选动态PRNG替代接口 | InitPRNGEngine(libPath) | project中未发现调用；runtime未知 | 同S100 | 支持条件受Linux/Unix、非Apple、GCC非Clang限制；不是通用Windows入口。 | [R01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r01) |
| rng.binary | 二元采样器是否使用 | Bernoulli(1/2) | h128 TUG内部调用BUG采±1符号；无直接binary-secret/PKE分支 | 同S100 | BUG不是独立的binary secret配置，但h128 TUG的符号采样实际调用它；DCRT二元构造器未作为当前PKE直接入口。 | [R07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r07) [R08](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r08) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) |
| rng.public_parameters | 公开素数/根构造也消费PRNG | MR100轮 / generator search | P预计算与ValidateBasis会消费 | 同S100 | 冻结Q避免Q生成不等于setup全确定；RootOfUnity最终根canonical但搜索流随机。 | [U19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u19) [U20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u20) [U21](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u21) [U23](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u23) [P10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p10) |
| build.native | NativeInteger位宽与中间乘法能力 | NATIVEINT/HAVE_INT128/MAX_MODULUS_SIZE | native64；q≤60bit；HAVE_INT128/中间乘法分支需绑定实际构建 | 同S100 | NATIVEINT=64与中间乘法实现分开记录；ApproxSwitchCRTBasis的HAVE_INT128/OpenMP/clang/优化/降噪组合决定分支，未鉴证历史binary。 | [P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [P11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p11) [B02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b02) [N02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n02) [N02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n02) |
| build.backend | 大整数后端 | MATHBACKEND | 4，bigintdyn | 4 | Poly大整数与cpp_int精确桥独立于single-tower native64；其他后端未启用。 | [B03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b03) [P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [P25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p25) |
| build.language | 工程语言和依赖版本 | C++17 / OpenFHE1.5.0 | 由CMake强制；依赖flags导入 | 同S100 | 只有版本号检测不是二进制commit认证；需要安装来源和cache attestation。 | [B01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b01) [B05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b05) |
| build.openmp | 并行编译开关与实际pragma | WITH_OPENMP / PARALLEL | workflow请求ON；binary分支待验证 | 同S100 | ON与运行线程数是不同层；private DGG即使1线程也默认构造。 | [B02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b02) [B04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b04) [B05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b05) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R23](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r23) |
| build.threads | 线程上限及调度 | OMP_NUM_THREADS / ParallelControls | 当前无执行；不从annulus工作流推定旧run线程实况 | 本轮未确认该run实际线程/库二进制 | 修改线程会改变PRNG实例及采样词归属；不能视为同随机实验。 | [R23](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r23) [R02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r02) [B05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b05) |
| build.reduced_noise | HYBRID近似基转换编译分支 | WITH_REDUCED_NOISE | 默认OFF；历史实际binary待取证 | 同S100 | 开启使用centered SwitchModulus替代特定累加分支，噪声/舍入分析不可沿用。 | [B02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b02) [N02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n02) |
| build.nativeopt | 机器特定优化与编译器影响 | WITH_NATIVEOPT | 默认OFF；实际flags待取证 | 同S100 | 结合Clang/OpenMP影响ApproxSwitchCRTBasis实现分支；别把不同编译结果合并。 | [B02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b02) [N02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n02) |
| build.debug_key | 上下文秘密调试访问 | DEBUG_KEY / SetPrivateKey | 项目禁止DEBUG_KEY；不向求值端注入secret | 同S100 | 只保留client rootSecret；不能为了排错把私钥放入公开context或日志。 | [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) [U16](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u16) [P30](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p30) |
| numeric.client | 公共高精度复数API | cpp_dec_float<100> | 声明100十进制位；非100binarybit | 同S100 | 从低精度double导入不能恢复已丢信息；内部guard digits不是额外精度承诺。 | [P22](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p22) [P25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p25) [P28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p28) [P29](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p29) |
| numeric.codec | 编码/解码主辅精度 | decimal digits | 160和220分别建roots/计算 | 同S100 | 提高observer/codec精度不自动降低PKE aggregate噪声；历史归因分离A/B/C。 | [P22](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p22) [P24](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p24) [P25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p25) [H05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h05) |
| numeric.round | 编码整数舍入与危险半整数拒绝 | nearest ties downward | margin=max(16·crossdiff,2^-400)；另检查2^-410 | 同S100 | 双精度舍入一致性与margin是诊断保证，不是所有三角函数形式证明。 | [P25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p25) [N07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n07) |
| numeric.codec_tolerance | 解码跨精度一致性阈值 | 2^-120 | 比较160/220读出最大分量差 | 同S100 | 不是E80准确性门槛；两个错误一致仍可能错，需独立oracle。 | [P28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p28) [T07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t07) |
| numeric.oracle | 独立明文与稀疏Horner精度 | binary digits | 512；10anchors | 512；10anchors | 十个中间锚点不证明所有中间槽无绕模；endpoint full16384另检查。 | [T01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t01) [T03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t03) [T06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t06) [T08](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t08) |
| numeric.observer | 端点独立twisted DFT | binary digits | 512与768；annulus使用两者 | candidate主要沿用512 Horner及production endpoints | roots分别生成，正号未归一化；共享官方inverse NTT的局限保留。 | [T12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t12) [T14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t14) [T15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t15) |
| numeric.one_norm | observer误差模型大小参数 | C=Σ\|c_i\|；k=ceil(log2(C/Δ))；2^k为上界 | 精确cpp_int/Fraction；C0时nullopt | 同机制 | 函数返回指数k而非2^k；C不是max系数norm；模型不支持/阈值重叠必须UNRESOLVED，不能PASS。 | [T16](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t16) [T17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t17) |
| numeric.wire | 规范observer数值序列化 | 110十进制有效位，119bytes | 有符号科学计数；5位指数；规范零 | S116stdout另100位，非同wire协议 | 序列化ties-to-even不同于编码ties-down；严禁通过float/double中转。 | [T18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t18) [T08](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t08) |
| numeric.norm | 判决采用的误差范数 | max component vs max complex modulus | original既有主E80检查为分量最大范数；端点observer另列范数 | 分量最大范数 | component≤complex≤sqrt2·component；更换范数必须换标识且不改旧结果。 | [T02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t02) [T07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t07) [T11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t11) [H03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h03) |
| numeric.gates | 科学验收阈值 | T=2^-80 | 原S100既有E80 FAIL保留；并非本轮重测 | 独立profile原输入历史完整数值PASS | COMPLETE/exit0/observerPASS不等于E80PASS；不放宽门槛。 | [T02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t02) [T07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t07) [T11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t11) [H01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h01) |
| input.formula | 固定输入族、相位与微差 | base/1024−(t mod16)/65536+s2^-75 | base1015/1024，完整相位和s·2^-75 | 与original逐项同输入base1015 | 并非随机输入；每次key/noise仍随机；本轮纯标量检查二者都在单位圆内。 | [T01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t01) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09) [H03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h03) |
| input.domain | 环带/单位圆与绝对条件数 | radius; \|f′(z)\|=256\|z\|^255 | radius约0.990983至0.991242，全部<1 | 同original | 半径改变绝对误差放大，非零z的相对条件数仍256；非配对样本不能作全量因果归因。 | [T01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t01) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09) [H04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h04) |
| input.witness | sub-binary64信息保留与非平凡输出检查 | δ=2^-75; annulus \|z^256\|>2^-10 | 相邻0/1槽微差；完整相位 | 同original | 不能因为真输出变小就宣布精度提高；保留微差与幅度下界。 | [T01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t01) [T02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t02) [T11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t11) |
| context.features | 启用的算法实现组件 | PKE\|KEYSWITCH\|LEVELEDSHE | 只启用三项 | 同S100 | Enable(KEYSWITCH)不替代SetKeySwitchingTechnique(HYBRID)安装；不启用FHE/PRE/MULTIPARTY。 | [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [U16](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u16) |
| context.keygen_level | context保存的key generation level | SetKeyGenLevel | 未设置；上游注释future use | 同S100 | 本项目不通过此字段选择family；使用完整显式basis投影。 | [U16](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u16) [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) |
| context.bootstrap | bootstrap/scheme-switch接口参数 | correctionFactor / BinCC / SwkFC | 未启用，不适用 | 同S100 | SetCKKSBootCorrectionFactor / SetBinCCForSchemeSwitch / SetSwkFC不进入当前乘法链。 | [U17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u17) [U18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u18) |
| context.cache | context / evalkey / NTT缓存不是一个缓存 | shared_ptr/maps | plan只清owned evalkeys；NTT表可能持久 | 同S100 | 同q同N换root需独立进程或受控重建所有依赖；不可在有live对象时随意全局Reset。 | [P04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p04) [U26](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u26) [N04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n04) |
| paper.t | Mult_t的元组长度 | t=2 | 双项(high,low) | 同S100 | 不是CKKS plaintext modulus，不是HYBRID局部t=0，不是multiplicativeDepth。 | [P15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p15) [P17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p17) [R18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r18) [A02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-a02) |
| paper.dnum | 论文gadget rank与OpenFHE分区数 | d_num | paper11；project root numPartQ11 | project root11但不同Q/d | 数值相同不证明HEaaN/OpenFHE gadget或噪声等价；必须核对α/P/升降基。 | [A02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-a02) [U09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u09) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) |
| metadata.map | 密文扩展metadata、sidecar键和值 | opaque object map | 验证/clone保存或要求空map；测试有负控 | 同S100 | 与scale metadata区分；不包含真正明文精度或可信密钥身份；禁止当秘密数据通道。 | [P14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p14) [P09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p09) |
| context.own_state | CryptoContext自身状态、两张静态key map及DEBUG_KEY条件成员 | 4实例字段+2静态map+1条件secret | params=手工CKKS，scheme=HYBRID，schemeId=CKKS，keyGenLevel0；evalkey按tag | 同S100 | 完整字段与7个Set方法见CONTEXT_API_SURFACE；不是枚举所有委托算法的参数。 | [U27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u27) [U28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u28) [U29](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u29) [U26](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u26) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [P30](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p30) |

小 N 诊断不是 paper 参数：`CreateRepeatedMult2DiagnosticSetup` 用 N64、slots16、depth9、scaling50、first55、numLarge10 的普通生成 seed context，再构造受限 families；较早客户端小 N 测试用 depth7/Q8 等另一套设置；h128 smoke 用 N256/order512/slots128、scaling40、first50、depth2、numLarge3。字典中的全局默认不是这些调用的最终参数，逐调用参数见 setter inventory 和相应测试 factory。[P07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p07) [T19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t19) [T20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t20)

## 8. 论文—上游—项目—profile 四方映射

| 论文符号/操作 | OpenFHE 1.5.0 对应表面 | 项目位置/处理 | S100 / S116 与不等价处 |
| --- | --- | --- | --- |
| R、N、2N | DCRTParams / ringDim / roots | MakeFamily frozen order | 两者 N=2^15；无需推测论文实际prime |
| t=2 | 没有同名 CKKS自动双精度开关 | DoubleCKKS pair | 两个密文项，不是 SetPlaintextModulus(2) |
| Δ≈2^100 | EncodingParams+metadata不足以表达全部逻辑 | ExactScale / high precision codec | S100 Δ0=2^100；S116=2^116不是原表参数 |
| Q_l / Base / Mult / Div | ordered RNS moduli / ParamsGen / ModReduce | 冻结Q；定制families；RS2 | S100 50×2+60×8+40；S11658×2+60×8+56 |
| P / gadget / d_num | HYBRID P、numPartQ、alpha、升降基 | numPartQ=familyQsize；α1 | root11相同数值不能证明HEaaN分解等价 |
| χ_sk、h=128 | SecretKeyDist / TUG(h) | custom h128，正项数条件63–65 | 普通 SPARSE 为192；HEaaN实际符号约束未知 |
| χ_enc / v | public EncryptZeroCore 分支 | 非GAUSSIAN→TUG(h0) | 不能把h128扩展给v；论文具体采样实现未公布 |
| χ_err / σ | DGG / float / Peikert / OpenMP分支 | root/PKE contextσ；evalkey条件σ | 论文HEaaN noise配置、版本未知；不是同名即可移植 |
| DCP | last-tower division + remainder | DCP / dH+L | 有中心carry；不是重新编码 |
| Tensor² | EvalMultNoRelin primitive | 三次乘法，省略低低项 | 不是一般 EvalMult 的参数化别名 |
| Relin² | HYBRID evalkey / ApproxModDown | dH零d塔提升+两次relin+DCP | 近似基转换/rounding不可假定完全可加 |
| RS² | ModReduceInternal / DCRT缩塔 | H 与完整RCB各RS，再重组L | 真Δ每轮除d·Mult；论文一处印刷公式缺d |
| RCB | 模环线性组合 | RCBWithReceipt | 当前不重随机化、不bootstrap、不刷新明文 |
| Table3 error统计 | 无自动可比接口 | 原E80及后继profile gates | 论文1000次均值≠本项目单样本/逐槽阈值 |

来源：[A01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-a01) [A02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-a02) [P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) [P15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p15) [P17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p17) [P19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p19) [P20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p20) [P21](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p21) [P28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p28) [R07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r07) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r18)。

论文 Table 3 的 t=2 行名义 QP 为680、N=2^15、d_num11、Base50×2/Mult60×8/Div40/P60、h128；t=1 对比行为 N=2^16、QP1000、d_num9。论文报告基于 HEaaN、1000次实验的平均误差/耗时，并说明相应硬件与单线程条件。Table1/2 的 h=21845、refresh 示例不能转用为 Table3 的 h128 eight-square 设置。论文没有提供这里所需的 HEaaN 固定版本、精确全部 primes、输入生成分布、真实噪声 sampler 参数，因此这些是明确 U，而不是完成本文的前置门槛。（原 PDF 第11–13页，Table3 对应 [A02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-a02)。）

原 PDF 第8页定理4.8的式子写成只除 $q_l$，与同页 scale 说明、前面 Tensor²/RS² 定义的 $q_{div}q_l$ 不一致；本轮原页核对并以纯整数例子重建，历史独立审核中也已提到。本文不将它宣传为新发现的代码 bug。论文普通 Tensor 交叉项符号及 Relin 可加性的过强中间等式亦须分别消歧；不能由这些局部问题直接推翻或证明全部最终误差界。见 FINDINGS F04–F06。[A01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-a01) [H06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h06)

## 9. 三种真实历史条件不能相互冒名

| 条件 | 真实来源 | 包内结论与边界 |
| --- | --- | --- |
| 原 S100 近单位圆固定输入 | run34039088536；source ed5fd192a89d6d4728ad295e87cf06a3f4abc832 | Linux/Windows完成8平方，但原E80 FAIL；绝不能因COMPLETE或observer PASS改写为科学PASS |
| S116 / d56 / Base58×2 | run34055816234；source2b8b349edf5575556347082c1b725f6696c743b6 | 两平台原输入完整数值PASS；改变参数、QP≈712；不是S100/Table3原参成功 |
| S100 annulus125 / base999 | run34184869227；source03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b | Linux单样本PASS；E8复模约1.4623141e-25；改变输入且新key/noise不配对；不是原压力样本修复 |

来源：[H01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h01) [H02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h02) [H03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h03) [H04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h04)。本包主要提供既有接受/结果/复核记录，并非本轮新下载完整所有 CI 原始日志或运行二进制；不能把回执阅读写成新复现。

### 9.1 固定输入的完整可重建定义

槽号$s=0,\ldots,16383$，令$t=\lfloor s/2\rfloor$，
$$a_s=\frac{B}{1024}-\frac{t\bmod16}{65536}+s2^{-75},\qquad b_s=(-1)^{\lfloor t/512\rfloor\bmod2}\frac{1+(\lfloor t/16\rfloor\bmod8)}{1024}.$$
令$u=\lfloor t/128\rfloor\bmod4$，$z_s$依次是$(a_s,b_s),(-b_s,a_s),(-a_s,-b_s),(b_s,-a_s)$。原S100与S116取B=1015，annulus取B=999。这些都是精确dyadic输入，无binary64转写和随机抽样。相位/符号不改变半径，但它们是槽顺序、共轭与微扰witness的必要部分。[T01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t01) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09)

原 S100 与 annulus 都在单位圆内。本轮标量核对原输入半径约[0.990983,0.991242]，annulus约[0.975358,0.975617]；annulus全槽严格小于125/128，而原输入不是。论文未公布具体分布，所以不能说原输入违反了论文明确公布的输入分布。两者微扰、相位与非平凡输出约束详见 scalar JSON / input记录。[T01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t01) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09) [T11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t11) [A02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-a02)

对 $f(z)=z^{256}$，微小 fresh 误差的首阶传播约为 $256z^{255}\varepsilon_0$。该导数解释输入模长为何会显著影响绝对误差，但它不是实际每个随机样本的完整误差界，也不排除后续乘法/基转换误差。端点归因分为
$$I_r=\widetilde z_0^{2^r}-z^{2^r},\qquad A_r=\widetilde z_r-\widetilde z_0^{2^r},\qquad E_r=I_r+A_r.$$
不要将“相对 fresh 实测值的 A 很小”当成“相对原始输入的 E 已 PASS”。annulus 约82.50绝对 bits、73.30相对 bits，只适用于其复模和相应真实输出归一化。[T02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t02) [H01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h01) [H04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h04)

另一个既有 fresh-attribution 样本把初始误差分成编码项、public PKE 聚合项和 decoder 项，聚合 PKE 项占主导；它不是原 S100 的同一 ciphertext，也不证明 DGG/PKE 实现出错。Gaussian安全配置、噪声放大、编码误差与程序缺陷必须分别判断。[H05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h05)

## 10. 排错定位表：先区分症状，再决定观测

本节是只读定位参考，检查名是后继可选项，**本轮未运行**。观测“非秘密量”只指参数、格式、tag关联、计数及授权范围内的聚合值；不得输出 s、seed、epk、v、e0/e1 或可重建它们的捕获数据。

| 症状 | 可能步骤/参数 | 适合记录的非秘密量 | 最小区分方向（未执行） | 定位文件 | 来源 |
| --- | --- | --- | --- | --- | --- |
| fresh E0大 | encoding舍入/Δ；epkv+e0+e1s；root/slot错误 | profile、Δexact、codec跨精度差、授权聚合E0，不公开分项秘密 | 先分编码roundtrip与PKE聚合；再判输入/根顺序；不能只因PKE大就判bug | high_precision_client_io.cpp / paper_h128_client_keypair_contract_test.cpp | [P25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p25) [P27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p27) [H05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h05) |
| 第一轮突增 | DCP carry、LL遗漏量、Relin2基/keys、真实scale | stage基序/arity/degree、receipt、公开预计算shape | 先验DCP/RCB和RS组合恒等式；逐阶段区别Tensor/Relin/RS；不通过改门槛消失错误 | double_ckks.cpp / paper_full_eight_square_contract_test.cpp | [P15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p15) [P17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p17) [P19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p19) [P20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p20) |
| 后期突增 | fresh误差正常放大；level/scale漂移；非wrap界不足 | E/I/A、d·m递推、活动塔、授权norm/headroom | 用原z与fresh两条oracle区分I和A；headroom不等于非wrap证明 | paper_full_eight_square_oracle.h / s100_annulus125_eight_square_test.cpp | [T02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t02) [H01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h01) [H04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h04) |
| 数值整体成比例偏 | metadata当真实Δ；忘除d；按2^bit代真实q | numerator/denominator、parent receipt、近似log仅辅 | 以纯Fraction核对八次闭式，不先跑FHE；比真实和记录scale来源 | repeated_mult2.cpp / experimental_precision116_eight_square_test.cpp | [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) [P28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p28) [T06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t06) |
| 全槽置换/共轭 | powers5/gap、DFT正负号、FFT归一化、NTTroot复用 | slot count、root序、公开monomial输出/索引 | 使用公开X及独立直接稀疏transform正控、故意置换负控；不能只看max error | paper_endpoint_transform.cpp / s100_annulus125_eight_square_test.cpp | [P24](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p24) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09) [T14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t14) [T15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t15) [N04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n04) |
| basis/key family拒绝或乱码 | tag碰撞/缓存、错误q/root投影、丢错塔、P变化 | ordered(q,root)、Q/P长度、keytag身份关联、evalkey数组shape | 先静态对齐完整family与pair活动基，再检查owned tag；不要仅改tag绕过保护 | repeated_mult2.cpp / double_ckks.cpp | [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [P09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p09) [P18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p18) [U26](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u26) |
| 看似PASS但科学结论错误 | COMPLETE冒PASS；A代E；component与complex混用；UNRESOLVED冒PASS | 退出类别、norm、oracle来源、门槛、覆盖slots | 重读判决逻辑和原始输入绑定；十anchor不代全槽；新profile独立命名 | paper_endpoint_exact_scalars.cpp / experimental_precision116_eight_square_test.cpp | [T07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t07) [T08](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t08) [T17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t17) [H01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h01) |
| 跨平台/线程差异 | DGG private默认、stdlib PRNG分布、float/double、compiled宏/NTT缓存 | 构建指纹、宏、实际线程设置、σ来源与分支（不含seed） | 先查二进制编译来源与private语义；并行=1不等于pragma被忽略 | keyswitch-hybrid.cpp / distributiongenerator.cpp / workflows | [R01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r01) [R10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r10) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [B05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b05) |
| 同seed却不同密文 | 先前公开MR/根/P或随机tag消耗、线程调度、拒绝循环 | 过程顺序、计数、构建/平台、测试入口名 | 辨别相同种子承诺与相同随机调用轨迹；不公开生产seed，也不重采挑样本 | nbtheory-impl.h / privatekey.h / tests | [U19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u19) [U20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u20) [U21](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u21) [R22](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r22) [T08](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t08) |
| observer无法裁决 | bound模型不覆盖、C/Δ过大、serial精度不足 | C的精确1-norm指数、allowance、distance、规范字串长度 | Fail与Unresolved分别处理；增加浮点位数不替代证明/正控 | paper_endpoint_scaled_norm.cpp / paper_endpoint_exact_scalars.cpp | [T13](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t13) [T16](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t16) [T17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t17) [T18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t18) |

所有对应测试文件都是定位入口，不代表当前测试能够自动证明任意改参的新 profile。特别是原先只测十个 Horner anchors 的检查不能单独证明所有16384槽；全槽 observer 的 positive/negative order control 和 PASS/FAIL/UNRESOLVED 判决必须一起理解。[T03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t03) [T07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t07) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09) [T14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t14) [T17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t17)

## 11. 修改依赖、证据失效与下一步边界

```text
N + ordered(q,root) + technique/partition + build macros
                ↓
完整Q/P/QP + CRT/NTT表 + EncodingParams + context身份
                ↓
根secret/public key → 同根family secret投影 → 各family evalkeys
                ↓
fresh DCRT编码 + public PKE → pair / 每阶段ExactScale receipt
                ↓
终端CRT/lift/decode → 独立oracle/observer → 科学门槛/报告

输入域/输入精度 ───────────→ 编码、true z^(256)、误差条件数
σ / v / secret分布 / PRNG与线程 ─→ 随机对象与安全/可重复性标签
诊断精度/范数/序列化 ─────────→ 判决能力，不改已有密文算术
```

改变 N、任一 q/root、P、d、KS 技术或分区意味着重建相关 factory/precomputation/key/ciphertext，不能继承旧对象的身份和数值结果。改变输入、噪声分布或公开/秘密加密模式会改变实验条件，不能只保留旧报告的 PASS 字样。改变 observer 可以重评历史合法捕获，但需要新的判决来源和精度控制，不意味着加密已重跑。逐参数变更矩阵与最小区分性检查见 `CHANGE_IMPACT.md`。

**本轮没有确认需要提交的生产补丁。** 新的条件性矛盾是 OpenMP 私有 DGG 参数来源；另有 root/NTT-cache 的条件性复用风险。二者都不足以从文档静态分析直接宣告原 S100 FAIL 的根因。具体发现、反例/前提、历史已知与新发现的划分，见 `FINDINGS.md`。

唯一下一步建议：**根端复核文档覆盖与事实，再选择具体诊断**。

## 12. 固定来源索引

以下行范围按附件中的原始 UTF-8 文件物理行核对；TXT 中 NUL 保留在原始 hash 中。源码链接均为固定 commit，不用 main/latest。对网页可达性没有做全部在线 HTTP 检测；本地 path/line/hash 检查不能伪装成远程树成员的独立再次获取。PDF 第4–9、11–13页实际目视范围、其余文件仅 hash/index 的情况分别记录在 coverage，而不是声称331份官方文件逐行全读。

<a id="src-p01"></a>
**P01 — `project/src/repeated_mult2.cpp:L21–L51`** · PaperGeometryProfile / kPaperQ / kExperimentalPrecision116Profile。支持：frozen exact primes and metadata。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/repeated_mult2.cpp#L21-L51)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p02"></a>
**P02 — `project/src/repeated_mult2.cpp:L111–L159`** · ValidateProfile。支持：manual effective configuration invariants。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/repeated_mult2.cpp#L111-L159)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p03"></a>
**P03 — `project/src/repeated_mult2.cpp:L176–L225`** · MakeFamily。支持：direct CryptoParameters construction; P precompute。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/repeated_mult2.cpp#L176-L225)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p04"></a>
**P04 — `project/src/repeated_mult2.cpp:L235–L277`** · ExactScale / plan construction。支持：rational scale; eight non-prefix families。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/repeated_mult2.cpp#L235-L277)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p05"></a>
**P05 — `project/src/repeated_mult2.cpp:L279–L311`** · receipt construction。支持：eight exact scale and metadata recurrences。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/repeated_mult2.cpp#L279-L311)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p06"></a>
**P06 — `project/src/repeated_mult2.cpp:L351–L398`** · InstallFamilyKeys / h128 validation。支持：same-root projection and fresh evalkeys。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/repeated_mult2.cpp#L351-L398)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p07"></a>
**P07 — `project/src/repeated_mult2.cpp:L400–L441`** · CreateRepeatedMult2DiagnosticSetup。支持：N64 diagnostic auto-generation seed context。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/repeated_mult2.cpp#L400-L441)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p08"></a>
**P08 — `project/src/repeated_mult2.cpp:L443–L469`** · CreatePaperRepeatedMult2Setup / candidate factory。支持：one h128 root setup。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/repeated_mult2.cpp#L443-L469)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p09"></a>
**P09 — `project/src/repeated_mult2.cpp:L473–L568`** · planned validation / reentry / terminal RCB validation。支持：receipt ancestry and context wrapping。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/repeated_mult2.cpp#L473-L568)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p10"></a>
**P10 — `project/src/paper_h128_client_keypair.cpp:L68–L103`** · ValidateBasis / ValidateScheme。支持：MR, root, scheme guards。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/paper_h128_client_keypair.cpp#L68-L103)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p11"></a>
**P11 — `project/src/paper_h128_client_keypair.cpp:L105–L184`** · HYBRID / context checks。支持：full Q public key basis and installed scheme。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/paper_h128_client_keypair.cpp#L105-L184)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p12"></a>
**P12 — `project/src/paper_h128_client_keypair.cpp:L193–L217`** · CreatePaperH128ClientKeyPair。支持：h128 sample and EncryptZeroCore private overload。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/paper_h128_client_keypair.cpp#L193-L217)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p13"></a>
**P13 — `project/src/double_ckks.cpp:L241–L279`** · DoubleCKKS constructor。支持：d last and nominal scale。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/double_ckks.cpp#L241-L279)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p14"></a>
**P14 — `project/src/double_ckks.cpp:L281–L368`** · ValidateCiphertext。支持：representation and metadata guards。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/double_ckks.cpp#L281-L368)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p15"></a>
**P15 — `project/src/double_ckks.cpp:L370–L462`** · DCP / DCP component split。支持：centered quotient and remainder。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/double_ckks.cpp#L370-L462)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p16"></a>
**P16 — `project/src/double_ckks.cpp:L699–L824`** · Add / Sub。支持：manual pair arithmetic。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/double_ckks.cpp#L699-L824)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p17"></a>
**P17 — `project/src/double_ckks.cpp:L826–L892`** · Tensor2。支持：three products, low-low omission and compatibility rescale。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/double_ckks.cpp#L826-L892)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p18"></a>
**P18 — `project/src/double_ckks.cpp:L894–L1009`** · Relin2 validation。支持：BV/HYBRID key shape validation。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/double_ckks.cpp#L894-L1009)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p19"></a>
**P19 — `project/src/double_ckks.cpp:L1011–L1078`** · Relin2 raised high and recomposition。支持：zero d tower lift and two relinearizations。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/double_ckks.cpp#L1011-L1078)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p20"></a>
**P20 — `project/src/double_ckks.cpp:L1080–L1211`** · RS2。支持：two true divisions; last Mult tower。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/double_ckks.cpp#L1080-L1211)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p21"></a>
**P21 — `project/src/double_ckks.cpp:L1213–L1285`** · Mult2 / RCB / RCBWithReceipt。支持：loop reentry and final wrap。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/double_ckks.cpp#L1213-L1285)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p22"></a>
**P22 — `project/src/high_precision_client_io.cpp:L54–L64`** · WorkReal aliases。支持：decimal160 and220。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/high_precision_client_io.cpp#L54-L64)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p23"></a>
**P23 — `project/src/high_precision_client_io.cpp:L176–L312`** · BindContext / live state。支持：manual supported profiles and metadata。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/high_precision_client_io.cpp#L176-L312)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p24"></a>
**P24 — `project/src/high_precision_client_io.cpp:L333–L404`** · Roots / special FFT。支持：independent high-precision complex roots。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/high_precision_client_io.cpp#L333-L404)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p25"></a>
**P25 — `project/src/high_precision_client_io.cpp:L407–L495`** · StableRound / ComputeEncoding。支持：integer lift, margin, rounding。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/high_precision_client_io.cpp#L407-L495)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p26"></a>
**P26 — `project/src/high_precision_client_io.cpp:L500–L544`** · bound state construction。支持：fresh / terminal exact scale。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/high_precision_client_io.cpp#L500-L544)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p27"></a>
**P27 — `project/src/high_precision_client_io.cpp:L606–L635`** · HighPrecisionClientIO::Encrypt。支持：direct DCRT public PKE bypassing ordinary codec。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/high_precision_client_io.cpp#L606-L635)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p28"></a>
**P28 — `project/src/high_precision_client_io.cpp:L638–L755`** · BindRepeatedRcb / Decrypt。支持：Poly* decrypt, exact normalization and cross precision。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/src/high_precision_client_io.cpp#L638-L755)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t01"></a>
**T01 — `project/tests/paper_full_eight_square_oracle.h:L25–L107`** · Real / frozen moduli / Inputs / Scales。支持：binary512 original exact dyadic input。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/paper_full_eight_square_oracle.h#L25-L107)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t02"></a>
**T02 — `project/tests/paper_full_eight_square_oracle.h:L125–L223`** · Error / residuals / CheckFull / ReadSecret。支持：component norm and original gates。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/paper_full_eight_square_oracle.h#L125-L223)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t03"></a>
**T03 — `project/tests/paper_full_eight_square_oracle.h:L225–L336`** · SparseDecrypt / Horner。支持：orthogonal CRT and ten anchors。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/paper_full_eight_square_oracle.h#L225-L336)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t04"></a>
**T04 — `project/tests/paper_full_eight_square_contract_test.cpp:L258–L372`** · RunPaper。支持：original one-chain endpoint and ancestry validation。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/paper_full_eight_square_contract_test.cpp#L258-L372)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t05"></a>
**T05 — `project/tests/experimental_precision116_profile_seam.h:L30–L78`** · kQ / witness constants。支持：independent candidate prime fixtures。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/experimental_precision116_profile_seam.h#L30-L78)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t06"></a>
**T06 — `project/tests/experimental_precision116_eight_square_test.cpp:L71–L192`** · ScaleOracle / Proth / sparse decryption。支持：candidate independent exact identities。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/experimental_precision116_eight_square_test.cpp#L71-L192)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t07"></a>
**T07 — `project/tests/experimental_precision116_eight_square_test.cpp:L321–L389`** · ObserveFull / ObserveAnchors。支持：S116 component E80 and codec gates。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/experimental_precision116_eight_square_test.cpp#L321-L389)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t08"></a>
**T08 — `project/tests/experimental_precision116_eight_square_test.cpp:L469–L652`** · RunCandidate / Run / main。支持：S116 complete gate versus abort; original FAIL retained。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/experimental_precision116_eight_square_test.cpp#L469-L652)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t09"></a>
**T09 — `project/tests/s100_annulus125_eight_square_test.cpp:L45–L89`** · Input / ObserverControl。支持：base999; full-slot monomial order control。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/s100_annulus125_eight_square_test.cpp#L45-L89)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t10"></a>
**T10 — `project/tests/s100_annulus125_eight_square_test.cpp:L161–L210`** · Evaluate / CheckObserver。支持：no interim decryption; conditional observer。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/s100_annulus125_eight_square_test.cpp#L161-L210)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t11"></a>
**T11 — `project/tests/s100_annulus125_eight_square_test.cpp:L226–L324`** · Run / annulus gates。支持：one sample complex norm E80 and A80/4。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/s100_annulus125_eight_square_test.cpp#L226-L324)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t12"></a>
**T12 — `project/tests/paper_endpoint_observer_contract.h:L17–L55`** · Binary aliases / Decision。支持：512/768 binary precision。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/paper_endpoint_observer_contract.h#L17-L55)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t13"></a>
**T13 — `project/tests/paper_endpoint_transform.cpp:L72–L108`** · RoundedInteger。支持：exact ties-to-even binary conversion。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/paper_endpoint_transform.cpp#L72-L108)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t14"></a>
**T14 — `project/tests/paper_endpoint_transform.cpp:L177–L287`** · DftTables / Transform。支持：positive twisted unnormalized DFT; powers5 slot order。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/paper_endpoint_transform.cpp#L177-L287)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t15"></a>
**T15 — `project/tests/paper_endpoint_transform.cpp:L342–L398`** · DirectSparseReference768。支持：independent per-slot sparse direct evaluation。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/paper_endpoint_transform.cpp#L342-L398)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t16"></a>
**T16 — `project/tests/paper_endpoint_scaled_norm.cpp:L7–L31`** · ScaledOneNormExponent。支持：ceil log2 exact coefficient one norm over scale。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/paper_endpoint_scaled_norm.cpp#L7-L31)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t17"></a>
**T17 — `project/tests/paper_endpoint_exact_scalars.cpp:L153–L173`** · AssessDifference。支持：fail / unresolved / pass decision。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/paper_endpoint_exact_scalars.cpp#L153-L173)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t18"></a>
**T18 — `project/tests/paper_endpoint_exact_scalars.cpp:L175–L274`** · CanonicalDecimal / IsCanonicalDecimal。支持：110 significant decimal digits wire grammar。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/paper_endpoint_exact_scalars.cpp#L175-L274)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t19"></a>
**T19 — `project/tests/paper_h128_client_keypair_contract_test.cpp:L53–L106`** · diagnostic context factory。支持：N256 h128 smoke distinct from paper profile。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/paper_h128_client_keypair_contract_test.cpp#L53-L106)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-t20"></a>
**T20 — `project/tests/data/paper_h128_profile.json:L1–L208`** · frozen profile receipt。支持：diagnostic sigma float and Q/P values。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/tests/data/paper_h128_profile.json#L1-L208)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-u01"></a>
**U01 — `official/src/pke/include/scheme/gen-cryptocontext-params-defaults.h:L46–L87`** · CKKSRNS_SCHEME_DEFAULTS。支持：all 33 pin-specific defaults。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/gen-cryptocontext-params-defaults.h#L46-L87)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u02"></a>
**U02 — `official/src/pke/include/scheme/gen-cryptocontext-params.h:L229–L262`** · getAllParamsDataMembers。支持：authoritative 33-field universe。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/gen-cryptocontext-params.h#L229-L262)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u03"></a>
**U03 — `official/src/pke/include/scheme/gen-cryptocontext-params.h:L368–L463`** · Params Set*。支持：32 generic setters。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/gen-cryptocontext-params.h#L368-L463)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u04"></a>
**U04 — `official/src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h:L55–L96`** · CCParams<CryptoContextCKKSRNS>。支持：eight disabled setters。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h#L55-L96)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u05"></a>
**U05 — `official/src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-internal.h:L35–L151`** · genCryptoContextCKKSRNSInternal。支持：ordinary context path and flooding defaults。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-internal.h#L35-L151)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u06"></a>
**U06 — `official/src/pke/lib/scheme/gen-cryptocontext-params-validation.cpp:L35–L191`** · validate parameters。支持：ordinary CCParams rejection predicates。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/gen-cryptocontext-params-validation.cpp#L35-L191)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u07"></a>
**U07 — `official/src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp:L57–L208`** · ParamsGenCKKSRNS。支持：N/security/QP bound and table generation。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp#L57-L208)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u08"></a>
**U08 — `official/src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp:L415–L533`** · single-prime chain generation。支持：nominal size versus exact primes。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp#L415-L533)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u09"></a>
**U09 — `official/src/pke/lib/schemerns/rns-cryptoparameters.cpp:L44–L183`** · PrecomputeCRTTables。支持：HYBRID partitions and P derivation。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-cryptoparameters.cpp#L44-L183)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u10"></a>
**U10 — `official/src/pke/lib/schemerns/rns-cryptoparameters.cpp:L238–L335`** · complementary basis tables。支持：HYBRID per-level basis and Barrett tables。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-cryptoparameters.cpp#L238-L335)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u11"></a>
**U11 — `official/src/pke/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp:L44–L189`** · CKKS PrecomputeCRTTables / FindAuxPrimeStep。支持：rescale inverses, actual versus nominal scale。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp#L44-L189)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u12"></a>
**U12 — `official/src/pke/include/schemerns/rns-cryptoparameters.h:L607–L648`** · GetScalingFactorReal / GetModReduceFactor。支持：FIXEDMANUAL compatibility values。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-cryptoparameters.h#L607-L648)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u13"></a>
**U13 — `official/src/pke/include/schemebase/rlwe-cryptoparameters.h:L65–L125`** · CryptoParametersRLWE constructors。支持：sigma/noiseScale base values。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/rlwe-cryptoparameters.h#L65-L125)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u14"></a>
**U14 — `official/src/pke/include/schemebase/rlwe-cryptoparameters.h:L319–L451`** · manual Set*。支持：mutations do not rebuild keys or tables。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/rlwe-cryptoparameters.h#L319-L451)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u15"></a>
**U15 — `official/src/pke/include/schemebase/base-cryptoparameters.h:L100–L162`** · base Set*。支持：encoding and element params。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/base-cryptoparameters.h#L100-L162)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u16"></a>
**U16 — `official/src/pke/include/cryptocontext.h:L949–L1000`** · Enable / SetKeyGenLevel。支持：features and future-use level。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L949-L1000)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u17"></a>
**U17 — `official/src/pke/include/cryptocontext.h:L3609–L3623`** · SetCKKSBootCorrectionFactor。支持：not enabled bootstrap mode。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L3609-L3623)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u18"></a>
**U18 — `official/src/pke/include/cryptocontext.h:L3938–L3963`** · SetBinCCForSchemeSwitch / SetSwkFC。支持：not enabled scheme-switch mode。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L3938-L3963)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u19"></a>
**U19 — `official/src/core/include/math/nbtheory-impl.h:L63–L123`** · RNG / FindGenerator。支持：public-parameter randomness。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/nbtheory-impl.h#L63-L123)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u20"></a>
**U20 — `official/src/core/include/math/nbtheory-impl.h:L183–L230`** · RootOfUnity。支持：canonical minimum primitive root。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/nbtheory-impl.h#L183-L230)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u21"></a>
**U21 — `official/src/core/include/math/nbtheory-impl.h:L261–L288`** · MillerRabinPrimalityTest。支持：random witnesses。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/nbtheory-impl.h#L261-L288)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u22"></a>
**U22 — `official/src/core/include/math/nbtheory-impl.h:L329–L398`** · FirstPrime / LastPrime / NextPrime / PreviousPrime。支持：exact step and nominal bit ambiguity。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/nbtheory-impl.h#L329-L398)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u23"></a>
**U23 — `official/src/core/include/math/nbtheory.h:L243–L252`** · MillerRabin default。支持：100 iterations。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/nbtheory.h#L243-L252)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r01"></a>
**R01 — `official/src/core/lib/math/distributiongenerator.cpp:L35–L110`** · InitPRNGEngine / GetPRNG。支持：engine selection and per-thread lazy state。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/lib/math/distributiongenerator.cpp#L35-L110)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r02"></a>
**R02 — `official/src/core/include/math/distributiongenerator.h:L35–L87`** · PseudoRandomNumberGenerator。支持：OpenMP threadprivate versus thread_local。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/distributiongenerator.h#L35-L87)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r03"></a>
**R03 — `official/src/core/lib/utils/prng/blake2engine.cpp:L35–L159`** · Blake2Engine / create default engine。支持：seed construction and refill。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/lib/utils/prng/blake2engine.cpp#L35-L159)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r04"></a>
**R04 — `official/src/core/include/utils/prng/blake2engine.h:L35–L115`** · Blake2Engine state。支持：seed/buffer/counter sizes。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/utils/prng/blake2engine.h#L35-L115)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r05"></a>
**R05 — `official/src/core/lib/utils/prng/blake2xb-ref.c:L27–L164`** · blake2xb key-init/update/final。支持：real XOF implementation。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/lib/utils/prng/blake2xb-ref.c#L27-L164)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r06"></a>
**R06 — `official/src/core/lib/utils/prng/blake2b-ref.c:L133–L195`** · blake2b_compress。支持：12 rounds add/rotate/xor。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/lib/utils/prng/blake2b-ref.c#L133-L195)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r07"></a>
**R07 — `official/src/core/include/math/ternaryuniformgenerator-impl.h:L40–L148`** · GenerateIntVector。支持：dense ternary / HWT sign conditioning。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/ternaryuniformgenerator-impl.h#L40-L148)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r08"></a>
**R08 — `official/src/core/include/math/binaryuniformgenerator-impl.h:L40–L67`** · GenerateVector。支持：binary Bernoulli alternative。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/binaryuniformgenerator-impl.h#L40-L67)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r09"></a>
**R09 — `official/src/core/include/math/discreteuniformgenerator-impl.h:L40–L102`** · GenerateInteger / GenerateVector。支持：uniform q rejection sampling。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/discreteuniformgenerator-impl.h#L40-L102)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r10"></a>
**R10 — `official/src/core/include/math/discretegaussiangenerator.h:L83–L104`** · DiscreteGaussianGeneratorImpl。支持：default sigma1.0。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/discretegaussiangenerator.h#L83-L104)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r11"></a>
**R11 — `official/src/core/include/math/discretegaussiangenerator-impl.h:L40–L184`** · SetStd / Initialize / GenerateIntVector。支持：Peikert truncated inverse CDF / Karney threshold。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/discretegaussiangenerator-impl.h#L40-L184)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r12"></a>
**R12 — `official/src/core/include/lattice/hal/default/dcrtpoly-impl.h:L126–L193`** · DCRTPoly sampler constructors。支持：small polynomial shared across towers; uniform independent。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/dcrtpoly-impl.h#L126-L193)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r13"></a>
**R13 — `official/src/pke/lib/schemebase/base-pke.cpp:L45–L100`** · PKEBase::KeyGenInternal。支持：ordinary secret h192 and pk signs。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-pke.cpp#L45-L100)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r14"></a>
**R14 — `official/src/pke/lib/schemerns/rns-pke.cpp:L40–L197`** · Encrypt / EncryptZeroCore overloads。支持：public payload v/e0/e1 and private zero encryption。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-pke.cpp#L40-L197)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r15"></a>
**R15 — `official/src/pke/lib/schemerns/rns-pke.cpp:L199–L224`** · DecryptCore。支持：prefix secret and component powers。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-pke.cpp#L199-L224)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r16"></a>
**R16 — `official/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp:L38–L98`** · Decrypt to Poly / NativePoly。支持：conditional flooding, retained full integer。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp#L38-L98)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r17"></a>
**R17 — `official/src/pke/lib/keyswitch/keyswitch-hybrid.cpp:L51–L129`** · KeySwitchGenInternal secret-to-secret。支持：partition evalkey formula and private(dgg)。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L51-L129)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r18"></a>
**R18 — `official/src/pke/lib/keyswitch/keyswitch-hybrid.cpp:L308–L437`** · KeySwitchCore / precompute / fast core。支持：raised digits and ModDown P。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L308-L437)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r19"></a>
**R19 — `official/src/pke/lib/keyswitch/keyswitch-bv.cpp:L40–L159`** · KeySwitchGenInternal BV。支持：base2 digit keys; same private(dgg) clause。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/keyswitch/keyswitch-bv.cpp#L40-L159)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r20"></a>
**R20 — `official/src/pke/lib/schemebase/base-leveledshe.cpp:L132–L164`** · EvalMultKeyGen / EvalMultKeysGen。支持：secret squared and key generator。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-leveledshe.cpp#L132-L164)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r21"></a>
**R21 — `official/src/pke/lib/schemebase/base-leveledshe.cpp:L300–L353`** · Relinearize。支持：evaluation consumes keys; no new sampling。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-leveledshe.cpp#L300-L353)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r22"></a>
**R22 — `official/src/pke/include/key/privatekey.h:L50–L118`** · GenerateUniqueKeyID / PrivateKeyImpl。支持：random tags consume stream。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/key/privatekey.h#L50-L118)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-r23"></a>
**R23 — `official/src/core/include/utils/parallel.h:L35–L136`** · ParallelControls。支持：thread limits and machine thread cache。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/utils/parallel.h#L35-L136)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-n01"></a>
**N01 — `official/src/core/include/lattice/hal/default/dcrtpoly-impl.h:L680–L711`** · DropLastElements / DropLastElementAndScale。支持：true last-tower centered divide。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/dcrtpoly-impl.h#L680-L711)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-n02"></a>
**N02 — `official/src/core/include/lattice/hal/default/dcrtpoly-impl.h:L888–L1004`** · ApproxSwitchCRTBasis / ApproxModDown。支持：HYBRID integer basis operations and reduced-noise branch。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/dcrtpoly-impl.h#L888-L1004)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-n03"></a>
**N03 — `official/src/core/include/lattice/hal/default/poly-impl.h:L399–L440`** · SwitchModulus / SwitchFormat。支持：centered lift and native bit-reversed NTT。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/poly-impl.h#L399-L440)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-n04"></a>
**N04 — `official/src/core/include/math/hal/intnat/transformnat-impl.h:L648–L775`** · ChineseRemainderTransformFTTNat。支持：primitive root tables indexed only by modulus and size。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/hal/intnat/transformnat-impl.h#L648-L775)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-n05"></a>
**N05 — `official/src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp:L172–L201`** · ModReduceInternalInPlace / LevelReduceInternalInPlace。支持：arithmetic versus metadata drop。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp#L172-L201)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-n06"></a>
**N06 — `official/src/pke/lib/encoding/ckkspackedencoding.cpp:L115–L132`** · CKKSPackedEncoding::Encode。支持：double complex FFT entry。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/encoding/ckkspackedencoding.cpp#L115-L132)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-n07"></a>
**N07 — `official/src/pke/lib/encoding/ckkspackedencoding.cpp:L190–L294`** · native64 Encode。支持：binary64 llround and magnitude handling。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/encoding/ckkspackedencoding.cpp#L190-L294)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-n08"></a>
**N08 — `official/src/pke/lib/encoding/ckkspackedencoding.cpp:L336–L365`** · Decode。支持：binary64 scaling and centered decode。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/encoding/ckkspackedencoding.cpp#L336-L365)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-n09"></a>
**N09 — `official/src/pke/lib/encoding/ckkspackedencoding.cpp:L450–L501`** · Decode real-only additional noise。支持：ordinary REAL decode normal sampler; bypassed。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/encoding/ckkspackedencoding.cpp#L450-L501)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-b01"></a>
**B01 — `project/CMakeLists.txt:L1–L38`** · build target / dependency imports。支持：C++17 OpenFHE1.5.0 exact version and inherited flags。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/CMakeLists.txt#L1-L38)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-b02"></a>
**B02 — `official/CMakeLists.txt:L67–L104`** · build options。支持：OpenMP/native/reduced-noise/nativeopt defaults。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/CMakeLists.txt#L67-L104)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-b03"></a>
**B03 — `official/CMakeLists.txt:L354–L378`** · MATHBACKEND。支持：native64/backend4 request。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/CMakeLists.txt#L354-L378)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-b04"></a>
**B04 — `official/CMakeLists.txt:L427–L464`** · OpenMP CMake。支持：compiler flags construction。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/CMakeLists.txt#L427-L464)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-b05"></a>
**B05 — `project/.github/workflows/s100-annulus125-once.yml:L119–L187`** · historical workflow build / run。支持：OpenMP ON request, cache and thread environment。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/.github/workflows/s100-annulus125-once.yml#L119-L187)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-h01"></a>
**H01 — `context/coordination/fs-endpoint-live-run-01/ACCEPTANCE.md:L1–L35`** · original S100 retained acceptance。支持：historical two-platform E80 FAIL。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/coordination/fs-endpoint-live-run-01/ACCEPTANCE.md#L1-L35)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-h02"></a>
**H02 — `context/coordination/precision116-eight-square-return-01/ACCEPTANCE.md:L1–L35`** · candidate acceptance。支持：historical changed-parameter PASS。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/coordination/precision116-eight-square-return-01/ACCEPTANCE.md#L1-L35)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-h03"></a>
**H03 — `context/coordination/s100-annulus125-20260908/RESULT.zh-CN.md:L5–L45`** · annulus result。支持：historical changed-input PASS and unresolved scope。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/coordination/s100-annulus125-20260908/RESULT.zh-CN.md#L5-L45)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-h04"></a>
**H04 — `context/coordination/annulus-independent-pro-review-20260908/RETURN_DISPOSITION.md:L28–L58`** · root historical review。支持：relative bits, prior paper findings, evidence limits。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/coordination/annulus-independent-pro-review-20260908/RETURN_DISPOSITION.md#L28-L58)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-h05"></a>
**H05 — `context/coordination/s100-fresh-error-repair-01/GREEN2_RESULT.md:L11–L32`** · fresh attribution receipt。支持：encoding/PKE/codec decomposition is not sampler bug。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/coordination/s100-fresh-error-repair-01/GREEN2_RESULT.md#L11-L32)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-h06"></a>
**H06 — `context/coordination/annulus-independent-pro-review-20260908/pro/REVIEW.md:L174–L190`** · prior paper discrepancies。支持：not new production RED。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/coordination/annulus-independent-pro-review-20260908/pro/REVIEW.md#L174-L190)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-a01"></a>
**A01 — `references/paper/PAPER-2023-1788.txt:L898–L951`** · PDF page8 Theorem4.8 / modulus consumption。支持：printed normalization inconsistency; visually checked。 来源为包内文件；完整 hash 见 SOURCE_REFERENCES.json。

<a id="src-a02"></a>
**A02 — `references/paper/PAPER-2023-1788.txt:L1562–L1604`** · PDF page13 Section6.3 Table3。支持：nominal parameters and statistical protocol。 来源为包内文件；完整 hash 见 SOURCE_REFERENCES.json。

<a id="src-u24"></a>
**U24 — `official/src/pke/lib/schemerns/rns-leveledshe.cpp:L182–L225`** · EvalMult / EvalSquare。支持：ordinary auto/manual path。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-leveledshe.cpp#L182-L225)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u25"></a>
**U25 — `official/src/pke/lib/schemerns/rns-leveledshe.cpp:L311–L338`** · ModReduce / LevelReduce。支持：FIXEDMANUAL dispatch。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-leveledshe.cpp#L311-L338)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u26"></a>
**U26 — `official/src/pke/lib/cryptocontext.cpp:L45–L139`** · static maps / EvalMultKeyGen / clear。支持：key-tag cache lifetime; NTT reset distinction。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/cryptocontext.cpp#L45-L139)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-p29"></a>
**P29 — `project/include/openfhe_2023_1788/high_precision_client_io.h:L21–L44`** · ClientReal / PositiveRationalScale。支持：100 decimal digits, exact numerator/denominator。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/include/openfhe_2023_1788/high_precision_client_io.h#L21-L44)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-p30"></a>
**P30 — `project/include/openfhe_2023_1788/repeated_mult2.h:L1–L39`** · DEBUG_KEY rejection / receipt semantics。支持：no context secret debug storage; receipt is not cryptographic lineage proof。 [固定提交原文](https://github.com/leemaple/20231788./blob/a4b815a733efe81897325e2a8e4c826a4ebfa439/include/openfhe_2023_1788/repeated_mult2.h#L1-L39)。提交 `a4b815a733efe81897325e2a8e4c826a4ebfa439`。

<a id="src-a03"></a>
**A03 — `references/paper/PAPER-2023-1788.txt:L329–L478`** · PDF pages5-6 Definition3.1/Theorem3.2/DCP。支持：centered quotient/carry and DCP/RCB。 来源为包内文件；完整 hash 见 SOURCE_REFERENCES.json。

<a id="src-u27"></a>
**U27 — `official/src/pke/include/cryptocontext.h:L242–L255`** · CryptoContextImpl own storage。支持：two static key maps; four per-context fields。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L242-L255)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u28"></a>
**U28 — `official/src/pke/include/cryptocontext.h:L462–L496`** · DEBUG_KEY privateKey / SetPrivateKey。支持：conditional secret access is prohibited by project。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L462-L496)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-u29"></a>
**U29 — `official/src/pke/include/cryptocontext.h:L3761–L3776`** · SetParamsFromCKKSCryptocontext。支持：exports values into scheme-switch params; inactive。 [固定提交原文](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L3761-L3776)。提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。

<a id="src-a04"></a>
**A04 — `references/paper/PAPER-2023-1788.txt:L258–L278`** · PDF page5 ordinary Tensor sign。支持：printed negative cross term versus (1,s) convention。 来源为包内文件；完整 hash 见 SOURCE_REFERENCES.json。

<a id="src-a05"></a>
**A05 — `references/paper/PAPER-2023-1788.txt:L666–L743`** · PDF pages7-8 Definition4.3/Lemma4.4 proof。支持：Relin2 lift and overstrong vector carry identity。 来源为包内文件；完整 hash 见 SOURCE_REFERENCES.json。
