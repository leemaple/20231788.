# EXECUTION_LEDGER — 本轮实际动作与证据层级

日期：2026-09-09（Asia/Singapore，UTC+08）。对象为用户本轮给定压缩包；未使用旧聊天、个人记忆、本机私库、实时仓库或外部账户。这里的“实际通过”只指明列的字节／标量／文档检查，不包括新密文实验。

## 1. 环境与身份

本轮工作容器：Linux x86_64，`Linux-6.18.35-x86_64-with-glibc2.41`；Python `3.13.5 (main, Jul 15 2026, 20:25:40) [GCC 14.2.0]`。这个编译器版本是已有 Python 解释器的版本字符串，不是本轮调用了编译器。当前任务和报告日期按UTC+08记录；文档检查回执中的 `2026-09-08T21:30:49.134811+00:00` 对应当地9月9日05:30:49。

本轮没有安装、初始化或运行 OpenFHE。历史 Ecd 作业的 Ubuntu24.04／Python3.12.14 和历史 FHE 作业的编译环境来自各自证据，不能用本容器环境替代。未查询或推断实际后端模型身份、内部思考档或其他审查者提供商身份。

工作目录：输入位于 `/mnt/data/completion_review/input`；中间阅读记录位于 `/mnt/data/completion_review/work`；交付候选位于 `/mnt/data/completion_review/output`。这些是本次沙箱路径，不是用户机器路径。

## 2. 接收、完整性及原件不变

真实挂载附件为 `/mnt/data/completion-contract-a7f54de.zip`。先通过 Files 对任务内容检索，未取得ZIP内可检索结果；随后使用已经挂载的原文件读取和安全解包。没有据“搜索无结果”推断附件内容缺失，没有物化或读取其他资料库文件。

| 检查 | 实际结果 | 保留记录 |
| --- | --- | --- |
| 外层大小／SHA-256 | 15,139,061 bytes；`31f5f6767718e2adaa1aae018dee8ceb44973f20125d0cef466a921a54174042`，匹配 | `evidence/INPUT_VERIFICATION.json` |
| ZIP 成员／CRC | 1007 个唯一安全常规相对路径；CRC PASS | 同上 |
| 根清单 | SHA-256 `0db730b71889a4854a6e8ec147784ccd6d4e0a2e24c8060dc75f879cbc57b081`；自身不列入载荷 | 同上 |
| 载荷集合／长度／SHA-256 | 1006 项全部匹配，展开总字节数（含清单）46,469,553 | 同上逐文件行 |
| 提供的 Git blob 绑定 | 995 项匹配，使用Git blob字节格式摘要核对，不调用Git服务 | 同上 |
| 结束前原件再核 | 全部1006载荷、根清单、成员集合、外层ZIP身份仍相同 | `evidence/INPUT_UNCHANGED_FINAL.json` |

清单内的 `tracked_state=clean` 以及供件方gitleaks／定向扫描结果是供件声明与回执。本轮未执行新的gitleaks扫描，未将其写成自身的扫描结果。Git blob匹配不是实时HEAD、历史构建二进制或全供应链证明。历史嵌套manifest只用于其标明的来源身份，没有替换当前根清单。

## 3. 实际阅读覆盖及限制

`evidence/READ_REQUESTS.jsonl` 记录100次文本窗口请求；`evidence/READ_COVERAGE.tsv` 按57个独立文件汇总请求行范围、总行数及SHA-256。这是读取索引，不是每行被形式化证明的覆盖率。部分大型工具返回被截断；对关键构造另以小范围复读。直接 `sed`／`cat`／JSON读取和PDF像素查看并非都经过该记录脚本，故此表也不是所有阅读动作的完整shell审计轨迹。

主要语义覆盖如下。

| 组别 | 本轮实际读取和使用 | 未宣称 |
| --- | --- | --- |
| 权威与根入口 | 完整 TASK、REQUIREMENTS_PREFLIGHT、scope、CHECK_AND_HANDOFF、REPRODUCE；参数图谱README及REVIEW_NOTES | 历史任务对本轮有指令权 |
| 核心生产 | `double_ckks.cpp` 1–1285；`repeated_mult2.cpp` 1–571；`paper_h128_client_keypair.cpp` 1–219；`high_precision_client_io.cpp` 1–779，及对应主要头文件 | 全程序机器证明、所有未声明调用状态正确 |
| 实际边界／测试 | 完整 full-eight-square test 1–500；oracle闭式尺度、truth、独立稀疏卷积／CRT及Horner片段；两轮语义链及I/O／Relin2／RS2关键负例、carry和边界 | 所有约数万行测试逐行语义覆盖；本轮运行了它们 |
| 构建与入口 | 当前 CMake主要注册范围与公开diagnostic；`dcp-rcb.yml`的固定来源／回归排除／构建步骤；现行 `run_once.py` 1–120 和 `harness_contract.py` 1–53 | 读取workflow等于执行CI；存在target等于实际运行 |
| 官方固定路径 | `rns-pke.cpp` 34–83、111–200；HYBRID keygen及双坐标mod-down 61–133、381–437；DCRT drop-scale 693–719；CKKS rescale 172–207 | 所有331份官方文件逐行形式化验证 |
| 图谱／证明 | 参数图谱主参考1–464的相关正文；22影响表；Relin2／初始提升／cap／Ecd的已采用契约、原候选数学审查和后继封套审查 | 重写旧证明、重新计算变换、查遍127引用作为机械门槛 |
| 历史数值与平台 | S100双平台审计；S116接受与runtime复核；fresh理想传播；annulus执行复核、独立Pro、root标量复核；历史构建来源结论 | 所有原始中间对象都仍可重建，或历史错误TSV是密文 |
| 最新证书 | ADOPTED_RESULT、MATH_ADOPTION_REVIEW、ROOT_INTAKE_REVIEW、GREEN_INTAKE；certificate RESULT及状态／退出封套；全部载荷字节已核 | 本轮重新判定32768行或执行任何逆变换 |

正文引用给出更细的文件／行号；其中 Add/Sub 的实现分别位于 `project/src/double_ckks.cpp:699–733`、`:735–768`，兼容性拒绝另见`:770–824`。源代码增量的字节关系由本轮标量／文本检查保留。

### 论文读取

读取附件 `references/paper/PAPER-2023-1788.txt` 的定义、算法与§6.3，并核对同目录PDF。PDF SHA-256：`61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`。

仅对公开原论文做过网页访问；没有查询账户、运行仓库动作、重复作者实验来源搜索或联系作者。公开PDF截图调用受抓取安全限制未成功；改用已给PDF的本地页面渲染查看第4、7、8、13页，包括Table3和公式。没有OCR，也没有把网页失败当成论文缺页。渲染的是PDF页面，不是FFT、数值输入、编码或采样。已阅读环境PDF处理说明；不生成或修改论文本体。

## 4. 本轮实际运行的33项有界检查

执行的命令为本包自己编写的脚本：

```text
python -B -I /mnt/data/completion_review/output/checks/bounded_checks.py /mnt/data/completion_review/input /mnt/data/completion_review/output/evidence/BOUNDED_CHECKS.json
```

退出0。`bounded_checks.stdout`、`bounded_checks.stderr`、`bounded_checks.exit` 与 `BOUNDED_CHECKS.json` 保留结果。33项全部PASS：

- 两平台各7项：读取原状态，精确比较E／I／A的保留有理边界，并核对同一行两个带符号分量的打印一致性；共14项。
- S116的四个有裕量摘要不等式及annulus的一个摘要不等式；共5项。S116／annulus输入使用明确标出的缩略已报数值，只证明这些缩略摘要的比较，不替代历史精确日志。
- 当前Ecd封套状态／计数／正间隙／入口一致性／退出码／原FAIL保留；共5项。
- 固定标量纯编码贡献区间、低于半个E80、半径前提；共3项。
- 三份生产TU哈希及高精度I/O基线、单独probe边界和移除增量后原字节一致；共6项。

这里的固定有理指数计算是一个标量后果检查，没有创建任何槽向量或多项式，没有重新分类任何Ecd系数行，没有解密或重新观察旧密文。原I/A的误差界仍依赖其明示观察模型，未被摘要比较提升成无条件数学事实。

## 5. 文档候选的实际 RED／GREEN

执行命令：

```text
python -B -I /mnt/data/completion_review/output/checks/doc_review_checks.py --input-root /mnt/data/completion_review/input --delivery-root /mnt/data/completion_review/output --evidence-dir /mnt/data/completion_review/output/evidence
```

外层退出0。仅调用本包新写的 `check_docs.py`，合计14次：旧文档一次，候选一次，12个临时文本副本各一次。其结果为：

| 检查 | 实际退出／结果 |
| --- | --- |
| 旧两文档 | 1，缺少本轮冻结的当前合同字段及入口 |
| 新两文档 | 0，14个状态字段和就地实验限定正确 |
| 12个文本负控 | 全部退出1并拒绝：改原两个FAIL、删除S116改参范围、捏造annulus Windows PASS、混同当前p与旧p、全项PASS、无条件算法证明、错误快照、重复字段、删除历史标签、去掉就地S116限定、缺文件 |
| 精确补丁 | 仅两个文档路径；所有旧context及hunk计数吻合；作用结果逐字节等于完整候选 |
| 历史正文与不变性 | 交付指南历史正文除明确标题重标外保留；原文件与完整候选未被负控修改 |

实际回执为 `evidence/DOC_CHECK_RECEIPT.json`，并保留RED／GREEN的stdout/stderr和外层退出码。旧文档没有本轮新字段是**文档合约RED**，不是旧算法失败；12个文字负控也不是12个密码学测试。固定标签检查不判断任意自然语言的真伪。

没有把补丁应用到输入树，更没有commit、push或merge；只有独立候选文件。采用者不需要为这两个文档修改编译或重新加密。脚本不导入供件数值模块，完整原件在结束前再核一致。

## 6. 命令和异常的诚实记录

文件读取使用标准Python `pathlib/json/hashlib/zipfile/Fraction`，以及 `cat/sed/nl/grep/find/wc` 等文本／元数据操作；阅读脚本只输出指定行并记下摘要。交付写入使用自己的文本生成与`difflib`，打包使用标准ZIP和SHA-256。没有把任何供件脚本作为命令执行。

一次目录计数／筛选类shell命令因包含目录返回非零；它不是试验或数学断言失败。Files检索未取到ZIP内容、公开PDF截图抓取受限均按上文处理。包装文档检查的一个shell工具返回中出现终端环境提示；保存的检查器stderr为0 bytes且外层退出0，不是算法输出或被掩盖的检查失败。没有把这些非数值工具问题当作FHE结果。

本账本不是伪造的全量shell录屏：未逐个记录所有阅读命令的墙钟时刻；真实数值／文本检查的退出与机器结果、所有输入和交付字节身份已另行保存。没有重跑供件中的历史脚本来填充日志。

## 7. 本轮明确 NOT_RUN

| 动作 | 本轮次数／状态 |
| --- | --- |
| 任意大小FFT／NTT、tiny inverse、full inverse、forward cap | 0 |
| 全输入生成、编码、随机采样、keygen、FHE | 0 |
| 编译、链接、CMake configure、CTest、CI | 0 |
| 原S100、S116、annulus新密文链 | 0／0／0 |
| 全系数单元重新分类、全槽错误TSV重放 | 0／0 |
| 作者联系、GitHub／其他外部账户操作 | 0 |
| 原件源码／测试／工作流修改，远端commit/push | 0 |

历史证书的3个CI gate、3个harness seam、53个scalar、4个tiny inverse、1个full inverse，以及历史FHE／60回归／平台PASS或FAIL，均属于已供证据，**不计入本轮执行**。

## 8. 完成状态

本轮完成独立审查、精确标量与字节核对、分项验收表和两个文档候选。生产修复为NONE：不是因为未知作者设置就拒绝工作，而是未找到一个可由现有规范证实、尚待修复的具体生产缺陷。原冻结E80仍为两平台FAIL，总验收标签仍为NOT_ALL_PASSED；本轮审查完成不能替换它。

最终 `MANIFEST.sha256.json` 自身排除，覆盖交付的其余全部常规文件；外层ZIP另有SHA-256侧车。交付可由原文件摘要、补丁、机器回执和明确命题逐项核查，不依赖继续联系本审查人。
