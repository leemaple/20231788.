# Relin2 BV zero-digit entry-format runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV zero-digit evaluation-key entry-format contract is green on
Linux and Windows**. Both warning-clean project builds and both compile-only
Relin2 public API-contract builds pass, followed by exactly `25/25` passing
CTest cases on each platform. This is not a whole-Relin2 arithmetic result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `a22e35be2783f2a6883a28901d51ac557b9e550f`;
- parent: `6524e53b862120a144609b8782ee431d6de2946e`;
- tree: `952154805540361dd3bcab6d9509b488fc32ffa2`;
- local, remote-tracking, and live remote implementation SHA matched after the
  non-force push.

Only `src/double_ckks.cpp` changed from the accepted BV zero-digit
entry-format red source: 12 inserted lines and no deletion. The production
guard is restricted to `BV && digitSize == 0`, runs after the complete BV
basis and A/B length validation, checks every A entry followed by every B
entry with the existing `IsInEvaluationFormat` helper, and throws exactly
`DoubleCKKS: Relin2 evaluation key BV entry must be in evaluation format`.
No public header, test, CMake registration, ciphertext arithmetic, metadata,
cache, or exception-catching code changed. Spec, TDD, and Delivery/API
read-only reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33611919737`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33611919737`;
- exact head SHA: `a22e35be2783f2a6883a28901d51ac557b9e550f`;
- terminal run state: `completed/success`.

Linux job `100188829527` and Windows/MSYS2 MinGW64 job `100188829985` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded.
CTest then reported exactly `25/25` passing on each platform, including
`relin2_key_bv_zero_digit_entry_format`. Linux reported 0.23 seconds and
Windows reported 0.46 seconds. This closes the preceding `6524e53` red while
preserving all 24 inherited behaviors.

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
retained. The completed directory will pass the same final scans; every
retained file except the manifest itself will then be bound by
`MANIFEST.sha256`.

The BV zero-digit entry-format boundary is accepted green on Linux and Windows.
The next independent TDD boundary is BV with nonzero `digitSize` entry format;
it must begin with a dedicated red test and must not be folded into ciphertext
raising, metadata work, or Relin2 arithmetic.
