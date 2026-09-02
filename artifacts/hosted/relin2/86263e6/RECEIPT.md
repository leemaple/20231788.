# Relin2 malformed-BV-nonzero-digit-A-length runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV nonzero-digit evaluation-key A-vector decomposition-count
contract is green on Linux and Windows**. Both warning-clean project builds,
both compile-only Relin2 public API-contract builds, and all 21 registered
runtime tests pass. This is not a whole-Relin2 arithmetic result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `86263e6d87899309981a10a0d108a07fdefb2251`;
- parent: `36d88dd731ec1e8c48cb90ca36e6ddc6f91c6486`;
- tree: `70230784f1a1aa8ec54b9956f4af4b2c6a1df56e`;
- local, remote-tracking, and live remote implementation SHA matched after the
  non-force push.

Only `src/double_ckks.cpp` changed from the accepted red source: 11 added lines
and no deletions. After Tensor, active-basis, cache, key-presence, context, tag,
concrete-subtype, and accepted BV digit-size-zero A/B validation, production
now handles only the BV nonzero-digit A-vector count. It sums integer ceiling
`(MSB + digitSize - 1) / digitSize` over every modulus in the bound context's
complete-Q parameters and compares that count with the already-cast
relinearization key's A-vector size. A mismatch emits exactly
`DoubleCKKS: Relin2 evaluation key BV A vector length mismatch`.

The `digitSize != 0` branch isolates the division from the accepted zero-digit
path. The change does not validate the corresponding B-vector count, BV entry
basis/format, or any Relin2 arithmetic. It does not alter tests, CMake, public
headers, HYBRID validation, ciphertext raising, key switching,
relinearization, metadata, or the existing scaffold. Spec, TDD, and
Delivery/API read-only source reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33593842748`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33593842748`;
- exact head SHA: `86263e6d87899309981a10a0d108a07fdefb2251`;
- terminal run state: `completed/success`.

Linux job `100133145890` and Windows/MSYS2 MinGW64 job `100133145698` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded. CTest
then reported exactly `21/21` passing on each platform, including
`relin2_key_bv_nonzero_digit_a_length`; Linux reported 0.19 seconds and Windows
reported 0.45 seconds.

The valid generated 15-entry A/B positive control reached the existing
scaffold and immediately completed Tensor, deep-metadata, A/B polynomial,
cache-map/vector/key-pointer, context, and tag invariance checks. The negative
control shortened only A from 15 entries to 14, observed the exact project
diagnostic, and then completed the same immutability checks. The final RAII
check confirmed that the initially empty global evaluation-key cache was
restored. The pinned full-Q MSB manifest `{35, 31, 30, 31}` and derived count 15
also passed on both platforms.

The GitHub Node.js 20 deprecation annotation concerns the hosted Action runtime,
and the Windows package-manager message is an MSYS2 installation notice.
Neither is a compiler warning from the warning-clean project build.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both full
job records and aggregate logs, the complete logs ZIP, exact workflow, and
source identity. The ZIP has 31 unique, unencrypted, path-safe members and
passes `unzip -t`; its aggregate job logs are byte-identical to the separately
retained logs. The uploaded artifact count is zero.

The retained raw download set, a fresh expansion of the ZIP, and the final
retained directory all passed Gitleaks 8.30.1 and independent targeted
filename/content scans. No matching secret text is retained. Every retained
file except the manifest itself is bound by `MANIFEST.sha256`.

The malformed BV nonzero-digit A-vector decomposition-count red/green boundary
is accepted green on Linux and Windows. The next isolated TDD boundary is the
BV nonzero-digit B-vector decomposition count. It must be introduced red-first
and must not combine entry basis/format, ciphertext raising, or arithmetic.
