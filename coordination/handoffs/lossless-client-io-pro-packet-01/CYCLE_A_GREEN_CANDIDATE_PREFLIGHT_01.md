# I/O Cycle-A candidate preflight — not GREEN acceptance

2026-09-05, Asia/Shanghai. Candidate engineering commit
`084ffa0af3cb21623151df0c826736ca84954140`, parent
`46568f52d0e47540a9093221e3b5a64904036a50`, branch
`codex/lossless-io-implementation-01`. The actual dual-host RED at
`12d8fae78cc0d0fed5038cf21cdbd2173fe1f1ef` was accepted and published at
`1e1b6339d3c24de6b675958f4de279fe29846fab` before production authoring.

Only the new public header, production source, and one CMake target_sources
line differ in the engineering paths. All tests, oracle, existing production
and workflow are byte-identical to the actual RED. The worktree was clean
before writing this receipt. Root rechecked the exact three-dot diff against
the parent and git diff --check passed. No local build or crypto was run.

| Candidate file | SHA-256 |
| --- | --- |
| include/openfhe_2023_1788/high_precision_client_io.h | 1121b1b1a5a79370b3fcce6f59f6b59ef1d4a25795a210bd8daf0c53941b4504 |
| src/high_precision_client_io.cpp | b9fb4322c137cfaab731026a9565b483c18d85d74b3f318884ca4b75650dbe11 |
| CMakeLists.txt | e437e40d3b7ee8958ca212ba85d4022998a473ed29026064d8c0ed546bbd9275 |

Frozen tracer SHA-256 remains
`6b3260a35db8715fff2a3d3a4b24137b7d8c12b6101db422c476b0236ed0ea16`;
frozen tests/precision_client_io_oracle.h remains
`9f7d8222ef6520bc845ab1b81fe735f5f7a46a48d5de2c2b984c63148e2c42af`.
An initial hash command used a nonexistent guessed helper basename; it exited
1 without mutation. rg located the actual helper above, which was rehashed
successfully. No missing file was silently treated as verified.

## Standards

Independent reviewer h128_candidate_standards: zero documented-standard
violations and zero actionable heuristic findings on the exact candidate.
Narrow public interface/private implementation, pre-call key-shape checks,
independent multiprecision codec and old-API preservation were inspected.
This is static review, not compilation or numerical execution.

## Spec

Independent reviewer h128_profile_provenance: one P2, still OPEN.
The initial context binding does not enforce the frozen firstModSize=55.
The task at lines175–181 requires scaling/first bits 50/55, but candidate
BindContext at lines150–174 accepts the supplied first Q tower after checking
the count, final two primes and consistent P/QP/partitions.

Root inspected the pristine pin df495ba2e91739a6dc8f1de254fc5a41155ce504,
src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp:415–445,500–516.
SinglePrimeModuliGen determines Q[1..7] from dcrtBits and selects Q[0]
separately from firstModSize. A fresh context requested with firstModSize=56
is therefore a source-supported acceptance-hole candidate, not an observed
runtime counterexample. This is initial-profile validation, not later live
shared-Params drift. No other actionable A-slice Spec finding was returned.

Disposition: Codex owns a separate public-constructor negative RED/GREEN
after the current frozen A tracer can compile and execute. Do not change the
existing positive vectors, oracle, thresholds or A malformed-key matrix.
Publication is solely to exercise this uncompiled development candidate in
hosted CI. It does not waive the P2, accept the whole I/O module or authorize
default-branch merge. A passing current tracer will not close the P2.

## Publication and execution gate

Root gitleaks8.30.1 scanned src (82,722 bytes) and include (12,394 bytes),
both zero findings, using env -u GITLEAKS_CONFIG -u GITLEAKS_CONFIG_TOML,
dir, --ignore-gitleaks-allow, --gitleaks-ignore-path /dev/null,
--max-decode-depth 5, --redact and --no-banner.

Push this exact engineering commit first, identify its automatic push run,
then commit this receipt and the dispatch identity as docs-only [skip ci].
No rerun/dispatch is needed. Compile/API compatibility, 32 rejection cases,
precision, focus1 and full58 are all NOT RUN for this candidate at preflight.
When the run becomes terminal, inspect exact raw logs and actual source
identity before reporting any GREEN or diagnosing a failure.

The user's newer instruction cancels the historical 1000-trial requirement.
See coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md. This changes no
frozen existing test or correctness threshold.
