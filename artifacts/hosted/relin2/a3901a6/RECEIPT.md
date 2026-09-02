# Relin2 malformed-BV-zero-digit-A-length runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV digit-size-zero evaluation-key A-vector-length contract is red
on Linux and Windows**. Both warning-clean project builds and both compile-only
Relin2 public API-contract builds pass; exactly the newly registered runtime
case is red against the old scaffold. This is not a whole-Relin2 arithmetic
result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `a3901a6703d463d4ac52b089a4da39995bbde422`;
- parent: `b9f26db29b53764930798340f4ebe9bed789a323`;
- tree: `2cc8db125b2105d93409c8d771cc4d3bd355536d`;
- local and remote implementation SHA matched after non-force push.

Only `CMakeLists.txt` and `tests/relin2_test.cpp` changed. Production headers
and source are byte-unchanged from the accepted HYBRID entry-format green. The
nineteenth fixture uses public OpenFHE parameters to construct a real BV,
`digitSize==0` context with a four-tower full `Q` basis. Public
`EvalMultKeyGen` produces one `EvalKeyRelinImpl` whose A and B vectors both
have the required full-`Q` digit count of four.

The valid-key positive control first reaches and accepts the old scaffold,
then completes immediate Tensor, deep metadata, A/B polynomial,
cache-map/vector/key-pointer, context, and tag invariance checks. The negative
control copies the generated A vector and removes only its last entry, changing
A from four entries to three while leaving B, context, tag, concrete subtype,
cache shape, and Tensor valid. Snapshots are taken after that deliberate
mutation and immediately before the production call. The exact expected
diagnostic is
`DoubleCKKS: Relin2 evaluation key BV A vector length mismatch`. Spec, TDD,
and Delivery/API read-only source reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33586601051`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33586601051`;
- exact head SHA: `a3901a6703d463d4ac52b089a4da39995bbde422`;
- terminal run state: `completed/failure`, caused only by the intended new red.

Linux job `100111946283` and Windows/MSYS2 MinGW64 job `100111946109` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded. CTest
then reported exactly `18/19` passing on each platform: tests 1 through 18
passed and only `relin2_key_bv_zero_digit_a_length` failed. Linux reported 0.25
seconds; Windows reported 0.41 seconds. Both platforms produced the same exact
failing observation:

`Relin2 test failure: Relin2 BV zero-digit A-vector length threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

The failure is inside the new exact-diagnostic helper, so this red receipt does
not claim that the negative case's later post-call invariance checks executed;
the paired green must execute them. The valid positive control ran earlier in
that same test and passed, proving the fixture is not an unconditional BV
rejection.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both full
job records/logs, complete logs ZIP, exact workflow, and source identity. The
ZIP passes `unzip -t`; the uploaded artifact count is zero. Both the retained
directory and a fresh expansion of the ZIP passed Gitleaks 8.30.1 and
independent targeted filename/content scans. Every retained file except the
manifest itself is bound by `MANIFEST.sha256`.

The malformed BV digit-size-zero A-vector-length contract is accepted red on
Linux and Windows. The green change may only compare the already cast key's A
length with the bound context's complete-`Q` tower count when the technique is
BV and digit size is zero, then emit the single exact project diagnostic. It
must leave nonzero-digit BV, B-vector validation, key-entry basis/format,
ciphertext raising, key switching/relinearization, arithmetic, metadata, and
the old scaffold untouched.
