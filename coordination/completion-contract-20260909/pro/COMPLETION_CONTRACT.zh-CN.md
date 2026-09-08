# COMPLETION-CONTRACT-01：完成边界与独立语义审查

日期：2026-09-09。审查对象是供件快照，不是实时仓库状态。当前源码 `a7f54de2701a1b9bc02660f66febeff707b56651`；任务 `fcd745ae30a3f54e37b8ac060c854c226e38feb1`；官方 OpenFHE 固定提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。本文中的路径均相对输入 ZIP 根目录；`project/` 是供件源码映射，不改变实际仓库路径。

## 1. 裁决

**现有证据足以对当前 t=2 实现作出“在列明前提和范围内，算法对应关系与工程正确性有积极支持”的判断；不能给出“所有验收项已通过”或“原 S100 八平方已经修好”的结论。** 原冻结 S100 近单位圆输入的 E80 门禁在 Linux、Windows 各一条链上仍然失败。该项是已知未达成的数值要求，不是尚未运行，也不能由 S116 或 annulus 的 PASS 抵销。

本轮没有定位到一个仍未处理、且能以现有规范构造生产 RED 的具体乘法代码缺陷。这个结论来自逐阶段代数对应、实际 C++ 路径、固定官方实现、已有独立正反例和多平台执行证据的结合，**不是从“没有发现错误”直接推导“绝对正确”**。最新当前公开 p 的严格 Ecd 证书关闭了一个真实的对应性缺口；现有材料没有再暴露出必须用新编码、采样或密文运行才能解决的同等级缺口。

因此，本轮的处置是：**接受有边界的算法／工程判断，保留原精度验收不通过；修正交付文档中混在一起的“完成”标签；不再派发无新判别依据的实验或审核。完整论文复现目标不降级，也不宣布已经全项完成。** 未取得作者实验设置，阻断的是“原作者表 3 同源统计／性能重现”的主张；它不是判定公开算法实现的万能阻断条件。反之，去掉这个不当总开关，也不会使原 E80 从 FAIL 变成 PASS。

来源：`TASK.md:5–19,31–41,59–70`；`context/current/coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md:3–35`。逐项权威及状态见 `REQUIREMENT_AUTHORITY.tsv`。

### 不合并的五个结论

| 判定对象 | 本轮状态 | 准确含义 |
| --- | --- | --- |
| 当前算法／实现语义 | **CONDITIONAL — 有条件支持** | 对声明的合法状态、诚实密钥、固定依赖及数值解释，关键构造与 t=2 定义一致，并有非平凡测试支持；不是全程序形式化验证。 |
| 原冻结 S100 E80 验收 | **CONTRADICTED — 两平台 FAIL 保留** | 原测试确实完成且未过精度门槛；既不改 truth，也不改范数、阈值、输入或噪声。 |
| 当前公开 p 的 Ecd 与既定非绕回前提 | **PROVEN／有明确模型前提** | 当前 p 的舍入对应已证；结合已采用契约，可使用该 p 的相容提升／非绕回结论。不是旧 p 的身份认证，也不是总误差 E80。 |
| 表 3 原作者统计／性能重现 | **MISSING — 未建立** | 不知道同源输入和具体实验配置，未重做千次均值／计时。千次不是用户要求，但不能声称完成该统计主张。 |
| 安全性／部署许可 | **MISSING — 未认证** | 不能从 h=128、N、QP680／712 或数值 PASS 推出安全级别、随机数质量和部署安全。 |

状态是按命题而非按文件给出的。已验证某个日志的哈希，不等于该日志所述一切数学命题都被形式化证明；一个被精确证书证明的当前 p，也不是对任意未来 p 的证书。

## 2. 什么有权成为完成门槛

用户的明确要求是“不需要1000次实验，能判断实现的正确就行”，同时保留尽量完成论文复现的目标。本轮 TASK 又明确要求保留原 truth／阈值、真实公钥输入与 evaluator-only 高精度乘法、有界论文规模链、负例／边界和 Linux／Windows 证据。不能将这些压缩成“小输入能过就算完成”。

原项目选定了具体近单位圆 dyadic 向量、实虚部分量范数和 `2^-80` 门禁；论文没有给出这个向量，也没有声称每个输入／密钥都满足该门禁。**这说明门禁的来源是项目，不说明本审查人有权删除它。** 当前 TASK 明确保留了它，因此原固定数值项仍为 FAIL。要改变这个状态，不能靠文档重新命名、改成预期失败或另取更容易的样本；本包没有这样做。

论文 §6.3 报告的是八次平方后、1000 次执行的平均 infinity-norm error，其中 t=2 约为 `2^-81.8`。100-bit 初始精度／尺度，不是“最终每次都保留 100 位”或“最终必有 80 位相对有效精度”的承诺。§6.2 的 18 级链和分段刷新不是 §6.3 的八平方规则，不能据此前者给本链增加刷新步骤。论文中的速度、内存、特定硬件和作者 HEaaN 构建来源，在本次用户目标下不是自动新增的工程正确性门禁。

证据：`references/paper/PAPER-2023-1788.pdf` pp.12–14，尤其 p.13 §6.3／Table 3；对应文本 `:1521–1596`。`coordination/paper-scale-integration-01/INPUT_DOMAIN_AUDIT_01.md:11–32,61–69`；`PRODUCTION_CONTRACT_01.md:96–149`。记录范围本身的权威限度见 `CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md:3–35`，并由当前 `TASK.md:17–19` 明确保留。

## 3. 不读 C++ 也可核对的算法语义

### 3.1 双密文不是“用两个普通乘法拼精度”

记一对密文的高、低解密相位为 H、L，除数为 d，合成相位为

\[
M=dH+L.
\]

DCP 按密文坐标作兼容的商余分解；RCB 把二者合回。这里首先是活动模数环内的等式。要把它解释成小整数／小实数，必须另有相容整数提升和非绕回条件，不能因为中心代表有正 headroom 就当作已经证明。

Tensor2 对两对输入生成高项 `H_a H_b` 和交叉项 `H_a L_b+L_a H_b`，有意省略低低项。于是其重新合成的目标为

\[
dH_aH_b+H_aL_b+L_aH_b
=\frac{M_aM_b-L_aL_b}{d}.
\]

**省略低低项是论文算法的近似项，不是本轮新发现的漏乘 bug。** 反过来，在没有相应界时也不能把它当作零误差。

### 3.2 Relin2 的顺序有实质意义

当前实现先把高张量乘 d 并以零 Div 塔提升到对应完整基，进行普通重线性化，再 DCP；低张量单独重线性化并加到低分量。其对应为

\[
\operatorname{Relin2}(H,L)
=\operatorname{DCP}_d(\operatorname{Rel}(dH))+(0,\operatorname{Rel}(L)).
\]

它不是 `d·Rel(H)+Rel(L)` 的线性替代。固定 HYBRID 后端的 key-switch 含数字分解、进位及两坐标 ApproxModDown；这些步骤不是任意线性映射。已采用的源码特定契约保留

\[
\nu_Q(c)=\frac{\sum_jD_j(c)e_j-r_0-sr_1}{P},
\qquad \nu_2=\nu_Q(dH_2)+\nu_Q(L_2),
\]

以及相应的模同余／整数提升条件。不能把零 carry 当作必要条件，不能省掉第一坐标余数，也不能用 `dν(H_2)` 取代实际 `ν(dH_2)`。

### 3.3 RS2 缩减的是正确的组合，尺度确实消耗 dμ

设本轮被消耗的乘法素数为 μ。当前实现对高项和 **RCB 后的合成项** 做普通 rescale，再取低项差：

\[
H'=\operatorname{RS}_{\mu}(H),\qquad
L'=\operatorname{RS}_{\mu}(dH+L)-dH'.
\]

因此，合法相容提升下的一般乘法递推可写为

\[
M'=\frac{M_aM_b-L_aL_b}{d\mu}+\frac{\nu_2}{\mu}+\varepsilon_C,
\qquad S'=\frac{S_aS_b}{d\mu},
\]

其中 ε_C 是相应合成项的 rescale 相位舍入项，不是额外人为修正项。以 y=M/S 归一化：

\[
y'=y_a y_b-\frac{L_aL_b}{S_aS_b}
+\frac{d\nu_2}{S_aS_b}+\frac{d\mu\varepsilon_C}{S_aS_b}.
\]

这给出了应当接受的近似算法契约：目标乘积、被省略的低低项、真实后端重线性化误差、rescale 误差分别存在。它不是“只要实现正确，总误差就小于任意指定阈值”的定理。

当前代码保存确切整数分子／分母尺度；八平方按 `S_(k+1)=S_k²/(d μ_k)` 消耗实际素数，而不是使用名义 40／60 bit 幂。终端客户端按该尺度解码，不能用 OpenFHE 的普通 double 尺度字段代替。论文 p.8 Thm 4.8 显示式遗漏 d、p.5 普通 Tensor 的交叉项负号均与定义／相位约定不一致；已有独立反例确认此处印刷歧义，不能据其“修”坏当前正确的尺度和正交叉项。

### 3.4 八步重入没有秘密辅助

当前计划包含八个活动参数族。每轮只消耗该轮乘法素数，下一族保留 Div；重入使用新 wrapper、相同密文系数和正确身份，不解密、不重新加密、不 key-switch 刷新，也不是恢复已丢弃塔。评估函数没有私钥参数或解密回调。客户端 setup 会生成并投影诚实根密钥到各族，评估阶段只使用评估密钥；私钥只在最终／事后检查器中使用。

收据证明的是计划、尺度、状态和归属；不是对任意攻击者构造密文的可验证计算证明。终端 immutable clone 防止结果观察中的别名修改；不能把这个性质夸大为密码学认证。

**源码锚点：** `project/src/double_ckks.cpp:398–438,826–892,1011–1054,1137–1188,1213–1283`；`project/src/repeated_mult2.cpp:257–311,351–397,481–569`；`project/src/high_precision_client_io.cpp:607–636,685–756`；`project/tests/paper_full_eight_square_contract_test.cpp:188–202,302–325`。官方对应：`official/src/core/include/lattice/hal/default/dcrtpoly-impl.h:693–711`；`official/src/pke/lib/keyswitch/keyswitch-hybrid.cpp:98–128,381–430`；`official/src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp:172–190`。误差与提升前提见 `coordination/relin2-bound-20260908/ADOPTED_CONTRACT.md:19–21,29–74`、`coordination/initial-lift-nonwrap-20260909/ADOPTED_CONTRACT.md:9–18`。

## 4. 当前公开 p 的证书关闭了什么

证书的对象固定为现存公开 p，payload SHA-256 为 `7cac9765f0c5e567840ebc347a59e50d039107c7b6a92c037152b69bd8edd94f`；系数流 SHA-256 为 `66c36716c1445b4b2c6c47e06996c2b080792b0b9251fe075033f542acd540be`。所有 32768 个理想逆嵌入系数严格落入实际整数 p 的 half-down 单元 `(p_j−1/2,p_j+1/2]`；没有 REFUTED／INCONCLUSIVE。最小正间隙在 26696，分子为 `19406664479890172665284974927688131526799829314786449218863104`，分母为 `2^224`。这个数是**舍入单元边界间隙**，不是噪声实测值。

独立逆变换的共轭补全、负指数、untwist 与 S/N 归一化对应公开的 powers-of-five 嵌入。原候选曾有输出路径和 status／exit 关联的有限封套缺陷；当前 root-owned harness 的真实路径排除与严格状态／退出码核对解决了它们。原始候选保持不变不构成“尚未修好的运行缺陷”：实际运行路径明确绕过旧 shell，不能拿旧候选标签覆盖现行封套证据。

由全系数最近舍入得到 `δ≤N/(2S)=2^-86`；由独立幅度证书得到 `||σ(p)||∞/S<0.991242<127/128`。两者联合后，当前 p 的**纯编码、理想八平方**贡献满足

\[
256(0.991242+2^{-86})^{255}2^{-86}
<0.424504491253\cdot2^{-80}<\tfrac12\,2^{-80}.
\]

本轮只复核这个固定标量不等式和证书状态／哈希关联，没有重算逆变换或重新判定全部系数单元。以上事实配合已采用的有限采样支持和源码特定 Relin2 契约，允许使用当前 p 的初始相容提升及八步非绕回证书。它的量词是“此 p、此参数、满足已列密钥／采样／整数运算前提的实例”，不是“全部输入／所有未来环境”。非绕回只控制代表的解释，不给出 E80 总误差保证。

仍不能推出：当前 p 等于历史 Linux／Windows p；两工作精度对任意输入都必产生理想舍入；历史整条 FHE 链被本证书重新执行；PKE 噪声消失；所有密钥均满足 E80。已核源码片段相同不是历史二进制／舍入结果相同。当前 stable-round 也会明确拒绝接近不确定舍入边界的输入；不能声称它对任意有限复数向量都是全定义的精确编码器。

来源：`coordination/public-s100-ecd-cell-20260909/{ADOPTED_RESULT.zh-CN.md:9–42,MATH_ADOPTION_REVIEW.md:82–169,ROOT_INTAKE_REVIEW.md:3–15,GREEN_INTAKE.json}`；`green-evidence/execution/{certificate/RESULT.json,OUTCOME_AGREEMENT.json,HARNESS_RESULT.json}`；`harness_contract.py:32–53`；`coordination/public-s100-encoder-cap-20260909/{ADOPTED_RESULT.zh-CN.md:12–40,SOURCE_BRIDGE_REVIEW.md:19–44}`。

## 5. 原 FAIL 说明了什么，不说明什么

令 T=`2^-80`，原目标 z 的实际 fresh 归一化相位为 w₀，最终读出为 w₈。定义同一条链中的

\[
E_8=w_8-z^{256},\quad I_8=w_0^{256}-z^{256},\quad A_8=w_8-w_0^{256}.
\]

则 E₈=I₈+A₈。I₈ 是起点误差的非线性传播，A₈ 是后续相对于这一理想传播的残差；不能用不同时刻／样本的最大值相减冒充逐槽归因。观察器 C 也不能作为实际 fresh 相位的一部分传播。

| 原实验 | 原门禁最大分量 E₈ | E₈/T 约值 | 状态 |
| --- | --- | --- | --- |
| S100 Linux，run 34039088536 | `9.1464736336494205×10^-24` | 11.0574 | FAIL，1 条完整链 |
| S100 Windows，同 run | `9.0653051740867722×10^-24` | 10.9593 | FAIL，1 条完整链 |

在原 E₈ 最坏分量处，Linux I₈ 约为 `−9.1483666072×10^-24`，A₈ 约为 `+1.8929735731×10^-27`；Windows I₈ 约为 `−9.0656272016×10^-24`，A₈ 约为 `+3.2202752979×10^-28`。二者方向相反，但抵消量不足。现存有界观察模型下，I₈ 的下界本身高于 T；本轮对保留的有理边界复核，`I_max,lower−A_max,upper>T` 在两平台也成立。这是合法的反三角下界，不是宣称两最大值同槽。

因此，**固定这两个已观察起点，把后续 A₈ 设为零仍然过不了原门槛**。这排除了“只消掉乘法后续误差就一定修复旧失败”的主张；不排除一切可能实现改善，也不证明错误起点本身是编码 bug。新 p 证书不允许把旧 I₈ 全部重新归因为 PKE；旧 p 身份尚未转移。另一次 fresh run 34110943783 的独立 A/B/C 分解与理想传播仅按其自身样本成立。

公钥噪声遵循 `e_pk·v+e₀+s e₁`，当前官方路径里的 v 是稠密三元，而 h=128 属于秘密 s。这个设置与论文抽象“χ_enc 为参数”的定义并不直接冲突。不能因其放大误差便认定错用了 h，也不能静默改成稀疏 v、私钥加密、减小 σ 或重抽噪声直到通过。官方 keygen 中生成公钥时使用私钥 EncryptZeroCore，和 payload 经 PublicKey 重载加密，是两个不同角色。

证据：`context/current/coordination/fs-endpoint-live-run-01/{LINUX_AUDIT.json,WINDOWS_AUDIT.json}`；`coordination/annulus-independent-pro-review-20260908/ROOT_SCALAR_REVIEW.md:34–46,62–66`；`context/current/coordination/s100-fresh-error-repair-01/FRESH_IDEAL_PROPAGATION.md:17–104`；`project/src/paper_h128_client_keypair.cpp:193–217`；`official/src/pke/lib/schemerns/rns-pke.cpp:111–196`。本包 `evidence/BOUNDED_CHECKS.json` 给出本轮精确比较结果。

## 6. 有效的积极验证证据，而不只是“没有 bug”

| 证据 | 能支持的性质 | 不能取代的性质 |
| --- | --- | --- |
| 固定算子和边界／负例 | DCP 商余、Tensor2 正交叉项、Relin2 carry 两坐标、RS2 对组合项缩减、状态拒绝和不变性有具体可证伪检查 | 全输入全随机数形式化正确性 |
| 低 N 高精度 I/O 与两操作链 | 高精度桥、非 double 可分辨见证、真实客户端／评估边界和独立 oracle | N=32768 的完整链证据 |
| 原论文规模完整两平台链 | 确实执行八次 evaluator-only 平方、全部槽位端点检查、十个事后 Horner 锚点；也真实暴露数值 FAIL | 原 E80 PASS；不能只挑流程完成字段 |
| S116 两平台完整链 | 改参后同类算法链的非平凡数值成功、精确尺度和公开终端 I/O 的积极集成证据 | 原 S100 修复、QP712 安全保证 |
| S100 annulus125 单 Linux 样本 | 原参数另行冻结输入的一次复数模 E80 PASS，独立标量重放一致 | 全 annulus／所有密钥通过；Windows PASS；原向量修复 |
| 当前 p 严格数学证书及提升契约 | 当前实际编码与理想 Ecd、当前 p 的小相位整数解释 | 历史端点重解密或总误差认证 |

这些正反证据共同支持第 1 节的有条件判断。不是用一次 annulus 成功来定义一个未经证明的“支持域”，也不是用 60 个名称当作 60 个独立密码样本。原 full test 的最终数值失败必须触发失败：源码会在保存端点证据后检查 `numericFailures==0`；它不会以“日志完整”覆盖误差 FAIL。

源码定位：`project/tests/relin2_test.cpp:4185–4252`（非零 carry／低项见证）；`rs2_test.cpp:685–735`（独立中心 CRT／rescale 目标）；`repeated_mult2_semantic_two_square_test.cpp:190–261`（精确尺度、重入系数不变、异族／终端／被改标记负例）；`paper_full_eight_square_oracle.h:78–127,182–195,198–262,283–332`（独立闭式尺度、固定 truth、稀疏卷积／CRT、Horner）；`paper_full_eight_square_contract_test.cpp:329–361,425–433`。已执行与仅有源码明确分开：本轮没有运行这些测试。

## 7. 样本、源码、平台不能串接成一个虚构的 PASS

S116 run `34055816234`，源码 `2b8b349edf5575556347082c1b725f6696c743b6`，Linux 最大分量误差约 `2.5905123324714234×10^-26`，Windows 约 `3.4805603371613677×10^-26`；每平台一条链。它使用 Base58、Div56、初始 S116、QP712，不能改写原 Base50／Div40／S100／QP680 的状态。

annulus125 run `34184869227`，源码 `03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b`，仅 Linux 一条链；最大**复数模**误差约 `1.46231410388×10^-25`。82.50 是绝对精度，约 73.30 才是该样本按同槽分母计算的相对精度。半径缩小时 x²⁵⁶ 的绝对导数减小，输出也缩小；相对局部条件数仍为 256。新旧样本的密钥／噪声没有配对，不能把观察到的全部变化归结为输入半径的单因素因果实验。

当前源码不是上述任何一次历史运行的完整提交。按供件哈希，本轮确认三份生产 TU 与 a4b815a 图谱输入身份一致；高精度 I/O TU 相对该基线只新增一个头文件引用和独立公开 inspection 函数，移除此增量后原字节相同。相同的是这些已定位代码，不是编译二进制、全部测试／工作流，也不是历史 p。`provenance/*MANIFEST` 只被用于历史身份比较，没有替换当前根清单。

已有最新 Linux 60/60 与 keyless control 1/1 的状态由 TASK、当前交付记录和独立复核支持；本包未提供该 `s100-output-finalization-01/GREEN_EVIDENCE.txt` 的独立实体副本，本轮不声称直接重放了它的原日志。两平台原／S116 结果另有各自审计与运行复核；没有“当前 a7f54de 最新双平台全套又跑过”的证据。当前 p 证书有完整 green-evidence，本轮核验所有文件字节，检查状态封套和数学／采纳链；没有把 certificate job 说成新 FHE 作业。

来源：`coordination/precision116-eight-square-return-01/{ACCEPTANCE.md:7–35,RUNTIME_REVIEW.md:12–63}`；`coordination/s100-annulus125-20260908/{RESULT.zh-CN.md:9–45,INDEPENDENT_RESULT_REVIEW.md:71–109}`；`coordination/annulus-independent-pro-review-20260908/pro/REVIEW.md:164–174`；`CHECK_AND_HANDOFF.zh-CN.md:49–52`；`provenance/ATLAS_INPUT_MANIFEST.json`；当前与 bound-source 两份 `high_precision_client_io.cpp` 的本轮文本比较。缺少这一原日志副本不否定已有可信审计记录，也不产生“为了补齐目录再跑一次”的要求。

## 8. 接受结论的明确前提与开放限制

数学解释依赖已列合法基、d 与实际 μ、正确参数族、诚实根秘密及配套评估密钥、固定采样实现成功返回时的有限支持、相容提升条件。运行解释依赖固定官方源码与构建绑定、普通编译器／Boost 数值语义、观察器的既定误差模型。接口不支持任意参数突变、混根共享缓存时期、恶意调包评估密钥或无限精度输入；有限状态拒绝检查不能覆盖全部攻击情形。

历史 S100／S116 OpenMP 配置已经通过原配置／编译记录闭合到 `WITH_OPENMP=ON`，不是仍待通用查询的问题。`private(dgg)` 对 evaluation-key 局部分布的源码条件，与公钥／payload 的噪声路径必须分开；不能说“所有 σ 都是 1”，也不能把它宣布为 old fresh error 的原因。annulus 的依赖 cache hit 没有历史库逐字节唯一来源证明，应保持运行来源的条件性。上述边界被明确承认，不等于每一个都自动升级为新阻断任务。

Current-p 最近舍入和 cap 可以关闭旧文档中的“当前编码关系未证”前提，但不能逆向改变旧密文样本的确定事实。观察器摘要与阶段程序源码共同支持流程和端点；十个 Horner 锚点不证明所有中间槽位，full-slot error TSV 的重放也不是重新解密不存在的旧密文。若要求全程序、任意输入、所有密钥或独立验证全部历史二进制，那是更强命题，需要额外证据；本轮未把它们伪装成已经取得。

来源：`docs/parameter-atlas/REVIEW_NOTES.zh-CN.md:31–56`；`coordination/build-provenance-20260908/ASSESSMENT.zh-CN.md:7–25`；`coordination/annulus-independent-pro-review-20260908/ROOT_SCALAR_REVIEW.md:26–32`；`coordination/public-s100-encoder-cap-20260909/SOURCE_BRIDGE_REVIEW.md:33–44`。

## 9. 为什么不再指定一次新的加密实验

新增行动必须能够在两个事先写明的解释之间作出区分，并改变一项有权威来源的验收判断。重跑已闭合的编码、cap、fresh 分解、同样 I₈ 传播、dense-v 或 OpenMP 发现不满足这个条件。再抽一条原 S100，即使偶然 PASS 也不修复旧样本；再抽一条 annulus 不能证明全域；补 Windows annulus 只增加一个配置的样本，不自动弥补算法语义缺口。改 σ、v、加密模式、输入或阈值寻找 PASS，则是改变问题。

当前没有一个已被本轮证据指出的规范违背，可以冻结“原源应失败、最小修复应通过”的生产测试。仅凭“原 E80 FAIL”编造这样的 RED，会把近似误差不足错误命名为源码缺陷；仅凭“没有新缺陷”宣布所有工作成功也同样不合法。**因此没有正当的下一项必做 FHE／编译／平台试验。** 这是当前证据下的行动裁决，不是宣称不存在任何未来改进。

本包完成唯一立即有依据的纠正：把交付的当前快照、各实验状态和完成标签写准确。文档勘误不改变数值结果；生产、测试、CMake、workflow 均未修改。`NEXT_BOUNDARY.md` 将这个有限动作、停止条件和重新开启的证据标准固定下来，而不是给出新一轮通用审核任务。

## 10. 对外可使用的结论

> 已完成公开 t=2 Double-CKKS 核心算法在固定官方 OpenFHE 上的 clean-room 实现，并已有真实高精度客户端、无秘密评估链、独立正反例、两平台论文规模运行及当前编码数学证书支持。当前可作有条件的算法／工程正确性判断。原 S100 冻结近单位圆八平方 E80 在 Linux／Windows 仍 FAIL；S116 两平台 PASS 和 S100 annulus 单 Linux PASS 各限于其明确条件。尚不能宣称所有冻结数值验收通过、论文表 3 原作者统计／性能完全重现、任意输入／密钥 E80 或部署安全。本轮未识别出有依据的最小生产修复，不安排无新判别依据的重复实验。

这段话允许交付已有实质成果，不把“审查完成”写成“原失败修复”，也不把“全目标尚未全项验收”扩张为无穷无尽的工作列表。

## 11. 本轮实际执行与交付

输入 ZIP：15,139,061 bytes；SHA-256 `31f5f6767718e2adaa1aae018dee8ceb44973f20125d0cef466a921a54174042`。1007 个安全常规成员，其中 1006 个载荷；CRC、集合、长度、SHA-256 全通过，995 项提供的 Git blob 绑定匹配。展开总计 46,469,553 bytes。Git blob 匹配不是对远端实时 HEAD 的证明。

本轮实际运行 33 项自己编写的有界精确标量／文本检查，全部通过；详见 `checks/bounded_checks.py` 与 `evidence/BOUNDED_CHECKS.json`。另有对文档候选的字节及语义字段检查，RED／GREEN 指文档旧状态与新状态，绝不是生产算法 RED／GREEN。

没有运行任何大小 FFT／NTT、全输入生成、编码、采样、FHE、编译或 CI，没有外部账户操作或作者联系。未重新执行供件中的任何算法、测试、候选脚本或检查器。原始供件保持不变。详见 `EXECUTION_LEDGER.md`、`evidence/INPUT_VERIFICATION.json`、`evidence/READ_COVERAGE.tsv`。
