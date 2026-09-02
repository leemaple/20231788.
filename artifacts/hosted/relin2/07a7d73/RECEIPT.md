# Relin2 malformed-BV-nonzero-digit-entry-basis runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV nonzero-digit evaluation-key entry-basis contract is green on
Linux and Windows**. Both warning-clean project builds, both compile-only
Relin2 public API-contract builds, and all 24 registered runtime tests pass.
This is not a whole-Relin2 arithmetic result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `07a7d734b6922f2810218e091b80648cf88dd0c2`;
- parent/red source: `9c40f801208f27d013d285e18a38fc8c65e41611`;
- tree: `29fa87d51bbcf79a715abd7ede4b30c6ba71faf3`;
- local, remote-tracking, and live remote implementation SHA matched after the
  non-force push.

Only `src/double_ckks.cpp` changed from the accepted red source: one line was
replaced. The existing BV full ordered-Q basis guard now applies to every BV
evaluation key instead of only `digitSize == 0`. It continues to use the
value-semantic `HasCompleteOrderedBasis` helper against
`parameters_->GetElementParams()` for every A and B entry and emits exactly
`DoubleCKKS: Relin2 evaluation key BV entry basis mismatch` on either mismatch.

The change adds no helper, catch block, test, public API, format validation,
ciphertext raising, metadata operation, cache mutation, `Relinearize` call, or
Relin2 arithmetic. It leaves the separate zero- and nonzero-digit A/B length
guards intact. Spec, TDD, and Delivery/API read-only source reviews each
returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33606983286`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33606983286`;
- exact head SHA: `07a7d734b6922f2810218e091b80648cf88dd0c2`;
- terminal run state: `completed/success`.

Linux job `100173107106` and Windows/MSYS2 MinGW64 job `100173106804` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded. CTest
then reported exactly `24/24` passing on each platform, including
`relin2_key_bv_nonzero_digit_entry_basis`; Linux reported 0.24 seconds and
Windows reported 0.45 seconds.

The new test's independently allocated, value-equal A-entry basis reached the
accepted scaffold and completed immediate Tensor, A/B polynomial,
cache-map/vector/key-pointer, context, and tag invariance checks. The isolated
first-two-tower swap in the final A entry then produced the exact required
diagnostic and completed the same immutability checks. All earlier BV
zero/nonzero length, zero-digit basis, HYBRID, key-shape, and Tensor behaviors
remained green. The final RAII check confirmed restoration of the initially
empty global evaluation-key cache.

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
retained. The completed directory also passes the same final scans; every
retained file except the manifest itself is bound by `MANIFEST.sha256`.

The BV nonzero-digit full ordered-Q entry-basis red/green boundary is accepted
and closed on Linux and Windows. The next isolated TDD boundary is BV
evaluation-key entry format. It must start from this exact source commit and
must not combine ciphertext raising, metadata changes, or Relin2 arithmetic.
