# 补充外部固定引用与读取边界

本任务主证据来自用户ZIP。本次没有泛泛公共搜索，没有检索新实验来代替包内证据，没有连接GitHub执行或写入任何任务。补读仅用于核对作者关于固定OpenFHE Gaussian inversion分支的窄事实。

## P0：论文PDF

```text
https://eprint.iacr.org/2023/1788.pdf
```

尝试用web打开并截图所需页面；截图路径受到访问限制，未得到可用远端截图。随后使用**包内PDF**的本地页图复核p5、p7、p8、p13（另渲染部分邻页但不把渲染等同于目视阅读），结合完整包内TXT与源码。没有使用OCR。论文结论依据本包版本，不依据另一份未核对的线上PDF。

包内PDF：759,375 B；SHA-256 `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`。论文打印/物理页定位见 `REVIEW.md`。

## S1：固定Gaussian实现

```text
https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/discretegaussiangenerator-impl.h
```

冻结FIRST_PASS之后读取。源文件SetStd/Initialize/FindInVector/GenerateVector附近（编辑器行约57–121）显示小标准差走inversion；表尾界由ceil(sigma*12.00610553538285)确定。对于所给3.19F，成功返回的系数位于[-39,39]。

## S2：固定Gaussian接口阈值

```text
https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/discretegaussiangenerator.h
```

冻结FIRST_PASS之后读取，源文件约75行的阈值为300，支持S1的小标准差分支判断。**没有宣称所有Gaussian算法都有限支持**，也没有执行PRNG采样。

外部两页是web返回的固定版本源码文本，未在本交付保存原始下载字节或计算它们的SHA；不能把URL中的commit当作本地重建或二进制来源认证。

## 本地独立推导与适用范围

配合包内官方PKE表达式、dense ternary v及||s||1=128，fresh聚合项的粗系数界为39(N+1+h)=1,282,983。在额外知道实际编码整数||m||∞≤2^164的条件下，根Q的整数不绕模不等式成立。`checks/post_author_scalar.py` 只执行这项条件性整数计算，结果见 `results/post_author_scalar.json`。

实际m的额外界属于所给历史fresh诊断的源绑定断言；本次annulus没有提供这一m证书。因此补充阅读没有让annulus获得fresh整数提升证明，更没有获得全电路或部署安全证明。它也没有推翻冻结的首轮结论。
