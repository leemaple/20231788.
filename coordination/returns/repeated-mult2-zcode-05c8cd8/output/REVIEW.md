# Independent incoming-candidate review — repeated-Mult2 bounded basis/key-routing probe

Mac LOCAL STATIC review, performed 2026-09-04 inside
`/Users/lifeng/Documents/20231788-openfhe-zcode-repeated-probe-review-20260904`.
Reviewer environment and every command actually run are recorded in
`output/EXECUTION_LEDGER.md`. Nothing outside `output/` was written.

## 1. Review target identity and verdict

| Identity | Value | Independently verified |
|---|---|---|
| Source commit | `774fe2dcfca47d7a08cab9c04b29c430e354cf9f`, branch `codex/repeated-mult2-01` | Yes — patches' context lines match packaged source bytes exactly (in-memory replay); `inputs/source/MANIFEST.json` declares the same commit; 155/155 payload records match disk |
| Source original ZIP | 1,451,817 bytes, SHA-256 `efc96137d3412bae57099b6e2f7f85a96bd175b4dd810b587083e1e3d324587d` | Yes — recomputed; 156 members, all equal expanded tree, 4,087,970 decoded bytes |
| Candidate commit / baseline | delivery `repeated-mult2-bounded-basis-routing-probe-774fe2d`, tested production `47907783a6141d0174da79eae264d779fc598f28` | Supplied claim; baseline identity of the three changed files re-derived through patch replay against the 774fe2d tree (all context lines matched, so the candidate baseline is byte-consistent with 774fe2d for every changed file) |
| Candidate original ZIP | 63,963 bytes, SHA-256 `bee2b27ebf88c901b5b91bc3e79fe386231f07ea580b5228512bf380fdac2fd2` | Yes — recomputed; 24 members equal expanded tree, 231,371 decoded bytes; internal manifest 22 payload + 2 declared self-exclusions (`MANIFEST.json`, `MANIFEST.sha256`) all verify; `MANIFEST.sha256` correctly hashes `MANIFEST.json` |
| OpenFHE pristine pin | `df495ba2e91739a6dc8f1de254fc5a41155ce504` | Supplied pin; 53 `official-full` files + supplemental `ciphertext-fwd.h` used as read-only verification basis (per-file provenance is a supplied claim from `OFFICIAL-SOURCE-PROVENANCE.json`; hashes of the packaged bytes themselves were re-verified inside the package closure) |
| Evidence commit | `05c8cd873070144b0c74b0f0c5cde93420924d46` (known-findings phase 3 copies) | Supplied claim; the four known-findings files were reviewed as packaged bytes only |
| Package manifest | `MANIFEST.json`: 196 non-self entries, 1 self-exclusion, 5,890,518 payload bytes | Yes — every entry re-hashed against disk; zero mismatches, zero extra files |
| Final archive | `distribution/repeated-mult2-zcode-local-static-774fe2d-05c8cd8.zip`, 3,084,581 bytes, SHA-256 `72320f25fc2b2e055cb9a0aaf761516667bdecd5cf5f7c124f968fcbf4f76ead` | Yes — recomputed; exactly 197 members = 196 entries + `MANIFEST.json`; every member matches manifest bytes/SHA-256; no unsafe/encrypted/duplicate members; `verification/archive-content/` is an exact 197-file extraction |
| TASK.md | 9,445 bytes, SHA-256 `184c24866e10a8af9f1a24907116cbb241dd996617515ea1370b32d090d49900` | Yes — recomputed |

**Integrity status: PASS.** No integrity failure occurred; review proceeded past phase 1.

**Verdict on incoming static candidate quality: REQUEST_CHANGES.**

The candidate is an honestly labeled `BOUNDED_PUBLIC_API_PROBE` Task-D return (per
`inputs/source/TASK.md` D: "return the smallest executable PUBLIC-API
construction/key-routing probe … with predicted observations and frozen
acceptance"). Its packaging, patch replay, CTest preservation, exact-arithmetic
contract and public-API citations all verify against pinned bytes, and its claim
separation (executed vs static vs not-run) is accurate everywhere I checked.
It is **not** a semantic second-Mult2 implementation and does not claim to be.
Changes are required before any hosted adoption because of one static build
defect (V1) and three uncorrected identity/verification gaps (V2–V4); none of
them invalidates the bounded delivery class itself. This verdict addresses
incoming candidate quality only, not full-project completion, and passing the
frozen rejection test would not establish semantic second-Mult2 RED/GREEN.

## 2. Independent findings (severity order)

Each finding states: observed source facts (O), inference (I), pending runtime
(P), with concrete file/line, originating requirement, effect, and a bounded
suggested correction. Corrections are recommendations for the report consumer;
per TASK.md no code/test change was made by this review, and the unanswered
client-setup/Mult2 seam decision continues to block implementation adoption.

### V1 — P1: both new CMake targets receive MSVC and GCC/Clang flags unconditionally

I derived this from the raw diff before opening `inputs/known-findings/`; it
coincides with root finding R1 and both prior reviews' P1.

- **Observed.** `complete/project/CMakeLists.txt:214` and `:226` add
  `target_compile_options(<new target> PRIVATE /W4 /WX)` and `:216`/`:228` add
  `target_compile_options(<new target> PRIVATE -Wall -Wextra -Wpedantic -Werror)`
  with no `if(MSVC)/else()` guard, for both `repeated_mult2_second_contract_test`
  and `repeated_mult2_basis_family_probe`. The pre-existing file guards exactly
  this choice at lines 104–142. The identical defect is inside
  `patches/0002-add-immutable-basis-key-routing-probe.patch` (CMake hunk).
- **Inference (compiler semantics, no compile run).** On GCC/Clang hosts
  `/W4` `/WX` are treated as input paths → hard error; on MSVC the GCC-style
  `-W*` options are unknown options that `/WX` escalates to errors. Both new
  targets therefore fail the required warning-as-error default build on both
  platform families (source `TASK.md` build block; `NEXT_PAPER_GATES.md` G0)
  before either new CTest can execute. The 55 existing targets are untouched.
- **Effect.** G0 "build the five existing explicit API targets plus both new
  targets" cannot pass as delivered; the probe never runs.
- **Bounded correction.** Wrap the four new `target_compile_options` calls in
  the repository's existing `if(MSVC)/else()/endif()` structure (mirroring
  lines 104–142), then record the actual compiler commands at the authorized
  hosted gate. Static diagnosis only — not an observed compile result.

### V2 — P2: family contexts are registered with `INVALID_SCHEME`

Coincides with R2 and the standards review's P2; independently re-derived.

- **Observed.** Probe `complete/project/tests/repeated_mult2_basis_family_probe.cpp:117-118`
  calls the two-argument `CryptoContextFactory<DCRTPoly>::GetContext(cryptoParameters, scheme)`.
  Pinned `official-full/src/pke/include/cryptocontextfactory.h:74-76` defaults
  `schemeId = SCHEME::INVALID_SCHEME`; `cryptocontext.h:253` initializes
  `m_schemeId{SCHEME::INVALID_SCHEME}`. The standard CKKS generator contrasts
  at `gen-cryptocontext-ckksrns-internal.h:145-146` (`GetContext` then
  `cc->setSchemeId(SCHEME::CKKSRNS_SCHEME)`). The probe never sets or asserts
  the scheme ID.
- **Observed (impact boundary).** `VerifyCKKSScheme(m_schemeId)` gates
  `MakeCKKSPackedPlaintext` (`cryptocontext.h:1178`) and the scheme-switching
  family (`cryptocontext.h:3654-3928`), but not `KeyGen`, `EvalMultKeyGen`,
  `Enable` or `Relinearize` as dispatched in this pin.
- **Inference.** The probe's own operations are not scheme-ID-gated, so this is
  an identity defect, not proof the probe would fail. Any later gate that
  creates CKKS plaintexts in a family context (e.g. the G2 semantic test
  encrypting operands) would throw "available for the CKKS scheme only".
- **Bounded correction.** Construct via the three-argument `GetContext(params,
  scheme, <CKKS scheme ID>)` and assert `getSchemeId()` on the returned
  context — do not call `setSchemeId` afterwards, because an interned context
  is shared state. Exact enum spelling is pending: `scheme-id.h` is not among
  the 53 packaged official files.

### V3 — P2: post-factory validation inspects the requested parameter object, and immutability checks are shape-partial

Coincides with R3 and the spec review's finding 2; independently re-derived.

- **Observed.** `DESIGN_DECISION.md:30` promises "a parameter fingerprint
  checked after factory lookup". In the probe, `MakeFamilyContext` lines
  124–140 check `cryptoParameters->GetElementParams()` — the freshly allocated
  object the probe itself constructed — never
  `context->GetCryptoParameters()`. Post-evaluation checks at lines 386–392
  re-inspect that same object's Q moduli plus key-row shapes; row checks at
  205–213 compare moduli and format but omit roots of unity.
- **Observed (interning is real).** `cryptocontextfactory.cpp:39-52`
  (`FindContext`) compares `*params` via `CryptoParametersRNS::CompareTo`
  (`rns-cryptoparameters.h:156-170`, includes element parameters, numPartQ,
  auxBits, …) and returns an existing context on equality. The probe's own
  `equivalentFamily1` construction demonstrates the case where the returned
  context's attached parameters are a different, earlier object.
- **Inference.** For `family0`/`family1` (unique ordered bases, different
  numPartQ from the seed) no interning target exists, so the fresh object is
  almost certainly the attached one — but the probe does not prove it, and its
  advertised "family context/key material unchanged after evaluation" is
  verified only for: element-params Q moduli, HYBRID row count/basis/format,
  and result ciphertext shapes — not roots, not partition/complement tables,
  Barrett constants or key values.
- **Bounded correction.** Validate parameters obtained from the returned
  context (and assert non-interning or record it), extend row checks to roots,
  and narrow the prose to the snapshots actually taken. No mutation was
  observed or alleged; this is a strength-of-evidence gap.

### V4 — P2: the new boundary executable prints the old test's identifier

Coincides with R4 and the standards review's P2.

- **Observed.** `complete/project/tests/repeated_mult2_second_contract_test.cpp:43`
  keeps `constexpr char kTestName[] = "precision_first_mult2_high_precision_contract";`
  and prints it in result lines at 1173, 1212, 1216. The CTest name is
  `repeated_mult2_current_second_call_boundary`.
- **Effect.** Hosted evidence from the new CTest would carry the old
  `precision_first_mult2…` label, inviting conflation of the two tests'
  samples in retained logs.
- **Bounded correction.** Distinct identifier constant for the boundary test;
  keep the original precision regression byte-unchanged.

### V5 — P2: DESIGN_DECISION bundles h=128 ahead of its own gate order

Coincides with R5; adjudicated below (section 3).

- **Observed.** `DESIGN_DECISION.md:5` withholds "a complete second-Mult2
  production patch until that probe and the h=128 setup gate execute", while
  `NEXT_PAPER_GATES.md:17-27` orders G2 (semantic second Mult2, low-N
  diagnostic) before G3 (h=128 family), and the frozen contract's development
  configuration (`SECOND_MULT2_EXACT_VECTORS.json`, `secret_key_distribution:
  UNIFORM_TERNARY`, ring 64) does not depend on h=128.
- **Disposition.** Confirmed as an internal inconsistency of the design text.
  h=128 is mandatory for the paper configuration (Table 3 caption, see V8) and
  for gates G3–G5, but it is not a prerequisite for the permitted low-N
  semantic diagnostic, which `inputs/source/TASK.md` B/E explicitly allows.
  Correct the sentence to gate only paper-configuration claims on h=128.

### V6 — P3 (new, not in any prior finding): `SOURCE_LINE_INDEX.json` mislabels two paper anchors

Found by this review; not present in R1–R6 or the two prior reviews.

- **Observed.** `SOURCE_LINE_INDEX.json` gives `section_6_3_repeated:
  1472–1517 (matched "8 repeated squarings")` and `table_3: 169–214 (matched
  "Table 3")`. In the supplied paper text, 1472 is the **Section 6.1** heading
  ("Error growth") and the first "8 repeated squarings" occurrence (1487) is
  the 6.1 error-growth experiment; Section 6.3 ("Increased precision") is
  1562–1590 and contains its own "8 repeated squarings" (1574) plus the actual
  Table 3 (caption 1580, data 1582–1590). Lines 169–214 contain only the
  §1.5 overview's first textual mention of "Table 3" (184). The same
  candidate's `SOURCE_IDENTITY.json` carries the correct ranges (definition
  895–935 ≈ actual 899; 6.2 1511–1525 ✓; 6.3/Table 3 1562–1590 ✓).
- **Effect.** The two candidate files contradict each other; anyone citing
  `SOURCE_LINE_INDEX.json` (as `SOURCE_CLAIM_TEST_LEDGER.md` §Paper anchors
  invites) would misattribute §6.1's error-growth data to §6.3 or read the
  intro mention as the table. Delivered code and patches are unaffected.
- **Bounded correction.** Regenerate the index with section-heading-anchored
  ranges (6.3 → 1562–1590; Table 3 → 1580–1590) and reconcile it with
  `SOURCE_IDENTITY.json`.

### V7 — P3 (minor): two ledger citations point at neighboring code

- **Observed.** S1 cites `project/src/double_ckks.cpp:L508-L524`; that range
  is `ValidatePair`'s RefreshRequired checks, while the claimed behavior —
  append the constructor-bound trailing tower and validate against the
  original full basis — actually lives at `double_ckks.cpp:925-942`
  (`towers.emplace_back(fullTowerParameters.back(), …)` then
  `ValidateCiphertext(…, fullModuli_, 0, …)`). S18 cites
  `privatekey.h:L134-L147`; `GetPrivateElement` is at 131 and
  `SetPrivateElement` at 139–151. Both claims' substance is confirmed; only
  the line ranges drift.

### V8 — Evidence-gap note (not a defect): `key/evalkey.h` is outside the pinned 53-file set

- **Observed.** The probe's `EvalKeyRelinImpl` accessors `GetAVector()/
  GetBVector()` are not declared in any packaged header
  (`official-full/src/pke/include/key/` contains only `privatekey.h`,
  `publickey.h`; the candidate's `PUBLIC_API_SYMBOL_AUDIT.tsv` does not list
  them either). Indirect pinned evidence: `official-full/src/pke/lib/keyswitch/
  keyswitch-hybrid.cpp:413-414` calls both accessors through
  `EvalKeyRelin<DCRTPoly>`, and the project's own hosted-tested
  `src/double_ckks.cpp:838-882` uses the identical pattern.
- **Pending.** Byte-level verification of the declaration file (and of
  `SCHEME::CKKSRNS_SCHEME`'s exact spelling in `scheme-id.h`) requires adding
  those pinned files; report as genuinely missing evidence, not as failure.

## 3. R1–R6 reconciliation, alias concern, h=128 recheck

| ID | Root finding (condensed) | My status | Independent evidence |
|---|---|---|---|
| R1 (P1) | Both new CMake targets get MSVC + GCC flags unconditionally; static build defect, not observed compile failure | **Confirmed** | Independently derived as V1 before reading known findings; exact lines 214/216/226/228 vs guarded block 104–142 |
| R2 (P2) | `GetContext(params, scheme)` omits the CKKS scheme ID; new context keeps `INVALID_SCHEME`; identity gap, not proof this Relin path fails | **Confirmed** | V2: factory header default, member init, generator contrast at `gen-cryptocontext-ckksrns-internal.h:146`, `VerifyCKKSScheme` gate list |
| R3 (P2) | Post-factory checks inspect requested (freshly allocated) parameters, not the returned context's; later checks prove selected shapes only | **Confirmed** | V3: probe 124–140/205–213/386–392 vs `FindContext`/`CompareTo` interning semantics |
| R4 (P2) | Rejection fixture prints the old first-precision test identifier | **Confirmed** | V4: line 43 constant; prints at 1173/1212/1216 |
| R5 (P2) | DESIGN withholds second-Mult2 production until h128, contrary to the permitted low-N diagnostic and its own gate order | **Confirmed** | V5: `DESIGN_DECISION.md:5` vs G2-before-G3 in `NEXT_PAPER_GATES.md`; contract uses UNIFORM_TERNARY at ring 64 |
| R6 (rec) | 59 KB first-test clone is substantial duplication | **Confirmed as recommendation** | Stage-1 test is 59,250 bytes = original fixture + 5-line header + 17-line rejection block + blank; oracle independence does not require full-file duplication; no refactor authorized before the seam decision |

**ConstCiphertext-reference concern: correctly dismissed (not a finding).**
The supplemental pinned header `inputs/supplemental/ciphertext-fwd.h:50`
defines `template <typename Element> using ConstCiphertext = const
std::shared_ptr<const CiphertextImpl<Element>>;` — top-level const on the
shared_ptr, const pointee. A `Ciphertext<Element>` lvalue converts to
`shared_ptr<const CiphertextImpl<Element>>`, and because the alias expansion
makes the referenced type const-qualified, the conversion temporary can bind
to an lvalue reference of type `ConstCiphertext<Element>&` (as taken by
`Relinearize`, `cryptocontext.h:2021`). This matches the root disposition and
is verified against the pinned bytes, including the provenance's Git blob
identity for the file. As the disposition itself states, it does not establish
that the whole candidate compiles — that remains hosted-pending.

**h=128 clarification recheck: agree, no disagreement found.**

- Table 3's caption (paper TXT line 1580) ends verbatim "The secret key has
  Hamming weight ℎ= 128." — h=128 is required for the selected t=2
  configuration (contrast: Table 2's depth experiment uses h=21,845 at 1513,
  confirming h is configuration-specific).
- KeyGen text (225–227): "Sample s ∈ R with coefficients in {−1, 0, 1} and
  Hamming weight h, **from a prescribed distribution**" — a distribution, not
  a set, so the uniform-over-set notation (line 201) does not apply; the
  paper nowhere specifies the nonzero-position law or sign joint distribution
  (my own full-text search of the packaged TXT agrees with the
  clarification's).
- Pinned sampler `ternaryuniformgenerator-impl.h` (both file hashes match the
  clarification's records): the h≠0 branch rejects duplicate positions
  (indices retried until exactly h nonzeros, lines 84–97/128+) and rejects
  whole rounds unless the +1 count is within `h/2 ± 1` (lines 77, 121) —
  for h=128 exactly 63/64/65 positives. Verified by code reading; sampler not
  executed.
- Therefore: requiring an exactly-weight-128 ternary secret for the paper
  configuration is paper-backed; demanding *equivalence to HEaaN's
  unspecified sampling law* would be an invented completion gate, and the
  official sampler likewise cannot be claimed to match HEaaN. The
  clarification's own framing (record the sampler, weight and sign
  constraint; treat HEaaN same-distribution as an unverified comparison
  limitation) is the correct disposition. Its one external citation (old
  audit line 94) resolves through the packaged
  `inputs/source/project/coordination/PAPER_H128_OFFICIAL_API_SUPPORT.md`
  (the 63–65 positive-sign condition appears there as an open question).

## 4. Preserved observations, true probe scope, remaining questions

### What independently verified clean (observed)

- **Manifests/closure.** 196/196 package entries byte-exact, no extras; final
  archive exactly the 197-member closure; source ZIP 156/156 and candidate ZIP
  24/24 members equal their expanded trees; candidate internal manifest and
  `MANIFEST.sha256` self-consistent; originals unchanged.
- **Patch replay.** My own static parser replayed patch 0001 then 0002
  in memory against the packaged 774fe2d baseline (all context lines matched;
  no writes, no `git apply`): the two CMake hunks reproduce
  `complete/project/CMakeLists.txt` byte-for-byte, and both new-file hunks
  reproduce the two complete test files byte-for-byte (with the standard
  final-newline convention; no "\ No newline" markers). The candidate's
  `PATCH_REPLAY.md` and `static_validation.json` claims (including the three
  final SHA-256 values and both patch hashes) reproduce exactly.
- **CTest preservation.** All 55 prior ordered name/COMMAND bindings are
  unchanged and in order (two independent extraction methods agree);
  `EXPECTED_CTEST_BINDINGS.tsv` equals the final CMakeLists bindings 1:1 with
  rows 56–57 additive (`repeated_mult2_current_second_call_boundary`,
  `repeated_mult2_basis_family_key_routing_probe composition_contract`).
- **Stage-1 test is purely additive over the unchanged first-Mult2 oracle.**
  Exactly three deltas vs `precision_first_mult2_contract_test.cpp`: a 5-line
  header comment, the 17-line rejection-freeze block after the first
  `Mult2` result, one trailing blank. The exact expected message is
  statically reachable and unreachable-otherwise: `Invalid()` prefixes
  "DoubleCKKS: " (`double_ckks.cpp:12-14`); the first Mult2 result is
  `RefreshRequired` and passed `ValidatePair` at construction
  (`double_ckks.cpp:1104-1112`), so on a second call the lifecycle check at
  `double_ckks.cpp:766-768` fires with exactly
  "DoubleCKKS: Tensor2 requires ReadyForFirstMult inputs" before any other
  rejection. If a future implementation made the second call succeed, the
  block throws `std::runtime_error` — the freeze is real.
- **Probe API usage verified symbol-by-symbol against pinned bytes** (all
  observed): `ILDCRTParams(corder, moduli, roots)` ctor
  `ildcrtparams.h:131-145`; `CryptoParametersCKKSRNS` 13-arg public ctor and
  `PrecomputeCRTTables` signature `ckksrns-cryptoparameters.h:70-94`
  (argument order matches); `numPartQ == |Q|` legality and `numPerPartQ == 1`
  via the guard `rns-cryptoparameters.cpp:78-88` (`sizeQ <= a*(numPartQ-1)`
  throws, so numPartQ=11 with sizeQ≤10 throws, and numPartQ=|Q| always
  passes); P generated from auxBits with Q-collision skipping and QP = Q‖P
  ordered (`:142-180`); `SchemeRNS::SetKeySwitchingTechnique`
  `rns-scheme.h:67-76`; `Enable(KEYSWITCH)` no-op comment
  `ckksrns-scheme.cpp:47-49`; factory equivalence search
  `cryptocontextfactory.cpp:39-77`; HYBRID key rows sized `numPartQ` on the
  family QP and tagged from the new key
  `keyswitch-hybrid.cpp:89-90,100-103,125-128`; active-digit derivation and
  family-local table indexing `:322-329,354-376`; hard-coded h=192 sparse
  KeyGen `base-pke.cpp:69` with the secret element on the QP basis
  (`base-pke.cpp:50`); `EvalMultKeyGen` squaring/route
  `base-leveledshe.cpp:136-144`; ciphertext constructors/setters
  `ciphertext.h:67-92,203-306`; `Relinearize` entry
  `cryptocontext.h:2021-2033`; `PrivateKeyImpl(cc)`/`SetPrivateElement`
  `privatekey.h:83,139-151`; static eval-key map
  `cryptocontext.h:1072`.
- **Design claims grounded.** B1 = full basis minus the RS2-consumed q_l with
  the same trailing q_div retained (probe lines 300–303) matches
  `PROBE_ACCEPTANCE.md`; distinct-context and interning semantics follow from
  `CompareTo` including element params and numPartQ; same-secret projection
  by modulus+root identity is well-formed because every family-1 prime exists
  in family-0's QP-basis secret; P-equality-across-families correctly treated
  as informational only; `alpha=1` vs paper `dnum=11` is disclosed as a
  per-family active-row-count realization, not a silent parameter change
  (`DESIGN_DECISION.md:34-38`), and paper Table 3's t=2 row (11 Q primes:
  Base 50×2 + Mult 60×8 + Div 40, dnum 11) genuinely gives alpha=1.
- **Exact contract independently re-verified** with my own from-scratch
  rational arithmetic (the candidate's verifier was inspected, not run):
  all 16 `Z=X·Y` products and 16 `W=Z²` squares exact; both distinguishing
  deltas exact and nonzero; slots 0/1 differ only at ~2⁻⁶⁸·⁶ / 2⁻⁷³·³
  relative — below binary64 resolution, so only exact arithmetic can
  distinguish; operands bounded (≤0.5); the embedded canonical-payload
  SHA-256 reproduces; threshold 2⁻⁸⁰ frozen for both stages and its
  plausibility rationale (observed first-Mult2 worst ≈1.7e-27 ≈ 2⁻⁹·² margin)
  is stated as falsifiable diagnostic, not theorem; development configuration
  matches `FROZEN_SECOND_MULT2_CONTRACT.md` exactly.
- **Security scans independently repeated.** gitleaks 8.30.1 (the version
  already installed on this host; nothing was installed) on
  `distribution/`: exit 0, ~7,305,207 bytes, zero findings — matching the
  root's recorded byte count and result. Full-package scan: exit 0, zero
  findings. My own 10-pattern regex scan over all 197 decoded archive
  members plus a denied-filename check: zero findings. These are scan
  evidence, not a guarantee.

### True probe scope (what it can and cannot decide)

The Stage-2 probe constructs two immutable ordered-Q CKKS-RNS/HYBRID contexts
from a generated seed, builds a family-local eval-mult key from the family-0
secret projected by prime identity during client setup, destroys all private
objects, then exercises evaluator-only relinearization of **uniformly random
controlled three-component tensors** on the active A1 prefix basis and the
full B1 basis, checks rehoming-by-fresh-construction with exact source
snapshots, and re-checks family shapes afterwards. It is a
construction/key-routing decision procedure — not a plaintext arithmetic,
precision or security oracle (the candidate says exactly this), and its
controlled tensors cannot validate relinearization *values*. The Stage-1
test freezes the current rejection boundary; per the candidate's own frozen
contract text, that rejection "must not be reported as a manufactured RED
unless it is actually executed", and nothing in the package claims a runtime
RED or any semantic second-Mult2 result. **No passing of either new test
would constitute semantic second-Mult2 GREEN, eight squarings, or paper
reproduction.**

### Remaining authorized-hosted questions (pending; none run here)

1. G0 compile/CTest of both new targets after V1's correction — including
   actual compiler commands, context fingerprints, ordered Q/P/QP, tags, row
   counts and the probe's predicted observations.
2. Runtime interning/P-equality observations the probe prints as
   informational.
3. G2 semantic second Mult2 against the frozen JSON contract (≤2⁻⁸⁰, four
   fresh keys, lossless codec) — the missing semantic delivery, which is a
   stated delivery boundary of this bounded package, not an additional bug.
4. G3 h=128 complete setup family; G4 eight ordered squarings; G5 paper
   §6.3/Table 3 reproduction (N=2¹⁵, h=128, dnum 11, P60, Base 50×2,
   Mult 60×8, Div 40, Δ=2¹⁰⁰, 1000 executions, ≈81.8-bit comparison); G6
   security/related-key disclosure. h=128 gates only these, not the low-N
   diagnostic.
5. Byte-level verification of `key/evalkey.h` and `scheme-id.h` (V2/V8) if
   those pinned files are added to the evidence set.
6. The user's unanswered client-setup/Mult2 acceptance-seam decision; until
   it is answered, every correction above stays a recommendation and no
   implementation may be adopted.

### Explicit absences checked

No other source-level defects were found in the delivered files beyond
V1–V7: the Stage-1 delta is exactly as claimed, the probe contains no
encrypt/decrypt/re-encryption/bootstrap/multiparty calls, no secret use after
setup scope, no caller-input mutation, no weakened threshold, no
redefinition of the paper target, and no h=128 prerequisite smuggled into the
low-N diagnostic. R6-style duplication (V-adjacent) and the documentation
issues V6/V7 complete the list.
