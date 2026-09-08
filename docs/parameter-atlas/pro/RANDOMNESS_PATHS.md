# 随机数、采样分布与生命周期：固定 OpenFHE/Mult² 路径

任务 `OPENFHE-CKKS-PARAMETER-RANDOMNESS-ATLAS-01`。工程 pin `a4b815a733efe81897325e2a8e4c826a4ebfa439`；上游 pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`。本文件是独立可读的机制说明；引用链接定位主图谱的固定源码索引。**只做源码分析，没有执行采样器或要求任何真实种子、秘密多项式、密钥/噪声转储。**

## 1. 先区分四个层次

数学分布描述“理想应如何随机”；采样算法描述“代码怎样把随机字变成整数”；PRNG 描述“如何从秘密状态产生随机字”；系统熵决定“初始秘密状态从哪里来”。相同 σ 的两个采样器不必逐样本相同；相同名义分布不保证相同有限表；相同 seed 也不保证线程、平台、调用轨迹改变后仍是同一实验。

当前链同时有均匀 RNS 多项式、小高斯多项式、稠密三元 v 和稀疏但符号平衡的 h128 根秘密。小多项式应先采一个整数向量，再逐塔映射；不是所有随机多项式都每塔独立采样。评估阶段的 Tensor/Relin/RS 不再采新噪声：其中 relinearization 消耗的是早已生成的随机评估密钥，基转换舍入则是给定输入后的确定过程。[R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r18) [R21](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r21) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12)

## 2. 完整调用图

```text
平台 C++ random_device（实际 OS 提供者未知）
  + time/thread-id/address 的预混合路径
    → Blake2SeedGenerator: 16 × uint32 seed
      → default_prng::createEngineInstance
        → Blake2Engine(seed, counter=0)
          → blake2xb(buffer4096B, counter64, key=seed64B)
            → blake2b XOF reference implementation
              → PRNG uint32 word stream, buffered/persistent

PseudoRandomNumberGenerator::GetPRNG()
  ├─ uniform_int_distribution / uniform_real_distribution
  │   ├─ DUG: uniform modulo q (rejection)
  │   ├─ BUG: Bernoulli(1/2)
  │   ├─ TUG h=0: {-1,0,+1}
  │   ├─ TUG h=128: uniform positions + BUG signs + whole-vector rejection
  │   └─ DGG σ<300: finite Peikert inverse-CDF table
  ├─ nbtheory: MR witnesses / generator search / prime-root precomputation
  └─ PrivateKeyImpl: random public identifier (tag), not a secret key

DCRTPoly(sampler, basis, format)
  ├─ Gaussian/ternary: one small integer vector → all q residues → per-q NTT
  └─ uniform: one independent uniform Poly per q

project CreatePaperH128ClientKeyPair
  ├─ h128 TUG → s on Q_root
  └─ private EncryptZeroCore(s,Q_root): a + e_pk → root public key
project InstallFamilyKeys
  ├─ same s projected to each Q_j
  └─ EvalMultKeyGen → s² → HYBRID KeySwitchGenInternal
       → a_part in QP_j + e_part (conditional OpenMP σ)
client HighPrecisionClientIO::Encrypt
  └─ public PKE → fresh dense v + e0 + e1 → payload ct
server DCP / eight Tensor2-Relin2-RS2 / RCB
  └─ no new sampling; uses frozen public ct and evalkeys
client scheme Decrypt(..., Poly*) + custom decode
  └─ current FIXED_NOISE_DECRYPT: no newly sampled flooding
```

图中每个箭头的固定源码依据：熵/引擎[R01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r01) [R02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r02) [R03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r03) [R04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r04) [R05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r05) [R06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r06)；采样器[R07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r07) [R08](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r08) [R09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r09) [R10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r10) [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11)；DCRT[R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12)；项目与密钥/加密[P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) [P27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p27) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r20) [R21](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r21)；参数随机性/标识[U19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u19) [U20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u20) [U21](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u21) [R22](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r22)；解密[R16](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r16) [P28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p28)。

## 3. 熵入口、引擎状态、线程与重置

### 3.1 默认 seed 的真实构造

`Blake2SeedGenerator` 的非 FIXED_SEED 分支先构造初始化 key：时钟 tick 截为32位；线程 ID hash 的低32位，适用64位条件下再取高32位；分配后释放一个字节的 heap 地址作预混合计数器。用该初始化 key 的 Blake2 引擎经 `uniform_int_distribution<uint32_t>` 生成16词临时 seed。随后建立 `std::random_device`，同样生成16词 `rdseed`，最终逐词以无符号模 $2^{32}$ 加到 seed。[R03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r03)

这段源码的安全评论并非本轮熵测量：不能把时间戳或地址的名义位数直接当成已验证的熵，更不能把“512-bit seed”直接说成这次运行有512bit安全。`random_device` 失败允许三次异常重试；三次均异常就抛出。成功返回但底层实现是否具有足够非确定性，仍需要实际平台/标准库证据，不能由这段包装源码证明。源码提到旧 MinGW 的历史实现问题；这不是认定本包历史 Windows 运行存在该问题。[R03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r03)

成功构造后 `rdseed`、外层局部 seed 被安全擦除，引擎析构也擦除其 seed。`Generate()` 不向 OS 再取熵，而是用当前 key 与 counter 生成缓冲并自增 counter。未发现本路径在“每次加密”或“每轮乘法”周期性 reseed 的调用；不能把持续 PRNG 消耗说成逐次系统熵采样。这里的 counter 字节表示传给 C 实现，跨不同数据模型/字节序也不能随意假定序列一致。[R03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r03) [R04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r04) [R05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r05)

### 3.2 默认 PRNG 的具体实现

`Blake2Engine` 的结果类型是 uint32，seed 数组16词，输出数组1024词、4096字节，counter为uint64。缓冲用尽时调用 `blake2xb`，输入是 counter 的内存字节、key是64字节seed、输出是整个缓冲。底层 `blake2xb-ref.c` 先做 BLAKE2b 根摘要再按 XOF node 生成输出；`blake2b-ref.c` 的压缩是12轮加法/XOR/rotate，轮函数旋转量32/24/16/63。本文追到这些函数，不把 `std::mt19937`、AES-CTR 或 OS CSPRNG 当作该 pin 的默认 PRNG 算法。[R03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r03) [R04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r04) [R05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r05) [R06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r06)

### 3.3 每线程引擎与可选替代

`WITH_OPENMP` 时 `m_prng` 是类静态成员并在相应头文件用 threadprivate（FIXED_SEED 宏分支另作处理）；非 OpenMP 源码使用命名空间 `thread_local shared_ptr<PRNG>`。首次 `GetPRNG()` 看到当前线程指针为空，在临界区初始化全局 factory 函数指针并创建当前引擎；后续返回持续状态。临界区只是串行化初始化部分，不把所有线程的后续采样变为同一个有固定顺序的全局流。[R01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r01) [R02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r02)

`InitPRNGEngine(path)` 只在 factory 尚未初始化时生效：已经初始化就直接 return；它**不是**一个可反复重置所有线程 seed/state 的入口。空路径选择默认引擎；非空路径的 `dlopen/dlsym("createEngineInstance")` 仅在源码允许的 Linux/Unix、非 Apple、GCC、非 Clang 条件下支持，其他分支抛出。project 中没有找到显式选择外部引擎的调用，不等于对历史所有动态链接库/初始化顺序作了运行取证。[R01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r01)

FIXED_SEED 分支将零初始化数组的第一词设为1并发出调试警告，注释要求单线程。它是公开可预测的调试状态，不能用于生产密钥或生产 ciphertext。此次既未启用它，也未根据其存在推断任何历史样本是确定性的。改变 `OMP_NUM_THREADS` 或 `ParallelControls` 改变的是线程请求/上限；不保证重置已创建引擎。[R03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r03) [R23](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r23) [B05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b05)

## 4. 从随机字到分布：实际分支表

| 对象/路径 | 数学目标与源码算法 | 当前实际可确定的参数 | 未启用/不能混同的路径 |
| --- | --- | --- | --- |
| DUG | 逐32位拼接，最高块限界，拒绝≥q；得到[0,q−1]均匀整数 | 每一独立uniform塔用该塔q | 不是先采一个“小”整数再映射所有塔 |
| BUG | 概率1/2的0/1 | h128 TUG内部用来决定±号，**确实在根秘密依赖闭包内** | DCRT binary构造器不是独立的当前PKE入口；不能因此把根当二元秘密 |
| TUG h0 | `uniform_int_distribution(-1,1)`，各系数稠密三元 | 当前public payload的v；诊断UNIFORM秘密 | h0不是全零向量，也不是HWT0 |
| TUG h128 | 随机索引，拒绝重复支持；BUG决定±；正项数偏离63–65时重做整向量 | 根secret恰128非零；符号有额外条件 | 普通SPARSE KeyGen调用h192，不能混用 |
| DGG小σ | double表有限Peikert反演：零质量+正CDF+符号 | contextσ=3.190000057220459；有效OpenMP private evalkey σ条件值1 | 不是连续normal，也不是Karney当前分支 |
| DGG大σ | threshold=300后 `GenerateIntegerKarney(0,σ)` | 当前σ1/3.19均不进入 | 仅识别分支，不声称本轮完整分析了Karney全部内部过程 |
| 其他DGG重载 | `(mean,stddev,n)` 有单独有限区间拒绝算法；一种有10000次防护 | 当前DCRT小整数构造没有调用该重载 | 不能把该10000上限当作当前Peikert每系数循环次数 |
| ordinary REAL Decode | 连续 `normal_distribution` 的额外解码噪声 | custom COMPLEX route绕过 | 不把它算到当前Poly* +自定义decoder的fresh噪声中 |

来源：[R07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r07) [R08](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r08) [R09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r09) [R10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r10) [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) [R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12) [R13](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r13) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [N09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n09)。DGG分支“参数有效”是依据源码条件，不是本轮采样统计。

### 4.1 Peikert 表、截断与浮点的区别

代码以 $T=\lceil12.00610553538285\sigma\rceil$ 建正整数1…T的权重表，$a=[1+2\sum_{x=1}^{T}\exp(-x^2/(2\sigma^2))]^{-1}$，正半边CDF乘a。每个系数用 `uniform_real_distribution<double>(0,1)-0.5`，中心区间返回0，其他用 `lower_bound` 找绝对值再按符号返回。理论描述应写“有限表近似的离散Gaussian”，而不是“连续正态取整”或未经误差说明的理想无限支撑 Dσ。表尾注释约2^-100只是设计说明，不能替代整个RLWE系统的安全论证。[R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11)

本轮用 struct pack/unpack 只核对了3.19f的值，再用标量公式核对T=39；σ1对应T=13。没有构建Peikert表、生成随机数或验其经验分布。代码如果 `lower_bound` 找不到会抛出，并非无限兜底；这意味着准确的“成功返回支持”可确定，具体平台发生异常的可能性则未实测。`IsInitialized()` 返回σ>1.000000001，但DCRTGaussian构造器直接调用 `GenerateIntVector`，没有把σ1私有对象修正成contextσ。[R10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r10) [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) [R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12)

### 4.2 h 参数并不是任意可替换的普通整数

当前固定 h128 和普通上游 h192 都满足此采样代码的正常意图。若以后改变h，需要同时记录支持数与正负约束；不能只修改 `SecretKeyDist` 枚举。源码 h>size 会先夹到size；小h与 unsigned `h/2-1`、外层 while 的初始化还涉及额外边界语义。因此本文没有声称该方法对任意h都给出理想固定权重分布。当前项目的严格h128验证挡住这类扩展，不在本轮尝试新h。[R07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r07) [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12)

## 5. DCRT 的跨塔语义

Gaussian 构造器调用一次 `GenerateIntVector(N)`，把同一数组 x[k] 逐塔映成 $x[k]\bmod q_i$；ternary 同理。这样CRT重组恢复一个小整数多项式，而不是一个模Q的巨大均匀多项式。随后按请求转为 EV；NTT本身对固定输入确定，不能被解释为“又随机了一次”。binary构造器复用一份小多项式，均匀DUG构造器则对每塔分别构造uniform Poly。[R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12) [N03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n03)

由此区分两种“独立”：a在不同塔的残值均匀独立，是标准 $R_Q$ 均匀元素；e、s、v各自跨塔故意保持整数一致。不同 e0/e1/epk 调用在密码学模型下是新采样，但源码共用PRNG系统，不应声称它们来自不同OS seed。评估密钥第part的e也跨QP全部塔使用同一个小整数样本；每part重新采，当前single-key分支a也重新采。[R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17)

## 6. 根秘密、公钥、payload 与评估密钥逐项账

| 随机量 | 谁生成/何时 | 分布、基和格式 | 生命周期/跨context | 给求值端的形态 |
| --- | --- | --- | --- | --- |
| 根 s | `CreatePaperH128ClientKeyPair`，一次被接受的根setup | TUG h128，N整数→Qroot EV | 所有family同根投影，不再次抽s；sampler内部拒绝不是多次挑key | 不给；仅客户端SK |
| 根public a | private `EncryptZeroCore` | 独立uniform R_Q，EV | rootPK持久；不是每payload重抽此a | 通过PK的−a发布 |
| 根 epk | 同上 | contextDGG小整数→Qroot EV | 随PK持久影响所有payload；不直接导出 | 隐藏在PK关系中 |
| payload v | public `EncryptZeroCore`，每次payload | 当前稠密TUG h0→当前加密基 | 每payload新采；不受h128约束 | 仅混在ct中 |
| payload e0/e1 | 同上两次构造 | contextDGG→同一活动基，EV | 每payload新采；e0与e1不同调用 | 仅混在ct中 |
| evalkey a_part | HYBRID `KeySwitchGenInternal`，每family每digit | uniform R_QP，EV | family专属key持久；当前ekPrev为空 | A数组公开 |
| evalkey e_part | 同上 | Gaussian σ依OpenMP private分支；整数→QP | 每digit新采，求值时不重采 | 隐藏在B数组中 |
| 私钥对象tag | PrivateKeyImpl构造 | 4个uint32产生128bit标识文本 | 某些投影对象tag随后覆盖；随机字仍已消耗 | tag可公开，但不是认证或密钥证明 |
| 参数验证随机数 | MR/FindGenerator/RootOfUnity | 数论算法的随机witness/search | 发生在crypto样本前后会改调用轨迹 | 固定prime/root可公开，内部随机字无必要输出 |

来源：[P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) [R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r20) [R22](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r22) [U19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u19) [U20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u20) [U21](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u21)。

普通 `KeyGenInternal` 支持 GAUSSIAN/UNIFORM_TERNARY/SPARSE_TERNARY分支，其SPARSE调用192；本项目不使用该函数来生成根s，而是显式128，然后用私钥零加密重载构造PK。这个“private重载”只是生成公钥的步骤，**不代表 payload 用了 secret-key encryption**。客户端真正payload调用 public Encrypt。[R13](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r13) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) [P27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p27)

## 7. 数学噪声对应到源代码

### 7.1 当前 public payload

采用project实际PK符号，$pk=(as+n_se_{pk},-a)$，payload为
$$c=(pk_0v+n_se_0+m,\;pk_1v+n_se_1).$$
解密得到 $m+n_s(e_{pk}v+e_0+e_1s)$。修改σ会影响epk/e0/e1（或evalkey的条件分支），修改v分布首先影响epk·v，修改h首先影响e1·s及密钥结构/安全假设；这些项不是同一个噪声源。改输入模长不改这套PKE采样公式，却改变编码m和之后八平方的误差传播条件。[R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) [P27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p27) [T01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t01) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09)

若改用secret-key payload零加密加m，误差关系变为 $m+n_se$，不是public模式里的三项和。少了epkv等项不能直接叫“修好原public实现”，而是不同接口与威胁/使用场景。此处只描述影响，不建议改部署模式。[R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14)

### 7.2 HYBRID key-switch key

`EvalMultKeyGen` 先构造 $s^2$ 的旧key，转换到新key s。新s从Q扩P时通过一个塔的中心小整数lift扩展，合法性依赖s确为跨塔一致的小整数。每part生成公开A=a，B满足逐塔
$$B_i=-A_i s_{new}+n_s e_i+P\cdot\mathrm{partIndicator}_i\cdot s_{old},$$
其中P塔的P项为0，Q塔只有该partition的注入项。当前old=s²、new=s。Evalkey不是明文s²；其分布、QP大小和噪声是新的公开攻击面，必须纳入安全参数记录。[R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r20)

使用key时先按分区做gadget/近似基扩展，乘A/B并累加，再从QP近似mod-down掉P。内部局部`t=0`由CKKS的n_s=1等条件决定，是是否做BGV类明文模数修正的开关，不是论文t=2。给定ct/key，这些计算是确定性的；它们的误差包括预置key噪声与近似/取整误差，而非每次relinearize主动采新e。[R18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r18) [N02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n02) [R21](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r21)

BV是另一种gadget体系：digitSize>0按二进制窗口，digitSize=0有每Q塔的专门路径，不使用同样的P升降基。BV的key公式/误差符号应按其独立源码读，不可套HYBRID数组shape；它同样出现private(dgg)，所以不能从技术名称推定σ已经修正。当前paper factory只接受HYBRID。[R19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r19) [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [P18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p18)

## 8. OpenMP 私有 DGG：明确前提而不是凭默认猜值

固定源码在循环外写 `auto dgg = cryptoParams->GetDiscreteGaussianGenerator();`，循环声明 `private(dug,dgg)`，循环体直接构造 `DCRTPoly e(dgg,paramsQP,EV)`。OpenMP private对象按私有局部对象初始化；C++类使用默认构造，非firstprivate复制。DGG的默认构造参数是1.0，构造器调用SetStd并初始化表。由此得到条件表：

| 条件 | 循环内dgg来源 | 可推导σ | 本轮证据 |
| --- | --- | --- | --- |
| OpenMP pragma被编译执行 | 默认构造私有对象 | 1.0 | 源码+标准语义，未构建/采样 |
| pragma忽略的顺序路径 | 循环外context DGG副本 | float3.19f转double | 源码，未运行 |
| 有效OpenMP且请求1线程 | 仍是private对象 | 1.0条件仍成立 | 不是“单线程就变成firstprivate” |
| 历史工作流请求ON且cache命中 | 需要库构建身份才能确定 | 当前不代填 | workflow请求不是库字节/宏鉴证 |

固定源码：[R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r19) [R10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r10) [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) [R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12) [B05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b05)。语言语义依据：[OpenMP 5.0 private/data-sharing initialization](https://www.openmp.org/spec-html/5.0/openmpsu105.html) 与 [OpenMP 5.2 firstprivate](https://www.openmp.org/spec-html/5.2/openmpsu38.html)，本轮实际网页查阅；并未据此声称所用编译器一定符合某个未核实实现版本。

这是一项必须标注的σ来源矛盾，不是已复现的生产bug或S100原FAIL的已证根因。它更可能首先改变evalkey误差和安全配置解释，不能解释成PKE epk/e0/e1已经被证实使用σ1。只读取context.GetDGG().GetStd()不足以区分循环内private对象；可行的后继验证方向是确认实际库构建/宏与局部构造语义，但由根端审阅后再选，不在本轮增补测试。

## 9. 改分布/模式时究竟改变什么

| 变更 | 直接数学作用 | 必须联动/失效 | 本轮态度 |
| --- | --- | --- | --- |
| σ | PKE epk/e0/e1与可能secretGaussian；evalkey须另核private分支 | 新key/PKE样本、统计报告、安全估计、float记录、sampler分支；不能继承旧E80 | 影响分析，不降低噪声求PASS的建议 |
| secret分布或h | e1s、keyswitch新secret扩P前提、稀疏结构安全 | customfactory/guards、所有同根family、所有key、paperh声明和旧样本失效 | 枚举SPARSE不等于可任意h |
| v分布 | epkv的卷积大小与RLWE masking分布 | 加密实现条件、使用场景/安全证明、新ciphertext与实验标签 | 当前h128不扩展到v |
| public→secret Encrypt | 聚合public噪声变为secret加密噪声模型 | 不再同一public payload任务；oracle输入可同而研究条件不同 | 不作为未授权修复 |
| OpenMP/线程/stdlib/PRNG | 随机轨迹、局部DGG构造、拒绝采样与浮点CDF实现 | 构建/运行身份、新随机样本标签，不能保留“同seed配对”无证明说法 | 只记录非秘密构建信息 |
| 输入模长 | 编码整数与八平方条件数 | 输入fixture/true oracle/绝对vs相对门槛/报告名 | 不改变PKE分布，但样本不可默认配对 |

来源：[R07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r07) [R10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r10) [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) [P27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p27) [T01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t01) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09) [H01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h01) [H03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h03)。

## 10. 确定性重现与秘密边界

可公开记录：source/官方pin与构建产物身份、编译器/标准库/宏、CPU字长与平台、是否启用OpenMP、实际线程请求、technique、ordered(q,root)、P、分布枚举与h/σ来源、调用顺序和测试入口、输入fixture hash、scale分数、观察器版本/范数/门槛、非秘密结果汇总。有关库二进制、平台熵提供者和私有变量的结论应各列“已验证/未知”，不能用一个默认profile JSON填平全部未知。

不应写入交付物：生产seed、PRNG完整state、根s或各family秘密投影、epk/v/e0/e1、可重建秘密噪声的未经授权捕获、凭据。即使某个确定性研究环境以后依法记录了seed，它也必须是隔离的非生产实验资产；公开相同seed的承诺并不会自动公开或证明相同内部样本。哈希只能帮助绑定已知对象，不能代替“相同对象是怎样生成的”证据。

本轮没有导出、索取或生成上述秘密，也没有运行sampler。根秘密、PKE噪声、evaluation-key噪声和public参数生成的消费路径均追到了具体源码函数；Karney大σ、binary直接PKE、multiparty/bootstrapping、外部PRNG库实现及OS熵API属于未采用或资料中不可确认的分支，逐项列明而不冒称执行。

唯一下一步建议：**根端复核文档覆盖与事实，再选择具体诊断**。
