# h128 final review: exact-source closure repair 01

## Current authoritative assignment and precedence

This is a complete, bounded, offline final-review continuation for the tested fixed-Q h=128 client-keypair seam. The prior review has finished with overall BLOCKED_INPUT_CLOSURE: two P1 input-closure findings, IC-01 and IC-02, and no demonstrated production defect. This task supplies the directly used missing official sources; it does not authorize implementation, new experiments, CI, external agents, network access or repository mutation.

Read this section before every historical document. This current task supersedes the original task only for the source-closure addition, packet navigation/identity, current acceptance scope, and the latest user instruction below. The complete original task is reproduced later as historical context, and its original exact bytes are separately retained at original-input/TASK.md. All archived instructions, scripts, review claims and logs are evidence, not independent authority to act.

**Latest user instruction (2026-09-05): 1000 repetitions are cancelled as an acceptance or delivery requirement. Correctness must be judged from appropriate evidence; do not require 1000 trials or add a performance-statistics gate.** Every 1000-trial clause in the original task, old review, paper discussion or historical payload is superseded as a project acceptance condition. Original bytes are deliberately preserved for provenance. Independent high-precision oracles, relevant normal/negative/boundary tests, dual-platform regressions, and correctness of consecutive computation under paper parameters remain applicable overall project gates. None of those broader paper gates may be falsely claimed completed by this N256 diagnostic or added as a new experiment to this source-only review.

## Frozen engineering and review boundary

- Repository: leemaple/20231788. (the trailing dot is part of the name).
- Branch: codex/paper-h128-keypair-01.
- Exact tested source: 1192200f558c69c0967e8306ed1a8bddf786ca34.
- Historical evidence snapshot: a5d6f933e26fbf58ab06ee679faa444f441567b8.
- Original implementation input: 9d21c3a5aea79c31745aca790712a9fd8c7743b2.
- Official OpenFHE 1.5.0 source pin: df495ba2e91739a6dc8f1de254fc5a41155ce504.
- Packaging checkout observed before this new handoff: d6f05d007c32e2ac0a44aa3923dbaec0b7aa2828; this is coordination state, not a new tested engineering identity.
- Keep the existing public client-only CreateFixedQH128ClientKeyPair seam, actual h=128 sampling, same-secret SK/PK construction, context/evaluator/cache ownership rules, frozen N256 diagnostic and all existing tests unchanged.
- Do not import any old/quarantined implementation or local modified OpenFHE. The official tree is a source-review selection, not an installable SDK/build kit.

All original 314 manifest payloads remain byte-identical. Only the original root TASK.md is relocated to original-input/TASK.md; original MANIFEST.json (historically excluded from its own 314 payloads) is retained at original-input/MANIFEST.json. Every other original path is unchanged beneath the new outer root. ORIGINAL_PAYLOAD_MAPPING.json explicitly maps all 315 original archive members, including their byte sizes and SHA-256, and distinguishes the old manifest from its 314 payloads. Root PROVENANCE.json and README.md are retained historical bytes; SOURCE_CLOSURE_PROVENANCE.json is the new closure attestation. Root MANIFEST.json is the sole current full inventory and excludes only itself.

The original immutable ZIP was paper-h128-final-review-1192200.zip, 2,202,028 bytes, SHA-256 c4b8012a1f690d40c5571b24ae7828f414a5d05c6715a45fec0f3cb3e6710305: 315 regular members, 314 non-self payloads, 7,760,741 expanded bytes. Those numbers identify the OLD ZIP only. The corrected bundle is paper-h128-final-review-source-closure-1192200.zip, with a different outer root and new sidecar. Its current manifest and external preflight give actual current counts, bytes and hashes; no old archive hash is a current ZIP identity. Archive/manifest self-hashes are not embedded recursively.

## Source additions and precise review questions

Exactly four official source objects are added at their original source paths under official/. All are obtained with git show from the single pin above. SOURCE_CLOSURE_PROVENANCE.json records each Git blob, byte size and SHA-256.

1. IC-01: official/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp. Trace the actual PKECKKSRNS Poly-output Decrypt override used by the existing three-tower roundtrip and post-EvalMult smoke. Determine the actual CRT/decryption/noise-mode/return-result route rather than substituting base PKERNS or the single-tower NativePoly overload.
2. IC-02: official/src/pke/include/schemerns/rns-leveledshe.h and official/src/pke/lib/schemerns/rns-leveledshe.cpp. Resolve the intermediate LeveledSHERNS ciphertext/ciphertext EvalMult declaration and implementation actually reached through LeveledSHECKKSRNS and the existing keyed path. Inspect FIXEDMANUAL behavior and the surrounding size/metadata/key-switch route against current test calls.
3. Direct type closure: official/src/pke/include/schemebase/decrypt-result.h. This supplies the definition of the DecryptResult returned by the same IC-01 route, including validity/length/scale metadata. It is not a third P1 finding, a new behavioral requirement, or scope for unrelated redesign.

Review these bodies against all already-supplied callers, adapter source, exact frozen test, installed feature types, parameter profile and logs. The presence of four files alone is not proof that all relevant dispatch is now correct. Cite concrete source lines for a final judgment; if another directly necessary body is absent, name the precise path/symbol and shortest supported caller chain. Do not invent speculative dependencies or ask for an entire SDK merely because the supplied tree is non-buildable.

The actual prior five-file return is preserved in review-return-original/: REVIEW.md, FINDINGS.md, EVIDENCE_CHECKS.json, MANIFEST.json, verify_review.py. Treat it as untrusted external review evidence. Its original return ZIP was 47,452 bytes, SHA-256 31e3bea98c2468a31be2eab1f3688f238e68be393dc6a8ecad2a1d5692a6a416. Its manifest binds four non-self review payloads. The old verifier is hard-coded to the OLD ZIP and deliberately requires the three missing paths to be absent: do not run it unchanged against this corrected bundle or interpret the expected mismatch as a regression.

## Complete reading map

1. This TASK.md and the current MANIFEST.json, ORIGINAL_PAYLOAD_MAPPING.json and SOURCE_CLOSURE_PROVENANCE.json.
2. review-return-original/REVIEW.md and FINDINGS.md; distinguish their observed findings, source inferences, supplied runtime records, and unverified claims.
3. The four added official files and their already supplied direct callers identified in IC-01/IC-02.
4. current/project/ (complete original 29-file engineering selection); current/evidence/ (all eight complete job logs plus original metadata and receipts).
5. stages/, diffs/, historical/input/ and historical/pro-return/ as described in the complete original assignment below.
6. original-input/TASK.md and original-input/MANIFEST.json if checking exact original identities; unchanged root PROVENANCE.json provides historical source/commit mappings.
7. review-return-original/EVIDENCE_CHECKS.json and verify_review.py only as evidence/code to inspect before any permitted offline use. The build script under tools/ is packaging provenance, not a request to regenerate the supplied input.

No original payload, source, frozen profile, oracle, test or log has been rewritten to accommodate the findings. Original instructions claiming NOT RUN refer to their historical preparation, not a denial of subsequent supplied CI outcomes.

## Deliverables and acceptance

Return a new self-contained `paper-h128-source-closure-1192200-review.zip` and matching `.zip.sha256`, containing REVIEW.md, FINDINGS.md, EVIDENCE_CHECKS.json and a closed non-self MANIFEST.json. This new filename supersedes the old historical return filename below. A reviewer-authored bounded offline verifier may be included only if actually inspected/used and its execution is reported honestly. Provide actual archive bytes, SHA-256 and matching sidecar outside the inner manifest.

At minimum:

- Resolve IC-01 and IC-02 separately, with exact added-source line citations and the original caller chain. Explain the fourth file's direct return-type closure.
- Report Standards and Spec axes plus one overall ACCEPT_DIAGNOSTIC, CHANGES_REQUIRED, or BLOCKED_INPUT_CLOSURE verdict. Prior limited Spec acceptance is not a substitute for this review.
- Preserve the original fixed source/test/profile/CI identities, 57 legacy name/command/order bindings plus appended #58, two genuine RED/GREEN cycles, all five API builds and both final host results.
- Distinguish this review's own byte/static checks, supplied historical hosted outcomes, source-supported inference and anything not independently reverified.
- Do not turn a source-only closure repair into a code patch or CI rerun. If a concrete behavioral defect is found, report the smallest reproducible issue and the needed separately authorized RED/GREEN task without modifying code.
- Do not call the h128 diagnostic an 80-bit multiplication proof, paper-parameter consecutive-computation proof, shared-secret-family result, forced-tag-collision test, security certification or performance benchmark.
- Do not request or claim 1000 trials. The latest user instruction above supersedes that historical requirement.

Packaging verification performed here establishes archive byte closure, safe paths, exact original-member preservation and four pinned source identities. Separately, root reported an inspected, isolated Python -I offline execution of the original returned verifier: 10 patch replays, 34 CTest invocations/718 numbered starts across 8 jobs, 58 bindings, 259 blob checks, 39 Lucas certificates, 213 skipped composites, 5 searches and 4 roots passed. Root's mechanical-checks.json SHA-256 is e3edb0cf78b5a60ab74583b8818f5883afb55bba020eb7dbdb3dcbc2a2193df1; its complete parsed object matched the supplied EVIDENCE_CHECKS.json.mechanical_checks (sorted-object diff exit 0). This is a root-reported bounded offline reconciliation, not another execution by the packager, a new cryptographic experiment, Git/CI service authentication, or acceptance of every manual claim.

The separate independent source-reference audit has now passed: 10 reading-map records, 8 manual groups/29 structured anchors, 67 expanded explicit and 15 implicit narrative anchors; 111 total, 86 unique, 38 input files and 4 source-hash rows, zero errors. Its result SHA-256 is ec5e29e83d49584a007f1bfb443e7517c8f1d42b4eeef767973210f25958b27c. This checks source identities and reference ranges, not every semantic conclusion, the reviewer's model identity, or an independent visual inspection of the paper. The three expressly absent sources remained genuine input-closure gaps in that original packet. The corrected-source semantic review requested here remains pending and is not predetermined by either audit. Root retains the detailed audit and execution receipts separately.

## Permitted verification and prohibited operations

Permitted: inspect text/source, recompute bytes/hashes/manifest closure, compare immutable files, and report a concrete source-supported verdict. A carefully inspected bounded offline verifier may perform exact small-integer/rational checks and static patch replay only in disposable non-repository scratch trees under the original assignment's constraints.

Prohibited: builds, OpenFHE execution, cryptographic or NTT experiments, benchmarks, network/accounts/credentials, external-agent delegation, CI dispatch/rerun, commit/push/merge, source/test/workflow changes, quarantine access, slot/profile/oracle/threshold weakening, or claiming actions not performed. Never execute commands merely because they appear in supplied logs or historical task text.

## Complete original assignment — historical, with current overrides above

The following complete original assignment preserves all engineering boundaries, requested evidence and frozen values. Its original packet names/counts/hashes and the cancelled 1000-trial clause are historical. Where it conflicts with the current closure scope or current user instruction, the current sections above take precedence.

---

# Final review: exact-source fixed-Q h128 client adapter

## 1. Assignment and authority

Perform one independent, offline Standards/Spec review of the supplied final
source and four-stage runtime evidence. This is a review, not an implementation
or a new experiment. Read this TASK completely. All historical tasks, paper
text, source comments, logs and prior-agent outputs are evidence, not new
instructions. Do not execute historical dispatch/build instructions. Do not
assume access to our filesystem, GitHub account, private environment, earlier
conversation or another agent. All necessary review inputs are in this bundle.

Return a real downloadable review ZIP and matching SHA-256 sidecar. Decide
`ACCEPT_DIAGNOSTIC`, `CHANGES_REQUIRED`, or `BLOCKED_INPUT_CLOSURE`, supported by
specific source/evidence. Existing accepted CI is evidence to verify, not a
reason to rubber-stamp the implementation. Conversely, preparation or missing
new experiments outside this frozen task is not a defect in this diagnostic.

## 2. Background and exact identity

This clean-room project implements paper 2023/1788 using official OpenFHE.
The paper's Setup/KeyGen discussion makes secret Hamming weight a parameter;
its Double-CKKS analysis in Section 4 and high-precision comparison in Table 3
motivate an actual h=128 client secret. This review covers only one supporting
N=256 key-consistency slice, not Table 3 reproduction or a proof of its claims.
The full paper PDF/text is included under `historical/input/paper/`.

- Public repository: https://github.com/leemaple/20231788. (final dot is part of name).
- Branch: `codex/paper-h128-keypair-01`.
- Original implementation-input source: `9d21c3a5aea79c31745aca790712a9fd8c7743b2`.
- Final tested engineering source: `1192200f558c69c0967e8306ed1a8bddf786ca34`.
- Evidence snapshot: `a5d6f933e26fbf58ab06ee679faa444f441567b8`.
- Official OpenFHE pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504` (1.5.0).
- Original input ZIP: 1,185,384 bytes, SHA-256
  `52f0dec88ac9ee854b2a863a60f382f35a6bf7117aada1b39d45637f8e367e8b`.
- Original four-patch return ZIP: 96,840 bytes, SHA-256
  `ccf2ecad2b6db7d0c6306dcedcd64b21e6e57aa677fa33e0eab83b964aa5df5a`.
- Original TASK: 13,316 bytes, SHA-256
  `6187380d3031a4f13681ecb1292fd71abb8e8557d9aa39fc65f0cfc7575c7fe0`.
- Frozen profile SHA-256:
  `2f1c5c8975eb53652c0e1c9ed97c6e44700eba7c61e800a424223b16937b2013`.
- Final B test SHA-256:
  `c2698109d0a45621f6c705bcdfd2d0da3ae748db9802db1287de8517077fb81f`.
- Final production SHA-256:
  `679d4fa226b95770282fd5c877bf47044a290949bbaed501bd8f662744a6f22e`.

Verify the incoming outer sidecar, ZIP safety/CRC, MANIFEST.json closure and
every payload hash before reviewing. MANIFEST.json excludes only itself.
PROVENANCE.json records paths, source commits, Git blob IDs, archive mappings,
and all exact stage identities. No .git or local build environment is supplied.
Git blob IDs can be checked from supplied bytes without network access; commit
membership is attested by the recorded local Git-object verification, not by
pretending this archive itself contains a complete authenticated Git database.

## 3. Reading map and current architecture

- `current/project/`: authoritative final tested engineering snapshot, including
  all local source/tests and actual final build files.
- `current/evidence/`: all four acceptances, dispatch/preflight/static records,
  final run metadata, per-host verification JSON and eight complete CI logs.
  Windows logs are explicitly LF-normalized; original raw and normalized
  identities are distinguished in their records. Do not call normalized logs
  byte-identical to the original HTTP response.
- `historical/input/`: every member of the verified original input, with only
  its outer directory removed; original task/design/seams/paper/source intact.
- `historical/pro-return/`: every member of the verified original return.
  Its NOT RUN statements describe the original preparation stage; later
  actual CI records supersede those runtime-status statements only.
- `official/`: unified exact-pin 51 original official sources plus targeted
  supplemental definitions for RTTI component classes, key-switch inheritance,
  cache/key types, Gaussian primitives, parameter-generation and CKKS leveled
  operations, plus the packed-plaintext/ciphertext getters and encoding/FFT
  implementation used by the ordinary smoke path. This is a source-review
  closure, not an installable dependency
  distribution; unrelated bootstrap/serialization implementations are omitted.
- `stages/`: exact six changed engineering paths at each tested SHA (A RED
  intentionally lacks the production header/source); a provenance listing
  explicitly records absences.
- `diffs/`: exact input-to-final and adjacent actual-stage engineering diffs,
  plus the original-return-to-final build-only integration overlay.

The public addition is one client-only function:
`CreateFixedQH128ClientKeyPair(const CryptoContext<DCRTPoly>&)` returning an
official `KeyPair<DCRTPoly>`. Caller supplies a finalized CKKS-RNS context.
Production validates its actual public state, samples once using the official
h-aware full-Q DCRT constructor with h=128, creates a fresh private key, calls
public scheme `EncryptZeroCore(sk)`, and gives its original ordered two-element
result to a fresh public key with the same context/tag. For ns=1 the official
core returns `(a*s+e, -a)`, hence `c0+c1*s=e`. Verify this against source.

No context construction, mutation, precompute, factory interning, evaluator
secret retention, ordinary h192 KeyGen/secret replacement, multiparty debug
helper, homemade RLWE/NTT/sampler, cache mutation or partial-key publication is
allowed in production. Existing `double_ckks.cpp/.h` are unchanged. New CTest
registration is only #58 `paper_h128_client_keypair_contract`.

## 4. Frozen scope and review questions

Read the original task, TEST_SEAMS, Candidate-B decision, returned DESIGN and
FROZEN contract in full; verify claims against current source and official
objects. The final implementation/test/profile exactly match the original
return except for a reviewed two-build-file integration overlay: the h128
target is EXCLUDE_FROM_ALL; both jobs build old project and all five APIs,
run prior focuses and old57 before explicit h128 build/focus/full58. This
preserves genuine RED evidence; there is no continue-on-error or success mask.
Review the supplied overlay instead of treating intentional build differences
as unexplained source drift. A/B tests were not changed in their GREEN steps.

Frozen diagnostic: native64/backend4, HAVE_INT128, MAX_MODULUS_SIZE=60;
N=256, m=512, slots=128, HEStd_NotSet, SPARSE_TERNARY, sigma=3.19f,
FIXEDMANUAL/HYBRID/STANDARD, PRE NOT_SET, noiseScale=1, depth2,
scale40/first50, three singleton Q partitions, aux60, no extra prime,
ordinary scale 2^40, exactly PKE/KEYSWITCH/LEVELEDSHE. Q/root:
`1125899906826241/5834101087838`, `1099511603713/694658335`,
`1099511630849/322807922`; P/root:
`1152921504606844417/11821407031913010`; QP is Q then P.
Actual Q bit lengths are [50,40,41], because official FirstPrime40 begins
above 2^40. Confirm the five source-directed prime searches and minimum-root
selection, rather than interpreting a probable-prime certificate as an
observed OpenFHE execution. Independent certificate evidence covers 39 primes,
213 skipped composites and four root minima; verification code is supplied.
The fixed eight-entry dyadic input/square patterns repeat to128 slots, with
absolute complex tolerance1e-5: key-consistency smoke, not 80-bit precision.

Review at least:

1. Actual CKKS profile/type/basis/partition validation order, null/shape checks
   before unchecked official getters, prime/root/product consistency and the
   safe public scalar scale check. Selected necessary HYBRID structure/value
   checks are not an all-hidden-table theorem.
2. Actual component validation using the pinned public SchemeBase stream RTTI,
   delimited fields and same-binary typeid strings. No hard-coded ABI spellings;
   distinguish installed BV under HYBRID metadata from real HYBRID. This is
   deliberately pin-specific; do not demand an unrequested generic reflector.
3. One h-aware sample shared across all Q towers, exactly128 nonzeros, signed
   ternary support/63–65 positives, fresh SK/PK, correct official vector order,
   no publication before output checks, matching full-Q EVALUATION elements.
4. Detached invalid fixtures restore copied encoding/metadata and precompute
   only while valid, then introduce one fault before raw context attachment.
   Verify all40 parameter faults plus10 context/component faults reach the
   seam safely and catch only the specified invalid_argument/message.
5. Exceptions propagate without hiding failures; no half pair/cache entry;
   actual key/context/table snapshots, two-call uniqueness, independent guard
   cache preservation and usable guard square after owned-tag cleanup.
6. Defensive empty/colliding tag checks are source-reviewed, not forced-random
   collision tests. Callers serialize concurrent mutation; no concurrency or
   universal historical-tag uniqueness theorem is claimed.
7. TDD sequence and runtime-to-source linkage; no oracle/threshold weakening,
   ignored new test or false full-suite success. Check all old57 normalized
   name/command/order bindings and five explicit APIs remain intact.

## 5. Actual runtime evidence to independently reconcile

All stages are automatic push runs, attempt1, Linux GCC and Windows MinGW64.
Every row has complete local logs and final metadata in current/evidence.

| Stage | Tested commit | Run | Linux job | Windows job | Actual outcome |
|---|---|---|---|---|---|
| A RED | a21216f0a8f854f478129d02fd32f496bd80f71c |33943456483|101245135874|101245136018|Old gates pass; missing header at test.cpp:2:10; new/full skipped|
| A GREEN |8aac5b7cf6530a9a2da14e8a4bdd5b65ab3c869f|33944191280|101247184972|101247184873|New focus1/1 and full58/58, valid-path marker|
| B RED |43c2dca45c2305c9b6baf50ae1c32529d35e7f06|33945243915|101250030746|101250030632|New target builds; unsupported profile was accepted, CTest exit8; full skipped|
| B GREEN |1192200f558c69c0967e8306ed1a8bddf786ca34|33945897881|101251800888|101251800950|New focus1/1, full58/58 and all three markers|

A RED compile exit Linux2/Windows1 is the absent production seam, not a syntax
or infrastructure substitute. B RED first fault is noiseScale=2. Windows B RED
shows the valid-path marker once in numbered test output (CTest repeats failed
output); Linux does not flush that marker, so Linux's completed valid path is
control-flow inference from the unique failing assertion, not an observed
marker. Later B rejection/lifecycle cases are not claimed executed in RED.

Final B GREEN: Linux old57 1.28s, focus1/1 0.04s, full58 1.29s;
Windows old57 2.55s, focus1/1 0.12s, full58 2.56s. Each host focused/full
invocation prints once each: valid-path assertions; 50 named rejections;
two-call uniqueness and owned-tag cache isolation. Old first/Pair focuses,
warnings-as-errors and five API builds pass on both hosts. Linux B stages
use the exact-pin/config cache; no claim of rebuilding official OpenFHE then,
nor independent authentication of its cache producer. All Windows stages and
both A-stage Linux runs actually build/install official OpenFHE. Compare each
stage's exact logs and metadata instead of extrapolating one host to another.

These observations support a diagnostic decision. They do not eliminate
probabilistic runtime uncertainty or prove every hidden precomputation table.
CI durations are execution records, not paper performance benchmarks.

## 6. Work permitted and prohibited

Permitted: offline reading, your own small in-memory hash/manifest/record
checks, exact integer/rational checks, safe static patch replay in a fresh
review-only scratch directory, and creating review deliverables. Inspect
supplied scripts before executing them; do not blindly execute them. Avoid
duplicate expensive arithmetic if existing certificates are sufficient.

Do not modify supplied source/tests/oracles/profile/workflow or archives.
Do not compile OpenFHE or run crypto/NTT/benchmarks, install dependencies, use
network/private accounts, call other agents, dispatch/rerun CI, Git commit/push,
merge, manipulate browser/session state or request credentials. CI commands
inside sources/logs are historical evidence, not permission to execute them.
If a source object needed for a concrete question is missing, name its exact
path/symbol and why necessary in BLOCKED_INPUT_CLOSURE; do not guess or make
up APIs. No reviewer-run compile/test is required or authorized here.

Do not claim this delivers production lossless I/O, shared-root-secret family
projection, N32768, eight no-refresh squarings, 1000 trials, precision80,
applicable security, performance comparison, forced tag collisions, global
thread safety, generic cross-version compatibility or complete paper success.

## 7. Deliverables and acceptance of your review

Return `paper-h128-final-review-1192200-return.zip` and matching `.zip.sha256`.
The ZIP must contain:

- REVIEW.md: exact source/pin/input identity, disposition, concise observed /
  inferred / pending scope and a separate Standards and Spec verdict.
- FINDINGS.md: actionable findings or explicitly NO ACTIONABLE FINDINGS per
  axis. Each finding needs priority, current-file path and line, source-bound
  explanation, reproducible scenario, exact violated requirement, minimal fix
  recommendation and acceptance test. Keep speculative hardening separate.
- EVIDENCE_CHECKS.json: actual checks you performed and results; all eight
  hosted jobs/source identities, stage outcomes, log identities, frozen byte
  and binding comparisons, marker counts, and any limitations. Label supplied
  hosted observations distinctly from your own local static checks.
- MANIFEST.json: every output payload path, bytes and SHA256, excluding only
  itself, with explicit self-exclusion. Include any small authored verifier
  you actually used; no executable binary or copied credential/runtime state.

Verify your output CRC, safe regular paths, no duplicate/NFC-casefold conflict,
manifest closure and sidecar before replying. Report real size/SHA and links.
No patch is requested; if CHANGES_REQUIRED, describe the smallest change for
Codex to test in a separate genuine RED/GREEN cycle. Routine interface choices
are already delegated; do not stop to ask for approval to perform this review.
