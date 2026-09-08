# 有界检查的范围与复算

需要 Python 3.10 或更新版本的标准库。没有第三方依赖；代码不导入 OpenFHE、numpy、FFT/NTT、随机数库，不进行真实 FHE 加解密。`Ring.phase` 只是合成整数多项式的代数内积，并非在真实密钥上执行解密。

## 三个入口

在返回包解压目录运行。将下面占位路径替换为实际原附件 ZIP／解包目录。结果建议写到返回包目录之外，避免修改已登记的交付物 manifest。

```text
python checks/verify_input_archive.py <原ZIP路径> --output ../relin2-recheck/INPUT_VERIFICATION.json
python checks/model_checks.py --output ../relin2-recheck/model_results.json
python checks/public_bounds.py --input-root <原ZIP解包目录> --output ../relin2-recheck/public_bounds.json --table ../relin2-recheck/public_bounds.tsv
python checks/verify_delivery.py .
```

`public_bounds.py` 的 `--input-root` 可以省略，此时仍可独立重算公开常量与全部不等式，但 `source_constants_match` 变为 null，不能称为本次源码 hash 的重新核验。数学结果与数值表不受影响。带正确原解包目录运行时，生成 JSON/TSV 与随包结果逐字节一致。

## 模型覆盖

|ID|所检验关系|失败变体／限制|
|---|---|---|
|M01|单分区且 κ=0 的双坐标 carry 反例|第一坐标为零、解密 carry 仅 ≤h 都过强|
|M02|修正的双坐标关系正例|只有附加 κ=0 条件才是两个单位 carry|
|M03|遗漏坐标0 carry、把 `−s*r1` 改成 `+s*r1`|两个故意错误的变体都被拒绝；不是生产 RED|
|M04|两个分区且存在 digit carry|遗漏 G 得到错误坐标；完整式给出 `(8,−8)`|
|M05|解密差 C10 的整数等式|坐标大不代表解密误差大|
|M06|六个固定的 N=4 非常数负循环多项式 fixture|检验环乘、单次误差界和近可加式；不是参数分布抽样|
|M07|额外 d digit=0、同 key 的限制恒等式|不涉及跨 family key equality|
|M08|Relin2 的 DCP 重组相消、两次误差直接相加|不是 `d*nu(H2)`|
|M09|RS2 重组、C22b 的两个 phase wrap 项|不能遗漏末次中心化项|
|M10|整数 Tensor 恒等式、Fraction 归一化|故意遗漏 d 的尺度变体被拒绝；旧归一化疑点不是本轮新发现|
|M11|充分 nonwrap 条件失败与真实 wrap 的逻辑区分|既有真实 wrap 例，也有界失败但实际不 wrap 的例|
|M12|多 prime 的 unreduced CRT 内部 carry|明确指出本单 prime 模型不能代表任意 HYBRID|
|M13|当前 S100 公开参数、N32768/h128 的符号见证|256 个确定性系数类别覆盖整条多项式，decoded carry=129；不是 256 次实验，也不是真实 key|

合成模型的 A/B/e/s 都是公开、刻意选择的代数常数或多项式。M01–M12 使用小环；M13 使用真实公开模数，但仅以闭式系数分类做整数运算。虽然模型按 key 方程构造行，但没有调用 cryptographic key generation，也没有任何真实 seed、秘密或噪声系数。模型未实现 NTT、并行运行、NTT cache、内存布局、C++ ABI 或生产上下文对象；这些不在模型 PASS 的证明范围内。

## 公开数值检查

`public_bounds.py` 从本源码 hash 下的 S100 Q/root/P 对照固定常量。它用整数模幂验证 12 组 `(modulus,root)` 的 `root^N=-1` 和 `root^(2N)=1`，并检验两两互素。这不是 NTT，也不是重新证明素数或 RLWE 安全性。

八个 family 都验证 `B2<Q/2` 和局部 `d*N*B2/S²<T/2^40`。同时明确检查粗 RS 界大于 T、初始 DCP 低部乘积粗界大于 T；这些结果的含义是“相应充分精度证明不够紧”，不是发现真实误差达到了上界。

`model_stdout.txt`、`public_bounds_stdout.txt`、`input_verification_stdout.txt` 是本轮实际运行输出。全部数学检查使用整数与 Fraction；科学计数显示仅通过整数除法截断，未参与断言。
