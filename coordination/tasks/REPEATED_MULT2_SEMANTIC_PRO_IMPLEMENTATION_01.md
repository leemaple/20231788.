# Repeated Mult2 semantic implementation 01

## Assignment and authority

Create the first production implementation and executable TDD slice for two
successive clean-room OpenFHE 1.5.0 `DoubleCKKS::Mult2` operations. The public
behavior is already approved in `project/coordination/TEST_SEAMS.md`: client-owned
setup/input/final-output work and evaluator-only repeated calls to the same public
`Mult2` interface. Do not ask for another interface token.

The project snapshot is exactly commit
`80d771c52df10bce1c60992b5e0edb4e64f145ca` on branch
`codex/repeated-mult2-semantic-01`. Treat every older task, return and review as
evidence, not active instructions. This file is the current assignment and wins
if older prose conflicts. Use only the clean-room project and the supplied
official OpenFHE source pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
Never consult an old/local implementation or a modified OpenFHE tree.

Before changing anything, read completely:

- `project/.agents/skills/openfhe-2023-1788-workflow/SKILL.md`,
  `project/.agents/skills/openfhe-2023-1788-workflow/references/engineering.md`,
  and `project/.agents/skills/openfhe-2023-1788-workflow/references/external-collaboration.md`;
- `project/coordination/TEST_SEAMS.md` and
  `project/coordination/REPEATED_MULT2_PHASE_CONTRACT_RESEARCH.md`;
- `paper/PAPER-2023-1788.txt` and the matching PDF, especially all definitions,
  algorithms and bounds in Section 4 plus the complete Section 6.3 experiment;
- current `project/include/`, `project/src/`, `project/CMakeLists.txt`,
  `project/.github/workflows/dcp-rcb.yml`, and relevant current tests;
- `evidence/repeated/REPEATED_MULT2_ACTUAL_RETURN_DISPOSITION.md`,
  `evidence/repeated/REPEATED_MULT2_ZCODE_RETURN_DISPOSITION.md`, and
  `evidence/repeated/PAPER_H128_SETUP_CANDIDATE_B_DECISION.md`;
- `evidence/io/LOSSLESS_CLIENT_IO_RETURN_SPEC_REVIEW.md`;
- the supplied prior Pro return, especially `DESIGN_DECISION.md`,
  `PROBE_ACCEPTANCE.md`, `FROZEN_SECOND_MULT2_CONTRACT.md`,
  `contracts/SECOND_MULT2_EXACT_VECTORS.json`, both proposed tests, and its
  CMake patch; and
- the exact official files cited by its source ledger. Recheck citations against
  the supplied official tree; do not rely on paraphrases alone.

Stop and report the exact missing/hash-mismatched input if this source commit,
the exact-vector JSON, or a cited official file is absent. Do not improvise from
conversation memory.

The handoff must place the following previously reviewed evidence at the exact
relative paths named above. Verify these bytes before using them:

| packet path | bytes / SHA256 | frozen origin |
| --- | --- | --- |
| `evidence/repeated/REPEATED_MULT2_ACTUAL_RETURN_DISPOSITION.md` | 7,796 / `203188048c6b419019fde4ec718ea15092f0b5ccc54ff11b8bb84c56ab932c49` | repeated evidence commit `46fc1e6a127cc70a8c1f3ea449d570995b129de2` |
| `evidence/repeated/REPEATED_MULT2_ZCODE_RETURN_DISPOSITION.md` | 4,592 / `598ab86c123d419594399fd9ae34db27e8c918392068e83299ef761bd701fab0` | repeated evidence commit `46fc1e6a127cc70a8c1f3ea449d570995b129de2` |
| `evidence/repeated/PAPER_H128_SETUP_CANDIDATE_B_DECISION.md` | 25,309 / `7d2d8ab1b1293bf72ec7b2efceb2644a72a5bec197184496031ff252ea9e208d` | repeated evidence commit `46fc1e6a127cc70a8c1f3ea449d570995b129de2` |
| `evidence/io/LOSSLESS_CLIENT_IO_RETURN_SPEC_REVIEW.md` | 4,674 / `0a0e790e835915e9f7cb69b026d9f8605fe90ef4eb90e459e862312c6ac112a3` | I/O evidence commit `afac29757832c8a9cc6626db4d0b5a7a5154f2b6` |
| `paper/PAPER-2023-1788.txt` | 90,235 / `60dd871a2769fddfe7ce7b2562d031d7c8d819a679eff3c2b6ebf3d7ea5769ae` | user-supplied paper text |
| `paper/PAPER-2023-1788.pdf` | 759,375 / `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac` | user-supplied paper PDF |

The expanded prior Pro return must be byte-derived from the 63,963-byte archive
whose SHA256 is
`bee2b27ebf88c901b5b91bc3e79fe386231f07ea580b5228512bf380fdac2fd2`;
do not substitute its visible-chat prose or a rewritten copy.

## Current baseline and the one vertical slice

At `80d771c`, `CiphertextPair` binds one context, divisor, ordered Q basis,
level, approximate `PaperScaleDescriptor`, recorded scale, lifecycle, tag,
slots, format and arity. `DoubleCKKS` owns one context and one fixed full basis.
`DCP` creates a family-0 level-1 pair; `Tensor2 -> Relin2 -> RS2` returns a
level-2 `RefreshRequired` pair; `Mult2` is only that wiring. Validation derives
all accepted states from the constructor's original basis/scale. A second call
therefore rejects. The source has no immutable family plan, exact rational scale
receipt, client setup object, or family re-entry.

The current CMake file has exactly 57 CTest name/command bindings in their
existing order and five explicitly built API targets:
`relin2_api_contract_test`, `rs2_api_contract_test`,
`mult2_api_contract_test`, `add_api_contract_test`, and
`sub_api_contract_test`. Preserve all 57 bindings, all existing assertions and
all existing first-Mult2 behavior. Append one new semantic CTest as binding 58.

Implement only this slice:

1. client setup for a low-N diagnostic with the immutable basis/key families
   needed for two operations;
2. client construction/encryption of two nontrivial complex inputs;
3. evaluator calls `Z = Mult2(X,Y)` and then `W = Mult2(Z,Z)` through the same
   public evaluator method, with no intermediate client operation; and
4. client-side final/diagnostic decryption plus an independent exact oracle for
   both Z and W.

Do not implement or claim the eight-operation test, production lossless I/O,
paper N/prime ordering, h=128 setup, 1000 runs, performance, security, or full
paper reproduction in this patch. Design the family plan/state transition as a
small ordered sequence so those later slices do not require replacing this API.
Do not implement generic FHE backends or speculative extensibility.
Let invalid states and failed assertions surface directly. Do not add broad
`try`/`catch` wrappers merely to continue or relabel a failure; catch only a
specific expected exception when the test itself proves that rejection contract.

## Required RED before GREEN

Return two independently applicable patches in order.

`0001-red-repeated-mult2-semantic-two-square.patch` contains the new semantic
test, its CMake registration, and only the minimum CI binding needed to select
it. It must add the exact push branch `codex/repeated-mult2-semantic-01` to
`.github/workflows/dcp-rcb.yml`; do not assume `workflow_dispatch` or claim that
the current push filter will run this branch. It contains no production
implementation. The test must express the
approved public seam and expect correct Z and W. A missing-API compile failure is
an acceptable genuine RED if a minimal setup type must first exist; otherwise
the current second-call rejection is the expected runtime RED. Record the exact
predicted/observed failure honestly. Never add a deliberate defect.

The prior `repeated_mult2_current_second_call_boundary` rejection and
`repeated_mult2_basis_family_key_routing_probe` shape probe are evidence only.
Neither is this RED, GREEN, or the new CTest. Do not copy their names or relabel
their output. Do not weaken an oracle after seeing a failure.

`0002-green-repeated-mult2-semantic-two-square.patch` applies after RED and
contains the smallest production/header changes that make that exact test pass.
Do not combine the patches, modify RED expectations in GREEN, or include
unrelated refactors.

## Public seam and secret boundary

Keep the caller's evaluation expression exactly two calls to the public
`Mult2` method. You may add minimal client-setup/result types, a factory, or a
constructor overload; justify their ownership in `DESIGN_DECISION.md`. Keep
basis-family choice, key-row choice, re-entry and exact scale transition inside
the implementation. Tests may observe read-only receipts/invariants, but may
not call private tower-copy, key-routing or phase helpers.

Client setup may hold/use the root secret and projected family secrets only
while creating matching family-local evaluation keys. The client may retain its
root secret separately for final decryption. Once evaluation starts, the
evaluator object graph and every `Mult2` path must contain/use no private key and
must not decrypt, re-encrypt, bootstrap, call an external DCP-after-RCB refresh,
or invoke Section 6.2 refresh. Prove this structurally by ownership and with
test-visible setup/evaluation scope separation; a comment is insufficient.

Ordinary native `KeyGen` is permitted for this low-N diagnostic. Keep key
creation behind a client-setup boundary that can later accept the independently
reviewed h=128 construction without changing evaluator calls. h=128 is a later
gate, never a prerequisite or simulated success here.

The legacy one-context constructor/path must retain its explicit
`RefreshRequired` terminal rejection and diagnostics. Repeated readiness is
valid only for pairs issued by a correctly configured family plan. Never make
an arbitrary old `RefreshRequired` pair acceptable by removing a guard.

## Immutable basis, context and key-family contract

For operation `i`, let `d` be the actual final Div prime, `A_(i-1)` the active
ordered basis, and `B_(i-1) = A_(i-1) || [d]`. Each identity includes modulus,
root of unity, cyclotomic order and order. For this slice create the exact
families required for two operations and the terminal receipt; do not mutate or
reuse a context's tables to impersonate another family.

- Each nonempty family owns its OpenFHE context, actual returned
  `CryptoParametersCKKSRNS`, Q/P/QP tables, scheme instance, distinct nonempty
  key tag and family-local evaluation-key row.
- Construct `B_i` by removing the actual `m_i` consumed by the preceding RS2
  and retaining the same exact `d`. A different ordered Q family must not alias
  the same factory context. Equivalent-family interning may be observed but not
  mistaken for different-family identity.
- Use HYBRID with `numPartQ = |B_i|` and verify `numPerPartQ = 1` (`alpha=1`).
  The paper's initial `dnum=11` describes the eleven-prime `B0`; it is not a
  legal fixed row count for smaller families. Each family may have its own P;
  require its own nonempty `P` and exact `QP = Q || P`. Never require P equality
  or share P/QP tables or key rows across families.
- Project one root secret during client setup by exact `(modulus, root)`
  identity into each family. Generate that family's evaluation keys from that
  projection, then release the projected private-key handle before evaluation.
  Never copy by an unchecked index/modulus-only match, generate unrelated
  secrets, or move private keys into evaluator state.
- Re-entry after RS2 creates independent ciphertext wrappers bound to the next
  context/tag and its active prefix while preserving every coefficient, exact
  root identity, slots, arity, format, logical receipt and noise/recorded-scale
  metadata. It must not mutate the source, perform DCP, multiply/divide by d,
  reset the scale, encrypt, decrypt or touch an unrelated cache row.
- Snapshot actual context parameters, P/QP tables, relevant evaluation-key rows
  and caller inputs before evaluation, and prove exact equality of their public
  structural fields and coefficient/residue values afterward. Do not compare
  raw object memory, pointer bytes, nondeterministic serialization, padding or
  allocator state. Clear only tags owned by the test/setup; never globally erase
  unrelated rows.

Correct all known prior-candidate defects rather than importing its patch:

1. Put `/W4 /WX` only in the MSVC branch and
   `-Wall -Wextra -Wpedantic -Werror` only in the non-MSVC branch.
2. Pass `CKKSRNS_SCHEME` explicitly to `CryptoContextFactory::GetContext`.
3. Use and validate the parameters attached to the context actually returned by
   the factory, never only the locally requested parameter object.
4. Give the semantic test its own test/log identity; do not emit the old
   first-precision label or make a current-boundary rejection its success.
5. Keep the permitted low-N semantic second operation independent of h=128.
6. Explicitly select `PREMode=NOT_SET` so `paramsPK=Q`, and assert the returned
   public-key basis/profile. Do not retain the old candidate's `INDCPA` choice.
7. Explicitly select and validate `CKKSDataType=COMPLEX`; the long constructor's
   default is `REAL`. Also explicitly select HYBRID/FIXEDMANUAL/STANDARD/HPS and
   the intended execution/noise/multiparty fields rather than relying on shifted
   or trailing defaults.

The actual-returned-profile check must cover at least scheme ID; Q moduli/roots
and order; P and QP; `numPartQ/numPerPartQ`; key-switch/scaling/encryption/
multiplication/PRE modes; CKKS data type; ring dimension, encoding parameters,
secret distribution and declared diagnostic security level. Fail closed before
keys/ciphertexts are exposed if any requested/returned field differs.

## Phase, lifecycle and exact scale contract

Represent exact logical normalization with arbitrary-precision integers as one
canonical reduced positive rational `S` per pair. Existing `double`/`long
double` scale metadata is compatibility metadata, not the authority. Receipts
must be immutable and parent-derived; reject mismatched family, phase, basis,
divisor, tag, local level, arity, format, slots, noise degree, recorded scale or
exact rational.

For `S0 = 2^100`, actual `d`, and actual active prime `m_i` consumed from the
right by RS2:

`T_i = S_(i-1)^2 / d`

`S_i = T_i / m_i = S_(i-1)^2 / (d*m_i)`

Thus `S1 = 2^200/(d*m1)` and
`S2 = 2^400/(d^3*m1^2*m2)`. Derive these from actual prime integers; do not use
nominal `2^p`, `SetScalingFactor`, binary64, or a reset to `S0` as a substitute.

Preserve this family-relative phase table:

| Phase | basis / local level / arity | exact meaning |
| --- | --- | --- |
| input pair | `A_(i-1)` / 1 / `(2,2)` | RCB normalization `S_(i-1)`; high `S/d`, low contribution `S` |
| Tensor2 | `A_(i-1)` / 1 / `(3,3)` | `T_i`; no low-low term |
| raised high `d*H_T` | `B_(i-1)` / 0 / 3 | normalization `T_i`; appended d-tower is zero |
| Relin2 result | `A_(i-1)` / 1 / `(2,2)` | RCB normalization `T_i`; internal DCP remainder retained |
| RS2 result | `A_i` in old family / 2 / `(2,2)` | `S_i`; low is `RS(dH+L)-d*RS(H)` |
| re-entry | same `A_i` in `B_i` / 1 / `(2,2)` | identical coefficients and `S_i`, next family/tag |

Do not rescale low directly. Do not drop the internal Relin2 DCP remainder,
invent a fresh-DCP low bound, or treat high/low as independently encoded
messages. `RCB` must remain `d*high+low` for either family. Add/Sub must require
the same exact receipt and family. A staged/direct comparison remains a wiring
check, never the arithmetic oracle.

## The new semantic test

Name the new binding exactly
`repeated_mult2_semantic_two_square_contract`. Append it after the existing 57
name/command pairs. Use the frozen dyadic `X`, `Y`, `Z=X*Y`, `W=Z*Z`, and
distinguishing deltas from the supplied
`contracts/SECOND_MULT2_EXACT_VECTORS.json`. Preserve that input byte-for-byte;
if it is arithmetically wrong, stop with an independent counterexample rather
than silently editing it.

Freeze the diagnostic parameters before GREEN: N=64, batch=16, depth=9,
scaling bits=50, first bits=55, input exact scale `2^100`, FIXEDMANUAL, HYBRID,
COMPLEX, UNIFORM_TERNARY, `HEStd_NotSet`, four fresh root keypairs per test
invocation. This is functional/precision evidence only.

The test must:

- independently recompute every Z and W literal with exact dyadic integer
  arithmetic, including `Z[0]-Z[1]` and `W[0]-W[1]`, before evaluation;
- retain the existing test-only multiprecision DCRT fixture only as an input
  adapter. Its stale binary64 plaintext cache must never be read, serialized or
  called production I/O. Do not add a shipping codec in this slice;
- put all private-key use in a visibly separate client setup/final-oracle scope;
  call no decryption or re-encryption between the two evaluator `Mult2` calls;
- independently decrypt/reconstruct pair coefficients with root-secret
  polynomial arithmetic and CRT by actual prime/root identity, then evaluate
  canonical slots with multiprecision Horner and the exact `S1`/`S2` rationals;
- check all 16 complex slots after each stage, plus `Z[0]-Z[1]` after Z and
  `W[0]-W[1]` after W, at absolute error `<= 2^-80`, with no binary64 expected
  values;
- check actual `d,m1,m2`, family Q/root order, P/QP and alpha-one row shapes,
  family/tag/local-level/lifecycle/phase/arity/format/slots, exact rational
  receipts, and first-to-second re-entry;
- snapshot and prove immutability of X/Y/Z inputs, source pair wrappers,
  contexts/tables and owned/unrelated key rows; and
- print one parseable record per `{trial,stage}` containing test name, trial,
  stage, N/profile, family/tag, actual d/m, exact scale numerator/denominator,
  max-slot error and delta error. Label scope `low-N-two-operation-diagnostic`.

Do not accept unity-only operands, zero-only paths, staged/direct equality,
metadata-only relabeling, the shape probe, or expected rejection as semantic
success. Do not claim a universal all-key/no-wrap theorem; report observed
headroom as observed only.

## Allowed production and integration surface

Change only files needed under `project/include/openfhe_2023_1788/`,
`project/src/`, `project/tests/`, `project/CMakeLists.txt`, and
`project/.github/workflows/dcp-rcb.yml`. New small project-owned headers/sources
are allowed when ownership becomes clearer. Do not modify vendored/upstream
OpenFHE, unrelated tests, thresholds, old CTest names/commands, or old
assertions. Avoid duplicate family/profile/oracle logic where one narrow helper
has a single authority.

Add the new branch trigger and the same focused CTest step to both Linux and
Windows jobs. Add the new target to the warning-as-error compiler branch that
matches the host. Preserve the existing precision and pair-composition focused
steps, the five explicit API builds and the full suite.

## Hosted commands and expected observations

You may perform source inspection, bounded exact/rational calculations, patch
replay, hashes and other static checks. Do not compile OpenFHE/project code or
run cryptography on a Mac, access credentials/accounts, use the network, push,
merge, dispatch CI, contact another agent, or claim Codex hosted results. Mark
every unexecuted command `NOT RUN`.

Codex will later run both hosted jobs with pristine OpenFHE 1.5.0:

```text
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=<pristine-install>
cmake --build build --parallel 2
cmake --build build --target relin2_api_contract_test --parallel 2
cmake --build build --target rs2_api_contract_test --parallel 2
cmake --build build --target mult2_api_contract_test --parallel 2
cmake --build build --target add_api_contract_test --parallel 2
cmake --build build --target sub_api_contract_test --parallel 2
ctest --test-dir build --verbose --output-on-failure -R '^repeated_mult2_semantic_two_square_contract$'
ctest --test-dir build --show-only=json-v1
ctest --test-dir build --verbose --output-on-failure
```

Hosted acceptance, not something you may pre-claim: warning-clean default build
and five API targets on Linux/Windows; focused 1/1; full 58/58 with the original
57 exact name/command/order bindings followed by the new binding; four fresh
trials and both stage records in focused and full invocations on each host; all
arithmetic/profile/immutability checks pass. Any genuine anomaly remains in the
log and blocks acceptance.

## Required return archive

Return one downloadable ZIP plus a SHA256 sidecar. It must contain:

- `README.md` with exact source/pin, scope and honest status;
- `DESIGN_DECISION.md` describing the minimal public setup/evaluator ownership,
  family plan, exact receipt, legacy boundary and later-extension seam;
- `patches/0001-red-repeated-mult2-semantic-two-square.patch` and
  `patches/0002-green-repeated-mult2-semantic-two-square.patch`;
- `complete/project/...` with every complete changed/new file after GREEN;
- the byte-identical frozen vector JSON and a source-claim/test ledger with
  exact official/project file-line evidence;
- `EXPECTED_CTEST_BINDINGS.tsv` with all 58 exact name/command pairs in order;
- `RED_GREEN_EXECUTION_LEDGER.md`, separating observed static, patch replay,
  compile, runtime and hosted states without upgrading `NOT RUN`;
- `NEXT_GATES.md`: production lossless I/O, h=128, asymmetric paper primes,
  then eight semantic operations, then paper N/1000-run precision/performance;
  and
- a closure manifest with every payload path, byte size, SHA256, origin and
  source commit, plus package-integrity notes.

The implementation source is the tracked `80d771c` snapshot, while this task is
a separate handoff overlay prepared afterward and may be tracked only by a
later documentation-only dispatch commit. Record the task's own payload
hash/origin in the handoff manifest; never mislabel it as a blob from `80d771c`
or as an implementation-source change.

Verify in a scratch copy that RED applies to exact `80d771c`, GREEN applies only
after RED, the replay tree equals every returned complete file byte-for-byte,
and no unexpected file changes. Exclude `.git`, builds, caches, credentials and
machine state. Do not silently repair source evidence or the original prior
return.

If a source-level blocker prevents a correct GREEN, return the smallest exact
counterexample and blocked ledger. Do not substitute another shape probe,
metadata facade, refresh, plaintext shortcut or claimed run. The requested
deliverable is a semantic implementation candidate for independent Codex
review and hosted testing, not proof that repeated Mult2, eight squarings or the
paper reproduction is complete.
