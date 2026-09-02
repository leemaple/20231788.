# Relin2 malformed-HYBRID-A-vector-length runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the valid HYBRID key with only a malformed A-vector length guard is
green on Linux**. HYBRID B length, basis, and format validation and all Relin2
arithmetic remain unimplemented. This is not a whole-algorithm result or a
Windows test result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `3645c4e02d3fb1eca4496b1de9be1ebb5517bac9`;
- parent: `b50448f561ba62f9a6271779f5c1eeb4903cac4e`;
- tree: `b1d0f395c59d01b3a2e4e78e3d10856dd746e687`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `src/double_ckks.cpp` changed. Four production lines immediately after
the accepted concrete-subtype guard compare the already cast key's public A
vector length with the bound context's `GetNumPartQ()` only when the configured
key-switching technique is HYBRID. A mismatch receives the stable
project-owned diagnostic. The change does not read B or an A entry; validate
basis or format; mutate the cache; clone or raise a ciphertext; call key
switching/relinearization; begin Relin2 arithmetic; or add production
`try`/`catch`. The old scaffold remains the next behavior. Spec, TDD, and
Delivery/CI read-only source reviews each returned `PASS` before commit, and
the same three review roles accepted the hosted green result.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33573158385`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33573158385`;
- exact head SHA: `3645c4e02d3fb1eca4496b1de9be1ebb5517bac9`;
- terminal run state: `completed/cancelled` after Linux evidence capture.

Linux job `100071272960` ran against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest passed exactly `15/15`
in 0.16 seconds, including `relin2_key_hybrid_a_length`.

The new negative case received the exact malformed-HYBRID-A diagnostic and
then completed all immediate Tensor/deep-metadata and cache-map/vector/key-
pointer, context, tag, subtype, A-vector, and B-vector invariance checks. Its
test-owned RAII scope restored the initially empty cache. The inherited real-
key positive control in test 14 still reached the current scaffold and
completed its post-call checks, proving the new guard is not an unconditional
HYBRID-key rejection.

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100071272807` stopped during `Install official Windows toolchain` and
supplies no OpenFHE build, project build, API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity. The ZIP
passes `unzip -t`; uploaded artifact count is zero. Before commit, both the
retained directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted
credential scans, then every retained file except the manifest itself is bound
by `MANIFEST.sha256`.

The malformed-HYBRID-A-length guard is accepted green on Linux. The next
isolated red must retain a valid first `EvalKeyRelinImpl` and valid HYBRID A
vector while making only the HYBRID B-vector length malformed, expect
`DoubleCKKS: Relin2 evaluation key HYBRID B vector length mismatch`, and prove
rejection before basis/format access, raising, key switching, or arithmetic.
