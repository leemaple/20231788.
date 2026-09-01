# Relin2 clean-room review and equation-to-code audit

Prepared: 2026-09-01 +08:00

## Verdict basis

No source/API blocker was found for the bounded `t=2` Relin2 slice. The input package gate passed byte-for-byte, the supplied pristine OpenFHE 1.5.0 exposes the required public composition seams, and the final local Linux implementation/test tree satisfies the bounded arithmetic, state, validation-order, key-preflight, and immutability contracts. Hosted Linux/Windows same-commit verification is deliberately not claimed here; it remains the downstream Codex gate required by the task.

## Input identity gate

Independently recomputed before source review or patching:

- dispatched ZIP SHA-256: `3e839a6b88a81107657442a2bb4f6b08385f6a24685cab11968db540436750f6`;
- external binding SHA-256: `3320efa8723f0c519da453a006617c328de5bfa2aca72392a6161c66a0489d2f`;
- standalone task SHA-256: `9323d631de4c6f6418ba1aa597332206a765846d0510a275df42a7a64b0a7513`;
- ZIP central-directory entries: 2,266; unsafe paths: 0;
- internal `HANDOFF_CONTENTS.md` SHA-256: `175900a8195e19023ef02316347a5272d8e00aef197962839a1d2f5fc80d629d`;
- internal `MANIFEST.sha256` SHA-256: `bb16e58b587c3ef964346e4b71213c62b5ed76ee5d82006af858787df5669ba4`;
- all 1,997 manifest records verified after fresh extraction;
- independently recomputed Git tree object for `cleanroom-project/`: `759d5195739684748d5a9664edabe3fa719e1acf`, matching the bound base tree for commit `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`;
- pristine OpenFHE identity record: `df495ba2e91739a6dc8f1de254fc5a41155ce504`;
- packaged paper, accepted Tensor2 evidence, preflight review, task audit, source-provenance diff/history, and tracked task hashes all matched the handoff records.

The package intentionally contains no `.git`; therefore this review verifies the exported tree identity and supplied binding/provenance records, not Git object ancestry beyond what those records can prove.

## Mathematical authority and equation-to-code mapping

Paper Definition 4.3 is treated as the mathematical authority:

`Relin2(CT) = DCP_qdiv(Relin(q_div * high3)) + (0, Relin(low3))`.

Writing `(u,v) = DCP_qdiv(Relin(q_div * high3))` and `w = Relin(low3)`, production returns exactly `(u, v+w)`.

Code mapping in final `src/double_ckks.cpp`:

1. `Relin2` starts with complete `ValidateTensorResult` and the Relin2-only active-basis precondition, then key preflight (lines 667-672).
2. The high member is cloned; each existing RNS tower is multiplied by integer `q_div`; an Evaluation-format zero final tower using the bound final tower parameters is appended; all three elements are installed before level is set to zero (674-687).
3. The raised high is completely validated against the full ordered basis, level 0, degree 3, factor `SF_T`, exact context/tag/slots/CKKS/Evaluation metadata (688-689, with full tower modulus/root/cyclotomic validation in `ValidateCiphertext`, 264-325).
4. Public OpenFHE `Relinearize` is invoked exactly once for raised high and exactly once for Tensor low (691-694).
5. Relinearized high is validated before the shared private DCP seam (696-698).
6. `DecomposeValidatedFullBasis` is the sole DCP arithmetic implementation. It derives last-tower factors locally and calls `DropLastElementAndScale` once per RLWE component in its single loop, then clones metadata, installs quotient/remainder elements, and sets level 1 (348-383). Public DCP calls this helper only after its existing DCP validation (386-404).
7. The private-DCP remainder and relinearized low are independently validated against the exact `Q_l` prefix and then compared for context, tag, slots, encoding, level, degree, and factor before addition (700-712). `ValidateCiphertext` separately enforces Evaluation format and exact tower modulus/root/cyclotomic parameters.
8. Only `v+w` is computed by `EvalAdd`; `u` is moved through unchanged (714-723).
9. The result is `ReadyForRS2`, level 1, two components per member, degree 3, factor `SF_T`, and copies Tensor high/recombined logical scales verbatim; complete pair validation occurs before return (718-725).
10. Public RCB validates either lifecycle before raw access and recombines `q_div*high + low` without mutating the pair (728 onward).

No `ModReduce`, `Rescale`, `EvalMultAndRelinearize`, floating CKKS scalar multiply, or project call to `KeySwitchCore` appears in production Relin2/DCP code.

Lemma 4.4 is asserted only as the exact per-residue recombination identity. No analytic precision/error bound is claimed or tested.

## Public API and lifecycle/state decision

The final seam is exactly one existing pair type plus one new state:

- `PairLifecycle::{ReadyForFirstMult, ReadyForRS2}`;
- `PaperScaleDescriptor` retains the original first three fields and appends `approximateRecombinedLogicalScalingFactor`;
- `CiphertextPair Relin2(const TensorCiphertextPair& tensor) const`.

`ValidatePair` explicitly rejects any unknown lifecycle, then branches expected degree/factor/scales by lifecycle (final source lines 407-459). `ReadyForFirstMult` remains level 1 / two components / degree 2 / fresh factor / exact `Q_l` prefix / high scale `SF/q_div` / recombined scale `SF`. `ReadyForRS2` is level 1 / two components / degree 3 / Tensor factor / same prefix / high and recombined scales equal to the Tensor transition values.

DCP initializes the appended recombined field directly from the fresh input recorded factor (391-397). Tensor2 computes its recombined output scale from each input's explicit `approximateRecombinedLogicalScalingFactor` field, not from `inputRecordedScalingFactor`; this is a source-order/equation-to-code proof because valid first-lifecycle values are numerically equal. Tensor2's final lifecycle guard is after complete pair/compatibility validation but before any ciphertext lvalue binding or multiplication (545-554).

### Stage table

| Stage | Ordered basis | Level | Components/member | Degree | Recorded factor |
|---|---|---:|---:|---:|---:|
| Tensor input | `Q_l` prefix | 1 | 3 | 3 | `SF_T` |
| raised high | complete `Q_l * q_div` | 0 | 3 | 3 | `SF_T` |
| relinearized high | complete basis | 0 | 2 | 3 | `SF_T` |
| private-DCP pair | `Q_l` prefix | 1 | 2 each | 3 | `SF_T` |
| relinearized low | `Q_l` prefix | 1 | 2 | 3 | `SF_T` |
| Relin2 output | `Q_l` prefix | 1 | 2 each | 3 | `SF_T` |

This is an engineering derivation tested by the public-seam suite, not paper wording.

## Pristine OpenFHE 1.5.0 observations

Observed directly in the supplied pristine tree, without modifying it:

- `cryptocontext.h` public output-returning `Relinearize` obtains the eval-mult vector by ciphertext key tag and dispatches to the scheme; the static all-key map is exposed by `GetAllEvalMultKeys`.
- `base-leveledshe.cpp` output-returning relinearization clones the ciphertext, switches all elements to Evaluation format, adds key-switch contributions from components 2+, and resizes to two components; it does not rewrite level/noise-degree/scaling-factor metadata.
- `evalkeyrelin.h` exposes concrete `EvalKeyRelinImpl<DCRTPoly>` A/B vectors through the public base API.
- `CryptoParametersRNS::GetNumPartQ()` and the HYBRID implementation show partition-indexed A/B use over the extended QP basis; `EvalFastKeySwitchCoreExt` indexes each A/B part against active `Q_l P` digits while mapping back to full-Q positions.
- `CryptoParametersRLWE::GetDigitSize()` and BV key-switch code use CRT decomposition for nonzero digit size, and consume A/B arrays by decomposition digit; level prefixes are obtained by dropping trailing key towers. This supports validating BV key internals against the complete Q key basis, not merely the level-one ciphertext prefix.
- `LeveledSHERNS::EvalAddInPlace` calls `AdjustForAddOrSubInPlace` for non-`NORESCALE` techniques, including FIXEDMANUAL; therefore project-owned exact compatibility must be checked before `EvalAdd` to prevent silent alignment.
- ciphertext setters/cloning and DCRTPoly construction from ordered NativePoly towers provide the needed metadata-preserving raise/decompose composition.

These are implementation-platform observations. The decision to preflight the key, restore the high basis, and reject pre-add mismatches is project design derived from the paper/task contract.

## Evaluation-key proof and validation order

Production key preflight uses `GetAllEvalMultKeys()` and `find(tag)` only. It never calls `operator[]`, never inserts/replaces/deletes production cache state, and validates only index zero. The exact order is missing row -> empty vector -> null index zero -> exact context -> exact actual key tag -> dynamic cast -> A/B getters -> technique-specific shapes/bases/formats (597-664).

HYBRID requires A/B length exactly `GetNumPartQ()` and each entry in Evaluation format on exact full `ParamsQP`. BV requires complete-Q digit count: `|Q|` for digit size 0, otherwise `sum_i ceil(MSB(q_i)/digitSize)`, and each entry in Evaluation format on exact full ordered Q. Unsupported key-switch techniques are rejected by a project-owned diagnostic. Extra later keys are allowed and deliberately never inspected.

Runtime adversarial tests exercised missing, empty, null-first, wrong-context, wrong-tag, wrong-subtype, HYBRID A/B length/basis/format, BV digit-size-zero A/B length/basis/format, BV positive-digit-size A/B length/basis/format, additional later valid key, and malformed/null later key. RAII restores the complete static map after each mutation, and tests snapshot the map and Tensor input to prove production did not mutate them.

Source order proves the fail-fast sequence that black-box tests cannot observe: complete Tensor validation at 668, active-basis check 669-671, complete key preflight 672, only then clone/raise at 674; public relinearization begins at 692. RCB's `ValidatePair` is the first statement at 729 before raw member access. Tensor2 validates both pairs and compatibility, then rejects non-`ReadyForFirstMult` at 549-552 before any multiplication.

## Independent arithmetic/oracle evidence

The valid public-seam case builds Tensor2 only through public DCP + Tensor2, generates the ordinary eval-mult key with the fixture secret key, independently raises the high path in test code, invokes the trusted public OpenFHE `Relinearize` primitive, and applies a Boost `cpp_int` centered CRT/DCP oracle. Every output member/component/tower/coefficient is compared to `(u,v+w)` modulo every active `Q_l` residue. The separate RCB reference checks `q_div*u + (v+w) = Relin(q_div*high3)+Relin(low3)` per residue after removing only the appended `q_div` tower.

Named witnesses from the executed final test binary:

- nonzero public key-switch contribution `K0/K1`: `component=0,tower=0,coefficient=0`;
- common nonzero DCP remainder `v` and low relinearization `w`: `component=0,tower=0,coefficient=0`;
- deterministic positive centered-remainder boundary: `component=0,coefficient=0`;
- deterministic negative centered-remainder boundary and quotient carry: `component=0,coefficient=1`.

The centered-boundary case uses a deterministic, valid-shaped BV digit-size-zero A/B fixture installed only through public test-side eval-key cache APIs and deterministic coefficient data. It does not search/regenerate random keys until a witness appears. OpenFHE public relinearization remains a trusted primitive; this suite does not claim independent verification of OpenFHE key-switch arithmetic itself.

## Assumptions and bounded non-claims

- The supplied source tree is treated as the exact exported base because its independently recomputed tree hash matches the binding; `.git` ancestry is outside what the package can independently prove.
- Static eval-key cache concurrent mutation is intentionally out of scope; no lock/retry/snapshot semantics were added in production.
- Windows/MSYS2 and hosted GitHub Actions verification of the final exact applied tree were not run by this work and remain `pending` downstream.
- No precision/error bound, performance result, serialization behavior, RS2, Mult2 wrapper, pair Add/Sub, rotations, refresh/bootstrapping, or repeated multiplication is claimed.
- No network-security work was performed.

## Blockers / unresolved questions

None for producing and applying this bounded patch series. Final hosted Linux/Windows same-commit success is an explicit downstream acceptance gate, not an executed result in this delivery.
