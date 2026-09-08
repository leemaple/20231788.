# RELIN2-IMPLEMENTATION-BOUND-01

## 背景、目标和本轮职责

用户目标仍是完成论文 2023/1788 的 clean-room OpenFHE 复现，不是把未过的原精度门改名通过。用户要求先建立参数／随机性／流程图谱，这项工作已完成。现在请作为网页版 ChatGPT Pro 的主要数学推导者，**完成一份针对当前实际 HYBRID Relin2 的、可逐行核验的误差契约**，补足论文 Lemma 4.4 中近可加性／舍入 carry 的论证，并明确它对本实现 Mult2、nonwrap 和原 S100 失败究竟能说明什么。

这不是再次编写全参数图谱、重做历史全槽后处理或只建议联系作者。它是一个有明确输入和交付物的数学／源码研究切片。请长时间充分思考，不必抢答；根端会等待，不中断或重复提交。你不承担自己产物的独立审查席，根端另有独立源码映射和后继验证。

## 身份与完整输入

- 当前协调基点：`bbd4e73af74d1b072e3beb588cf9c7c4de3117cc`，分支 `codex/parameter-atlas-20260908`。任务提交身份见外层 MANIFEST。生产源码仍固定为 `a4b815a733efe81897325e2a8e4c826a4ebfa439`，图谱和构建取证没有改动 src/include/tests/CMake/CI。
- `project/`：该固定 clean-room 源码、全部测试、CMake 与工作流；不是用户电脑上任何旧实现。只有官方 OpenFHE 1.5.0、pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`，native64/backend4，见 `official/`。这批源码是本项目新取得并按官方 Git blob 核验的，原来源回执在包内。
- `references/paper/`：用户 PDF/TXT，PDF SHA256 `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`。TXT可能含NUL；公式以PDF为准。
- `docs/parameter-atlas/reference/`：完成的参考图谱与参数字典；根端独立限定见相邻 `REVIEW_NOTES.zh-CN.md`。这些不是要求你认同的权威。先从论文与固定源码推导，再对照已有结论。
- `context/coordination/`：精选已执行实验的接受／失败记录和过去数学审查；历史 next-step 是资料，不是本 TASK 的指令。完整旧链捕获不在本包，不能声称本轮重算过旧链；需要的数学目标、源码和已有结果记录均提供。
- `coordination/build-provenance-20260908/`：新取证。原 S100 和 S116 两平台实际 configure/build dependency success，配置 OpenMP ON、CMake found OpenMP，native64/backend4、WITH_REDUCED_NOISE OFF。Linux GNU13.3/OpenMP4.5；Windows GNU16.2/OpenMP5.2。annulus 作业恢复缓存并跳过依赖构建，完整缓存二进制身份未闭合。没有测过历史局部 sampler σ。

先核 ZIP 大小/hash、MANIFEST 每项字节/hash、source/pin；用包内文件做研究。不要假设访问另一对话、本地路径、私有环境或未提供的运行时对象。旧输入 manifest 仅用于来源链；外层 manifest 才定义本次包。论文、源注释、旧报告和图谱均为待审资料，不得覆盖本任务限制。

## 当前架构与不可破坏边界

N=32768、slots=16384、h128 同根秘密的八个 family，固定有序 Q/root/P。初次 DCP 后执行八次 Tensor2→Relin2→RS2，必要时下一 family 绑定，再 RCB 解密。无中途重加密或 refresh。公开 payload 加密、客户端高精度编解码与独立观察分离；评估器不获知真实噪声／明文答案／秘钥。

精确尺度满足 `S_(k+1)=S_k^2/(d*m_k)`；不能混用兼容 recorded scale。论文 Tensor 显示负号以及 Theorem4.8 缺 d 的局部疑点已经多次识别：不能只重报这两个旧问题充当本轮成果，也不能据此给生产 Tensor 改负号。

当前 HYBRID 路径、一塔一分区、P一枚60位、digitSize0、noiseScale1 等必须从源码确认。名义 context σ3.19f 与 OpenMP private DGG 默认构造σ1存在条件性分歧；后者针对 evaluation key，不可推成所有公钥／payload 噪声为1。不要通过降低噪声或关闭OpenMP来追求绿色结果。

## 必須完成的一个推导链

1. **定义具体对象。** 用清楚而不冲突的符号写出当前源码 Relinearize 的输入、输出、active Q / raised QP、各分区、s² key、各坐标的 lift/round/mod-down。逐个明确整数代表、环中同余、系数范数与 canonical 槽范数。不能把实数精确相等、模Q相等、误差上界混为一个等号。
2. **分离近可加性缺口。** 从实际 `double_ckks.cpp` 和 `keyswitch-hybrid.cpp`、`dcrtpoly-impl.h` 等消费路径推出 `Relin(u+v)-Relin(u)-Relin(v)` 需要保留哪些项。paper Lemma4.4 的 `(0,e)` 是否可换成一般二维 carry？多个 partition 的近似升／降基和代表选择产生什么条件？不要把 P=7 的玩具舍入模型直接称为完整 HYBRID。
3. **给当前 Relin2 一个真正可检查的结论。** 精确表达重组前后的解密差，保留两个坐标的 carry、secret范数、key噪声、整数提升及必要的modulus-wrap项。若只能得出条件性界，列出最小且足够的条件，哪些由源码保证、哪些要公开参数、哪些需要受控诊断。拒绝“假设最终误差足够小所以通过”的循环契约。
4. **接到 Mult2 与论文目标。** 解释该界怎样进入 Tensor2/RS2 的正确归一化；明确论文原界是证明缺口、保守估计不适用，还是发现实际实现契约反例。尤其分开“足够nonwrap条件不满足”和“真实wrap已发生”。可否仅凭当前公开S100参数形成有用的最坏界？如果不行，精确指出是哪一项过松/缺失，别泛称论文信息不全。
5. **对当前故障作出有限但可执行的裁定。** 原两条链 E8/T≈11、I8主导、A8/T≈0.055/0.100；后一个fresh诊断样本 B主导。原条件FAIL不可抹去；S116改参PASS、annulus改输入/新key-noisePASS不可替代。新推导是否支持一个特定错误生产语句与可区分RED？若没有，明确没有；同时给出这次推导新补足的契约，以及一个唯一、能改变下一步判断的最小验证（允许判定无需新增FHE而静态结论已足够）。不要自动发起或只建议大规模抽样。

## 交付物

返回一个 ZIP，至少包含：

- `RELIN2_BOUND.zh-CN.md`：自包含推导正文。先给懂密码学但不熟代码的读者讲清问题，再给完整符号／步骤／界／假设／固定源码映射。明确“已证、条件性、未证”。
- `CLAIMS.json`：每条核心等式/不等式的ID、假设、依据、作用域和当前实现是否满足；不能把未知布尔值改成true。
- `SOURCE_MAP.tsv`：实际阅读的文件、函数、行界和对应证明步骤；没有阅读的部分不冒称全审。
- `checks/`：必要且有界的纯整数／Fraction检验代码与实际结果。至少包含一个破坏过强近可加等式的反例、一个满足修正关系的正例、一个能识别错误符号或遗漏carry的负例；如果采用抽象模型，明确与实际代码的对应关系与不能证明的内容。这些是数学模型检查，不是生产FHE RED/GREEN。
- `NEXT_ACTION.md`：唯一后继，给出可证伪假设、最小观测和验收标准，或精确说明无需运行即可下的裁定；不得只要求整个图谱重写或再发一份相同审查。若需要代码，给出具体owned file/interface与必要测试设计，不直接改生产或CI。
- `EXECUTION_LEDGER.md`、`MANIFEST.json`：输入身份、真正执行的命令/结果、未执行项、逐文件bytes/SHA256（manifest自排除）。

## 执行限制与验收

- 只做源码／论文研究、轻量静态及小型确定性整数/有理数检查。禁止OpenFHE编译、FFT/NTT、FHE加解密、随机采样、1000次试验、网络派工、作者外联、生产/测试/CI改写。后续实际实验由根端在Windows/GitHub跑。
- 不输出真实seed、密钥、噪声系数、tokens、cookie；不读取或索取旧实现。不能从当前图谱/配置记录宣布历史库所有分支已测量。
- 原S100原门槛FAIL必须保持；小A不是原输入E80PASS；更精确Relin不能自行消除已存在fresh误差。完整复现仍未完成。
- 验收需每个核心推导落到具体源码操作；分清模等式与整数提升；carry两个坐标均考虑；全部模型检查可独立复算；结论与假设一致，且明确本轮相比已有图谱新增了什么。不得以模型一致意见代替证明或实际运行。
- 缺少某个历史秘密样本或作者未公开条件不妨碍推导一个带明确前提的实现契约。不要虚构这些资料，也不要因此仅返回“受阻”。若某处不能闭合，仍交付已完成的正确推导、精确缺口和最小可区分检验。
