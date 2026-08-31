# Relin2 external-task audit

Prepared: 2026-09-01 Asia/Shanghai

## Bound task

- Task: `coordination/tasks/chatgpt-pro-relin2-01.md`.
- Exact implementation base:
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.
- Exact base tree: `759d5195739684748d5a9664edabe3fa719e1acf`.
- Final task size: 32,866 bytes / 610 lines.
- Final task SHA-256:
  `9323d631de4c6f6418ba1aa597332206a765846d0510a275df42a7a64b0a7513`.

## Independent audit axes

Two read-only reviewers independently checked the task against the accepted
Relin2 preflight and exact source base:

1. Paper/TDD audit: Definition 4.3, Lemma 4.4, logical-scale meanings,
   independent oracle/witness observability, ordered red-green evidence, and
   bounded scope.
2. Pristine OpenFHE 1.5.0/API audit: public relinearization, evaluation-key
   cache and concrete key shape, HYBRID/BV bases, tower restoration, metadata,
   warning-clean scaffold, and public-fixture feasibility.

Neither reviewer edited source, built, tested, or accessed an old
implementation.

## Findings resolved before final gate

- Split the Tensor2 lifecycle guard into a genuine post-Relin2 test-only red
  and a following minimal green guard; the dependency scaffold is no longer
  mislabeled as guard coverage.
- Corrected private DCP wording to one helper invocation and exactly one
  `DropLastElementAndScale` call per each of the two DCRTPoly components.
- Made post-relinearization witnesses mandatory and executable: named nonzero
  `K0`/`K1`, `v`/`w`, and deterministic centered remainder/carry cases. Missing
  witnesses prevent `ready to apply`.
- Separated black-box diagnostic/immutability evidence from source-order
  claims for validation before clone, raise, key switching, and arithmetic.
- Moved Tensor2's equal-valued recombined-field source proof to equation-to-code
  review while retaining runtime corruption rejection.
- Corrected the complete-validation wording to allow validation-owned element
  inspection before non-validation access.
- Corrected the BV digit-size source anchor and integer ceiling expression, and
  required both zero and nonzero digit-size coverage.
- Required an unnamed scaffold definition parameter under `-Wextra -Werror`.
- Defined publicly observable key-switch contribution residues and added a
  valid index-zero plus malformed later-key success case, proving later keys
  are ignored rather than merely proving vector size may exceed one.

## Final gate

- Paper/TDD reviewer: **PASS**.
- OpenFHE/API reviewer: **PASS**.

This gate approves the task definition for packaging. It is not a Relin2
implementation, compile, runtime-red, green, CI, precision, performance, or
security result.
