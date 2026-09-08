# 执行账本：INITIAL-LIFT-NONWRAP-01

## 1. 本轮执行边界与环境

只读取附件源码/历史文档/论文，运行本轮新写的有限整数、Fraction、标量区间、文本/AST/hash检查，并生成本返回包。输入解包位置为 `/mnt/data/initial_lift_input`；返回候选写入独立 `/mnt/data/INITIAL-LIFT-NONWRAP-01`。没有修改解包输入的任何文件。

实际容器Python为3.13.5，Linux x86_64；详细字符串见 `evidence/ENVIRONMENT.json`。阅读账本使用容器UTC（2026-09-08，对应Asia/Singapore次日时段），只记录容器观察，不认证用户端时间、Windows构建或后端型号。本轮没有可验证的Pro界面档位/API模型证明，不声称认证了这些设置。

下表记录实质执行及保留位置。无全局shell会话逐字记录的短暂内容查看命令没有伪造成可复现日志；可独立重放的数学/验包/一致性命令、结果与错误另行保留。

## 2. 实际执行顺序

| 阶段 | 实际执行与结果 | 保留材料 |
|---|---|---|
| E00 附件取得 | Files语义搜索两次无结果；使用本轮已挂载ZIP。未访问旧对话、本机或私库。 | 用户输入身份；本账本 |
| E01 首次验包 | ZIP bytes/SHA、537普通唯一成员、安全路径、CRC、536载荷size/SHA、527个Git blob本地内容核对通过；完整读取根TASK。 | `evidence/INPUT_VERIFICATION.json`、`INPUT_TASK.md` |
| E02 阅读依赖 | 先ADOPTED_CONTRACT，后两复核与所需原推导/源码/历史来源。采用三处修订，不执行旧TASK/NEXT_ACTION。 | `READ_REQUESTS.jsonl`、`SOURCE_MAP.tsv`、`evidence/adopted_dependency/` |
| E03 论文 | 对公开PDF取得/截图入口的尝试受限，没有得到可用网络截图；回到随包同hashPDF。局部PyMuPDF文字提取与页面渲染，视觉核对物理页4/6/7/8/13；未OCR。 | `PDF_READ_RECORD.json`；SOURCE_MAP的P4/P6/P7/P8/P13 |
| E04 主检查首试 | 新写 `checks/run_checks.py`。前述数学模型和预算生成后，最后读回JSON分子/分母字符串构造Fraction发生TypeError，exit1；不算本轮总PASS。 | `evidence/attempt01/run_stderr.txt`，空stdout |
| E05 主检查修正 | 只把分子/分母显式转int；主检查exit0。之后将重复的大尺度整数改成等价因子幂表示，closed product断言不变，再次exit0。 | 前版成功stdout在`attempt02/`；最终主检查`checks/run_stdout.txt`、空stderr、5个主结果文件 |
| E06 候选标量 | 只导入区间候选的无副作用定义。运行7500个端点断言、区间平方、Machin π和小角Taylor标量包络、13个schema负例，exit0。合成零p只用于解析，不是实际编码。 | `checks/results/candidate_scalar_checks.json`、对应stdout/空stderr |
| E07 候选静态 | 01/02 patch仅内存重放，四完整文件相符，原ComputeEncoding函数体原字节不变、无新增密钥/采样调用、默认CTest/CI不变；只做AST解析，exit0。 | `checks/results/candidate_static_checks.json`、对应stdout/空stderr |
| E08 交付交叉核对首试 | 为标量/静态检查增加可选`--out`，不改变数值模型或候选代码。新一致性脚本重放3个作者侧命令均exit0；其后把实际7个结果文件误写为应有8个，计数断言失败。 | `evidence/attempt03/final_consistency_stderr.txt`及该次3命令stdout/stderr |
| E09 计数更正后核对 | 仅修正交付文件计数7。再用独立输出目录重放同3命令，均exit0；7个结果逐字节一致；20主张/6假设DAG、51源码范围hash与阅读请求覆盖、8原字节快照、168个精确有理数显示包络均通过。 | `FINAL_CONSISTENCY.json`、stdout/空stderr、`replay_final/` |
| E09b 候选精度静态修正 | 识别原16项Taylor在未来N4小模型下余项不能保证2^-160宽度；改为32项，补最坏角11/14下余项<2^-256的标量断言，标量与最终3命令重放通过。没有执行变换或修改C++/生产公式。 | 最终候选、标量结果与FINAL_CONSISTENCY回执 |
| E10 再验输入 | 便携只读验包器核对原ZIP与整个解包目录逐字节未变；额外949个嵌套来源row的大小/hash通过；所有pin标签与清单相同。 | `INPUT_UNCHANGED.json`、`verify_input_stdout.txt`/空stderr |
| E11 交付检查 | 有限literal凭据模式扫描；生成自排除MANIFEST，ZIP封装并读回CRC/成员/hash。最终归档回执置于ZIP外，以避免自hash循环。 | `RETURN_TARGETED_SCAN.json`；`MANIFEST.json`；外置`DELIVERY_RECEIPT.json` |

另有两次不影响数学或输入的内容查看错误：一次使用不存在的测试文件名后改读实际 `paper_full_eight_square_contract_test.cpp`；一次将结果JSON字典误按索引0查看产生KeyError，后按键读取。它们不构成测试运行、数值失败或生产缺陷。本账本没有把交付工具自身的三类失败隐藏为全程成功。

## 3. 可复算的实际命令

首轮主命令（路径来自当前容器，不要求根端存在这些绝对路径）：

```text
python -B /mnt/data/INITIAL-LIFT-NONWRAP-01/checks/run_checks.py --input-dir /mnt/data/initial_lift_input --out-dir /mnt/data/INITIAL-LIFT-NONWRAP-01/checks/results
```

最终可携带重放使用原字节快照：

```text
python -B checks/run_checks.py --input-dir checks/bound_source --out-dir <fresh-external-dir>/results
python -B checks/check_candidate_scalars.py --out <fresh-external-dir>/results/candidate_scalar_checks.json
python -B checks/check_candidate_static.py --input-dir checks/bound_source --out <fresh-external-dir>/results/candidate_static_checks.json
```

完整3个真实argv、exit0、UTC及stdout/stderr SHA记录在 `FINAL_CONSISTENCY.json`；实际stdout/stderr备份在 `evidence/replay_final/`。每次重放只运行本轮作者脚本，不执行输入工程。最终3个子命令stderr均为空。

只读输入/交叉检查命令：

```text
python -B checks/verify_input.py --zip <input.zip> --extracted <immutable-input-dir> --out <return-dir>/evidence/INPUT_UNCHANGED.json
python -B checks/final_consistency.py --input-dir <immutable-input-dir> --replay-dir <fresh-external-dir> --out <return-dir>/evidence/FINAL_CONSISTENCY.json
```

一致性脚本核对JSON pointer、DAG及证书显示不是独立科学复核；模型作者没有将自身检查标为根端/第三方采用。

## 4. 本轮明确为0或未执行的项目

| 项目 | 本轮实际次数/状态 |
|---|---|
| C++编译/链接/运行 | 0；候选RED链接失败和GREEN均未观察 |
| 任何FFT/NTT或canonical变换 | 0；candidate函数`canonical_bounds`未调用 |
| `candidate/tests/test_transform_models.py` | 未运行；其中4个小型变换属于未来后继 |
| 密码学采样/keygen/Encrypt/Decrypt | 全部0 |
| DCP/Tensor/Relin/RS/RCB生产执行 | 全部0；只读源码、运行小型整数模型 |
| 原项目测试/旧脚本 | 0 |
| 历史完整全槽记录读取/重处理 | 0；包内未提供，未外部取得 |
| 原S100、S116、annulus重跑 | 全部0 |
| 输入项目改写 | 0；E10逐字节验证 |
| CI、Actions、网络派工、作者外联 | 全部0 |
| 源码支持39对应的历史sigma测量 | 未执行/未知 |
| 实际编码幅度K-CAP | 未测，null |
| 历史wrap整数与高低秘密相位 | 未取回，null |

整数模幂核对冻结root不等于NTT；16,384个公开公式平方模不等于历史全槽捕获重算；标量Taylor/π包络不等于运行变换；patch文本重放不等于编译GREEN。

## 5. 返回包扫描与归档

输入自带上传前/后Gitleaks零发现记录只作为输入证据。本容器未取得可用gitleaks程序，因此**不宣称本返回包通过独立Gitleaks扫描**。本轮只执行有限的AWS/GitHub/Slack token和PEM私钥标记literal模式检查，具体范围/模式/零发现记录在 `RETURN_TARGETED_SCAN.json`；该扫描不等价于任意秘密泄漏证明，也不扫描无关环境/账户。

`MANIFEST.json`列出除自身外所有payload的实际bytes/SHA256，绑定原输入ZIP/hash与固定四个提交标签。最终ZIP bytes/SHA256、归档成员集合、CRC和逐文件核对结果写到外置 `DELIVERY_RECEIPT.json`，不将zip自身hash塞进被其包含的文件。根端应先验归档，再读全文和脚本、独立挑战数学。

## 6. 终态

主数学模型与有理条件证书PASS；实际K-CAP仍未知；C++/变换候选未执行。原S100 E80 FAIL保持；S116改参PASS与annulus改输入/新样本PASS各自保留。唯一后继为 `PUBLIC-S100-ENCODER-CAP-01`。没有论文全目标完成声明。
