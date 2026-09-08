# 独立发现、严重度与处置

对象：当前供件 a7f54de。P1 表示可能使总体验收失真的重要问题；P2 表示范围有限的文档／证据表达问题；LIMIT 表示必须披露但本轮不据此新增任务的限制。这里不把既有算法近似误差或印刷错误伪装成新生产 bug。

## F01 — P1／验收合同：单一“完整复现”标签承载了不同命题

**证据。** `TASK.md:15–19,35–41` 同时要求充分判断实现、保留原冻结门禁、免除1000次统计。`CHECK_AND_HANDOFF.zh-CN.md:9` 将表3同源材料与不能声称完整复现连在一起；`coordination/annulus-independent-pro-review-20260908/RETURN_DISPOSITION.md:60–62` 没有区分工程判断与同源统计主张。若“完整”指同源原实验，这些历史话并非错误；若用其阻断任何算法判断，则没有相应用户／论文依据。

**判别。** Ecd 严格证书与公开算法对应不依赖作者具体输入法则；但原 S100 已失败也不依赖作者法则。移除不当统计总门禁既不能撤销已经取得的工程证据，也不能将原FAIL改成PASS。

**处置。** 本合约拆开五类判断，并维持 `OVERALL_FROZEN_ACCEPTANCE=NOT_ALL_PASSED`。文档候选把该语义写到当前入口。不改原测试为 expected-fail，不降低目标。

## F02 — P2／确定文档缺陷：入口“最新”仍指旧分支

**位置。** `CHECK_AND_HANDOFF.zh-CN.md:7` 的最新分支为9月8日 annulus 分支；当前 `TASK.md:7` 和根 `MANIFEST.json` 则为 `codex/public-s100-ecd-cell-20260909`／a7f54de。两者不能同时作为当前包入口。`REPRODUCE.zh-CN.md:3` 已有 S116 历史限定，但 `:11` 的最重要验证句单独摘引时没有就地指出改参。

**处置。** 提供 `docs/01-completion-labels.patch` 和两份完整新文件。新顶栏固定供件身份、原两个FAIL、各独立PASS与证书限制；原9月8日及9月7日内容显式改作历史。S116验证句补充就地限定。不改原日志、既有Pro原件或计算文件。

**实际检查。** 旧顶栏不具备冻结的新合同字段，文档检查 RED；候选具有全部字段且保留历史，GREEN。这个 RED 是文档合约缺失，不是原生产程序执行失败。

## F03 — P1／已知未达数值要求，不是新代码缺陷

**位置。** 原 `LINUX_AUDIT.json`、`WINDOWS_AUDIT.json` 均为完整一条链、CTest exit8、E80 FAIL。原 oracle `project/tests/paper_full_eight_square_oracle.h:182–195` 使用实／虚部最大分量门禁，`paper_full_eight_square_contract_test.cpp:425–433` 保存证据后仍强制数值失败。

**本轮有界核对。** `evidence/BOUNDED_CHECKS.json` 中，原 E8 分量/T 约为11.057408和10.959281；原 I 的有理下界超过T。保留观察模型下的 `I_lower−A_upper` 分量范数下界分别超过11.00448T、10.85928T。符号关系使用同一E8最坏槽行检查，不把独立最大值当作相同观测。

**处置。** 两FAIL原样保留。不写无规范反例的“修复补丁”，不宣称无法存在任何未来改善。

## F04 — 已关闭的正确性缺口：当前 p 与理想 Ecd 的对应

**证据。** 当前 `green-evidence/execution/certificate/RESULT.json`，独立 `MATH_ADOPTION_REVIEW.md:82–169`，root intake及全文件哈希。32768 CERTIFIED／0 REFUTED／0 INCONCLUSIVE。它不是之前只有幅度cap的证书。

**处置。** 当前 p 的编码对应及纯编码预算可采用；不重复验证动作。当前cap／Ecd与已采用相容提升契约联合使用，不能逆向认证旧p或旧FHE状态。旧 `conditional_spectral_certificate.json` 中的历史 `premise_actual_encoding_canonical_bound_verified=false` 不应修改；它描述产出当时的前提状态，后继证书负责满足前提。

## F05 — 已解决的候选封套问题，不能当成当前未修缺陷

**位置。** `coordination/reproduction-adjudication-20260909/CANDIDATE_REVIEW.md:25–47` 的 R1/R2 是原 shell输出排除与status/exit遗漏。现行 `coordination/public-s100-ecd-cell-20260909/harness_contract.py:32–53` 做真实路径排除及精确整数状态映射；实际 `OUTCOME_AGREEMENT.json` 与捕获退出码一致。根接收复核明确 actual run不调用旧shell。

**处置。** 关闭于现有 successor。没有运行或修改原候选，不复制其历史 TODO 为新任务。新文档讲清“不可变旧作者文件”和“实际现行运行入口”的区别。

## F06 — 无生产修复依据：算法构造与印刷歧义已区分

**源码。** Tensor2正交叉项 `double_ckks.cpp:826–892`；Rel(dH)在DCP之前 `:1011–1054`；RS组合项 `:1137–1188`；实际除dμ尺度 `repeated_mult2.cpp:257–311`。固定官方 `keyswitch-hybrid.cpp:381–398` 对两个坐标分别 ApproxModDown。

**反例依据。** 已有独立 `coordination/annulus-independent-pro-review-20260908/pro/REVIEW.md:178–186` 给出普通Tensor印刷负号、Thm4.8漏d和舍入加法中间向量式的精确标量反例。它们不是当前生产代码RED。已采用Relin契约也没有使用这些不成立的线性简化。

**处置。** 不按印刷歧义修改当前实现，不把缺少低低项当作漏实现，不给出伪造TDD补丁。

## F07 — LIMIT／来源与量词边界

当前三个生产TU与图谱a4b815a哈希相同；第四个高精度I/O移除独立公开probe增量后字节一致。本轮确实复核了这个文本关系。源片段桥 `SOURCE_BRIDGE_REVIEW.md:25–44` 仍不证明旧编译器／Boost／舍入整数完全相同。

annulus独立标量重放验证的是错误记录及传播，不是重新解密未保留的旧密文（`ROOT_SCALAR_REVIEW.md:26–32`）；当前p是一个确定对象，不是所有输入的证书。源码中stable-round对不确定输入明确拒绝（`high_precision_client_io.cpp:418–496`）。当前API的诚实状态和所有权检查不是对恶意密文的认证。

**处置。** 本合约列为条件，不宣传“普遍正确／普遍E80”。无具体反例时不自动启动新的平台或范围扩展任务。

## F08 — LIMIT／最新回归原记录副本不齐，不制造重跑

当前TASK:39、交付说明:49–52及独立Pro `REVIEW.md:170–172` 均支持run34116227668的Linux60/60与一次keylesscontrol。该独立报告引用的 `coordination/s100-output-finalization-01/GREEN_EVIDENCE.txt` 原实体不在本包；本轮不能声称直接查看了那个原文件。已有S116双平台 `RUNTIME_REVIEW.md:24–27` 和其他当前源码／历史审计仍可用。

**处置。** 以审计记录级而非“本轮重放原日志”级表述。精确缺项如上，但不以重新运行代替历史证据，也不把该缺项升级成一项必须做的新FHE任务。报告不称a7f54de刚通过最新双平台全套。

## F09 — LIMIT／安全、作者来源与统计的真正位置

`PAPER_EXPERIMENT_PROVENANCE.md:24–66` 记录同源输入法则、具体分布、版本和实验API未闭合且查找已停止。`build-provenance-20260908/ASSESSMENT.zh-CN.md:7–25` 已补四个历史作业OpenMP配置事实，而annulus缓存二进制唯一身份仍有条件。两者都不能成为未经来源支持的噪声修改理由。

**处置。** 同源Table3主张仍缺资料；安全级别仍未认证；用户不需要1000次、不允许作者联系。不给出部署认证或原作者统计“重现成功”，也不把这些未知笼统写成全部工程工作无法判断。

## 结尾

本轮发现需要修正的是验收表达与快照入口；没有发现一个有规范证据、未处理且可提交生产RED的源码缺陷。算法肯定判断依赖主报告列出的正向源码／数学／测试证据。全冻结数值项仍未全通过；本轮静态与标量审查结束，不自动产生新加密实验。
