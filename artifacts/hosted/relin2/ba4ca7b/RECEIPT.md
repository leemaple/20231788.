# Relin2 wrong-context-evaluation-key runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the nonnull-first-key/wrong-context guard is green on Linux**.
Later actual-tag, concrete-subtype, A/B shape/basis/format validation and all
Relin2 arithmetic remain unimplemented. This is not a whole-algorithm result
or a Windows test result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `ba4ca7b4c93fc4d750618418aeeaf1db6b1dd32d`;
- parent: `0a8f84039e61c36548987df2b68153754c519442`;
- tree: `87b4e8e09cdc8d319fec6b71da7f2ad66abd91e4`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `src/double_ckks.cpp` changed. Three production lines immediately after
the accepted null guard compare the first nonnull evaluation key's public
`CryptoContext` identity with the bound module context and reject mismatch
with the stable project-owned diagnostic. The change does not inspect the key's
actual tag, concrete subtype, A/B vectors, key-switching technique, basis, or
format; clone or raise a ciphertext; call relinearization/key switching; begin
Relin2 arithmetic; or add production `try`/`catch`. The old scaffold remains
the next behavior. Spec, TDD, and Delivery/CI read-only reviews each returned
`PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33566113002`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33566113002`;
- exact head SHA: `ba4ca7b4c93fc4d750618418aeeaf1db6b1dd32d`;
- terminal run state: `completed/cancelled` after Linux evidence capture.

Linux job `100049553404` ran against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest passed exactly `12/12`
in 0.12 seconds, including `relin2_key_wrong_context`. That passing case
executed the immediate post-call Tensor/deep-metadata checks and the full
map/vector/key pointer-identity, context, tag, and A/B equality checks, then
proved the test-owned RAII scope restored the initially empty global cache.

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100049553494` stopped while installing the official Windows toolchain and
supplies no project-build, API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity. The ZIP
passes `unzip -t`; uploaded artifact count is zero. Before commit, both the
retained directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted
credential scans, then every retained file except the manifest itself is bound
by `MANIFEST.sha256`.

The wrong-context-key guard is accepted green on Linux. The next isolated red
must keep context identity valid and place a generated, nonnull relinearization
key whose actual key tag differs from the Tensor tag under the Tensor-tag map
row. It must reach the old scaffold without adding subtype, A/B, or arithmetic
behavior ahead of that boundary.
