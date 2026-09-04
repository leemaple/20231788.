# Commands and execution ledger

## Reconstruction status

All new source and patch bytes in this repair are **RECONSTRUCTED**. Exact prior
code recovery was impossible because the prior delivered/sanitized trees contain
no patch or final-source bytes. New hashes are bound in the package manifests.

## Hosted red commands

Run only on Linux GCC and Windows MinGW64 against pristine OpenFHE 1.5.0 commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`, maximum two build threads:

```bash
git apply --check <delivery>/patches/0001-red-freeze-dcp-rcb-high-precision-contract.patch
git apply <delivery>/patches/0001-red-freeze-dcp-rcb-high-precision-contract.patch

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=<pristine-openfhe-1.5.0-install>

cmake --build build --parallel 2 \
  --target dcp_rcb_test precision_dcp_rcb_contract_test

ctest --test-dir build --verbose --output-on-failure \
  -R '^precision_dcp_rcb_high_precision_contract$'
```

The valid red must reach one of the unchanged `2^-80` positive assertions. A
compile failure, basis/state failure, or non-wrap failure is not the intended
red and must be diagnosed rather than relabelled. The expected red says only
that the explicitly incomplete binary64 fixture cannot supply the frozen input.

## Hosted green commands

Without editing any frozen file:

```bash
git apply --check <delivery>/patches/0002-green-replace-only-precision-fixture.patch
git apply <delivery>/patches/0002-green-replace-only-precision-fixture.patch

PYTHONDONTWRITEBYTECODE=1 python3 \
  <delivery>/tools/verify_contract_continuity.py \
  <exact-bda8791-selected-project> <delivery>

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=<pristine-openfhe-1.5.0-install>

cmake --build build --parallel 2 \
  --target dcp_rcb_test precision_dcp_rcb_contract_test

ctest --test-dir build --verbose --output-on-failure \
  -R '^precision_dcp_rcb_high_precision_contract$'
```

A green requires every frozen assertion in all four fresh-key trials on both
hosts. Do not rename, remove, skip, invert, or relax the test after red.

## Compatibility command after Codex transplant

```bash
cmake --build build --parallel 2
ctest --test-dir build --verbose --output-on-failure
```

Bind the exact transplanted tree and report actual results. The exact bda8791
context retains an earlier 42/44 BV history; the user separately reports a
later combined implementation at 53/53 per host. That later source is not part
of this repair baseline, and functional green does not establish precision.

## Static verifier command executed here

```bash
PYTHONDONTWRITEBYTECODE=1 python3 \
  tools/verify_contract_continuity.py \
  <extracted-original-input>/project <delivery-root>
```

Result: PASS for exact baseline hash set, both numbered patch applications,
fixture-only green change, frozen-file hashes, aggregate equality, final-copy
equality, and forbidden-call checks.

## Standalone prerequisite command executed here

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -O0 \
  tools/standalone_precision_contract_math.cpp \
  -o <external-temporary-directory>/standalone_precision_contract_math
<external-temporary-directory>/standalone_precision_contract_math
```

The binary was deleted and is not delivered. The result was:

```text
binary64_collapses_sub_ulp=1
canonical_witnesses_pass=1
max_slot_error=2.22651837220627644059228883481591788253357366068371395462859574726257422794534356e-30
delta_error=1.96253444754786498577749932293067605503948640092412919257082327783698940021086596e-30
below_2^-80=1
```

This executes only Boost multiprecision arithmetic. It is not OpenFHE, Encrypt,
DCP, RCB, CTest, security, or performance evidence.

## Execution ledger

| Action | Status | Evidence / limit |
|---|---|---|
| Outer ZIP SHA-256/size | EXECUTED, PASS | `evidence/INPUT_INTEGRITY.txt` |
| Outer 21-payload manifest | EXECUTED, PASS | zero missing/extra/mismatch |
| Previous full-context 3-payload manifest | EXECUTED, PASS | nested identities match |
| Original input 51-payload manifest | EXECUTED, PASS | zero missing/extra/mismatch |
| Prior design return 25-entry manifest | EXECUTED, PASS | zero missing/extra/hash mismatch |
| Exact 30-file baseline hash set | EXECUTED, PASS | `evidence/BASELINE_PROJECT.sha256` and verifier |
| Red/green/aggregate patch application | EXECUTED, PASS | static only |
| Contract continuity and final-copy equality | EXECUTED, PASS | static only |
| Python verifier source compile in memory | EXECUTED, PASS | no `.pyc` generated |
| Standalone C++ compile/runtime | EXECUTED, PASS | Boost-only arithmetic |
| CMake configuration probe | EXECUTED, OpenFHE NOT FOUND | stopped at `find_package` |
| Candidate OpenFHE compilation | **NOT EXECUTED** | pristine dependency absent |
| Encrypt/DCP/RCB runtime | **NOT EXECUTED** | no candidate build |
| Hosted focused red | **NOT EXECUTED** | Codex owns observation |
| Hosted focused green | **NOT EXECUTED** | Codex owns observation |
| Full CTest | **NOT EXECUTED** | no integration claim |
| GitHub Actions dispatch/rerun/cancel | **NOT EXECUTED** | prohibited |
| Git push/merge | **NOT EXECUTED** | prohibited |
| Mac OpenFHE/crypto | **NOT EXECUTED** | prohibited |
| Security/performance benchmark | **NOT EXECUTED** | outside scope |
| Output Gitleaks scan | **NOT EXECUTED** | tool unavailable; no inherited scan is recast as current execution |

## Result labels for hosted evidence

A build-only result is `BUILD ONLY`. A focused pass is
`DCP→RCB DIAGNOSTIC PRECISION ONLY`. Neither may be reported as Mult2 precision,
repeated precision, Table 3, security, performance, or production I/O support.
