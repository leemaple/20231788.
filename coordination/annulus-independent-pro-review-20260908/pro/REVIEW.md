# ANNULUS-INDEPENDENT-PRO-REVIEW-20260908
## 独立终审：接受有限端到端证据，不把它当作原压力条件修复

### 最终裁决

**接受 `s100-annulus125-e80-v1` 的这一条 Linux 实际样本，作为 S100 明确条件下、从原始高精度明文出发的有限端到端正确性证据。原 near-unit S100 的双平台 E80 FAIL 保持不变；S116 的双平台 PASS 作为改变参数后的历史证据保留。完整表3同源复现与部署安全均未完成。**

没有发现可据以启动“保留原 stress、原密码参数、公开 evaluator 语义”的生产算术修复的具体缺陷或机制。因此本次**不交付生产补丁，不建议新加密或盲抽样**。发现并验证的历史接收器缺陷在当前版已修复；纸面公式反例也不能伪装成生产 RED/GREEN。唯一后继决定见 `NEXT_ACTION.md`。

## 1. 独立性、身份、引用规则

首轮文件在 **2026-09-08 04:33:32.438385 UTC（新加坡时间12:33:32.438385）**冻结，SHA-256：

```text
eaea61c388cadb419097917d16ac93edf7abbce2833a87bf03e7389b9ef444e5
```

冻结后才读取 `after-first-pass/` 正文，随后实施历史/当前接收器对照。`FIRST_PASS.freeze.json` 绑定首轮文件、当时脚本和结果；这只是可核验的本地顺序记录，不冒充第三方时间戳认证。整个源包哈希验证会读取所有文件的原始字节，但不预读作者报告语义。任务描述、源码注释和允许先读的历史材料当然已经可见，不能宣称完全盲化。

输入 `annulus-independent-03f37b6.zip` 为13,743,164 B，SHA-256 `c30b6e84a53712792cfda4ec17da7b434ee745aebad3949916a90fa6d528a0b1`；310常规成员，309自排除载荷。输入 MANIFEST SHA-256 `6cbd05178b788ead26ea12c1032c6fa8f22cad77e65488886a8d1b8f74fbbcdc`。CRC、全部大小和SHA-256、解包字节一致性通过；170个git blob内容散列相符。结果见 `results/packet_and_process.json`。

|身份|绑定|
|---|---|
|实际实验源码|`03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b`|
|本次task/evidence commit|`695a951a7379d355cf9d167cd14c9a17d9d25199`|
|science compile-controls base|`def248a04b7212088e41a72e2239496dcd3e7027`；同字节关系由所给workflow检查与日志支持，不假装拥有完整git历史|
|官方OpenFHE1.5.0|`df495ba2e91739a6dc8f1de254fc5a41155ce504`，native64/backend4|
|实测run/job|34184869227 / 101931070901，Linux，attempt1|

下文 `src/`、`tests/`、`.github/` 均指输入包 `project/` 下路径；`NEW/` 指 `project/coordination/s100-annulus125-20260908/`；`AUTHOR/` 指 `after-first-pass/coordination/s100-annulus125-20260908/`。论文页码是所给PDF物理页，PDF SHA-256 `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`。本报告中的源码行号按原文件1起始；槽号按TSV从0计数。结果JSON是本次实算，不是引用作者评价。

## 2. 到底验证了哪一个数学问题

令 \(x_s\) 为原始 exact dyadic 输入，\(w_{0,s}\) 为fresh密文按实际尺度解码的观测值，\(w_{8,s}\) 为八次平方后的观测值：

\[
E_{0,s}=w_{0,s}-x_s,\quad
I_{8,s}=w_{0,s}^{256}-x_s^{256},\quad
A_{8,s}=w_{8,s}-w_{0,s}^{256},\quad
E_{8,s}=I_{8,s}+A_{8,s}.
\]

论文§2.2和§4.2的局部乘法分析主要比较输入密文的解密量与输出解密量，不直接许诺任意初始加密噪声经过八平方后都满足原始输入E80。此次测试确实另做了更完整的原始输入端点检查，而不是把 \(w_0^{256}\) 偷换成最终真值。`tests/s100_annulus125_eight_square_test.cpp:285–310` 对原始 \(x^{256}\) 写出两种E8，并把A8单列。

门槛 \(T=2^{-80}\)。科学误差口径为
\(\max_s\sqrt{\operatorname{Re}(e_s)^2+\operatorname{Im}(e_s)^2}\)。它与旧测试的实/虚分量最大值不同；observer的一致性检查又采用component-distance，不能随意改成另一门槛。

**证据上限**：原始TSV是有限精度观测，并未包含全部整数phase或密文。以下端到端结论接受源码/执行链和条件性observer可信前提，不据此宣布数学真值、FFT误差、所有整数提升或安全性已经形式化证明。

## 3. 论文算法、实际尺度与公开数据流

### 3.1 运算对应

记高低解密量为 \(H,L\)，重组量为 \(m=dH+L\)。忽略暂时单列的重线性化/舍入误差，Tensor2保留

\[
U=dH_1H_2+H_1L_2+L_1H_2
 =\frac{m_1m_2-L_1L_2}{d}.
\]

Relin2尽量保持U，而RS2再除实际素数q，故物理尺度必须递推为

\[
S_{k+1}=\frac{S_k^2}{d q_{k+1}},\qquad
S_k=\frac{S_0^{2^k}}{\prod_{j=1}^{k}(dq_j)^{2^{k-j}}}.
\]

|环节|论文定位|当前源码及独立判断|
|---|---|---|
|DCP/RCB|§3；§4.1|`double_ckks.cpp:398–437,1227–1243`：丢d塔并形成高部，低部为原prefix−dH；重组为dH+L。不是读出明文后拆位。|
|Tensor2|p7 Def4.1/Lemma4.2|同文件826–891：HH以及HL+LH，LL有意省略；使用正确正交叉项。|
|Relin2|p7 Def4.3|同文件894–1078，关键1011–1056：高部先乘d、升入dQ、Relin再DCP；低部单独Relin后合并。不把新增高部Relin噪声简单乘d。|
|RS2|p8 Def4.5/Lemma4.6|同文件1080–1211：H'=RSq(H)，L'=RSq(dH+L)−dH'；相关舍入，非各自独立缩放H/L。|
|Mult2与reentry|p8 Def4.7|同文件1213–1224；`repeated_mult2.cpp:509–537`：组合运算与系数不变的上下文转接，不是再次DCP或秘密刷新。|
|实际尺度|p8 §4.2末段|`repeated_mult2.cpp:279–310` 与TSV九个有理尺度一致；S8/S0≈1.000364850389604。|
|终点读出|§2.1编码/解码语义|`high_precision_client_io.cpp:703–755`：官方Poly解密、exact CRT/居中与精确尺度解码，而非machine double槽解码。|

官方固定引用 `references/official-full/src/core/include/lattice/hal/default/dcrtpoly-impl.h:693–711` 与 `.../scheme/ckksrns/ckksrns-leveledshe.cpp:172–190` 支持实际塔上的rescale；`.../schemebase/base-leveledshe.cpp:620–659` 明确正交叉项。物理尺度与兼容OpenFHE的double元数据不是同一个量。若把终点强行按2^100而非实际S8解码，会引入约3.65×10^-4的相对系统偏差；当前实现不是这个错误。

### 3.2 私钥、oracle、fresh误差是否流入Evaluator

`tests/s100_annulus125_eight_square_test.cpp:161–185` 的Evaluate只有公开plan和cipher输入；258–264一份root setup、一次public payload Encrypt，完成DCP及八次Mult2；266之后才解密和观察，之后才形成E0/I8/A8/E8。未发现低噪声key筛选、提前用fresh误差决定换样本、秘密补偿或答案回流。Family投影生成的公开eval keys不等于多条独立payload实验。

`paper_h128_client_keypair.cpp:193–217` 使用官方h128 ternary生成器以及官方EncryptZeroCore构造公钥；`high_precision_client_io.cpp:606–635` 的payload仍走public-key Encrypt。固定官方 `rns-pke.cpp:148–196` 的非GAUSSIAN分支令临时v为dense ternary，不能把secret的h128误认为v也稀疏。h128符号采样内部的近均衡拒绝采样见官方 `ternaryuniformgenerator-impl.h:103–143`，它不等于根据实际误差挑密钥。

这确认的是客户端与Evaluator之间的API/调用数据流。二者在同一测试进程中运行，客户端保留私钥是必要事实；未将此说成操作系统级隔离或恶意服务安全证明。

## 4. 新样本：原始字节、进程和全槽重算

### 4.1 记录闭合

`NEW/experiment-evidence/sample/raw.tsv`：12,652,749 B，SHA-256：

```text
b0d17e0c2c819b1b59921fd7f8020a5265ae6c70e3b5fa87152a63abfbe4b5fd
```

raw第23行为列头，第24–16407行为16384槽数据，第16408–16412行为max声明，第16413–16418行为门禁和终态。**先从行数据重算，后比较声明**，不从声明制造答案。

五件start/end/raw/stdout/stderr的哈希、15件experiment inventory和9件compile inventory全部一致。真实end是exit0、timed_out=false，stdout完整PASS、stderr为空。原始绝对argv和output路径未经重写。03:51:41.304409–03:52:15.157288 UTC相差 **33.852879秒**；这包含控制、编码、密钥、运算、观察和写出，不是纯乘法benchmark。二进制本体不在包内，只有所给runner记录的摘要。

### 4.2 真正独立于作者replay的算法

`checks/independent_scalar.py` 不导入任何项目代码，使用标准库Decimal/Fraction，180/230**十进制有效位**各读同一真实TSV一遍。全部域/尺度/门禁/argmax逐槽计算。I8使用差幂因式分解，而作者采用误差递推；又与独立算两个端点的八平方相减核对。

max/argmax与230位远端接收结果一致；本地180/230最大值差约10^-203或更小。那是同一文件的重复解释，**不是第二次加密**；增加小数精度也不是数值证明的替代。`results/independent_180.json`、`independent_230.json`保留完整数字、runner-up差距、门槛差距及不能从TSV恢复的项目。

|量|最大复数模|0起始槽号|门槛|
|---|---:|---:|---|
|E0_obs|3.270821691859982×10^-25|56|≤T，通过|
|E8_obs|1.462314103884337×10^-25|56|≤T，通过|
|E8_prod|1.462314103884337×10^-25|56|≤T，通过|
|I8_obs|1.462318467043391×10^-25|56|诊断传播量，不单列PASS门槛|
|A8_obs|3.162058289835670×10^-27|2527|≤T/4，通过|

E8/T≈0.176782927657；A8/T≈0.003822693910。所有科学门禁的超限槽数都是0。producer与observer终点component-distance的全槽最大值约9.9999477511×10^-129；fresh producer列并未序列化，因此不能用TSV独立重放fresh这一内部交叉检查。

### 4.3 输入不是“零输出投机”，但也不是整个环带的证明

独立整数验证全部输入互异、四相位各4096、每槽满足 \(|x|<125/128\) 与 \(|x^{256}|>2^{-10}\)。实际半径0.9753575445–0.9756172183，输出模0.0016824610–0.0018011125。补充精确检查16384个输入模平方均互异，因此理想输出的模也全互异（正数128次幂严格递增）。

slot0/1原始输入微差精确为2^-75，理想输出差约15.0022T>4T；生产输出差与理想差的误差约3.93076×10^-26≤2T。该witness通过，说明不能随意将小差异抹成同一输出。

单条密文的16384槽共用本次密钥/噪声结构，**不是16384个独立随机试验**，不能据此估算跨密钥成功率，不能声称整个annulus或任意输入保证。

### 4.4 绝对精度与相对精度

\[
b_{abs}=-\log_2\max_s|E_{8,s}|\approx82.49995,
\]
\[
b_{rel}=-\log_2\max_s\frac{|E_{8,s}|}{|x_s^{256}|}\approx73.30263.
\]

相对误差与误差最大值的比较必须同槽做比值，不能任取两个max相除。本次两个最坏槽恰为56，但方法不能依赖这个巧合。不能写成“实现了82.5位相对精度”或“已经普遍保留80位有效信息”。

## 5. 原S100、S116、fresh与当前controls各归各位

### 5.1 旧S100两平台完整原始数据仍FAIL

来源为 `evidence/coordination/fs-endpoint-live-run-01/{linux,windows}/`。run34039088536、源码ed5fd192a89d6d4728ad295e87cf06a3f4abc832。实际状态记录CTest exit8；本次未重跑。两个TSV各16384行分别以180/230十进制位重算；原component范数门禁没有被改写，下面另报更强复数模口径。

|平台|E0最大复数模|E8最大复数模／槽|I8最大复数模|A8最大复数模／槽|
|---|---:|---|---:|---|
|Linux|3.6155741678e-25|9.6203727237e-24 / 11656|9.6226221104e-24|4.9199615432e-26 / 11143|
|Windows|3.3860027717e-25|9.0998222806e-24 / 5091|9.0998972912e-24|8.3532257685e-26 / 15050|

原component最大E8分别9.14647363365e-24、9.06530517409e-24；这与复数模结果不同，不是数据冲突。原日志9/7是数值断言miss数，不是失败槽数。本次复数模E8超T的槽数分别10465、10286；component口径分别9686、9510。详细输出见 `results/old_s100_230.json`。

两例E0均≤T，A8也已小于T/4；I8却分别11.6330T和11.0011T。令s*为I8最大槽，

\[
|E_{8,s_*}|\ge |I_{8,s_*}|-\max_s|A_{8,s}|>T.
\]

因此该两例的**观测新增残差没有足够幅度抵消继承误差**；即使把A8理想化为0仍FAIL。这没有证明任何合法新算法都无效，也没有排除未来有明确机制的改进，但足以否定“现在随便提高后八步精度就能修好原样本”的推断。

旧输入半径约0.991，\(256|x|^{255}\)≈25.4–27.2；新输入对应约0.4416–0.4726。且对非零x，幂函数的局部相对条件数 \(|xf'(x)/f(x)|=256\)，并未因缩小半径而变成1。旧例相对位数约73.18、73.28，新例约73.30，正好提醒我们不要将约6位的**绝对**误差改善误当同等相对信息改善。

**因果限定**：新样本用了新密钥和新噪声，不是同密钥同噪声的配对半径试验。上述导数机制及逐槽误差分解有数学/数据支持；不能把新旧实测差額全量估计成纯半径的因果效应。作者最新RESULT第9行已经修正“沿用噪声”为沿用噪声参数/采样机制，保留这一限定。

### 5.2 S116是有限正证据，不是S100的改名成功

run34055816234、源码2b8b349edf5575556347082c1b725f6696c743b6的S116/d56/Base58/QP712双平台PASS见 `evidence/coordination/precision116-eight-square-return-01/ROOT_RUN_RECEIPT.json` 与 `RUN_34055816234_STATUS.json`。收据报告端点component误差2.5905123324714e-26、3.4805603371614e-26。即乘√2作复数模保守上界，约0.0443T、0.0595T仍低于T。

本包没有S116完整槽TSV，故这是**核对既有记录并作范数上界推导**，不是本次独立全槽重放，不编造S116最坏复数槽。它支持改变参数后的完整算法链有限成功，不能提供原S100/Table3参数成功，也不认证712bit模数链的部署安全。

### 5.3 fresh诊断与控制修复不冒充端点通过

fresh run34110943783是单个Linux公钥起点诊断；`remote-green2-diagnostic-steps.log` 第94–121行显示编码误差约1.30e-28、公钥聚合项约3.37e-25、读出差约1e-128。这里只接收原日志的单样本诊断范围，没有本次重算全fresh多项式，也没有把20个预选分量当完整全槽端点。

最新run34116227668/source223667e的 `evidence/coordination/s100-output-finalization-01/GREEN_EVIDENCE.txt:18–84` 明确是Linux默认60回归及一次keyless control，原S100/S116/fresh入口skipped。源码 `s100_fresh_error_diagnostic_test.cpp:57–60,251–267,721–724` 检查最终flush失败；其完成不改变旧E80。

作者关于固定Gaussian inversion成功返回支持[-39,39]的窄事实，经本次按官方固定commit补读确认；由此可给 \(39(N+1+h)=1,282,983\) 的fresh聚合系数粗界。**只有再有实际编码整数m的界时，才可作fresh无绕模推导。** 本次annulus没有序列化/取得该编码m的证书，不能继承历史fresh的m≤2^164，更不能推广到八步HYBRID全链。补充来源与有界标量结果见 `EXTERNAL_REFERENCE_NOTES.md`、`results/post_author_scalar.json`。

## 6. 已执行的反例与证据边界挑战

### 6.1 印刷Thm4.8不能用来改坏尺度

PDF p8 Thm4.8写1/q目标，与p7 Def4.1/Lemma4.2及p8末段的dq尺度消费相矛盾。本次使用不同于作者报告数值的精确标量例：N2,h1,d13,q101,Q=101×1009,H10,L0，并取理想精确Relin情形；没有生成密钥或密文。

大小前提33801<101909/2。Tensor后H=100,L=0；RS后H'=1,L'=0，RCB=13。印刷目标16900/101，误差15587/101>界102/101；定义支持的目标1300/101，误差13/101≤界。`literal_paper_red.log` 真实exit1，意为**印刷归一化式的RED**，不是生产旧源失败。当前S'=S²/(dq)正确，不能据此强交修复补丁。

PDF p5普通Tensor负交叉项也与(1,s)约定冲突：取全部标量1，书面式给0，应为4。官方和候选均用正确正号。

PDF p8 Lemma4.4“舍入加法差总为(0,e)”这一中间断言过强。P7、rlk=(3,4)、s1，两次c2=1与一次c2=2的舍入差(-1,1)已经反驳该中间向量式。**不单独推出最终ERelin+h必假**，也不认为OpenFHE HYBRID ApproxModDown天然严格线性。完整理论界仍需独立证明，局部执行特定残差不是普遍证明。

### 6.2 防止错误统计或整数解释

已执行独立纯标量测试：误差(.75T,.75T)的分量门禁通过但复数模失败；E=1、I=−1时maxE−maxI=0却A=2；55模101居中为−46仍有正headroom9，却已改变整数提升。这些说明正确范数、逐槽残差和无绕模证明各有独立职责。

另外穷举1105个小模数DCP坐标和48841个相关RS坐标恒等式。它们确认对应标量恒等式，不是C++/FFT执行或实际大参数证明。

还以精确Fraction核对环带的条件预算。令r=125/128，差幂因式分解给

\[
|I_8|\le 256(r+T)^{255}|E_0|,\qquad
256(r+T)^{255}+1/4<171/200=0.855.
\]

因此在|x|<r、|E0|≤T且同槽|A8|≤T/4的前提下，|E8|<0.855T。这解释了输入域选择的预算，不保证其他随机样本也满足E0/A8前提，更不是观察器准确性证明；E8门禁仍按原始x直接计算，而不以该条件推论替代数据。

### 6.3 真正复验了历史接收器的修复

读完作者处置后，`checks/receiver_regression.py` 对历史不可变receiver和当前receiver执行同一组**内存合成变体**。真实raw保持逐字节不变；真实raw正例在两版都PASS。

第一变体只把slot1024的producer误差改为(4e-25,0)并同步修改其max声明；其他科学门槛及witness仍可通过，但与observer的一致性矛盾。第二、三变体为缺末LF与CRLF。结果：历史版三例均误报PASS，真实exit1记录预期RED；当前版第一例在`terminal producer/observer disagreement`处拒绝，后两例在framing处拒绝，exit0为GREEN。

源码差异定位：当前 `NEW/replay_annulus125.py:43–46,84–112`；历史 `after-first-pass/coordination/comprehensive-reassessment-20260908/pro/checks/replay_annulus125.py:43–103`。这是可复核的**接收器既有修复**，非乘法修复、非新增真实实验；结果和两版原始SHA见 `receiver_historical_red.json`、`receiver_current_green.json`。

### 6.4 一次性与绝对路径挑战

实际纯函数 `check_one_shot_gate.py:event_allowed` 对首次create/attempt1通过、attempt2拒绝；无历史状态的同一函数不能区分“从未出现的tag”与“删除后重新create的新run”。纯合成事件反例通过，未调用GitHub。这与作者 `ONE_SHOT_CI.md:28–32` 的stateless限定一致，不是发现实际重建过tag。

另将五份**未经修改**的远端记录复制到临时本地目录，当前finalizer如预期拒绝 `wrong process entry/output`，未产生verification。不修改原始绝对路径来骗过门禁。新真实记录的接收通过独立路径/字段核对与标量replay完成，不谎称本地finalizer成功接收了迁移后的目录。

## 7. 作者材料对照和最终finding处置

严重性指它对**所争议的科学声明**的影响，不等于发现运行中的安全漏洞。fixed只表示这里描述的具体历史缺口已关闭，不扩大为完整算法已证明。

|ID／严重性|发现或待检验命题|终态|证据、处置|
|---|---|---|---|
|F01 高|原S100已被新PASS修好|unsupported|§5原始两平台重放仍FAIL；AUTHOR/RESULT:5,39,45也明确否认替代。|
|F02 高|终点producer/observer矛盾记录可能假PASS|fixed|§6.3本次历史RED/当前GREEN；当前replay:84–112；作者RETURN_DISPOSITION:65–85旧pending被后继EXECUTION_CONTRACT:40–44及实际修复覆盖。|
|F03 中|缺末LF/CRLF仍可当完整原始TSV|fixed|§6.3当前framing正确拒绝，EXECUTION_CONTRACT:45–51与代码相符。|
|F04 中|最终输出刷新失败仍返回成功|fixed|fresh源57–60,251–267,721–724；run34116227668控制日志。仅核对保留C++执行，不冒称本次运行。|
|F05 中|共享非锚点错槽序不能被旧控制捕获|fixed|当前全槽单项式X、swap2/3负控制；新测试79–88及旧controls实际日志。修复这个盲点不等于形式化FFT证明。|
|F06 高|因印刷式缺d或Tensor负号应改生产运算|unsupported|§3/§6.1和独立精确RED表明定义与当前实现一致，不能按矛盾印刷式改坏代码。纸面完整勘误/证明仍非本轮完成。|
|F07 中|Lemma4.4中间(0,e)进位式可直接作普遍证明|pending|精确(-1,1)反例；主定理误差如何重新严格覆盖两坐标和HYBRID尚非本轮证明。|
|F08 高|observer跨精度相符就是正式误差/全链整数提升证明|unsupported|§3.1、§5.3、§6.2；保留CONDITIONAL_OBSERVER_NOT_FORMAL。并非否定有限端点证据。|
|F09 中|所有receipt、fresh producer、512位/Horner细节可从TSV恢复|accepted-limitation|相关列未序列化；SOURCE-bound运行断言，AUTHOR/INDEPENDENT_RESULT_REVIEW:92–107已准确限定。|
|F10 中|stateless tag gate证明全历史永远只有一次|accepted-limitation|§6.4真实predicate合成反例；ONE_SHOT_CI:28–32/独立作者复核109–114已承认历史需额外记录。不指控实际重建。|
|F11 中|源码pin/hash足以独立证明缓存二进制来源和runner身份|accepted-limitation|cache hit，包中无二进制本体/完整链接闭包/远端认证；作者独立复核84–90也保留条件。|
|F12 高|单条16384槽PASS支持全环带/所有key/成功率|unsupported|实验统计单位为一条链；域预算是条件蕴含，不保证所有加密前提。|
|F13 高|论文表3的输入/采样/统计/性能已同源复现|pending|PDF p13、作者RESULT:39–43、所给历史provenance只确定缺口，不补成答案。不是默认启动外联或1000次。|
|F14 高|当前参数已达到部署安全|pending|security=UNRESOLVED，HEStd_NotSet；没有独立安全估计/认证。|
|F15 中|新旧差额可全量归因于只改变输入半径|accepted-limitation|新key/noise不是配对样本；机制由导数和逐槽分解支持，精确因果效应未测。作者最新RESULT:9已澄清新随机实现。|
|F16 低|这个精简包能原样重跑全部compile-CI检查|accepted-limitation|compile workflow:85引用`coordination/comprehensive-reassessment-20260908/check_receiver_agreement.py`，该脚本未打包。本次自行构造同机制RED/GREEN，未声称整套CI本地可运行。|
|F17 低|当前receiver文首UNEXECUTED是当前运行状态|pending|它是遗留注释，实际运行身份/原始记录已明确；不影响本样本接收，不拿文案清理冒充生产精度修复。|

作者报告没有推翻首轮主判断。后读材料新增可闭合的事实是历史receiver修复链与已承认的stateless/provenance限定；本次又用独立变体真实复验，不以作者或多个模型同意替代证据。历史文件中的“待运行”“尚未GREEN”按其checkpoint解释，不当作新的执行指令，也不掩盖后续已完成记录。

## 8. 四层“完成/未完成/原因”与唯一决定

|层次|当前可交付|仍未完成及原因|
|---|---|---|
|算法有限端到端证据|**完成本次有界接收**：一条S100 annulus125、全槽、公开PKE、八平方、原始x真值的条件性PASS；S116另有历史正证据|不是任意key/input/circuit正确性，不是所有整数提升/数值模型的正式证明|
|论文具体参数与输入|S100的N/h/名义尺度和模数位长布局对齐；实际素数/roots/噪声参数在项目内冻结|原near-unit stress仍FAIL；base999不能替代全目标；原表3准确输入生成、HEaaN/API/采样细节未绑定|
|表3统计/性能同源复现|原论文参数表与当前实测被清楚分开|1000次平均误差的同源生成及聚合顺序、179ms、350 NTT、5.08MB/30.6MB未同口径复现；本次总进程时间不能替代|
|部署安全|记录UNRESOLVED且不掩饰|参数/采样/公开eval-key family及实现面的独立安全保证未给出，数值通过不能认证安全|

论文在p13给的是1000次执行的平均误差叙述，不是每次都E80的普遍承诺；不知道生成和聚合口径时，不能把本项目两例失败解释成证伪论文，也不能把一例更容易域的通过解释成完全复现。缺原始同源数据是**声明范围的边界**，不是要求无限准备或等待作者的任务。

**唯一具体决定：保留生产源码，不启动无已定位缺陷的修复或新加密；将本次有界PASS与原FAIL、S116及四层未完成事项分别冻结归档。** 此报告与随包可重放结果已完成这一审查处置。后续只有出现新的源码反例或版本绑定机制证据，才有理由另开工程修复；当前材料没有这样的候选。没有用输入缩小、S116、低噪声筛选、改sigma/v、secret payload或秘密补偿包装原stress修复。
