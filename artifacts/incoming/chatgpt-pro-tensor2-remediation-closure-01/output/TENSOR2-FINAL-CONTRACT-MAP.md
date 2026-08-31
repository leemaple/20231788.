# Tensor2 final contract map

Exact reviewed source/test head: `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`  
Recomputed exported source tree: `759d5195739684748d5a9664edabe3fa719e1acf`  
Pristine OpenFHE 1.5.0: `df495ba2e91739a6dc8f1de254fc5a41155ce504`

Status vocabulary:

- **PASS** — supported by the supplied source/evidence and, where stated, independent local execution.
- **UNCERTAIN / UNVERIFIED BOUNDARY** — deliberately not proved by this clean package; not treated as a defect when outside the acceptance claim.
- **FAIL** — a current-head contract violation. There are no FAIL rows in this review.

| Contract area | Status | Exact-current basis | Evidence class / boundary |
|---|---|---|---|
| Input package identity | **PASS** | ZIP `9,334,115` bytes, SHA-256 `54c4ab7a...1832`, 2,310 entries; internal handoff/manifest hashes match; 2,037/2,037 manifest entries verify | Local byte/hash/archive checks + supplied binding |
| Exported current Git tree | **PASS** | Temporary Git indexing of `cleanroom-project/` recomputed tree `759d5195739684748d5a9664edabe3fa719e1acf` | Local byte-tree computation |
| Git object ancestry / no history rewrite | **UNCERTAIN / UNVERIFIED BOUNDARY** | Package deliberately excludes `.git`; exported histories/diffs and timestamps can be audited but cannot prove object ancestry | Explicit task boundary; not a merge-blocking finding |
| Definition 4.1 high output | **PASS** | `src/double_ckks.cpp:518` computes `EvalMultNoRelin(left.high,right.high)` | Current source + supplied paper |
| Definition 4.1 low output | **PASS** | `src/double_ckks.cpp:519-523` computes high-low and low-high then one cross-term `EvalAdd` | Current source + supplied paper |
| Low-low omission | **PASS** | Tensor2 has no `EvalMultNoRelin(left.low,right.low)`; independent test also rejects cross-plus-low-low | Current source + independent oracle test |
| Exactly three multiplications | **PASS** | Exactly three public `EvalMultNoRelin` calls at `src/double_ckks.cpp:518-520` | Current source |
| No relinearization / coefficient rescale / ModReduce in Tensor2 | **PASS** | Tensor2 body has none; only metadata setters normalize degree/factor | Current source |
| Active ordered RNS basis unchanged | **PASS** | Result validator requires the input active basis/tower parameters; no coefficient/tower operation occurs in Tensor2 | Current source + tests |
| High paper scale `H_out = H_1 * H_2` | **PASS** | `TensorScaleDescriptor` is computed from the two input high logical scales | Paper derivation + current source/test |
| Recombined paper scale `R_out = R_1 * R_2 / q_div` | **PASS** | Result descriptor uses both input recombined scales divided by the actual integer divisor | Paper derivation + current source/test |
| FIXEDMANUAL degree `3` | **PASS** | Module records `left.degree + right.degree - 1`; current first lifecycle is 2+2-1 | OpenFHE observation + bounded module-design inference + test |
| FIXEDMANUAL recorded factor `SF_1 * SF_2 / baseSF` | **PASS** | Tensor2 derives baseSF from `GetScalingFactorReal(0)` and applies metadata-only normalization | OpenFHE observation + bounded module-design inference + test |
| `q_div` kept distinct from `baseSF` | **PASS** | Paper descriptor uses actual integer divisor; OpenFHE metadata uses real baseSF; test explicitly requires they are not conflated | Current source/test + paper/OpenFHE separation |
| Distinct Tensor result state | **PASS** | `TensorCiphertextPair` is a separate final type; exactly three components per member are validated | Current header/source |
| Public result immutability | **PASS** | Private constructor, `DoubleCKKS` friendship only, read-only ciphertext getters; no mutable result construction seam | Current header |
| No speculative Tensor lifecycle | **PASS** | Tensor result type has no lifecycle enum/flag | Current header |
| Complete left validation before access/arithmetic | **PASS** | `ValidatePair(left)` precedes raw handle acquisition and all OpenFHE arithmetic | Current source |
| Complete right validation before access/arithmetic | **PASS** | `ValidatePair(right)` precedes raw handle acquisition and all OpenFHE arithmetic | Current source + right-manifest negative test |
| Mutual compatibility before access/arithmetic | **PASS** | `ValidateTensorCompatibility(left,right)` runs before handle acquisition | Current source + slots/key-tag negative tests |
| Key-tag ordering observable | **PASS** | Different-key-tag test requires project `invalid_argument`; OpenFHE `TypeCheck` would otherwise reject downstream with different attribution | Test + pristine OpenFHE `TypeCheck` source |
| Stable project exception attribution | **PASS** | Negative tests require `std::invalid_argument`, `DoubleCKKS: ` prefix, and field-specific text | Current tests |
| Independent coefficient oracle | **PASS** | Boost `cpp_int` schoolbook negacyclic oracle computes expected values without OpenFHE multiplication | Current test source |
| All output components/towers/coefficients checked | **PASS** | Oracle comparison iterates all three components, all active towers, and all coefficients for high and low | Current test source + local/hosted green |
| Negacyclic wrap witness | **PASS** | Fixture asserts `X^(N-1) * X = -1` modulo active tower | Current test source + green execution |
| Signed modular-wrap witness | **PASS** | Fixture uses signed product crossing active tower modulus | Current test source + green execution |
| Nonzero low-low omission witness | **PASS** | Independent low-low witness is `-323` at component 0 / tower 0 / coefficient 5; output is cross-only and not cross-plus-low-low | Current test source + green execution |
| Full input immutability | **PASS** | Tests snapshot ciphertext elements and deep metadata-map state for both inputs and compare after Tensor2 | Current test source + green execution |
| DCP/RCB regression boundary | **PASS** | Existing `dcp_rcb` test remains present and passes locally and on exact-current hosted Linux/Windows | Current tests + local/remote execution |
| Shared validator retains DCP predicates | **PASS** | Remediation changes only the DCP state-label argument; predicate set remains unchanged | Remediation diff + current source |
| Legacy DCP empty-key-tag diagnostic restored | **PASS** | Public DCP test requires `DCP input key tag does not match its pair state`; production now supplies state label `pair` | Test-only P3 red + one-label source fix + final green |
| P2 raw hosted evidence | **PASS** | All 12 intermediate raw run/jobs/Linux/Windows files hash-match retained mapping and support exact intended boundaries | Recomputed hashes + retained provider evidence |
| Compile-red boundary | **PASS** | Run `33425868973`: Linux production library builds, compile-only Tensor API target fails because public seam is absent; CTest not run; Windows cancelled | Retained raw provider evidence |
| Complete scaffold runtime-red boundary | **PASS** | Run `33426712752`: Linux strict build passes, DCP/RCB passes, five Tensor2 CTests separately fail on scaffold; Windows cancelled | Retained raw provider evidence |
| First implementation green | **PASS** | Run `33427271692`: Linux strict build and 6/6 CTest pass; Windows cancelled and not claimed | Retained raw provider evidence |
| P3 public-seam red before fix | **PASS** | Run `33436068864` at test-only head `9d1d10a...`: strict Linux build, only `dcp_rcb` fails with `ciphertext state`, five Tensor2 tests pass; Windows cancelled | Retained raw provider evidence + exported timestamps/history |
| Exact-current final CI binding | **PASS** | Run `33436252725` has `head_sha=fb862a3...`, `completed/success` | Retained raw provider run/jobs evidence |
| Exact-current Linux hosted gate | **PASS** | `ubuntu-24.04`, CMake 3.31.6, GCC 13.3.0, OpenFHE exact commit, project build, 6/6 CTest | Retained remote execution |
| Exact-current Windows hosted gate | **PASS** | Windows Server 2022 / MSYS2 MINGW64, CMake 4.4.2, GCC 16.2.0, OpenFHE exact commit, builds and 6/6 CTest | Retained remote execution |
| Independent local Linux gate | **PASS** | Debian GCC 14.2.0, CMake 3.31.6; pristine supplied OpenFHE built/installed; exact current project strict build and 6/6 CTest | Local execution; no local Windows claim |
| Hidden dependency / test backdoor | **PASS** | No new dependency, mutable constructor, friend test seam, or production setter found | Current source/diff review |
| Production exception recovery | **PASS** | No production `try`/`catch` added | Current source/diff review |
| Unsupported later-operation implementation | **PASS** | No Relin2/RS2/Mult2/Add/Sub/rotation/bootstrap/repeated-mult/t>2 implementation or scaffold added | Current source/diff review |
| Precision/performance claims | **OUT OF SCOPE / NOT CLAIMED** | No such execution or claim is used for this verdict | Task boundary |
| Separate Windows ZCode/Zima same-commit review | **PENDING EXTERNAL CONDITION** | Not supplied or run here | Required condition on `MERGEABLE`, not a current source defect |

## Final map disposition

There are no current-head contract failures in the bounded DCP/RCB + first-lifecycle Tensor2 slice. The previous P2 evidence gap and P3 diagnostic regression are both closed. The only intentionally unresolved verification boundary is Git object ancestry/history rewriting because `.git` is excluded, plus the separately required Windows ZCode/Zima same-commit review outside this review's execution.
