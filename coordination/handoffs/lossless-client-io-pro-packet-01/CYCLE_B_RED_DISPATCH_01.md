# Cycle-B ownership/shared-basis RED dispatch

Recorded 2026-09-05 18:18 Asia/Shanghai. This is a reviewed candidate and an
actual CI dispatch identity, not an observed RED or implementation acceptance.

## Exact identities and publication order

- Branch: `codex/lossless-io-implementation-01`.
- Parent: `b3e026ea2617eb5070e8d503aeee98c21620e86f`, which had already
  published actual dual-host first-modulus GREEN acceptance before B was written.
- Engineering commit: `4648da463c6ec77f6f23acb1a56c5dce88c7732e`.
- [Actual automatic push run33960255214](https://github.com/leemaple/20231788./actions/runs/33960255214):
  attempt1, created10:17:32Z, in progress at dispatch.
- Linux job101290796500; Windows job101290796367. Both started10:17:34Z.
- Pristine dependency remains `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

The engineering commit was pushed first and this exact automatic run was then
resolved before these documentation files were created. No manual dispatch,
rerun, merge, or Mac build/cryptography was performed. Documentation HEAD is
not the engineering source under test.

## Candidate and invariant preservation

The sole engineering change is
`tests/precision_client_io_first_mult2_contract_test.cpp`, +239/-2,
846 lines,56986 bytes, SHA-256
`93640b6c7ba49b4594ad2f84309cbaf48e31974039d403f6941e375558f7d950`.
The staged diff is18789 bytes, SHA-256
`feb06fea56e5dff6f0bc4778f560e69328dbc26a9da1980f3cbc9a410911068f`.

Root independently reconstructed the exact prior A/firstMod test by removing
only the new helpers/calls/include and restoring the marker newline/main
placement. This restored the original SHA
`e6d795c5789ce9728eaa8e7fdb00b2f093f1de41964791167b2ea98897b61746`.
The subsequent public-API correction changes only a newly added expression,
and its inverse restores the previously verified candidate byte-for-byte.
All original numerical vectors/assertions/thresholds and actual56 rejection
remain unchanged. Production, oracle, DoubleCKKS, CMake, workflow, original57
bindings and all five API targets are unchanged. Exact file identities are
retained in CYCLE_B_RED_PREFLIGHT_01.json.

Root checked the final staged diff with gitleaks8.30.1, ignored no allowlist,
cleared configuration environment overrides, and used max-decode-depth5 and
max-archive-depth1. Actual exit0/no leaks across18789 bytes; diff-check passed.
No key coefficients or credentials are emitted by the new test snapshots.

## Standards

Independent reviewer `/root/io_b_red_standards` completed a full diff review
and confirmed the final candidate hash. Disposition:
ACCEPT_STATIC_RED_CANDIDATE; zero remaining hard violations or added design
suggestions. This is source review, not compiled evidence.

Root first found a P1 in the uncommitted draft: direct invocation of protected
CryptoParametersRNS::CompareTo (official rns-cryptoparameters.h:155, protected
region:59–172). Root replaced it with public value comparison
`*cp == *originalParams` (base-cryptoparameters.h:115–116), which invokes
the same virtual comparison. The independent reviewer verified the access
restriction and the exact one-line fix. This correction prevents an unrelated
compile failure from being confused with the intended behavior RED.

## Spec

Independent reviewer `/root/h128_candidate_spec` verified the frozen B
requirements, original contracts and exact-pin ownership APIs. Its initial
zero-finding report missed the protected invocation and is explicitly
superseded by its acknowledgment and final one-line delta recheck. The new
candidate has zero remaining known Spec findings. No original requirement
or equality semantics was weakened to make the correction.

Fresh and result clones are mutated through public APIs. Values, scalar state,
present-empty metadata maps, sibling/later clones and cached receipt state are
checked independently. The last fixture retains strong references to all
original contexts, clears only the context factory registry, and checks actual
context/crypto/encoding/Q/P/QP/partition/native-Params nonaliasing. It uses one
additional matching keypair and one eval-key generation because it genuinely
performs evaluator arithmetic before the mutation. All original evaluation
cache entries are retained and checked; only its own new tag is removed after
successful rejection checks.

Standards:0 open, P1 fixed. Spec:0 open, prior access-check omission corrected.
Neither reviewer claims a fresh compile, crypto test or CI result.

## Expected next actual gate

Execution order is original A success, actual56 rejection, fresh/result clone
isolation, valid disposable Clone/Bind/Decrypt setup, then one shared Q mutation
from8 to7 towers with exact prefix/composite-modulus verification. The original
fixture stays alive and unchanged; State remains a nonthrowing cached value.

The frozen project diagnostic is:

`HighPrecisionClientIO: shared context basis changed`

The first required rejection is CloneForEvaluation. Current production still
directly clones without this live-basis guard, so the source-supported expected
RED is the missing rejection at that boundary. BindFirstMult2Rcb and Decrypt
are sequenced after it and are NOT RUN if the first assertion fails. Their
inputs were genuinely constructed before drift; no cryptography or NTT is
invoked on the deliberately malformed basis. A build/toolchain/earlier
numerical failure would not establish this intended RED.

Await actual terminal logs on both hosts, confirm the old57/five APIs and all
pre-drift markers, and publish accepted RED evidence before writing the
smallest production guard. No GREEN is included in this commit.

The user's correctness-only scope controls: no1000-run/statistical/performance
gate. I/O B, three-track integration and paper-size full-packing eight
no-refresh squarings are not complete.

## Supplementary closed first-modulus Linux audit

The Linux supplementary review that was still running when b3e026e was
published has now completed. FIRST_MODULUS_GREEN_LINUX_VERIFICATION_01.json
is21173 bytes, SHA-256
`a877d95a4581a72887c1f9c41414ee051103eee4645cd44ab31281b1955d61f8`.
It independently confirms119 actual Start/command/PASS bindings, five APIs,
the unchanged57/58 lists, two original A numerical records, and actual56
rejections. It agrees with root's already published full-log acceptance and
is not an extra gate. The original historical run snapshot remains unchanged;
this review does not rerun tests or change accepted source01c90e8.
