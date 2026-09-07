# S100-CONDITION-DECISION-01：有界科学决定

日期：2026-09-07  
当前源码：`2c14d7f394ec385029716d00f9c03fad974ba88b`  
任务／证据提交：`da5b59fe0e5f8f2a957434df0fd76eb271d8720a`  
分支：`codex/s100-fresh-error-repair-20260907`

## 1. 唯一优先决定

**本轮不下发 S100 数值实现修补任务；将下一步收敛为一次针对 §6.3／Table 3 的原始实验条件澄清。外联须先取得用户明确授权，文内问询稿未发送。**

当前证据没有定位出一个违反已确定算法／接口契约、且有依据能够修复原冻结输入、公钥加密、S100 端点的生产实现缺陷。新结果具体支持上一轮 `PAPER_FRESH_CONDITIONS_UNRESOLVED` 分支，而不是“继续提高 encoder／decoder 计算精度”或“修改 Mult2 直到变绿”。也没有证据可以把未公开的论文条件差异当作已经查明的根因。

这项决定不缩减用户目标：仍须实现 clean-room OpenFHE 的论文方法，并解决、验证原冻结 S100 精度短缺。当前只拒绝无缺陷依据的数值补丁；不是接受一个更窄的完成定义，也不是断言不存在可行改进。具体原因是：**在新样本已观察到的起始相位上，把后续八平方做得完全精确，仍有三个预定锚点分量超门。因此，“仅消除额外评估误差”不是该样本达到 E80 的充分补救。** 这一结论的数值前提、样本范围和抵消例外见第 4–5 节。

| 必须分开的状态 | 本轮裁定 |
|---|---|
| 算法构造与工程流程复现 | 既有源码、回归和完成的链支持 DCP → 八次 Tensor2／Relin2／RS2 → RCB 的构造对应与执行示范；不是每个输入／密钥、所有中间前提的形式证明。 |
| 原冻结 E80 验收 | **仍为 FAIL。** 历史 Linux、Windows 原始端点未被撤销或覆盖；本轮没有新链结果。 |
| 论文 §6.3／Table 3 对应的 −81.8 bit 经验报告值 | **依据现有条件不能作同条件精确对比。** 缺少实验输入、具体噪声／加密路径、版本及误差测量实现的来源闭合。 |
| 整体论文复现完成 | **未完成。** 诊断 COMPLETE、观察器修补、另一 profile 的 PASS 均不补足原 S100 验收。 |

依据：随包上一轮 `evidence/coordination/s100-independent-semantic-review-01/pro/NEXT_STEP.md:9–35,67–87`；当前测试 `project/tests/s100_fresh_error_diagnostic_test.cpp:538–618,642–672`；历史报告 `evidence/coordination/fs-endpoint-scientific-review-return-01/pro/DECISION.md:15–56`。以下均以本次 ZIP 为来源，不访问旧包或外部系统。

## 2. 输入身份与实际核验

先独立核验 ZIP，再读取 TASK、清单、更新证据与决定相关的源码。结果如下。

| 核验项 | 结果 |
|---|---|
| 输入文件 | `s100-condition-decision-2c14d7f.zip` |
| 字节数 | `1620988`，匹配用户声明 |
| SHA-256 | `3bbb738d56a17f76807546f5f7fc4a1f4c83e889164e1dab71f0409d190e417b`，匹配 |
| 成员与路径 | 200 个唯一普通文件；无重复、目录成员、符号链接、加密成员或路径穿越 |
| 总展开字节数 | `4784453` |
| CRC | 全成员通过 |
| 内置清单 | `MANIFEST.json`，309166 字节；自排除，列出 199 个载荷 |
| 清单 SHA-256 | `0eb3c9674262cbaf9bc79202139576343b039b32826b2ff2af8194860e00d80b` |
| 逐载荷核验 | 199/199 字节数及 SHA-256 一致；成员集合与清单一致 |
| 显式 Git blob 核验 | 116/116 按 `blob <bytes>\0<payload>` 重算 SHA-1 一致 |
| TASK SHA-256 | `a84c41331f7d3035ef028b7b8133e9f467becff1d340f35377b43d9ccd04f837` |
| 论文 PDF 身份 | 759375 字节；SHA-256 `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac` |

清单的 source_commit、task_commit 与用户给定值一致。以上证明收到的字节与声明相符，不等同于独立验证远程完整 Git tree、提交签名或 runner 的二进制链接环境。清单中的历史密钥扫描属于附带证据，本轮没有重新执行 Gitleaks 或安全认证。

本轮读取范围为本次完整 TASK、传播说明／脚本／结果、完整新诊断日志和三份运行回执、观察器 RED／GREEN 证据、原始条件查证说明，以及这些结论涉及的现行源码、冻结输入／尺度、官方 PKE 和论文段落。上一轮报告按本包保全副本引用，没有改写其当时的未核验状态。本轮使用随包定版论文 TXT 定位 §2、§6；沿用已保全的上一轮 PDF 图文核对，不声称重新逐页审核图表，也没有新做公共网络溯源。

## 3. 与上一轮裁定的逐项对账

### 3.1 样本、源码与运行分账

下表中的 CI 结果均来自本包回执／日志，本轮仅离线核对，未查询 CI，未执行 CTest。

| 证据对象 | 准确身份 | 已建立的事实与界限 |
|---|---|---|
| 原完整 S100 链 | `ed5fd192…`；run `34039088536` attempt 1 | Linux E8 ≈ `9.1464736336494205e-24`；Windows ≈ `9.0653051740867722e-24`；均超过 T，原 FAIL 保留。这里使用保全的历史复核结果，不冒称重验未随本包提供的全部原始链日志。 |
| 新 fresh 诊断 | `a448b787…`；run `34110943783` attempt 1 | Linux 编译成功，默认 60 项通过，controls 通过，随后一个 fresh payload 公钥加密通过诊断；Windows／旧完整链／S116 跳过。它是一个新样本，不是上行旧链密文。 |
| 观察器行为 RED | `f5382eb4…`；run `34113380315` attempt 1 | 诊断构建和默认回归成功；共同交换槽 2/3 未被旧门拒绝，报 `EXPECTED_REJECTION_MISSING`，CTest 退出 8，run 真实失败；fresh 跳过。 |
| 观察器行为 GREEN | `2c14d7f…`；run `34113920247` attempt 1 | 默认 60 项通过；controls 10.43 秒通过；独立全槽单项式门使上述置换被按指定原因拒绝；fresh 明确跳过，未生成新 payload 样本。 |

新诊断的完整数值日志为 `evidence/coordination/s100-fresh-error-repair-01/remote-green2-diagnostic-steps.log`，SHA-256 `cbd1d44bb8bdfbf7f9ff74bc94e950ce8a7602cd5e8bb43d98da6f403b2a874d`。源码标识在第 33、82 行，末尾 COMPLETE 在第 130 行，CTest 成功记录在第 131–144 行；默认 60 项在同目录 `remote-green2-provenance-summary.log:27–28`。离线回执检查确认默认回归早于诊断，controls 早于 fresh，RED 没有被吞成成功。重叠的 focused/default 调用不累计为额外独立测试数。“一个 fresh payload”也不是说整个回归任务只发生过一次加密或密钥生成。

**不能把 a448b787 的数值结果改标为 2c14d7f 的 fresh 结果。** 后者只有新 controls 证据。历史 E8 的具体数值／极值位置见 `evidence/coordination/fs-endpoint-scientific-review-return-01/pro/results/independent_linux.json` 与 `independent_windows.json` 的 `/source_commit`、`/maxima/E8`、`/same_component`。

### 3.2 原三项发现的当前状态

**R-01：有边界的回归覆盖缺口已经修补，不是生产槽序缺陷已经修复。** 当前 `project/tests/s100_fresh_error_diagnostic_test.cpp:441–478` 构造归一化单项式 X，以 `DirectSparseReference768` 的每槽实／虚值比较两种观察精度，然后故意共同交换槽 2/3。`Reject` 在 `:100–108` 要求精确异常原因；新增门在 `:463–467` 抛 `OBSERVER_SLOT_ORDER`。原始观察须通过、置换须拒绝，不能靠无条件失败制造 GREEN。直接参考实现见 `project/tests/paper_endpoint_transform.cpp:342–390`。

这对该单项式的固定非恒等槽置换提供覆盖；它不证明任意依赖输入的变换错误都不可能存在。也不把新 controls 当作旧 fresh 密文的重新观察。本次决定所用三个超门分量都来自原先就有独立 Horner 路径的十个锚点，而不是依赖一个新增、未经核实的非锚点最大值。

**R-02：保留论文局部符号差异说明，不改 Tensor。** 论文 TXT `:233–237,270–278` 的 `c0+c1s` 解密约定和 Tensor 显示中间项不自洽；当前 `project/src/double_ckks.cpp:854–859` 仍组合官方 `EvalMultNoRelin` 和正交叉项。把该纸面负号照搬进代码会破坏原已确认的乘法恒等式，不是 S100 数值修补。

**R-03：日志收尾健壮性缺口仍未修复，但不是当前数值决定的阻断项。** `project/tests/s100_fresh_error_diagnostic_test.cpp:669–672` 仍只在最终 COMPLETE／flush 之前检查输出状态。本包已保全完整数值记录、末尾标志和运行结果，故不能以此丢弃本次有效证据，也不能把它升格为精度根因。后续修补由 root 单独持有，不在本轮下发。

上一轮的 `ldexp(Binary768(1), -300)` 语义判断得到新源码运行证据支持：a448b787 的诊断已在原 warnings-as-errors 下构建并执行。这里更新的是“附件已提供实际远端结果”，不改写上一轮曾经静态审核、当时未运行的事实。

## 4. A／B／C 实际观测与正确传播

### 4.1 分解的对象

记 m 为实际共享 encoder 产生的整数多项式，p 为独立稀疏解密取得的中心整数代表，d 为整数域的原始 `p−m`，Δ=`2^100`。在既定 canonical embedding／槽投影下令 `O(q)=can(q)/Δ`。逐槽、逐实虚分量定义：

\[
A=O(m)-z,\qquad B=O(d),\qquad
C=D_{\rm prod}(ct)-O(p),\qquad E_0=D_{\rm prod}(ct)-z.
\]

对于精确线性 O：

\[
\varepsilon=A+B=O(p)-z,\qquad E_0=A+B+C.
\]

**传播到密态平方的起始扰动是 ε=A+B，而不是 E0。** C 是本次客户端读出误差，没有被写进初始 ciphertext。新样本的 C 极小，也不能由此宣称未来最终 RCB 的读出误差已经实测同样小。

当前来源支持这一对象选择：`project/tests/s100_fresh_error_diagnostic_test.cpp:162–179` 在整数域减法，不重新中心化 d，并检查足够 headroom；`:562–592` 绑定同一基、根、尺度和官方公钥加密；`:611–638` 独立观察 m、p、d 再组成同分量带符号元组。正 headroom 证明所选中心代表与整数重构自洽，**不证明未观察到的 sampler 整数提升不含 Q 的倍数**，亦不分离公钥噪声、v、e0、e1 的贡献。

### 4.2 一个新样本的观测

| 数量 | 全槽最大绝对分量（日志观察值） | 位置 |
|---|---:|---|
| A | `1.3048302662304672e-28` | 5957／虚部 |
| B | `3.3701443335174188e-25` | 6908／实部 |
| C | `9.9998075384801244e-129` | 4555／虚部 |
| E0 | `3.3704198850846009e-25` | 6908／实部 |

不是将四个极值相加。在 E0 最大的**同一**位置 `(6908, real)`，日志保留：

\[
A\approx-2.7555156718212400\times10^{-29},\quad
B\approx-3.3701443335174188\times10^{-25},
\]
\[
C\approx+6.2203252659843302\times10^{-129},\quad
E_0\approx-3.3704198850846009\times10^{-25}.
\]

这支持“该样本该端点的主要 fresh 误差来自 B”，没有观察到需要重写 encoder／decoder 的主导异常。它不是噪声总体分布认证；A 低于条件舍入界也不证明全部舍入正确。来源：新日志 `:94–101`。

同分量重构与独立线性残差的全槽观察最大值均约 `7.3237828182e-154`；十个锚点字符串按精确有理数重新相加的最大 `|A+B+C−E0|` 约 `3.2043784981e-154`。`2^-300≈4.9090934653e-91` 是观察一致性门，不是 E80 门；打印的 `2^-86` 舍入界仍明示前提未由本次运行建立。来源：新日志 `:122–130`；本轮标量检查见附录。

### 4.3 有界传播已独立复核

对原来就固定的十个锚点，分别将本槽实、虚部配成复数，计算：

\[
G_s=(z_s+\varepsilon_s)^{256}-z_s^{256},\qquad \varepsilon_s=A_s+B_s.
\]

本轮实际执行了随包校正脚本，使用 250 位十进制定向区间运算；每个 ε 分量在相加后加上 `±1e-100` 分析包络。另写了只处理这 20 组已保留锚点分量数据的独立检查：用精确整数／有理数递推

\[
w_{k+1}=w_k^2,\qquad
\delta_{k+1}=2w_k\delta_k+\delta_k^2,\qquad
w_0=z,\ \delta_0=A+B
\]

得到 δ8，再与原脚本内部完整区间比较。它不用浮点 FFT、三角函数、密文、私钥或 OpenFHE。20 个精确中心值全部落入原定向区间，门限分类一致。

冻结门保持

\[
T=2^{-80}=8.2718061255302767487140869206996285356581211090087890625\times10^{-25}.
\]

| 预定锚点 | Re G（带符号，四舍五入展示） | Im G（带符号，四舍五入展示） | 最大绝对分量／T |
|---:|---:|---:|---:|
| 0 | `+8.8269878841e-27` | `+2.7084005245e-25` | 0.32742553 |
| 1 | `−6.9441679252e-25` | `−7.0552604625e-25` | 0.85292865 |
| 256 | `−6.5688703517e-27` | `−2.3633256430e-25` | 0.28570854 |
| 257 | `−1.7108610071e-25` | `−5.8445648250e-26` | 0.20683040 |
| 512 | `+3.3703142405e-25` | `+1.0990385494e-24` | **1.32865608** |
| 513 | `−4.2931017710e-25` | `+3.9135517142e-26` | 0.51900416 |
| 768 | `−2.3326331948e-25` | `−1.8060143613e-25` | 0.28199805 |
| 769 | `−1.2370525619e-24` | `+3.0007431478e-25` | **1.49550478** |
| 1023 | `+2.0832419874e-24` | `−7.2738450018e-25` | **2.51848503** |
| 16383 | `+5.7381221436e-25` | `−5.8561076728e-25` | 0.70795998 |

实际定向计算给出三个**向外取整的绝对值包围区间**，不是将上表近似值当作严格界：

| 分量 | 包围区间 |
|---|---|
| 512／虚部 | `[1.09903854939e-24, 1.09903854940e-24]` |
| 769／实部 | `[1.23705256187e-24, 1.23705256188e-24]` |
| 1023／实部 | `[2.08324198737e-24, 2.08324198738e-24]` |

三个下界均严格大于 T，其他 17 个分量的区间上界小于 T。最大传播区间宽度约 `1.4633843208594526e-98`。槽 0 被单独保留，两个分量均低于 T；**一个槽通过不能证明所有槽通过，而一个可信分量超门足以否定这一次理想相位传播的全槽通过**。1023 只是十个已保留锚点中的最大分量位置，不是新样本全槽理想传播最大位置的测定。

数值来源：`evidence/coordination/s100-fresh-error-repair-01/FRESH_IDEAL_PROPAGATION.md` 全文、`check_fresh_ideal_propagation.py` 全文和新日志 `:102–121`；本轮复算命令与结果见附录 A–C。旧 E0 传播记录保留为历史，未重标为 A+B 的执行记录。

### 4.4 必须保留的包络前提

定向运算严格包围的是“打印出来的 A+B 所定义的箱体”中的复数幂。用于真实 canonical 相位时，仍要求正确的十进制格式／解析，以及独立观察足够准确。`1e-100` 不来自对 Boost π／sin／cos 和全部上游操作的形式认证；尤其不能仅从“通过 2^-300 一致性门”推出误差小于更小的 `1e-100`。

实际记录的跨精度／Horner 差异及残差远小于这些量级，为数值解释提供支持；独立精确递推验证了后处理算术，**没有把观察器本身变成严格超越函数区间 oracle**。分析包络和一致性门均未改变密码实验的 T。这里不推导任何 sampler tail、总体成功率或安全强度结论。

## 5. 为什么不能据此发一个“评估器提精度”补丁

### 5.1 直接数学理由与抵消限制

令 x=`O(p)=z+ε`。对同一新样本的某个假想最终读出 y 定义

\[
r=y-x^{256}.
\]

r 是相对于理想起始相位幂的额外端点差，包含后续评估与最终读出，不能以本次 fresh 的 C 代替。恒等式为

\[
y-z^{256}=G+r.
\]

对于 1023／实部，若 y 要满足该分量的 E80 门，则 r 必须与正的 G 反向，并至少具有约 `1.25606137482e-24` 的绝对量级。必要条件来自**同一个分量**的 `|r|≥|G|−T`；其精确中心所需量约 `1.2560613748246244e-24`。

因此，在起始相位、输入目标和平方语义不变时，让 r 趋近零，端点趋向的不是 z^256，而是 x^256。仅把后续算术“更精确”没有给出可靠消除 inherited error 的机制。

但不能反过来声称任何评估器、任何样本都必败。`r=−G` 是一个立即成立的代数抵消反例；适度减少某种额外误差也可能偶然改善或恶化总误差。当前没有该新样本的真实 r，故 G 的下界**不是新密文真实 E8 的无条件下界**。历史两条链在其各自端点最大位置已经显示小的反向额外差，见历史报告 `pro/DECISION.md:49–56`，不能把那里的 r 数值移用到这个新样本。

### 5.2 没有证据支持的修复不得伪装成工程任务

当前真正的 payload 入口仍在 `project/src/high_precision_client_io.cpp:606–635`：共享 `ComputeEncoding` 后调用官方 `GetScheme()->Encrypt(element, publicKey)`，不是绕过公钥加密。共享计算与检查入口见 `:462–495,597–603`。

官方公钥加密与当前密钥构造的相位关系为

\[
p\equiv m+e_{\rm pk}v+e_0+e_1s\pmod Q.
\]

依据：`references/official-full/src/pke/lib/schemerns/rns-pke.cpp:138–196`，`project/src/paper_h128_client_keypair.cpp:193–217`。后者的官方私钥零加密调用是在**构造公钥**，不是将 fresh payload 改为私钥加密。当前 h128 限定的是 s；官方非 GAUSSIAN 分支的 v 没有传入 h128 限定。配置在 `project/src/repeated_mult2.cpp:137–142,184–185` 固定 `3.19F` 和 noiseScale=1。B 观察没有证实这条路径的实现违反上述方程，也没有证实它与未知 HEaaN 条件不一致。

诊断中的 p／d 来自持有实验私钥的独立观察端；其 A／B 值不是服务端可无条件取得的纠错输入。将私钥、真实误差或冻结 z 的预计算答案注入评估器，会改变可用信息或绕开复现语义。**这不是所有无密钥纠错算法的信息论不可能性证明**；只是本包没有提供一个保持冻结条件、可计算且可验证的纠错机制，更没有把某个现有语句证伪。

本轮因此没有“受影响的错误生产行＋可区分 RED”的数值修复 brief。R-01 的可区分 RED 已用于观察器覆盖并闭合；R-03 是另一项低优先级日志健壮性修补。二者不能充当 E80 数值补丁的证据。

## 6. 精确缺失的论文条件及唯一后继

论文 §2.1 明确给了抽象公钥 Enc；这不能反推 §6.3 实际调用了哪个 HEaaN API。§6.3 明确是八次平方、100-bit 场景并报告 1,000 次执行的平均无穷范数误差及对应 −81.8 bit 数字；没有给出足够的消息／噪声／版本／测量代码以唯一还原该数字。用户删除 1,000 次要求的决定保持有效，论文的平均统计也不是每一次密文必过 T 的保证。

§6.1／6.2 的 h=21845、较低尺度和 18 层刷新策略属于另外两组实验，不能借来为 Table 3 的 h128／S100 改条件。§6.3 明确说明无需 §6.2 的中途重组分解策略；本轮没有据此要求添加刷新。来源：论文 TXT `:1464–1476,1479–1507,1509–1525,1562–1590`；已保全的上一轮 `CLAIM_BOUNDARY.md` 的论文映射。

真正欠缺的不是更多模型赞同，而是以下**一个版本绑定的实验条件记录**：

| 待闭合字段 | 对本决定的具体作用 |
|---|---|
| 消息生成器、值域、实／复数与槽数／布局、固定还是重抽 | 决定 fresh 扰动经过 z→z^256 的增益；一般的单位范数假设不是实际输入分布。当前 16384 槽 dyadic 家族是明确的冻结复现条件，不能冒称论文公开数据。 |
| 具体 χ_enc／χ_err 及使用位置 | 需要 v 的支撑／权重／概率，公钥、payload 和相关键误差的数值参数、截断／尾部规则；h128 不能唯一决定这些量。 |
| 实际 HEaaN 版本与加密调用、执行间重采样对象 | 需要发布版／commit／可公开构建标识、public/secret-key API 与参数；抽象 Enc 的公钥公式不等于实验调用记录。 |
| 实际尺度、模数及编码／解码配置 | Table 3 的位数和近似关系不是所用每个素数、编码精度／舍入与尺度实现的完整配置；本包 Q／roots 是当前 OpenFHE 冻结实例。 |
| 误差测量函数与参考真值 | 要区分原始意图 z^256 与 realized-fresh 值的幂，是否包括 fresh I/O，复数模长范数与实虚最大分量，以及平均／取对数的实现顺序。当前冻结门明确是实虚分量最大值，不能悄悄换量。 |

最后一项是为精确比较补齐测量定义，**不是声称已经发现论文用了另一种定义**。按通常复向量无穷范数与当前实虚最大分量范数的关系，单个分量超 T 也会使复数模长超 T；因此不能靠把“分量门”改成“复数模长门”消除这三个超门事实。

随包 `PAPER_EXPERIMENT_PROVENANCE.md:16–22,43–66` 记录了 2026-09-07 的有界第一方检索，未找到能闭合主要缺口的公开 artifact。这是已提供的检索记录，不是本轮重新上网确认，也不是“任何私人材料都不存在”的证明。它支持停止无目标的重复搜索，改为一次定向复现咨询。

**唯一后继：在用户明确授权一次外部作者联系后，由任务所有者发出 `NEXT_ACTION.md` 的未发送问询，索取与 Table 3 明确绑定、可核对的实验脚本／配置／文字说明。** 当前授权只允许常规代码／设计判断，不包括发送邮件；本轮未联系作者、未建立邮箱草稿、未读取或修改外部账户。

若日后来源说明条件不同，须先准确登记差异；调整 profile 或验收需另行明确授权，且必须作为另一个条件结果保留，不能覆盖原 FAIL。若来源相同，也不自动证明 OpenFHE 有 bug；只有具体的源代码／方程冲突及可区分测试才足以重新打开有界实现修补。当前目标仍然未完成。

## 7. 不确定性分类与结束条件

| 不确定性 | 类别／处置 |
|---|---|
| Table 3 原始输入、加密／噪声、版本和测量定义未闭合 | **阻断同条件经验值解释**；直接决定下一步取证对象。并不妨碍保留原冻结门已失败的事实。 |
| 观察器到精确 canonical 相位的误差包络不是形式证明 | **限制反事实结论的严格度**；定向标量算术已复核，超门结论仍带观察前提。不将其升级为全算法不可行定理。 |
| 本包没有原冻结 S100 修复后通过的完整链证据；新 fresh 只有一个样本 | **运行证据尚未建立**；本轮不推算或生成它，不承诺后续会通过。当前 2c14d7f 也未运行 fresh。 |
| R-03 最终流状态、一般性任意输入错误、统计成功率与安全估计 | **不作为本轮数值主任务**；日志完整性已针对本次证据核对，其他结论不由此建立。 |

本轮决定到此成立：**无有证据支持的 E80 数值补丁；只提交一次需授权的原始条件澄清请求。** 不新增加密、FFT／全槽数值回放、CI 调用、采样器改动或实验套件；不以重试直到通过替换原始结果。


---

## 附录 A：身份及回执检查的实际执行记录

以下为本轮已执行的本地检查，不是后续待执行的工程任务。Python 版本为 **3.13.5**。命令均退出 **0**；没有运行 C++ 测试、OpenFHE 构建、加密、FFT／全槽 replay 或网络动作。为了保持交付只有三个成员，本轮新增的核验脚本嵌在 Markdown 中，不作为项目补丁或新测试门提交。

命令中的 `/mnt/data/s100_condition_work` 仅是本轮临时工作路径，不是假设用户或 root 存在的路径。复核时可将附录代码分别保存为对应文件名并替换工作路径；不得据此扩大为密码实验。

### A.1 输入核验

实际命令：

```sh
python /mnt/data/s100_condition_work/verify_input.py
```

实际标准输出／保存的 `verify_input.json`：

```json
{
  "archive_bytes": 1620988,
  "archive_sha256": "3bbb738d56a17f76807546f5f7fc4a1f4c83e889164e1dab71f0409d190e417b",
  "unique_regular_members": 200,
  "uncompressed_bytes": 4784453,
  "CRC": "PASS",
  "manifest_bytes": 309166,
  "manifest_sha256": "0eb3c9674262cbaf9bc79202139576343b039b32826b2ff2af8194860e00d80b",
  "verified_payloads": 199,
  "verified_git_blob_hashes": 116,
  "source_commit": "2c14d7f394ec385029716d00f9c03fad974ba88b",
  "task_commit": "da5b59fe0e5f8f2a957434df0fd76eb271d8720a",
  "TASK_sha256": "a84c41331f7d3035ef028b7b8133e9f467becff1d340f35377b43d9ccd04f837"
}
```

实际执行脚本 `verify_input.py`：

```python
from pathlib import Path, PurePosixPath
import zipfile, hashlib, json, stat
root=Path('/mnt/data/s100_condition_work')
p=Path('/mnt/data/s100-condition-decision-2c14d7f.zip')
b=p.read_bytes(); assert len(b)==1620988
assert hashlib.sha256(b).hexdigest()=='3bbb738d56a17f76807546f5f7fc4a1f4c83e889164e1dab71f0409d190e417b'
with zipfile.ZipFile(p) as z:
 infos=z.infolist(); names=[i.filename for i in infos]
 assert len(infos)==len(set(names))==200
 for i in infos:
  q=PurePosixPath(i.filename); mode=(i.external_attr>>16)&0xffff
  assert not q.is_absolute() and '..' not in q.parts and not i.is_dir()
  assert not (i.flag_bits & 1)
  assert stat.S_IFMT(mode) in (0,stat.S_IFREG)
 assert z.testzip() is None
 mb=z.read('MANIFEST.json'); m=json.loads(mb)
 assert m['manifest_self_excluded'] is True
 assert m['source_commit']=='2c14d7f394ec385029716d00f9c03fad974ba88b'
 assert m['task_commit']=='da5b59fe0e5f8f2a957434df0fd76eb271d8720a'
 entries=m['files']; assert len(entries)==199
 assert len(set(e['path'] for e in entries))==199
 assert set(e['path'] for e in entries)==set(names)-{'MANIFEST.json'}
 blob_count=0
 for e in entries:
  eb=z.read(e['path']); assert len(eb)==e['bytes'],e['path']
  assert hashlib.sha256(eb).hexdigest()==e['sha256'],e['path']
  o=e.get('origin',{})
  if o.get('git_blob'):
   assert hashlib.sha1(b'blob '+str(len(eb)).encode()+b'\0'+eb).hexdigest()==o['git_blob'],e['path']
   blob_count+=1
 z.extractall(root/'input')
result={'archive_bytes':len(b),'archive_sha256':hashlib.sha256(b).hexdigest(),'unique_regular_members':len(infos),'uncompressed_bytes':sum(i.file_size for i in infos),'CRC':'PASS','manifest_bytes':len(mb),'manifest_sha256':hashlib.sha256(mb).hexdigest(),'verified_payloads':len(entries),'verified_git_blob_hashes':blob_count,'source_commit':m['source_commit'],'task_commit':m['task_commit'],'TASK_sha256':hashlib.sha256((root/'input/TASK.md').read_bytes()).hexdigest()}
(root/'verify_input.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2))
```

### A.2 离线运行回执与日志检查

实际命令：

```sh
python -B -I /mnt/data/s100_condition_work/check_receipts.py /mnt/data/s100_condition_work/input
```

实际标准输出：

```text
GREEN2_RUN.json: source=a448b787399b43b6024d82c170add403969b493c default60=success diagnostic_build=success controls=success fresh=success Windows=skipped source_scoped_order=PASS
OBSERVER_ORDER_RED_EVIDENCE.txt: source=f5382eb4f2015a991322f1e7430b28055cc0a4fb default60=success diagnostic_build=success controls=failure fresh=skipped Windows=skipped source_scoped_order=PASS
OBSERVER_ORDER_GREEN_EVIDENCE.txt: source=2c14d7f394ec385029716d00f9c03fad974ba88b default60=success diagnostic_build=success controls=success fresh=skipped Windows=skipped source_scoped_order=PASS
fresh_log: 144 lines; 4 extrema/24 signed tuples/20 anchor components/3 oracle summaries/terminal markers=PASS
maximum=encoding line=94 slot=5957 component=imag
maximum=encryption line=96 slot=6908 component=real
maximum=decoding line=98 slot=4555 component=imag
maximum=E0 line=100 slot=6908 component=real
scalar=same_component_reconstruction_max line=122
scalar=independent_linearity_max line=123
scalar=legacy_decimal100_bridge_max line=124
scalar=production_internal_cross_precision line=125
scalar=agreement_absolute_tolerance line=126
scalar=conditional_nearest_coefficient_rounding_component_bound line=127
historical_linux: retained_E8_lower=9.14647363364942050896448083752962E-24; not re-executed
historical_windows: retained_E8_lower=9.06530517408677222408922761230152E-24; not re-executed
RESULT=PASS; hosted outcomes are supplied evidence, not local CTest executions
```

这里检查的是已提供的 step 状态、先后顺序、失败原因、完整标志、锚点键与十进制字段。对 global maxima 的检查限于“最大值记录与其同位置元组吻合”，不是从未保留的全槽数组重算最大值；对历史 E8 的检查限于已保全报告，不是重新运行历史 observer。回执中的远端 success 不是本轮本地测试 success。

实际执行脚本 `check_receipts.py`：

```python
"""Offline receipt/text check only; no network, build, FFT or cryptography."""
from pathlib import Path
from fractions import Fraction as F
import json
import re
import sys

root=Path(sys.argv[1])
base=root/'evidence/coordination/s100-fresh-error-repair-01'
cases=(
 ('GREEN2_RUN.json','a448b787399b43b6024d82c170add403969b493c','success','success','success'),
 ('OBSERVER_ORDER_RED_EVIDENCE.txt','f5382eb4f2015a991322f1e7430b28055cc0a4fb','failure','failure','skipped'),
 ('OBSERVER_ORDER_GREEN_EVIDENCE.txt','2c14d7f394ec385029716d00f9c03fad974ba88b','success','success','skipped'),
)
for filename,sha,outcome,control,fresh in cases:
    raw=(base/filename).read_text()
    run,end=json.JSONDecoder().raw_decode(raw.lstrip())
    assert run['headSha']==sha and run['attempt']==1
    assert run['status']=='completed' and run['conclusion']==outcome
    jobs={j['name']:j for j in run['jobs']}
    assert jobs['linux-gcc']['conclusion']==outcome
    assert jobs['windows-mingw64']['conclusion']=='skipped'
    steps={s['name']:s for s in jobs['linux-gcc']['steps']}
    default=steps['Run complete 60-test three-track suite']
    build=steps['Build S100 fresh-error diagnostic']
    controls=steps['Run S100 encoding inspection contract once']
    payload=steps['Run S100 fresh-error diagnostic once']
    assert default['conclusion']==build['conclusion']=='success'
    assert controls['conclusion']==control and payload['conclusion']==fresh
    assert default['completedAt']<=build['startedAt']<=build['completedAt']<=controls['startedAt']
    if fresh=='success':
        assert controls['completedAt']<=payload['startedAt']
    assert steps['Build paper full eight-square contract']['conclusion']=='skipped'
    for name,step in steps.items():
        if 'precision116' in name:
            assert step['conclusion']=='skipped'
        if 'public API contract' in name:
            assert step['conclusion']=='success'
    tail=raw.lstrip()[end:]
    if filename=='OBSERVER_ORDER_RED_EVIDENCE.txt':
        assert 'reason=EXPECTED_REJECTION_MISSING' in tail
        assert 'Process completed with exit code 8' in tail
        assert 'status=COMPLETE mode=controls' not in tail
    if filename=='OBSERVER_ORDER_GREEN_EVIDENCE.txt':
        assert tail.count('rejected_shared_nonanchor_permutation=1 public_encryptions=0')==1
        assert tail.count('status=COMPLETE mode=controls')==1
        assert 'status=COMPLETE mode=fresh' not in tail
    print(f'{filename}: source={sha} default60=success diagnostic_build=success '
          f'controls={control} fresh={fresh} Windows=skipped source_scoped_order=PASS')

text=(base/'remote-green2-diagnostic-steps.log').read_text()
assert text.count('status=COMPLETE mode=controls')==1
assert text.count('status=COMPLETE mode=fresh public_encryptions=1')==1
assert 'status=INVALID' not in text
assert 'original_S100_E80=NOT_RERUN prior_FAIL=RETAINED precision_claim=NONE' in text
assert 'established_by_this_run=NO' in text and 'hidden_sampler_wrap_claim=NONE' in text
maxima={}; tuples={}; scalars={}; oracles={}; rowcount=0
for number,line in enumerate(text.splitlines(),1):
    fields=dict(re.findall(r'(\w+)=([^\s]+)',line))
    if 'S100 tuple=' in line:
        key=(fields['tuple'],int(fields['slot']),fields['component'])
        assert key not in tuples
        values={k:F(fields[k]) for k in ('z','encoding','encryption','decoding','E0','reconstruction_residual')}
        assert abs(values['encoding']+values['encryption']+values['decoding']-values['E0'])<=F(1,2**300)
        tuples[key]=(number,values);rowcount+=1
    if 'S100 maximum=' in line:
        name=fields['maximum'];assert name not in maxima
        maxima[name]=(number,F(fields['value']),int(fields['slot']),fields['component'])
    if 'S100 scalar=' in line:
        name=fields['scalar'];assert name not in scalars
        scalars[name]=(number,F(fields['value']))
    if 'S100 oracle=' in line:
        name=fields['oracle'];assert name not in oracles
        assert int(fields['slots'])==16384 and int(fields['horner_anchors'])==10
        assert 0<=F(fields['cross512_768'])<=F(1,2**300)
        assert 0<=F(fields['horner_max'])<=F(1,2**300)
        oracles[name]=number
assert len(maxima)==4 and rowcount==24 and set(oracles)=={'m','p','raw_p_minus_m'}
for name,(_,value,slot,component) in maxima.items():
    assert abs(tuples[(name,slot,component)][1][name])==value
assert scalars['agreement_absolute_tolerance'][1] > 0
assert scalars['conditional_nearest_coefficient_rounding_component_bound'][1]==F(1,2**86)
assert set((s,c) for selector,s,c in tuples if selector=='anchor')=={
    (s,c) for s in (0,1,256,257,512,513,768,769,1023,16383) for c in ('real','imag')}
print('fresh_log: 144 lines; 4 extrema/24 signed tuples/20 anchor components/3 oracle summaries/terminal markers=PASS')
for name,(number,_,slot,component) in maxima.items():
    print(f'maximum={name} line={number} slot={slot} component={component}')
for name,(number,_) in scalars.items():
    print(f'scalar={name} line={number}')
for os in ('linux','windows'):
    p=root/f'evidence/coordination/fs-endpoint-scientific-review-return-01/pro/results/independent_{os}.json'
    result=json.loads(p.read_text())
    assert result['source_commit']=='ed5fd192a89d6d4728ad295e87cf06a3f4abc832'
    assert F(result['maxima']['E8']['lower'])>F(1,2**80)
    print(f'historical_{os}: retained_E8_lower={result["maxima"]["E8"]["lower"]}; not re-executed')
print('RESULT=PASS; hosted outcomes are supplied evidence, not local CTest executions')
```

## 附录 B：校正 A+B 定向区间脚本的实际执行

在完整阅读脚本、确认其只使用标准库处理十个锚点后，实际执行：

```sh
python --version
python -B -I /mnt/data/s100_condition_work/input/evidence/coordination/s100-fresh-error-repair-01/check_fresh_ideal_propagation.py
```

Python 输出 `Python 3.13.5`；脚本退出 0。脚本就在输入包中，不在本交付重复创建一个改版；其 SHA-256 为 `9fbf1e6747c621400a6992ecdba0fee37724aebf5719466010b5fa92e23d734f`。

以下为本轮实际输出。一般展示字段只有约 24 位有效数字，不能把显示相同的 `signed_low`／`signed_high` 当成零宽区间；三个 `certified_component_abs_interval` 是特意向外显示的粗包络。这里的 “certified” 只指脚本输入箱体的定向算术，不认证真实超越函数观察误差、噪声或密文端点。

```text
source=a448b787399b43b6024d82c170add403969b493c log_sha256=cbd1d44bb8bdfbf7f9ff74bc94e950ce8a7602cd5e8bb43d98da6f403b2a874d
checker_sha256=9fbf1e6747c621400a6992ecdba0fee37724aebf5719466010b5fa92e23d734f
perturbation=A+B excluded_readout=C historical_E0=CONSISTENCY_ONLY
anchors=10 components=20 squarings=8 decimal_precision=250
input_component_radius=1E-100 threshold=8.2718061255302767487140869206996285356581211090087890625E-25
slot=0 component=real signed_low=8.82698788412665017301662E-27 signed_high=8.82698788412665017301662E-27 ratio_low=1.06711735625462119385028E-2 ratio_high=1.06711735625462119385028E-2 status=BELOW
prespecified_slot0_witness component=real status=BELOW absolute_upper_bound=8.82698788412665017301662E-27 ratio_upper=1.06711735625462119385028E-2
slot=0 component=imag signed_low=2.70840052450442799718872E-25 signed_high=2.70840052450442799718872E-25 ratio_low=3.27425532393120716477784E-1 ratio_high=3.27425532393120716477784E-1 status=BELOW
prespecified_slot0_witness component=imag status=BELOW absolute_upper_bound=2.70840052450442799718872E-25 ratio_upper=3.27425532393120716477784E-1
slot=1 component=real signed_low=-6.94416792520149884607294E-25 signed_high=-6.94416792520149884607294E-25 ratio_low=8.39498390051584093340398E-1 ratio_high=8.39498390051584093340398E-1 status=BELOW
slot=1 component=imag signed_low=-7.05526046252440221490977E-25 signed_high=-7.05526046252440221490977E-25 ratio_low=8.52928653725200067056370E-1 ratio_high=8.52928653725200067056370E-1 status=BELOW
slot=256 component=real signed_low=-6.56887035170718994054169E-27 signed_high=-6.56887035170718994054169E-27 ratio_low=7.94127697387985201007958E-3 ratio_high=7.94127697387985201007958E-3 status=BELOW
slot=256 component=imag signed_low=-2.36332564302873040198802E-25 signed_high=-2.36332564302873040198802E-25 ratio_low=2.85708539001477843129026E-1 ratio_high=2.85708539001477843129026E-1 status=BELOW
slot=257 component=real signed_low=-1.71086100710270276057422E-25 signed_high=-1.71086100710270276057422E-25 ratio_low=2.06830404525834484077326E-1 ratio_high=2.06830404525834484077326E-1 status=BELOW
slot=257 component=imag signed_low=-5.84456482501507588252625E-26 signed_high=-5.84456482501507588252625E-26 ratio_low=7.06564532137218235354596E-2 ratio_high=7.06564532137218235354596E-2 status=BELOW
slot=512 component=real signed_low=3.37031424049790731702872E-25 signed_high=3.37031424049790731702872E-25 ratio_low=4.07445990555278903116274E-1 ratio_high=4.07445990555278903116274E-1 status=BELOW
slot=512 component=imag signed_low=1.09903854939649781464032E-24 signed_high=1.09903854939649781464032E-24 ratio_low=1.32865607911723423288160E+0 ratio_high=1.32865607911723423288160E+0 status=EXCEEDS
certified_component_abs_interval=[1.09903854939E-24,1.09903854940E-24]
slot=513 component=real signed_low=-4.29310177096118474565223E-25 signed_high=-4.29310177096118474565223E-25 ratio_low=5.19004157714826628426343E-1 ratio_high=5.19004157714826628426343E-1 status=BELOW
slot=513 component=imag signed_low=3.91355171419836120535744E-26 signed_high=3.91355171419836120535744E-26 ratio_low=4.73119371369149080891225E-2 ratio_high=4.73119371369149080891225E-2 status=BELOW
slot=768 component=real signed_low=-2.33263319482653412055103E-25 signed_high=-2.33263319482653412055103E-25 ratio_low=2.81998049691595874004706E-1 ratio_high=2.81998049691595874004706E-1 status=BELOW
slot=768 component=imag signed_low=-1.80601436134520791291859E-25 signed_high=-1.80601436134520791291859E-25 ratio_low=2.18333739202504653427149E-1 ratio_high=2.18333739202504653427149E-1 status=BELOW
slot=769 component=real signed_low=-1.23705256187948193549070E-24 signed_high=-1.23705256187948193549070E-24 ratio_low=1.49550478227652947333495E+0 ratio_high=1.49550478227652947333495E+0 status=EXCEEDS
certified_component_abs_interval=[1.23705256187E-24,1.23705256188E-24]
slot=769 component=imag signed_low=3.00074314777302624044291E-25 signed_high=3.00074314777302624044291E-25 ratio_low=3.62767586937448805825689E-1 ratio_high=3.62767586937448805825689E-1 status=BELOW
slot=1023 component=real signed_low=2.08324198737765211086267E-24 signed_high=2.08324198737765211086267E-24 ratio_low=2.51848502704613704383691E+0 ratio_high=2.51848502704613704383691E+0 status=EXCEEDS
certified_component_abs_interval=[2.08324198737E-24,2.08324198738E-24]
slot=1023 component=imag signed_low=-7.27384500176740543945115E-25 signed_high=-7.27384500176740543945115E-25 ratio_low=8.79353903051143441877048E-1 ratio_high=8.79353903051143441877048E-1 status=BELOW
slot=16383 component=real signed_low=5.73812214359002760419156E-25 signed_high=5.73812214359002760419156E-25 ratio_low=6.93696401548842699910786E-1 ratio_high=6.93696401548842699910786E-1 status=BELOW
slot=16383 component=imag signed_low=-5.85610767276337141254612E-25 signed_high=-5.85610767276337141254612E-25 ratio_low=7.07959976804697740401456E-1 ratio_high=7.07959976804697740401456E-1 status=BELOW
exceeding_components=[(512, 'imag'), (769, 'real'), (1023, 'real')]
largest_anchor_lower_bound_slot=1023 component=real
largest_anchor_absolute_lower_bound=2.08324198737765211086267E-24
maximum_signed_interval_width=1.46338432085945257529734E-98
maximum_anchor_readout_abs=9.21787115704854228446977E-129
maximum_anchor_E0_minus_phase_abs_upper=9.21787115704854228446977E-129
maximum_anchor_ABC_minus_E0_abs_upper=3.20437849809351507366057E-154
largest_anchor_required_opposing_error_lower_bound=1.25606137482E-24
```

## 附录 C：独立精确整数递推及交叉核对

实际命令：

```sh
python -B -I /mnt/data/s100_condition_work/independent_scalar_check.py /mnt/data/s100_condition_work/input/evidence/coordination/s100-fresh-error-repair-01
```

退出 0。独立部分包括重新解析、严格检查源码／日志身份与冻结 z、用高斯整数表示复数并执行误差递推。它不调用原脚本计算中心误差；只导入已经读过的区间函数，取内部全精度区间进行包含性和门限交叉检查，不执行其 main。没有迭代抽样，也没有读取私钥。程序只证明列明的标量关系；观察精度的前提仍见正文 §4.4。

实际执行脚本 `independent_scalar_check.py`：

```python
"""Only ten public, retained complex anchor scalars; no FFT or cryptography."""
from pathlib import Path
from fractions import Fraction as F
from decimal import Decimal as D, localcontext
from math import lcm
import importlib.util
import hashlib
import re
import sys

base = Path(sys.argv[1])
log = base / 'remote-green2-diagnostic-steps.log'
raw = log.read_bytes()
assert hashlib.sha256(raw).hexdigest() == 'cbd1d44bb8bdfbf7f9ff74bc94e950ce8a7602cd5e8bb43d98da6f403b2a874d'
text = raw.decode('utf-8')
assert text.count('status=COMPLETE mode=fresh public_encryptions=1') == 1
assert 'mode=--fresh source=a448b787399b43b6024d82c170add403969b493c' in text
anchors = (0, 1, 256, 257, 512, 513, 768, 769, 1023, 16383)
rows = {}
for line in text.splitlines():
    if 'S100 tuple=anchor ' not in line:
        continue
    fields = dict(re.findall(r'(\w+)=([^\s]+)', line))
    key = (int(fields['slot']), fields['component'])
    assert key not in rows
    rows[key] = {k: F(fields[k]) for k in ('z', 'encoding', 'encryption', 'decoding', 'E0')}
assert set(rows) == {(s,c) for s in anchors for c in ('real','imag')}

# Import the audited standard-library interval code without executing its main.
# Independent center values below use integer delta recurrence, not this code.
script = base / 'check_fresh_ideal_propagation.py'
assert hashlib.sha256(script.read_bytes()).hexdigest() == '9fbf1e6747c621400a6992ecdba0fee37724aebf5719466010b5fa92e23d734f'
spec = importlib.util.spec_from_file_location('supplied_interval', script)
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


def gaussian_product(x, y):
    return (x[0]*y[0]-x[1]*y[1], x[0]*y[1]+x[1]*y[0])


def exact_error(z, e):
    denominator = lcm(*(v.denominator for v in (*z,*e)))
    znum = tuple(v.numerator*(denominator//v.denominator) for v in z)
    enum = tuple(v.numerator*(denominator//v.denominator) for v in e)
    for _ in range(8):
        ze = gaussian_product(znum,enum)
        ee = gaussian_product(enum,enum)
        enum = (2*ze[0]+ee[0],2*ze[1]+ee[1])
        znum = gaussian_product(znum,znum)
        denominator *= denominator
    return tuple(F(v,denominator) for v in enum)


def decimal_exact(x):
    # The log numbers terminate in decimal and fit within 250 digits.
    with localcontext() as ctx:
        ctx.prec = 250
        result = D(x.numerator)/D(x.denominator)
    assert F(result) == x
    return result


def frozen(slot):
    t = slot//2
    a = F(1015,1024)-F(t%16,65536)+F(slot,2**75)
    b = F(1+(t//16)%8,1024)
    if (t//512)%2:
        b = -b
    return ((a,b),(-b,a),(-a,-b),(b,-a))[(t//128)%4]


def display(x):
    with localcontext() as ctx:
        ctx.prec = 30
        return format(D(x.numerator)/D(x.denominator), '.23E')


threshold = F(1,2**80)
exceeding = []
comparisons = 0
largest = (F(0),None,None)
residual_max = F(0)
readout_max = F(0)
for slot in anchors:
    pair = [rows[(slot,c)] for c in ('real','imag')]
    z = tuple(row['z'] for row in pair)
    assert z == frozen(slot)
    e = tuple(row['encoding']+row['encryption'] for row in pair)
    exact = exact_error(z,e)
    observed_intervals = []
    for row in pair:
        phase = module.add(module.point(decimal_exact(row['encoding'])),
                           module.point(decimal_exact(row['encryption'])))
        error_box = (module.subtract(phase,module.point(module.RADIUS))[0],
                     module.add(phase,module.point(module.RADIUS))[1])
        observed_intervals.append(module.add(module.point(decimal_exact(row['z'])),error_box))
        residual_max = max(residual_max,abs(row['encoding']+row['encryption']+row['decoding']-row['E0']))
        readout_max = max(readout_max,abs(row['decoding']))
    reference = module.eight_squares(tuple(module.point(decimal_exact(v)) for v in z))
    propagated = module.eight_squares(tuple(observed_intervals))
    for index,component in enumerate(('real','imag')):
        enclosure = module.subtract(propagated[index],reference[index])
        assert F(enclosure[0]) <= exact[index] <= F(enclosure[1])
        lo,hi = module.absolute_bounds(enclosure)
        decision = abs(exact[index]) > threshold
        assert (F(lo)>threshold if decision else F(hi)<=threshold)
        comparisons += 1
        if decision:
            exceeding.append((slot,component))
        if abs(exact[index]) > largest[0]:
            largest = (abs(exact[index]),slot,component)
        print(f'slot={slot} component={component} exact_center_error={display(exact[index])} '
              f'ratio={display(abs(exact[index])/threshold)} interval_contains_center=YES '
              f'status={"EXCEEDS" if decision else "BELOW"}')
assert residual_max < F(1,10**100)
assert readout_max < F(1,10**100)
print(f'anchors={len(anchors)} exact_integer_recurrence_components={comparisons}')
print(f'exceeding_components={exceeding}')
print(f'largest_anchor_slot={largest[1]} component={largest[2]}')
print(f'exact_center_required_cancellation={display(largest[0]-threshold)}')
print(f'maximum_exact_printed_ABC_residual={display(residual_max)}')
print('RESULT=PASS; no observation-accuracy or ciphertext-chain certification')
```

实际标准输出：

```text
slot=0 component=real exact_center_error=8.82698788412665017301662E-27 ratio=1.06711735625462119385028E-2 interval_contains_center=YES status=BELOW
slot=0 component=imag exact_center_error=2.70840052450442799718872E-25 ratio=3.27425532393120716477784E-1 interval_contains_center=YES status=BELOW
slot=1 component=real exact_center_error=-6.94416792520149884607294E-25 ratio=8.39498390051584093340398E-1 interval_contains_center=YES status=BELOW
slot=1 component=imag exact_center_error=-7.05526046252440221490977E-25 ratio=8.52928653725200067056370E-1 interval_contains_center=YES status=BELOW
slot=256 component=real exact_center_error=-6.56887035170718994054169E-27 ratio=7.94127697387985201007958E-3 interval_contains_center=YES status=BELOW
slot=256 component=imag exact_center_error=-2.36332564302873040198802E-25 ratio=2.85708539001477843129026E-1 interval_contains_center=YES status=BELOW
slot=257 component=real exact_center_error=-1.71086100710270276057422E-25 ratio=2.06830404525834484077326E-1 interval_contains_center=YES status=BELOW
slot=257 component=imag exact_center_error=-5.84456482501507588252625E-26 ratio=7.06564532137218235354596E-2 interval_contains_center=YES status=BELOW
slot=512 component=real exact_center_error=3.37031424049790731702872E-25 ratio=4.07445990555278903116274E-1 interval_contains_center=YES status=BELOW
slot=512 component=imag exact_center_error=1.09903854939649781464032E-24 ratio=1.32865607911723423288160E+0 interval_contains_center=YES status=EXCEEDS
slot=513 component=real exact_center_error=-4.29310177096118474565223E-25 ratio=5.19004157714826628426343E-1 interval_contains_center=YES status=BELOW
slot=513 component=imag exact_center_error=3.91355171419836120535744E-26 ratio=4.73119371369149080891225E-2 interval_contains_center=YES status=BELOW
slot=768 component=real exact_center_error=-2.33263319482653412055103E-25 ratio=2.81998049691595874004706E-1 interval_contains_center=YES status=BELOW
slot=768 component=imag exact_center_error=-1.80601436134520791291859E-25 ratio=2.18333739202504653427149E-1 interval_contains_center=YES status=BELOW
slot=769 component=real exact_center_error=-1.23705256187948193549070E-24 ratio=1.49550478227652947333495E+0 interval_contains_center=YES status=EXCEEDS
slot=769 component=imag exact_center_error=3.00074314777302624044291E-25 ratio=3.62767586937448805825689E-1 interval_contains_center=YES status=BELOW
slot=1023 component=real exact_center_error=2.08324198737765211086267E-24 ratio=2.51848502704613704383691E+0 interval_contains_center=YES status=EXCEEDS
slot=1023 component=imag exact_center_error=-7.27384500176740543945115E-25 ratio=8.79353903051143441877048E-1 interval_contains_center=YES status=BELOW
slot=16383 component=real exact_center_error=5.73812214359002760419156E-25 ratio=6.93696401548842699910786E-1 interval_contains_center=YES status=BELOW
slot=16383 component=imag exact_center_error=-5.85610767276337141254612E-25 ratio=7.07959976804697740401456E-1 interval_contains_center=YES status=BELOW
anchors=10 exact_integer_recurrence_components=20
exceeding_components=[(512, 'imag'), (769, 'real'), (1023, 'real')]
largest_anchor_slot=1023 component=real
exact_center_required_cancellation=1.25606137482462443599126E-24
maximum_exact_printed_ABC_residual=3.20437849809351507366057E-154
RESULT=PASS; no observation-accuracy or ciphertext-chain certification
```

### C.1 本轮脚本与原始输出摘要

这些是工作区记录的哈希，代码或输出均已逐字嵌入上文；不是声称 ZIP 另外含有这些文件。字节数以对应 UTF-8 文件计，包含末尾换行。

| 记录 | 字节数 | SHA-256 |
|---|---:|---|
| `verify_input.py` | 2030 | `f792a50ea13907ab6d050ad50f9f343dae161ec743bcbe6d407f390b75c557b7` |
| `check_receipts.py` | 5533 | `341ba651f9a773ccbd7d44f78bde09e2f655fe6e11da142f26092a43bef96aa7` |
| `independent_scalar_check.py` | 5165 | `5f694847986dfb0507be1afd392b9f2b24c2b455807d51b811365e268a00d9a6` |
| `verify_input.json` | 591 | `e0b68a2b2071680f9794c569079eb93f23399541990e2f9b88681c6660dddca4` |
| `check_receipts.stdout` | 1454 | `a3ef772963ffb69ed429857eae8d4a493124c9d62862307afe1d2927be8159cd` |
| `scalar_supplied.stdout` | 5460 | `9e3866b37765cd50509f563c54ed996e2b1c5e5f5b043086bf191f47d3e8208f` |
| `independent_scalar.stdout` | 3376 | `62355fbcb3785f2078b8762d86a0bb1a68501b7c8fcbda54e6ef1006299c1771` |

### C.2 交付前的只读复核

实际命令：

```sh
python -B -I /mnt/data/s100_condition_work/final_source_check.py
```

退出 0。实际输出：

```text
input_archive_identity=UNCHANGED
input_all_200_files_byte_identical_to_archive=PASS
embedded_executed_scripts_3_of_3=EXACT
embedded_saved_results_4_of_4=EXACT
markdown_fences_and_UTF8=PASS
RESULT=PASS; read-only byte/text checks only
```

该检查逐字节比较输入目录全部 200 个成员与原 ZIP，确认本轮未改动源码、证据或上一轮报告；同时核对上述代码／结果嵌入无改写。脚本如下：

```python
from pathlib import Path
import hashlib
import json
import zipfile
root=Path('/mnt/data/s100_condition_work')
archive=Path('/mnt/data/s100-condition-decision-2c14d7f.zip')
assert archive.stat().st_size==1620988
assert hashlib.sha256(archive.read_bytes()).hexdigest()=='3bbb738d56a17f76807546f5f7fc4a1f4c83e889164e1dab71f0409d190e417b'
with zipfile.ZipFile(archive) as z:
    names=set(z.namelist())
    assert z.testzip() is None
    actual={p.relative_to(root/'input').as_posix() for p in (root/'input').rglob('*') if p.is_file()}
    assert actual==names
    for name in names:
        assert (root/'input'/name).read_bytes()==z.read(name),name
md=(root/'deliverable/DECISION.md').read_text()
for name in ('verify_input.py','check_receipts.py','independent_scalar_check.py'):
    script=(root/name).read_text()
    assert '\n```python\n'+script.rstrip()+'\n```' in md,name
for name in ('verify_input.json','check_receipts.stdout','scalar_supplied.stdout','independent_scalar.stdout'):
    assert (root/name).read_text().rstrip() in md,name
for name in ('DECISION.md','NEXT_ACTION.md'):
    content=(root/'deliverable'/name).read_text()
    assert sum(line.startswith('```') for line in content.splitlines())%2==0
    assert '\ufffd' not in content
print('input_archive_identity=UNCHANGED')
print('input_all_200_files_byte_identical_to_archive=PASS')
print('embedded_executed_scripts_3_of_3=EXACT')
print('embedded_saved_results_4_of_4=EXACT')
print('markdown_fences_and_UTF8=PASS')
print('RESULT=PASS; read-only byte/text checks only')
```

交付前检查记录补充：最初打包后的再次复查曾退出 1，报 Markdown fence 计数 `AssertionError`。原因是检查器对文本中所有三个反引号子串计数，把附录脚本自身的字符串字面量也算作 fence；不是文档缺少结束代码块，也不影响标量结果。检查器已改为只数行首 fence 标记，以上嵌入的是修正后实际执行版本；最终复跑退出 0。原输入 200 个文件仍逐字节一致，交付 ZIP 重新生成并核验。
