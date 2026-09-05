# Three-track integration — exact-source hosted checkpoint

2026-09-05 Asia/Shanghai. Engineering committed and pushed:
`9c4d83b5cde16e5c5af89886bd73fe5252a99002`,
branch `codex/three-track-integration-20260905`.
Worktree: /Users/lifeng/Documents/20231788-openfhe-three-track-integration-20260905.
The original I/O, Repeated and h128 worktrees are unchanged and preserved.

## Observed before hosted execution

The new branch starts at I/O document HEAD
`3fd03fab118c884001710b5e17eca367703aa219`, whose production I/O is the tested
`5f26c77598a350bbdce9f572f64aada9d38c4117`.
Seven Repeated source/test files are exact bytes from
`d09f15f535f0dbf22ef89b33255e947166cc392a`; four h128 source/test files are exact
bytes from `1192200f558c69c0967e8306ed1a8bddf786ca34`.
All 23 other parent src/include/tests files remain exact parent bytes.

Only CMake and workflow wiring is newly edited. No algorithm, existing test
body, oracle, threshold or public contract is weakened. No new behavioral test
was authored for this co-build checkpoint: each imported track already has
its own retained RED/GREEN history. This is neither a newly observed RED nor
permission to bypass RED for the next behavior change.

Root static checks and two independent static checks found no blocking issue.
CMake has 60 unique name/command registrations: the original 57 in original
order, followed by I/O, Repeated, h128. All three EXCLUDE_FROM_ALL targets are
explicitly built, with the Repeated fixture and warning-as-error flags intact.
Five API targets and pristine OpenFHE 1.5.0 pin/native64/backend4 configuration
are preserved. Git diff checks and a gitleaks 8.30.1 scan of all 13 staged files
(221565 framed bytes, SHA256
31c351b2dc8681624e68ea4318d626a09c1662469687050370b02b3fa32a0e92)
returned exit0/no leaks. These are static/scan results, not crypto results.

## Actual hosted submission

[Run 33964209898](https://github.com/leemaple/20231788./actions/runs/33964209898)
was automatically triggered by the engineering push at 11:46:08Z, attempt1.
Linux job101301287648 and Windows job101301287513 were observed in progress.
No manual dispatch, rerun, default-branch merge or Mac C++/OpenFHE build occurred.

Expected per-host executed checkpoints: 1 first-precision +2 pair-input +57
legacy +1 I/O +2 Repeated/h128 +60 full suite =123 actual test invocations.
Before acceptance, check complete raw Start/command/result bindings and live
57/60 listings, all API/contract builds, exact-source/config provenance, and
the original numerical/negative/ownership evidence from each contract.
The running badge alone proves none of those final results.

## Observed hosted completion and retained evidence

The retained root audit is bound to engineering source
`9c4d83b5cde16e5c5af89886bd73fe5252a99002`, run `33964209898`, attempt 1,
Linux job `101301287648` and Windows job `101301287513`. Linux reached terminal
`success` at 2026-09-05T11:52:16Z; Windows reached terminal `success` at
2026-09-05T11:57:06Z. The documentation baseline supplied for this release
preparation is `688b3c406e268994efcc58ecb17faf9c611bf5bb`.

`ROOT_VERIFICATION_01.json` records `PASS_RETAINED_EXECUTION_AUDIT` for both
retained host logs: each has 123 complete Start/command/result bindings, live
57-test and 60-test listings with matching source backtraces, five public API
targets, and the three added I/O, Repeated and h128 contract targets. The final
suite is 60/60 on each host. The existing root parser was not rewritten or
rerun during publication preparation.

The six imported evidence files under `evidence/` are byte-for-byte copies
of the retained handoff artifacts; `evidence/SHA256SUMS` verifies those six
imports (run `shasum -a 256 -c SHA256SUMS` from `evidence/`).
`RUN_TERMINAL_01.json` separately retains the lead's fresh GitHub run-status
read, with both completed job step lists and exact source identity.
In particular, `windows-job-101301287513-lf.log` is the retained
LF-normalized, UTF-8 connector-decoded log. It is not original HTTP transport
bytes. Linux and Windows results establish a cross-host diagnostic co-build
and regression checkpoint only: they do not prove production I/O-to-Repeated
integration, same-root paper-scale h128 execution, or completion of the paper.
No Mac cryptographic run, CI rerun, or additional randomized trial was done.

## Explicit remaining boundary

This checkpoint puts the three diagnostic modules into one build and regression
suite. It does not yet connect production I/O to the repeated evaluator:
the I/O profile is N64/Q8/UNIFORM first-operation; Repeated is N64/Q10/two-family
and changes context/tag/level; h128 requires a supported SPARSE context with
N at least128. The original context-based DoubleCKKS constructor retains a null
plan and does not re-enter families. No old binary is reused after its layout
changes.

Next vertical slice must join client I/O to the owning evaluator's immutable
exact-scale receipt and same-root h128 setup without weakening these diagnostic
contracts. Full N32768/16384-slot eight-no-refresh squaring remains unimplemented
and unverified. The current user scope cancels1000-run/statistical/performance
gates, not the representative end-to-end correctness gate.

Final I/O source review remains the single submitted
[Review Final Implementation](https://chatgpt.com/c/6a9bfcc1-9aa0-83ec-b3c9-22cd8bbc6c6a).
A read-only check now observes its final `PASS_WITHIN_STATED_SCOPE` response,
with P0/P1/P2 all zero and the original N64 first-operation scope. The lead
received the actual 33085-byte return ZIP and matched its displayed SHA256
`0c372df3c8554d02e8be987dc9b4f8a155ffca1f363fca432daa5ec674723668`.
Its package/content acceptance is a separate recorded review step, not proof
of the paper-scale behavior still missing above. Do not resend the review.
