# 初始化与表示路径的现有覆盖

以当前 `33722b9` 的实际调用者为准；“已有”指测试源码或所附历史记录，不是本轮重新执行。源码定位见 SOURCES.md。已阅读参数图谱根 REVIEW_NOTES，使用经勘误的 reference 版，不把 132 参数图谱缺失当成新发现。[S12]

| 高风险消费路径 | 已有控制 | 精确剩余缺口 / 本次处置 |
|---|---|---|
| `HighPrecisionClientIO::InspectEncoding/ComputeEncoding`：scale、round-to-nearest、signed 系数、gap | 当前原输入 p 全 32768 系数最近舍入证书；S100 controls 有符号、半整数拒绝、lift、observer-order；N64 public client 有独立插值和门禁 | 当前证书不是原历史 p 身份，也止于 PKE 前。**不重跑证书、不重新提出 observer-order 或 cap 检查。** [S01,S09–S11] |
| `Poly(BigVector)→DCRTPoly`：整数宽度、COEFF/EV 标志 | 当前 Encrypt 确实走官方构造器；N64 高精度客户端已经真实使用；构造器源码保留 rhs format | 既有 h128 smoke 输入走 double plaintext，没有本文的 >64 位有符号精确逐塔 oracle。新切片直接测这个公开构造器，但不声称覆盖 HighPrecisionClientIO 的 paperN 包装器。[S01,S03,S06] |
| `CreateFixedQH128ClientKeyPair`：完整 Q、公钥次序、secret 支持 | 身份/tag、fullQ/EV、h128、正负平衡、跨塔 secret 一致、context/eval cache 不变、非法 profile 拒绝；1e-5 roundtrip/square smoke | 尚未在此夹具用**不共享官方 INTT**的 oracle 检查 `pk0+pk1*s` 全系数支持与相干性。本次补这一检查，不把现有 h128 检查说成不存在。[S02,S03] |
| 官方 public PKE：dense v、e_pk/e0/e1、noiseScale | 图谱已分清 sparse secret 与 dense v、DGG 源码和 PRNG；S100 fresh 已在同次输入/密文上分离 m 与 phase−m | 本次检查真实公开 PKE 输出的**必要**整数不变量，不窥取 v/e0/e1、不声称验出采样分布；保守 bound PASS 不能排除“小但有偏”的错误。[S04,S06,S09,S12] |
| Native EV/root/bitreverse → 系数 | 官方 transform、DCP/RCB/Tensor 等精确整数 tests；现有 paper `SparseDecrypt` 独立卷积/CRT | `paper_full_eight_square_oracle.h:210,240` 仍调用官方 INTT；正逆共享可能掩盖同错。新切片的公开 Horner 与直接逆求值独立于官方 transform。固定 clean-process N256，不等于历史缓存竞争回归。[S07,S08,S10] |
| 公开 `Scheme->Decrypt(...,Poly*)` 与中心提升 | 客户端显式调用 Poly*，保留解密模式，检查格式/模数/中心系数；fresh 与 endpoint 有独立高精度观察 | 新切片把**同一密文**公开解密与独立求得的唯一 p+f 整数逐系数精确比较；不要求 Dec(Enc(p))=p。[S01,S05,S09] |
| DCP/RCB、Tensor2、Relin2、RS2、scale/receipt | DCP exact oracle；Tensor 的负循环整数 oracle；Relin 结构/键/后续行/immute 广覆盖；Mult2 composition、N64 客户端与两平方 | 不是“没有精确算术测试”。本切片不重复它们，不按不成立的 Relin 线性或精确近似乘法性质新增测试。[S10,S14；`project/tests/relin2_test.cpp:4255–4421`、`mult2_e2e_oracle_test.cpp`] |
| fresh→八平方 I/A/局部误差与最终 E80 | 原 full8 同一 ciphertext 的 freshPowers / previousAnchors / actual；完整 endpoint；原两 FAIL 的历史记录 | ten-anchor replay 不是 live regression，也没有原历史全相位对象。本次不复制已有 I/A 链，不将新夹具 PASS 合并进原 frozen acceptance。[S08,S11] |
| OpenMP privateDGG / NTT cache 前提 | atlas 已指出；后继 build-provenance 已确认原 S100/S116 实际启用 OpenMP；cache 混根缺历史证据 | 不再建议“先看是否只设置了 OMP 请求”，不据此改 σ。新测试不生成 eval keys，不声称测试 privateDGG，也不向进程灌入替代根。[S12,S13] |

本轮新增覆盖仅沿一个短路径：公开 h128 初始化 → 精确大整数导入 → 一次 public PKE → 同一密文的独立相位/公开解密差分。不是新的公共 API、参数族或全局性质测试框架。
