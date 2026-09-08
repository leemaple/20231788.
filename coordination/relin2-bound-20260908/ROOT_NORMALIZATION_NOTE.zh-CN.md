# 独立补充：条件性 Relin 局部误差界的八轮尺度核算

2026-09-08，根端独立核算，Pro 同一推导任务仍在运行；本文件没有发进它的思考上下文。

**状态：仅验证条件公式的代入与归一化，不是已接受的 HYBRID 定理，不是 FHE 精度结果。**

## 核算什么

此前 `ROOT_ALGEBRA_DRAFT.md` 的 B 节给出一个尚待独立证明核验的普通 HYBRID 上界。设当前 active Q 素数集合为 A，N=32768，h=128，假设每个 eval-key 噪声系数绝对值不超过 E=39，单辅助素数为 P，则该候选 canonical 上界为

    B(A) = N * [N*E*sum_(q in A)(q-1) + (1+h)*(P-1)] / P.

这里仍要求 alpha=1、单 P、noiseScale=1、诚实密钥、共同的小秘密整数提升、正确模运算与相应 sampler 前提。E=39 是此前根端草稿的条件性支持界，不是本轮读取或测量了历史密钥噪声。

Relin2 的 high 调用使用 A∪{d}，low 调用使用 A。因此重组相位的局部候选界是 `B(A∪{d})+B(A)`，不能假设两次误差独立或相消。

RS2 的源码构造 `newLow = rescaledRecombined - d*rescaledHigh`。故在输出模环中，重组 RS2 的结果恰好等于 `rescaledRecombined`。在能够使用相容数值提升并单独记录 rescale 舍入项的条件下，**仅 Relin 所贡献的局部线性误差项**的归一化上界为

    b_k = [B(A_k∪{d})+B(A_k)] / (m_k*S_k)
        = [B(A_k∪{d})+B(A_k)] * d / S_(k-1)^2,

其中 `S_k=S_(k-1)^2/(d*m_k)`，`S_0=2^100`。不得在第一式的分母额外再乘一次 d。RS2 自身的舍入误差不包含在 b_k 中。

## 实际检查与结果

`check_root_normalization.py` 先核源码 SHA256，再从冻结的 S100 素数表提取有序 Q/P，使用 Python 标准库 Fraction 执行八轮精确有理数运算。每轮检查两种归一化表达式完全相等、额外除以 d 的错误表达式不相等，并用整数比较给出二进制包围区间。小数列仅用于阅读；区间关系用精确有理数判定。

| 轮次 | active/raised Q 塔数 | b_k 近似值 | 精确核验的上界 |
| --- | --- | --- | --- |
| 1 | 10/11 | 4.585598772362e-37 | ≤ 2^-120 |
| 2 | 9/10 | 4.012534616015e-37 | ≤ 2^-120 |
| 3 | 8/9 | 3.439463898272e-37 | ≤ 2^-121 |
| 4 | 7/8 | 2.866383336936e-37 | ≤ 2^-121 |
| 5 | 6/7 | 2.293289646957e-37 | ≤ 2^-121 |
| 6 | 5/6 | 1.720182816894e-37 | ≤ 2^-122 |
| 7 | 4/5 | 1.147075940151e-37 | ≤ 2^-122 |
| 8 | 3/4 | 5.740214280446e-38 | ≤ 2^-123 |

实际命令：

```text
/Users/lifeng/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/bin/python3 -B -I coordination/relin2-bound-20260908/check_root_normalization.py
```

2026-09-08 23:54 CST 执行一次，exit 0；全部八行的等式、负向检查及精确区间检查均完成。结果保存在 `ROOT_NORMALIZATION_CHECK.json`，带脚本与输入源码 SHA256。无 OpenFHE 调用、编译、FFT/NTT、随机采样、CI 或秘密数据读取。

## 对排错的有限意义

这个计算说明：**如果候选普通 HYBRID 界及数值提升前提成立，则正确归一化后的本轮局部 Relin 项可以非常小；“系数界很大”本身不能推出明文误差界无用。** 这为接收 Pro 推导时提供了独立的量纲和数量级核查。

但不能据此宣布 Relin 实现无错或原 S100 已通过。候选普通界尚未接受；这里未证明真实链无绕回，未覆盖 fresh 误差、Tensor2 舍弃 low×low、RS2 舍入、跨轮放大。尤其是，重组时 carry 的抵消不等于 pair 两个成员的扰动消失：下一轮 Tensor2 使用各自成员，仍须把成员误差与 low 范数一起纳入契约，不能直接将八个 b_k 相加当成终点总误差。

原 S100 E80 FAIL 不变。这个结果不授权修改阈值、输入分布、噪声分布或生产语句。下一步仍是等待并核验同一 Pro 任务的完整实现契约。

## 本轮实际读取的源码锚点

- `src/repeated_mult2.cpp:24–35`：原 S100 的 Q、Div、P 常量。
- `src/repeated_mult2.cpp:279–312`：初始 exact scale、Tensor 与 RS2 的有理数尺度递推。
- `src/double_ckks.cpp:1009–1060`：两个不同 active basis 的 Relin 调用和重组关系。
- `src/double_ckks.cpp:1149–1210`：两次 Rescale、新 low 的精确补偿构造。
- 官方 pin df495ba2 的 `src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp:172–191`：rescale 的 DropLastElementAndScale 消费路径。
- 同 pin 的 `src/core/include/lattice/hal/default/dcrtpoly-impl.h:693–712` 和 `src/pke/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp:62–83`：系数域末塔换模、除数与预计算因子。读取范围不是全上游审计。

开始查找 ExactScale 独立文件时，两个猜测路径不存在；实际定义在 `src/repeated_mult2.cpp:235–239`，随后读取了正确位置。这是路径定位失败，不是源码缺失或编译失败。
