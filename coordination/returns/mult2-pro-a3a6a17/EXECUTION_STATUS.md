# 执行状态与证据边界

## 1. 已执行

| 项目 | 结果 | 证据 |
|---|---|---|
| 输入 ZIP 大小/SHA-256 | PASS | `verification/input-integrity.txt` |
| ZIP 路径安全、重复名、symlink 检查 | PASS | 同上 |
| SOURCE-MANIFEST 30 个对象逐项大小/SHA-256 | PASS | 同上 |
| 8 个补丁从精确包内源码快照顺序重放 | PASS | `verification/patch-replay-git.log` |
| 重放文件树与最终项目逐字节比较 | PASS | `verification/patch-replay.txt` |
| 重放 Git tree 比较 | PASS；均为 `66bad80110c866a4616194a74eb3b6fa55b096af` | 同上 |
| 生产差异范围与组合式唯一性 | PASS | `verification/static-scope-audit.txt`、`verification/production-code-diff.txt` |
| 红/绿提交顺序静态检查 | PASS | `verification/red-green-chronology.txt` |
| workflow YAML 解析和关键步骤检查 | PASS | `verification/workflow-static-audit.txt` |
| 合成 OpenFHE 包元数据下的 CMake 配置图 | PASS；声明 49 个 CTest | `verification/cmake-graph-probe.log` |
| 定理分歧的整数反例 | PASS | `verification/theorem-counterexample.txt` |

“CMake 配置图 PASS”只说明 target/test 注册关系可生成；合成元数据没有 OpenFHE 头文件和库，不能提升为 C++ 编译证据。

## 2. 实际尝试但因环境失败

当前容器没有 `OpenFHEConfig.cmake`、OpenFHE 头文件或库。按任务命令执行 CMake 配置时在 `find_package(OpenFHE 1.5.0 CONFIG REQUIRED)` 处失败，记录在 `verification/configure-probe.log`。

随后仅在隔离 Linux 工作区尝试获取固定 OpenFHE 仓库；DNS 无法解析 `github.com`，未取得依赖，也没有开始 OpenFHE 编译。原始错误保存在 `verification/dependency-acquisition.log`。

这些是环境阻断，不是候选代码的失败或成功判定。

## 3. NOT EXECUTED / UNVERIFIED

以下候选工作均未在本环境执行：

- C++ 编译与链接；
- 默认 `cmake --build build --parallel 2`；
- `tensor2_api_contract_test`、`relin2_api_contract_test`、`rs2_api_contract_test`、`mult2_api_contract_test` 的显式构建；
- 49 项 `ctest --output-on-failure`；
- 10 项新 Mult2 测试的任何运行结果或数值证书；
- Linux GCC `-Wall -Wextra -Wpedantic -Werror` 候选门禁；
- Windows MSYS2 MinGW64 候选门禁；
- 候选 GitHub Actions run；
- 生产安全、性能、53/106-bit 精度或通用定理证明。

因此，没有生成“绿色候选日志”，也没有伪造 CI 输出。

## 4. 包内旧绿色日志的准确范围

`final-project/artifacts/tdd/rs2-valid-arithmetic/green.txt` 记录的是源码一致的旧实现提交 `ed00f3518d65223a482e1e9db54111eb24573f2c`：Linux 与 Windows 的告警构建、显式 Relin2/RS2 API 目标和 39 项测试成功。

该日志明确说明它不是 Mult2 测试、不是原始加密槽位乘法精度结果，也不是定理证明。它只能证明本次基线之前的范围，不能被引用为本候选的运行证据。

## 5. 集成方应产生的最小运行证据

集成方应在固定 OpenFHE 提交上执行 README 中的完整命令，并保留：

```text
1. OpenFHE 实际 commit 和干净状态；
2. 项目实际 commit/tree 和干净状态；
3. CMake configure 完整日志；
4. 默认 warning-clean build；
5. 四个显式 API target build；
6. 49 项 CTest 完整输出；
7. 四个 e2e 用例打印的参数、Q_l bits、M_high/M_low、h、经验 E_Relin、non-wrap、系数误差、经验 bound、ratio 和槽位误差；
8. Linux GCC 与 Windows MinGW64 两端结果。
```

只有这些真实执行完成后，才能把相应状态从 `NOT EXECUTED` 更新为 PASS/FAIL。
