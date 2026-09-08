# REPRODUCTION-ADJUDICATION-01 交付入口

先读 `DECISION.zh-CN.md`，再看 `REQUIREMENT_EVIDENCE.tsv`（32项）和 `FINDINGS.md`。本轮**未发现有具体证据支撑的生产算术缺陷，不提交生产修复补丁**；原 S100 E80 仍 FAIL。

新增内容是两个真实系数的精确理想值证明、一个 source-bound 幅度合格/Ecd不合格语义负控、条件性的纯编码八平方预算，以及唯一下一切片的完整候选。当前最终标量套件 53 项通过；完整逆嵌入候选未执行。

`SCALAR_PROOF.zh-CN.md` 给出所有新数学细节；`NEXT_ACTION.md` 冻结 PUBLIC-S100-ECD-CELL-01 的输入、解析预期、RED/GREEN、四项tiny/一次full上限、GitHub runner 命令及停止规则。**不要在本次审查额度中直接运行 candidate/run_reviewed_once.sh。** 它需要 Codex 独立审核及另行批准的隔离 runner。

`EXECUTION_LEDGER.md` 区分已执行与仅建议；`SOURCE_INDEX.tsv` 的路径/行号对应原输入包，SHA256 指向当前供件原字节。`source/`、`fixtures/` 以及标明 retained 的 evidence 是原件拷贝，不是本轮重新实验。

验证交付内容（只哈希，不运行实验）：

```bash
python -B tools/verify_delivery.py
```

根 `MANIFEST.sha256.json` 列出所有其他载荷，明确自排除。ZIP 外另给 `.sha256`；不要把运行结果写进本交付目录，否则严格 manifest 核验会拒绝未登记文件。重放 scalar 时 `--output` 同样应指向外部全新路径。
