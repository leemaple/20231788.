# RS2 follow-up independent static review — candidate 7928bb7

- **Reviewer seat:** ZCode, visible model GLM-5.3 (`builtin:bigmodel-coding-plan/GLM-5.3`). Static-only
  review per LOCAL-REVIEW-TASK.md and the archive TASK.md. No compilation, execution, crypto tests,
  installs, git operations, source modification, CI dispatch, agent delegation, or external messaging
  on this Mac. Every execution claim below is attributed to the packet's CI logs, never to this reviewer.
- **Candidate under review:** source `7928bb7634baa3603daf32806d70bd790938a353`, branch
  `agent/codex-rs2-01`, against pristine OpenFHE 1.5.0 `df495ba2e91739a6dc8f1de254fc5a41155ce504`
  (pin re-confirmed in `project/.github/workflows/dcp-rcb.yml:17,33,41`).
- **Prior review:** ZCode PASS_WITH_GAPS at `a801e2c6646b187bbae8a9ce4a3ee6808c259579`
  (`project/coordination/returns/rs2-zcode-a801e2c/REVIEW.md`), with Codex disposition in
  `project/coordination/RS2_REVIEW_DISPOSITION.md`. That verdict does not cover post-a801e2c changes.

## 0. Packet integrity (independently verified)

- `rs2-followup-7928bb7.zip`: **957646 bytes, SHA-256
  `467328d94f7d00a798e922db23cdd7a9245e65c577d3ec5017e0e84f6c05640d`** — matches LOCAL-REVIEW-TASK.md
  exactly (both size and hash).
- **Safety before extraction:** `zipinfo` over all 52 entries shows only directories (`drwxr-xr-x`) and
  regular files (`-rw-r--r--`); no symlinks, no absolute paths, no `..` traversal entries, no device/
  executable classes. Extraction was confined to the dedicated review folder.
- **Manifest closure:** `SOURCE-MANIFEST.json` enumerates 35 payload files (it excludes itself by
  design). All 35 were re-hashed from the fresh extraction: **35/35 matched** on both byte length and
  SHA-256. Disk holds exactly those 35 plus `SOURCE-MANIFEST.json` — no extras, none missing. The
  manifest's Gitleags/excluded-path claims are its own; my independent check is the zipinfo scan above.

## 1. Files inspected

Full read: `TASK.md`, `SOURCE-MANIFEST.json`, `CHANGES-SINCE-REVIEW.patch`,
`project/src/double_ckks.cpp` (all 1023 lines), `project/include/openfhe_2023_1788/double_ckks.h`,
`project/tests/rs2_test.cpp` (all 1031 lines), `project/coordination/RS2_REVIEW_DISPOSITION.md`,
`project/coordination/returns/rs2-zcode-a801e2c/REVIEW.md` (prior verdict),
`project/coordination/TEST_SEAMS.md`, `project/coordination/INDEPENDENT_ORACLE_PLAN.md`, all seven
`project/artifacts/tdd/**` files, and the relevant sections of `PAPER-2023-1788.txt`
(Definitions 4.1/4.3/4.5/4.7, Lemma 4.6, RCB identities). Targeted read of official references:
`dcrtpoly-impl.h` (operator==, DropLastElement/DropLastElements/DropLastElementAndScale),
`ckksrns-leveledshe.cpp` (ModReduceInternalInPlace), `rns-leveledshe.cpp` (ModReduce dispatcher),
`base-scheme.cpp` (SchemeBase::ModReduce), `base-leveledshe.h` (virtual declarations),
`cryptocontext.h` (Rescale), `rns-cryptoparameters.h` (GetModReduceFactor). Spot checks:
`project/CMakeLists.txt` (43 `add_test` entries), `project/tests/relin2_test.cpp` (deep-snapshot
precedent at 3580–3700), `project/.github/workflows/dcp-rcb.yml` (pins). Not needed for this slice:
`dcp_rcb_test.cpp`/`tensor2_test.cpp` bodies (unchanged by the diff), `PAPER-2023-1788.pdf` (text
version used).

## 2. Verdict — **PASS_WITH_GAPS**, scoped precisely

Scope: the RS2 slice of the Double-CKKS module (DCP, Tensor2, Relin2, RS2, RCB public seams;
FIXEDMANUAL, ≥3-tower contexts, level ≤ 2) at commit 7928bb7, **statically** audited against paper
2023/1788 and the supplied pristine OpenFHE references, with hosted-CI evidence treated as data.

Basis: the one production change since the first review (the declared-basis validation fix, §3) is
sound by construction and by demonstrated red→green on both platforms; I found **no new production
defect** in it or in the unchanged RS2/Tensor2/Relin2/DCP/RCB arithmetic. The gaps that keep this from
an unqualified PASS: hosted CI for the exact current head is still in progress (the deep-snapshot and
witness-discrimination assertions have no green run at 7928bb7 — §6 N1), a small enumerated set of
residual shallow comparisons and unexercised validation positions remains (N2, N3, N6), and by design
this review establishes nothing about decoded precision, Theorem 4.8, security, or performance.

## 3. Audit of the 9-line production fix (68d0d98) — the declared-basis defect

The defect (found after the first review): `ValidateCiphertext` checked each element's **actual**
towers (moduli, roots, order, format) but not the **declared** aggregate parameter basis, so a DCRT
element declaring the full 4-tower basis while containing only the 3 genuine active towers was accepted
by RS2. Demonstrated red on both platforms at f8e9760 (run 33835497108: 41/42, only
`rs2_declared_basis_mismatch` failing with "RS2 invalid input did not fail fast" — Linux job
100907050619, Windows job 100907050796), then green at 68d0d98 (run 33835813969: 42/42 both
platforms, test 42 rejecting both high- and low-member corruption with the exact new message).

The fix (`project/src/double_ckks.cpp:325–329` builds the expected basis; `:348–351` enforces it —
byte-identical to the patch hunk, confirmed in source):

- **Soundness of construction.** `expectedBasis` is `std::make_shared<DCRTPoly::Params>(
  *parameters_->GetElementParams())` followed by `level` × `PopLastParam()`. This is *exactly* the
  idiom official OpenFHE itself uses to maintain declared bases through tower drops —
  `DCRTPolyImpl::DropLastElement` (dcrtpoly-impl.h:675–677) and `DropLastElements` (:685–688) both do
  `new Params(*m_params); newP->PopLastParam();`, and `DropLastElementAndScale` (:693–712, called by
  `LeveledSHECKKSRNS::ModReduceInternalInPlace`, ckksrns-leveledshe.cpp:183–184) routes through it.
  Genuine pipeline elements therefore carry a declared basis produced by the same copy-then-pop
  construction the fix compares against; the CI-green genuine HYBRID/BV pipelines (§5 F2) confirm no
  false rejection. The exact-match test message ("declared RNS basis mismatch") also proves the
  rejection in the green run came from this check, not another.
- **Diagnostic ordering preserved.** The new check sits *after* the pre-existing per-element checks
  (aggregate format :332, ordered actual moduli :335, per-tower format :340, per-tower root/order
  :343–346) inside the same element loop, and after the level/basis-size coherence check (:322) that
  bounds the pop loop. All prior diagnostics keep their messages and precedence.
- **Safe parameter sharing.** `expectedBasis` is a fresh `make_shared` copy per `ValidateCiphertext`
  call; `PopLastParam` mutates only that copy's child vector (shared children are not written), and
  the bound context's pristine `GetElementParams()` object is never touched. Two constructions per
  pair validation (high+low) are negligible at these sizes. The `level ≤ fullModuli_.size()` guard at
  :322 runs before the pop loop; through the public seam `ValidateCiphertext` is private and only ever
  receives level 0, 1, or 2 against a ≥3-tower context, so the basis can never be popped empty.
- **No unintended restriction demonstrated.** Every genuine element source in scope (Encrypt at level 0,
  DCP's `DropLastElementAndScale` prefix, Tensor2 multiplication propagation, Relin2's raised-high with
  appended final tower, Relinearize output, Rescale output) was exercised CI-green at 68d0d98/b5f3d9f
  with the check active, on both HYBRID and BV. Restriction to the exact expected active prefix is the
  module's declared invariant, not an over-reach within its scope.
- **Rejection precedes arithmetic.** `ValidatePair` → `ValidateCiphertext` (double_ckks.cpp:886,
  568–571) runs before any RS2 rescale/arithmetic (927+); the fixture asserts the pair is deep-unchanged
  after rejection (rs2_test.cpp:959).
- **Honest limits (per task instruction, not claimed safe):** a declared basis whose *child* parameter
  vector contains null or field-mismatched entries relies on `ILDCRTParams::operator==` semantics;
  that class is defined in `dcrtpoly.h`, which is **not** in the packet, so null/malformed-child
  behavior is unproved here (the aggregate null pointer is checked at :349). No blanket statement is
  made that every malformed parameter object is safely rejected.

**Assessment: correct, minimal, and consistent with official idiom. No defect found.**

## 4. Paper/spec correctness — unchanged RS2 core re-verified

The diff does not touch RS2/Tensor2/Relin2/DCP/RCB arithmetic; I re-verified the unchanged core against
the paper text (numbering: Definition 4.1 = pair tensor, 4.3 = pair relinearize, 4.5 = pair rescale,
4.7 = Mult2 := RS2∘Relin2∘Tensor2; note TASK.md's "Definition 4.7/RS2" shorthand refers to the
composite):

- **Definition 4.5** (PAPER-2023-1788.txt:785–807): `RS2(CT) = (RS_{q_l}(ĉt), RS_{q_l}(q_div·ĉt+ĉt) −
  q_div·RS_{q_l}(ĉt))` over `Q_{l−1}`. Implementation: two independent `context_->Rescale` calls on
  clones (double_ckks.cpp:952–953, comment cites Def. 4.5), `EvalMultNoCheck(rescaledHigh, qDiv)`
  (:966, verified against cryptocontext.h semantics — integer multiply without metadata adjustment),
  guarded raw per-component subtraction into a clone (975–988), output basis = ordered prefix minus
  last (:916–918), only `q_l` consumed, `q_div` preserved (:1000–1004). Correct.
- **Definition 4.1** (txt:584–596): Tensor2 discards `ĉt1⊗ĉt2`, yields 3-component members with the
  fixed `q_div` carried: matches Tensor2 (671–676: high×high; cross terms added; no low×low) and the
  divisor/manifest plumbing. Correct.
- **RCB identity** (txt:809–815): `RCB(RS2(CT)) = RS(RCB(CT))` — asserted coefficientwise by the
  independent oracle (rs2_test.cpp:754–756) and structurally exact since the subtraction cancels the
  `q_div·RS(high)` term in the recombination. Correct.
- **Scale bookkeeping** (correcting the prior review's table erratum, per disposition): with
  S = 2^30 — for FIXEDMANUAL `GetModReduceFactor` returns `m_approxSF` = the configured scaling factor,
  *not* the prime (rns-cryptoparameters.h:642–649) — the recorded chain is DCP S² → ReadyForRS2 S³
  (Tensor2 normalization S²·S²/S) → RefreshRequired S³/S = S², while the two *logical* scales divide by
  the native prime `q_l` (double_ckks.cpp:926–934). Source and test assertions
  (rs2_test.cpp:636–641: modReduceFactor == GetScalingFactorReal(0) **and** ≠ droppedModulus) pin the
  distinction. Consistent.
- **Lifecycle boundaries:** RS2 ReadyForRS2→RefreshRequired (:887–889) and Tensor2's ReadyForFirstMult
  guard (:660–663) reject before arithmetic; RS2 never accesses evaluation keys anywhere in its body
  (only Relin2 does). Verified.

**No paper/spec defect found in the production change or the unchanged core.**

## 5. Disposition of prior findings F1–F9 and the new defect

Distinguishing, as required: **implemented coverage** (tests exercising already-present behavior),
**demonstrated red-green correction** (a real pre-fix failure retained, then green), and **unexecuted
mutation tests** (witness properties computed in the oracle, no production code mutated).

| Item | Disposition | Evidence class | Status |
|---|---|---|---|
| F1 terminal rejections | Closed for the requested cases by b5f3d9f: genuine pipeline to `RefreshRequired`, own eval key removed, repeated `RS2` + Tensor2 left/right/both rejected with exact messages, pairs deep-unchanged, cache untouched (rs2_test.cpp:891–928; guards 887–889/660–663). Run 33836142693: 43/43 both platforms (0.60 s / 0.97 s). | Implemented coverage of existing guards | **Closed, observed (CI)** |
| F2 untouched pipeline | Closed by 7041a48: HYBRID and BV(digit 0) genuine complex nonzero pipelines, no coefficient replacement, own key removed before RS2/RCB, full state manifest + cpp_int exact oracle over every coefficient/tower (witness flags off — correct for random data), input ciphertexts/pairs deeply re-checked (rs2_test.cpp:839–889). Run 33834861766: 41/41 both platforms; re-confirmed green in runs 33835813969 and 33836142693. Coefficient/state evidence only, no decoded-precision claim — correctly scoped. | Implemented coverage | **Closed, observed (CI)** |
| F3 shallow snapshots | Coverage extension authored in 21c13fd: `PolynomialSnapshot` pins aggregate params identity/order/modulus/root/format, every declared and actual tower's params identity/order/modulus/root, tower formats, and every native value (rs2_test.cpp:69–142); wired into all pair/ciphertext immutability checks (:235). Residual shallowness enumerated in N2. | Implemented coverage | **Authored, hosted run PENDING** |
| F4 shallow key-cache | Coverage extension authored in 21c13fd: retained-cache fixture requires own **and** unrelated genuine key rows, deep-snapshots every row/key (identity, context, tag, subtype) and all A/B polynomials incl. values, checked after RS2 and again after the oracle's RCB (rs2_test.cpp:144–200, 820–834). Keyless fixtures keep the shallow map `==`, which is vacuous there (own row asserted removed; single-context process per CTest case ⇒ empty cache). | Implemented coverage | **Authored, hosted run PENDING** |
| F5 ModReduce dispatcher | Gap closed with supplied pinned sources: `cryptocontext.h:2507–2510` Rescale → `base-scheme.cpp:95–99` SchemeBase::ModReduce → `rns-leveledshe.cpp:311–321` clone + FIXEDMANUAL branch → virtual (base-leveledshe.h:735) → `ckksrns-leveledshe.cpp:172–192` tower drop + degree/level/scale update. I verified each definition in-packet. Remaining external link: `ckksrns-leveledshe.h` class declaration (cited by disposition as pinned GitHub blob `52bf074b…`) — inheritance/override inferred from the supplied .cpp definitions + base virtual + CI behavior. | Source verification | **Closed in-packet except one header link (inferred)** |
| F6 partial corruption positions | Accurately scoped, retained: declared-basis fixture corrupts element 0 of one member per iteration (green covers both members); mixed-format fixture same shape. No claim of exhaustive positional coverage. See N3. | Partial coverage, by design | **Open (minor)** |
| F7 dotted repo URL | Unchanged since first review; intentional (repository name has a trailing dot). | Resolved | **Closed** |
| F8 minimal-tower boundary | Deliberate boundary retained (Relin2 requires towers ≥ degree). | Documented | **Closed** |
| F9 prime-role witness | Split outcome. **Arithmetic witness: closed as authored coverage** — 7928bb7 adds ±(min(q_div,q_l)/2+1) high coefficients (rs2_test.cpp:554–560) and the oracle's `differsFromDivisorQuotient` assertion (:760–761, 769–770). I verified the math: under the oracle's centered rounding (`Center`, remainder in (−q/2, q/2], strict `>` — rs2_test.cpp:392–398, 446–454), x = ±(p+1)/2 gives quotient ±1 under division by the smaller prime p and 0 under the larger (≥ p+2, so x ≤ (q−1)/2 even in the twin-prime case — no tie exception), for **either prime ordering and both signs**; the integer quotient difference is exactly ±1, nonzero in every target tower. This is an **oracle-computed mathematical witness, not an executed production mutation** — production is never actually given a wrong-divisor rescale. **Metadata-swap fixture still absent** (set the pair's divisor manifest to the q_l value, expect the guard at double_ckks.cpp:444–446): one-block fixture analogous to the dcp_rcb tamper suite remains possible. | Witness coverage (unexecuted mutation) | **Arithmetic half: authored, hosted run PENDING; metadata half: open (minor)** |
| New declared-basis defect | **Demonstrated red-green correction** (see §3): real pre-fix failure on both platforms at f8e9760 (41/42), minimal 9-line fix at 68d0d98 green on both platforms (42/42), still green at b5f3d9f (43/43). Fix audited sound this review. Not an unexecuted mutation — the malformed input was really fed to public RS2. | Red-green correction | **Closed, observed (CI)** |

## 6. Findings

### Standards (test/coverage/engineering)

| ID | Severity | Finding | Location | Consequence | Minimal test/fix | Label |
|---|---|---|---|---|---|---|
| N1 | Medium (process) | No hosted run at the exact reviewed head 7928bb7: the 21c13fd deep-snapshot assertions and the 7928bb7 `differsFromDivisorQuotient` assertion have green evidence only at earlier slices (7041a48/68d0d98/b5f3d9f), not at this head | artifacts/tdd/*; TASK.md line 21 | A compile or assertion defect in the newest test-only code would surface only at hosted CI; nothing here certifies the current tree end-to-end | Await the in-progress run (cmake configure pinned OpenFHE → build -j2 → explicit API targets → ctest --output-on-failure). Not executed locally | Pending |
| N2 | Low | Enumerated residual shallowness after 21c13fd: (a) keyless fixtures compare the eval-key cache by shallow map `==` (vacuous — cache asserted empty); (b) `PolynomialSnapshot` pins params identity + {order, modulus, root} but not fields of `ILDCRTParams`/`ILNativeParams` beyond those (class not in packet, cannot enumerate); (c) ciphertext-scalar immutability beyond `Ciphertext` `operator==`'s scope (`ciphertext.h` not in packet) | rs2_test.cpp:885, 69–109, 234 | An in-place mutation of an unenumerated derived/shared field or unlisted ciphertext scalar would pass immutability checks; no production path in RS2/RCB writes such fields (clone-only arithmetic — inferred from source) | Optional: extend `NativeParameterSnapshot` with `GetRingDimension()`; no action required for acceptance | (a) observed/vacuous; (b)(c) inferred, unverifiable in packet |
| N3 | Info | Corruption fixtures still replace element 0 of one member at a time (declared-basis, mixed-format); production loops all elements × members; no separate low-member red retained (green covers both members) | rs2_test.cpp:945, 978; production 331–352 | A guard bug affecting only element 1 / the other member's element 1 would not be caught by these fixtures (shared-path risk is low: same loop) | Optional second-element/second-member variant | Observed |
| N4 | Info | `BoundaryValues` requires ringDimension ≥ 21, but the two witness overwrites at `ringDimension−2/−1` need ≥ 23 to keep all 21 boundary entries; MakeContext pins 32, so current runs are safe; a future ring-dim change to 21/22 would silently drop two boundary witnesses | rs2_test.cpp:503–504, 559–560 | Latent fixture fragility only under a parameter change; the differs-flags could weaken without any signal | One line: require `ringDimension >= witnesses.size() + 2` | Observed (current) / inferred (future) |
| N5 | Info | `MakeContext` now defaults to HYBRID (+ digit size 0), changing the regime of the pre-existing rs2 fixtures (previously OpenFHE default); all 43 tests are green at b5f3d9f under the new default on both platforms | rs2_test.cpp:512–530; CMakeLists 43 tests | None open; noted for traceability of what the green runs exercised | None | Observed (CI) |

### Paper/spec correctness

**No new spec finding.** The production change adds validation only; it does not alter any paper
operation. The unchanged core re-verified against Definitions 4.1/4.5 and the RCB identity (§4). The
prior review's scale-table erratum (p vs 2^30) is corrected in §4 and does not affect any assertion.
F9's metadata-swap half is carried as the only spec-adjacent residual (§5, minor).

## 7. Explicitly NOT EXECUTED in this static review

- No cmake configure, no build (neither main nor warning nor the explicit Relin2/RS2 API contract
  targets), no ctest, no benchmark — on any platform. **All compiler and test outcomes cited are the
  packet's hosted-CI logs (runs 33834861766, 33835497108, 33835813969, 33836142693), not this
  reviewer's execution; source-level assertions in this REVIEW.md are reading evidence, not execution
  evidence.**
- No OpenFHE or project code was compiled, installed, mutated, or executed; no git operations; no CI
  dispatch; the in-progress hosted run for 7928bb7 was not awaited or influenced.
- No Mac-side cryptographic computation of any kind.

## 8. Smallest remaining acceptance gaps

1. **Hosted green at 7928bb7** (N1) — the only blocking gap: current-head evidence for the deep
   immutability snapshots and the new witness assertion.
2. **N2 residuals** — unenumerated params/ciphertext fields; acceptable if acknowledged rather than
   claimed. Any wording asserting "full" or "complete" immutability of parameters/keys beyond the
   snapshotted fields would be overbroad and should not be made.
3. **N3 positional coverage** and the **F9 metadata-swap fixture** — one-block test additions if
   desired; neither protects a presently demonstrated defect.
4. Precision/theorem claims (decoded accuracy, Theorem 4.8 non-wrapping, Lemma 4.6 bound,
   53/106-bit behavior, security, performance) remain **out of scope and unclaimed** for ring-32,
   depth-3, p=30 fixtures; this review adds nothing to them.

If a future bug is suspected in the fix itself, the reproducible public-seam test is exactly the
retained fixture: build `rs2_declared_basis_mismatch` against the suspect source — it constructs the
independent declared-4/actual-3 element through `const_pointer_cast` + `GetElements().at(0)`
(rs2_test.cpp:941–959) and requires the exact `DoubleCKKS: pair high|low declared RNS basis mismatch`
rejection with the pair deep-unchanged.

*Static line references are to the extracted packet files (paths relative to the ZIP root;
`output/MANIFEST.sha256` records the exact bytes this review consumed, including the archive itself).*
