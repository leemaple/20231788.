# RS2 follow-up independent static review

**Review date:** 2026-09-04  
**Reviewer/model:** ChatGPT Pro, visible model identity **GPT-5.6 Pro**  
**Mode:** independent static review of the supplied packet only  
**Project review target:** `7928bb7634baa3603daf32806d70bd790938a353`  
**Branch recorded by the packet:** `agent/codex-rs2-01`  
**Pristine OpenFHE reference:** `df495ba2e91739a6dc8f1de254fc5a41155ce504`  
**Verdict:** **PASS_WITH_GAPS**

## 1. Scoped verdict

Within the authorized static-review boundary, I found **no blocking production defect** in the current RS2 implementation and no reason to reject the nine-line declared-basis validation correction. The correction is sound for the demonstrated public-seam inconsistency: a DCRT element that advertises the original four-tower aggregate basis while carrying only the genuine three active native towers is now rejected before RS2 arithmetic. The correction retains the pre-existing actual-tower diagnostic order and follows the same copy-then-drop aggregate-parameter pattern used by the supplied pristine OpenFHE source.

The verdict is not an execution verdict. The current `7928bb7...` revision contains later test-only deep-snapshot and prime-witness changes for which the packet explicitly says hosted CI was still in progress. I did **not** compile, link, execute CTest, run a cryptographic operation, dispatch or inspect live CI, or run a production mutation. The supplied earlier CI logs are reviewed evidence records, not commands executed or independently re-fetched in this review.

The remaining gaps are narrow:

1. the current revision still needs its own hosted warning-clean/API/CTest result;
2. “all parameter fields / complete immutability” is broader than the observable fields actually snapshotted, and the bound context's full aggregate parameter object is not directly snapshotted;
3. two keyless fixtures retain pointer-shallow cache comparisons, although the dedicated retained-key fixture is now deep and non-vacuous;
4. null native-child and arbitrary malformed declared-child parameter safety is not established;
5. no decoded multiplication precision, Theorem 4.8 non-wrap condition, Lemma 4.6 bound, production security, 53/106-bit behavior, performance, refresh, second multiplication, Add/Sub, or Mult2 conclusion follows.

No source change is required by this static review. Final acceptance remains Codex's responsibility after current hosted results and any other required reviewers.

## 2. Evidence labels

- **OBSERVED-SOURCE:** directly read in the supplied current source, paper, or pristine reference files.
- **OBSERVED-PACKET-LOG:** stated and supported by complete sections in a supplied retained log; not executed or live-verified by this reviewer.
- **INFERRED:** a static or mathematical conclusion from the supplied bytes.
- **PENDING:** requires the authorized future hosted build/test or an additional narrow fixture.

## 3. Packet identity, safety, and manifest verification

### 3.1 Archive

| Check | Result |
|---|---|
| Supplied archive | `rs2-followup-7928bb7.zip` |
| Actual byte length | `957646` bytes — matches dispatch |
| Actual SHA-256 | `467328d94f7d00a798e922db23cdd7a9245e65c577d3ec5017e0e84f6c05640d` — matches dispatch |
| ZIP directory entries | 52 total: 36 files including `SOURCE-MANIFEST.json`, plus 16 directory entries |
| Manifest payload entries | 35 |
| Payload set | Exact equality: 35 manifest paths = 35 non-manifest files |
| Length/hash verification | 35/35 matched; zero mismatches |
| Extracted file set | Exactly the 35 payload files plus `SOURCE-MANIFEST.json`; zero extras |
| Duplicate ZIP names | 0 |
| Unsafe absolute/traversal/drive/backslash/NUL paths | 0 |
| Symlinks or special filesystem nodes | 0 |
| Prohibited repository/build/cache/compiled/credential file classes by path or extension | 0 |
| `SOURCE-MANIFEST.json` own SHA-256 | `6d1dfa8e1cc88ac9e638f26590064717a2504960955a9671f15d065f98b2af35` |

The manifest records the project commit, branch, clean archive state, and OpenFHE commit at `SOURCE-MANIFEST.json:2-6`. Because the authorized packet contains no Git object database and Git operations were prohibited, I did not independently recompute the mapping from the supplied project bytes to Git commit `7928bb7...`. I verified the bytes against the packet manifest and treat the commit association as packet provenance, not as a separately reproduced Git fact.

### 3.2 Materials inspected

All 35 payload files were integrity-inspected. Substantive static review covered:

- control and delta: `TASK.md`, `CHANGES-SINCE-REVIEW.patch`, and `SOURCE-MANIFEST.json`;
- paper: `PAPER-2023-1788.pdf` and `PAPER-2023-1788.txt`; the PDF is 15 pages, and pages 7-8 were rendered and visually checked against the extracted text for Definitions 4.1, 4.5, 4.7, Lemma 4.6, and Theorem 4.8;
- all nine pristine references: `base-leveledshe.cpp`, `base-leveledshe.h`, `base-scheme.cpp`, `base-scheme.h`, `ckksrns-leveledshe.cpp`, `cryptocontext.h`, `dcrtpoly-impl.h`, `rns-cryptoparameters.h`, and `rns-leveledshe.cpp`;
- current project source/build seams: `.github/workflows/dcp-rcb.yml`, `CMakeLists.txt`, `include/openfhe_2023_1788/double_ckks.h`, `src/double_ckks.cpp`, and the three compile-only API contract tests;
- current behavioral tests: `dcp_rcb_test.cpp`, `tensor2_test.cpp`, `relin2_test.cpp`, and especially the complete current `rs2_test.cpp`;
- prior review/disposition and test-seam/oracle records under `project/coordination/`;
- complete supplied red/green or green-only records under `project/artifacts/tdd/rs2-declared-basis/`, `rs2-public-pipeline/`, and `rs2-terminal-rejections/`.

The prior review and disposition were treated as evidence to reconcile, not as authority over the current source.

## 4. Standards and implementation-contract findings

### S1 — declared aggregate-basis correction is sound for the demonstrated defect

- **Severity:** Info / affirmative finding
- **Evidence status:** OBSERVED-SOURCE + OBSERVED-PACKET-LOG + INFERRED
- **Locations:** `project/src/double_ckks.cpp:321-350`; `project/tests/rs2_test.cpp:930-960`; `official-openfhe/dcrtpoly-impl.h:668-688`; `official-openfhe/rns-leveledshe.cpp:311-320`

**Finding.** After the existing level-versus-active-basis-size check, production constructs a new aggregate parameter object from the bound full parameters and calls `PopLastParam()` exactly `level` times (`double_ckks.cpp:321-329`). Each ciphertext element is still checked first for aggregate evaluation format, actual ordered native moduli, every actual native tower's format, root of unity, and cyclotomic order (`331-346`). Only after those checks does it compare the element's declared aggregate basis with the expected active prefix and issue `declared RNS basis mismatch` (`348-350`).

This ordering is correct for the reported defect. A malformed element with valid three-tower native storage but a declared four-tower aggregate passes the actual-tower checks and reaches the new precise diagnostic. Existing actual-basis, native-format, and native-parameter diagnostics retain priority. The expected-basis construction itself is based only on the already-validated bound context and level, so it does not pre-empt one malformed-ciphertext diagnostic with another.

**Safe parameter sharing.** The code copy-constructs a new `DCRTPoly::Params` object before dropping suffix parameters; it does not call `PopLastParam()` through `parameters_->GetElementParams()`. The supplied official `DCRTPolyImpl::DropLastElement` and `DropLastElements` use the same pattern—copy `*m_params`, pop only on the copy, then install the copy on the local polynomial (`dcrtpoly-impl.h:668-688`). The official output-form `LeveledSHERNS::ModReduce` also clones the ciphertext before its in-place path (`rns-leveledshe.cpp:311-320`). These definitions support the static conclusion that the nine-line change does not intentionally mutate the bound context or caller-owned element parameters.

**Unintended restrictions.** Exact active-prefix agreement is consistent with the module's explicit bound-context and ordered-basis invariants. It does not require aggregate parameter pointer identity; it invokes value equality. The supplied green record after `68d0d98...` includes the ordinary project suite and both HYBRID/BV untouched public paths, so no normal-path restriction was observed in that revision. The exact breadth of `ILDCRTParams::operator==`, however, is not defined in the nine reference files; this review therefore does not claim exhaustive rejection of every same-size malformed declared child.

**Concrete consequence.** The demonstrated declared-four/actual-three object is rejected before either `context_->Rescale` call. The shared validation becomes stricter for DCP/Tensor2/Relin2/RS2/RCB objects in exactly the intended context-prefix dimension.

**Minimal test/fix.** Keep the current source. Retain the existing high/low public-seam fixture. The cheapest additional falsifier, only if broader malformed-object safety is required, is a same-tower-count declared aggregate whose child descriptor is deliberately inconsistent while the actual native towers remain genuine; require the same declared-basis diagnostic and unchanged input. Do not generalize that result to null children without separate tests.

### S2 — parameter immutability is now materially deeper, but “all fields” remains overbroad

- **Severity:** Low
- **Evidence status:** OBSERVED-SOURCE + INFERRED + PENDING
- **Locations:** `project/tests/rs2_test.cpp:67-140`, `202-249`, `798-836`; `official-openfhe/dcrtpoly-impl.h:430-434`, `437-442`, `668-688`

**Finding.** The current snapshot is a real improvement. It records:

- aggregate parameter identity, cyclotomic order, aggregate modulus, aggregate root, and polynomial format;
- each declared and actual native parameter pointer, cyclotomic order, modulus, and root;
- every native tower format;
- every native coefficient value;
- ciphertext object identity, clone equality, metadata-map identity, each metadata-object identity, and a deep metadata value;
- all pair-manifest fields.

This closes the prior reliance on `DCRTPolyImpl::operator==` alone. That operator compares only aggregate format, aggregate cyclotomic order, aggregate modulus, vector count, and vector values (`dcrtpoly-impl.h:430-434`), so the new explicit snapshots are necessary and useful.

It is still not literally an exhaustive snapshot of every parameter field or every caller-owned parameter object:

1. `NativeParameterSnapshot` contains four values—pointer, cyclotomic order, modulus, and root (`rs2_test.cpp:69-70`). The aggregate snapshot adds format but does not directly record `GetRingDimension()`, which is a separately used observable in the supplied OpenFHE source. The packet does not include the complete parameter-class definition needed to prove that these tuples exhaust every mutable field.
2. The test snapshots level-1/level-2 polynomial parameter objects reachable from ciphertexts, but it does not directly snapshot `context->GetCryptoParameters()->GetElementParams()`, the full bound aggregate object copied by the new production check. Static source strongly indicates safety, but the current test does not independently observe that specific caller-owned aggregate.
3. `CheckCiphertextUnchanged` still retains `*ciphertext == *clone` (`rs2_test.cpp:228-235`), but that check is now only a supplement; it does not expand the deep snapshot's field set.

**Concrete consequence.** The current source supports a claim that the listed parameter identities/values, native formats/values, metadata, and pair state are unchanged. It does not support the unrestricted phrase “every parameter field and every context-internal parameter is immutable.” This is a claim/evidence limitation, not an observed production mutation.

**Minimal test/fix.** Either narrow the claim to the fields actually enumerated, or add a small `SnapshotFullContextBasis(context)` around RS2 and RCB, including aggregate identity, child identities, all public scalar getters used by the pinned parameter types, and `GetRingDimension()`. No framework or production redesign is needed. Current execution of the already-authored deep snapshot remains pending.

### S3 — the retained-key cache fixture is deep; two keyless checks remain pointer-shallow by construction

- **Severity:** Info, with a claim-scope caution
- **Evidence status:** OBSERVED-SOURCE + PENDING
- **Locations:** `project/tests/rs2_test.cpp:144-199`, `818-834`, `839-889`, `891-927`

**Finding.** F4's substantive gap is addressed in the controlled valid fixture. `SnapshotEvaluationKeyCache` traverses every current map row, every key entry, the key object/context/tag, and every A/B DCRT polynomial through the deep parameter/format/value snapshot (`144-199`). The fixture creates both the RS2 input's genuine relinearization-key row and a distinct unrelated genuine row, requires both to exist, checks the cache after RS2, invokes public RCB through the oracle, and checks it again (`818-834`). That comparison is non-vacuous and materially deep for genuine `EvalKeyRelinImpl<DCRTPoly>` rows.

Two other fixtures intentionally remove their own key and then use ordinary map equality:

- untouched public pipeline: `rs2_test.cpp:866-886`;
- terminal rejection: `rs2_test.cpp:903-926`.

A `std::map`/`std::vector` containing shared pointers compares pointer identities, not pointed-to key contents. In a fresh keyless process the check is adequate to show that no row was added or removed. If unrelated rows were present, it would not detect their in-place mutation. This does not reopen F4 for the dedicated retained-cache case, but it means those two checks should be described as keyless membership/identity checks rather than independent deep-cache proofs.

**Concrete consequence.** The suite has one strong deep retained-cache test, while the terminal and keyless pipeline cases primarily establish that RS2 succeeds/rejects without its own evaluation key and does not alter cache membership. The production RS2 body itself contains no evaluation-key lookup or write.

**Minimal test/fix.** No production fix. Either assert the keyless cache is empty after clearing the fixture's only key, relabel the assertion narrowly, or reuse `SnapshotEvaluationKeyCache` with one unrelated retained row. The first option is cheapest if the intended claim is merely “RS2 needs no key.”

### S4 — no blanket malformed-parameter safety conclusion is available

- **Severity:** Info
- **Evidence status:** OBSERVED-SOURCE + PENDING
- **Locations:** `project/src/double_ckks.cpp:16-23`, `321-350`; `project/tests/rs2_test.cpp:930-960`

**Finding.** A null declared aggregate pointer is explicitly rejected by the short-circuit at `double_ckks.cpp:348-350`. The supplied defect fixture proves the non-null full-declared/active-actual inconsistency. It does not prove clean handling of null actual native parameter pointers, null children inside a declared aggregate, or every internally inconsistent same-count aggregate. `OrderedModuli` calls `tower.GetModulus()` (`16-21`), and validation later calls native root/order getters (`343-344`); the supplied files do not establish how those getters behave if an internally malformed tower has null parameters. The complete `ILDCRTParams::operator==` implementation is also absent.

**Concrete consequence.** It would be inaccurate to state that every possible malformed DCRT parameter graph is guaranteed to produce the intended `std::invalid_argument` rather than another failure mode. No such broad claim is needed for acceptance of the reported fix.

**Minimal test/fix.** Add only constructible, public-seam malformed cases that can be created without undefined behavior, and give each an exact expected diagnostic. Otherwise retain the explicit scope limit. Do not weaken the current validation to accommodate malformed objects.

### S5 — build-system and API seams are coherent statically; current execution is pending

- **Severity:** Medium acceptance-evidence gap, not a production-code finding
- **Evidence status:** OBSERVED-SOURCE + OBSERVED-PACKET-LOG + PENDING
- **Locations:** `project/CMakeLists.txt:1-13`, `66-84`, `124-130`; `project/.github/workflows/dcp-rcb.yml:16-18`, `79-95`, `195-233`; `project/tests/rs2_api_contract_test.cpp:7-16`; `TASK.md:21`, `33-37`

**Finding.** CMake requires C++17, exact OpenFHE 1.5.0, warning-as-error compilation, an explicit RS2 API target, and six current RS2 CTest entries. The workflow pins the supplied OpenFHE commit and defines warning build, explicit Relin2/RS2 API builds, and CTest on Linux GCC and Windows MinGW64. The public signature remains `CiphertextPair RS2(const CiphertextPair&) const`, and `RefreshRequired` is a distinct enum state.

The packet provides successful execution records for earlier source points, but not a completed hosted result for the final `7928bb7...` test-only changes. A source assertion, static type expression, or this review is not proof that the current target compiles or that all 43 cases pass.

**Concrete consequence.** Static acceptance may proceed, but release/integration acceptance should wait for the exact current commit's hosted result.

**Minimal test/fix.** No source change. Run the authorized hosted commands at the exact commit and retain complete warning/API/CTest output. The reviewer did not execute them.

## 5. Paper and specification correctness findings

### P1 — task citation number is inaccurate; implementation citation is correct

- **Severity:** Info/documentation
- **Evidence status:** OBSERVED-SOURCE
- **Locations:** `TASK.md:10`; `PAPER-2023-1788.txt:785-815`, `895-901`; PDF page 8; `project/src/double_ckks.cpp:951-953`

**Finding.** Pair rescale/RS2 is **Definition 4.5**, not Definition 4.7. Definition 4.7 defines pair multiplication as `Mult2 := RS2 ∘ Relin2 ∘ Tensor2`. The production comment correctly cites Definition 4.5.

**Concrete consequence.** No algorithmic consequence; the task wording could cause later review or documentation to cite the wrong definition.

**Minimal fix.** Change only the prose reference to “Definition 4.5; Definition 4.7 composes Mult2.”

### P2 — Tensor2 and RS2 match the paper's algebraic structure

- **Severity:** Info / affirmative finding
- **Evidence status:** OBSERVED-SOURCE + INFERRED
- **Locations:** `PAPER-2023-1788.txt:570-606`, `785-815`; PDF pages 7-8; `project/src/double_ckks.cpp:657-705`, `885-1006`

**Finding.** Tensor2 computes `high1 ⊗ high2` and the two cross terms, with no low-low term (`double_ckks.cpp:671-676`), matching Definition 4.1. RS2 computes:

1. an independent rescale of high;
2. RCB of the input, i.e. `q_div * high + low`, followed by an independent rescale;
3. `newLow = rescaledRecombined - q_div * rescaledHigh`;
4. an output pair on the active prefix with `RefreshRequired` lifecycle.

The two `context_->Rescale` calls are separate (`951-953`), and the correction uses a checked direct DCRT subtraction (`973-988`) rather than an alignment-capable high-level subtraction. The public RCB identity checked by the oracle is exactly the paper's `RCB(RS2(CT)) = RS(RCB(CT))`.

**Concrete consequence.** A one-rescale shortcut or `RS(low)` implementation would not satisfy the current source oracle. Only the active last prime is consumed; `q_div` remains a pair descriptor.

**Minimal test/fix.** Existing exact CRT oracle is the appropriate minimal test. No source change indicated.

### P3 — the supplied official ModReduce/Rescale dispatch closes prior F5

- **Severity:** Info / resolved
- **Evidence status:** OBSERVED-SOURCE
- **Locations:** `official-openfhe/cryptocontext.h:448-460`, `2501-2510`; `official-openfhe/base-scheme.cpp:94-100`; `official-openfhe/rns-leveledshe.cpp:311-320`; `official-openfhe/ckksrns-leveledshe.cpp:172-190`; `official-openfhe/rns-cryptoparameters.h:637-649`, `662-665`; `official-openfhe/dcrtpoly-impl.h:691-712`

**Finding.** The source chain is now explicit:

`CryptoContext::Rescale` validates and calls scheme `ModReduce` with the context composite degree; `SchemeBase::ModReduce` dispatches to the configured leveled-SHE implementation and restores the input key tag; `LeveledSHERNS::ModReduce` clones the input and enters `ModReduceInPlace`; FIXEDMANUAL dispatches to the virtual internal implementation; `LeveledSHECKKSRNS::ModReduceInternalInPlace` reduces degree, increments level, calls `DropLastElementAndScale` on each component, and divides recorded scale by `GetModReduceFactor(sizeQl - 1 - i)`.

For FIXEDMANUAL, `GetModReduceFactor` returns `m_approxSF`, while the native tower drop is the last active modulus. This confirms that the implementation is right to keep three quantities distinct:

- `q_l`: the native active last prime used by centered rescaling and logical-scale division;
- `q_div`: the already-removed fixed decomposition divisor retained in pair recombination;
- the recorded metadata divisor returned by `GetModReduceFactor`.

The output-form rescale path clones before mutation. `EvalMultNoCheck` independently clones and multiplies each component by the native integer (`cryptocontext.h:2073-2079`).

**Concrete consequence.** The prior missing-wrapper inference is closed for the supplied pinned source. This does not turn ring-level exact tests into decoded-precision or theorem evidence.

**Minimal test/fix.** No source fix. Keep exact state/residue tests because source dispatch inspection is not runtime evidence.

### P4 — q_div/q_l witness is mathematically guaranteed for either ordering

- **Severity:** Info / affirmative mathematical finding
- **Evidence status:** INFERRED + PENDING execution
- **Locations:** `project/tests/rs2_test.cpp:392-454`, `474-570`, `685-771`

**Finding.** Let `s = min(q_div, q_l)`. Both are required to be distinct odd primes. The new controlled coefficients are

`x = (s + 1) / 2` and `-x`.

For centered division by `s`, `x` is the first positive integer in the upper half, so its centered remainder is `x - s` and its quotient is `+1`; `-x` gives `-1`. For division by the larger prime, `|x|` remains in the centered lower half, so both quotients are `0`. Therefore:

- if `q_div < q_l`, the intentionally wrong q_div quotient is `±1` while the correct q_l quotient is `0`;
- if `q_l < q_div`, the correct q_l quotient is `±1` while the intentionally wrong q_div quotient is `0`.

The quotient difference is `±1`, so it cannot disappear modulo any retained target modulus greater than one. Because `s <= q_l` and the active source modulus contains `q_l` times the other active coprime towers, these small witnesses are not changed by source-modulus centering. The 21 earlier boundary entries are preserved: coefficients 0-20 already contain one complete rotated 21-value set before the last two coefficients are overwritten.

The oracle explicitly calculates the wrong q_div quotient and requires at least one actual target residue to differ (`rs2_test.cpp:718-770`). This discrimination is independent of the separate direct-`RS(low)` witness.

**Concrete consequence.** The authored test cannot accidentally pass merely because the chosen coefficients make q_div-division and q_l-division coincide. It works for both prime orderings and both signs.

**Minimal test/fix.** No mathematical fix. Hosted execution of the current test remains required. This is **not** an executed mutation of production RS2 and is **not** a q_div/q_l metadata-swap test.

### P5 — lifecycle and evaluation-key boundaries are correctly represented

- **Severity:** Info / affirmative finding
- **Evidence status:** OBSERVED-SOURCE + OBSERVED-PACKET-LOG
- **Locations:** `project/src/double_ckks.cpp:440-571`, `657-676`, `885-889`, `1000-1006`; `project/tests/rs2_test.cpp:839-927`

**Finding.** Relin2 produces a two-component `ReadyForRS2` pair; RS2 validates the pair and lifecycle before arithmetic, then returns a two-component `RefreshRequired` pair. A repeated RS2 call fails before rescaling. Tensor2 validates both operands and rejects any non-`ReadyForFirstMult` lifecycle before its `EvalMultNoRelin` calls. RS2 and RCB contain no evaluation-key lookup or cache mutation.

The untouched public pipeline clears its own key after genuine Relin2 and before RS2/RCB. The terminal fixture reaches a genuine RS2 output, clears its own key, and exercises repeated RS2 plus Tensor2 with terminal left, right, and both. Supplied prior logs report these cases green at the cited earlier commits.

**Concrete consequence.** The current architecture exposes an explicit stop boundary; it does not silently perform refresh or permit a second multiplication.

**Minimal test/fix.** The authored tests are sufficient for the named terminal combinations, subject to current-commit hosted execution. No refresh/Mult2 expansion belongs in this review.

### P6 — untouched complex pipelines are ring/state evidence, not decoded correctness or Theorem 4.8 evidence

- **Severity:** Info / mandatory claim limit
- **Evidence status:** OBSERVED-SOURCE
- **Locations:** `project/tests/rs2_test.cpp:839-889`; `project/artifacts/tdd/rs2-public-pipeline/README.md:6-21`; `PAPER-2023-1788.txt:899-907`

**Finding.** The test uses genuine nonzero complex plaintexts, Encrypt, DCP, Tensor2, Relin2, RS2, and RCB for HYBRID and BV with no coefficient replacement. It then checks exact ciphertext-ring residues and state. It does not decrypt the resulting multiplication, compare decoded slots to a host product, compute the Theorem 4.8 non-wrap precondition, establish Lemma 4.6's noise bound, or measure production parameter security/performance.

**Concrete consequence.** It validly closes the “untouched real pipeline metadata/arithmetic compatibility” gap, but cannot support claims of decoded product precision, theorem-certified correctness, 53/106-bit precision, security, or speed.

**Minimal test/fix.** Keep those claims out of RS2 acceptance. Any eventual decoded/Theorem evidence belongs to a separately scoped Mult2 experiment with explicit parameters and bounds.

## 6. Prior-finding and new-defect disposition

| Item | Current disposition | Evidence classification | Remaining limit |
|---|---|---|---|
| **F1 — terminal RS2/Tensor2 rejection** | **Implemented coverage; behavior observed green at an earlier source point.** Genuine `RefreshRequired` output; own key removed; repeated RS2 and Tensor2 terminal left/right/both are present at `rs2_test.cpp:891-927`. Supplied run `33836142693`, source `b5f3d9f...`, reports Linux 43/43 in 0.60 s and Windows 43/43 in 0.97 s. | OBSERVED-SOURCE + OBSERVED-PACKET-LOG | Coverage of existing behavior, not a production red-green correction. Current `7928bb7...` execution is pending. |
| **F2 — untouched nonzero public pipeline** | **Implemented and observed green at an earlier source point.** HYBRID and BV genuine complex Encrypt→DCP→Tensor2→Relin2→RS2→RCB, no coefficient replacement, own key removed before RS2. Supplied run `33834861766`, source `7041a489...`, reports 41/41 on both platforms. | OBSERVED-SOURCE + OBSERVED-PACKET-LOG | Exact ring/state evidence only; no decoded precision or theorem certificate. |
| **F3 — shallow ciphertext/parameter snapshot** | **Materially addressed by current test-only code.** Declared/actual parameter identities and selected scalar values, native formats, all native values, metadata and pair fields are checked. | OBSERVED-SOURCE + PENDING | Hosted execution pending; “all fields/context internals” remains overbroad as described in S2. |
| **F4 — shallow/vacuous evaluation-key cache check** | **Substantively addressed for a retained genuine cache.** Own and unrelated rows are required; every row/key/context/tag and every A/B polynomial snapshot is checked before/after RS2 and RCB. | OBSERVED-SOURCE + PENDING | Current hosted execution pending. Keyless/terminal ordinary map comparisons remain shallow if unrelated rows exist; arbitrary malformed key objects are outside the fixture. |
| **F5 — missing ModReduce dispatch definition** | **Closed statically.** The nine references now expose the Rescale→scheme→RNS clone/in-place→CKKS internal path and FIXEDMANUAL recorded-factor behavior. | OBSERVED-SOURCE | Runtime exactness remains a separate test question. |
| **F9 — explicit q_div/q_l arithmetic witness** | **Mathematical witness implemented.** Positive and negative first-upper-half values for the smaller prime guarantee different q_l and q_div quotients; oracle requires a differing target residue. | INFERRED + PENDING | Not an executed production mutation and not a metadata-swap fixture. |
| **New declared-basis defect** | **Demonstrated red-green correction.** Supplied `f8e9760...` logs show the malformed high member accepted on Linux and Windows: 41/42, sole failure `rs2_declared_basis_mismatch`. Supplied `68d0d98...` green log reports 42/42 on both platforms after the nine-line production fix, exercising high and low malformed members. Current production contains the fix. | OBSERVED-PACKET-LOG + OBSERVED-SOURCE + INFERRED | Red stopped on high, so no separate low red is claimed. No exhaustive null-child/same-count-malformed safety claim. |

## 7. Supplied execution evidence and exact non-execution statement

### 7.1 Supplied retained records reviewed as data

- `project/artifacts/tdd/rs2-declared-basis/red-linux.txt:3-12,96-108`: source `f8e9760...`; Linux warning/API build stated green; 41/42; only declared-basis test failed because malformed high was accepted; CTest exit 8, 0.60 s.
- `project/artifacts/tdd/rs2-declared-basis/red-windows.txt:3-10,94-106`: same source/run; Windows 41/42; same sole failure; CTest exit 8, 1.12 s.
- `project/artifacts/tdd/rs2-declared-basis/green.txt:3-12,97-104,189-196`: source `68d0d98...`; Linux 42/42, 0.46 s; Windows 42/42, 1.06 s; warning/API builds stated green.
- `project/artifacts/tdd/rs2-public-pipeline/green.txt:3-8,98-103,188-193`: source `7041a489...`; Linux and Windows 41/41, with public pipeline green.
- `project/artifacts/tdd/rs2-terminal-rejections/green.txt:1-8,164-173,359-368`: source `b5f3d9f...`; Linux and Windows 43/43, including terminal rejection.

These records are internally consistent with the disposition and current test names. They predate at least the final deep-snapshot and q_div/q_l witness-only revisions. I did not contact GitHub, re-run, dispatch, cancel, or live-verify any run.

### 7.2 Compiler/tests explicitly **NOT EXECUTED** in this review

The following were **not executed**:

- CMake configure;
- project or OpenFHE compilation/linking;
- warning-as-error builds;
- `relin2_api_contract_test` or `rs2_api_contract_test` builds;
- CTest or any individual test binary;
- encryption, key generation, DCP, Tensor2, Relin2, RS2, RCB, decryption, or any other cryptographic operation;
- benchmark, profiling, sanitizers, fuzzing, or mutation execution;
- Git commands or commit checkout/comparison;
- CI dispatch, rerun, cancellation, or remote log retrieval.

Static assertions, comments, source structure, mathematical reasoning, and retained log text are not substituted for current execution evidence.

The authorized future hosted acceptance sequence is the one stated by the task, at the exact current commit with pinned OpenFHE:

```text
cmake -S . -B build -DCMAKE_PREFIX_PATH=<pinned-openfhe-prefix>
cmake --build build --parallel 2
cmake --build build --target relin2_api_contract_test --parallel 2
cmake --build build --target rs2_api_contract_test --parallel 2
ctest --test-dir build --output-on-failure
```

This review does not authorize or claim those commands were run.

## 8. Smallest remaining acceptance gaps

1. **Current-commit hosted result.** Retain complete Linux GCC and Windows MinGW64 configure, warning build, explicit API-target builds, and all 43 CTest results for `7928bb7634baa3603daf32806d70bd790938a353`. This is the only material execution gate created by the latest test-only changes.
2. **Narrow immutability wording or one additional context snapshot.** Do not claim every internal parameter field is covered. Either state the exact observed fields or directly snapshot the bound context's full aggregate basis plus any remaining public parameter getters.
3. **Cache wording.** Treat the deep own+unrelated fixture as the F4 proof. Treat terminal/public-pipeline map equality as keyless membership evidence unless an unrelated row is deliberately retained and deep-snapshotted there too.
4. **Malformed-object scope.** Do not state universal safe rejection for null native/declared children or every same-count inconsistent aggregate. Add only narrow public-seam tests if that guarantee is actually required.
5. **Mutation status.** The q_div witness has sound distinguishing mathematics, but a production mutation replacing q_l with q_div has not been executed. Do not describe it as mutation-test evidence unless such a hosted mutation run is retained.
6. **Precision and theorem boundary.** Ring-32 exact residue fixtures are not decoded multiplication, Theorem 4.8 non-wrap, Lemma 4.6, production security, performance, or 53/106-bit evidence.

## 9. Final risk conclusion

- **Blocking/high findings:** 0
- **Medium production findings:** 0
- **Medium acceptance-evidence gaps:** 1 — current `7928bb7...` hosted result pending
- **Low claim/test-depth findings:** 1 substantive group — exhaustive parameter/context immutability is not proved
- **Informational scope cautions:** keyless shallow cache comparisons, malformed-child limits, citation numbering, and precision/theorem boundaries

The strongest remaining rejection risk is not the RS2 algebra or the declared-basis production fix. It is an overstatement of evidence—claiming current-commit execution, exhaustive parameter immutability, universal malformed-object safety, or decoded/theorem/security precision from source inspection and ring-32 residue tests.

**FINAL VERDICT: PASS_WITH_GAPS — static source acceptance only; current hosted verification and final Codex acceptance remain pending.**
