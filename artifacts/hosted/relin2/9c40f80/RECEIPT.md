# Relin2 malformed-BV-nonzero-digit-entry-basis runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV nonzero-digit evaluation-key entry-basis contract is proven
red on Linux and Windows**. Both warning-clean project builds and both
compile-only Relin2 public API-contract builds pass. CTest then reports exactly
`23/24` passing on each platform, with only the new boundary failing for the
intended missing production behavior. This is not a whole-Relin2 arithmetic
result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `9c40f801208f27d013d285e18a38fc8c65e41611`;
- parent: `ed6bdb77f893bb89409059a5e4179686ddbd66d4`;
- tree: `6ca3fd4f0eeff8f8925f3c53a178529ffbcf6c44`;
- local, remote-tracking, and live remote implementation SHA matched after the
  non-force push.

Only `CMakeLists.txt` and `tests/relin2_test.cpp` changed from the accepted BV
zero-digit entry-basis green source: 248 inserted lines and no deletion. No
production source or public header changed. The new registered runtime case is
`relin2_key_bv_nonzero_digit_entry_basis`.

The fixture uses BV with `digitSize == 10`. Against pristine OpenFHE 1.5.0 at
the pinned commit, `KeySwitchBV::KeySwitchGenInternal` obtains `ep` from
`sNew.GetParams()` and constructs every decomposed A/B digit over that complete
basis in `Format::EVALUATION`. The fixture independently observes the exact
four-tower full-Q bit-width manifest `{35, 31, 30, 31}` and the exact 15-entry
A/B vectors.

Before the negative check, the test replaces the final A entry with a clone
whose aggregate and per-tower parameter objects have independent identities but
equal values. That durable positive control reaches the accepted scaffold and
immediately proves Tensor, A/B polynomial, cache-map/vector/key-pointer,
context, and tag invariance. The negative control then changes only the ordered
basis of the final A entry by swapping its first two complete NativePoly
towers; B and all preceding A entries stay equal. It requires exactly
`DoubleCKKS: Relin2 evaluation key BV entry basis mismatch` and repeats the
immediate invariance checks. RAII restores the initially empty global
evaluation-key cache. Spec, TDD, and Delivery/API read-only reviews each
returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33604728646`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33604728646`;
- exact head SHA: `9c40f801208f27d013d285e18a38fc8c65e41611`;
- terminal run state: `completed/failure`, which is the required TDD red.

Linux job `100166014843` and Windows/MSYS2 MinGW64 job `100166014520` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded. CTest
then reported exactly `23/24` passing on each platform. On both systems, the
only failure was `relin2_key_bv_nonzero_digit_entry_basis`, with the complete
message:

`Relin2 test failure: Relin2 BV nonzero-digit evaluation-key entry basis threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

Linux reported 0.18 seconds and Windows reported 0.48 seconds. This proves the
valid positive path was not blanket-rejected, the malformed nonzero-digit A
entry reached the still-missing guard, and the existing 23 behaviors remained
green.

The GitHub Node.js 20 deprecation annotation concerns the hosted Action runtime,
not the project. The warning emitted by the MSYS2 toolchain installer asks that
other MSYS2 programs be closed; it is not a compiler warning from the project.
The warning-clean project build steps succeeded on both platforms.

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

The BV nonzero-digit entry-basis boundary is accepted red on Linux and Windows.
The next production change must start from this exact commit and only broaden
the already accepted BV full-Q ordered-basis guard from zero-digit to all BV
keys. It must not combine entry-format validation, ciphertext raising,
metadata changes, or Relin2 arithmetic.
