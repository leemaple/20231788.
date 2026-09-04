# SOURCE_FACT_AUDIT

本文件严格区分 **INSPECTED（已检查）**、**INFERRED（由已检查事实推导）**、**EXECUTED（实际执行）** 与 **UNVERIFIED（未验证）**。路径均相对于本交付根目录；论文和官方参考行号指向输入 ZIP 中的同名对象，任务与清单的原样副本位于 `source-records/`。

## A. INSPECTED — 输入身份与边界

1. `source-records/TASK.md:1-41` 明确要求定义 4.7 的首次 Mult2、精确公开签名、红/绿补丁顺序、真实加密端到端 oracle、HYBRID/BV、REAL/COMPLEX、定理条件、双尺度说明和 `NOT EXECUTED` 边界。
2. `source-records/SOURCE-MANIFEST.json:1-7` 声明：源提交 `a3a6a171b60c4829f674f0dc4f5b35d658d47868`、分支 `codex/mult2-01`、OpenFHE 固定提交 `df495ba2e91739a6dc8f1de254fc5a41155ce504`。
3. `source-records/SOURCE-MANIFEST.json:8-159` 列出 30 个输入对象的大小和 SHA-256。实际逐项复算全部一致，详见 `verification/input-integrity.txt`。
4. 输入 ZIP 不含 `.git` 对象。因此可以验证 `project/` 的全部受清单约束字节，但不能从包内独立重算 Git commit `a3a6a...`；该 commit 是**经清单绑定的供应身份**，不是本环境独立从 Git 对象证明的身份。
5. 输入 PDF SHA-256 为 `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`；PDF 共 15 页。第 7–8 页已按页面视觉检查，未只依赖 OCR/文本提取。

## B. INSPECTED — 论文定义、引理与尺度

输入 `PAPER-2023-1788.txt` 的关键位置：

- `584-606`：定义 4.1 的 Tensor2，保留 high×high 与两个 cross 项，丢弃 low×low；
- `608-660`：引理 4.2 的恒等式与“Tensor2 近似普通 Tensor 除以 `q_div`”解释；
- `664-783`：定义 4.3/引理 4.4 的 Relin2 构造与 `E_Relin+h` 关系；
- `785-891`：定义 4.5/引理 4.6 的 RS2 公式与 `(h+1)/2` 误差；
- `899-901`：定义 4.7 明确 `Mult2 := RS2 o Relin2 o Tensor2`；
- `903-935`：PDF 第 8 页的定理 4.8 显示比较项仅带 `1/q_l`；
- `937-945`：定理证明声称组合相邻引理；
- `947-958`：后续解释把主要误差写成含 `q_div*q_l`，并明确 overall scale 除以 `q_div*q_l`。

视觉检查确认第 8 页公式中的比较项确实只打印 `1/q_l`；不是文本抽取漏掉了紧邻的 `1/q_div`。

## C. INSPECTED — 供应的 OpenFHE 1.5.0 参考

1. `official-openfhe/rns-cryptoparameters.h:601-649`：FIXEDMANUAL 下 `GetScalingFactorReal` 与 `GetModReduceFactor` 都返回 `m_approxSF`，注释将其描述为 `2^p`。
2. `official-openfhe/ckkspackedencoding.cpp:331-333`：编码后记录尺度按 `noiseScaleDeg` 幂次更新。
3. `official-openfhe/ckkspackedencoding.cpp:336-406`：FIXEDMANUAL DCRT 解码路径按 `2^{-p*(noiseScaleDeg-1)}` 和最终 `2^{-p}` 处理，而不是把任意 ciphertext SF 当成逻辑尺度。
4. `official-openfhe/ckksrns-leveledshe.cpp:172-190`：ModReduce/Rescale 丢弃末塔，并更新 level、noise-scale degree 和 recorded scaling factor。
5. `official-openfhe/cryptocontext.h:1072-1079`：评估乘法密钥全局 map 和按 key tag 读取接口。
6. `official-openfhe/cryptocontext.h:1175-1205`：REAL/COMPLEX CKKS packed encoding 重载。
7. `official-openfhe/cryptocontext.h:1250-1277`：加密保留 slots/level/noise-scale/SF/encoding metadata。
8. `official-openfhe/cryptocontext.h:1325-1338`：解密公开接口。
9. `official-openfhe/cryptocontext.h:2021-2031`：公开 `Relinearize` 使用 ciphertext key tag 对应的 eval key vector。

## D. INSPECTED — 基线源码接缝

`final-project/` 是应用全部补丁后的完整导出；除下述 Mult2 新增外，其余行与清单绑定基线一致。

- `final-project/include/openfhe_2023_1788/double_ckks.h:38-137`：pair 构造器私有，公开状态只读；
- `final-project/include/openfhe_2023_1788/double_ckks.h:139-174`：一个 DoubleCKKS 绑定 context，并公开 DCP/Tensor2/Relin2/RS2/Mult2/RCB；
- `final-project/src/double_ckks.cpp:280-341`：物理 ciphertext 验证；
- `final-project/src/double_ckks.cpp:428-559`：pair/context/basis/scale/lifecycle 结构验证；
- `final-project/src/double_ckks.cpp:562-596`：Tensor2 双输入兼容性；
- `final-project/src/double_ckks.cpp:645-693`：Tensor2 现有算法体；
- `final-project/src/double_ckks.cpp:696-870`：Relin2 现有算法体及 HYBRID/BV key guards；
- `final-project/src/double_ckks.cpp:873-994`：RS2 现有算法体；
- `final-project/src/double_ckks.cpp:1001-1011`：RCB 现有算法体。

任务要求不修改上述五个算法体。最终生产 diff 只含：

- `final-project/include/openfhe_2023_1788/double_ckks.h:147`：精确 `const` 签名；
- `final-project/src/double_ckks.cpp:997-999`：三算子组合。

逐行差异见 `verification/production-code-diff.txt`。

## E. INSPECTED — 新测试与构建接入

### API 与行为红/绿

- `final-project/tests/mult2_api_contract_test.cpp:1-21`：用成员函数指针类型约束精确 `const` API；
- `final-project/tests/mult2_test.cpp:22-31`：参数与在实现前冻结的 `1e-3` 逻辑槽位绝对误差阈值；
- `final-project/tests/mult2_test.cpp:44-162`：真实 REAL 加密→DCP→Mult2→RCB→Decrypt，状态、basis、immutability、ratio 和解码误差。

### 独立系数 oracle

- `final-project/tests/mult2_e2e_oracle_test.cpp:34-49`：固定参数与预先冻结阈值；
- `62-205`：独立 `cpp_int` 算术、CRT、中心化和学校式负循环卷积；
- `222-367`：按真实 secret key 的 `c0+c1*s+...` 独立系数解密、pair 重组和 hamming weight；
- `440-460`：固定 OpenFHE 上下文参数；
- `482-530`：host envelope 与 `Q_l` 预选预算；
- `532-635`：公开输出的 context/encoding/level/degree/SF/tag/slots/components/format/精确 basis prefix 和双尺度状态；
- `654-777`：实际 `M_high/M_low/h/N/Q_l/q_div/q_l`、标准 relinearization 经验误差、pair Relin2 误差、non-wrap 和有理数误差上界；
- `785-838`：独立记录偏差核验与诊断性逻辑尺度校正后的槽位误差；
- `841-901`：完整参数与证书输出，明确 `conservative_E_Relin_available=false`、`universal_theorem_gate=UNPROVED`；
- `904-978`：真实加密、公开 staged seams、候选 Mult2、输入不变性、独立 oracle；
- `980-1047`：HYBRID/BV×REAL/COMPLEX 四用例选择及入口。

系数期望来自独立解密的输入 pair 与学校式负循环乘积；公开结果也由独立解密并直接 pair 重组。生产 RCB 只用于单独的调用者槽位解码路径，不作为系数 oracle 的 expected value。

### 拒绝路径

- `final-project/tests/mult2_validation_test.cpp:157-210`：eval-key cache 的指针级快照/恢复；
- `212-245`：固定夹具与缓存清理；
- `247-285`：第一次 Mult2 后清除 keys，终态作为左/右输入均必须先因生命周期失败，并保持输入/空缓存不变；
- `287-393`：context、key tag、recorded scale、ordered basis 失配的精确异常与不变性；
- `395-442`：五个独立用例入口。

### CMake/CI

- `final-project/CMakeLists.txt:57-78`：Mult2 API、行为、e2e oracle、validation targets；
- `final-project/CMakeLists.txt:80-106`：告警即错误门禁；
- `final-project/CMakeLists.txt:150-159`：10 个新增 CTest；
- `final-project/.github/workflows/dcp-rcb.yml:3-18`：`codex/mult2-01` 与固定 OpenFHE commit；
- `89-102`、`219-254`：Linux/MinGW 四个显式 API targets 与 CTest；
- `45-49`、`148-159`：Boost 依赖。

## F. EXECUTED — 本环境实际动作

1. 输入完整性、安全路径与 manifest 全量重算：PASS。
2. 从包内 baseline 内容创建临时本地审查历史并生成 8 个 format-patch。
3. 在新的 baseline 副本中按 `patches/series` 运行 `git am`：8/8 应用成功。
4. 比较 22 个最终项目文件：无缺失、额外或内容偏差；Git tree 一致。
5. 静态差异审计：生产只改 2 个文件、增加 5 行；无删除；没有改五个既有算法体。
6. workflow YAML 解析与关键步骤断言：PASS。
7. 使用**合成 OpenFHE CMake package metadata**执行 configure-only 图检查：声明 49 个 CTest；该动作没有编译任何 C++。
8. 在真实命令形态下尝试配置：因找不到 OpenFHE 1.5.0 package 失败。
9. 在隔离 Linux 工作区尝试获取固定 OpenFHE 源：因 DNS 失败，未开始构建。

完整记录位于 `verification/`。

## G. INFERRED — 从已检查事实推导

1. `m_i=q_div*a_i+b_i` 与定义 4.1 直接推出 Tensor2 重组目标含 `1/q_div`；RS2 再引入 `1/q_l`。因此定理 4.8 显示式很可能漏写 `1/q_div`。这是推断性勘误。
2. 在 FIXEDMANUAL 下，最终 logical/recorded 比为 `2^(2p)/(q_div*q_l)`；普通 decoder 会呈现相应确定性偏差，调用者需显式校正或未来采用新的公共 decode contract。
3. 一行组合会按语言求值顺序先调用 Tensor2，再 Relin2，再 RS2；因此既有验证和异常自然传播，没有必要在 Mult2 中复制验证或包裹异常。
4. 端到端测试设计满足“生产输出不能作为自身期望”的结构要求，但是否实际编译、是否所有随机执行均通过，仍需真实工具链裁决。

## H. UNVERIFIED — 明确保留

1. 清单所称源 commit `a3a6a...` 与包内字节的 Git-object 级一致性（包内无 `.git`）。
2. 新 API 和 3 个新测试可执行文件在 OpenFHE 1.5.0 上的实际编译/链接。
3. Linux GCC 和 Windows MinGW64 的 warning-clean 结果。
4. 49 项 CTest 的实际结果以及四条端到端证书数值。
5. `1e-3` 阈值是否由真实候选执行满足；阈值只确认已在行为绿之前冻结。
6. 深度 7 夹具在实际生成参数上的所有随机样本是否稳定；测试会对实际 `Q_l` 和实际 non-wrap 条件 fail-closed。
7. 通用保守 `E_Relin`，以及定理 4.8 的作者确认勘误。
8. 任何生产安全、性能、53/106-bit 精度或第二次乘法能力。

## I. 基线绿色证据的限制

`final-project/artifacts/tdd/rs2-valid-arithmetic/green.txt:1-25` 把 run 绑定到旧提交 `ed00f...` 和固定 OpenFHE；`100-113` 与 `185-199` 分别记录 Linux/Windows 的 39 项成功。该文件自己明确排除 Mult2、decoded precision 与定理证明，故不能用于本候选 PASS。
