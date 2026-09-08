# EXECUTION_LEDGER — RELIN2-IMPLEMENTATION-BOUND-01

## 1. 身份、角色与输出性质

本记录对应用户提供的唯一附件和任务取证批次 **2026-09-08**，不以批次日期冒充每条命令的执行时间。本轮角色为数学／源码契约作者，不是本产物的独立审查者。交付不修改生产实现，不声称完成原 S100 复现。

|对象|核验值|
|---|---|
|输入 ZIP|`relin2-implementation-bound-a4b815a.zip`|
|ZIP bytes|`3319606`|
|ZIP SHA-256|`1aba188016d93ca8b38ec72f0f2592c6b45de562c455cb33e6d303d95ec6dc4c`|
|普通唯一成员／展开 bytes|`500 / 10640184`|
|外层 MANIFEST SHA-256|`34b3a05c62c48a55dba5bdcdc8fde0e1f397cb831dce35a95f53cc97129422b3`|
|任务提交|`def8ccfde3c67bf00d7dc6a009451704b1b045b4`|
|协调证据基点|`bbd4e73af74d1b072e3beb588cf9c7c4de3117cc`|
|生产 source|`a4b815a733efe81897325e2a8e4c826a4ebfa439`|
|官方 OpenFHE pin|`df495ba2e91739a6dc8f1de254fc5a41155ce504`，1.5.0，native64/backend4|
|附件论文 PDF SHA-256|`61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`|

核验的是附件与其记录的固定身份一致。491 个 Git blob SHA-1（其中官方文件 331 个）由文件原字节重新计算；没有以此冒称本轮向远端重新验证了全部文件的 commit membership。内层旧 manifest 只作来源记录，成员闭合以外层 manifest 为准。

## 2. 实际读取与检索

先尝试 Files 对当前上传 ZIP 的任务名／Relin2 检索，两次均无可用解析结果；随后使用已经挂载的附件原字节进行容器读取，没有索取另一对话或本机资料。

执行了安全成员检查、CRC 检查及解包；阅读完整根 TASK，沿论文与 `double_ckks.cpp`、`repeated_mult2.cpp`、官方 HYBRID／DCRT／Rescale／DGG 消费路径作定点源码研究，再对照提供的图谱和历史报告。`SOURCE_MAP.tsv` 保存 **50 条实际论证定位，24 个唯一文件**，包括 PDF；其中文本映射窗口的并集为 2106 行。该统计是论证覆盖，不是所有阅读量，也不是声称全审了 500 文件。

`evidence/READ_REQUESTS.jsonl` 保存读取辅助脚本的请求日志。早期大窗口曾有工具输出截断；因此它不能单独证明每个被请求行都已逐行阅读。最终用于证明的文件、函数和具体窗口，以 `SOURCE_MAP.tsv` 为准。`evidence/READ_COVERAGE.json` 为其汇总。检查全部文件的 hash 不等于全文审阅。

论文公式以附件 PDF 图像为准。曾打开 IACR 公共 PDF 并尝试 web PDF screenshot，第 7／8 页对应的截图请求受工具访问限制未成功；随后本地渲染并实际查看附件 PDF 第 **4、5、7、8** 页。没有把 TXT 中的 NUL／公式排版缺失当作公式证据，也没有声称已查看全部论文页面。公共网页只作定位；本契约以附件固定 PDF 和源码为输入。未联系作者，未发起网络派工。

## 3. 实际运行的可复算检查

以下路径记录本轮运行位置；使用者复算时可替换工作目录。纯 Python 环境为：

```
Python 3.13.5 (main, Jul 15 2026, 20:25:40) [GCC 14.2.0]
```

这里的 GCC 是本轮 Python 环境的构建标识，**不是**重新构建 OpenFHE 的证据。原 S100/S116 的 GNU13.3／GNU16.2 及 OpenMP 版本来自附件历史构建取证记录。

### 3.1 输入验证

实际执行：

```sh
cd /mnt/data/relin_work/output
python checks/verify_input_archive.py \
  /mnt/data/relin2-implementation-bound-a4b815a.zip \
  --output evidence/INPUT_VERIFICATION.json
```

结果：exit 0；500 普通成员，499 项载荷 bytes／SHA-256 全部匹配，491 个 Git blob 重算匹配，其中官方 331 个；CRC 和成员闭合 PASS。stdout 保存在 `checks/input_verification_stdout.txt`，逐文件证据在 `evidence/INPUT_VERIFICATION.json`。本轮最终还对解包后的所有 499 个载荷及原外层 MANIFEST 再次计算 hash：全部未变，成员闭合仍为 500，见 `evidence/EXTRACTED_INPUT_UNCHANGED.json`。

### 3.2 整数／有理模型

实际执行：

```sh
python checks/model_checks.py --output checks/model_results.json
```

最终结果：exit 0，**13 项 PASS**，见 `checks/model_results.json` 与同字节的 `checks/model_stdout.txt`。模型逐步补充检查后，以本包的最终 13 项为准；不是生产 FHE 的 RED/GREEN 迭代。

覆盖过强单坐标近可加等式的反例、修正双坐标关系正例、遗漏 G 或 carry／错误符号的预期失败变体、N=4 非常数负循环模型、零 d digit 与同 key 限制、DCP 相消、RS2 phase-wrap 与高低部关系、Fraction 归一化、充分条件失败但并未 wrap 的区分，以及当前公开 N/h/Q/d/P 上的符号见证 M13。

M13 的 256 是同一个确定性多项式的**系数类别数**，不是 256 次随机抽样；没有构造 NTT／FFT，也没有生成实际 evaluation key。所有模型 key／秘密／error 常数均为明确标记的公开合成对象，不使用或导出任何历史真实系数。

### 3.3 八个 S100 家族的公开界

实际执行：

```sh
python checks/public_bounds.py \
  --input-root /mnt/data/relin_work/input \
  --output checks/public_bounds.json \
  --table checks/public_bounds.tsv
```

结果：exit 0；固定源文件 hash 和 Q/root/P 常量匹配；8 个家族的精确尺度和 Fraction 界计算通过；12 组根的整数模幂阶条件及模数两两互素检查通过。没有执行变换，没有重新证明模数素性或密码安全等级。

保存精确分子／分母，不用显示截断数字进行断言。8 个局部 Relin 项均严格小于 `2^-80 / 2^40`；这些只是 C23 中局部项的公开界，不是原链 nonwrap 或总精度 PASS。粗 RS 项界大于 T 也不是实际误差的下界。

### 3.4 最终独立重算目录与静态完整性

在 `/mnt/data/relin_work/recheck` 重新运行上述三个脚本，四件结果文件——输入验证 JSON、模型 JSON、公开界 JSON／TSV——与交付目录原结果**逐字节相同**。所有子进程 exit 0，stderr 为空；实际 argv、stdout hash 和 Python 标识在 `evidence/DETERMINISTIC_RECHECK.json`。这只是本作者的可重复性自检，不冒称根端另行安排的独立审核已经完成。

四个交付 Python 脚本均经 `ast.parse` 语法检查。30 条命题的前提状态、50 条源码映射与输入 hash、未知历史运行条件保留 null，均经本轮静态交叉检查，见 `evidence/DELIVERY_STATIC_CHECK.json`。

输出还做了一次有限的 PEM 私钥／AWS access key／GitHub token／Slack token 字节模式检查，未发现匹配项。它不是 Gitleaks，不是完整秘密扫描认证；不把附件内原有的秘密扫描回执记成本轮执行。

## 4. 交付验证与清单规则

`MANIFEST.json` 对输出每个普通文件登记相对路径、bytes 和 SHA-256，自身排除；输入身份另存于 manifest 元数据。可运行：

```sh
python checks/verify_delivery.py /path/to/unpacked-delivery
```

验证器检查成员闭合、逐文件 hash、命题／源码引用、源文件 hash 与原输入身份一致，以及 13 项模型和 8 家族结果状态。验证成功仅代表交付文件完整性与内部引用一致，不是底层 NTT 正确性、历史 sampler 状态或原实验通过证书。
终封时实际运行上述交付验证器，exit 0、stderr 为空：成员闭合 24 件（含自排除 manifest），23 件载荷 hash 全部匹配，30 条命题、50 条映射、13 项模型、8 家族结果交叉检查 PASS。将本记录纳入清单后再次运行，结果相同。
输出 ZIP 的最终 bytes／SHA-256 在交付消息中列出，不将 ZIP 自身 hash 写入其内部制造循环依赖。

## 5. 明确未执行及未证明

未执行 OpenFHE configure/build/install、任何 FFT／NTT、FHE key generation／加解密／EvalMult／Rescale、PRNG 或 Gaussian 抽样、1000 次试验、生产测试或 CI；没有改写 `project/`、`official/`、测试、CMake 或工作流。没有访问用户旧实现、历史秘密对象、另一对话的附件或未提供运行时。

本轮没有重算原两条链的全槽捕获，没有测量历史 private DGG σ，没有闭合 annulus 缓存二进制身份，没有证明原链各步输入 lift／phase-wrap 变量为零，没有找到一个错误生产语句或相应生产 RED。

保留结论：`original_S100=FAIL`；`S116/annulus` 仅为各自条件下的历史 PASS；`production_patch=NONE`；`new_FHE_runs=0`；`complete_reproduction=NOT_COMPLETED`。唯一后继是 `NEXT_ACTION.md` 所列的静态契约采用，而非新增抽样或改噪声追求绿色结果。
