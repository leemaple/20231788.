# First-modulus constructor boundary GREEN candidate: preflight and actual dispatch

Recorded 2026-09-05, Asia/Shanghai. Engineering source:
`01c90e8eeec696b62b92a17be9a49d4a014664d8`; branch
`codex/lossless-io-implementation-01`; exact remote
`https://github.com/leemaple/20231788..git`.
Parent `c1bbae203820770fd1ba498c15ae01a86185ed96` contains the accepted
actual Linux and Windows RED evidence from run33952773643, before this fix
was written. See FIRST_MODULUS_RED_ACCEPTANCE_01.md.

## Minimal repair and frozen test

Only src/high_precision_client_io.cpp changed (+3/-1). BindContext now also
requires actual first-Q modulus GetMSB() == 55, after ReadBasis validates the
non-null basis/towers and after the size-eight short-circuit check. It uses
the actual integer modulus, not a nominal request or floating-point estimate.
It retains the existing domain_error and exact unsupported diagnostic Q basis
message. No B shared-Params behavior has been added.

Production source: 537 lines, 30,860 bytes, SHA-256
`72ae54f668667312f1c2f9af9d980f3a40715d828a5723d48f8f15f359370f5e`.
Exact diff: 1,014 bytes, SHA-256
`59615946c580472e1f8eeb48afca487985cbd58f94a5b67c59aecf107f817608`.

The genuine RED test is byte-identical: 609 lines, 40,374 bytes, SHA-256
`e6d795c5789ce9728eaa8e7fdb00b2f093f1de41964791167b2ea98897b61746`.
Oracle SHA-256 remains
`9f7d8222ef6520bc845ab1b81fe735f5f7a46a48d5de2c2b984c63148e2c42af`.
Header, all other tests, CMake, workflow, five public API targets, vectors,
precision bounds, original A path and key generation counts are unchanged.
The user explicitly cancelled 1,000 trials as a completion gate; correctness
scope in coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md governs.

## Static review and publication gates

Root inspected the entire one-file diff and ReadBasis safety preconditions.
Independent Standards (h128_candidate_standards): zero documented violations
and zero actionable baseline-heuristic findings.
Independent Spec (h128_profile_provenance): zero actionable findings; 26
non-changing file identities checked. Neither review claims a runtime result.

Before the engineering commit, exact branch/base/only-file/hash/index and
staged git diff --check passed. Gitleaks8.30.1 scanned the entire 1,014-byte
staged diff with config variables unset, inline allows disabled, ignore path
/dev/null, decode depth5, archive depth1 and redaction: zero findings.
The engineering commit was pushed first and HEAD/origin matched with a clean
worktree; this subsequent documentation commit uses [skip ci].

## Actual automatic CI and still-required acceptance

[Run33953977794](https://github.com/leemaple/20231788./actions/runs/33953977794),
push attempt1, exact source01c90e8, created 2026-09-05T07:58:12Z / 15:58:12
Asia/Shanghai. Linux job101273809680 and Windows job101273809728 both started
07:58:15Z. FIRST_MODULUS_GREEN_DISPATCH_01.json preserves actual metadata.

At this dispatch receipt, Linux job is completed/success; Windows remains
in_progress. The Linux dashboard success alone is not accepted numerical or
full-suite evidence. Both complete raw logs still require source/pin/toolchain,
all actual command/result groups, original A numerical bounds and actual56
fixture followed by the expected rejection-success marker to be audited.
No candidate GREEN acceptance is claimed here.

No workflow dispatch/rerun, Mac build, cryptographic experiment, NTT or
benchmark was performed by this dispatch. B may start only after the unchanged
first-modulus test has actually passed and its dual-host evidence is accepted.
