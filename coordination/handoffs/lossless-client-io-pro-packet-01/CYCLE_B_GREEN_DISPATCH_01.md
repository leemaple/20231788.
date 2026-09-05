# Cycle-B minimal production GREEN candidate dispatch

Recorded2026-09-05, Asia/Shanghai. Status: reviewed candidate, hosted execution
in progress. This is not B GREEN acceptance or a full-paper result.

## Actual RED before implementation

The previous source4648da463c6ec77f6f23acb1a56c5dce88c7732e/run33960255214
failed at the required missing CloneForEvaluation shared-basis rejection on
both hosts, after all positive preconditions and original numerical tests.
Its complete seven-file evidence was accepted and pushed at
`96f5238c0ef389de122a5258dbf71cbffe50b7d9` before any GREEN source edit.
See CYCLE_B_RED_ACCEPTANCE_01.md.

The assigned author then returned an actual model-at-capacity error. Root
verified the worktree was clean, with no partial source edit, and took over
the two production files. This is the documented Codex fallback, not a Pro,
ZCode or Fable implementation/review. No model identity was substituted and
no external long-running conversation was interrupted.

## Candidate and actual automatic CI

- Engineering source: `5f26c77598a350bbdce9f572f64aada9d38c4117`.
- Branch: `codex/lossless-io-implementation-01`.
- [Actual push run33961604938](https://github.com/leemaple/20231788./actions/runs/33961604938),
  attempt1, created10:47:45Z, in progress at dispatch.
- Linux job101294323897 started10:47:47Z.
- Windows job101294323767 started10:47:48Z.
- Pristine pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

The engineering phase was committed/pushed first; the exact automatic run was
resolved before this documentation was created. There was no CI rerun/manual
dispatch, default-branch merge, Mac build or cryptographic execution.

Only the public header's private representation and its production source
changed, +88/-21 overall. Source598 lines/34158 bytes, SHA-256
`ba9ba482b14583d4de9b1c63d2ed06b172f9fa7947e91732de4bee05ed5ce264`.
Header147 lines/5032 bytes, SHA-256
`0b31c76919586b080f4cff116396034321398372f0585602943dd8381504766f`.

A private opaque shared immutable binding retains owned Q/P/QP/PK/partition
basis values, partition counts and PModq. Bound receipts keep it alive
independently of the client object. The underlying upstream Context/Params
remain shared, not transitively deep-copied. Every Clone/Bind/Decrypt boundary
revalidates relevant bindings before returning/using raw ciphertext or calling
a cryptographic primitive. Existing Encrypt profile validation also rechecks
the same captured structure. State() remains the unchanged nonthrowing cached
receipt accessor. The exact shared-basis diagnostic is unchanged.

Root statically verified the numerical transforms/rounding, rational methods,
ciphertext checks and key-shape checks byte-for-byte unchanged. Public value
types, frozen test/oracle, DoubleCKKS, CMake and workflow are unchanged.
The test remains SHA93640b6c7ba49b4594ad2f84309cbaf48e31974039d403f6941e375558f7d950;
the oracle remains SHA9f7d8222ef6520bc845ab1b81fe735f5f7a46a48d5de2c2b984c63148e2c42af.

Strict final staged diff-check passed. Gitleaks8.30.1 scanned all13037 bytes
with no allowlist/config override, max-decode-depth5/max-archive-depth1:
exit0/no leaks. Diff SHA-256
`2b6f31e112278ca03fb94c6ee31271ec320fbf88a8666c945b4b3d6f3da4780e`.
Exact preflight/scanner output is retained in CYCLE_B_GREEN_PREFLIGHT_01.json.

## Standards

Independent /root/io_b_red_standards fully reviewed the exact two-file diff
against96f5238 and confirmed both candidate hashes. ACCEPT_STATIC_GREEN_CANDIDATE,
zero open hard findings or additional design suggestions. Owned immutable
basis values, opaque shared-pointer lifetime, initialization/type lookup,
null/count-before-index ordering and preserved context/profile behavior were
checked. Exact upstream GetNumberOfQPartitions reports the actual vector size
(rns-cryptoparameters.h:409), while GetParamsPartQ indexes unchecked (:420);
the new check precedes every such access. No catch, deep-copy framework or
cryptographic/slot-transform change was introduced.

## Spec

Independent /root/h128_candidate_spec fully reviewed the same frozen diff.
Zero missing, incorrect or out-of-scope requirements found. TASK correction2
and B4 are implemented in the source: full structural snapshot, live guard
before Clone/Bind/Decrypt use, original State() noexcept and ordinary clone
policy preserved. Relevant client, parent and input receipt bindings are
checked. First55/profile, arithmetic, scale and physical-metadata sequence
remain unchanged. This review does not prove runtime passage or safety under
arbitrary unsupported concurrent context mutation.

Standards:0 open. Spec:0 open. Both reviews are STATIC; compilation,
cryptographic execution and GREEN CI were NOT RUN by the reviewers.

## Next actual gate and full-objective boundary

Await complete logs on both hosts. Require five API builds, original57
regression tests and focused1/full58 with actual clone-isolation, valid
disposable fixture, Q8-to7 mutation, all three exact drift rejections and
owned-tag cleanup. Confirm the original A numerical bounds and counts remain
unchanged; do not count CTest lists or repeated log output as execution.
Retain decoded Windows CRLF/BOM separately from checked-in LF evidence.

Only after hosted GREEN acceptance proceed to final source review and
three-track integration, followed by same-root h128 family and N32768/full
packing/eight no-refresh squarings. User-cancelled1000-run and performance
statistics are not completion gates. This low-N guard does not establish
those remaining full implementation results.
