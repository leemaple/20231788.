# Relin2 wrong-concrete-evaluation-key-subtype runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the valid-row/nonnull/same-context/same-tag but wrong concrete key
subtype guard is green on Linux**. A/B length, basis, and format validation and
all Relin2 arithmetic remain unimplemented. This is not a whole-algorithm
result or a Windows test result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `331dd7d64d53e6a87d94555a00aab29b1eaa998e`;
- parent: `fafe3850d7de4b6e725eed707f32865ff0d2de7d`;
- tree: `421b0f6147698d0055e1477c56c63ca3e6e46041`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `src/double_ckks.cpp` changed. Five production lines immediately after
the accepted actual-tag guard dynamically cast the already validated first key
to public `EvalKeyRelinImpl<DCRTPoly>` and reject a null cast with the stable
project-owned diagnostic. The new shared pointer is used by that null check and
is ready for the next isolated validation boundary. The change does not read
A/B; inspect key-switching technique, basis, or format; mutate the cache; clone
or raise a ciphertext; call relinearization/key switching; begin Relin2
arithmetic; or add production `try`/`catch`. The old scaffold remains the next
behavior. Spec, TDD, and Delivery/CI read-only reviews each returned `PASS`
before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33571134323`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33571134323`;
- exact head SHA: `331dd7d64d53e6a87d94555a00aab29b1eaa998e`;
- terminal run state: `completed/cancelled` after Linux evidence capture.

Linux job `100065111800` ran against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest passed exactly `14/14`
in 0.11 seconds, including `relin2_key_wrong_subtype`.

In that passing test, the real `EvalKeyRelinImpl` positive control continued to
the exact current scaffold and then completed all immediate Tensor/deep-
metadata and map/vector/key pointer, context, tag, and A/B invariance checks.
The exact-base `EvalKeyImpl` negative control then received the new diagnostic
and completed its immediate Tensor/deep-metadata and map/vector/key pointer,
context, tag, and concrete-type invariance checks without ever calling a base
A/B getter. Both test-owned RAII scopes restored the initially empty cache.

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100065111576` stopped during `Install official Windows toolchain` and
supplies no OpenFHE build, project build, API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity. The ZIP
passes `unzip -t`; uploaded artifact count is zero. Before commit, both the
retained directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted
credential scans, then every retained file except the manifest itself is bound
by `MANIFEST.sha256`.

The wrong-concrete-subtype guard is accepted green on Linux. The next isolated
red must retain a valid first `EvalKeyRelinImpl` and HYBRID B vector while
making only the HYBRID A-vector length malformed, expect
`DoubleCKKS: Relin2 evaluation key HYBRID A vector length mismatch`, and prove
rejection before basis/format access, raising, key switching, or arithmetic.
