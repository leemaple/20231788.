# Independent Standards first pass

Reviewer context: `/root/partial_primitives_standards`. Requested selector `gpt-5.6-sol`, effort `high`; inference identity not attested (`requested-unverified`). This is an independent reasoning context, not proof of a different provider. It read the returned code and project standards; it changed no files and ran no tests.

The reviewer returned these findings:

1. **P1 — Horizontal slicing / no per-test RED.** `pro/patches/01-test-first.patch` adds the entire 23-method suite before `02-implementation.patch` adds the whole implementation. The supplied ledger records only a missing-module import error. None of the 23 individual behavior assertions has a captured RED. This does not satisfy the project's per-test RED/TDD requirement.
2. **P1 — Tautological formula tests.** `pro/files/tests/test_endpoint_evidence_primitives.py:64` through line 90 repeats the implementation allowance expressions, rather than using source-derived literal fixtures or a structurally independent oracle. A shared transcription error can stay green.
3. **P2 — Test bypasses the seam.** Test lines 39–44 exercise stdlib Fraction but never call the module's public interface; they cannot detect a module regression.
4. **P3 — Speculative generality.** Implementation lines 18 and 29–32 contain unused significant-digit, producer-transport and commit-identity constants; remove them if they have no accepted use.

No other meaningful standards or named-smell findings were reported. The author correctly labels the deliverable as a partial primitive module, not a completed observer.

## Root disposition at this checkpoint

Retain the return as provenance/reference, not an integrated GREEN deliverable. The root's 23-test replay is a reproducibility check only. It neither creates retrospective per-test RED history nor makes duplicated formulas independent. The Fraction-only assertion can be a sanity check, but must not count as coverage of the module or the absent C++ exact-difference helper.

The findings require proportionate disposition before accepting reusable code: independently derived worked fixtures and genuine future test-first slices are useful; repeatedly replaying the same import failure is not. No assertion that the formulas are numerically incorrect follows from a test-independence finding alone. Full Spec review remains pending; a separate reviewer could not start because of the agent thread limit.
