# 唯一后继：PUBLIC-S100-ENCODER-CAP-01

## 只改变一个未知判断

对原 near-unit S100 输入，认证**原 `ComputeEncoding` 一次实际返回的公开整数多项式 p** 是否满足

`max_{zeta^(32768)=-1} |p(zeta)/2^100|² <= (127/128)² = 16129/16384`。

`127/128` 是分析前提，不改输入、参数、scale、阈值或密码学样本。根端先独立审查本包推导、patch 与区间算法，随后才在所述 Windows/GitHub 构建环境手动执行本后继。本轮没有发出构建、CI 或外联请求。

这不是再次运行八平方，也不是精度修复。当前作用是把 `CLAIMS.json` 的 `A04_ACTUAL_ENCODER_CAP` 从未知变为已认证、被否定或区间不确定。若认证成立，结合已有诚实路径与精确环语义前提，八层条件相位证书适用于该 p 的全部有限支持噪声，而无需选择或新抽 key。

## 冻结输入与来源门槛

生产基点为 `a4b815a733efe81897325e2a8e4c826a4ebfa439`；官方依赖 pin 为 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。使用原 `paper_full_test::ClientInputs(paper_full_test::Inputs())`，N=32768、slots=16384、gap=1、S=2^100，原十一塔 q/root 顺序。新入口复用原 `ComputeEncoding` 函数体，不调用带采样的 setup/factory。

根端必须记录：基点/补丁/构建树 hash；编码函数、输入函数、实际 Boost headers 与编译选项的来源；编译器、OpenFHE/native64/backend4/构建缓存身份；命令、exit/stdout/stderr；公开 JSON SHA256。输出 JSON 的 commit/compiler 字段只是标签，不能自行证明来源。现有 Windows GCC/MinGW 风格 `__VERSION__` 路径可供审查；未声称已在 MSVC 编译。

无历史 p 或可核相关数值依赖等价证明时，本证书首先只适用于**该次公开编码结果**。不能仅靠同名输入把它配给旧全槽状态。来源等价由根端审查，不请求历史秘密、噪声向量或全槽加密记录。

## 一个动作内的 RED→GREEN 与执行上限

下列均是**未来计划，未在作者侧运行**。

1. 在独立工作树应用 `candidate/01-red.patch`。显式开启 `OPENFHE1788_PUBLIC_ENCODER_DIAGNOSTIC=ON`，只构建 `public_s100_encoding_dump`。预期 RED 是缺少 `InspectFixedS100PublicEncoding` 定义导致链接失败；若先发生环境错误，不算该 RED。保留实际日志。
2. 应用 `candidate/02-green.patch` 后，只重建同一 target。运行一次 `--api-negative`，应在创建变换表前拒绝空输入，计数 `encoding_calls=0`。这一步尚无真实 GREEN 记录。
3. 独立审查区间算法后，运行一次 `candidate/tests/test_transform_models.py`，恰四个解析可判定的小型变换模型。它们是未来有界 TDD，不是生产样本。
4. 运行一次 `public_s100_encoding_dump public_s100_encoding.json`。只允许一个原输入的 `ComputeEncoding` 调用；该调用内部原本有 primary/check 两条逆变换。没有 key generation、采样、Encrypt、Decrypt、DCP、Tensor、Relin、RS、RCB 或 observer。
5. 对这一个完整公开 JSON，运行一次：

```text
python -B candidate/certify_public_encoder.py --public-encoding public_s100_encoding.json --out public_s100_cap.json --allow-transform-after-root-review
```

只做一个全 N 向外舍入 canonical 正向变换，输出聚合证书；不得因失败/不确定再抽 key、换输入或自动重跑。命令中的显式许可开关只防误触，不是授权证明或安全隔离。

本动作预算：有效原输入编码 1 次（内部逆变换 2 条）；空输入 API 负例 1 次且编码 0 次；小型正向模型 4 次；完整区间正向变换 1 次；密码学采样/加解密/八平方各 **0 次**。TDD/构建计次与编码计次分别记录，不把编译次数算成采样。

CMake 候选是 OFF + EXCLUDE_FROM_ALL，无 `add_test`，不会让现有默认 CTest/CI 自动运行诊断。示意配置只增加 `-DOPENFHE1788_PUBLIC_ENCODER_DIAGNOSTIC=ON`，其余沿用根端已审查依赖配置；本包不虚构 Windows 本地路径或运行器名。

## 输出与可证伪验收

公开中间 JSON 包含原 p 的 32768 个整数、精确尺度、q/root、公开 geometry、输入/profile/构建标签与计数；它不是密文或秘密相位。根端保留该 JSON 以便独立重放。对外结论可只给聚合证书，包含完整数据 hash、p 的系数最大值、全奇根平方模最大值的有理上下界、区间宽度、root seed 区间与运算计数。不输出 s、v、e、seed、密文、evaluation key 或秘密导出量。

| 结果 | 判断与验收 |
|---|---|
| `ENCODER_CAP_CERTIFIED` / exit 0 | **上界** ≤ 16129/16384；元数据、原输入/函数/依赖与计次来源审查通过；四模型真实通过。可把本次 p 接入八步条件证书，仍不宣布 E80 PASS。 |
| `ENCODER_CAP_REFUTED` / exit 2 | **下界** > 16129/16384。只否定所选 K-CAP，不推出实际绕回、错误舍入或生产 bug；返回原材料和不满足的确切 margin。 |
| `INCONCLUSIVE_INTERVAL` / exit 3 | 区间跨阈值，判断仍为未知，不自动提高精度或重复执行，不宣称反例。 |
| 拒绝 / exit 4 或 C++失败 | JSON/schema/来源/维度/数值 guard 等不满足，不能使用该结果实例化任何历史链；保留完整失败。 |

候选本轮只完成 patch 内存重放、源函数体不变、接口解析负例与**标量**区间检查；这不是上述真实 RED/GREEN。函数 `canonical_bounds`、四个 transform TDD 与 C++ 均未执行。

## 停止线

完成一次上述有界动作并返回其原始公开输出与来源回执后停止。不得在该动作内转入 S116、annulus125、噪声补偿、密钥选择、刷新或完整论文 Table3 复现实验。原 S100 E80 FAIL 保持，完整论文目标继续进行。
