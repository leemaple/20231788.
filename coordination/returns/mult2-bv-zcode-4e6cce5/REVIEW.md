# Independent static review — Mult2 BV conditional execution certificate

**Verdict: PASS_WITH_GAPS** (full project acceptance explicitly out of scope)

- **Reviewer seat:** ZCode third review seat, static-only audit on macOS (darwin 25.3.0, arm64).
- **Actual model/UI identity:** ZCode agent powered by `builtin:bigmodel-coding-plan/GLM-5.3`.
  This is **not** Fable 5.1 and **not** ChatGPT Pro. No Fable review exists (its terminal
  request returned 403 before inference; correctly not counted).
- **Date:** 2026-09-04 (Asia/Shanghai). Work confined to
  `/Users/lifeng/Documents/20231788-openfhe-zcode-bv-review-20260904`.
- **Scope honored:** no project/OpenFHE compilation, no crypto tests, no dependency installs,
  no source edits, no git mutation, no CI dispatch, no browser/external access, no other agent
  dispatch. Only `output/REVIEW.md` and `output/MANIFEST.sha256` written; extraction under
  `extract-4e6cce5/` only.

---

## 1. Input verification (independently rechecked)

| Gate | Result |
|---|---|
| ZIP byte size | 1,064,259 bytes — exact match to LOCAL-REVIEW-TASK.md |
| ZIP SHA-256 | `83a28e43e72d0874700be3ed49f67e8ba9f85984507887fafcca4210ac2e7479` — exact match |
| Entry count | 77 total = 59 regular files + 18 directories — matches |
| Unsafe paths | None: no absolute/drive paths, no `..` segments, no non-printable names |
| Duplicates / links | No duplicate entry names; no symlink members (mode check) or on-disk links |
| CRC structural test | `testzip` clean for all 77 entries |
| SOURCE-MANIFEST.json | All **58** entries re-hashed: every size and SHA-256 matches byte-exact |
| Closure | Disk = 59 files = 58 manifest entries + `SOURCE-MANIFEST.json` itself — exact |
| Composition | 46 `project/` files + 11 paper/official files + TASK.md = 58, matching stated provenance |
| Secret scan | Independent crude credential-pattern scan: no findings (benign field names only, e.g. `secret_h`) — consistent with, but weaker than, the claimed Gitleaks 8.30.1 zero-findings |
| Post-review integrity | All 59 input files re-hashed after the audit completed: unchanged (see `output/MANIFEST.sha256`) |

Not independently re-verifiable in this seat (accepted as claimed background evidence, flagged
as such): the byte-match of the 46 project files against Git `4e6cce53b23a6022bf6f942ab973aaa6bf9e5bf6`
(no clone/network authorized), the "tested `9bf86cb…` plus CI/coordination-only delta" claim,
the match of the 11 paper/official files against the *earlier* pinned manifest (this packet's
manifest hashes were re-verified), the existence of the hosted GitHub runs themselves, and the
Gitleaks 8.30.1 scan. Hosted results below are quoted from the retained log files with the
documented run/job identity, not re-fetched.

## 2. Answers to the six mandatory questions

### Q1 — Production Relin2/Mult2 vs official ordinary Relinearize and BV/CRT decomposition

**Finding: the wrapper faithfully implements the paper's Definitions 4.3/4.5/4.7 (with the
corrected Theorem 4.8 scale); no coefficient/basis/scale/state mismatch survives.**

Trace (all line numbers from this packet):

- `Mult2` is literally `RS2(Relin2(Tensor2(left, right)))` — `project/src/double_ckks.cpp:997-999`;
  the e2e test additionally asserts the staged composition equals the composed call by deep
  ciphertext equality (`project/tests/mult2_e2e_oracle_test.cpp:1152-1161`).
- `Tensor2` (`double_ckks.cpp:645-694`) computes `EvalMultNoRelin(highL, highR)` and the two
  cross terms via `EvalAdd`, dropping low-low — matching Lemma 4.2's
  `RCB(CT1⊗2CT2) = q_div·(ĉt1⊗ĉt2) + (ĉt1⊗ĉkt2 + ĉkt1⊗ĉt2)` (paper txt 634-638). The
  low-low omission is proven nonzero-and-absent by an explicit witness in
  `project/tests/tensor2_test.cpp:239,511-528` (17·(−19) = −323 at coefficient 5).
- `Relin2` (`double_ckks.cpp:807-852`) multiplies each tower by `q_div`, appends a zero
  `q_div` tower, calls the **official** `context_->Relinearize` on the full 8-tower basis,
  separately `Relinearize`s the low on the 7-tower prefix, then re-decomposes the high with
  the same `DecomposeValidatedCiphertext` used by DCP and sums the remainder into the low.
  This is exactly paper Definition 4.3 (`PAPER-2023-1788.txt:664-672`).
- `RS2` (`double_ckks.cpp:873-995`) performs the two **independent rounded** rescales
  `Rescale(high)` and `Rescale(RCB(pair))`, then `low = RS(recombined) − q_div·RS(high)`
  component-wise — exactly Definition 4.5 (paper txt 785-807), with the two-call rounding
  requirement stated in a code comment at `double_ckks.cpp:939`.
- Backend path: `cryptocontext.h:2021-2031` → `base-leveledshe.cpp:327-340`
  (`RelinearizeInPlace`: `KeySwitchCore` on `cv[2]`, add to `cv[0]/cv[1]`, resize to 2) →
  `keyswitch-bv.cpp:227-278` (`EvalFastKeySwitchCore` drops keys by `diffQl` to the input's
  basis) with digits from `dcrtpoly-impl.h:230-250` (`CRTDecompose`).
- DCP's high part uses `DropLastElementAndScale` (`double_ckks.cpp:364-404`), i.e. the
  official approximate divide-by-last-tower, with an **exact** integer reconstruction
  `low = sourcePrefix − q_div·high` in the truncated basis, so `q_div·high + low = source`
  exactly mod Q_trunc.
- The state machine (lifecycle, level, ordered-basis prefix, noise-scale degree, recorded and
  logical scales, key tag) is revalidated at every seam (`ValidatePair`/`ValidateCiphertext`,
  `double_ckks.cpp:280-643`); the e2e suite re-checks all of it plus the official
  FIXEDMANUAL recorded-scale factors (`mult2_e2e_oracle_test.cpp:713-777`).

**Wrapper arithmetic vs backend proof applicability — the decisive distinction.** The
wrapper's ring arithmetic is exact (identity-checked, see Q2). What fails is the *paper's
Lemma 4.4 near-additivity model applied to OpenFHE BV*: `CRTDecompose(0)`
(`dcrtpoly-impl.h:237-250`) builds each digit by re-emitting tower *i*'s **centered**
representatives into every other tower via `SwitchModulus`. Centering is nonlinear across
modular carries, so `D(x) + D(y) − D(x+y) ≠ 0`; the per-coefficient carry offsets (±q_i)
multiply the Gaussian key errors `e_i`, giving a deviation of order
`Σ_i q_i·|ε_i|·‖e_i·ns‖` — far above the paper's `(0,e), ‖e‖∞ ≤ 1` and its `h` impact. I
derived this independently from the official sources before reading Pro's diagnosis §5, which
reaches the same mechanism (`DIAGNOSIS.md:152-190`). This explains the observed split —
HYBRID (rounding-based `/P`-style decomposition, residuals 9–19 vs h ≈ 38–50) versus BV
(full-tower digits, residuals 2.6e11–4.1e11 vs h 41–50) — and why exactly the two BV cases
failed while all 42 non-BV cases passed in every red/diagnostic/probe run.

### Q2 — Independence of the certificate's oracles; circularity

The certificate (`CheckRelin2PathIdentity`, `mult2_e2e_oracle_test.cpp:397-477`) is **not
circular in a material way**, with one intentional, disclosed reuse:

- **Independent:** BigInt textbook CRT reconstruction with centered lifting
  (`ReconstructCentered`, lines 121-133); direct secret-polynomial evaluation per tower with
  exact negacyclic schoolbook products (`IndependentDecrypt`, lines 222-278); the exact
  integer product of input plaintexts (`NegacyclicProductInteger`, lines 913-914); measured
  Hamming weight with ternary-support assertion (`SecretHammingWeight`, lines 487-509); all
  wrap/centering arithmetic. None of this calls OpenFHE decryption or production helpers.
- **Intentional reuse:** the public `context_->Relinearize` primitive on two separately
  constructed inputs (raised high alone; low alone). This is reuse of the backend *primitive*
  to measure actual backend error, not duplication of the system under test, and not
  derivation of the bound from the accepted value — the claim matrix says exactly this
  (`CLAIM-TO-TEST-MATRIX.md:26`).
- **Why the identity holds:** BV and HYBRID key switches are deterministic given the eval key
  (fresh randomness exists only in key generation — `keyswitch-bv.cpp:49-103`,
  `keyswitch-hybrid.cpp:92-161`; none in `KeySwitchCore`/`EvalFastKeySwitchCore`). For
  deterministic `Relin`, `RCB(Relin2(T))·(1,s) ≡ Rel_{Q_l·q_div}(q_div·T_hat)|_{Q_l}·(1,s) +
  Rel_{Q_l}(T_low)·(1,s)` is an exact congruence — the production DCP/RCB bookkeeping
  contributes nothing (the decomposition reconstructs `X_trunc` exactly). The test verifies
  this coefficientwise as centered representatives (equal congruence class + both centered ⇒
  equal), so the check is exact, not approximate.
- **Concrete discriminating witness:** any wrapper defect (wrong DCP reconstruction, wrong
  tower handling, metadata/scale mismatch changing values) breaks
  `Check(predictedPairError == actualPairError, …)` (lines 446-465) — the probe phase would
  have failed there before the fatal original check; it did not, on either host. The absolute
  anchor is the final comparison against the exact integer input product (Q3), which does not
  pass through the backend at all beyond the decrypt oracle.
- **Residual shared trust:** the OpenFHE secret-key object and the eval-key generation; a
  systematically wrong *backend* consistent across all calls would still satisfy the identity
  but would have to also land within the tight final bound vs the exact product — which is
  what the green runs show it does (coefficient errors ~1e-10 of the bound).

### Q3 — Mathematical validity of the triangle bound, non-wrap, and final coefficient bound

I re-derived the whole chain independently; **the gate is mathematically valid as an
execution-specific (conditional) statement.**

- **Triangle bound:** `pairPathError = max_i |Center(highErr_i + lowErr_i, Q_l)| ≤
  max_i(|highErr_i| + |lowErr_i|) ≤ highPathError + lowPathError` — valid because both sides
  are centered representatives of congruent classes; aliasing could only occur if
  `|highErr_i| + |lowErr_i| ≥ Q_l/2`, which the non-wrap regime excludes (and any aliasing
  would break the bound loudly, not validate a wrong value).
- **Non-wrap witness** (lines 900-904): checks `2(N·env² + T) < Q_l` with
  `env = M_high·q_div + M_low` measured from this execution's decrypts and `T` the triangle
  bound. Needed integer identities: `|tensor recombined| = |(m1m2 − m̌1m̌2)/q_div| ≤
  2N·env²/q_div < Q_l/2` (implied since q_div ≥ 2) and `|tensor + pairErr| ≤
  2N·env²/q_div + T < Q_l/2` (implied by the witness). Formally sufficient. An edge wrap at
  the final `Q_{l−1}` boundary (possible only within `(h+1)/2` of the boundary) would make
  the final bound check fail by ~Q_l — again a loud failure, not a false pass.
- **Final coefficient bound** (lines 918-939): asserts
  `2·max_i |out_i·q_div·q_l − (m1·m2)_i| ≤ 2N·M̌² + 2·q_div·T + q_div·q_l·(h+1)`, i.e.
  `err ≤ N·M̌²/(q_div·q_l) + T/q_l + (h+1)/2`. My derivation reproduces exactly this:
  `q_div·q_l·out − m1m2 = −m̌1m̌2 + q_div·pairErr + q_div·ρ` with `|ρ| ≤ q_l(h+1)/2` from
  the two per-component rescale roundings (`‖r_i‖∞ ≤ q_l/2`, `‖r_0 + r_1·s‖∞ ≤
  q_l(h+1)/2` for the verified ternary secret with Hamming weight h). The RS2 low-part
  `q_div`-amplified rounding cancels exactly in recombination because
  `RCB(RS2(pair)) = RS(RCB(pair))` as ring elements — also directly identity-tested in
  `project/tests/rs2_test.cpp:600`.
- **Denominators/roles:** `q_div` (last full-basis tower, odd, distinct from q_l) divides at
  Tensor2/DCP; `q_l` (last active tower) divides at RS2; the corrected Theorem 4.8 product
  normalization is `1/(q_div·q_l)` — used consistently in the bound and in the decoded-slot
  ratio `2^(2p)/(q_div·q_l)` (`mult2_e2e_oracle_test.cpp:984-1000`, `mult2_test.cpp:130-135`).
- **Coefficient vs canonical norms:** all certificate bounds are coefficient ∞-norms,
  matching Lemmas 4.4/4.6 and Theorem 4.8. The canonical-embedding machinery of Theorem 4.9
  (low-part growth) is **not** exercised — an explicit gap for repeated multiplication.

**Theorem 4.8 missing `1/q_div` (verified inference, not an author-confirmed erratum).**
I re-verified three ways: (a) extracted the PDF's page-8 content stream — the display's
subtraction term carries only the `q_l` glyph, no `q_div`, while `q_div` glyphs appear
elsewhere in the same statement; (b) re-derived the bound from the proof chain — the proof's
own tensor bound has the `/q_div` denominator, so the printed main term `1/q_l·m1m2` is
inconsistent with the proof's error term unless corrected to `1/(q_div·q_l)`; (c) re-checked
the exact witness of `MULT2_SCALE_ALGEBRA_CHECK.md` by hand: RCB(Tensor2)=3757=48841/13,
RS2 output (17,0), final recombined 221 = 48841/221 exactly, while the printed form gives
2873 with error 2652 ≫ the stated 18/17; the magnitude condition
2(2·221²+1)=195366 < 208913 holds. The project's implementation and tests consistently use
the corrected normalization. Classification: paper-side discrepancy, P2, pending author
reconciliation; it does not indicate a code defect here.

### Q4 — Honesty/usefulness of the corrected gate; the retained theorem obligation

The gate is honest and useful as conditional arithmetic evidence:

- Every green line prints `execution_certificate=PER_PATH_CONDITIONAL`,
  `conservative_E_Relin_available=false`, `universal_theorem_gate=UNPROVED`
  (`mult2_e2e_oracle_test.cpp:1083-1085`), and the BV cases still print
  `paper_additivity_execution_observed=false` (residuals ~3.1e11–4.1e11 vs h 46–48 in the
  retained green logs) — the proof gap is displayed, not hidden.
- It does **not** lower the universal obligation: the code comment
  (`mult2_e2e_oracle_test.cpp:891-897`) and patch 0002 state that a single combined
  Relinearize measurement is not a universal E_Relin and that the certificate is conditional
  on this exact key/ciphertexts/basis/execution.
- It is not a manufactured green: the original invalid inequality is retained in history
  (red and probe logs), the coefficientwise identity is mandatory, and the old red is
  preserved as evidence (`0000-RED-BASELINE.md`, `MULT2_BV_PROBE_INTEGRATION.md`).

**Smallest testable next step toward the paper goal (recommendation):** derive a
backend/domain-specific conservative bound for one BV `digitSize=0` key switch on the two
actual domains (8-tower raised high; 7-tower low/combined), e.g.
`E_KS ≤ ns·Σ_i ‖e_i‖∞·⌈q_i/2⌉·N` from the retained evaluation-key error polynomials
(measurable offline with the secret key, no runtime cost), and instantiate Theorem 4.8's
Relin term as `E_KS_high + E_KS_low` instead of the combined single-call sample. That
converts the per-path conditional certificate into an a priori gate without lowering the
scheme to ordinary CKKS and without a redesign. Discriminating test: assert
`E_KS_high + E_KS_low + h ≥ measured pair error` across many independently generated keys
and fixtures (both hosts). Follow-up slice after that: the explicit refresh boundary
(re-decomposition after modulus reduction) to unblock the second multiplication, with
Theorem 4.9-style low-part growth tracking.

### Q5 — Test coverage, narrowed to actual snapshots

Covered (per retained executions and code inspection): all four e2e cases (BV/HYBRID ×
REAL/COMPLEX) with per-tower, per-component BigInt oracle decryption; exact coefficientwise
Relin2 path identity; triangle, non-wrap and final coefficient bound; decoded-slot recorded
bias and logical error against the frozen 1e-3 tolerance; composition equality; output
state/metadata; input immutability (identity + deep value of enumerated fields); Hamming
weight with ternary assertion; second-multiplication rejection with exact message, operand
immutability and deep key-cache preservation (`relin2_test.cpp:4560+`,
`TestTensor2RequiresFirstLifecycle`); RS2 exact `RCB(RS2)=RS(RCB))` identity
(`rs2_test.cpp:600`); low-low omission witness (`tensor2_test.cpp:239,511-528`); DCP/RCB
oracle slice; 44 registered CTest cases (`CMakeLists.txt:103-148`) — matching the hosted
44/44; API-signature `static_assert`s for all seams; BV key-shape validation for zero and
nonzero digit sizes and HYBRID partition shapes (31 relin2_* registrations).

Limits (do not broaden): one parameter set (N=64, p=30, first 35, depth 7, FIXEDMANUAL,
UNIFORM_TERNARY, HEStd_NotSet functional-only); fixed host vectors of length 8 (batch 16);
no deterministic key seed — each run is a fresh random key, so results are observed samples
(8 independent BV fixtures across the red/diagnostic/probe generations, 4 more in green);
snapshot equality covers enumerated fields/metadata and the process-wide eval-key cache in
the focused tests, not all hidden library state; no second multiplication, no Add/Sub
(separate worktree), no cross-branch RS2 fixes, no canonical-embedding (Theorem 4.9) checks;
decoded slot errors ~1e-9–1e-8 on binary64 hosts are functional evidence only, not
precision.

### Q6 — Reconciling red / probe-red / conditional green

All three states verified against the retained logs; the historical contract was never
altered:

| Stage | Source / run | Hosted result (quoted, not re-executed) |
|---|---|---|
| Original red | matrix `3087ff81…` run 33839781546; diagnostic `bda8791…` run 33840176712 | 42/44 both hosts; only tests 43/44 (`mult2_e2e_bv_real`/`bv_complex`) fail with `pair relinearization error exceeded empirical E_Relin + h` (verified verbatim in `matrix-red-linux.txt:439-459`, `bv-diagnostic-linux.txt:410-451`) |
| Probe red | `2936b5b5…` run 33844736013 (Linux job 100934063985, Windows 100934063875) | 42/44 both hosts, same fatal message; all new coefficientwise path identities passed in all 4 cases (log fields verified) |
| Conditional green | `9bf86cb…` run 33846077283 (Linux job 100938151001, Windows 100938151165) | 44/44 both hosts; `100% tests passed, 0 tests failed out of 44` verified in `mult2-bv-execution-certificate/linux.txt:444` and `windows.txt:471` |

- Patch 0001 (probe) only *adds* the identity/triangle measurement and prints; the fatal
  check is untouched — confirmed in the patch body and in the probe logs' identical failure
  message.
- Patch 0002 changes exactly three acceptance sites (triangle bound, non-wrap term, final
  bound term) plus print labels in `tests/mult2_e2e_oracle_test.cpp` only; no production
  code, CMake, registration, case vectors, BV/HYBRID choice, N64/p30 parameters, or the
  frozen 1e-3 tolerance — verified hunk by hunk. The integration's only extra change is the
  print-label rename `[DEBUG-mult2-bv-bound]` → `[RELIN2-EXECUTION]`
  (`mult2_e2e_oracle_test.cpp:878`).
- No inaccurate report wording found in the packet's own claims; the two known prose defects
  are in the *retained historical* Pro documents and are already flagged (below). The
  44/44 conditional functional CI is nowhere relabeled as goal acceptance; coordination docs
  repeatedly state it is not precision, not a universal theorem, not project completion.
- Fable 5.1: no successful review exists (403 before inference) — consistently and correctly
  not counted anywhere in the packet.

## 3. Findings

No production defect found. No REQUEST_CHANGES-level finding. Gaps and minor items:

| ID | Severity | Finding | Evidence | Disposition |
|---|---|---|---|---|
| F-1 | P2 (paper-side) | Theorem 4.8 as printed lacks the `1/q_div` in the comparison term; strong algebraic inference (PDF stream glyphs, proof-internal inconsistency, exact witness re-verified here) but **not** an author-confirmed erratum | `PAPER-2023-1788.txt:903-945`; `MULT2_SCALE_ALGEBRA_CHECK.md`; this review §Q3 | Keep documented as inference; pursue author reconciliation; implementation already uses the corrected form |
| F-2 | P2 (obligation, not defect) | Universal gate remains unproven: no conservative backend/domain E_Relin for OpenFHE BV digitSize=0; Lemma 4.4's near-additivity model does not transfer to BV full-tower-digit decomposition | `dcrtpoly-impl.h:237-250`; `DIAGNOSIS.md:152-190`; green logs `universal_theorem_gate=UNPROVED` | Open obligation; smallest next step in §Q4 (E_KS from key error polynomials) |
| F-3 | P3 (doc, historical) | Pro DIAGNOSIS §3 calls the old norm inequality *equivalent* to the residual bound; it is sufficient, not necessary. Codex's objection is mathematically correct; the reverse-triangle **lower bound for the observed failures remains valid** (no wrap at these magnitudes) | `DIAGNOSIS.md:99-124`; `MULT2_BV_PROBE_INTEGRATION.md` §"Codex review and corrections" | Already corrected in the coordination record; original retained verbatim — no action |
| F-4 | P3 (doc) | TEST-PROTOCOL Phase B names `ordinary_combined_relin_execution_error=` while patch 0001 prints `empirical_E_Relin` | `TEST-PROTOCOL.md:139` vs patch 0001 hunk | Documentation mismatch only; historical artifact preserved — no action |
| F-5 | P3 (evidence provenance) | Items not independently re-checkable in this seat: Git byte-match to `4e6cce5…`, hosted run existence, Gitleaks scan, earlier pinned-manifest match | §1 of this review | Recorded as claimed/pending, not as verified |
| F-6 | P4 (informational) | Final-bound derivation can tolerate an edge wrap at `Q_{l−1}` only within `(h+1)/2` of the boundary; such a wrap would fail the bound loudly (error ~Q_l) rather than validate wrongly — no silent-pass path found, but no dedicated diagnostic names this case | `mult2_e2e_oracle_test.cpp:918-939` | Optional: a one-line wrap diagnostic if the fixture ever approaches the boundary |

**Explicitly NOT found:** no source mutation by tests (immutability checks pass and are
retained); no tolerance or parameter tampering across red→green (constants identical in all
retained revisions); no blanket exception handling in production (single fail-fast
`Invalid()` path); no added public API beyond the seven seams; no scope expansion into
Add/Sub, second multiplication, or cross-branch material.

## 4. Remaining proof/precision limits (even though no production defect was found)

1. **Universal theorem obligation open:** Theorem 4.8's E_Relin premise has no conservative
   backend-specific instantiation for BV (and HYBRID is only empirically within `h` on
   sampled executions). The green gate is per-execution conditional.
2. **Paper erratum unconfirmed:** the missing-`1/q_div` reading of Theorem 4.8 remains an
   inference pending author reconciliation (F-1).
3. **Precision not demonstrated:** 1e-9–1e-8 logical slot errors on fixed unit-L1-envelope
   binary64 vectors under a frozen 1e-3 functional tolerance; no precision-bit claim, no
   high-precision goal acceptance.
4. **Repeated multiplication not supported:** second Mult2 is rejected at RefreshRequired
   (intentional interim limitation); Theorem 4.9 low-part growth (canonical embedding) is
   unexercised; no refresh boundary exists yet.
5. **Coverage limits:** single parameter set; fixed vectors; fresh random keys per run (no
   seed/distribution claim); enumerated-field snapshots only; N=64 functional fixture with
   HEStd_NotSet.
6. **Cross-branch isolation:** later RS2 validation fixes on a separate branch are not
   included; no cross-branch combined acceptance is claimed or implied by this packet.

## 5. Files inspected

Full read: `TASK.md`, `SOURCE-MANIFEST.json`, `project/include/openfhe_2023_1788/double_ckks.h`,
`project/src/double_ckks.cpp`, `project/tests/mult2_e2e_oracle_test.cpp`,
`project/tests/mult2_test.cpp`, both Pro patches, `0000-RED-BASELINE.md`,
`MULT2_BV_DIAGNOSIS.md`, `MULT2_BV_PROBE_INTEGRATION.md`, `MULT2_SCALE_ALGEBRA_CHECK.md`,
`INDEPENDENT_ORACLE_PLAN.md`, `TEST_SEAMS.md`, `DIAGNOSIS.md` (Pro, targeted sections),
`TEST-PROTOCOL.md` (targeted), `EXECUTION-LEDGER.md`, `0000-RED-BASELINE.md`,
`CLAIM-TO-TEST-MATRIX.md` (targeted). Paper: `PAPER-2023-1788.txt` lines 278-1010 (sanitized
extraction) plus direct PDF content-stream extraction of the Theorem 4.8 region; exact
witness arithmetic re-computed. Official references (targeted, line-cited):
`keyswitch-bv.cpp`, `base-leveledshe.cpp`, `cryptocontext.h`, `dcrtpoly-impl.h`
(`CRTDecompose`, drop/scale primitives), `keyswitch-hybrid.cpp` (randomness audit).
Project tests (targeted, line-cited): `relin2_test.cpp` (lifecycle rejection, key-cache),
`rs2_test.cpp` (identity), `tensor2_test.cpp` (low-low witness), the four API contract
tests, `CMakeLists.txt` (44 registrations), `.github/workflows/dcp-rcb.yml`. Logs: all four
green/probe/red Linux+Windows sections' certificate rows and totals, plus
`matrix-red-linux.txt` failure rows. All 59 files hash-verified twice (pre/post audit).

## 6. Execution statement

- **NOT EXECUTED by this reviewer:** OpenFHE or project compilation, any runtime/CTest/CI
  execution, cryptographic operations, dependency installation, hosted CI access. All
  execution results above are quoted from the retained packet logs with their documented
  commit/run/job identity (`9bf86cb…`/run 33846077283; `2936b5b5…`/run 33844736013;
  `bda8791…`/run 33840176712; matrix `3087ff81…`/run 33839781546) and are evidence to
  audit, not this reviewer's executions.
- **Executed locally (static only):** hashing/verification, ZIP structural inspection, PDF
  stream text extraction, exact BigInt-style witness arithmetic (via Python integers),
  source/log/patch inspection, crude credential-pattern scan. No supplied script was run;
  no source file modified; input tree verified unchanged after review.

**Bottom line:** the integrated candidate is an honest, mathematically sound **conditional**
execution certificate of the actual public Mult2/Relin2 pipeline; the historical red was an
invalid gate (unjustified cross-execution bound), not a production defect; the universal
theorem obligation, the paper's suspected Theorem 4.8 misprint, genuine high precision, and
repeated multiplication all remain open — PASS_WITH_GAPS.
