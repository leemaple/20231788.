# 初始相位 exact contract 的执行计划

## 固定身份和权限

输入 ZIP：`testing-methods-33722b9.zip`，2630139 bytes，486 个普通文件；SHA-256 `572cf0db6765dfbb1c8ee658d2425fc8e27933a540adf7b1d1145310e50de809`。

运行源码基线：`33722b9c9ba9d32b36e672ee7cdaa3c3fb2a95d1`；目标分支：`codex/testing-methods-diagnosis-20260911`。OpenFHE **1.5.0**：`df495ba2e91739a6dc8f1de254fc5a41155ce504`，native64/backend4/HAVE_INT128/MAX_MODULUS_SIZE=60。论文附件 SHA-256 `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`。

只增加 `tests/initial_phase_exact_contract_test.cpp` 和 CMake 末尾 opt-in 目标。生产文件、现有测试、历史记录、工作流、原 S100 输入/参数/门槛完全不变。依赖使用 root 已审阅的固定安装；本包不给出编译 OpenFHE、安装新库或 dispatch CI 的命令。本地只做标量/静态检查，C++ 由 root 在已有 Windows/Linux 环境审阅后运行。

## 性质和推导：先定义什么才算错

令 `R=Z[X]/(X^256+1)`。使用现有 N256 h128 夹具的三枚 Q 素数及其冻结根；它们已经在原测试中作为期望常数，而非从被测 context 读出再用作期望值。根端运行时必须逐项核对 Q/P/QP、分区、σ、模式。安全级别为 HEStd_NotSet，**绝不是可部署的安全参数集**。[S03]

在固定 STANDARD、SPARSE_TERNARY secret、noiseScale=1 的官方实现中：

```text
pk = (a*s+e_pk, -a)
c0 = pk0*v + e0 + p
c1 = pk1*v + e1
c0+c1*s = p + f (mod Q)
f = e_pk*v + e0 + s*e1
```

这里的加减次序从 adapter 和固定官方源码导出；论文使用等价的另一种 a 符号约定。不能看到论文的 `(-a*s+e,a)` 就把当前公钥次序当 bug。[S02,S04,S15]

固定 DGG 的 σ 是 binary32 的 3.19f，传入 double 后精确值为 3.190000057220458984375。它小于 Karney 门限 300，走初始化有限表的成功 Peikert 路径；`fin=ceil(σ*12.00610553538285)=39`。FindInVector 成功返回绝对值不超过 fin 的整数；找不到时是异常，不是一个更大的成功样本。DCRT 的 DGG/TUG 构造器把**同一整数向量**转换到各 q 塔，而 public PKE 的 v 是系数属于 {-1,0,1} 的 dense ternary。不是 h128 的 v。[S04,S06]

因此逐系数必有：

```text
||e_pk||∞ <= G = 39
||s||1 = h = 128
||v||1 <= N = 256
||f||∞ <= G*(N+h+1) = F = 15015
```

这是整数系数无穷范数的**必要支持界**，不是典范嵌入范数、典型误差大小、尾概率或分布检验，不设任何经验容差。不把 15015 与 T=2^-80 直接比较。

公开明文有六个非零系数：

```text
p[0]   =  2^100 + 2^30 + 1
p[1]   = -(2^100 - 2^29 - 3)
p[2]   =  2^65 + 7
p[127] = -(2^80 + 5)
p[128] =  2^54 + 9
p[255] = -11
```

它不具有任意对称性，覆盖低位、超过 double/native64 的整数位宽、正负和跨中点/负循环边界；其余系数为零。`Q=1361129441679586514673376111479624265217`，且对每个 i 都有 `2*(|p_i|+F)<Q`，所以真实 p+f 的中心提升不绕回。**这不是改变原 S100 明文，而是一个另有名字的公开 API 合同夹具。**

对每塔独立计算：

```text
f[t][i] = Center_qt( phase[t][i] - p[i] )
```

必须先跨塔逐系数相同，再检查绝对值≤F。因为 q_min>2F，若性质成立，f 是唯一的这个范围内的整数提升。公开 Poly* 解密返回的规范剩余类必须恰等于 `(p_i+f_i) mod Q`。所以断言的是**公开解密和独立相位相同**，不是错误的 `Decrypt(Encrypt(p)) == p`。[S05]

## 独立 oracle 的范围

固定 native FTT 的布局由其蝶形和 bitreverse 根表决定：[S07]

```text
EV[k] = a(psi^(2*rev8(k)+1))
a_i = N^(-1) * sum_k EV[k] * (psi^(2*rev8(k)+1))^(-i) mod q
```

`psi^N=-1`、`psi^(2N)=1`，且 q 为固定素数，N 在模 q 下可逆。令 `x_k=psi*omega^rev8(k)`，omega=psi²，则对 0≤i,j<N，求和 `sum_k x_k^(j-i)` 在 i=j 时为 N，否则为 0。这给出逆式，不依赖浮点、舍入或近似 roundtrip。

测试实现的逆式是 O(N²) 的 `cpp_int` 直接累加，用扩展 Euclid 算逆元，没有官方变换、官方 CRT 或官方解密。随后用整数 schoolbook 负循环卷积求 `c0+c1*s`。不能用同一个被测 pointwise multiplication 来充当期望结果。

在引入秘密前，公开宽整数 p 还经过实际官方正 NTT 与独立 Horner 的全部 EV 坐标比较，并由独立逆式恢复全部公开系数。这样把“oracle 的点序/符号推错”尽量前置为可用公开夹具定位的问题。它不是密码安全证明，也不是对所有变换尺寸的认证。

测试故意不要求 e 或 f 非零，不要求正负平均，不要求精确 CKKS 加密往返、不要求舍入可加、不要求近似乘法结合律。非空性来自非平凡公开输入、真实随机 key/ciphertext、完整系数检查以及必被拒绝的错误输出负对照。[W1–W3]

## 最小化与同噪声条件

只用一个 context、一个 h128 keypair、一个 payload encryption。没有 eval key，没有 Mult2、平方或中间刷新。N256 复用已有 adapter 夹具；N64 不可能有 128 个非零 secret 系数，而引入 N128 新 profile 并没有现成覆盖收益。

全部基线检查与两个负对照在 `--controls` 的同一进程内共享原始 key、p 和实际密文；两个 mutant 不消耗新随机数。只需三塔才能保持现有夹具，六个公开非零明文系数足够触发表示边界。负对照最小化到单个系数的确定偏移，不收集秘密失败向量，也不进行随机 shrinking 或反复取种子。N256不是声称全局最小反例。

若公开 anchor 失败，先只用公开 p、固定 q/root 和这个已缩小夹具核查 oracle/环境；若只有 secret/phase 失败，保留固定标签和运行元数据，停止。不得通过输出秘密数组来再缩小。

## 应用补丁

工作目录必须是源码仓库根，里面直接有 `src/`、`include/`、`tests/` 和 `CMakeLists.txt`。不要在整个供件 ZIP 根目录直接 apply。以下只读 base 检查和本地 patch 不会触发 GitHub。

Linux shell（PACKAGE 为本交付包解压目录的绝对路径）：

```bash
BASE=33722b9c9ba9d32b36e672ee7cdaa3c3fb2a95d1
test "$(git rev-parse HEAD)" = "$BASE" || exit 2
test -z "$(git status --porcelain)" || exit 2
git apply --check "$PACKAGE/initial-phase.patch" || exit 2
git apply "$PACKAGE/initial-phase.patch" || exit 2
```

若 root 已把此补丁纳入新测试提交，记录新提交与该 base 的完整 diff，不用修改断言来假装 HEAD 仍是 base。不能应用到不同生产源码后继续使用本结论。

备用：完整新增 C++ 文件在 `tests/`，CMake 追加内容在 `CMakeLists.append.txt`；优先 apply patch，不同时手工复制又 apply。补丁只改两处，已实际做 apply-check，并验证生成文件与完整文件字节相同。

## 最小构建/运行

前提：root 已有固定 pin 的可信 OpenFHE 安装以及项目原有 Boost 头。`OpenFHE_DIR` 指向该安装中 OpenFHEConfig.cmake 所在目录；不要为了让 find_package 通过而换成系统最新库。记录实际加载的库路径和已有构建来源；打印的 `dependency_pin_declared` 只是声明，**不是运行库的密码学证明**。使用工作流已有的 CMake/编译器，不在浏览器容器执行这些命令。

Linux：

```bash
: "${OpenFHE_DIR:?Use the already-verified pinned OpenFHE install}"
cmake -S . -B build-initial-phase -DCMAKE_BUILD_TYPE=Release \
  -DOpenFHE_DIR="$OpenFHE_DIR" -DOPENFHE1788_INITIAL_PHASE_DIAGNOSTIC=ON || exit 2
cmake --build build-initial-phase --config Release \
  --target initial_phase_exact_contract_test --parallel 2 || exit 2
ulimit -c 0
export OMP_NUM_THREADS=2
cmake -E chdir build-initial-phase ctest -C Release -N -R '^initial_phase_exact_contract$'
# 上一行必须恰好列出 1 个测试；否则停止，不得把 0 tests 当 PASS。
cmake -E chdir build-initial-phase ctest -C Release -V -R '^initial_phase_exact_contract$'
```

Windows PowerShell（沿用 root 的已有 MinGW/OpenFHE 安装，勿把二进制混用到另一 ABI 编译器）：

```powershell
$Base = '33722b9c9ba9d32b36e672ee7cdaa3c3fb2a95d1'
if ((git rev-parse HEAD).Trim() -ne $Base) { throw 'wrong base' }
if (git status --porcelain) { throw 'worktree not clean' }
git apply --check "$Package/initial-phase.patch"
if ($LASTEXITCODE -ne 0) { throw 'patch check failed' }
git apply "$Package/initial-phase.patch"
if ($LASTEXITCODE -ne 0) { throw 'patch failed' }
if (-not $env:OpenFHE_DIR) { throw 'verified OpenFHE_DIR is required' }
cmake -S . -B build-initial-phase -DCMAKE_BUILD_TYPE=Release `
  "-DOpenFHE_DIR=$env:OpenFHE_DIR" -DOPENFHE1788_INITIAL_PHASE_DIAGNOSTIC=ON
if ($LASTEXITCODE -ne 0) { throw 'configure failed' }
cmake --build build-initial-phase --config Release --target initial_phase_exact_contract_test --parallel 2
if ($LASTEXITCODE -ne 0) { throw 'build failed' }
$env:OMP_NUM_THREADS = '2'
cmake -E chdir build-initial-phase ctest -C Release -N -R '^initial_phase_exact_contract$'
# 必须恰有 1 test，然后才运行。
cmake -E chdir build-initial-phase ctest -C Release -V -R '^initial_phase_exact_contract$'
if ($LASTEXITCODE -ne 0) { throw 'diagnostic did not pass; preserve fixed labels and stop' }
```

未指定新 generator 是为了沿用 runner 的既有配置；fresh build 目录必须选择与已安装库一致的原工具链。若环境缺失该 pin 或其构建来源无法确定，记录缺失并停止，不安装/替换依赖来追绿。CLI 参数、编译输出和运行结果均需 root 记录；本包未观察到这些步骤。

CTest 注册只由 `OPENFHE1788_INITIAL_PHASE_DIAGNOSTIC=ON` 打开，目标 EXCLUDE_FROM_ALL，focused 正则恰好一项。关闭选项时原 registry 不变；开启后也不要运行不带 -R 的整个 suite。180 秒是测试执行保护限，不是性能要求；超时不算捕获数值故障。

## 接收结果与停止条件

| 观察 | 允许结论 | 后续动作 |
|---|---|---|
| 诚实基线 PASS；两 mutant 用指定原因拒绝；原基线仍相同 | 这个样本、这个夹具的表示/支持/解密合同成立，断言有检测两类故障的能力 | 结束本切片；不能说原 S100 修好、分布正确或所有 N 都正确 |
| 公开 anchor / profile /导入先失败 | 实际环境、夹具、oracle 或公开实现前提有不一致 | 停止；先独立复核公开反例，不调噪声或参数 |
| profile 与 anchors 通过，但诚实 pk/f/公开解密失败 | 存在此夹具下的实现/前提偏差候选，不能用 CKKS 近似容差直接豁免 | 保留标签、平台、base/diff/依赖身份；独立核查后才讨论生产修复 |
| 诚实基线 PASS，mutant 未被拒绝或错因拒绝 | 测试弱、mutation 写错或 oracle/状态隔离有错 | 停止并修订测试；不是原算法 bug 证明 |
| 编译/链接/普通异常/超时/0 tests | 运行未完成或不受支持 | 不计数字故障、不计 PASS，不做样本重试 campaign |

既有原 S100 数值 FAIL 属于不同门禁，不能变成 expected-failure。新 test 的 exact equality 属于模环/整数运算，不适用于近似 slots。即使两平台各一次都绿，也只结束这个 contract slice；不存在“继续试种子直到 green”的分支。

## 秘密与观察边界

root 在客户端诊断进程中持有 s、e_pk 的推导值和 f；不生成 v/e0/e1 的捕获，也不序列化任何 secret、ciphertext、phase、随机种子或 tag。日志仅固定公共标签、N/塔数/常数和源版本声明，没有逐槽误差输出。第三方异常只打印 UNCLASSIFIED_FAILURE，不打印 what()。

应使用不收集进程转储、core、内存快照的 runner；Windows 预先确认 WER/CI 未开启 dump 上传，Linux 已给 ulimit -c 0。源码没有承诺安全清零堆内存、禁用系统 swap 或防御恶意宿主。这些是 runner 的访问控制前提，不能用“没有 ofstream”冒充完整秘密保护证明。不会把 secret 传入 evaluator，本 slice 没有 evaluator，也不接入原 acceptance chain。
