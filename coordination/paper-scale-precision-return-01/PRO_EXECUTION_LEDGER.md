# Execution ledger — independent paper-scale precision review

Date: 2026-09-05. This ledger distinguishes incoming hosted evidence, local review checks, unsuccessful preparation attempts, and **NOT RUN** production validation. No result below is an independent approval of this review's own candidate.

## 1. Environment and boundaries

Local review runtime, recorded in `evidence/review_environment.txt`:

```
Linux 6.18.35 x86_64
g++ (Debian 14.2.0-19) 14.2.0
Python 3.13.5
BOOST_LIB_VERSION 1_83
git version 2.47.3
```

This is the review container, **not the user's Mac and not either hosted production job**. The local compiler differs from the failed hosted Linux GCC 13.3 compiler. The four supplied Boost 1.83 headers were compared byte-for-byte with the installed headers actually used by the probes; all four matched. This does not assert byte identity for the entire installed Boost tree. The comparison and hashes are in `evidence/boost_header_comparison.json`.

No OpenFHE project was configured, compiled, linked, or executed here. No FHE, NTT, FFT, benchmark, key selection, ciphertext retry, 1,000-trial batch, or full-slot numeric transform was executed locally. Scalar complex arithmetic, ten scalar root calculations, Python exact-rational/Decimal calculations, text/ZIP processing, and PDF rendering were the only relevant computational checks. The probe binaries are outside the delivered archive.

There was no repository push, commit, merge, CI dispatch/rerun, account change, credential access, external write, or reuse of an old implementation. Patch application occurred only in disposable copies of the supplied source; the extracted input itself remains unchanged.

## 2. Input extraction and verification — PASS

Input file:

```
/mnt/data/paper-scale-precision-b1b024e3.zip
bytes: 1480623
SHA256: 28fa7ee1297af78ac4c2848b85d1fce1efdd81bbd1a13d15b0e75076c2719336
```

Initial Python ZIP inspection verified CRC, safe unique regular member names, no symlink/path traversal, exact manifest closure, and per-file size/hash, then extracted to `/mnt/data/input`. The initial interactive command was not retained as a standalone script. A read-only verification was subsequently made reproducible and executed exactly as follows:

```sh
python3 /mnt/data/precision_review/checks/verify_input.py \
  /mnt/data/paper-scale-precision-b1b024e3.zip /mnt/data/input \
  > /mnt/data/precision_review/evidence/input_verification.json
```

**Exit 0.** 134 unique safe regular members; 133 non-self manifest payloads; 4,548,367 expanded bytes; CRC PASS; all manifest hashes/sizes match; all extracted files match their ZIP bytes; no additional extracted files. Baseline, checkpoint, and official pin are recorded in the output. This verifies internal binding, not remote Git authenticity.

## 3. Source, paper, and log inspection — read-only

The read set comprised the four production `.cpp` modules and four public headers, both paper test files, CMake/workflow, `TEST_SEAMS.md`, `CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md`, production contract, nominal-scale/input-domain/oracle audits, relevant complete pinned OpenFHE/Boost files, paper, and first-run evidence. Inspection used read-only `cat`, `nl -ba`, `sed`, `rg`, and Python file reading. The exact evidentiary ranges and function locators supporting findings are cited in `DIAGNOSIS.md`; not every exploratory text-view command is retained as a shell transcript.

Files content search was attempted twice for the uploaded archive and returned no indexed content; extraction of the already-mounted ZIP was used instead. No Library/history search substituted for snapshot evidence.

The PDF instructions were read. All 15 pages of the supplied PDF were locally rendered with PyMuPDF at a 1.45 scale, to `/mnt/data/paper_renders/p01.png` through `p15.png`. Relevant equations/algorithms and experiment pages were visually inspected, including physical pages 4–9, 13–14. This is **not a claim that every rendered page was visually inspected**. Text was read directly from the supplied full paper text. No OCR was used. Input PDF SHA-256:

`61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`

A bounded external lookup of the official ePrint paper/landing page was also attempted. The landing page and a PDF text view were accessible; the requested web PDF screenshots failed due to access restrictions. Local rendering of the supplied, hash-bound PDF supplied the visual evidence instead. No remote repository, Actions log, code revision, or new external implementation was imported into this snapshot review. Unrelated search results were not used.

The initial source/log view and independent scalar reasoning were saved to `evidence/PRE_AUDIT_VIEW.md` before the supplied `FRESH_PROPAGATION_AUDIT.md` and its script were opened. The supplied audit was subsequently challenged and its calculations reproduced. This chronology is recorded honestly, without claiming independent-provider approval or tamper-proof timing.

## 4. Incoming hosted results — inspected, NOT rerun

Incoming `evidence/RUN_TERMINAL_01.json` identifies run 33971779479, attempt 1, exact head b1b024e, completed FAILURE on both jobs.

Linux job 101321455160: five API builds and 60 regressions succeeded; paper compilation failed on two array-bounds warnings promoted to errors; focused paper execution was skipped. Raw regression total: 2.41 seconds; this is an incoming log datum, not a local benchmark. [Incoming LINUX_RAW.log:5451–5497.]

Windows job 101321455226: five API builds, 60 regressions, and the paper build succeeded. The one focused paper process failed at round 4. The live `61:` interval is 7380–7508; later unprefixed output is CTest replay. Incoming regression total: 3.69 seconds; paper process reported 15.50 seconds. These are incidental retained timings, not performance reproduction. [Incoming WINDOWS_LF.log:5773–5775,7380–7509.]

No new hosted result is claimed. The existing failure is neither discarded nor converted to a pass.

## 5. Independent bounded scalar analysis — PASS

Executed:

```sh
python3 /mnt/data/precision_review/checks/analyze.py /mnt/data/input \
  > /mnt/data/precision_review/evidence/independent_scalar.json
```

**Exit 0.** This uses exact dyadic Fractions for the ten frozen inputs/derivatives and 120-decimal Decimal evaluation for propagation intervals, remainders, and reverse-triangle residual lower bounds. It includes a conservative relative allowance for printed norms and clearly does not implement certified interval arithmetic. It reads 60 distinct live numeric fields, including 50 fresh/round-1–4 anchor norms. Round-8 entries are explicitly counterfactual ideal propagation, not FHE observations.

No encryption or transform was performed. The output independently confirms the slot-512 round-4 inherited interval around `[1.49406650020095744e-24, 1.53821965758586599e-24]`, observed gate ratio 1.83736238570213248, and failed round-4 anchors 257/512/768.

After the independent view, executed the exact supplied scalar script:

```sh
python3 /mnt/data/input/evidence/fresh_propagation_audit.py \
  > /mnt/data/precision_review/evidence/supplied_audit_reexecution.txt
```

**Exit 0.** It identifies original log interval 7380–7508 and exactly 50 anchor observations. Its principal numbers agree with the independent calculation; the new calculation uses a tighter optional radius factor in the nonlinear remainder. The reproduced script's conclusions are not treated as an independent approval of the current patches.

## 6. Patch preparation — generated, then locally corrected

Preparation scripts were run as:

```sh
python3 /mnt/data/precision_review/checks/make_patches.py
python3 /mnt/data/precision_review/checks/make_probes.py
```

They write only review artifacts under `/mnt/data/precision_review`. They are authoring helpers with this review's fixed scratch paths, not project build tools or an instrumentation framework. The delivered unified diffs and complete files are the integration artifacts; those authoring scripts need not be run by the integrator.

During applicability validation, both standalone patches and DIAGNOSTIC-then-PORTABILITY applied, but **PORTABILITY-then-DIAGNOSTIC initially failed** because the diagnostic helper's insertion context overlapped the original `AnchorRoots` body. A second diagnostic run printed the failing hunk/context explicitly. This was a patch-layout problem, not a compiler or production failure.

The helper `ObserveCoefficientScale` was moved to an earlier unchanged location, before `RecombinedPolynomial`. No numerical behavior changed. Standard three-line diff context was retained. Both patches were regenerated and all four application cases subsequently passed; details are in §9.

The scalar probe generator was rerun after that final layout adjustment. SHA-256 comparisons confirmed the three probe sources remained **byte-identical** to the versions used by the compiler/scalar checks below. See `evidence/probe_source_identity.json`.

## 7. Isolated compiler/root check — limited local success

### 7.1 Interrupted first compiler batch

An initial Python subprocess batch attempted these probes in order: `roots_original`, `roots_portable`, `scalar_diagnostics`. Each compiler invocation had this shape:

```sh
g++ -std=c++17 -O3 -Wall -Wextra -Wpedantic -Werror \
  /mnt/data/precision_review/checks/NAME.cpp \
  -o /mnt/data/probe_bins/NAME
```

The inner `subprocess.run` used captured output and a 45-second timeout; the enclosing container call timed out before a complete batch ledger was saved. **Only the original compiler diagnostic text was retained.** `evidence/roots_original_compile.txt` contains GCC array-bounds errors in the original fixed-storage promoted conversion, ending with warnings treated as errors. The original invocation's numeric exit status and elapsed time were not retained. The batch does not establish completion of its later portable/scalar invocations. No probe executable was present immediately after the timeout; no lingering compiler process was found during the subsequent check.

This unsuccessful attempt is not reported as a completed compiler test suite. It motivated separately bounded, explicitly recorded invocations, not retries of any FHE experiment or random key.

### 7.2 Allocator-backed candidate compiled in isolation

Executed:

```sh
/usr/bin/time -f 'elapsed_seconds=%e maxrss_kb=%M' timeout 35s \
  g++ -std=c++17 -O3 -Wall -Wextra -Wpedantic -Werror \
  /mnt/data/precision_review/checks/roots_portable.cpp \
  -o /mnt/data/probe_bins/roots_portable \
  > /mnt/data/precision_review/evidence/roots_portable_compile.txt 2>&1
```

**Exit 0**, appended to the log. Recorded elapsed 20.32 seconds; maximum resident set 720,692 KB. These are incidental compiler resource records, not a benchmark or production CI timing.

The probe contains the exact candidate `AnchorRoots` body, original binary512 output type, and ten original anchor exponents, but **no OpenFHE include, key, ciphertext, NTT, FFT, or project target**. It also statically asserts that `Real` retains 512 binary digits.

Executed:

```sh
/mnt/data/probe_bins/roots_portable \
  > /mnt/data/precision_review/evidence/roots_portable_run.txt 2>&1
```

**Exit 0**, appended as `run_exit=0`. Ten candidate root pairs were compared with 160-decimal scalar sine/cosine references. Maximum component discrepancy:

`2.844188349377234960988017517547e-154`

The probe's deliberately strict check was `<2^-480`. Both numerical implementations use Boost but different precisions/representations; this is not a fully independent transcendental-library proof. It is useful local evidence that allocator-backed root temporaries do not reduce precision and avoid the local compile problem. The complete hosted GCC-13.3 paper build remains **NOT RUN**.

## 8. Isolated scalar diagnostic checks — PASS with intended failure exits

Executed:

```sh
/usr/bin/time -f 'elapsed_seconds=%e maxrss_kb=%M' timeout 35s \
  g++ -std=c++17 -O3 -Wall -Wextra -Wpedantic -Werror \
  /mnt/data/precision_review/checks/scalar_diagnostics.cpp \
  -o /mnt/data/probe_bins/scalar_diagnostics \
  > /mnt/data/precision_review/evidence/scalar_diagnostics_compile.txt 2>&1
```

**Exit 0**, appended to log. Elapsed 11.09 seconds; maximum resident set 603,772 KB. This probe extracts the exact new scalar residual/logging/failure-counter helpers, not the whole paper test. In particular it does not validate integration with OpenFHE headers or the production evaluator.

Executed once per deterministic mode, retaining each output:

```sh
/mnt/data/probe_bins/scalar_diagnostics algebra \
  > /mnt/data/precision_review/evidence/scalar_algebra.txt 2>&1
/mnt/data/probe_bins/scalar_diagnostics numeric-failure \
  > /mnt/data/precision_review/evidence/scalar_numeric-failure.txt 2>&1
/mnt/data/probe_bins/scalar_diagnostics nonfinite \
  > /mnt/data/precision_review/evidence/scalar_nonfinite.txt 2>&1
```

Recorded exits in `evidence/scalar_exits.txt`:

| Mode | Exit | Meaning |
|---|---:|---|
| algebra | 0 | Ten synthetic anchors, eight scalar rounds, signed E/I/A/L logged |
| numeric-failure | 1 | A finite acceptance miss increments the counter, reaches synthetic cleanup, then exits FAIL; no PASS marker |
| nonfinite | 1 | NaN aborts immediately; no synthetic cleanup marker and no continuation |

The algebra data is explicitly synthetic: start from `31/32 + slot*2^-20 + i/128`, inject a known fresh dyadic perturbation, then a known local perturbation at scalar round 3. It is not the frozen FHE experiment. Python exact-Fraction recomputation independently checks all **640 signed components** (10 anchors × 8 rounds × 4 residual kinds × 2 components). Maximum printed-scalar disagreement is approximately **4.40944684225965e-126**, below the independent check tolerance 2^-360. This scalar-check tolerance is not a replacement for any production acceptance gate.

The synthetic cleanup check demonstrates helper control flow, not actual key-cache destruction. Actual cleanup remains subject to the unchanged hosted test.

## 9. Final patch/static validation — PASS

Executed after the hunk-layout correction:

```sh
python3 /mnt/data/precision_review/checks/validate.py /mnt/data/input \
  > /mnt/data/precision_review/evidence/validation.json
```

**Exit 0.** The validator performs the exact scalar recomputation above, checks the intended finite/nonfinite failure outputs, and verifies byte-identical retained functions in the diagnostic-only candidate: input/scale generators, client-input conversion, sparse-secret reader/decryptor, recombination, AnchorRoots, Horner, evaluator, and major state/receipt/family/rejection checks. It verifies one `client.Encrypt` call, unchanged catch count, retained fail-fast codec/headroom/anchor-agreement predicates, and the final numeric-failure gate preceding COMPLETE PASS after cleanup.

It also rechecks all 133 incoming manifest payloads. This is source/static evidence, not proof that every runtime assertion has been exercised.

For each case below, the validator creates a disposable `/tmp/precision-apply-.../project` source copy. Each individual application executes:

```sh
git apply --check /mnt/data/precision_review/NAME.patch
git apply /mnt/data/precision_review/NAME.patch
```

All commands returned **0**:

| Case | Outcome |
|---|---|
| DIAGNOSTIC alone | Applies; exactly two test files differ; matches `complete/diagnostic/` |
| PORTABILITY alone | Applies; only oracle header differs; matches `complete/portability/` |
| DIAGNOSTIC then PORTABILITY | Applies; matches `complete/combined/` |
| PORTABILITY then DIAGNOSTIC | Applies; byte-identical combined result |

No `git init`, commit, remote operation, user checkout edit, workflow edit, or test-count change is part of these checks. The complete files are supplied so application can be audited independently of the diff tool.

## 10. Packaging and pending hosted validation

`MANIFEST.json` binds every other delivered file by path, byte count, and SHA-256, explicitly excluding itself. The output contains documentation, two patches, complete alternative changed-file sets, scalar/compiler check sources, and honest review evidence. It does **not** include probe binaries, OpenFHE binaries, keys, credentials, copied input archives, or an assertion of independent review approval. The final archive is CRC/manifest-closure checked before delivery; its outer byte count and SHA-256 are given in the final response.

**NOT RUN:** complete changed paper-target build; hosted Linux GCC 13.3 and Windows builds; the five API builds or 60 regressions after this patch; live 61-test listing after integration; any fresh encrypted chain; signed FHE residual observations; client round-5–8 or final full-slot/binder/witness/cleanup acceptance; any production-fix regression; security or tail certification.

Codex owns integrating/reviewing the candidate and the unique hosted push. The existing workflow's relevant commands remain unchanged, for example:

```sh
cmake --build build --target paper_full_eight_square_contract_test --parallel 2
ctest --test-dir build --show-only=json-v1
ctest --test-dir build --verbose --output-on-failure -R '^paper_full_eight_square_contract$'
```

These commands are **pending hosted steps**, not executed here and not instructions to perform heavy work on the Mac. Windows uses its workflow's `$build` path. No repeat-until-success flags, extra encrypted chains, or 1,000-trial gate are added. A future run that completes diagnostics but retains any original numeric miss must exit failure, and the original b1b024e first failure remains part of the evidence.
