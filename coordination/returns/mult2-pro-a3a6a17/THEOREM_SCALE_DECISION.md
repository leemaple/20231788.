# Theorem 4.8 与 Mult2 双尺度裁决

## 1. 裁决

对附件 PDF 第 8 页可见公式、定义 4.1、引理 4.2、定义 4.5、引理 4.6、定义 4.7、定理后的 modulus/scale 段落做交叉审计后，本交付采用以下判断：

> **在未发现额外归一化约定的前提下，定理 4.8 的显示式很可能漏写了一个 `1/q_div`。这是独立推断的勘误，不是作者确认的 erratum。**

测试只核对组合算法对应的修正式和论文给出的误差表达式；绝不声称用测试“证明”了 PDF 原样打印的定理。

## 2. 独立代数检查

令一对明文分量为

```text
m_i = q_div * a_i + b_i .
```

定义 4.1 的 Tensor2 丢弃 `b_1*b_2`，其重组明文是

```text
t = q_div * a_1*a_2 + a_1*b_2 + b_1*a_2 .
```

直接展开得到

```text
q_div * t
  = q_div^2*a_1*a_2 + q_div*a_1*b_2 + q_div*b_1*a_2
  = m_1*m_2 - b_1*b_2,
```

所以

```text
t = (m_1*m_2 - b_1*b_2) / q_div .
```

Relin2 只改变表示并引入重线性化误差，不消除这一除数。RS2 随后再除以本层原生末塔 `q_l`。因此理想组合目标是

```text
(m_1*m_2 - b_1*b_2) / (q_div*q_l),
```

而不是仅除以 `q_l`。

### 极小反例

取

```text
q_div=5, q_l=7,
m_1=m_2=5,
b_1=b_2=0,
a_1=a_2=1.
```

则 Tensor2 重组为 `5`，RS2 后为 `5/7`。PDF 定理显示目标 `m_1*m_2/q_l=25/7`；加入缺失的 `1/q_div` 后目标为 `25/(5*7)=5/7`。当 `q_div≠1` 时两者不能由同一未说明归一化同时成立。

该可重放算术也保存在 `verification/theorem-counterexample.txt`。

## 3. 与论文相邻陈述的一致性

附件文本中：

- 定义 4.1 与引理 4.2 明确把 Tensor2 描述为相对于普通 Tensor **除以 `q_div`**，并说明该除法不消费模数；
- 定义 4.5/引理 4.6 的 RS2 再相对于输入除以 `q_l`；
- 定义 4.7 把 Mult2 定义为 `RS2 o Relin2 o Tensor2`；
- 定理后的 modulus-consumption 段落明确写出一次乘法的 overall scale 被 `q_div*q_l` 除。

这些相邻事实共同支持“显示式漏因子”的解释；未在这些定义中找到可把 `q_div` 吸收进 `m_i` 或解密符号的额外归一化。

## 4. OpenFHE FIXEDMANUAL 的记录尺度与逻辑尺度

记基础记录尺度

```text
Delta_0 = 2^p.
```

本夹具以 `noiseScaleDeg=2` 编码，DCP 输入和 pair 的记录尺度为

```text
SF_in = Delta_0^2.
```

### Tensor2 后

OpenFHE 原始密文乘法给出 `SF_left*SF_right`；现有 Tensor2 按 FIXEDMANUAL 的基础因子 `Delta_0` 归一记录元数据，所以：

```text
SF_Tensor,recorded = SF_left*SF_right / Delta_0 = Delta_0^3.
```

但 pair 高分量和重组分量的论文逻辑尺度分别是：

```text
SF_Tensor,high,logical       = (SF_left/q_div)*(SF_right/q_div),
SF_Tensor,recombined,logical = SF_left*SF_right/q_div.
```

### RS2 后

OpenFHE 的 FIXEDMANUAL `GetModReduceFactor(...)` 返回 `Delta_0`，而实际环上 Rescale 是按原生末塔 `q_l` 取整并丢塔。因此：

```text
SF_out,recorded = Delta_0^3 / Delta_0 = Delta_0^2,
SF_out,recombined,logical = Delta_0^4 / (q_div*q_l).
```

两者比值为

```text
r = SF_out,recombined,logical / SF_out,recorded
  = Delta_0^2 / (q_div*q_l)
  = 2^(2p)/(q_div*q_l).
```

测试从实际 `q_div`、实际被丢弃的 `q_l` 和 `p` 计算并输出该比值；不假定原生素数恰等于 `2^p`。

## 5. 调用者如何解码

供应的 OpenFHE 1.5.0 `CKKSPackedEncoding::Decode` 在 FIXEDMANUAL 的 DCRT 路径中根据 `p` 和 `noiseScaleDeg` 使用 `2^{-p}` 系列因子，不把任意 ciphertext `GetScalingFactor()` 当作最终逻辑尺度。因此 RCB 保留的记录元数据会让普通解码返回：

```text
decoded_recorded ≈ true_product * r.
```

当前测试中的**诊断性**恢复是：

```text
decoded_logical = decoded_recorded / r
                = decoded_recorded * q_div*q_l / 2^(2p).
```

这不是对期望明文的静默改写：测试同时核对未经校正的解码值确实符合预测的确定性偏差，再单独计算校正后的槽位绝对误差。

仅修改 ciphertext 的记录 `scalingFactor` 不能透明修复该 FIXEDMANUAL 解码路径。若产品要求调用者直接得到逻辑值，需要后续明确增加公开解码辅助函数或调整 pair/RCB 的公共契约；本切片按任务边界不做该变化。

## 6. 定理门与 `E_Relin`

端到端 oracle 使用真实 secret key 独立计算：

```text
M_high, M_low, h, N, Q_l, q_div, q_l.
```

并分别测量：

1. 把 Tensor pair 重组成标准三分量 ciphertext 后，调用官方 `Relinearize` 的本次执行误差 `empirical_E_Relin`；
2. 实际 Relin2 pair 重组前后的总误差 `empirical_pair_relin_error`；
3. 关系 `empirical_pair_relin_error <= empirical_E_Relin + h`。

随后以整数比较检查本次执行的 non-wrap 条件：

```text
N*(M_high*q_div+M_low)^2 + empirical_E_Relin + h < Q_l/2,
```

以及本次执行的误差表达式：

```text
(N*M_low^2/q_div + empirical_E_Relin + h)/q_l + (h+1)/2.
```

所有有理数比较都交叉相乘为 `cpp_int`，避免浮点溢出或阈值调参。

但是，单次测得的 `empirical_E_Relin` 不是对所有密钥、噪声和输入成立的保守上界。因此测试固定输出：

```text
conservative_E_Relin_available=false
universal_theorem_gate=UNPROVED
```

本包没有证明定理 4.8，也没有用经验值伪装成通用证明。
