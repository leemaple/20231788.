# 实际执行账本

## 1. 范围与环境

本轮是静态科学审查、轻量标量验证和测试草拟。**C++编译次数0，项目测试程序执行次数0，FFT次数0，新增加密payload次数0，远程写入/dispatch/消息次数0。** 只在隔离工作目录创建返回材料及临时patch应用副本，没有修改用户源材料、远程仓库或自动化。

本容器：Python3.13.5，Linux x86_64，GCC14.2.0；只查询了git2.47.3、CMake3.31.6和c++版本。没有建立完整官方OpenFHE pin的已验证checkout/install。对/usr/local/lib、/usr/lib、/opt、/usr/local/include、/usr/include的有限浅层探测未发现OpenFHE安装；这不是全文件系统的不存在证明。没有configure或compile尝试，更没有凭这些版本声称新测试已构建。

本轮Gitleaks执行次数0。输入清单内的历史扫描为随包证据，不能称为本轮重扫通过。

## 2. 实际读取与核验

起点为已挂载 `/mnt/data/comprehensive-reassessment-3c02988.zip`。先用Files尝试检索ZIP内任务／Double-CKKS内容，两次均无解析结果；随后使用已挂载原文件作容器解包、阅读和逐字节核验，没有转去其他会话或Library补出上下文。

完整读取当前TASK；解析当前MANIFEST并逐载荷核对。核心生产源码 `double_ckks.cpp`、`repeated_mult2.cpp`、`high_precision_client_io.cpp`、`paper_h128_client_keypair.cpp` 与公开接口按完整范围审阅；重点八平方、S116、fresh诊断及其oracle/observer沿调用路径审阅；大体量旧单元测试按与本轮结论相关的系数oracle、phase、精度和校验段落抽查。没有声称对全包每份2000行测试都完成逐行形式证明。

论文TXT全1698行按分段阅读；本地渲染15页PDF。关键页面p4–p10、p11–p13用视觉复核公式、误差假设和表3；其中重复打开p5/p11/p12/p13。未OCR。其余引言、结论/参考文献文字通过TXT读取，不把“渲染了全部页”写成“每页图像都人工审查”。

当前workflow、CMake、README/REPRODUCE和交接说明按执行入口及适用范围核对。历史材料在形成论文／实现判断后对照：原S100两平台status/audit/TSV、S116的状态/验收/receipt、fresh完整诊断与provenance、旧semantic review及condition decision的决定/数学段落、最新output-finalization RED/GREEN。旧文的暂停与外联建议只按历史处置读取。

`READ_RANGES.jsonl`保存阅读辅助器实际请求的文件、范围与hash，共101条请求。它是阅读定位记录，不是所有工具调用的完整转录：初始TASK/manifest、若干cat/grep/JSON读取、PDF视觉与网页阅读不在该JSONL；部分长控制台输出曾截断，关键段另以较短窗口展开，不将截断部分视为已经逐字看到。主报告逐项提供可复核原路径、页码或行号。

77份选定官方和4份Boost均完成存在性/hash核验；重点PKE、HYBRID、CRT、rounding、ternary sampler读取详见READ_RANGES及SUPPLEMENTARY_SOURCES。额外固定官方raw页的读取与下载失败分开记录。

## 3. 实际执行命令和结果

下表为本轮命令族的准确摘要；脚本与机器可读结果均在返回包。初期标量脚本为13项，补入有限sampler提升及exact-prime尺度后最终版本为15项，输出已按最终版本重新生成。

| 操作 | 实际结果 | 产物／限制 |
|---|---|---|
| Python zipfile：成员/path/CRC、SHA256和230载荷核验 | exit0；231成员；全payload匹配 | `results/packet_patch_static.json`；输入字节未改变 |
| 117个未经转换的直接Git blob重新计算SHA1 | 全部匹配 | 不验证远程commit签名、完整tree或runner二进制 |
| 两个gzip来源TSV | 解压载荷hash匹配；原gzip来源hash保留 | 不将解压字节的Git hash与原gzip blob相等作为条件 |
| 本地PDF渲染 | 15页生成成功 | 视觉核对见上；未OCR/未执行FFT |
| `scalar_reassessment.py --literal-paper-red` | **exit1，预期RED** | `results/scalar_red.txt`：676/5错误目标违反6/5界 |
| `scalar_reassessment.py` | **exit0，15项PASS_SCALAR_ONLY** | `results/scalar_green.json`：精确反例、穷举、域、预算、fresh提升、尺度 |
| `replay_retained_endpoints.py INPUT --digits 150` | exit0 | 双平台16384槽各重放一次；`retained_endpoint_replay_150.json` |
| 同上 `--digits 210` | exit0 | 同一保留数据，不产生新样本；`_210.json` |
| 独立两精度输出比较 | exit0；16组最大值/argmax一致，数值差<1e−120 | `retained_precision_comparison.json` |
| `replay_fresh_anchors.py INPUT` | exit0；20分量、3个条件性区间超T | `fresh_anchor_interval_replay.json`；A+B±1e−100，未传播C |
| `replay_annulus125.py --self-test` 最终版本 | exit0；1合成正例，7负例拒绝 | `annulus_parser_self_test.json`；零实际密文 |
| Python `ast.parse` 返回包脚本 | 全部成功 | 只是Python语法解析，不执行项目源码 |
| 临时project副本 `git apply --check CONTRACT.patch` | exit0，stdout/stderr空 | patch可对指定输入应用 |
| 同一临时副本 `git apply CONTRACT.patch` | exit0，stdout/stderr空 | 与full_files字节相同；除CMake追加/新test外原文件不变 |
| 工具版本查询与浅层依赖探测 | exit0，未建立OpenFHE安装身份 | 没有接着configure/build |

本包轻量脚本导入的共享helper只来自本轮自己写的 `checks/scalar_reassessment.py`。没有导入、执行项目现有Python finalizer、历史作者重放脚本或项目测试。

最终解析负例还核对了拒绝位置：复数模负例的最大值记录正确，必须到达独立数值gate才拒绝，不能靠元数据或max不一致蒙混通过。TEST_PLAN中的两段未来Python入口也仅作AST语法解析，未执行。

新C++源码与CMake已作静态接口/数据流检查及实际patch应用检查，未调用编译器验证语法或链接；新测试PASS不得从patch exit0推断。

## 4. 实际失败、纠正与非成功动作

1. Files两次检索没有ZIP解析结果；使用挂载文件继续，并非判定附件内容为空。
2. 在线PDF screenshot失败403；改读同hash输入PDF的本地渲染。初次某次公开PDF打开失败，后续文本页面可读；没有把失败请求记为截图成功。
3. 两次读取猜错历史文件名而出现FileNotFound：`project/REPRODUCE.md`、`s100-output-finalization-01/RESULT.md`。随后读取实际的 `REPRODUCE.zh-CN.md` 和 `RED_RESULT.md`/`GREEN_RESULT.md`，未据错误路径捏造内容。
4. 固定官方补充文件的urllib下载遇DNS错误，container.download也失败；官方网页文本成功读取。返回包未包含伪造的raw下载文件或其hash。
5. 新解析器首次合成控制因Python3.13默认4300位整数转字符串上限失败exit1；第8步精确尺度约7707位。改成有界10000位上限后重新执行，最终1正例及7负例通过。这是本轮Python接收器修正，不是OpenFHE或C++修复。
6. 新静态校验器初版错误地把两个gzip解压TSV当未经转换的Git blob，抛出Git blob mismatch。逐条核对输入origin的transform说明后修正：117个原样条目核Git blob，两个转换条目核解压hash并保留原gzip身份。最终检查通过；**不是输入源包发现两处损坏**。
7. 纸面literal-paper模式的exit1是设计的失效证明，一直保留，不改写成命令全部exit0。

本节没有保留上述调试的每个临时stderr文件，因此只称实际工具返回的摘要，不冒充完整不可篡改终端录屏。关键最终结果和反例RED原始文本已保留。

## 5. 额外计算的解释

选择输入半径前，仅做了几个公开dyadic候选的标量条件数比较，例如31/32、125/128、63/64；没有加密、调用FFT、尝试不同密钥或筛选失败样本。随后以125/128冻结条件性预算并写独立输入公式。最终15项检查覆盖所选域的全部16384点和不为零的输出下界。

历史full-slot重放是从已给定TSV读取误差，以Decimal计算复数平方与范数，不调用任何傅里叶变换。fresh区间重放的±1e−100仍是假设，不是对Boost超越函数的完整误差证明。产生的JSON明确写出scope。

## 6. 尚未执行

没有owner采用记录，没有新Git commit，没有CI dispatch，没有新C++编译RED/GREEN，没有annulus实际TSV，没有任何新E0/E8数值结果。当前60个默认测试、旧S100全链、S116全链及fresh诊断在本轮均未运行。其历史状态只来自输入证据。

后续执行应严格按TEST_PLAN审查ref/guards；推荐入口为一次受1200秒超时控制的直接新binary调用，保存真实子进程exit；CTest仅show-only预检，不重复运行另一入口。本包不把这一未来方案写成已dispatch或已通过。

## 7. 返回包完整性

`MANIFEST.json`采用自排除规则，列出所有其他返回文件大小/SHA-256、输入ZIP及source身份、已执行和未执行范围。最终ZIP另行核CRC与成员唯一性；ZIP自身hash在交付消息中给出，不把ZIP自己的hash递归放入包内manifest。
