# First-modulus constructor boundary RED: preflight and actual dispatch

2026-09-05, Asia/Shanghai. This records a reviewed test candidate and its actual
automatic push run. The runtime RED has not yet been observed or accepted.

Engineering source: `f1ea03f35a6a553d65db30c93e771738f6bc0e1d`.
Parent: `d1e4723e41d1d49631cafc97e51dd13d80d039af`.
Branch: `codex/lossless-io-implementation-01`.
Exact remote: `https://github.com/leemaple/20231788..git`.

The preceding A production tracer at engineering source
`084ffa0af3cb21623151df0c826736ca84954140` passed actual Linux/Windows
focus 1/1 and full 58/58 in run33950923304. The complete audited acceptance
evidence was pushed at parent d1e4723 before this new RED was authored.
See CYCLE_A_GREEN_ACCEPTANCE_01.md; that acceptance does not close the open P2.

## Scope and source-supported defect

The frozen client profile requires scaling/first bits 50/55 in
coordination/tasks/LOSSLESS_CLIENT_IO_PRO_IMPLEMENTATION_01.md:167-181.
The current production BindContext checks the eight-Q shape, tail primes,
P/QP and partitions but does not enforce the first-Q 55-bit requirement.

Pristine pin df495ba2e91739a6dc8f1de254fc5a41155ce504:
ckksrns-parametergeneration.cpp:419-445 selects the tail independently from
firstModSize; line504 selects Q[0] with LastPrime(firstModSize). The official
LastPrime implementation checks its returned bit length; FIXEDMANUAL scaling
uses 2^50 here. This supports a fresh firstModSize56 fixture. It is still
source inference until the specific runtime failure is captured.

This is a public-constructor initial-profile test, not the later B shared-Params
drift test. The user has cancelled 1000 experiments as a completion gate:
coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md governs current scope.
No vectors, oracle, precision thresholds or existing assertions were weakened.

## Exact candidate

Only tests/precision_client_io_first_mult2_contract_test.cpp changed:
+31/-3; 609 lines; 40,374 bytes; SHA-256
`e6d795c5789ce9728eaa8e7fdb00b2f093f1de41964791167b2ea98897b61746`.
The complete diff is 3,143 bytes, SHA-256
`873ca4ac68709d7c336bd55ec5816118a6aa7106b820e35841c1231a38ddd799`.

The four edits add stdexcept, parameterize MakeContext with unchanged default
55, append CheckUnsupportedFirstModulus, and call it after unchanged
RunContract. The new function creates a fresh requested first56 context,
validates its actual context/basis/table shape, checks actual first bits56 and
base scale 2^50, then flushes a fixture-ready marker before calling the public
client constructor. It catches only std::domain_error with the exact expected
diagnostic and checks context/table nonmutation before requiring rejection.

Both author and root independently reversed only those four changes in memory
and reproduced the complete previous test byte-for-byte, including its original
SHA-256 6b3260a35db8715fff2a3d3a4b24137b7d8c12b6101db422c476b0236ed0ea16.
The original positive path, 32 malformed-key checks, owned outputs, vectors,
all numerical bounds, single keypair and evaluation-key generation remain
unchanged. The additional constructor fixture contains no KeyGen, Encrypt,
Decrypt or EvalMultKeyGen. All production files, other tests, oracle, CMake,
five explicit API targets and CI workflow are unchanged.

## Standards

Independent h128_candidate_standards: zero documented-standard violations and
zero actionable baseline-heuristic findings on the frozen candidate.
The default55 fixture remains unchanged; the narrow expected-exception catch
is permitted by TASK:89-91,371-373 and the fail-loud engineering boundary.
It introduces no framework, production guard, extra key or B mutation.

## Spec

Independent h128_profile_provenance: zero actionable findings on this RED.
The actual shape/bit-width/base-scale checks precede the flushed ready marker
and public constructor; the exact rejection and unchanged context are checked.
Root separately inspected the complete diff, actual preconditions and official
SinglePrimeModuliGen source and reproduced the unchanged-A bytes.
These are static dispatch judgments, not proof that the P2 is closed.

## Required actual RED observation

Both hosts must first compile the new target warning-clean and pass the existing
old focus, Pair, legacy57 and five explicit API gates. Inside the new focused
test, the unchanged A positive contract must reach its success record first.
Then the log must contain this flushed fixture observation:

```text
first_modulus_boundary_fixture ready=1 actual_first_bits=56 Q_towers=8 N=64 M=128 fixture_new_keypairs=0
```

The expected failure is:

```text
required rejection was accepted: HighPrecisionClientIO: unsupported diagnostic Q basis
```

A compile failure, fixture-construction error, wrong exception/diagnostic,
different invariant failure or infrastructure problem is not this expected RED.
The full58 step is expected to be skipped after the failing focus, not passed.
Retain and inspect actual complete logs and source identity before accepting RED.
Only after that evidence is saved may the smallest first-Q55 guard be written;
do not change this test in GREEN or add the separate B behavior early.

## Publication and CI identity

Before commit, root checked the exact branch/base, only this changed file,
candidate hash, empty previous index and git diff --check. The staged
git diff --check passed; gitleaks8.30.1 scanned the entire 3,143-byte staged diff
with both config environment variables unset, inline allows disabled,
ignore file /dev/null, decode depth5, archive depth1 and redaction: zero findings.

Engineering commit f1ea03f was created and pushed first; HEAD and remote tracking
both matched it with a clean worktree. Its automatic push run is
[33952773643](https://github.com/leemaple/20231788./actions/runs/33952773643),
attempt1, created 2026-09-05T07:30:59Z / 15:30:59 Asia/Shanghai.

- Linux job101270511248 started 07:31:01Z.
- Windows job101270511113 started 07:31:02Z.
- At the retained metadata observation, both were in_progress. Linux was at
  the pristine-install cache step and Windows at toolchain installation.
- No workflow dispatch or rerun was requested. Runtime RED, candidate precision
  results and candidate full-suite acceptance are PENDING.

FIRST_MODULUS_RED_DISPATCH_01.json preserves the actual gh run view result.
This receipt is documentation only and follows the engineering push with
[skip ci]; its later docs commit must not be substituted for the tested source.
No Mac build, crypto, NTT or benchmark was executed.
