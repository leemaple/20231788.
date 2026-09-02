# Relin2 malformed-BV-nonzero-digit-A-length runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV nonzero-digit evaluation-key A-vector decomposition-count
contract is red on Linux and Windows**. Both warning-clean project builds and
both compile-only Relin2 public API-contract builds pass; exactly the newly
registered runtime case is red against the old scaffold. This is not a
whole-Relin2 arithmetic result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `36d88dd731ec1e8c48cb90ca36e6ddc6f91c6486`;
- parent: `3411d65e752272a70d6dc147e8e7239014221196`;
- tree: `42e3a9f056db4b30106137e90d1497e6c568b436`;
- local, remote-tracking, and live remote implementation SHA matched after the
  non-force push.

Only `CMakeLists.txt` and `tests/relin2_test.cpp` changed: 173 insertions and no
deletions. Production headers and source are byte-unchanged from the accepted
BV digit-size-zero B-length green.

The twenty-first fixture uses public OpenFHE parameters to construct a real BV,
`digitSize==10` context with the complete four-tower `Q` basis. It independently
derives the expected A/B decomposition count using integer ceiling on every
bound full-Q modulus MSB. It also freezes the ordered MSB manifest as
`{35, 31, 30, 31}` and the expected count as 15, so parameter-generator drift
fails rather than silently weakening the oracle. Public `EvalMultKeyGen`
produces exactly one `EvalKeyRelinImpl` whose A and B vectors both have 15
entries.

The valid-key positive control first reaches and accepts the existing scaffold,
then completes immediate Tensor/deep-metadata, A/B polynomial,
cache-map/vector/key-pointer, context, and tag invariance checks. The negative
control copies the generated A vector, removes only its last entry, and installs
the shortened 14-entry A vector while retaining the complete 15-entry B vector.
The fixture rechecks that the A prefix and B vector are unchanged. Snapshots are
taken after that deliberate mutation and immediately before the production
call. The exact expected diagnostic is
`DoubleCKKS: Relin2 evaluation key BV A vector length mismatch`. Spec, TDD, and
Delivery/API read-only source reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33592368406`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33592368406`;
- exact head SHA: `36d88dd731ec1e8c48cb90ca36e6ddc6f91c6486`;
- terminal run state: `completed/failure`, caused only by the intended new red.

Linux job `100128793337` and Windows/MSYS2 MinGW64 job `100128793139` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded. CTest
then reported exactly `20/21` passing on each platform: tests 1 through 20
passed and only `relin2_key_bv_nonzero_digit_a_length` failed. Linux reported
0.17 seconds; Windows reported 0.40 seconds. Both platforms produced the same
exact failing observation:

`Relin2 test failure: Relin2 BV nonzero-digit A-vector length threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

The failure is inside the exact-diagnostic helper, so this red receipt does not
claim that the negative case's later post-call invariance checks executed; the
paired green must execute them. The valid positive control ran earlier in that
same test and passed, proving the fixture is not an unconditional BV rejection.

The GitHub Node.js 20 deprecation annotation concerns the hosted Action runtime,
and the Windows package-manager message is an MSYS2 installation notice.
Neither is a compiler warning from the warning-clean project build.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both full
job records and aggregate logs, the complete logs ZIP, exact workflow, and
source identity. The ZIP has 30 unique, unencrypted, path-safe members and
passes `unzip -t`; its aggregate job logs are byte-identical to the separately
retained logs. The uploaded artifact count is zero.

The retained raw download set, a fresh expansion of the ZIP, and the final
retained directory all passed Gitleaks 8.30.1 and independent targeted
filename/content scans. No matching secret text is retained. Every retained
file except the manifest itself is bound by `MANIFEST.sha256`.

The malformed BV nonzero-digit A-vector decomposition-count contract is
accepted red on Linux and Windows. The green change may only compute the
expected decomposition count from the bound context's complete-Q modulus MSBs
using integer ceiling when the technique is BV and digit size is nonzero,
compare it with the already-cast key's A length, and emit the single exact
project diagnostic. It must leave the B-vector contract, key-entry basis/format,
ciphertext raising, key switching/relinearization, arithmetic, metadata, and the
old scaffold untouched.
