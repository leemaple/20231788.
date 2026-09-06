# Experimental precision116 eight-square correctness acceptance

## Outcome

The paper's t=2 Double-CKKS algorithm has a passing full-eight numerical implementation under the separately named `experimental-s116-d56-b58-v1` profile. This accepts the bounded correctness experiment, not exact Table3 parameter reproduction or deployment security. The overall project still needs final default-branch delivery and its completion audit.

In plain language: one encrypted input containing16384 complex values was squared eight times without decrypting or refreshing during evaluation. Client-side checks then compared the decrypted answers with independently computed high-precision plaintext answers. Both Linux and Windows passed the unchanged absolute real/imaginary component-error bound `2^-80`, about `8.27e-25`.

| Evidence | Linux | Windows |
| --- | --- | --- |
| Job | 101547399114 | 101547399237 |
| Final maximum component error | 2.5905123324714234e-26 | 3.4805603371613677e-26 |
| Limit divided by observed error | 31.93116 | 23.76573 |
| New full-eight test duration | 20.54s | 21.89s |
| Prior unique regression tests | 60/60 | 60/60 |
| New numerical test | 1/1 PASS | 1/1 PASS |
| Cleanup / numeric misses | PASS / 0 | PASS / 0 |

Run[34055816234](https://github.com/leemaple/20231788./actions/runs/34055816234), attempt1, exact source`2b8b349edf5575556347082c1b725f6696c743b6`. Five public API compile contracts also built on each host. Focused checkpoints repeat some tests;124 passing invocations per host represent61 unique tests, not124 independent experiments. Test durations include validation and are not isolated multiplication benchmarks.

## Accepted boundary

- Clean-room implementation on pristine OpenFHE1.5.0 commit`df495ba2e91739a6dc8f1de254fc5a41155ce504`, native64/backend4.
- N32768,16384 slots, same-root sparse h128 secret and fixed original dyadic input family.
- Public encryption; one evaluator-only DCP and eight `Mult2(pair,pair)` operations, followed by terminal RCB; no refresh, key search, known-plaintext correction, relaxed gate or retry.
- Exact ordered modulus/key-family/scale transitions,32-node receipt ancestry, ownership and negative-boundary assertions.
- High-precision production client I/O against an independent plaintext ideal; separate sparse-CRT/binary512 Horner checks at ten anchors per stage; full-slot fresh/final component errors, codec/witness/domain/headroom predicates and wrong-scale falsifier.

Root read the final independent[hosted-runtime review](RUNTIME_REVIEW.md), SHA-256`f72832c072213d5cf9221ac51a56b0a64678dbd32908e915fd88277c4e01d907`, and found its disposition consistent with the retained source/status/logs and root's scalar reconciliation. See[HOSTED_EXECUTION.md](HOSTED_EXECUTION.md),[ROOT_RUN_RECEIPT.json](ROOT_RUN_RECEIPT.json) and[RUN_34055816234_STATUS.json](RUN_34055816234_STATUS.json) for exact identities. Raw logs are retained without reformatting. No new numerical replay is required to close this slice.

## What this does not prove

The original S100 profile remains FAIL and is preserved unchanged. The candidate raises the precision scale to2^116 and changes three primes; it is an explicitly different parameter regime, not a retroactive fix to the recorded Table3 experiment. QP exposure is about712bits, so security remains UNRESOLVED and no128-bit claim is inherited.

Two successful randomized draws do not prove every possible key/noise draw. Ten intermediate anchors do not prove all-slot intermediate accuracy/nonwrap or Tensor/Relin lift safety; the runtime records those limits. Acceptance follows the user's correctness-focused scope with no1000-trial quota and does not add an unrequested security-certification project.

## Remaining delivery

Keep this scientific source/test evidence frozen. Safely promote reviewed descendants to the default branch while preserving user reports. Make default CI run ordinary regressions without repeating either expensive numerical experiment, verify that delivery-only condition change, and provide clear reproduction/limitations guidance. The overall goal stays ACTIVE until final delivery and requirement-by-requirement audit are complete.
