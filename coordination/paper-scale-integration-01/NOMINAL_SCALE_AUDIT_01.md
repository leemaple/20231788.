# Nominal50 元数据独立审查

结论：**有条件接受 nominal50；不接受“当前完整实现已经支持论文参数”或“任意 OpenFHE CKKS API 均兼容”的说法。** Tensor2/RS2 的 recorded 元数据可保持固定点，实际 CRT 运算不要求 Mult 素数也是50位。必须以受约束的 evaluator 调用路径和精确 receipt 解码实现该分离。无需改官方源码。

范围与证据：只读审查，未构建、运行密码运算或访问远端。工程 `9c4d83b5cde16e5c5af89886bd73fe5252a99002`，文档 HEAD `688b3c406e268994efcc58ecb17faf9c611bf5bb`，分支 `codex/three-track-integration-20260905`；既有 STATUS/evidence 工作区变化未动。已读 workflow 及 engineering/model-routing/external-collaboration、完整 TASK。以下 `O/` 指本根 `artifacts/handoffs/paper-final-fable51-01/decoded/official-full/`，其官方 pin 为 `df495ba2e91739a6dc8f1de254fc5a41155ce504`；`O/` 文件路径均从 `src/pke/` 起。论文来源为同一 decoded/paper/PAPER-2023-1788.txt:1561–1594，§6.3/Table3。

## 已观察的符号链

1. `O/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp:63–83,172–180`：CRT rescale 表由实际有序 qi 计算；FIXEDMANUAL 单独设 `m_approxSF=2^p`。`O/include/schemerns/rns-cryptoparameters.h:608–648` 两个 getter 均返回这个 nominal 标量，**GetModReduceFactor 不是实际被删除的素数**。
2. `src/double_ckks.cpp:273–274,840–861,1098–1108`：fresh recorded 必须是 nominal²；raw product 为2^200，Tensor2仅改 metadata 为2^150/degree3；RS2改为2^100/degree2。因此每轮 `e'=2e−2p`，p=50/e0=100 是精确 binary64 固定点，八轮无上下溢。把 p 改100会首先要求 fresh2^200；改60会要求2^120；强塞 e0=100 后 p60 的第六轮 Tensor normalization 下溢为零。这些不应作为生产方案。
3. `O/lib/schemerns/rns-leveledshe.cpp:182–191,479–484` FIXEDMANUAL 乘法只对齐塔；本实现先验证相同 basis/level。`O/lib/schemebase/base-leveledshe.cpp:620–659` 直接多项式乘法再乘 recorded metadata；ScalingFactorInt 仍会模 p 相乘，冻结初值1则一直1。Tensor2 的 cross EvalAdd 两边均三分量；`rns-leveledshe.cpp:402–445` 的 nominal 明文升阶分支只对一分量对象生效。
4. RS2 的 `Rescale` 调用（本地1139–1140）经 `rns-leveledshe.cpp:317–320` 到 `ckksrns-leveledshe.cpp:172–189`：`DropLastElementAndScale` 用真实尾塔表，metadata 独立除 nominal。DCP 本地403–409真实除 d；因此高分量 scale=S/d，重组 scale 精确递推 `S_i=S_(i−1)^2/(d*m_i)`，不是2^100。现有 receipt 在 `src/repeated_mult2.cpp:204–234` 已按整数分子分母执行这个递推；long double descriptor 只能作兼容检查。
5. `O/lib/schemerns/rns-cryptoparameters.cpp:65–180` HYBRID 分区、P选择、QP及NTT从实际 Q/roots 得出。alpha1/maxBits60/auxBits60给一个P；P与Q冲突搜索也独立于p。`O/lib/keyswitch/keyswitch-hybrid.cpp:51–116,308–398` 用真实 basis；noiseScale=1 时 `t=0`（386），不把 nominal50 当模数参与 ApproxModDown。不能放松 noiseScale=1。
6. `O/lib/schemerns/rns-pke.cpp:111–196` 两种 EncryptZeroCore 只读 basis、noiseScale、sigma、分布；SK路线产生 `(a*s+e,-a)`。本地 `paper_h128_client_keypair.cpp:193–217` 恰用它生成PK。公开密钥加密的临时 v 为 ternary，不会重新采样根 h128。`O/lib/scheme/ckksrns/ckksrns-pke.cpp:70–95` 的 Poly* 解密不读取 nominal scale。

## 具体反例与冻结边界

**标准 Decode 是反例。** `O/lib/encoding/ckkspackedencoding.cpp:336–383` 的 FIXEDMANUAL 大整数路线按 `2^(−p*noiseScaleDeg)` 还原，忽略传入 scalingFactor 的真实含义。终态 degree2/p50 会将结果除2^100；输出变成 `z^256*S8/2^100`。探针 `decoded/historical-probe/table3_profile_probe.cpp:36–47` 的 d=1099510054913、所有 m 均小于相应2次幂，故

`S8/2^100 = ∏[2^100/(d*m_i)]^(2^(8−i)) > 1`。

这不是舍入到2^-80可忽略的区别。必须沿用本地 `high_precision_client_io.cpp:563,580–581` 的官方 Poly* 解密与 receipt 精确S变换；任何把终态scale硬写2^100的 binder 都错误。scalar/plaintext 的便捷 API 也不在获准路径内，例如 `ckksrns-leveledshe.cpp:748–758` 实际编码 scalar 并改 degree/scale，不能拿它替代原生 DCRT 整数乘 d。

必须冻结：N32768/M65536/full16384/gap1；实际 Q/root/order 表，B0..B7 各11..4塔且Div始终末位，Mult按尾部依次消费；native64/backend4、FIXEDMANUAL/HYBRID/STANDARD/NOT_SET/COMPLEX、compositeDegree1、noiseScale1、alpha1/aux60；每个实际返回 context 均核验 nominal50 两getter及Q/P/QP，而非只核验请求参数。每轮 Tensor/RS2 必须 degree3/2、recorded2^150/2^100；第八轮 Tensor尚有3塔，满足本地 `double_ckks.cpp:888–889` 检查。一次根h128采样，同符号系数投影产生各family evalrow；重入仅换wrapper，保留精确S和系数；终态在同根/原始prefix验证后归回B0、level9。不能在重入时从 recorded 重建S。

## 最小 RED 与验收

先写一个直接 N32768 的八平方生产测试：使用预先冻结的非零复数满槽向量及独立 `z^256` oracle；实际执行 `Encrypt→DCP→8×Mult2→受信终态绑定→Decrypt`，断言满槽误差≤2^-80、两种工作精度交叉检查≤2^-120，并逐轮检查上述 metadata 和用**测试侧独立整数公式**计算的S。加入定点非零见证：若错误地除2^100，则结果偏差必须超过该误差门限；这是可区分 nominal/actual 混淆的结果断言，不能仅测 receipt 自洽。

当前 `repeated_mult2.cpp:201` 限两family，profile限N64/UNIFORM；I/O限16槽，终态生产绑定尚缺。因此首个缺API编译失败只能记 API RED；修复接口后必须留存真正执行的语义测试结果。上述源码推断允许推进实现，**不构成已运行 GREEN 或精度保证**。本审查不新增1000次实验或强制N256阶段。
