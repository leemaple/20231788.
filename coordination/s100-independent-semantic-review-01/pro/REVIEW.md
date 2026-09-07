# S100 独立论文／语义审查

任务：`S100-INDEPENDENT-SEMANTIC-REVIEW-01`  
审查对象：`a448b787399b43b6024d82c170add403969b493c`  
分支：`codex/s100-fresh-error-repair-20260907`  
审查日期：2026-09-07  
性质：独立首轮、只读、中文交付；不是补丁作者自评，也不是运行验收。

## 1. 首轮裁定

**在本包可比的生产代码范围内，未发现共享编码器抽取改变正常生产数学行为、原校验顺序、官方公钥加密路径或整数转换的实质缺陷。新诊断的三项分解在其明确限定的中心提升和数值观察模型下，具有解释价值。**

但这不是“全数值语义已经形式证明”的结论。本轮登记两项中等级别的解释／覆盖问题，以及一项低等级别的实际日志收尾问题：全槽的两种精度不是两种独立算法；附件论文 §2.2 的局部 Tensor 符号与其 §2.1 解密定义不自洽；最终输出刷新之后缺少流状态复核。三者详见第 3 节，不能混称为已找到 S100 精度失败根因。

建议将本切片定位为：**静态语义基本自洽、可继续作为受限诊断候选；编译与运行证据尚未验收；原 S100 的八平方精度验收仍未成立。** 没有依据要求改采样器、噪声、输入、模数、逻辑尺度或 Mult2 算术。

本次没有构建 OpenFHE，没有编译 C++，没有执行 controls、加密、解密或 32k 数值变换，没有派发、重启或查询任务 CI。下述“检查通过”均明确指附件完整性或静态文本／shell 语法检查，不指工程测试通过。

## 2. 输入身份、证据等级与读取范围

### 2.1 独立核验结果

| 项目 | 本次核验结果 |
|---|---|
| 输入 ZIP | `s100-independent-review-a448b78.zip` |
| ZIP 字节数 | `1511925`，匹配 |
| ZIP SHA-256 | `2ca65c697e57eec29e29afa89a45172855aeb4f70a367c1260c7fbff710bf401`，匹配 |
| 成员 | 177 个唯一、普通文件成员；无目录成员、路径穿越或符号链接 |
| 总展开字节数 | `4506404` |
| CRC | 全成员检查通过 |
| 内置清单 | `MANIFEST.json`；176 个载荷，自排除清单本身 |
| 清单 SHA-256 | `89c657ac091c915604ff6c6a97d654ac0f9d650da40a336c43aa19d4ffa24959` |
| 逐文件身份 | 176/176 字节数和 SHA-256 相符；清单与 ZIP 成员集合完全相符 |
| 清单提供的 Git blob | 93/93 按 Git blob 原始对象格式重算 SHA-1 相符 |
| 论文 PDF | 759375 字节；SHA-256 `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac` |

逐 blob 校验说明所交付文件与清单声明一致，不等于已经独立验证远程完整 commit/tree、作者签名或远端实际链接的二进制库。清单记录的密钥扫描结果属于随包证据，本次未重新运行 Gitleaks，不将其描述成我执行的安全扫描。

### 2.2 判断依据与未提供证据

完整阅读了当前 TASK；比对四个 baseline 文件与当前对应文件；完整阅读新增诊断、生产 client-I/O 头文件和实现、整数稀疏解密与全槽／Horner oracle，以及直接相关的固定 S100 setup、密钥生成、尺度收据、CMake 和工作流路径。核对了附带 OpenFHE 的 PKE、DCRT 构造／CRT、采样器、CKKS 解密和相关编码路径，以及 Boost 的相关转换代码。论文主文、结论及参考文献已读取；关键公式、§2、§6 和表 3 另对 PDF 页图核看，不以 PDF 文本流中的表格排列推断单元格。

本包的 `baseline/` 仅提供 `e6c4cc1...` 的四件文件。它足以审查这四件的差分，但不能单凭它证明整棵树相对 e6c4cc1 没有其他变更。也未提供 `1e664bbb...` 的完整父版源码，故“a448b78 相对第一 GREEN 只修改一个表达式”是任务提供的历史说明；我独立验证的是当前表达式、随包编译栈及其语义，不冒充做过该缺失父版的完整 tree diff。

运行证据按下表区分：

| 事件 | 可使用的证据 | 本轮能下的结论 |
|---|---|---|
| 原 S100，run 34039088536 attempt 1 | 当前 TASK／用户给定 Linux、Windows E8 约 `9.14647e-24`、`9.06531e-24` 及 FAIL 状态 | 保留既有 FAIL；不是本次重算或重验该历史日志 |
| RED，run 34109701777 | TASK 说明缺失 `InspectEncoding` 导致编译失败；baseline 确无该 API | 源码支持 fail-first 缺口；完整 RED 原始日志未附，不能独立重新认证整段远端过程 |
| 第一 GREEN，run 34110349729 | `evidence/GREEN1_COMPILER.txt:26–64` | 栈指向固定 512 → 动态 768 转换，GCC 将 array-bounds 警告作为错误，构建退出 2；不是精度 FAIL，更不是 controls 通过 |
| 当前源码，run 34110943783 | 输入冻结时仅说明已派发，未附结果 | `BUILD/CONTROLS/FRESH = NOT_VERIFIED_HERE`；不对交付时实时 CI 状态作推断 |
| S116 | TASK 提供的独立实验性 profile 状态 | 不能替代原 S100，也不据此补写安全结论 |

先完成上述一手源码／论文判断，随后只核对了 IACR 官方题录和 GitHub 官方工作流表达式语义；未查阅该补丁的作者结论、其他评审意见或其他聊天。

## 3. 分级发现

等级约定：P1 为阻断正确解释／使用的重大问题；P2 为需要明确记录的中等覆盖或规范问题；P3 为低等级健壮性问题。**本次没有定位到 P1 级、由本补丁引入的生产算术或诊断整数分解缺陷。** 以下“反例”均是数学／源代码路径反例，不是声称已运行的 fault-injection 测试。

### R-01 · P2：全槽双精度加十个锚点仍有共同索引错误的覆盖盲区

**类别：检验能力的确定限制；未发现当前实现真的发生该索引错误。**

位置：`project/tests/paper_endpoint_transform.cpp:228–285,331–339`；`project/tests/s100_fresh_error_diagnostic_test.cpp:402–433,578–602`；锚点集合见 `project/tests/paper_full_eight_square_oracle.h:46–47`。

512 和 768 两个观察值分别计算、分别生成本精度根表，且与生产特殊 FFT 不同，这比自我比较强得多。然而两者共同实例化同一个 `Transform<Bits>` 算法；Horner 只覆盖 10 个既定槽位。因此“全槽跨精度一致”不能提升为“每个槽位均有算法独立的正确性证书”。

**具体反例。** 设一个共同错误只交换两个非锚点槽位 2 和 3，对 `m,p,d` 的两种精度输出都使用同一置换 π，且不动锚点和精确一范数。则跨精度差、锚点差、有限性和范围检查均不变；对于每个槽位仍有

\[
O(m)_{\pi(s)}+O(d)_{\pi(s)}-O(p)_{\pi(s)}\approx0.
\]

于是记录的错误项会变成

\[
A'_s=O(m)_{\pi(s)}-z_s,\quad
B'_s=O(d)_{\pi(s)},\quad
C'_s=D_s-O(p)_{\pi(s)},
\]

但其和仍然是 \(D_s-z_s\)。这个论证对本任务的冻结 z 也成立，无须更换实际输入。为展示假归因的大小，再给出一个合法整数多项式的理想化数学例子：令 `m=p=2^97·X`、`d=0`、`Δ=2^100`，并令 `z=D=O(m)`。正确的 A、B、C、E0 全为零；交换槽 2、3 后，在槽 2 却有

\[
A'_2=\frac{\xi^{125}-\xi^{25}}8\ne0,\qquad C'_2=-A'_2,\qquad B'_2=E_{0,2}=0.
\]

该多项式的整数范围和一范数满足本观察包络，十个锚点不受置换影响。后一个例子仅解释数学假归因，不是替换冻结输入或声称执行了无噪声实验；前面的置换恒等式已经说明原冻结数据也存在相同覆盖盲区。

**对当前源码的处置。** 当前代码确实使用正向未归一化 DFT、输入 twist 和 `bin=((5^s mod 2N)−1)/2`，第 6 节给出了公式核对；我没有据上述假设置换判定当前代码错误。其严重性在于不能把有限 controls 的覆盖范围写得比实际更大。

**最小行动。** 本轮保留“全槽跨精度核对＋十点独立 Horner＋静态索引推导”的准确表述。若以后要求对共同索引故障进行自动化回归保证，应在另行授权的切片中绑定独立已知稀疏多项式的槽序证据，至少使上述非锚点置换明确失败；不能仅再增加一种调用同一 `Transform` 的精度。本轮不增加或运行这样的测试。

### R-02 · P2：附件论文 §2.2 的 Tensor 中间分量与其解密约定局部不自洽

**类别：论文原文的局部符号问题；不是本次 seam 的实现缺陷，不应转化为改噪声或反转 B 的理由。**

位置：`references/paper/PAPER-2023-1788.pdf` PDF 第 4 页 §2.1、第 5 页 §2.2；对照 `references/official-full/src/pke/lib/schemerns/rns-pke.cpp:199–223`。当前 `project/src/double_ckks.cpp:850–859` 调用官方 `EvalMultNoRelin` 并相加 pair 的交叉项；本轮没有修改它。

论文先定义 `sk=(1,s)`、`Dec(ct)=[ct·sk]Q`，公钥第一项为 `−a·s+e`。按这一约定，两个 ciphertext 的标准张量积中间项应为 `b·a'+a·b'`。但所附 PDF §2.2 的显示公式写成了 `−a·b'−a'·b`，紧接着又声称与 `(1,s,s²)` 的内积等于两次解密之积。

**具体反例。** 取常数多项式 `b=b'=2,a=a'=1,s=1`，并选足够大的奇模数使这些值不绕回。两次解密之积为 `(2+1)²=9`；按该显示公式得到 `(4,−4,1)`，与 `(1,1,1)` 内积为 `1`。两者不等。虽然这个常数例子不是 h128 实验样本，它足以否定该通用代数恒等式按字面成立。

**最小行动。** 将此局部不自洽登记到论文映射说明，以 §2.1 的 `c0+c1·s` 解密恒等式和官方实现为符号锚。不把显示公式中的负号直接迁移进实现，也不借此宣布整篇 Mult² 方法失效。本轮 fresh 分解本来就不执行 Tensor，故此项不阻断 fresh 诊断，但阻断“源码逐字符照搬所有论文公式即可正确”的宣称。

### R-03 · P3：最终 COMPLETE 写入／刷新失败仍可能返回 0

**类别：当前源码的低等级输出健壮性问题；不是已观测到的运行故障。**

位置：`project/tests/s100_fresh_error_diagnostic_test.cpp:384,627–630,653–654`。

fresh 在第 627 行检查 `std::cout.good()`，之后才写 COMPLETE 并执行最终 `flush`；刷新后没有复核。controls 的最终输出也没有在返回前完成同等复核。在默认不因 `badbit` 抛异常的流配置下，若最后一次同步／写入才失败，调用可以返回正常，`main` 仍返回 0。CTest 又以退出状态为主要成败信号，工作流没有另行解析完整诊断记录。

**具体路径反例。** 设到第 627 行流状态正常，而最后 COMPLETE 的 `streambuf::sync()` 返回失败；它设置流失败状态但不抛异常。后续没有检查，控制流到达第 654 行返回 0。因此“进程退出 0”不能独自证明最后状态行已经成功保存。该反例不等于声称 GitHub runner 实际丢失了日志。

**最小行动。** root 对本次结果按“完整记录＋状态行＋退出码”一并保全、核对；缺日志不得解读为已完成的诊断。若另行授权健壮性修订，应在两个模式的最后刷新后检查输出状态，而不是放宽数值门。这个问题不推翻一份已经完整保全的数值日志。

## 4. 生产行为与 seam 最小性

### 4.1 实际差分不是重写编码器

`baseline/src/high_precision_client_io.cpp` 的原编码计算块，与当前 `ComputeEncoding` 的计算块，在仅将 `impl_->primary/check` 替换成形参名后逐字相同。进一步把原 `Encrypt` 中该块替换成 helper 调用、移走只读 `geometry` 引用、把最终 `residues` 改为 `encoded.residues` 后，整个 Encrypt 函数与当前版本逐字一致。附录记录了实际执行的文本验证。

当前顺序为：

\[
\text{profile}\to\text{public key}\to\text{slot count}\to
\text{exact scale}\to\text{finite values}\to
\text{inverse transforms}\to\text{stable rounding}\to
\text{coefficient range／official conversion}\to\text{official Encrypt}.
\]

源码位置：`project/src/high_precision_client_io.cpp:462–495,606–635`；baseline 对应 `:554–611`。无效公钥仍先于非法输入槽数报错，slot 错误仍先于 scale，scale 仍先于非有限输入。`InspectEncoding` 没有公钥参数，所以省去的只是公钥校验，不得声称其错误序列包含加密专属的公钥错误。

本裁定针对数学结果、校验／异常先后和副作用路径，不保证分配次数、峰值内存、ABI、耗时或 `bad_alloc` 时机完全相同。

### 4.2 拥有结果，不引入原始加密入口

`EncodingInspection` 的系数是拥有的 `vector<cpp_int>`；basis 是拥有的字符串向量；scale 拥有分子、分母整数，几何和 projection 是值。见 `project/include/openfhe_2023_1788/high_precision_client_io.h:24–51,70–80,153–157`，及返回点 `project/src/high_precision_client_io.cpp:597–604`。

结果不包含私钥、公钥、ciphertext、context 视图或对输入数据的引用。调用者改结果不会改变随后真实 Encrypt 的输入，也不能把这个结果重新注入某个新 raw-encrypt API。复制和移走结果不依赖原输入／spec 的寿命；类型本身支持独立存活。测试验证了输入修改、结果修改、复制结果和后续调用之间的隔离，但没有单独运行“销毁 client 后使用结果”的测试；不能把静态拥有关系说成执行过该用例。

真实 Encrypt 仍构造官方大整数 `Poly`，设置 `COEFFICIENT` 值，经官方 DCRT 构造逐模映射，再调用官方 public PKE，随后原样写入 receipt 对应 metadata。见 `project/src/high_precision_client_io.cpp:620–633`、`references/official-full/src/core/include/lattice/hal/default/dcrtpoly-impl.h:59–68`、`references/official-full/src/pke/lib/schemerns/rns-pke.cpp:56–70`。没有新添原始多项式加密绕过路径。

### 4.3 整数、舍入和尺度

S100 初始精确尺度是收据的 `2^100/1`，不是从 nominal encoding 50 或某个 double getter 倒推出的近似值。helper 先匹配这一有理尺度，所以只乘 numerator 而不除 denominator 在当前可接受 fresh 域上成立；它不是任意有理尺度 encoder。见 `project/src/repeated_mult2.cpp:279–280`、`project/src/high_precision_client_io.cpp:471–478,503–507`。

整数路径使用 `cpp_int`，按 `2|m_j|<Q` 拒绝 wrap；负系数只在进入官方非负残余类时加 Q；十进制转官方 BigInteger 后逐项读回核对，没有先经 `double` 或 64 位有符号整数。基和根的实际顺序与完整模数乘积被校验。

原有 `StableRound` 的双工作精度为十进制 160／220 位；先检查支持范围，再检查到半整数的距离和两个整数舍入是否相符。数学 tie 约定为 nearest、半整数向下，但原有精度安全合同明确拒绝精确半整数及附近歧义点。**这是一项继承的受限接受域，不是这次 seam 新引入的行为；也不能宣称实现接受了论文 Ecd 的全部数学定义域。** 见 `project/src/high_precision_client_io.cpp:54–57,416–436`。

## 5. controls 的区分能力

以下均为源码审查，未声称执行通过。主要位置：`project/tests/s100_fresh_error_diagnostic_test.cpp:184–385`。

| 要区分的故障 | 独立依据／反例输入 | 实际覆盖及限度 |
|---|---|---|
| 正负号、零、低位丢失 | 常数 0、±1、±3/8，以及 2^-75 的精确整数系数期望（255–268） | 不是用生产 decode 生成期望；能抓住符号、幅度、丢低位等常见错误 |
| 实虚部与 stride | `−i` 对应 `N/2` 系数，N64/S16 的 gap=2；单项式和混合多项式（269–303） | 确认应写偶数槽和后半实系数；小域测试不等于全 N32768 索引穷举 |
| slot 顺序、共轭、尺度 | 输入交换槽 0/1、复共轭、幅度翻倍后要求结果变化（304–314） | 是非退化的输入区分控制，不是实际对实现植入故障后的 mutation test |
| nearest 与符号舍入 | ±1/4、±3/4、±5/4、±7/4 个量化单位（275–284） | 对零截断、向下取整和错误正负处理给出精确不同整数 |
| 半整数拒绝 | ±1/2、±3/2 个量化单位（285–291） | 锁定继承的 ambiguity 合同；不是验证 half-to-even |
| 输入／错误序列 | 0、15、17 槽、spec 不匹配、2^99 或除以 3 的 scale、两分量 NaN/±Inf、超范围；组合错误（316–359） | 核对异常类别及完整原因字符串；不把 profile／key 路径的静态审查说成 keyless controls 的运行覆盖 |
| 拥有关系 | 改输入、spec、结果的系数／basis／scale／几何，再核对副本和后续调用（361–383） | 对共享可变返回缓存、输入别名有区分力；寿命结论还依赖实际字段类型 |
| 原始有符号差 | Q=101 的 −98／+98、保守 headroom 不足、Q 不同、未中心输入（184–210） | 能抓住把差偷偷 center 成小量的关键误判 |
| 重构和极值定位 | 四个不同槽／分量上的 A、B、C、E0 极值及抵消、错误符号差（455–485） | 确认不是把四个不同位置的最大值混成一条等式 |

`SmallPolynomialInputs` 使用正向直接多项式求值构造输入，固定期望是整数多项式本身，并不复用生产 inverse/rounder。其角度、5 次幂槽序和 N64 根与论文 embedding 的定义可独立核对。它仍使用同一 Boost 数值基础设施，而不是严格区间算术。

## 6. 三项观察的数学连线与数值可信度

令 \(\xi=e^{\pi i/N}\)。当前 observer 先令 \(y_j=c_j\xi^j/\Delta\)，再计算正号、未归一化 N 点 DFT，最后取 \(k=((5^s\bmod2N)-1)/2\)。于是

\[
\sum_j y_j e^{2\pi i jk/N}
=\Delta^{-1}\sum_j c_j\xi^{j(2k+1)}
=\Delta^{-1}c(\xi^{5^s}).
\]

这核对了正负号、无额外 1/N、奇根选择和槽序。依据为 `project/tests/paper_endpoint_transform.cpp:162–211,228–285`。Horner 路径直接在十个角度根处求值，不复用 FFT 蝶形／根表，见 `project/tests/paper_full_eight_square_oracle.h:283–314`。

稀疏相位观察以真正 h128 的私钥系数作负循环卷积，逐 tower 得到 `c0+c1*s`，再用精确 CRT 重构并只对 p 作中心提升。它不调用生产 `Decrypt`；但仍信任官方 inverse NTT、基础 NativeInteger 和 DCRT 数据表示，因此“独立”有明确边界。见同 oracle `:198–263`。

`m,p,d` 都分别调用 `Observe`，d 并非以 `O(p)−O(m)` 代替；见新测试 `:565–576`。三份输入的长度、奇 Q、精确尺度及整数一范数均被检查。`||c||₁/Δ≤2^64` 与 `Δ=2^100` 使每个输入整数不超过 `2^164`，从而当前整数→binary512/768 的转换无需丢掉整数位；尺度本身亦精确可表示。

生产解码值经 170 位科学计数文本，并先做 ClientReal 往返相等检查再进入 binary512，不经过 binary64。这个过程保留十进制后端值，**不意味着任意十进制数到二进制浮点是实数意义上完全精确**。在 `2^64` 包络内 binary512 的转换舍入远小于 `2^-300`；实际 dyadic 输入还逐槽要求桥接后与冻结 z 精确相等。见新测试 `:70–88,387–427,512–519`。

两精度和 Horner 的 `2^-300` 门是**观察器一致性门**；生产内部 `2^-120` 是其两工作精度的一致性门；二者都不是 E80 精度门。Bridge 的 legacy decimal100 比较另有门；对于异常大的读出值它可能先报 INVALID，这并不使 `2^64` 包络成为对所有十进制桥接都充分的误差定理。

最重要的解释限制是：

\[
(A+B+C)-E_0=O(m)+O(d)-O(p).
\]

所以重构残差首先核查线性连线，不能单独证明 A/B/C 各自正确，更不能证明 E0 小。当前实现用独立整数来源、分开的 observer 和锚点使它不是简单自我赋值，但仍受 R-01 的共同错误限制。完整可说／不可说范围见 `CLAIM_BOUNDARY.md`。

## 7. 第一 GREEN 的编译修正

第一 GREEN 编译栈指出：`CheckObservation` 原第 423 行实例化了源为固定存储 `cpp_bin_float<512,...,void>`、目标为带 allocator 的 `cpp_bin_float<768,...>` 的转换。Boost 1.83 `references/boost-1.83.0/include/boost/multiprecision/cpp_bin_float.hpp:251–276` 创建源类型的临时整数，再进入 `copy_and_round`；相关实现见 `:617–689`。随包日志 `GREEN1_COMPILER.txt:26–64` 是 GCC 静态 array-bounds 诊断升级为错误的证据，**不是观测到的运行期越界，也不是加密数值结果**。

当前测试 `:423–427` 改成在目标类型内构造 `ldexp(Binary768(1), -kAgreementBits)`。`1` 与二进制指数 −300 在该类型中精确表示，所以仍严格得到同一个 `2^-300`；没有放宽 bound、修改比较符或缩减样本／槽位。该表达式也不再需要原来的固定 512 → 动态 768 临时转换。

仍在 `:415–416` 的扩精度转换，其源是 observer 的 **动态** binary512，不是 `pf::Real` 的固定后端；`:420` 和 `Value512` 的转换是相同 512 位精度的观察值读取。类型差异在此有实质意义，不能仅因两者都写“512”就把所有路径等同于原报错路径。

裁定：**这是语义保真的、针对报错实例化路径的合理修正。是否已消除该 compiler/toolchain 的全部构建错误，必须由当前源码的真实构建结果证明；静态审查不替它宣布 GREEN。** 不应为绕过该问题关闭 `-Werror` 或把门限改成 double。

## 8. CMake 与 Linux RED／GREEN 路径

`project/CMakeLists.txt:305–331` 的选项默认 OFF；目标 `EXCLUDE_FROM_ALL`；仅开启后注册 controls/fresh 两个测试。原 `baseline/CMakeLists.txt` 的 17909 字节是当前文件的完全相同前缀。默认无条件注册项实际上是 62 个，既有工作流排除 full endpoint 和 S116 seam 后运行 60 个；不能把“60 项 checkpoint”误说成整个默认注册表只有 60 项。

Linux 首个新增步骤之前的 26 个步骤与 baseline 的解析结果完全一致，包括构建、public API、57 项 checkpoint、focused contracts 和 60 项回归。之后的路径如下，见 `project/.github/workflows/dcp-rcb.yml:165–205,207–314`：

| 条件 | 新诊断构建 | controls | fresh | legacy 八平方／endpoint | Windows job |
|---|---|---|---|---|---|
| 指定 RED ref，前置步骤成功 | 尝试构建；预期 missing API 失败 | 不运行 | 不运行 | 跳过 | 跳过 |
| RED ref 却意外构建成功 | `always()` guard 明确 `exit 1` | 不运行 | 不运行 | 跳过 | 跳过 |
| repair ref，前置／构建成功 | 正常必需 | 一次，失败即停止后续普通步骤 | 仅前项成功后一次 | 跳过 | 跳过 |
| repair ref 构建或 controls 失败 | 失败保留 | 按失败位置停止 | 不运行 | 跳过 | 跳过 |

没有 `continue-on-error`，没有将 RED 编译错误吞成成功，也没有 fresh 的重复运行／成功重试循环。`--no-tests=error` 和两个首尾锚定的 CTest 正则防止“没有选中测试却返回成功”。每个模式显式 `OMP_NUM_THREADS=2`；CMake 的 controls/fresh TIMEOUT 分别 120 和 1200 秒，且 `RUN_SERIAL`。

工作流的一般 `if` 会受默认 `success()` 约束，所以 controls 失败后，fresh 不因“ref 仍匹配”而继续。这一解释已按 GitHub 官方 expressions 文档的 Status check functions 及 contexts 的 `steps.outcome` 定义核对。RED 的反向 guard 使用 `always()` 加 `outcome=='success'`；若 RED 因其他 compiler error 变红，工作流自身并不会认证那就是预期 missing API 原因，仍需原始日志。

这里的“一次 public encryption”是 **fresh 模式的 payload 加密次数**，不是声称整个 CI、既有回归或 setup 总共只进行一次随机操作。S100 setup 还会创建公私钥和多组评估钥；这些不属于反复尝试 fresh 精度直到成功的行为。

两个 S100 分支未加入 `on.push.branches`，但 `workflow_dispatch` 已存在；因此这是可按 ref 显式派发的路径，不是本文件保证的自动 push 触发。`RUN_SERIAL` 本身也不建立任意 CTest 调用下的先后依赖；本任务的先 controls 后 fresh 来自明确的两条工作流步骤。

核对使用的官方说明定位（不是任务 CI 访问）：

```text
GitHub Docs, Evaluate expressions in workflows and actions, Status check functions
https://docs.github.com/en/actions/reference/workflows-and-actions/expressions
GitHub Docs, Contexts reference, steps context / outcome
https://docs.github.com/en/actions/reference/workflows-and-actions/contexts
GitHub Docs, Workflow syntax for GitHub Actions
https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax
查阅日期：2026-09-07
```

## 9. 保密性和未决事项

`InspectEncoding` 仅返回调用者明文对应的编码整数，不返回 key。新测试对私钥／公钥的快照和稀疏读出留在进程内；未发现直接打印私钥系数、完整 ciphertext、完整相位多项式或任意上游异常内容的路径。上游异常正文被压缩为固定原因，见新测试 `:662–668`。

但 A/B/C 以及相位相关锚点是**由私钥解密派生的调试信息**。不打印私钥，不等于日志满足零知识、IND-CPA^D 或“对长期生产密钥公开也安全”。可接受的使用边界是专用一次性实验密钥、已公开固定输入、受控日志保全；不能直接把本测试改造成对外解密／噪声查询服务。本轮既未证明泄漏足以恢复密钥，也未作“不会泄漏任何秘密信息”的反向保证。

| 未决事项 | 分类 | 阻断什么、不阻断什么 |
|---|---|---|
| 当前构建、默认回归、controls、fresh 实际结果 | 运行待证 | 阻断 runtime 验收；不阻断本次静态审查交付 |
| R-01 全槽共同算法／槽序故障 | 覆盖／解释限制 | 阻断“全槽独立形式认证”表述；当前索引静态推导未发现错 |
| R-03 最终日志刷新 | 低等级运行健壮性 | 只有退出码／不完整日志时阻断解释；完整保全日志可继续分析 |
| 原始 sampler lift 与 Q 的不可观测倍数 | 观测不可识别性 | 阻断 sampler 无 wrap／噪声分布证明；不阻断所选中心相位的整数差分解 |
| 论文实验输入、χenc、χerr 的具体参数未给全 | 论文条件未闭合 | 阻断把当前 B 等同于论文噪声模型并宣告精确复现；不自动证明实现错误 |
| 安全估计、长期密钥调试泄漏、S116 安全性 | 本切片之外 | 均不能由 COMPLETE 推出，也不以改变参数的方式在本轮“补足” |
| 其他历史 commit 的完整树／旧 E8 逐槽记录未附 | 证据边界 | 不假设本机或旧聊天有它们，不伪造跨样本因果对应 |

## 10. 本轮实际执行的有界检查

除只读文本、差分、PDF 读取／渲染外，实际执行了下方脚本；仅进行 ZIP/哈希、字符串比较、YAML 解析、`bash -n` 以及小整数／有理数恒等式检查。`bash -n` 不执行脚本中的命令；YAML／shell 检查不是 GitHub Actions 引擎执行或 CMake configure。

命令、脚本全文及原样结果附后。脚本放置在独立审查工作目录，未写入输入 `project/`，也不是提交给工程的实现代码。审查包只包含三份文档及其自排除清单。


### 10.1 精确执行命令与结果

在本轮容器中执行以下命令，整体退出码为 0：

```bash
set -o pipefail
python /mnt/data/s100_review_work/static_checks.py /mnt/data/s100-independent-review-a448b78.zip | tee /mnt/data/s100_review_work/static_checks.txt
```

这些绝对路径仅是本轮独立容器中的实际工作路径，不是要求访问 root 的机器或用户未提供的本地环境。将下面脚本保存为任意位置的 `static_checks.py`，以原 ZIP 路径作为唯一参数，即可复核同类检查。运行需要 Python 及 PyYAML；脚本不链接 OpenFHE。

```text
INPUT bytes=1511925 members=177 payloads=176 uncompressed=4506404 git_blobs=93 CRC=OK hashes=OK
INPUT_MANIFEST_SHA256=89c657ac091c915604ff6c6a97d654ac0f9d650da40a336c43aa19d4ffa24959
ENCODER normalized_shared_body=EQUAL production_Encrypt=EQUAL_AFTER_EXTRACTION baseline_API=ABSENT
CMAKE baseline_prefix_bytes=17909 EQUAL default_registry=62 selected_default_checkpoint=60
WORKFLOW preceding_steps=26 EQUAL linux_run_snippets_bash_n=36 ALL_EXIT_0 no_continue_on_error=TRUE
FRESH textual_public_encrypt_call_sites=1 exact_destination_bound=2^-300
EXACT_SMALL_ALGEBRA rounding_bound=2^-86 hidden_Q_multiple_same_residue=TRUE paper_tensor=1 product=9
ENV python=3.13.5 PyYAML=6.0.3
NO_CXX_BUILD NO_OPENFHE_BUILD NO_CRYPTO NO_NUMERICAL_TRANSFORM NO_CI_ACTION
```

这里 `selected_default_checkpoint=60` 来自 CMake 中既有默认注册项的文本枚举及两个排除项，**不是 CTest 的实际通过数**。`bash_n=36` 仅表示 36 个 Linux `run` 文本片段通过 shell 语法解析。编译器版本兼容性、CMake configure、链接成功、官方库真实 pin、controls 的行为、fresh 数值和八平方精度均未由这些结果证明。

### 10.2 已执行脚本全文

```python
from pathlib import Path, PurePosixPath
import hashlib, json, re, stat, subprocess, sys, zipfile
from fractions import Fraction
import yaml

archive = Path(sys.argv[1])
buf = archive.read_bytes()
sha = lambda b: hashlib.sha256(b).hexdigest()
assert len(buf) == 1511925
assert sha(buf) == '2ca65c697e57eec29e29afa89a45172855aeb4f70a367c1260c7fbff710bf401'
with zipfile.ZipFile(archive) as z:
    infos = z.infolist()
    assert len(infos) == 177 and len({i.filename for i in infos}) == 177
    for i in infos:
        p = PurePosixPath(i.filename)
        mode = i.external_attr >> 16
        assert not i.is_dir() and not p.is_absolute() and '..' not in p.parts
        assert '\\' not in i.filename and stat.S_IFMT(mode) in (0, stat.S_IFREG)
    assert z.testzip() is None
    data = {i.filename: z.read(i) for i in infos}
m = json.loads(data['MANIFEST.json'])
assert m['manifest_self_excluded'] is True
assert set(data) == {'MANIFEST.json'} | {e['path'] for e in m['files']}
blobs = 0
for e in m['files']:
    b = data[e['path']]
    assert len(b) == e['bytes'] and sha(b) == e['sha256'], e['path']
    if 'git_blob' in e.get('origin', {}):
        blobs += 1
        actual = hashlib.sha1(b'blob ' + str(len(b)).encode() + b'\0' + b).hexdigest()
        assert actual == e['origin']['git_blob'], e['path']
print(f'INPUT bytes={len(buf)} members={len(data)} payloads={len(m["files"])} '
      f'uncompressed={sum(map(len, data.values()))} git_blobs={blobs} CRC=OK hashes=OK')
print('INPUT_MANIFEST_SHA256=' + sha(data['MANIFEST.json']))
text = lambda p: data[p].decode('utf-8')
old = text('baseline/src/high_precision_client_io.cpp')
new = text('project/src/high_precision_client_io.cpp')
start = '    if (values.size() != geometry.slots || spec.slots != geometry.slots)'
end = '    // Official large-Poly/DCRT constructor'
old_method = old.split('BoundCiphertext HighPrecisionClientIO::Encrypt', 1)[1].split(
    'BoundCiphertext HighPrecisionClientIO::BindFirstMult2Rcb', 1)[0]
new_method = new.split('BoundCiphertext HighPrecisionClientIO::Encrypt', 1)[1].split(
    'BoundCiphertext HighPrecisionClientIO::BindFirstMult2Rcb', 1)[0]
old_block = old_method[old_method.index(start):old_method.index(end)]
helper = new.split('EncodingWork ComputeEncoding', 1)[1].split('}  // namespace', 1)[0]
new_block = helper[helper.index(start):helper.index('    return {std::move(coefficients)')]
normalized = old_block.replace('impl_->primary', 'primaryTable').replace('impl_->check', 'checkTable')
assert normalized == new_block
expected = old_method.replace('    const auto& geometry = binding.geometry;\n', '', 1)
expected = expected.replace(old_block,
    '    auto encoded = ComputeEncoding(binding, values, spec, impl_->primary, impl_->check);\n', 1)
expected = expected.replace('std::move(residues)', 'std::move(encoded.residues)', 1)
assert expected == new_method
assert 'InspectEncoding' not in text('baseline/include/openfhe_2023_1788/high_precision_client_io.h')
old_cmake = data['baseline/CMakeLists.txt']
assert data['project/CMakeLists.txt'].startswith(old_cmake)
cmake = text('project/CMakeLists.txt')
default_part = cmake.split('# Opt-in full eight-square numerical experiment.', 1)[0]
names = re.findall(r'add_test\(NAME\s+(\S+)', default_part)
assert len(names) == 62
selected = set(names) - {'paper_full_eight_square_contract', 'experimental_precision116_profile_seam'}
assert len(selected) == 60
print('ENCODER normalized_shared_body=EQUAL production_Encrypt=EQUAL_AFTER_EXTRACTION baseline_API=ABSENT')
print(f'CMAKE baseline_prefix_bytes={len(old_cmake)} EQUAL default_registry=62 selected_default_checkpoint=60')
w = yaml.load(text('project/.github/workflows/dcp-rcb.yml'), Loader=yaml.BaseLoader)
old_w = yaml.load(text('baseline/.github/workflows/dcp-rcb.yml'), Loader=yaml.BaseLoader)
steps = w['jobs']['linux-gcc']['steps']
idx = next(i for i,s in enumerate(steps) if s.get('name') == 'Configure S100 fresh-error diagnostic build')
assert steps[:idx] == old_w['jobs']['linux-gcc']['steps'][:idx]
assert 'continue-on-error' not in w['jobs']['linux-gcc']
assert all('continue-on-error' not in s for s in steps)
checked = 0
for i,s in enumerate(steps):
    if 'run' not in s:
        continue
    r = subprocess.run(['bash','-n'], input=s['run'], text=True, capture_output=True)
    assert r.returncode == 0, (i, s.get('name'), r.stderr)
    checked += 1
new_steps = steps[idx:idx+5]
assert len(new_steps) == 5
assert new_steps[3]['run'].count("'^s100_encoding_inspection_contract$'") == 1
assert new_steps[4]['run'].count("'^s100_fresh_error_diagnostic$'") == 1
print(f'WORKFLOW preceding_steps={idx} EQUAL linux_run_snippets_bash_n={checked} ALL_EXIT_0 no_continue_on_error=TRUE')
test = text('project/tests/s100_fresh_error_diagnostic_test.cpp')
fresh = test.split('void Fresh() {',1)[1].split('} // namespace',1)[0]
assert fresh.count('client.Encrypt(') == 1
assert 'boost::multiprecision::ldexp(observer::Binary768(1), -kAgreementBits)' in test
assert 'constexpr int kAgreementBits = 300;' in test
assert 'OBSERVER_512_768_DISAGREEMENT' in test
print('FRESH textual_public_encrypt_call_sites=1 exact_destination_bound=2^-300')
assert Fraction(32768, 2 * 2**100) == Fraction(1,2**86)
q, coeff, phase = 101, 3, 5
assert phase-coeff == 2 and (coeff+103) % q == phase
assert q-2*(abs(coeff)+abs(phase-coeff)) == 91
assert 2*2 - (1*2+1*2) + 1 == 1 and (2+1)*(2+1) == 9
print('EXACT_SMALL_ALGEBRA rounding_bound=2^-86 hidden_Q_multiple_same_residue=TRUE paper_tensor=1 product=9')
print('ENV python=' + sys.version.split()[0] + ' PyYAML=' + yaml.__version__)
print('NO_CXX_BUILD NO_OPENFHE_BUILD NO_CRYPTO NO_NUMERICAL_TRANSFORM NO_CI_ACTION')
```
