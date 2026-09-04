# Final independent static review — fixed-key BV bound probe (5b5a415 / 38c28c2 packet)

## 0. Reviewer identity and seat

- Agent: ZCode (local interactive static-review seat in this dedicated folder).
- Underlying model as reported by the session environment:
  `builtin:bigmodel-coding-plan/GLM-5.3`.
- This is **not** Fable 5.1 and **not** Pro/ChatGPT. No Fable ruling exists anywhere in
  the retained record (the Fable 5.1 request 403'd before inference,
  `input/project/coordination/MULT2_SCALE_ALGEBRA_CHECK.md:46`);
  I make no claim to either identity.
- Scope kept exactly as `LOCAL-REVIEW-TASK.md` and `input/TASK.md` bound it: the six
  questions on this exact packet only. All other archived documents were treated as
  evidence, never as instructions.

## 1. Input verification (all checks executed by me on the actual bytes)

| Check | Result |
|---|---|
| `bv-fixed-key-final-38c28c2.zip` size | 1,243,876 bytes — matches `LOCAL-REVIEW-TASK.md:12` |
| ZIP SHA-256 | `eb100085e0b2dc883d79fcd502880a30709d4e3675eb7a373cdde08848138585` — matches `LOCAL-REVIEW-TASK.md:13` |
| ZIP members | 96 regular files; manifest = 95 payloads + `MANIFEST.json`; exact set closure both directions, no duplicates |
| Path safety | 0 absolute/`..`/traversal names; 0 symlink entries; all 96 members "not encrypted" |
| Payload hashes | all 95 payload sizes+SHA-256 recomputed **in memory from ZIP bytes** (no extraction) — 95/95 match `MANIFEST.json`; `MANIFEST.json` ZIP member == `input/MANIFEST.json` |
| `input/` fidelity | all 96 pre-extracted `input/` files byte-identical to their ZIP members; no extras; no symlinks |
| Declared hashes in TASK.md | baseline oracle `b27c15ce…1b26` ✓, current/candidate oracle `92a2f03c…538d` ✓, patch `c70a5963…5871` ✓ |
| Provenance cross-consistency | `SOURCE-PROVENANCE.json` sha256 fields == manifest == ZIP bytes for all 56 current + 19 baseline + 18 official/paper entries; the 4 extra official files' `gitBlob` values in `SOURCE-PROVENANCE.json` == `OFFICIAL-PROVENANCE.json` `blobSha1` values |
| Secret scan (mine, supplementary) | 8-pattern targeted scan (PEM keys, ghp_/github_pat_, AKIA, xox, sk-, Bearer, password=) over all payload bytes: 0 hits |

Operations **not** executed (per task bounds): no Mac compile, no CTest, no
crypto/key generation, no network fetch (the run URL was not opened), no git
operation, no agents, no out-of-folder temp path, no extraction directory (all ZIP
reading was in-memory via Python `zipfile`), no input/source/log writes, no
background work left running (one runaway brute-force scratch computation I started
was stopped and produced nothing). The only files written are
`output/REVIEW.md` and `output/MANIFEST.sha256`.

## 2. Q1 — Does the patch implement the intended fixed-key, ciphertext-uniform bound, constructed before plaintext/encryption/path errors?

**Yes.** Verified directly in `input/project/tests/mult2_e2e_oracle_test.cpp`:

1. **Construction order (frozen before observation).** `RunCase` does
   `MakeContext` → `KeyGen` → `EvalMultKeyGen` (lines 1470–1473), then for BV only:
   `CheckBvCenteredDigitLiftBoundaryProbe` and `BuildFixedKeyBvBound`
   (lines 1475–1482) — strictly before `MakeCKKSPackedPlaintext` (1484–1487) and
   `Encrypt` (1488–1489), hence before Tensor2/Relin2 and before any path error is
   measured. The comment at 1477–1478 states exactly this and the code order
   enforces it. The bound is a function of the key, secret, and basis only.

2. **Key-row residual is the pinned-source identity.** Per row
   `residuals[c][t] = b + a·s − gadget (mod q_t)` with
   `gadget = (row==tower) ? s² mod q_t : 0` (lines 366–382), CRT-reconstructed and
   centered per coefficient (lines 121–133, 384–388). This is
   `ρ_i = Center_Ql(π(b_i + a_i·s − G_i(s²)))`. I verified against the pinned
   official keygen `input/official-openfhe/keyswitch-bv.cpp:86-96` (digitSize=0
   branch): `bv[i].SetElementAtIndex(i, sOld tower i)` then
   `bv[i] -= (av[i]*sNew + DCRTPoly(dgg)*ns)` — i.e.
   `b_i + a_i·s − G_i(s²) ≡ −ns·e_i (mod key basis)`, with `G_i` the CRT
   idempotent gadget (s² residue in tower i, zero elsewhere). For relinearization
   `s_old = s²`, `s_new = s`.

3. **No second ns multiplier.** The comment at lines 401–403 is source-true: ρᵢ is
   the *measured* residual `−ns·e_i mod Q_l`, so `ns` is already inside
   (`noiseScale` is recorded/printed, line 410, never multiplied back in).
   `fixed_key_bv_noise_scale=1` in all four log records.

4. **Active modulus order / lengths / restriction / immutability.** Full key basis
   `fullRows = elementParams size` (8 = 7 active + q_div), `activeRows = fullRows−1`
   (lines 296–300); secret must be on the full basis (302–303); A/B vectors must
   have `fullRows` entries (334–336); each row is checked to be on the full basis
   (356–358), then `DropLastElements(fullRows−activeRows)` removes exactly the
   trailing q_div tower and the *ordered* basis equality
   `GetModuli(a) == activeModuli` is asserted (359–364). q_div coprimality with
   every active tower is checked via extended GCD (315–321). Key immutability:
   A/B snapshots taken at 337–338 and re-compared at 397–399; behaviorally green in
   all four records. Context/type checks (BV + digitSize=0, exactly one eval-mult
   key, correct relin subtype) at 286–336.

5. **Bound formula.** `perPathRawBound += N·floor(q_row/2)·rowNorm` (392–393) and
   `pairRawBound = 2·perPathRawBound` (396) — exactly
   `B_path = N·Σ_i floor(q_i/2)·‖ρ_i‖∞`, `B_pair = 2·B_path`.

6. **Correct quantifier discipline.** This is a *fixed-key, ciphertext-uniform*
   certificate: after the key is fixed, the bound is deterministic for every
   ciphertext third component on that basis (worst-case digit radius floor(q_i/2)),
   and it is **not** (a) a fit to the observed ciphertext error — the observed path
   errors are only later *compared against* the bound (lines 1220–1225), and the
   nontriviality checks 1216–1219 reject a bound that is merely the trivial
   `Q_l/2` cap; nor (b) a raw-Gaussian or across-key claim — the certificate prints
   `fixed_key_bv_unconditional_gaussian_key_bound=false` and
   `universal_theorem_gate=UNPROVED` (lines 1432–1433). This matches the three
   quantifier levels in `BOUND-DERIVATION.md §1/§10`.

## 3. Q2 — Centered-lift premise traced through the pinned source; boundary probe coverage

**The premise holds in the pinned source, and I derived it from the bytes myself**
(not deferred to Pro's or Codex's words):

- `DCRTPolyImpl::CRTDecompose(0)` — `input/official-openfhe/dcrtpoly-impl.h:237-251`:
  digit `i` keeps tower `i` verbatim; every other tower `k` is
  `Poly tmp(coef tower i); tmp.SwitchModulus(q_k, root_k, 0, 0)`.
- `PolyImpl::SwitchModulus` — `input/project/coordination/official-references/bv-centered-lift/poly-impl.h:399-407`:
  delegates to `m_values->SwitchModulus(modulus)`. `LazySwitchModulus` is a *different*
  method (409–417) and is **not** on this path — I confirmed no silent substitution.
- `NativeVectorT::SwitchModulus` — `mubintvecnat.cpp:109-122`:
  `halfQ = om >> 1` (= floor(om/2)); `diff = |om−nm|`;
  if `nm > om`: `v.AddEqFast((v > halfQ) ? diff : 0)`;
  else: `v.ModSubEq((v > halfQ) ? diff : 0, nm)`.
- Primitives — `ubintnat.h:325-327` `AddEqFast` is plain addition (no reduction;
  safe here because `v + diff < nm ≤ ~2^35 < 2^64`); `ubintnat.h:891-902` `ModSubEq`
  is exact `(a − b) mod m` with wrap handled.

**Semantics (my derivation).** For odd `om` and canonical `0 ≤ v < om`, the
condition `v > floor(om/2)` selects exactly the negative centered half
`v − om ∈ [−(om−1)/2, −1]`. Increasing target: `v' = (v−om) + nm` lands in
`[nm−(om−1)/2, nm−1] ⊂ [0,nm)`; non-positive half and v≤half both give `v mod nm`.
Decreasing target: `v' = (v−om+nm) mod nm` or `v mod nm`. In **every** case the
output is the `nm`-residue of the **same centered integer** `L(v) ∈
[−floor(om/2), floor(om/2)]`. Boundary behavior is *strict*: `v = floor(om/2)`
stays non-negative; `v = floor(om/2)+1` lifts to `−floor(om/2)`; `v = om−1` lifts
to `−1`; `0` stays `0`. This is exactly the Section-6 contract of
`BOUND-DERIVATION.md` (which uses `floor`, not `ceil`). The same pinned files also
implement `DropLastElementAndScale` (`dcrtpoly-impl.h:693-712`), the RS2 rescale
path, whose dropped-tower correction is likewise center-lifted — consistent premise.

**The probe exercises the boundaries as claimed**
(`mult2_e2e_oracle_test.cpp:228-281`): it writes coefficients
`[0, half, half+1, q−1]` in every active tower (247–250), decomposes with
`CRTDecompose(0)`, and checks every (source, target) tower pair against the
expected centered lift `[0, +half, −half, −1] mod q_target` (260–278). Since tower 0
is 35-bit and towers 1–6 are 30-bit, the checks cover both increasing and
decreasing modulus switches, and both same-size and mixed-size pairs, on all four
boundary values — including the exact strict boundary (half stays; half+1 lifts).
`CheckBvRelin2DigitDomains` (589–613) verifies the raised-high decomposition has
`orderedModuli.size()+1 = 8` digits, the low has 7, and the eighth (q_div-source)
digit is identically zero over every tower. `BuildFixedKeyBvBound` measures
`activeRows = 7` rows. All of these appear as executed PASS assertions in both
hosted logs (`centered_digit_boundary_probe=PASSED`,
`fixed_key_bv_raised_high_q_div_digit=ZERO`, `fixed_key_bv_active_rows=7`,
8/7 digit counts implicit in the passing checks).

**Source proof vs sampled evidence.** The centered-lift statement is now *proved
from pinned source* (all-residue, all valid odd-modulus pairs) — this closes the
old "P1 source gap". The runtime probe is a *falsifier witness* on the actual
executed binary, not the proof itself; conversely the hosted probe proves the
built artifact behaves as the source says on the fixture. What executable evidence
adds: compilation of this exact source, real key/basis shapes, real digit domains.
What it cannot add: behavior on inputs outside the fixture, or a distributional
statement over keys. Both records say this (`CODEX_BV_CENTERED_LIFT_SOURCE_NOTE.md`
§"New source closes a specific missing premise", §"Bounded arithmetic checks").

## 4. Q3 — Raised-high/full-basis restriction, prefix-low, pair algebra, and the looser-but-valid norm choice

- **Raised-high and full basis.** Production `Relin2` multiplies every active tower
  of the high third component by `q_div`, appends a **zero** `q_div` tower, and
  relinearizes at `Q⁺ = Q_l·q_div`
  (`input/project/src/double_ckks.cpp:925-947`; the `emplace_back(..., true)`
  at line 933 allocates the zero tower), then relinearizes low at `Q_l` and
  recombines via exact private-DCP decomposition (954–970). The test's reference
  oracle mirrors this (`RaiseTensorHighReference`, 536–571). Since
  `gcd(q_div, q_i)=1` (checked, and confirmed by my factorization), multiplying by
  `q_div` permutes residues towerwise, so the raised digits have the same
  worst-case radius `D_i = floor(q_i/2)` — no smaller radius is justified, and none
  is claimed. The inactive eighth digit is identically zero because its source
  tower is zero and `SwitchModulus(0) = 0` (both argued in source and verified at
  runtime, lines 604–612): **that is why the extra digit contributes zero** — it
  multiplies key row 7, contributing `0·(row 7)` to the error, so only the seven
  active rows matter and the test measures exactly those.
- **B_pair with no extra q_div factor and no +h.** The recombination identity is
  exact: the test checks per coefficient that the actual pair error equals
  `highPathError + lowPathError` (centered) exactly (`CheckRelin2PathIdentity`,
  lines 682–690 — `predictedPairError == actualPairError` is an equality Check, not
  an inequality). This holds because production's private-DCP step preserves
  `q_div·quotient + remainder ≡ relinearizedHigh (mod Q_l)` exactly and low is
  added linearly (`double_ckks.cpp:954-970`). Hence
  `|ε_pair| ≤ B_H + B_L = 2·B_path` with **no additional rounding term**: the
  paper's `+h` arises only in Lemma 4.4 (`PAPER-2023-1788.txt:735-777`), which
  compares *two separate Relin calls with one combined Relin call* and introduces
  an integral rounding residual `‖e‖ ≤ 1` then `‖s·e‖ ≤ h`. The direct per-path
  bound never makes that comparison, so `+h` would be unjustified slack, and no
  extra `q_div` factor belongs because the error is already measured on the Q_l
  side of the raise (the raise itself is exact towerwise multiplication). The
  test's comment at 1236–1238 states this correctly.
- **Looser-but-valid norm.** The candidate uses
  `B_path = N·Σ D_i·‖ρ_i‖∞`; the tighter valid bound is
  `Σ D_i·‖ρ_i‖₁` (each output coefficient of the negacyclic product `d_i·ρ_i` is a
  signed sum of N products, ≤ `‖d_i‖∞·‖ρ_i‖₁`). Since `‖ρ_i‖₁ ≤ N·‖ρ_i‖∞`, the
  candidate's expression is pointwise ≥ the tight one: **looser, not false**.
  The N factor is necessary for the ∞-form (the exhaustive toy witness in
  `evidence/static-witnesses.txt` attains 6 with N and fails with 3 without).
  `ZCODE`'s historical `ns·N·Σ ceil(q_i/2)·‖e_i‖∞` would additionally have
  double-counted `ns` (Q1 item 3) and used ceil; both were corrected in the final
  formula.
- **BV combined-call near-additivity is NOT a theorem — and is observed false.**
  In all four BV records the certificate prints
  `paper_additivity_execution_observed=false` with residuals
  2.95e11 / 3.85e11 / 3.83e11 / 5.11e11 versus `h` = 44 / 52 / 49 / 40. The single
  ordinary `Relinearize` on the recombined tensor does **not** reproduce the pair
  error within `h`. The certificate therefore never uses it: the accepting chain is
  `execution_certificate=PER_PATH_CONDITIONAL` with
  `conservative_E_Relin_available=false`, and the conservative bound uses the
  key-only `B_pair`, not any combined-call measurement. (HYBRID showed
  `true` in all four of its records — an empirical observation only, not promoted
  to a theorem.)

## 5. Q4 — Both integer-lift/non-wrap inequalities and the final comparison, recomputed independently

With `N=64`, envelope `A = E = M_high·q_div + M_low` (measured from the
independent decryption of the two input pairs — the run-time meaning of the
notes' `‖Dec(input)‖`/`M_L` input envelope), `M_L = M_low` (low-member envelope),
`h` = measured secret Hamming weight, `Q = Q_l` (215-bit product of the seven
active towers), `q_l` = last active tower, `q_div` = pair divisor = full-basis
eighth tower, output basis `Q_{l−1} = Q_l/q_l`:

1. **Pre-RS non-wrap.** Test checks `2·(N·A² + B_pair) < Q_l`
   (lines 1229–1234; the execution-specific variant uses the measured triangle at
   1125–1129). This is the *stronger* paper-style sufficient condition
   (`N·A² + B_pair < Q_l/2`); the tighter valid form would be
   `N·A²/q_div + B_pair < Q_l/2` (`BOUND-DERIVATION.md §13.2`). Stronger ⇒ still
   sufficient. Recomputed for all four records: log `nonwrap_left − N·E²` equals
   the logged triangle bound *exactly*, and `2·left < Q_l/2` holds with ~5 orders
   of magnitude margin.
2. **Conservative coefficient bound.**
   `C = 2·N·M_low² + 2·q_div·B_pair + q_div·q_l·(h+1)`, `D = 2·q_div·q_l`,
   checked as `2·coefficientErrorNumerator ≤ C` (lines 1239–1247), where the
   measured numerator is `max_k |output_recombined·q_div·q_l − exactInputProduct|`
   (1143–1150). Recomputed C for all four records — **byte-exact match** to the
   logged `fixed_key_bv_conservative_bound`, denominators equal `2·q_div·q_l`, and
   `2·coefErr ≤ C` holds; margin ratios 1.3%–2.5% (computed by me; the earlier
   ZCode review's "~1e-10" claim was false and is not repeated).
3. **Final integer lift.** `2·N·A² + C < q_div·Q_l` (lines 1249–1258) — the exact
   cross-multiplied form of "target magnitude plus conservative error below
   `Q_{l−1}/2` with common denominator `2·q_div·q_l`". Recomputed numerator
   **byte-exact** for all four records; right side `q_div·Q_l` divides exactly by
   `q_div` to a 215-bit product, matching the logged `Q_l_bits=215`.
4. **Domains/precision/circularity.** All inequality arithmetic is
   `boost::multiprecision::cpp_int` (line 23); `ConvertToInt()` moves native
   (< 2^35) residues into BigInt exactly; the only floating point (long-double
   scale-ratio checks, lines 1290–1306) is not fed into any inequality — **no
   double-to-int precision loss, no stale mixed moduli** (`q_div`, `q_l`, `Q_l`
   all read from the live pair/basis at run time, lines 1055–1059, 645–647).
   `B_path`/`B_pair` contain **no observed-error input** (Q1), so the conservative
   chain is non-circular; the measured envelope `A` is used only as the
   execution-witness input magnitude and is labeled as such. **No
   modular-triangle-as-unwrapped-history shortcut**: the centered modular
   differences are upgraded to integer errors only behind the explicit half-modulus
   non-wrap inequalities (the Q=101, 40+40→−21 counterexample is retained in
   `evidence/static-witnesses.txt` and `BOUND-DERIVATION.md §13`).
5. **Normalization.** The `1/(q_div·q_l)` normalization (ratio
   `2^(2p)/(q_div·q_l)` ≈ 1.00000023656 in all records) is an **independently
   inferred algebraic correction** supported by the exact toy witness in
   `MULT2_SCALE_ALGEBRA_CHECK.md` and the derivation algebra; the printed
   Theorem 4.8 (`PAPER-2023-1788.txt:899-945`) appears to lack the `1/q_div` in
   its comparison target, but **no author has confirmed an erratum** — every
   retained document keeps this as an inference, and so do I. The test's own
   certificate derives its normalization from the implementation algebra, not from
   the paper's print.
6. **Not claimed.** No statement that arbitrary inputs/keys are wrap-free: the
   checks are per-execution (measured A) plus key-uniform B; the labels
   `fixed_key_bv_integer_lift_nonwrap=true` and
   `fixed_key_bv_final_integer_lift` are record fields for *this* execution.

## 6. Q5 — Four retained records, 53-test closure, coverage, byte closure

- **The four retained BV records** (`dual-platform-5b5a415.json`:
  linux/windows × bv_real/bv_complex) match the hosted logs field-for-field
  (norms, bounds, observed errors — verified programmatically; zero mismatches).
  Independent recomputation from the logs: `B_pair == 2·B_path` (4/4);
  conservative numerator/denominator (4/4 byte-exact); final-lift numerator
  (4/4 byte-exact); non-wrap (4/4); `2·coefErr ≤ C` (4/4); observed
  high/low ≤ B_path and pair ≤ B_pair and pair ≤ high+low (4/4).
- **Modulus table closed independently.** I factored the 215-bit `Q_l` implied by
  `finRight/q_div`: `34359736577, 1073744257, 1073738753, 1073742721, 1073739649,
  1073742209, 1073741441` — all prime, all ≡ 1 (mod 128) (NTT-compatible), product
  exactly equals `finRight/q_div`. Solving the four `B_path` equations over tower
  permutations gives a **unique** ordering matching this factorization, and
  reproduces every logged `B_path` **exactly** from `N·Σ floor(q_i/2)·norm_i`.
  The same table appears in `BOUND-DERIVATION.md §2` and
  `evidence/static-witnesses.txt` — three independent routes agree. I also
  replicated OpenFHE's deterministic `FIXEDMANUAL` prime selection
  (`ckksrns-parametergeneration.cpp:415-446`): `FirstPrime(30,128)=1073741441`
  (= logged `q_l`) and `FirstPrime(35,128)=34359736577` (tower 0) reproduce
  exactly; `q_div = 1073741953` is the next `≡1 mod 128` prime above `q_l`,
  matching the full element-params tower list.
- **53 CTest names/bindings.** Linux log: 53/53 Passed; Windows log: 53/53
  Passed; the two name sequences are identical to each other and to the 53
  `add_test(NAME …)` entries in `input/project/CMakeLists.txt:131-187` in order.
  `100% tests passed` on both hosts. The current branch has **53** tests (no 54th).
- **Warnings-as-errors and API coverage.** `CMakeLists.txt` applies
  `/W4 /WX` (Windows) and `-Wall -Wextra -Wpedantic -Werror` (Linux) to every
  target including `mult2_e2e_oracle_test`; the workflow's "Build warning-clean
  project" plus five explicit API contract builds (Relin2, RS2, Mult2, Add, Sub)
  all succeeded on both hosts (evidence JSON steps). The full `ctest --verbose`
  runs the whole suite.
- **HYBRID and REAL/COMPLEX regressions.** `mult2_e2e_hybrid_real/complex`
  (tests #50/#51) and `bv_real/complex` (#52/#53) passed on both hosts; hybrid
  certificates correctly report `fixed_key_bv_bound_available=false` (no BV claim
  leaked into HYBRID).
- **No disabled assertion / adjusted vector / tolerance.** I recomputed the
  baseline→current diff: exactly **+363/−3 in 8 hunks**; the only removals are the
  two-line `PrintCertificate(` call and one print-continuation line, replaced by an
  expanded call — purely additive otherwise. `kLogicalDecodedAbsoluteTolerance =
  1.0e-3` and the frozen input vectors are untouched. Applying the archived Pro
  patch `0001-PROBE-fixed-key-bv-bound.patch` to the baseline file reproduces the
  current file **byte-for-byte** (replayed in memory); its git-index line carries
  the same blob pair as `SOURCE-PROVENANCE.json`; candidate file is `cmp`-identical
  to the current test (hash `92a2f03c…`).
- **Workflow diff.** `project/.github/workflows/dcp-rcb.yml` vs baseline differs by
  exactly one line: `- codex/bv-fixed-key-01` added to the push allowlist. Jobs,
  OpenFHE pin `df495ba…` with `rev-parse` verification, cache key, build/test
  commands unchanged. Linux used the pinned cache (Configure/Build steps skipped on
  cache hit), Windows configured and built the pinned source — matching the claims.
- **Input immutability.** Everything above ran read-only against `input/` (hashes
  rechecked after writing outputs, see §9). No patch-applicability scratch copy was
  needed because the replay was done in memory.

## 7. Q6 — Reconciliation of every relevant earlier finding

| # | Earlier finding (source) | Disposition now | Evidence / owner |
|---|---|---|---|
| 1 | Pro fixed-key return `c7cd790`: **P1** "all-residue centered-lift implementation absent from packet" (`returns/bv-fixed-key-pro-c7cd790/REVIEW.md:21`) | **FIXED (verified, not deferred to words).** Four pristine files at pin `df495ba…` supplied; I re-read `poly-impl.h:399-407`, `mubintvecnat.cpp:109-122`, `ubintnat.h:325-327/891-902` and re-derived the strict-`v>floor(p/2)` centered semantics myself (§3). Pro's own closure return independently agrees (`returns/bv-centered-lift-pro-a151fc6/REVIEW.md:5-9`), and the hosted probe passed the boundary falsifier on both hosts. | Closed for the pinned odd-modulus native domain |
| 2 | Pro `c7cd790`: **P0=0** | **Confirmed** — I found no P0 either | this review §8 |
| 3 | Pro `c7cd790`: **P2** no numerical across-key DGG tail statement | **DEFERRED (unchanged, correctly labeled).** The certificate prints `unconditional_gaussian_key_bound=false`; unbounded Gaussian support admits no finite probability-one bound (`BOUND-DERIVATION.md §10`). | Owner: future across-key probabilistic work with a pinned tail/δ |
| 4 | Pro `c7cd790`: **P2** no precision/full-parameter conclusion | **DEFERRED.** Out of this probe's scope; the separate HYBRID first-Mult2 precision task continues independently and does not block on this probe (`PRO_BV_CENTERED_LIFT_CLOSURE_A151FC6.md:76-77`). | Owner: precision task |
| 5 | Prior ZCode `4e6cce5` **F-1**: missing-`1/q_div` reading of printed Thm 4.8 = inference, not author-confirmed erratum | **RETAINED as inference.** All current docs keep exactly this framing (`BOUND-DERIVATION.md §12`, `MULT2_SCALE_ALGEBRA_CHECK.md`, closure review). Implementation/test use the corrected algebra; the paper text is unchanged. | Owner: author reconciliation (none attempted; no external contact) |
| 6 | Prior ZCode **F-2**: no conservative backend/domain `E_Relin`; fixed-key formula was a "research lead", `+h` unjustified | **ADDRESSED for this exact diagnostic scope — implemented, derived, source-backed, hosted green.** `B_path/B_pair` with explicit domains (7 active rows, full-basis restriction, zero eighth digit) now exist and held in 4/4 records; the pair identity is exact so no `+h` (§4). The **universal** gate stays UNPROVED by design (`universal_theorem_gate=UNPROVED`). | Universal theorem still open — owner: theory |
| 7 | Prior ZCode **F-3/F-4**: historical Pro norm-equivalence wording; old TEST-PROTOCOL field name vs patch print | **Preserved historical artifacts.** Originals retained verbatim in the returns; the current test prints `ordinary_combined_relin_execution_error=` and the current TEST-PLAN agrees; no live mismatch in this packet. | No action |
| 8 | Prior ZCode **F-5**: provenance not re-checkable in that seat | **Partially closed here.** I verified every payload hash, manifest closure, and internal provenance cross-consistency offline. The Git blob ↔ GitHub claims remain externally sourced provenance I cannot re-derive without network (out of scope) — correctly labeled claim, consistent with all internal evidence (deterministic prime tables reproduce `q_0/q_l`; log arithmetic closes exactly). | External verification remains with the repo owner |
| 9 | Prior ZCode **F-6**: optional wrap diagnostic absent | **FIXED (stronger than proposed).** The patch adds explicit conservative non-wrap, per-path/pair nontriviality, and final integer-lift Checks (lines 1216–1258), and retains the execution-specific variants. | — |
| 10 | Codex corrections to prior ZCode prose ("42 non-BV", "~1e-10 coefficient claim", "aliasing would break loudly") | **Accepted and not repeated.** My ratios: coefficient error is 1.3–2.5% of the conservative bound; wrap-vs-triangle handled by explicit inequalities (§5). | — |
| 11 | Hosted-probe record `BV_FIXED_KEY_HOSTED_PROBE_01.md`: first-observed-green claims | **Verified against retained logs/JSON** (53/53 both hosts, job IDs 101002832323/101002832196, run 33866620400 metadata, log SHAs match the manifest). First-observed-green is honest: bound/vectors/tolerance frozen pre-run, no post-run tuning (the +363/−3 diff contains no threshold change). | — |

**Separation honored:** no P0/P1 defect affects this narrow diagnostic. The
paper-scale gaps (universal theorem, >53-bit precision, repeated multiplication /
8-squaring chains, production I/O, deployment security) are future obligations and
are all still explicitly unclaimed by the artifact under review.

## 8. Findings (prioritized)

- **P0: none.**
- **P1: none.**
- **P2-1 (obligation, not a defect):** across-key/sampler-tail statement absent;
  probe correctly labels it. Witness: `BOUND-DERIVATION.md §10`,
  `fixed_key_bv_unconditional_gaussian_key_bound=false`. Remediation if ever
  desired: pin a DGG tail function and δ, derive `B ≤ ns·N·T·ΣD_i` — hosted
  red/green not required for a math doc, only for any test that gates on it.
- **P2-2 (obligation):** universal Theorem 4.8 `E_Relin` gate remains unproved;
  BV combined-call near-additivity is *observed false* in all four records.
  Witness: `paper_additivity_execution_observed=false` ×4. The certificate already
  avoids the premise; no code change warranted.
- **P2-3 (paper-side):** `1/(q_div·q_l)` normalization vs printed Theorem 4.8
  remains an unconfirmed inference. Witness: `PAPER-2023-1788.txt:899-945`,
  `MULT2_SCALE_ALGEBRA_CHECK.md`. Owner: author reconciliation.
- **P3-1 (observation, no action):** the boundary probe mutates a *copy* of the
  secret-key element (line 231) without a post-check on the original secret (the
  eval key does get an immutability check). The copy semantics make mutation of the
  original impossible via this path; benign.
- **P3-2 (observation):** `BOUND-DERIVATION.md §2` cites earlier-era `h` values
  (48/46…); the current run's measured `h` are 44/52 (Linux) and 49/40 (Windows).
  The certificate measures `h` per key at run time, so the doc's historical numbers
  are prose, not a defect.
- **P3-3 (evidence provenance):** Git blob SHAs and the hosted-run URL are
  externally sourced; I could verify internal consistency and all payload bytes,
  not the external Git/GitHub identity itself (no network permitted).

## 9. Observed / inferred / pending

- **Observed (executed evidence in the packet):** dual-platform 53/53 hosted runs
  (Linux 1.00 s, Windows 2.13 s), all four fixed-key certificates' logged values,
  boundary-probe/digit-domain/key-immutability PASS assertions, HYBRID
  regressions, warning-clean + five API builds on both hosts.
- **Inferred/derived by me (static, exact arithmetic):** manifest/ZIP closure;
  patch replay and diff stats; the centered-lift source semantics; the modulus
  table (factorization + unique permutation + deterministic-prime-table
  reproduction); every inequality recomputation in §5–§6. None of this is a new
  hosted or cryptographic execution.
- **Pending/external:** across-key tail bound; universal `E_Relin` theorem; author
  confirmation of the Theorem 4.8 normalization; precision/repeated-multiplication/
  paper-scale goals; external re-verification of Git/GitHub provenance and of the
  hosted run itself.

## 10. Verdict

**ACCEPT this exact test-only diagnostic.** The +363/−3 change to
`tests/mult2_e2e_oracle_test.cpp` at tested source `5b5a415` (snapshot `38c28c2`,
one CI allowlist line otherwise) is a faithful, purely additive implementation of
the fixed-key, ciphertext-uniform BV bound: built from the key before any
plaintext/encryption/error observation, exact in its arithmetic, correct against
the pinned OpenFHE source (keygen identity including `ns`, centered digit lift,
gadget/row/basis structure), with honest conditional labels retained everywhere.
Its four dual-platform records satisfy every inequality under my independent exact
recomputation, and the 53-test suite, names, bindings, tolerance, vectors,
production code, and warnings-as-error posture are unchanged and green on both
hosts. No defect requires a patch; no remediation is required for acceptance of
this diagnostic. Nothing here claims full paper reproduction, >53-bit accuracy,
repeated multiplication, deployment security, fresh CI execution, or final project
completion — those remain open, exactly as the artifact itself states.
