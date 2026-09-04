# Execution Ledger — Final Pair Add/Sub Static Review

## Reviewer and boundary

- **Actual model:** GPT-5.6 Pro
- **Review date:** 2026-09-04, Asia/Singapore
- **Mode:** independent, bounded, static-only review of the supplied packet
- **Host:** Linux `6.18.35`, x86_64
- **Inspection tools used:** Python 3.13.5, Git 2.47.3, Info-ZIP 6.00, shell text/hash utilities
- **Present but deliberately not used for project execution:** CMake 3.31.6 and Debian C++ 14.2.0
- **Source under review:** manifest-identified tested source `d73824c2d382013c3aadbd7cb29c57008e839714`, branch `codex/integration-01`
- **Official OpenFHE pin:** `df495ba2e91739a6dc8f1de254fc5a41155ce504`

This reviewer did not dispatch another model or agent. No source, test, workflow, retained log, or manifest in the input packet was modified.

## Status vocabulary

- **EXECUTED-STATIC:** performed in this review environment without configuring, compiling, linking, running CTest, or executing cryptographic code.
- **SOURCE-FACT:** read directly from a hash-verified supplied source or document.
- **INFERENCE:** a conclusion derived from source facts; not an observed runtime result.
- **SUPPLIED-CI:** execution reported by the retained hosted logs/ledgers; owned by that CI, not by this reviewer.
- **NOT EXECUTED:** deliberately outside this review seat.
- **OPEN:** expressly unresolved by this Pair Add/Sub review.

## Executed static checks

| Check | Status | Result |
|---|---|---|
| Outer ZIP byte count and SHA-256 | EXECUTED-STATIC | `1305833` bytes; `e3dd499889e66a3406fa8ca755b559505db802c2d4cd7c8e1615d74900225fce`; exact match. |
| ZIP structural safety | EXECUTED-STATIC | 91 entries: 71 regular files and 20 directories; no duplicate names, absolute paths, `..` traversal, backslash/drive paths, symlinks, special files, or encrypted members. |
| Outer `SOURCE-MANIFEST.json` closure | EXECUTED-STATIC | Manifest is 13001 bytes, SHA-256 `5193f793959813a5561e93ee7201cc4124c9ac52609bc40792c64700ae1f0c85`; all 70 declared payload files match size and SHA-256; no declared file missing and no unlisted payload file. The manifest itself is the 71st regular member and intentionally does not list itself. |
| Complete original Pro return closure | EXECUTED-STATIC | `ORIGINAL-PAIR-PRO-RETURN.zip` is 212032 bytes, SHA-256 `735dea4e6c164ced95c2829ea8eb5316201eb900fd5d77b1aad171e94e2676c4`; 88 entries/66 regular files; no unsafe entry. Its four internal ledgers recomputed cleanly: 65/65, 12/12, 21/21, and 9/9. |
| Fresh official-reference provenance | EXECUTED-STATIC | Recomputed SHA-256 and Git blob object IDs for supplied `ciphertext.h`, `metadata.h`, `cryptoobject.h`, and `evalkeyrelin.h`; all match `OFFICIAL-REFERENCE-PROVENANCE.json`. Other paper/official inputs close through the outer manifest. No network fetch was used. |
| Paper equation inspection | EXECUTED-STATIC | Read supplied paper text for RNS ring arithmetic, Section 2.1 Add/Sub, and Section 4 componentwise pair Add/Sub. PDF identity was hash-verified; no OCR was used. |
| Production source trace | EXECUTED-STATIC | Inspected current public declarations, pair/ciphertext validators, Add, Sub, compatibility, Tensor2 lifecycle gate, RS2 post-rescale validation, Mult2 composition, and RCB. |
| Pair production delta check | EXECUTED-STATIC | Confirmed four header insertions and 106 source insertions. Reverse applicability to the selected current tree was checked. A throwaway copy was reverse-applied and forward-applied; resulting selected-file hashes returned exactly to the supplied state. The input tree remained unchanged. |
| Official operation/Clone trace | EXECUTED-STATIC | Inspected exact-pin `EvalAddCore`, `EvalSubCore`, DCRT `operator+=`/`operator-=`, `CiphertextImpl::CloneEmpty/Clone`, metadata-map type, context identity, and relinearization-key A/B accessors. |
| Complete Pair test inspection | EXECUTED-STATIC | Inspected independent CRT/modular expected-value code, controlled witnesses, public lifecycle fixtures, RCB oracle, provenance and mutation snapshots, key-cache window, compatibility/malformed rejection matrix, aliases, and dispatch. |
| Original-return versus integrated-test comparison | EXECUTED-STATIC | Compared the complete original return with current Pair files and integration ledger; verified the six stated integration corrections and that production arithmetic was not changed. |
| CMake registration parse | EXECUTED-STATIC | Found 15 executable targets and 53 unique CTest registrations. Every registered command names an existing target; every CMake-referenced source exists. Pair dispatcher arguments match the registered names. |
| Workflow parse | EXECUTED-STATIC | Parsed the supplied YAML and inspected both Linux and MinGW64 jobs. The exact OpenFHE pin, warning-as-error settings, full CTest invocation, and explicit Relin2/RS2/Mult2/Add/Sub API target builds are present on both hosts. |
| Retained combined-log parse | EXECUTED-STATIC | Extracted 53 unique CTest starts from each combined log; names and order match CMake exactly. Located the reported 53/53 summaries and all five mandatory API-target build completions on each host. This is log inspection, not execution. |
| Small independent integer sanity check | EXECUTED-STATIC | Used textbook integer arithmetic only, with `Q=35`, to confirm representative wrap/sign examples such as `17+1 -> -17`, `-17-1 -> 17`, and `17-(-1) -> -17` under the paper's centered convention. No project or OpenFHE code was invoked. |
| Post-review input rehash | EXECUTED-STATIC | Repeated outer archive, manifest payload, original-return archive, and original internal-ledger verification after inspection; all values remained identical and all mismatch lists remained empty. |

## Explicitly not executed

The following were not run in this review environment:

- OpenFHE or project CMake configuration;
- C++ compilation or linking;
- any API contract executable;
- CTest, unit tests, or regression tests;
- encryption, decryption, DCP, Tensor2, Relin2, RS2, Mult2, Add, Sub, or RCB;
- performance, precision, error-bound, security, or memory/concurrency experiments;
- installation or dependency changes;
- Git commit, branch, merge, push, tag, dispatch, rerun, cancellation, or repository mutation;
- GitHub API or web verification of run status/head SHA;
- execution of any bundled script or binary.

No supplied hosted result is represented as this reviewer's execution.

## Supplied hosted evidence inspected, not reproduced

| Boundary | Supplied source/run/job evidence | Retained result read from packet |
|---|---|---|
| Add runtime red | source `e22a2e1fb343731ca89cc0ea2e6444e7988bdc5e`, run `33839675559`; Linux `100919225214`, Windows `100919225336` | 41/42 on each; sole Pair Add runtime case failed at the throwing scaffold. |
| Add runtime green | source `a7681c2f02fe51dca80c9be51420788db9bde99c`, run `33839950608`; Linux `100920033485`, Windows `100920033616` | 42/42; 0.34 s and 0.98 s. |
| Sub API red | source `43bbe9d3ef0d4da262772b51a7b3b6a9102a5c14`, run `33841224322`; Linux `100923761068`, Windows `100923760882` | Existing runtime suite 42/42; explicit absent Sub API compilation failed as intended. |
| Sub API scaffold green | source `43f6c469896a7945456d15230e53dd1e03791b04`, run `33842361373`; Linux `100927075374`, Windows `100927075540` | 42/42 and explicit API targets passed. |
| Sub runtime red | source `934a0950790eeb872f2c4ecb22ab37d1ffeaaafa`, run `33842680856`; Linux `100928002005`, Windows `100928002268` | 42/43 on each; sole Sub runtime case failed at the missing implementation. |
| Sub runtime green | run `33842934325`; Linux `100928737964`, Windows `100928738258` | 43/43; 0.49 s and 0.99 s. |
| Exact controlled oracle | source `d4419afcd0818d3b122e91f57f0b1c43da8cbe32`, run `33843650508`; Linux `100930842605`, Windows `100930842388` | 44/44; 0.57 s and 1.35 s. |
| Public lifecycle/keyless matrix | source `af35784e540d556a3af231f8f1a7bd374c49649d`, run `33850393475`; Linux `100951617670`, Windows `100951617865` | 45/45; 0.53 s and 1.65 s. |
| Final separate Pair source | source `4b170183f29b415329c232a17ea1924acdd0d954`, run `33852796677`; Linux `100959175670`, Windows `100959175902` | 46/46; 0.52 s and 1.69 s. |
| RS2+Mult2 first parent | source `1e2487fb0539d4659e953ef232020bb800968f8e`, run `33851712076`; Linux `100955780944`, Windows `100955781223` | 48/48; 0.78 s and 1.34 s. |
| Exact combined source | source `d73824c2d382013c3aadbd7cb29c57008e839714`, run `33854419062`; Linux `100964299802`, Windows `100964299593` | 53/53; 0.68 s and 2.27 s; warning/default build and explicit Relin2/RS2/Mult2/Add/Sub API builds reported passed. |

The exact source/run/job associations above are source facts from the supplied ledgers and logs. This offline packet does not contain a Git object database, signed CI attestation, or live service response, so this reviewer did not independently authenticate GitHub's server-side state.

## Open items outside this review

- true high-precision input/output validation;
- repeated Pair multiplication and refresh strategy;
- conservative BV error bound;
- universal theorem or greater-than-53-bit precision claim for Mult2;
- paper-scale security and performance evaluation;
- complete project acceptance.

The output-file hashes are recorded in `MANIFEST.sha256`. That manifest excludes itself to avoid recursive hashing.
