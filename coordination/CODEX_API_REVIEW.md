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
- **Observed:** `NativeVector::SwitchModulus` treats residues strictly greater than half the old odd modulus as negative before mapping them into the new modulus (`src/core/lib/math/hal/intnat/mubintvecnat.cpp:98-122`). This matches the paper's centered interval `(-q/2, q/2]` for the odd RNS primes used here.
- **Inferred from the observed precomputation:** for retained product `Q_l`, divisor `q_div`, and retained prime `q_i`, OpenFHE stores `q_div^-1 mod q_i` and `((Q_l * (Q_l^-1 mod q_div) - 1) / q_div) mod q_i` (`src/pke/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp:64-84`). The latter is `-q_div^-1 mod q_i`. Consequently `DropLastElementAndScale` computes `(x-r)/q_div mod q_i`, where `r` is the centered last-tower residue. Constructing low as `x - q_div*high` on the retained prefix therefore yields `r mod q_i`. This is the source-level algebra behind the candidate DCP, but the independent CRT boundary test remains mandatory.
- **Observed:** a `DCRTPoly` can be constructed from an ordered vector of native polynomial towers, with parameters derived in the same order (`src/core/include/lattice/hal/default/dcrtpoly-impl.h:109-123`). This provides a project-owned way to append the required zero `q_div` tower without changing OpenFHE.
- **Observed:** `SchemeBase::KeySwitchCore` is exposed through the scheme wrapper and delegates to the configured key-switch implementation (`src/pke/include/schemebase/base-scheme.h:288-329`). Standard multiplication itself uses this interface on the third component (`src/pke/lib/schemebase/base-leveledshe.cpp:181-198`).
- **Observed:** `CryptoContextImpl::EvalMultNoRelin` is a public raw-tensor entry point. It type-checks its operands and delegates to the scheme multiplication without relinearization; the adjacent public `EvalMultNoRelinNoCheck` shows the exact two-by-two three-component convolution and cloned metadata update (`src/pke/include/cryptocontext.h:1958-2014`). Under `FIXEDMANUAL`, the scheme only aligns operand levels before `EvalMultCore` (`src/pke/lib/schemerns/rns-leveledshe.cpp:178-190,478-486`).
- **Inferred:** after strict pair validation, Tensor2 can compose three calls to public `EvalMultNoRelin` and one ciphertext addition, then replace the ordinary product metadata with the paper-derived pair logical metadata. This is smaller and less coupled than copying the protected `EvalMultCore` implementation.
- **Observed:** public `CryptoContextImpl::EvalMultNoCheck(ciphertext, NativeInteger)` clones a ciphertext and multiplies every `DCRTPoly` component directly by the integer, without encoding a CKKS scalar or changing metadata (`src/pke/include/cryptocontext.h:2077-2084`).
- **Inferred:** after the module's own exact validation, this is the appropriate KISS primitive for every ring multiplication by `q_div`; public CKKS `EvalMult(ciphertext, double)` is not.
- **Observed:** HYBRID decomposition chooses precomputed complementary-basis tables using the current tower count and ordered part indexes; its fast core maps evaluation-key tower indexes using `delta = fullSize - currentSize` (`src/pke/lib/keyswitch/keyswitch-hybrid.cpp:314-376,402-430`).
- **Observed:** BV similarly truncates evaluation-key polynomials by dropping their last `diffQl` towers (`src/pke/lib/keyswitch/keyswitch-bv.cpp:245-277`).
- **Inferred:** both HYBRID and BV safely support a current ciphertext basis only when it is the exact ordered prefix expected by the full evaluation key. Matching tower count alone is insufficient.
- **Observed:** CKKS `ModReduceInternalInPlace` changes noise-scale degree, level, scaling factor, and every DCRT element together (`src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp:164-191`).
- **Inferred:** DCP cannot blindly call ciphertext-level ModReduce because pair decomposition needs the element quotient/remainder identity while preserving pair logical-scale semantics explicitly.
- **Observed:** under `FIXEDMANUAL`, `GetScalingFactorReal` returns the fixed approximate factor `2^p` (`src/pke/include/schemerns/rns-cryptoparameters.h:598-618`). CKKS encoding first scales by that factor, multiplies by one more CRT copy when `noiseScaleDeg == 2`, and records `scalingFactor = pow(scalingFactor, noiseScaleDeg)` (`src/pke/lib/encoding/ckkspackedencoding.cpp:115-337`).
- **Observed:** the public `MakeCKKSPackedPlaintext` accepts `noiseScaleDeg`, and the encryption wrapper copies the plaintext's noise-scale degree and scaling factor to the ciphertext (`src/pke/include/cryptocontext.h:1161-1207,1241-1301`). Therefore a fresh `FIXEDMANUAL` plaintext encoded with `noiseScaleDeg == 2` has an actual and recorded scale of `2^(2p)`; this is not merely a metadata override.
- **Observed:** the non-first `FIXEDMANUAL` RNS primes are generated around a common `p`-bit prime, while the first modulus is handled separately (`src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp:415-517`). Hence adjacent `p`-bit towers chosen as `q_l` and `q_div` satisfy `q_l * q_div approximately 2^(2p)`, but neither factor nor their product is exactly `2^p` or `2^(2p)`.
- **Inferred:** the initial paper-scale requirement can be represented without mutating ciphertext metadata: encode and encrypt at `noiseScaleDeg == 2`, then assert that the measured ratio `2^(2p) / (q_l * q_div)` is within the test parameter's explicit tolerance. The implementation and tests must say “approximately,” not claim exact scale equality.

## Candidate logical metadata schedule

**Inferred.** Individual high and low ciphertexts are not independently meaningful CKKS encodings: only `RCB(high, low)` has the pair's CKKS scale. The pair module should therefore own the paper/logical scale state. Separately, it must keep both component ciphertexts on the same OpenFHE recorded scale so cloning and final RCB output remain well formed. These quantities are close but not identical under `FIXEDMANUAL`:

| Operation | Ordered basis | level | paper/logical scale | OpenFHE recorded scale | degree |
| --- | --- | ---: | --- | --- | ---: |
| fresh input | `[q0, ..., q_l, q_div]` | 0 | `Delta approximately 2^(2p)` | `2^(2p)` | 2 |
| DCP | `[q0, ..., q_l]` | 1 | recombined pair: `Delta`; high quotient component: approximately `Delta / q_div` | `2^(2p)` | 2 |
| Tensor2 | `[q0, ..., q_l]` | 1 | `S1 * S2 / q_div` | `SF1 * SF2 / 2^p` | 3 |
| Relin2 | `[q0, ..., q_l]` | 1 | unchanged | unchanged | 3 |
| RS2 / Mult2 | `[q0, ..., q_(l-1)]` | 2 | `S1 * S2 / (q_div * q_l)` | `SF1 * SF2 / 2^(2p)` | 2 |

Here `S` denotes the mathematical scale of a recombined pair and `SF` denotes OpenFHE's recorded approximate scale. The integer division uses the actual RNS primes, while `FIXEDMANUAL` decoding tracks powers of `2^p`. The difference is intentional CKKS approximation and must be measured, not erased by writing the actual prime quotient into only a metadata field. Tests must assert both the symbolic prime transition and the OpenFHE metadata transition.

For the implemented DCP slice, `PaperScaleDescriptor::approximateLogicalScalingFactor` records the high quotient component's approximate `SF / q_div` value; it is not the logical scale of the recombined pair. The pair represented by `q_div * high + low` still carries the input logical scale `Delta`, while both stored OpenFHE ciphertext components deliberately retain the same recorded scale metadata as the input.

**Observed risk.** In the default single-prime generator, the last scaling prime is the initial `p`-bit prime and the immediately preceding one is its previous admissible prime. If those two towers are used without reordering, then `q_l < q_div`, whereas the paper's modulus-consumption discussion recommends `q_l` slightly larger than `q_div`. Theorem 4.8 does not require that ordering, but the selected parameter vector must either justify the near-unity reversed ratio under its executable error bound or construct a supported ordering with `q_l >= q_div`; tests may not silently assume the recommendation is met.

## Explicit next-multiplication boundary

**Inferred.** After the first RS2, the pair basis is `[q0, ..., q_(l-1)]`. A second Relin2 would need `[q0, ..., q_(l-1), q_div]`. That is not an ordered prefix of the original full basis `[q0, ..., q_l, q_div]`. Existing HYBRID/BV key-switch table selection can therefore address the wrong prime residues even when the tower count looks plausible.

The accepted first slice should fail fast on a second Mult2 attempt and test that boundary. Supporting repeated Mult2 would require separately justified per-level contexts/evaluation keys or a small upstream basis-aware key-switch extension; neither belongs in the first YAGNI slice.

## Pending proof obligations

1. Verify with an independent signed big-integer/CRT oracle that OpenFHE's last-tower division matches the paper's nearest-integer quotient and centered remainder at tie and negative cases.
2. Determine and assert the exact OpenFHE ciphertext metadata changes for DCP, Tensor2, Relin2, and RS2 rather than relying only on DCRT tower counts.
3. Verify the zero-tower modulus raise and both full/prefix key-switch calls under the configured HYBRID and/or BV technique on an actual runner.
4. Derive the executable precision threshold directly from Theorem 4.8 and the selected parameter vector.

No build, encryption, decryption, or test has been run for these conclusions yet.
