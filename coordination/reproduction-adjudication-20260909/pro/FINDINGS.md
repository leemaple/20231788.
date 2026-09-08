# 发现登记：不把证据缺口伪装成生产缺陷

分类：BUG=实际生产代码缺陷；GAP=证据不足；SCOPE=结论/条件边界；CLOSED=本轮或此前已关闭的义务。严重性针对复现判决，不等同软件漏洞等级。当前**已确认影响生产算术正确性的 BUG 数为 0**。

## F01 — P1 / GAP：当前公开 p 的全系数 Ecd 最近舍入对应尚未认证

**位置**：`project/src/high_precision_client_io.cpp:418–437,463–484`；`project/tests/s100_fresh_error_diagnostic_test.cpp:687–691`；`coordination/public-s100-encoder-cap-20260909/ADOPTED_RESULT.zh-CN.md:35–40`。

**证据**：StableRound 比较两条使用相同变换结构的 decimal160/220 路径、检查离半整数的距离和工作精度单位。这是有意义的工程拒绝门禁，但没有提供每个理想 a_j 的向外区间。fresh 诊断显式打印该数学前提未建立；新 cap 只认证实际 p 的幅度。既有全槽读出/小环正负控并未被忽略或抹去。

**具体反例**：原 p 的常数项理想值与实际值都为 −2^32。标量负控 p′=p+1 仍满足当前半径预算，却在常数系数上确定违反 Ecd。完整封闭式与执行结果见 SCALAR_PROOF 和 SCALAR_CHECKS_COMPLETE。

**受影响断言**：不能从新 cap 或双精度一致直接宣称“当前公开编码就是论文理想最近舍入编码”，也不能无条件采用 N/(2S) 全系数最近舍入界。

**处置**：唯一下一行动 PUBLIC-S100-ECD-CELL-01。提供独立区间逆嵌入候选，先审核、再 runner 执行；不改生产精度、舍入规则、输入或 E80。若产生区间分离的反例，才获得可定位的代码/输入桥缺陷候选；当前并无此反例。

## F02 — P1 / SCOPE：当前 nonwrap 已推进，原 S100 数值门槛未推进

**位置**：cap 采用结果 18–24、35–40 行；原 Linux/Windows audit 的 `status`、`maxima[id=E8]`；`project/tests/paper_full_eight_square_oracle.h:125–127,181–195`。

**证据**：本轮从保留 audit 有理数直接核对两平台 E8 下界均大于 2^-80；两条历史链各 chain_count=1、CTest exit8。原 cap run 没有密钥或八平方。当前 p 的幅度证明不能借源码片段桥直接贴到历史 p。

**处置**：保留原 FAIL；不写修复补丁，不通过新样本抽中通过者改变结论。F01 GREEN 只能关闭当前 Ecd 对应及条件预算前提，仍不能更改 F02。

## F03 — P2 / CLOSED：本轮确定两个真实理想系数，并给出条件预算

**位置**：原输入头文件 97–119；导出入口 55–58；本交付 `candidate/rounding_contract.py` 中 `closed_form_two_coefficients`；`tools/run_scalar_checks.py`。

**新结果**：实际 `p0=p16384=−4294967296` 精确正确；53 项最后一轮有限检查通过（包括两平台既有端点有理证据复核）。纯编码误差经八次理想平方的条件预算比 T 小于 0.424504491253。

**界限**：没有对剩余 32766 系数作变换；预算含 F01 前提，没有采样/PKE/Relin2/RS2/读出项。它不是新的“八轮 PASS”。

## F04 — P1 / NO-DEFECT-FOUND：未发现 Tensor2/Relin2/RS2 或尺度/family 的可修算术错误

**位置**：`project/src/double_ckks.cpp:398–438,826–892,894–1078,1080–1210,1213–1282`；`project/src/repeated_mult2.cpp:257–311,351–378,473–571`；官方 `keyswitch-hybrid.cpp:308–437`、`dcrtpoly-impl.h:693–712,966–1014`、`base-leveledshe.cpp:327–340`、`ckksrns-leveledshe.cpp:172–190`。

**核对结论**：真实 DCP 余数重构、三项 Tensor、dH 零塔提升、两坐标 ModDown、活动前缀密钥映射、两次 rescale 的 RCB 恒等式、d·m 精确尺度、同源秘密投影与重新包装均与已采用契约一致。FIXEDMANUAL 的同层 AdjustForMult 不暗中执行所缺的 d 除法。未把已有 carry 修订退回论文过强的可加性简化。

**受影响测试**：现有 DCP/RCB、Tensor2、Relin2、RS2、两次平方语义 oracle、客户端精确 receipt/错族负控；本轮仅审源码，不声称重新运行。这些有限工程测试也不是任意 profile 的全称正确性证明。

**处置**：无生产补丁。若没有具体违反上述调用关系的原件，重排 q_div 或再推“噪声太大所以 Relin 有 bug”不能成立。

## F05 — P2 / SCOPE：fresh 全槽独立证据已经存在，符号与样本不能混用

**位置**：`project/tests/s100_fresh_error_diagnostic_test.cpp:583–691`；`context/coordination/s100-fresh-error-repair-01/GREEN2_RESULT.md`；`context/current/coordination/s100-fresh-error-repair-01/FRESH_IDEAL_PROPAGATION.md`。

**证据**：fresh run 已对 m、初始相位、原始整数差作全槽 binary512/768 独立读出，检查十个 Horner 锚点；已有理想传播分析。这里的 A/B/C 是编码/PKE/读出；历史端点的 A8 是 evaluator 追加误差，两者不是同一项。初始密文相位误差使用 A+B，不加入读出 C；不能用新 fresh 的噪声对旧 E8 配对。

**处置**：退役“补一个全槽 fresh observer”“重做理想 fresh 传播”“再确认稠密 v”等建议；新候选不调用 fresh 诊断。

## F06 — P3 / SOURCE-NOTE：测试头历史注释不能替代当前调用关系

**位置**：`project/tests/paper_endpoint_transform_negative_contract.h:14–16` 注释称未接线/NOT RUN；当前 `paper_full_eight_square_contract_test.cpp:8,470–477` 已 include 并在 `--endpoint-observer-self-test` 调用 `transform_negative::Run()`。

**结论**：这是一条可以静态判明的旧注释滞后，不是当前测试一定没接线的证据，也不单凭调用代码证明历史已运行。应由保留运行证据判定执行，本文不据注释制造新缺失负控任务。没有为这项非阻断文字问题安排第二行动或生产补丁。

## 不再安排的已完成/已否定建议

| 旧建议 | 已有处理与本次处置 |
|---|---|
| 重新取得当前 p 的幅度以实例化 A04 | run34265676284 已完成，本轮只标量验端点，不再变换 |
| 再推初始 DCP 兼容代表与八轮 half-modulus | 已采用，现有 cap 填入条件；没有具体反例则不重开 |
| 再笼统声称 Relin 可加/忽略 carry | 已被源码特定合同修订，禁止回退 |
| 重做 full-slot fresh A/B/C 与理想传播 | 已做；选中的是理想 Ecd 舍入区间，而非读出重复 |
| 重查 h128 是否等于 v 稀疏 | 已明确不等；不能当新根因 |
| 只查历史 OpenMP 是否找到 | 四个 S100/S116 作业已确认 ON/找到；局部 sampler 实测和因果性另有限定 |
| 再做 annulus 或把 S116 通过合并进原结果 | 改条件，不是修复；本轮不安排 |
| 无区别重跑原链、1000 次取均值/挑样本 | 没有判别力或违背用户边界；不安排 |

没有意见一致、旧 CI 徽章或新 scalar PASS 被记作本轮加密测试通过。
