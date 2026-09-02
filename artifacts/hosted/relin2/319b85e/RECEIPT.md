# Relin2 malformed-BV-zero-digit-entry-basis runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV zero-digit evaluation-key entry-basis contract has an
independent red on Linux and Windows**. Both warning-clean project builds and
both compile-only Relin2 public API-contract builds succeeded. Exactly the new
23rd runtime test failed on both platforms by reaching the already accepted
Relin2 scaffold. This is intended TDD-red evidence, not a whole-Relin2 result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `319b85e8f68e0b5f844fff0feac36066606e4e4c`;
- parent: `3015f3f14e4bfc457bb702d5f1050a99b7697d90`;
- tree: `c56f4563e35f63a7f720cde0c34c9a7d767f2425`;
- local, remote-tracking, and live remote implementation SHA matched after the
  non-force push.

Relative to the accepted 22/22 source, only `CMakeLists.txt` and
`tests/relin2_test.cpp` changed: 237 insertions and 3 message-only deletions.
The new `relin2_key_bv_zero_digit_entry_basis` case uses BV key switching with
`digitSize == 0`. It first proves that generated A/B entries have the full,
ordered four-tower Q basis in Evaluation format. Its durable positive control
replaces only the last B entry with an independently allocated aggregate and
per-tower parameter structure that is semantically identical, then requires
the current production path to reach the accepted scaffold while immediately
checking Tensor, metadata, key vectors, cache, context, and tag invariance.

The negative control resets from the generated key and swaps only the first two
complete towers of the final B entry. It preserves A/B lengths, Evaluation
format, context, tag, subtype, all unswapped tower values, and every earlier B
entry. It requires exact `std::invalid_argument` text
`DoubleCKKS: Relin2 evaluation key BV entry basis mismatch`, followed by the
same deep immutability checks and final cache restoration. Existing helper
messages were generalized from a HYBRID-specific basis name to "expected
basis" without changing helper behavior. Spec, TDD, and Delivery/API read-only
source reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33599676266`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33599676266`;
- exact head SHA: `319b85e8f68e0b5f844fff0feac36066606e4e4c`;
- terminal run state: `completed/failure`, as required for this red boundary.

Linux job `100150374720` and Windows/MSYS2 MinGW64 job `100150374465` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded. CTest
reported exactly `22/23` passing on both platforms. The only failure was
`relin2_key_bv_zero_digit_entry_basis`, with the same exact line on each:
`Relin2 test failure: Relin2 BV zero-digit evaluation-key entry basis threw the
wrong exception type: DoubleCKKS: Relin2 is not implemented`. Linux reported
0.25 seconds and Windows 0.32 seconds. This proves that the valid independent
positive control still reaches the existing scaffold and that the isolated
malformed basis is not yet rejected by the required guard.

The GitHub Node.js 20 deprecation annotation concerns the hosted Action runtime,
not the project. The retained warning-clean build-step logs contain no compiler
warning diagnostic.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both full
job records and aggregate logs, the complete logs ZIP, exact workflow, and
source identity. The ZIP has 30 unique, unencrypted, path-safe members and
passes `unzip -t`; its aggregate job logs are byte-identical to the separately
retained logs. The uploaded artifact count is zero.

The raw download set and a fresh expansion of the ZIP passed Gitleaks 8.30.1
and independent targeted filename/content scans. No matching secret text is
retained. A final scan will run over the completed directory; every retained
file except the manifest itself will then be bound by `MANIFEST.sha256`.

This red is accepted only as provenance for the BV zero-digit entry-basis
boundary. The next production change must be a minimal BV `digitSize == 0`
full-Q ordered-basis guard. It must not yet broaden the guard to BV nonzero
digit size, validate entry format, raise ciphertexts, or implement Relin2
arithmetic, so the next independent BV nonzero-digit basis case can still
prove its own red.
