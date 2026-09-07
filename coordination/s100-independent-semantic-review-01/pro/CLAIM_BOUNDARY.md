# 诊断结论边界与论文条件对照

任务：`S100-INDEPENDENT-SEMANTIC-REVIEW-01`  
源码：`a448b787399b43b6024d82c170add403969b493c`  
本文使用输入 ZIP 内相对路径；PDF 页码按文件顺序从 1 开始。`D_s` 表示生产 `Decrypt` 的输出经诊断桥接后的第 s 槽，不是理想解密的同义词。

## 1. 观察的究竟是哪三个对象

在原 S100 fresh profile 上，令

\[
N=32768,\quad S=N/2=16384,\quad \Delta=2^{100},\quad
Q=\prod_{i=0}^{10}q_i,
\]

\[
O(c)_s=\frac{1}{\Delta}\sum_{j=0}^{N-1}c_j
\exp\!\left(\frac{2\pi i\,j(5^s\bmod 2N)}{2N}\right).
\]

这里 O 是预期的数学 canonical embedding；实际测试以 binary512/768 算法和十个 Horner 锚点近似观察它。因此后文的相等式在真实记录中还受 `2^-300` 一致性门和浮点舍入限制。

| 对象 | 取得方式 | 不应替换成的概念 |
|---|---|---|
| `z` | `paper_full_test::Inputs()` 的冻结 dyadic 全槽输入；转 ClientReal 后逐槽往返核对 | 论文公开给定的完整实验向量：论文并没有给出它 |
| `m` | `InspectEncoding` 返回的 signed coefficient vector，来自与真实 Encrypt 相同的私有 encoder helper | 通过 production decode 倒推的编码值；也不是在 sampler 内截获的明文 |
| `p` | 对同一 fresh ciphertext 用真实 h128 secret 独立负循环稀疏卷积、逐塔残余类和精确 CRT 重建的中心相位 | 未约减的原始 sampler 多项式，或任意可恢复的历史 lift |
| `d` | 逐系数在整数环中作 **原始** `p−m`，不再 `%Q`、不再 center | `center(p−m)`，也不是 `O(p)−O(m)` 构造出来的槽值 |
| `D` | 官方公开 `Poly*` decrypt 路径、中心化、生产高精度 forward、ClientReal 输出、诊断 bridge 的合成结果 | 绝对无误差的相位／解码真值 |

源码依据：`project/tests/s100_fresh_error_diagnostic_test.cpp:162–181,496–576`；`project/tests/paper_full_eight_square_oracle.h:97–124,198–263`；`project/src/high_precision_client_io.cpp:462–495,597–635,703–755`。

`InspectEncoding` 与真实 Encrypt 不是两个独立 encoder：这正是设计所需的 seam。它通过“确定性同一 helper ＋相同输入／spec ＋前后不变性检查”把 m 与实际公钥加密连接起来，而不是新开一个可绕过生产编码的 raw API。对 m 的**正确性**检验来自独立已知整数 controls 和 O(m) 观察，不来自这两个入口相等本身。

## 2. A／B／C／E0 的精确定义

每个槽位、每个实／虚分量独立定义：

\[
A=O(m)-z,\qquad B=O(d),\qquad C=D-O(p),\qquad E_0=D-z.
\]

在所选相位提升和 O 线性成立的条件下：

\[
O(p)=O(m)+O(d),\qquad A+B+C=E_0.
\]

### 可以说

A 测量所观察编码多项式相对冻结输入的偏移；B 测量**所选中心相位与编码整数的原始差**在 canonical embedding 中的贡献；C 测量生产读出相对该独立相位观察的差；E0 是本次新鲜密文的生产读出总误差。四者是当前样本的点态观察，不是分布参数。

### 不能说

不能仅因 B 最大就宣布“高斯采样器实现错误”或“论文必须使用更小 σ”。B 在典型官方 public PKE 映射下包含公钥噪声乘临时明文、多项式加密噪声以及 secret 乘噪声的合成项，不是单个 e0。

也不能把 C 自动命名为“仅 forward FFT 误差”。它跨过了官方相位计算／CRT 与测试的独立相位路径，以及生产 forward、最终 ClientReal 转换和诊断桥接。若 C 异常，应先在其中定位具体层次，不能只凭名字断定 encoder 有错。

**C 是新鲜解码读出的误差，不是已经写进 ciphertext 的输入误差。** 分析后续八次平方的初始扰动应使用 `A+B`；只有在 C 已证实相对目标可忽略时，才可把 E0 当作其近似。

### 重构的逻辑强度

\[
(A+B+C)-E_0=O(m)+O(d)-O(p).
\]

因此即使生产解码整体多出一个很大的固定偏差 γ，C 与 E0 同时多出 γ，重构仍可成立。当前诊断刻意不对 A/B/C/E0 大小施加 E80 门，所以这种情况返回 COMPLETE 本身不是“偷过精度测试”，而是有效地观察到了较大的 C 和 E0。需要禁止的是把 COMPLETE 改写成 precision PASS。

全槽共同索引错误也可能使归因出错而重构继续成立，具体非锚点置换反例见 `REVIEW.md` R-01。当前源码的索引公式已作静态核对，但测试覆盖不是形式完备的。

## 3. 原始差与 headroom：已经排除了什么

代码要求每个系数满足

\[
2|m_j|<Q,\quad 2|p_j|<Q,\quad
 d_j=p_j-m_j\ \text{（在 }\mathbb Z\text{ 中）},\quad 2|d_j|<Q,
\]

\[
H_j=Q-2(|m_j|+|d_j|)>0.
\]

这给出一个保守、充分的代表元一致性条件：选出的 m、d 可以在不触及中心边界的范围内相加，并等于 p。Q 是奇数，故中心区间端点的实数半整数不会和整数系数相撞。

**对偷换 center 的排除是实质的。** 取 `Q=101,m=49,p=−49`，原始差为 `−98`，必须拒绝；若偷偷 center 会变成 `+3`，造成巨幅差被误报为小噪声。当前 `RawDifference` 以及对应反例均明确避免这件事。见新测试 `:168–201`。

**这个 headroom 不是必要条件。** 取 `Q=101,m=49,p=0`，原始差为 `−49`，m、p、d 分别都在中心区间，但保守三角条件给出 `101−2(49+49)=−95`，仍拒绝。因此 `INSUFFICIENT_LIFT_HEADROOM` 说明本诊断的充分条件未满足，不能倒推出 sampler 已发生 wrap。

**更不能排除不可见的 Q 倍数。** 取 `Q=101,m=3,p=5`，诊断得到 `d=2`、`H=91>0`。但未约减的真实相位噪声若为 `2+101=103`，也给出同一个模 Q 相位 p。两个假设对该观测完全不可区分。这是模运算丢失的信息，而不是多打印几位小数可以解决的问题。

因此输出中的 `hidden_sampler_wrap_claim=NONE` 是必要且正确的边界；`COMPLETE` 和 `positive=1` 均不得被转述成“证明官方噪声未跨 Q”“证明 sampler 的尾概率”或“建立了全局 Gaussian 保证”。

## 4. 精度桥、观察器与极值的位置

### 4.1 三层独立性，不是三种形式证明

生产编码／解码使用自己的十进制 160／220 工作精度和特殊 FFT；测试 observer 使用独立的正向 twist＋普通 FFT 和 binary512/768 根表；锚点用直接 Horner。m、p、d 亦分别计算观察值，未通过相减捏造 B。

但是 binary512 与 binary768 共享算法模板，Horner 只验证十个槽，且都依赖 Boost 的三角函数和常数实现；稀疏解密还依赖官方 inverse NTT。这里没有经严格区间算术认证的全局数值误差上界。准确表述应是“多路径、多精度一致性证据”，不是“512/768 认证了全部 16384 槽的数学真值”。

源码：`project/tests/paper_endpoint_transform.cpp:162–211,228–285,331–339`；`project/tests/paper_full_eight_square_oracle.h:283–314`；新测试 `:387–433`。

### 4.2 哪些转换精确，哪些只是足够精细

在被接受的 fresh 包络中，`||c||₁≤2^164`，整数及 `2^100` 在 binary512/768 内精确可表示。binary512 到 binary768 的扩精度同样不需要牺牲已有二进制尾数位。后续三角求值、乘加和十进制输出桥接则是有限精度运算。

170 位字符串桥先要求 ClientReal 往返相等，避免抹掉它的后端 guard digits；然后转换成 binary512。这个十进制→二进制转换并不是任意输入都“完全无舍入”，只是当前幅度及 512 位精度相对 `2^-300` 门有很大余量。对冻结 dyadic z，代码额外逐分量要求精确相等，不靠“足够接近”混过输入变更。

第一 GREEN 的修正只把 `2^-300` 直接放进目标 binary768 类型，既不改变精度要求，也不消除对真实 compiler 的验证责任。详见 `REVIEW.md` 第 7 节。

### 4.3 最大值不能当成一个槽位的误差分解

记录使用分量最大范数：

\[
\|x\|_{\mathrm{cmp}}=\max_s\max\{|\Re x_s|,|\Im x_s|\}.
\]

A、B、C、E0 的四个最大值各自带槽号和分量；输出会在**各个最大值自己的位置**重算完整带符号 tuple，并非拼接四个最大值。`Update` 对并列值保留首个位置，初始全零时也有确定位置。见新测试 `:448–485,578–613`。

测试的四个分量例子中，A/B/C 最大值分别为 8、9、10，而 E0 最大值是另一个分量上的 7.5。不能说 `8+9+10=7.5`，也不能据不同位置上的正负值宣称抵消。合法的等式是每个相同 `(slot,component)` 上的 A+B+C=E0。

## 5. COMPLETE、INVALID 与舍入上界的含义

| 记录或状态 | 能建立的内容 | 不能建立的内容 |
|---|---|---|
| `COMPLETE mode=controls` | 若真实执行且日志完整：有限小域输入／拒绝／拥有关系／差分和极值 controls 完成 | S100 参数安全、实际公钥加密正确、八平方精度通过 |
| `COMPLETE mode=fresh` | 若真实执行且日志完整：同一 S100 fresh 样本的三项观察、范围／lift、双精度／锚点、同分量重构等检查完成；源码只调用一次 payload Encrypt | A/B/C/E0 必须小，旧 E8 已修好，采样器分布得到统计认证 |
| `INVALID`／退出 2 | 某前提、形状、范围、精度桥、观察一致性、输出或外部调用未被诊断接受；stage/reason 可帮助定位 | 不能一概等同“密码学失败”或“论文错误”，也不能使用不完整项作完整归因 |
| 超时、编译失败、无测试选中 | 没有得到被接受的诊断 | 不是数值精度结果 |
| `lift_check ... positive=1` | 当前中心代表元的充分 headroom 条件成立 | 未约减 sampler lift 没有不可观测的 Q 倍数 |
| `rounding_bound ... established_by_this_run=NO` | 打印并校验一个条件性理论表达式的算术值 | 没有证明其“精确 inverse＋正确 nearest”前提已成立 |
| `expected_openfhe_source=...` | 二进制中的期望 pin 标签 | 单独不能证明实际链接库确由该 pin 构建；须结合 root 的源码／构建 provenance |

最终流刷新有 `REVIEW.md` R-03 的边界。因此表中对 COMPLETE 的解释以“真实成功执行且日志完整”为前提，不能只看 CTest 总体绿色。

### 条件性舍入界的推导

若 \(r=\Delta\,\mathrm{can}^{-1}(z)\) 是精确逆嵌入系数，且 \(m_j\) 确实是正确 nearest integer，则每个舍入误差 \(|m_j-r_j|\le1/2\)，故

\[
|A_s|\le\frac{1}{\Delta}\sum_{j=0}^{N-1}|m_j-r_j|
\le\frac{N}{2\Delta}=2^{-86}.
\]

这个界也上界每个实／虚分量。它不是 `2^-100` 的普遍槽误差界，因为 N 个系数误差会通过嵌入叠加。

但是有限工作精度、双精度舍入一致、以及 A 没超过该界，都不能反向证明每个系数严格正确。例如 z=0 时把应为 0 的常数系数错写为 1，A 仅为 `2^-100`，仍小于 `2^-86`；具体常数错误会被现有零 controls 抓到，但该数学反例说明“满足上界”本身不是 nearest 正确性的充要条件。

当前代码未把这个界用作 A 的接受阈值；其打印逻辑明确标注前提未由本次运行建立。见新测试 `:619–626`。

## 6. 论文 §§2、6.1–6.3 的对应关系

### 6.1 基本语义与不能混淆的约定

论文 §2 的对象是 `Z[X]/(X^N+1)`，模结果选中心提升，编码是 `round(Δ·can^-1(z))`，解码是 `can(m/Δ)`；canonical embedding 取 `ξ^(5^s)`。这些与本切片审查使用的数学目标相符。半整数“向下”与原生产的“歧义拒绝”必须并列说明，不能忽略接受域不同。

论文 §2.1 把 χenc、χerr 和 BDec 放进 setup；其解密正确性并非“任何中心相位都自动是正确明文”。本测试检查的是所选代表元及一致性，未建立论文所有计算阶段的小量条件／BDec 前提。§3–§4 从模同余提升到整数／实数误差等式时也需要非绕回条件；fresh 的一个 headroom 检查不能替代八个 Mult2 阶段的条件。

论文 §2.2 存在已在 `REVIEW.md` R-02 登记的 Tensor 符号局部不自洽。当前 fresh 相位使用 `c0+c1*s` 有官方源码依据；不应把论文那一个显示负号当作反转相位或 B 的依据。

### 6.2 三组实验不是同一个验收对象

| 论文实验 | 主要参数／目的 | 与当前原 S100 的关系 |
|---|---|---|
| §6.1／表 1 | N=2^15、h=21845、8 平方；比较 Mult 与 Mult² 误差增长；Mult² 的 Δ 约 61 位、Mult primes 38 位、Div 23 位 | 不是 h128／S100；图中的误差分量也不是本诊断的 A/B/C |
| §6.2／表 2 | h=21845；比较最大深度，Mult² 达 18 层，6→7 与 12→13 间使用重组再分解刷新 | 不能把此刷新策略默默加进 S100 八平方作为“修复” |
| §6.3／表 3 | 高精度、8 平方；Mult²：N=2^15、h=128、Δ=2^100、dnum=11，Base 50×2、Mult 60×8、Div 40、P 60，最大 QP 约 680 位 | 当前 frozen S100 对应的名义实验配置；不是逐项实现条件已完全给出的规范 |

表 3 的普通 Mult 对照是 N=2^16、dnum=9、约 1000 位 QP；不能拿该行的 paired 50-bit multiplication primes 替换 Mult² 的 60-bit 行。§6.3 也明确说明 60／40 的间隔使其不需要 §6.2 的中途重组再分解刷新。依据：PDF 第 11–13 页，表 1–3 页图及对应正文。

论文使用 CryptoLab HEaaN 的 C++ proof-of-concept，不是 OpenFHE；安全估计引用论文文献 [1,15]。因此“参数位数相同”既不证明采样条件相同，也不自动搬运安全评估结果。依据：PDF 第 11 页 §6 和脚注 5。

### 6.3 原 S100 在当前 OpenFHE 中怎样落实

当前精确参数身份来自 `project/src/repeated_mult2.cpp:23–40,123–146,177–225,279–310,443–466` 和 `project/tests/paper_full_eight_square_oracle.h:29–90`，不是把表 3 的位数当成了具体素数。

名义位数相加是 Base 100＋Mult 480＋Div 40＝**fresh Q 约 620 位**；再加 P 60 才是最大 QP 约 680 位。初始 DCP 去掉 Div 后，pair 活跃模数约 580 位；最后保留两枚 Base。不能把 680 位全部称为 fresh ciphertext 的 Q，也不能把当前精确十一枚 Q 素数／根当成论文刊登的值。

逻辑尺度初始严格为 `2^100/1`，后续按

\[
\Delta_r=\frac{\Delta_{r-1}^{\,2}}{q_{\mathrm{div}}q_{\mathrm{mult},r}}
\]

作为有理数传递。`EncodingParams` 的 nominal 50、ciphertext 的 `noiseScaleDeg=2` 和记录的 `2^100` metadata 不替代这条真实尺度关系。表 3 只说相关素数乘积近似匹配 Δ，不能要求每轮物理尺度仍严格等于 `2^100`，也不能用错误 nominal normalization 掩盖漂移。

OpenFHE 映射固定为 native64/backend4、FIXEDMANUAL、HYBRID、fresh 的 11 个单塔 partition、P 一塔、STANDARD、HPS、COMPLEX，噪声配置 `3.19F`、noiseScale=1、`EXEC_EVALUATION`、`FIXED_NOISE_DECRYPT`，security 字段为 `HEStd_NotSet`。这些是当前源码的具体选择和校验，不是论文逐字给出的每一项配置。

### 6.4 实际 public encryption 噪声，不能只看 h128

当前 client setup 调用官方 ternary sampler 得到 h=128 的 secret，再用官方 secret-key `EncryptZeroCore` 生成公钥。见 `project/src/paper_h128_client_keypair.cpp:193–217`。

在当前 noiseScale=1 下，它给出等价形式

\[
(pk_0,pk_1)=(a s+e_{\rm pk},-a).
\]

官方 public encryption 使用

\[
c_0=m+pk_0v+e_0,\quad c_1=pk_1v+e_1,
\]

所以在模 Q 约减前的代数形式为

\[
c_0+c_1s=m+e_{\rm pk}v+e_0+e_1s.
\]

公钥形式相对论文的 `(-a s+e,a)` 仅是均匀 a 的重参数化，不是符号 bug。依据：`references/official-full/src/pke/lib/schemerns/rns-pke.cpp:111–197`。

一个容易误读的细节是：`SPARSE_TERNARY` 指定 secret 分布，但 public EncryptZeroCore 中只区分 GAUSSIAN 与“其他”，后者构造的是 **默认 h=0 的 dense ternary v**。不能把这个 v 也视为 h128。依据：同文件 `:148–169`；`references/official-full/src/core/include/lattice/hal/default/dcrtpoly.h:111`；`dcrtpoly-impl.h:177–192`；`references/official-full/src/core/include/math/ternaryuniformgenerator-impl.h:103–108`。

另一个细节是官方 h128 secret sampler 内部对正负数量作条件化：正号数量需在 63–65 之间，否则重新生成该向量。实际 source 层的一次 sampler 调用，不能改写成“采样器内部没有拒绝过程”，也不能改写成“在所有恰好 h128 的符号向量上完全均匀”。依据：`ternaryuniformgenerator-impl.h:111–143`。这是所供官方版本的行为，不是本补丁新增重抽样，更不是按 fresh 精度反复试到成功。

因此，如果后来 B 占主导，首要未决问题是**论文具体 χenc／χerr 与当前官方 public-PKE 组合是否相同**，而不是擅自令 v 稀疏、改用对称加密、降低 σ 或去掉某项噪声。

### 6.5 解密模式与安全声明

官方 `Poly*` 解密只有在 `NOISE_FLOODING_DECRYPT` 且 `EXEC_EVALUATION` 时额外加 flooding 噪声；当前 fixed 模式不走该分支。见 `references/official-full/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp:70–95`。`FIXED_NOISE_DECRYPT` 这个名字不意味着新鲜加密没有噪声，也不自动证明可安全公开任意精细解密误差。

当前生产路径没有改成 `DecryptCore`；它延续公开 scheme `Poly*` 路由，而不是高层 Plaintext 的 binary64 Decode。此选择已存在于 baseline。是否满足某种部署解密安全目标，需要单独论证；`NumAdversarialQueries=1` 或 `HEStd_NotSet` 这样的字段不是安全证明。

### 6.6 论文没有给全、必须保留为空的条件

在本包论文主文中，未找到足以唯一确定本实验的完整输入向量或采样分布、实验性输入幅度生成规则、χenc 的具体参数、χerr 的具体标准差／采样算法／截断条件、h128 符号条件化细节、精确素数与根及顺序、完整 HEaaN 构建版本／编码舍入保护实现，以及逐样本 BDec／安全估计配置。§4.3 讨论常见 `||Dcd∘Dec||∞≤1` 的分析假设，不等于公布了 §6.3 的输入生成脚本。

“未给全”应写成来源限制，不是暗示作者采用了零噪声，也不是断言当前 OpenFHE 参数不安全。没有这些条件时，B 的合法观测可以支持“需核对初始条件”，不能独自裁定“就是论文条件不同”，更不能由此自动排除实现缺陷。

## 7. 论文报告的精度与冻结门限

§6.3 报告在其参数和实现下，对八次平方误差的平均无穷范数，给出约 −81.2 和 −81.8 的 log2 精度数值；后者对应 Mult²。论文说明该平均来自 1000 次执行。这是对已发表结果的描述，**本审查没有提出恢复 1000 次试验要求**。

平均值不等于每次运行的确定上界，更不等于当前异库、冻结输入下的单样本承诺。一个纯数学例子是四个非负误差 `(0,0,0,1.15T)`：平均约 `0.2875T`，接近 `2^-1.8 T`，其中仍有一个误差大于 T。这个例子不描述论文真实样本，只说明不能从平均 −81.8 推出每次必过 −80。

当前 frozen oracle 的门是全槽实／虚分量最大值 `≤2^-80`，见 `project/tests/paper_full_eight_square_oracle.h:125–128,181–195`。通常复向量无穷范数按复数模长定义，两者满足

\[
\|e\|_{\mathrm{cmp}}\le\max_s|e_s|\le\sqrt2\,\|e\|_{\mathrm{cmp}}.
\]

度量差至多为半个二进制位，不能解释任务提供的原 E8 约为 `2^-80` 门的十一倍；而且本轮不能借此变更冻结门。论文统计口径与代码门应并列披露，不做未经说明的等同。

**保留的最终边界：原 S100 既有 FAIL 不被撤销；当前 fresh 的 COMPLETE 不是八平方 precision PASS；S116 是另一实验性 profile，不是原 S100 的补发验收。**
