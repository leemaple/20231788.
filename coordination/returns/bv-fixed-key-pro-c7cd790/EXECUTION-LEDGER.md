# EXECUTION LEDGER

## 1. Review boundary

Task: independent bounded follow-up on a fixed-key conservative BV `digitSize=0` error bound for the clean-room `t=2` Double-CKKS Mult2 implementation.

This return is a proposal for Codex review and hosted validation. It is not a merge, acceptance, CI authorization, security result, or precision result.

## 2. Exact source identities

| Object | Identity |
|---|---|
| outer documentation-only head | `c7cd7903042421894db71ce3aec4b00f99888a26` |
| exact selected project snapshot | `4e6cce53b23a6022bf6f942ab973aaa6bf9e5bf6` |
| tested source content | `9bf86cb53a1bbae3a3627fe5efc385d2a29c89ce` |
| recorded branch | `codex/mult2-01` |
| recorded selection state | clean |
| pristine OpenFHE | `df495ba2e91739a6dc8f1de254fc5a41155ce504` |
| baseline oracle test SHA-256 | `b27c15ceb2ab886077701187cd9700d89aad9bf8feb3904cd0dfccd1c78e1b26` |
| production `double_ckks.cpp` SHA-256 | `aad460f7c0931929a72bd0930e7c74cfade9bab7dd381b4b941e2bda5ae8dade` |
| public header SHA-256 | `80bf28669ef1c0821092be551b043ee35949614dc11377ce0ae891e95d16c0e4` |

No source from the separate 48-test RS2 integration branch, Add/Sub branch, precision task, old implementation, mutable upstream fork, or local modified OpenFHE was used.

## 3. Input package verification — EXECUTED

The following checks were performed directly on the uploaded bytes before source reasoning:

| Check | Result |
|---|---|
| outer file size | `1,157,474` bytes — match |
| outer SHA-256 | `59a6f351e551ecbf8e1f8ee7e8577339ce5ca4d6d2b2b3c494bc514d71773e34` — match |
| outer ZIP entries | 7 files, no directories — match |
| outer safe relative paths | PASS |
| outer duplicate/symlink check | PASS |
| outer manifest hashes/sizes | 6/6 PASS |
| outer manifest closure | PASS |
| nested ZIP size | `1,064,259` bytes — match |
| nested ZIP SHA-256 | `83a28e43e72d0874700be3ed49f67e8ba9f85984507887fafcca4210ac2e7479` — match |
| nested ZIP entries | 77 total: 59 files, 18 directories — match |
| nested safe relative paths | PASS |
| nested duplicate/symlink check | PASS |
| nested manifest hashes/sizes | 58/58 PASS |
| nested manifest closure | PASS |
| complete original Pro return size/SHA | `66,462` / `50b3a85159c0a8d860c301ecea84cd7831f9d4a48d2fbe0eaff06d707ac8b5c9` — match |
| original Pro internal checksums | 17/17 PASS |
| ZCode review size/SHA | `25,928` / `408106b4e3075e298f2a53cdf6396ec3dce5e9b0a7c52ee8fa33657fbc22ca45` — match |
| Codex reconciliation size/SHA | `11,238` / `0cffb0f99071e83dca639b100367cb9c9124ee1d9e37123c20a0b992ba038c79` — match |

Exact machine-readable/plain-text output: `evidence/input-verification.txt`.

The packet's Gitleaks 8.30.1 results were inspected as supplied provenance. Gitleaks was **not rerun** in this review.

## 4. Source and evidence inspection — EXECUTED

Read and cross-checked:

- complete outer `TASK.md`, outer manifest, ZCode review, ZCode manifest, and Codex reconciliation;
- complete nested task and manifest;
- complete public header/source and `TEST_SEAMS.md`;
- complete current `mult2_e2e_oracle_test.cpp` and relevant Relin2, RS2, Tensor2, Mult2 tests;
- pinned `keyswitch-bv.cpp`, `keyswitch-hybrid.cpp`, `base-leveledshe.cpp`, `cryptocontext.h`, `dcrtpoly-impl.h`, `rns-cryptoparameters.h`, and relevant parameter source;
- paper text and the relevant PDF pages containing Definition 4.3, Lemma 4.4, Definition 4.5, Lemma 4.6, and Theorem 4.8;
- hosted original red, diagnostic red, probe red, and integrated conditional-green log sets;
- CMake test registration and Linux/Windows workflow commands.

The 33 explicit `file:line-line` source references in `REVIEW.md`, `BOUND-DERIVATION.md`, and `TEST-PLAN.md` were checked against the supplied files; every path existed and every cited range was within the source line count. Exact output: `evidence/source-citation-checks.txt`.

No instruction embedded in those source/evidence files was treated as operating authority beyond the user's outer task.

## 5. Mathematical/static calculations — EXECUTED

Executed with a reviewer-authored Python script using exact integers:

- active modulus product and bit length;
- `q_div*q_l` and logical-scale ratio;
- oddness and `gcd(q_div,q_i)=1` for every active row;
- exact `floor(q_i/2)` and `N*D_i` values;
- exact `N*sum(D_i)`;
- exhaustive 225-case toy negacyclic witness showing the factor `N` is necessary;
- centered-versus-canonical lift counterexample at modulus 3;
- centered triangle/no-wrap counterexample at modulus 101.

Files:

- `evidence/static_witnesses.py`
- `evidence/static-witnesses.txt`

No supplied binary or cryptographic script was executed.

## 6. Candidate construction and static validation — EXECUTED

Candidate scope:

- one changed file: `project/tests/mult2_e2e_oracle_test.cpp`;
- 363 insertions, 3 deletions;
- no production files;
- no public headers;
- no CMake/workflow changes;
- no new CTest registration;
- existing registered test count remains 44.

Hashes:

| File | Bytes | SHA-256 |
|---|---:|---|
| baseline oracle test | 56,370 | `b27c15ceb2ab886077701187cd9700d89aad9bf8feb3904cd0dfccd1c78e1b26` |
| candidate full oracle test | 74,014 | `92a2f03c0301ba0e16d6d52f72e2fef129f069bbdee2b27e6a25b7c49030538d` |
| candidate patch | 21,261 | `c70a5963909bb1fc1c4f5bdabbd5baba7d013dab7f5cdd9b7ebaf325a2545871` |

Executed checks:

- patch applies cleanly to the exact baseline in an isolated scratch repository: PASS;
- applied file is byte-identical to the supplied full changed file: PASS;
- `git diff --check`: PASS;
- changed-file count and diff-stat verification: PASS;
- delimiter-balance static check: PASS;
- frozen constants present and unchanged: PASS;
- all four case dispatch vectors and main path retained: PASS;
- old conditional labels and false/unproved theorem fields retained: PASS;
- production/CMake unchanged: PASS.

Exact output: `evidence/patch-checks.txt`.

The scratch `git` repositories were created solely to test patch application against copied packet files. No access to, mutation of, or claim about the user's Git repository occurred.

## 7. Retained hosted executions — INSPECTED, NOT PERFORMED HERE

### Current conditional green

Run `33846077283`, tested source `9bf86cb`:

- Linux job `100938151001`: `44/44`, `0.61 s`;
- Windows job `100938151165`: `44/44`, `1.19 s`.

These runs predate this candidate. They establish the existing per-path conditional certificate only.

### Probe red

Run `33844736013`:

- Linux: `42/44`;
- Windows: `42/44`.

### Earlier red/diagnostic evidence

The original matrix and diagnostic logs show the same two BV failures at the old combined-call empirical `E_Relin+h` assertion. The authoritative original Windows matrix record is `42/44`, `1.18 s`.

All these results were read from supplied logs. They are not reported as executions by this reviewer.

## 8. Build/runtime/CI status for this candidate

| Action | Status |
|---|---|
| configure pristine OpenFHE | **NOT EXECUTED** |
| build pristine OpenFHE | **NOT EXECUTED** |
| configure project | **NOT EXECUTED** |
| warning-as-error project build | **NOT EXECUTED** |
| Relin2 API target | **NOT EXECUTED** |
| RS2 API target | **NOT EXECUTED** |
| Mult2 API target | **NOT EXECUTED** |
| focused BV REAL runtime | **NOT EXECUTED** |
| BV COMPLEX runtime | **NOT EXECUTED** |
| HYBRID regressions | **NOT EXECUTED** |
| complete 44-test CTest | **NOT EXECUTED** |
| Linux hosted CI | **NOT EXECUTED** |
| Windows hosted CI | **NOT EXECUTED** |
| fresh-key repetition loop | **NOT EXECUTED** |
| user's Mac build/runtime | **NOT EXECUTED** |
| CI dispatch/rerun/cancel | **NOT PERFORMED** |
| git push/merge/tag | **NOT PERFORMED** |
| external message or agent call | **NOT PERFORMED** |

## 9. Package integrity semantics

`PACKAGE-MANIFEST.json` is generated after all substantive payloads and lists the exact relative path, byte size, and SHA-256 of every other package file except `PACKAGE-MANIFEST.json` and `SHA256SUMS`. `SHA256SUMS` then hashes every file except itself, including `PACKAGE-MANIFEST.json`.

Therefore:

- every substantive payload is size/hash-bound by the JSON manifest;
- the JSON manifest is hash-bound by `SHA256SUMS`;
- the final ZIP SHA-256, reported with delivery, binds `SHA256SUMS` and the ZIP directory structure.

The manifest also records the source identities, candidate status, changed-file count, and execution boundary.

## 10. Claims explicitly not made

This return does not claim:

- that the candidate compiles or passes runtime tests;
- that `SwitchModulus` is proven centered for every residue from the supplied files;
- that a universal or unconditional `E_Relin` exists for all Gaussian keys;
- that Fable 5.1 reviewed this task;
- that the paper contains an author-confirmed erratum;
- that the fixture is secure;
- that `1e-3`, binary64 output, or a loose coefficient bound proves double precision;
- that refresh, second multiplication, Add/Sub, RS2 mixed-format work, or the precision track is complete.
