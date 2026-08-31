# ChatGPT Pro 01 — clean-room OpenFHE Mult2

## Objective

Design and write from scratch a minimal C++17 implementation of the `t=2` Double-CKKS multiplication construction in Cheon–Cho–Kim–Stehle, *Homomorphic Multiple Precision Multiplication for CKKS and Reduced Modulus Consumption* (CCS 2023 / IACR ePrint 2023/1788), for official pristine OpenFHE 1.5.0.

The destination is `https://github.com/leemaple/20231788`, but its prior implementation is wrong and deliberately excluded. Do not inspect, request, reproduce, or adapt it or any related local code. Use only the supplied paper, official pristine OpenFHE 1.5.0 source/documentation, this brief, and greenfield files in the package. This is the paper's higher-precision CKKS method, not IEEE-754 binary64 arithmetic.

## Boundaries

- Empty starting codebase: invent the smallest cohesive project-owned interface based on the paper, not a historical API.
- Prefer supported/public OpenFHE APIs. Record exact upstream tag/commit and source links. If a required primitive is unavailable, prove the gap and separately propose the smallest upstream patch and tests.
- Scope: DCP, RCB, Tensor2, Relin2, RS2, Mult2, and only pair add/subtract required by tests.
- Out: `t>2`, bootstrapping, GPU, production benchmarks, compatibility shims, speculative frameworks.
- TDD, KISS, YAGNI, fail-fast. No catch-all exception handling.

## Research and design

1. Re-derive each operation from the paper, stating plaintext meaning, ciphertext component count, RNS basis/tower order, coefficient/NTT form, level, scale, rounding, and error terms at input/output.
2. Derive the exact `Delta`, `q_div`, and `q_l` relationship dimensionally; do not inherit prior conclusions.
3. Map required primitives to official OpenFHE 1.5.0 declarations/implementations with citations.
4. Give a concrete prime-role schedule and transition table for one multiplication and either the next multiplication or an explicit refresh boundary.
5. Identify HEaaN assumptions in the paper prototype that differ from OpenFHE.

## Deliverables

- `DESIGN.md` with equation/API mapping, invariants, tower/level/scale tables, correctness risks, and any proven upstream gap.
- Complete minimal CMake/C++17 headers/source/tests for a new empty branch.
- Red-first evidence and exact commands/output for tests actually run; clearly mark unexecuted claims.
- Deterministic independent plaintext/big-integer/CRT oracles; never use the implementation to compute expected values.
- `REVIEW.md` classifying observed, inferred, and pending findings, plus deferred work.

## Mandatory tests

- Prime-role/tower manifest and lifecycle.
- DCP/RCB coefficient reconstruction against signed big-integer arithmetic.
- DCP/RCB encrypted-message oracle on fixed real/complex slots, signs, zero/near-zero, and supported magnitudes.
- Coefficient-versus-NTT format invariants.
- Level, scale, basis, and component metadata after every public operation.
- Relin2 component/key checks and RS2 independent rounding/rescale oracle.
- End-to-end `decompose -> mult2 -> recombine -> decrypt` with a paper-derived precision threshold.
- Second-multiplication lifecycle, or an executable fail-fast refresh-boundary test.
- Negative cases for incompatible parameters/bases/levels/scales, missing keys/primes, and malformed pairs.
- Warning-enabled build with warnings reported.

Fix seeds where OpenFHE permits it and print enough transition context to diagnose failures.

## Prohibited operations and claims

Do not access old/local implementation code; assume no filesystem/private-repository/prior-chat/browser access; do not push, merge, open PRs, change GitHub settings, or run Actions; do not request credentials or browser/runtime state; do not modify OpenFHE silently; do not weaken acceptance thresholds; do not claim builds/tests/precision/performance/security not actually observed.

## Acceptance

All code is greenfield; every nontrivial operation has a retained red-first independent-oracle test; design tracks all modulus/scale/level/domain/component transitions; an actual runner builds against pristine OpenFHE 1.5.0 and passes the accepted slice without hidden precision warnings or invalid metadata; the next-level lifecycle is executable or explicitly tested as a boundary; uncertainty is explicit and testable.

