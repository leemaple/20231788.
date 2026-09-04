# First Mult2 high-precision final independent review (LOCAL STATIC)

Reviewed 2026-09-04 in workspace
`/Users/lifeng/Documents/20231788-openfhe-zcode-first-mult2-final-review-20260904`.

## 1. Actual reviewer identity and environment

- Reviewer: ZCode interactive coding agent (desktop app on macOS), performing the
  authorized LOCAL STATIC fallback review. Underlying model reported by this
  environment: `builtin:bigmodel-coding-plan/GLM-5.3`.
- Host: darwin 25.3.0 arm64, zsh. NOT Windows execution. NOT Fable 5.1 — the
  required Fable5.1 terminal previously returned 403 before inference, so no
  Fable review exists; per LOCAL-REVIEW-TASK.md that is not a blocker for this
  independent ZCode review.
- Constraints honored: no local OpenFHE/project builds, no crypto runtime, no
  benchmarks, no network, no subagents, no git mutation, no dependency installs,
  no execution of supplied scripts, no /tmp writes, no brute force. All writes
  confined to `output/` (REVIEW.md, MANIFEST.sha256; transient scratch removed).
  Exact-integer/rational checks were small and bounded (Python
  `fractions`/`decimal` and `int`).

## 2. Immutable archive and input verification (observed facts)

- `precision-first-mult2-final-e482362.zip`: 1304902 bytes,
  SHA-256 `50660206ef1657d19c4ca2cd7540aa61ba301a7fe946f5734e3d3861f179f281`
  — matches LOCAL-REVIEW-TASK.md exactly.
- ZIP contains 119 members, all unique regular files, no directory entries, no
  absolute paths, no `..` segments, no backslashes — path-safe for extraction.
- `input/MANIFEST.json` (`sourceCommit e48236231f32651ae3c7a3f07ef0b7d67a7560b2`)
  lists 118 files = exactly the 119 members minus itself. Closure verified:
  nothing missing, nothing extra; every entry's `bytes` and `sha256` match the
  ZIP payload (118/118, zero mismatches).
- `input/` is byte-identical to the ZIP for all 119 files; no extra or missing
  files. Inputs are immutable as supplied.
- Gitleaks 8.3.1 claim (scan of 2848390 extracted bytes, zero findings, after
  the earlier pre-extraction scan failure) is recorded as supplied evidence;
  not re-executed by this static review (no tool runs were required or
  performed). The disposition in LOCAL-REVIEW-TASK.md (successful completed
  scan is the applicable evidence) is coherent with the recorded sequence.

## 3. Provenance and continuity (observed facts)

`input/SOURCE-PROVENANCE.json`: branch `codex/precision-01`, source commit
`e48236231f32651ae3c7a3f07ef0b7d67a7560b2`, tested source
`47907783a6141d0174da79eae264d779fc598f28`, baseline `c9ee28d...`, OpenFHE pin
`df495ba2e91739a6dc8f1de254fc5a41155ce504` (1.5.0), dirtyState = four untracked
coordination notes, excluded per `dirtyInputsIncluded`.

`diff baseline/ project/` shows exactly:

1. `.github/workflows/dcp-rcb.yml` (Codex-attributed, see below);
2. `CMakeLists.txt` — exactly 9 added lines: the 5-line
   `add_executable(precision_first_mult2_contract_test ...)` + link block, one
   MSVC `/W4 /WX` line, one GCC `-Wall -Wextra -Wpedantic -Werror` line, and the
   2-line `add_test` registering `precision_first_mult2_high_precision_contract`
   at position 3 (54 → 55 bindings; all 54 prior names/commands unchanged and in
   order);
3. new `tests/precision_first_mult2_contract_test.cpp` (58273 bytes, 1197 lines,
   SHA-256 `60a7c27c7f88420b1c55899bbe4d4b3ad54cbbf6a06da90d2fdfb0a0646770db`);
4. additive evidence trees `artifacts/`, `coordination/`.

Production `src/double_ckks.cpp` and `include/.../double_ckks.h`, the fixture
(`precision_dcp_rcb_fixture.{h,cpp}`), the old precision contract and every old
test are byte-identical to baseline (verified by diff; same git blobs in the
provenance record; fixture/old-contract SHA-256s match the values recorded in
`FIRST_MULT2_PRECISION_RETURN_AND_HOSTED.md`).

Workflow diff is purely additive on both hosts: a "Record clean-room run
provenance" step (asserts `git rev-parse HEAD == $GITHUB_SHA`; prints
PROJECT_SOURCE_COMMIT / GITHUB_RUN_ID / GITHUB_RUN_ATTEMPT) and a "Run focused
first-Mult2 precision contract" step (targeted build + `ctest -R
'^precision_first_mult2_high_precision_contract$'`). OpenFHE pin, toolchains,
cache/resource rules, `--parallel 2`, the 5 explicit API target builds
(relin2/rs2/mult2/add/sub) and warnings-as-errors are preserved. Integrated
workflow SHA-256 `4cb094e594bfeb7340e5ded3ef2f8020f56b00b44444087417fa158d659b3485`
matches the coordination record.

Pro return integrity: the patch
`0001-add-first-mult2-high-precision-contract.patch` touches only the two
claimed paths; `final-changed-files/` copies equal the live project files bytewise
(cmp + SHA-256).

## 4. Static review of the new test

### 4.1 Structure and parameters

Constants at `project/tests/precision_first_mult2_contract_test.cpp:38-51`
match the frozen contract: N=64, batch=16, depth=7, scalingModSize=50,
firstModSize=55, FIXEDMANUAL, HYBRID, digitSize=0, maxRelinSkDeg=2,
UNIFORM_TERNARY, HEStd_NotSet (diagnostic only), input scale 2^100, product
scale numerator 2^200 (= 2^100·2^100, re-derived at line 1012), acceptance
2^-80, frozen centered-headroom gate 128 bits, 4 fresh-key trials.

Two nontrivial 16-slot literal vectors (`LeftValues` 492-518, `RightValues`
520-541 — only slot 7 is a deliberate zero) pass the unit L1 envelope
(`CheckLiteralEnvelope` 608-616), are multiplied host-side in multiprecision and
compared to the frozen literal product table at 1e-75
(`CheckHostProductsAgainstFrozen` 586-598), and the frozen table itself is
witness-checked (dyadic slots 0/2, non-dyadic slot 3, zero slot 7, and the
sub-binary64 delta identity `product[1]-product[0] == (2^-71+2^-75, -3·2^-74)`,
618-638).

### 4.2 Oracle independence — could the oracle share the tested defect?

No. The decode path contains no binary64 and no production Decrypt:

- `IndependentDecrypt` (253-313): per-tower schoolbook RLWE
  `c0 + Σ c_i·s^i` with its own negacyclic modular convolution
  (192-215), own extended-GCD CRT reconstruction (108-155). It validates ring
  dimension, ordered RNS basis consistency across components and against the
  secret key before use.
- Exact input product: integer negacyclic product of the independently
  decrypted, independently recombined (`q_div·high + low`) pair plaintexts
  (1081-1090).
- Slot decode: direct multiprecision Horner at ζ^(5^j) (`DirectEvaluateRational`
  404-419) with exact rational normalization by `(q_div·q_l)/2^200`
  (417-418, 1121-1123). `cpp_dec_float_100` (~332-bit significand) is ample for
  ~101-bit integers and 2^-80-level comparisons (accumulated evaluation error
  ≪ 2^-200 here).
- Evaluator calibration: constant → 1, X^32 → i, and the ordered X^2 witness
  table discriminate normalization, phase direction, conjugation, sign and slot
  ordering (460-490). I re-derived the X^2 table at 130-digit precision: worst
  deviation from exact cos/sin(2π·2·5^j/128) ≈ 4.96e-81, within the test's
  1e-70 tolerance. (My first pass at this check used a double-precision π and
  produced ~1e-16 artifacts — my tooling error, corrected; recorded for
  transparency. The table itself is sound.)
- The only shared substrate between SUT and oracle is OpenFHE's own
  Encrypt/DCP/Tensor2/Relin2/RS2/RCB primitives — exactly what a regression
  test may share.
- Teeth against the targeted defect class: the binary64 negative control
  (662-694) demonstrates slots 0/1 of the left vector collapse to identical
  doubles and identical standard-encoded DCRT elements. Any implementation that
  lost the sub-binary64 delta (the defect class this regression guards) would
  produce a slot-1 vs slot-0 delta error ≈ |(2^-71+2^-75, -3·2^-74)| ≈ 2^-70.8,
  roughly 2^9 above the frozen 2^-80 gate, and would fail loudly
  (1132-1139 print the observed error).

### 4.3 Coefficient/scale/divisor algebra — verified from production source

From `project/src/double_ckks.cpp` (unchanged production code):

- DCP (377-417): `high` is the per-tower quotient via
  `DropLastElementAndScale` (OpenFHE rescale-style rounded division by the
  dropped final tower `q_div`), `low = sourcePrefix − q_div·high`; hence
  `RCB(pair) = q_div·high + low ≡ original ciphertext mod Q_level1`, which the
  test's `Recombine` (315-326) mirrors exactly.
- Tensor2 (763-812): `high3 = h_a·h_b`, `low3 = h_a·l_b + l_a·h_b`, so
  `RCB(tensor) = (L·R − l_a⊙l_b)/q_div` where L, R are the recombined
  decrypted inputs. The dropped `l_a⊙l_b` term is the scheme's intrinsic
  first-mult truncation (|l| ≲ q_div/2 per coefficient), and it is included in
  the measured end-to-end error — the test measures real precision, not
  nominal metadata precision.
- RS2 (991-1112): two independently rounded `Rescale` calls (paper Def. 4.5),
  `newLow = rescaledRecombined − q_div·rescaledHigh`, so
  `RCB(output) = Rescale_{q_l}(RCB(relin tensor))` and the output logical scale
  is exactly `2^200/(q_div·q_l)`. `Mult2 = RS2(Relin2(Tensor2))` (1115-1117).
- Therefore the test's error metrics are the right ones:
  `|O·(q_div·q_l) − L⊙R|` per coefficient (944-957, 1141-1144) and slot error
  `eval(O)·(q_div·q_l)/2^200` vs the frozen products (1121-1139). Double and
  long-double recorded scales are asserted only as state/metadata
  (CheckResultState 809-847), never as the oracle — matching the frozen claim.
- `MULT2_SCALE_ALGEBRA_CHECK.md`'s small exact witness (q_div=13, q_l=17,
  recombine 221, tensor high 289) re-verified by this review: output
  recombines to 221 = 48841/221 exactly, error 0, while a 1/q_l-only
  normalization gives error 2652. The implemented `q_div·q_l` normalization is
  correct for this implementation; the printed paper Theorem 4.8 comparison
  remains a pending paper-side reconciliation (owner Pro), not a code defect.

### 4.4 Stage/state/level/basis checks and mutation boundaries

`CheckCiphertextState` (696-726) and the per-stage checkers (728-847) pin
context identity, encoding, level (inputs 0 → pairs 1 → output 2), noise-scale
degree (2 → tensor/relin 3 → output 2), ordered RNS basis prefixes, divisor =
removed final tower, key tag, slots, EVALUATION format, component counts
(2/3/2), the exact one-tower drop at the output (825-830), and
`Q_out = Q_level1 / q_l` exact divisibility (1096). Headroom gates:
`max|exact input product| << 128 < Q_level1/2` (1091-1093) and the same for
the output (1102-1104) — these also guard the oracle's own no-wrap validity.
Snapshots of both encrypted inputs, both DCP pairs, and the result (before RCB)
are verified unchanged after DCP/Tensor2/Relin2/RS2/Mult2/RCB
(1033-1047, 1068-1073, 1106-1119). Staged vs direct equality (1065-1067) is
wiring only and is labeled as such; the precision verdict rests on the exact
oracle, not on that equality. RCB's public output is cross-checked against the
independent `q_div·high + low` recombination (1112-1118).

### 4.5 Exception handling and RNG

`main` (1187-1197) translates only `TestFailure` into `=FAIL` + exit 1; there
is no masking try/catch anywhere in the test or fixture — unexpected exceptions
surface uncaught (nonzero exit, CTest failure). No RNG seeding; 4 fresh
`KeyGen`/`EvalMultKeyGen` trials per invocation (1015-1019), unseeded.

### 4.6 Test-only fixture boundary (ownership)

The reused fixture (`precision_dcp_rcb_fixture.cpp`, byte-identical to
baseline) builds the exact 2^100 DCRT element via a multiprecision special
inverse FFT with round-half-away-from-zero per coefficient (fixture 140-201,
238-263); the plaintext's binary64 packed-value cache remains the zero
placeholder, with the ownership comment at fixture 258-261 forbidding
packed-value getters, production Decrypt, serialization or shipping-codec use.
The new test consumes only the DCRT element plus `Encrypt`. The encoder/decoder
pair is exactly inverse (the fixture's rotation-group inverse FFT over ω with
even-coefficient packing vs the test's Horner at ζ^(5^j); the bridge to the
standard encoder is the DcrtEqual negative control). Prior reviewer F1
(unsigned remainder) was disproved against pinned OpenFHE source and exact
arithmetic; I read `ZCODE_PRECISION_PRECURSOR_DISPOSITION.md` and did not
repeat the resolved claim — my own source-level derivation above (centered
remainder, |low| ≲ q_div/2) is consistent with that disposition.

## 5. Runtime evidence audit (actual Linux/Windows logs)

Verified against the raw logs and the authoritative job JSONs:

- Linux job 101023587797 (run 33873114880, attempt 1, head_sha 4790778...,
  branch codex/precision-01): log 65962 bytes, SHA-256
  `300e7d3075164fb18191dacabb67a9d96ded9a0c0b44835a5cc9daec3aac3de6` — matches
  `first-observed-linux-4790778.json`; completed 2026-09-04T12:33:05Z =
  20:33:05 CST.
- Windows job 101023588186: log 69843 bytes, SHA-256
  `4b1cdda58b2800bcb54b9653cb4b3bcb20c336d7b80787cf09120ee94c6a98f2` — matches
  its JSON; completed 12:39:26Z = 20:39:26 CST.
- Both logs carry in-band markers
  `PROJECT_SOURCE_COMMIT=47907783a6141d0174da79eae264d779fc598f28
  GITHUB_RUN_ID=33873114880 GITHUB_RUN_ATTEMPT=1`, matching the job JSON
  `head_sha`/`run_id`/`run_attempt` (the prospective fix for the precursor's
  F-2 log-binding gap).
- Linux: focused 1/1 (case 0.17 s, total 0.28 s); full 55/55, total 1.03 s.
  Windows: focused 1/1 (0.25 s / 0.25 s); full 55/55, total 2.30 s. All match
  the TASK.md claims.
- Default warnings-as-error builds are clean (zero warnings in both logs); the
  5 explicit API target builds and the focused target build all ran on both
  hosts.
- All 55 CTest names in each full log, in order, are identical to the 55
  `add_test` names in `project/CMakeLists.txt` (verified programmatically);
  the 55 `actualCases` in both job JSONs also match that order, with the new
  test at position 3.
- Trials: exactly 8 first-mult2 diagnostic lines per host (4 focused + 4 full),
  16 total. (Four similar-looking `max_slot_error` lines per log belong to the
  pre-existing precision_dcp_rcb test and are not first-mult2 samples.)
- Observed errors, all ≤ 2^-80 = 8.2718...e-25:
  - Linux worst max-slot 1.6696072195146116607129673424340031160212e-27,
    worst delta 1.6958307879080880932103073456218202834178e-27;
  - Windows worst max-slot 1.1983491464553656560716630483927336252837e-27,
    worst delta 4.5596134479312013030922868162011951012964e-28.
  The overall maxima claimed in TASK.md match the logs exactly (both come from
  Linux). Margin to the gate ≈ 49.5x (≈ 9.04 bits). Per-trial
  |deltaError| ≤ 2·maxSlotError holds everywhere.
- Headroom 160 bits (exact input product) / 210 bits (output) in all 16
  records, above the frozen 128-bit gate; every record prints
  denominator 1267650600226646386227681786497 with q_div 1125899906843009 and
  q_l 1125899906840833.
- The precursor's true runtime RED/GREEN remain supplied and unchanged
  (red-linux shows the genuine 53/54 failure, delta error 1.44e-15 ≫ 2^-80, on
  the frozen binary64 fixture).

## 6. Independent exact-arithmetic checks performed by this review

Bounded Python (`fractions.Fraction`/`decimal`/`int`) checks, no brute force:

1. `1125899906843009 × 1125899906840833 = 1267650600226646386227681786497`
   (equals 2^100 − 1406·2^50 − 689535) — matches TASK.md and every log record.
2. The test's canonical exponent table equals 5^j mod 128, j = 0..15.
3. All 16 frozen products equal the exact rational products of the literal
   vectors (tolerance 1e-75, worst observed deviation 0 at exact arithmetic);
   `frozen[1] − frozen[0]` equals (2^-71+2^-75, −2^-72+2^-74) exactly; the unit
   L1 envelope holds for all 32 inputs.
4. The X^2 witness table equals cos/sin(2π·2·5^j/128) to ≤ 4.96e-81 (130-digit
   π), within the test's 1e-70 tolerance.
5. The MULT2_SCALE_ALGEBRA_CHECK.md witness arithmetic (221/289/48841, errors
   0 vs 2652, magnitude bound 195366 < 208913) re-verified.

## 7. Original Pro chat discrepancy — verified and correctly reconciled

`pro-visible-reply.txt` (the retained visible chat final) says: exact-unity
right operand; "strictly below 2^-80"; precursor audit "P2: 2"; "one new CTest
binding appended"; and cites `CTEST_BINDINGS.tsv` / `SOURCE_CITATION_INDEX.md`,
which do not exist in the return.

The canonical bytes say otherwise, and I verified each point:
TWO nontrivial vectors (`RightValues` has 16 real slots, test 520-541);
`<= tolerance` (test 1132, 1136); P2: 4 (`PRECURSOR_REVIEW.md:9,50-57`);
binding inserted at position 3, not appended (`CMakeLists.txt:148`);
28 static guards (`STATIC_SOURCE_GUARDS.txt`: STATIC_GUARD_COUNT=28, failures
0); the actual TSVs are `CANDIDATE_55_CTEST_BINDINGS.tsv` /
`CURRENT_54_CTEST_BINDINGS.tsv`. The reconciliation in
`FIRST_MULT2_PRECISION_RETURN_AND_HOSTED.md` ("Authority is the coherent source
package, not the inconsistent chat summary") is accurate, and the 33-file
original Pro return is byte-exact under
`coordination/returns/first-mult2-precision-pro-c9ee28d/` (its recorded
SHA-256s for CMake/test/fixture/old-contract all match the live files). No
unity-only implementation exists in the code; chat prose was not treated as
authoritative.

## 8. Findings

**P0: 0. P1: 0. P2: 5** (none blocking this test-only diagnostic).

- **P2-1 (carry-over, open) — small-sample statistics.**
  Source: test line 50 (`kFreshKeyTrials = 4`), unseeded `KeyGen` at 1016;
  evidence: 16 total samples across hosts. Mechanism: fresh unseeded keys give
  observed samples, not deterministic replay or a probabilistic upper bound on
  the 2^-80 gate (worst observed 2^-89.07, ≈49.5x margin). Acceptance
  criterion: a separately registered repeated-trial experiment with a stated
  sampling model before any statistical/universal claim is made. Owner: Codex
  (bounded follow-up; not required for this verdict).
- **P2-2 (new, cosmetic) — right-input expected key tag sourced from the left
  input.** Source: test 1028-1031 passes `leftInput->GetKeyTag()` as the right
  input's expected tag in `CheckCiphertextState`, before line 1031 checks
  equality directly. Mechanism: if the tags ever differed, the failure would
  surface with the state-check label ("right encrypted input key-tag
  mismatch") rather than line 1031's clearer dedicated message; correctness is
  unaffected (equality is still enforced). Acceptance criterion: derive the
  expected tag from `rightInput` itself at the next legitimate touch. No fix
  implemented (per task instruction).
- **P2-3 (new, optional hardening) — no executed RED/mutation demonstration for
  this specific regression.** Source: task/`FIRST_MULT2_PRECISION_RETURN_AND
  HOSTED.md` first-observation rule; artifacts contain only first-observed
  GREEN for first-mult2 (the supplied RED/GREEN pair belongs to the DCP/RCB
  precursor). Mechanism: the test's teeth against a precision-losing Mult2 are
  analytic (binary64 negative control at 662-694 shows a binary64-truncating
  path would miss the 2^-71 delta by ~2^9), not demonstrated by an executed
  sabotage run. This is by declared scope (first-observed GREEN, no fabricated
  RED), so it is not a scope violation. Acceptance criterion (optional,
  owner Codex): one hosted mutation run that breaks Mult2 precision and shows
  this test failing, if executable teeth evidence is ever required.
- **P2-4 (carry-over, open) — automatic CI stale-cache misuse guard still
  TODO.** Source: fixture 258-261 comment; `FIRST_MULT2_PRECISION_RETURN_AND_
  HOSTED.md` ("standalone automated CI misuse guard remains an explicit
  follow-up owner Codex"). Mechanism: the current 28-guard static inspection is
  manual; nothing mechanically prevents a future test from reading the stale
  binary64 packed-value cache. Acceptance criterion: a scoped mechanically
  checked prohibition distinguishing production Decrypt/getters/serialization
  from the test-owned `IndependentDecrypt` oracle, at the next integration
  boundary. Owner: Codex.
- **P2-5 (carry-over, open) — observed headroom is not a universal no-wrap
  theorem.** Source: test 1091-1104; log fields explicitly named
  `input_product_centered_headroom_bits` / `output_centered_headroom_bits`
  (observed 160/210). Mechanism: bit-gap of observed centered representatives
  for these trials only; no all-key/all-noise-history bound. Acceptance
  criterion: a stage-by-stage integer-history bound only if a universal claim
  is later needed. (Correctly labeled "observed" everywhere in the packet.)

Explicitly NOT findings (verified or out of scope): the disproved prior F1
unsigned-remainder claim (disposition read, not repeated); the chat's
unity-RHS/strict-</2-P2/appended/13-guards prose (canonical bytes rule, §7);
the workflow change (separately attributed, purely additive Codex addition;
pin/toolchains/API builds/warnings preserved); production codec,
serialization, deep/repeated-use key immutability, paper Table-3
parameters/security/performance and the printed Theorem 4.8 scale (all outside
this diagnostic's established scope, and the packet says so).

## 9. Verdict

**ACCEPT** — for this exact TEST-ONLY first-Mult2 high-precision diagnostic at
tested source `47907783a6141d0174da79eae264d779fc598f28` (archive source
`e482362...`), on the strength of: verified immutable inputs and manifest
closure; additive-only change (test + 9 CMake lines; production, fixture, old
tests untouched); an exact-integer/multiprecision oracle that cannot share the
targeted binary64 precision-loss defect and whose scale algebra
(2^200/(q_div·q_l)) I re-derived from production source; actual hosted
Linux+Windows 55/55 with in-band provenance markers, 16 fresh-key trials, all
errors ≤ 2^-80 with ≈49.5x margin; and correct handling of the Pro chat
discrepancy. Counts: P0 = 0, P1 = 0, P2 = 5.

This ACCEPT does not call the full paper implementation complete: universal
no-wrap/all-key behavior, repeated-use/deep key immutability, a production
lossless codec, paper 40/60 N=2^15 h=128 parameters, security and performance
remain unestablished (consistent with the packet's own statements). Needed
fixes: none blocking. Bounded follow-ups with owners: P2-1/P2-3/P2-4 → Codex;
printed Theorem 4.8 scale reconciliation → Pro at its next completed boundary.
No fixes were implemented by this review; a later Codex turn reconciles and
runs any required hosted tests.

## 10. Commands actually run and outcomes

All read-only except the two output writes; no builds, no crypto, no network.

1. `shasum -a 256 precision-first-mult2-final-e482362.zip` →
   `50660206...1f179f281`, matches; `stat` → 1304902 bytes, matches.
2. `unzip -Z1` / Python `zipfile` listing → 119 unique regular members, no
   unsafe paths, no directory entries.
3. Python: parse `MANIFEST.json`, verify 118-entry closure vs non-self members
   and every bytes+sha256 → 0 mismatches, 0 missing/extra.
4. Python: walk `input/`, byte-compare all 119 files vs ZIP → identical; no
   extra/missing files.
5. `diff -rq input/baseline input/project` → only workflow, CMakeLists, new
   test file, plus additive evidence dirs.
6. `diff` of CMakeLists (9 added lines, position-3 registration; 54→55
   `add_test` counted) and of the workflow (additive provenance + focused
   steps both hosts; pin/parallel/API-build lines confirmed present).
7. Full reads: new test (1197 lines), fixture .h/.cpp, double_ckks.h, and the
   relevant double_ckks.cpp sections (DCP/Tensor2/Relin2/RS2/Mult2/RCB);
   coordination docs (return-and-hosted, scale-algebra check, precursor
   disposition, static guards, precursor review), provenance JSONs, Pro chat
   reply.
8. Python exact checks (§6): q_div·q_l, 5^j mod 128 table, 16 frozen products
   vs literals, delta identity, L1 envelope, X^2 witness table at 130-digit π,
   MULT2_SCALE_ALGEBRA witness.
9. Log audits: provenance markers, pass counts, timings, per-trial error
   extraction, headroom/denominator fields, trial counts (8+8), CTest-name
   order vs CMake (55/55 identical), log SHA-256/byte-size vs job JSONs,
   `actualCases` vs CMake order.
10. `cmp`/`shasum`: live test/CMake vs `final-changed-files/` copies; patch
    path list; fixture/old-contract hashes vs recorded values.
11. Output writes: `output/REVIEW.md` (this file), `output/MANIFEST.sha256`
    (sidecar, relative path). Transient scratch (two name-list files used for
    the CTest-order comparison) was deleted; final output contains only the two
    deliverables.

## 11. Final input-immutability verification

Re-verified at review close (results in the final turn): ZIP SHA-256 still
`50660206ef1657d19c4ca2cd7540aa61ba301a7fe946f5734e3d3861f179f281`; all 119
`input/` files still byte-identical to the ZIP payloads; LOCAL-REVIEW-TASK.md,
input/TASK.md and all inputs untouched. Review stopped here; no fixes
implemented.
