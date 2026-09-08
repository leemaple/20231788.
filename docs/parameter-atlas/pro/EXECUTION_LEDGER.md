# 实际执行账

任务：`OPENFHE-CKKS-PARAMETER-RANDOMNESS-ATLAS-01`。本轮仅文档与轻量静态/纯标量检查。下述结果属于主文档作者的本地自检，不是独立审核，也不是FHE算法RED/GREEN。没有虚构阅读时间、运行耗时或后台任务。

## 1. 环境与输入

本地工具环境：Python `3.13.5`；`Linux-6.18.35-x86_64-with-glibc2.41`。不等于用户本机、历史Linux/Windows作业或安装的OpenFHE环境。没有为本轮构建/安装OpenFHE。

工作输入根为 `/mnt/data/atlas_work/input`；交付根为 `/mnt/data/atlas_return`。原始ZIP位于 `/mnt/data/openfhe-parameter-atlas-a4b815a.zip`。原archive的SHA256、bytes、成员、CRC、manifest以及有标识payload的Git blob重算在 `checks/input_verification.json` / `checks/input_identity_recheck.json` 保留。

实测：ZIP 2,273,139 bytes；SHA256 `abc4df778754a2d3713f1442fbfe34d69af274f3638e65d4d362f35aa93fdd68`；454唯一普通成员；展开7,161,934 bytes；453 payload；446个有Git blob标识的文件重算匹配，其中官方331。输入manifest SHA256 `1dd20ae1b69b342f243ab57c9c7a8cadb9ca6002d5a95cc6d7746a787bdbc0ac`。没有CRC错误、未列payload或hash差异。

源/官方pin是输入manifest与逐文件origin绑定的pin；Git blob按 `SHA1("blob " + decimal(len) + NUL + bytes)` 重算。这不等于本轮重新取得全Git tree验证每个文件在远程commit中的成员资格。所供获取回执及双秘密扫描结果是**供应方历史记录**，本轮没有重新运行Gitleaks，也不伪造当前扫描报告。

## 2. 实际执行项目、命令及输出

| 编号 | 实际操作/命令 | 实际结果和边界 |
| --- | --- | --- |
| L01 | Files两次当前附件内容搜索；ZIP在已挂载路径直接读取 | 两次搜索均无解析命中；转用容器处理ZIP，没有推断附件不存在 |
| L02 | `python /mnt/data/atlas_work/verify_input.py`；zipfile CRC、路径/普通成员/唯一名检查、manifest/hash/Git blob核验和安全解包 | 初始结果 `checks/input_verification.json`；全部身份检查闭合；没有执行解包源码 |
| L03 | `python scripts/check_input_identity.py /mnt/data/openfhe-parameter-atlas-a4b815a.zip checks/input_identity_recheck.json`（在交付根） | 独立只读ZIP再核对；JSON failures=[]；脚本副本已交付 |
| L04 | `python /mnt/data/atlas_work/read_source.py RELPATH RANGE PURPOSE`；按日志逐个范围读取 | `READ_REQUESTS.jsonl`185个范围请求、72文本路径；127引用锚点；部分大输出截断，不宣称每个请求行都读完 |
| L05 | Python pathlib/re对项目C/C++的 `Set[A-Z]\w*` / `Enable` 词法枚举；另有profile/常量宽松行索引 | 527处47符号；668索引行；包含注释/负控/工作流上下文；不作AST可达性结论 |
| L06 | 所供论文二进制安全TXT读取；本地 `fitz.open(PDF)`、`load_page(n).get_pixmap(...)` 渲染，配合图像工具目视 | TXT90235 bytes含4 NUL；PDF15页；目视4/5/6/7/8/9/11/12/13页；交付7/8页PNG；没有OCR |
| L07 | Web固定官方源码/固定OpenMP标准条款查询与网页读取；公开论文PDF打开/截图尝试 | 官方PRNG/HYBRID和OpenMP private/firstprivate用于交叉核对；PDF远端返回403，改读已供同hash本地PDF；不假装截图成功 |
| L08 | `python scripts/check_scalar_constants.py /mnt/data/atlas_work/input checks/scalar_constants.json`（在交付根，stdout另存） | 纯公开整数、Fraction、模幂、输入半径界；`checks/scalar_constants.json`和`checks/scalar_stdout.json`实际输出；不导入/执行OpenFHE |
| L09 | `python /mnt/data/atlas_work/make_coverage.py` | 454行SOURCE_COVERAGE；185请求/72路径/127锚点/67引用文件；hash/index349、词法32、定点语义来源67、定点上下文5、PDF1 |
| L10 | `python /mnt/data/atlas_work/refine_dictionary.py` | 仅修订输出文档/字典：33字段补类型/单位、条件运行证据等级、observer指数与幂上界消歧；未触碰源输入 |
| L11 | `python scripts/check_document_consistency.py /mnt/data/atlas_work/input /mnt/data/atlas_return checks/document_consistency.json` | 实际自检结果见JSON：字段/setter/引用/行界/本地链接/表格/profile行/读取账/源未变核对；不是独立语义证明 |
| L12 | Python hashlib/json/zipfile生成self-excluded输出manifest及ZIP；逐个成员再验bytes/hash并CRC | 以最终MANIFEST逐文件记录为准；ZIP自身hash另在交付答复，不在内部制造循环自hash |

上述通用命令中的 `RELPATH/RANGE/PURPOSE` 不是假造单次命令：具体执行范围和purpose逐项保存在READ_REQUESTS日志，原范围显示脚本副本在 `scripts/source_reader_used.py`。直接sed/grep/JSON/ls检查用于目录、表格和已引用内容的再核对，不伪装成新增全文语义阅读。

## 3. 纯标量检查的准确结果

两profile各12条Q/P常数、同余q≡1 mod65536、两两互素、固定7-base Miller–Rabin筛查、root的N/2N幂条件已核对。固定公开模幂枚举找最小有效根：S100全为最小；S116三枚新Base0/Base1/Div为有效非最小根。这里只做模幂和乘法，**没有执行NTT变换**。三枚新S116 prime另有显式Proth证书（witness5/7/11）。不把固定base筛查描述为已经调用某外部证明器。

S100 Q/ QP位长620/680；S116为652/712。两profile各8步Fraction递推与闭式相等，记录每轮active/family基、丢弃Mult、完整分子/分母、local level与兼容元数据。第8轮真实scale的log2近似分别100.000526271848、116.000021932683；近似小数仅展示，精确判断用有理数。

context `3.19f`扩为double是3.190000057220459，float32 bits为0x404c28f6；Peikert截断ceil分别39（context）与13（条件private默认σ1）。这只验证常数/语言分支推导的数值，不证明历史局部DGG实际采用哪个σ。

16384个输入槽的精确有理半径平方枚举：原输入全部严格在单位圆内；annulus全部严格在125/128内，输出最小幅度的下界检查通过。没有FFT、编码、密文或新随机数；它不是新annulus加密PASS。

论文定理4.8的常数多项式代数一致性例子：N2,d13,q17,Q=17×12289；按定义输出221，显示目标2873，差2652而显示界18/17。JSON核对nonwrap条件与加d后的归一化一致。它只针对印刷公式，不是当前生产加密反例或算法RED。

## 4. 文档自检发现并修正的输出问题

正文、随机数路径和字典统一为：h128 TUG内部实际消费BUG符号；不能把binary sampler记作整个闭包完全未用。`ScaledOneNormExponent`返回k=ceil(log2(C/Δ))的整数指数，2^k才是幂上界；两者已分开。少数测试定位名称已改成输入中实际存在的文件名。33 CC字段的类型/单位已逐项补充。上述是本轮**文档作者自校正**，不报告为项目算法缺陷。

读取辅助步骤曾有一个猜测测试文件名不存在、一次range参数格式错误和一次JSON键名误用，均在真实目录/字段确认后改正；它们不是源码构建或算法测试失败。大输出截断已在覆盖表逐项警示，没有用请求日志掩盖它。文档一致性检查第一次22组中21组通过，binary路径术语检查失败：辅助检查器错误地要求说明中必须出现汉字“内”，但实际字典已写“h128 TUG的符号采样实际调用它”。原脚本与实际失败结果保存在 `checks/document_consistency_attempt01.py` / `.json`。仅将该词法断言改为同时检查BUG、h128、TUG、实际调用四个已有词；不修改任何项目源码，也未借此证明采样器语义。修正后的再次执行结果在 `checks/document_consistency.json`。

## 4.1 最终实际复核

修正仅涉及文档检查器的措辞匹配后，22组文档/静态检查全部通过，failures=[]，结果保存在 `checks/document_consistency.json`。初次失败与其原检查器仍保留，没有把初次失败改写为通过。

另一次实际命令为 `python scripts/check_scalar_constants.py /mnt/data/atlas_work/input checks/scalar_recheck.json > checks/scalar_recheck_stdout.json`。对两次完整JSON逐字节比较为一致；二者SHA256均为 `34c32a054b0f1d2bf6895b43ae44c0321df40360ec0d13472a357478641a42de`。详情 `checks/scalar_reproducibility.json`；这只是确定性纯标量检查复核，不是第二次加密试验。

最终把解包目录的实际文件集与输入manifest比较，并重算453个payload的SHA256：454总成员，missing/extra/changed均为空。记录在 `checks/source_unchanged.json`。输入project/official/测试/工作流字节均未更改。

## 5. 明确未执行的事项

未build/CMake配置、未运行ctest/pytest或项目测试、未FFT/NTT、未加密/解密、未调用PRNG/采样器、未取/要求/输出实际seed或秘密key/noise、未运行CI、未远程提交、未修改生产/测试/CI文件、未调用旧实现或改版OpenFHE、未联系作者、未测性能/新安全位数、未派发1000次或任何新实验。没有假冒独立审查者、Fable或任何未实际使用的系统；没有声称可核验页面思考档/额度。

本轮只读取已提供历史运行接受记录，原S100 E80 FAIL、S116改参PASS、annulus改输入单样本PASS各自成立于其原样本与来源；不借本轮静态检查改写这些状态。所有GitHub源码链接固定commit，本轮验证的是本地对应path/hash/行界；没有声称逐条HTTP可达。

**根端复核文档覆盖与事实，再选择具体诊断。**
