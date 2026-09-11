# 两个同密文负对照

## 类型和边界

这两个是可控的**错误输出实现**：复制真实公开加密返回值后，只修改 c0，模拟错塔写入或额外一致偏置。它们不修改生产源码，不修改 oracle，不重抽 key/noise，不证明生产中的某一行已经写错。源代码中的 AddPublicConstant、ExpectRejected 只属于新测试。

`--controls` 在一个进程中先完成诚实基线，再对该基线的两个值副本分别注错，最后重验原基线未变。参数、密钥、公钥、明文、密文 c1、原始随机量、阈值、观察方法都固定。每个 mutant 只有一个注错因素。

| 负对照 | 精确改动 | 必须出现的检查失败 | 为什么必然不是合法噪声或等价变异 |
|---|---|---|---|
| M1：one-tower-plus-one | 仅 q0 塔的 c0，所有 EV 坐标加 1 mod q0 | `FRESH_CRT_COHERENCE` | 常数 1 的所有求值都为 1，所以只改变该塔的第 0 个系数。诚实 f 各塔相同且远离 q/2；一塔变 f0+1、另两塔不变，必不一致。不能只改 EV[0]，那不代表常数系数。 |
| M2：all-towers-plus-30031 | 全部三塔的 c0，所有 EV 坐标加 δ=2F+1=30031 | `FRESH_SUPPORT_BOUND`，而非 coherence | 第 0 个系数变为 f0+δ∈[F+1,3F+1]。各塔仍一致，且 q_min>2(3F+1)，不会因中心取模绕回支持界内。只检测跨塔一致的弱测试会漏掉这个 mutant。 |

F=15015 来自 pinned 成功 DGG 支持界，不是从基线误差拟合。测试先检查 coherence 再检查 support，因此预期失败类别确定。基线没有先通过全部相位/公开解密检查时，**不得**执行/计分 mutation。

## 默认一次运行

focused CTest 执行 `initial_phase_exact_contract_test --controls`。根端需保存完整 stdout/stderr 和退出码；期待有一条 baseline=PASS 和两条不同原因的 status=REJECTED，最后 slice=PASS，退出 0。这表示“测试拒绝错误副本”，不是两个 mutant 通过数值检查。

下面是预期示意，**不是本地或远端实测日志**：

```text
baseline=PASS profile=initial-phase-n256-h128-v1 N=256 towers=3 H=128 G=39 F=15015
mutation=one-tower-plus-one status=REJECTED reason=FRESH_CRT_COHERENCE
mutation=all-towers-plus-30031 status=REJECTED reason=FRESH_SUPPORT_BOUND
slice=PASS originalS100=UNCHANGED source_base=... dependency_pin_declared=...
```

## 需要独立 RED 退出码时

同一 executable 另提供 `--fault-one-tower`、`--fault-coherent`。每个模式仍先在同一进程完成基线，再放任相应 NumericFailure 到 main，预期退出 **1**，stderr 分别是：

```text
NUMERIC_FAIL FRESH_CRT_COHERENCE
NUMERIC_FAIL FRESH_SUPPORT_BOUND
```

这是替代捕获形式，不是要求追加随机试验。通常只需默认 `--controls` 一次；只有 root 的接收系统需要 RED 进程码时才选择这两个模式，不把它们注册成 WILL_FAIL 以免任意失败都被误报为成功。Linux 可运行后检查 `$?`；PowerShell 检查 `$LASTEXITCODE`。必须同时匹配基线 PASS、确切故障标签和退出 1，不能只检查“非零”。

## 分类和停止

预期 NumericFailure 被捕获才算检出。`CONTRACT_FAIL`/退出 2、`UNCLASSIFIED_FAILURE`/退出 3、构建失败、链接失败、信号退出和超时全是 **未完成/其他失败**，不是数值检出。第三方异常文本被刻意隐藏，不能在本进程直接扩大异常打印来调试秘密对象。

如果 mutant 存活、标签错误或 mutation 改坏了基线，停止且检查测试本身；不调整生产参数、不重复抽种子。若诚实基线与两个负对照符合预期，该切片的问题已答完，停止。没有 mutation campaign、覆盖率 KPI、千次试验或性能结论。
