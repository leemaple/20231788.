# Final bounded static review — integrated Pair Add/Sub

**Verdict: PASS_WITH_GAPS**  
**Actual reviewer model: GPT-5.6 Pro**  
**Review mode: independent static-only review; no implementation, build, cryptographic execution, CI dispatch, or project acceptance**  
**Date: 2026-09-04, Asia/Singapore**

## 1. Decision

The integrated Pair Add/Sub slice is suitable for Codex integration disposition within the stated boundary. I found **no P0, P1, or P2 defect** in the current componentwise arithmetic, validation order, public contract, independent oracle, mutation/key-independence checks, or combined build/test wiring.

The verdict is not an exhaustive correctness proof. It retains three material evidence limits:

1. OpenFHE cloning intentionally shares metadata-entry pointers and parameter provenance, so the demonstrated property is **nonmutation during each Add/Sub/RCB call**, not arbitrary post-return mutation isolation.
2. The tests deeply snapshot the specified ciphertext, native parameter, and relinearization-key values, but they do not instrument every hidden OpenFHE field, cache, allocator, precomputation, or concurrent access path.
3. The exact source/run associations are hash-closed inside the supplied packet, but this offline review did not independently authenticate GitHub or reconstruct the asserted commits from a Git object database.

These are bounded gaps, not defects in the scoped Pair Add/Sub implementation.

### Severity count

| Severity | Count | Disposition |
|---|---:|---|
| P0 | 0 | None found. |
| P1 | 0 | None found. |
| P2 | 0 | None found. |
| LIMIT/INFO | 4 | Documented below; no source change required for this slice. |

## 2. Reviewer, environment, and evidence vocabulary

- **Model:** GPT-5.6 Pro, not Fable and not GLM.
- **Host:** Linux 6.18.35, x86_64.
- **Static inspection tools:** Python 3.13.5, Git 2.47.3, Info-ZIP 6.00, shell text/hash tools.
- **CMake 3.31.6 and Debian C++ 14.2.0 were present but were not used to configure or compile this project.**
- No other agent was dispatched.

Evidence labels used here:

- **SOURCE-FACT:** directly read from a supplied, hash-verified file.
- **STATIC-OBSERVED:** independently checked in this review environment without executing project/OpenFHE code.
- **INFERENCE:** derived from source facts and explicitly bounded.
- **SUPPLIED-CI:** retained hosted execution evidence; not this reviewer's execution.
- **OPEN:** outside this Pair Add/Sub review.

The detailed activity ledger is in `EXECUTION-LEDGER.md`, SHA-256 `afd81ce7747eef396688b349ece78a9d34ec8b1144d512369030a1731b2a99ce`.

## 3. Scope and exact source identity

### Reviewed source boundary

- Packet-stated tested combined source: `d73824c2d382013c3aadbd7cb29c57008e839714`
- Branch: `codex/integration-01`
- Documentation/source head stated separately by the manifest: `f550eac1251f2005222e60aa4f07cc2e57380c46`
- Incoming Pair parent: `613064117e980d30244dfd7c53915d0869a54a89`
- Pair tested source: `4b170183f29b415329c232a17ea1924acdd0d954`
- Other merge parent: `7afb77d496e606efcaca71767913ef51221ced09`
- Clean-room merge base: `7041a489ae1afa98b75322ec334543f29f10b738`
- Official OpenFHE pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`

The selected `project/` production files, tests, CMake, workflow, coordination records, and retained logs are all bound by `SOURCE-MANIFEST.json`. The packet does not include `.git` objects, so the commit identities above are packet provenance facts rather than independently reconstructed Git commits. The reviewed bytes themselves are fully identified by the manifest.

### Excluded conclusions

This review does not accept the entire Double-CKKS project, does not resolve Mult2 normalization or full-precision questions, and does not convert the current conditional Mult2 per-path certificate into a universal theorem. It does not establish greater-than-53-bit precision, security, or performance.

## 4. Input verification and immutability

### Outer packet

- Expected and observed size: **1,305,833 bytes**
- Expected and observed SHA-256:  
  `e3dd499889e66a3406fa8ca755b559505db802c2d4cd7c8e1615d74900225fce`
- ZIP structure: 91 members — 71 regular files and 20 directories.
- No duplicate paths, absolute paths, `..` traversal, Windows drive/backslash paths, symlinks, special files, or encrypted entries.

### Authoritative source manifest

- `SOURCE-MANIFEST.json`: 13,001 bytes
- SHA-256: `5193f793959813a5561e93ee7201cc4124c9ac52609bc40792c64700ae1f0c85`
- Declared payload entries: 70
- Recomputed size/hash mismatches: 0
- Missing declared files: 0
- Unlisted payload files: 0

The manifest itself is intentionally not self-listed and is the 71st regular ZIP member.

### Complete original Pro return

`ORIGINAL-PAIR-PRO-RETURN.zip` independently recomputed as:

- 212,032 bytes
- SHA-256 `735dea4e6c164ced95c2829ea8eb5316201eb900fd5d77b1aad171e94e2676c4`
- 88 members, 66 regular files
- no unsafe paths or special entries

Its internal ledgers all verify: `FILE_HASHES.sha256` 65/65, `PATCH_HASHES.sha256` 12/12, `FINAL_PROJECT_HASHES.sha256` 21/21, and `CHANGED_FILE_HASHES.sha256` 9/9.

### Official references

The four newly supplied exact-pin references have matching SHA-256 and Git blob IDs under `OFFICIAL-REFERENCE-PROVENANCE.json`: `ciphertext.h`, `metadata.h`, `cryptoobject.h`, and `evalkeyrelin.h`. All other supplied paper and official files close through the outer manifest. This was an offline byte/provenance check; no live fetch was used.

### Post-review verification

After all static inspection, the outer ZIP, the 70 declared payloads, the original return, and all four original-return internal ledgers were recomputed again. The byte counts and hashes remained identical; every mismatch, missing, unsafe, and unlisted list remained empty. The review wrote only to the separate output directory.

## 5. Findings and bounded limitations

| ID | Severity | File/line or evidence | Finding | Disposition / minimal recommendation |
|---|---|---|---|---|
| L-1 | LIMIT, non-defect | `official-openfhe/ciphertext.h:390-405,517-518`; `official-openfhe/metadata.h:44-45`; `project/tests/pair_arithmetic_test.cpp:61-121,583-659` | `CloneEmpty` creates a fresh outer metadata map but copies `shared_ptr<Metadata>` entries. Clone also preserves shared context/parameter provenance. A later caller mutation of a shared metadata entry could therefore be visible through the source and result. | Current promise is correctly limited to per-call nonmutation and left-member provenance. Do not upgrade it to arbitrary future mutation isolation without a deliberate deep-clone policy and tests. No current fix required. |
| L-2 | LIMIT, non-defect | `project/tests/pair_arithmetic_test.cpp:123-230,714-875` | Snapshots independently copy native DCRT values and selected context/key fields, but not every hidden OpenFHE object field, precomputation table, allocator/global state, non-EvalMult cache, or concurrent access effect. Non-relinearization eval-key subtypes receive identity/type/context/tag checks, not A/B deep-value checks. | Preserve the exact bounded claim. Add broader instrumentation only if a later contract requires it; it is unnecessary for minimal Pair Add/Sub. |
| L-3 | PROVENANCE LIMIT | `SOURCE-MANIFEST.json`; `project/coordination/CROSS_BRANCH_INTEGRATION_02.md:83-105`; retained logs | The packet identifies exact commits/runs/jobs and closes all supplied bytes, but contains no Git object database, signed CI attestation, or live server response. A byte-identical review is possible; independent server-side source/run authentication is not. | Codex may authenticate the commit and CI run in its controlled integration environment. No source correction is indicated. |
| N-1 | INFO, historical evidence ergonomics | `project/tests/pair_add_test.cpp:91-96,113-115`; `project/artifacts/tdd/pair-add-runtime/red-linux.txt:86-88`; original patches 0002/0003 | The Add runtime-red test's dedicated scaffold branch expected `"DoubleCKKS: Add arithmetic is not implemented"`, while the retained red surfaced through the top-level unexpected-exception path as `"DoubleCKKS: Add is not implemented"`. The sole failing test and exact message still establish an authentic missing-Add runtime red, but the specialized diagnostic label was not exercised. | Do not rewrite history or current green code. For any future new scaffold cycle, align the expected exception type/text with the scaffold before recording red evidence. |

No falsifiable current source defect was identified. The three LIMIT findings state exactly what the evidence does not prove.

## 6. Mandatory question 1 — paper/source correctness

### Paper mapping

The supplied paper defines ordinary CKKS Add/Sub as ring addition/subtraction modulo the active ciphertext modulus (`PAPER-2023-1788.txt:252-254`) and explicitly states that multi-precision addition and subtraction are performed componentwise on ciphertext-pair representations (`PAPER-2023-1788.txt:557-578`). Its RNS preliminaries state that CRT is a ring isomorphism and permits componentwise arithmetic in each native modulus (`PAPER-2023-1788.txt:203-207`).

For a pair representing

```text
RCB(h, l) = q_div * h + l  (mod Q),
```

the direct equations are therefore:

```text
Add((hL,lL),(hR,lR)) = (hL+hR, lL+lR),
Sub((hL,lL),(hR,lR)) = (hL-hR, lL-lR).
```

Recombining gives, by distributivity:

```text
q_div*(hL ± hR) + (lL ± lR)
= (q_div*hL + lL) ± (q_div*hR + lR)  (mod Q).
```

There is no low-low multiplication term in Add/Sub. The paper's omission of the low-low product concerns its approximate pair multiplication rule, not addition or subtraction.

### Current production trace

The public signatures are exactly present at `project/include/openfhe_2023_1788/double_ckks.h:143-150`.

`DoubleCKKS::Add` at `project/src/double_ckks.cpp:657-682`:

1. validates `left`;
2. validates `right`;
3. validates mutual compatibility;
4. clones `left.high_` and `left.low_` separately;
5. updates each of the two corresponding RLWE components with direct DCRT `+=` against `right.high_` and `right.low_` respectively;
6. copies the left pair manifest;
7. validates the result.

`DoubleCKKS::Sub` at `project/src/double_ckks.cpp:684-708` follows the same path and uses direct **left-minus-right** DCRT `-=` for both high and low members.

No cross-member operation occurs: high is combined only with high; low only with low. The loop cannot drop an RLWE component after validation because both member ciphertexts are required to contain exactly two components.

### Exact upstream policy

The supplied OpenFHE pin's core Add policy clones the left ciphertext and applies componentwise `cv1[i] += cv2[i]` (`official-openfhe/base-leveledshe.cpp:565-590`). Its core Sub policy clones the left and applies `cv1[i] -= cv2[i]` (`:592-617`). The DCRT operators apply native tower-by-tower `+=` and `-=` (`official-openfhe/dcrtpoly-impl.h:373-407`).

The project deliberately implements only this core ring step after stronger pair validation. It does not call an OpenFHE ciphertext convenience Add/Sub path that could align levels or scales.

### Clone and metadata provenance

`CiphertextImpl::CloneEmpty` constructs a fresh ciphertext and copies scalar fields, then copies the outer metadata map's contents; `Clone` additionally copies the element vector (`official-openfhe/ciphertext.h:390-405`). Because `MetadataMap` stores `shared_ptr<Metadata>` values (`official-openfhe/metadata.h:44-45`), the outer map is fresh while entry pointers preserve left provenance. Current tests assert exactly that, not a stronger deep-isolation claim.

The production high result derives only from the left high member, and the low result only from the left low member. Same-object left/right inputs are safe: output objects are cloned before mutation, while right-side references continue to point to the unchanged input.

### No hidden operation

Static inspection of the Add/Sub bodies found no call to `EvalAdd`, `EvalSub`, `Rescale`, `ModReduce`, `KeySwitch`, relinearization, key generation/cache access, Tensor2, Relin2, RS2, Mult2, normalization, or alignment. The supplied production delta is insertion-only: four header lines plus 106 source lines.

### Combined-source continuity

The later RS2 checks remain at `project/src/double_ckks.cpp:1079-1098`: equal post-rescale component count, exact declared basis equality, then direct subtraction. `Mult2` remains the literal composition `RS2(Relin2(Tensor2(left,right)))` at `:1115-1117`. `Tensor2` still rejects any state other than `ReadyForFirstMult` at `:763-769`, so Add/Sub preserving `RefreshRequired` does not enable a prohibited second multiplication.

**Answer to question 1: PASS.** The source implements the paper-faithful, componentwise ring operations with correct subtraction order and without hidden normalization or key/scale/level operations.

## 7. Mandatory question 2 — contract order and completeness

### Validation order

The order is exact and visible in both functions:

```text
ValidatePair(left)
ValidatePair(right)
ValidatePairCompatibility(left, right, operation)
clone and arithmetic
ValidatePair(result)
```

This satisfies the requirement that each operand be independently rejected before any mutual comparison or ring arithmetic.

### Independent validity checks

`ValidatePair` (`project/src/double_ckks.cpp:440-572`) enforces:

- exact bound context identity;
- the bound `q_div`;
- pair component count 2 and evaluation format;
- lifecycle-specific active level and ordered RNS basis;
- lifecycle-specific recorded scaling factor and noise-scale degree;
- paper recorded factor, divisor, logical scale, and recombined logical scale;
- nonempty key tag;
- independent high and low ciphertext validation.

`ValidateCiphertext` (`:280-353`) further enforces for each member:

- nonnull member;
- exact context identity;
- CKKS packed encoding metadata;
- expected level;
- exact RLWE component count;
- exact noise-scale degree and recorded scaling factor;
- key tag and slot count;
- active-basis size;
- evaluation format at DCRT and native-tower levels;
- ordered tower moduli;
- roots of unity and cyclotomic orders from the bound context;
- exact declared aggregate DCRT basis.

### Mutual compatibility checks

`ValidatePairCompatibility` (`:710-761`) compares:

- context identity;
- lifecycle;
- divisor;
- ordered basis;
- level;
- recorded scaling factor;
- noise-scale degree;
- paper recorded factor;
- paper divisor;
- logical scale;
- recombined logical scale;
- key tag;
- slots;
- format;
- component count.

Some comparisons are defense-in-depth because two independently valid pairs under one bound module cannot genuinely disagree on all of these fields. In particular, divisor, lifecycle-derived level/basis/degree/scales, evaluation format, and two-component shape are fixed invariants. The tests correctly use isolated malformed fixtures to exercise those validators instead of demanding an impossible pair of public, valid objects.

The suite also contains genuine public differences where they can exist:

- different valid lifecycle states;
- different key pairs/key tags in the same context;
- genuine 8-slot and 4-slot plaintext encodings, each passing DCP/RCB independently;
- a pair valid under a different context/module, rejected independently by the receiving module.

### Diagnostic order witnesses

The right-before-compatibility witness corrupts the right logical scale while also using a distinct key tag, proving right validation is reached before mutual key-tag comparison (`project/tests/pair_arithmetic_test.cpp:1255-1264`).

The left-before-right witness makes **both** descriptors invalid with different exact diagnostics: left recombined logical scale and right logical scale. It expects the left diagnostic (`:1266-1277`), so a right-first implementation cannot accidentally satisfy it.

All rejection cases snapshot both operands before Add and Sub and verify no mutation after failure (`:992-1027`).

### Output and aliases

The result inherits the exact validated left manifest and is independently revalidated. Fresh high/low ciphertext objects are required and checked. `Add(left,left)` and `Sub(left,left)` are supported because only fresh clones are mutated; self-subtraction is checked as exact zero.

**Answer to question 2: PASS.** The contract order and applicable compatibility conditions are complete for the stated public seam. No mutable public factory or impossible valid mismatch is needed.

## 8. Mandatory question 3 — oracle independence and discrimination

### Independent expected-value construction

The exact Pair oracle uses `boost::multiprecision::cpp_int` (`project/tests/pair_arithmetic_test.cpp:22`) and implements its own:

- positive modular reduction (`:298-304`);
- extended Euclidean algorithm and modular inverse (`:306-326`);
- modulus product and centered representative (`:328-355`);
- textbook CRT reconstruction from native residues (`:344-355,380-389`);
- signed host Add/Sub with materialized `BigInt` return values (`:508-513`).

For each high and low member, it checks both RLWE components, every active native tower, and every ring coefficient (`:515-547`). The expected value is reconstructed from the **input residues**, combined with host integer `+` or `-`, and reduced independently modulo each tower.

It does not build expected outputs with production Add/Sub, OpenFHE `EvalAdd`/`EvalSub`, production RCB on either operand, or DCRT `operator+=`/`operator-=` output. It uses the pinned OpenFHE format conversion on copies to observe coefficient residues; this is a bounded dependency on the library's NTT conversion, not a circular arithmetic oracle.

### Independent public RCB equation

`CheckRcbOracle` calls public RCB only on the actual returned Pair, then independently computes

```text
q_div * (high_left ± high_right) + (low_left ± low_right)
```

from the original member residues (`:549-581`). It does not call RCB on the operands to form the expected answer.

### Witness coverage and what it discriminates

| Witness | What it can discriminate | What it does not prove alone |
|---|---|---|
| `0`, `+1`, `-1`, and points around `±Q/2` (`:438-454`) | Modular wrap, carry, sign convention, and subtraction direction at centered boundaries. | General CKKS decoded precision or error bounds. |
| Distinct phase schedules for high/low and each RLWE component (`:471-500`) | High/low swaps, dropped members, copied wrong component, or a shared single result used for both members. | Every possible semantically equivalent implementation defect. |
| Reverse Add and reverse Sub (`:1317-1325,1343-1351`) | Add commutativity and Sub noncommutativity/order. | Mutation or metadata provenance without separate snapshots. |
| Self-Add and self-Sub with the exact same Pair object (`:1327-1333,1353-1362`) | Alias safety, doubling, and exact zero subtraction. | Cross-object compatibility failures. |
| Independent RCB oracle | Wrong divisor, wrong high/low member, wrong sign, or incorrect recombined relation. | RCB correctness outside the active validated Pair states. |
| Public DCP → Tensor2 → Relin2 → RS2 fixtures (`:913-981`) | Actual public construction and all three lifecycle states without relying only on coefficient-replaced fixtures. | Full end-to-end decoding precision, a second multiplication, or a universal Mult2 theorem. |
| Explicit COMPLEX context and literal `0.125` imaginary plaintext check (`:919-938`) | Prevents a nominally complex witness from being silently reduced to REAL before encryption. | It does not itself prove encrypted imaginary-slot accuracy after Add/Sub. |
| Basic decoded literal Add/Sub tests | Coarse functional slot behavior on a small public fixture. | The frozen `1e-6` tolerance is not a bit-precision, theorem, or security claim. |

The suite does not claim that any single identity catches every implementation defect; discrimination comes from the conjunction of exact member residues, exact RCB residues, distinct member/component patterns, aliases, reversed subtraction, lifecycle coverage, provenance checks, and rejection order.

### Small independent sanity witness

This reviewer separately checked the centered arithmetic with the toy modulus `Q=35`:

| Host operation | Residue modulo 35 | Centered representative |
|---|---:|---:|
| `17 + 1` | 18 | -17 |
| `-17 - 1` | 17 | 17 |
| `17 - (-1)` | 18 | -17 |

This is only a static algebra sanity check. It is not an execution of project/OpenFHE code and does not establish CKKS precision.

**Answer to question 3: PASS_WITH_GAPS.** The expected arithmetic is independent and strongly discriminating. The remaining bounded dependency is observation through the pinned library's coefficient/evaluation format conversion, plus the normal limitation that finite witnesses are not exhaustive.

## 9. Mandatory question 4 — mutation, key independence, and provenance

### Key-independence window

The public-lifecycle fixture first creates a genuine unrelated BV context and EvalMult key row (`project/tests/pair_arithmetic_test.cpp:913-917`). It separately creates the HYBRID fixture context and prepares all required lifecycle states (`:919-949`). Only after preparation does it remove the fixture key-tag row (`:958-968`), confirm the unrelated BV row remains, and begin the Add/Sub/RCB mutation window (`:969-981`). The guard row is removed only after the final checks (`:989`).

This establishes that the tested Add/Sub calls do not require the fixture's multiplication key and do not clear or rewrite the retained unrelated row. It does not establish that no hidden cache anywhere in OpenFHE exists; static inspection additionally confirms the production Add/Sub bodies contain no key-cache access.

### Input snapshots are value-retaining, not identity-only

The ciphertext snapshot stores:

- original pointer identity;
- a ciphertext value clone;
- context, encoding, level, degree, scaling factor, key tag, and slots;
- metadata map and entry identities plus a deep `Metadata::Clone` value for each entry;
- independently copied native residues and parameter facts for every DCRT component/tower.

See `project/tests/pair_arithmetic_test.cpp:61-230`. The final checks compare both semantic ciphertext equality and explicit native values/parameters. This is materially stronger than shallow `shared_ptr` equality alone.

The Pair snapshot also preserves all public manifest fields (`:232-296`). Rejection tests and valid arithmetic tests compare both operands before and after the call.

### Context and key snapshots

Context snapshots retain the crypto-parameter object identity, element-parameter identity, every tower parameter identity, modulus, root of unity, and cyclotomic order (`:714-762`).

EvalMult key snapshots retain the map and row-vector identities, key pointer identity, context, tag, dynamic type, and—when the key is `EvalKeyRelinImpl`—independent DCRT native-value/parameter snapshots for all A and B entries (`:764-875`). The official exact-pin getters return const references to the A/B vectors (`official-openfhe/evalkeyrelin.h:141-143,171-173`); the test copies their DCRT values into its snapshot structures.

### Exact Clone semantics and provenance

Official `CloneEmpty` constructs a fresh ciphertext and fresh outer metadata map, then copies the map entries; `Clone` copies the encrypted elements (`official-openfhe/ciphertext.h:390-405,517-518`). Because entries are shared pointers (`official-openfhe/metadata.h:44-45`), output tests correctly require:

- fresh ciphertext object;
- fresh outer metadata map;
- metadata entries and parameter identities derived from the corresponding **left** member;
- unchanged deep metadata value during the operation.

High provenance is never taken from low, and low provenance is never taken from high (`project/tests/pair_arithmetic_test.cpp:583-659`).

### Exact blind spots

The evidence does **not** prove:

1. post-return isolation from a future caller mutating a shared metadata entry;
2. deep cloning for every possible future `Metadata` subclass beyond the tested clone/equality contract;
3. immutability of every hidden context field, precomputation table, allocator, or process-global state;
4. A/B deep-value immutability for an eval-key subtype that is not `EvalKeyRelinImpl`;
5. immutability of automorphism keys or other caches not included in `GetAllEvalMultKeys`;
6. thread safety, race freedom, or concurrent cache behavior;
7. an NTT implementation independent of OpenFHE's `SetFormat` conversion;
8. decoded complex precision, theorem-level precision, security, or performance.

These limits are explicit and consistent with the minimal operation contract.

**Answer to question 4: PASS_WITH_GAPS.** The tests independently retain the values they claim to snapshot and verify targeted key independence and corresponding-left provenance. They correctly stop short of arbitrary future caller-mutation isolation or total hidden-state proof.

## 10. Mandatory question 5 — Codex integration corrections

I compared the complete original Pro return with the integrated Pair tests and the current production delta. The six stated corrections are present and narrow:

| Correction | Static result |
|---|---|
| Exactly one `HasNonzeroValue` helper | Present once at `project/tests/pair_arithmetic_test.cpp:690-704`. It is available before both lifecycle and controlled-oracle use. The original patch sequence's later duplicate/ordering problem is not present. |
| Materialized `BigInt` branches | `ExpectedArithmetic` returns explicit `BigInt(left + right)` or `BigInt(left - right)` at `:508-513`, avoiding incompatible Boost expression-template branch types. Expected arithmetic is unchanged. |
| Explicit COMPLEX public lifecycle fixture | `MakeContext` accepts a data type and calls `SetCKKSDataType` (`:410-425`); the public fixture selects `COMPLEX`, asserts it, and checks the exact `0.125` imaginary cached plaintext value before encryption (`:919-938`). The official REAL constructor would zero imaginary values (`official-openfhe/ckkspackedencoding.h:88-102`), so this correction is necessary and valid. |
| Genuine slot mismatch | The current negative fixture uses real 8-slot and 4-slot plaintext encodings, and both pairs pass their own DCP/RCB path before incompatibility is tested. It does not relabel a ciphertext as seven slots. This is a genuine valid-but-incompatible seam. |
| Strong left-before-right witness | Both operands are independently malformed with different diagnostics (`project/tests/pair_arithmetic_test.cpp:1266-1277`). A reversed validation order would produce the right-side logical-scale diagnostic and fail the assertion. |
| Well-defined test-owned corruption | Descriptor corruptions cast away const from objects originally created mutable and held in non-const local variables. Ciphertext pointees were originally mutable allocations stored by the Pair and are exposed through a const view; `const_pointer_cast` recovers the original type. Fixtures are local/isolated and nonnull/shape-checked before mutation. No mutation of a truly const allocation, null dereference, or out-of-bounds construction was found. |

The original and current production Add/compatibility bodies are substantively identical; Sub arithmetic is also unchanged, with only explanatory comment wording differing. `PAIR-PRODUCTION-DELTA.patch` remains the narrow four-header-line plus 106-source-line insertion.

One nuance is worth retaining: a shallow copy of `CiphertextPair` can share ciphertext pointees, so corruption tests must remain case-local. The current tests reconstruct local fixtures per case and snapshot both operands; no cross-case shared mutation was found.

**Answer to question 5: PASS.** The corrections repair test construction/compilation/discrimination without changing production arithmetic or weakening the contract.

## 11. Mandatory question 6 — combined regression and honesty

### CMake registration

Static parsing of `project/CMakeLists.txt` found:

- C++17 and exact OpenFHE 1.5.0 requirement (`:1-13`);
- 15 executable targets, all with existing source files;
- explicit compile-only Relin2, RS2, Mult2, Add, and Sub API targets (`:45-67`);
- warning-as-error settings for every relevant target: `/W4 /WX` under MSVC and `-Wall -Wextra -Wpedantic -Werror` otherwise (`:94-127`);
- exactly 53 unique CTest registrations (`:130-187`);
- no registration naming a missing target;
- Pair dispatcher arguments that exactly match `controlled_oracle`, `public_lifecycles_keyless`, and `compatibility_rejections` (`project/tests/pair_arithmetic_test.cpp:1370-1379`).

### Workflow wiring

The supplied workflow:

- pins OpenFHE to `df495ba2e91739a6dc8f1de254fc5a41155ce504`;
- builds the default warning-clean project on Linux/GCC and Windows/MinGW64;
- explicitly builds Relin2 and RS2 API targets before CTest;
- runs full verbose CTest;
- explicitly builds Mult2, Add, and Sub API targets;
- contains no `continue-on-error` path masking these steps.

Each of the five mandatory API targets occurs in both host jobs. The Pair integration branch and combined integration branch are in the push list.

### Retained combined logs

The Linux and Windows combined logs each contain 53 unique `Start` entries. Their names and order match the CMake registration list exactly. Both logs contain the reported 53/53 completion and each mandatory API-target build completion.

The packet's integration ledger binds these logs to:

- exact source `d73824c2d382013c3aadbd7cb29c57008e839714`;
- run `33854419062`;
- Linux job `100964299802`: 53/53, 0.68 s;
- Windows job `100964299593`: 53/53, 2.27 s.

These are **SUPPLIED-CI** facts. This reviewer parsed the retained text but did not run or authenticate the hosted jobs.

### Parent continuity

The selected current tree retains:

- Pair Add/Sub basic runtime tests;
- the exact controlled CRT oracle;
- public lifecycle/keyless coverage;
- compatibility and malformed-input coverage;
- the full Tensor2, Relin2, RS2, and Mult2 tests and API targets;
- later RS2 post-rescale component/basis validation;
- literal Mult2 composition;
- warning-as-error settings on both host paths.

`CROSS_BRANCH_INTEGRATION_02.md` states that only CMake and workflow required manual merge resolution and that production source auto-merged. The current production and tests are manifest-bound. Because parent Git trees are not supplied as object databases, I cannot independently reproduce the merge commit or a byte-for-byte diff against both parents; that is the provenance limit L-3, not evidence of a current regression.

### Red/green history

The retained stages are authentic and distinct:

- Add absent-API compile red;
- Add throwing scaffold API green;
- Add runtime red at the scaffold;
- Add behavior green;
- Sub absent-API compile red;
- Sub throwing scaffold API green;
- Sub runtime red at the scaffold;
- Sub behavior green.

The later 44/45/46-test Pair runs add exact oracle, public lifecycle/key independence, and compatibility/malformed-input regression coverage. They are not relabeled as new missing-feature red/green cycles. The 53-test combined run is a merge regression, not a proof of project completion.

The only historical diagnostic nuance is N-1: the Add runtime red reached the generic top-level exception message rather than the test's specialized scaffold label. That does not erase the red because the exact source stage, sole failing test, and missing-Add message remain retained.

### No detected weakening

Within the supplied current bytes:

- the exact coefficient oracle uses equality, not a tolerance;
- the basic decoded Add/Sub tolerance remains explicitly bounded at `1e-6` and is not used for a bit-precision claim;
- the complex and genuine-slot corrections strengthen fixture validity;
- the unrelated BV key guard remains present;
- no test registration or mandatory API target is removed;
- the exact OpenFHE version/pin and host backends remain stated;
- no production catch, relaxed validation, automatic refresh, or second multiplication was added.

**Answer to question 6: PASS_WITH_GAPS.** The current CMake/workflow/log set is internally consistent and retains both parent test families. The hosted results are not this reviewer's execution, and server-side provenance remains externally unverified in this offline seat.

## 12. Claim ledger for this review

### Supported within the bounded source review

- Add/Sub are public const member functions with the required signatures.
- Each valid pair operand is independently validated before compatibility and arithmetic.
- Current Add/Sub perform corresponding high/high and low/low ring arithmetic with correct left-minus-right subtraction.
- The result is fresh, uses corresponding left-member metadata provenance, preserves the validated Pair manifest/lifecycle, and is revalidated.
- No Add/Sub rescale, key switch, alignment, relinearization, key generation/cache access, normalization, or automatic refresh is present.
- Exact controlled tests independently check every high/low member, RLWE component, native tower, and coefficient.
- Public RCB is checked against the independent distributive equation.
- The three valid lifecycle states, aliases, self operations, reverse subtraction, genuine key/slot differences, and malformed invariant violations are represented.
- The targeted key-removal and retained unrelated BV-row checks are correctly bounded.
- Current CMake/workflow registration is internally consistent for 53 cases and five mandatory API targets on both configured host paths.

### Not supported or not claimed

- exhaustive absence of all implementation defects;
- independent NTT correctness;
- arbitrary post-return mutation isolation;
- every hidden context/cache/global field remaining unchanged;
- concurrency safety;
- true high-precision decoded I/O;
- repeat multiplication or refresh completeness;
- conservative BV error bound;
- universal Mult2 theorem or greater-than-53-bit precision;
- security level, production parameters, or performance;
- overall project completion or acceptance.

## 13. Minimal disposition recommendations

1. **No Pair Add/Sub source or test change is required for this bounded slice.** Preserve the current exact source/tests and the historical red/green logs.
2. Codex should authenticate the stated commit/run in its own controlled integration environment before final repository disposition; this is provenance closure, not a request to rerun or weaken tests.
3. Future documentation must keep the Clone promise narrow: fresh outer ciphertext/map and per-call nonmutation, while metadata-entry and parameter provenance remain shared by upstream policy.
4. Do not use the 53/53 result to close the separate high-precision, repeated-multiplication, BV-bound, theorem, security, or performance tasks.

## 14. Output provenance

This return contains only:

- `REVIEW.md` — this review;
- `EXECUTION-LEDGER.md` — exact executed/not-executed/supplied-CI ledger;
- `MANIFEST.sha256` — hashes for the input archive, every extracted input member, every extracted original-return member, and the two non-manifest output files.

`MANIFEST.sha256` excludes itself to avoid recursive hashing. The delivery ZIP and its sidecar SHA-256 are produced after these files are frozen. No source or test file is returned or modified.

---

**VERDICT: PASS_WITH_GAPS**
