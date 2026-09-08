# 本次独立审查执行账本

## 1. 执行环境、输入和工作边界

本次工作容器：Linux x86_64，Python 3.13.5。标量脚本使用Python标准库Decimal/Fraction及整数运算；没有安装/调用OpenFHE执行环境，没有编译或运行C++、执行FFT、生成密钥或生成新密文。查看源码中的FFT实现不等于执行FFT。

输入原件：`/mnt/data/annulus-independent-03f37b6.zip`，13,743,164 B，SHA-256 `c30b6e84a53712792cfda4ec17da7b434ee745aebad3949916a90fa6d528a0b1`。

隔离解包：`/mnt/data/annulus_packet`。交付文件：`/mnt/data/annulus_review`。未改写源包、解包载荷或远端记录。本次最终质量检查再次比较全部输入成员。

源代码绑定03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b；task/evidence commit695a951a7379d355cf9d167cd14c9a17d9d25199；固定官方OpenFHE pin df495ba2e91739a6dc8f1de254fc5a41155ce504。内容散列相符不等于获得完整git对象库、提交树证明、缓存二进制来源或远端平台认证。

## 2. 独立首轮与作者阅读顺序

先完成ZIP身份、CRC、310个安全常规唯一成员、自排除MANIFEST及309件载荷大小/SHA核对；检查了170个所声明git blob的内容散列。字节核验必须读取作者文件字节，但当时未展示/解释作者正文。

首轮阅读覆盖包内论文完整TXT及关键PDF页图、四个生产src和四个include、当前端到端测试、相关observer/oracle/controls、实际run_once/finalize/replay/CI，以及本次和原S100原始数据/执行记录。原始大TSV逐槽由程序检查，而非声称人工目视逐字阅读。测试与历史材料按相关范围读取，不声称手工逐字审完全部310件载荷。

`FIRST_PASS_READ_RANGES.jsonl`保留冻结前80项范围阅读记录。个别长工具返回被截断后，对关键范围作了继续阅读；单纯“输出过一个很大的范围”不等于每个字符都已在工具界面可见。该索引不囊括所有cat/grep/容器操作，不冒充完整、第三方认证的终端录屏。

**2026-09-08T04:33:32.438385Z**冻结FIRST_PASS及当时脚本/结果，清单见`FIRST_PASS.freeze.json`。FIRST_PASS SHA-256：

```text
eaea61c388cadb419097917d16ac93edf7abbce2833a87bf03e7389b9ef444e5
```

作者正文的首个范围阅读记录为**2026-09-08T04:33:45.651214Z**，严格在冻结之后。后续范围记录保留在`READ_RANGES.jsonl`（共100项，其中18项为after-first-pass正文）。作者阅读后才执行历史/当前receiver对照和补充固定Gaussian源码解释。首轮主判断未被作者报告推翻，首轮文件未回写。

该顺序记录是本地工作记录，不是可信时间戳服务或完全盲化证明；任务描述、源码注释、允许先读的历史解释事先可见。

## 3. 真实样本的纯标量重放：实际已执行

“真实”指输入是保留的实际观测；**本次运行的是标量检查器，不是再次运行加密试验。** 所有新/旧样本都来自同一输入ZIP，无替换密钥。

|执行项|实际输入和精度|本地脚本结果／退出|保留文件|
|---|---|---|---|
|新annulus独立重放|全部16384槽，180十进制有效位|PASS，exit0|`independent_180.json/.log`|
|新annulus独立重放|同一真实TSV，230十进制有效位|PASS，exit0|`independent_230.json/.log`|
|旧S100独立重放|Linux及Windows全部槽，180十进制位|脚本完成exit0；两条历史E80均FAIL|`old_s100_180.json/.log`|
|旧S100独立重放|同上，230十进制位|脚本完成exit0；两条历史E80均FAIL|`old_s100_230.json/.log`|
|身份/进程/清单复核|输入ZIP、全部成员、五文件hash、run/job/argv/时间/门禁|PASS_WITH_PACKET_PROVENANCE_ASSUMPTION，exit0|`packet_and_process.json/.log`|
|所给当前receiver补充重放|真实annulus TSV，source03f37b6，process-exit0，precision230|PASS，exit0|`supplied_replay_230.json/.log`|

表中JSON位于`results/`、log位于`logs/`。0退出的旧样本解释器只是正确复算出原失败，不能写成旧实验PASS。原run34039088536的CTest exit8保留在结果；新run34184869227的实际payload returncode0/timed_out=false来自未修改的end记录并与raw/stdout/run绑定。

独立程序使用原始exact dyadic x、复数模、同槽I8/A8、差幂因式分解以及独立端点相减复核。它不调用作者replay来产生核心结果。所给receiver运行只是附加比较，不是唯一依据。

新样本whole-process耗时33.852879秒；Python标量重放自己的耗时也写入JSON，二者均不能冒充论文纯乘法计时。

## 4. 合成／精确标量／静态挑战：与真实样本严格区分

|执行项|实际结果|说明|
|---|---|---|
|`independent_boundaries.py`|13组检查通过，exit0|纸面归一化/符号/进位反例，范数/残差/headroom反例，小模数恒等式，条件域预算，旧I8反事实，内存负例，一次性事件谓词和迁移路径拒绝|
|同脚本`--literal-paper-red`|实际exit1，**预期RED**|印刷Thm4.8归一化式的标量反例，不是C++生产源码RED|
|历史receiver三负例|实际exit1，**预期RED**|真实原记录正例PASS；合成producer不一致/缺LF/CRLF均假通过|
|当前receiver三负例|实际exit0，GREEN|真实原记录正例PASS；三合成负例在对应检查点全部拒绝|
|`post_author_scalar.py`|实际exit0|相对条件数256、实际新域绝对增益、S116收据的√2上界、固定Gaussian分支的条件性整数算术|

历史/当前receiver内存变体没有写回raw.tsv，更没有把变体包装成实际实验。原始raw的SHA在结果中重复核对。

对finalizer的负向挑战只是将五件未经修改的远端原始文件复制到临时目录；它因远端绝对argv/output不对应当前目录而拒绝`wrong process entry/output`，未生成verification。没有改原记录去骗过这个约束，亦不声称迁移后的本地finalizer曾成功。

## 5. 未执行和无法从现有材料恢复的事项

未执行新加密、keygen、任何实际FFT、C++构建、默认60测试、controls、原S100重跑、S116重跑或新Windows样本。涉及这些旧记录的判断分别标为保留日志/收据，不伪装成本机执行。

S116本包没有完整槽TSV，因此没有独立全槽重放；fresh诊断没有在本次重算全部多项式；annulus TSV没有序列化fresh producer全槽列、512/768变换明细、Horner明细、全部整数phase或密文。因此源码内的这些断言不能全部由本次TSV独立恢复。

未证明FFT绝对误差、全链整数提升、HYBRID舍入普遍界或参数部署安全。未认证缓存二进制构建来源、runner真实性、git历史完整性或tag全历史从未重建。所给workflow和记录支持这一次调用，不支持无状态谓词无法表达的永久唯一性。

未执行GitHub写入、dispatch、rerun、消息、作者联系、外部任务或定时任务；没有访问用户本机、旧实现、浏览器状态、凭据或私钥。没有建立新依赖树或假定已有本地私库。

用户报告原包定向扫描和Gitleaks8.30.1均0 findings，本次**没有重新运行Gitleaks**，不把内容哈希核对写成独立的秘密扫描认证。输出仅包含审查文档、标准库脚本、阅读记录、实际检查JSON与日志，不包含密钥或新密文。

## 6. 工具受限、失败与透明性

Files读取ZIP没有可解析正文，故直接使用已挂载附件路径作安全解包和程序化处理。在线PDF截图尝试受访问限制；改看所给PDF本地页图，没有OCR。外部补读仅两个固定commit的官方Gaussian源码URL，详见`EXTERNAL_REFERENCE_NOTES.md`。

精简包的compile workflow引用了`project/coordination/comprehensive-reassessment-20260908/check_receiver_agreement.py`，本包中缺此文件。尝试读取时得到FileNotFound，后续查找确认缺项；这不是实际远端CI失败。本次独立receiver对照覆盖了所需机制，但未冒称能在此精简包原样运行完整compile-CI。

预期RED的exit1和finalizer预期拒绝如实记录，不计作新实验失败，也不掩藏成成功。某些log保存的是脚本实际stdout以及调用层追加的退出码说明，而非逐个shell命令的全量原始会话转录；复现命令见`REPRODUCE.md`。

## 7. 最终交付核验

`checks/verify_delivery.py`仅进行读取、AST解析和结果一致性检查，未再执行加密或原有标量实验。实际结果写入`results/delivery_quality.json`和`logs/delivery_quality.log`。它核对FIRST_PASS冻结、输入原件/解包不变、两档数值/槽/门禁、旧FAIL保留、合成RED/GREEN分离、脚本语法和必需文档。

最后生成自排除`MANIFEST.json`，逐件列出大小和SHA-256并绑定原ZIP与源码。交付ZIP在输出目录之外生成，避免自包含；压缩包CRC和每个输出载荷散列再次核验。校验链只能证明交付内容与所声明文件相符，不承担未获得的科学或安全证明。
