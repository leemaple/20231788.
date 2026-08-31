# Codex paper/OpenFHE API review

Reviewed: 2026-08-31

Inputs are limited to the user-supplied paper and official pristine OpenFHE 1.5.0 at commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`. No prior 2023/1788 implementation or locally modified OpenFHE tree was consulted.

This is a design review, not implementation or test evidence. Statements are classified as **observed**, **inferred**, or **pending**.

## Paper identities

**Observed.** For a pair `CT = (high, low)`, paper Definitions 3.1/3.3 and 4.1/4.3/4.5/4.7 require:

- `RCB(CT) = q_div * high + low`.
- `Tensor2((h1,l1),(h2,l2)) = (h1 tensor h2, h1 tensor l2 + l1 tensor h2)`; the `l1 tensor l2` term is deliberately discarded.
- `Relin2(high, low) = DCP(Relin(q_div * high)) + (0, Relin(low))`.
- `RS2(high, low) = (RS(high), RS(q_div * high + low) - q_div * RS(high))`.
- `Mult2 = RS2 o Relin2 o Tensor2`.

Theorem 4.8 supplies the correctness precondition and error bound; a passing decryption alone is not evidence that the modulus/scale lifecycle matches the construction.

## Candidate first-multiplication basis schedule

**Inferred.** A one-multiplication implementation can use the full OpenFHE ciphertext basis in this order:

1. Full input basis: `[q0, ..., q_l, q_div]`, where the actual last OpenFHE tower is the divisor.
2. DCP drops and scales by the last tower, producing both pair members on the ordered prefix `[q0, ..., q_l]`.
3. Tensor2 remains on `[q0, ..., q_l]` and creates two three-component ciphertexts.
4. For `Relin(q_div * high)`, multiply every existing high tower by `q_div` and append a zero tower for `q_div`. This is exact because `q_div * high` is zero modulo the newly appended `q_div` tower. The resulting basis is again the original full ordered prefix `[q0, ..., q_l, q_div]`.
5. Relinearize the raised high part with the full evaluation key, then DCP its two components back to `[q0, ..., q_l]`. Relinearize the low part directly on that ordered prefix.
6. RS2 drops the then-last tower `q_l`, producing `[q0, ..., q_(l-1)]`.

This schedule must be rejected if the supplied basis, tower order, evaluation key, ciphertext key tag, component count, format, level, or scale does not match exactly.

## Relevant OpenFHE behavior

- **Observed:** `DCRTPoly::DropLastElementAndScale` converts the last tower to coefficient form, removes it, and applies the precomputed division/rounding transform to the remaining towers (`src/core/include/lattice/hal/default/dcrtpoly-impl.h:691-711`).
- **Observed:** a `DCRTPoly` can be constructed from an ordered vector of native polynomial towers, with parameters derived in the same order (`src/core/include/lattice/hal/default/dcrtpoly-impl.h:109-123`). This provides a project-owned way to append the required zero `q_div` tower without changing OpenFHE.
- **Observed:** `SchemeBase::KeySwitchCore` is exposed through the scheme wrapper and delegates to the configured key-switch implementation (`src/pke/include/schemebase/base-scheme.h:288-329`). Standard multiplication itself uses this interface on the third component (`src/pke/lib/schemebase/base-leveledshe.cpp:181-198`).
- **Observed:** HYBRID decomposition chooses precomputed complementary-basis tables using the current tower count and ordered part indexes; its fast core maps evaluation-key tower indexes using `delta = fullSize - currentSize` (`src/pke/lib/keyswitch/keyswitch-hybrid.cpp:314-376,402-430`).
- **Observed:** BV similarly truncates evaluation-key polynomials by dropping their last `diffQl` towers (`src/pke/lib/keyswitch/keyswitch-bv.cpp:245-277`).
- **Inferred:** both HYBRID and BV safely support a current ciphertext basis only when it is the exact ordered prefix expected by the full evaluation key. Matching tower count alone is insufficient.
- **Observed:** CKKS `ModReduceInternalInPlace` changes noise-scale degree, level, scaling factor, and every DCRT element together (`src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp:164-191`).
- **Inferred:** DCP cannot blindly call ciphertext-level ModReduce because pair decomposition needs the element quotient/remainder identity while preserving pair logical-scale semantics explicitly.

## Explicit next-multiplication boundary

**Inferred.** After the first RS2, the pair basis is `[q0, ..., q_(l-1)]`. A second Relin2 would need `[q0, ..., q_(l-1), q_div]`. That is not an ordered prefix of the original full basis `[q0, ..., q_l, q_div]`. Existing HYBRID/BV key-switch table selection can therefore address the wrong prime residues even when the tower count looks plausible.

The accepted first slice should fail fast on a second Mult2 attempt and test that boundary. Supporting repeated Mult2 would require separately justified per-level contexts/evaluation keys or a small upstream basis-aware key-switch extension; neither belongs in the first YAGNI slice.

## Pending proof obligations

1. Verify with an independent signed big-integer/CRT oracle that OpenFHE's last-tower division matches the paper's nearest-integer quotient and centered remainder at tie and negative cases.
2. Determine and assert the exact OpenFHE ciphertext metadata changes for DCP, Tensor2, Relin2, and RS2 rather than relying only on DCRT tower counts.
3. Prove how a paper-scale `Delta` approximately equal to `q_div * q_l` is represented by OpenFHE 1.5.0 encoding/scaling metadata under the chosen scaling technique.
4. Verify the zero-tower modulus raise and both full/prefix key-switch calls under the configured HYBRID and/or BV technique on an actual runner.
5. Derive the executable precision threshold directly from Theorem 4.8 and the selected parameter vector.

No build, encryption, decryption, or test has been run for these conclusions yet.
