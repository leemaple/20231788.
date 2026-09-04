# Minimal RED / PROBE / GREEN candidate

## Scope invariant

The series is test-only. It does not change:

- `src/double_ckks.cpp` or any public header;
- Tensor2, Relin2, RS2, Mult2, DCP, or RCB production behavior;
- the exact vectors or `1e-3` decoded threshold;
- `FIXEDMANUAL`, `p=30`, `N=64`, `digitSize=0`, or key-switch selection;
- context/key guards, lifecycle checks, basis order, rescaling, or security/decryption protections;
- CMake test registration or the existing count of 44 tests.

## 0000 — RED baseline record

File: `patches/0000-RED-BASELINE.md`

No synthetic code patch is supplied because the immutable source and two retained platform logs already provide the required real red. The record binds the old fatal assertion, exact Linux values, Windows completion update, and the fact that later BV checks were not reached.

## 0001 — PROBE: same-input Relin2 path oracle

File: `patches/0001-PROBE-per-path-relin2-execution-oracle.patch`

Changed file:

```text
tests/mult2_e2e_oracle_test.cpp
```

The probe independently reconstructs the actual ordinary paths from the public Tensor2 output:

1. clone the Tensor high ciphertext;
2. multiply every active tower by `q_div`;
3. append the exact final full-basis `q_div` tower as zero;
4. ordinary-relinearize this raised high ciphertext;
5. independently drop the final tower for a `Q_l` view;
6. ordinary-relinearize Tensor low on `Q_l`;
7. independently decrypt both paths; and
8. compare, for every coefficient,

   ```text
   production pair error == independent high-path error + independent low-path error (mod Q_l).
   ```

It also records:

- high-path maximum error;
- low-path maximum error;
- their triangle bound;
- exact coefficientwise paper-additivity residual; and
- whether that residual is at most runtime `h`.

The historical assertion

```text
pair error <= ordinary combined execution error + h
```

remains fatal in 0001. This guarantees that the probe does not erase or retrospectively convert the real red into a pass.

### Required interpretation of a hosted probe

| Probe result | Required action |
|---|---|
| Path identity fails | Stop. Reopen production Relin2/DCP arithmetic and both independent decrypt/reference paths. Do not apply 0002. |
| Identity passes; residual exceeds `h` | Production path composition is supported for that execution; paper Lemma 4.4's near-additivity step is not supported for the backend execution. Preserve diagnostic values. |
| Identity passes; residual is at most `h` | The old inequality should also pass by triangle inequality. Any contrary result means a test/oracle inconsistency and is a hard stop. |
| Any exact Tensor2/Relin2/RS2 regression fails | Stop. A green execution certificate cannot override an arithmetic regression. |

## 0002 — GREEN CANDIDATE: separate execution certificate

File: `patches/0002-GREEN-separate-execution-certificate.patch`

Precondition: 0001 has been applied and successfully adjudicated on both required platforms.

This patch:

- removes the old fatal use of one combined-call sample as `E_Relin+h`;
- retains the combined call and exact additivity residual as diagnostics;
- accepts only the independently reconstructed per-path triangle for the exact execution;
- substitutes that conditional pair-error bound into the non-wrap witness and corrected execution-specific coefficient expression;
- labels the result `execution_certificate=PER_PATH_CONDITIONAL`;
- labels the theorem status `conservative_E_Relin_available=false` and `universal_theorem_gate=UNPROVED`.

It does **not** declare the paper's BV theorem proven. It merely allows the existing BV public seam to proceed to the independent output-coefficient and decoded checks without using a disproved backend-specific inequality as a generic fatal certificate.

## Final changed files

Exactly one repository file differs after both patches:

```text
tests/mult2_e2e_oracle_test.cpp
```

Candidate SHA-256:

```text
4be87bd5b16f5139b7b39d09143865ff1d4df60b4c533ddd08407d5f99da8c14
```

A byte-identical copy is at `candidate/tests/mult2_e2e_oracle_test.cpp`.

## Static patch validation performed

The retained `evidence/static-patch-checks.txt` records:

- 0001 `git apply --check --whitespace=error-all`: PASS;
- 0001 application and `git diff --check`: PASS;
- 0002 ordered `git apply --check --whitespace=error-all`: PASS;
- 0002 application and `git diff --check`: PASS;
- final changed-file set: only the e2e test;
- final candidate byte comparison: PASS;
- structural presence/absence checks: PASS.

These are patch/static checks, not C++ compilation or numerical execution.
