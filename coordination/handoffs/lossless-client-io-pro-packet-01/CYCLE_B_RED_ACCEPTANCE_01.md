# Actual Cycle-B shared-basis RED acceptance

Date: 2026-09-05, Asia/Shanghai.
Disposition: ACCEPT_EXECUTED_CYCLE_B_RED_DUAL_HOST.
This closes the genuine missing-behavior RED gate, not the B implementation.

## Exact engineering and actual failure

Source `4648da463c6ec77f6f23acb1a56c5dce88c7732e`, branch
`codex/lossless-io-implementation-01`.
[Actual push run33960255214](https://github.com/leemaple/20231788./actions/runs/33960255214)
attempt1 was created10:17:32Z and completed/failure10:26:36Z.

Linux job101290796500 completed10:20:33Z (179s).
Windows job101290796367 completed10:26:35Z (541s).
Each job's only failed step was the focused production client-I/O test.
The new target itself compiled successfully. Both hosts first completed:

| Gate | Linux | Windows |
| --- | --- | --- |
| Original focused first Mult2 | 1/1,0.21s | 1/1,0.31s |
| Pair Add/Sub inputs | 2/2,0.31s | 2/2,0.19s |
| Original regression suite | 57/57,1.37s | 57/57,2.75s |
| Relin2/RS2/Mult2/Add/Sub API builds | 5 successful | 5 successful |
| New I/O focused contract | expected missing-rejection failure,0.48s | expected missing-rejection failure,0.37s |
| Complete58 suite | skipped | skipped |

Root independently parsed each complete raw log and verified122 actual
Start/command/result bindings:120 passes and exactly two new-contract
failures. Both live CTest JSON57 lists match the exact source CMake
name/command/order/backtrace-line entries. The original57 ledger SHA remains
`3527832e2d46591c46a93d3cb96d5469a9362ec4ca1ba39c8ed0587964e77f8b`.
There is no full58 pass claim for this RED.

Linux used an exact-key pristine install cache, not a new upstream build.
Windows actually configured/built/installed pristine OpenFHE. Both used the
pinned dependency `df495ba2e91739a6dc8f1de254fc5a41155ce504`,
native64/backend4, and warning-clean project builds.

## Preserved positive checks and distinguishing failure

Both actual new-test invocations completed original A numerical assertions,
the actual56 constructor rejection, and fresh/result clone coefficient/scalar/
present-empty-map isolation. They then proved the disposable same55 fixture
was distinct and valid: one additional matching keypair, one necessary
evaluation-key generation, real evaluator result binding and valid decryption
all preceded the sole deliberate Q8-to7 mutation.

The first missing rejection is observed at Linux raw line2665 and Windows
raw line3547:

`required shared-Params rejection was accepted at CloneForEvaluation: HighPrecisionClientIO: shared context basis changed`

The mutation-ready marker precedes that failure, and the exact source test
checks the shortened basis prefix and changed composite modulus first.
Thus this is the required missing public-boundary guard, not a compiler,
factory-alias, invalid-input, numerical, or infrastructure failure.

The remaining BindFirstMult2Rcb and Decrypt rejection assertions, and owned
evaluation-tag cleanup, were not reached. They are NOT RUN, not passed.
CTest's error-output replay is not another invocation. Root accepted one A
numerical record per host, not the unprefixed replay.

All original A errors remain within the frozen2^-80 gate, with independent
Horner/cross-precision disagreements within2^-120, exact rational S1, positive
centered headroom, original one keypair/one eval-key generation and32
malformed-key rejections. The extra B fixture counts are recorded separately.
Maximum public product error: Linux
3.83655910728679940918621882687725507470269937e-28; Windows
4.54784973371181088092204454296118402616786539e-28.
These are small diagnostic observations, not an all-domain accuracy or
security proof.

## Retained complete evidence

CYCLE_B_RED_RUN_FINAL_01.json is root's actual terminal run metadata.
CYCLE_B_RED_ROOT_VERIFICATION_01.json is the successful independent full-log
parser output. Its source is retained at ignored
artifacts/handoffs/io-cycle-b-red-01/root_verify.py; it performs no build,
cryptography or network operation. Independent host verification JSONs are
retained alongside the exact JOB logs after their completed reviews.

Linux log:206784 bytes, SHA-256
`1aad417e26fcda08faeaf8b4d7c93183621c70b6b1fc63329c79f8386692b3a9`.
Windows checked-in LF log:289595 bytes, SHA-256
`2b7ae9fe3490e412279ac008ca4884eee99265ec3b555620a62b210c55a8a41e`.
Windows complete connector decoded raw_log UTF-8:293163 bytes, SHA-256
`abe91009bf538b632b78e409f5dceab002ce2a11a5a39ffbbde794bdb9a0171d`.
Exactly3568 CRLF pairs normalize to LF; BOM and all other bytes are retained.
The original raw_log remains losslessly stored in ignored windows-api-capture.json.
These identities are decoded-string UTF-8 identities, not HTTP transport bytes.

## Next implementation boundary

Publish this actual RED evidence first. Only then add the smallest immutable
structural context/basis snapshot and live revalidation at CloneForEvaluation,
BindFirstMult2Rcb and Decrypt. Preserve State() as a nonthrowing cached value,
all tests/oracles/thresholds, DoubleCKKS and the current CMake/workflow.
Revalidate complete ordered Q/P/QP/PK/partition structure, not just the
tested tower count; do not add a deep-copy context framework or cryptographic
changes. Retain the exact frozen diagnostic and later genuine GREEN evidence.

The protected CompareTo draft error was fixed before the RED engineering
commit; source reviews and the exact correction are in CYCLE_B_RED_DISPATCH_01.md.
It is not the observed RED cause. All engineering bytes remain unchanged
while collecting these logs.

No Mac compile/crypto, CI rerun/dispatch, default merge or old implementation
access occurred. The user cancelled1000-run/statistical/performance gates.
B GREEN, three-track integration and full paper-size eight no-refresh
squarings remain incomplete.
