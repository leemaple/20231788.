# h128 Cycle-B RED accepted — 2026-09-05

Observed: both hosted jobs fail for the intended missing unsupported-profile
rejection, after all prior build/regression gates pass. This receipt is committed
and pushed before applying original 0004; no B GREEN is yet present.

- Source `43c2dca45c2305c9b6baf50ae1c32529d35e7f06`.
- [Run 33945243915](https://github.com/leemaple/20231788./actions/runs/33945243915),
  automatic push, attempt 1, completed/failure.
- Linux job `101250030746`, complete 04:44:16 UTC.
- Windows job `101250030632`, complete 04:50:56 UTC.
- A GREEN acceptance `094d320d9404cd67e03be3d45711f2f723f93f96` was already
  pushed before the one-file, original 0003 B test.

| Actual check | Linux | Windows MinGW64 |
|---|---|---|
| Official pin / native64 backend4 | exact-pin install cache | actual dependency build/install |
| Warning-clean build / five API targets | PASS / PASS | PASS / PASS |
| Prior first-Mult2 focus | 1/1, 0.30 s | 1/1, 0.21 s |
| Prior Pair Add/Sub focus | 2/2, 0.20 s | 2/2, 0.19 s |
| Legacy suite | 57/57, 1.40 s | 57/57, 2.50 s |
| New h128 target compile/link | PASS | PASS |
| New focused invocation | 0/1, 0.22 s | 0/1, total 0.07 s |
| Exact failure | unsupported profile was accepted | unsupported profile was accepted |
| CTest exit | 8 | 8 |
| Full 58 invocation | SKIPPED | SKIPPED |

The exact official pin remains
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both failures are uncaught
`std::runtime_error` from the intended public test assertion, not a build,
fixture, timeout, loader or infrastructure error. No project C++ warnings were
found. CMake/workflow/production/header/profile and frozen A assertions were
unchanged; resulting original B test SHA-256 is
`c2698109d0a45621f6c705bcdfd2d0da3ae748db9802db1287de8517077fb81f`.

## Observed versus inferred

Windows prints the numbered valid-path marker once; CTest repeats its failed
output, so two textual copies are not two executions. Linux does not print the
marker: the newline-only stdout can remain buffered on exception termination.
For Linux, completion of ValidPath and entry to the first NoiseScale=2 case is
a frozen-source control-flow inference supported by the unique semantic
diagnostic, not a claimed observed marker. Neither RED reaches subsequent named
rejections or Lifecycle. No test modification was made to force logging.

Independent reviewers separately matched each host's actual legacy 57
names/normalized commands/order against the tested Git CMake. Root separately
captured Linux's full API log and matched it byte-for-byte to the independent
retained log, checked required metadata/failure evidence, and parsed all 61
Windows starts/commands across the four invocations. Root's first Windows
parser rejected CTest's aligned Start spacing; a whitespace-normalization fix
then passed all bindings. This was a parser correction, not a test/code fix.

## Retained evidence identities

- Linux raw/retained: 129,058 bytes, SHA-256
  `67102e8c6d47ac80db01c3c3aada966ab09d72b5efa9c0c5f7db8a874c644304`.
- Windows raw: 213,909 bytes, SHA-256
  `74d56d1e22d860f173fa3cf33746fb6d3addbf038c841697ba0c0886f14361b9`.
- Windows retained LF: 211,792 bytes, SHA-256
  `9722726ba4982b7403797e075641a27035e4641349bf2ce3145b23b26dfed6d8`.

Adjacent CYCLE_B_RED logs, per-host verification JSON and final run metadata
retain the full evidence. Ignored
`artifacts/handoffs/h128-cycle-b-red-01/windows-api-capture.json`
losslessly retains original CRLF text. Hashes match independent Windows review.
Raw runner trailing whitespace is preserved; only the two exact raw log paths
may be excluded from docs whitespace checks.

Next: original 0004 production-only GREEN, keeping B test and every other
engineering file byte-frozen, then dual-host focused 1/1/full 58/58 and existing
build/regression gates. No Mac build/crypto, default merge, paper-scale,
high-precision I/O, family-sharing, eight-square, security or performance claim.

Pre-publication gitleaks 8.30.1 scans at 12:54 Asia/Shanghai found no leaks:
1,540,896 bytes in the handoff evidence directory and 220,233 bytes in the
separate ignored original Windows capture directory.
