# Engineering constraints and verification

Read this reference before modifying implementation, tests, build files, or CI.

## Derive target behavior

1. Do not inspect or reuse pre-existing local project/OpenFHE implementation code. Locate exact paper sections, notation, algorithms, assumptions, and parameter regime.
2. Map each paper operation to official pristine OpenFHE 1.5.0 APIs or a small project-owned abstraction. Record the upstream tag/commit, deviations, and unsupported assumptions.
3. Define executable independent oracles and acceptance vectors before implementation. Cover representative real/complex slots, signs, near-zero values, supported magnitudes, repeated multiplication/lifecycle behavior, and paper-derived precision limits.
4. Keep cryptographic correctness, approximate-number semantics, and performance claims separate.

## Red-green-refactor

1. Write the smallest test for missing behavior and retain its initial red output.
2. Implement the smallest clear change that makes it pass.
3. Run the focused test, relevant suite, then configured CI/build checks.
4. Refactor only while tests stay green. Avoid speculative framework, duplicate representations, and premature optimization.

Use deterministic property tests or generated vectors backed by an independent plaintext/big-integer oracle where useful. Fix seeds and record parameters.

## Boundaries and resources

- Build on OpenFHE rather than reimplementing its cryptographic primitives.
- Prefer supported/public APIs. Prove and isolate the smallest upstream patch if one is essential.
- Prefer explicit control flow and narrow interfaces. Add only capabilities demanded by a current acceptance test.
- Fail loudly on invariant violations. Catch only at a boundary that can recover, add necessary context, or translate into a specified result.
- Run sustained OpenFHE builds and cryptographic tests on Windows or GitHub Actions by default. Keep Mac commands bounded and low-concurrency.

## Review

Verify equation/algorithm correspondence; numeric representation; parameter/key/ciphertext/tower/domain/scale/level/component handling; correctness and negative coverage; overflow, precision loss, nondeterminism, platform assumptions; public API impact; and unsupported claims. Record exact commands, environments, outcomes, and skipped checks.

