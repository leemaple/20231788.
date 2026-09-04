# Repeated Mult2 phase contract — bounded source research

2026-09-05；`codex/repeated-mult2-01` 起始 HEAD `d524dae752e767486bb5737a69daed9ecf77d1d3`，起始 clean。**新结论是可具体填写的 phase-normalization 表及 re-entry 不变量，不是新算法、API 批准或精度证明。**
先完整读 workflow/engineering/research，以及既有 scale-algebra、raised-basis、visible-source-recheck 和 I/O handshake notes；它们已给出总体尺度/基底约束，但未把 low 的修正语义、raised-high 临时 normalization、RS2 双 rounding 与逐 phase receipt 合在一起。只写本 note；无测试/编译/密码运行/CI/外部派工/Git 提交，不声称本树任何新 suite 已运行。

## 1. 记号与精确性的边界

- 仅为纸面模型，令 `D_s(c)=Σ c_k s^k`（两项普通 ciphertext、三项 tensor）；H、L 分别代表 high/low 的这一环内值，不表示 evaluator 持有或调用 secret。令 `V=dH+L`，`d=q_div`，S 是 **重组表示唯一 exact rational normalization**。（P415–477；C1119–1129）
- 在相容的整数 lift/无 wrap 解读下，定义 `xH=H/(S/d)`、`xL=L/S`，故 `V/S=xH+xL`。**low 是同一表示的修正贡献，不是另一个可任意指定编码 scale 的独立 message。** S/d 与 S 是派生分母，不要设三个独立 authority。
- 环恒等式先在当前 active modulus 下成立；除法若作模逆并不自动等于实数归一化。中心化、跨模数 lift、乘积与 rounding 的实数/slot 解读仍受 wrap/noise 条件约束。P614–660 明确将无条件模恒等式与有大小条件的整数结论分开；S 精确不意味着 V/S 精确等于用户输入或其幂。
- 本推导沿用 Def4.1/4.5 与 P951 的整体除数 `d*m_i`；打印 Theorem4.8 的 `1/q_l` 显示式（P919）差异已由既有 `MULT2_SCALE_ALGEBRA_CHECK.md` 记录，不在此宣告官方勘误或复用其理想 fixture 作为 crypto oracle。

## 2. 第 i 次操作的 basis 与 phase（i=1..8，重点 i≥2）

按 **实际消耗顺序** 编号 Mult primes 为 m1..m8；不是按原列表从左到右的显示编号。写 `A_(i-1)=[b0,b1,m8,...,m_i]`、`B_(i-1)=A_(i-1)||[d]`；`A_i` 删除 m_i，`B_i=A_i||[d]`。每个名字包含对应 roots 的精确顺序；第8次后 A8=[b0,b1]。此处 B/A 是 ordered vectors，不是可互换的乘积。（RD15–32,72,91）
对一次二元操作令输入重组 scales 为 S_L,S_R，`T=S_L*S_R/d`；重复平方取 S_L=S_R=S_(i-1)。

| Phase / 实际值语义 | ordered basis；local level；component arity | exact normalization / receipt 后果 |
| --- | --- | --- |
| 初次 Fresh → DCP（仅一次） | B0/0/2 → A0/1/(2,2) | Fresh S0=2^100；pair 保留 S0，high 分母 S0/d，low贡献分母 S0。DCP 不是“两个相同 scale 的独立明文”。 |
| 第 i 次 input pair | A_(i-1) 属于 B_(i-1)；1；(2,2) | 必须承接 S_(i-1)，high S_(i-1)/d，low贡献及 RCB S_(i-1)。 |
| Tensor2 | A_(i-1)；1；(3,3) | H_T=H_L H_R，L_T=H_L L_R+L_L H_R；high 分母 T/d，low贡献/RCB 分母 T。 |
| raised-high U=d·H_T | B_(i-1)；0；3 | 高支路现为单 ciphertext，分母 **T**；已有 towers 乘 d，新增 d tower 为零。它只表示 high×high 贡献，不能因分母为 T 就当成完整 product/RCB。 |
| Relin(U) 与 Relin(L_T) | 分别 B_(i-1)/0/2 与 A_(i-1)/1/2 | 两条分母均为 T，但语义不同；relinearization 引入误差，不修改 exact normalization。 |
| Relin2 内部 DCP(Relin(U)) → 合并 low | A_(i-1)；1；(2,2) | quotient high 分母 T/d；内部 remainder 与 Relin(L_T) 都作为 T 分母的修正，二者相加。输出 pair 的 S=T，**不是再除 d 后的 T/d**。 |
| RS2 | A_i；旧 B_(i-1) 内 level2；(2,2) | 输出 S_i=T/m_i，high 分母 S_i/d，low贡献/RCB 分母 S_i。两次 rescale 的差保存在 low。 |
| re-entry / rehome | 同一 A_i；新 B_i 内 level1；(2,2) | S_i、d、组件系数和表示原样保留；仅按已建立的 same-secret/exact-basis 关系构造新的 context/tag wrapper。**不再 DCP、不乘/除 d、不重置 S0。** |
| terminal RCB / client output | A8 在 B0 内 absolute level9；2 | RCB 仍 S8；回 B0 tag/context 不改变 scale。当前 first-only binder 的 level2、Fresh-origin 条件不适用。 |

表的操作来源：P433–477,584–618,664–684,783–815；当前 C393–434,763–809,925–986,997–1110。RD58–59,72 给出 rehome/terminal 路由，但当前候选未实现本表的完整语义。
抬升是 `R_Q → R_(Qd): x ↦ d*x` 的专门映射；普通 re-entry 没有抬升到 B_i 全基底，只把 A_i 当作它的 one-tower-short prefix。P463–467 提醒：运算后的 RCB 一般仅在 Q 上定义，没有自然的 Qd lift。

| 平方编号 i | raised B_(i-1) 塔数 | input/Tensor/Relin pair A_(i-1) 塔数 | RS2/re-entry A_i 塔数 | 最终若回 B0 的 prefix level |
| --- | ---: | ---: | ---: | ---: |
| 2 | 10 | 9 | 8 | 3 |
| 3 | 9 | 8 | 7 | 4 |
| 4 | 8 | 7 | 6 | 5 |
| 5 | 7 | 6 | 5 | 6 |
| 6 | 6 | 5 | 4 | 7 |
| 7 | 5 | 4 | 3 | 8 |
| 8 | 4 | 3 | 2 | 9 |

这是论文11塔初始列表与 RD family 规则的计数推论，不是运行证明。每次 local-level 轨迹仍为 `1 → (raised 0 → back 1) → 2 → next-family 1`；raised B_i 对 B0 一般不是 prefix，不能给它伪造“B0 level”。P1588–1590 给出 Base50×2/Mult60×8/Div40 目标。

## 3. 必须保留的环恒等式与误差来源

1. **Tensor 的丢项不可被 normalization 隐藏：** `d*V_T=V_L*V_R-L_L*L_R`（环内精确）。在有效 lift 下除以 S_L*S_R 得 `V_T/T=(xH_left+xL_left)*(xH_right+xL_right)-xL_left*xL_right`。S_T=T 是精确语义记账；省掉的 low×low、已有输入误差和之后的 crypto 噪声仍是实际误差，不能宣布误差为零。（P614–660,947；C777–804）
2. **Relin2 内部 DCP 是误差补偿，不是外部刷新：** 令 ctU'=Relin(raised-high)、ctY=Relin(ctL_T)，内部 DCP 输出 (ctH',ctRho)。由 ciphertext 分解关系 `d*ctH'+ctRho=ctU' mod A`，所以 `RCB(output)=ctU'+ctY mod A`。high 的 quotient rounding 由 ctRho 保存在 low；ctU'/ctY 的 relin 误差没有消失。Relin2 净基底仍 A、S 仍 T。（P672,735–783；C945–980）
3. **RS2 并非 (RS(H),RS(L))：** 令 `ctV=d*ctH+ctL`，则 `ctH_o=RS_m(ctH)`、`ctL_o=RS_m(ctV)-d*RS_m(ctH)`，故 `RCB(output)=RS_m(ctV)` 在输出环内精确。若用相容 lifts 写两次 rounding residual 为 eta_H、eta_V，则 `ctL_o=ctL/m+eta_V-d*eta_H`；不能省掉补偿项或当成“low 未变/仍是 fresh remainder”。（P785–815,1028–1052；C1048–1093）
4. 低部在连续运算中增长是论文显式研究对象（P964–1010）；重新赋予 fresh-DCP 的小 low bound 是无根据的。P1521–1523 的 DCP∘RCB 刷新消耗额外 Div primes，而 P1574–1576 的八次实验明确不用该策略。故 family re-entry **保留现有 pair**，不能用重新 DCP 偷换算法或把低部“清零”。

**极小纸面 rounding 例（非测试）：** d=13,m=17，某次 RS2 前单系数 H=8,L=8；最近整数舍入且无 tie 时，RS(H)=RS(L)=0，但 RS(dH+L)=round(112/17)=7，故 L_out=7≠RS(L)，且 7>floor(d/2)=6。这里只比较单系数 DCP remainder 范围，不是解密 low 的 secret-convolution bound、OpenFHE 执行或精度结果。

## 4. 精确 receipt 与现有 first-only metadata 的具体差距

- 每个 pair 以 **S 与固定 d** 派生 high/low贡献/RCB normalization；Tensor 根据不可变 parent S_L,S_R 算 T，RS2 根据实际最后 active modulus m_i 算 T/m_i。每次乘法都使用新 S_i，闭式为 `S_i=S0^(2^i)/(d^(2^i-1)*Π_(j=1..i)m_j^(2^(i-j)))`。raised 单支路另标其 phase 与分母 T；不要维护三个可独立修改的 scale fields。
- “pair-normalized state”与“freshly decomposed”必须分开：两者可都有 local level1/degree2/相同 recorded factor，但第二次的 low 已增长、S1 一般不等于 S0。当前 C475–495、530–539、638–648 仍从 constructor 的 expectedInputScalingFactor_ 重算 first-only logical scales，C766–768 只接受 ReadyForFirstMult；**仅移除 lifecycle guard 或 rehome 不能闭合此契约**。
- 当前 Tensor 先取得 degree4 / factor F_L*F_R，然后仅改 metadata 为 degree3 / factor F_L*F_R/b（b=GetScalingFactorReal(0)）；C795–798 没有除多项式。其 exact S 只能由 T 推导，不能再把 b 当作物理 scale 除数。Relin2 保留该 metadata；RS2 将 degree3→2 并除以 GetModReduceFactor，但实际 CRT rescale 使用当前最后素数的表。[O1][O2]
- 若所有 family 保留相同 FIXEDMANUAL b 且 F0=b²，名义 recorded factor 每次可回到 b²，而 **S_i 仍按实际 d,m_i 变化**；这正是 fresh-like metadata 无法证明 fresh scale 的原因。具体浮点观察仍按执行顺序验证，不用这一代数简式冒充跨平台逐位运行结果。
- 最小待填契约是：phase 对应单支路/修正/重组语义、不可变 parent-derived S、实际 d/m_i、上表 basis/roots 与 local level 变化，以及由哪次 operation 发行可被 I/O 消费的 receipt。既有 I/O handshake 的高/低阶段问题由此具体化；不需要新增任意-scale binder、generic framework 或新用户门槛。原 seams 的确认状态仍 pending。

## 5. 证据定位、hash 与未做事项

P=[原论文 TXT](/private/tmp/repeated-mult2-handoff-recheck.WkleUq/payload/PAPER-2023-1788.txt)；C=[当前 clean-room src/double_ckks.cpp](/Users/lifeng/Documents/20231788-openfhe-codex-repeated-mult2-01/src/double_ckks.cpp)；H=[header](/Users/lifeng/Documents/20231788-openfhe-codex-repeated-mult2-01/include/openfhe_2023_1788/double_ckks.h)；RD=[实际 Repeated DESIGN](/private/tmp/repeated-mult2-return-review.QGVKRy/repeated-mult2-bounded-basis-routing-probe-774fe2d/DESIGN_DECISION.md)。
已比较的既有发现：scale note51行、raised-basis78行、visible-source-recheck52行；[I/O handshake review](/Users/lifeng/Documents/20231788-openfhe-codex-lossless-io-01/coordination/LOSSLESS_REPEATED_H128_HANDSHAKE_REVIEW.md:28)；I/O 原返回 `REPEATED_MULT2_HANDSHAKE.md:24–38,40–71,75–88`。本文只补 phase 语义，不重开 PRE/key/packing/security 审计。

| 输入 | Bytes / whole-file SHA256 |
| --- | --- |
| P | 90235 / `60dd871a2769fddfe7ce7b2562d031d7c8d819a679eff3c2b6ebf3d7ea5769ae` |
| C @ d524dae | 51962 / `3ed23f2261cd7d8efea7a76a5724bfe44f6f20c521c20aedccd4fec69e6e916a` |
| H @ d524dae（18–34 的 enum/approximate fields） | 7560 / `3d089be507d7ea64e64b5ed3eb97b25ec249675c4da19e8e90d517151654bdfb` |
| RD（本次读13–95） | 8108 / `9c139ea61c755fe0b9f757b3a5c2488bd7a66ad351fac07ea722a4d026fba1db` |
| [O1] CKKS leveledshe.cpp | 36039 / `68fd392d9aaabe27e7ebf1e1cf354dd4c856da7bfac2d98afd5b6d796214f282` |
| [O2] rns-cryptoparameters.h | 61640 / `29e3d0666c41a36a4a7627fc4dd26487999ed2d81f6e0dd68046624275cb2340` |

O1/O2 来自已 verified official53 ZIP，pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`；archive SHA256 `8cc569e227c959c94c9289227a27f741536fab30e351d16aade5f65db5e31784`。O1 本次重验 size/SHA256/Git blob `f4e666685afa124d1515dbcfad23138307f964aa`；O2 复用同包已核验内容。没有本机 OpenFHE 或旧实现输入，没有新外部 reviewer 推论。
**未做：** source/test/config 改动、crypto smoke、第二至八次 execution、误差/no-wrap 证明及 paper reproduction。环恒等式、精确 rational 和阶段计数均不承担这些结论。

[O1]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp#L172-L190
[O2]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-cryptoparameters.h#L608-L648

## Root review and disposition

Codex 主审已全文复核本 note，并独立读取上述当前 DCP/Tensor2/Relin2/RS2/RCB、相关 validation、论文 Def3.3/4.3/4.5/4.7 与 §6.3，以及 pinned O1/O2 的实际 rescale/metadata getter 函数。阶段恒等式、塔数/level 表及纸面 rounding 例与这些来源一致；采纳为后续实现的修订依据，不采纳为运行证明。
主审另核对当前 src/header SHA256 与表中一致，且和默认分支 `08fe88e23c1090312457ea40d04dd88c75b92d0b` 的 src/header 无差异。Repeated 树仍保留其原来的 CMake/e2e tests，不能把默认分支的 57/57 声称为本树的新运行。
本轮仅新增本 note，source/header/tests/CMake/workflows 相对 `d524dae752e767486bb5737a69daed9ecf77d1d3` 未变。公开 client/setup/Mult2/I/O 测试接口确认尚未收到，因此没有写入或采纳新测试/实现。这里是 Codex 主审及独立 Codex 源码核对；不是 Pro 的修订回包，也不是 Fable5.1 审核，既有相关 Fable5.1 调用的 403/no-inference 结果未被改写。
