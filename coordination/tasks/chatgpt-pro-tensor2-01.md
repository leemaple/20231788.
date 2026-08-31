# ChatGPT Pro 01 — clean-room Tensor2 vertical slice

Prepared: 2026-08-31 Asia/Shanghai

## Background and objective

Independently design and draft the smallest test-driven C++17 implementation of
the paper's `t=2` pair-tensor operation for a separate consumer of official,
pristine OpenFHE 1.5.0. This is an algorithm task, not a network-security task.

The supplied project is a clean-room reimplementation. Review and modify only
the supplied greenfield project, the user-supplied IACR ePrint 2023/1788 paper,
and the supplied official OpenFHE 1.5.0 source. Do not seek, inspect, infer, or
reuse any previous 2023/1788 implementation.

The exact project base is branch `agent/codex-tensor2-01`, commit
`a9507d1031e1bbed38bdb3d856539c29d9539772`. Its last production/test change is
slot-metadata hardening commit
`4971d2292b5af0ddbbe0c7dbe5a2e87f45102ff1`; the later commits retain its
red/green evidence and current documentation. That production commit passed
GitHub Actions run
`https://github.com/leemaple/20231788./actions/runs/33404816846` on Linux/GCC and
Windows 2022/MSYS2 MinGW64, with a strict build and 1/1 CTest on both jobs. The
source package contains no `.git`, so treat these commit/run identities as a
binding manifest and independently report any inconsistency you observe.

## Authoritative algorithm and source anchors

Use the paper as mathematical authority and OpenFHE only as the implementation
platform. In the supplied paper:

- Section 4, fixed-point identity: for
  `m_i = q_div * high_i + low_i`, discard `low_1 * low_2` and retain
  `(high_1 * high_2, high_1 * low_2 + low_1 * high_2)`.
- Definition 4.1, Pair tensor:
  `Tensor2((h1,l1),(h2,l2)) = (h1 tensor h2,
  h1 tensor l2 + l1 tensor h2)` in two three-component RLWE ciphertexts over
  the unchanged modulus `Q_l`.
- Lemma 4.2: the ordinary tensor equals `q_div` times the recombined Tensor2
  result plus the deliberately omitted decrypted low-low term.

Relevant official OpenFHE 1.5.0 source is pinned at commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`:

- `src/pke/include/cryptocontext.h`, public `EvalMultNoRelin` and the adjacent
  `EvalMultNoRelinNoCheck` convolution/metadata implementation;
- `src/pke/lib/schemerns/rns-leveledshe.cpp`, `LeveledSHERNS::EvalMult` and
  `AdjustForMultInPlace` under `FIXEDMANUAL`;
- `src/pke/lib/schemebase/base-leveledshe.cpp`, `EvalMultCore`;
- `src/pke/include/schemerns/rns-cryptoparameters.h`, the fixed real scaling
  factor API.

Distinguish facts observed in those sources from design inferences.

## Current architecture and boundaries

- `DoubleCKKS` is bound to one exact `CryptoContext<DCRTPoly>` and is the only
  constructor of valid pair values.
- `DCP` accepts a fresh level-zero, noise-scale-degree-two, evaluation-format
  CKKS ciphertext on `[q0, ..., q_l, q_div]`; it returns a read-only
  `CiphertextPair` on `[q0, ..., q_l]`, level 1, lifecycle
  `ReadyForFirstMult`, with two RLWE components in each member.
- `RCB(pair) = q_div * high + low`. It validates the complete pair before raw
  access and does not mutate it.
- The module validates exact context identity, encoding type, key tag, ordered
  RNS basis and tower parameters, level, scale metadata, noise-scale degree,
  format, component count, divisor, and its own pair manifest.
- The accepted DCP implementation derives local last-tower division factors and
  calls pristine `DCRTPoly::DropLastElementAndScale`; it must not regain a
  dependency on unchecked outer OpenFHE precomputation rows.
- Do not weaken validation, expose mutable pair construction, modify upstream
  OpenFHE, or change the working modulus in Tensor2.

Use a distinct read-only `TensorCiphertextPair` result type so three-component
Tensor2 output cannot accidentally enter an API that requires two components.
Keep its constructor private to `DoubleCKKS`. It must expose only the invariant
facts callers/tests need, in the same spirit as `CiphertextPair`.

## Scope to research and modify

Implement only `DoubleCKKS::Tensor2(const CiphertextPair& left,
const CiphertextPair& right) const` and the minimal result type/validation and
tests it requires.

The intended smallest arithmetic composition, subject to your independent
source verification, is:

1. validate both inputs completely and validate their mutual compatibility;
2. `high3 = EvalMultNoRelin(left.high, right.high)`;
3. `low3 = EvalMultNoRelin(left.high, right.low) +
   EvalMultNoRelin(left.low, right.high)`;
4. omit `EvalMultNoRelin(left.low, right.low)` entirely;
5. normalize only the result metadata required by the paper/OpenFHE dual scale
   contract; do not consume a tower or alter polynomial coefficients for scale
   normalization.

For fresh FIXEDMANUAL inputs with recorded factors `SF1` and `SF2`, verify the
candidate recorded transition `SF1 * SF2 / baseSF` and noise-scale degree 3,
where `baseSF = CryptoParametersCKKSRNS::GetScalingFactorReal(0)`. Separately
track the paper transition for the recombined pair:
`S_out = S1 * S2 / q_div`. Do not silently equate the actual prime `q_div` with
`baseSF`.

The current `PaperScaleDescriptor` was sufficient for the DCP/RCB slice but its
single approximate field may be ambiguous between a high-component scale and a
recombined-pair scale. Inspect that contract explicitly. If it cannot represent
both the existing DCP state and the Tensor2 transition without ambiguity, make
the smallest clear correction and update the DCP tests/documentation. Preserve
the exact DCP/RCB arithmetic and all previously enforced invariants.

Do not implement pair Add/Sub, Relin2, RS2, Mult2, refresh, a second
multiplication, serialization, tuple length greater than two, performance work,
compatibility layers, or speculative extension points.

## Required test-first deliverables

Return one ZIP with these independently usable artifacts:

1. `REVIEW.md`: equation-to-code mapping, observed OpenFHE behavior, metadata
   contract, assumptions, and unresolved questions.
2. `01-red-tests.patch`: tests/build registration only, applicable to exact base
   commit `a9507d1031e1bbed38bdb3d856539c29d9539772` and expected to fail because
   Tensor2 is missing. State the expected failure precisely.
3. `02-implementation.patch`: the minimum header/source/documentation changes
   that make those tests pass when applied after patch 01.
4. Complete replacement files as a fallback if a patch is not reliably
   applicable; clearly label them and keep tests separate from production code.
5. `TESTS.md`: exact commands you actually ran, environment, exit status, test
   counts, and any test not run. Do not convert inspection into an execution
   claim.

Do not combine the red test and production implementation into one patch.

## Tests that must be drafted

The tests must be black-box tests of the public Tensor2 seam and must not call
production-private tensor helpers or use `EvalMultNoRelin` to calculate the
expected answer.

- Build independently chosen two-component pairs using the existing public DCP
  path and deterministic coefficient fixtures.
- Implement a Boost `cpp_int` schoolbook negacyclic convolution oracle in
  `Z_q[X]/(X^N+1)` for every RLWE component, coefficient, and active RNS tower.
- Check all three high output components against `h1 tensor h2` and all three
  low output components against `h1 tensor l2 + l1 tensor h2`.
- Choose low inputs whose independent `l1 tensor l2` is nonzero. Assert the
  output is the cross term and is not the cross term plus low-low, so omission
  is proved rather than assumed.
- Check exact unchanged ordered basis, level 1, evaluation format, context,
  CKKS encoding, key tag, exactly three components per member, noise-scale
  degree 3, FIXEDMANUAL recorded scale, divisor, result type/state, and the
  distinct paper/recombined scale transition.
- Check both inputs are completely unchanged.
- Fail fast before tensoring for a wrong context, key tag, ordered basis/tower
  parameters, level, lifecycle, encoding, format, component count, recorded
  scale, divisor, or inconsistent paper-scale manifest. Include at least one
  cross-input incompatibility case.
- Keep every existing DCP/RCB test passing without weakening its oracle or
  diagnostic attribution.

Fix all deterministic seeds and keep the ring dimension small. The Mac is not
the sustained build runner; Codex will run the accepted patches on GitHub
Actions and Windows.

## Prohibited operations and claims

- Do not access any old/local/private 2023/1788 implementation or any modified
  OpenFHE tree.
- Do not search for or copy the paper authors' proof-of-concept code.
- Do not change official OpenFHE source, use a fork, add a dependency, weaken
  C++17/warning settings, or replace FIXEDMANUAL.
- Do not add production `try`/`catch`; invariant failures should remain loud.
- Do not commit, push, open a PR, dispatch/rerun CI, use credentials, or inspect
  unrelated files.
- Do not perform or discuss network-security review; this scope is algorithm,
  numeric semantics, integration correctness, and tests.
- Do not claim a build, test, Windows result, precision result, performance
  result, or security result unless you actually executed it and provide the
  corresponding evidence. A proposed patch is not a passing test.
- Do not broaden the task when a concrete defect can be fixed locally.

## Acceptance criteria

Codex must be able to apply patch 01 alone to the exact base and obtain the
documented missing-behavior red, then apply patch 02 and obtain:

- the independent coefficient/tower oracle green for both Tensor2 members;
- explicit nonzero-low-low omission evidence;
- complete metadata, validation, immutability, and DCP/RCB regression coverage;
- strict C++17 warning-clean Linux and official OpenFHE Windows/MinGW64 builds;
- all CTest entries green on a single exact commit;
- no upstream change, hidden dependency, credentials, unsupported claim, or
  implementation outside this bounded slice.

Lead your response with a bounded verdict: `ready to apply`, `changes needed`,
or `blocked`, followed by the exact reason. Return the ZIP once, and do not ask
Codex to infer omitted context from another conversation.
