# Relin2 malformed-BV-zero-digit-B-length runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV digit-size-zero evaluation-key B-vector-length contract is red
on Linux and Windows**. Both warning-clean project builds and both compile-only
Relin2 public API-contract builds pass; exactly the newly registered runtime
case is red against the old scaffold. This is not a whole-Relin2 arithmetic
result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `c9ba4e4068608d9929de62d2d010636e4d11b45f`;
- parent: `fd847ae5005c8eb179e9d5799b78b2d555f4981c`;
- tree: `ed724ee6983e2b79112e18dd718a6ec184320652`;
- local, remote-tracking, and live remote implementation SHA matched after the
  non-force push.

Only `CMakeLists.txt` and `tests/relin2_test.cpp` changed: 150 insertions and no
deletions. Production headers and source are byte-unchanged from the accepted
BV digit-size-zero A-length green. The twentieth fixture uses public OpenFHE
parameters to construct a real BV, `digitSize==0` context with a four-tower
complete `Q` basis. Public `EvalMultKeyGen` produces one
`EvalKeyRelinImpl` whose A and B vectors both have the required complete-`Q`
digit count of four.

The valid-key positive control first reaches and accepts the existing scaffold,
then completes immediate Tensor, deep metadata, A/B polynomial,
cache-map/vector/key-pointer, context, and tag invariance checks. The negative
control copies the generated B vector and removes only its last entry, changing
B from four entries to three while leaving A, context, tag, concrete subtype,
cache shape, and Tensor valid. Snapshots are taken after that deliberate
mutation and immediately before the production call. The exact expected
diagnostic is
`DoubleCKKS: Relin2 evaluation key BV B vector length mismatch`. Spec, TDD, and
Delivery/API read-only source reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33588981664`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33588981664`;
- exact head SHA: `c9ba4e4068608d9929de62d2d010636e4d11b45f`;
- terminal run state: `completed/failure`, caused only by the intended new red.

Linux job `100118896875` and Windows/MSYS2 MinGW64 job `100118897067` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded. CTest
then reported exactly `19/20` passing on each platform: tests 1 through 19
passed and only `relin2_key_bv_zero_digit_b_length` failed. Linux reported 0.20
seconds; Windows reported 0.42 seconds. Both platforms produced the same exact
failing observation:

`Relin2 test failure: Relin2 BV zero-digit B-vector length threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

The failure is inside the exact-diagnostic helper, so this red receipt does not
claim that the negative case's later post-call invariance checks executed; the
paired green must execute them. The valid positive control ran earlier in that
same test and passed, proving the fixture is not an unconditional BV rejection.

The GitHub Node.js 20 deprecation annotation concerns the hosted Action runtime,
and the Windows package-manager message is an MSYS2 installation notice.
Neither is a compiler warning from the warning-clean project build.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both full
job records/logs, the complete logs ZIP, exact workflow, and source identity.
The ZIP has 30 unique, unencrypted, path-safe members and passes `unzip -t`;
its aggregate job logs are byte-identical to the separately retained logs. The
uploaded artifact count is zero.

The retained raw download set, a fresh expansion of the ZIP, and the final
retained directory all passed Gitleaks 8.30.1 and independent targeted
filename/content scans. No matching secret text is retained. Every retained
file except the manifest itself is bound by `MANIFEST.sha256`.

The malformed BV digit-size-zero B-vector-length contract is accepted red on
Linux and Windows. The green change may only compare the already cast key's B
length with the bound context's complete-`Q` tower count when the technique is
BV and digit size is zero, then emit the single exact project diagnostic. It
must leave nonzero-digit BV, key-entry basis/format, ciphertext raising, key
switching/relinearization, arithmetic, metadata, and the old scaffold untouched.
