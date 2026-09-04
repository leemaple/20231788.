# h=128：论文约束与未指定的实验采样细节

2026-09-04；只读核对，起始 HEAD `8dcadb4544ed567c6de3a7d3825857f89470b29e`、clean。仅使用所供论文 TXT、已验证 official53 ZIP 的两个 ternaryuniformgenerator 文件及现有 h128 审计；未读 HEaaN 实现、联网、运行密码/测试或修改实现。

## Observed：原文与源码

1. §2 的记号规定 `x←S` 表示从集合均匀抽样（TXT 201）；但 KeyGen 对秘密写的是三元系数、Hamming weight h、从 “a prescribed distribution” 抽样（223–227），并未写从全部重量 h 的多项式集合均匀抽样。[论文定义][P1]
2. 对 TXT 全文 1697 行检索 ternary / Hamming / distribution / uniform / support / sign / balanced / random / independent / HEaaN 等并复核相关上下文，未找到额外强制的非零位置选择律或正负号联合分布定义。Theorem 3.2 及其证明按 h 给出界，未附符号概率条件（363–407）。Table 3 指定 h=128（1580）；§6 仅说明基于 CryptoLab HEaaN，未给出秘密采样代码或版本（1466、1476）。这是所供文本的核对结果，不是对所有外部资料的断言。[定理][P2] [实验][P3] [Table 3][P4]
3. 官方 `GenerateIntVector(size,h)` 非零 h 分支按随机索引拒绝重复位置，并按正号计数拒绝整轮；`GenerateVector` 有对应逻辑。API 明确接收 h。源码中 h=128、N≥128 且正常返回时，恰有 128 个非零系数，正号数为 63、64 或 65；这不是对所有符号组合不加限制的采样。该结论来自代码条件，未运行采样器。[API 64–82][O1] [实现 67–99、111–143][O2]

## Inferred / disposition：应如何使用这些事实

- h 是通用算法的参数，不是所有算法固定为 128；本项目选定 Table 3 后，应验证最终实际秘密是三元、重量恰为 128。三元重量 h 给出 ∥s∥₁=h，因而上述按重量的确定性界不需要先证明正负号独立均匀；这不解除定理的其余前提。
- 可将官方非零 h 采样器作为**明确记录的 prescribed distribution 候选**：63–65 正号条件本身不违反已找到的三元/固定重量文字约束。不能反向宣称它与 HEaaN 实验采样律相同，也不能把未知自动写成已知分布偏离。
- 建议澄清[旧审计第 94 行][A]：记录实际采样器、重量、符号约束及 HEaaN 同分布未核实这一实验比较限制；**不把补齐论文未指定的 HEaaN 采样实现提升为额外 completion gate**。实现论文算法/公开参数，与逐项重现原库全部随机实验细节是不同主张。

## Pending / limits

匹配密钥的实际生成与使用、既定论文参数和真实重复乘法实验仍需原有验收。本 note 不证明安全级别、PRNG 性质、噪声/误差统计或 HEaaN 等价；也不以 h=128 这一项宣告复现完成。没有新增运行结果。

来源校验（完整字节 SHA256；两官方文件同时与 ZIP provenance 的 bytes/Git blob SHA1 核对一致）：

- 论文 TXT：90235 bytes，`60dd871a2769fddfe7ce7b2562d031d7c8d819a679eff3c2b6ebf3d7ea5769ae`。
- `ternaryuniformgenerator.h`：3510 bytes，`483a1373b3c1371381db47dc7fc28ed9143e247a9be673c1185a7902ba741a2c`。
- `ternaryuniformgenerator-impl.h`：5325 bytes，`dfc13aaa9ac2d8f34c99fa9483e6cdcf7284ca4eb983237bc369e65a02fbebbf`。

[P1]: /private/tmp/repeated-mult2-handoff-recheck.WkleUq/payload/PAPER-2023-1788.txt:201
[P2]: /private/tmp/repeated-mult2-handoff-recheck.WkleUq/payload/PAPER-2023-1788.txt:363
[P3]: /private/tmp/repeated-mult2-handoff-recheck.WkleUq/payload/PAPER-2023-1788.txt:1466
[P4]: /private/tmp/repeated-mult2-handoff-recheck.WkleUq/payload/PAPER-2023-1788.txt:1580
[O1]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/ternaryuniformgenerator.h#L64-L82
[O2]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/ternaryuniformgenerator-impl.h#L67-L143
[A]: /Users/lifeng/Documents/20231788-openfhe-codex-repeated-mult2-01/coordination/PAPER_H128_OFFICIAL_API_SUPPORT.md:94
