# Relin2 malformed-HYBRID-B-vector-length runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the valid HYBRID key with only a malformed B-vector length guard is
green on Linux**. Basis and format validation and all Relin2 arithmetic remain
unimplemented. This is not a whole-algorithm result or a Windows test result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `0e240f37ca22fd683f50a108c40e3e4bfa62f593`;
- parent: `0efab27ea1e2946db94b8621be53c2ecaaa282e8`;
- tree: `cb8d846bb6c070e84aae69cc1e26c2e35353aa7a`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `src/double_ckks.cpp` changed. Four production lines immediately after
the accepted HYBRID A-length guard compare the already cast key's public B
vector length with the bound context's `GetNumPartQ()` only when the configured
key-switching technique is HYBRID. A mismatch receives the stable
project-owned diagnostic. The change does not read an A/B entry; validate
basis or format; mutate the cache; clone or raise a ciphertext; call key
switching/relinearization; begin Relin2 arithmetic; or add production
`try`/`catch`. The old scaffold remains the next behavior. Spec, TDD, and
Delivery/API read-only source reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33574998256`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33574998256`;
- exact head SHA: `0e240f37ca22fd683f50a108c40e3e4bfa62f593`;
- terminal run state: `completed/cancelled` after Linux evidence capture.

Linux job `100076899535` ran against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest passed exactly `16/16`
in 0.14 seconds, including both `relin2_key_hybrid_a_length` and
`relin2_key_hybrid_b_length`.

The new malformed-B case received the exact project diagnostic and then
completed all immediate Tensor/deep-metadata and cache-map/vector/key-pointer,
context, tag, subtype, A-vector, and B-vector invariance checks. Its test-owned
RAII scope restored the initially empty cache. The inherited malformed-A test
continued to receive the A diagnostic, proving A-before-B validation priority.
The inherited real-key positive control still reached the current scaffold and
completed its post-call checks, proving the new guard is not an unconditional
HYBRID-key rejection.

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100076899237` stopped during `Install official Windows toolchain` and
supplies no OpenFHE build, project build, API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity. The ZIP
passes `unzip -t`; uploaded artifact count is zero. Before commit, both the
retained directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted
credential scans, then every retained file except the manifest itself is bound
by `MANIFEST.sha256`.

The malformed-HYBRID-B-length guard is accepted green on Linux. The next
isolated red must keep both HYBRID vector lengths and entry formats valid while
making only one entry's complete `ParamsQP` basis identity wrong, expect
`DoubleCKKS: Relin2 evaluation key HYBRID entry basis mismatch`, and prove
rejection before raising, key switching, or Relin2 arithmetic.
