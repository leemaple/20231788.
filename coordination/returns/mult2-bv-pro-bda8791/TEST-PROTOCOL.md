# Hosted test protocol

## 1. Required environment and immutable inputs

Run cryptographic builds/tests only on the prescribed hosted environments:

- Linux with GCC;
- Windows with MinGW64;
- pristine OpenFHE 1.5.0 at commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`;
- exact project commit `bda879104c8a8b1ba6ac9301385b5b1919bef440` before patch application;
- compiler warnings enabled as errors (`-Wall -Wextra -Wpedantic -Werror` on the non-MSVC path).

Do not run the OpenFHE cryptographic suite on the user's Mac. Do not use another OpenFHE build, fork, or modified local install.

Verify the candidate test file only after ordered patch application; do not copy it over an unknown checkout.

## 2. Frozen parameters and runtime facts

| Quantity | Exact value / rule |
|---|---|
| Ring dimension `N` | `64` |
| Batch size | `16` |
| Host vector length | `8` |
| Scaling technique | `FIXEDMANUAL` |
| `p` / scaling-modulus size | `30` |
| First-modulus size | `35` |
| Multiplicative depth | `7` |
| Encoding noise-scale degree | `2` |
| Encoding level | `0` |
| Key-switch techniques covered | genuine `HYBRID` and genuine `BV` |
| BV digit size | `0` |
| Max relinearization secret-key degree | `2` |
| Secret distribution | `UNIFORM_TERNARY` |
| Security level | `HEStd_NotSet` — functional-only |
| Frozen decoded logical absolute threshold | `1e-3`; not a precision-bit or security claim |

Full ordered eight-tower context basis:

```text
34359736577
1073744257
1073738753
1073742721
1073739649
1073742209
1073741441
1073741953
```

Active `Q_l` is the first seven factors:

```text
Q_l factors =
[34359736577, 1073744257, 1073738753, 1073742721,
 1073739649, 1073742209, 1073741441]

Q_l =
52656049226897061758347970843194892279389197066160739197584863617

bit length(Q_l) = 215
q_l   = 1073741441
q_div = 1073741953
2^(2p)/(q_div*q_l) = 1.000000236556032764816331812324746...
```

`h` is the Hamming weight of the newly generated ternary secret and must be recomputed every execution. It must not be frozen or tuned. The retained Linux diagnostic observed:

```text
HYBRID REAL h=42
HYBRID COMPLEX h=46
BV REAL h=43
BV COMPLEX h=44
```

The authoritative Windows matrix did not print BV `h`; do not infer it.

## 3. Phase A — exact baseline reproduction

Before applying any patch, verify the source identity and reproduce the current result.

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=<pristine-install>
cmake --build build --parallel 2
cmake --build build --parallel 2 --target \
  relin2_api_contract_test rs2_api_contract_test mult2_api_contract_test
ctest --test-dir build --verbose --output-on-failure
```

Focused public BV REAL loop:

```bash
ctest --test-dir build \
  -R '^mult2_e2e_bv_real$' \
  --verbose --output-on-failure
```

Also run the exact four-case matrix explicitly:

```bash
ctest --test-dir build \
  -R '^mult2_e2e_(hybrid_real|hybrid_complex|bv_real|bv_complex)$' \
  --verbose --output-on-failure
```

Expected historical signature from retained evidence is `42/44`, with only the two BV tests failing at the old empirical-bound message. A different signature is not automatically accepted; preserve logs and investigate source/environment identity first.

## 4. Phase B — apply and run 0001 PROBE only

```bash
git apply --check --whitespace=error-all \
  patches/0001-PROBE-per-path-relin2-execution-oracle.patch
git apply patches/0001-PROBE-per-path-relin2-execution-oracle.patch
git diff --check
cmake --build build --parallel 2 --target mult2_e2e_oracle_test
```

Run BV REAL and COMPLEX separately so each complete diagnostic is retained:

```bash
ctest --test-dir build -R '^mult2_e2e_bv_real$' \
  --verbose --output-on-failure
ctest --test-dir build -R '^mult2_e2e_bv_complex$' \
  --verbose --output-on-failure
```

Run both HYBRID regressions:

```bash
ctest --test-dir build \
  -R '^mult2_e2e_hybrid_(real|complex)$' \
  --verbose --output-on-failure
```

Required probe fields include:

```text
ordinary_combined_relin_execution_error=
high_path_relin_error=
low_path_relin_error=
execution_relin2_triangle_bound=
paper_additivity_residual=
paper_additivity_execution_observed=
oracle_basis_agreement=true
```

### Probe stop conditions

Stop and reject 0002 if any of these occurs:

- `Relin2 pair error differs from independent high + low paths ...`;
- basis, modulus, or coefficient shape disagreement;
- the path triangle itself fails;
- any exact Tensor2/Relin2/RS2 regression changes from green;
- input, key, metadata, or evaluation-key cache mutation appears;
- the old assertion outcome cannot be reconciled with the exact residual.

If path identity passes and the old BV assertion still fails, that is the expected discriminating result: it supports production path composition for that execution while preserving the BV/paper near-additivity gap.

## 5. Phase C — conditional 0002 GREEN candidate

Apply only after Phase B is adjudicated:

```bash
git apply --check --whitespace=error-all \
  patches/0002-GREEN-separate-execution-certificate.patch
git apply patches/0002-GREEN-separate-execution-certificate.patch
git diff --check
sha256sum tests/mult2_e2e_oracle_test.cpp
```

Required candidate file SHA-256:

```text
4be87bd5b16f5139b7b39d09143865ff1d4df60b4c533ddd08407d5f99da8c14
```

Reconfigure from a clean build directory if the CI policy requires it, then execute the full hosted loop:

```bash
rm -rf build
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=<pristine-install>
cmake --build build --parallel 2
cmake --build build --parallel 2 --target \
  relin2_api_contract_test rs2_api_contract_test mult2_api_contract_test
ctest --test-dir build --verbose --output-on-failure
```

Explicit exact arithmetic and composition subset:

```bash
ctest --test-dir build \
  -R '^(tensor2_valid_arithmetic_immutability|relin2_valid_arithmetic_state_immutability|relin2_bv_zero_digit_valid_shapes|rs2_valid_arithmetic_state_immutability|mult2_composition_contract)$' \
  --verbose --output-on-failure
```

Explicit e2e matrix:

```bash
ctest --test-dir build \
  -R '^mult2_e2e_(hybrid_real|hybrid_complex|bv_real|bv_complex)$' \
  --verbose --output-on-failure
```

## 6. Acceptance interpretation

A candidate run is acceptable for integration review only if:

- all 44 registered tests pass on both required platforms;
- all three API targets build under warnings-as-errors;
- the new same-input path identity passes coefficientwise in all four e2e cases;
- the independent final coefficient expression and decoded checks are actually reached;
- input ciphertexts/pairs remain unchanged;
- evaluation keys and process-wide caches remain unchanged;
- no parameters, vectors, thresholds, test registrations, or production files differ beyond the supplied patch series; and
- output retains the conditional/unproved labels.

Even then, the correct claim is:

```text
The exact tested implementation paths and execution-specific certificates passed.
```

It is **not**:

```text
A conservative E_Relin was established.
Lemma 4.4 or Theorem 4.8 was proved for OpenFHE BV/HYBRID.
BV precision or security was established.
The printed theorem was authoritatively corrected.
```

## 7. Failure triage after 0002

| Failure | Interpretation |
|---|---|
| Same-input path identity fails | Production Relin2/DCP arithmetic or one independent path/decrypt oracle is wrong; investigate exact coefficient first |
| Tensor2 exact oracle fails | Tensor construction defect; execution error bounds are irrelevant |
| Relin2 exact component oracle fails | Raised-high/DCP/recombination wrapper defect; do not accept e2e triangle |
| RS2 exact oracle fails | Rescale/recombination defect; do not adjust a noise bound |
| Conditional non-wrap fails | Current random execution is outside the witnessed no-wrap condition; do not tune to maxima |
| Final coefficient expression fails | Investigate Tensor2 omission term, Relin2 path errors, RS2 rounding, and inferred scale normalization separately |
| Decoded logical check fails | Precision/encoder/scale interaction; do not weaken `1e-3` in this task |
| All implementation tests pass but additivity residual exceeds `h` | Expected proof-gap outcome for BV; theorem gate remains `UNPROVED` |
