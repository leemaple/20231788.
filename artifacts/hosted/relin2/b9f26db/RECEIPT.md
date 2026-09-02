# Relin2 malformed-HYBRID-entry-format runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the HYBRID evaluation-key entry-format contract is green on Linux
and Windows**. Both warning-clean project builds, both compile-only Relin2
public API-contract builds, and all 18 registered runtime tests pass. This is
not a claim that the remaining Relin2 arithmetic has been implemented.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `b9f26db29b53764930798340f4ebe9bed789a323`;
- parent: `b1f4459d9e1d3009da5420954f26384b96ba3e57`;
- tree: `7a05d020200ce147241b96f86523b5097339ec0d`;
- local and remote implementation SHA matched after non-force push.

Only `src/double_ckks.cpp` changed from the accepted runtime-red commit: 22
lines were added. The production guard first preserves the already accepted
HYBRID length and complete ordered `ParamsQP` basis checks. It then scans every
A entry followed by every B entry, rejecting either an aggregate DCRTPoly or
any NativePoly tower that is not in Evaluation format with the exact project
diagnostic `DoubleCKKS: Relin2 evaluation key HYBRID entry must be in
evaluation format`. BV validation, ciphertext raising, key switching,
relinearization, arithmetic, metadata, tests, CMake, public headers, and the
old not-implemented scaffold are unchanged.

Before commit, Spec, TDD, and Delivery/API read-only reviews each returned
`PASS` against source SHA-256
`972f34153a5a63269052b1a9cbd88912f8eb1e4dc411a316cc6e16f4a7e40a7e`.
An earlier unsupported `lbcrypto::Format` spelling was found locally and
corrected to OpenFHE 1.5.0's existing `Format` spelling before those final
reviews and before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33582263190`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33582263190`;
- exact head SHA: `b9f26db29b53764930798340f4ebe9bed789a323`;
- terminal run state: `completed/success`.

Linux job `100098827445` and Windows/MSYS2 MinGW64 job `100098827360` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default
project builds and both compile-only Relin2 public API-contract builds
succeeded. CTest reported exactly `18/18` passing on both platforms, including
`relin2_key_hybrid_entry_format`: Linux reported 0.18 seconds and Windows 0.42
seconds. Thus the valid control still reaches the old scaffold, the malformed
Coefficient-format A entry now receives the exact `std::invalid_argument`, and
the test's immediate Tensor, deep metadata, A/B polynomial,
cache-map/vector/key-pointer, context, and tag invariance postchecks all
complete.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both full
job records/logs, complete logs ZIP, exact workflow, and source identity. The
ZIP passes `unzip -t`; the uploaded artifact count is zero. Before evidence
commit, both the retained directory and a fresh expansion of the ZIP must pass
Gitleaks 8.30.1 and independent targeted filename/content scans, then every
retained file except the manifest itself is bound by `MANIFEST.sha256`.

The malformed-HYBRID-entry-format boundary is accepted green on Linux and
Windows. The next production boundary must be separately introduced red-first;
this receipt does not authorize combining BV-specific shape validation or any
Relin2 arithmetic with this change.
