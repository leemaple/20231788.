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
`87c84b879c13b55cf15d6559d3317853228fdc05`. Its last production-code change is
slot-metadata hardening commit
`4971d2292b5af0ddbbe0c7dbe5a2e87f45102ff1`; the exact base additionally closes
the final DCP/RCB review's immutability-test gap with whole-observable-state
comparisons plus independent deep metadata-map snapshots and changes no
production source. The exact base passed
GitHub Actions run
`https://github.com/leemaple/20231788./actions/runs/33411494861` on Linux/GCC and
Windows 2022/MSYS2 MinGW64, with a strict build and 1/1 CTest on both jobs. The
source package contains no `.git`, so treat these commit/run identities as a
binding manifest and independently report any inconsistency you observe.

## Input-package verification gate

The dispatched ZIP must include `HANDOFF_CONTENTS.md` and `MANIFEST.sha256`.
The handoff record supplied with the attachment must state the final ZIP byte
size, SHA-256, central-directory entry count, staged and extracted secret-scan
tool/version/results, archive integrity result, targeted exclusion result, and
extracted-tree equality result. Before reviewing or patching, verify the task,
source, paper, OpenFHE, and prior-review identities against those records. If a
required identity is absent or inconsistent, return `blocked`; do not guess.

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

Before drafting any scale test or production patch, prove or reject the scale
contract in `REVIEW.md`. For each input pair, let `H_i` be its independently
recorded DCP high-component logical scale and let `R_i` be the independently
recorded scale of the value reconstructed by `q_div * high_i + low_i`. The two
paper expectations must be represented and tested separately:

- high-output scale: `H_out = H_1 * H_2`;
- recombined Tensor2 scale: `R_out = R_1 * R_2 / q_div`.

Expected test values must be calculated from the two input manifests and
`q_div`, never read back from the result. For fresh FIXEDMANUAL inputs with
OpenFHE recorded factors `SF1` and `SF2`, the current candidate OpenFHE metadata
transition is `SF1 * SF2 / baseSF` with noise-scale degree 3, where
`baseSF = CryptoParametersCKKSRNS::GetScalingFactorReal(0)`. This candidate is
not yet an acceptance fact: prove it from the paper and supplied OpenFHE source
before encoding it in tests. If it cannot be proved, return `blocked` with the
exact ambiguity and do not write scale or production patches. Do not modify the
existing DCP descriptor merely to force the candidate to pass, and do not
silently equate the actual prime `q_div` with `baseSF`.

The current `PaperScaleDescriptor` was sufficient for the DCP/RCB slice but its
single approximate field names only the DCP high-component scale. Inspect that
contract explicitly. Freeze the existing DCP descriptor and public API in this
slice. Represent `H_out` and `R_out` only with the smallest clear Tensor-result
descriptor. If a correct Tensor2 design genuinely requires changing the DCP
descriptor/API, return `blocked` with the exact proof and wait for approval;
do not reopen the already reviewed DCP contract inside this task. Preserve the
exact DCP/RCB arithmetic, API, tests, and all previously enforced invariants.

Do not implement pair Add/Sub, Relin2, RS2, Mult2, refresh, a second
multiplication, serialization, tuple length greater than two, performance work,
compatibility layers, or speculative extension points.

## Required test-first deliverables

Return one ZIP. Patches are the only source of truth and must apply in the exact
order below to base commit `87c84b879c13b55cf15d6559d3317853228fdc05`.
Do not include competing full-file replacements.

1. `REVIEW.md`: equation-to-code mapping, the scale proof or exact blocker,
   observed OpenFHE behavior, metadata contract, assumptions, and unresolved
   questions. If the scale contract is blocked, return only review/evidence and
   no speculative code.
2. `01-red-api.patch`: build/workflow registration plus one compile-only public
   API contract test. Its expected red is the missing `TensorCiphertextPair`
   and `DoubleCKKS::Tensor2` seam. It must contain no production code.
3. `02-api-scaffold.patch`: final public declarations and the smallest temporary
   implementation that immediately throws
   `std::logic_error("DoubleCKKS: Tensor2 is not implemented")` before reading a
   ciphertext element or invoking OpenFHE arithmetic. It exists only to make
   behavioral tests compile and is explicitly non-mergeable, not a behavioral
   green or a fallback. It must never return a partial or knowingly incorrect
   public result.
4. `03-red-tensor2-contract.patch`: the complete public-seam Tensor2 tests. Add
   separate named CTest cases (or an aggregate runner that executes and reports
   every case despite failures) for valid arithmetic/immutability, result and
   scale contract, right-input validation, and mutual compatibility. Applied
   after patches 01-02, all cases must compile, execute, and be observed red;
   the log must prove no first failure masked the other cases. Valid-operation
   cases must fail on the project scaffold, and negative cases must reject its
   `logic_error` as the wrong exception type/diagnostic.
5. `04-green-tensor2.patch`: one complete production implementation that
   removes the scaffold and, before every raw element access or OpenFHE
   arithmetic, fully validates both pairs and their mutual compatibility. It
   must return simultaneously correct high and low three-component members and
   a complete proved result/scale manifest. No public intermediate state may
   contain a placeholder member, missing validation, or speculative metadata.
6. `05-final-docs.patch`: final design/API documentation only after all Tensor2
   and existing DCP/RCB cases are green.
7. `TESTS.md`: for every patch boundary, list `git apply --check`, configure,
    build, and CTest commands actually run, environment, exit status, exact
    failing assertion or compiler diagnostic, test counts, and all unrun work.
    Do not convert inspection into an execution claim.

The compile red must be observed before the scaffold, and the complete runtime
red must be observed before the implementation. Never call the scaffold green,
never let one compiler/runtime failure stand in for unexecuted cases, never
weaken an oracle between red and green, and never expose a knowingly invalid
Tensor result merely to manufacture another red.

## Tests that must be drafted

The tests must exercise the public Tensor2 seam and must not call
production-private tensor helpers or use `EvalMultNoRelin` to calculate the
expected answer. Test-only adversarial mutation may use only the existing
read-only public getters, as current DCP/RCB tests do; do not add friend access,
a production setter, a test-only constructor, or any other production backdoor.

- Build independently chosen two-component pairs using the existing public DCP
  path and deterministic coefficient fixtures.
- Implement a Boost `cpp_int` schoolbook negacyclic convolution oracle in
  `Z_q[X]/(X^N+1)` for every RLWE component, coefficient, and active RNS tower.
- Include an explicit wraparound witness equivalent to
  `X^(N-1) * X = -1` and a signed product that crosses at least one active tower
  modulus. Low-degree positive-only fixtures that cannot distinguish ordinary
  convolution from negacyclic convolution are insufficient.
- Check all three high output components against `h1 tensor h2` and all three
  low output components against `h1 tensor l2 + l1 tensor h2`.
- Choose low inputs whose independent `l1 tensor l2` is nonzero modulo at least
  one named active tower at one named component/coefficient. Record that witness
  and assert the output is the cross term and is not cross-plus-low-low, so
  omission is proved rather than assumed.
- Check exact unchanged ordered basis, level 1, evaluation format, context,
  CKKS encoding, key tag, exactly three components per member, the proved
  FIXEDMANUAL recorded degree/factor transition, divisor, result type, and both
  independently calculated `H_out` and `R_out` paper-scale transitions. The
  `TensorCiphertextPair` type itself represents this state; do not add a
  lifecycle enum or mirrored state flag in this slice.
- Check both inputs are completely unchanged.
- Prove Tensor2 reuses the already tested complete `ValidatePair` path for both
  operands with one right-input manifest corruption. Do not duplicate every
  existing DCP/RCB invariant test. Separately construct two individually valid
  public DCP results with incompatible slots and require fail-fast mutual
  rejection before multiplication. An invalid lifecycle test is forbidden in
  this slice because only `ReadyForFirstMult` is constructible and the getter
  returns by value; defer it until a second valid lifecycle exists.
- Both negative cases must require the project's `std::invalid_argument`, the
  stable `DoubleCKKS: ` diagnostic prefix, and the expected field-specific
  message. Any `std::logic_error`, generic exception, or downstream OpenFHE
  `TypeCheck` failure is a test failure; this proves rejection occurred in the
  project before raw arithmetic.
- Keep every existing DCP/RCB test passing without weakening its oracle or
  diagnostic attribution.

Use explicit deterministic coefficient vectors and record any OpenFHE RNG that
cannot be controlled; do not claim global determinism merely by saying “fixed
seed.” Keep the ring dimension small. The first test/build patch must add
`agent/codex-tensor2-01` to the existing workflow trigger. The Mac is not the
sustained build runner; Codex will preserve each Linux red and run the final
exact commit on both GitHub Actions Linux/GCC and Windows 2022/MSYS2 MinGW64.

## Exact downstream verification contract

The existing workflow is the command authority. Every Linux red and green must
record the exact source commit, OpenFHE commit, `ubuntu-24.04`, compiler/CMake
versions, commands, exit code, test count, and Actions URL. With pristine
OpenFHE installed at `${OPENFHE_PREFIX}`, the project commands are exactly:

```sh
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=${OPENFHE_PREFIX}
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

The final exact commit must also run on `windows-2022` with MSYS2 `MINGW64`,
`mingw-w64-x86_64-gcc`, `mingw-w64-x86_64-cmake`, Boost, Make, and the exact
workflow paths. After building/installing official OpenFHE `df495ba...`, its
project commands are exactly:

```sh
: "${OPENFHE_PREFIX:?OPENFHE_PREFIX must name the installed official OpenFHE tree}"
PROJECT_BUILD="$PWD/build"
prefix="$(cygpath -u "$OPENFHE_PREFIX")"
cmake -S . -B "$PROJECT_BUILD" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$prefix"
cmake --build "$PROJECT_BUILD" --parallel 2
export PATH="$prefix/bin:$prefix/lib:$PATH"
ctest --test-dir "$PROJECT_BUILD" --output-on-failure
```

The Linux and Windows final results must bind the same project SHA. Red Windows
runs are unnecessary once the intended Linux red is retained. Any unexecuted
platform or command must be marked `pending`, never inferred from another run.

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

Codex must be able to apply every numbered patch in order to the exact base.
The API patch must produce the named compile red; the scaffold must fail before
all raw access; the contract patch must produce independently reported runtime
reds for every named case; and only the complete implementation may make the
Tensor2 contract green. The final ordered tree must obtain:

- the independent coefficient/tower oracle green for both Tensor2 members;
- explicit negacyclic wraparound, signed modular wrap, and nonzero-low-low
  omission witnesses;
- separately derived and asserted `H_out` and `R_out` paper-scale values plus a
  source-proved OpenFHE metadata transition;
- complete metadata, validation, immutability, and DCP/RCB regression coverage;
- module-attributed right-input and mutual-compatibility failures observed
  before any OpenFHE arithmetic or downstream `TypeCheck` exception;
- strict C++17 warning-clean Linux and official OpenFHE Windows/MinGW64 builds;
- all CTest entries green on a single exact commit;
- no upstream change, hidden dependency, credentials, unsupported claim, or
  implementation outside this bounded slice;
- no new lifecycle enum, private-state test backdoor, duplicate full-file
  deliverable, partial public Tensor result, or future-operation scaffold.

Lead your response with a bounded verdict: `ready to apply`, `changes needed`,
or `blocked`, followed by the exact reason. Return the ZIP once, and do not ask
Codex to infer omitted context from another conversation.
