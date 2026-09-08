# 唯一下一行动：PUBLIC-S100-ECD-CELL-01

## 1. 要裁定的唯一问题

对已保留的公开 `public_s100_encoding.json`（SHA256 `7cac9765…dd94f`；完整值见代码常量），检查全部 32768 系数是否严格认证为论文定义的

```
p_j = nearest_ties_down(2^100 · σ⁻¹(z)_j),
z = 原冻结 Input(s)，不是 annulus，不经 binary64，slot 顺序为 ξ^(5^s)。
```

不重新取得 p，不重新导出系数，不做 fresh 分解，不重跑幅度正向证书，不编译或链接 OpenFHE，不运行任何加密、解密、NTT、sampler 或八次密文平方。所需完整逆变换只属于本切片的独立公开观察器，不属于生产 Ecd。

## 2. 责任和依赖

Codex 独立审核本包数学推导、源公式转录、区间原语来源和候选接口后，拥有一次隔离 GitHub-hosted Linux job 的执行和采用职责。作者本轮已完成源码/标量部分，**没有执行候选 tiny/full 变换，没有派发 CI，没有替 Codex 宣布审核通过**。

选用 ubuntu-24.04、Python 3.12.x（记录实际 patch 版），只需标准库。所有数字操作为整数/Fraction，固定网格 224 位；禁止自动加精度重试。环境版本选择不是“最新版本”推荐，而是与已有公开认证所用 Python 系列一致的冻结执行约束。

输入、来源和代码全部随交付，运行前校验根 manifest 与 `source/SOURCE_BINDINGS.json`。不需要仓库私有访问、旧聊天、OpenFHE 安装、网络取得数学库或作者初始化资料。不能直接复用原 dcp-rcb 广泛工作流，以免带起旧加密链；应在专门隔离的已审 job 中只执行下述脚本。

## 3. 文件和接口

`candidate/rounding_contract.py`：固定 p 身份、原精确输入公式、舍入区间分类器和两个封闭式系数；导入无变换。

`candidate/interval_core.py`：原样抽取已供 cap 检查器 39–140 行的向外区间基本算术、π 和小角种子；不是新的生产数学库。不独立于旧 cap 的基础区间原语，但独立于生产 Boost special transform。

`candidate/check_rounding.py`：普通负号 radix-2 DFT、Hermitian 完成、ξ⁻ʲ twist 与 S/N 归一化；四项 tiny 模型控制及一次完整认证。提供两种互斥模式：

```
--controls --allow-reviewed-transform --output-dir NEW_DIR
--certify --allow-reviewed-transform --output-dir NEW_DIR
```

每次创建全新目录和 STARTED 文件；已存在即拒绝，失败不删除标记。完整模式不接受替代 N、S、输入、p 或精度参数。代码没有导入 OpenFHE、Boost 或生产编码函数。

`tools/run_scalar_checks.py`：本轮已经执行的 53 项有限精确检查；再次在 runner 上只用于来源/语义负控门禁，不算新样本。

`tools/verify_rounding_result.py`：不运行任何变换，只重新核对全部系数行覆盖、区间分类、margin、数据哈希与结果状态。它是证书记账复核，不替代对逆变换证明和实现的独立审核。

`candidate/run_reviewed_once.sh`：显式审核开关、GitHub/Linux/attempt1 门禁、immutable delivery 校验、控制先行、单次完整认证、退出码保留。审核开关仅是执行门禁文字，不是独立审核的密码学证明。

## 4. 冻结预期与 RED/GREEN 顺序

首先锁定代码和真值，再执行。不把候选实际输出作为预期生成器。

| 阶段 | 输入及真值来源 | 预期 |
|---|---|---|
| 身份/标量 | 原公开 p、原证书有理端点、原输入封闭式 | 53 项检查通过；p0=p16384=−2^32 |
| 语义 RED 负控 | 原身份检查通过后，在标量内存中用 p0+1、p0−1、符号/尺度变异 | REFUTED；不能靠文件哈希失配替代数值检出 |
| 半整数负控 | 固定正负整数处的开左、闭右、跨界区间 | 正确半向下分类；跨界必须 INCONCLUSIVE |
| tiny GREEN | N4/N8 的常数+中间单项、N4 的 X、X³；解析谱给定，√2/2 由整数 isqrt 包围 | 所有预定理想系数被包含，虚部含零，宽度<2^-100；X³ 符号变异被拒绝 |
| 完整实际 p | 原 dyadic z 的一次独立逆嵌入；不是重新编码 p | 结果事先未知，只允许下面四类判决，不预填 GREEN |

这里的 RED/GREEN 是**新增观察器的语义测试和有限证书验收**，不是声称原生产代码已有一个本轮实际 RED，或已经有生产修复 GREEN。负控被拒绝是测试通过；真 p 完整证书仍未执行。

## 5. 完整数学与输出合同

令 ξ=e^(πi/N)，将 z_s 放入 k=(5^s mod2N−1)/2，并把共轭放在 N−1−k。完整槽数组 y 的逆 DFT 得到

```
a_j=(S/N) ξ^(−j) Σ_k y_k exp(−2πijk/N).
```

使用 outward 整数区间，所有真实 a_j 必须在返回实部区间内，虚部区间必须包含 0。每个整数 p_j 的舍入原像为 `(p_j−1/2,p_j+1/2]`。不得仅检查“有交集”；完整 GREEN 还要求每项 strict margin>0。精确边界不能通过增加浮点位数自动变成假定通过。

保留 `coefficient_cells.tsv`：连续 j=0…32767、实际 p、实/虚包围的整数端点、统一分母 2^224、分类、margin。`RESULT.json` 必须绑定输入/整数流/candidate/core/table SHA256，计数为 32768 项，245760 butterflies、32767 root multiplications、完整 inverse=1；新生产 encoding=0、FHE=0、采样=0、链=0。这些计数由已审源码路径和 job 步骤佐证，不单凭 JSON 自述接受。

完整模式必须通过已知 a0=a16384=−2^32 的外部解析控制和 p0±1 语义负控，不能在发现与解析系数不符时把它直接指责为生产编码缺陷。tiny/core 错误先归 ORACLE_INVALID。

## 6. 精确 runner 命令（仅在独立审核之后执行；本轮未执行）

在隔离 GitHub-hosted Linux job 中把本交付 ZIP 放于 `$RUNNER_TEMP/REPRODUCTION-ADJUDICATION-01-delivery.zip`，已配置 Python 3.12.x，解压目录此前不存在：

```bash
set -euo pipefail
umask 077
python3 -c 'import sys; assert sys.version_info[:2] == (3,12)'
python3 -m zipfile -e \
  "$RUNNER_TEMP/REPRODUCTION-ADJUDICATION-01-delivery.zip" \
  "$RUNNER_TEMP/ecd-cell-adjudication"
BUNDLE="$RUNNER_TEMP/ecd-cell-adjudication/REPRODUCTION-ADJUDICATION-01"
python3 -B "$BUNDLE/tools/verify_delivery.py"
# 下行只能在 Codex 已独立完成审核后设置。
export ECD_REVIEW_APPROVED=REPRODUCTION-ADJUDICATION-01
bash "$BUNDLE/candidate/run_reviewed_once.sh" \
  "$RUNNER_TEMP/public-s100-ecd-cell-$GITHUB_RUN_ID-$GITHUB_RUN_ATTEMPT"
```

job 应使用 always 条件保存完整输出父目录，即便脚本返回非零；包括 STARTED、ONCE_RESERVED、各 stdout/stderr、退出码、版本、全部区间表和 RESULT/FAILURE。不能删输出目录或换 run_attempt/输出名再次尝试获得通过；owner 在执行前登记本切片一次额度。独立审核发现代码问题可以在运行前修订并重新冻结包；任何修订必须有新 manifest，并不得冒称与本候选字节相同。

shell 会在标量或 tiny 控制失败时停止，不执行完整变换。完整结果 exit=3/4 虽可通过记账 intake，脚本仍保留该非零退出码，不用 intake PASS 覆盖数值状态。未提供触发现有仓库工作流的 tag/dispatch 指令，避免授权外状态变化。

## 7. 停止规则及其对最终裁定的影响

| 结果 | 判据 | 可以得出的结论 | 不能得出的结论 |
|---|---|---|---|
| exit0 / ECD_ROUNDING_CERTIFIED | 所有32768项 CERTIFIED、严格正 margin、虚部/均值/负控/来源均通过 | 在已审区间数学下，这份 p 恰是原 z 的理想最近舍入 Ecd；可采用 SCALAR_PROOF 的 η≤2^-86 及纯编码八平方预算 | 不能称原 E80修复、历史p相同、新密文PASS、Table3同源复现或安全认证 |
| exit3 / ECD_ROUNDING_REFUTED | 至少一项整个理想包围落在 p_j 舍入区间之外，控制/来源均有效 | 保留该 j、p_j、理想包围为有限反例；当前 p 与规格不一致，需要沿固定输入桥/编码路径定位 | 不能未经分析立刻宣称它解释原 E8，也不能自动提高精度或改单元真值 |
| exit4 / INCONCLUSIVE_INTERVAL | 有跨界项或最小严格 margin≤0 | 本次固定预算未裁定，保留精确失败/跨界项后停止 | 不能认定生产错或对的；不自动再跑更高精度 |
| exit2/控制失败/超时/基础设施失败 | 哈希、解析控制、虚部、代码环境或完整输出不成立 | 新观察器或运行证据无效，停止并提交原记录 | 不能把它算作原生产测试 FAIL/PASS，不能换样本 |

REFUTED 后的修复定位不是本次另行授权的第二任务；它只是本行动输出对后续决策的条件分支。GREEN 后也不在此文自动派发新的加密实验。**执行一次可判别的现存对象检查后就停止。**
