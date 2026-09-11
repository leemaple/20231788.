# Initial-phase one-shot runner 独立审核

## 范围、对象与结论

本审核只覆盖当前工作树中的：

- `.github/workflows/initial-phase-exact-once.yml`，SHA-256
  `7d72e826db39eb33f690ceef34ee6857d22ae4054be5d385f92f7acd26639b1f`；
- `coordination/testing-methods-20260911/runner_guard.py`，SHA-256
  `a16d94fc00649635c23db8bc59e7bf0a224cc588ce92f046478d0756a744e47e`；
- `coordination/testing-methods-20260911/test_runner_guard.py`，SHA-256
  `450a3e95c59b2be1fd1181576f097101402defccfe90ca59c0c57865ae2f7cf0`。

未构建、未加密、未采样、未派发 CI、未浏览、未执行 Git 变更，也未重做
C++/CMake 数学与实现审核。唯一执行是：

```text
python3 -B -I coordination/testing-methods-20260911/test_runner_guard.py
...
Ran 3 tests in 0.002s
OK
```

**结论：测试选择、单进程单样本、官方依赖归属、动态库解析和 core-dump
控制的主链条设计是 fail-closed 的；但按“工作流自身必须证明全局只运行一次，
并且上传集合必须由完整 manifest 严格封闭”的字面标准，当前仍有两个派发前
阻断项。** 若仓库外能证明该 tag 受保护且永不删除/重建，并把已审核 tag commit
与所有可执行输入/上传文件清单作为外部不可变证据，则第一个问题可降为治理
条件；第二个仍建议在 workflow 内补齐。

## 阻断 1：同名 tag 删除后重建可形成第二个 `run_attempt == 1`

workflow 只监听精确 tag 名的 push（YAML 3-6），job 条件同时要求
`created=true`、`deleted=false`、`forced=false`、`run_attempt==1`（25-29）。
guard 又独立重复校验相同条件（`runner_guard.py:17-20`），其单测覆盖错误事件、
错误 ref 和 attempt 2（`test_runner_guard.py:11-20`）。因此：

- 普通 branch push、tag 更新、tag 删除、workflow rerun/attempt 2 均不会执行 job；
- 同一 run 的重试被 job 条件和 guard 双重拒绝；
- `concurrency.cancel-in-progress=false`（YAML 11-13）只保证同 ref 的并发 run 排队，
  不提供历史去重。

现有事件载荷没有历史状态。若先删除
`initial-phase-exact-once-20260911`，再用同名 tag 新建，新的 push 仍是
`created=true, forced=false, run_attempt=1`，workflow 与 guard 无法把它和第一次
区分。这意味着代码能证明“每个新建事件只接受第一次 attempt”，不能单独证明
“该 tag 在仓库生命周期最多执行一次”。

派发前二选一：

1. 提供并保留 tag 保护/不可删除不可重建的仓库侧证据，把它写入运行完成条件；
2. 使用外部持久状态查询并拒绝已有同名 workflow/tag 的历史 run，同时固定允许的
   tag commit。仅靠当前 `run_attempt` 和 concurrency 不能闭合全局一次性。

## 阻断 2：可执行输入与上传集合没有完整、强制的 manifest

### 可执行输入

初始 gate 确认 `SOURCE_BASE=33722b9...` 是 HEAD 祖先、`src/` 和 `include/`
相对 base 未变、`tests/` 只有目标测试变化，并硬校验测试文件 SHA-256
（YAML 51-57）。这是有效的生产代码和测试对象边界。

但 `source-sha256.txt` 只记录测试、`CMakeLists.txt`、`runner_guard.py` 和 workflow
（61-62）；它遗漏了在任何 build 之前实际执行的 `test_runner_guard.py`（49），
也没有把这些散列与 workflow 内或外部签署的预期 manifest 比较。与此同时，
base 到 tag 的其他路径没有 allowlist，尤其根 `CMakeLists.txt`、workflow 和 guard
自身可以变化；“把当前值写入证据”只能事后辨认，不能由本次 gate 自动拒绝未审核
版本。

最低修复是把 `test_runner_guard.py` 纳入 source hash，并在派发前把精确
`GITHUB_SHA` 或一份外部审核的 `{path, sha256}` manifest 作为不可变允许值校验。
若以固定 tag commit 作为唯一根信任，应明确记录并人工/签名验证该 commit，而不是
只检查它继承自 `33722b9...`。

### 上传集合

上传 action 递归上传整个 `$EVIDENCE_DIR`（YAML 154-161）。当前正常路径中所有
显式写入都属于公开构建/来源证据；测试二进制、项目 build 目录、OpenFHE build/
install 目录和潜在 working-directory core 文件均不在上传路径内。这一点是好的。

不过 workflow 在上传前没有：

- 固定允许的文件名集合；
- 拒绝额外文件、目录、符号链接或非常规文件；
- 生成覆盖全部实际上传文件的最终 `{sha256, size, relative path}` manifest。

所以“只上传白名单目录”并不等于“只上传白名单文件”；任何意外写入
`EVIDENCE_DIR` 的内容都会被递归上传。若要求绝对保证不上传随机性、密钥或内存
转储，应在 upload 前执行最终 allowlist/类型检查，拒绝 `core*`/dump 类文件，生成
全量 artifact manifest，并让 upload 只消费经检查的 staging 目录或逐项 path。

## 唯一 CTest 与 S100 隔离：通过静态审核

workflow 仅显式构建项目 target `initial_phase_exact_contract_test`（YAML
101-110）。这会构建其必需的项目库依赖，但不会请求其他测试 target。配置完成后，
先用锚定正则的 `ctest --show-only=json-v1` 生成选择结果（128-133）；selection guard
要求 JSON 中恰好一个 test，名称必须是 `initial_phase_exact_contract`，命令必须精确
为审核的可执行文件加唯一参数 `--controls`（`runner_guard.py:23-27`）。对应单测
拒绝 0/2 个 test、错误名称、`--baseline` 和错误可执行文件
（`test_runner_guard.py:22-30`）。

真正执行使用同一锚定正则和 `--no-tests=error`（YAML 148-149），没有 fault-mode
CLI、重采样、S100 target 或全测试套件命令。目标测试内部的 baseline 与两项 control
在同一进程、同一 ciphertext 上执行；runner 没有第二次加密调用。就当前固定测试
散列和 CMake 注册而言，不会误跑原始 S100。

限制：根 CMake 是可执行构建输入，因此上述结论依赖阻断 2 所述的 tag commit/
manifest 信任；单凭显式 target 名不能抵抗一个未审核 CMake 给该 target 增加额外
依赖。

## CTest PASS 认证：组合链成立，guard 不是单独证书

执行块以 `set -euo pipefail` 开始，CTest 通过 pipe 写入 `tee`（YAML 139-151）。
因此 CTest 非零退出、timeout、crash 或未找到测试都会在运行 result guard 前令 step
失败。只有 CTest 返回 0 后，result guard 才验证四条程序输出恰好按顺序、恰好一次
出现，并拒绝三个显式失败前缀（`runner_guard.py:30-45`）。单测覆盖缺行、重复、
错误分类、错误 commit 和 unclassified failure（`test_runner_guard.py:32-41`）。

这条“shell `pipefail` + CTest exit 0 + classified output + `result=PASS` 文件存在”
的组合链足以避免假绿，属于非阻断项。需要精确限定：`runner_guard.result()` 本身不
解析 `100% tests passed` 或 `1/1 Test ... Passed`；它的单测甚至直接给四条程序行
即可通过（`test_runner_guard.py:32-35`）。因此脱离本 workflow 的 shell 控制流，
`result-gate.txt` 不能单独证明 CTest PASS。若希望下载后的 artifact 自包含地认证
CTest，建议再解析唯一 test 的 PASS/总数摘要，或保存并校验 CTest JUnit/XML，且把
CTest 原始退出状态写入 manifest。

另一个非阻断限制是 `result()` 容许四条目标行之外的任意日志；当前固定 C++ 已独立
审核为只输出公开固定标签，官方异常文本也不由通过路径输出，所以目前没有发现
密钥/随机性日志源。但 guard 自身不是通用的日志泄漏过滤器。

## core dump 与秘密材料：当前控制链通过

最新 workflow 在唯一测试进程启动前：

1. 把 `kernel.core_pattern` 强制设为普通文件名 `core` 并回读断言，排除可能绕过
   RLIMIT 的管道式 handler（YAML 141-144）；
2. 设置并回读 `ulimit -c 0`（145-146），子进程 CTest/测试继承该限制；
3. 只把公开的两项控制值写入 `core-dump-controls.txt`（147）。

任何 sysctl 权限或回读不一致都会在测试前 fail closed。runner 是一次性 Ubuntu VM，
无需在后续共享作业中恢复全局 core pattern。即使假设出现工作目录下的 `core`，
它也位于 `$PROJECT_BUILD` 而不是 `$EVIDENCE_DIR`；当前 upload path 不会包含它。
结合固定测试源不序列化/打印 key、secret、ciphertext 或 residual，本链条没有发现会
把随机性、密钥对象或内存转储上传的正常路径。上传目录缺乏最终文件 allowlist 的
剩余风险已单列为阻断 2，不能由 core 控制替代。

## 官方依赖、配置与动态库归属：可核验，仍有运行态待证

依赖 checkout action 固定到 action commit，并把 OpenFHE checkout 固定到
`df495ba2e91739a6dc8f1de254fc5a41155ce504`，递归取 submodule、禁用 credential
持久化（YAML 64-71）。随后校验 OpenFHE HEAD、源码 cleanliness，保存递归 submodule
状态，并逐 submodule 校验 cleanliness（73-83）；构建后再次校验顶层源码树未变
（85-99）。未使用 actions/cache，build/install/project 目录必须预先不存在，且
`provenance.txt` 明记 `cache_used=false`。

官方固定源中的 CMake 选项名称与 workflow 相符：`BUILD_SHARED`、`BUILD_STATIC`、
`GIT_SUBMOD_AUTO`、`WITH_NATIVEOPT`、`WITH_NOISE_DEBUG`、
`WITH_REDUCED_NOISE`、`NATIVE_SIZE` 和 `MATHBACKEND` 均为该 pin 的实际配置项。
workflow 明确关闭 reduced-noise/noise-debug/native-opt，选 native64/backend4，禁止
官方 unit/example/benchmark/extras，只构建 shared library（YAML 89-97）；两份
CMakeCache 和工具链版本被保留（82,99,110）。这使配置可审计，但只有实际 run 的
cache 才能把最终生效值从“请求”提升为“观察”。

项目可执行文件先散列，再由 `ldd` 记录实际动态解析（111-125）。内嵌 Python 要求
至少解析到 `libOPENFHEcore.*` 和 `libOPENFHEpke.*`，并对每一个已解析的
`libOPENFHE*` 库做 `resolve(strict=True)`、要求真实路径位于本次新建 install prefix
之下，再记录库文件 SHA-256。路径逃逸、系统 OpenFHE 混链或缺少 core/pke 都会失败。
CTest 在同一 job、未改变 loader 环境后运行，因此这条归属链是充分的静态设计。

小的非阻断加固项：脚本未显式拒绝 `ldd` 中任意 `=> not found`，也没有记录
`readelf` 的 RPATH/RUNPATH；这类缺库通常会让随后的 CTest 失败，不能形成假 PASS，
但显式拒绝可令失败更早、证据更清晰。Ubuntu/toolchain 和 `apt` 包版本不是固定镜像
摘要，只是在 `toolchain.txt` 中记录，因此可追溯但不保证逐位可复现。

## 派发前最小关闭条件

1. 关闭“tag 删除重建”边界：工作流内做历史去重，或保存不可删除/不可重建的 tag
   保护证据并把该外部条件写入验收。
2. 补齐执行输入 manifest，至少加入 `test_runner_guard.py`，并用固定 tag commit 或
   外部审核 manifest 校验当前文件，而不只是记录当前散列。
3. 在 upload 前对白名单 evidence 文件做最终 allowlist、常规文件/无 symlink 检查和
   全量 hash/size manifest；不要递归上传一个仅凭约定保持干净的目录。
4. 实际 run 后只在同时存在：唯一 selection 证据、CTest 零退出链、精确四行分类、
   `result=PASS`、core 控制、官方 cache/commit/submodule、`ldd` 归属和最终 artifact
   manifest 时接受 green。实际 GitHub 平台语义、sysctl 权限、编译、运行时长和测试
   PASS 在本静态审核中仍为 pending。

## 关闭条件复核（当前工作树，后续追加）

本节只复核上文两个阻断项的最小修改，不重新评价 C++、CMake 或其他 runner
设计。复核对象的当前 SHA-256 为：

- `.github/workflows/initial-phase-exact-once.yml`：
  `e89a44bc0540db6efbbd63bf86231d0fa8bcab60eae416b0699eeef6e47b86be`；
- `runner_guard.py`：
  `905364fe584b3b12e47789f7c2809b79078deec8b1a66e8451336fef9f35d74a`；
- `test_runner_guard.py`：
  `b83f7c46354a059eb873d4fa9b16e4091e0858bcf93b0e1f341b97a4e9bb023a`。

我在该快照上重新执行纯标准库 guard tests，观察到：

```text
......
----------------------------------------------------------------------
Ran 6 tests in 0.424s

OK
```

### 阻断 1：在声明的保留历史边界内关闭

workflow 已增加最小 `actions: read` 权限，且 `GH_TOKEN` 只暴露给历史查询 step
（YAML 8-10、67-76），不会进入 build 或测试进程。查询固定 `event=push`、精确 tag
branch、`per_page=100`，并使用 `--paginate --slurp` 收集全部返回页。`history()` 再按
当前 workflow path、tag `head_branch` 和 push event 过滤，要求只剩一条记录，并要求
其 `id`、`head_sha`、`run_attempt=1` 与当前 run 完全相等
（`runner_guard.py:38-46`）。0 条、2 条、错误 SHA、attempt 2 和错误 run id 均有拒绝
单测（`test_runner_guard.py:47-56`）。

这会拒绝“删除 tag 后重建”产生的第二条**仍被 GitHub 保留**的同 workflow/tag run，
也保留原先 job/event 对普通 rerun 的拒绝。因此上文阻断 1 在以下明确边界内关闭：

- 外部主控把精确 tag 指向完整审核后的 commit X，并通过 GitHub 读回确认；
- GitHub API 返回完整保留历史；查询失败、权限失败、当前 run 尚不可见或字段不符时
  均 fail closed；
- 不声称抵抗管理员删除全部旧 run 记录、平台历史保留之外的记录消失，或管理员绕过
  仓库治理。该限制已经显式接受，不再作为本次派发阻断。

没有发现可令一个**可见的**旧同-path/tag push run 被现有过滤器漏掉、同时让当前 run
通过的具体反例。平台实跑仍需确认 `gh --paginate --slurp`、API 可见性和 token 权限；
这些失败只会阻止执行，不会形成假绿。

### 阻断 2：在外部 commit 根信任条件下关闭

`source-sha256.txt` 现在包含先于 build 执行的 `test_runner_guard.py`，连同固定测试、
根 CMake、主 guard 和 workflow 一起记录（YAML 62-65）。对于 workflow 不能安全
内嵌自身最终 commit SHA 的问题，所述外部流程——先完整审核并提交执行 commit X，
在后续文档 commit Y 固定 X 与全部执行输入 hashes，最后只把精确 tag 指向 X 并从
GitHub 读回——提供了非循环根信任。条件是 Y 中记录的三项当前散列及测试/CMake
散列必须与 X 和运行产物 `source-sha256.txt` 逐项一致。受此条件约束，上文“只记录
当前值而不校验预期值”的阻断关闭；workflow 本身仍不应被描述为自认证。

上传边界也已闭合。`EVIDENCE` 明列 19 个允许文件
（`runner_guard.py:12-17`）；seal 要求根不是 symlink 且为目录、拒绝集合外名称，至少
要求两项 provenance，成功执行时要求 19 项集合完全相等，并逐项只接受直接子层的
普通文件、非 symlink、单文件不超过 32 MiB，再记录实际 bytes 和 SHA-256
（49-64）。`MANIFEST.json` 用 exclusive-create 在检查后生成，并明确自排除
（100-108）。最新 CLI 正向测试实际创建全部 19 项，验证 manifest 一一覆盖、来源
SHA、success outcome 和 self-exclusion，并验证第二次 seal 必须失败
（`test_runner_guard.py:82-98`）；原测试继续拒绝额外 `core.1`、symlink、目录和成功
状态下的不完整集合（58-80）。

workflow 仅在 seal step 成功后上传同一 evidence 目录（YAML 169-185）。正常成功路径
上传的是经检查的 19 项加新建 manifest；失败路径可以上传被 allowlist 限定的公开
子集加 manifest，且 manifest 明记非 success outcome，不能伪装成完整成功证据。
没有任何 build、test、OpenFHE install 或 CTest working directory 位于该上传目录。

在没有并发后台写入者这一当前 workflow 前提下，未发现 seal 与 upload 之间可加入
未审查文件的正常路径。理论上的文件系统 TOCTOU 或拥有 runner 管理员权限的对手不在
此诊断的信任模型内。故上文阻断 2 在外部 X/Y 根信任与当前 19-file seal 条件下关闭。

### 关闭后的剩余验收边界

此前两个阻断现在均可标记为 **closed, conditional**，不再阻止一次性派发。仍不得在
实际 run 前声称平台语义、build 或 CTest PASS 已被观察。green 只在以下证据同时成立
时接受：外部确认 tag→X；历史 gate PASS；成功路径 19 项完整；manifest 与上传内容
匹配；唯一 selection/`--controls`；CTest 零退出和精确分类；core 控制；官方 pin、
cache、submodule 与动态库归属。若历史查询或 seal 任一步失败，最多保留带 failure/
skipped outcome 的公开部分证据，不能将该 run 解释为成功诊断。
