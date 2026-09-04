# Independent static review — Pair Add/Sub boundary (t=2 Double-CKKS, paper 2023/1788)

## 1. Actual model, environment and seat

- Seat: ZCode static-review seat (CLI agent), launched from
  `LOCAL-REVIEW-TASK.md` in the dedicated folder
  `/Users/lifeng/Documents/20231788-openfhe-zcode-pair-review-20260904`.
- Underlying model as reported by this harness environment:
  `builtin:bigmodel-coding-plan/GLM-5.3` (GLM-5.3). This is the identity the
  UI/provider exposed to this session; I cannot independently verify
  provider-side routing beyond that environment string. This review is NOT
  Fable 5.1 and NOT ChatGPT Pro, and no Fable review is invented here.
- Host activity: reading supplied files, SHA/Git-blob recomputation with
  Python 3 standard library, and text diffing only. No project or OpenFHE
  configure/compile/CTest, no cryptographic execution, no benchmark, no
  dependency install, no source/Git change, no CI dispatch, no browser, no
  other agent, no credentials or user state, no other local project, and no
  old implementation or modified OpenFHE was inspected. Supplied scripts and
  binaries were never executed.

## 2. Scope and source identity

Scope is ONLY the mandatory six-question audit of the componentwise Pair
Add/Sub boundary on pristine official OpenFHE 1.5.0 (pin
`df495ba2e91739a6dc8f1de254fc5a41155ce504`), as defined by the extracted
`TASK.md`. This is an independent static review — not implementation, not
project acceptance.

- Selected/tested combined source: `d73824c2d382013c3aadbd7cb29c57008e839714`,
  branch `codex/integration-01` (manifest `testedSourceCommit`).
- Snapshot/documentation head: `f550eac1251f2005222e60aa4f07cc2e57380c46`
  (manifest `sourceCommit`). No documentation-only substitution was detected:
  the audited `project/src/double_ckks.cpp` and `project/tests/*` match the
  delta/log narrative of the tested source.
- Merge parents `7afb77d496e606efcaca71767913ef51221ced09` (RS2+Mult2) and
  `613064117e980d30244dfd7c53915d0869a54a89` (Pair Add/Sub, tested source
  `4b170183f29b415329c232a17ea1924acdd0d954`), merge base
  `7041a489ae1afa98b75322ec334543f29f10b738` — as recorded in the manifest
  and CROSS_BRANCH_INTEGRATION_02.md. These commits are quoted packet data;
  I had no Git access and did not re-derive them.

## 3. Input verification (before and after review)

Archive `pair-combined-review-d73824c.zip`:

- Size 1,305,833 bytes and SHA256
  `e3dd499889e66a3406fa8ca755b559505db802c2d4cd7c8e1615d74900225fce` — both
  match the launcher exactly (recomputed with `shasum -a 256` and Python
  hashlib).
- Safe entry listing before extraction: 91 entries = 71 regular files + 20
  directories, no absolute or `..` paths, no backslash/colon tricks, no
  duplicate names, no symlink or non-regular external attributes.
  Extraction confined to `extract-d73824c/` in this folder; no supplied
  script or binary executed.
- `SOURCE-MANIFEST.json` closure: 70 payload entries + the manifest itself =
  the 71 extracted regular files, exact set match both directions. Every one
  of the 70 payloads recomputed to its recorded size and SHA256 — zero
  mismatches.
- Nested original Pro return `ORIGINAL-PAIR-PRO-RETURN.zip` matches its
  separately recorded identity: 212,032 bytes, SHA256
  `735dea4e6c164ced95c2829ea8eb5316201eb900fd5d77b1aad171e94e2676c4`, 88
  entries / 66 regular files. Its per-file `FILE_HASHES.sha256` was not
  recomputed file-by-file (not required by the launcher; outer identity
  verified).
- `OFFICIAL-REFERENCE-PROVENANCE.json`: for all four new official files
  (`ciphertext.h`, `metadata.h`, `cryptoobject.h`, `evalkeyrelin.h`) the
  recorded bytes, SHA256 and Git blob SHA-1 (`git hash-object` construction)
  all recompute locally. The other official/paper files carry the earlier
  pinned-manifest verification as quoted provenance; I verified their
  internal manifest hashes but did not re-fetch any upstream URL (offline
  seat — see §10 limits).
- Post-review immutability re-check (after all reading): outer ZIP hash/size
  unchanged; all 70 manifest payloads re-verified unchanged; extracted set
  still exactly 71 files. Inputs were not modified by this review.

## 4. Independent derivation from the paper (before consulting prior conclusions)

From PAPER-2023-1788 (§2.1, §3.2, §4, §4.2–4.3; extracted text verified
against the packet copy):

- §2.1: `Add/Sub(ct, ct')` returns `[ct + ct']_{Q_l}` resp. `[ct − ct']_{Q_l}`
  — left minus right — with decryption linearity.
- §4 preamble: "homomorphic multi-precision addition and subtraction can be
  performed componentwise on pair representations of ciphertexts", i.e. for
  pairs `(hL,lL)`, `(hR,lR)`: result = `(hL ± hR, lL ± lR)` — high with high,
  low with low, at unchanged modulus `Q_l`, unchanged level, unchanged
  degree-2 shape, no key switching, no scale change, no normalization, and no
  cross-term. The low-part-drop ("low-low") rule belongs exclusively to
  Tensor2/§4.2 multiplication; §4.3 calls the addition/subtraction analysis
  "elementary" — no special handling is licensed.
- Def 3.3/§3.2: `RCB(h,l) = q_div·h + l`, and RCB is linear, so the exact
  public invariant is `RCB(L ± R) = q_div·(hL ± hR) + (lL ± lR)` per ring
  element. This is the oracle equation the tests must implement independently.

Derived falsifiable counterexamples used to judge the tests: swapping the
Subtraction operands must change the result wherever `hL ≠ hR` or `lL ≠ lR`;
an implementation that rescaled, switched keys, mixed components, or touched
scale/level metadata must fail either the exact residue oracle, the manifest
checks, or the RCB oracle.

## 5. Answers to the six mandatory questions

### Q1. Paper/source correctness of Add/Sub — PASS

`project/src/double_ckks.cpp:657-682` (Add) and `:684-708` (Sub):

- Both clone the corresponding LEFT member
  (`left.high_->Clone()`, `left.low_->Clone()`), then for each RLWE component
  index apply `highElements[i] += rightHighElements[i]` /
  `lowElements[i] -= rightLowElements[i]`. This is exactly high/high and
  low/low ring arithmetic.
- Official operator policy traced, not assumed: pinned
  `official-openfhe/dcrtpoly-impl.h:374-380` (`operator+=`) and `:402-408`
  (`operator-=`) are per-RNS-tower modular add/sub over `m_vectors[i]` with
  no modulus change, no rescale, no key access, no format change — the exact
  `[ct ± ct']_{Q_l}` of §2.1. Because compatibility enforces identical
  ordered bases, per-tower modular arithmetic is well-defined.
- Subtraction order is left minus right (`-=` on the left clone), matching §2.1.
- No hidden rescale (no `context_->Rescale`), no key switch (no
  `Relinearize`), no convenience `EvalAdd/EvalSub` that could align levels or
  scales, no cross-component mixing, no low-low rule, no normalization: the
  result manifest copies `left`'s divisor/basis/level/paper-scale/recorded
  scale/noise degree/lifecycle/key tag/slots/format/component count verbatim
  (lines 676-679, 702-705) and `ValidatePair(result)` re-derives all of them
  from the bound context.
- Clone policy traced from pinned `official-openfhe/ciphertext.h:390-406`:
  `Clone()` = `CloneEmpty()` + value copy of `m_elements`;
  `CloneEmpty()` re-copies scalar metadata and assigns a fresh outer metadata
  map. Deep DCRT value copy + fresh scalars is exactly what Add/Sub mutate.
- Boundary preserved: `Tensor2` still requires `ReadyForFirstMult`
  (`double_ckks.cpp:766-769`), `RS2` still requires `ReadyForRS2` (`:993`),
  `RCB` accepts every valid lifecycle (`:1119-1130`, only `ValidatePair`);
  Add/Sub copy `left.lifecycle_` unchanged, so the second-multiplication
  prohibition and the RCB-accepts-all-states boundary are untouched.
- Delta verification: `PAIR-PRODUCTION-DELTA.patch` is insertion-only (110
  added lines, 0 removed) = 4 header lines + 106 source lines, and the 106
  source lines are byte-identical to current `double_ckks.cpp:657-762`
  (programmatically compared). No production arithmetic outside Add/Sub/
  ValidatePairCompatibility changed relative to the merge base.
- Combined source retains the later RS2 validation fixes and Mult2
  composition: RS2 (`:991-1113`) still carries the odd-`q_l`, `q_l ≠ q_div`,
  mod-reduce-factor index/basis cross-checks, finite recorded factor,
  per-stage `ValidateCiphertext`, the two independently rounded Rescale calls
  of Definition 4.5, and the basis-matched low subtraction;
  `Mult2` (`:1115-1117`) is literally `RS2(Relin2(Tensor2(left, right)))`.

### Q2. Contract order and full compatibility audit — PASS

Code order at `double_ckks.cpp:658-660` and `:685-687` is
`ValidatePair(left)` → `ValidatePair(right)` → `ValidatePairCompatibility` →
clone/arithmetic → `ValidatePair(result)`.

`ValidatePair` (`:440-572`) audits, per operand: bound-context identity,
divisor equality with the bound context, componentCount 2 + EVALUATION
format, lifecycle-specific level (1 or 2) and ordered basis (full or
truncated `firstPairModuli_`), lifecycle-specific exact recorded scale,
noise-scale degree, all four paper-scale descriptor fields (consistency
re-derived from context parameters), non-empty key tag, and per-member
`ValidateCiphertext` (`:280-353`: null, context, CKKS packed encoding,
level, component count, degree, exact recorded scale, key tag, slots,
level/basis agreement, per-element format, per-tower root-of-unity and
cyclotomic order against the bound context, declared RNS basis equality).

`ValidatePairCompatibility` (`:710-761`) additionally audits every applicable
mutual condition: context, lifecycle, divisor, ordered basis, level,
recorded scale, noise-scale degree, all four paper-scale fields, key tag,
slots, format, component count.

Test discrimination (all with exact-diagnostic assertions and full
snapshot-verified non-mutation after rejection):

- Right-validation-precedes-compatibility: right is a genuine different-key
  pair whose descriptor is also corrupted; the expected diagnostic is the
  right-validation message, not the key-tag compatibility message
  (`pair_arithmetic_test.cpp:1255-1265`).
- Left-before-right: BOTH operands corrupted with DIFFERENT defects (left
  recombined-scale vs right logical-scale); the expected diagnostic is
  left's, and the two possible diagnostics differ textually, so a
  right-first validator cannot accidentally pass (`:1266-1278`).
- Genuine valid-but-incompatible inputs: mixed lifecycles from the real
  DCP→Tensor2→Relin2→RS2 pipeline (`:1084-1087`); two genuine key
  generations (`:1047-1054, 1088-1091`); genuine eight-slot vs four-slot
  plaintext encodings, both individually passing DCP and RCB
  (`:1056-1067, 1092-1095`); foreign-context pair (`:1069-1099`).
- Deliberately malformed fixtures (test-owned corruption, see Q5):
  divisor, ordered basis, each paper-scale field, member level/degree/
  recorded scale/key tag/slots/encoding/component count/tower format
  (`:1101-1254`) — each expecting the exact validation diagnostic.

Observation (not a defect): for two individually-valid pairs of the same
module, the compatibility branches for context, divisor, basis, level,
recorded scale, noise degree, paper-scale fields, format and component count
are unreachable, because `ValidatePair` pins each to the same context-derived
value; the only reachable genuine incompatibilities are lifecycle, key tag
and slots — precisely the three the matrix exercises. The unreachable
branches are defense-in-depth should validation ever loosen. Correctly, the
tests do NOT demand an impossible valid mismatch (the foreign-context case
surfaces through right-validation, the only observable path) and no mutable
public factory was introduced anywhere in the delta.

### Q3. Oracle independence and discrimination — PASS

`pair_arithmetic_test.cpp`:

- Member oracle (`:515-547`): for each RLWE component (both) and each ring
  coefficient, reconstructs signed cpp_int values of left and right via an
  independent CRT (own extended-GCD modular inverse, `:306-356`), computes
  `expected = BigInt(left ± right)` with materialized Boost branches
  (`:508-513`), and asserts every native-tower residue of the actual member
  equals `expected mod q_tower`. Exact — no tolerance. Expected values never
  come from production Add/Sub/EvalAdd/EvalAddInPlace/DCRT arithmetic.
- RCB oracle (`:549-581`): recomputes per coefficient
  `q_div·(highL ± highR) + (lowL ± lowR)` from the operand members and checks
  the public `module.RCB(result)` residues per tower — the §4/Def 3.3
  linearity invariant, with production RCB as the quantity under test, not
  the source of expected values.
- Representation note: `ToCoefficient` uses OpenFHE `SetFormat` (evaluation →
  coefficient). This is a representation adapter permitted by
  TEST_SEAMS.md; because Add/Sub are linear and the check compares residues
  of identically transformed polynomials, the oracle is invariant under that
  linear change of representation.
- Signed wrap witnesses (`:438-455`): 0, ±1, centered half-modulus boundary
  values, half±small offsets that wrap the centered representative in both
  directions, and distinct nonzero pairs ({2,−3}, {−5,7}) — exact modular
  wrap correctness is verified per tower, not approximated.
- Distinct high/low: high and low use different phase offsets
  (`:489-496`) and the test asserts high ≢ low element-wise (`:1302-1304`),
  so a high/low swap cannot pass.
- Alias/self: `Add(left,left)` and `Sub(left,left)` are oracle-checked with
  the same operand on both sides; self-Sub is additionally asserted to be
  exactly zero in every tower/coefficient of both members (`:1353-1360`);
  result members are asserted not to alias either input, each other, or to
  share a metadata map (`:650-658`).
- Reverse operands: `Add(right,left)` value-commutativity asserted;
  `Sub(right,left)` asserted genuinely different from `Sub(left,right)`
  (`:1343-1351`) — the falsifiable operand-order counterexample.
- Public encrypted lifecycles: all three lifecycles exercised on genuinely
  encrypted REAL/COMPLEX fixtures with the full oracle + manifest +
  non-mutation battery (`:877-911, 976-981`).

Discrimination limits (what these witnesses cannot show — bounded, not
defects): Add is commutative, so no value-level witness can distinguish Add
operand order (none exists mathematically; metadata provenance would still
reveal a right-clone policy). The exact-residue identity cannot detect a
defect that is simultaneously identity on every member, tower, coefficient
and on RCB — no such defect is known and none is claimed to be excluded by
any single witness. The decrypt-based runtime checks (`1e-6`, finite guards)
are functional-accuracy checks, not precision-bit or security claims, and
cannot by themselves distinguish Add operand order. No composed
Add-then-Tensor2/Mult2 path is exercised in the retained suite (Add
preserves `ReadyForFirstMult`, so such a composition is representable but
untested — a bounded blind spot, listed with recommendations).

### Q4. Mutation/key independence/provenance — PASS

- Fixture key surgery (`:913-969`): after full lifecycle preparation (and
  only then), the fixture's `EvalMultKey` row is removed via
  `ClearEvalMultKeys(fixtureTag)`; an unrelated genuine BV guard row from a
  separate context/key generation remains. Removal specificity asserted
  before and after.
- Snapshots retain VALUES, not just identities: `CiphertextSnapshot`
  (`:169-202`) keeps the shared identity, a deep `Clone()` value copy,
  scalar metadata, a metadata snapshot holding map identity + per-entry
  identity + per-entry deep `Clone()` value, and the full per-tower
  coefficient vector (modulus, root, cyclotomic order, format, all native
  values). `CheckCiphertextUnchanged` (`:204-230`) verifies identity,
  scalars, semantic equality with the deep clone, per-component DCRT value
  AND pointed-to parameter identity, and metadata map/entry/deep-value
  equality. `EvalKeyCacheSnapshot` (`:766-875`) retains the static cache map
  and row identities plus full A/B DCRT values of every retained key;
  `ContextParameterSnapshot` (`:714-762`) retains crypto/element/tower
  parameter identities AND their native values (moduli, roots, cyclotomic
  orders) for both contexts.
- Both operands, both contexts and the key cache are verified unchanged
  after every Add/Sub/RCB window, in the lifecycle battery, the rejection
  matrix, and the controlled battery (`:909-910, 983-987, 1280-1283,
  1364-1365`).
- Official Clone semantics verified from the pinned sources, not assumptions
  or a sliced accessor: `ciphertext.h:390-406` — `CloneEmpty` copies scalars
  and performs `*(ct->m_metadataMap) = *(m_metadataMap)`, a fresh outer map
  whose entry `shared_ptr<Metadata>` values are shared with the source
  (and the copy/move constructors at `:99/:116` share even the outer map);
  `metadata.h:44-75` — `MetadataMap` is a
  `shared_ptr<map<string, shared_ptr<Metadata>>>` and `Metadata::Clone()` is
  virtual with a fresh-object base behavior. `GetHigh()/GetLow()` return
  read-only views; no sliced accessor is used anywhere.
- The tests assert exactly this and no more: the result's metadata map is a
  NEW map (≠ source map identity), entries preserve the left member's entry
  identities (shared pointers) and deep-equal values
  (`CheckMetadataDerivedFrom`, `:104-121`), and per-tower parameter
  provenance is the left member's (`CheckMemberStructureDerivedFrom`,
  `:583-617`). Per-call nonmutation of both operands is proven; arbitrary
  future caller-mutation isolation is correctly NOT claimed.
- Enumerated blind spots: metadata entry pointers (and DCRTPoly::Params
  pointers) remain shared between result and left operand by official
  design — a future mutating caller of the result's metadata entries would
  affect the left operand; the immutability window opens only after
  lifecycle preparation and targeted key removal (preparation-pipeline
  mutations are outside the attribution window, deliberately); only the
  `EvalMultKey` static cache is snapshotted (rotation-key caches are not —
  Add/Sub/RCB perform no key switching); hidden context fields beyond the
  enumerated native parameters are not claimed.

### Q5. Codex integration corrections vs the original Pro return — ALL VERIFIED

Compared the complete original return (extracted from the verified
212,032-byte `ORIGINAL-PAIR-PRO-RETURN.zip`) with the current tests:

1. `HasNonzeroValue`: Pro patch0009 calls it (patch line 739) without
   defining it; the definition first arrives in Pro patch0010 (line 106).
   The integrated file defines it exactly once
   (`pair_arithmetic_test.cpp:690-704`), before both uses — the controlled
   slice is self-contained, no duplication (grep across `project/` confirms
   a single definition).
2. `ExpectedArithmetic`: Pro's final file returns
   `kind == Add ? left + right : left - right` (final-project test line
   506-508) — a conditional over Boost cpp_int expression templates. The
   integrated version materializes explicit `BigInt(left + right)` /
   `BigInt(left - right)` branches (`:508-513`). Mathematical values are
   unchanged; only the ill-advised conditional over expression templates
   was removed.
3. Public-lifecycle fixture: Pro patch0010 builds the fixture context with
   `MakeContext(lbcrypto::HYBRID)` — the REAL default. Pinned
   `ckkspackedencoding.h:89-102` proves the REAL constructor path zeroes
   every imaginary component, so the complex `rightValues` witness would
   have been silently discarded. The integrated test selects
   `MakeContext(lbcrypto::HYBRID, 8, lbcrypto::COMPLEX)`, asserts
   `GetCKKSDataType() == COMPLEX`, and asserts the literal nonzero 0.125
   imaginary component survives in the constructed plaintext cache before
   encryption (`:919-938`).
4. Slot mismatch: Pro patch0011 encrypts the same plaintext twice and calls
   `slotRightInput->SetSlots(GetSlots() - 1)` — a metadata relabel, not a
   genuine different encoding. The integrated test creates a genuine
   four-slot plaintext via the public constructor's explicit slots argument
   and asserts 8 vs 4 real slot counts, both operands independently passing
   DCP and RCB first (`:1056-1067`).
5. Ordering witness: Pro patch0011's left-order witness corrupts only the
   left descriptor with a valid right operand — it proves rejection of the
   malformed left, not order (a right-first validator would emit the same
   eventual diagnostic and accidentally pass). The integrated witness makes
   BOTH descriptors invalid with DIFFERENT exact diagnostics (left
   recombined-scale vs right logical-scale) so right-first validation
   produces the wrong message and fails (`:1266-1278`), complemented by the
   right-validation-precedes-compatibility witness (`:1255-1265`).
6. const_cast/const_pointer_cast well-definedness: every corrupted object is
   a locally created, non-const `auto` copy (`auto right = makePair();`,
   `auto right = secondKeyPair;` — copies, not the const originals) or an
   internally cloned ciphertext whose pointee was created non-const by DCP;
   mutations through the cast-away constness are therefore defined, the
   objects are isolated to their block, and null checks precede use. No UB
   or null fixture found. No production arithmetic was changed: the delta
   contains only Add/Sub/ValidatePairCompatibility (Q1 verification).

The coordination trail (`PAIR_ORACLE_PREFLIGHT.md`,
`PAIR_ARITHMETIC_INTEGRATION.md`) records each correction as a preflight
static fix — explicitly not as observed compiler/runtime reds — consistent
with what the patches and current tests show.

### Q6. Combined regression and honesty — PASS

- CMake (`project/CMakeLists.txt`): 53 `add_test` registrations,
  programmatically counted, no duplicate names. Both parents' families are
  present: dcp_rcb(1), pair_add/pair_sub/3×pair_arithmetic (6, incoming
  parent), tensor2(5), relin2(31), rs2(6), mult2(5). Warning-as-error is
  applied to ALL 16 targets on both compiler families (MSVC `/W4 /WX`;
  GNU/Clang `-Wall -Wextra -Wpedantic -Werror`) — both parents' warning
  settings survive the merge. API contract executables remain compile-only
  and deliberately outside CTest, built explicitly by CI.
- Workflow (`project/.github/workflows/dcp-rcb.yml`): both branch allowlists
  (including `codex/integration-01` and `codex/pair-arithmetic-01`);
  OpenFHE pin `df495ba...` checked out and verified by rev-parse +
  dirty-status on both hosts; Windows additionally verifies the project
  checkout equals `GITHUB_SHA`; verbose CTest; no cancel-in-progress, no
  continue-on-error, no threshold/parameter/backend/security change.
- Combined run 33854419062 (quoted hosted evidence — not my execution):
  Linux log shows the default warning-clean build of all targets, explicit
  Relin2/RS2/Mult2/Add/Sub API target builds, and
  `100% tests passed, 0 tests failed out of 53`; Windows log shows
  `100% tests passed out of 53` with the same five API builds and the run
  id embedded in its build paths (`project-build-33854419062-1`). The exact
  53 CTest names in both logs equal the 53 CMake registrations — full
  name-closure verified programmatically, no missing/extra/duplicate case.
- Red/green authenticity: the retained Add/Sub reds are distinct genuine
  stages, not relabelings — Sub API red is a real compile failure
  ("'Sub' is not a member of 'openfhe_2023_1788::DoubleCKKS'",
  pair-sub-api/red-linux.txt:191) preceding the scaffold; Add runtime red is
  41/42 with the sole failure `DoubleCKKS: Add is not implemented`
  (pair-add-runtime/red-linux.txt:87-88); Sub runtime red is 42/43 with
  `DoubleCKKS: Sub is not implemented` (pair-sub-runtime/red-linux.txt:89-90);
  greens are 42/42 and 43/43 on both hosts with identical literal vectors
  and tolerance. Timestamps increase monotonically across stages
  (05:14 → 05:19/05:24 → 05:40 → 06:03 → 06:06/06:12 UTC on 2026-09-04).
  The later 44/45/46-test slices and the 53-test merge are labeled
  additional regression coverage of already-red-green behavior — the ledger
  never presents them as new missing-feature red-green cycles, and the
  controlled/lifecycle matrices are quoted with their own run ids
  (33843650508 44/44, 33850393475 45/45, 33852796677 46/46, both hosts).
- No weakening anywhere I compared: tolerances (1e-6) and literal vectors
  are identical in red and green stages; warning settings extended to new
  targets only; no test removed, renamed or disabled in the merge.

## 6. Findings

No defect requiring a source change was found in the audited Add/Sub
boundary. Findings below are confirmations and bounded observations.

| # | Severity | Location | Finding |
|---|----------|----------|---------|
| F1 | Confirmed-OK | `src/double_ckks.cpp:657-708` | Add/Sub implement the paper equation exactly (componentwise per-tower modular ±, left-minus-right, clone-left, no rescale/key-switch/alignment/normalization/low-low rule); result revalidated. |
| F2 | Confirmed-OK | `PAIR-PRODUCTION-DELTA.patch` | Insertion-only 4 header + 106 source lines, byte-identical to current source 657-762; no other production change. |
| F3 | Confirmed-OK | `double_ckks.cpp:991-1117` | RS2 validation fixes and the literal Mult2 composition survive the merge. |
| F4 | Minor / bounded | `double_ckks.cpp:713-733` | Several `ValidatePairCompatibility` branches (context/divisor/basis/level/scale/degree) are unreachable for two individually-valid pairs of one module because `ValidatePair` pins those fields to context-derived values. Defense-in-depth; tests correctly exercise only the reachable genuine mismatches (lifecycle/key tag/slots) and do not demand an impossible valid mismatch. No action required. |
| F5 | Minor / blind spot | retained suite | No composed Add-then-Tensor2/Mult2 regression exists: an Add result at `ReadyForFirstMult` is never fed into multiplication. Values/provenance imply it must work, but the composition is untested evidence. Optional recommendation R1. |
| F6 | Minor / cosmetic | `tests/pair_add_test.cpp:91-97` | Dead scaffold-detection branch: the compared string `"DoubleCKKS: Add arithmetic is not implemented"` differs from the historical scaffold message (`"DoubleCKKS: Add is not implemented"`). Harmless — both catch arms fail the test — but the branch can never distinguish anything now. Optional cleanup only; no correctness impact. |
| F7 | Confirmed-OK | `ciphertext.h:390-406`, `metadata.h:44-75` | Official Clone shares metadata entry pointers and parameter provenance while copying the outer map and element values; the tests assert exactly this semantics and claim only per-call nonmutation — no isolation overclaim. |
| F8 | Limitation | all logs | Hosted run identities (33854419062 etc.) are quoted evidence. The retained Linux combined log section begins at project configure, so its head_sha/checkout line is not in-section; run identity is corroborated by the ledger URL/claim and by the Windows log's run-id-bearing build paths. I performed no network access. |

Falsifiable counterexamples considered and excluded: operand swap in Sub
(caught by `Sub order was not distinguished`, test line 1349-1351, since
high/low differ from their counterparts); high/low member swap (caught by
per-member exact residues with phase-distinct members); any rescale/key
switch/metadata drift (caught by exact manifest revalidation, scale/level/
degree assertions and the key-cache/context snapshots); partial arithmetic
(e.g., adding only one RLWE component, or only high) — caught by the
per-component/per-tower exact residue oracle.

## 7. Exact tests and claims covered, and blind spots

Covered by this audit: the six Add/Sub-relevant CTest entries
(`pair_add_runtime_behavior`, `pair_sub_runtime_behavior`,
`pair_arithmetic_controlled_oracle`, `pair_arithmetic_public_lifecycles_keyless`,
`pair_arithmetic_compatibility_rejections`, plus `dcp_rcb` for RCB context)
and the five compile-only API contracts, as wired in CMake and executed in
run 33854419062 on both hosts (quoted evidence).

Explicitly NOT covered / remaining open (consistent with TASK.md; these are
separate active tasks, not reasons to stall this review):

- True high-precision I/O (>53-bit canonical precision) and repeated
  multiplication beyond the first Mult2.
- Conservative BV `E_Relin` bound: the Mult2 per-path certificate remains
  conditional (PER_PATH_CONDITIONAL) and the universal theorem UNPROVED.
- Paper-scale security and performance verification.
- The Add→Mult2 composition case (F5/R1).
- Hidden OpenFHE context/cache fields beyond the enumerated snapshots;
  metadata entry-pointer sharing semantics (F7) as a future-caller
  consideration.
- Anything requiring execution: all run/test results above are hosted,
  quoted, and were not reproduced in this seat.

## 8. Execution statement

No build, test, cryptographic operation, benchmark, script or binary was
executed in this review seat. All 53/53 (and 41/42, 42/42, 42/43, 43/43,
44/44, 45/45, 46/46, 48/48) outcomes are quoted hosted evidence from the
retained logs and dated ledger, cross-checked internally (counts, names,
messages, timestamps, run-id-bearing paths) but not re-derived by execution.
"No issue found" here is not exhaustive proof — it is the absence of
findings within the audited boundary and evidence.

## 9. Verdict

**PASS_WITH_GAPS** for the audited Pair Add/Sub static-review boundary:
paper-correct arithmetic, complete ordered contract with genuine rejection
matrices, independent exact oracle with discriminating witnesses, verified
per-call nonmutation with honest provenance semantics, faithfully integrated
Pro corrections, and internally consistent combined CI wiring and red/green
history. The gaps are the bounded blind spots of §7 — precision, repeated
multiplication, BV bound, composition case, and the quoted (not re-executed)
nature of all hosted evidence. The overall project is NOT accepted by this
review; those boundaries remain open as separate tasks.

## 10. Minimal recommendations (not source mutations)

- R1 (optional, next regression slice): add a case feeding `Add(left,right)`
  (and ideally `Sub`) at `ReadyForFirstMult` into `Tensor2→Relin2→RS2`,
  with expected values from the existing independent cpp_int machinery,
  closing finding F5. Register it through the normal CMake/CI path.
- R2 (cosmetic): when the file is next legitimately touched, align or remove
  the dead scaffold-message branch in `pair_add_test.cpp:91-97` (F6). Do not
  open a change solely for this.
- R3 (documentation): keep a one-line rationale near
  `ValidatePairCompatibility` noting which branches are defense-in-depth
  vs reachable, so future readers do not mistake untested branches for
  coverage gaps (F4).
- No production code change, no test/threshold/vector change, and no guard
  removal is requested by this review.

## 11. Input post-review verification and output provenance

- Post-review (after all reading) recomputation: outer ZIP
  `pair-combined-review-d73824c.zip` = 1,305,833 bytes, SHA256
  `e3dd499889e66a3406fa8ca755b559505db802c2d4cd7c8e1615d74900225fce`
  (unchanged); all 70 `SOURCE-MANIFEST.json` payloads re-verified byte/size
  exact (unchanged); extracted set still exactly the manifest closure + the
  manifest itself. Inputs immutable under this review.
- Outputs: `output/REVIEW.md` (this file) and `output/MANIFEST.sha256`.
  `output/MANIFEST.sha256` records the SHA256 of this file plus the
  post-review input hash, in sha256sum two-column format.
- Review performed 2026-09-04 in the dedicated folder
  `/Users/lifeng/Documents/20231788-openfhe-zcode-pair-review-20260904`;
  extraction confined to `extract-d73824c/`; no files outside `output/`
  were created or modified.
