# S100-FRESH-ERROR-REPAIR-01 — 共享编码接口与 fresh-only 三项诊断

## 0. 交付状态与边界

本包是**待根代理远端编译、执行和独立复核的 RED/GREEN 源码候选**，不是一次已经通过的数值实验，也不是原始 S100 精度修复完成声明。未发现妨碍这次最小提取的接口结构性问题；这不等于证明编码器或密码学实现没有缺陷。

本会话执行了附件身份/CRC/逐文件哈希核验、源码阅读、补丁生成、补丁回放及静态保护检查。**没有执行 CMake 配置、C++ 编译、C++ 控制测试、加密、解密、FFT、Horner 数值实验、既有回归或 API 构建。** 本地未建立或使用所锁定的 OpenFHE 构建环境；附件中的官方参考文件不是完整已安装依赖。没有触发远端任务，也没有伪造 RED/GREEN 构建日志。`STATIC_CHECKS.json` 只记录实际静态检查。

当前 TASK.md 和用户本轮补充是本次权限来源。旧科学处置、旧测试注释和旧任务仅作历史/源码背景。原始 S100 的既有远端完整链 FAIL 保留；独立 S116 配置的合格 PASS 不被引用为 S100 修复证据。本次不重跑任何八次平方链。

输入身份：

| 项目 | 身份 |
|---|---|
| 输入 ZIP | `s100-fresh-error-repair-e6c4cc1.zip`，1,514,765 bytes |
| ZIP SHA-256 | `58bdc872577c3e267dd2f1fe823bf1751242abf5520e211cceed2834be6e14aa` |
| 基线源码 | `e6c4cc1ddfa261ee17681cbfc1ca6023660df5cb` |
| TASK 来源提交 | `2e7ce350c0253249340e9fee144038aed0cd93e9` |
| 输入 MANIFEST SHA-256 | `28c37db169b61efe1afe5504c7a2694fcd7e2e640e6904a2322f3ffc966c3d8c` |
| TASK SHA-256 | `efd62bf0c2948dc1488f5e31506f532efc96a5802a96cc8c41e05a5e1c3c8462` |
| 官方 OpenFHE pin | `df495ba2e91739a6dc8f1de254fc5a41155ce504` |
| 成员核验 | 181 个唯一普通成员；清单自排除，180 个载荷；展开 4,394,649 bytes |

输入清单记录的两阶段扫描是**输入方已有证据**，不是本会话重新执行的扫描。本轮没有生成任何实际密钥、密文或噪声向量。

## 1. 恰好四个仓库文件改变

| 阶段 | 仓库路径 | 改动 |
|---|---|---|
| RED | `CMakeLists.txt` | 末尾追加一个默认 OFF 的选项、一个 EXCLUDE_FROM_ALL 目标、两个显式测试 |
| RED | `tests/s100_fresh_error_diagnostic_test.cpp` | 唯一新测试源文件；`--controls` 与 `--fresh` 两个互斥模式 |
| GREEN | `include/openfhe_2023_1788/high_precision_client_io.h` | 一个 owned 结果类型和一个 const 方法 |
| GREEN | `src/high_precision_client_io.cpp` | 提取唯一共享编码函数；InspectEncoding 和真实 Encrypt 都调用它 |

`01-red.patch` 加测试和构建接线，不声明、不模拟新 API。测试通过 `auto encoded = client.InspectEncoding(...)` 调用成员，既不预先声明新结果类型，也不使用检测成员存在性的回退。**预期 RED 原因是基线没有 InspectEncoding 成员；这尚未由实际编译观察到。** 缺少依赖、无效工具链、其他语法错误不能计为合格 RED。

`02-green.patch` 只改变客户端头文件和实现，不修改已经提交的测试或 CMake。它应在 RED 后应用；`modified/` 存放二者合并后的四个完整文件。既有测试/观察器/参数工厂/工作流逐字节不改。静态回放用的是附件创建的合成 Git 快照；该合成 HEAD **不是**工程基线提交。原始文件字节及 Git blob 与输入清单核对后，才用合成仓库验证补丁。

## 2. 最小共享接口及保留的不变量

```cpp
struct EncodingInspection final {
    std::vector<ExactInteger> signedCoefficients;
    OrderedDcrtBasis basis;
    PositiveRationalScale logicalScale;
    std::uint32_t slots;
    std::uint32_t strideGap;
    CanonicalProjection projection;
};

EncodingInspection InspectEncoding(
    const std::vector<ClientComplex>& values,
    const FreshEncodingSpec& spec) const;
```

结果没有密钥、密文、上下文指针、回调、引用视图或用于注入密文的句柄。整数向量、基的有序模数/根字符串以及精确比例尺均按值拥有。客户端仍只支持原来允许的上下文/plan 几何与尺度，接口没有扩展输入域。

实现提取私有 `ComputeEncoding(...) -> EncodingWork`。工作对象包含精确有符号系数和官方 BigVector 残数。它执行原 Encrypt 中同一段代码：输入槽数、精确尺度、有限性检查；同一套 160/220 位十进制逆变换；同一 StableRound；同一系数范围及 wrap 检查；同一 BigInteger 十进制往返核验。InspectEncoding 复制诊断元数据并移动系数，丢弃局部残数；Encrypt 使用同次 helper 产生的残数继续原官方转换/公开加密。

选择连官方精确残数转换也共享，是为了避免 InspectEncoding 接受一个实际 Encrypt 会因整数转换而拒绝的编码结果。该转换没有调用 PKE 或 RNG。InspectEncoding 在有效上下文下不需要公钥或私钥；全尺寸 plan 的既有只读身份核验不等于执行同态运算。

真实 Encrypt 的顺序保持：

```text
CheckProfile
→ 公钥/context/tag/arity/有序基校验
→ ComputeEncoding（槽数 → 尺度 → 非有限值 → 逆变换 → 舍入 → wrap/转换）
→ 官方 Poly.SetValues / DCRTPoly 构造
→ 同一个 GetScheme()->Encrypt(element, publicKey)
→ 同一 FreshState / 元数据 / CheckCiphertext / BoundCiphertext
```

没有为了复用而从 Encrypt 调用公共 InspectEncoding，因此没有把公钥校验移到编码之后。`StableRound`、`PaperRound`、`Transform`、`Inverse`、`Forward`、`Decrypt` 均未改变。纸面 ties-down 规则和公共边界的 ambiguous-half 拒绝仍是原语义；新测试不将精确半整数改为可接受输入。舍入/转换段的逐字节等价检查只允许把原 `impl_->primary/check` 改为 helper 形参名称。

未改变采样器、标准差、秘密 h128、临时量分布、公开加密模式、输入、Q/P、尺度递推、密钥格式、旧门槛或安全声明。没有 raw-encrypt/raw-decrypt 新接口，也没有零密钥或固定密文模拟。

## 3. 单次真实公开加密的三项定义

设固定输入为 `z = paper_full_test::Inputs()`；`values = ClientInputs(z)`。测试逐个实/虚分量检查十进制桥接后的值与原 dyadic z **相等**，不新增输入近似项或改变固定输入。

- `m`：InspectEncoding 返回的整数系数；与真实 Encrypt 共享唯一计算函数、同一输入及 spec。真实加密后再做一次确定性 inspection 并检查结果相同，不做第二次加密。
- `p`：对那一次真实公开加密的同一个 BoundCiphertext 的 owned clone，用既有独立 sparse-CRT 解密得到的居中整数多项式。
- `d = p - m`：在整数环中直接相减，绝不对 d 取模或重新居中。
- `O`：独立规范嵌入观察器，按精确 `Delta = 2^100` 归一化。
- `y`：这同一 BoundCiphertext 的实际 `client.Decrypt(...)` 返回值，经不经过 double 的高精度十进制桥接进入 binary512。

对每个 `s = 0..16383` 和 `c ∈ {real, imag}`，分别计算：

\[
A_{s,c}=O(m)_{s,c}-z_{s,c},\qquad
B_{s,c}=O(d)_{s,c},\qquad
C_{s,c}=y_{s,c}-O(p)_{s,c},\qquad
E_{0,s,c}=y_{s,c}-z_{s,c}.
\]

逐分量检查

\[
\left|(A+B+C)-E_0\right|\le 2^{-300},
\qquad
\left|O(p)-O(m)-O(d)\right|\le 2^{-300}.
\]

**B 是直接对 raw d 做观察，不是 y−O(m)，也不是两个 FFT 输出相减。** 第二个关系单独检查观察器线性一致性；第一个关系不能作为独立密码学正确性的证明。

B 的命名包括真实公钥生成噪声传播和本次公开加密的新增噪声；不将其宣称为单独 e0 或 e1。C 是指定的“productionDecrypt 对独立 p 的差”，因此它也包含生产整数解密路径与独立解密路径不一致时的影响，不能仅凭 C 大就断言生产 FFT 有 bug。

四个最大值 A/B/C/E0 各自保存 `(magnitude, slot, component)`。平局固定取最小 slot，实部优先。每个极值位置均输出同位置的 `z,A,B,C,E0,reconstruction_residual`；另外输出十个既有 anchor 的两个分量。绝不对不同位置的范数做加减来代替逐分量归因。输出不是完整槽位/系数侧录：只有四个极值元组、20 个 anchor 元组和数值摘要。

### 3.1 不借重新居中制造“小噪声”

对每个系数，先确认 m、p 是同一正奇数 Q 下的居中代表，再计算 raw d，并要求

\[
2|d_j|<Q,\qquad
Q-2\bigl(|m_j|+|d_j|\bigr)>0,\qquad
m_j+d_j=p_j.
\]

第二个条件是**充分而非必要**的有符号 lift 余量检查。不能满足它时输出 INVALID，而不是断言已实际发生 wrap。只有对 p 的独立 CRT 重构允许既有居中步骤，d 的整个构造过程没有 `Center`、`Mod` 或 `%`。

反例控制采用 Q=101、m=49、p=−49：raw d=−98 必须拒绝；不得把它变成 +3。反向例子也测试。另有合法 raw 差、不同模数、未居中输入和充分余量不足的拒绝控制。

**可观测性的限制必须保留：**仅有居中 m/p，无法排除某个未观察的采样噪声代表恰为 d+kQ。这里认证的是实际使用的整数 lift 与它的充分余量，不是关于所有隐藏采样值绝不绕回的概率定理。程序明确输出 `hidden_sampler_wrap_claim=NONE`，不偷偷调整样本来消除这个不确定性。

## 4. 独立观察器、数值容差与条件舍入线索

### 4.1 三条观察路径

全槽路径复用**未改动**的 `paper_endpoint_transform.cpp` 和 `paper_endpoint_scaled_norm.cpp`。它是 N 点正号 twisted DFT，输出索引由 `(5^s mod 2N − 1)/2` 选择；不是生产 N/2 点 packed inverse/forward 的第二份拷贝。它独立产生 binary512 和 binary768 根/表。

对 m、p、d 三个多项式分别执行 `Observe`，检查每个槽位/分量 512 对 768 的一致性；对每个多项式另外调用 `paper_full_test::Horner` 和 `AnchorRoots`，检查既有 anchors：

```text
0, 1, 256, 257, 512, 513, 768, 769, 1023, 16383
```

Horner 是直接多项式求值，不调用生产编码/解码或观察器 FFT。p 来自既有 `ReadSecret` + `SparseDecrypt`：实际 signed h128 在内存中检查，逐塔稀疏负循环卷积和精确 CRT 不调用生产 Decrypt/DecryptCore。共享底层为官方 native tower 逆 NTT；这不是完全独立的 NTT 实现。

三个数值路径仍共同依赖 Boost 多精度及其 pi/sin/cos。双精度层级一致性和十个 Horner 点提供有力交叉检查，但不是全槽超越函数正确舍入的形式证明。没有声称本次建立了无条件规范嵌入定理。

### 4.2 2^-300 是诊断一致性门槛，不是精度目标

固定绝对容差 `tau = 2^-300` 仅用于观察器之间、Horner、线性关系和三项重构的一致性。它没有替换或放宽任何原 E80 门槛。A、B、C、E0 没有新增大小验收门槛；例如 B 很大但观察有效，仍可 COMPLETE。

测试先检查每个多项式的精确 `sum(abs(c_j))/Delta <= 2^64`，及参与分解的有限值范围；超出该数值观察包络即 INVALID/MODEL_ENVELOPE。这里 Delta 恰为 2^100，所以已接受系数最多为约 165 个整数位，binary512 可以精确承载它们，除以该二次幂不会引入一般有理尺度问题。

选择 300 位而非接近 512 位的比较门槛，是为 N=32768 的累积求值、根近似和十进制输出桥接保留很宽裕的数值余量。条件于普通浮点/根误差模型，甚至用保守的 N² 累积放大量估计，2^64 的一范数包络仍把 binary512 算术误差推到大约 2^-418 的量级；额外常数及转换余量远小于 2^-300。**这只是容差设计说明，不是对 Boost transcendental error 的已证明上界。**实际交叉不一致必须失败，不能靠扩大容差或改样本恢复 COMPLETE。

生产 ClientReal 使用 170 位十进制字符串桥接，并检查在 ClientReal 中往返相等，随后进入 binary512；不先落入 binary64。原 `FromClient` 的 100 位字符串桥接另作全槽对照，记录并要求其与本次桥接的差不超过 tau。这个门禁也是诊断一致性检查，不能据此宣布生产编码正确。数据最终以 170 位科学计数法输出，根代理可保留原日志，而不是只抄短小的四个范数。

生产 Decrypt 的原 `maximumCrossPrecisionDisagreement <= 2^-120`、实际居中余量和状态校验继续使用；没有把 C 限制到 2^-120 来预先假定解码无误。

### 4.3 2^-86 的正确用法

若真实输入对应的精确逆规范嵌入为 f，且每个系数确实执行最近整数舍入，则 `|m_j−Delta*f_j| <= 1/2`。由于规范根的模长为 1，任一实/虚分量均有

\[
|O(m)_{s,c}-z_{s,c}|
\le \frac{1}{\Delta}\sum_{j=0}^{N-1}\frac12
=\frac{N}{2\Delta}=2^{-86}.
\]

代码检查该数值等式，并输出条件标签及 `established_by_this_run=NO`。它既不是无条件编码正确证明，也不是本次已发现 bug 的声明。A 若显著超过这个界，应检查前提——输入桥接、规范映射、逆变换、舍入、系数捕获或观察器——而不能把“只要用了高精度数值类型”当作前提已经成立。

## 5. 控制测试及退出语义

`--controls` 不生成任何密钥。它复用原 N64 / 16 slots / Delta=2^100 的公开上下文配置，在没有加密材料时调用 const inspection。控制覆盖：

| 类别 | 控制内容 |
|---|---|
| 可手算整数编码 | 0、±1、±3/8、2^-75 实常数：只有 coefficient[0]=Delta*c；−i：只有 coefficient[32]=−Delta |
| 舍入 | ±1/4、±3/4、±5/4、±7/4 个系数单位；正负精确半整数仍必须以原诊断拒绝 |
| 符号/排列/尺度 | 由独立直接 Horner 产生的 X² 单项式及混合稀疏多项式输入；检查所有 64 个精确系数及 stride 零位 |
| 反事实控制 | 调换两个槽位、共轭输入、整体放大两倍，必须不能再匹配原单项式系数 |
| 拒绝 | 错误 values/spec 槽数、错误整数/有理尺度、实部或虚部 ±Inf/NaN、有限但超支持范围；检查异常类型和原完整诊断字符串 |
| 验证顺序 | slots 在 scale 之前，scale 在 nonfinite 之前；公钥检查顺序通过原 Encrypt 逐字节恢复检查保留 |
| 所有权 | 输入/spec 改变不影响返回对象；返回系数、基、尺度与几何字段改动不影响另一份拷贝、客户端和后续结果 |
| 归因报告本身 | 四个 term/E0 极值安排在四个不同 slot/component，包含抵消；检查极值位置、平局和错误符号导致重构不一致 |
| raw lift | Q=101 的正/反绕回伪小差反例及合法/非法代表控制 |

不能谎称覆盖的边界：在当前公开输入域，StableRound 的支持范围限制会先于极大系数的 Q-wrap 分支触发。测试实际覆盖的是超支持范围拒绝；原编码器 wrap/官方整数转换检查靠原样保留及静态等价检查保护。toy raw-difference wrap 反例不是“生产编码器所有 wrap 路径已执行”的证据。

`--fresh` 在原 `CreatePaperRepeatedMult2Setup()` 上运行。该既有工厂会建立原 plan 所需的家族上下文及评估密钥；这属于原 setup，不是新加的同态链。新执行路径不调用 DCP、Mult2、RS2、RCB、Evaluate 或其他 S116 工厂。只有一处实际 `client.Encrypt(setup.publicKey, values, spec)`，无循环重试。工厂中原公钥构造用的私钥 `EncryptZeroCore` 不能误算为又加密了一次固定输入。

程序在任何上下文、密钥或全尺寸根表创建之前分派参数，并要求环境变量 `OMP_NUM_THREADS` 恰为字符串 `2`。这不是实际线程数的性能测量。macOS 编译此新测试目标被显式禁止。

- `COMPLETE / exit 0`：该模式所有控制/诊断一致性检查有效。fresh 的最终行明确记录 `original_S100_E80=NOT_RERUN prior_FAIL=RETAINED precision_claim=NONE`。
- `INVALID / exit 2`：非有限值、状态/基错误、raw 差或余量失败、观察包络超限、独立一致性失败等。上游异常只输出受控阶段与类别，不把可能含密钥数据的任意异常文本写入日志。
- 构建失败、超时、进程被杀或缺少最终 COMPLETE：没有有效完成记录；不得记成有限精度 FAIL，更不得记 PASS。

不会输出秘密、密文、原始 p/d 系数、全量 per-key 槽位噪声或浏览器/状态数据。`COMPLETE` 不表示新鲜误差足够小、不表示原八次平方链通过、不表示安全参数已合格。

## 6. 远端 RED/GREEN 命令提议（本会话未执行）

以下仅供根代理在**既有锁定的远端 Linux 工具链**中执行；Windows 使用同一 target、选项与测试名，由根代理继承已有工具链安排，不据此替换编译器或改 workflow。首次数值观察选择一个既有远端环境；不把另一台主机或自动重试当作寻找较好噪声样本的方法。

下列变量由根代理从实际环境提供：`DELIVERY_DIR` 为解包目录，`OPENFHE_DIR` 为已锁定安装的 CMake 配置目录，`OPENFHE_SOURCE_DIR` 为其已核准的源码 checkout，`RUN_ROOT` 为源码树外的新日志/构建目录。源码目录从**干净、精确基线**开始。保留原 OpenFHE/Boost/生成器编译选项，下面只展示本切片所需增量。

```bash
set -euo pipefail
: "${DELIVERY_DIR:?set to extracted delivery directory}"
: "${OPENFHE_DIR:?set to the approved pinned OpenFHE CMake directory}"
: "${OPENFHE_SOURCE_DIR:?set to the approved dependency checkout}"
: "${RUN_ROOT:?set to a new artifact/build directory outside the source tree}"
test "$(uname -s)" != Darwin
BASE=e6c4cc1ddfa261ee17681cbfc1ca6023660df5cb
PIN=df495ba2e91739a6dc8f1de254fc5a41155ce504
test "$(git rev-parse HEAD)" = "$BASE"
test -z "$(git status --porcelain=v1 --untracked-files=all)"
test "$(git -C "$OPENFHE_SOURCE_DIR" rev-parse HEAD)" = "$PIN"
mkdir -p "$RUN_ROOT"
export OMP_NUM_THREADS=2

# RED: add only calling tests and opt-in wiring, then record the real revision.
git apply --check "$DELIVERY_DIR/01-red.patch"
git apply "$DELIVERY_DIR/01-red.patch"
git diff --check
git add CMakeLists.txt tests/s100_fresh_error_diagnostic_test.cpp
git commit -m "test: freeze S100 shared-encoding and fresh attribution contract"
git rev-parse HEAD > "$RUN_ROOT/red-source.txt"
cmake -S . -B "$RUN_ROOT/red-build" -DCMAKE_BUILD_TYPE=Release \
  -DOpenFHE_DIR="$OPENFHE_DIR" \
  -DOPENFHE_2023_1788_ENABLE_S100_FRESH_ERROR_DIAGNOSTIC=ON
# First establish that the unchanged production library/dependency build works.
cmake --build "$RUN_ROOT/red-build" --config Release \
  --target openfhe_2023_1788 --parallel 2
set +e
cmake --build "$RUN_ROOT/red-build" --config Release \
  --target s100_fresh_error_diagnostic_test --parallel 2 \
  > "$RUN_ROOT/red-build.log" 2>&1
RED_RC=$?
set -e
printf '%s\n' "$RED_RC" > "$RUN_ROOT/red-build.exit"
test "$RED_RC" -ne 0
grep -n 'InspectEncoding' "$RUN_ROOT/red-build.log"
```

这里的非零退出码和 grep **本身不足以确认合格 RED**。根代理必须核对真实主错误是新成员缺失，而不是本测试其他编译错误；不合格时停止，不把错误原因换名。

```bash
# GREEN: same tests; only the shared public interface/private implementation.
git apply --check "$DELIVERY_DIR/02-green.patch"
git apply "$DELIVERY_DIR/02-green.patch"
git diff --check
git add include/openfhe_2023_1788/high_precision_client_io.h \
        src/high_precision_client_io.cpp
git commit -m "feat: share exact client encoding with deterministic inspection"
git rev-parse HEAD > "$RUN_ROOT/green-source.txt"
cmake -S . -B "$RUN_ROOT/green-build" -DCMAKE_BUILD_TYPE=Release \
  -DOpenFHE_DIR="$OPENFHE_DIR" \
  -DOPENFHE_2023_1788_ENABLE_S100_FRESH_ERROR_DIAGNOSTIC=ON
cmake --build "$RUN_ROOT/green-build" --config Release \
  --target s100_fresh_error_diagnostic_test --parallel 2 \
  > "$RUN_ROOT/green-build.log" 2>&1

# Each anchored regex selects just one new CTest entry. No repeat/retry flags.
(cd "$RUN_ROOT/green-build" && \
 ctest -C Release -R '^s100_encoding_inspection_contract$' -V) \
 > "$RUN_ROOT/controls.log" 2>&1
(cd "$RUN_ROOT/green-build" && \
 ctest -C Release -R '^s100_fresh_error_diagnostic$' -V) \
 > "$RUN_ROOT/fresh.log" 2>&1
```

根代理记录每一步真实退出码、构建/控制/数值日志及其哈希、实际提交和依赖来源。运行必须检查确实各发现且执行了一个测试；0 tests 不是成功。源代码输出的 `source=` 来自配置时 HEAD，必须在各阶段实际提交后配置；不把未提交补丁误标为原提交。`expected_openfhe_source=` 只是期望 pin，不能替代已安装依赖的实际构建来源证据。

CMake 选项默认 OFF；开启后 controls 超时 120 秒、fresh 超时 1200 秒，两者 `RUN_SERIAL=TRUE` 且环境 `OMP_NUM_THREADS=2`。这里只提议一次 fresh；不运行全套 CTest 正则、不执行原 S100/S116 长链、不做 1000 次统计、不找 seed/key、不改变门槛，也不以一次耗时声称性能。既有回归及五项 API 构建按根代理原协议维护，本包不修改其名单或实现。

## 7. 阅读依据与来源边界

路径均相对输入 ZIP，生产源码定位指**基线文件**。完整输入 MANIFEST 提供其字节来源；本包 MANIFEST 另外列出直接依赖的测试观察器/源码哈希。

| 来源 | 对本次设计的作用 |
|---|---|
| `TASK.md` 及本轮用户补充 | 权限、四文件边界、三项定义、原始 FAIL 保留 |
| `references/paper/PAPER-2023-1788.pdf` | 完整论文；第 4 页 §2.1 规范嵌入/舍入/公开加密，第 13 页 §6.3/Table 3 另经本地页面图像核对 |
| `project/src/high_precision_client_io.cpp:331–452,553–616,684–733` | 现有变换/StableRound、真实编码和公开加密、生产解码 |
| `project/src/repeated_mult2.cpp:24–43,434–471` | 原 S100 与独立 S116 工厂分离，既有 family/key setup |
| `project/src/paper_h128_client_keypair.cpp:193–219` | 固定 h128 秘密与官方 full-Q 公钥构造 |
| `project/tests/paper_full_eight_square_oracle.h:93–115,196–257,279–315` | 冻结输入、独立 signed h128/稀疏 CRT、十个 Horner anchors |
| `project/tests/paper_endpoint_transform.cpp`、`paper_endpoint_scaled_norm.cpp` | 已有独立正号 N 点观察器和整数一范数尺度；复用且不修改 |
| `references/official-full/src/pke/lib/schemerns/rns-pke.cpp:56–69,111–196,199–223` | 实际公共 Encrypt、私钥/公钥 EncryptZeroCore、生产整数解密结构 |
| `references/official-full/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp:71–95` | 已固定的 Poly 解密/噪声模式行为 |
| `references/official-full/src/core/include/lattice/hal/default/poly.h:81–91`、`dcrtpoly-impl.h:59–68` | 保留的官方 Poly/DCRT 精确转换 |
| `references/boost-1.83.0/.../cpp_bin_float.hpp` 和 `.../detail/functions/trig.hpp` | 观察器精度/根运算及既有动态 limb 存储选择的边界 |
| `project/coordination/fs-endpoint-scientific-review-return-01/pro/DECISION.md` | 历史 S100 E0/E8 和条件归因；不执行其中已过时的下一步指令 |
| `project/coordination/precision116-eight-square-return-01/ROOT_RUN_RECEIPT.json` | 另一个配置的已有记录，只用于不混淆其身份 |

论文 §6.3/Table 3 使用 N=2^15、h=128、Delta=2^100 的 t=2 例子，并报告连续八次平方后 1000 次执行的平均 infinity-norm error；不是本测试的单次固定 stress-input / 实虚分量最大值合同。原文中的 chi_enc/chi_err 是分布符号；本包不能从该符号推断论文原型与本项目用了相同的具体公开加密采样细节或输入分布。

本项目源码可核对的公开加密贡献形式为 `e_pk*v + e0 + s*e1`。h128 限定根秘密 s，不会自动把官方公开加密的临时 v 变成 h128。这里只记录已证实的实现路径和未写明的论文条件；不据此修改任一分布、推出安全性或制造协议差异的确定结论。

## 8. 完整交付后的剩余不确定性及唯一后续分流

构建可行性和控制用例的实际行为仍需真实远端 RED/GREEN 证据，尤其是严格 warning-as-error、Boost 多精度转换和原上下文工厂的联合编译。静态检查不能替代它们。

A/B/C 哪一项在原始 S100 中主导，**目前未知**；本包没有生成新 E0 数据，不复制旧 E0 作为本次结果，也没有为旧公开加密找更好的样本。一次观察也不能证明跨密钥分布或失败概率。

若有效观察显示 A 明显超出条件舍入界，根代理以具体分量/系数关系开最小可证伪编码测试，再决定是否修生产算法。若 B 主导而 A/C 小，先核对论文输入、指标、公钥构造和分布条件，不改采样器。若 C 主导，则先区分生产整数解密与解码/桥接路径，不把它并进“加密噪声”。观察无效则先修观察的独立判据/实现问题，不能扩大门槛获得 COMPLETE。

上述分流均不授权本轮进入原完整链、替换参数或宣告 S100 修复。原 E80 的完整回归及任何新科学/安全结论属于根代理后续另行限定任务。
