# ChatGPT Pro 01 — clean-room Relin2 vertical slice

Prepared: 2026-09-01 Asia/Shanghai

## Background and objective

Independently design and draft the smallest test-driven C++17 implementation of
the paper's first-multiplication `Relin2` transition for a separate consumer of
official, pristine OpenFHE 1.5.0. This is an algorithm and numeric-integration
task, not a network-security task.

The supplied project is a clean-room reimplementation. Review and modify only
the supplied greenfield project, the user-supplied IACR ePrint 2023/1788 paper,
the supplied official OpenFHE 1.5.0 source, and the supplied accepted-project
evidence. Do not seek, inspect, infer, or reuse any previous 2023/1788
implementation or author proof-of-concept.

The exact project source/test base is branch `agent/codex-relin2-01`, commit
`fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`, tree
`759d5195739684748d5a9664edabe3fa719e1acf`. Its only change after the accepted
Tensor2 implementation is a one-token project-owned DCP diagnostic correction;
the retained red proves the old wording and the exact-current hosted run proves
the corrected wording. GitHub Actions run
`https://github.com/leemaple/20231788./actions/runs/33436252725` passed the exact
commit on Linux/GCC and Windows 2022/MSYS2 MinGW64 with 6/6 CTests on both jobs.
The package contains no `.git`, so treat commit, tree, and run identities as a
binding manifest and independently report any inconsistency.

## Input-package verification gate

The dispatched ZIP must include `HANDOFF_CONTENTS.md` and `MANIFEST.sha256`.
The handoff record supplied with the attachment must state the final ZIP byte
size and SHA-256, central-directory entry count, staged and fresh-extraction
secret-scan tool/version/results, archive-integrity result, targeted-exclusion
result, path-safety result, manifest result, exact project Git-export equality,
and task-file identity. Before reviewing or patching, verify the task, source,
paper, pristine OpenFHE, accepted Tensor2, and retained evidence identities
against those records. If a required identity is absent or inconsistent,
return `blocked`; do not guess.

## Mathematical authority and exact bounded formula

Use the paper as mathematical authority and OpenFHE only as the implementation
platform. Definition 4.3, paper page 7 / extracted-text lines 664-674, starts
from the accepted Tensor2 result

```text
CT = (high3, low3) in R^3_Ql x R^3_Ql
```

and defines

```text
Relin2(CT) = DCP_qdiv(Relin(q_div * high3)) + (0, Relin(low3)).
```

If

```text
(u, v) = DCP_qdiv(Relin(q_div * high3))
w      = Relin(low3),
```

the exact output is the two-member ciphertext pair `(u, v + w)`, with two RLWE
components in each member over the unchanged working basis `Q_l`. Relin2 does
not consume a working tower.

The high path must temporarily represent `q_div * high3` over
`q_div * Q_l`, relinearize on that restored full basis, and privately DCP back
to `Q_l`. For this accepted first-lifecycle ordered RNS layout, the engineering
representation is: multiply every existing residue tower of every high
component by the integer `q_div`, then append the `q_div` residue as exactly
zero. The low path is publicly relinearized directly on `Q_l`.

The following are forbidden substitutions:

- independently relinearizing both paths only on `Q_l`;
- `DCP o Relin o RCB`, which consumes an extra `q_div` factor;
- `EvalMultAndRelinearize`, which performs another multiplication;
- `ModReduce`, `Rescale`, or a floating-point scalar multiply;
- direct `KeySwitchCore` access.

Lemma 4.4, paper pages 7-8 / extracted-text lines 688-783, supplies the exact
per-residue recombination identity modulo the active `Q_l` basis:

```text
RCB_qdiv(Relin2(CT))
  = Relin(q_div * high3) + Relin(low3)  (mod Q_l).
```

It does not justify an executable analytic error-bound claim unless the key-
switch error, secret-key Hamming weight, centered norms, and all hypotheses are
actually computed. Assert the exact identity; do not claim or test an unproved
precision/error bound.

Relin2 preserves both accepted Tensor2 logical-scale meanings:

```text
H_after_Relin2 = H_after_Tensor2
R_after_Relin2 = R_after_Tensor2.
```

This is a mathematical inference from the formula. Keep the integer prime
`q_div`, OpenFHE's real `baseSF`, and the future RS2 modulus `q_l` distinct.

## Authoritative OpenFHE 1.5.0 anchors

The supplied pristine OpenFHE source is pinned at commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Verify, rather than merely trust,
these observed anchors:

- `src/pke/include/cryptocontext.h` and
  `src/pke/lib/cryptocontext.cpp`: public `Relinearize`,
  `GetAllEvalMultKeys`, `GetEvalMultKeyVector`, `InsertEvalMultKey`, and the
  static global evaluation-key cache;
- `src/pke/include/key/evalkey.h` and `evalkeyrelin.h`: the base key API and
  concrete `EvalKeyRelinImpl<DCRTPoly>` A/B vectors;
- `src/pke/include/schemerns/rns-cryptoparameters.h`: key-switch technique and
  HYBRID partition count;
- `src/pke/include/schemebase/rlwe-cryptoparameters.h`: BV digit size;
- `src/pke/lib/keyswitch/keyswitch-hybrid.cpp` and `keyswitch-bv.cpp`: complete
  key shape/basis assumptions and active-prefix use;
- `src/pke/lib/schemebase/base-leveledshe.cpp`: output-returning
  relinearization clones the input, reduces three components to two, and does
  not normalize level/degree/factor metadata;
- `src/core/include/lattice/hal/default/dcrtpoly.h` and
  `dcrtpoly-impl.h`: construction from an ordered `NativePoly` vector;
- `src/pke/include/ciphertext.h` and `src/pke/lib/ciphertext-impl.cpp`:
  `SetElements`, `SetLevel`, and clone/metadata behavior;
- `src/pke/lib/schemerns/rns-leveledshe.cpp`: FIXEDMANUAL `EvalAdd` can align
  levels, so project-owned compatibility must reject a mismatch before add.

Separate observations from design inferences in `REVIEW.md`. Do not modify the
supplied pristine OpenFHE tree.

## Frozen current architecture and boundaries

- `DoubleCKKS` is bound to one exact `CryptoContext<DCRTPoly>` and is the only
  constructor of valid pair/Tensor values.
- Public DCP accepts only a fresh level-zero, degree-two, two-component CKKS
  ciphertext over the complete ordered basis `[q0, ..., q_l, q_div]`. It returns
  a read-only `CiphertextPair` over `[q0, ..., q_l]`, level 1, lifecycle
  `ReadyForFirstMult`, with two components per member.
- `RCB(pair) = q_div * high + low` validates the complete pair before raw
  access and does not mutate it.
- Tensor2 accepts two fully validated `ReadyForFirstMult` pairs. It returns a
  private-construction `TensorCiphertextPair` on `Q_l`, level 1, exactly three
  components per member, noise-scale degree 3, normalized recorded factor
  `SF_T`, and independent high/recombined logical scales.
- The module validates context identity, CKKS encoding, actual key tag, ordered
  modulus/tower parameters, level, degree/factor, slots, format, component
  count, divisor, lifecycle, and its private manifest before arithmetic.
- The accepted DCP derives its last-tower division factors locally and calls
  pristine `DCRTPoly::DropLastElementAndScale`; it must not regain a dependency
  on unchecked outer OpenFHE precomputation rows.
- Existing public signatures, DCP/RCB/Tensor2 arithmetic, validation ordering,
  project-owned diagnostics, warnings, and independent oracles are frozen
  except for the exact lifecycle/scale/helper changes required below.

Implement only Relin2 and the minimum changes needed to represent and validate
its result. Do not implement RS2, the Mult2 wrapper, pair Add/Sub, rotations,
refresh/bootstrapping, repeated multiplication, serialization, performance
work, compatibility layers, or speculative extension points.

## Exact public state seam and metadata contract

Add exactly the following bounded public seam:

```cpp
enum class PairLifecycle : std::uint8_t {
    ReadyForFirstMult,
    ReadyForRS2,
};

struct PaperScaleDescriptor final {
    double inputRecordedScalingFactor;
    lbcrypto::NativeInteger divisor;
    long double approximateLogicalScalingFactor;
    long double approximateRecombinedLogicalScalingFactor;
};

CiphertextPair Relin2(const TensorCiphertextPair& tensor) const;
```

Retain the existing three `PaperScaleDescriptor` fields and their meanings;
append the explicit recombined field. Do not rename or reorder existing fields,
introduce a second pair class, expose mutable construction, add a test-only
setter/friend, or add a future-operation API.

`ValidatePair` must explicitly branch on lifecycle:

- `ReadyForFirstMult` keeps its existing exact predicates and stable error
  texts: level 1, two components, degree 2, fresh recorded factor, exact `Q_l`
  prefix, high logical scale `inputRecordedScalingFactor / q_div`, and new
  recombined logical scale `inputRecordedScalingFactor`;
- `ReadyForRS2` is level 1, two components, degree 3, recorded Tensor factor
  `SF_T`, exact first-pair `Q_l` prefix, and the exact copied Tensor high and
  recombined logical scales;
- invalid enum values receive a stable project-owned lifecycle diagnostic.

DCP initializes the new recombined field to its input recorded factor. Tensor2
must calculate its recombined logical scale from each input's explicit new
recombined field, not silently substitute `inputRecordedScalingFactor`. Because
the valid first-lifecycle contract makes those two values numerically equal,
this field-source requirement is proved by equation-to-code source review;
black-box corruption tests prove validation, not which equal field was read.
Once `ValidatePair` accepts both valid lifecycles, Tensor2 must explicitly reject
any input not `ReadyForFirstMult` before multiplication. RCB must accept and
exactly recombine either valid lifecycle.

Relin2 returns a `CiphertextPair` with:

- exact bound context, `q_div`, key tag, slots, CKKS encoding, Evaluation
  format, and exact ordered `Q_l` prefix;
- level 1 and exactly two RLWE components in each member;
- lifecycle `ReadyForRS2`;
- Tensor input noise-scale degree 3 and exact recorded factor `SF_T`;
- `PaperScaleDescriptor{SF_T, q_div, Tensor.highLogical,
  Tensor.recombinedLogical}`.

The complete input and every deep ciphertext/metadata-map observable must be
unchanged.

## Required fail-fast implementation sequence

Implement this order exactly. Do not move key lookup ahead of Tensor or basis
validation and do not let OpenFHE auto-alignment hide a project error.

1. Run the complete accepted `ValidateTensorResult` before any non-validation
   raw access, cloning, multiplication, tower construction, or key lookup.
2. Require `tensor.GetOrderedModuli().size() >= tensor.GetNoiseScaleDegree()`.
   For the current degree-three state, `Q_l` needs at least three active towers,
   hence the complete context needs at least four including `q_div`. This is a
   Relin2-only precondition; do not narrow the existing three-full-tower DCP/RCB
   support.
3. Read-only find and fully validate the exact evaluation key described below.
4. Raise a clone of Tensor high to the full basis exactly as described below.
5. Validate the complete raised-high manifest and all three components.
6. Publicly relinearize raised high exactly once and low exactly once.
7. Validate relinearized high completely before private DCP.
8. Run the one private raw DCP arithmetic seam on relinearized high.
9. Before `EvalAdd`, require the DCP remainder and relinearized low to match in
   exact context identity, key tag, slots, CKKS encoding, Evaluation format,
   ordered `Q_l` basis/tower parameters, two components, level 1, degree 3, and
   recorded factor `SF_T`.
10. Compute only `v + w`; preserve `u` unchanged.
11. Construct the complete `ReadyForRS2` pair and run complete pair validation
    before return.

No input depth may change.

## Evaluation-key preflight contract

The global static evaluation-key cache is keyed only by string tag. Public
`Relinearize` does not prove context or key internals for this composition.
Before raising or any A/B getter:

1. Call `CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys()` and use `find(tag)`.
   Do not use `operator[]`, `GetEvalMultKeyVector`, or any production cache
   insertion/deletion/replacement path.
2. Require a present vector of size at least one. Extra later keys are allowed;
   validate and consume only index zero, the `s^2 -> s` key needed for a
   three-component ciphertext.
3. Require the first pointer non-null, exact bound context identity, and exact
   actual key tag.
4. Before any A/B getter, dynamically cast the first key to
   `EvalKeyRelinImpl<DCRTPoly>`. A wrong subtype must receive a stable
   project-owned `std::invalid_argument`, not a base-getter OpenFHE exception.
5. Branch on the bound `CryptoParametersRNS::GetKeySwitchTechnique()`:
   - HYBRID: A/B lengths each equal exactly `GetNumPartQ()`; every entry uses
     Evaluation format and the exact bound full `ParamsQP` basis, including
     tower order, modulus, root of unity, and cyclotomic order;
   - BV: A/B lengths each equal the complete-Q digit count: `|Q|` if digit size
     is zero, otherwise the exact integer expression
     `sum_i ((q_i.GetMSB() + digitSize - 1) / digitSize)`; every entry uses
     Evaluation format and the exact bound full ordered Q basis, including
     modulus, root, and cyclotomic order;
   - reject an unsupported technique with a project-owned diagnostic.

Key internals are validated against the complete context key basis, not the
current level-one ciphertext prefix. Do not require the vector to contain
exactly one key. Do not mutate the cache in production.

Public `Relinearize` will read the global cache again after this preflight. This
bounded slice does not promise thread safety against a concurrent cache
mutation; do not add locks, retries, snapshots, or a compatibility layer.

## High-path basis restoration and public relinearization

- Clone Tensor high. For each of its three `DCRTPoly` components, multiply
  every existing active tower by the integer `NativeInteger q_div`.
- Construct one Evaluation-format zero `NativePoly` from the bound context's
  final tower parameters and append it after the ordered `Q_l` towers.
- Reconstruct each `DCRTPoly` from the ordered tower vector. Install all three
  restored elements first, then set ciphertext level to zero. Do not mutate
  degree, factor, tag, slots, encoding, or metadata map.
- Validate raised high as exact full ordered Q, level 0, three components,
  degree 3, factor `SF_T`, exact context/tag/slots/CKKS/Evaluation metadata.
- Bind each input to a named `ConstCiphertext<DCRTPoly>` lvalue required by the
  public output-returning signature, then call `context_->Relinearize(...)`
  exactly once for raised high and exactly once for Tensor low.
- Do not use a floating scalar API. The append-zero operation is an RNS
  representation construction, not a CKKS plaintext-scale operation.

The expected stage table is:

| Stage | Ordered basis | Level | Components/member | Degree | Recorded factor |
|---|---|---:|---:|---:|---:|
| Tensor input | `Q_l` prefix | 1 | 3 | 3 | `SF_T` |
| raised high | complete `Q_l * q_div` | 0 | 3 | 3 | `SF_T` |
| relinearized high | complete basis | 0 | 2 | 3 | `SF_T` |
| private-DCP pair | `Q_l` prefix | 1 | 2 each | 3 | `SF_T` |
| relinearized low | `Q_l` prefix | 1 | 2 | 3 | `SF_T` |
| Relin2 output | `Q_l` prefix | 1 | 2 each | 3 | `SF_T` |

Treat this table as a tested engineering derivation, not as paper wording.

## One private DCP arithmetic seam

Extract the accepted coefficient arithmetic currently inside public `DCP` into
one private helper accepting an already fully validated, two-component,
complete-basis ciphertext and returning the raw quotient/remainder ciphertexts.
The helper must:

- remain private and add no public API;
- avoid calling or weakening public `ValidateDcpInput`;
- use the existing locally derived division factors and invoke
  `DropLastElementAndScale` exactly once on each of the two input
  DCRTPoly/RLWE components during one private-helper invocation;
- clone and preserve context/tag/slots/encoding/format, degree, recorded factor,
  and metadata map while installing the decomposed elements and setting level
  1;
- perform no lifecycle-specific validation or public pair construction.

Public DCP keeps its exact fresh degree-two contract, invokes the helper only
after its existing validation, and constructs `ReadyForFirstMult`. Relin2
validates the degree-three full-basis relinearized high first, invokes the same
helper, and constructs `ReadyForRS2` only after adding relinearized low. Do not
duplicate a second DCP arithmetic implementation.

## Required ordered TDD deliverables

Return one ZIP. Numbered patches are the only source of truth and must apply in
the exact order below to base commit `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.
Do not include competing full-file replacements or a second patch series.

1. `REVIEW.md`: equation-to-code mapping; OpenFHE source observations; public
   API/state decision; lifecycle/scale table; key-cache and HYBRID/BV proof;
   validation order; assumptions; exact blockers/unresolved questions. If any
   required contract cannot be justified, return `blocked` with review evidence
   and no speculative code.
2. `01-red-relin2-api.patch`: add the workflow branch trigger, CMake target, and
   one compile-only public API contract test. Production library must still
   compile. Building only the new target must fail specifically because
   `ReadyForRS2`, `PaperScaleDescriptor::approximateRecombinedLogicalScalingFactor`,
   and `DoubleCKKS::Relin2` are missing. It contains no production code.
3. `02-api-scaffold.patch`: add exactly the final public declarations and a
   temporary Relin2 implementation that immediately throws
   `std::logic_error("DoubleCKKS: Relin2 is not implemented")` before validation,
   raw access, key lookup, or arithmetic. It is non-mergeable and must not
   return a partial/invalid pair. Extend DCP's aggregate initialization with the
   already specified correct recombined-field value so all existing public
   pairs remain valid and warning-clean; do not yet add corruption validation,
   change Tensor2's scale source, or implement lifecycle branching. Those
   behaviors must remain runtime red. Keep the final parameter name in the
   header, but define the throwing scaffold with an unnamed parameter so
   `-Wextra -Werror` does not reject it:

   ```cpp
   CiphertextPair DoubleCKKS::Relin2(const TensorCiphertextPair&) const
   ```
4. `03-red-relin2-contract.patch`: register all independent public-seam cases
   below except the `Tensor2(ReadyForRS2)` guard case. After patches 01-03,
   every added case must compile, execute, and be independently observed red.
   Relin2 cases must reject the scaffold as the wrong exception/result;
   legacy-field cases must fail on their unmet validation/oracle. One failure
   must not mask unexecuted cases. Existing DCP/RCB/Tensor2 cases must remain
   green on the scaffold boundary.
5. `04-green-relin2-core.patch`: implement the complete Relin2 algorithm,
   private DCP extraction, lifecycle/scale validation, key preflight, basis
   restoration, two public relinearizations, pre-add validation, final pair,
   RCB support, field-source change, and all corresponding regressions. Remove
   the scaffold. Deliberately do not add the Tensor2 `ReadyForFirstMult` guard
   yet, because no valid `ReadyForRS2` value existed before this boundary. All
   Relin2/core cases must now be green, but this intermediate patch is still
   non-mergeable until the next red-green pair closes that guard.
6. `05-red-tensor2-lifecycle.patch`: test only. Use public Relin2 to obtain a
   valid `ReadyForRS2` pair, call Tensor2 with it, and require the exact
   project-owned `ReadyForFirstMult` lifecycle diagnostic. Observe a directed
   red on the wrong downstream behavior/diagnostic. This is the lifecycle
   guard's first valid red; the earlier Relin2 scaffold failure is only a
   dependency-not-implemented red and must not be reported as guard coverage.
   The runtime proves the diagnostic/attribution; `REVIEW.md` source-order audit
   must separately prove that the eventual guard precedes multiplication.
7. `06-green-tensor2-lifecycle.patch`: add only the smallest pre-arithmetic
   Tensor2 lifecycle guard that makes the directed red and complete suite green.
8. `07-final-docs.patch`: final design/API documentation only after all old and
   new cases are green.
9. `TESTS.md`: for every patch boundary, give exact `git apply --check`,
   configure, target-build, full-build, and CTest commands actually run;
   environment/tool versions; exit statuses; exact compiler/test failures;
   test counts; and explicitly unrun work. Inspection is not execution.
10. `PATCHES.sha256`: SHA-256 for every returned patch and Markdown deliverable.

Never call the scaffold a Relin2 green, let one failure stand in for other
unexecuted cases, weaken an oracle between red and green, or expose a knowingly
invalid public value merely to manufacture a red. A case may include a passing
DCP field-propagation assertion before reaching its intended red lifecycle/
validation assertion; report both observations rather than mislabeling the
whole scaffold as behaviorally complete.

## Complete public-seam test contract

Tests may use public OpenFHE APIs and test-owned arithmetic, but must not call a
production-private raise/DCP/key-validation helper or add a production
backdoor. Use small rings and explicit deterministic polynomial fixtures. Test
code may use `try`/`catch` to assert failures; production code may not.

At minimum register separately named cases for:

### Exact valid arithmetic, state, scales, and immutability

- Generate the ordinary evaluation key using the fixture secret key.
- Build a valid Tensor2 result through public DCP and Tensor2. Independently
  clone/raise its high path in test code, use the trusted public OpenFHE
  `Relinearize` primitive on raised high and unchanged low, and run a test-owned
  Boost `cpp_int` centered DCP oracle. The expected result is `(u, v + w)`.
- Compare every output member/component/tower/coefficient to that independent
  expected result modulo each active `Q_l` residue. Also assert the exact
  per-residue RCB recombination identity after removing only the test-appended
  `q_div` residue from the full-basis reference; never call `ModReduce` or
  `Rescale` in the reference.
- Algebraically force a nonzero third input component and a nonzero key-switch
  contribution. Define the publicly observable contribution residues as
  `K0 = relinRaised[0] - raisedHigh[0]` and
  `K1 = relinRaised[1] - raisedHigh[1]`; prove at least one has a nonzero named
  tower/coefficient. An equivalent cross-check may relinearize the same raised
  high once as supplied and once with only its third input component set to
  zero in test-owned clones. Also force and record a named position at which
  the test-owned DCP remainder `v` and `w = Relin(low3)` are both nonzero.
- Deterministically construct and record named component/coefficient witnesses
  for centered quotient/remainder sign boundaries and quotient carry on the
  actual full-basis `Relin(q_div * high3)` output immediately before the
  independent DCP. Test-controlled, valid-shaped A/B contents may be used
  through public test APIs if needed; explain why the resulting fixture still
  exercises the trusted public primitive. Merely scanning a random generated
  key until a witness appears is insufficient. If these witnesses cannot be
  constructed, the verdict must be `changes needed` or `blocked`, not
  `ready to apply`; never substitute a random correct-versus-naive inequality.
- Assert result type/lifecycle, exact basis/tower parameters, level, two-
  component shape, degree/factor, divisor, context/tag/slots/encoding/format,
  and separately copied high/recombined logical scales.
- Deep-snapshot the Tensor input ciphertexts and metadata maps and prove every
  observable unchanged. Also prove public RCB accepts `ReadyForRS2`, does not
  mutate it, and yields the exact recombined result.

Public OpenFHE `Relinearize` is a trusted primitive under this composition test;
do not claim this slice independently verifies OpenFHE's key-switch arithmetic.
A correct-versus-naive assertion may be permanent only if a fixed construction
algebraically guarantees the difference.

### Validation and execution order

- Corrupt one Tensor manifest field while no evaluation key is installed and
  require that exact field-specific project diagnostic. This proves stable
  project attribution; `REVIEW.md` must prove by source order that complete
  Tensor validation precedes cloning, key lookup, public relinearization, and
  arithmetic.
- On the existing full-three-tower fixture, Tensor degree 3 has only two active
  towers. Require a stable Relin2 insufficient-active-basis diagnostic while
  existing public DCP and RCB still succeed; use source review, not the black-
  box diagnostic alone, to prove the basis check precedes key lookup/arithmetic.
- With a fully valid Tensor and no evaluation key, require the exact project-
  owned missing-key `std::invalid_argument`, never an OpenFHE exception.
- After core Relin2 is green, add the separately ordered test-only lifecycle
  patch described above. It must first reach Tensor2 with a valid
  `ReadyForRS2` value and fail on the wrong downstream behavior/diagnostic;
  only the following minimal guard patch may make it green.

Every negative case must require `std::invalid_argument`, a `DoubleCKKS: `
prefix, and a field-specific stable message. A `logic_error`, generic exception,
OpenFHE exception, silent normalization, crash, or successful return fails it.

### Evaluation-key adversarial matrix

Use only public OpenFHE cache APIs in test code. Every mutation of the static
key map must be wrapped in a test-owned RAII guard that restores the entire
prior state even if an assertion throws. `EvalMultKeyGen` does not overwrite an
existing tag entry; tests must not rely on it doing so. Independently cover:

- expected tag absent;
- present vector empty;
- null first entry;
- wrong key context under the expected map tag;
- wrong actual key tag under the expected map tag;
- wrong concrete key subtype, proving rejection before a base A/B getter;
- HYBRID A or B length mismatch, exact `ParamsQP` basis mismatch, and a
  non-Evaluation-format A/B entry;
- BV A or B digit-count mismatch for each of `digitSize==0` and
  `digitSize>0`, exact full-Q basis mismatch, and a non-Evaluation-format A/B
  entry;
- a valid vector with an additional later valid key, proving no erroneous
  `size()==1` restriction;
- a valid key at index zero followed by a malformed or null later key, with
  successful Relin2, proving later entries are not inspected or consumed.

Each malformed-key case must prove stable project-owned diagnostic attribution
and that the Tensor inputs and static key map are unchanged. A public-seam
runtime test cannot observe whether an internal temporary clone/raise or a
discarded successful primitive call happened. Prove by source-order review in
`REVIEW.md` that Tensor/basis/key validation precedes cloning, raising, public
relinearization, and other arithmetic; do not overclaim this from the black-box
result. Execute at least one valid full-basis and one valid level-one-prefix
public relinearization with the same exact generated key under each supported
configured technique. If pristine OpenFHE cannot construct one requested
malformed fixture through its public types, return the exact source/API blocker
rather than adding production access.

### Existing lifecycle and regression coverage

- Extend DCP tests so the new recombined field equals the fresh input recorded
  factor and participates in manifest validation and deep snapshot checks.
- Corrupt the new field on `ReadyForFirstMult` and require RCB to reject it
  with the field-specific diagnostic; confirm by source-order review that this
  check precedes non-validation raw access.
- Corrupt the new field on a valid DCP input pair and require Tensor2 to reject
  it with the field-specific diagnostic; confirm by source-order review that
  validation precedes multiplication. For valid equal-valued fields, record
  the equation-to-code source proof that Tensor2 reads the explicit recombined
  field; do not claim the black-box test distinguishes two equal values.
- Preserve all existing DCP/RCB and Tensor2 oracle strength, exact diagnostics,
  immutability checks, and 6/6 accepted exact-current CTest behavior.

## Exact downstream verification contract

The existing workflow is command authority. The first test patch must add
`agent/codex-relin2-01` to its push trigger. Every Linux red and green must bind
the exact project SHA, OpenFHE SHA, `ubuntu-24.04`, compiler/CMake versions,
commands, exit code, named test count, and Actions URL. Codex will retain the
intended reds and will run the final exact commit on Linux and Windows; do not
push or dispatch CI yourself.

With official OpenFHE installed at `${OPENFHE_PREFIX}`, the project commands are:

```sh
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=${OPENFHE_PREFIX}
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

The final exact commit must also run on `windows-2022`, MSYS2 `MINGW64`, Boost,
Make, `mingw-w64-x86_64-gcc`, and `mingw-w64-x86_64-cmake`, using the existing
workflow's explicit out-of-dotted-workspace paths. Linux and Windows must bind
the same project SHA. Red Windows runs are unnecessary once each intended Linux
red is retained. Mark every unexecuted environment/command `pending`; never
infer it from another platform.

## Prohibited operations and claims

- Do not access any old/local/private 2023/1788 implementation, any known-wrong
  project tree, author code, or modified OpenFHE tree.
- Do not change official OpenFHE, use a fork, add a dependency, weaken C++17 or
  warning-as-error settings, replace FIXEDMANUAL, or broaden supported state.
- Do not add production `try`/`catch`, key-cache mutation, locking, retry logic,
  silent repair, unchecked cast, hidden fallback, or direct `KeySwitchCore`.
- Do not use `EvalMultAndRelinearize`, floating scalar multiplication,
  `ModReduce`, or `Rescale` in Relin2 or its exact reference path.
- Do not commit, push, open a PR, dispatch/rerun CI, use credentials, browse for
  implementations, or inspect unrelated files.
- Do not perform or discuss network-security review. The bounded scope is
  algorithm, RNS semantics, OpenFHE integration, and tests.
- Do not claim a build, test, Windows result, witness, precision/error result,
  performance result, or security result unless actually executed and recorded.
- Do not treat public OpenFHE relinearization as independently verified by a
  reference that calls the same primitive.
- Do not broaden the design where a concrete defect can be fixed locally.

## Acceptance criteria and response format

Lead with exactly one bounded verdict: `ready to apply`, `changes needed`, or
`blocked`, followed by the exact reason. Return the ZIP once; do not ask Codex
to infer context from another conversation.

Codex must be able to apply every numbered patch, in order, to exact base
`fb862a3...`. The API target must produce only the specified compile red after
the production library builds. The scaffold must fail before all Relin2 access.
Every patch-03 case must be independently red on the scaffold/legacy contract;
the Tensor2 lifecycle guard must instead have its directed red after core
Relin2 can construct a valid `ReadyForRS2` value. The core suite may be green at
patch 04, but the task's complete suite may be green only after the minimal
patch-06 guard. The final tree must obtain all of the following without
weakening an old oracle:

- exact `(u, v+w)` coefficient/tower equality and exact RCB recombination;
- deterministic explicit nonzero `K0`/`K1`, `v`/`w`, and centered DCP
  boundary/carry witnesses; absence of any required witness prevents
  `ready to apply`;
- correct full-basis high restoration, two public relinearizations, one shared
  private DCP arithmetic seam, and no tower consumption;
- exact lifecycle, metadata, dual logical scales, basis, and deep immutability;
- complete pre-arithmetic Tensor/basis/key/pre-add validation in the required
  order, including HYBRID/BV internal key-shape tests;
- stable `ReadyForFirstMult` behavior plus `ReadyForRS2` RCB acceptance and
  Tensor2 rejection;
- strict warning-clean C++17 build and all old/new CTests green on one exact
  Linux/Windows commit when Codex performs hosted verification;
- no upstream change, credential, unsupported claim, partial public result,
  future-operation scaffold, hidden dependency, or work outside this slice.
