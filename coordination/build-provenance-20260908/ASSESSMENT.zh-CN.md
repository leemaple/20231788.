# 参数图谱补充：历史 OpenFHE 构建来源核查

2026-09-08 Asia/Shanghai。证据采集开始于 2026-09-08T15:10:15.643078Z（23:10:15 CST）。这是已完成参数文档后的只读诊断，不是新的精度实验。

## 结论先行

**已观察：原 S100 和 S116 的两平台作业都实际配置、编译并安装了 OpenFHE，配置输出均为 WITH_OPENMP=ON，且 CMake 找到了 C/C++ OpenMP。** 因而不能继续把这四个作业的 OpenMP 状态仅描述为“工作流请求，是否找到 OpenMP 未知”。annulus125 则恢复了安装缓存，本作业跳过依赖配置与编译；不能把项目编译器版本当成缓存库的完整编译证明。

结合固定官方 CMake 的源码，可推断前四个正常构建会加入 `-DPARALLEL` 和 `-fopenmp`，图谱中的 `private(dgg)` 条件值得纳入实际行为分析。**没有直接读取历史 sampler 的局部 σ，没有证明所有噪声均为 1，也没有证明该分支是 S100 失败的根因。** 本轮未修改采样器、参数、算法或测试。

## 原始记录与身份

所有依赖固定提交均为 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。下表的 OpenMP 版本是 CMake 检测结果，不是对运行时全部特性的认证。

| 原实验 / 作业 | 项目源码 | 依赖构建证据 | C++ / OpenMP |
| --- | --- | --- | --- |
| [S100 Linux](https://github.com/leemaple/20231788./actions/runs/34039088536/job/101502439156) | ed5fd192a89d6d4728ad295e87cf06a3f4abc832 | configure 与 build/install success；缓存未命中 | GNU 13.3.0 / 4.5 |
| [S100 Windows](https://github.com/leemaple/20231788./actions/runs/34039088536/job/101502439304) | 同上 | configure 与 build/install success | GNU 16.2.0 / 5.2 |
| [S116 Linux](https://github.com/leemaple/20231788./actions/runs/34055816234/job/101547399114) | 2b8b349edf5575556347082c1b725f6696c743b6 | configure 与 build/install success；本次随后保存缓存 | GNU 13.3.0 / 4.5 |
| [S116 Windows](https://github.com/leemaple/20231788./actions/runs/34055816234/job/101547399237) | 同上 | configure 与 build/install success | GNU 16.2.0 / 5.2 |
| [annulus125 Linux](https://github.com/leemaple/20231788./actions/runs/34184869227/job/101931070901) | 03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b | cache hit；依赖 configure 与 build/install skipped | 本轮项目 GNU 13.3.0；库的完整配置未直接取得 |

前四个依赖配置还均输出 `NATIVE_SIZE=64`、`MATHBACKEND=4`、`WITH_NATIVEOPT=OFF`、`WITH_REDUCED_NOISE=OFF`。这些是配置日志事实，不扩展为所有编译宏已核验。

annulus 恢复的 key 为 `openfhe-1.5.0-Linux-gcc-df495ba2e91739a6dc8f1de254fc5a41155ce504-native64-backend4`。S116 Linux 曾保存同名 key，但**单凭同名 key 不宣布两个库逐字节相同或唯一来源已经闭合**：尚缺缓存 scope/version/内容身份的直接关联。已有 [annulus provenance](../s100-annulus125-20260908/experiment-evidence/provenance.txt) 也只记录 `openfhe_cache_hit=true`。

固定官方 [CMakeLists.txt:429–504](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/CMakeLists.txt#L429-L504) 在 WITH_OPENMP 分支定义 PARALLEL，并在发现 OpenMP 后将其 flags 加到 CMAKE_CXX_FLAGS。本轮读取的是本项目图谱任务新取得、已按官方固定 Git blob 验证的源码镜像，不是本机隔离区 OpenFHE。

## 证据采集及其限制

- [采集脚本](collect_build_evidence.py) 使用 GitHub CLI 只读请求 3 个 run、5 个 job 的元数据与既有日志，以及 artifacts 元数据；没有 dispatch/rerun、下载或运行实验二进制。
- [首轮回执](GITHUB_BUILD_EVIDENCE.json) 保留不动。CLI 把日志 step 字段返回为 `UNKNOWN STEP`，首轮严格按 step 名筛选导致 0 行；这是采集器筛选缺陷，不是“没有启用 OpenMP”的证据。
- [第二轮回执](GITHUB_BUILD_EVIDENCE_V2.json) 按信号筛选并保留原 step 标签，不伪造 step 名。各 job 分别保留 59、52、57、49、32 行。配置输出的 UTC 时刻可与 API step 起止时刻核对；测试摘要可能因携带依赖 pin 被一并保留，不算新测试。
- 每个 job 记录 CLI 返回日志的字节数和 SHA-256。hash 针对 CLI 返回字节，不是 GitHub 原始归档或二进制哈希；未保存完整日志。真实 ANSI 控制序列被移除，CLI 输出的可见 `^[[` 转义文本可能保留，不影响原日志 hash。
- S116 artifacts API 返回空列表；不据此抹去已有运行结果。原 S100 两个平台和 annulus 的既有 artifact 元数据仍可取得。

## 对排错的影响

1. **来源问题得到推进，不是精度问题修好。** 原 S100 原门槛 FAIL、S116 改参 PASS、annulus 改输入且重新抽 key/noise 的一次 PASS，仍保持原结论。
2. **不能先改 σ。** HYBRID 的 evaluation-key 噪声与 public-key/payload 的噪声路径不同。既有原 S100 残差证据显示 fresh 误差的传播占主导；即使发现 evaluation-key 配置偏差，也不能跳过因果区分，把它宣布为失败原因。
3. **NTT 根缓存仍是另一条独立风险。** 这次构建核查未证明同 q 的混根、跨缓存时期对象冲突，也未运行相关实验。
4. **下一个最小诊断的验收条件：** 先完成实际构建 flags、依赖缓存身份与公开采样配置状态的可核验记录；若需 runner 探针，必须只输出非秘密参数/宏/版本，不导出种子、秘钥或随机系数；不得通过更换输入、放宽门槛或反复抽样来替代原失败条件。复杂因果方案继续交网页版 Pro 完整上下文复核后再执行，不把本说明当成已派发任务。

本轮只执行 API/日志读取、静态源码核对和文档检查。没有 OpenFHE 编译、FHE/FFT/NTT、随机采样、性能测试、新 CI、外部 Agent 中断或高频定时任务。
