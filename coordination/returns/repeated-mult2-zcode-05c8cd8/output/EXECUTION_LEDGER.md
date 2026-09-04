# Execution ledger — Mac LOCAL STATIC repeated-Mult2 incoming-candidate review

All work on 2026-09-04, working directory
`/Users/lifeng/Documents/20231788-openfhe-zcode-repeated-probe-review-20260904`
(unless noted). Only `output/REVIEW.md` and `output/EXECUTION_LEDGER.md` were
written. Every command below was read-only over the package; no compilation,
execution of project/candidate code, patch application write, network access,
or agent contact occurred.

## Host / runtime identity (as observable)

- Host: macOS, `darwin 25.3.0 arm64` (Darwin kernel via environment banner);
  shell `zsh`.
- Tools actually invoked: system `python3` (standard library only; ad-hoc
  read-only hash/regex/arithmetic scripts), `shasum -a 256`, `diff`, `grep`,
  `sed`, `wc`, `find`, `ls`, `cat`, `unzip` — none — note: `unzip` was NOT
  invoked; ZIP handling was done inside `python3` `zipfile` (read-only).
- `gitleaks` version `8.30.1` at `/opt/homebrew/bin/gitleaks` (pre-existing
  installation; nothing was installed).
- Model identity running this review: **not observable from within the
  session — unavailable** (per assignment: do not substitute another model's
  identity). This review is Mac LOCAL STATIC work; it is not Windows
  execution and not Fable 5.1.

## 1. Package integrity checks (mine)

| # | Check (command class) | Result | Files/hashes involved |
|---|---|---|---|
| 1.1 | `shasum -a 256 TASK.md`, distribution ZIP + `ls` of all subdirectories | TASK.md 9,445 B → `184c2486…49900` matches assignment; ZIP 3,084,581 B → `72320f25…6ead` matches; `output/` empty (no pre-existing deliverable → proceed, not stop) | `TASK.md`, `distribution/repeated-mult2-zcode-local-static-774fe2d-05c8cd8.zip` |
| 1.2 | Read `PACKAGE_INTEGRITY.md`; `python3` inspection of `MANIFEST.json` schema | 196 entries, 1 self-exclusion (`MANIFEST.json`), 4 not-packaged dirs; payloadBytes 5,890,518; commits match assignment | `MANIFEST.json` |
| 1.3 | `python3`: re-hash all 196 manifest entries against disk; walk for extra files | 196/196 size+SHA-256 exact; zero missing, zero mismatch, zero extra files outside manifest/self/notPackaged | whole package |
| 1.4 | `python3` `zipfile`: enumerate distribution ZIP members, safety screen, hash-compare each to manifest | 197 unique members = exactly 196 entries + `MANIFEST.json`; all bytes/SHA-256 match; no absolute/`..`/backslash/non-normalized paths; no encrypted members; total decoded 6,020,396 B = payload + MANIFEST.json | distribution ZIP |
| 1.5 | `shasum -a 256 inputs/originals/*.zip` | Source ZIP `efc96137…587d` (1,451,817 B) and candidate ZIP `bee2b27e…2fd2` (63,963 B) both match TASK.md | both originals |
| 1.6 | `python3`: ZIP members vs expanded trees. First pass had a checker bug (my `os.walk` filter skipped `.github`) → apparent archive-only member; corrected pass excludes only `.DS_Store` and strips the candidate ZIP's top-level directory prefix | **Source: 156 members = 156 disk files, 0 differing, 4,087,970 decoded B. Candidate: 24 = 24, 0 differing, 231,371 decoded B** — matches PACKAGE_INTEGRITY.md | `inputs/source/`, `inputs/candidate/repeated-mult2-bounded-basis-routing-probe-774fe2d/` |
| 1.7 | `python3`: candidate internal `MANIFEST.json` (22 entries) + `MANIFEST.sha256`; source internal manifest (155 files) | Candidate: 22/22 verify; self-exclusions exactly the 2 declared; `MANIFEST.sha256` value equals recomputed SHA-256 of `MANIFEST.json` (`cdeeab6e…de4b`). Source: 155/155 verify; manifest excludes itself | candidate + source internal manifests |
| 1.8 | `python3`: `verification/archive-content/` vs distribution ZIP | exact 197/197 byte-equal extraction, zero extras | `verification/archive-content/` |

## 2. Candidate reading and identity checks (mine)

| # | Check | Result |
|---|---|---|
| 2.1 | Read (Read tool) `README.md`, `SOURCE_IDENTITY.json`, `DESIGN_DECISION.md`, `PROBE_ACCEPTANCE.md`, `FROZEN_SECOND_MULT2_CONTRACT.md`, `ACTUAL_EXECUTION_LEDGER.md`, `PATCH_REPLAY.md`, `NEXT_PAPER_GATES.md`, `SOURCE_CLAIM_TEST_LEDGER.md`, `SOURCE_LINE_INDEX.json`, `EXPECTED_CTEST_BINDINGS.tsv` | delivery class `BOUNDED_PUBLIC_API_PROBE`; source commit `774fe2d…`, tested production `4790778…`, OpenFHE pin `df495ba…`; internal claims catalogued for verification |
| 2.2 | `shasum -a 256` + `wc -c` of the three `complete/project/` files | `13259 / 1325c941…d12e` (CMakeLists), `59250 / 65b11ef6…503e` (Stage-1 test), `17768 / fbdc5190…7bb` (probe) — all equal `PATCH_REPLAY.md` values |
| 2.3 | `cat` of `evidence/patch_apply.txt`, `evidence/static_validation.json`, `evidence/verify_exact_contract.txt`, `evidence/PUBLIC_API_SYMBOL_AUDIT.tsv`; **inspect-only** read of `tools/verify_exact_contract.py` | candidate's own evidence reviewed; verifier script read as input, **never executed** |

## 3. Independent source inspection (mine)

| # | Check | Result |
|---|---|---|
| 3.1 | `diff inputs/source/project/CMakeLists.txt` vs candidate complete CMakeLists | purely additive 25 lines (206–231); defect found: unconditional dual-family `target_compile_options` at 214/216/226/228 vs guarded block 104–142 (review V1 = R1, derived before reading known findings) |
| 3.2 | Full read of source `CMakeLists.txt`; `python3` regex extraction of `add_test` from both files | source 55 bindings, candidate 57; first 55 identical in order; additive rows 56 (`repeated_mult2_current_second_call_boundary`) and 57 (`repeated_mult2_basis_family_key_routing_probe composition_contract`) |
| 3.3 | `python3` compare `EXPECTED_CTEST_BINDINGS.tsv` (ordinal/name/command/status) to extracted bindings. First pass used wrong column indices (my bug, all 55 "mismatched"); corrected | TSV == candidate bindings exactly; TSV[0:55] == source bindings; ordinals 1–57 correct; statuses `additive-stage1`, `additive-stage2-probe` |
| 3.4 | `diff` Stage-1 test vs `precision_first_mult2_contract_test.cpp` | exactly 3 additive hunks: 5-line header (1–5), 17-line rejection block after line 1056, 1 trailing blank; oracle otherwise byte-identical |
| 3.5 | Static rejection trace in production source: `Invalid()` (`src/double_ckks.cpp:12-14`), `ValidatePair` (440–572), `Tensor2` lifecycle check (763–769), `Relin2` (814+), `RS2` output `RefreshRequired` + `ValidatePair` at construction (≈1090–1112), `Mult2` (1115–1116); original test context around 1000–1060 | exact message `"DoubleCKKS: Tensor2 requires ReadyForFirstMult inputs"` is thrown and nothing preempts it on a second call (pair passed `ValidatePair` identically at construction); if a future second `Mult2` succeeded, the added block throws `runtime_error` — freeze is genuine. Static diagnosis only |
| 3.6 | Full read of `repeated_mult2_basis_family_probe.cpp` (419 lines); `grep` for `Encrypt/Decrypt/Bootstrap/Multiparty/Plaintext` | none present — evaluator-only after setup; controlled-tensor scope confirmed |
| 3.7 | Symbol-by-symbol verification of probe API usage against pinned headers (Read/grep of `official-full` files listed in REVIEW §4) | all observed-present: constructors, `PrecomputeCRTTables`, `numPartQ` guard (→ `numPerPartQ=1` legality + numPartQ=11/sizeQ≤10 throw), P/QP assembly, `SetKeySwitchingTechnique`, `Enable(KEYSWITCH)` no-op, factory interning `CompareTo`, HYBRID key-row sizing/tagging, digit derivation, h=192 KeyGen with secret on QP basis, ciphertext ctors/setters, `Relinearize`, `PrivateKeyImpl`/`SetPrivateElement`, static eval-key map. Gap recorded: `key/evalkey.h` not among the 53 pinned files → `GetAVector/GetBVector` verified only indirectly (pinned `keyswitch-hybrid.cpp:413-414`; project `double_ckks.cpp:838-882`) |
| 3.8 | `shasum -a 256` of both patches | `7aa8d749…535f`, `c8973283…087d` — match `static_validation.json` |
| 3.9 | My own unified-diff parser (`python3`, in-memory only): replay patch 0001 then 0002 against packaged 774fe2d baseline; context lines asserted during replay; new-file hunks reconstructed. First new-file pass was 1 byte short (my parser dropped the final newline; corrected with standard final-newline convention; confirmed no `\ No newline` markers in either patch) | CMake after 0001+0002 == `complete/project/CMakeLists.txt` byte-exact; both new-file hunks == complete files byte-exact. No `git apply`, no writes — `PATCH_REPLAY.md`'s claim independently reproduced |

## 4. Exact-contract and paper checks (mine)

| # | Check | Result |
|---|---|---|
| 4.1 | `python3` (fractions, written from scratch — candidate script not run): 16 `Z=X·Y`, 16 `W=Z²`, both distinguishing deltas, slot-0/1 relative deltas, magnitudes, threshold arithmetic, canonical-payload SHA-256 | all exact and nonzero; slots 0/1 differ only at ~2⁻⁶⁸·⁶/2⁻⁷³·³ relative (below binary64); max operand 0.5, max W component ≈0.0342; 2⁻⁸⁰ ≈ 8.27e-25 > observed 1.7e-27 margin consistent with frozen rationale; embedded canonical hash reproduces under sorted-key/compact JSON |
| 4.2 | Paper TXT `shasum -a 256` + `python3` line reads/searches (embedded NUL handled by decode) | TXT hash `60dd871a…69ae` matches `SOURCE_LINE_INDEX.json`; Definition 4.7 at line 899 (`Mult2 := RS2∘Relin2∘Tensor2`); §6.1=1472 (first "8 repeated squarings" 1487), §6.2=1509 (18 squarings 1525, h=21,845 at 1513), §6.3=1562–1590 (depth 8, Δ=2¹⁰⁰, "8 repeated squarings … 1,000 executions", −81.8 bit); Table 3 caption 1580 ends "The secret key has Hamming weight ℎ= 128."; t=2 row 1588/1590 (N=2¹⁵, dnum 11, Base 50×2, Mult 60×8, Div 40, P 60) |
| 4.3 | Cross-compare `SOURCE_LINE_INDEX.json` vs `SOURCE_IDENTITY.json` vs actual paper lines | new finding V6: index mislabels §6.1 range as `section_6_3_repeated` (1472–1517) and points `table_3` at the §1.5 first mention (169–214); `SOURCE_IDENTITY.json` ranges are correct; internal contradiction |
| 4.4 | Pinned sampler check: hashes of `ternaryuniformgenerator{,-impl}.h`; Read of `-impl.h:45-129` | hashes match h128 clarification records; h≠0 branch yields exactly h nonzeros (duplicate-position retry) and accepts only rounds with +1-count in `h/2±1` → 63/64/65 positives for h=128 (code reading; sampler not executed) |
| 4.5 | Supplemental alias check: full read of `inputs/supplemental/ciphertext-fwd.h` | `ConstCiphertext = const std::shared_ptr<const CiphertextImpl>` (line 50) → conversion temporary binds to `ConstCiphertext&`; alias concern dismissal stands |
| 4.6 | Read `inputs/source/TASK.md` (originating spec) and `inputs/source/project/coordination/PAPER_H128_OFFICIAL_API_SUPPORT.md:88-100` | bounded Task-D probe return is the authorized shape; old-audit 63–65-positive question resolved via packaged equivalent |
| 4.7 | Read `policy/engineering.md`, `policy/OPENFHE_WORKFLOW.md` | claim-separation and static-scope constraints applied; their collaboration/build preferences superseded by this assignment's static-only boundary |

## 5. Known-findings reconciliation (mine)

Read all four files in `inputs/known-findings/` only **after** completing §3–4.
Statuses (evidence in REVIEW §3): R1 confirmed (also independently derived as
V1 pre-read), R2 confirmed (incl. `VerifyCKKSScheme` call-site mapping via a
`python3` function-name extractor over `cryptocontext.h`; contrast at
`gen-cryptocontext-ckksrns-internal.h:145-146`), R3 confirmed (probe
124–140/205–213/386–392 vs `FindContext`/`CompareTo`), R4 confirmed
(`kTestName` at candidate line 43, prints 1173/1212/1216), R5 confirmed
(`DESIGN_DECISION.md:5` vs G2-before-G3), R6 confirmed as duplication
recommendation only. h128 clarification: agreed, all citations verified. Sidecar
(`8e08b5f7…0619`), `REDELIVERY_VISIBLE_FINAL.md` and `returned-decoded-gitleaks.json`
are root-side artifacts not packaged here — their contents remain supplied
claims, not verified in this environment.

## 6. Security scans (mine)

| # | Command | Result |
|---|---|---|
| 6.1 | `python3` 10-pattern scan (private keys, AKIA, `sk-`, `gh[pousr]_`/`github_pat_`, bearer literals, session cookies, JWT, signed URLs, `AIza`, `xox*`) + denied-filename regex over all 197 decoded archive members | 0 findings; 0 denied filenames |
| 6.2 | `gitleaks version` | 8.30.1 (pre-installed) |
| 6.3 | `env -u GITLEAKS_CONFIG -u GITLEAKS_CONFIG_TOML gitleaks dir distribution --redact --no-banner --no-color --report-format json --report-path - --gitleaks-ignore-path /dev/null --ignore-gitleaks-allow --max-archive-depth 3 --max-decode-depth 5 --max-target-megabytes 10` | exit 0; "scanned ~7305207 bytes (7.31 MB)"; "no leaks found"; `[]` — matches root receipt byte count and result |
| 6.4 | Same command with `dir .` and `--max-archive-depth 2` (package now contains `distribution/` + `verification/`, hence ~21,930,508 bytes vs root's 7.17 MB at pack time) | exit 0; "no leaks found"; `[]` |

## 7. Input-immutability verification (mine)

- Before substantive review (step 1.3): all 196 manifest entries byte-exact.
- Mid-review re-check after all analysis: 196/196 entries still byte-exact;
  `TASK.md` and both original ZIP hashes still match assignment values.
- Final re-check after writing both deliverables: 196/196 entries byte-exact;
  originals and TASK.md unchanged (final command below). **Package inputs
  remain byte-identical before and after this review.** The two outputs live
  in `output/`, which is outside the 197-member archive closure.

## 8. Explicit NOT RUN for this review

- **C++ configure/build (CMake/compiler/linker): NOT RUN.**
- **CTest / any test execution: NOT RUN.**
- **Cryptographic execution (OpenFHE or otherwise): NOT RUN.**
- **Windows execution / CI dispatch / GitHub Actions: NOT RUN.**
- **Candidate scripts (`tools/verify_exact_contract.py`) and project/candidate binaries: NOT RUN** (inspected only).
- **Patch application writes (`git apply` or equivalent), Git index/commit/push/merge: NOT RUN** (replay was in-memory `python3`).
- **Second Mult2 (semantic repeated multiplication): NOT IMPLEMENTED / NOT RUN** — bounded delivery boundary per source TASK.md D; no claim made here.
- **Paper §6.3 reproduction (eight squarings, 1000 trials, 81.8-bit): NOT RUN.**
- **Network/browser/account/credential access; dependency installation: NOT RUN.**
- **External-agent delegation (Codex/Pro/ZCode/Fable/CI or any other): NOT RUN — no external agent was contacted.**

## 9. Supplied claims vs my checks (separation)

- **Supplied, not re-executed here:** candidate `ACTUAL_EXECUTION_LEDGER.md`
  RUN/PASS rows (ZIP safety, 81 blob identities, 53 official identities,
  generation of exact vectors, their verifier run, their `git apply` replay,
  55-binding preservation) — I independently reproduced the replay, the
  bindings, the exact-arithmetic checks and the archive/manifest closures with
  my own tooling, but their runtime rows remain their claims; hosted
  first-Mult2 Linux/Windows results cited in `inputs/source/TASK.md`
  (runs 33873114880, jobs 101023587797/101023588186) are supplied
  source-associated evidence; root's gitleaks receipts — independently
  re-run and matched (§6); root's provenance chronology in
  `SOURCE_SNAPSHOT.json`/`FINAL_ARCHIVE_RECEIPT.json` — internal-consistency
  checked against packaged bytes only; no network verification attempted.
- **My checks:** everything in sections 1–7, with the three honest
  intermediate checker errors I made and corrected (`.github` walk skip in
  1.6; TSV column indexing in 3.3; final-newline handling in 3.9) recorded in
  place.

## Final state

Deliverables written: `output/REVIEW.md`, `output/EXECUTION_LEDGER.md` (this
file). Final post-write integrity command: `python3` re-hash of all 196
manifest entries + `shasum -a 256` of `TASK.md`, both original ZIPs, and the
distribution ZIP — all byte-identical to the pre-review values recorded in
§1. No external agent was contacted during this review.
