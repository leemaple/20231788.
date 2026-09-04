# Independent static review — RS2 candidate a801e2c (OpenFHE paper 2023/1788)

- **Reviewer seat:** ZCode (Mac, static-only per LOCAL-REVIEW-TASK.md). No compilation, execution,
  crypto tests, candidate modification, agent dispatch, or external messaging. All execution claims
  below are attributed to the packet's CI evidence, never to this reviewer.
- **Candidate:** `a801e2c6646b187bbae8a9ce4a3ee6808c259579` (branch `agent/codex-rs2-01`).
- **OpenFHE reference:** pristine 1.5.0 `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- **Verdict: PASS_WITH_GAPS.** No production defect was observed in `RS2`, its shared validation
  changes, or its tests. Four actionable test-coverage gaps / snapshot blind spots (F1–F4 below) and
  three informational notes (F5–F8) remain; none invalidates the RS2 implementation.

## 0. Packet integrity (independently verified)

- `rs2-review-a801e2c.zip`: 913137 bytes, SHA-256
  `91ea855f4bcf81655ea10b3ad058f7e0a2d3660c2b590f175675c3812c87411c` — matches the brief exactly.
- `SOURCE-MANIFEST.json`: all 30 entries re-hashed against the fresh extraction — **30/30 matched**
  (bytes and SHA-256); no files on disk beyond the manifest and no manifest entry missing.
- Candidate files were only read; `output/` contains the only writes. The draft Windows `TASK.md`
  inside the ZIP was treated as context; the local brief overrode it.

## 1. What the paper requires (source of truth)

From `PAPER-2023-1788.txt` (Cheon–Cho–Kim–Sthlé, ePrint 2023/1788):

- **Definition 4.5 (Pair rescale)**, lines 785–807: for `CT = (ĉt, ˇct) ∈ R²_{Q_l} × R²_{Q_l}`,
  `q_l = Q_l/Q_{l−1}`,
  `RS2_{q_l}(CT) = ( RS_{q_l}(ĉt), RS_{q_l}(q_div·ĉt + ˇct) − q_div·RS_{q_l}(ĉt) )`,
  with result in `R²_{Q_{l−1}} × R²_{Q_{l−1}}`.
- **Recombination identity**, lines 809–815: `RCB_{q_div}(RS2_{q_l}(CT)) = RS_{q_l}(RCB_{q_div}(CT))`,
  and `RS2` "only consumes a factor `q_l`" (contrast with the naive `DCP∘RS∘RCB` which also loses `q_div`).
- **Lemma 4.6**, lines 817–851: the decryption error introduced by `RS2` is bounded by `(h+1)/2`.
  This is a theorem about *noise growth*, not something a synthetic coefficient test can establish.

## 2. What official OpenFHE 1.5.0 actually does (verified against packet references)

- `Rescale` (cryptocontext.h:2507–2510) = `GetScheme()->ModReduce(ct, GetCompositeDegreeFromCtxt())`;
  for FIXEDMANUAL the composite degree is the default `BASE_NUM_LEVELS_TO_DROP = 1`
  (rns-cryptoparameters.h:131, 1529), so one call drops exactly one tower.
- `LeveledSHECKKSRNS::ModReduceInternalInPlace` (ckksrns-leveledshe.cpp:172–191): drops the **last
  active tower** of each component via `DropLastElementAndScale` (centered rounding; the last tower is
  switched to COEFFICIENT form for the rounding correction, dcrtpoly-impl.h:693–712), sets
  `noiseScaleDeg −= 1`, `level += 1`, and divides the recorded scaling factor by
  `GetModReduceFactor(sizeQl − 1 − i)`.
- `GetModReduceFactor` (rns-cryptoparameters.h:642–649) **returns `m_approxSF` (the configured 2^p),
  not the native prime `q_l`, for FIXEDMANUAL** — the index argument is ignored for this technique.
- `EvalMultNoCheck(ct, k)` (cryptocontext.h:2073–2079): clones and multiplies every component by the
  integer `k` **without touching level/degree/scale metadata** — exactly what `q_div·RS_{q_l}(high)`
  needs.
- Table indexing in `ModReduceInternalInPlace` uses `diffQl = sizeQ − sizeQl` computed from the
  ciphertext's *actual* tower count, which for a level-1 pair member (3 of 4 towers) selects the row
  that drops `fullModuli_[size−2]` — i.e. `q_l`, never `q_div` (which is not an active tower of the pair).

## 3. RS2 implementation audit (project/src/double_ckks.cpp:876–998)

Line-by-line against Definition 4.5:

| Paper term | Candidate code | Verdict |
|---|---|---|
| `RS_{q_l}(ĉt)` | `context_->Rescale(highInputConst)`, line 943, on a clone of `high_` (927) | correct — one centered rescale of the high member on the active basis |
| `RS_{q_l}(q_div·ĉt + ˇct)` | `recombinedInput = RCB(relinearized)` (933, = `q_div·high + low` mod `Q_l`, RCB at 1000–1011) then `context_->Rescale(...)` (944) | correct — second, *independent* centered rescale; the two `Rescale` calls are separate (comment at 942 cites Def. 4.5) |
| `− q_div·RS_{q_l}(ĉt)` | `EvalMultNoCheck(rescaledHigh, qDiv)` (957) then per-component DCRTPoly subtraction `newLowElements[index] -= scaledHighElements[index]` (966–979) | correct — raw subtraction with an explicit per-component basis-equality guard (972–977) instead of `EvalSub`, so no hidden level/degree alignment can occur |
| result in `R²_{Q_{l−1}} × R²_{Q_{l−1}}` | `outputModuli` = ordered prefix minus last (907–908), `outputLevel = level+1 = 2` (909), two components (995) | correct — only `q_l` consumed |
| `q_div` preserved | result divisor `qDiv` (991), paper-scale divisor `qDiv` (985–990), RCB still uses `divisor_` | correct |

**Prime roles and guards** (883–905): `q_l = orderedModuli_.back()` (last *active* tower), checked odd,
checked `q_l ≠ q_div`, checked to equal `fullTowerParameters[orderedModuli_.size()−1]` (the OpenFHE
mod-reduce table index for a 3→2 tower drop), `q_div` checked to equal the final full-basis tower, and
`GetModReduceFactor` checked finite/positive. These are consistent-by-construction with the ordered
basis already enforced by `ValidatePair` (448/456) — redundant but sound belt-and-braces.

**Both scale contracts** (task Q1): the recorded scaling factor is divided by the *official getter*
value (`recordedFactorDivisor = GetModReduceFactor(...)`, 902, 911–912) = 2^p in FIXEDMANUAL — matching
`ModReduceInternalInPlace`'s own metadata update (ckksrns-leveledshe.cpp:188–189). The two *logical*
scales divide by the *native* `q_l` (917–921). The two are deliberately different quantities and the
test pins the distinction (rs2_test.cpp:485–490 asserts `modReduceFactor == GetScalingFactorReal(0)`
**and** `droppedModulus != modReduceFactor`). Full chain with p = 2^30, 4-tower context:

| state | level | towers | deg | recorded | logical high | logical recombined |
|---|---|---|---|---|---|---|
| DCP input | 0 | 4 | 2 | p² | — | — |
| ReadyForFirstMult | 1 | 3 | 2 | p² | p²/q_div | p² |
| Tensor2 / ReadyForRS2 | 1 | 3 | 3 | p³ | (p²/q_div)² | p⁴/q_div |
| **RS2 output (RefreshRequired)** | 2 | 2 | 2 | p³/2^p | (p²/q_div)²/q_l | p⁴/(q_div·q_l) |

All powers of two, so the double arithmetic is exact; the long-double logical-scale expressions are
recomputed in `ValidatePair` (461–554) in the same operation order as they were produced in
DCP/Tensor2/RS2, so the exact-equality checks are rounding-stable.

**Level/degree/format/basis/context/tag/slots:** every output member is validated by
`ValidateCiphertext` immediately after each production step (948–954, 959–962, 981–983) and the final
pair by `ValidatePair(result)` (996): level 2, degree 2, EVALUATION format aggregate **and per tower**,
ordered basis prefix, exact context identity, key tag and slots preserved, two components,
`RefreshRequired` lifecycle, `q_div` unchanged. The mixed native-tower format guard added by this
candidate is double_ckks.cpp:333–342 (per-tower `GetFormat()` check inside `ValidateCiphertext`) — this
is the "three-line guard" of the diff, and it is shared by every lifecycle, not RS2-only.

**No input mutation:** RS2 clones before both rescales (927; RCB internally clones, 1003), subtracts
into a clone of the rescale output (966), and never writes through `relinearized`'s members. RS2 never
accesses evaluation keys at all (contrast Relin2 at 704–812).

**Algebraic identity check:** `RCB(RS2(CT)) = q_div·RS(h) + [RS(q_div·h+l) − q_div·RS(h)]
= RS(q_div·h + l) = RS(RCB(CT))` — exact per coefficient (the rounding already happened inside `RS`);
this is precisely what the independent oracle asserts (rs2_test.cpp:586–600).

## 4. Mandatory question 2 — rejection before arithmetic

`RS2` order of operations: `ValidatePair` (877, full manifest + both members through
`ValidateCiphertext`) → lifecycle guard (878–879) → prime/table guards (883–905) → output-scale
finiteness (913, 922–925) → arithmetic (927+). Everything rejectable is rejected before any rescale.
Coverage inventory in `ValidateCiphertext` (280–344): null; exact context pointer identity (293);
CKKS packed encoding (296); level (299); component count (302); noise degree (307); recorded scale
finite/positive/exact (310); non-empty matching key tag (314); slots (317); level-vs-basis-size
coherence (322); per-element aggregate format (327); **actual** per-tower moduli vs manifest
(`SameOrderedModuli`, 330); per-tower format (335); per-tower root-of-unity and cyclotomic order vs the
bound context (338–339). `ValidatePair` adds lifecycle-specific level/basis/scale/degree expectations
per lifecycle (442–554), paper-scale descriptor consistency (544–553), and the invalid-enum default
(533–534).

Corruption fixtures use only public seams, as required: `const_cast` on accessor references and
`const_pointer_cast` + `SetElements`/`SetKeyTag`/… on the read-only members — dcp_rcb_test.cpp:652–762
(divisor, basis manifest, key tag manifest+member, encoding, slots, component count, tower order,
aggregate format, recorded scale, level, noise degree, paper scale, via RCB), rs2_test.cpp:680–696
(mixed tower format via RS2, both members), rs2_test.cpp:611–633 (wrong lifecycle via RS2). Because
RS2, RCB and Tensor2 all funnel through the same `ValidatePair`, the RCB-labelled tamper suite covers
RS2's shared rejection paths; lifecycle-specific expected values are positively pinned by the pipeline
tests. One residual blind spot: no RS2-labelled corruption of a *ReadyForRS2* member's recorded scale
(the machinery is identical to the RCB-tested path, so risk is negligible) — see F9 (minor).

## 5. Mandatory question 3 — oracle independence and coverage

`CheckExactArithmeticOracle` (rs2_test.cpp:534–609):

- **Independence:** expected values are computed with `boost::multiprecision::cpp_int` via textbook
  extended-GCD modular inverses (221–241), CRT reconstruction (259–271), centered remainder/quotient
  (251–257, 305–313). No OpenFHE rescale/helper is used to build expectations (OpenFHE is used only
  for the coefficient-format switch of *actual* outputs, 282–286, which is format plumbing, not
  rescale arithmetic). The expected low is `RescaleCentered(recombined) − divisor·RescaleCentered(high)`
  — the definition itself, not the production shape.
- **Coverage:** all 32 ring coefficients × both RLWE components × **every** target tower, residues
  compared directly (586–604); both members' inputs reconstructed; output high, output low and the
  public `RCB` of the result all checked. A "dynamic mathematical witness" of this kind is not an
  executed production mutation — correctly, none is claimed.
- **Distinguishing power:** (a) `RS(low)` shortcut is distinguished by `differsFromDirectLowRescale`
  (601–608), computed on *oracle* values so it validates the witness set itself, not just the
  implementation; boundary witnesses at ±q_l/2, ±q_div, ±Q_l/2 (333–369) make the rounding non-linearity
  visible. (b) `q_l` vs 2^p metadata divisor distinguished at 485–490. (c) `q_div` vs `q_l` prime-role
  confusion is caught *arithmetically*: the oracle's expected low uses `divisor = GetDivisor()` while
  the dropped modulus is `orderedModuli_.back()`; swapping the roles in production would break the
  per-tower residue equalities. The known open question — an *explicit metadata* q_div↔q_l swap
  witness (set the pair divisor manifest to the q_l value, expect `pair divisor does not match...`,
  guarded at 435–436) — is not present; it is a one-block fixture analogous to dcp_rcb_test.cpp:656–661
  (F9, low value).

## 6. Mandatory questions 4–6

**Q4 (untouched nonzero public-pipeline coverage):** the only valid-arithmetic RS2 test encrypts
zeros (rs2_test.cpp:639) and then *replaces* both members' coefficients (655, `InstallControlledValues`).
No test runs RS2 on untouched, nonzero, genuinely key-switched pipeline output; Relin2 has exactly such
a case (`relin2_representative_public_input`, relin2_test.cpp:4283–4307, nonzero complex plaintexts at
3392–3401). Needed: one additional `rs2_representative_public_input` case — nonzero plaintexts, public
DCP→Tensor2→Relin2→RS2 untouched, assert the full result-state manifest and run the same cpp_int exact
oracle over the untouched coefficients (the oracle is value-agnostic; keep the
`differsFromDirectLowRescale` witness assertion only in the controlled test, since arbitrary real data
is not guaranteed to distinguish). This closes the "validation too strict for genuine pipeline metadata /
arithmetic divergence off boundary values" risk. Per the brief: no decoded-precision, theorem, or
security conclusion follows from either the existing or the proposed synthetic test. → **F2**.

**Q5 (snapshot blind spots vs production mutation):** both blind spots are real, both are *test* gaps,
neither is a production mutation:
- `CheckCiphertextUnchanged` (rs2_test.cpp:88–109) relies on `*ct == *clone`;
  `DCRTPolyImpl::operator==` (dcrtpoly-impl.h:430–434) compares aggregate format, cyclotomic order,
  aggregate modulus and tower values — **not** per-tower root-of-unity/modulus/format or params
  identity. RS2's production code (927–998) performs only Clone/Rescale/EvalMultNoCheck/checked
  subtraction and never re-points tower params, so nothing observable is being missed today. Minimal
  independent fix: extend `CiphertextSnapshot` with the per-tower `{modulus, root, cyclotomicOrder,
  format, values}` snapshot pattern already proven in relin2_test.cpp:3577–3605
  (`TowerSnapshot`/`DcrtSnapshot`) — ~30 lines, no framework. → **F3**.
- rs2_test.cpp:657–663 copies the process-wide eval-mult-key map and compares with `==`;
  `std::map` of `shared_ptr` compares pointer identity, so an in-place `EvalKey` mutation would pass.
  RS2 never reads evaluation keys (no `GetAllEvalMultKeys` in RS2), so this is vacuous rather than
  wrong. Minimal fix: reuse `SnapshotDeepKeyCache`/`CheckDeepKeyCacheMatches`
  (relin2_test.cpp:3634–3700, genuinely deep A/B-vector snapshots) or drop the assertion. → **F4**.

**Q6 (RCB-ability and terminal rejection):** the RS2 output *is* publicly RCB'd inside the oracle
(rs2_test.cpp:554–564) with level/degree/scale preservation and the exact `RCB(RS2(CT)) = RS(RCB(CT))`
identity; all three lifecycles are RCB-covered across the suite (ReadyForFirstMult:
dcp_rcb_test.cpp:502 + 652–762; ReadyForRS2: relin2_test.cpp:3893–3911 with deep DCRT snapshots;
RefreshRequired: rs2_test.cpp:554–564). Terminal rejection *guards* exist and precede any arithmetic
and any key access (RS2: 878–879, and RS2 touches no keys; Tensor2: 651–653, before the
`EvalMultNoRelin` calls at 662–667). **Missing negative coverage:** no test feeds a `RefreshRequired`
pair to `RS2` or to `Tensor2` — rs2_test's wrong-lifecycle case uses a ReadyForFirstMult pair only
(620–629), and relin2_test's `TestTensor2RequiresFirstLifecycle` (4560–4598) covers ReadyForRS2
operands only (it predates RS2, the only RefreshRequired producer). → **F1**.

## 7. Findings summary

Classifications per the brief: *observed defect / inference / test blind spot / pending validation*.

| ID | Sev. | Class | Summary | Location |
|---|---|---|---|---|
| F1 | Medium | test blind spot | No `RS2(RefreshRequired)` or `Tensor2(RefreshRequired, …)` rejection tests; guards exist and precede key access but are unexercised | guards double_ckks.cpp:878–879, 651–653; gap in rs2_test.cpp:611–633 / relin2_test.cpp:4560–4598 |
| F2 | Medium | test blind spot | No untouched nonzero public-pipeline RS2 coverage (zeros + replaced coefficients only) | rs2_test.cpp:639–655; precedent relin2_test.cpp:4283–4307 |
| F3 | Low | test blind spot | rs2 input-immutability snapshot misses per-tower params/root/format changes (`operator==` scope) | rs2_test.cpp:88–109 vs dcrtpoly-impl.h:430–434; deep pattern relin2_test.cpp:3577–3605 |
| F4 | Low | test blind spot | Eval-key cache invariance check is pointer-shallow (and vacuous for RS2) | rs2_test.cpp:657–663; deep pattern relin2_test.cpp:3634–3700 |
| F5 | Low | pending validation | `SchemeCKKSRNS::ModReduce` dispatcher file is outside the six provided references; FIXEDMANUAL routing inferred from cryptocontext.h:2507–2510 + base-leveledshe.h:654 + ckksrns-leveledshe.cpp:172–191 and closed behaviorally by CI-green exact metadata assertions | — |
| F6 | Info | test blind spot (minor) | Mixed-format fixture corrupts only element 0 / tower 0 per member; production guard loops all elements×towers; red run observed only the high member | rs2_test.cpp:684–685; guard double_ckks.cpp:333–342; red.txt:14–15 |
| F7 | Info | resolved | `https://github.com/leemaple/20231788..git` is intentional (repository name has a trailing dot, per TASK.md:3); CI fetches succeeded | dcp-rcb.yml:118 |
| F8 | Info | — | 3-full-tower minimum contexts cannot reach RS2 (Relin2 requires towers ≥ degree, 701–703); documented and deliberate | dcp_rcb_test.cpp:602–620 |
| F9 | Info | test blind spot (minor) | No explicit q_div↔q_l *metadata-swap* witness (arithmetic-role confusion is already caught by the exact oracle); one-block fixture possible | suggested at ValidatePair guard 435–436 |

**Suggested public-seam fixtures (minimal, no framework):**

1. `rs2_terminal_rejections` (F1): produce `result = module.RS2(relinearized)`; snapshot; assert
   `RS2(result)` throws exactly `DoubleCKKS: RS2 requires ReadyForRS2 input` and
   `Tensor2(result, result)` throws exactly `DoubleCKKS: Tensor2 requires ReadyForFirstMult inputs`;
   assert pair unchanged (deep snapshots per F3) and the eval-key cache untouched (deep snapshot per F4).
2. `rs2_representative_public_input` (F2): nonzero plaintexts; untouched pipeline; full
   `CheckResultState`-style manifest assertions + cpp_int exact oracle over the real coefficients;
   no `differsFromDirectLowRescale` requirement; optionally decrypt `RCB(RS2(pair))` vs a plaintext-
   domain reference only as a smoke signal, labelled as such.
3. Extend `CiphertextSnapshot` with per-tower modulus/root/cyclotomicOrder/format/values (F3) and swap
   the shallow key-map copy for the deep cache snapshot (F4) — both patterns already exist verbatim in
   relin2_test.cpp.

**Minimal fixes:** F1/F2/F3/F4 require *test-only* additions; no candidate source change is required
for any finding. F5 needs no action beyond recording the inference.

## 8. Evidence basis and non-claims

- Execution evidence is the packet's CI logs only: run 33831920036 (ed00f35, 39/39), run 33832462125
  (cd2165a, 39/40, only `rs2_mixed_tower_format` red because RS2 accepted the mixed format), run
  33833020685 (a801e2c, Linux job 100899799757 40/40 in 0.37 s; Windows job 100899799442 40/40 in
  0.90 s; warning builds and explicit Relin2/RS2 API contract builds passed). These are CI observations,
  not this reviewer's execution.
- This review did **not** establish: decoded numerical precision, Lemma 4.6's noise bound, any
  security property, 53/106-bit behaviour, or completeness of `Mult2`/`Add`/`Sub` (absent from this
  candidate by design).
- Static line references are to the extracted packet files (paths relative to the ZIP root).
