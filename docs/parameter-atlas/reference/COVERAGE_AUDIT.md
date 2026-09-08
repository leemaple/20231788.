# 覆盖验收与明确限制

> 根端勘误阅读版：精确函数名已修正；NTT 缓存结论限于表示/生命周期风险，不能据此认定已有算术失败。请与[根端复核说明](../REVIEW_NOTES.zh-CN.md)合读。`checks/` 保留原作者原稿自检记录，发布版复核另见 coordination 台账。

本文是主文档作者的交付自检，不是独立审查结论。统计来自 `checks/coverage_summary.json`、`checks/document_consistency.json` 和逐位置 JSON；不据此宣称对全部 OpenFHE 算法做了审计。

## 实际计数与含义

| 对象 | 本轮范围 | 不应推导出的声明 |
| --- | --- | --- |
| 输入 | 454 个普通成员；453 个 manifest payload；7,161,934 展开字节 | 不包含用户电脑/私库/未附历史二进制 |
| 固定官方源码 | 331 个文件逐字节、SHA256、所供 Git blob 标识核对 | 不是本轮重新下载全部官方 Git tree 后的独立来源认证 |
| Git blob | 共446个有blob标识的payload重算SHA1闭合 | 没有blob标识的论文等不伪造Git归属 |
| 主参数字典 | 132条＝33个CCParams字段＋24条profile内Q/P表项＋75条手工/派生/数值/对象参数 | 24条是两profile各12项；不是24枚互不相同的prime |
| CCParams | 33字段、32通用setter；CKKS特化明确禁用8个 | 底层保存一个字段不等于用户setter合法，也不等于当前路径消费它 |
| CryptoContext自有表面 | 4实例字段、2静态map、1条件DEBUG字段；7个Set命名方法 | 不是枚举所有委托算法的全部内部成员 |
| 项目setter出现 | 527处，47个符号，逐处映射参数条目 | 包含注释/负控；不是527个有效正例配置，也不是AST可达性证明 |
| profile/constant宽松索引 | 668行词法记录；逐条核对原行 | 有上下文/工作流假阳性；不是全部C++常量表达式求值器 |
| 固定源码引用 | 127锚点，67个不同文件；路径/行界/hash检查 | 不把共享大范围引用当作每行都有独立结论 |
| 范围读取记录 | 185次显示请求，72个文本路径 | 部分大输出被工具截断；请求到的区间不等于区间每行均完成语义审阅 |
| 覆盖状态 | 67定点语义来源、5定点上下文、32仅词法、349仅hash/index、1PDF目视 | 未宣称331官方文件全部逐行阅读 |
| 论文 | 固定PDF15页；TXT1698物理行、4个NUL；目视页4/5/6/7/8/9/11/12/13 | 其余页未逐页图像审读；未枚举任意t的通用实现 |

`SOURCE_COVERAGE.tsv` 为全部454成员逐件列出状态、实际请求范围、参与结论的函数区间、对应要求和未覆盖原因。`READ_REQUESTS.jsonl` 不包含虚构阅读耗时/时间戳。其作用是保全范围请求，不是假装眼动、逐行完成度或独立审查凭证。

## A—E要求与交付位置

| 要求 | 交付定位 | 可核验的完成内容/保留限制 |
| --- | --- | --- |
| A1 N/order/logN/slot/embedding | 主文档1、2、7；ring.* | N与slots区分；奇根/5幂槽序；复杂度仅渐近分析，没有新性能测量 |
| A2 exactQ/P/QP/d/roots/drop | 主文档3；basis.*及24个prime条目；scalar JSON | 两profile精确整数、阶条件、互异性、全部8轮基变化；P派生纯标量核对 |
| A3 true scale/metadata/depth/t | 主文档1、3、6；scale.*；CC字段 | Δ有理递推、noiseScaleDeg、local level与root level分开；t=2符号消歧 |
| A4 KS/digits/P/family | 主文档3、5、6；CHANGE_IMPACT | HYBRID partition=每family完整Q塔数，alpha1；同根s逐塔投影；evalkey各自产生 |
| A5 secret/h/sigma/v/noise/security | 主文档5；RANDOMNESS_PATHS全篇 | h128符号平衡与BUG、v稠密、floatσ、conditional private σ；HEStd_NotSet不作安全保证 |
| A6 全字段/setter与不适用模式 | 主文档2；PARAMETERS；CONTEXT_API；setter账 | 33/32/8及context7方法完整词法/字段表对应；不扩展为所有BFV/BGV/bootstrap算法审计 |
| A7 编译/PRNG线程/数值/observer | 主文档7；numeric/build/rng条目 | 十进制与二进制位数、FFT舍入、Double绕过、119-byte wire、observer口径与阈值 |
| B PRNG到所有当前随机消费点 | RANDOMNESS_PATHS 2—8；R01—R23 | Blake2Xb底层、entropy包装、threadprivate/thread_local、DUG/BUG/TUG/DGG、PKE/evalkey/tag/参数随机性 |
| B 多Gaussian分支及生命期 | RANDOMNESS_PATHS 4—7 | 当前σ1/3.19进入小σPeikert；Karney大σ只识别入口，不冒称当前执行或全算法分析 |
| B 确定性/保密/变更影响 | RANDOMNESS_PATHS 9—10；CHANGE_IMPACT | seed不等于实验身份；无秘密转储、无生产可预测随机性建议 |
| C 完整计算步骤 | 主文档6的13步表与展开；P01—P30等 | 输入输出CF/EV、基、scale、高低项、keys、public/secret边界、公式、不变量、测试定位 |
| C 普通CKKS对照 | 主文档6.3及相关表 | GenCryptoContext、普通Encode/Decode与自动level管理绕过点；eval key/PKE仍消费上游 |
| D 论文四方映射 | 主文档8；A01—A05；3/9节 | HEaaN迁移不自动等价；paper t/Δ/d/P/d_num/h清楚区分 |
| D 全部精确scale/输入差异 | 主文档3/9；scalar JSON | 两profile各8步Fraction；原/annulus输入范围16384槽纯标量界；不采新key/noise |
| D 修改依赖与对象重建 | CHANGE_IMPACT | 22类矩阵、4例、旧证据失效、oracle/fixtures/manifests/门槛；不只改Set函数 |
| E 排错导航 | 主文档10；CHANGE_IMPACT；FINDINGS | 症状—可能参数/步骤—非秘密观测—区分检查—测试文件；未执行这些后继检查 |
| E 新矛盾与历史界 | FINDINGS F01—F08 | 2条件性新风险；已知论文公式问题再核对；不宣布新生产bug/RED，也不以旧结论限制后续 |

## 不能由所供文件补齐的精确事实

1. **历史构建有效宏/二进制**：包内有 `official/configure/config_core.in` 模板及 CMake/工作流请求，但没有历史安装产物的 `include/openfhe/core/config_core.h`、对应构建 `CMakeCache.txt`/`compile_commands.json`、实际链接库及其与run的hash绑定。因此 WITH_OPENMP、HAVE_INT128、REDUCED_NOISE、外部引擎、缓存命中后的实际构建身份不能全部由请求值证明。不是缺少算法源码。
2. **OS熵具体后端**：`std::random_device` 属实际C++标准库/平台实现；本包没有该历史libstdc++/libc++/MSVC STL实现和运行日志。不能把Linux一律写成某个syscall，也不能把Windows的编译器/版本推断为论文噪声配置。文档已追到底层包装及BLAKE2参考C实现，平台熵提供者仍标未知。
3. **论文HEaaN实验配置**：给定论文没有公开对应HEaaN精确版本、源码commit、输入生成器和全部噪声/加密配置。所缺不是可以从OpenFHE当前默认值补出的字段；这些事实不阻止完成迁移映射与源码图谱。
4. **既有实测层级**：本轮引用包内对应接受/结果记录，没有重新获取完整远端CI日志或执行其程序；不能把主分支源码当前状态反向等同每个历史样本的运行二进制。
5. **范围外算法**：BFV/BGV、完整bootstrapping、PRE、多方协议、scheme-switch、全部Karney细节与所有未实例化backend分支只按不适用或未展开登记。当前任务是CKKS依赖闭包及项目相关表面，而非整个OpenFHE证明。

F01的决定性缺口是**private局部DGG的实际构造分支**，不是context.GetStd值；F02缺口是**同q/N不同root在同进程的真实先后使用**，不是root阶检查。它们在源代码层面的事实不因这些运行缺口消失，但运行后果不能冒称已证。

## 验收与唯一下一步

最终22组静态文档检查通过；首次检查器措辞误报及修正记录保留，纯标量两次输出逐字节相同。本轮自检的范围是文件/引用/字典/常数一致性；不等于独立科学PASS或安全认证。交付中没有算法补丁，没有新测试/CI文件修改，没有秘密/真实seed。原S100 E80 FAIL、S116改参PASS、annulus改输入PASS分别保留。

**根端复核文档覆盖与事实，再选择具体诊断。**
