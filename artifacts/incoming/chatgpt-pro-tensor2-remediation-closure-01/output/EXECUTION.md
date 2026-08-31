# Execution record

This file distinguishes commands actually executed by this reviewer from retained hosted evidence inspected in the package.

## 1. Local environment

Observed locally during this review:

```text
OS/kernel: Linux x86_64, kernel 6.18.35
CMake:     3.31.6
C++:       c++ (Debian 14.2.0-19) 14.2.0
Boost:     /usr/include/boost/multiprecision/cpp_int.hpp present
```

No local Windows environment was used.

## 2. Input/package commands actually run

The supplied ZIP, binding, and standalone task were processed directly from their mounted attachment paths. The following classes of commands were executed before source review/build:

```sh
stat / size inspection of the ZIP and standalone files
sha256sum <ZIP>
sha256sum <binding>
sha256sum <task>
unzip -Z1 <ZIP> | wc -l
unzip -t <ZIP>
unzip -q <ZIP> -d /mnt/data/tensor2_remediation_closure_input
sha256sum HANDOFF_CONTENTS.md
sha256sum MANIFEST.sha256
(cd /mnt/data/tensor2_remediation_closure_input && sha256sum -c MANIFEST.sha256)
cmp FINAL_REVIEW_TASK.md <standalone-task>
```

Results:

- ZIP size: `9,334,115` bytes — match;
- ZIP SHA-256: `54c4ab7ad202bfe8cb9c95a58816bd9d30af2132978b75ea3a074e1c4f561832` — match;
- binding SHA-256: `3572ed940fda807c20c19bd57ada8cfff08e3a6da0cc8dba95f8c2bc849954fc` — match;
- standalone task SHA-256: `71537ff615292b56a701866c22ecc722020d4175f2ba81b4f504364b833680b2` — match;
- ZIP entries: `2,310` — match;
- `unzip -t`: exit `0`;
- internal handoff SHA-256: `ba01c3fd5c8537d75223ae40f28684f85c22ce76f6abd0deade8ae3ebc10f5e3` — match;
- internal manifest SHA-256: `4fdc1087cf28fa058d013e2ff25f4b20bdb197bbf80a2eaa250bb3e1a2c4f95e` — match;
- manifest verification: `2,037/2,037` files passed, exit `0`;
- internal task vs standalone task: byte-identical, exit `0`.

Additional local inspections checked ZIP path safety and the task's excluded filename/directory classes. No absolute/`..` traversal entry, `.git`, generated build tree, `node_modules`, CMake cache tree, browser-state database, `.env`, or credential/key container matching the targeted exclusions was found. The package's Gitleaks 8.30.1 results were **not rerun locally**; they are retained packaging evidence only.

## 3. Current-tree and diff commands actually run

A disposable copy of the supplied exported project was used for Git tree computation. No supplied source bytes were edited.

Representative command sequence:

```sh
git init
git add -f -A
git write-tree
git apply --reverse --check REMEDIATION_DIFF.patch
git apply --reverse REMEDIATION_DIFF.patch
git add -f -A
git write-tree
git apply --reverse --check PROJECT_DIFF.patch
```

Results:

- exact current exported tree recomputed as `759d5195739684748d5a9664edabe3fa719e1acf` — match;
- reverse-check of `REMEDIATION_DIFF.patch` — exit `0`;
- reversing the remediation range reconstructed previous reviewed tree `2269bee6bac5e7cd1124ab78c49a750af9a38942` — match;
- reverse-check of full `PROJECT_DIFF.patch` — exit `0`.

The clean package has no `.git`, so no command in this review could independently prove commit-object ancestry or absence of history rewriting. That remains explicitly unverified.

## 4. Source, paper, OpenFHE, and evidence inspection actually performed

The exact current project source/tests/workflow, both supplied diff/history ranges, previous review material, internal two-axis reviews, raw intermediate/final GitHub run/jobs/log files, supplied paper, and the relevant pristine OpenFHE source files were inspected.

For the PDF, all pages were rendered from the supplied file and the page containing Section 4 / Definition 4.1 / Lemma 4.2 was visually inspected. This was source inspection, not an execution result for the C++ project.

SHA-256 values for the intermediate hosted raw evidence files and the P3/final hosted evidence files were recomputed and compared with their tracked mapping records. JSON execution states/head SHAs/timestamps and job-log outcomes were parsed/inspected. These are **retained remote execution facts**, not local CI runs.

## 5. Local configure/build/CTest actually run

Before building:

```sh
cmake --version
c++ --version
test -f /usr/include/boost/multiprecision/cpp_int.hpp
```

Results: CMake 3.31.6; Debian GCC 14.2.0; Boost `cpp_int` header present.

The supplied pristine OpenFHE source was configured outside the supplied trees using the task's bounded command core:

```sh
cmake -S /mnt/data/tensor2_remediation_closure_input/openfhe-1.5.0 \
  -B /tmp/t2-rem-openfhe-build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/tmp/t2-rem-openfhe-install \
  -DBUILD_UNITTESTS=OFF -DBUILD_EXAMPLES=OFF \
  -DBUILD_BENCHMARKS=OFF -DBUILD_EXTRAS=OFF -DWITH_OPENMP=ON
```

Configure result: exit `0`.

OpenFHE was then built with at most two jobs:

```sh
cmake --build /tmp/t2-rem-openfhe-build --parallel 2
```

The execution environment imposes a 240-second per-tool-call ceiling. The long OpenFHE build therefore encountered five tool-level timeout terminations at that ceiling. Each continuation reused the same generated build tree; these were sandbox command-duration interruptions, not compiler or test failures. A sixth continuation completed the OpenFHE build to 100% with exit `0`. The cumulative attempt remained within the task's 45-minute cap.

Installation:

```sh
cmake --install /tmp/t2-rem-openfhe-build
```

Result: exit `0`.

Exact-current project configure/build:

```sh
cmake -S /mnt/data/tensor2_remediation_closure_input/cleanroom-project \
  -B /tmp/t2-rem-project-build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=/tmp/t2-rem-openfhe-install
cmake --build /tmp/t2-rem-project-build --parallel 2
```

Results:

- project configure: exit `0`;
- strict project build: exit `0`.

CTest was executed after the build:

```sh
ctest --test-dir /tmp/t2-rem-project-build --output-on-failure
```

Initial post-build result: exit `0`, **6/6 passed**.

A final packaging-time recheck ran the same CTest command again. Result: exit `0`, **6/6 passed**, reported total real test time `0.09 sec`:

```text
1/6 dcp_rcb ................................... Passed
2/6 tensor2_valid_arithmetic_immutability ..... Passed
3/6 tensor2_result_scale_contract ............. Passed
4/6 tensor2_right_input_validation ............ Passed
5/6 tensor2_mutual_compatibility .............. Passed
6/6 tensor2_prearithmetic_key_compatibility ... Passed
```

No supplied project/OpenFHE source tree was modified by these builds; generated build/install files were kept under `/tmp`.

## 6. Retained hosted execution inspected, not run locally

No CI was dispatched or rerun. The following provider records were inspected from the package:

- `33425868973` — intended Linux public API compile red; Windows cancelled/unrun for project;
- `33426712752` — Linux strict build, DCP/RCB green, five independently reported Tensor2 scaffold reds; Windows cancelled/unrun for project;
- `33427271692` — first Linux implementation green, 6/6; Windows cancelled/unrun for project;
- `33436068864` — P3 test-only Linux red, 5/6 with only `dcp_rcb` failing on `ciphertext state`; Windows cancelled;
- `33436252725` — exact-current `fb862a3...` final run, Linux 6/6 and Windows 6/6, overall success.

These are described only as retained remote evidence.

## 7. Work not run / claims not made

Not run or not independently proven in this review:

- no local Windows build or CTest;
- no GitHub Actions dispatch/rerun/cancel operation;
- no credential use;
- no Windows ZCode/Zima same-commit review;
- no precision or performance benchmark;
- no Relin2/RS2/Mult2 or later-operation execution;
- no serialization or `t>2` work;
- no network-security assessment;
- no Git object ancestry/no-history-rewrite proof because `.git` is excluded;
- no fresh Gitleaks execution.

## 8. Patch disposition

No concrete current-head defect was found. Consequently `0001-tensor2-remediation-closure-fixes.patch` is **not produced**, as required by the task's conditional deliverable rule.
