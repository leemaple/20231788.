# Independent final review: Pair Add/Sub result -> first Mult2 composition (LOCAL STATIC)

## 1. Verdict

**ACCEPT** for the exact test-only functional slice defined by this task:
TEST-ONLY regression coverage that feeds a public Pair `Add`/`Sub` result at
`ReadyForFirstMult` into the first public `Mult2`, with frozen `1e-3`, N64,
HYBRID/COMPLEX diagnostic fixture, and exactly 53 old + 2 new = 55 CTest
bindings.

Finding counts: **P0: 0, P1: 0, P2: 3** (all informational / accepted-risk;
none blocks this slice; details in section 7).

Bounded functional diagnostic verdict: the composition coverage is soundly
engineered and independently verifiable in static form. The exact-arithmetic
oracle chain (independent CRT/secret recovery of the TWO ORIGINAL pairs ->
centered BigInt sum/difference identity on the composed pair -> exact integer
negacyclic product certificate with separately constructed high/low Relin2
path errors -> public RCB decoded-slot observation against frozen dyadic
literals at 1e-3) is internally consistent, and the supplied hosted stage2
evidence on both platforms is consistent with it. This ACCEPT covers only the
inspected bytes, exact arithmetic, and audited supplied logs; it is NOT
paper precision, repeated multiplication, a universal key/noise/BV theorem,
security, performance, or full project completion.

## 2. Actual identity, model and host

- Reviewing agent: ZCode (interactive coding agent), operating per the root
  `LOCAL-REVIEW-TASK.md` wrapper and `input/TASK.md`.
- Model label as declared by the harness environment:
  `builtin:bigmodel-coding-plan/GLM-5.3`. Per task rules this self-declared
  label is not treated as an independently proven version.
- Host: local macOS, `darwin 25.3.0` (arm64), zsh, single local process.
- Mode: user-authorized LOCAL STATIC fallback review. This is NOT a Windows
  execution and NOT Fable5.1 (terminal Fable5.1 calls previously returned
  403; no Fable5.1 review exists). No OpenFHE/project build, crypto, CTest,
  network, subagent, CI or Git operation was performed in this review.

## 3. Input/provenance gate (all independently verified)

Commands actually run (abridged; full ledger in section 9):

    shasum -a 256 pair-composition-final-da50a7e.zip      # + stat -f %z
    python3 (zipfile)   # closure/safety/CRC/member-hash vs MANIFEST.json
    python3 (walk)      # input/ tree vs ZIP member bytes, forbidden classes
    python3 (json)      # SOURCE-PROVENANCE.json vs input/ and MANIFEST.json

Outcomes, all matching the wrapper's claims exactly:

- ZIP `pair-composition-final-da50a7e.zip`: 1,293,653 bytes, SHA-256
  `4417299f03ab04277aa479cfa8a761d03b854c957c7b17d6174ed90f2610e64b`. Match.
- ZIP contains exactly 111 entries, all unique, all regular files, no
  directories/symlinks, no absolute/`..`/backslash/unsafe paths. `testzip()`
  CRC check passes on all 111.
- `input/MANIFEST.json`: 110 non-self entries (`manifestSelfExcluded: true`),
  exact closure against the ZIP (ZIP-only set = {MANIFEST.json};
  manifest-only set = empty). Every one of the 110 declared byte sizes and
  SHA-256 hashes matches the streamed ZIP member. All 110 verified: PASS.
- Extracted `input/` tree: 111 files, byte-for-byte identical to the
  corresponding ZIP members (all 111 compared). No `.git`, `node_modules`,
  build/cache/database/runtime/browser state or credential-looking members
  (the only `.git`-substring hits are the two legitimate `.github/workflows/`
  files).
- `SOURCE-PROVENANCE.json`: 86 gitFiles (67 `project/` @ `da50a7e1af…`,
  19 `baseline/` @ `8a465764…`) + 22 pinnedReferenceFiles + explicit new
  `TASK.md` + the provenance file itself + MANIFEST = the 111 manifest paths
  exactly. All 86 + 22 declared sizes/hashes re-verified against `input/`.
  Declared identities: sourceCommit `da50a7e1af05eaaff40d7ce82068e65da02815a2`,
  testedSourceCommit `b48b54e22f14bbfe988a6890f1b03eac9efb11a3`, baseline
  `8a465764044d8b1e1578f462ea4916f7123428a4`, branch
  `codex/pair-mult2-composition-01`, OpenFHE pin
  `df495ba2e91739a6dc8f1de254fc5a41155ce504` (pin also visible in the raw
  hosted logs and workflow, section 6).
- Codex's gitleaks 8.30.1 zero-findings claim is supplied evidence; I did not
  re-run it and treat it as evidence, not a guarantee.

## 4. Source review (active diff vs baseline)

Changed files between `input/baseline/` and `input/project/` (build/test
inputs only): `CMakeLists.txt`, `tests/mult2_e2e_oracle_test.cpp`,
`.github/workflows/dcp-rcb.yml`. Everything else identical:

- `src/double_ckks.cpp` and `include/openfhe_2023_1788/double_ckks.h`:
  byte-identical to baseline (SHA-256 `3ed23f22…` / `3d089be5…`). No
  production or public-header change. PASS.
- All 14 other old test sources: byte-identical to baseline (cmp per file).
- Active CMake SHA `bf27cc6e2514049019671bbc8bc2461e65e0b63431dddd3a1b0194fa020180fa`,
  oracle SHA `7522b2e3aaf3f2730e52ae7fe0a45c77517bb45f973dc4eca6ed65da59e7be8d`,
  workflow SHA `f8549f97bff475990f19a5172af5a2f20412f6f12b71b356ebd3a828ac115511`
  — all three match the TASK-declared values, and both CMake/oracle equal the
  Pro return's `final-project/` copies byte-for-byte. PASS.

### 4.1 CMake 53 -> 55 closure

`diff baseline/CMakeLists.txt project/CMakeLists.txt` shows exactly two
appended `add_test` bindings:

- `mult2_pair_add_input_hybrid_complex` -> `mult2_e2e_oracle_test pair_add_input_hybrid_complex`
- `mult2_pair_sub_input_hybrid_complex` -> `mult2_e2e_oracle_test pair_sub_input_hybrid_complex`

Counts: baseline 53, current 55; the first 53 bindings are identical and in
the original order (diff of the binding lists: empty); no duplicate names.
No executable target or compile/warning setting was touched. PASS.

### 4.2 Workflow delta (Codex-attributed, additive only)

`diff` shows exactly three additions: one branch-admission line
(`codex/pair-mult2-composition-01`) and two focused CTest steps (Linux;
Windows msys2) running
`ctest … -R '^mult2_pair_(add|sub)_input_hybrid_complex$'`. Full CTest step,
the five explicit API-contract builds, warnings-as-error build, OpenFHE pin
verification (`test "$actual" = "$OPENFHE_COMMIT"`), resource caps and
platform setup are unchanged. The original Pro task said "do not change
workflow"; TASK.md explicitly acknowledges these Codex-owned verification
edits, and they are outside the Pro candidate's two-file scope. Verified
additive; recorded as observation O-1 (section 7), not a violation by the
candidate.

### 4.3 Pro patch scope and cumulative equality

Applied the three supplied raw patches with `patch -p1` in a scratch copy
under `output/tmp-patch-check/` (temporary; removed after use; named in the
ledger):

- `0001-add-input-first-mult2-regression.patch` (SHA `0fde6df4…`, matches the
  return doc) applies cleanly to the two baseline files; the intermediate
  files match the claimed stage1 bytes exactly: CMake 11,269 bytes
  `05bacc8c…`, oracle 73,356 bytes `ba71c84b…`.
- `0002-sub-input-first-mult2-regression.patch` (SHA `3e342451…`, matches)
  applies on top; cumulative result is byte-identical to the current
  `project/CMakeLists.txt` and `project/tests/mult2_e2e_oracle_test.cpp`.
- `candidate-final.patch` applied alone to the baseline reproduces the same
  two current files byte-identically.
- Each patch touches only `CMakeLists.txt` and
  `tests/mult2_e2e_oracle_test.cpp` (no production/header/workflow/other-test
  mutation). The only line removed anywhere in the oracle diff is the old
  usage string, extended with the two new selectors. PASS.

### 4.4 Oracle test audit (new code at tests/mult2_e2e_oracle_test.cpp)

The full diff was read; every helper the new code calls is pre-existing and
unchanged (the diff is purely additive except the usage string). Line numbers
below refer to the current file.

- Independent CRT/secret oracle (pre-existing, read in full):
  `IndependentDecrypt` (`:222-278`) recomputes RLWE decryption per tower with
  explicit BigInt negacyclic modular products — it does not call OpenFHE
  Decrypt; `ReconstructCentered`/`Center` (`:78-133`) does exact CRT with
  symmetric centering; `Recombine` (`:280-290`) forms `divisor*high + low`
  centered; `IndependentDecryptPair` (`:298-306`). This is test-only exact
  arithmetic, not an evaluator shortcut.
- Composition expectation from the TWO ORIGINALS: in
  `RunPairArithmeticInputCase` (`:1348-1472`), `leftPlain`/`rightPlain` are
  independently decrypted at `:1416-1417` BEFORE `ComposeFirstMultInput`
  (`:1418`) runs; `ExpectedPairInputCoefficient` (`:1193-1205`) materializes
  explicit `BigInt(left + right)` / `BigInt(left - right)` branches (avoiding
  Boost expression-template conditionals) and centers mod Q;
  `CheckPairInputRecombination` (`:1208-1229`) requires per-coefficient
  equality of the composed pair's recombination with the centered
  sum/difference of the two ORIGINAL recombinations. The expected value is
  not production `EvalAdd`/`EvalSub` (neither appears anywhere in the file),
  not the composed output, and not staged/direct self-equality. PASS.
  Arithmetic inference: production `Add`/`Sub` (`src/double_ckks.cpp:657-683`,
  `:684-710`) are componentwise per-tower modular `+=`/`-=` with strict
  fail-fast compatibility validation (`ValidatePairCompatibility`), so the
  centered-CRT linearity identity checked here is an exact algebraic
  identity, valid regardless of encryption noise.
- Staged vs direct as WIRING only: `Tensor2->Relin2->RS2` staged path and
  direct `Mult2` (`:1438-1441`); result-state checks for both; equality of
  the two is explicitly commented as wiring evidence only (`:1435-1437`).
  PASS.
- Exact product certificate: `CheckIndependentArithmetic` (`:801-960`)
  independently decrypts composed/multiplier/tensor/relinearized/result,
  cross-checks the independently recombined Tensor ciphertext, and compares
  the actual Mult2 output against `NegacyclicProductInteger` of the two input
  recombinations with an execution-specific integer bound
  (`:913-939`), a non-wrap witness (`:900-904`), and the separately
  constructed high/low Relin2 path certificate
  (`CheckRelin2PathIdentity`, `:397-476`: raised-high path and direct-low
  path each Relinearized independently, per-coefficient
  predicted==actual pair error, triangle bound). The conservative labels
  `execution_certificate=PER_PATH_CONDITIONAL`,
  `conservative_E_Relin_available=false`,
  `universal_theorem_gate=UNPROVED` are printed by `PrintCertificate`
  (`:1083-1085`) for every case including the two new ones. No BV theorem
  claim is made. PASS.
- Public RCB decoded-slot observation: `CheckDecodedSlots` (`:968-1025`)
  uses public `module.RCB` + standard Decrypt, verifies the descriptor ratio
  equals `2^(2p)/(q_div*q_l)` within 32 long-double ulps, the recorded-scale
  deterministic bias, and logical-scale error <= frozen
  `kLogicalDecodedAbsoluteTolerance = 1.0e-3` (`:49`, unchanged) against the
  frozen product literals. PASS.
- State/assertion set on the composed pair before multiplication
  (`CheckReadyForFirstMultInputState`, `:1293-1346`): lifecycle
  ReadyForFirstMult, context identity, divisor, ordered moduli, level == 1,
  recorded scaling factor, noise-scale degree == 2, key tag, slots, EVALUATION
  format, component count == 2, paper-scale quartet, plus
  `CheckPhysicalCiphertextState` on high and low members. Output state after
  Mult2 (`CheckPublicResultState`, `:713-746`): RefreshRequired, level 2,
  degree 2, exact one-tower drop, prefix basis, etc. PASS.
- Fresh genuine keys + eval-key row: per-case `KeyGen` + `EvalMultKeyGen`
  (`:1369-1374`); `GetAllEvalMultKeys()` row presence asserted before Mult2
  (`:1430-1433`) and still present after the whole window (`:1468-1471`).
  Different secret Hamming weights across the eight hosted records
  (36-49) corroborate fresh fixtures per execution. Presence-only scope is
  disclaimed (P2-1).
- Three imaginary witnesses before Encrypt (`:1385-1392`): slot-0 imaginary
  literals `-0.0625`, `0.03125`, `0.125` checked against the packed-plaintext
  cache of the three fixtures; consistent with the frozen A/B/C slot-0 values.
  PASS.
- Snapshots: all three encrypted inputs and all three original pairs
  snapshotted before the composition window (`:1406-1414`), composed pair
  snapshotted before Mult2 (`:1429-1430`); full nonmutation checks
  (identity + deep clone equality + all manifest fields + paper-scale
  quartet) after the window (`:1454-1467`). PASS.
- Frozen context: N64/batch16/p30/first35/depth7/FIXEDMANUAL/HYBRID/
  digitSize0/maxRelinDegree2/UNIFORM_TERNARY/HEStd_NotSet/COMPLEX, encoding
  degree 2 / level 0 (`:34-43`, `MakeContext` `:582-603`) — matches the TASK
  fixture exactly; unit L1 envelope `|re|+|im| <= 1` (`CheckHostEnvelope`,
  `:635-642`) enforced on A, B, C and the expected composed values.
- Failure paths / KISS / portability: dispatch defaults `throw TestFailure`;
  `main` (`:1620-1641`) catches `TestFailure` then `std::exception`, returns
  nonzero; no broad catch, no speculative backend, no production API
  expansion, no tolerance weakening. `std::ldexp` literals are portable. PASS.

### 4.5 Independent exact dyadic verification of the frozen vectors

Exact `fractions.Fraction` arithmetic on
`input/project/coordination/handoffs/pair-mult2-composition-frozen-host-vectors.json`
(matching Codex's records in `codex-host-vectors.json` and
`pair-mult2-composition-host-vector-check.json`):

- All 112 scalars across the 7 arrays are exactly representable dyadic
  binary64 values (JSON round-trip exact).
- All 32 complex identities hold EXACTLY: sum = A+B, difference = A-B,
  sumTimesC = (A+B)*C, differenceTimesC = (A-B)*C per slot; plus the
  distributivity cross-check (A+B)*C == A*C + B*C for all 8 slots.
- Unit L1 envelopes (max |re|+|im|): A = 3/8, B = 3/16, C = 3/4,
  sum = 7/32, difference = 9/16 — all match the documented claims and are
  within the unit envelope. (Products: sumTimesC 19/256,
  differenceTimesC 21/128.)
- Slot-0 distinguishing values: (A+B)*C = (-11/256, 1/32), (A-B)*C =
  (-1/256, 1/32), and dropping B gives A*C = (-3/128, 1/32); real-part
  separations 0.0390625 and 0.01953125, both far above 1e-3. Tiny slot-5
  values (~2^-20 and below) are controls only; nothing here supports a
  >53-bit precision claim (the binary64 controls are exact dyadics, and the
  1e-3 gate is unchanged).
- Cross-check of the C++ literals: parsed all seven `FrozenPair*Values()`
  functions (including `std::ldexp`/coefficient*`ldexp` forms) from the
  current oracle test — all 112 scalars are EXACTLY equal (binary64 `==`) to
  the frozen JSON values. PASS.

## 5. Supplied hosted evidence audit (audited, not re-run)

All items below are supplied GitHub-Actions evidence that I verified for
internal consistency against the audited source bytes; nothing was executed
in this local review environment.

Stage2 (tested source `b48b54e…`), run 33871723090:

- Linux job 101019032191 (`linux-gcc`, ubuntu-24.04): completed
  `2026-09-04T12:17:07Z`, conclusion success. Focused Add/Sub 2/2
  (total 0.43 s), full 55/55 (total 1.06 s). Warning-clean build step and
  all five explicit API-contract builds (Relin2, RS2, Mult2, Add, Sub)
  succeeded. Matches TASK.md.
- Windows job 101019032537 (`windows-mingw64`, windows-2022): completed
  `2026-09-04T12:21:00Z` per the supplied job JSON (authoritative), success.
  Focused 2/2 (total 0.15 s), full 55/55 (total 1.68 s); same five API
  builds and warning-clean step green. Matches TASK.md.
- Raw logs re-hashed: `add-sub-linux.txt` 63,386 bytes SHA
  `b4ea478c185fbd3f10f66b75425d897380a44066d5f42907ce7a9f958115cc02`;
  `add-sub-windows.txt` 67,039 bytes SHA
  `fcd4fc79f708e8aceb20c1e6c97770a5e415ddee5b986e49f0e5335fe9e33148`.
  Both match TASK.md and the JSON sidecars.
- Actual CTest records (not badges): parsed every full-suite test line from
  both logs — 55/55 sequential entries, every one `Passed`, and the actual
  name sequence equals the current CMake binding order exactly on both
  hosts. Focused logs show exactly the two new composition tests.
- Composition certificates: each stage2 log contains exactly four new pair
  records (Add focused, Sub focused, Add full, Sub full) — eight across both
  hosts — with fresh per-execution fixtures. The four older
  `mult2_e2e_{hybrid_real,hybrid_complex,bv_real,bv_complex}` certificates in
  each full run were NOT counted as new composition samples.
- Every certificate's printed integer comparisons re-verified exactly for
  all 8 stage2 + 4 stage1 records: triangle bound == high+low path errors;
  pair Relin2 error <= triangle bound; additivity residual <= secret h;
  non-wrap witness `2*left < Q_l`; `2*coefficient_error <= execution_bound`;
  measured logical slot error <= 1e-3 (max observed ~8.53e-10). Also
  verified `2^60/(q_div*q_l)` = 1.00000023655603276 exactly as printed
  (q_div = 1073741953, q_l = 1073741441), 7 active towers = 215 bits =
  35 + 6*30, and >= 4 retained towers.
- OpenFHE pin `df495ba…` appears in the run env of every step in both logs.

Stage1 (source `cdc4711…`), run 33869933158: Linux job 101013240028
focused Add 1/1 (0.05 s case, 0.52 s total), full 54/54 (0.51 s); Windows
job 101013240269 focused 1/1 (0.09 s, 0.10 s total), full 54/54 (2.19 s);
54-name sequences match the stage1 CMake order; log hashes
`f09918a5…`/`6dd61670…` match the return doc. Both additions were
FIRST-OBSERVED GREEN against already-implemented behavior; the prior
Add/Sub/Mult2 features retain genuine red/green history; no artificial red
was manufactured and no threshold changed after any observed failure.

## 6. Distinction of evidence classes

- Inspected facts: everything in sections 3 and 4 (bytes, hashes, diffs,
  patches, source bodies, literals, log/JSON contents).
- Arithmetic inference: exact Fraction/integer identities in sections 4.5
  and 5 (dyadic identities, certificate inequalities, ratio/bit arithmetic);
  the algebraic soundness argument for the centered-CRT Add/Sub identity.
- Supplied hosted runs: everything in section 5 (GitHub Actions jobs/logs/
  JSONs). They were NOT run in this environment; consistency with the
  audited source was verified, which is the strongest check available to a
  local static review.

## 7. Findings

No P0 or P1 findings. Three P2-class items, none of which blocks this slice:

- **P2-1 (accepted risk)** — `tests/mult2_e2e_oracle_test.cpp:1430-1433` and
  `:1468-1471`: the evaluation-key checks establish ROW PRESENCE in
  `GetAllEvalMultKeys()` only. Mechanism/counterexample: a hypothetical
  mutation that rewrote eval-key MATERIAL while leaving the row present and
  the Mult2 results numerically unchanged would not be caught by these two
  checks alone (functional corruption would still be caught by the exact
  coefficient certificate). This is an enumerated-check, not deep
  key/cache/context immutability — exactly as the task and return documents
  disclaim. Acceptance criterion (out of current frozen scope): a
  key-material digest asserted across the composition window. Status:
  accepted risk.
- **P2-2 (accepted, fail-closed)** — `tests/mult2_e2e_oracle_test.cpp:1231-1259`
  (`CheckFrozenPairHostArithmetic`) uses exact binary64 `==` on
  `materialized * multiplier[slot]`. This is sound only because every frozen
  value and product is an exactly representable dyadic (independently
  re-verified in section 4.5; all products carry <= ~17 significant bits).
  A future non-dyadic fixture would make the check fail spuriously — never
  pass falsely. Acceptance criterion: if the fixture arrays are ever changed,
  re-verify dyadic representability of values AND products before hosted
  execution. Status: accepted for the frozen fixture.
- **P2-3 (accepted coupling, test-only)** —
  `tests/mult2_e2e_oracle_test.cpp:1385-1392`: the three imaginary witnesses
  hard-code slot-0 literals (-0.0625 / 0.03125 / 0.125) that must move in
  lockstep with the frozen A/B/C arrays. Mechanism: editing the arrays
  without these constants would produce a confusing failure rather than a
  silent pass (fail-closed). Acceptance criterion: any fixture change must
  update the witnesses in the same commit. Status: accepted.

Observation (not a finding against the candidate):

- **O-1** — `.github/workflows/dcp-rcb.yml:13`, `:98-102`, `:242-258`:
  the workflow differs from baseline although the Pro drafting task had said
  "do not change workflow". The delta is Codex-attributed CI admission plus
  the two focused steps; verified additive-only with full suite/API/warning
  steps and pin verification unchanged, and TASK.md explicitly carries this
  attribution. Status: accepted with attribution (documented integration
  fact, not hidden).

## 8. Scope boundaries and remaining uncertainty

- This review is a bounded LOCAL STATIC review. No compile, run, crypto,
  CTest, network, subagent or Git operation occurred here; hosted results
  are audited evidence, not local observations.
- The accepted slice is ordinary functional composition coverage at the
  existing first-Mult2 seam: (A+B)*C and (A-B)*C, fixed 1e-3 tolerance, N64
  p30 HEStd_NotSet diagnostic fixture. It is NOT high precision, NOT paper
  Table 3 parameters, NOT repeated multiplication (Mult2 output is
  RefreshRequired and never re-multiplied), NOT a universal key/noise/BV
  theorem (`universal_theorem_gate=UNPROVED`,
  `conservative_E_Relin_available=false` retained), NOT security or
  performance evidence, and NOT full project completion. The separate
  precision track's 2^-80 results were not borrowed.
- Tiny slot-5 values are controls; the 1e-3 gate alone could not certify
  accuracy at their magnitude — arithmetic correctness at that scale rests
  on the exact coefficient certificate, not on the decoded tolerance.
- Remaining uncertainty: hosted evidence is trusted as supplied GitHub
  artifacts consistent with the audited bytes; a local static review cannot
  prove what ran on the runners beyond that consistency. Codex's gitleaks
  claim was not re-run. Model identity of prior agents (Pro "GPT-5.6 Pro"
  metadata, Codex notes) is supplied self-report, per the return docs
  themselves.
- Incident disclosure: one `Write` tool call during deliverable creation
  targeted a mistyped directory (`…final-review-20260804/output/REVIEW.md`,
  a one-digit typo of this folder). It contained only the word "placeholder",
  was deleted immediately with its directory, and touched no project, input
  or ZIP data. The authorized folder was not affected.

## 9. Command ledger (what was actually executed here)

Read-only inspection and hashing (all under the authorized folder):

- `shasum -a 256`, `stat -f %z`, `zipinfo -l` on the ZIP.
- Python3 `zipfile` closure/safety/CRC/member-hash verification vs
  `input/MANIFEST.json`; Python3 walk comparing all 111 `input/` files to
  ZIP member bytes; forbidden-class scan; `SOURCE-PROVENANCE.json` vs
  `input/` vs manifest closure (67+19+22+TASK+provenance+manifest = 111).
- `diff -rq`/`diff -u` baseline vs project (3 changed files identified);
  `cmp`/`shasum` for the 14 unchanged old tests, production/header, Pro
  final copies; `grep`/`diff` of the 53->55 CMake binding lists.
- Python3 `fractions.Fraction` exact verification of all 112 scalars, 32
  complex identities, distributivity, L1 envelopes, slot-0 margins;
  Python3 parser cross-checking the seven `FrozenPair*Values()` C++
  literal arrays (ldexp forms included) against the frozen JSON (112/112
  exact binary64 equal).
- `patch -p1` (dry-run then apply) of Pro patches 0001, 0002 and
  candidate-final in `output/tmp-patch-check/` (temporary scratch copy of
  the two baseline files; SHA/cmp of intermediates and cumulative results;
  directory deleted afterwards — the only temporary files used, named here
  as required).
- Reading of full helper bodies in the oracle test, production
  Add/Sub/compatibility code, workflow, CMake warning settings.
- Python3 exact verification of all 12 hosted certificate records'
  integer inequalities, ratio and tower-bit arithmetic; regex extraction of
  the actual full-suite/focused name sequences from all four raw logs vs
  the current CMake order; `grep` scans for summaries, certificate counts,
  API builds, pin env, warnings steps.
- Final re-verification of all 111 inputs, the ZIP and the wrapper
  (section 10).

Not executed (prohibited and not attempted): any build, OpenFHE use,
CTest, benchmarks, network, browser, subagents, Git mutation, CI actions,
fixes, script execution, /tmp or other outside-folder writes (apart from
the disclosed-and-deleted typo directory), and any read of other local
projects or implementations.

## 10. Deliverables and closing re-verification

- This file: `output/REVIEW.md`.
- Sidecar: `output/MANIFEST.sha256` (SHA-256 of REVIEW.md with its correct
  relative path).
- After writing both, all 111 manifest inputs, the extracted `input/` tree,
  the ZIP (size + SHA-256) and the root `LOCAL-REVIEW-TASK.md` wrapper were
  re-verified unchanged. Actual re-run outcome: ZIP 1,293,653 bytes /
  `4417299f…e64b` match; all 110 manifest size+SHA comparisons PASS against
  both disk and ZIP members; 111 `input/` files; archive CRC OK;
  `input/MANIFEST.json` SHA `1ad88add…625f` unchanged; wrapper 2,832 bytes,
  unmodified since receipt. UNCHANGED/PASS.

Review stops here per the task: no fixes, no follow-up implementation.
