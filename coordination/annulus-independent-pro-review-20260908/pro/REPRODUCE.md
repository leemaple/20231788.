# 只用标量重放复核本交付

需要原始`annulus-independent-03f37b6.zip`的完整解包目录，以及本审查包。主检查仅需Python标准库；本次实际Python3.13.5。推荐使用Python3.11或更高；其他版本未在本次执行验证。**不需要OpenFHE，不执行FFT/加密/构建。**

输入是已核验的用户数据包。涉及receiver/finalizer的脚本会导入所给Python模块，所以不要对任意未经审阅的第三方ZIP直接执行这些命令。本次已审查这些模块，操作限于解析、hash和本地临时目录。

下列命令是本次实际调用参数的可移植整理。修改前三个绝对路径；输出写到新临时目录，不覆盖随包的冻结结果。不要改原始process记录里的远端绝对路径。

```bash
set -eu
P='/absolute/path/to/extracted-original-packet'
R='/absolute/path/to/extracted-review-package'
A='/absolute/path/to/annulus-independent-03f37b6.zip'
OUT="$(mktemp -d)"
mkdir -p "$OUT/results" "$OUT/logs"
S="$P/project/coordination/s100-annulus125-20260908"
RAW="$S/experiment-evidence/sample/raw.tsv"
END="$S/experiment-evidence/sample/program-end.json"
COMMIT='03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b'

for D in 180 230; do
  python3 -B "$R/checks/independent_scalar.py" \
    --tsv "$RAW" --end "$END" --precision "$D" \
    --output "$OUT/results/independent_${D}.json" \
    >"$OUT/logs/independent_${D}.log" 2>&1
  python3 -B "$R/checks/replay_old_s100.py" \
    --packet "$P" --precision "$D" \
    --output "$OUT/results/old_s100_${D}.json" \
    >"$OUT/logs/old_s100_${D}.log" 2>&1
done

python3 -B "$R/checks/verify_packet.py" \
  --zip "$A" --packet "$P" --results "$OUT/results" \
  --output "$OUT/results/packet_and_process.json" \
  >"$OUT/logs/packet_and_process.log" 2>&1

python3 -B "$R/checks/independent_boundaries.py" \
  --packet "$P" --results "$OUT/results" \
  --output "$OUT/results/boundary_challenges.json" \
  >"$OUT/logs/boundary_challenges.log" 2>&1

# 论文印刷式反例：必须返回1。不是生产源码RED。
set +e
python3 -B "$R/checks/independent_boundaries.py" \
  --packet "$P" --results "$OUT/results" --literal-paper-red \
  --output "$OUT/results/literal_paper_red.json" \
  >"$OUT/logs/literal_paper_red.log" 2>&1
RC=$?
set -e
printf 'process_exit=%s; expected=1\n' "$RC" >>"$OUT/logs/literal_paper_red.log"
test "$RC" -eq 1

# 补充：运行提供者的receiver；核心结论不以此代替独立检查。
python3 -B "$S/replay_annulus125.py" \
  --tsv "$RAW" --source-commit "$COMMIT" --process-exit 0 --precision 230 \
  >"$OUT/results/supplied_replay_230.json" \
  2>"$OUT/logs/supplied_replay_230.log"

# 历史receiver应假接收合成负例，脚本以1报告预期RED。
set +e
python3 -B "$R/checks/receiver_regression.py" --packet "$P" --target historical \
  --output "$OUT/results/receiver_historical_red.json" \
  >"$OUT/logs/receiver_historical_red.log" 2>&1
RC=$?
set -e
printf 'process_exit=%s; expected=1\n' "$RC" >>"$OUT/logs/receiver_historical_red.log"
test "$RC" -eq 1

python3 -B "$R/checks/receiver_regression.py" --packet "$P" --target current \
  --output "$OUT/results/receiver_current_green.json" \
  >"$OUT/logs/receiver_current_green.log" 2>&1

python3 -B "$R/checks/post_author_scalar.py" --results "$OUT/results" \
  --output "$OUT/results/post_author_scalar.json" \
  >"$OUT/logs/post_author_scalar.log" 2>&1

# 读取本审查原始结果和冻结清单，核验交付本身；不覆盖原结果。
python3 -B "$R/checks/verify_delivery.py" --review "$R" --packet "$P" --zip "$A" \
  --output "$OUT/results/delivery_quality.json" \
  >"$OUT/logs/delivery_quality.log" 2>&1
printf 'Scalar outputs: %s\n' "$OUT"
```

预期：新annulus两档PASS；旧Linux/Windows的标量程序正常退出，但其`retained_E80`必须仍为FAIL；13组边界检查通过；历史receiver出现预期RED，当前receiver为GREEN。S116只有收据上界检查，不会生成不存在的全槽重放数据。

时间戳、绝对输出路径、elapsed_scalar_seconds可能因本地环境不同而变化，不要求新生成JSON逐字节等同。本包MANIFEST用于核对**原交付字节**，复核数学结果应比较门禁、槽号和足够精度的数值，不用运行耗时或新文件hash充当数值正确性判断。

独立脚本是固定任务检查器，不是通用或恶意输入安全的TSV服务。它们对本次列序、来源、规模、精度档和元数据严格绑定；不建议以“兼容其他输入”为由放宽当前证据门禁。
