# RELIN2-IMPLEMENTATION-BOUND-01
## 固定 HYBRID Relin2 的双坐标、分区 carry 与整数误差契约

**性质：数学／源码契约交付，不是生产补丁，不是独立审查结论，不是新增 FHE 实验。**

任务取证批次：2026-09-08。生产源码 `a4b815a733efe81897325e2a8e4c826a4ebfa439`；官方 OpenFHE 1.5.0 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。本文件中 `[Cxx]` 是 `CLAIMS.json` 的命题 ID，`[Sxx]` 是 `SOURCE_MAP.tsv` 的源码／文献定位 ID。所有行号按本次附件原字节文件、从 1 开始计数。

## 0. 结论先行：本轮实际补上的内容

论文 Lemma 4.4 的关键中间步骤把两次 Relin 与一次 Relin 的差写成 `(0,e)`，再用秘密的 Hamming weight 控制 `s e`。这不能直接成为当前 OpenFHE HYBRID 的契约：当前实现对**两个输出坐标**分别执行 ApproxModDown，而且输入第三坐标的**分区 digit 会在相加时换代表**。仅把 `(0,e)` 改成两个绝对值不超过 1 的坐标，仍然不够。

本轮给出三个层次的结果。

**已证的源码语义结论。** 在固定一塔一分区、单枚 P、`WITH_REDUCED_NOISE=OFF` 的分支上，分区提升是非负整数余数提升，HYBRID mod-down 是逐系数的向下商；它不是 DCP／CKKS Rescale 使用的中心余数舍入。完整近可加式包含分区代表 carry 引入的 `G_i`，以及两个输出商的 carry `η_i`。raised-high 的额外 d 分区 digit 恒为零，由此得到同一家族 key 下的精确限制恒等式，以及 Relin2 重组后的直接误差契约。证明不需要先假设论文的近可加等式。[C04–C16]

**条件性、但已有公开数值的误差界。** 对诚实生成、固定源代码路径的 evaluation key，有限 Peikert 表成功返回的单系数绝对值最多 39；这个界同时覆盖名义 `3.19F` 和 OpenMP private 默认构造 `1.0` 两种情形，不要求把历史 σ 测成 1。按真实 S100 素数和精确尺度计算，八个家族的未发生 phase wrap 时的局部 Relin2 归一化项，保守界从约 `4.585598×10^-37` 降到 `5.740211×10^-38`，均小于原门槛 `T=2^-80` 的 `2^-40` 倍。这里是**局部项的界**，不是八次平方总误差，也不是 nonwrap 证明。[C18–C19]

**没有得到的结论。** 没有定位到一个错误生产语句，也没有得到生产 FHE RED。原 S100 两平台 FAIL 保持不变。现有 fresh 误差及其传播不能被一份更精确的 Relin2 证明消除。粗糙的确定性 RS 舍入界本身约为 `2.015T`，初次 DCP 后低部乘积的粗界约为 `4.063T`；这说明这些最坏界不足以证明原门槛，而不是说明真实舍入或真实 wrap 必然达到该界。[C24–C30]

唯一后继见 `NEXT_ACTION.md`：**采用带前提的实现契约，关闭“仅因 Lemma 4.4 缺口而必须先改 Relin2／新增随机试验”的分支；本切片无需新增 FHE 运行来作出这一裁定。** 这不是宣布整个复现完成。

## 1. 输入身份、证据级别与阅读边界

原 ZIP 的实际大小为 3,319,606 bytes，SHA-256 为 `1aba188016d93ca8b38ec72f0f2592c6b45de562c455cb33e6d303d95ec6dc4c`。500 个普通唯一成员，展开 10,640,184 bytes；外层 manifest 的 499 项 bytes／SHA-256、CRC、成员闭合均通过。外层 manifest SHA-256 为 `34b3a05c62c48a55dba5bdcdc8fde0e1f397cb831dce35a95f53cc97129422b3`。重算通过 491 个已记录的 Git blob，其中官方源码 331 个。详细逐文件记录在 `evidence/INPUT_VERIFICATION.json`。

任务提交为 `def8ccfde3c67bf00d7dc6a009451704b1b045b4`；协调证据基点为 `bbd4e73af74d1b072e3beb588cf9c7c4de3117cc`。本轮核验的是文件与包内固定提交／blob 回执的一致性，不冒称重新联网证明所有文件的 Git commit membership。

论文以附件 PDF 为准，SHA-256 为 `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`。已查看 PDF 第 4、5、7、8 页图像，重点为 §2.1–2.2、Definition 4.3、Lemma 4.4、Definition 4.5 和 Theorem 4.8。TXT 的 NUL／版式丢失不用于裁定公式。

先沿固定生产源码和官方消费路径建立以下推导，再对照旧图谱与历史报告。`SOURCE_MAP.tsv` 是实际论证覆盖表；读取请求日志中早期较宽的窗口曾发生输出截断，因此该请求日志**不等于**逐行全审证明。没有声称审核全部 500 文件，也没有重放旧全槽捕获。

下文将 DCRT 的加、乘、格式转换视为在已验证的有序模数／根上实现精确商环运算。这是底层环运算语义前提，不是“假设 Relin2 正确”。本轮既未执行、也未逐条重新证明 FFT／NTT／Barrett 的全部底层实现。若底层算子违反此语义，属于另一个需具体反例的原语故障；当前包没有建立这样的故障。

## 2. 对象、代表与范数：三个不能混用的等号

### 2.1 环与整数代表 [C01]

设

\[
R=\mathbb Z[X]/(X^N+1),\qquad N=32768,\qquad R_M=R/MR.
\]

所有整数多项式乘法均在 `X^N=-1` 下做负循环卷积。记 `[f]_M` 为商环元素；记 `U_M(f)` 为各系数落在 `0,…,M−1` 的整数代表；对本任务的奇数模数，记

\[
C_M(f)_t=\bigl((f_t+(M-1)/2)\bmod M\bigr)-(M-1)/2.
\]

`=` 表示 R 或有理系数环中的精确等式；`≡ (mod M)` 表示差属于 `MR`；`≤` 是指定范数下的实数界。商环中除以与 M 互素的 d，不等于实数／有理数中除以 d。只有先证明相应整数提升关系，才允许进入解码误差式。

秘密 `s∈R` 为同根、稀疏三元、恰好 h=128 个非零系数。记

\[
\|f\|_c=\max_t|f_t|,\quad \|f\|_1=\sum_t|f_t|,
\quad \|f\|_{\rm can}=\max_j|\operatorname{can}(f)_j|.
\]

于是 `∥s∥₁=h`，且

\[
\|fg\|_c\le\|f\|_c\|g\|_1\le N\|f\|_c\|g\|_c,
\quad \|f\|_c\le\|f\|_{\rm can}\le N\|f\|_c,
\quad \|fg\|_{\rm can}\le\|f\|_{\rm can}\|g\|_{\rm can}.
\tag{C01}
\]

第一式逐项取绝对值即可；canonical 上界来自 N 项求值，下界来自逆嵌入是带单位复数权重的平均。此处只是数学证明，没有执行变换。

原 S100 `E80` 使用槽实部／虚部分量最大绝对误差。若以 `∥·∥comp` 表示该量，则
`∥z∥comp≤∥z∥can≤√2∥z∥comp`。本文的 canonical 上界也控制 component 误差；不能把历史两种范数的观测值混在一列比较。[S01, S31]

### 2.2 家族基、输入输出和 key [C02]

家族 f=0,…,7 的完整生产基记为 `F_f=d Q_f`，其中

\[
Q_f=\prod_{j=0}^{9-f}q_j,\quad m_f=q_{9-f},\quad Q'_f=Q_f/m_f.
\]

初始 DCP 后，pair 的两部分都在 `R_{Q_f}²`。Tensor2 输出 `H,L∈R_{Q_f}³`。Relin2 内部先把 H 变为 `dH∈R_{F_f}³`，再分别 Relinearize；HYBRID 又分别使用 `F_f P` 和 `Q_f P` 作为**辅助 key-switch 基**。`dQ` 的“raise”和 `QP` 的辅助提升不是同一个操作。

本任务固定

```
d = 1099510054913
P = 1152921504606584833
N = 32768, slots = 16384, h = 128
HYBRID, FIXEDMANUAL, digitSize = 0, noiseScale = 1
numPerPartQ = 1, P 塔数 = 1, auxBits = 60
```

通用 `DoubleCKKS` 还存在 BV 类型的检查；本定理只覆盖本任务 frozen factory 的 HYBRID 对象，不声称覆盖所有可配置实例。

`repeated_mult2.cpp:24–32,130–158,176–209` 固定有序 Q/root/P 和 profile；`351–398,443–463` 在八个 family 上投影同一根秘密并各自生成 evaluation keys。**同根秘密不等于不同家族的 evaluation key 或噪声相同。** 以下“同一个 key”恒指同一家族的同一组 A/B 行。[S02–S04]

## 3. 当前一次 HYBRID Relinearize 的整数模型

### 3.1 `s²→s` key 的精确关系 [C03]

暂固定 active `Q=∏q_j`。输入为 `u=(u₀,u₁,c)∈R_Q³`。`base-leveledshe.cpp:136–144` 先生成 `sOld=s²`，再调用 key switch。`keyswitch-hybrid.cpp:105–119` 在属于分区 j 的 Q 塔加入 `P*sOld`，其他 Q 塔和所有 P 塔不加该项。[S05, S06]

令 `θ_j∈Z_Q` 是第 j 个 CRT selector：在 q_j 上为 1，在其他 active Q 塔上为 0。对该行的任意整数代表 `B_j,A_j∈R`，诚实生成的 key 满足

\[
B_j+A_j s\equiv P\theta_j s^2+e_j\pmod{QP}.
\tag{C03}
\]

`e_j` 是该行 DGG 产生的**一个有符号整数多项式**。DCRTPoly 的 DGG 构造一次生成整条向量后映入各塔，因此不能把每塔噪声当作独立的不同整数多项式。[S07]

秘密的 Q→QP 扩展通过第一个 Q 塔的中心代表完成；稀疏三元秘密在各塔具有一致小系数，故该扩展仍代表同一个 s。`noiseScale=1` 已在 profile 中检查。

本式属于诚实 setup 前提。`DoubleCKKS::Relin2` 对 A/B 行数、类型、key tag、完整 QP 顺序与格式的检查不包含验证隐藏的 key 方程。一个仅通过结构检查、但任意伪造的 A/B 对象，不自动满足 C03。[S06, S08]

### 3.2 一塔一分区：digit 是非负余数，而非中心余数 [C04]

`EvalKeySwitchPrecomputeCore` 复制分区自身塔，转换到 COEFFICIENT，调用 ApproxSwitchCRTBasis，再把补集塔并回 QP。该分支每分区只有 q_j 一塔，故分区内 `QHat=1`、逆元也是 1。`WITH_REDUCED_NOISE=OFF` 时该原语使用未中心化的 NativeInteger 值，因而

\[
D_j(c)=U_{q_j}(c)\in R,\qquad 0\le D_j(c)_t\le q_j-1.
\tag{C04}
\]

其所有补集塔就是这个整数多项式在相应模数下的余数。原分区塔与补集塔因此共同表示 `[D_j(c)]_{QP}`。这里没有浮点舍入；“Approx”并不意味着本路径引入一个未知小浮点误差。[S09–S11]

快路径使用 unsigned 乘加及 128 位归约；对本例单输入塔且 QHat=1，不存在多项求和溢出的积累。未启用 reduced-noise 的备用分支同样是不中心化的乘加。不能把 reduced-noise 分支的 `SwitchModulus` 行借给当前构建使用。[S10, S12]

### 3.3 两个累加器和两个 mod-down [C05–C06]

选定以上 key 的整数代表后，定义**未做模约简的整数**负循环累加器

\[
z_0(c)=\sum_jD_j(c)B_j,\qquad z_1(c)=\sum_jD_j(c)A_j.
\]

实际 extended core 分别在 QP 上累加这两个量的剩余类。P 塔从 full key 取值时使用 `idx=i+delta`，而不是把已删除的 Q 塔误当作 P 塔。[S13]

定义

\[
r_i(c)=U_P(z_i(c)),\quad t_i(c)=\frac{z_i(c)-r_i(c)}P\in R,\quad i=0,1.
\tag{C05}
\]

`ApproxModDown` 的源操作是
`(Z_qi − lift(Z_P)) * PInvmodqi`。P 只有一枚时，PHat 与其逆元都是 1；未中心化的 P→Q 提升等于 `U_P`，所以它确切返回 `[t_i(c)]_Q`。对任意整数 lift z，逐系数即 `floor(z/P)`，不是最近整数。两坐标分别执行：`keyswitch-hybrid.cpp:389–397`。[S10, S11, S14, S50]

若另选 `z_i+QP a_i`，则 `r_i` 不变，`t_i` 改为 `t_i+Q a_i`。因此这是 lift 无关的 R_Q 算子，但不是 lift 无关的实数商。向下商相对 `z_i/P` 的偏差为 `−r_i/P∈[-(P−1)/P,0]`，不能用 `1/2` 替代。

`Relin_Q(u)` 的两个坐标为

\[
\mathcal R_Q(u)=\bigl([u_0+t_0(c)]_Q,[u_1+t_1(c)]_Q\bigr).
\tag{C06}
\]

这是 `base-leveledshe.cpp:335–340` 把 KeySwitchCore 结果分别加入 cv[0]、cv[1] 的直接消费关系。[S15]

### 3.4 解密相位差：整数、模差与边界 wrap [C07–C08]

设 `c̃` 是 c 的任意整数 lift。由 C03，且 `Σ_jθ_jD_j(c)≡c̃ (mod Q)`，存在整数多项式 Γ 使

\[
z_0+s z_1=P\widetilde c\,s^2+E(c)+QP\Gamma,
\qquad E(c)=\sum_jD_j(c)e_j.
\]

代入 C05，得到

\[
\nu_Q(c):=\frac{E(c)-r_0(c)-s r_1(c)}P\in R,
\qquad t_0+s t_1=\widetilde c\,s^2+\nu_Q(c)+Q\Gamma.
\tag{C07}
\]

分子可被 P 整除，是前一个 key 方程模 P 的推论，不是新增舍入假设。因此

\[
\mathcal R_Q(u)\cdot(1,s)\equiv u\cdot(1,s,s^2)+\nu_Q(c)\pmod Q.
\]

一个直接可检查的系数界是

\[
B_Q(c):=\frac{\sum_j\|D_j(c)\|_c\|e_j\|_1+(1+h)(P-1)}P,
\qquad \|\nu_Q(c)\|_c\le B_Q(c).
\tag{C08}
\]

这里明确保留了两个余数、秘密范数和 key 噪声。没有 A/B 本身的巨大范数，是因为它们在**解密方程**中按 C03 消去；不是忽略了 key 项。

若 `μ=C_Q(u·(1,s,s²))`，定义

\[
w=\frac{\mu+\nu_Q(c)-C_Q(\mu+\nu_Q(c))}Q\in R.
\]

则中心化解密前后差为 `ν_Q(c)−Qw`。而中心化的**模差**仅为 `C_Q(ν_Q(c))`，其系数范数不超过 B。两者不同。`∥μ∥c+B<Q/2` 是使 w=0 的充分条件，不是 w=0 的必要条件。[C17]

## 4. 完整近可加式：不能只修一个坐标

### 4.1 digit 代表 carry 和输出商 carry [C09]

设 u、v 在同一 active Q、同一 key 下，w=u+v（商环加法）。其第三坐标分别为 c_u、c_v、c_w。由 C04，每个系数都有

\[
\kappa_j=\frac{D_j(c_u)+D_j(c_v)-D_j(c_w)}{q_j}\in\{0,1\}^N.
\]

记 `K_{0j}=B_j, K_{1j}=A_j`，定义

\[
G_i=\sum_j q_j\kappa_j K_{ij},\quad
\gamma_i=U_P(G_i),\quad g_i=(G_i-\gamma_i)/P,
\]
\[
\eta_i=\left\lfloor\frac{r_i(c_u)+r_i(c_v)-\gamma_i}P\right\rfloor,
\qquad \eta_i\in\{-1,0,1\}^N.
\]

上述乘法中的 `κ_j K_ij` 仍是负循环卷积，不是逐系数相乘。由 raw 累加器精确关系 `z_i(c_w)=z_i(c_u)+z_i(c_v)−G_i`，逐系数欧几里得除法给出

\[
\boxed{\quad
\mathcal R_Q(w)_i-\mathcal R_Q(u)_i-\mathcal R_Q(v)_i
\equiv-g_i+\eta_i\pmod Q,\quad i=0,1.
\quad}
\tag{C09}
\]

等价写法为

\[
\delta_i\equiv-\frac{G_i+r_i(c_w)-r_i(c_u)-r_i(c_v)}P\pmod Q.
\]

这是当前路径需要的完整二维关系。`−g_i` 不能一般丢掉：即便解密后较小，两个 ciphertext 坐标本身可以很大。

只有额外满足**所有分区 κ_j=0**时，G_i=0，才有两个坐标分别为 `η_i∈{0,1}^N`。此时解密 carry 的界是 `1+h`，而不是 h。注意方向：本文 δ 定义为“一次减两次”；若写“两次等于一次加 correction”，correction 是 `−δ`。[C11]

### 4.2 解密后的近可加误差 [C10]

令 `Δr_i=r_i(c_w)−r_i(c_u)−r_i(c_v)`。直接相减 C07 得到**整数恒等式**

\[
\Delta\nu:=\nu_Q(c_w)-\nu_Q(c_u)-\nu_Q(c_v)
=\frac{-\sum_jq_j\kappa_j e_j-\Delta r_0-s\Delta r_1}P\in R.
\tag{C10}
\]

它给出 `δ₀+sδ₁≡Δν (mod Q)`，并有

\[
\|\Delta\nu\|_c\le
\frac{\sum_jq_j\|\kappa_j\|_c\|e_j\|_1+2(1+h)(P-1)}P.
\]

其中 `Δr_i` 的逐系数范围包含 `−2(P−1)`，所以这里的系数 2 不能无理由改成 1。若对 δ 的各坐标选择中心代表，其解密整数值与 Δν 还可能相差 Q 的倍数；那些是代表选择项，不能自动解释为明文已经发生物理 wrap。

### 4.3 小模型反例与当前 S100 参数的符号见证

`checks/model_checks.py` 的 M01 使用公开合成单分区 `q=11,P=17,s=1,A=B=10,e=3`，满足 `B+As=P s²+e`。令第三坐标输入为 1 和 1、前两坐标为零。两次向下商分别为 `(0,0)`，合并输入后的向下商为 `(1,1)`。所以 δ=`(1,1)`，第一坐标不为零，解密 carry=2，而 h=1。这里没有 digit carry，已经足以破坏过强的第一坐标为零断言。同一常数代入论文 §2.2 的逐坐标最近舍入公式，δ 变为 `(−1,−1)`，第一坐标仍不为零；M01 同时核验了这个区别。

M04 则使用两个分区 `q₀=5,q₁=11,P=17,s=1`，CRT selectors 为 11、45，合成行 `(A₀,B₀,e₀)=(30,158,1)`、`(A₁,B₁,e₁)=(7,756,−2)`。输入第三坐标 4 和 4 相加时，q₀ 分区产生 κ₀=1，q₁ 分区 κ₁=0。计算得到

```
G = (790,150), eta = (-1,0)
实际 delta 的中心代表 = (8,-8)
错误地遗漏 G 后的预测    = (-1,0)
```

两坐标并非有界单位 carry；但解密差在本例恰好为 0，与 C10 完全一致。该例正说明“坐标很大”和“解密误差很大”不能混为一谈。

M13 进一步使用**当前 S100 的真实公开 Q/d/P、N=32768 和 h=128**，但仍仅构造公开的代数见证。取 `s=1+X+…+X^127`、`J=1+X+…+X^(N−1)`、`A₀=((P+1)/2)J`、其他 A 行为零、所有 e 行为零，并按完整 F=dQ 的 selector 定义 `B_j=Pθ_j s²−A_j s (mod FP)`。第三坐标 1 与 1 相加，在全部分区均无 digit carry。

只需检查 256 个系数类别：`s²` 是三角系数，`sJ` 在 k<127 时系数为 `2k−126`、其后恒为 128，所以所有 k≥255 共享一个计算类别。实际向下商给出

```
delta_0[k] = 0 (k<64), 1 (k>=64)
delta_1[k] = 1 (所有 k)
||delta_0+s*delta_1||_c = 129 = h+1 > h
```

这说明修正的 `1+h` 在该公开参数、h128 的代数契约中可以达到；并非仅因把生产 N/h 缩成 1 才出现第一坐标 carry。它不是生成 256 个随机样本：256 是一个确定性多项式的系数分类数；没有进行任何变换。该见证满足所用参数、稀疏三元条件和 key 方程，但不声称历史真实 key 等于它，也不对特定 setup 分布抽中它的概率作任何判断。

这些是满足相应 key 方程的**合成整数模型**，不是生产 S100 密钥、不是运行 OpenFHE 的实测反例，也不单独证明 Lemma 4.4 最终数值不等式在其所有约定下均为假。它们否定的是过强的中间等式及不适用的实现迁移。[C09–C11, S01]

### 4.4 超出当前 profile 时还要保留什么 [C12]

对包含多个 q_i 的分区、分区模数 M_j，未中心化 ApproxSwitchCRTBasis 的整数模型是

\[
D_j=\sum_{q_i\mid M_j}U_{q_i}\!\left(c_i(M_j/q_i)^{-1}\right)(M_j/q_i)
=U_{M_j}(c)+M_j a_j,
\]

其中每系数 `0≤a_j≤分区塔数−1`。同样，如果 P 有多枚 prime，P→Q 使用的余数提升是

\[
r_i^{\rm app}=U_P(z_i)+P\beta_i,
\quad 0\le\beta_i\le P\text{ 的塔数}-1,
\]

相应 mod-down 是 `floor(z_i/P)−β_i`。输入相加时 a_j、β_i 的变化，以及分区被截短、表索引变动，都必须进入 carry 契约。reduced-noise 分支还改变余数代表，不能沿用本节的非负范围。

本任务 α=1、单 P，使**分区内部**的 a_j 与 β_i 为零；这绝不意味着不同输入相加产生的 κ_j 为零。M12 用 `5×11` 的 CRT 提升 `1→56=1+55` 区分这两件事。本节仅给出由已读原语产生的未中心化基转换模型，不声称已完成任意 HYBRID 配置的完整误差定理。

## 5. Relin2 的直接定理：绕开错误近可加前提

### 5.1 raised-high 的零 d digit 与限制恒等式 [C13]

源码 `double_ckks.cpp:1011–1023` 将 H 的每个 active 塔乘 d，再追加**全零 d 塔**。对任意 H 的整数 lift，它表示 `[dH]_{F}`，F=dQ；这是合法的 lift，因为换 lift `H+QJ` 后 dH 仅变化 FJ。

在一塔一分区下，第三坐标的 d digit 恒为 `U_d(dH₂)=0`。其他 q_j digit 为 `U_qj(dH₂)`，与直接在 active Q 上对 dH 作 key switch 相同。key 的 Q 塔和 P 塔限制也相同：`EvalFastKeySwitchCoreExt:425` 的 delta 索引确保 active 核心仍取 full key 的 P 塔。[S08, S09, S13]

因此，额外 d 行贡献为零；两个 QP 累加器在保留 Q 与 P 塔后逐塔相同。P 余数相同，mod-down 的各 native Q 输出也相同。得到**坐标级商环恒等式**

\[
\boxed{\ \pi_Q\mathcal R_F(\operatorname{raise}_d H)
=\mathcal R_Q(dH)\quad\text{in }R_Q^2.\ }
\tag{C13}
\]

右侧是用**同一家族、同一组 key 行**限制到 active Q 的语义算子；没有声称另一个家族的新 key 会给出相同 ciphertext。这个结论不需要 key 噪声独立性，甚至坐标级等式本身只依赖结构相容，不依赖隐藏 key 方程。

### 5.2 DCP 的中心余数以及它怎样精确消去 [C14–C15]

对 `A=Relin_F(raise_d H)`，令各坐标整数 lift 为 A_i，中心 d 余数为 `ρ_{d,i}=C_d(A_i)`。DCP 的输出在 R_Q 中满足

\[
\widehat A_i\equiv(A_i-\rho_{d,i})/d,\qquad
\check A_i\equiv\rho_{d,i},\qquad
[d\widehat A_i+\check A_i]_Q=[A_i]_Q.
\tag{C14}
\]

源码 `DropLastElementAndScale` 先以 NativeVector 的中心规则 `SwitchModulus` 提升最后一塔，再乘 `−d^{-1}` 加到 `d^{-1}A_i`。这里的余数是真正的**中心余数**，与 §3.3 的非负 P 余数不同。`double_ckks.cpp:415–426` 又通过 `sourcePrefix−d*high` 得到 remainder。[S16–S18]

Relin2 的低部是 `check A + Relin_Q(L)`。由于 FIXEDMANUAL 下这两个已验证同层、同基、同 component 数的 ciphertext 相加不会额外 rescale，重组后

\[
\boxed{\operatorname{RCB}_d(\operatorname{Relin2}(H,L))
=\mathcal R_Q(dH)+\mathcal R_Q(L)\quad\text{in }R_Q^2.}
\tag{C15}
\]

DCP 的两个坐标舍入并未被忽视：它们已在 `d*high+remainder` 中**代数上精确抵消**。[S08, S19, S20]

### 5.3 实现版 Relin2 误差契约 [C16–C17]

写

\[
T=(dH+L)\cdot(1,s,s^2)\quad\text{的任意整数 lift},
\quad \nu_2=\nu_Q(dH_2)+\nu_Q(L_2).
\]

由 C07、C15，重组相位满足

\[
\operatorname{RCB}_d(\operatorname{Relin2}(H,L))\cdot(1,s)
\equiv T+\nu_2\pmod Q,
\quad \|\nu_2\|_c\le B_Q(dH_2)+B_Q(L_2).
\tag{C16}
\]

**这里没有 `d·ν_Q(H₂)` 项。** 当前实现是先把输入乘 d，再做 key switch；不是把已经 relinearize 的高部结果乘 d。也没有必要用未经证明的 `E_Relin+h` 替换右侧。

设输入中心相位 `τ=C_Q(T)`，输出中心相位 `τ'=C_Q(T+ν₂)`。则

\[
\tau'-\tau=\nu_2-Qw_2,\quad
w_2=(\tau+\nu_2-C_Q(\tau+\nu_2))/Q\in R.
\tag{C17}
\]

如果 `∥τ∥c+B_Q(dH₂)+B_Q(L₂)<Q/2`，则 w₂=0。无此前提时，仍可无条件断言模差为 `C_Q(ν₂)`，而不能断言两个中心解密值之差只有 ν₂。数字公开界还可证明本配置 `∥ν₂∥c<Q/2`，但这仍不能排除 τ 本身贴近 Q/2 时发生输出 phase wrap。

如需对照论文写成“先 Relin(dH+L) 再校正”，则
`ν₂=ν_Q(dH₂+L₂)−Δν`，其中 Δν 必须按 C10 计算。只有无 digit carry 的额外条件下，才可简化为“一次 Relin 的界 +1+h”。一般情形直接使用 C16 更清楚，且避免把巨大 ciphertext 坐标 correction 当作解密误差。

## 6. key 噪声的公开确定性界，而非改 σ 追求通过

### 6.1 成功生成的有限支持 [C18]

`KeySwitchGenInternal` 的 `auto dgg = context DGG` 位于 parallel for 外；其 pragma 明确写 `private(dug,dgg)`。在启用并按该 OpenMP private 类对象规则执行时，私有 DGG 默认构造，默认标准差为 1；忽略 pragma 的对应顺序路径使用外面的名义 3.19F 对象。包中 S100/S116 两平台 configure/build 记录支持 OpenMP ON、found OpenMP 和 reduced-noise OFF，但没有读取历史局部 DGG 对象来测 σ。[S06, S21, S32]

本契约不选择较小 σ。DGG 源码在 std<300 时使用 Peikert 有限表，长度
`fin=ceil(std*M)`，`M=12.00610553538285`；成功返回的是 0 或 ±(1,…,fin)，查不到时抛异常，不会成功返回表外整数。DCRT key error 构造消费的就是此 `GenerateIntVector` 路径。[S07, S21]

因此，在本诚实 setup 范围，σ=1 时系数界为 13，名义 σ=3.19F 时为 39。保守有理包络 `M<12.007, 3.19F<3.20` 也给出 `ceil(12.007×3.20)=39`。以下统一采用

\[
\|e_j\|_c\le39,\qquad \|e_j\|_1\le39N.
\tag{C18}
\]

这不是无限支撑理想离散高斯的无条件界，而是已读实现成功返回值的有限支持界。没有假定噪声独立、均值为零、典型值接近 σ，亦没有把 evaluation key 的局部 DGG 推成所有公钥／payload 噪声均为 1。历史实际 σ 和实际系数仍为未观测，未索取。有限支持界也不构成分布等价、随机性质量或 RLWE 安全证书。

### 6.2 当前八个家族的公开界 [C19]

由 C04、C08、C18，可直接取

\[
B_K(Q)=\frac{39N\sum_{q_j\mid Q}(q_j-1)+(1+h)(P-1)}P,
\quad B_2(Q)=2B_K(Q),
\quad B_{2,\rm can}=N B_2(Q).
\tag{C19}
\]

这给出了真正不依赖秘密系数、实际 digit 或最终误差的上界。更紧的诊断界可以使用真实 `∥D_j∥c、∥e_j∥₁` 的受控聚合，但**本轮的公开界不需要它们**。

所有八个 family 均满足 `B₂(Q)<Q/2`。使用精确 S_f 后，进入一次 Mult2 归一化式的 Relin 项是 `d N B₂(Q)/S_f²`。下表是整数／Fraction 计算结果的七位有效数字截断显示，精确分子分母在 `checks/public_bounds.json`。它不是测量值或性能时间。

|f|active Q 塔数|B₂ 系数界|局部 Relin 项界|该项／T|
|---:|---:|---:|---:|---:|
|0|10|2.045248e7|4.585598e-37|5.543648e-13|
|1|9|1.789657e7|4.012534e-37|4.850856e-13|
|2|8|1.534067e7|3.439463e-37|4.158056e-13|
|3|7|1.278476e7|2.866383e-37|3.465244e-13|
|4|6|1.022886e7|2.293289e-37|2.772416e-13|
|5|5|7.672961e6|1.720182e-37|2.079573e-13|
|6|4|5.117057e6|1.147075e-37|1.386729e-13|
|7|3|2.561153e6|5.740211e-38|6.939489e-14|

这些数值否定了“必须先拿到历史噪声系数，才能对当前 Relin2 给出任何有用最坏界”的说法。它们**不**证明八步都 nonwrap，不证明小扰动不会跨越相位边界，不是把八个局部误差直接相加得到全链误差，也不证明改变 Relin2 后低部轨迹完全不变。

## 7. 接入 Tensor2、RS2 与精确尺度

### 7.1 先选输入相位 lift，再谈实数乘法 [C20–C21]

对两输入 pair，令其高、低 ciphertext 的中心相位分别为 `h_a,ℓ_a,h_b,ℓ_b∈R`。定义

\[
M_a=d h_a+\ell_a,\quad M_b=d h_b+\ell_b,
\quad T^*=d h_a h_b+h_a\ell_b+\ell_a h_b.
\]

源码 Tensor2 的高部是 high×high，低部是两项正交叉项之和，故其重组相位与 T* 模 Q 同余，并且在整数环中

\[
dT^*=M_aM_b-\ell_a\ell_b.
\tag{C21}
\]

这里没有对模 Q 的乘积直接作有理除法。生产正号与 `(1,s,s²)` 相位相容；不依据旧论文符号疑点改负号。[S22, S23]

若 `μ_a=C_Q(M_a)`，则 `M_a=μ_a+Qα_a`，类似定义 α_b。希望右侧表示输入的中心解码值乘法时，需要 α_a=α_b=0；充分条件是 `d∥h_a∥c+∥ℓ_a∥c<Q/2`，另一输入同理。未满足时，须额外保留

\[
\frac{Q\operatorname{can}(\alpha_a\mu_b+\alpha_b\mu_a)
+Q^2\operatorname{can}(\alpha_a\alpha_b)}{S_aS_b}
\]

而不是把这些项藏在浮点误差内。本文主公式先用明确的 M_a、M_b，因此即使 α 未知也没有等号偷换。

对 Relin2 输出的低部，有

\[
\ell_{\rm rel}\equiv h_a\ell_b+\ell_a h_b+\nu_Q(L_2)+\rho_{d,0}+s\rho_{d,1}\pmod Q,
\]

其中 `∥ρ_d,0+sρ_d,1∥c≤(1+h)(d−1)/2=:R_d`。DCP 的误差虽然在**当步重组**中消去，仍决定下一轮低部的大小；不能从 C15 推断低部误差永远不存在。[C20]

### 7.2 RS2 只在重组值中保留一次普通 Rescale 舍入 [C22]

固定本步 m=m_f，Q'=Q/m。`RS2` 确实分别调用 Rescale(high) 和 Rescale(RCB(pair))，然后令
`low_next = rescaled_recombined − d*rescaled_high`。因此

\[
\operatorname{RCB}_d(\operatorname{RS2}(\mathrm{pair}))
=\operatorname{RS}_m(\operatorname{RCB}_d(\mathrm{pair}))\quad\text{in }R_{Q'}^2.
\tag{C22a}
\]

这是精确相消，不能把 high 的那次 rescale 舍入再重复加入重组误差。[S24]

设 Relin2 的重组 ciphertext 为 C=(C₀,C₁)，其中心相位为 `φ=C_Q(T*+ν₂)`。定义**中心 m 余数**

\[
r_{m,i}=C_m(C_i),\qquad
\rho_m=-\frac{r_{m,0}+s r_{m,1}}m\in R\otimes\mathbb Q,
\qquad \|\rho_m\|_c\le R_m:=\frac{(1+h)(m-1)}{2m}.
\]

rescale 表中的系数确为 `−m^-1 mod q_i`：令 `Q'=Q/m`，表计算 `floor((Q'^{-1} mod m)Q'/m)`；因 `(Q'^{-1} mod m)Q'=1+km`，故 `k≡−m^-1 (mod q_i)`。结合中心 `SwitchModulus`，输出 native 塔代表 `(C_i−r_m,i)/m`。[S17, S18, S25–S27]

定义整数多项式

\[
w_{\rm pre}=(T^*+\nu_2-\phi)/Q,
\quad w_{\rm out}=\frac{\phi/m+\rho_m-C_{Q'}(\phi/m+\rho_m)}{Q'}.
\]

`φ/m+ρ_m` 是整数多项式：C 的相位与两个中心余数的相位模 m 相等。最终中心相位 y 满足

\[
\boxed{\ y=T^*/m+\nu_2/m+\rho_m-Q'(w_{\rm pre}+w_{\rm out}).\ }
\tag{C22b}
\]

这里 `w_pre` 是相对于 T* 的输入相位代表变化，`w_out` 是最后一次中心化变化。不能把模差、各坐标代表变化和这两个 phase wrap 项合并成一个无条件“小舍入误差”。

### 7.3 正确归一化与可检查的 local-error 不等式 [C23–C24]

精确尺度由 frozen receipt 计算：`S₀=2^100`，`S_next=S_a S_b/(d m)`，平方时为 `S_f²/(d m)`。兼容 `recordedScalingFactor` 不是这条等式的代用品。[S28]

令 `W=w_pre+w_out`，把 C21、C22b 除以精确 S_next 并应用 canonical embedding，得到

\[
\boxed{
\frac{\operatorname{can}(y)}{S_{\rm next}}
=\frac{\operatorname{can}(M_a)\operatorname{can}(M_b)}{S_aS_b}
-\frac{\operatorname{can}(\ell_a\ell_b)}{S_aS_b}
+\frac{d\operatorname{can}(\nu_2)}{S_aS_b}
+\frac{dm\operatorname{can}(\rho_m)}{S_aS_b}
-\frac{Q'}{S_{\rm next}}\operatorname{can}(W).
}
\tag{C23}
\]

canonical 槽乘法为逐槽乘。只在输入 α_a=α_b=0、且 W=0 时，才可以直接对**中心输入解码值的乘法**使用

\[
\left\|\frac{\operatorname{can}(y)}{S_{\rm next}}
-\frac{\operatorname{can}(\mu_a)\operatorname{can}(\mu_b)}{S_aS_b}\right\|_{\rm can}
\le\frac{L_aL_b+dN B_2(Q)+dm N R_m}{S_aS_b},
\tag{C24}
\]

其中 `L_a≥∥ℓ_a∥can,L_b≥∥ℓ_b∥can`。这三项依次是被丢弃的低部乘积、两次实际 key-switch 误差、重组值的一次 RS 舍入。

对单步，一个非循环、完全明确的充分条件为

\[
\|T^*\|_c+B_2(Q)+mR_m<Q/2,
\tag{NW}
\]

另加上述两个输入的 α=0 条件。NW 同时留出了 Relin2 与末次 RS 的相位余量，推出 w_pre=w_out=0；它不要求预先知道最终误差小于 T。

如仅有高低部系数上界 `H_a,H_b,L_a^c,L_b^c`，可用

\[
\|T^*\|_c\le N(dH_aH_b+H_aL_b^c+L_a^cH_b)
\]

代入 NW。这可能很保守，但每一项含义明确。

### 7.4 为后继分析保留高低部递推，而不假装全链已证 [C25]

中心化不会增大**系数**绝对值，因此下面系数范数递推可以在商环相位关系上给出，无须先把所有 phase wrap 设为零。记输出高、低中心相位的上界为 H_next、L_next^c，则

\[
H_{\rm next}\le\frac{N H_aH_b}{m}+\frac{B_K(Q)+R_d}{dm}+R_m,
\]
\[
L_{\rm next}^c\le\frac{N(H_aL_b^c+L_a^cH_b)+B_K(Q)+R_d}{m}+(1+d)R_m.
\tag{C25}
\]

第一式来自 raised-high 的相位是 `d h_a h_b+ν_Q(dH₂)`，DCP 除以 d 并减去 d 余数相位，再对 high 做 RS。第二式来自低部式 C20，以及 `low_next=RS(C)−d RS(high)`。两次 RS 在**低部**中不相消；这与重组中的 C22a 没有矛盾。

初始 DCP 若已获得上游 fresh 中心相位系数界 M₀，可取 `H₀≤(M₀+R_d)/d`、`L₀^c≤R_d`。这是一个真实的上游输入接口，不是“假设最终 E8 合格”。递推中的 N 因子很可能过松；不能擅自把“中心化不增大系数范数”替换成“中心化不增大 canonical 范数”。本轮未把这些粗递推包装成八步成功证书。

## 8. nonwrap、最坏精度和论文界的准确裁定

### 8.1 可以证明什么，缺什么 [C26]

对于一次 Relin2，公开参数加诚实 key 支持界已经给出有用的 `B₂(Q)`。对于整个原 S100，参数本身不包含具体未模化 T* 的大小；结构合法的 ciphertext 也不自动具有小相位。完成 C24 的实际 nonwrap 前提，需要上游 M₀／各步高低部界，或者受控客户端诊断给出的**相位余量聚合证书**。不需要向评估器提供秘密、明文答案或噪声系数。

本轮没有此类原两条历史链的全量 lift 捕获，因而 `α_a=α_b=W=0` 的逐步历史事实保持未证。仅知道 A8 很小，是末端误差观察，不能代替这些中间前提。

另外，数值障碍并不是一句“论文信息不全”。按 C24 的确定性粗界：

* 每步 RS 项 `N R_m/S_next` 约为 `2.014889T–2.015622T`，它一个上界就已超过 T。
* 初始 DCP 的 `∥ℓ₀∥can≤N(1+h)(d−1)/2` 给出第一步低部乘积界约 `4.062732T`，也不能证明 T。
* 参数几何上，f=7 的 `S_f²/d` 约为该 active Q 的 `Q/2` 的 `2.000729` 倍。这个比值只说明使用单位幅度及三角不等式的简单上界可能失败；它既不是实际系数幅度，也不是 wrap 观测。

这些界过大来自最坏符号同向、`coefficient→canonical` 的 N 损失、低部大小及后续传播没有利用结构抵消。它们不是实际误差下界。纯参数无法证明一个已观测 FAIL 的全链 PASS；需要区分“改进证明的紧度”和“改变实际输入／随机样本／实现”。

### 8.2 对 Lemma 4.4 的分类 [C27]

论文 §2.2 的 Relin 公式对 key 的两个坐标进行舍入；Lemma 4.4 证明却直接把近可加 correction 限于第一坐标为零，缺少支持这个限制的推导。对本文实现，还额外存在 digit 代表 carry 与非负 mod-down 语义差异。因此本轮裁定是：

**论文该中间等式存在证明缺口，且 `E_Relin+h` 不能未经重建就当作本 HYBRID 的实现保证。** 不是仅把一个经验常数稍微调大；也不是已发现当前生产算法违反了它本来应满足的 C03–C25 契约。

替代结论是 C16 的 `B_Q(dH₂)+B_Q(L₂)`，并由 C17／C22b 明确保留 phase wrap。对特殊无 digit carry 情形，才有“一次 Relin 界 +1+h”的更紧比较式。关于论文 Tensor 符号和 Theorem 4.8 归一化的旧疑点，仅在 C21／C23 中保持正确归一化，没有将它们当成本轮新发现或生产改符号的理由。

## 9. 必要前提与当前满足情况

|前提／对象|在本文的作用|当前证据状态|
|---|---|---|
|固定有序 Q/root/P、α=1、P 单塔、FIXEDMANUAL、ns=1|C04–C16 的具体算子|固定 factory 与 guards 可定位；公开常量已复算|
|reduced-noise OFF 的原语分支|保证非负 lift／floor，而非中心 lift|固定源分支已读；S100/S116 configure/build 记录支持 OFF|
|DCRT 算子正确实现指定商环与格式转换|将源操作解释为 R_Q 运算|基础语义前提；本轮未执行／全审底层 NTT|
|诚实 `s²→s` evaluation key，h128 同根 secret|C03、C07、C08|factory 路径支持；结构校验不能认证任意外来 key 的隐藏方程|
|成功 Peikert 返回值支持 ≤39|公开 B_K，无须实际噪声|源码条件可证；历史局部 σ、实际系数未测|
|所有 digit 加法无 carry|只用于 C11 的特殊简化|**不是源码保证；一般不成立**，不用于主定理|
|输入相位 lift 与中心输入一致、逐步 W=0|把 C23 化成无 wrap 的真实解码误差界|原历史全链未在本轮核证，JSON 为 null，不置 true|
|低部范数和 fresh 初值界|全链传播与 nonwrap|可由明确接口／递推／受控诊断补充；本轮只有局部和粗界|
|所有 public-key／payload 噪声都是 σ1|无正当用途|不采用，不从 private evaluation DGG 推出|

原 S100、S116 的 Linux GNU13.3/OpenMP4.5 与 Windows GNU16.2/OpenMP5.2 取自本包实际 configure/build 取证记录，不是本轮重新构建。annulus 的依赖恢复缓存并跳过构建，完整缓存二进制身份未闭合；不把它视作同一份已测 sampler 的旁证。[S32]

## 10. 对当前故障的有限裁定

### 10.1 旧失败样本与新样本必须分开 [C28–C29]

`fs-endpoint-live-run-01/ACCEPTANCE.md:24–33` 的原两条链记录如下。它们均采用原 component norm，T=`2^-80`：

|原样本|E8|I8|A8|A8／T（约）|
|---|---:|---:|---:|---:|
|Linux|9.14647363e-24|9.14836661e-24|4.56710917e-26|0.0552|
|Windows|9.06530517e-24|9.06562720e-24|8.30355460e-26|0.1004|

E8 约为 11T；I8 主导。该记录还给出 `I_lower−A_upper>T` 的 conditional allowance 结论。本轮未重算旧全槽文件，仅引用包中已接受记录。不能据本表的有限位显示值重新宣称建立了历史独立区间证书。

`GREEN2_RESULT.md:17–26` 是后来独立生成的 fresh 诊断样本：编码 A 约 `1.304830266e-28`，聚合公钥加密贡献 B 约 `3.370144334e-25`，decoder C 约 `1e-128`。这个样本 B 主导，但不能反向认定原两条链每个噪声系数、每个消费者或每个 draw 相同；B 也不是“已找到某一行 PKE bug”的同义词。[S31, S33]

S116 的另一个固定参数 profile 在两平台通过原 component 阈值，是改参结果。annulus 的 inward 输入、新 key/noise 的一次 Linux PASS 是另一条件，报告还采用 complex-modulus norm；Windows 未运行。两者不能覆盖原 S100 原条件 FAIL，也不能替代原历史中间状态的 nonwrap 证明。[S34, S35]

### 10.2 有没有生产语句与可区分 RED？

**没有。** 本轮发现／修复的是分析契约缺口，而不是一个已定位的实现违约。C09 的模型负例不等于生产 RED；C13、C15 反而解释了为什么现有 high raise＋DCP 不能随意改成“分别 Relin 后重组”。

局部 Relin 归一化上界极小，说明无需继续以“Relin2 误差没有任何可计算界”为理由阻塞分析。但没有中间 nonwrap 余量时，不能据此宣称整个 Relin2 从不涉及边界问题；很小的扰动仍可在贴边输入上改变 wrap。对现有样本，也不能把“修得更精确”解释为可以消去其已存在的 inherited fresh 误差。

即使把整个 post-fresh 算术换成理想精确计算，固定 fresh 样本的继承误差 I8 仍在；接受记录中的 I8 已大于 T。故“单靠消除 Relin2／后继算术误差就能使这些固定样本过门”的动机没有证据基础。刻意让算术误差抵消 fresh 误差不属于正确复现，也不构成应追求的修复。[C30]

## 11. 已执行的有界检查与新增量

`checks/model_checks.py`：13 项确定性检查全部通过，包括两个坐标的反例、分区 carry 反例、完整修正关系正例、N=4 非常数负循环模型、额外 d digit 为零、限制恒等式、DCP 重组相消、RS2 的两个 phase wrap 项、Fraction 归一化以及故意遗漏 carry／改变秘密余数符号的负例变体。所有 key 行、秘密和 error 数字都是公开合成模型常数，没有使用真实密钥或真实噪声。

`checks/public_bounds.py`：八个 S100 家族的精确 Fraction 界全部通过；直接校验固定源码中的 Q/root/P 常量，12 组根的模幂阶条件和两两互素关系通过。只是整数模幂，未执行 NTT／FFT，也未重新证明素性或安全级别。模型结果与精确分子分母均随包提供。

本轮相对旧图谱新增的是：**(a)** 可逐行对应的 unsigned digit／floor mod-down 整数定义；**(b)** 含 G₀/G₁ 与 η₀/η₁ 的完整近可加式；**(c)** 零 d digit 导出的坐标限制恒等式；**(d)** 不依赖错误近可加假设的 Relin2 直接界；**(e)** 将有限 sampler 支持带入八个真实 S100 家族并形成数值界，另给出 h128 公开参数下使 `1+h` 取到的符号见证；**(f)** 带输入 lift 和两个 phase-wrap 项的 Mult2 归一化及高低部递推。旧的 P=7 玩具例、两个已知论文符号疑点、OpenMP 名义／private σ 区别本身，不作为新增成果重复计数。

全部模型一致性只支持明确的整数关系；它不是模型投票，不取代上述代数证明和源码映射，不认证历史二进制运行状态。原始失败与未证项均保留。
