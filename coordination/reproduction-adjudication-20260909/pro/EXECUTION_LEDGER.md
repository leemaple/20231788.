# 真实执行账本

## 1. 身份与环境

任务：REPRODUCTION-ADJUDICATION-01，当前用户供件为唯一项目事实来源。没有调用个人记忆/旧聊天，没有访问本机私库，没有改动仓库/GitHub/agent状态。

输入挂载：`/mnt/data/reproduction-adjudication-31e24be.zip`；5400318 bytes；760 个安全普通唯一成员；展开 17915984 bytes。ZIP SHA256 `7e3ea9fee4a04d5535cc47aab42a71e38367db18c70b804a609551345affcbe4`。根 MANIFEST SHA256 `776fd05dc43adf11ca9a912487ae8bf5c1269dfe69149addd8263cbe34b8edaf`；759 项载荷全部字节数/哈希一致。749 个可追溯 Git blob SHA1 另核通过；官方目录331文件。CRC、成员/manifest集合、路径/类型检查通过。

当前 source `31e24bec1eb2db5d13de3b442a9e909e26db7206`；task `d8deee649eb4a0e265d55ee98a323ecefb8240e6`。历史嵌套 manifest 只作为来源链，不拿历史 project 文件哈希验证当前不同版本。

本轮实际标量环境为 Python 3.13.5、Linux x86_64、glibc2.41；详见 `FINAL_STATIC_CHECKS.json` 的真实版本和 UTC 记录时刻。下一行动冻结 GitHub ubuntu-24.04/Python3.12.x，与本轮环境不同，不能把本轮结果描述成 Windows/GitHub 实测。标量结果只依赖整数/Fraction，不以浮点近似作门槛判决。

没有可读取或切换浏览器 Pro 思考档的工具；无法独立核验用户页面“最高可见”档位，未把不可见设置、模型后台资源或耗时写成已验证事实。

## 2. 实际阅读范围

阅读了当前 TASK、参数图谱及复核中的相关章节、论文原始定义/算法/参数页、生产 DCP/Tensor2/Relin2/RS2/RCB/精确尺度/family/key 投影、高精度编码/读出、公开编码导出、官方 PKE/HYBRID/ModDown/缩塔调用、独立全槽观察器及 fresh 拆分、相应负控和 CMake/单次 cap 工作流；并核对采用的 Relin2、initial-lift/nonwrap、cap 合同、原 S100/S116/annulus 和 fresh 的保留接受/审计记录。

`evidence/READ_REQUESTS.jsonl` 保留带编号读取工具实际请求的文件、SHA256和行窗；`READ_COVERAGE.json` 汇总相同请求。较早 `sed/grep` 的直接读取不完全包含在该追踪里，某些长工具输出有截断，故**该表不是逐行全读或完整终端转录的证明**。主要结论所用具体位置另在 `SOURCE_INDEX.tsv` 中列明并哈希绑定。沒有声称逐行审计全部331份官方源或全部历史测试文件。

原论文物理第4、7、8、13页实际渲染并目视，用于半整数向下、canonical顺序、Tensor2/Relin2、RS2归一化及Table3口径；未用OCR。按工具要求尝试 web PDF screenshot，访问失败/403 后用供件原PDF本地渲染核页。外部固定论文页面访问不替代供件源码或版本依据。

Files 对 ZIP 内容的两次检索均无索引结果，随后使用已挂载原附件解包/读取；没有把无搜索结果解释为资料缺失。

## 3. 实际执行的检查

| 检查 | 实际结果 | 证据 |
|---|---|---|
| 原包完整性/当前manifest/Git blobs | exit0；760成员、759载荷、749 blob、331官方文件均合格 | INPUT_VERIFICATION.json |
| 第一阶段有限数学/负控 | exit0，47项PASS | SCALAR_CHECKS_PHASE1_47.json（最初输出名INITIAL，原字节保留） |
| 增加候选文件后的相同有限套件 | exit0，47项PASS | SCALAR_CHECKS_PHASE2_47.json（最初输出名FINAL，原字节保留） |
| 最终套件，追加两平台端点有理检查 | exit0，53项PASS | SCALAR_CHECKS_COMPLETE.json；这是本轮采用的最后数值结果 |
| 用交付版输入核验脚本复核ZIP | exit0；所有身份一致 | INPUT_RECHECK.json |
| 全部7个候选/工具Python文件AST | 解析成功；非Python字节码编译 | FINAL_STATIC_CHECKS.json |
| runner shell 的 bash -n | exit0；仅语法检查 | FINAL_STATIC_CHECKS.json |
| 交付静态一致性 | exit0，76项：32行矩阵、36来源、复制字节/代码哈希/必需文件等 | DELIVERY_STATIC_QA.json |

实际命令组、退出状态及两次无结果检索/失败截图见 `evidence/COMMANDS.json`。生成 Markdown/TSV/代码、复制公开原件、序列化 JSON/哈希清单、打包 ZIP 属于交付物制作，未运行项目程序。

53项的具体标签全部随JSON提供：既有cap原始有理端点、两项封闭式理想值、实际两系数、原身份之后的±1/符号/scale负控、正负半整数开闭边界、少量精确输入标量、向外区间基础算术、条件性的 `4(R+2^-86)^255<1/2` 以及两平台历史E8下界>T和同分量归属。该条件有理数被精确夹于 `[424504491252/10^12,424504491253/10^12)`。

历史 E8 检查只对本包保留 audit 的原始有理端点和字段作核验。没有声称再次下载完整日志、解压旧8MB数值sidecar、重新执行观察器、重放旧随机流或产生新端点。

## 4. 明确未执行

本轮 build=0、OpenFHE/FHE调用=0、密钥生成=0、加密/解密=0、采样=0、FFT/NTT=0、完整变换=0、全槽输入构造/编码=0、新八平方链=0、GitHub dispatch/rerun=0。

`candidate/check_rounding.py` 的四个 tiny 解析模型及完整 inverse **均未执行**；仅静态源码/数学审查及AST。其runner命令是未来独立审核后的建议命令，不在本轮执行列表里。代码里打印的计数和审批环境变量不是运行发生或审核完成的证据。

原输入 p 只读取、哈希及取少数既有系数。负控只在标量内存构造常数项变异的数学对象，没有改写 `fixtures/public_s100_encoding.json`。源码、噪声、输入、阈值及旧原件均未改动。

没有运行 Gitleaks 或宣称本轮重复了严格秘密扫描。供件者的扫描声明被保留为供件来源信息，CRC/哈希核验不是秘密扫描。交付拷贝的是公开p、公开源码、采用回执及公开标量诊断；本轮根本未产生 s、seed、v、e_pk、e0/e1 或密钥对象。

## 5. 候选与原件、假设与采用

`source/`、`fixtures/`、标明 retained/cap 的 evidence 为原件逐字节复制，映射在 SOURCE_BINDINGS。`candidate/interval_core.py` 中区间函数原样取自固定 cap 脚本39–140行，新增代码只实现外部逆嵌入观察和舍入区间判决；没有生产补丁。

本轮采用新的两系数证明和有限scalar结果；采用现有nonwrap/Relin合同的明确条件；不对未实测的历史 sampler、二进制等同性或作者初始化作额外推定。不把新候选自行判为独立审核通过。

根 `MANIFEST.sha256.json` 为全部最终载荷提供 bytes/SHA256并自排除。最终归档核验回执另在下载ZIP旁提供；避免让文件把自身的最终哈希包含进自身而造成循环依赖。所有原始阶段输出保留其实际代码哈希，不能因为后来新增工具就把旧阶段记录伪造成最终脚本执行。
