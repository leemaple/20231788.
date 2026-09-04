# Commands and execution ledger

## 1. Exact hosted commands

Run inside a clean checkout containing the exact project baseline and pristine
OpenFHE 1.5.0 installation. Use distinct red and green worktrees or preserve the
exact resulting tree hash after each patch.

### 1.1 Apply and verify RED

```bash
git apply --check patches/0001-red-freeze-dcp-rcb-high-precision-contract.patch
git apply patches/0001-red-freeze-dcp-rcb-high-precision-contract.patch

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=<pristine-openfhe-1.5.0-install>

cmake --build build --parallel 2 \
  --target dcp_rcb_api_test precision_dcp_rcb_contract_test

ctest --test-dir build --verbose --output-on-failure \
  -R '^precision_dcp_rcb_high_precision_contract$'
```

Expected status is a **real hosted red at the unchanged positive precision
assertion**, not a compile failure and not a state/basis failure. Preserve the
complete log. Do not reword the result as an upstream encoder or DCP defect: the
red fixture discards the source delta before the public seam.

### 1.2 Apply and verify GREEN

```bash
git apply --check patches/0002-green-replace-only-precision-fixture.patch
git apply patches/0002-green-replace-only-precision-fixture.patch

python3 tools/verify_contract_continuity.py \
  <exact-baseline-project-directory> patches

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=<pristine-openfhe-1.5.0-install>

cmake --build build --parallel 2 \
  --target dcp_rcb_api_test precision_dcp_rcb_contract_test

ctest --test-dir build --verbose --output-on-failure \
  -R '^precision_dcp_rcb_high_precision_contract$'
```

A valid green requires all four fresh-key trials to satisfy the frozen
assertions on both Linux GCC and Windows MinGW64. No threshold or vector may be
edited after observing red.

### 1.3 Full compatibility accounting

```bash
cmake --build build --parallel 2
ctest --test-dir build --verbose --output-on-failure
```

The supplied baseline has two inherited BV empirical certificate failures.
They are outside this patch. Keep them visible and report them separately; do
not suppress, skip or relabel them to manufacture a full-suite green.

### 1.4 Standalone arithmetic prerequisite

This command checks only the fixture arithmetic and direct canonical evaluator;
it is not OpenFHE or crypto evidence.

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -O0 \
  tools/standalone_precision_contract_math.cpp \
  -o standalone_precision_contract_math
./standalone_precision_contract_math
```

## 2. What was executed in this review environment

| Action | Status | Evidence / limitation |
|---|---|---|
| Outer archive SHA-256 and four-member manifest verification | EXECUTED, PASS | `evidence/INPUT_INTEGRITY.txt` |
| Nested input 51-payload manifest verification | EXECUTED, PASS | zero missing/hash/size mismatches |
| Prior return 25-entry manifest verification | EXECUTED, PASS | 26 regular files including manifest |
| Sequential red/green `git apply --check` and application | EXECUTED, PASS | `evidence/PATCH_APPLY_AND_CONTRACT_CHECK.txt` |
| Candidate-final patch reapplication and byte comparison | EXECUTED, PASS | final changed files matched |
| Red→green changed-path check | EXECUTED, PASS | only `tests/precision_dcp_rcb_fixture.cpp` |
| Contract/CMake/header hash continuity | EXECUTED, PASS | `evidence/TEST_CONTRACT_HASH_LEDGER.txt` |
| Python verifier syntax check | EXECUTED, PASS | `python3 -m py_compile` |
| Output-directory Gitleaks scan | NOT EXECUTED | tool unavailable; inherited user-supplied scan remains source evidence |
| Standalone Boost arithmetic compile with warnings as errors | EXECUTED, PASS | `evidence/STANDALONE_MATH_CHECK.txt` |
| Standalone arithmetic runtime | EXECUTED, PASS | binary64 collapse, witnesses and `2^-80` arithmetic check only |
| CMake environment/configure probe | EXECUTED | OpenFHE package absent; see `evidence/CMAKE_ENVIRONMENT_PROBE.txt` |
| Candidate OpenFHE build | **NOT EXECUTED** | pristine OpenFHE unavailable in this container |
| Candidate encryption, DCP, RCB runtime | **NOT EXECUTED** | no OpenFHE build |
| Focused CTest red | **NOT EXECUTED** | Codex/hosted runner owns observation |
| Focused CTest green | **NOT EXECUTED** | Codex/hosted runner owns observation |
| Full CTest suite | **NOT EXECUTED** | inherited 42/44 evidence only |
| GitHub Actions dispatch/cancel | **NOT EXECUTED** | prohibited and not attempted |
| Git push/merge | **NOT EXECUTED** | prohibited and not attempted |
| Mac OpenFHE/crypto execution | **NOT EXECUTED** | prohibited and not attempted |
| Security/performance benchmark | **NOT EXECUTED** | outside scope |

## 3. Standalone result interpretation

The retained output demonstrates only that, in an independent Boost-only
program:

- the chosen `(2^-70, 2^-73)` difference collapses when converted to binary64
  near `(0.125, -0.0625)`;
- the direct canonical evaluator passes constant, phase and hard-coded ordering
  witnesses;
- fresh `2^100` coefficient rounding reconstructs the literal vector within the
  frozen arithmetic threshold.

It does not execute `Plaintext`, `Encrypt`, DCP, RCB, CRT towers or any OpenFHE
code and cannot certify the candidate green.

## 4. Hosted claim ledger format

Each hosted artifact should bind:

```text
OpenFHE commit
project baseline commit/tree
patch file SHA-256
resulting project tree
runner OS/image
compiler and CMake versions
focused CTest raw log
per-trial q_div, active modulus bits, delta_error, max_slot_error
full-suite raw log and inherited BV status
```

A build-only result is labelled **build-only**. A focused green is labelled
**DCP→RCB diagnostic precision only**. Neither is a Mult2, repeated-precision,
security or performance result.
