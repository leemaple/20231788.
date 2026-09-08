# INITIAL-LIFT-NONWRAP-01 返回包

## 阅读入口

先读 `INITIAL_LIFT_NONWRAP.zh-CN.md`：真实公钥初始化 → DCP整数代表 → 八层相容提升 → 每类nonwrap margin → 原S100有限核算。`CLAIMS.json` 用已证/条件性/未证/历史记录/候选未运行分开事实；`SOURCE_MAP.tsv` 给51条固定文件行范围/PDF页/hash锚点；`NEXT_ACTION.md` 只有一个后继。

本次裁定：相容整数提升的代数桥已完成。以**实际编码**canonical幅度≤127/128为前提，八步所有列出的相位充分margin通过；但本轮未运行编码，该前提仍未知。原C25粗预算第5步首次失效不等于实际绕回。原S100 E80 FAIL、S116改参PASS、annulus改输入/新样本PASS保持分离。

## 目录

| 路径 | 内容 |
|---|---|
| `INITIAL_LIFT_NONWRAP.zh-CN.md` | 自包含中文推导、真实初值、八步证书、边界和结论 |
| `CLAIMS.json` / `SOURCE_MAP.tsv` | 主张依赖/未知量；源码与纸面锚点 |
| `checks/` | 本轮可运行的纯整数/Fraction/标量/静态检查、实际输出与原字节绑定快照 |
| `candidate/` | 未编译、未执行变换的公开编码诊断：RED/GREEN补丁、完整文件、独立区间候选与未来4模型TDD |
| `evidence/` | 输入核验、阅读账本、PDF记录、原TASK、已采用依赖及失败保留 |
| `NEXT_ACTION.md` / `EXECUTION_LEDGER.md` | 唯一后继的验收；本轮实际执行与禁止项0计次 |
| `MANIFEST.json` | 自排除清单；每项bytes/SHA256及输入绑定 |

`checks/bound_source/` 是4个输入原字节快照，仅用于独立重放本包检查和patch内存核对，不是完整可构建工程。`candidate/full_files/` 才是候选文件，不得将快照误当生产修复。完整源/PDF引用由输入ZIP的hash与 `SOURCE_MAP.tsv` 绑定，未重复打包整个输入或历史全槽记录。

## 在包目录重放本轮允许的检查

以下命令仅需Python标准库；不会运行OpenFHE、变换、采样或项目测试。为避免覆盖原始实测结果，使用一个新的输出目录。

```text
python -B checks/verify_delivery.py --root .
python -B checks/run_checks.py --input-dir checks/bound_source --out-dir replay/results
python -B checks/check_candidate_scalars.py --out replay/candidate_scalar_checks.json
python -B checks/check_candidate_static.py --input-dir checks/bound_source --out replay/candidate_static_checks.json
```

源码绑定有误、结果不符或断言失败均应保留并停止。若保有输入ZIP，可再运行：

```text
python -B checks/verify_input.py --zip /path/to/initial-lift-nonwrap-a4b815a.zip --out replay/input_verification.json
```

`--extracted /path/to/initial_lift_input` 可额外逐字节核对已解包目录；验证器不解包、不写输入。生成replay目录后，目录清单会因额外文件而严格拒绝“原包集合一致”；可删除独立replay目录或直接使用 `verify_delivery.py --zip 原返回ZIP` 核验原始归档。原始交付内部清单不会为后加文件自动背书。

**不要在本轮检查命令中运行 `candidate/certify_public_encoder.py` 的主程序或 `candidate/tests/test_transform_models.py`。** 它们含真实变换，只属于根端审查后的唯一后继。导入候选做标量测试不会触发变换。

## 证据口径

本轮执行9组代数模型、16384个公开公式平方模、冻结q/root核对、三条8层预算，以及标量区间/schema/patch静态检查。第一次主检查的字符串→Fraction TypeError有原stderr；修正后通过。没有C++/FFT/NTT/密码学运行，没有历史全槽重处理，没有派CI或改输入项目。

输入原件身份、537成员/536载荷与527个Git blob内容身份核验通过；这不等于远端Git祖先/二进制身份认证。候选RED/GREEN仅为设计，未声称真实发生。具体命令与限制见执行账本。

外置 `DELIVERY_RECEIPT.json` 记录最后ZIP大小/hash、清单hash与归档读回结果，以避免在包内制造自身ZIP hash循环依赖。完整论文目标仍在进行，本子任务不代表其完成。
