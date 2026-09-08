# 最小区分测试与执行约束

基线：`3c02988fb5655dc6ea48f4f7d559e62d9d4a9d37`。以下“已执行”只指本包的轻量标量、文件或补丁检查；没有执行项目测试程序、FFT或加密。

## 1. 测试矩阵

| ID | 检验命题／反例 | 预期 | 环境与预算 | 状态与失败处理 |
|---|---|---|---|---|
| S-01 | Thm4.8打印1/q与定义的1/(dq) | literal-paper模式exit1；归一化目标满足精确界 | Python标准库Fraction，一例，无随机数 | **已执行**；是纸面规格RED，不是旧C++失败 |
| S-02 | Tensor负交叉项；Relin两坐标舍入进位；分量范数与复数模 | 三个精确反例成立 | 整数/Fraction，常数预算 | **已执行**；不据此更改正确生产符号或宣称整个Lemma4.4被推翻 |
| S-03 | DCP模恒等式、相关RS2重组 | 1105个DCP、7225个RS坐标全部满足独立式 | 小整数穷举，不导入项目 | **已执行**；非OpenFHE全环证明 |
| S-04 | 新旧输入是否单位圆内；新输入域及非平凡输出 | 两族各16384个唯一dyadic；新半径<125/128，所有|x^256|>2^-10 | 纯整数检查，无FFT | **已执行**；不以“原输入越界”为失败解释 |
| S-05 | 条件数预算、有限sampler fresh提升、实际prime尺度 | K+1/4<0.855；fresh条件性非绕模；8尺度closed-product与递推一致 | Decimal向上舍入／整数／Fraction | **已执行**；sampler前提来自固定官方源码，不是实际概率认证 |
| R-01 | 原S100两个TSV的逐槽E8=I8+A8 | 保持exit8和FAIL；两精度重放一致 | 150、210位Decimal，各两平台原数据，零新样本 | **已执行**；只重放留存数值，不重新证明observer |
| R-02 | fresh-only的A+B理想传播 | 原20分量中3个条件性区间超过T | 240位向外舍入，±1e-100盒 | **已执行**；不是新的实际E8 |
| P-01 | 新TSV接收器不会把缺行/错误范数/错误身份/非零exit当PASS | 合成正例1个；负例7个全部拒绝 | Python标准库，全为内存合成零误差记录，无密钥 | **已执行**；不能引用为密码学PASS |
| I-01 | patch绑定与非目标文件不变 | apply --check、实际应用都exit0，CMake旧字节前缀不变 | 输入project的临时副本 | **已执行**；不是编译证明 |
| C-01 | 新C++目标能在官方pin与工具链编译 | 新target编译成功，无warning-as-error失败 | 授权Linux/Windows runner，各构建一次；不在Mac/本容器编译 | **未执行**；编译失败先静态修复，不宣称精度FAIL |
| N-01 | 新域是否同时满足fresh预算、后续预算及原始输入E80 | 全槽复数E0≤T、A8≤T/4、双路径E8≤T，所有身份/控制通过 | 1根setup+1public payload+8Mult2；每平台1条，最多2条 | **未执行**；不因失败换seed、key、半径、槽集合 |

S-01…S-05由一个15项检查脚本实现；不要把坐标穷举数当作加密次数。实际穷举数量为1105+7225，仅标量。P-01是解析格式控制，不是现场运行的抽样数据。

## 2. 轻量检查复现命令

将输入ZIP解压到独立目录，返回包解压到另一目录。下面以环境变量表达真实路径；脚本不修改输入源目录。

```bash
export INPUT_ZIP=/absolute/path/comprehensive-reassessment-3c02988.zip
export INPUT_ROOT=/absolute/path/extracted-input
export RETURN_ROOT=/absolute/path/extracted-return

python -B "$RETURN_ROOT/checks/scalar_reassessment.py" --literal-paper-red
# 预期 exit 1：这是刻意保留的纸面反例。
python -B "$RETURN_ROOT/checks/scalar_reassessment.py"
python -B "$RETURN_ROOT/checks/replay_retained_endpoints.py" "$INPUT_ROOT" --digits 150
python -B "$RETURN_ROOT/checks/replay_retained_endpoints.py" "$INPUT_ROOT" --digits 210
python -B "$RETURN_ROOT/checks/replay_fresh_anchors.py" "$INPUT_ROOT"
python -B "$RETURN_ROOT/checks/replay_annulus125.py" --self-test
python -B "$RETURN_ROOT/checks/verify_packet_and_patch.py" \
  "$INPUT_ZIP" "$INPUT_ROOT" "$RETURN_ROOT"
```

接收器使用不超过10000位的整数字符串转换上限，以容纳第8步约7707位的精确尺度分子/分母；不是关闭所有输入大小限制。脚本中的数值计算与证明范围见各文件docstring。

## 3. 契约补丁采用前检查

`CONTRACT.patch`的两个变化是：新增独立测试源；在原CMake末尾追加默认OFF选项 `OPENFHE_2023_1788_ENABLE_S100_ANNULUS125`。完整文件同时提供，二者应二选一采用，不能重复叠加。

本包没有名为生产 `GREEN.patch` 的文件，因为不存在已经证实的生产错误供修复。若owner把新增测试目标的“基线未存在”当构建RED，只能称“新增接口/测试注册的RED”，不能称“原S100精度RED”；真正保留的S100精度RED是旧CI原始失败。

审查重点：独立Input公式确为999而非1015；使用pf::Scales固定的S100具体primes；evaluator函数参数无secret、oracle、error或callback；单次payload；任何decrypt在8次计算完成之后；全槽真值来自原始x^256；A8以独立observer的fresh相位传播，不加入客户端readout C；所有数值fail仍保留完整行；最终flush/close失败直接暴露。

`ReadOnlyCiphertext`与receipt是公开状态/归一化边界，不是密码学身份认证。上游context、param handle不得由测试或调用方修改；已有factory与live validation继续生效。新文件未引入这些API语义的变更。

## 4. 现有workflow不能照抄dispatch

当前 `.github/workflows/dcp-rcb.yml`：

- `s100_scope`默认fresh（25–34）。修复ref上的controls-only只影响该ref下fresh诊断步骤；Linux默认60与controls不运行原八平方精度。
- Linux旧full-target构建（216–218）和旧端点运行（269–271）使用“ref不在若干名单”的条件。一个新ref不在排除名单时，**即使传controls-only，也可能运行旧完整链**。
- Windows job（321–322）只排除修复/red ref；新ref会启用Windows分支，其旧完整链也有相同风险。
- 默认60套件显式排除 `paper_full_eight_square_contract` 和 `experimental_precision116_profile_seam`（174–177、566–574）。不能无筛选ctest。新target也为EXCLUDE_FROM_ALL。
- push触发采用枚举分支列表，不是所有新branch都会自动触发；但workflow_dispatch可对新ref生效，不能依赖“不在push列表”抵消dispatch风险。

因此，本包不提供任何 `gh workflow run` 命令，也没有修改workflow。owner必须安排独立、显式选择N-01的执行路径，或先单独审查一份只注册新切片的workflow。现有repair ref controls-only的已知安全范围，不自动扩展到新分支。

## 5. 授权runner上的准确构建与单次运行

### 5.1 前置身份

只能在GitHub Actions或专用Windows等已授权环境，不在本地Mac重编译。必须有完整的官方OpenFHE `df495ba2e91739a6dc8f1de254fc5a41155ce504` 与递归子模块；确认无tracked/untracked补丁，并保留实际build cache、编译器和Boost版本。输入的77份摘取参考不是可直接构建的完整安装。

要求native64/backend4；原CI配置还包括WITH_OPENMP=ON。沿用记录过的Linux GCC/Boost与Windows MINGW64环境；不能把MSVC与MINGW64库混链接。当前返回的C++在任一工具链均尚未编译，因此不能声称“警告已测干净”。

owner审查采用后的源码会有**新的确切commit**。`S100_ANNULUS125_SOURCE_COMMIT`由现有CMake的Git HEAD取得，接收时要比对新commit，不能还写旧3c02988以掩盖加入了测试。新build目录必须不存在；每平台独占目录，避免输出覆盖或并发竞态。

### 5.2 只运行新目标的Bash/MINGW64命令草案

```bash
set -euo pipefail
: "${EXPECTED_REVIEWED_COMMIT:?set reviewed integration commit}"
: "${TASK_OPENFHE_PREFIX:?set verified pristine installation}"
: "${BUILD_ONCE:?set a new exclusive build directory}"

test "$(git rev-parse HEAD)" = "$EXPECTED_REVIEWED_COMMIT"
test -z "$(git status --porcelain --untracked-files=all)"
test ! -e "$BUILD_ONCE"
export PATH="$TASK_OPENFHE_PREFIX/bin:$TASK_OPENFHE_PREFIX/lib:$PATH"
export OMP_NUM_THREADS=2

cmake -S . -B "$BUILD_ONCE" -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$TASK_OPENFHE_PREFIX" \
  -DOPENFHE_2023_1788_ENABLE_S100_ANNULUS125=ON
cmake --build "$BUILD_ONCE" \
  --target s100_annulus125_eight_square_test --parallel 2
ctest --test-dir "$BUILD_ONCE" --show-only=json-v1 \
  -R '^s100_annulus125_eight_square_v1$' > "$BUILD_ONCE/selection.json"
python - "$BUILD_ONCE/selection.json" <<'PY'
import json, sys
j=json.load(open(sys.argv[1],encoding='utf-8'))
names=[x['name'] for x in j['tests']]
assert names == ['s100_annulus125_eight_square_v1'], names
PY

# 本切片只采用下面一次受控binary入口，保存真实子进程exit。
# 不再执行上面注册的CTest，否则会产生第二条样本。
python - "$BUILD_ONCE" "$EXPECTED_REVIEWED_COMMIT" <<'PYRUN'
import json, os, pathlib, re, subprocess, sys
os.umask(0o077)
b=pathlib.Path(sys.argv[1]).resolve(); commit=sys.argv[2]
assert re.fullmatch(r"[0-9a-f]{40}",commit)
exe=b/("s100_annulus125_eight_square_test.exe" if os.name=="nt"
       else "s100_annulus125_eight_square_test")
output=b/f"s100-annulus125-{commit}.tsv"
assert exe.is_file() and not output.exists()
start=b/"program-start.json"; end=b/"program-end.json"
assert not start.exists() and not end.exists()
argv=[str(exe),"--output",str(output)]
with start.open("x",encoding="utf-8") as f:
    json.dump({"argv":argv,"source_commit":commit,"timeout_seconds":1200},f)
    f.flush()
env=dict(os.environ); env["OMP_NUM_THREADS"]="2"
with (b/"program-once.log").open("xb") as log:
    timed_out=False
    try:
        result=subprocess.run(argv,stdout=log,stderr=subprocess.STDOUT,
                              timeout=1200,env=env,check=False)
        code=result.returncode
    except subprocess.TimeoutExpired:
        timed_out=True; code=124
with end.open("x",encoding="utf-8") as f:
    json.dump({"program_exit":code,"timed_out":timed_out,
               "entry":"DIRECT_BINARY_ONCE_NOT_CTEST"},f)
    f.flush()
sys.exit(0 if code==0 else 1)
PYRUN
```

这是**一个且只有一个数值运行入口**，不执行CTest，也不伪造本次CTest PASS。CTest在这里仅用于构建后show-only预检，确认新增注册恰好一个。若owner另行决定用CTest实际执行，应重新核对子进程exit保存方案并替换此入口，不能两个都跑。

超时显式记124并留下部分日志；它不是E80数值FAIL，更不是PASS。进程负返回码、异常或缺结束记录都视为运行无效并保全。启动记录一旦存在，本轮不自动再次启动。返回包装器自身0/1不等于binary原始exit，接收器使用 `program-end.json` 的 `program_exit`。

新TSV文件位于 `BUILD_ONCE/s100-annulus125-<integration-commit>.tsv`。MINGW64下Python必须是已核验的native Python；CMake路径及DLL PATH沿用原CI的cygpath转换要求，不能将MSYS Python误称已验证的Windows工具链。输入返回包不包含该future TSV。

### 5.3 接收与重放


把original TSV、stdout/stderr、进程exit、CTest/直接入口身份、源码/依赖commit、cache、toolchain、运行ID/attempt和文件hash一起归档。接收器不证明元数据就是实际运行来源；provenance必须另行核对。

```bash
python -B "$RETURN_ROOT/checks/replay_annulus125.py" \
  --tsv "$TSV" --source-commit "$EXPECTED_REVIEWED_COMMIT" \
  --process-exit "$PROGRAM_EXIT" --precision 180 > replay-180.json
python -B "$RETURN_ROOT/checks/replay_annulus125.py" \
  --tsv "$TSV" --source-commit "$EXPECTED_REVIEWED_COMMIT" \
  --process-exit "$PROGRAM_EXIT" --precision 230 > replay-230.json
```

这两次是对**同一文件**的标量重放，不是两条加密链。numeric FAIL接收器返回1并给出完整FAIL对象；格式/来源或边界不确定直接异常，不归为E80 PASS。

## 6. 预算、预测和停止标准

构建仅新target及其库依赖；每平台至多一次进程、一个根setup、一次public payload、8次平方、终点两条全槽观察及10点Horner。各数值进程上限1200秒，OMP_NUM_THREADS=2。论文179ms与历史20/85/118秒不作为新测试时长承诺；新observer与控制会增加时间。完整官方依赖首次构建属于授权runner准备，不在本机执行。

预测是：若新抽样仍有与历史样本相近的fresh噪声量级、后续残差未显著增大，半径预算将使原始输入E80有余量；这不是已实现的结果。统计结论仅限一条链/平台，不估总体通过率。

失败分类：编译错误→候选集成失败；控制或observer异常→不可用观测；E0>T→fresh预算失败；A8>T/4→后续预算失败；双预算通过而E8失败→恒等式/尺度/记录异常优先；timeout/flush/close失败→运行/证据不完整。任何失败不删除、不换key、不换输入半径、不补跑到通过。独立审查后再决定是否授权另一个有界任务。
