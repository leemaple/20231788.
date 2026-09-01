# Relin2 wrong-actual-evaluation-key-tag runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the nonnull, same-context first-key/wrong-actual-tag guard is green
on Linux**. Later concrete-subtype, A/B shape/basis/format validation and all
Relin2 arithmetic remain unimplemented. This is not a whole-algorithm result
or a Windows test result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `4bfe4fcf5d2c095eb99cdfd9f1df59f261b0b72d`;
- parent: `82c9fa972e528a181a38fda4f3d71b9210547357`;
- tree: `7ced7f73590dcd698a55e49b8305be955805876f`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `src/double_ckks.cpp` changed. Three production lines immediately after
the accepted context guard compare the first evaluation-key pointee's public
`GetKeyTag()` value with the Tensor key tag and reject mismatch with the stable
project-owned diagnostic. The cache map row remains selected by the Tensor tag;
the new guard validates the actual pointee tag rather than trusting that row.
The change does not inspect the key's concrete subtype, A/B vectors,
key-switching technique, basis, or format; clone or raise a ciphertext; call
relinearization/key switching; begin Relin2 arithmetic; or add production
`try`/`catch`. The old scaffold remains the next behavior. Spec, TDD, and
Delivery/CI read-only reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33567914096`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33567914096`;
- exact head SHA: `4bfe4fcf5d2c095eb99cdfd9f1df59f261b0b72d`;
- terminal run state: `completed/cancelled` after Linux evidence capture.

Linux job `100055212547` ran against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest passed exactly `13/13`
in 0.13 seconds, including `relin2_key_wrong_tag`. That passing case executed
the immediate post-call Tensor/deep-metadata checks and the full
map/vector/key pointer-identity, context, tag, and A/B equality checks, then
proved the test-owned RAII scope restored the initially empty global cache.

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100055212687` stopped while installing the official Windows toolchain and
supplies no project-build, API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity. The ZIP
passes `unzip -t`; uploaded artifact count is zero. Before commit, both the
retained directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted
credential scans, then every retained file except the manifest itself is bound
by `MANIFEST.sha256`.

The wrong-actual-tag guard is accepted green on Linux. The next isolated red
must keep map row, nonnull pointer, context identity, and actual key tag valid
while supplying the wrong concrete evaluation-key subtype. It must reject
before any A/B getter and without adding A/B-shape, basis, format, or arithmetic
behavior ahead of that boundary.
