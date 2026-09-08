# RELIN2-IMPLEMENTATION-BOUND-01 交付入口

本包是针对固定 clean-room 源码 `a4b815a733efe81897325e2a8e4c826a4ebfa439`、OpenFHE 1.5.0 pin `df495ba2e91739a6dc8f1de254fc5a41155ce504` 的数学／源码契约，不是生产补丁或 FHE 实验结果。

## 阅读顺序

先读 `RELIN2_BOUND.zh-CN.md`：它自包含地定义环、整数代表、两坐标和分区 carry、key 噪声、Relin2 重组、归一化、nonwrap 前提及原失败裁定。`CLAIMS.json` 提供 30 条机器可读命题和未知状态；`SOURCE_MAP.tsv` 提供 50 条固定原字节定位。

`NEXT_ACTION.md` 只给一个后继：采用带前提的实现契约，本缺口无需新增 FHE 运行，也不支持立即修改某个生产语句。原 S100 原条件 FAIL 不变。

`checks/README.zh-CN.md` 给出复算命令和 13 项确定性模型的边界。`checks/public_bounds.json` 保存八家族精确 Fraction 结果；TSV 是截断显示，不是测量值。模型常数全部公开合成，不含真实密钥、seed 或噪声系数。

`EXECUTION_LEDGER.md` 与 `evidence/` 记录输入 hash 核验、目标阅读覆盖、真正执行的检查、重复性自检和未执行项。`MANIFEST.json` 对本包全部载荷自排除登记 bytes／SHA-256。

## 验证完整性

需要 Python 3.10 或更新版本，仅用标准库：

```sh
python checks/verify_delivery.py .
```

独立复算结果建议写到本包目录之外，避免修改已冻结的成员集合；详见 `checks/README.zh-CN.md`。本返回包不重复附带完整输入源码和论文：输入身份及逐文件 hash 已保留，复核源码时使用用户提供的原 ZIP。

**状态：模型 13 PASS；公开界 8 家族计算 PASS；生产 FHE 运行 0；生产补丁无；原 S100 FAIL；完整复现尚未完成。**
