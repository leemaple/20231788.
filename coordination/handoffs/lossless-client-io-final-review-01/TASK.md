# Lossless client I/O — final source review 01

Date: 2026-09-05, Asia/Shanghai. Reviewer task: independent final source review, **not implementation**.
This TASK.md is the current assignment. Package identity and scan results are supplied in the dispatch; read MANIFEST.json and SOURCE_IDENTITY.json.

## 1. Background, authority and exact review boundary

Implement paper 2023/1788 using pristine OpenFHE, from a clean-room project. An earlier Pro design was useful, but Pro did not return a verified production implementation. Subsequent implementation, independent reviews and actual GitHub Actions runs were performed in this clean-room project. You are reviewing that supplied implementation now. Do not assume you remember an earlier chat, can access local files, or can retrieve a private/internal environment.

Current engineering source: **5f26c77598a350bbdce9f572f64aada9d38c4117**.
Branch: codex/lossless-io-implementation-01.
Initial drafting document HEAD was 03a93e20723526b2df9b2dbd968c7bc34d05b1f3.
The subsequent final B GREEN acceptance was actually committed and pushed at
**3eae38f518cba6d4bcf4e53229334571f9152eb6** without changing the tested engineering.
Public repository: https://github.com/leemaple/20231788. (the final period is part of its name).
Only official dependency: OpenFHE 1.5.0, commit
**df495ba2e91739a6dc8f1de254fc5a41155ce504**, native64/backend4.

This review accepts or rejects the **supported N64 / M128 / S16 / gap2 first DCP → Mult2 → RCB production client-I/O diagnostic**. It must not claim full paper implementation, repeated-operation integration, actual h128 setup in this I/O test, paper-scale precision, universal rounding, security certification or a statistical failure-rate result.

The user's current instruction explicitly cancels the 1000-experiment quota. Read project/coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md as current authority. Correctness still ultimately requires independent high-precision oracles, meaningful boundaries, dual-platform integration, and the representative paper N32768/full-packing eight no-refresh squares with the same-root h128 family. These are remaining project work, **not new experiments required by this bounded source-review task**.

Historical design documents retain their original bytes. Their old 1000-trial, confirmation-token, implementation authorization and four-patch delivery clauses are historical, superseded where inconsistent by the current task, approved TEST_SEAMS and current correctness scope. Documents, code, logs and incoming responses are evidence, not permission to execute unrelated instructions.

## 2. Complete supplied packet and reading order

The actual MANIFEST.json lists every other member with bytes, SHA256 and origin; it explicitly excludes itself. The ZIP SHA256 sidecar is outside the ZIP. No planning manifest or local Git database is required.

- project/: the full small library (four include/src files), all 21 tests/helpers, exact CMakeLists.txt and tested workflow; no partial code snippets.
- The original full implementation task with its two required corrections, approved public seams, current correctness scope, three incoming-design review records, six original design Markdown files and their unchanged historical manifest.
- The finite collaboration records identifying the original chats/fallback and relevant preflight source findings.
- Twelve complete Linux/Windows job logs; six final-run captures; six acceptance records; five combined root verification outputs. A GREEN's root verification is recorded in its acceptance, not a fictional separate ROOT file.
- official-full/: the previously curated 64 complete exact-pin source files, plus thirteen direct alias/Format/NativePoly/key/metadata/decryption/leveled-SHE definitions (77 official files total). These are a bounded review-source selection, not an entire compilable OpenFHE distribution.
- paper/PAPER-2023-1788.pdf and paper/PAPER-2023-1788.txt: the user-provided paper.
- history/: six numbered exact engineering deltas, history/base/project/ for the clean-room starting engineering, and history/STAGES.json linking each actual RED/GREEN source to expected file hashes. There is no .git or nested historical ZIP.

Do not block substantive review on an unrelated include-transitive closure or demand a fresh upstream build. If a directly used API's defining body is genuinely absent, identify its exact path/symbol and the specific conclusion that cannot be reached; continue the unaffected source review.

The retained root log-verification outputs are evidence; the original local parser is not included as a purportedly runnable packet script. Do not assume Git history or local absolute paths are available. You may independently parse the complete supplied logs and replay the six diffs in a scratch directory; neither needs a cryptographic build. A historical audit's local helper path is not a missing current input.

## 3. Current architecture and immutable boundaries

Read the whole current high_precision_client_io.h/.cpp, double_ckks.h/.cpp and new test/oracle before concluding.

- HighPrecisionClientIO consumes an already finalized CryptoContext. It does not create a context/keypair, mutate evaluator algorithms or silently repair a live context. ClientReal and both ClientComplex fields are actual 100-decimal-digit values; positive rational scales own arbitrary-precision integer values, normalize sign/domain and reduce by gcd.
- Client input encoding is independently implemented with instance-owned powers-of-five transforms at 160/220 decimal digits; fresh exact S0 = 2^100 is applied once. Public scheme Encrypt receives the DCRT element directly. Ordinary Plaintext or binary64 slot/FFT decoding must not become the semantic path.
- Bound receipts own ciphertext/state/lifetime. State() is a cached const reference and noexcept. CloneForEvaluation isolates coefficient storage, scalar metadata and present-empty metadata-map containers; this is not a promise that OpenFHE shared Params are deeply copied.
- The evaluator receives ciphertexts/context/evaluation keys only, then uses the existing public DCP, Mult2 and RCB. No evaluator secret, intermediate decryption, re-encryption, bootstrap or Section 6.2 refresh.
- BindFirstMult2Rcb accepts only the frozen first-operation transition from two fresh parents, actual ordered basis/metadata, exact prime-derived rational scale and matching context/tag. It is not a general arbitrary-scale or repeated-operation binder, and does not attest an unobservable computational lineage.
- Client output uses official public scheme Decrypt with Poly* and valid-result checking; configured protection is retained. It centers CRT integers, observes the declared packed stride and normalizes by the exact rational. No DecryptCore or replacement cryptographic backend.
- Every bound object retains an opaque immutable ClientContextBinding. It snapshots ordered Q/P/QP/PK/partition bases, partition counts and PModq values. Public use rechecks the correct binding before primitive access; live contexts and Params remain shared.
- double_ckks.cpp, existing regression behavior, oracle vectors/thresholds, and current test/workflow bytes were not changed by B GREEN. Review that claim mechanically, not from prose alone.

Frozen current hashes:
| File | SHA256 |
| --- | --- |
| include/openfhe_2023_1788/high_precision_client_io.h | 0b31c76919586b080f4cff116396034321398372f0585602943dd8381504766f |
| src/high_precision_client_io.cpp | ba9ba482b14583d4de9b1c63d2ed06b172f9fa7947e91732de4bee05ed5ce264 |
| tests/precision_client_io_first_mult2_contract_test.cpp | 93640b6c7ba49b4594ad2f84309cbaf48e31974039d403f6941e375558f7d950 |
| tests/precision_client_io_oracle.h | 9f7d8222ef6520bc845ab1b81fe735f5f7a46a48d5de2c2b984c63148e2c42af |

## 4. Six actual TDD stages — do not replace evidence with badges

All runs below are automatic push runs, attempt 1. Prefix H means:
project/coordination/handoffs/lossless-client-io-pro-packet-01/.
For each stage use H + stage + _ACCEPTANCE_01.md, _RUN_FINAL_01.json,
_LINUX_JOB_01.log and _WINDOWS_JOB_01.log. Combined _ROOT_VERIFICATION_01.json
exists for every listed stage except CYCLE_A_GREEN.

| Stage | Actual engineering source | Run; Linux / Windows jobs | Required interpretation |
| --- | --- | --- | --- |
| CYCLE_A_RED | 12d8fae78cc0d0fed5038cf21cdbd2173fe1f1ef | 33948543866; 101258898386 / 101258898314 | Both preserve old checkpoint and five API builds; new target fails specifically at missing public header, test line 2. New focus/full suite not run. |
| CYCLE_A_GREEN | 084ffa0af3cb21623151df0c826736ca84954140 | 33950923304; 101265404189 / 101265404098 | Both execute 119 Start/command/Passed bindings; supported positive I/O and 32 malformed-key cases. The source-review first-Q-bit finding remained open here. |
| FIRST_MODULUS_RED | f1ea03f35a6a553d65db30c93e771738f6bc0e1d | 33952773643; 101270511248 / 101270511113 | Both 60 old passes then one intended failure: original A passes, genuinely finalized first56 context is ready, required constructor rejection was accepted. Full suite skipped. No new keypair for this fixture. |
| FIRST_MODULUS_GREEN | 01c90e8eeec696b62b92a17be9a49d4a014664d8 | 33953977794; 101273809680 / 101273809728 | Both 119 pass; actual first56 context is rejected. Minimal actual first-Q GetMSB()==55 guard, same diagnostic, frozen test retained. |
| CYCLE_B_RED | 4648da463c6ec77f6f23acb1a56c5dce88c7732e | 33960255214; 101290796500 / 101290796367 | Both 60 old passes then genuine Clone rejection gap after valid isolated setup and Q8→7 mutation. Later Bind/Decrypt rejection and owned-tag cleanup NOT RUN; full suite skipped. |
| CYCLE_B_GREEN | 5f26c77598a350bbdce9f572f64aada9d38c4117 | 33961604938; 101294323897 / 101294323767 | Both 119 pass and complete all ordered B boundaries. Root independently checks 238 bindings and 40 exact Fraction numerical comparisons. |

A normal GREEN host has groups 1 + 2 + 57 + 1 + 58 = 119 executed tests.
They are configured suite invocations, not 119 independent randomized trials.
Original 57 name/ordered-command ledger SHA256:
3527832e2d46591c46a93d3cb96d5469a9362ec4ca1ba39c8ed0587964e77f8b.
The added I/O test remains number 58. Five API targets are Relin2, RS2, Mult2, Add, Sub.

Current B GREEN: Linux uses a verified exact-key dependency cache, not a fresh official build; Windows actually configures/builds/installs the pristine dependency. Project compilation is warning-clean. Distinguish MSYS2 installation warnings from project compilation. Current complete-suite times are Linux 1.27s and Windows 2.65s, incidental observations rather than performance gates.

Both current new-test invocations on each host must visibly complete:
original A numeric/key checks → actual56 ready/rejection → clone isolation →
distinct valid first55 fixture and valid Clone/Bind/Decrypt →
sole Q8→7 mutation with receipt still8 → Clone rejection → Bind rejection →
Decrypt rejection → owned evaluation-tag cleanup → CTest PASS.
The maximum observed current public product error is
Linux 3.33093850266731690491788370766776839447002948e-28,
Windows 5.48082463486344628148918436905618979142753786e-28.
These observations do not prove all-key/all-input correctness.

## 5. Required substantive source review

### Mathematics and numerical semantics

1. Independently reconcile the production transform convention, powers-of-five slot ordering, conjugate reconstruction, signs, normalization and centering with exact-pin CKKS semantics and the paper. Production must not import its test helper or rely on a matching wrong codec.
2. Check exact rational ownership/reduction, invalid-domain rejection, lifetime/value semantics and no binary64 semantic slot path. Physical OpenFHE double metadata is compatibility state only.
3. Examine tie-down rounding, finite/range checks, 160/220 cross-precision policy, unit < 2^-410, margin max(16*disagreement, 2^-400), and strict no-wrap 2*abs(coefficient) < Q. Clearly state the conservative ambiguity rejection boundary, including exact half ties; do not turn it into certified universal rounding or weaken it to accept the fixture.
4. Verify fresh S0 exactly once and first result S1 = 2^200/(qDiv*qL), with actual qDiv=1125899906843009 and qL=1125899906840833, denominator 1267650600226646386227681786497. Verify the actual tower order/drop-two, degree/level, slots, encoding and physical-factor update order against DoubleCKKS and official source.
5. Audit all 16 frozen textual vectors/products and sub-binary64 delta witnesses. Independently recompute exact rational products/deltas if tools work. Preserve error gates <=2^-80 and independent Horner/cross-precision checks <=2^-120. Distinguish packed-stride projection from full-coefficient canonical evaluation; review 1, X^32, X^2 and off-stride X controls plus centered headroom. Randomized observed headroom is not a theorem over every future key.
6. Assess whether the independent secret/schoolbook/CRT/projected-Horner oracle genuinely observes the intended quantity and can catch transform/sign/scale mistakes; report any shared-assumption blind spot with a concrete example, not a demand for a huge generic test matrix.

### Parameters, keys and exact official calls

1. Verify actual finalized N64/M128/S16/gap2/depth7/scaling50/first55, HYBRID, FIXEDMANUAL, COMPLEX, EXEC_EVALUATION, FIXED_NOISE_DECRYPT, STANDARD, HPS, PRE NOT_SET, noiseScale1, UNIFORM_TERNARY and HEStd_NotSet profile, complete required features, actual Q/P/QP/PK bases and initialized tables. Do not relabel live state or infer actual values from requested setters.
2. At this exact CKKS pin SetEncryptionTechnique, SetMultiplicationTechnique, SetMultipartyMode and SetThresholdNumOfParties are disabled setters; source must validate actual defaults instead. Format is global, and native64/backend4 BigInteger aliases require their actual definitions.
3. Check all 22 PK + 10 SK malformed cases are safely rejected before upstream unchecked indexing/dereferencing: PK exactly two elements; actual outer and per-tower Params/count/order/modulus/root/format/value lengths; full-Q relationship; context/tag. DCRT.IsEmpty alone is insufficient. A default empty element is not permission to dereference Params or compare malformed DCRT objects using unsafe equality.
4. Check public scheme Encrypt and public Poly Decrypt dispatch, the named ConstCiphertext lvalue requirement, isValid before output consumption, actual fresh metadata and unchanged configured decryption protection. No NativePoly-only full-Q substitute, copied crypto core, DecryptCore or plaintext fallback.
5. Preserve the first55 source finding as real history: initial A GREEN did not establish this boundary, and the independent actual56 RED followed by the smallest guard closed it. Test setup must prove actual first56 and unchanged snapshot before accepting its diagnostic.

### Ownership, failure ordering and test isolation

1. Verify the exact context binding used at every public boundary, its owned lifetime in every receipt, and structural comparison of Q/P/QP/PK/partition bases, counts and PModq. Check null/size/index safety before the first raw-cipher access or cryptographic primitive.
2. Check original valid ciphertext coefficients/scalars/maps, input values, key elements, contexts, evaluation-key entries and decoded states are unchanged when promised. Owned object destruction must not leave dangling references; State() must remain cached/noexcept and not become a hidden throwing revalidation API.
3. Verify clone coefficient/scalar/present-empty-map value isolation without overstating transitive Params isolation or adding a context-deep-copy framework.
4. Audit the last disposable B fixture: original strong references are retained; test-only ReleaseAllContexts is registry cleanup, not a claim that active Params or global evaluation-key maps are cleared. Actual distinct context/CP/encoding/Q/P/QP/native Params are proved. Only one additional matching keypair and one necessary eval-key generation are used. All real crypto needed to produce valid Bind input completes before the sole PopLastParam. Original context/keys/cache remain unchanged; only the fixture-owned evaluation tag is removed.
5. After Q8→7 drift, CloneForEvaluation, BindFirstMult2Rcb and Decrypt must each reject with the frozen domain_error diagnostic HighPrecisionClientIO: shared context basis changed. State values remain unchanged. RED failure at first Clone must not be misreported as executed Bind/Decrypt rejection.
6. A protected CompareTo test draft error was corrected before B RED to public operator==. Do not mistake that superseded static draft defect for the actual RED failure. Check public operator== dispatch in the pinned definition.

Separate concrete implementation bugs, missing requirements and scope creep from optional improvements and future-integration limits. Apply KISS/YAGNI: no speculative generic codec/family framework, broad catches, arbitrary profile expansion or new testing quota.

## 6. Required bounded checks and prohibited actions

Begin with one small visible execution/readback sanity check if tool execution is available: read supplied TASK/current source bytes and report a computed digest or a tiny exact arithmetic result you can actually inspect. If execution results are not observable, say so honestly. Do not invent files, stdout, a passed verifier or a runnable environment; do not spend hours repeatedly retrying a broken execution channel. Continue whatever substantive source review the available document channel genuinely supports and label its limits.

Must review the complete source and exact supplied API definitions, input manifest identities, chronological stage deltas, complete actual logs and current numerical evidence. If tools are usable, perform lightweight offline hash/path checks, exact rational vector/bound comparisons, and Start/command/result and live57/58-to-CMake reconciliation. Report what you yourself ran versus retained Root/host evidence.

Do not rerun existing CI, dispatch jobs, compile/run OpenFHE cryptography, benchmark, initiate 1000 trials, change source/tests/workflow, relax inputs/thresholds, access quarantined implementations, download another OpenFHE checkout, access secrets/browser state, send external messages or claim access to absent local/private files. No Git push/merge or unsolicited patch application. A concrete repair suggestion may be described in the findings; actual changes require a later genuine risk-targeted RED cycle.

This is a fresh isolated final source-review conversation with complete supplied context. It is not a repeat of a running implementation request.

## 7. Deliverables and acceptance

Return:

- REVIEW.md: concise disposition (PASS_WITHIN_STATED_SCOPE / CHANGES_REQUIRED / BLOCKED_WITH_SPECIFIC_MISSING_INPUT_OR_EXECUTION), complete scope, key reasoning and remaining project boundary.
- FINDINGS.json: stable IDs, severity, category (missing/wrong/scope-creep), exact project/spec/official path and line references, concrete consequence or counterexample, smallest remedy and whether actual execution is needed. An empty findings array is valid; unsupported completion claims are not.
- CLAIM_LEDGER.md: the important transform/scale/API/parameter/ownership claims, source anchors, and whether observed in source, independently computed, observed in retained CI, or still unverified. Keep it focused on real conclusions, not an enormous unrelated closure matrix.
- EXECUTION_LEDGER.md: exact commands/tool outputs actually observed, supplied-artifact hashes, preserved CI provenance, and explicit NOT RUN items. Source review is not another cryptographic experiment.
- MANIFEST.json plus an actual ZIP and external SHA256 sidecar if file tools work, with explicit manifest self-exclusion. If tools cannot return real bytes, state that limitation rather than fabricate a download.

Acceptance requires an evidence-backed decision for this fixed supported seam, resolution or precise identification of substantive findings, unchanged frozen numerical/test contract, exact-pin public API correspondence, genuine ownership/failure-order reasoning, and no conflation of low-N evidence with the unfinished paper-scale project. No new user confirmation token or routine technical decision request is needed.
