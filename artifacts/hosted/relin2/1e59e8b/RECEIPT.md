# Relin2 BV nonzero-digit entry-format runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV nonzero-digit evaluation-key entry-format contract is green
on Linux and Windows**. Both warning-clean project builds and both compile-only
Relin2 public API-contract builds pass, followed by exactly `26/26` passing
CTest cases on each platform. This is not a whole-Relin2 arithmetic result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `1e59e8b36d5119ceb2b463922f1053e03a029bd4`;
- parent: `09ee350f60bad6f64f03339ac7f0429cc26fcbb3`;
- tree: `4e3a8b4857aeb8f5f7ef07dd2f01b5f74079ba77`;
- local, remote-tracking, and live remote implementation SHA matched after the
  non-force push.

Only `src/double_ckks.cpp` changed from the accepted BV nonzero-digit
entry-format red source: one line inserted and one line deleted. The existing
BV format guard was broadened from `BV && digitSize == 0` to all BV. Its
placement, all-A-then-all-B traversal, `IsInEvaluationFormat` helper, and exact
`DoubleCKKS: Relin2 evaluation key BV entry must be in evaluation format`
diagnostic are unchanged. The separate zero- and nonzero-digit A/B length
checks and the HYBRID checks are unchanged. No public header, test, CMake
registration, ciphertext arithmetic, metadata, cache, or exception-catching
code changed. Spec, TDD, and Delivery/API read-only reviews each returned
`PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33615958354`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33615958354`;
- exact head SHA: `1e59e8b36d5119ceb2b463922f1053e03a029bd4`;
- terminal run state: `completed/success`.

Linux job `100201705002` and Windows/MSYS2 MinGW64 job `100201705264` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded.
CTest then reported exactly `26/26` passing on each platform, including both
BV zero- and nonzero-digit entry-format cases. Linux reported 0.18 seconds and
Windows reported 0.52 seconds. This closes the preceding `09ee350` red while
preserving all 25 inherited behaviors.

The GitHub Node.js 20 deprecation annotation concerns the hosted Action runtime,
not the project. The warning emitted by the MSYS2 toolchain installer asks that
other MSYS2 programs be closed; it is not a compiler warning from the project.
The warning-clean project build steps succeeded on both platforms. The Linux
runner spent several minutes in `apt-get` for the independent oracle dependency
before progressing normally; no retry or rerun was used.

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

The BV nonzero-digit entry-format boundary is accepted green on Linux and
Windows. Together with the earlier HYBRID and BV key-shape, basis, and format
boundaries, the frozen shape/basis/format matrix is complete. Two separately
required later-key success semantics, `TestExtraLaterValid` and
`TestMalformedLaterIgnored`, remain unproven and must be closed before Relin2
arithmetic. The next work must begin as an isolated later-key TDD boundary and
must not retroactively weaken these checks.
