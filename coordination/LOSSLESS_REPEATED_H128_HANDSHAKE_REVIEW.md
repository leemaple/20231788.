# I/O × repeated-Mult2 × h128 handshake — source/design review

2026-09-05。**建议：保留三者的分工，但当前 proposal 不能直接拼接为共同实现契约。** 下列修订由主审/用户确认后才能进入测试或代码；本 note 不授权新 seam。
按 workflow / engineering / research 作只读交叉审核；目标 worktree 起始 HEAD `87018e13cb0c47233ae053bc4b6dc3ec7813e90d`、branch `codex/lossless-io-01`，起始 clean。无构建、密码运行、CI、外部联系或 Git mutation。

## 证据边界

- **IH/IP/ID**：I/O 实际返回中的 `REPEATED_MULT2_HANDSHAKE.md`、`PROPOSED_PUBLIC_INTERFACE.md`、`DESIGN_DECISION.md`。它们是 design-only proposal，不是已执行实现；7-member/CRC/6-hash 接收核验由 root 报告，本审核未冒充重跑。
- **RD/RG/RP**：Repeated 实际返回中的 `DESIGN_DECISION.md`、`NEXT_PAPER_GATES.md`、`complete/project/tests/repeated_mult2_basis_family_probe.cpp`；读了设计/门槛与参数、factory、secret 投影/setup 段。probe 只属 shape/basis 路由候选。
- **HB**：root 报告已推送 `aa1607aac2425566d35e9ecf2d01bad0cf810204` 的 h128 Candidate B note；本审核已核对 Git blob 与当前文件 SHA256 相同。
- **Observed** 下文指文本或源码直接事实；**Inferred** 指代数/接口后果；**Pending** 指尚须确认、实现或运行的契约。

## 可直接对齐（仍不是用户批准）

1. **客户端/求值端职责一致。** I/O 只在客户端加密/输出解密；Repeated 只拿 context、ciphertext、eval keys，不拿 secret、不 decrypt/re-encrypt；h128 属 setup，不进入 I/O codec 或 evaluator。I/O 不建 context/key，HB 只接受已 finalized context，正好可分开。直接 Element Encrypt 的 slots/level/degree/encoding 等由客户端派生补齐，而非由 key setup 提供。[O10] 保持官方 scheme Encrypt / Poly Decrypt 的既定路线；不为提高精度换成 DecryptCore、关闭已配置保护或添加 multiparty helper。（IH9–20；IP166–179,245–258；RD51–59；HB推荐/生命周期）
2. **精确尺度的值语义兼容。** Repeated 的 factor ledger 可无损表示为 I/O 的正、约分任意精度 rational；二者都把 OpenFHE double factor/degree/level 视为兼容 metadata，而非精确归一化。（IH40–71；IP19–27,225–243；RD76–87）
3. **重组尺度递推相同。** 令实际消耗的 Mult prime 为 `m_i`，固定 Div prime 为 `d`：`S_i=S_(i-1)^2/(d*m_i)`。IP 的首步正是 `S_1=S_0^2/(d*m_1)`；展开得 `S_2=S_0^4/(d^3*m_1^2*m_2)`，与 RD81–84 一致。这是代数核对，不是执行证明；首步冻结的 diagnostic 素数不可移植成论文实际 40/60-bit identity。
4. **immutable family / ordered identity / own HYBRID rows 一致。** 每个 family 自己的 Q/P/QP、roots、partitions、tag 必须匹配；不要求跨 family 的 P 恰好相同，也不凭 tower count/level 判定基底。RD 的 `numPartQ=|B_i|` 可作为显式 active-row 方案交给主审确认：源码确实拒绝 `L<=ceil(L/dnum)*(dnum-1)`，故初始 11 digits 不能原样放到 ≤10-tower family；这不证明论文原库采用同一安排。([O7], [O8]；IH24–38；RD23–49)

## 必须先修/补全的共同契约

| 项 | Observed / Inferred | 最小决策或修订（Pending） |
| --- | --- | --- |
| **PRE 与 PK basis 冲突** | RD61、RP86–99 固定 `HYBRID/STANDARD/INDCPA`；HB 必须 `PREMode=NOT_SET` 且实际 `GetParamsPK()==Q`。源码 HYBRID+PRE≠NOT_SET 优先返回 QP，STANDARD 不能覆盖它。[O1] 普通 KeyGen 在 paramsPK 采 s/a/e，候选 B 的 private zero-core 默认在 Q。[O2][O3] | 对未来共同 Q-only setup，建议统一 fresh family 构造为 **NOT_SET + 实际 PK basis=Q**，再预计算/生成 keys；不要修改已 interned/live context。原 INDCPA shape probe 不因此成为密码失败证据，也不能直接宣称满足 HB。若坚持 INDCPA，必须另行确认非本 Candidate B profile，不能默默放宽。 |
| **I/O profile 未覆盖 setup 的关键配置** | IP45–55,260–270 未列 encryption technique、PREMode、noiseScale、declared SecretKeyDist、PK element basis；Encrypt 的现有清单只明确非空 key/context/tag。 | 共同 profile 应记录/核对 STANDARD、NOT_SET、ns=1、声明 SPARSE_TERNARY、真实 PK 的两个 EVALUATION/Q elements，及已初始化 HYBRID 表/实现。实际 h128 由 client setup 检查/证据负责，不能从 public key/tag 或声明推断；不要让 I/O/evaluator 为此接收 secret。 |
| **REAL 默认值 vs COMPLEX** | RP86–99 的 CKKS 参数构造只显式传到 PRE；官方此重载后续默认 `EXEC_EVALUATION/FIXED_NOISE_DECRYPT/ns=1`，但最后的 `ckksDataType=REAL`。[O13] IP175,258 则要求 COMPLEX，明确拒绝 REAL。 | 共同 fresh setup 显式选 COMPLEX，并核对实际返回 context；不能从 seed EncodingParams 或“都是 CKKS”推断。保持既定 decryption-noise policy，不因冲突去改 live context；原 Relin shape probe 不因此自动判错。 |
| **factory 后的实际对象 / scheme ID** | RP117–139 调 GetContext 时省略 scheme ID，且检查/返回的是请求时的 cryptoParameters。官方默认 INVALID_SCHEME；factory 可返回已有 context，其查找不比较 schemeId。[O4][O5] | 构造请求显式 CKKSRNS_SCHEME；随后取 **实际返回 context 的** scheme ID、参数、Q/P/QP/roots/partition/feature state 作验证。若复用对象不符就拒绝，不补写 live context；HB 和 I/O 指针绑定不能接纳“请求对象正确、返回对象未验”。 |
| **exact scale authority / high-low 阶段状态仍缺** | RD68–72 仅提出 familyIndex/logicalScale；RD81–84 只给重组 `S_i`。IH26,35–37 要求 high、low、raised、post-RS2、re-entry 与 RCB 的状态及发行者。IP 的 rational 不能与另一个 evaluator ledger 独立变更。 | 建议 **Repeated operation 发行唯一 exact state receipt，I/O 只消费其 rational 值**。逐 phase 明确 component 的 normalization、实际 d/m factors、physical metadata 与 level/context/tag 转换；不要用 nominal 2^p 或首步 physical-factor 公式替代后续契约。FIXEDMANUAL getters 返回 nominal approximate scale 而非实际被除素数。[O9] |
| **BindFirstMult2Rcb 仅首步桥** | IP214–243 必须两个 Fresh parents、同 context/tag、drop-two 原前缀、level2、16 slots；IH75–88 明确只认 FreshClientEncoding/FirstMult2Rcb，不准第二次。Repeated 要 B_i 的 level2 → B_(i+1) 的 level1，并最终回 B0。 | 可以保留它作已确认后的独立首步 tracer，**不能把它当 repeated adapter**。共同集成需 operation-specific output receipt/adoption 处理 family rehome 及 terminal B0 clone，不提供任意 scale binder。最终 basis 要逐 modulus/root 等于 B0 的真实前缀，保留同一 secret 对应关系；源码解密按 tower count 丢 secret 的尾塔，无法替你证明任意非前缀映射。[O12] |
| **slot observation / diagnostic 不同** | I/O：N64,S16,gap2；RP273–274：N256,S8,gap16；论文目标：N32768,S16384,gap1。IP305 尚不支持其他 N/full packing。官方 Decode 从 idx、idx+N/2 按 gap 取系数。[O6] | 不把三种配置的验收互相替代。I/O 的 packed-stride 与 full-coefficient canonical Horner 在 gap>1 且 off-stride noise 存在时不等价；repeated 独立 oracle 必须标明观测量。共同 tracer 的 N/S/Q/profile 要另行冻结，不能静默改 I/O v1；论文 full packing gap1 才覆盖全部 N 系数。 |

## 同 secret / key-family 的最小边界

- **建议组合：B0 只采一次 h128 → Candidate B 的 matching B0 SK/PK → setup 内精确投影该 signed ternary secret 到实际使用的 B_i → 每个 family 调官方 EvalMultKeyGen，验证其 own QP rows/tag。** RP143–170,322–336 已展示按 prime/root 投影的候选；它目前用普通 KeyGen，在 N256 的 sparse 分支是 h192，不是 h128 结果。[O2][O8]
- 不逐 family 调 fresh-sampling Candidate B，否则得到不同 secrets；不只替换已有 SK、沿用 h192 PK/eval keys；tag 改名不是关系证明。RD54 只在 B0 输入加密，故最小方案只需要 **B0 public key**；不要平添“每个未用于 Encrypt 的 family 都必须生成 public key”的门槛。
- 明确“destroy family private keys”不包括客户端最终仍需的 B0 解密能力：临时 family SK 可在 setup 完成后释放，B0 SK 留在客户端；求值端从未取得它。RP321–341 的局部 probe 释放全部 SK 仅支撑其无解密 shape scope，不能作为可解密最终系统的持钥生命周期。
- RD72 的 rehome 是在 same-secret/exact-basis 条件下转交 ciphertext，而非 key switching 到另一独立 secret。由其列表可推：论文 B0 有11塔，八次后 active basis 剩2塔，最终回 B0 的 absolute level 是 **9**，不是首步 binder 的2；B8 若用作 terminal 描述，其 local level1 也不能冒充 B0 level1。（RD15–23,58–59,72,91）
- EvalMult 静态 cache 按 tag 跳过重建；每 trial/family 的新 tag 应有有限生命周期，结束后只清本任务拥有的非空 tag，不清全局或共享 context 的其他 entries。[O11] 本审核没有清理 cache。

## 哪些可以独立推进，哪些仍待确认

- **可独立作为下一设计/确认材料：** multiprecision value、正 rational、no-Plaintext client seam、packed-stride 标签、精确 centering 与官方 crypto primitive 的职责划分；h128 B0 setup 的小型独立契约也不需要等待八次 repeated 成功。任何 code/TDD 仍需相应 user seam 确认。
- **共同集成前先解决：** 上表 PRE/PK/COMPLEX profile、factory 实际对象、单一 scale owner、phase/rehome/terminal receipt、相同 packing 的验收契约。先冻结小型功能证据，再做八次/1000-trial 的实证；不得把 shape probe 当 second-Mult2 semantic GREEN。
- RD5 将完整 second patch 先系于 h128，RG23–27 又把 h128 列为 G3；应统一为 **h128 是论文 setup 必需，不阻止已允许的低 N semantic second diagnostic**。RG41–45 的 estimator/related-key review 限定具体 security claim，不升级成普通功能实现的通行证。
- 无法确认 HEaaN 的未公开 sign law 只是一项比较披露；不新建 HEaaN-distribution-equivalence、所有 key 的安全定理、serialization 或 rotation 门槛。源/代数推断不能替代后续受控 public Encrypt/Decrypt、matching eval 与 repeated precision 实验。

## 证据定位与完整性

I/O 返回根：`/private/tmp/lossless-io-return-review.cf6HsI`；Repeated 返回根：`/private/tmp/repeated-mult2-return-review.QGVKRy/repeated-mult2-bounded-basis-routing-probe-774fe2d`。下面 SHA256 固定本次读取的 proposal，不表示采纳。

| ID / 文件 | SHA256 |
| --- | --- |
| [IH — REPEATED_MULT2_HANDSHAKE.md](/private/tmp/lossless-io-return-review.cf6HsI/REPEATED_MULT2_HANDSHAKE.md) | `71b1553faa2a9b6cb9a1bfb75f3a827d65472e067f2c93c86a06829f38772f2f` |
| [IP — PROPOSED_PUBLIC_INTERFACE.md](/private/tmp/lossless-io-return-review.cf6HsI/PROPOSED_PUBLIC_INTERFACE.md) | `cabf77b63ea73fe70b87909b444ae82282eff4ddb303e80ffd477a77d4e9b17d` |
| [ID — I/O DESIGN_DECISION.md（本次重点98–323）](/private/tmp/lossless-io-return-review.cf6HsI/DESIGN_DECISION.md:98) | `7518535a6be62d320414c4c48a365ba9cda2cbdd285af2b61a7f249569439429` |
| [RD — Repeated DESIGN_DECISION.md](/private/tmp/repeated-mult2-return-review.QGVKRy/repeated-mult2-bounded-basis-routing-probe-774fe2d/DESIGN_DECISION.md) | `9c139ea61c755fe0b9f757b3a5c2488bd7a66ad351fac07ea722a4d026fba1db` |
| [RG — NEXT_PAPER_GATES.md](/private/tmp/repeated-mult2-return-review.QGVKRy/repeated-mult2-bounded-basis-routing-probe-774fe2d/NEXT_PAPER_GATES.md) | `6f55ee5317ff3f98935df2176aa70f89bf42bccc61aaa39183468fdb12702fa1` |
| [RP — complete/project/tests/repeated_mult2_basis_family_probe.cpp](/private/tmp/repeated-mult2-return-review.QGVKRy/repeated-mult2-bounded-basis-routing-probe-774fe2d/complete/project/tests/repeated_mult2_basis_family_probe.cpp:72) | `fbdc5190fc3a90658fdfe1e69cbeb6044be79d46d8dcdd186eb9f65410cdb7bb` |
| HB — [PAPER_H128_SETUP_CANDIDATE_B_DECISION.md](/Users/lifeng/Documents/20231788-openfhe-codex-repeated-mult2-01/coordination/PAPER_H128_SETUP_CANDIDATE_B_DECISION.md) | `7d2d8ab1b1293bf72ec7b2efceb2644a72a5bec197184496031ff252ea9e208d` |

官方源码仅来自已验证 official53 ZIP（SHA256 `8cc569e227c959c94c9289227a27f741536fab30e351d16aade5f65db5e31784`），以下文件字节数/SHA256/Git blob SHA1 与其既有 provenance 相符。缓存的同包已核验源码复用；本次新增小读亦逐项核验。无网络 fetch、本机 OpenFHE 或旧实现输入。

| 引用 / upstream path | Bytes | SHA256 |
| --- | ---: | --- |
| [O1], [O9] — src/pke/include/schemerns/rns-cryptoparameters.h | 61640 | `29e3d0666c41a36a4a7627fc4dd26487999ed2d81f6e0dd68046624275cb2340` |
| [O2] — src/pke/lib/schemebase/base-pke.cpp | 7991 | `371a22ceaea7ed39a2ae164a1456966b130afbbcc5eafc9f323feda98a5d8e84` |
| [O3], [O12] — src/pke/lib/schemerns/rns-pke.cpp | 8654 | `e1103468a406d16202fc66d918b28fdf93d436aeadb86efda4a01032fac3bf19` |
| [O4] — src/pke/include/cryptocontextfactory.h | 3609 | `09b9a58957a62884e913c1be7a53249bd0c397a91060141255280d7c363880c6` |
| [O5] — src/pke/lib/cryptocontextfactory.cpp | 4060 | `b7b8da50247a2267e0d05bb1a211a69336a49b0eb0b18238a901d4a3fdd5cbdb` |
| [O6] — src/pke/lib/encoding/ckkspackedencoding.cpp | 22759 | `ce967bf68d80a63101984e8927298923c6c3c6754dc522e63355e7ec40655401` |
| [O7] — src/pke/lib/schemerns/rns-cryptoparameters.cpp | 21187 | `4dbf003cce3cc02d92ebeb62398c2350c0f9e31d171b19a592e1e0fa6b9c9907` |
| [O8] — src/pke/lib/keyswitch/keyswitch-hybrid.cpp | 21228 | `872d5e594ac2343f9d055a4e628871987113f67f2142d3a664b2015f17e1d345` |
| [O10] — src/pke/include/cryptocontext.h | 176160 | `cb88d34595f87eb9c525279a2000b20d15b24c3d71f0d177bd10f1a933538cfb` |
| [O11] — src/pke/lib/cryptocontext.cpp | 42570 | `de14bbf46686facd807485e3471f6293271cc0d2f03ae300d64f14f71aa217db` |
| [O13] — src/pke/include/scheme/ckksrns/ckksrns-cryptoparameters.h | 7505 | `b10acefbde0e36e7eb1c15e73d117a15e913221bbc5d19ce80768cc546192d54` |

[O1]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-cryptoparameters.h#L263-L269
[O2]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-pke.cpp#L47-L98
[O3]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-pke.cpp#L111-L196
[O4]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontextfactory.h#L74-L76
[O5]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/cryptocontextfactory.cpp#L41-L76
[O6]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/encoding/ckkspackedencoding.cpp#L336-L405
[O7]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-cryptoparameters.cpp#L62-L90
[O8]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L51-L129
[O9]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-cryptoparameters.h#L608-L648
[O10]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L1250-L1266
[O11]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/cryptocontext.cpp#L87-L129
[O12]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-pke.cpp#L199-L223
[O13]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/ckksrns-cryptoparameters.h#L70-L88
