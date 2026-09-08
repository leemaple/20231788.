# OPENFHE-CKKS-PARAMETER-RANDOMNESS-ATLAS-01

> 根端勘误阅读版：精确函数名已修正；NTT 缓存结论限于表示/生命周期风险，不能据此认定已有算术失败。请与[根端复核说明](../REVIEW_NOTES.zh-CN.md)合读。`checks/` 保留原作者原稿自检记录，发布版复核另见 coordination 台账。

本包是固定源码的中文参考文档及实际静态检查记录，不含生产补丁，也不包含新加密实验。

工程：`a4b815a733efe81897325e2a8e4c826a4ebfa439`。OpenFHE 1.5.0：`df495ba2e91739a6dc8f1de254fc5a41155ce504`。输入ZIP：2,273,139 bytes；SHA256 `abc4df778754a2d3713f1442fbfe34d69af274f3638e65d4d362f35aa93fdd68`。

## 阅读顺序

| 文件 | 用途 |
| --- | --- |
| [中文主图谱](OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md) | 完整机制、参数、精确profile/scale、13步计算链、论文映射与排错表 |
| [随机数路径](RANDOMNESS_PATHS.md) | 底层熵/PRNG/采样器到根密钥、PKE和evalkey的分布与生命周期 |
| [变更影响](CHANGE_IMPACT.md) | 参数变更必须联动的对象、预计算、fixtures/oracle/门槛和失效证据 |
| [发现登记](FINDINGS.md) | 条件性新风险与历史公式问题，明确未证明的运行后果 |
| [覆盖验收](COVERAGE_AUDIT.md) | 精确覆盖计数、未逐行阅读部分和构建/论文未知项 |
| [执行账](EXECUTION_LEDGER.md) | 真正运行的hash/词法/Fraction/链接检查及未执行项 |

`PARAMETERS.json`含132条参数记录。`SOURCE_REFERENCES.json`给出127个固定源码锚点；正文引用可跳到同文件源索引并跟随固定commit链接。`PROJECT_SETTER_OCCURRENCES.json`为527个词法出现位置；`PROJECT_PROFILE_CONSTANTS.json`为668行宽松profile常量索引；`CONTEXT_API_SURFACE.json`分开登记context自有字段。`SOURCE_COVERAGE.tsv`覆盖全部454输入成员，`READ_REQUESTS.jsonl`保留185次实际显示请求（不是全部请求行都完整审阅的声明）。

## 复核脚本

三个可携带脚本只用Python标准库，不导入OpenFHE，也不运行源项目。将原ZIP另行安全解包到 `INPUT_ROOT`；参数为原始附件路径、解包根目录及本包目录。所有脚本只读取源输入，在指定输出JSON写结果。

```bash
python scripts/check_input_identity.py /path/openfhe-parameter-atlas-a4b815a.zip /tmp/input-check.json
python scripts/check_scalar_constants.py /path/INPUT_ROOT /tmp/scalar-check.json
python scripts/check_document_consistency.py /path/INPUT_ROOT /path/ATLAS_DIR /tmp/document-check.json
```

`source_reader_used.py`是实际使用的原作者工作区辅助脚本副本，带工作区绝对路径，只为执行来源留档；不把它宣称成无需改路径的可移植入口。已有实际JSON在 `checks/`，不是脚本预期输出。两个论文页PNG是给定PDF的本地渲染证据，不是从TXT重画的公式。

最终 `MANIFEST.json`逐件列bytes/SHA256，自身排除。核验脚本的通过只表示它声明的静态检查通过，不能替代源码语义审查、性能/安全验证或新的FHE数值结果。原S100仍为E80 FAIL；S116是QP约712的独立改参PASS；annulus是改输入且重新生成key/noise的单样本PASS。

**根端复核文档覆盖与事实，再选择具体诊断。**
