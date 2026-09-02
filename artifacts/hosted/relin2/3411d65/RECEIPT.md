# Relin2 malformed-BV-zero-digit-B-length runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV digit-size-zero evaluation-key B-vector-length contract is
green on Linux and Windows**. Both warning-clean project builds, both
compile-only Relin2 public API-contract builds, and all 20 registered runtime
tests pass. This is not a whole-Relin2 arithmetic result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `3411d65e752272a70d6dc147e8e7239014221196`;
- parent: `c9ba4e4068608d9929de62d2d010636e4d11b45f`;
- tree: `572925d4819a32eefb258af1ed37b79deb551cc0`;
- local, remote-tracking, and live remote implementation SHA matched after the
  non-force push.

Only `src/double_ckks.cpp` changed from the accepted red source: four added
lines and no deletions. After Tensor, active-basis, cache, key-presence,
context, tag, concrete-subtype, and BV digit-size-zero A-length validation,
production now checks only the corresponding B-vector length. It compares the
already-cast relinearization key's B-vector length with the complete bound `Q`
tower count and emits exactly
`DoubleCKKS: Relin2 evaluation key BV B vector length mismatch` on mismatch.

The change does not validate nonzero-digit BV decomposition, BV entry
basis/format, or any Relin2 arithmetic. It does not alter tests, CMake, public
headers, HYBRID validation, ciphertext raising, key switching,
relinearization, metadata, or the existing scaffold.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33590046548`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33590046548`;
- exact head SHA: `3411d65e752272a70d6dc147e8e7239014221196`;
- terminal run state: `completed/success`.

Linux job `100122018402` and Windows/MSYS2 MinGW64 job `100122018678` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded. CTest
then reported exactly `20/20` passing on each platform, including
`relin2_key_bv_zero_digit_b_length`; Linux reported 0.18 seconds and Windows
reported 0.37 seconds.

The valid-key positive control reached the existing scaffold and immediately
completed Tensor, deep-metadata, A/B polynomial, cache-map/vector/key-pointer,
context, and tag invariance checks. The negative control shortened only B from
four entries to three, observed the exact project diagnostic, and then
completed the same immutability checks. The final RAII check confirmed that the
initially empty global evaluation-key cache was restored.

The GitHub Node.js 20 deprecation annotation concerns the hosted Action runtime,
and the Windows package-manager message is an MSYS2 installation notice.
Neither is a compiler warning from the warning-clean project build.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both full
job records/logs, the complete logs ZIP, exact workflow, and source identity.
The ZIP has 31 unique, unencrypted, path-safe members and passes `unzip -t`;
its aggregate job logs are byte-identical to the separately retained logs. The
uploaded artifact count is zero.

The retained raw download set, a fresh expansion of the ZIP, and the final
retained directory all passed Gitleaks 8.30.1 and independent targeted
filename/content scans. No matching secret text is retained. Every retained
file except the manifest itself is bound by `MANIFEST.sha256`.

The malformed BV digit-size-zero B-vector-length red/green boundary is accepted
green on Linux and Windows. The next isolated TDD boundary is the BV
nonzero-digit A-vector decomposition count. It must be introduced red-first and
must not combine the nonzero-digit B length, entry basis/format, ciphertext
raising, or arithmetic.
