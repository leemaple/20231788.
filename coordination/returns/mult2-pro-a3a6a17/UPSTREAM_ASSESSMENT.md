# 上游算子与公共契约评估

## 结论

本次最小 Mult2 组合**没有发现必须先修改 DCP、Tensor2、Relin2 或 RS2 算法体的上游阻断项**。生产实现应保持：

```cpp
CiphertextPair DoubleCKKS::Mult2(const CiphertextPair& left,
                                 const CiphertextPair& right) const {
    return RS2(Relin2(Tensor2(left, right)));
}
```

现有公开算子自然提供所需的验证顺序和 fail-fast 行为：Tensor2 首先验证 pair/context/lifecycle/兼容性，Relin2 再访问并严格检查 HYBRID/BV 评估密钥，RS2 再执行两次独立公开 Rescale。Mult2 不捕获异常、不对齐输入、不隐藏 rescale、不修改调用者对象。

## 明确不纳入本补丁的事项

### 1. RS2 mixed-tower-format 加固

任务说明该工作由另一个分支独立推进。本交付没有修改 RS2 或其验证体，也不把另一分支的潜在修复预先合并进来。若后续确认它是上游必要修复，应作为独立前置/后置补丁处理并重新跑完整 49 项测试。

### 2. 定理 4.8 显示式勘误

这是论文陈述问题，不是通过改生产代码去“适配”错误公式的问题。详见 `THEOREM_SCALE_DECISION.md`。生产组合遵循定义 4.7 和三个现有算子的实际语义。

### 3. 透明逻辑尺度解码

当前 RCB 保留记录 scaling factor，而 FIXEDMANUAL decoder 依据 `p` 与 `noiseScaleDeg` 解码，因此调用者需按

```text
q_div*q_l / 2^(2p)
```

校正普通解码结果。测试中的校正只用于诊断和核验。

若产品 API 要求 `RCB -> Decrypt` 直接返回逻辑乘积，需要另开公共契约切片，例如提供显式 `DecodeLogicalPairProduct`/scale descriptor 消费接口。不能只篡改 ciphertext 的 recorded SF 并宣称已修复，因为供应的 decoder 在该路径不采用任意 SF。

### 4. 通用 `E_Relin` 上界

当前没有从附件材料导出并实现保守的、对所有夹具成立的 `E_Relin`。本测试只暴露单次经验测量并把通用门标成 `UNPROVED`。补充通用证明或参数生成器属于独立研究/证明任务，不应塞入首次 Mult2 KISS 切片。

## 集成风险

本候选未在真实 OpenFHE 安装上编译和运行。最强剩余工程风险是：新增独立 oracle 对 OpenFHE 1.5.0 具体 API/类型细节或随机执行边界存在编译/运行问题；必须由固定提交的 Linux/MinGW CI 真实裁决，不能由静态审查替代。
