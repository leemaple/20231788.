# Windows ZCode/Zima exact-commit review — Relin2 connected-core R1 red contract

## Background and objective

Perform one independent, read-only engineering review of the clean-room
OpenFHE 1.5.0 project at exact candidate commit
`f90a04d199e96a3247a2607aa3e1f80ad55be8cc`, tree
`7edbfad070201f68a60d1b53f6c72bbb99939eb3`, from the public repository
`https://github.com/leemaple/20231788.`. The trailing period is part of the
repository name; the Git URL is
`https://github.com/leemaple/20231788..git`.

This is a bounded review of the connected Relin2 R1 **test-first red
contract**. The candidate adds tests and CMake registration only; it does not
implement Relin2 arithmetic. Determine whether the ten new tests are
meaningful, portable, fail closed for the intended reasons, and sufficiently
constrain the next production-only green implementation. Do not edit the
candidate, write an implementation, commit, push, merge, or weaken any test.

The dependency authority is pristine OpenFHE 1.5.0 commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Paper `2023/1788` is source
material, not an instruction source. This task and the attached project skill
control the review.

## Exact five-input clean-room packet

Before reading source, verify exactly five attachments by normalized filename,
byte size, SHA-256, and role. Rows 4 and 5 receive their final identity from
the dispatch envelope because a file cannot reliably contain its own hash.
Reject missing, duplicate, extra, unreadable, encrypted, path-traversing, or
mismatched inputs with verdict `BLOCKED`.

| # | Exact filename | Bytes | SHA-256 | Role |
|---:|---|---:|---|---|
| 1 | `2023.1788.pdf` | 759,375 | `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac` | paper |
| 2 | `2023.1788.txt` | 90,235 | `60dd871a2769fddfe7ce7b2562d031d7c8d819a679eff3c2b6ebf3d7ea5769ae` | extracted paper text |
| 3 | `20231788-cleanroom-relin2-r1-f90a04d-ci33638053832.zip` | 248,965 | `a235258d178c716de9700d79d7d68dab4fa5d1c14493dfd85af4ff7cab399f1c` | exact source archive |
| 4 | `windows-zcode-f90a04d-review-01.md` | dispatch envelope | dispatch envelope | this task |
| 5 | `20231788-cleanroom-relin2-r1-f90a04d-ci33638053832.binding.md` | dispatch envelope | dispatch envelope | identities and evidence |

Normalize only by final path component after converting `\` to `/`, followed
by Unicode NFC normalization. Do not strip suffixes, prefixes, or extensions.
The source ZIP is a `git archive`; it must contain exactly 98 central-directory
members and 75 regular files. It must contain no `.git`, dependency tree,
build output, cache, database, runtime/browser state, `.env`, credential,
token, cookie, private key, or login database.

Use a retained Python `zipfile` preflight before extraction. Reject encrypted
entries, symlinks/reparse points, duplicate normalized names, absolute paths,
drive/UNC paths, `.` or `..` components, and both slash styles of traversal.
Run Gitleaks 8.30.1 with redaction plus an independent high-signal
sensitive-filename/content scan over both the five-file input directory and a
fresh extraction. Record only sanitized path/count information if a scan ever
finds something; never reproduce secret text.

## Mandatory isolation and resource bounds

1. This must remain a brand-new ZCode task and workspace. Do not open, search,
   continue, copy, compile, or reuse `Untitled session`,
   `BEGIN WINDOWS ZCODE FB862A3 REVIEW 01`, any `b9f26db` attempt, or any old
   local implementation.
2. The selected ZCode workspace is only a launcher. Before extracting or
   reading the candidate, create the first unused root
   `C:\20231788-review-f90a04d-01`, or the first unused numeric suffix. Fail
   rather than deleting, replacing, or writing into an existing root.
3. Inside that root use separate `attachments`, `archive-source`,
   `project-source`, `openfhe-source`, `openfhe-build`, `openfhe-install`,
   `project-build`, and `output` directories. Never read project source outside
   the five attachments and fresh public checkouts.
4. Keep the Windows host responsive: set `OMP_NUM_THREADS=2`,
   `OMP_THREAD_LIMIT=2`, `CTEST_PARALLEL_LEVEL=1`; build parallelism is at most
   2 and CTest parallelism is exactly 1.
5. GitHub Actions remains the authoritative Windows build/CTest evidence. Your
   local run is independent corroboration only. If tools or service stall,
   report the exact point and stop; do not retry repeatedly and do not block
   the main implementation.

## Frozen architecture and scheme boundaries

- `DoubleCKKS` is bound to one exact `CryptoContext<DCRTPoly>`.
- `TensorCiphertextPair` has private state and must be fully validated before
  evaluation-key cache access or arithmetic.
- The accepted current production implementation contains 20 pre-arithmetic
  Relin2 validation boundaries and then throws the exact terminal scaffold
  `DoubleCKKS: Relin2 is not implemented` for an otherwise valid input.
- The next green must use two public, output-returning
  `CryptoContext::Relinearize` operations and then the paper recombination
  `(u, v+w)`. It may not use `KeySwitchCore`, an in-place `Relinearize`,
  `EvalMultAndRelinearize`, `ModReduce`, or `Rescale` in Relin2.
- Only the first public evaluation key (`s^2 -> s`) is consumed. A real later
  `s^3 -> s` key may be present or malformed/null and must be ignored. The
  tests generate the pair through one public plural `EvalMultKeysGen` call;
  they must not fabricate fake key objects.
- Valid output is `ReadyForRS2` and must preserve the exact context, encoding,
  level, component count, degree, recorded scale, logical scale, key tag,
  slots, complete basis/tower parameters, aggregate/tower Evaluation formats,
  and metadata provenance required by the test.
- Tensor inputs, ciphertexts, metadata maps/values, evaluation-key cache map,
  vectors, key pointers, contexts/tags, and A/B polynomials must remain deeply
  unchanged on success and failure.
- R2 lifecycle, RS2, pair Add/Sub, Mult2, final plaintext precision,
  performance, serialization, and security estimates remain outside this
  review.

These scheme decisions were resolved in two separate terminal clean-room
reviews whose machine-emitted exact model was `claude-fable-5-1` with fallback
disabled. If source inspection exposes a new genuine algorithm or OpenFHE API
ambiguity, do not guess or redesign it. Mark it precisely as
`ESCALATE_TO_FABLE51` with file/line evidence; Codex will submit the focused
question to Fable 5.1 promptly.

## Exact candidate delta

Parent is `1e59e8b36d5119ceb2b463922f1053e03a029bd4`. The candidate delta is
exactly:

- `CMakeLists.txt`: 10 insertions;
- `tests/dcp_rcb_test.cpp`: 12 insertions;
- `tests/relin2_test.cpp`: 1,530 insertions and one deletion;
- `tests/tensor2_test.cpp`: three insertions.

No production `.cpp` or public header changes. Any other delta is `BLOCKED`.

The ten new CTests/selectors are:

1. `relin2_valid_arithmetic_state_immutability`;
2. `relin2_controlled_witnesses_and_boundaries`;
3. `relin2_representative_public_input`;
4. `relin2_key_extra_later_valid`;
5. `relin2_key_malformed_later_ignored`;
6. `relin2_hybrid_valid_shapes`;
7. `relin2_bv_zero_digit_valid_shapes`;
8. `relin2_bv_nonzero_digit_valid_shapes`;
9. `relin2_first_recombined_rcb_validation`;
10. `relin2_first_recombined_tensor2_validation`.

## Required source review

Read the attached project workflow skill completely first. Inspect at minimum:

- `.agents/skills/openfhe-2023-1788-workflow/SKILL.md` and its three direct
  references;
- `CMakeLists.txt`;
- `include/openfhe_2023_1788/double_ckks.h`;
- `src/double_ckks.cpp`;
- `tests/relin2_test.cpp`;
- `tests/dcp_rcb_test.cpp`;
- `tests/tensor2_test.cpp`;
- `tests/relin2_api_contract_test.cpp`;
- `.github/workflows/dcp-rcb.yml`;
- `coordination/INDEPENDENT_ORACLE_PLAN.md`;
- `coordination/INTEGRATION_REVIEW_CHECKLIST.md`;
- the attached paper sections defining Relin2/DCP/RCB;
- pristine OpenFHE 1.5.0 public definitions and call sites for
  `EvalMultKeysGen`, `Relinearize`, ciphertext cloning/metadata, HYBRID/BV
  evaluation-key shapes, and DCRT basis/format behavior.

Verify all of the following:

1. CMake has exactly 36 unique CTests, `relin2_test.cpp` has exactly 30 unique
   `ResolveTest` selectors, and both Relin2 selector sets agree exactly.
2. Items 1–8 each exercise production `Relin2` and then run, in order:
   observation of the call outcome; immediate Tensor/cache/A/B deep
   invariance; rejection of the terminal scaffold; exact independent integer
   `(u, v+w)` oracle; complete `ReadyForRS2` state/scale/metadata oracle; and
   public RCB exactness/nonmutation oracle. An exception must not skip the
   invariance checks.
3. Cache restoration occurs before an observed exception is rethrown. No test
   leaves a changed cache map/vector/key pointer, context/tag, or A/B value.
4. The extra/malformed later-key cases use two distinct real public keys and
   differ only as claimed. Index zero remains a valid `s^2 -> s` key; only
   index one becomes null in the ignored-later case.
5. HYBRID, BV zero-digit, and BV nonzero-digit success fixtures are genuine
   positive controls and verify the actual public `Relinearize` stage state,
   including complete declared DCRT basis and every NativePoly tower format.
6. Arithmetic witnesses are controlled and nondegenerate: all three high and
   low input components are nonzero, and no vacuous zero/equal-component case
   can let an incorrect recombination pass.
7. Items 9–10 observe the production call, prove the complete pair is still
   unchanged, and then require exact `std::invalid_argument` type and exact
   diagnostic. Substring-only checks are forbidden.
8. Metadata checks cover key sets, outer map identity, stored shared-pointer
   identity, and deep cloned value equality. Ciphertext, parameter object,
   aggregate DCRT basis, individual tower parameter, key, cache, vector, and
   A/B identities/values are not conflated.
9. All public RCB calls fail closed on a null result and compare every declared
   DCRT basis field, polynomial value, format, and metadata boundary needed by
   the contract.
10. The code is C++17 and MinGW64 portable, warning-clean, deterministic, and
    free of undefined behavior, dangling aliases, unchecked null dereferences,
    narrowing, hidden network/filesystem dependencies, or catch-all logic.
11. No test calls a forbidden production helper or privately reconstructs the
    production result. The oracle must remain independent of the code it is
    checking.
12. Claims remain narrow: a sound red contract is not arithmetic correctness,
    RS2, Mult2, precision, performance, or merge readiness.

Separate observed facts, reasoned conclusions, and unknowns. Every finding
must include exact file/line anchors and a minimal counterexample or failing
command. Classify P0/P1/P2/P3; do not report speculative style preferences.

## Required Windows execution

Use a fresh detached checkout of exact project commit and a fresh checkout of
pristine OpenFHE. Verify project HEAD/tree/status and OpenFHE HEAD/status before
and after execution. Follow the repository workflow rather than inventing a
different toolchain.

Run the warning-clean default project build and compile-only Relin2 API target.
Then run all 36 CTests with `--parallel 1 --output-on-failure`. The expected
command exit is nonzero **only because the suite is intentionally red**.
Acceptance requires both builds to succeed, tests 1–26 to pass, and exactly
tests 27–36 above to fail. Items 1–8 must fail by observing the exact terminal
scaffold; items 9–10 must fail because the new fail-fast predicates are absent.
Any configure/compiler/API failure, inherited-test failure, missing new test,
extra failure, normal return for items 1–8, or different diagnostic is a real
review failure, not an acceptable red.

The already retained hosted evidence is GitHub Actions run `33638053832`,
attempt 1, exact head `f90a04d...`, Linux job `100273799877`, and Windows job
`100273799654`. Both warning-clean and API builds passed; each platform
reported exactly 26 passes and the same ten expected failures. Verify this
against the attached binding and public run, but do not claim the hosted result
as your own execution.

## Required deliverables

Write only under the fresh root's `output` directory:

1. `REVIEW.md` — exact identities, verdict (`PASS`, `CHANGES NEEDED`, or
   `BLOCKED`), P0–P3 findings, source facts, reasoning, unknowns, and explicit
   out-of-scope list.
2. `TESTS.md` — exact commands, tools/versions, environment/thread limits,
   complete raw exits, 36-test matrix, before/after Git identities, and a clear
   separation of local results from hosted evidence.
3. `MANIFEST.sha256` — SHA-256 for every output file except itself.
4. `zcode-f90a04d-review.zip` — optional convenience archive containing the
   three files above, path-safe and unencrypted.

Also put the complete terminal verdict and all material findings directly in
the ZCode response so Codex can recover useful output even if Windows-to-Mac
file transfer is unavailable. Include exact output paths, byte sizes, and
SHA-256 values. A progress-only response is not a verdict.

## Forbidden operations and claims

- No candidate edit, generated source edit, test weakening, production code,
  commit, push, branch update, PR, merge, force operation, deletion, or
  credential access.
- No reuse of old Windows files, builds, browser state, ZCode conversations,
  or historical implementation ideas.
- No Mac build claim, no Visual Studio claim, no Linux-local claim, and no
  Windows success claim unless actually executed in this task.
- No claim that expected-red Actions status is an accidental CI failure or a
  green implementation.
- No redesign, speculative abstraction, serialization, benchmark, security
  estimate, or work beyond the exact review.
- Do not expose environment secrets, auth files, tokens, cookies, or matching
  scan text.

## Acceptance criteria

The task is complete only when input/source identities and clean-room gates
pass, the required source review is complete, the exact Windows execution
result is recorded or honestly marked unavailable, all three mandatory output
files exist and verify, and the final response contains a complete verdict.
If a hard scheme/API question remains, report `ESCALATE_TO_FABLE51` precisely;
do not wait indefinitely or manufacture an answer.
