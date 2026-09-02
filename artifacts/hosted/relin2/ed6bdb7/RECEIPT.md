# Relin2 malformed-BV-zero-digit-entry-basis runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV zero-digit evaluation-key entry-basis contract is green on
Linux and Windows**. Both warning-clean project builds, both compile-only
Relin2 public API-contract builds, and all 23 registered runtime tests pass.
This is not a whole-Relin2 arithmetic result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `ed6bdb77f893bb89409059a5e4179686ddbd66d4`;
- parent: `319b85e8f68e0b5f844fff0feac36066606e4e4c`;
- tree: `2372d5aa2a93c7f025e7c1c2ae1df21c341432e7`;
- local, remote-tracking, and live remote implementation SHA matched after the
  non-force push.

Only `src/double_ckks.cpp` changed from the accepted red source: 13 inserted
lines. After the already accepted BV `digitSize == 0` A/B length checks and
before any nonzero-digit, HYBRID, format, raising, or arithmetic work, production
uses the existing value-semantic `HasCompleteOrderedBasis` helper against
`parameters_->GetElementParams()`. Every A and B entry must retain the full,
ordered Q basis at both its aggregate parameters and every actual NativePoly
tower. Either vector emits exactly
`DoubleCKKS: Relin2 evaluation key BV entry basis mismatch` on a mismatch.

The guard deliberately remains restricted to BV `digitSize == 0`, so the next
BV nonzero-digit basis behavior can retain its own independent red. It accepts
the test's independently allocated but semantically equal aggregate and
per-tower parameters; it rejects the isolated swap of the first two complete
towers in the final B entry. The change does not inspect pointer identity, add
format validation, alter any test or public header, raise ciphertexts, touch
metadata, modify the evaluation-key cache, call `Relinearize`, or implement
Relin2 arithmetic. It adds no catch block or abstraction. Spec, TDD, and
Delivery/API read-only source reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33601661404`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33601661404`;
- exact head SHA: `ed6bdb77f893bb89409059a5e4179686ddbd66d4`;
- terminal run state: `completed/success`.

Linux job `100156481823` and Windows/MSYS2 MinGW64 job `100156482101` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded. CTest
then reported exactly `23/23` passing on each platform, including
`relin2_key_bv_zero_digit_entry_basis`; Linux reported 0.15 seconds and Windows
reported 0.45 seconds.

The independent-parameter positive control reached the accepted scaffold and
completed immediate Tensor, deep-metadata, A/B polynomial,
cache-map/vector/key-pointer, context, and tag invariance checks. The malformed
final-B-entry basis then produced the exact required diagnostic and completed
the same immutability checks. The final RAII check confirmed restoration of the
initially empty global evaluation-key cache.

The GitHub Node.js 20 deprecation annotation concerns the hosted Action runtime,
not the project. The warning emitted by the MSYS2 toolchain installer asks that
other MSYS2 programs be closed; it is not a compiler warning from the project.
The warning-clean project build steps succeeded on both platforms.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both full
job records and aggregate logs, the complete logs ZIP, exact workflow, and
source identity. The ZIP has 31 unique, unencrypted, path-safe members and
passes `unzip -t`; its aggregate job logs are byte-identical to the separately
retained logs. The uploaded artifact count is zero.

The raw download set and a fresh expansion of the ZIP passed Gitleaks 8.30.1
and independent targeted filename/content scans. No matching secret text is
retained. A final scan will run over the completed directory; every retained
file except the manifest itself will then be bound by `MANIFEST.sha256`.

The malformed BV zero-digit entry-basis red/green boundary is accepted green on
Linux and Windows. The next isolated TDD boundary is the BV nonzero-digit entry
basis. It must start from this exact source commit, prove a separate red before
production broadens the guard, and must not combine entry format, ciphertext
raising, or Relin2 arithmetic.
