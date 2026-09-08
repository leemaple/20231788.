# FINDINGS — 矛盾、限定风险与待验证假设

**基线固定；无生产/测试补丁；无新FHE运行；无伪造RED/GREEN。** 本轮身份是参考文档作者，不是自身文档的独立审查者。以下状态区分：新识别的条件性源码问题、已有论文问题的独立再核对、正常的误差/测量边界。优先级只表示文档复核关注度，不是漏洞分级，也不自动派发实验。

## 总表

| ID | 事项 | 本轮地位 | 可以下的结论 | 不能下的结论 |
| --- | --- | --- | --- | --- |
| F01 | HYBRID/BV OpenMP private DGG不是context DGG副本 | **新识别；源码+标准条件推导；运行分支待证** | 有效private分支默认σ1，与外部contextσ3.19不同 | 已证明历史使用σ1；已证明S100 FAIL根因；应立刻改代码 |
| F02 | native NTT缓存未把root作为键 | **新识别的条件复用风险** | 同q/N换root需重建表/EV依赖；S116三冻结根非canonical-minimum | 当前S100/S116已发生缓存污染或算法失败 |
| F03 | h128含符号平衡与内嵌BUG；v仍稠密TUG | **源码事实澄清** | h128不完整描述秘密分布；binary sampler也在根依赖闭包 | v是h128；论文HEaaN必然使用相同采样规则 |
| F04 | 论文定理4.8归一化缺d | **历史已知，本轮原页/纯整数再核对** | 该显示公式与定义/scale不一致，有标量反例 | 新发现的生产RED；整个方案已被推翻；项目应按错误式修改 |
| F05 | 论文普通Tensor交叉项符号 | **历史已知，原PDF与实现对照** | 在文中(1,s)约定下负交叉项式不自洽；项目用正号 | 当前Tensor2因此有负号bug |
| F06 | 论文Relin近可加性的中间等式过强 | **历史已知，限定待证** | rounding不能一般只产生第二坐标误差；需更精确carry界 | 已经反驳最终所有误差界或证明现有HYBRID正确性 |
| F07 | 当前源码注释与后来S116实测时间线 | **文档漂移，不是算法缺陷** | 早期注释“尚未E80测试”不能覆盖后来的接受记录 | 注释优先于真实后继结果 |
| F08 | fresh误差、输入条件数、observer口径 | **正常数学/证据界，不是新bug** | 传播主导不等于PKE错误；E/A/component/complex必须区分 | annulus修好原S100；S116就是Table3；小A自动E80通过 |

## F01 — OpenMP 私有局部高斯采样器的σ来源

**位置。** `keyswitch-hybrid.cpp:92–102` 外部先复制context DGG，98行声明 `private(dug,dgg)`，102行用这个局部dgg构造e。`discretegaussiangenerator.h:83–104` 构造器默认参数1.0；impl52–66构造调用SetStd，75–88建立Peikert表；DCRT构造器126–150直接GenerateIntVector。BV的对应KeySwitchGenInternal也有private(dgg)写法。[R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r19) [R10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r10) [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) [R12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r12)

**推理。** OpenMP private类对象按新局部对象构造，不是firstprivate复制context对象。若该pragma有效，循环私有DGG由默认构造得到σ1；若pragma忽略，body引用循环外context副本，σ为float3.19f扩成double。请求一个OMP线程仍不改变private初始化语义。源码中 `IsInitialized()` 的σ>1检查没有在这里修复默认值。标准依据为本轮查阅的 [OpenMP 5.0 private initialization](https://www.openmp.org/spec-html/5.0/openmpsu105.html) / [OpenMP 5.2 firstprivate](https://www.openmp.org/spec-html/5.2/openmpsu38.html)。

**影响。** evalkey中的Gaussian误差分布及其安全参数解释可能与记录contextσ的profile不同；σ1对应表长13，context3.19f对应39。这不是“所有PKE噪声都是1”的结论：root epk和payload e0/e1的消费者在另一函数中，不能跨分支套用。评估误差大小、随机流消耗以及理论安全模型都可能受影响。更小evalkey噪声也不自动解释原S100的fresh误差主导。

**证据边界。** 所供工作流请求OpenMP ON并允许缓存命中，不能代替历史库二进制及实际宏的鉴证；本轮没有编译、instrument、采样、重跑，也没有记录任何隐藏种子。故本条是“固定源码+标准的条件性矛盾”，不是已复现的生产bug/漏洞或数值RED。改变为某种另一写法不是本轮授权的修复方案。

**最小后继验证方向。** 根端先核对实际依赖库构建身份/宏，再选择对局部DGG构造来源的受限检查；仅读取context.GetDGG().GetStd()不具区分力。此方向只有在总文档被复核后才可选定，本轮未提交候选源码或CI。[B05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b05) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r10)

## F02 — NTT根身份与缓存键不完全一致

**位置。** `transformnat-impl.h:714–755` 中PreCompute以modulus查表，再检查表长度是否等于ringDim；没有把传入root与已缓存root比较。正/逆NTT调用按modulus取预计算向量。`Reset`负责清表；context的全局ClearStaticMapsAndVectors也清这些表，但plan析构只清其owned evalkey tags，不是相同操作。[N04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n04) [U26](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u26) [P04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p04)

**本轮整数结果。** S100全部冻结根为最小有效根；S116的Base0、Base1、Div冻结根是有效阶根，但分别不是最小根。详见scalar JSON每prime的 `minimum_primitive_root` 与 `frozen_root_is_minimum`。上游RootOfUnity将候选归到最小根，所以“相同q重新调用RootOfUnity”不能恢复S116这三枚冻结root。[P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [U20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u20)

**精确风险条件。** 同一进程先按root A为相同q和N建立表，后又按不同有效root B请求表，且没有受控清理/重建，缓存可能继续使用A的表，而对象元数据写B。此时独立的元数据根阶检查会通过，但CF↔EV映射不一定对应B。风险源是共享状态与坐标身份，不是“非最小根数学无效”。

**为何不是当前失败证明。** S100与S116的三枚不同root本来就对应不同prime；本轮没有证据证明某个当前执行路径为**同q/N**先后建立了不同root表，也没有运行NTT。显式frozen factory本身可以始终使用同一合法非最小root。本条不产生新数值FAIL，也不能据此否定已有S116 PASS。

**最小后继验证方向。** 先核同一进程所有相同q/N的root来源及缓存生命周期；参数更改时把root纳入对象身份与重建范围。全局Reset在仍有live对象时也可能影响其他上下文，不能未经设计就加作修复。[P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [N04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n04) [U26](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u26)

## F03 — h128不等于“任意固定权重三元”，更不等于v128

**位置与事实。** custom root使用 `DCRTPoly(tug,paramsQ,EV,128)`。TUG的GenerateIntVector在随机不同索引上选择±1，符号由BUG生成；正项数不在h/2±1时清空并重做。因此对h128，正项数只可能63、64、65。普通上游KeyGen的SPARSE分支为192；public PKE在非GAUSSIAN枚举下调用的是h0的稠密TUG。[P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) [R07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r07) [R08](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r08) [R13](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r13) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14)

**影响。** 参数文档若只写“h=128”和“ternary noise”会遗漏安全/可重复性相关分布信息。BUG不是直接的binary-secret加密路径，但它被稀疏TUG内部消费，不能在依赖闭包中写成“完全未使用binary sampler”。本轮交付字典、调用图已相互对齐这些事实。

**不是缺陷声明。** 正负平衡可能是有意设计，本轮没有把它判为算法错误。论文HEaaN是否具有相同约束仍未知。小h的unsigned边界代码需在以后真的扩展h时另查，当前factory固定128且校验，不扩展结论到任意h。

## F04 — 论文定理4.8显示归一化缺少q_div

**原始来源。** 给定PDF第7–8页：Tensor²按Def4.1先产生除d的双项表示；RS²按Def4.5再除q_l。同页scale段落明确是Δ²/(d q_l)，但Theorem4.8显示式的目标只写除q_l。本轮查看原PDF页图确认不是TXT的NUL或抽取丢字造成。当前project递推也为Δ²/(d·Mult)。[A01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-a01) [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) [P17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p17) [P20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p20)

**历史地位。** 后读的包内独立REVIEW已经记录此问题，故本条是“本轮独立重建并再核对的已有论文矛盾”，不是首次发现，也不把历史作者意见当作本轮推导的替代。[H06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h06)

**本轮可复核纯整数例子。** 仅用常数多项式，取N=2、d=13、q_l=17、Q_l=17×12289=208913、h=1、s=1，输入pair高项ct=(17,0)，低项ct=(0,0)，假设无s²项需要转换产生的重线性化误差E_Relin=0。原RCB明文是221。按定义平方：Tensor高项289、低项0；Relin不改变这类无s²项；RS后高17、低0，RCB为221。

只除q_l的显示目标却是221²/17=2873；差2652。显示的非wrap前提在这个例子中成立：$2\cdot221^2+1=97683<Q_l/2=104456.5$；其界为 $(0+1)/17+(1+1)/2=18/17$，远小于差2652。若目标多除d则221²/(13×17)=221，与定义复合相同。该例的全部整数与条件在 `paper_theorem_4_8_scalar_normalization` JSON 中，由纯Fraction脚本执行。**这是显示数学式的代数一致性反例，不是任何生产加密样本或FHE测试RED。**

**影响与限度。** 论文映射必须以一致的定义/scale和注明的公式差异为依据；不能拿少d的式子指控project的精确递推多除一次。局部归一化式有误，也不等于已证明整个Mult²最终误差理论无效；需要独立处理正确目标下的误差界。

## F05 — 论文普通Tensor交叉项的符号约定不自洽

**位置。** 原PDF第5页普通Tensor定义，在文中ciphertext解密内积为(1,s)的约定下显示负交叉项；包内先前REVIEW174–190亦说明此问题。当前project Tensor2调用的上游乘法以及其高低交叉和采用正号。[A04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-a04) [H06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h06) [P17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p17) [U24](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u24)

**数学区分。** 对s=1、ct=(1,1)、ct′=(1,1)，应有解密值2×2=4。张量(1,+2,1)给4，写成(1,−2,1)给0；这是符号约定的直接代数检查，而非本轮新调用密码函数。若另有全套统一的负号key/ciphertext约定，可写出等价体系，但不能只在张量交叉项孤立换号。

本轮没有把论文这一显示问题归因于project代码，也不修改生产Tensor2。后续文档引用应显式说明所采用的ct=(c0,c1)、解密c0+c1s的正号约定。

## F06 — Relin近可加性不等于只在第二坐标出现carry

**位置。** 原PDF第8页Lemma4.4证明中，从“近似可加”写到一个只加(0,e)的向量等式；历史REVIEW已给出舍入carry的反例。上游HYBRID使用ApproxSwitchCRTBasis/ApproxModDown，是有取整/近似成分的运算，不能仅凭名字“Relin”假定严格线性。[A05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-a05) [H06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h06) [R18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r18) [N02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n02)

例如对一个有分母P的最简rounding key-switch模型，P=7、key坐标(3,4)，系数1分别转换两次与系数2转换一次的差为(−1,+1)：第一坐标也有carry。这是对**该模型下过强中间等式**的说明，不是把OpenFHE完整HYBRID当成这个模型并宣布已有实际失败。本轮没有运行此key-switch模型代码，更没有FHE反例；历史记录与源代码足够支持“不应无条件使用该向量等式”的文档警示。

最终关于解密误差的界可能仍能通过更细的两个坐标/secret范数界成立。本轮不宣称已推翻全部最终上界，也没有补出未核准的完整新证明。所需后继是根端选定后明确具体实现的rounding/carry假设，再定位最小可证断言。

## F07 — S116早期注释与后继实际状态属于不同时间点

`repeated_mult2.cpp`候选profile附近的早期说明未把E80当成已测试，但包内后继precision116接受记录已经有Linux/Windows原输入完整数值PASS。早期注释可以解释设计背景，不能覆盖更晚实际结果；本轮不修改源码，只在图谱明确“候选profile名/早期注释”与“后继实测状态”区别。[P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [H02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h02)

这种漂移是文档维护问题，不是参数生成或求值算术的反例。S116后继PASS仍只归属于变参QP≈712，不得顺势覆盖S100。

## F08 — 没有可由历史结论自动推导的生产根因

原S100完成8平方而E80 FAIL，这是当前必须保留的科学状态。历史独立归因显示该样本fresh传播I8为主要贡献；另一个fresh诊断样本显示public聚合PKE项占主要部分。这些都是具体样本的误差事实，并不证明采样实现违背其定义。[H01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h01) [H05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h05)

annulus输入更靠内，$|256z^{255}|$较小，绝对误差下降有合理条件数解释；但它还换了key/noise，不能当作严格单变量配对。S116扩大Base/d/Δ，改变有效参数。二者分别“改输入PASS”和“改参数PASS”，都不是原S100压力样本修复。原输入仍在单位圆内，论文未公布其具体生成分布，不能给原输入贴不符合公布条件的标签。[H02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h02) [H03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h03) [H04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h04) [T01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t01) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09)

observer的PASS是带模型、范数、阈值与覆盖范围的断言；UNRESOLVED不等于通过，COMPLETE不等于E80通过，A8小不等于E8小，component bits不能直接当complex-modulus bits。两种高精度解码一致也不单独证明没有共享顺序错误；需要原输入oracle和独立控制。[T02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t02) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09) [T14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t14) [T17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t17) [H01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h01)

## 结论与唯一下一步

本轮完成的是源码事实与参数依赖图谱，发现了有价值的条件性矛盾，但**没有新增“已确认必须修改生产实现”的结论**；这不是限制未来发现新bug。没有发起实验、没有调参选样本、没有联系作者、没有提交补丁。

**根端复核文档覆盖与事实，再选择具体诊断。**
