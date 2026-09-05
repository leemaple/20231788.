# Actual first-modulus constructor GREEN acceptance

Date: 2026-09-05, Asia/Shanghai.
Disposition: ACCEPT_FIRST_MODULUS_GREEN_DUAL_HOST. The initial first-Q55 profile
finding is closed at this bounded client constructor seam. B ownership/drift
and the full paper implementation remain incomplete.

## Identity and genuine RED before GREEN

The accepted engineering source is
`01c90e8eeec696b62b92a17be9a49d4a014664d8`, branch
`codex/lossless-io-implementation-01`.
[Actual push run33953977794](https://github.com/leemaple/20231788./actions/runs/33953977794)
is attempt1, created 07:58:12Z, completed/success 08:07:27Z.

The genuine preceding RED source
`f1ea03f35a6a553d65db30c93e771738f6bc0e1d` ran on both hosts in
run33952773643. Both reached original A success, actual56 fixture readiness,
then failed solely because the public constructor accepted the unsupported
basis. Its complete evidence was accepted and pushed at
`c1bbae203820770fd1ba498c15ae01a86185ed96` before the guard was authored.

GREEN changed only the actual first-Q bit-width guard in
src/high_precision_client_io.cpp (+3/-1). The RED test and all numerical
oracles, inputs, thresholds, original assertions and other engineering files
remain frozen. Source/review/scanner preflight and actual automatic-dispatch
identity are in FIRST_MODULUS_GREEN_DISPATCH_01.md/.json, published at
a11af469766e197856f143b486cc112d792ae712. Documentation commits are not the
tested source and do not replace that source's actual run.

## Actual hosted results

| Gate | Linux101273809680 | Windows101273809728 |
| --- | --- | --- |
| Job elapsed | 170 seconds | 552 seconds |
| Old focused first Mult2 | 1/1, 0.23s | 1/1, 0.24s |
| Pair Add/Sub inputs | 2/2, 0.12s | 2/2, 0.21s |
| Original suite | 57/57, 1.01s | 57/57, 2.50s |
| New production I/O focus | 1/1, 0.14s | 1/1, 0.22s |
| Complete suite | 58/58, 1.10s | 58/58, 2.62s |
| Relin2/RS2/Mult2/Add/Sub API builds | all five successful | all five successful |
| Pristine OpenFHE | accepted cache hit; not rebuilt here | actual configure/build/install |

Both source/pin/native64/backend4 identities and warning-clean project builds
were checked. Windows' sole setup warning is interleaved MSYS2 update output
at raw lines232-233, not a compiler/test warning. No warning threshold was
weakened. The hosted official pin is
df495ba2e91739a6dc8f1de254fc5a41155ce504.

Root independently parsed all 238 actual CTest Start/command/PASS triplets,
not just test lists or summary totals. Both live JSON57/58 lists match actual
source CMake bindings, including argument, order and source line. The preserved
original57 normalized ledger SHA-256 is
`3527832e2d46591c46a93d3cb96d5469a9362ec4ca1ba39c8ed0587964e77f8b`.

Each new test invocation first completed the original A numerical contract,
then recorded actual56 readiness and exact rejection success before CTest PASS:

- Linux focus: A2657, ready2658, rejection2659; full: A4630, ready4631,
  rejection4632.
- Windows focus: A3543, ready3544, rejection3545; full: A5523, ready5524,
  rejection5525.

The ready record proves N64/M128, eight Q towers, actual first bits56 and zero
fixture keypairs. The public-constructor rejection checks the exact existing
domain_error diagnostic and unchanged context/tables. Neither test was
rewritten to make the repair pass.

## Preserved numerical contract

All four new-test numerical records retain the original one matching keypair,
one evaluation-key generation and 32 malformed-key rejections per invocation.
All fresh/product public/oracle slot errors and adjacent-delta errors remain
at most 2^-80; independent projected Horner/forward-transform and dual-precision
disagreements remain at most 2^-120. Root rechecked each printed decimal as an
exact Fraction against its frozen bound, the exact reduced S1 numerator2^200
and denominator1267650600226646386227681786497, and positive centered headroom.

Maximum observed public product error across focus/full:

- Linux: 4.00335297032478680968740567152238141966353869e-28.
- Windows: 4.36994733547527522577283735732648216681334969e-28.

The frozen source test checks all16 slots and the numerical controls before
printing success. These are small diagnostic observations, not full-domain
proof, security certification, paper-scale accuracy or an eight-square result.

## Retained raw evidence and independent reconciliation

FIRST_MODULUS_GREEN_RUN_FINAL_01.json is root's actual terminal run read.
The complete two JOB logs, root VERIFICATION and independent Windows
VERIFICATION JSON are retained alongside this acceptance. A supplementary
Linux-agent audit was still in progress at publication and is not claimed as
finished or made an extra gate. Root's separately authored read-only
verifier and its run metadata are in
artifacts/handoffs/io-first-modulus-green-01/; its actual result is
FIRST_MODULUS_GREEN_ROOT_VERIFICATION_01.json. It does no build, cryptography
or network request. An initial parser attempt used the Make-style API build
marker for Ninja too; it stopped without changing evidence. Root corrected
only the platform log-marker handling and obtained complete exit0 verification.

Linux log: 335530 bytes, SHA-256
`6deb51f547cda977349d9929d8afc0bd028d2338be3765ae0245b44dc1fe5fa2`.
Windows retained LF log: 419975 bytes, SHA-256
`26e767ebe4c5ba821c4c91ba97b6ebc30d02b2b54d8c1bfde8e1c3e611b644e3`.
Windows complete decoded raw_log UTF-8: 425506 bytes, SHA-256
`7f224465ea3f074a36bd68351c1e106cb448c127873bb10ff809d89e18320a4c`.
Exactly5531 CRLF pairs become LF, preserving the initial BOM and every other
byte. Original raw_log is losslessly retained in ignored
artifacts/handoffs/io-first-modulus-green-01/windows-api-capture.json
(478044 bytes, SHA-256
`a43ae3f43d9f1d5db7ca72c616020e3f4b0728595734d58a7f7163e10ce30fa2`).
These are connector-decoded string identities, not claims about HTTP transport
bytes. Root independently confirmed the exact transformation.

## Next scope

The user cancelled 1000-trial and performance-statistics completion gates.
This acceptance adds no substitute trial count. The next required slice is
only the agreed B public clone-isolation/shared-Params-drift RED then smallest
GREEN. Do not skip its genuine failure or pre-implement its guard. After I/O
closure, integrate the separately accepted repeated and h128 diagnostics, then
verify the full same-root h128 family and paper-size no-refresh eight-square
path with independently frozen correctness criteria. No default-branch merge,
CI rerun/dispatch, Mac build or quarantined implementation access occurred here.
