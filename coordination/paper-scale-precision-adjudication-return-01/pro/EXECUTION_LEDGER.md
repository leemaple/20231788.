# Execution ledger

Review date: 2026-09-06, Asia/Singapore. This is a record of this review environment, not hosted CI. The user's Mac, original local captures and original Git checkout were not accessed.

## 1. Input and authority checks — executed

| Check | Actual result |
|---|---|
| Mounted input | `/mnt/data/paper-scale-precision-adjudication-9f6c8eae.zip` |
| Outer size / SHA-256 | 2,046,500 bytes / `1584a5b7362c9568d3f8f7fa8acfea9f28a4a934cabe2dcdd283aa6f02e9b7da`; MATCH |
| ZIP CRC | PASS |
| Member closure/safety | 154 unique regular members, no traversal/absolute/backslash/drive paths or symlinks; PASS |
| Manifest identity | SHA-256 `d19303d2d6fd5364f14c3076ed2d3f306a6e90da4d11d257deab0e6b1e7e2c98`; MATCH |
| Manifest payloads | Exactly 153, every size/hash matched, manifest self-excluded; PASS |
| TASK-first boundary | TASK was the first payload read. Historical analyses were treated as evidence, not instructions. |
| Extraction | Fresh scratch directory `/mnt/data/review_input`; input archive itself not changed. |

Files was tried for the mounted archive; it returned no readable archive contents. Container ZIP inspection/extraction was then used. No Library search, personal memory, private repository or prior conversation was used to supply missing evidence.

Manifest origin fields and raw source/pin markers were inspected and checked for internal consistency. Hash closure does **not** independently authenticate a Git commit, prove the claimed cross-commit production identity, reproduce the original capture, or prove source correspondence to an externally fetched pristine checkout. The packet supplies 77 complete selected upstream source files, not an independently cloned full OpenFHE repository.

## 2. Source and paper review — performed

Read in full: the four current production `.cpp` modules and four matching public headers; both paper test/helper files; all six frozen requirement/seam/audit documents. Traced the actual official encryption, ternary-constructor default, integer decryption, rescale, integer multiplication, CRT/basis, HYBRID keyswitch and canonical-transform dependencies. Inspected the Boost allocator/fixed-storage representation relevant to the original compiler failure. CMake/workflow and legacy/API execution bindings were inspected for the relevant build/run boundaries; this is not a claim of a new exhaustive review of every old test body.

Read the full archived paper text, including its mathematical definitions, conditional correctness claims and experimental discussion. Rendered all 15 local PDF pages; visually inspected the decisive mathematical and empirical pages, including physical pages 7–9 and 13–14. PDF extraction/rendering is document inspection, not an FHE/FFT/NTT experiment.

The official public IACR paper page/PDF was consulted as a supplementary reference. Web screenshot attempts for the key PDF pages were rejected by the tool's restricted-URL handling; local archived-page renders were used instead. An alternative ACM retrieval returned HTTP 403. These retrieval limitations did not prevent inspection of the complete supplied PDF. No external source was used to override the frozen archive.

Formed a preliminary independent view from source, paper and raw logs before reading the previous diagnosis narratives; recorded it in the working directory. Subsequently compared against the supplied new acceptance/audit narratives, signed-error report, fresh-propagation audit and previous PRO_DIAGNOSIS. The ordering record is a review-work record, not a cryptographically attested timestamp. Agreement with those authors is not counted as another independent execution.

## 3. Independent log/scalar checks — executed, final exit 0

Reproduction command, using only Python's standard library:

```sh
python review_scalar_checks.py paper-scale-precision-adjudication-9f6c8eae.zip --output CHECK_RESULTS.json
```

The final delivered checker and JSON were generated in this environment. `CHECK_STDOUT.txt` contains its actual successful stdout. The checker does not extract/execute archive scripts, import the project, compile code, instantiate ciphertexts, run a transform, contact a service or change the input.

| Check | Linux | Windows |
|---|---:|---:|
| First live paper stream, physical raw lines | 7046–8024 | 7368–8346 |
| Live payload lines | 979 | 979 |
| Unique finite numeric fields | 835 | 835 |
| Exact rational scale receipts checked | 9 | 9 |
| Exact profile/root rows, two matching passes | 60 × 2 | 60 × 2 |
| Retained numeric misses | 7 | 7 |
| Supplied Start/argv/PASS bindings matched to raw | 123 | 123 |
| Final full E / original gate | 10.416873724120 | 10.262604700037 |
| Maximum signed identity discrepancy | 1.12085804e-124 | 1.11297614e-124 |
| Minimum same-anchor norm I/A over 80 stage/anchor pairs | 40.00094 | 34.31801 |

The five API build/link markers per host were separately checked at the raw locations recorded by LA/WA. Linux records `Built target`; Windows Ninja records `Linking CXX executable`. Supplied terminal metadata places the only failing step in each new job at the paper runtime, not these earlier build steps. The 123 PASS bindings also have 123 distinct raw result locations.

The 123 execution bindings are repeated groups 1,2,57,1,2,60, not a claim of 123 distinct regressions. Each referenced Start, command argv and Passed record was independently checked against the supplied raw line; the archived original local-Git frozen-inventory parser was not run. The 32 reported E/I/A/L maxima and maximizing anchors per host in the supplied signed JSON were independently matched to reconstructed raw signed vectors.

The CTest replay matched the live payload: Linux raw lines 8026–9004; Windows 8348–9327, excluding exactly the interleaved `Errors while running CTest` stderr line at Windows 8549. Replay is not counted as another chain.

Original evidence was also checked: no original Linux paper BEGIN and an actual array-bounds compile error; original Windows round-4 observations and no round-5 observation. Supplied terminal metadata consistently identifies original run 33971779479 and new run 33978202814, each push attempt 1 and failure, with the expected source SHAs and host jobs. These checks establish packet consistency, not fresh authentication of hosting state.

The Windows LF payload was mechanically expanded to CRLF: 924,947 bytes and SHA-256 `7f781be87251a85da24a8a51ff25b0176f3215789718f528d6472c49a89b27f8`, matching the supplied preflight's decoded-capture identity. This is explicitly a reconstruction from supplied LF, **not possession or reproduction of the unavailable original connector capture**.

Scalar work used exact Python integers/Fraction for the input, anchor powers and scales, and Decimal precision 190 for rounded-log calculations, moments and conditioning. It checked E=I+A, I/A from z+E0, L from consecutive observed values, aggregate maxima, exact scale recurrence and metadata, derivative bounds, noise moment estimates and the literal theorem-antecedent lower bound. These operations are not a cryptographic or codec run.

Three preliminary checker invocations stopped on this reviewer's parser assumptions: two CTest whitespace layouts, and one interleaved Windows stderr line. Those parser defects were corrected; no source evidence or FHE gate was altered. An earlier scratch scalar pass used printed near-unit w0 and therefore had larger serialization residuals; the delivered checker reconstructs x0=z+E0. The final successful checker was then extended with explicit supplied-signed-JSON/metadata/CRLF checks and rerun successfully. These are local audit-tool development runs, not additional encrypted trials. A subsequent ad hoc output-QA check first mishandled list-valued API line locations and then assumed the Linux build marker on Windows; both inspection assumptions were corrected, and the per-host build/link markers and unique PASS locations were checked successfully.

## 4. Explicitly NOT RUN / NOT CLAIMED

- No FHE, NTT, FFT, numeric codec, C++/OpenFHE compilation, benchmark, encrypted experiment, new key generation, CI dispatch/rerun or repository push/merge.
- No execution of `verify_signed_run.py`, `verify_signed_error.py`, `fresh_propagation_audit.py`, their original pinned local-capture/Git guards, or their claimed original clean HEAD. Their supplied results were analyzed as evidence only.
- No fresh gitleaks/decoded-secret scan. The packet reports zero findings; this review does not relabel that report as an independently rerun scan.
- No recovery of unlogged full-slot I/A for the original chain; no implementation or execution of FS-RESIDUAL-ENDPOINT-01.
- No threshold, sampler, sigma, initial scale, input, modulus chain, production/test source or frozen requirement patch; no acceptance-draft adoption.
- No all-key correctness, security level, formal certified numerical-library bound, universal nonwrap proof, or Section 6.3 empirical average reproduction claim.

## 5. Output verification

The delivered ZIP is a decision/specification package, not an implementation or hosted CI artifact. Its `MANIFEST.json` covers every other delivered payload by relative path, byte size and SHA-256 and explicitly excludes itself. All cited source paths and explicit line-range endpoints were checked for existence/range validity. Output CRC, unique regular paths, manifest closure and payload hashes were checked after packaging. The final response supplies the outer ZIP size/hash; a ZIP cannot contain its own nonrecursive outer hash.

The successful local audit status means the bounded checks above passed. **The original FHE E80 result remains FAIL.**
