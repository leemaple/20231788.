# 固定证据索引

除 [W1–W3] 外，以下路径均相对已核验输入 ZIP 的根目录，行号是 UTF-8 源文件的物理行。不是对未附文件的推测。输入根 MANIFEST 已核验；本包不重复分发官方库、论文或历史密钥材料。

| 编号 | 固定材料与定位 | 本轮使用的内容 |
|---|---|---|
| S01 | `project/src/high_precision_client_io.cpp:463–497,598–637,704–756` | Ecd 的整数/剩余类生成；Poly→DCRT 构造器；公开 scheme Encrypt/Poly* Decrypt 消费路径 |
| S02 | `project/include/openfhe_2023_1788/paper_h128_client_keypair.h:7–20`; `project/src/paper_h128_client_keypair.cpp:193–219` | 现有公开 h128 客户端入口与公钥的 `(a*s+e,-a)` 次序 |
| S03 | `project/tests/paper_h128_client_keypair_contract_test.cpp:33–146,211–290` | 原封不动复用的 N256 参数夹具/检查；现有 secret 检查和 1e-5 smoke 的实际范围 |
| S04 | `official/src/pke/lib/schemerns/rns-pke.cpp:56–69,111–197` | PKE 公私钥零加密、实际 DGG、dense ternary v、明文加入位置 |
| S05 | `official/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp:70–95` | FIXED_NOISE_DECRYPT + EXEC_EVALUATION 的 Poly* 解密不另加 flooding |
| S06 | `official/src/core/include/lattice/hal/default/dcrtpoly-impl.h:59–83,126–149,178–192`; `official/src/core/include/math/discretegaussiangenerator-impl.h:60–114`; `official/src/core/include/math/discretegaussiangenerator.h:79` | 构造器与赋值算子 format 区别；DGG/TUG 跨塔复用整数向量；Peikert 成功样本支持界、Karney 门限 |
| S07 | `official/src/core/include/math/hal/intnat/transformnat-impl.h:200–242,303–375,648–755` | bit-reversed negacyclic EV 次序；公开 transform 调用及按 q 缓存的条件 |
| S08 | `project/tests/paper_full_eight_square_oracle.h:1–7,200–263`; `project/tests/paper_full_eight_square_contract_test.cpp:258–370` | 既有独立 CRT/稀疏卷积仍共享官方 INTT；同一密文已有 fresh^256 与实际链的 I/A 分解 |
| S09 | `project/tests/s100_fresh_error_diagnostic_test.cpp:270–411,467–561,564–695` | 既有 signed/lift/observer-order 控制与一次加密的 m、phase、phase−m 分解 |
| S10 | `project/tests/dcp_rcb_test.cpp:434–528`; `project/tests/precision_client_io_first_mult2_contract_test.cpp:129–147,724–835`; `project/tests/tensor2_test.cpp:149–169` | 既有准确算术、客户端误差门禁，以及 oracle 中仍有官方转换的实际消费者 |
| S11 | `coordination/completion-contract-20260909/pro/FINDINGS.md:21–33,49–67` | 原两 FAIL 与 I−A 历史判断、当前 p 全系数证书的适用边界；这是所附审计记录，不是本轮新运行 |
| S12 | `docs/parameter-atlas/REVIEW_NOTES.zh-CN.md:1–79`; `docs/parameter-atlas/reference/RANDOMNESS_PATHS.md` | 必须读经勘误 reference 版；根缓存不等于已发生错误，h128/v 和 OpenMP 条件已有覆盖 |
| S13 | `coordination/build-provenance-20260908/ASSESSMENT.zh-CN.md:7–27,37–42` | 原 S100/S116 已实际启用 OpenMP 的记录；不能再称仅是 workflow 请求；不能据此改 σ |
| S14 | `coordination/initial-lift-nonwrap-20260909/ADOPTED_CONTRACT.md`; `coordination/relin2-bound-20260908/ADOPTED_CONTRACT.md` | 已有初始化支持界与后续相容提升/Relin 条件；本轮不声称首次发现这些理论界 |
| S15 | `references/paper/PAPER-2023-1788.pdf`，PDF 第 4 页 §2.1；第 12 页图表 | R=Z[X]/(X^N+1)、加解密方程、系数/典范范数区别；论文统计试验不替代逐样本 E80 门禁 |
| S16 | `project/CMakeLists.txt:1–37,111–121,303–365`; `SKILL_RESEARCH.md` | 固定库版本检查、HEAD 元数据及已有 opt-in 组织；本次仅追加 opt-in 目标 |

本轮在线核读的测试方法，均固定到输入指定的 commit，不把其建议当作密码学规范：

[W1] Trail of Bits, property-based-testing, commit `321ccfe628eca0d314b0ee4eaffcdd8a05639aaf`。使用“有前提的非空性质、独立 oracle、区分错误性质与错误实现”。
https://github.com/trailofbits/skills/blob/321ccfe628eca0d314b0ee4eaffcdd8a05639aaf/plugins/property-based-testing/skills/property-based-testing/SKILL.md

[W2] ECC, cpp-testing, commit `c9148d0bb239ed01a95724a5928b98cdf9c30658`。使用真实入口、已有 C++/CTest、小目标；不引入 GoogleTest 或新框架。
https://github.com/affaan-m/ECC/blob/c9148d0bb239ed01a95724a5928b98cdf9c30658/skills/cpp-testing/SKILL.md

[W3] Matt Pocock, diagnosing-bugs, commit `3cca18b368ae95cdbdebbff572ccafa662551015`。使用可被错误实现触发的失败检查、单变量负对照与明确停止条件，不采用千次试验建议。
https://github.com/mattpocock/skills/blob/3cca18b368ae95cdbdebbff572ccafa662551015/skills/engineering/diagnosing-bugs/SKILL.md

论文在线 landing page 已读取；PDF 在线抓取和 web screenshot 均失败，因此以已核验的附件 PDF 为准，并实际渲染、目视检查第 4、12 页。没有用网页新版本替换输入论文或固定官方库。
