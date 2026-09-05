# Execution ledger — FS-RESIDUAL-ENDPOINT-01

## Scope of this ledger

This records source/byte/static/scalar work in the review container, separately from historical evidence and future hosted work. No C++/OpenFHE configuration or compilation, crypto, FFT, NTT, numeric codec, root evaluation, encrypted trial, CI dispatch, Git push/merge, account/quota call or missing GREEN implementation was executed.

The input was the confirmed mounted attachment `/mnt/data/fs-residual-endpoint-red-9f6c8eae-v2.zip`. Scratch extraction was under `/mnt/data/fs_review_work/`. Return construction is under `/mnt/data/fs_residual_endpoint_red_review_v1/`. These are review-container paths, not an assumed path on the user's computer.

## Performed operations and results

| Operation | Command/request and actual result |
|---|---|
| Attachment retrieval | Files semantic search for the supplied task/proposals, then a simpler exact task query scoped to the attachment: no indexed archive text. Used the already mounted path; no Library or prior-chat content was substituted. |
| Initial archive preflight | Own Python ZIP/hash/stat/manifest code in `/mnt/data/fs_review_work/verify_inputs.py`; outer and both nested identities, safe regular paths, case-unique names, CRC and payload hashes verified before inspection. No code from either nested ZIP was executed. |
| Authoritative repeatable preflight/scalar/patch audit | Exact successful command below; exit0, stdout in STATIC_SCALAR_STDOUT.txt, JSON in STATIC_SCALAR_RESULTS.json. |
| Patch generation | Own Python `difflib.unified_diff` over the exact supplied paper test plus the newly authored declaration/assertion fixture header. RED.patch and both complete files were written; BASE_HASHES.json contains base/result identities and43 unchanged-file identities. |
| Patch applicability | `git apply --check /mnt/data/fs_residual_endpoint_red_review_v1/RED.patch` in a fresh `tempfile.TemporaryDirectory` containing only the44 supplied project files: exit0, empty stdout/stderr. |
| Disposable application | `git apply /mnt/data/fs_residual_endpoint_red_review_v1/RED.patch` in that disposable tree: exit0, empty stdout/stderr. Complete result files equal delivered bytes; exactly2 changed/new paths and43 byte-identical old paths. No Git repository, commit, network or production tree was mutated. |
| Source/paper review | `cat`, `sed`, `nl`, `grep` and own text reading only. Complete required contracts/test/helper files and relevant implementations/dependencies inspected; exact citation aliases and line-range checks are supplied. Source reading is not a compilation result. |
| PDF render | `python /home/oai/skills/pdfs/scripts/render_pdf.py /mnt/data/fs_review_work/source/paper/PAPER-2023-1788.pdf --out_dir /mnt/data/fs_review_work/paper_render --dpi 130`: completed,15 rendered pages. Complete supplied text read; pages7,8,12,13 visually inspected. No OCR. |
| Supplementary PDF screenshot attempt | Public IACR PDF opened only for supplementary screenshot attempts; screenshots of zero-index pages6,7,12 failed403/restricted access. No public bytes were substituted for the supplied PDF or production/dependency source. The locally rendered supplied PDF governs the review. |
| Integer/Fraction checks | Exact powmod slot permutation, nine independent scales vs recurrence, conservative root/DFT/Horner/power majorant inequalities at512/768, formal terminal K cap, radius3/2 derivative inequality, synthetic K examples and integer half-even fixtures: passed. No trigonometric function, numeric codec, FFT/NTT or encrypted operation was called. |
| Historical log checks | Own parser reads primary `61:` records only: one BEGIN/COMPLETE FAIL,9 scale receipts,835 named OBS fields,7 E80 misses and cleanup per host; printed final E/gate ratios match the supplied values. No replay counted as another trial. |
| Historical returned checker | Its JSON/stdout size/hash checked only. `review_scalar_checks.py` was NOT run or imported. The historical Mac exit0 and byte-identical result remain attributed to the supplied receipt, not this author. |
| Packaging | Own standard-library hashing/ZIP construction, self-excluding manifest and postconstruction safe-path/CRC/exact-manifest verification. Outer return bytes/SHA are reported externally, not recursively in the manifest. |

Successful replayable command actually run:

```
python /mnt/data/fs_residual_endpoint_red_review_v1/validate_static_scalar.py \
  /mnt/data/fs-residual-endpoint-red-9f6c8eae-v2.zip \
  --output /mnt/data/fs_residual_endpoint_red_review_v1/STATIC_SCALAR_RESULTS.json \
  > /mnt/data/fs_residual_endpoint_red_review_v1/STATIC_SCALAR_STDOUT.txt
```

Actual stdout:

```
PASS_STATIC_SCALAR_ONLY: 3 archives; exact payload closure; scalar bounds; 2-path patch; no compiler/transform/crypto.
```

The delivered validator is this author's own code. It imports only standard-library modules, never project or input-archive code, and its sole subprocesses are the two recorded `git apply` commands. It does not implement the missing observer/formatter/packer. Python lexical checks are not a live C++/Python codec execution.

## Corrections during review, not hidden passes

The first run of the newly authored static validator exited1 with `ValueError: historical primary run inventory`: it looked for a literal `SCALE` prefix, whereas the source and actual log emit `RECEIPT operation=`. The parser was corrected after inspecting T:127–157 and the raw log. The successful delivered run requires exactly9 such primary receipts. This was a review-script parsing error, not a project failure or a new experiment. No result JSON was claimed from the failed run.

Two source-reading path guesses for headers omitted the supplied `openfhe_2023_1788/` subdirectory and returned missing-file errors; the correct visible paths were then read. A historical decision document was initially requested as `ACCEPTANCE_DRAFT.md`; the listed actual file is `ACCEPTANCE_ADJUDICATION_DRAFT.md`, which was then read. These were read-path corrections, not unresolved archive omissions or source substitutions. The public screenshot failures are recorded above rather than described as successful visual evidence.

## Directly checked identities and preservation

Input archive counts are12/154/9 regular members and11/153/8 non-self payloads. Every declared size/hash matched. Supplied source provenance remains a manifest assertion: no fresh Git object fetch/comparison was made against the documentation or production commit IDs.

Patch applicability was verified on exact engineering bytes. `src/`, `include/`, all old tests/API fixtures, original paper oracle, frozen contracts, CMake and workflow remain unchanged. There is no workflow hunk. There are61 unchanged CTest declarations; the existing normal name/argv/order/timeouts remain. All production and protected-file bytes are covered by BASE_HASHES.json and the validator's exhaustive comparison.

The PDF is759375 bytes, SHA `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`. Independent S8 numerator/denominator decimal lengths are7707/7677; the exact fraction ASCII hash is in STATIC_SCALAR_RESULTS.json. These are integer-derived checks, not transform results.

## Explicit NOT RUN / not established

C++ syntax checking, compiler/linker invocation, CMake configure, OpenFHE build, native tests, the new self-test, observer/table generation, Horner numeric execution, production codec, FHE/FFT/NTT, key generation, encryption, an additional chain, CI dispatch/rerun, Git commit/push/merge and any account/quota call: **NOT RUN**.

Actual hosted API RED: **NOT OBSERVED**. Expected link failure is a source-level prediction to be verified by Codex. Numerical GREEN, a complete full-slot live sidecar/status, a failure-preserving wrapper, and always-run upload implementation: **NOT AUTHORED / NOT EXECUTED**.

The supplied secret-scan receipts were inspected as historical provenance; no new Gitleaks or secret scanner was run or claimed. No claimed Fable call, separate-provider review, backend identity attestation, security proof, instantiated paper nonwrap theorem or original E80 acceptance is added. The conditional scalar majorants do not prove the Boost8u trig premise, and static source inspection does not prove hosted performance.

The final return ZIP receipt and MANIFEST.json bind the actual delivered files. They do not convert any NOT RUN row into a pass.
