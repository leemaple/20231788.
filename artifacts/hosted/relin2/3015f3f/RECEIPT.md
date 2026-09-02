# Relin2 malformed-BV-nonzero-digit-B-length runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV nonzero-digit evaluation-key B-vector decomposition-count
contract is green on Linux and Windows**. Both warning-clean project builds,
both compile-only Relin2 public API-contract builds, and all 22 registered
runtime tests pass. This is not a whole-Relin2 arithmetic result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `3015f3f14e4bfc457bb702d5f1050a99b7697d90`;
- parent: `34678817fe863d2759e07b6358ffff2feaaf9d02`;
- tree: `51f150ca46d8ee068e8b5534710c330441ffc54a`;
- local, remote-tracking, and live remote implementation SHA matched after the
  non-force push.

Only `src/double_ckks.cpp` changed from the accepted red source: six inserted
lines and three deleted lines. In the existing BV `digitSize != 0` branch, the
previous A-specific local count was renamed to a shared vector count. The
already accepted integer-ceiling sum over the bound complete-Q modulus MSBs is
still compared with A first; production now also compares the same count with
the already-cast relinearization key's B-vector size. A B mismatch emits
exactly
`DoubleCKKS: Relin2 evaluation key BV B vector length mismatch`.

The change does not add another decomposition rule or change validation
precedence. It does not validate BV entry basis/format or perform any Relin2
arithmetic. It does not alter tests, CMake, public headers, HYBRID validation,
ciphertext raising, key switching, relinearization, metadata, or the existing
scaffold. It adds no catch block or general-purpose abstraction. Spec, TDD, and
Delivery/API read-only source reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33596456570`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33596456570`;
- exact head SHA: `3015f3f14e4bfc457bb702d5f1050a99b7697d90`;
- terminal run state: `completed/success`.

Linux job `100140773937` and Windows/MSYS2 MinGW64 job `100140773851` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded. CTest
then reported exactly `22/22` passing on each platform, including
`relin2_key_bv_nonzero_digit_b_length`; Linux reported 0.19 seconds and Windows
reported 0.40 seconds.

The valid generated 15-entry A/B positive control reached the existing
scaffold and immediately completed Tensor, deep-metadata, A/B polynomial,
cache-map/vector/key-pointer, context, and tag invariance checks. The negative
control shortened only B from 15 entries to 14, observed the exact project
diagnostic, and then completed the same immutability checks. The final RAII
check confirmed that the initially empty global evaluation-key cache was
restored. The pinned full-Q MSB manifest `{35, 31, 30, 31}` and independently
derived count 15 also passed on both platforms.

The GitHub Node.js 20 deprecation annotation concerns the hosted Action runtime,
not the project. The retained warning-clean build-step logs contain no compiler
warning diagnostic.

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

The malformed BV nonzero-digit B-vector decomposition-count red/green boundary
is accepted green on Linux and Windows. The next isolated TDD boundary is the
BV evaluation-key entry basis. It must begin from this exact source commit,
introduce an independent red before production, and must not combine entry
format, ciphertext raising, or arithmetic.
