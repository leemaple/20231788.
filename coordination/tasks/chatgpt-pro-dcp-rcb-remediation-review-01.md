# ChatGPT Pro remediation review — current DCP/RCB exact commit

Prepared: 2026-08-31 Asia/Shanghai

## Bounded objective

Continue the existing saved DCP/RCB review conversation and independently
decide whether the two changes made after your prior review close its only P1
without introducing a new DCP/RCB defect. This is a same-commit algorithm,
numeric-semantics, OpenFHE-integration, and test review. It is not a network-
security review.

Review only the supplied clean-room project at branch
`agent/codex-dcp-rcb-01`, exact commit
`02b34bac9cb87afc8acb9df275d5c0e137b554e7`, against the supplied paper and
official pristine OpenFHE 1.5.0 at exact commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The archive contains no `.git`;
these identities are a binding manifest. Report any inconsistency rather than
assuming it away.

The project is a clean-room reimplementation. Do not seek, inspect, infer, or
reuse any previous/local/private 2023/1788 implementation or the paper authors'
proof-of-concept code.

## Complete prior-review context

In this same conversation, you reviewed project commit
`a3df1c5843e8bb843f8d9becc3c8a135ffba63cd` from input package SHA-256
`1943bab0829792f18a55e90bad3c07efa05ccb94b000a12b898597d3db69b6ac`.
Your returned archive was `FINAL-DCP-RCB-REVIEW-a3df1c5.zip`, 17,537 bytes,
SHA-256
`72a6e89b0a88540b59093607ce5149bc3f5f809985be24d38662d7aa9cd9dc8f`.
It is included under `prior-review/` and passed archive-integrity, targeted
credential, and Gitleaks checks after download.

Your bounded verdict was `NEEDS NAMED FIXES`, P0 = 0, P1 = 1. You found no
DCP/RCB formula, centered quotient/remainder, RCB recombination, or OpenFHE
mapping defect. The sole P1 was incomplete evidence that DCP leaves its source
ciphertext wholly unchanged and RCB leaves both pair members wholly unchanged.
You proposed adding whole-object comparisons to existing clone snapshots and
made no local build/CTest or Windows pass claim because your OpenFHE build timed
out.

The current package contains that entire returned review so this request does
not rely on conversation memory.

## Change 1 — test-only closure of the named P1

Codex independently inspected official OpenFHE 1.5.0 before implementing the
remediation. `CiphertextImpl::Clone()` preserves the ciphertext elements and
observable metadata. `CiphertextImpl::operator==` compares its `CryptoObject`
base, slot count, level, hop level, noise-scale degree, floating and integer
scaling factors, encoding type, metadata-map contents, and elements
(`src/pke/include/ciphertext.h`, around lines 385-432 in the supplied source).

Test-only commit `02b34bac9cb87afc8acb9df275d5c0e137b554e7` adds these assertions to the
latest tests:

```cpp
Check(*fixture.input == *inputBefore,
      "DCP mutated input observable ciphertext state");
Check(*pair.GetHigh() == *highBefore,
      "RCB mutated pair high observable ciphertext state");
Check(*pair.GetLow() == *lowBefore,
      "RCB mutated pair low observable ciphertext state");
```

These assertions cover already required non-mutation behavior, so no fabricated
red test was claimed or expected. This commit changes no production source.

Exact GitHub Actions run
`https://github.com/leemaple/20231788./actions/runs/33406650125` tested exact
head `02b34bac9cb87afc8acb9df275d5c0e137b554e7`:

- Linux/GCC job `99535769420`: strict project build succeeded; 1/1 CTest passed
  (`dcp_rcb`, 0.02 s).
- Windows Server 2022/MSYS2 MinGW64 job `99535769356`: checked out official
  OpenFHE commit `df495ba...`, built and installed it, compiled
  `src/double_ckks.cpp` and `tests/dcp_rcb_test.cpp`, linked
  `dcp_rcb_test.exe`, and passed 1/1 CTest (`dcp_rcb`, 0.12 s).

Treat these retained logs as execution evidence, not as a substitute for source
review.

## Change 2 — independently discovered slot-manifest hardening

While your review of `a3df1c5` was running, Codex independently found a
separate observable-metadata gap: the private pair manifest did not bind the
CKKS slot count. Official OpenFHE copies plaintext slots into an encrypted
ciphertext (`src/pke/include/cryptocontext.h`, around lines 1255-1263) and copies
the ciphertext slot count into the decoded CKKS plaintext before decoding
(`src/pke/lib/cryptocontext.cpp`, around lines 479-489). Other CKKS operations
also consume `GetSlots()`. A pair member whose slot count has been tampered can
therefore change the observable decoding/operation contract even when its ring
elements and all previously checked fields match.

The retained TDD sequence is:

1. Test-only commit `d0cbc97190c9cc5be2164c7bcbff82109fd2ca55`
   changed one pair member's slot count and required RCB to reject it before raw
   arithmetic. In run
   `https://github.com/leemaple/20231788./actions/runs/33404277096`, the Linux
   strict build succeeded and CTest failed for exactly
   `tampered pair slot metadata did not fail fast`. After preserving that red,
   the redundant Windows job was cancelled to avoid another full OpenFHE build;
   no Windows result is claimed for the red commit.
2. Minimal production commit
   `4971d2292b5af0ddbbe0c7dbe5a2e87f45102ff1` adds `slots_` to the private,
   read-only `CiphertextPair` manifest, captures it in DCP, and cross-validates
   both members before RCB arithmetic. It does not change DCP/RCB polynomial
   arithmetic.
3. Exact run
   `https://github.com/leemaple/20231788./actions/runs/33404816846` passed the
   strict project build and 1/1 CTest on both Linux/GCC and Windows
   2022/MSYS2 MinGW64 for that production commit.
4. The current exact run `33406650125` then passed both platforms after the
   P1-closing test-only assertions above.

Your prior review noted that arbitrary metadata-map, hop-level, integer-scale,
or slot fields are not automatically mathematical pair-manifest invariants.
Codex agrees with that general constraint but inferred that slots are distinct
because OpenFHE uses them as the CKKS decode/operation length and upcoming
Tensor2 must also require mutually compatible pairs. Resolve this concrete
judgment explicitly: is binding equal slot count across the pair a necessary,
correct, KISS-sized public-seam invariant, or should this production addition
be removed? Base the answer on supplied source and observable behavior, not on
the future Tensor2 plan alone.

## Current architecture and non-negotiable boundaries

- `DoubleCKKS` is bound to one exact `CryptoContext<DCRTPoly>` and is the only
  constructor of valid pair values.
- `DCP` accepts a fresh level-zero, noise-scale-degree-two, evaluation-format
  CKKS ciphertext over exact basis `[q0, ..., q_l, q_div]`, using
  `FIXEDMANUAL`. It returns a read-only `CiphertextPair` over the retained prefix
  `[q0, ..., q_l]`, at level 1 and lifecycle `ReadyForFirstMult`, with exactly
  two RLWE components in each member.
- DCP uses locally derived `q_div^-1 mod q_i` and its negative with pristine
  `DCRTPoly::DropLastElementAndScale`; it must not regain unchecked access to
  absent OpenFHE precomputation rows.
- `RCB(pair) = q_div * high + low`; complete validation must precede raw member
  access/arithmetic and the operation must not mutate the pair.
- The module distinguishes OpenFHE's recorded ciphertext scale from the paper's
  recombined-pair scale. Do not collapse those contracts.
- Do not weaken exact context identity, CKKS encoding, key tag, ordered basis
  and tower parameters, level, scale, noise-scale degree, evaluation format,
  component-count, divisor, lifecycle, and private-manifest validation.
- Do not modify official OpenFHE, introduce a fork, add a dependency, change
  FIXEDMANUAL, add production `try`/`catch`, or broaden the public API.
- KISS and YAGNI apply. A concrete local defect should receive only its smallest
  defensible remediation.

## Required review questions

1. Does the whole-object equality coverage on the existing clone snapshots
   fully close your sole P1 for all state exposed by OpenFHE 1.5.0's equality
   contract? Identify any exact residual gap; do not request redundant
   field-by-field assertions unless equality omits a relevant field.
2. Is the new slot-count pair manifest/validation semantically necessary and
   correctly placed for DCP/RCB, or is it unnecessary state? Check preservation,
   cross-member validation, error ordering, and whether it can reject a valid
   DCP/RCB use.
3. Review the complete supplied diff
   `a3df1c5843e8bb843f8d9becc3c8a135ffba63cd..02b34bac9cb87afc8acb9df275d5c0e137b554e7`.
   Did either change introduce a P0/P1 algorithm, invariant, API, test-oracle,
   portability, or integration defect?
4. For this exact current commit only, is the DCP/RCB slice `MERGEABLE`,
   `NEEDS NAMED FIXES`, or `NOT MERGEABLE`? A `MERGEABLE` verdict is still
   conditional on the separately required same-commit Windows ZCode/Zima review
   and does not certify any later multiplication operation.

Classify every finding as P0, P1, P2, or P3 and distinguish observed source
facts, derived conclusions, and unverified claims. Avoid hypothetical findings
without a reachable failure or violated stated contract.

## Required returned deliverables

Return one ZIP containing:

1. `REMEDIATION-REVIEW.md` — lead with the exact bounded verdict, P0/P1 counts,
   answers to the four questions, and any finding with exact file/line, proof,
   impact, and smallest remediation.
2. `REMEDIATION-CONTRACT-MAP.md` — pass/fail/uncertain map for prior-P1 closure,
   slot preservation/validation, DCP/RCB arithmetic non-regression, validation
   ordering, and input immutability.
3. `REMEDIATION-TEST-GAPS.md` — only concrete remaining gaps for this exact
   DCP/RCB slice, or an explicit statement that none remain at P0/P1.
4. `EXECUTION.md` — commands actually run, environment, exit status, test
   counts, timeouts, and every check not run. Inspection is not execution.
5. `0001-remediation-fixes.patch` only if a concrete current-commit defect needs
   a change. It must apply to exact commit `02b34bac...`, remain inside DCP/RCB,
   and must not weaken a test merely to make it pass.

Include a SHA-256 for the returned ZIP in the response. Do not rely on a later
message to supply missing context.

## Prohibited operations and claims

- Do not implement, sketch, or review pair Add/Sub, Tensor2, Relin2, RS2,
  Mult2, refresh, serialization, `t>2`, performance work, compatibility layers,
  or speculative extension points.
- Do not access old/private/local 2023/1788 code or modified OpenFHE trees.
- Do not perform or discuss a network-security assessment. This is strictly an
  algorithm and integration-correctness task.
- Do not commit, push, open a PR, dispatch/rerun CI, use credentials, or inspect
  unrelated local files.
- Do not claim local build, CTest, Windows, precision, performance, or security
  results unless actually executed and recorded in `EXECUTION.md`.
- Do not reinterpret retained GitHub Actions evidence as a local execution
  claim.

## Acceptance standard

The review is accepted only if it is bound to exact current commit `02b34bac...`,
explicitly resolves both the prior P1 and the slot-invariant judgment, reviews
the whole delta rather than only the three new equality lines, separates actual
execution from inspection, and makes no claim about later algorithms or the
pending Windows ZCode/Zima same-commit review.
