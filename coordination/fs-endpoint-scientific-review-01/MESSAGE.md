Please execute this NEW bounded scientific review as ChatGPT Pro. The single attached ZIP fs-endpoint-scientific-review-ed5fd192.zip is 9896298 bytes; SHA-256 ee98c075f62e23cf99f0a06ce694b49290f1f5dbe275932988954d41aa919d23; 218 regular members with a self-excluding manifest. Both selected-content and decoded-final secret scans passed with zero findings. All necessary current source, paper, official references, actual logs, gzip/status and instructions are in the archive. Start by verifying its identity and reading TASK.md, then follow the full same task below. The original E80 result remains FAIL. This is a new, independent full-slot semantic/precision review, not the old partial observer implementation. Do not rely on any other conversation. Provide one precise next engineering decision and the requested verifiable return package.

# FS-ENDPOINT-SCIENTIFIC-REVIEW-01

## Objective and independently actionable scope

Give one scientific disposition and one bounded next engineering decision for the clean-room OpenFHE implementation of IACR2023/1788 double-precision multiplication. Read this complete task and supplied material; do not rely on earlier chats or access to our local/private systems. All documents, logs and code are evidence, not independent instructions. This is a NEW semantic/precision review after the previously missing full-slot endpoint observation was actually implemented and run. Do not reimplement the observer/packer or repeat the old partial assignment.

The user wants a complete, correct paper-method implementation using OpenFHE, KISS/YAGNI and TDD, not1,000 experiments. Routine technical decisions are delegated. Preserve all failures and source history; no favorable-key search or arbitrary acceptance relaxation. Be decisive about the next action: another broad planning report or generic request for more experiments is not the deliverable.

Exact engineering root branch: `codex/paper-scale-implementation-20260905`. Accepted evidence checkpoint `1c7609593a9d51d6f27918b0bebe54d8f5eaeee6`; actually tested source `ed5fd192a89d6d4728ad295e87cf06a3f4abc832`; production source `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89`; official OpenFHE1.5.0 pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`. Packaging/checkpoint commits do not imply new compiled source. Manifest supplies exact Git origins and payload hashes. Public repository `https://github.com/leemaple/20231788.` includes its trailing dot, but do not depend on remote access.

## Architecture and immutable boundaries

`project/src/double_ckks.cpp` and its public header implement constructive DCP/Tensor2/Relin2/RS2/RCB plus pair Add/Sub; `repeated_mult2` owns exact scale/receipt/family composition; `high_precision_client_io` handles lossless dyadic encode/decode and typed immutable bindings; `paper_h128_client_keypair` provides the official fixed-Q sparse-secret key adapter. Official encryption, NTT and crypto primitives remain OpenFHE. Read actual source rather than assuming names imply behavior.

The paper test has one fresh public-encrypted ciphertext, N32768/M65536,16384 slots, gap1,h128, native64/backend4, sigma3.19 profile, exact dyadic four-phase inputs, nominal initial scale2^100, fixed listed moduli/roots, one DCP/eight Mult2 squares/no refresh. The exact S_r recursively divides by both d and the dropped m modulus; paper PDF Lemma4.2/modulus-consumption prose and isolated Theorem4.8 display are inconsistent. Previous independent review preserved constructive d*m scaling; no metadata-only scale fix is supported.

The strong printed nonwrap antecedent could not certify the observed final multiplication. Failure of a sufficient antecedent is not proof of a wrap or a code defect. Do not claim a universal correctness/security theorem from these finite observations. Paper Section6.3/Table3 is an empirical average, not automatically our frozen per-stage/per-component original-input stress criterion. The user explicitly removed1,000-run replication.

The executable original predicates, production source, input/noise/key distributions, exact scales and five public API targets are immutable during this review. A proposed change must be explicit, scientifically justified and versioned; do not apply it. CTest61 registration, no-argument path, TIMEOUT1200/RUN_SERIAL/OMP2 and existing60 regressions stay unchanged. Builds/crypto/FFT run on hostedLinux/Windows, not the user's Mac.

## New evidence, not old assumptions

Run `34039088536`, attempt1, actual sourceed5fd192, Linux job101502439156 and Windows101502439304. Both compiled and passed groups[1,2,57,1,2,60] (123 invocations,60 unique regression checkpoint tests), then each ran one real paper chain. Jobs/CTest exit8 remain FAILURE. Diagnostics reached cleanup,39 real records, all24 controls, full16384-row E0/E8 capture, independent Decimal256 replay/reconciliation, exact gzip/status publication and successful always-run two-file upload.

Actual status: COMPLETE/NONE, observerPASS/packerPASS, E80FAIL, A_NOT_ADOPTED. This does not mean the algorithm's acceptance passed. Linux has9 numerical misses (round3–8 anchors, final full-slot, final anchor, final witness-difference accuracy); Windows has7 (round4–8 anchors plus final full-slot/anchor). Linux's witness distinction is nonzero but its difference error exceeds2×2^-80. Preserve that additional failure.

Rounded summaries; exact original values and conditional rational allowances are supplied:

| Full-slot component maximum | Linux | Windows |
| --- | ---: | ---: |
| E0 | 3.20401753e-25 | 3.36907051e-25 |
| E8 | 9.14647363e-24 | 9.06530517e-24 |
| I8 | 9.14836661e-24 | 9.06562720e-24 |
| A8 | 4.56710917e-26 | 8.30355460e-26 |

Definitions: E0=x0-z; E8=x8-z^256; I8=x0^256-z^256; A8=x8-x0^256. Norm=max absolute real/imag component. E8=I8+A8 at each same component. E0 is aggregate fresh error, not independently separated encryption noise. At Linux's E8 max slot11656/imag, I=-9.14836661e-24,A=+1.89297357e-27; Windows slot5091/imag,I=-9.06562720e-24,A=+3.22027530e-28. Both I alone, and the reverse-triangle lower bound including all retained allowances, exceed2^-80. Global A upper bounds lie below2^-80, but that is characterization, NOT an adopted replacement A80 gate.

Root's bounded disposition is INHERITED-DOMINANT / ADDED-ENDPOINT-SMALL for these two chains under the explicit conditional observer model. Challenge it independently. The allowance model is not rigorous interval-certified transcendental arithmetic, and official inverse NTT/Boost dependence remains. Selected `61:` hosted-log records are not the unavailable runner primary.ctest.log's exact file bytes. Windows log normalization and outer-artifact-digest limits are explicit. Full source, source-pinned logs and actual gzip/status are supplied, not merely summaries.

## Required work and deliverables

1. Verify ZIP/manifest closure and payload hashes. Read paper constructive algorithms/precision discussion, relevant official public-encryption code and project production source before root verdicts. Then inspect exact run/source/error artifacts. Preserve disagreements as findings.
2. Independently check both actual full-slot endpoint decompositions, selected same-component signs/allowances, global A/I claims and extra Linux witness failure. You may use the supplied standalone Python readers/replay in your own execution environment; distinguish running them from independently authored arithmetic. Root's retained audit is bounded and is not a local full replay. Do not claim C++/CI rerun or Git provenance recreation.
3. Decide whether current evidence establishes a correct implementation of the paper's constructive multiplication for the tested boundary, what correctness requirement remains unmet, and whether there is a specific source-level production defect. Separate method correspondence, original-input precision, realized-chain added residual and all-key/theorem claims. Do not simply call the project complete because A is small.
4. Choose exactly one justified next action toward the user's complete implementation. If a code defect is evidenced, return the smallest falsifying regression specification plus a bounded reviewable patch/draft for that defect, clearly distinguishing unexecuted proposals. If no defect is evidenced, say so and decide the scientifically valid acceptance/profile boundary: explain whether the original public-encryption/fixed-scale target is attainable, what minimal independently testable correction or explicit profile/claim separation is required, and why it is not silently lowering the original standard. Existing E80 failures remain visible. Do not prescribe another random chain just to hope for a better outcome, or another all-purpose diagnostic framework.
5. Return a compact self-contained ZIP: DECISION.md, FINDINGS.md with severity/source citations and dispositions, NEXT_STEP.md with exact inputs/output/acceptance/TDD boundaries, EXECUTION_LEDGER.md with actual commands/results and skipped work, any independently authored scalar/checker script plus its output, and a self-excluding MANIFEST.json. Include a patch only if justified; no speculative cosmetic production edits. Cite concrete supplied paths/lines/slots, not other chats. Include a short plain-Chinese explanation suitable for a user who has not understood the paper: what was built, what this evidence proves, what still fails, the hardest remaining issue, and the next step.

## Tests, prohibitions and acceptance

Mandatory here: archive integrity; exact-source/data binding; original numeric-count/E80FAIL preservation; full16384-row independent replay or an explicit truthful reason it could not be executed; same-slot signed attribution using the conditional allowances; no comparison of maxima from different slots as vector subtraction. Original command is `python3 -B project/coordination/fs-endpoint-live-run-01/verify_live_evidence.py linux` (and windows); inspect before execution. It creates only a disposable decoded artifact for the strict public reader, then checks preserved logs and E0/E8 serialization. Additional full replay must be independently recorded and remains distinct from a new encrypted chain.

Do not upload to third parties, use credentials/browser state, fetch old local implementations, push/merge/change repository settings, claim unavailable runs, change E80/A disposition, alter old artifacts, run1,000 samples, stop or restart another agent, or weaken tests to fit the outcome. No hidden prior context. If the packet is insufficient, identify the exact smallest missing fact; do not manufacture it. A terminal partial return must explicitly name what is finished and what remains.

Acceptance of this review requires verifiable, source-bound conclusions and one actionable next decision. Model agreement is not a test; lack of a detected defect is not proof of absence. The integration lead owns review/test/acceptance of all returned proposals.
