# Relin2 malformed-BV-nonzero-digit-B-length runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV nonzero-digit evaluation-key B-vector decomposition-count
contract is red on Linux and Windows**. Both warning-clean project builds and
both compile-only Relin2 public API-contract builds pass; exactly the newly
registered runtime case is red against the old scaffold. This is not a
whole-Relin2 arithmetic result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `34678817fe863d2759e07b6358ffff2feaaf9d02`;
- parent: `86263e6d87899309981a10a0d108a07fdefb2251`;
- tree: `2f4edf16e8744bf482d4b1c4e146eee8f9e03c08`;
- local, remote-tracking, and live remote implementation SHA matched after the
  non-force push.

Only `CMakeLists.txt` and `tests/relin2_test.cpp` changed: 173 insertions and no
deletions. Production headers and source are byte-unchanged from the accepted
BV nonzero-digit A-count green.

The twenty-second fixture uses public OpenFHE parameters to construct a real
BV, `digitSize==10` context with the complete four-tower `Q` basis. It derives
the expected A/B decomposition count independently using integer ceiling on
each bound full-Q modulus MSB. It also freezes the ordered MSB manifest as
`{35, 31, 30, 31}` and the expected count as 15, so parameter-generator drift
fails rather than silently weakening the oracle. Public `EvalMultKeyGen`
produces one `EvalKeyRelinImpl` whose A and B vectors both have 15 entries.

The valid-key positive control first reaches and accepts the existing scaffold,
then completes immediate Tensor/deep-metadata, A/B polynomial,
cache-map/vector/key-pointer, context, and tag invariance checks. The negative
control copies the generated B vector, removes only its last entry, and installs
the shortened 14-entry B vector while retaining the complete 15-entry A vector.
The fixture rechecks that the B prefix and A vector are unchanged. Snapshots are
taken after the deliberate mutation and immediately before the production
call. The exact expected diagnostic is
`DoubleCKKS: Relin2 evaluation key BV B vector length mismatch`. Spec, TDD, and
Delivery/API read-only source reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33595207012`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33595207012`;
- exact head SHA: `34678817fe863d2759e07b6358ffff2feaaf9d02`;
- terminal run state: `completed/failure`, caused only by the intended new red.

Linux job `100137152803` and Windows/MSYS2 MinGW64 job `100137152400` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded. CTest
then reported exactly `21/22` passing on each platform: tests 1 through 21
passed and only `relin2_key_bv_nonzero_digit_b_length` failed. Linux reported
0.18 seconds and Windows 0.37 seconds. Both platforms produced the same exact
failing observation:

`Relin2 test failure: Relin2 BV nonzero-digit B-vector length threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

The failure is inside the exact-diagnostic helper, so this red receipt does not
claim that the negative case's later post-call invariance checks executed; the
paired green must execute them. The valid positive control ran earlier in that
same test and passed, proving the fixture is not an unconditional BV rejection.

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
retained. A final scan will run after this receipt and manifest are complete;
every retained file except the manifest itself will then be bound by
`MANIFEST.sha256`.

The malformed BV nonzero-digit B-vector decomposition-count contract is
accepted red on Linux and Windows. The green change may only reuse the already
computed expected nonzero-digit BV decomposition count, compare it with the
already-cast key's B length, and emit the single exact project diagnostic. It
must leave the A-vector contract, key-entry basis/format, ciphertext raising,
key switching/relinearization, arithmetic, metadata, and the old scaffold
untouched.
