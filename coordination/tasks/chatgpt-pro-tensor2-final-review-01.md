# ChatGPT Pro exact-current Tensor2 closure review

Prepared: 2026-09-01 Asia/Shanghai

## Background and bounded objective

Continue the same saved Tensor2 conversation. Review the complete supplied
clean-room project at branch `agent/codex-tensor2-01`, exact commit
`55f3b43c47b5b2464625afcc6a1f244724336d5b`, and decide whether the bounded
`t=2` Tensor2 slice is mergeable into the clean-room integration line.

This is an algorithm, numeric-semantics, OpenFHE-integration, test, and
engineering-evidence review. It is not a network-security review. Do not seek,
inspect, infer, or discuss the repository's known-wrong former implementation.

Use only the complete materials in this package: the user-supplied paper, exact
clean-room project, your full preceding Tensor2 delivery, Codex scale proof,
retained TDD/CI evidence, final internal reviews, and pristine OpenFHE 1.5.0 at
exact commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`. Do not assume access to
local files, a private repository, previous conversation memory, browser state,
or anything outside the package.

## Complete preceding-review state

The corrected input package in your preceding turn was
`20231788-cleanroom-tensor2-base-87c84b-ci334114-r2.zip`, 8,691,359 bytes,
SHA-256
`eea8aca629e98bfc4fc719c2c7ddbf38610f654630bf92d499d5aa75826753be`.
It bound the project base
`87c84b879c13b55cf15d6559d3317853228fdc05` and the successful base-only
Actions run `33411494861`.

You completed naturally with verdict `ready to apply` and returned
`tensor2-pro-01-ready-to-apply.zip`, 33,877 bytes, SHA-256
`d869a8c27e650e20dbd5f56ea7c99f492c5f6aa6219e8f84fade24ab4e4c1808`.
The complete original ZIP and all 22 extracted files are supplied under
`prior-tensor2-delivery/`. Your ordered patch hashes were:

```text
758dfdbc1c28099749d65be3438ea245d24e66d0b90793443137b8997f4b7723  01-red-api.patch
e725c1e9d289f733ec5aeb25e209431ae5045bfc1daba66a9fd4effc9d66a31d  02-api-scaffold.patch
38d5857430504527d9a8fe3441938364c3d12c36f0eb65dc6427ca0512e37416  03-red-tensor2-contract.patch
27d10a4a929f4f7d994c2aa477d607723be41e606f60de8af5b6c62879a04fb2  04-green-tensor2.patch
be3b2c0bad3bd86bbdf6d0a9f7bc6a210e80f14e6e9978105d9747e53ef29703  05-final-docs.patch
```

Codex verified that output once, read every file, and replayed all five patches
in order on a disposable exact-base worktree. Do not rely on this summary:
compare your supplied patches and evidence with the complete final source,
exact base-to-head diff, and commit history in this package.

## Exact downstream changes to adjudicate

Codex applied the five supplied patches in their prescribed order, preserving
their identities in separate commits, then made only these independently
reviewed changes:

1. Added explicit CMake/compiler version output to both hosted jobs.
2. Added a fifth independent CTest,
   `tensor2_prearithmetic_key_compatibility`. It creates two individually valid
   same-context, same-slot DCP pairs with different key tags. The test requires
   the project-owned `std::invalid_argument` and field-specific diagnostic.
   Because OpenFHE `EvalMultNoRelin` calls `TypeCheck`, which also rejects
   different key tags, an implementation that multiplies before project-owned
   mutual validation fails this test with the wrong exception attribution.
3. After the first production Linux green, removed the duplicated Tensor-only
   ciphertext validator by parameterizing the existing private validator with
   expected component count and state wording. Existing two-component
   diagnostics, including the words `exactly two RLWE components`, were
   retained. No public API or arithmetic changed.
4. Updated documentation and workflow display names to record the fifth test,
   exact evidence boundary, and current review status. Command authority did
   not change.

No Codex change adds a fourth multiplication, low-low term, tower drop,
relinearization, production `try`/`catch`, mutable result construction, new
lifecycle state, dependency, or upstream OpenFHE modification.

## Bounded algorithm and metadata contract

For left pair `(h1,l1)` and right pair `(h2,l2)`, review the implementation
against Definition 4.1:

```text
high = h1 tensor h2
low  = h1 tensor l2 + l1 tensor h2
```

`l1 tensor l2` must be omitted. Exactly three public OpenFHE
`EvalMultNoRelin` calls and one cross-term addition are expected. The active
ordered RNS basis remains unchanged at level 1; Tensor2 performs no
relinearization, coefficient rescale, or `ModReduce`.

The paper and OpenFHE metadata scales are distinct:

```text
H_out = H_1 * H_2
R_out = R_1 * R_2 / q_div

noiseScaleDegree = 3
recordedSF       = SF_1 * SF_2 / baseSF
baseSF           = CryptoParametersCKKSRNS::GetScalingFactorReal(0)
```

The first two equations use the actual integer `q_div`; the metadata equation
uses OpenFHE's real base scaling factor. `q_div` and `baseSF` must not be
equated. The supplied independent scale derivation states which points are
paper facts, observed OpenFHE behavior, and module-design inferences.

## Required validation and state boundary

- `Tensor2` must invoke the complete existing pair validator on the left, then
  the right, then mutual compatibility before raw element access or OpenFHE
  arithmetic.
- Mutual checks must include key tag and slot count; supported failures must be
  module-attributed `std::invalid_argument` diagnostics with the stable
  `DoubleCKKS: ` prefix.
- A distinct `TensorCiphertextPair` with private construction and read-only
  ciphertext getters must represent the exactly-three-component state so it
  cannot enter DCP/RCB APIs that require two components.
- Both inputs must remain completely unchanged, including deep metadata-map
  key/value state.
- Existing DCP/RCB arithmetic, public API, validation behavior, and tests must
  remain green.

## Independent oracle requirements

Confirm that test expectations do not call OpenFHE multiplication and instead
use a Boost `cpp_int` schoolbook negacyclic-convolution oracle over every active
RNS tower, output component, and coefficient. Confirm the executed fixtures
contain and assert all three distinguishing witnesses:

1. `X^(N-1) * X = -1` for negacyclic wrap;
2. signed multiplication crossing an active tower modulus;
3. a named independently nonzero low-low term whose omission is proved by
   checking cross-only and rejecting cross-plus-low-low.

Expected paper and OpenFHE scales must be computed from input manifests and
context parameters, never copied from the result.

## Exact ordered TDD and hosted evidence

All identities below are supplied with complete logs and concise retained
records. Inspect their `headSha`, commands, failure attribution, and test
counts rather than accepting this summary.

1. API compile red: commit
   `f3db12ef9fb0d13df0f779157eed168b8d582ea4`, run `33425868973`.
   Linux compiled the existing production library and failed only because
   `TensorCiphertextPair`, `TensorScaleDescriptor`, and `DoubleCKKS::Tensor2`
   were absent. CTest was not run; Windows was cancelled after the red.
2. Non-mergeable scaffold: commit
   `76bac1800553f79c1dbaff15ccebf6e50c65ad89`. It immediately throws the
   specified `logic_error` before access/arithmetic and was never claimed as a
   behavioral green.
3. Complete runtime red: exact test head
   `482d27d0c43c22779aa548e00955ed90175dee97`, run `33426712752`.
   The strict Linux build passed; `dcp_rcb` passed and all five Tensor2 CTests
   independently failed on the scaffold. Windows was cancelled after the red.
4. First implementation green: commit
   `1408d46217e97a1c14d43d49b64791da22f652da`, run `33427271692`.
   Linux passed the strict build and 6/6 CTest; the intermediate Windows job
   was cancelled because final cross-platform verification remained.
5. Final exact source/test/workflow head:
   `55f3b43c47b5b2464625afcc6a1f244724336d5b`, run
   `https://github.com/leemaple/20231788./actions/runs/33428194982`.
   The run's `headSha` is exact and conclusion is `success`:
   - Linux `ubuntu-24.04`, CMake 3.31.6, GCC 13.3.0: strict build and 6/6
     CTest passed;
   - Windows Server 2022, MSYS2 `MINGW64`, CMake 4.4.2, GCC 16.2.0: pristine
     OpenFHE and project builds succeeded, then 6/6 CTest passed.

The final independent Standards and Spec reviews both returned PASS with zero
actionable findings. Their complete two-axis record is supplied. Verify rather
than inheriting those conclusions.

## Required review questions

1. Does exact head `55f3b43...` implement Definition 4.1 with exactly the
   required three products, correct cross addition, and no low-low term?
2. Are the `H_out`, `R_out`, degree, and recorded-factor transitions correct
   and separately represented without equating `q_div` and `baseSF`?
3. Does all project-owned pair and mutual validation occur before access and
   arithmetic? Does the added key-tag test genuinely make the ordering
   observable?
4. Is the distinct result type immutable at its public seam, correctly shaped,
   and completely validated without introducing a speculative lifecycle?
5. Does the independent oracle prove every output coefficient/tower/component,
   all named witnesses, low-low omission, full input immutability, and result
   metadata?
6. Did the shared-validator refactor preserve every current DCP/RCB predicate
   and diagnostic attribution while removing duplication?
7. Do the retained commits/logs establish the requested compile-red,
   non-mergeable scaffold, complete runtime-red, green, docs, and exact
   cross-platform final gates without rewriting history or overstating an
   unexecuted result?
8. Did Codex add any unsupported behavior, hidden dependency, test backdoor,
   portability defect, or scope beyond this Tensor2 slice?
9. Return one exact-head verdict: `MERGEABLE`, `NEEDS NAMED FIXES`, or
   `NOT MERGEABLE`. `MERGEABLE` is bounded to DCP/RCB plus Tensor2 and remains
   conditional on the separately required Windows ZCode/Zima same-commit
   review. It says nothing about Relin2, RS2, Mult2, pair Add/Sub, repeated
   multiplication, precision, or performance.

Classify every finding P0, P1, P2, or P3. For each finding provide exact
file/line, proof, reachable impact, and smallest remediation. Separate observed
source facts, mathematical derivation, retained remote execution evidence,
local execution, and unverified claims.

## Required deliverables

Return one ZIP containing:

1. `TENSOR2-EXACT-CLOSURE-REVIEW.md` — verdict, P0/P1/P2/P3 counts, and answers
   to all nine questions;
2. `TENSOR2-CONTRACT-MAP.md` — pass/fail/uncertain map for arithmetic, dual
   scales, state/type boundary, validation order, oracle/witnesses,
   immutability, DCP/RCB regression, and exact CI binding;
3. `TENSOR2-TDD-EVIDENCE-AUDIT.md` — ordered red/green/docs history and exact
   retained run audit, including every cancelled/unrun platform boundary;
4. `INTERNAL-REVIEW-DISPOSITION.md` — explicit agreement or disagreement with
   each final Standards/Spec conclusion and the resolved key-tag finding;
5. `EXECUTION.md` — commands actually run, environment, exit status, test
   counts/timeouts, and checks not run; inspection is not execution;
6. `0001-tensor2-exact-closure-fixes.patch` only if a concrete current-head
   finding requires a change. It must apply to exact `55f3b43...`, remain
   inside this bounded slice, and must not weaken tests merely to pass.

State the returned ZIP byte size and SHA-256 in chat. Do not rely on a later
message for missing content.

## Prohibited operations and claims

- Do not access old/private/local 2023/1788 implementations or the authors'
  proof-of-concept code.
- Do not modify pristine OpenFHE, add a dependency, change FIXEDMANUAL, or
  broaden into later operations.
- Do not add production exception recovery or speculative compatibility code.
- Do not push, merge, open a PR, dispatch/rerun CI, use credentials, or inspect
  unrelated files.
- Do not perform or discuss a network-security assessment.
- Do not claim local build, CTest, Windows, precision, performance, or security
  evidence unless actually executed and recorded. Retained hosted logs are
  remote evidence, not your local execution.

## Acceptance standard

The review is accepted only if it is bound to exact head `55f3b43...`, examines
the complete current source and exact base-to-head history, independently
checks the paper/OpenFHE scale separation and all oracle/ordering properties,
audits exact hosted evidence without converting inspection into execution, and
returns a bounded verdict with no claim about unimplemented later operations.
