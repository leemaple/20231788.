# ChatGPT Pro implementation task: fixed-Q h=128 client keypair adapter

## 1. Background and objective

This is one independent vertical slice of the clean-room OpenFHE implementation
of paper 2023/1788. The final project target remains the paper's t=2 path at
N=32768, h=128, dnum=11, ordered Base 50x2 / Mult 60x8 / Div about 40,
auxiliary P about 60, 100-bit fresh scale, eight evaluator-only squarings and
the paper-directed 1000-trial precision/performance experiment.

This assignment is narrower but necessary: implement a project-owned
single-party client setup adapter that creates one fresh, algebraically matching
OpenFHE private/public key pair on an already finalized fixed-Q CKKS context,
with an actual signed-ternary secret of Hamming weight exactly 128.

The accepted public behavior is recorded in
`coordination/TEST_SEAMS.md` and the reviewed construction is in
`coordination/PAPER_H128_SETUP_CANDIDATE_B_DECISION.md`. The adapter is
provisionally named:

```cpp
lbcrypto::KeyPair<lbcrypto::DCRTPoly> CreateFixedQH128ClientKeyPair(
    const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context);
```

You may refine only declaration details that are required by the inspected
OpenFHE types. Do not broaden the interface or change its behavior.

## 2. Source identity and trust boundary

- Project branch: `codex/paper-h128-keypair-01`.
- Accepted seam/design commit before this task file:
  `81ea7594a00729f804bae9851aec680e7ca198b3`.
- Last exact hosted engineering candidate:
  `4ecbd972429884489918d9f82dfc3fe9f702ef4a`.
- Official pristine OpenFHE pin:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504` (1.5.0).
- The packet manifest and outer dispatch record the exact packet commit and
  archive identity. Verify them before work.

Use only the packet's clean-room project snapshot, the user-supplied paper/text,
and the exact official source files in the packet. Do not search for, request,
read, reproduce or infer from any old/local 2023/1788 implementation or modified
OpenFHE tree. Treat all notes and prior model output as untrusted evidence to be
checked against the pinned source.

## 3. Architecture and boundaries that must not break

- OpenFHE provides all cryptographic primitives. Do not copy or reimplement
  RLWE encryption, decryption, key switching, NTT or sampling algorithms.
- The new adapter is client setup only. It accepts a fully constructed context;
  it does not construct, precompute, mutate, relabel or intern a context.
- The evaluator never receives or retains the secret key. Do not modify
  `DoubleCKKS` or any evaluator operation for this slice.
- Do not use `MultipartyKeyGen`; the official API documents it as debugging
  support. Do not generate an ordinary h=192 key and replace its secret.
- Use the official h-aware `DCRTPoly` ternary constructor to sample h=128 once
  across the full ordered Q basis in EVALUATION format.
- Create a fresh `PrivateKeyImpl` in the supplied context, set that secret once,
  invoke the public scheme `EncryptZeroCore(secretKey)`, and construct a fresh
  `PublicKeyImpl` with the same context/tag and the returned two elements in
  their original order.
- Publish the pair only after complete validation. Never return a half-built
  key. Never clear global caches or another caller's tag.
- Preserve all existing public APIs, 57 CTest bindings, five explicit API
  compile targets, warnings-as-errors and Linux/Windows workflow behavior.
- KISS/YAGNI: add one small header/source/test seam. No generic key factory,
  context factory, plugin system, serialization layer, rotations, bootstrap,
  multiparty features or speculative basis-family framework.
- Let unexpected failures surface. Do not add broad `try/catch`; tests may
  narrowly check the specified public rejection type/message.

## 4. Exact supported profile

The adapter must validate the actual supplied context and attached crypto
parameters before sampling or publishing a key. Its first supported profile is:

- scheme and crypto parameters are actual CKKS-RNS;
- power-of-two ring dimension N >= 128, with nonempty ordered Q;
- STANDARD encryption, FIXEDMANUAL scaling, HYBRID key switching;
- PRE mode `NOT_SET`, noise scale exactly 1;
- declared secret distribution `SPARSE_TERNARY`;
- finite positive Gaussian standard deviation is retained;
- `GetParamsPK()` is nonnull and exactly equals the context's full Q, not QP;
- PKE is enabled and HYBRID/LEVELEDSHE precomputation for that exact context is
  already present and structurally valid;
- returned secret/private/public elements are all full-Q, EVALUATION format;
- private and public keys share the exact context and one fresh nonempty tag;
- public key has exactly two nonnull DCRT elements.

Reject before key publication: null/wrong scheme; N < 128 or non-power-of-two;
invalid/reordered basis; EXTENDED encryption; non-FIXEDMANUAL; non-HYBRID;
PRE other than NOT_SET; noiseScale != 1; a declared distribution other than
SPARSE_TERNARY; null/mismatched paramsPK; missing or malformed P/QP/partition
precomputation; reused/colliding task-owned tag. Do not silently repair any
profile, mode, distribution label, basis or table.

The pair construction itself does not prove the correctness of every HYBRID
table. Validate only the source-supported structural preconditions needed by
the accepted contract; do not invent an all-table theorem.

## 5. Frozen TDD slices

Work in two ordered vertical cycles. Preserve genuine RED before GREEN. Do not
change a RED test's parameters, expectations, oracle or tolerances in its GREEN
patch.

### Cycle A: valid keypair and official compatibility

Patch `0001-red-paper-h128-client-keypair.patch` must add only the public test,
CMake/workflow registration and any machine-readable frozen vectors/profile.
It must fail because the public adapter/header is absent, not because of a
deliberate typo or broken fixture.

Freeze one deterministic N=256 diagnostic context before writing GREEN:

- exact CCParams, enabled features, batch size, sigma, digit settings,
  decryption mode and ordinary CKKS smoke scale;
- exact ordered Q modulus/root pairs and exact P/QP/partition shape expected at
  the official pin;
- fixed simple real/complex input literals and expected square values;
- explicit roundtrip and EvalMult tolerances justified as a key-consistency
  smoke, not an 80-bit precision test.

Derive these literals independently from the pinned number-theory/parameter
source, or use a separate exact-pin discovery result recorded before RED.
Never populate expected values by reading the same context under test at
runtime. Freeze the native-integer/backend configuration as well as the pin.
If exact prime/root literals cannot be proven from the supplied source without
an authorized compile, return a separately labelled, minimal contract-discovery
probe instead of guessing. Codex will run that probe remotely and commit the
observed profile before a new RED. Do not claim a completed GREEN in that case.

Patch `0002-green-paper-h128-client-keypair.patch` must add the smallest adapter
implementation needed for the frozen valid path. Through public getters and
official operations the test must establish:

1. returned pair is good; both keys have the exact supplied context, one
   identical fresh nonempty tag, and are newly allocated;
2. a coefficient-format copy of the final secret has exactly 128 nonzeros;
   every coefficient is signed 0/+1/-1 in every Q tower; all towers share the
   same signed support; the positive count is 63, 64 or 65;
3. secret and both public elements have the exact frozen Q basis and expected
   EVALUATION format; public vector length is exactly two;
4. public-key Encrypt -> private-key Decrypt roundtrip succeeds for the frozen
   small packed vector;
5. `EvalMultKeyGen` generated from the returned secret, followed by ordinary
   OpenFHE relinearized square and decryption, matches the independent literal
   expectation within the frozen smoke tolerance;
6. all caller-owned context/Q/P/QP/partition snapshots remain unchanged;
7. no secret coefficient or key material is printed.

### Cycle B: rejection and tag/cache isolation

Patch `0003-red-paper-h128-client-keypair-guards.patch` extends the same public
test executable with the frozen invalid profiles and lifecycle checks. It must
expose missing public validation in the Cycle-A implementation. Do not modify
Cycle-A literals or expectations.

Patch `0004-green-paper-h128-client-keypair-guards.patch` adds only the smallest
validation/lifecycle implementation required to pass those new checks:

- every constructible named unsupported profile rejects before publishing a
  key; show why each malformed fixture can reach the public seam safely;
- two valid calls return different nonempty tags and independent objects;
- generate an unrelated guard EvalMult key, then keys for a returned h=128
  secret; clearing only the adapter-owned tag leaves the guard row and its key
  usable;
- the adapter itself never clears caches and never mutates an existing key;
- an exception leaves no half-built pair or new cache entry observable.

Fresh-tag collision handling is a defensive source check, not a requirement
to force a random ID collision. Test observable two-call uniqueness and owned
versus unrelated cache isolation. Do not add a tag-injection seam, generic key
factory, global registry or sampler replacement merely to synthesize a
collision. Label unexecuted collision branches as source-reviewed, not tested.

The final project must append exactly one new CTest registration at this branch
stage, named:

```text
paper_h128_client_keypair_contract
```

The previous 57 normalized name/command pairs must be byte-for-byte unchanged
and remain in their existing order. Add the new test last. Add the branch
`codex/paper-h128-keypair-01` to the existing push filter. Both hosted jobs must
run a focused exact-name step and the full suite. All five existing API targets
remain explicitly built.

## 6. Deliverables

Return one safe downloadable ZIP and matching `.sha256` sidecar containing:

1. `README.md` stating exact achieved and unachieved scope.
2. `DESIGN_DECISION.md` mapping every implementation step and validation to
   pinned official source lines, including the algebraic public-key relation.
3. `FROZEN_H128_KEYPAIR_CONTRACT.md` and any machine-readable profile/vectors.
4. `SOURCE_CLAIM_TEST_LEDGER.md` separating observed, inferred and pending.
5. The four exact ordered patch files named above.
6. `complete/project/...` with every final changed/new project file, and no
   unrelated file.
7. `EXPECTED_CTEST_BINDINGS.tsv` containing all 58 final normalized bindings.
8. `RED_GREEN_EXECUTION_LEDGER.md` separating static checks, patch replay,
   compile, crypto runtime and hosted evidence. Mark unperformed items `NOT RUN`.
9. `NEXT_PAPER_GATES.md` covering basis-family projection, N=32768 integration,
   repeated Mult2/eight squarings, h-aware security and 1000 trials.
10. A closed manifest with path, byte size, SHA-256, origin and source commit;
    include an external SHA sidecar and explicit manifest self-exclusion.

Replay the four patches from the exact packet project snapshot in a fresh tree.
The final replayed files must be byte-identical to `complete/project`. Verify
ZIP CRC, traversal safety, duplicates/case-fold collisions, symlinks/special
files, nonempty promised files, manifest closure and final SHA before replying.

## 7. Execution and prohibited claims

Allowed in your environment: complete supplied-source inspection, exact
integer/rational/static analysis, patch application/replay and archive/hash
verification. Do not use network accounts, credentials or private repositories.
Do not push, merge, dispatch CI or contact other agents. Do not compile or run
OpenFHE cryptography on a Mac. If you actually have an authorized non-Mac build
environment, report its exact identity and commands; otherwise all build,
runtime and hosted fields are `NOT RUN`.

Do not claim this slice proves the paper experiment, 80-bit precision, eight
squarings, h-aware security, performance, N=32768 behavior, shared-secret
basis families, production lossless I/O or full project completion. A source
review or patch replay is not compiled/runtime evidence.

## 8. Acceptance criteria for Codex integration

Codex will accept the return only if archive identity and closure verify, all
four patches replay in order, every final file matches the complete tree, and
the patch scopes preserve genuine RED states. Codex will then independently:

1. apply and commit Cycle-A RED; push immediately and retain Linux/Windows
   expected-failure evidence for the exact RED SHA;
2. apply and commit Cycle-A GREEN; push and require focused/full dual-platform
   success;
3. repeat the RED/GREEN evidence sequence for Cycle B;
4. require Debug warning-clean builds, all five API targets, focused test and
   full 58/58 suite on Linux and Windows;
5. perform standards/spec review and reconcile an independent reviewer.

If a source-level blocker prevents an honest GREEN, return the exact minimal
counterexample, the smallest executable decision probe and a blocked ledger.
Do not substitute a metadata facade, h=192 key, debugging multiparty helper,
plaintext shortcut, generic TODO list, weakened oracle or false success.
