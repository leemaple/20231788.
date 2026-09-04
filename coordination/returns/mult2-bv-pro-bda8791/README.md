# Independent BV/Relin2 diagnosis and corrective candidate

## Decision

**AMEND — the current fatal BV certificate is not a valid theorem-level certificate, and the pinned OpenFHE BV backend does not satisfy the paper proof's near-additivity step on the two retained BV executions. The supplied evidence does not establish a defect in the production `Relin2` wrapper arithmetic.**

This package therefore makes **no production-code change**. It provides a two-stage, test-only candidate:

1. a **PROBE** patch that independently reconstructs the exact high and low ordinary-relinearization paths used by `Relin2`, checks the public-seam identity coefficient by coefficient, computes the paper-additivity residual, and deliberately leaves the historical fatal assertion in place; and
2. a **GREEN CANDIDATE** patch, to be considered only after the probe passes on hosted Linux and Windows, that separates an execution-specific per-path certificate from the still-unproved Lemma 4.4/Theorem 4.8 gate.

The final candidate continues to print:

- `conservative_E_Relin_available=false`
- `universal_theorem_gate=UNPROVED`
- `execution_certificate=PER_PATH_CONDITIONAL`

Passing the candidate would establish only the tested implementation/execution claims. It would not establish a conservative `E_Relin`, a universal theorem, a security claim, a precision-bit claim, or an author-confirmed correction to the printed paper.

## Exact input boundary

- Input ZIP: `mult2-bv-diagnosis-bda8791.zip`
- Bytes: `1024988`
- SHA-256: `cfd92c7181253a92dc0e8fac2a7b18bafba19eadb2fbe86416d638963eb0009d`
- Manifest SHA-256: `d68de3d44065fdbe966cc1f6060ee33cb6b86219d537a9b680844a9a3e1ad7e2`
- Project commit: `bda879104c8a8b1ba6ac9301385b5b1919bef440`
- Branch: `codex/mult2-01`
- Pinned OpenFHE commit: `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- Manifest-bound payloads: `51`; ZIP regular files including the manifest: `52`
- Identity result: no missing, extra, size-mismatched, or hash-mismatched payloads

The user-supplied update for Windows run `33839781546`, job `100919538008`, is incorporated from `evidence/matrix-red-windows.txt`: `42/44`, `1.18 s`, with the same two BV failures. No successful Fable review exists; the reported attempts ended with HTTP 403 before inference.

## Package map

- `DIAGNOSIS.md` — equations, paper/OpenFHE mapping, hypothesis adjudication, and exact limits of the conclusion.
- `SOURCE-IDENTITY.md` — archive, manifest, Git, reference, CI, and provenance checks.
- `PATCH-SERIES.md` — RED/PROBE/GREEN sequence and the single final changed file.
- `CLAIM-TO-TEST-MATRIX.md` — non-circular arithmetic gates versus execution-only and universal claims.
- `TEST-PROTOCOL.md` — exact hosted reproduction and candidate-validation protocol.
- `EXECUTION-LEDGER.md` — what was and was not executed in this review environment.
- `patches/0000-RED-BASELINE.md` — immutable historical red record; no synthetic code patch.
- `patches/0001-PROBE-per-path-relin2-execution-oracle.patch` — probe-only patch.
- `patches/0002-GREEN-separate-execution-certificate.patch` — conditional green candidate, applied after 0001.
- `candidate/tests/mult2_e2e_oracle_test.cpp` — final file after both patches.
- `evidence/` — retained Linux/Windows logs, original source manifest, and this review's identity/static checks.
- `SHA256SUMS` — content hashes for every package file except `SHA256SUMS` itself.

## Patch application

Run from the root of an exact checkout of project commit `bda879104c8a8b1ba6ac9301385b5b1919bef440`:

```bash
git apply --check --whitespace=error-all \
  patches/0001-PROBE-per-path-relin2-execution-oracle.patch
git apply patches/0001-PROBE-per-path-relin2-execution-oracle.patch
```

Build and run the probe on the required hosted Linux and Windows environments. Do **not** apply 0002 merely to make the current red disappear. Apply 0002 only if the new coefficientwise path identity passes, the old failure is reproduced or otherwise explained by the measured additivity residual, and the exact Tensor2/Relin2/RS2 regressions stay green:

```bash
git apply --check --whitespace=error-all \
  patches/0002-GREEN-separate-execution-certificate.patch
git apply patches/0002-GREEN-separate-execution-certificate.patch
```

The complete hosted commands and stop conditions are in `TEST-PROTOCOL.md`.

## Review-environment status

Archive verification, source/PDF inspection, exact integer calculations, patch construction, ordered patch application checks, whitespace checks, and final-byte comparison were executed. No exact OpenFHE build was available here. All CMake builds, executable runs, CTest runs, cryptographic operations, and hosted CI runs for the candidate are therefore **NOT EXECUTED**. The retained CI files are supplied evidence, not executions performed by this reviewer.
