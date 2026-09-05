# Actual Cycle-B shared-basis GREEN acceptance

Date: 2026-09-05, Asia/Shanghai.
Disposition: ACCEPT_EXECUTED_CYCLE_B_GREEN_DUAL_HOST.

This closes the low-N client-I/O ownership repair, not final Pro review,
three-track integration, or the paper-scale implementation.

## Exact tested source and result

Engineering source `5f26c77598a350bbdce9f572f64aada9d38c4117`,
branch `codex/lossless-io-implementation-01`.
[Actual automatic push run33961604938](https://github.com/leemaple/20231788./actions/runs/33961604938)
attempt1 was created10:47:45Z and completed/success10:56:43Z.
Linux job101294323897 completed10:50:46Z (179s).
Windows job101294323767 completed10:56:43Z (535s).

| Executed gate | Linux | Windows |
| --- | --- | --- |
| Original focused first Mult2 | 1/1,0.34s | 1/1,0.25s |
| Pair Add/Sub inputs | 2/2,0.14s | 2/2,0.19s |
| Original regression suite | 57/57,1.12s | 57/57,2.36s |
| Relin2/RS2/Mult2/Add/Sub API builds | 5 successful | 5 successful |
| New I/O focused contract | 1/1,0.28s | 1/1,0.41s |
| Complete suite including I/O | 58/58,1.27s | 58/58,2.65s |

Root independently parsed both complete logs:238 actual
Start/command/Passed bindings, live57/58 JSON name/command/order/CMake-line
identities and five API builds. Both independent host reviewers reached
the same conclusion with zero unresolved findings in this runtime boundary.
These are repeated configured suite invocations, not119 independent
random trials per host or a statistical experiment quota.

Linux used the exact-key pristine dependency cache, not a fresh upstream
build. Windows actually configured/built/installed pristine OpenFHE.
Pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`; native64/backend4.
Project builds are warning-clean; the Windows MSYS2 installation warning
is distinguished from project compilation.

## Genuine RED then smallest repair

The actual B RED and complete evidence were published at
`96f5238c0ef389de122a5258dbf71cbffe50b7d9` before the GREEN source.
Both RED hosts reached deliberate shared Q8-to7 mutation and failed
specifically because CloneForEvaluation lacked the required rejection.
Bind/Decrypt rejection and cleanup were not reached in RED.

The repair changes only high_precision_client_io.cpp/.h (+88/-21).
An opaque immutable owned structural snapshot records ordered Q/P/QP/PK/
partition bases, partition counts and PModq. Public use revalidates that
snapshot at CloneForEvaluation, BindFirstMult2Rcb and Decrypt before
cloning or cryptographic primitives. It does not deep-copy an OpenFHE
context, add cryptographic operations or change numeric transforms.
State() remains a nonthrowing cached value.
All tests, independent oracle, thresholds, DoubleCKKS, CMake and workflow
remain byte-identical to the accepted RED source.

Frozen test SHA256:
`93640b6c7ba49b4594ad2f84309cbaf48e31974039d403f6941e375558f7d950`.
Frozen oracle SHA256:
`9f7d8222ef6520bc845ab1b81fe735f5f7a46a48d5de2c2b984c63148e2c42af`.
Original57 name/command ledger SHA256:
`3527832e2d46591c46a93d3cb96d5469a9362ec4ca1ba39c8ed0587964e77f8b`.

Root drafted the minimal fix after the delegated author returned an actual
model-at-capacity error without edits; this was not Fable, ZCode or Pro output.
Two independent static Standards/Spec reviews reported zero open findings
before engineering push. This does not substitute for the pending final
complete-context Pro review.

## Observed ownership and numerical boundary

Both new-test invocations on each host completed, in order:

1. Original A precision and32 malformed-key rejections.
2. Actual56 first-modulus construction and rejection.
3. Fresh/result clone coefficient/scalar/present-empty-map isolation.
4. Distinct valid55 fixture with one additional matching keypair and one
   evaluation-key generation; valid Clone/Bind/Decrypt before mutation.
5. Deliberate shared Q8-to7 drift while the immutable receipt remains8.
6. Required CloneForEvaluation, BindFirstMult2Rcb and Decrypt rejections.
7. Owned evaluation-tag cleanup and actual CTest PASS.

Linux raw2657–2668 and4638–4649; Windows raw3539–3550 and5527–5538
contain the complete ordered records.
Root independently used exact Fraction comparisons for all ten numerical
gates per invocation: original eight errors <=2^-80, independent Horner and
cross-precision disagreements <=2^-120, exact
S1=2^200/1267650600226646386227681786497 and positive centered headroom.
Maximum observed public product error over the two invocations:
Linux3.33093850266731690491788370766776839447002948e-28;
Windows5.48082463486344628148918436905618979142753786e-28.
No threshold, vector or oracle was relaxed.

## Retained evidence and scope

CYCLE_B_GREEN_ROOT_VERIFICATION_01.json is the actual successful root
full-log parser output. The read-only parser and final run capture remain
in ignored artifacts/handoffs/io-cycle-b-green-01/. It performs no build,
cryptography or network. The two independent VERIFICATION JSONs and full
JOB logs are preserved with CYCLE_B_GREEN_RUN_FINAL_01.json.

Linux full log:337688 bytes, SHA256
`c8c09173e865140d624529c948ee020918727595d312fc3b13e4a6ea2c2d9f61`.
Windows tracked LF log:421780 bytes, SHA256
`d05837d34ae99476ac90823e70d38c1192d3476ca845468faf282b8c15bbf927`.
Windows complete connector decoded raw_log UTF-8:427323 bytes, SHA256
`fa9c7a8cfd788cbaf263f304e1dfddea373866a4ecb428f3676ba048ac6adc02`.
Exactly5543 CRLF pairs normalize to LF; BOM and other bytes are retained.
The original decoded string remains losslessly in windows-api-capture.json.
These are decoded UTF-8 identities, not HTTP transport-byte identities.

No Mac compilation/crypto, CI rerun/dispatch, default merge or access to old
implementation occurred. The user cancelled1000-run/statistical/performance
gates. Current evidence is the supported N64/S16/gap2 first-operation
diagnostic with HEStd_NotSet, not a security claim or all-domain proof.
Next: final I/O source review, isolated three-track integration with60 tests,
then same-root h128 family and actual paper N32768/full packing eight
no-refresh squares with criteria frozen before execution.
