# Independent static final precision precursor audit — DCP→RCB (2023-1788 t=2)

Verdict: **PASS_WITH_GAPS** (each gap explicitly scoped in §8)
Review date: 2026-09-04. Review seat: authorized local ZCode STATIC fallback.
Scope honored: bounded Q1–Q6 review of the accepted DCP→RCB precursor only. The
ZIP's `TASK.md` was read as background data for a separate Pro assignment; no
code, test, vector or patch was drafted here. No source, input, Git, CI, remote,
settings, auth, quota or browser change was made. Nothing was compiled and no
cryptographic test was run on this Mac.

## 1. Reviewer identity

- Agent/UI: ZCode CLI (local static review seat), macOS 15.3 darwin arm64.
- Model: `builtin:bigmodel-coding-plan/GLM-5.3` (visible as GLM-5.3).
- I am not Fable, ChatGPT/Codex or any other model; no other agent was dispatched.
- All runtime/red/green results quoted below are **hosted-CI artifacts reviewed
  statically**; I did not execute, compile, fetch or re-run any of them.

## 2. Exact input verification (all observed first-hand)

- `precision-first-mult2-c9ee28d.zip` — 1,163,390 bytes;
  SHA256 `e49e3fcb897ea7b9fa0cf31bc376a9289e0bd4fe8f4a1ff70a35622bd5fe0461`
  (matches LOCAL-REVIEW-TASK.md exactly). `unzip -t`: no CRC errors; 62 entries.
- `input/` is a byte-exact extraction of the ZIP (`diff -rq` clean): 62 regular
  files = 61 `MANIFEST.json` payloads + the manifest, exactly as claimed.
- Every MANIFEST payload re-hashed independently with python3 hashlib:
  **61/61 SHA256+size OK; zero extra files on disk.**
- `SOURCE-PROVENANCE.json` is self-consistent with the manifest: 40 project
  files at `c9ee28d0370eeee1ec7a1965402ed0b5e91f425e` (branch `codex/precision-01`,
  clean), tested code `bd141806bd1e0b1dad80c7ad47bfd92fc334db55`, 16 pinned
  official OpenFHE files at `df495ba2e91739a6dc8f1de254fc5a41155ce504`, paper
  PDF/text and the prior Pro return.
- `PRIOR-PRO-PRECURSOR-RETURN.zip` — SHA256
  `601c7bfbf195d383146ad63b508797322ed22cab12590fc470a241753da6f906` (matches);
  CRC clean; 41 entries = **34 regular files + 7 directories**; its internal
  `MANIFEST.sha256` covers the other 33 files, **33/33 verify OK**. Outer 62 +
  inner 34 = **96 regular files total**, resolving the task's "96" figure.
- No manifest claim was accepted on faith: hashes, counts and cross-references
  were recomputed. Archive-safety properties (no symlink/unsafe paths) were
  spot-checked by listing; a Gitleaks re-scan was NOT repeated (no tool
  invocation permitted beyond read/verify; the supplied no-findings scan result
  is recorded as unverified-by-me).

## 3. Commands actually executed (STATIC ONLY, all inside this folder)

- `shasum -a 256` on the outer ZIP, inner ZIP, archived and current precursor files.
- `unzip -t`, `unzip -l`, `unzip -q -o … -d` (outer verified into `/tmp/review-verify`
  for a read-only `diff -rq` against `input/`; inner extracted into
  `input/prior-pro-extracted/` — the only permitted extraction, inside `input/`).
- `python3` scripts (hashlib verification of 61 payloads; CTest-name closure
  parsing of the four logs; patch-0001 RED-fixture extraction + hash).
- `diff` (archived proposal vs current contract/fixture; RED vs GREEN fixture).
- `grep`/`sed`/`head`/`wc` reads of supplied text only.
- Never run: cmake/make/compilers, OpenFHE or project builds, CTest, any
  cryptographic operation, Gitleaks, git mutations, network fetch of run URLs.

## 4. Q1 — Public DCP/RCB transport meaning and seam

**Paper grounding.** DCP/RCB are defined in the supplied paper:
`PAPER-2023-1788.txt` Definition 3.3 (≈line 437): `DCP_qdiv(ct) =
(Quo_qdiv(ct), Rem_qdiv(ct))`; RCB (≈line 445): `RCB_qdiv(ĉt,ˇct) =
qdiv·ĉt + ˇct`; and line 459: `RCB∘DCP(ct) = [ct]_Q'` over the reduced basis
(the exact identity the precursor exercises). The canonical embedding
(≈lines 214–219) evaluates at primitive roots ξ^{5^j} — the slot convention the
test's oracle uses.

**Seam.** `double_ckks.h:143,150` exposes public `DCP`/`RCB` on `DoubleCKKS`;
`TEST_SEAMS.md:9-10` records these as the agreed seams. The contract test
(`precision_dcp_rcb_contract_test.cpp`, `RunContract` lines 585–691) uses only:
context generation, `KeyGen`, `Encrypt`, the fixture, `module.DCP(input)`,
`module.RCB(pair)`, and read-only metadata accessors. No private helper is
called; the oracle adapters are test-owned, as `TEST_SEAMS.md:19` permits.

**Divisor/tower ordering.** `double_ckks.cpp:266` sets `divisor_ =
fullModuli_.back()` (oddness checked 267–269) — "DCP removes last q_div". The
pair keeps the exact ordered prefix (`firstPairModuli_`, line 270); the test
asserts `GetOrderedModuli() == full prefix` (contract 536–538) and
`GetDivisor() == fullModuli.back()` (539–540). Hosted logs record
`q_div=1125899906843009` (≈2^50, consistent with ScalingModSize 50).

**Decomposition arithmetic (verified against pinned sources).**
`DecomposeValidatedCiphertext` (double_ckks.cpp:376–416) calls the *public*
`DCRTPoly::DropLastElementAndScale` (dcrtpoly-impl.h:693–712) — the same public
primitive OpenFHE's own `ModReduceInternalInPlace` uses
(ckksrns-leveledshe.cpp:186–187) — but passes its own vectors
`q_i − q_div^{-1} mod q_i` / `q_div^{-1} mod q_i` so the per-tower result is
`([ct]_{q_i} − [ct_last]_{q_i})·q_div^{-1}`, i.e. an **exact quotient by the
dropped prime**, versus OpenFHE's rounded-rescale precomputation
(ckksrns-cryptoparameters.cpp:69–84). The low part is `prefix − q_div·high`
(397–401), so `q_div·high + low = ct` exactly over the prefix basis and `RCB`
(1119–1130, `q_div·high + low`) inverts `DCP` bit-exactly. One convention
deviation from the paper is derived in §7/F-1 (non-centered remainder); the
reconstruction identity — the property under test — is unaffected.

**Key/context/state/level/scale/input domain.** Context identity is pinned
(`CiphertextPair::GetContextIdentity() == context.get()`, contract 523);
key tags propagate from the encrypting key (534, 641); level/noise-degree/scale
are validated in production (`ValidateDcpInput` requires level 0, degree 2,
scale exactly Δ²=2^100, FIXEDMANUAL — double_ckks.cpp:355–374) and re-asserted
by the test (629–631, 554–559). RCB output is checked at the prefix basis,
level 1, degree 2, scale 2^100 (637–641). The input domain (fresh degree-2
FIXEDMANUAL ciphertext, i.e. paper §3's post-first-Tensor state) is enforced,
documented and consistent with the placeholder plaintext's metadata.

**Q1 conclusion:** the precursor genuinely exercises the public DCP→RCB
transport at the agreed seam, with correct divisor/tower ordering, key/context
binding, metadata and input domain. Evidence is source lines + consistent
hosted logs, not a badge.

## 5. Q2 — High-precision fixture audit

File: `tests/precision_dcp_rcb_fixture.cpp` (GREEN, SHA256 `4bcd633c…`).

- **Fresh multiprecision inverse special FFT.** `FftSpecialInverse`
  (fixture 140–176) is a faithful cpp_dec_float_100 port of pinned
  `DiscreteFourierTransform::FFTSpecialInv` (dftransform.cpp:209–239): identical
  decimation butterfly, identical root index `(lenq − (rotGroup[j] % lenq))·gap`,
  identical `BitReverse` + `/= size` normalization, identical root convention
  `e^{+2πij/M}` (MakeRoots 109–121 vs dftransform.cpp:63–70) and rotation group
  5^j mod M (MakeRotationGroup 98–107 vs dftransform.cpp:52–59).
- **Round ONCE at 2^100.** Coefficients are
  `RoundHalfAwayFromZero(inverse[i]·2^100)` (243–248) — a single rounding at the
  full deg-2 scale, in ~100-decimal-digit arithmetic. Contrast with ordinary
  degree-2 binary64 encoding (pinned `CKKSPackedEncoding::Encode`,
  ckkspackedencoding.cpp:115–333): values are scaled by the **base** Δ=2^50,
  `llround`-rounded **in binary64** (279–280), then exactly multiplied by 2^50
  (285, 303–309) — so recorded scale is 2^100 (line 332
  `scalingFactor = pow(scalingFactor, noiseScaleDeg)`) but information
  granularity is only 2^-50 with ~53 significant bits. The fixture's inputs
  survive to 2^-100 granularity; the standard path cannot (proven by the test's
  negative control, below). My independent bound for the fixture's deterministic
  encode-side error: ≤ 64·0.5·2^-100 = 2^-89 per slot — comfortably under the
  2^-80 gate, matching the hosted ~2^-90.7 observations.
- **Slot geometry.** Real at `gap·index`, imaginary at `gap·(index+16)`,
  gap = 64/32 = 2 (234–248) — exactly OpenFHE's `temp[i]`/`temp[i+slots]` layout
  through `FitToNativeVector` (`gap·i`, ckkspackedencoding.cpp:282–283, 516–533).
  Ring dim 64, M=128, batch 16.
- **Coefficient signs/moduli.** `PositiveMod` per tower into `[0, q_i)` residues
  (187–198), then COEFFICIENT→EVALUATION — matches OpenFHE native-vector
  conventions. A wrap guard requires `2|c| < Q` (250–256).
- **Public DCRT injection.** A pristine zero placeholder is created via public
  `MakeCKKSPackedPlaintext({{0,0}}, 2, 0)`; only `plaintext->GetElement<DCRTPoly>()`
  (public mutable access, plaintext.h:258–269) is replaced (262). `Encrypt`
  consumes the element directly — `cryptocontext.h:1255/1292`:
  `GetScheme()->Encrypt(plaintext->GetElement<Element>(), …)` — no re-encode.
- **Stale cache correctly unobserved and test-only.** `GetCKKSPackedValue()`
  returns the raw stale `value` member with no recompute
  (ckkspackedencoding.h:143–145); production `Decode` routes through binary64 and
  then **zeroes the element** (ckkspackedencoding.cpp:405). The fixture's comment
  (258–261) prohibits packed-value getters, production Decrypt, serialization and
  codec promotion; I verified by grep that none of
  `GetCKKSPackedValue/GetRealPackedValue/Decrypt/Serialize` appears in the
  contract/fixture (only the comment and the test-owned `IndependentDecrypt`).
  It is confined to `tests/` with no production include. Residual enforcement
  gap → §7/F-5.
- **Negative control (correctly labeled).** `CheckBinary64NegativeControl`
  (466–486) proves slots 0/1 collapse to the *same* binary64 value and to
  byte-identical DCRT elements under the standard encoder — a control, not a
  production regression claim.

**Q2 conclusion:** fixture claims verified against pinned sources; the stale
cache is real, unobserved in this test path, and correctly limited to test-only
use. One enforcement nit (F-5).

## 6. Q3 — Independent decryption oracle and witnesses

- **Independent cpp_int CRT/secret schoolbook decryption**
  (`IndependentDecrypt`, 218–278): per-tower negacyclic schoolbook
  `c0 + c1·s + …` (`NegacyclicProductMod` 178–201, X^N=−1 fold), centered CRT
  reconstruction with explicit extended-gcd inverses (129–141). It does **not**
  use production `Decrypt`, decode, or DCRTPoly arithmetic — only public
  read-only accessors on ciphertext/secret-key elements. No FFT round-trip,
  no metadata, no binary64 conversion anywhere in the oracle.
- **Expected values are 16 literal slots** (416–442): exact rationals
  (0.125, −0.0625, 0.2…), 30-digit decimals (exact in decimal-radix
  cpp_dec_float_100), exact 2^-k shifts, zeros, and magnitudes down to
  1.23e-28. Slot1 = slot0 + (2^-70, 2^-73): both sums are terminating decimals
  with ≤73 fractional digits (< 100-digit precision) — **provably lossless**
  source values, and the delta assertion compares against the exact
  (2^-70, 2^-73).
- **Canonical roots/slot ordering valid.** `CanonicalExponents()` (349–352) =
  5^j mod 128 for j=0..15 — I recomputed all 16 values by hand; they are the
  paper's ξ^{5^j} slots. `DirectEvaluate` is Horner at `e^{2πi·e/128}` in
  multiprecision, divided by 2^100 (324–347) — a different algorithm from the
  fixture's butterfly.
- **Witnesses are independent and discriminating.** `CheckCanonicalOracleWitnesses`
  (386–414): constant → all 1; X^32 → all +i (X^32 = ±i in Z[X]/(X^64+1); at
  ξ^{5^j} it is exactly +i since 5^j ≡ 1 mod 4 — verified analytically);
  X^2 → a hard-coded 16-entry decimal table that I verified analytically equals
  `e^{iπ·5^j/32} = (ξ^{5^j})²` at spot-checked entries j=0,1,2. The table is
  external literal data, not generated by either transform implementation.
- **Tautology/shared-bug analysis.** A shared butterfly/twiddle/bit-reversal
  error between encode and decode cannot cancel here: decode is direct Horner
  with explicit roots; expected slots are literals; the X^2 table pins the
  slot-order convention to independent data. Residual shared dependencies,
  honestly scoped: (a) boost multiprecision cos/sin/pi (~2^-332 relative,
  witness-checked), (b) OpenFHE's NTT pair used for COEFFICIENT↔EVALUATION
  format conversion on both encrypt and oracle-read sides (standard reliance,
  corroborated by the 53 functional tests), (c) production DCP/RCB arithmetic
  itself is *not* an oracle for itself — the oracle decrypts the RCB output
  independently. The expected answers are **not** replaced by production
  DCP/RCB, FFT round-trip, metadata or binary64 conversion.
- **Discrimination power.** The delta (2^-70 ≪ ULP(0.125)=2^-55) is
  sub-binary64: the RED run's 1.44e-15 ≈ 2^-49.3 failure is exactly the standard
  encoder's 2^50-grid rounding signature, and the negative control proves the
  collapse. Transport corruption (wrong tower order, wrong divisor, metadata
  drift) would produce errors ≫ 2^-80; observed GREEN worst errors ~2^-90.7
  leave ~2^10 margin above the deterministic 2^-89 floor.

**Q3 conclusion:** oracle independence and witness discrimination are genuine;
expected values are lossless; no tautology found.

## 7. Q4 — Claim limits and label audit

- **Centered headroom is not a no-wrap theorem.** The contract asserts the
  *observed* centered recovered coefficients have ≥128-bit headroom (contract
  649) and logs `approximate_headroom_bits=258` (98-bit max coefficient,
  356-bit active modulus). The coordination record explicitly limits this:
  "not by itself a proof about every possible unwrapped integer/noise history,
  universal Gaussian tails, or a later Mult2 integer lift"
  (PRECISION_RECONSTRUCTED_RETURN_AND_TDD.md 97–100). No code or log promotes it
  to a theorem.
- **Misleading-label vs defect.** The assertion text "lacks the frozen 128-bit
  **non-wrap headroom**" (contract 649) reads slightly stronger than the
  measured quantity (headroom of this trial's actual centered coefficients);
  the log field name `approximate_headroom_bits` and the doc disclaimers are
  accurate. This is a **label nit only** — the measured slot errors and the
  verdict are unaffected (→ F-3). Similarly `HEStd_NotSet` is logged as
  `functional-diagnostic-only` by the precision test — accurate; the older
  functional tests log `functional-only` (inconsistent phrasing, → F-4).
- **Four fresh keys/host are not statistical/security evidence.** Doc lines
  243–247 call the eight trials "sampled diagnostic absolute errors, not
  universal precision bounds"; TASK.md 26–30 repeats the limits. The test makes
  no statistical claim; N=64/p50/HEStd_NotSet is diagnostic-only, and the docs
  say the p50/50 experiment "is not that reproduction" of paper §6.3/Table 3.
- **No overclaim found in code/log language beyond the nits above.** Compile
  failure is correctly classified as "a test-candidate namespace error, NOT the
  intended positive precision red" (doc 150–152) — matching the retained
  compile log (four `'lbcrypto::Format' has not been declared` at contract
  lines 145/512/531/614, compile-failure-windows.txt 57–70). The RED is
  correctly attributed to the lossy input fixture, not to DCP/RCB or pristine
  encoding.

**Q4 conclusion:** recorded scope and disclaimers match what the code/logs
actually measure; two label-level nits (F-3, F-4), no defect in measured results.

## 8. Q5 — True TDD continuity

All items below verified from bytes, not from prior reviewers' prose.

1. **Frozen hash continuity.** Current files = runtime-red freeze
   (`precision-runtime-contract-e38764a.json`) = MANIFEST = SOURCE-PROVENANCE:
   contract `ad677414499c3e98e7f798ed940d587cb35c6cc791c7b0f81166ca1e6917f854`,
   fixture header `4b7b1c4f2670f5dc93e8d28f1ad585a47bb9cf4b81130bb45200a3af82e6b554`,
   CMake `ab5a9873f90f5ab7d292dca4e54684e1242102ac469e0810f71508b26e39c91b`,
   workflow `e3e1d23250b73f70747a3681975e3da870d1f337a3e2f54e674bb05a8900f951`,
   GREEN fixture `4bcd633c3fb4b6ad4fa5d2088908e7992ccd50dec0d3b826fe976e590a3aa596`.
2. **Namespace corrections.** Archived proposal contract `137612719a…` → current
   `ad6774…`: diff = **exactly four** `lbcrypto::Format::`→`Format::` changes at
   lines 145/512/531/614 — matching the retained Windows compile failure at
   fe35a099 (a compile failure, correctly NOT claimed as the runtime RED).
   Archived proposal GREEN fixture `09f1ff5c…` → current `4bcd633c…`: **exactly
   three** corrections (lines 185/197/199). Fixture header unchanged throughout.
3. **RED fixture.** Extracted from archived patch 0001; SHA256 = the frozen
   `f47a2b2f0446a97b62e77f41c1a1b36d759e07c7eb1b003e2b2b842383e23017` (after the
   patch's trailing newline). Its body is the deliberately lossy
   `convert_to<double>()` → `MakeCKKSPackedPlaintext(..., 2, 0)` path, with a
   comment stating a red diagnoses the fixture only. It contains no `Format`
   usage, consistent with compiling unchanged at e38764a.
4. **GREEN changes only the fixture.** RED→GREEN diff is confined to
   `tests/precision_dcp_rcb_fixture.cpp` (lossy body → fresh 2^100 construction);
   contract/header/CMake/workflow hashes identical across red and green states
   per the freeze record and current bytes. **No assertion, vector, witness,
   tolerance (2^-80), trial count (4), or headroom bound (128 bits) changed** —
   they all live in the unchanged contract.
5. **CTest name/command closure.** CMakeLists registers **54** `add_test` names;
   my parser found **exactly those 54 unique names** in each of the four logs
   (red/green × linux/windows), no duplicates, none missing.
6. **Trial records.** GREEN: `trial=0..3` lines on both hosts; the eight
   delta/max-slot values in the logs match
   `dual-platform-green-bd141806.json` digit-for-digit (worst delta
   5.9416098364710929682297021517222122255998e-28 and worst slot error
   5.0925606891564857369051272102149462810231e-28, both Windows trial 2 —
   ≈2^-90.6/2^-90.7). RED: fails at trial 0 on the **unchanged positive 2^-80
   delta assertion** — Linux 1.44371708012868399100616073892039201145429249341635e-15,
   Windows 1.44371708012915718138302173730655083622918985218763e-15 (matching the
   doc verbatim; the magnitude is the expected 2^50-grid binary64 signature).
   No four-trial RED is claimed anywhere — correct.
7. **Prior 53 preserved.** Red logs: 53/54 pass, sole failure the new contract;
   green logs 54/54; the earlier `all-seams-combined` logs show 53/53 at the
   pre-precision state.
8. **Warning/public-API evidence.** CMakeLists applies `/W4 /WX` or
   `-Wall -Wextra -Wpedantic -Werror` to every target including the new one
   (lines 100–137); the compile-failure log shows the full warning-as-error
   command line; both green logs link all 16 executables including the five
   explicit API targets (workflow order: build → relin2/rs2 API → CTest →
   mult2/add/sub API, dcp-rcb.yml 90–108), and the red logs correctly contain
   only the pre-CTest relin2/rs2 API builds (later steps skipped by the failing
   CTest, exactly as documented). The workflow pins `OPENFHE_COMMIT
   df495ba…` with an equality assertion (lines 21/45/180) and allowlists
   `codex/precision-01`.
9. **Prior Pro bytes unchanged.** The inner archive is intact (hash above,
   33/33 internal manifest OK) and its proposal files differ from current files
   only by the documented namespace corrections — no silent rewriting.
10. **Evidence limitations (see also §9).** Windows logs embed the run IDs
    (`project-build-33863067661-1` red, `project-build-33864080896-1` green)
    matching the claimed runs; the Linux logs contain no internal run/commit
    marker, so their binding to runs 33863067661/33864080896 and commits
    e38764a/bd141806 rests on the retained docs/evidence JSON plus fully
    consistent content, timestamps and output format (→ F-2).

**Q5 conclusion:** true TDD continuity is verified to the maximum extent
possible from static bytes; the compile failure is correctly segregated from
the runtime RED; nothing was weakened.

## 9. Q6 — Remaining issues for the first high-precision Mult2

(Stated without reviewing or drafting the unwritten candidate.)

1. **Exact rational normalization vs approximate descriptors.** Production
   `PaperScaleDescriptor`/`TensorScaleDescriptor` carry `double`/`long double`
   fields (double_ckks.h:24–34) — named "approximate", host-dependent (80-bit
   x86 long double vs 64-bit MSVC/ARM), and structurally incapable of
   expressing the first-Mult2 logical scale `2^200/(q_div·q_l)` exactly (a
   ~100-bit product denominator). The next slice's expected normalization must
   be exact integer/rational arithmetic (cpp_int), as TASK.md 98–103 requires;
   descriptors may remain wiring diagnostics only.
2. **Low-low omission + key-switch error.** Tensor2 discards `ˇct1⊗ˇct2`
   (double_ckks.cpp:777–782; paper line 129 gives the resulting error term
   ∥Δ^{-1}·Dec(ˇct1⊗ˇct2)∥∞), and Relin2 adds HYBRID key-switch error at the
   raised full basis (825–947). Neither is measured by the DCP→RCB precursor;
   the unresolved BV fixed-key premise and
   `universal_theorem_gate=UNPROVED/conservative_E_Relin` labels (visible in
   the retained log lines) remain a separate Pro review; current HYBRID work
   must not block on it (and this review does not).
3. **Remainder-convention deviation matters for tight noise budgets** (F-1):
   the implementation's low part is the non-centered remainder (coefficients in
   [0, q_div) rather than (−q_div/2, q_div/2]); the paper's Theorem 3.2-style
   bound `∥I∥∞ ≤ (h+2)/2` becomes effectively `≤ (h+2)/2 + h` under this
   convention. Harmless for the exact DCP↔RCB identity proven here; it must be
   accounted in any tight first-Mult2 no-wrap/error derivation.
4. **Lifecycle/refresh.** After RS2 the pair is `RefreshRequired` and a second
   Mult2 is rejected (Tensor2 requires `ReadyForFirstMult`, 766–769; RS2
   requires `ReadyForRS2`, 993–995). Repeated multiplication/refresh is an
   open, separate gate.
5. **Paper §6.3/Table 3 parameters remain a separate gate:** Δ≈2^100,
   Div≈2^40/Mult≈2^60 pairing, Base 50×2, N=2^15, h=128, dnum, 8 repeated
   squarings, 1000 executions, reported ≈2^-81.8 average error (paper
   lines 1566–1580). The N=64/p50/HEStd_NotSet precursor is not that
   reproduction and does not claim to be.
6. **Production lossless I/O** (a real codec replacing the test-only DCRT
   injection) and security/performance evaluation remain open.

## 10. Findings register

No P0 findings. No defect was found that affects any measured result of the
accepted precursor.

**F-1 (P2, theory fidelity / next-slice impact).** DCP's low part uses the
non-centered remainder convention. Witness: double_ckks.cpp:382–405 with
dcrtpoly-impl.h:693–712 gives `low ≡ ct_last (mod Q')` (coefficients in
[0,q_div)) rather than the paper's centered `[m]_q` (paper Def 3.1/3.3,
coefficients in (−q/2,q/2]); equivalently `(high, low) = (Quo − T, Rem + q_div·T)`
for a 0/1 polynomial T. Impact: the exact reconstruction identity
`q_div·high + low = ct` (and RCB∘DCP) is unaffected — the precursor's measured
results stand — but paper Lemma/Theorem constants (e.g. ∥I∥∞ ≤ (h+2)/2,
|Rem|∞ ≤ q_div/2) do not apply verbatim; tight first-Mult2 noise budgets must
use the implementation's convention. Smallest remediation: one paragraph in the
coordination record deriving the adjusted constants (no code change); or center
the remainder in a future slice if bit-exact paper accounting is wanted.

**F-2 (P2, evidence provenance).** Linux red/green logs carry no internal
run/commit identifier (Windows logs embed `project-build-<runid>`). Impact: the
Linux logs' binding to runs 33863067661/33864080896 and commits e38764a/bd141806
is inferred from consistent docs/evidence JSON/format, not provable from the log
bytes alone. Smallest remediation: in future runs, echo `$GITHUB_RUN_ID` and
`git rev-parse HEAD` into the retained log section.

**F-3 (P2, label).** Assertion text "lacks the frozen 128-bit non-wrap headroom"
(contract line 649) can be read as a no-wrap claim; what is measured is the
headroom of the *observed* centered recovered coefficients per trial. The log
field `approximate_headroom_bits` and the doc disclaimers (Q4) are accurate.
Impact: none on measured results — label-only. Smallest remediation: reword to
"observed centered-coefficient headroom below frozen 128-bit diagnostic minimum"
at the next permitted touch of the file (it is currently frozen; do not change
it just for this).

**F-4 (P2, label consistency).** `HEStd_NotSet(functional-diagnostic-only)`
(precision test) vs `HEStd_NotSet(functional-only)` (older functional tests,
e.g. green-linux line 502). Both convey diagnostic-only; phrasing differs.
Smallest remediation: unify the string when older tests are next legitimately
touched.

**F-5 (P2, guard).** The stale-cache prohibition is enforced by comment and by
one-time archived checks (`tools/verify_contract_continuity.py` + static grep in
the prior-pro package), but the current CI workflow runs no such guard. Impact:
a future edit could read the stale cache without CI noticing. Smallest
remediation: add a two-line CI grep step asserting the forbidden getters/Decrypt
are absent from `tests/precision_dcp_rcb_*`.

**F-6 (informational).** The historical doc says "NINE added CMake lines" for
the RED integration; the current CMake shows the precision target as 6
structural lines + 2 warning lines. Immaterial; the structural claim (only the
precision target/test added, all 53 prior registrations retained) is verified.

**F-7 (informational, input metadata).** LOCAL-REVIEW-TASK.md's "(96 regular
files total)" is correct (62 outer + 34 inner) once directory entries are
excluded; a naive entry count (41) could misread it. No action.

## 11. Observed / inferred / pending

**Observed (byte-verified here):** everything in §2; all source citations in
§4–§8; the 54-name closure in four logs; the RED/GREEN hash chain including the
patch-extracted RED fixture; the four/three namespace diffs; trial values in
logs matching the evidence JSON digit-for-digit; the compile-failure content;
warning flags per target; workflow pin/order/allowlist; analytic verification of
canonical exponents, X^32/X^2 witnesses, literal exactness, and the standard
encoder's 2^50-granularity rounding; `Encrypt`'s direct element path; stale
`value` cache behavior; `Decode`'s element-zeroing.

**Inferred (consistent, not byte-provable offline):** authenticity of the hosted
logs as GitHub Actions output of the named runs/commits (Linux legs lack
internal run IDs — F-2); the algebraic derivation of the non-centered remainder
convention (F-1) from pinned + project sources (high confidence, not executed);
that the executed binaries correspond to the frozen sources (strongly
corroborated by exact failure/diagnostic message formats and values).

**Pending (out of scope / cannot resolve statically):** first high-precision
Mult2 behavior (§9); universal no-wrap/key-switch/Gaussian bounds and the BV
theorem discussion (separate Pro review, not blocking HYBRID work); paper
Table 3 reproduction and any security claim; production lossless I/O and
refresh; Gitleaks re-verification (supplied zero-findings result accepted as
recorded, not re-run); the printed-Theorem-4.8 vs inferred 1/q_div
normalization discrepancy (Pro reconciliation ongoing per
MULT2_SCALE_ALGEBRA_CHECK.md).

## 12. Verdict

**PASS_WITH_GAPS**, each gap scoped: (1) the accepted precursor verifies the
DCP→RCB transport only — first-Mult2 precision is unmeasured by it; (2) all
runtime evidence is hosted-CI artifacts reviewed statically — nothing was
compiled or executed by this reviewer; (3) no-wrap/headroom is a per-trial
diagnostic, not a theorem, and the implementation's remainder convention
deviates from the paper's lemma constants (F-1); (4) N64/HEStd_NotSet/p50 is
diagnostic-only — paper-scale parameters and security are separate gates;
(5) the high-precision encoder is a test-only adapter with a deliberately stale
placeholder cache — not a production codec; (6) the next slice requires exact
rational normalization and must independently account low-low omission and
key-switch error. Within those scoped gaps, the precursor's claims are
source-cited, internally consistent, independently verified where statically
possible, and free of overclaiming.

— End of bounded review. Stopping here as instructed.
