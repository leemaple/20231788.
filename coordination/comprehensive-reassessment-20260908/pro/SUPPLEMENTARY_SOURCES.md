# 引用与补充官方来源

## 1. 固定输入来源

所有项目代码和历史执行事实均来自用户本轮ZIP，而不是远程默认分支、其他会话或旧实现。相对路径和原行号见主报告；核心文件hash与来源条目另存 `INPUT_BINDING.json`。原ZIP的全部230载荷hash已验证，原MANIFEST的两个gzip→TSV转换条目保留其不同身份，不冒充解压字节等于原Git blob。

论文：输入 `references/paper/PAPER-2023-1788.pdf`，759375 bytes，SHA-256 `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`。本轮完整读取对应TXT，用本地PDF页面渲染核对核心公式及表格，没有OCR。公开落地页与PDF也作只读交叉核对：

```text
https://eprint.iacr.org/2023/1788
https://eprint.iacr.org/2023/1788.pdf
```

在线PDF screenshot请求返回403，故图形、公式依据用户已给定hash的本地PDF，不声称在线截图成功或已另行计算远程PDF原字节hash。

## 2. 原77份官方参考中的重点路径

下列路径均在 `references/official-full/` 下，来源固定到官方OpenFHE1.5.0提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。本包含全部77份选定文件，不代表完整OpenFHE依赖树。

| 路径 | 主要用途 |
|---|---|
| `src/pke/lib/schemerns/rns-pke.cpp` | public/private EncryptZeroCore，v和Gaussian噪声的实际路径 |
| `src/pke/lib/schemebase/base-pke.cpp` | 常规secret sampler默认稀疏参数与KeyGen |
| `src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp` | 固定噪声模式下Poly解密入口 |
| `src/pke/lib/schemebase/base-leveledshe.cpp` | 常规Tensor的正交叉项及no-relin语义 |
| `src/pke/lib/schemerns/rns-leveledshe.cpp` | FIXEDMANUAL相关元数据/乘法调整 |
| `src/pke/lib/keyswitch/keyswitch-hybrid.cpp` | HYBRID key generation、basis转换及ApproxModDown |
| `src/core/include/lattice/hal/default/dcrtpoly-impl.h` | 跨CRT塔的同一整数采样、DropLastElementAndScale |
| `src/core/include/math/ternaryuniformgenerator-impl.h` | dense v的h=0路径和h128符号近均衡采样 |

Boost1.83.0的4份参考均已核对存在及hash，供固定/动态多精度类型与转换行为审查；本轮没有把这些摘取文件编译成已验证工具链。

## 3. 额外只读补充的固定官方文件

这四个底层文件不在上述77份的相应选定路径中；本轮通过固定commit的官方raw页面阅读，未用本机改动版。网络容器下载尝试失败，所以返回包**不包含冒充原始文件的重建副本，也不提供未计算的原始字节hash**。

统一前缀：

```text
https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/
```

| 编号 | 前缀之后的路径 | 读取依据与用途 |
|---|---|---|
| S1 | `src/core/include/math/discretegaussiangenerator-impl.h` | `SetStd`、`Initialize`、`FindInVector`、`GenerateInt`及vector采样；网页0起始行57–119：sigma低于阈值使用有限inversion表，表长39，成功返回支持有界 |
| S2 | `src/core/include/math/discretegaussiangenerator.h` | 网页0起始行74：KARNEY_THRESHOLD=300；与S1合用，不把Karney路径错误套到sigma3.19 |
| S3 | `src/core/lib/math/hal/intnat/mubintvecnat.cpp` | 网页0起始行101–114：SwitchModulus的居中符号解释 |
| S4 | `src/core/include/math/hal/intnat/mubintvecnat.h` | NativeVector接口声明；用于确认S3实现与调用层一致 |

网页行号为web文本的0起始标记；与下载文件的编辑器1起始行号相差1。这里没有大量抄录源码，审查结论是支持范围、路径和独立数学后果。

S1/S2带来的新推导是：在成功采样与既定参数下，fresh aggregate PKE误差系数绝对值≤39(N+1+h)。这并不是宣称截断采样器与论文χerr的概率完全一致，也不是安全认证；更没有把这个fresh界延伸到全部HYBRID中间量。

## 4. 公开溯源的边界

本轮对论文标识/标题与GitHub作过有界只读检索，并阅读公开论文落地页；搜索还返回出版页和作者publication页。没有获得版本绑定的Table3实验生成记录。这个结论只说明本轮检索与输入包未提供该记录，不证明所有公开/私人来源均不存在。

输入 `evidence/coordination/s100-fresh-error-repair-01/PAPER_EXPERIMENT_PROVENANCE.md` 保留2026-09-07的先前有界溯源记录，应按其日期和范围理解。未发邮件、issue或消息，未建立外联草稿。
