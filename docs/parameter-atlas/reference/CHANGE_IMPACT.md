# 参数改动联动、对象重建与证据失效矩阵

> 根端勘误阅读版：精确函数名已修正；NTT 缓存结论限于表示/生命周期风险，不能据此认定已有算术失败。请与[根端复核说明](../REVIEW_NOTES.zh-CN.md)合读。`checks/` 保留原作者原稿自检记录，发布版复核另见 coordination 台账。

**只读设计参考；不授权改动任何实现/测试/CI。** 本轮所有最小检查均为“根端复核后可选的区分方向”，不是已执行或已经派发的任务。身份与精确常数以主图谱和 `PARAMETERS.json` 为准；S100/S116/annulus 的历史状态不得相互覆盖。

## 1. 先确定改的是哪一层

常见误区是把参数修改理解成“调用一个 Set 函数”。这里实际有五层：请求配置，factory手工覆盖/派生，已创建的预计算，已经绑定context/key/basis的对象，以及独立的oracle/门槛/历史证据。一个setter只改第一层或某个字段，不会自动使其余四层一致。

paper factory直接给定完整Q/roots并构造 `CryptoParametersCKKSRNS`，锁定HYBRID、FIXEDMANUAL、COMPLEX、h128等。普通CCParams默认值的修改可能**没有到达**paper factory；某些底层setter虽能写字段，但没有生成新的CRT/NTT表、key和receipt。修改前必须沿“设置→派生/验证→消费”三个位置分别核对，字典为每项保留了这些位置。[P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [U03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u03) [U04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u04) [U14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u14) [U15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u15) [P23](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p23)

## 2. 依赖图与重建层级

```text
[N, order, ordered primes, roots, native width]
                 ↓
[full Q / active Q / P / QP; α, partition; CRT/NTT precomputation]
                 ↓
[CryptoParameters + installed Scheme + context cache identity]
                 ↓
[root secret / public key] → [same-root family projection + tags + evalkeys]
                 ↓
[encoding/input + exact scale] → [fresh ct + H/L + stage receipts]
                 ↓
[terminal client lift/decode] → [oracle/observer/norm/gates/serialization]
                 ↓
[profile manifest + build/run identity + paper mapping + report]

σ / h / v / PKE overload → key & payload distributions and security assumptions
OpenMP / stdlib / PRNG → implementation branch + state/consumption order
input domain → m, true z^256, condition number, absolute/relative error interpretation
observer precision → ability to certify a result; not old ciphertext arithmetic
```

**“可数学复用”与“可直接复用C++对象”不同。** 例如只改P而不改Q，旧secret多项式在数学上仍有意义，旧payload也没有突然改变；但HYBRID预计算和评估密钥已失效，新context的指针、tag、缓存与验证要求也必须重新满足。不能凭这点把旧opaque对象塞进新plan，更不能把必要的受控投影写成“重新选择同一个随机样本”。[P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [P09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p09) [P14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p14) [P18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p18) [U09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u09) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17)

## 3. 修改联动矩阵

| 改动类别 | 直接影响的关系 | 必须同步的位置/派生量 | 旧对象与旧证据 | 最小区分检查方向（本轮未执行） | 源码/测试入口 |
| --- | --- | --- | --- | --- | --- |
| **N/order/logN** | 环维数、根阶、系数长度、槽嵌入、噪声卷积与安全估计 | factory、所有q≡1 mod2N、roots/NTT、batch/gap、全部长度/slot控件、h≤N、oracle锚点/DFT表、成本报告 | 原secret向量/PK/evalkeys/ct全不兼容；重新生成与新安全/性能证据；不能只改SetRingDim | 先整数核N=2^k/q同余/root阶，核所有维度，再选独立monomial槽序控制 | [P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [P24](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p24) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09) [T14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t14) [U07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u07) |
| **slots/batch/gap** | 复数投影与系数稀疏布局 | EncodingParams、ct.slots、binding、stride、input/oracle、anchor数及全槽声明 | Q/secret可在数学上相同；但当前paper全槽guard拒绝变更，旧ct解释不可任意重贴 | 公开X、共轭/相位、gap与正向bin直接对照；不只验max norm | [P23](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p23) [P24](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p24) [P25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p25) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09) [T14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t14) |
| **替换某个Base prime** | 初始/终端容量、完整CRT lift、root基身份 | exactQ/root、P避碰与QP、所有family、CRT逆元、安全估计、终端observer尺度与fixtures | 旧Q上的PK/ct/keys不能原样使用；终端数值证据失效 | 先精确模数/根/CRT逆元检查；S116独立fixture/证书方式定位 | [P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [T05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t05) [T06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t06) [U09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u09) |
| **替换某个Mult prime** | 特定轮真实除数以及后续每轮Δ递推 | drop顺序、d·m乘积、round表、所有受影响family/keys/receipts、oracle/manifest | 改动轮及后续scale/误差证据失效；“位数相同”不保持结果 | 纯Fraction递推/闭式与basis前后表；再区分RS舍入与测量scale | [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) [P20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p20) [T06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t06) [N01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n01) |
| **增加/删除一枚Mult / 改平方次数** | family数、链深度、最终指数2^r、密钥数量 | profile数组、loop/receipt ancestry、计划terminal条件、numPartQ、oracle指数、全轮tests和报告 | 原八步证明/门槛/时间标签不适用；已有前缀ct不是自动合法后继plan | 静态核每步基长度、terminal两Base和精确闭式；明确新任务不是8平方 | [P04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p04) [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) [P09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p09) [P21](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p21) [T04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t04) [T08](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t08) [T11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t11) |
| **改变d=q_div** | DCP余商、低项范围/carry、Tensor归一化、Relin零塔提升 | 每family末尾d、NTT/CRT逆元、Δ²/d、完整S和Q预算、same-root投影、fixtures/理论界 | 所有pair与family keys/receipt失效；不能只改一个成员变量或metadata | DCP→RCB与RS→RCB两条精确恒等式；独立Fraction检查；不先改E80 | [P13](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p13) [P15](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p15) [P19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p19) [P20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p20) [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) |
| **改变同一q的root** | EV坐标定义与NTT表身份 | factory每个(q,root)、旧缓存、全部EV对象与键、CF↔EV转换、fixture | 数学环可相同，旧EV坐标不兼容；q-only NTT缓存可能保留旧表；旧roundtrip未证明新root | 先检查root身份而非只验阶；定位NTT缓存键；受控全对象重建或隔离进程设计后再验证 | [N03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n03) [N04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n04) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) |
| **改变P或auxBits** | evalkey模数、ApproxModDown误差、公开QP暴露 | P生成、rootP、PModq/P^-1/part互补表、evalkey QP shape、安全/大小报告 | Q不变时数学secret/payload可能仍可解释；evalkeys和P相关预计算必重建，opaquecontext另核 | 先对P生成和所有逆元/表尺寸做精确静态核对；不套用旧key | [U09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u09) [U10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u10) [U11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u11) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r18) [P18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p18) |
| **改变partition / numLargeDigits** | α=ceil(L/digits)、每digit basis、P数量/大小、key shape | factory显式参数、普通默认0派生、完整PrecomputeCRTTables、scheme/keys、memory声明 | 旧evalkeys不能按新分区读取；root数字d_num相同也不是算法等价证明 | 列每digit区间覆盖与最后非空、P预算、A/B长度；定位独立key-shape负控 | [U05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u05) [U09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u09) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [P18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p18) |
| **HYBRID↔BV / digitSize** | gadget体系、是否升QP、误差和数组语义 | 参数类+scheme实现两处、BV窗口/0特殊分支、预计算、key验证、所有family evalkeys | 旧HYBRID key不得标成BV；paper对应与安全报告失效；private DGG问题不自动消失 | 静态检查scheme installed与params一致；分别按BV/HYBRID源码读key结构 | [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [P18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p18) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r18) [R19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r19) |
| **改变Δ初值 / b / desiredPrecision** | 编码整数/真实归一化；metadata与运行技术可能分离 | highprecision binding、ExactScale、b记录、Q容量、inputwitness、wrong-scale负控；不能用desiredPrecision代真实Δ | Δ变则旧m/ct不是新编码；只改metadata不改数学ct且可能被guard拒绝；旧精度证据失效 | 用纯fraction追踪真Δ；区分“初值改了”“仅记录值改了”“CCParams无效” | [P03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p03) [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) [P25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p25) [P28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p28) [U12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u12) [U05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u05) |
| **FIXEDMANUAL↔自动scaling / compositeDegree** | 自动对齐、rescale时机、每步塔数、noise/level语义 | factoryguard、普通AdjustForMult、ModReduce表、双项算法推导、receipt、测试电路 | 不能静默让上游多rescale一次；旧八轮规模/元数据不适用 | 先列每API会不会真正除模，重建步数和basis表，再决定是否是新算法 | [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [U24](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u24) [U25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u25) [N05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n05) [P17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p17) [P20](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p20) |
| **secret枚举/h** | 根分布、e1s项、安全结构、投影/扩P前提 | customh128入口与所有guard、普通KeyGen区别、same-root family、PK/keys、h正负条件、论文h声明 | 新s意味着所有key/ct/样本失效；改变标签但不换s同样不满足新声明 | 静态分支/支持大小/符号条件核对；现有h128smoke是定位，不推广所有h | [P06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p06) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) [R07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r07) [R13](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r13) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [T19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t19) |
| **改变σ** | epk/e0/e1，可能Gaussian s/v；evalkey另看private构造 | float参数、DGG表/阈值/截断、OpenMP局部σ、factoryguard、安全估计、实验身份 | 新生成key/noise才采用新σ；旧PK的epk不会因SetStd变化；已有密文噪声不会改变 | 先分contextσ/局部evalkeyσ与build分支，再选择是否需要受限验证；不降低σ刷PASS | [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [R10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r10) [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [H05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h05) |
| **改变v分布** | epk·v项和PKE masking安全 | publicEncrypt分支、数学安全假设、payload来源记录、噪声/oracle/报告标签 | h128原本不约束v；旧payload不是新分布实验；rootPK可同但不能宣称随机配对 | 先追实际公钥Encrypt重载和TUG(h0)；明确是新配置而非原bug证据 | [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [P27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p27) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) |
| **public↔secret payload模式** | public三项噪声和secret单e模型不同 | API、调用方持钥权限、输入/输出接口、威胁模型与实验命名 | 不能称修复原public任务；既有public误差报告不继承 | 静态重载解析、PKE公式对照、权限边界检查 | [R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12) [P27](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p27) |
| **输入域/微扰/phase** | 编码m、真实z^(256)、fresh误差传播条件数 | Input生成器、fixture hash、oracle、绝对/相对norm、witness与输出幅度下界 | key可重用需显式设计；当前annulus是新key/noise且非配对；原压力样本FAIL保留 | 先无FHE的全槽范围/微扰/指数下界检查；明确只改哪个项 | [T01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t01) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09) [T11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t11) [H01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h01) [H03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h03) [H04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h04) |
| **输入或codec工作精度/舍入** | 编码整数、异常拒绝区间、解码精度 | ClientReal100、Work160/220、根表、roundmargin、lift桥、codec阈值/fixture | 若整数m变了则新payload；纯decoder变更可解释旧合法ct但产生新测量证据 | 先参数/单位和精确整数边界核对；不得把更多位数等同正确 | [P22](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p22) [P24](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p24) [P25](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p25) [P28](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p28) [P29](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p29) |
| **observer精度/范数/序列化** | 报告值、可裁决性、接口兼容；不改固定ct | 512/768bit、C的1-norm、allowance、PASS/UNRESOLVED、110sig wire、replay/parser、阈值与报告 | 旧ciphertext算术未变；旧判决可能需重评，旧raw capture须有合法来源；不称重新加密PASS | 正向/置换负控、exact rounding/schema、component↔complex口径；只沿合法捕获做后继评估 | [T12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t12) [T13](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t13) [T14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t14) [T16](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t16) [T17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t17) [T18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t18) |
| **OpenMP/线程/PRNG/标准库** | 局部DGG构造、线程引擎初始化与拒绝/消耗顺序 | 真实依赖binary+flags、并行宏/环境、engine factory、调用轨迹、实验fingerprint | 旧随机样本/配对声明失效；“seed相同”不足；普通contextσ getter不能证明局部σ | 先验证构建身份与private/firstprivate语义，不采样；分支确认后由根端决定诊断 | [R01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r01) [R02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r02) [R03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r03) [R07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r07) [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R23](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r23) [B05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b05) |
| **NATIVEINT/backend/优化宏** | 数值类型、模乘/基转换实现、兼容性 | CMake全部flags与OpenFHE binary、native roots实现、WITH_REDUCED_NOISE、序列化与平台报告 | 旧二进制对象/时间数据不自动兼容；NATIVE128默认也不同于native64 | 从配置/预处理证据确认分支；不在文档轮构建；大整数标量校验不能证明该binary | [B01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b01) [B02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b02) [B03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b03) [B04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-b04) [N02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n02) |
| **仅安全级别HEStd / 统计λ / 未启用模式** | 普通自动生成安全查表或噪声模式；可能当前完全不消费 | 区分NotSet与HEStd、λ30、queries1、flood/MP/boot/PRE；重新评估模式启用前提 | 改标签不产生安全证明；paper手工factory可能拒绝；不能用E80当安全PASS | 静态核setter到consumer是否可达，再判断是否要定义新profile | [U05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u05) [U06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u06) [U07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u07) [U17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u17) [U18](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u18) [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) |

## 4. 四个具体联动例子

### 4.1 “S100改成S116”并非只把100写成116

实际差异是两枚Base50→58、Div40→56和对应的三枚root、初始Δ2^100→2^116、metadata b50→58。N、slots、8枚Mult60和P保持相同。真实QP从约680变为约712，终端模数也扩大；因此需要独立profile、precision116 fixture/证书、scale oracle、constructor guard和新key family。历史原输入PASS只能归属于S116，不能覆盖S100 FAIL。[P01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p01) [P02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p02) [P05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p05) [T05](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t05) [T06](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t06) [H01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h01) [H02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h02)

### 4.2 “只把输入往单位圆内部缩一点”也必须变更研究标签

annulus只改生成器的base1015为999，保留S100 Q/P、N、Δ和相位/微差规则。本轮可纯标量验证域和非平凡输出下界，但历史annulus同时生成了新的root key与PKE/evalkey噪声，所以没有固定旧ciphertext只改变一个明文量的配对关系。绝对误差变小既可能受条件数影响，也受新随机样本影响；不能据此独立识别其份额，更不能把原输入污名化为违反论文公开分布。[T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09) [T11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t11) [H01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h01) [H03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h03) [H04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h04)

### 4.3 “只换另一枚同阶NTT root”有隐含状态

有限域上同阶根很多，数学变换都可成立，但同一q/N下不同root意味着EV坐标不同。当前native NTT预计算缓存查找以modulus为键，再看长度，不比较root；所以缓存状态也是隐含输入。新的root必须与全部EV对象及其表一起绑定。plan析构只清owned evalkey tags，不等于清理global NTT。直接全局Reset也可能破坏其他仍存活对象的预期，故不能把“加一行Reset”作为本轮修复建议。[N04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-n04) [U26](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u26) [P04](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p04)

### 4.4 “只改σ或OMP线程数”可能没有到达预想的随机项

已有PK携带的epk固定；改变contextσ不会重生成它。payload的新v是否Gaussian取决于secret分布枚举，当前是稠密三元；σ对其无直接作用。HYBRID evalkey循环又可能默认构造σ1私有DGG，因而context SetStd变更不一定到达该消费点。即便只把有效OpenMP的线程数设为1，也不会把private变为context副本。应先完成静态/构建归因，不能先用更小σ或重采key追求好结果。[R14](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r14) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [R10](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r10) [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) [P12](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-p12)

## 5. 证据继承规则

**可继承的是来源事实，不是未经限定的结论。** 同一个fixed源码公式、相同独立标量身份检查可作为新分析输入；但其应用前提应重新核对。若Q/d/N变更，旧的crt/scale/安全/数值结果不能直接继承；若输入变更，旧原输入FAIL仍是旧条件的真实证据；若采样重新进行，必须给新sample/run身份，不得用“同配置”替代“同随机量”。

**测试通过的逻辑范围不能扩大。** 小N结构测试说明其明确断言，没有自动证明N32768/16384槽/所有key；十anchor稀疏Horner的强项是独立解密/投影机制，弱项是未覆盖全部slot；full-slot observer的强项是全槽，仍须确认误差模型、控制样本和范数口径。只有finalizer/packer/status完成不能替代E80数学门槛。[T03](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t03) [T07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t07) [T09](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t09) [T17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t17) [T19](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-t19) [H01](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h01)

**安全、准确度、性能分开出结论。** E80 PASS不证明RLWE128bit安全；HEStd某目标不保证80bit数值精度；单线程HEaaN表的毫秒数不可直接作多线程OpenFHE+多精度observer的目标对比。更小noise虽可能改变数值误差，却可能改变安全假设，本文不作可部署建议。[U07](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-u07) [A02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-a02) [R11](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r11) [R17](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-r17) [H02](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md#src-h02)

## 6. 变更后的最小文档闭合清单

任何后继选定的单一变更，至少需要记录：旧/新参数差分（含实际整数而不是仅bit数）、设置点与最终consumer、重建对象范围、失效证据列表、仍可继承事实的前提、独立oracle/negative control、范数/门槛不变或新命名的理由、source/build/input/run身份。新manifest必须绑定这些对象；不能修改原S100历史回执来使它显示成功。

本轮没有修改factory、源码、测试或CI；没有生成任何FHE对象。**唯一下一步建议：根端复核文档覆盖与事实，再选择具体诊断。**
