# Relin2 null-first-evaluation-key runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the present-vector/null-first-evaluation-key contract is green on
Linux**. First-key context/tag/subtype/shape validation and Relin2 arithmetic
remain intentionally unimplemented; this is not whole-algorithm green or a
Windows test result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `37d1758b515a77e3a6f880f182462e223b2d2bd5`;
- parent/red commit: `66d28156a698df78ecdfdd71ce8ddeb513308a7a`;
- tree: `fe314423147bf028b1075070908f47ee8bbcbebc`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `src/double_ckks.cpp` changed, adding four lines. After the accepted
nonempty-vector guard, production binds a const reference to `front()`, tests
the shared pointer for null, and emits
`DoubleCKKS: Relin2 first evaluation key is null`. It does not dereference or
clone the key, inspect context/tag/subtype/A/B vectors, change basis or format,
perform arithmetic, mutate cache/Tensor state, or add production `try`/`catch`.
A nonnull first key still reaches the exact old scaffold. Spec, TDD, and
Delivery/CI read-only reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33562728326`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33562728326`;
- exact head SHA: `37d1758b515a77e3a6f880f182462e223b2d2bd5`;
- terminal run state: `completed/cancelled` after Linux evidence capture.

Linux job `100038781719` completed success against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest passed exactly `11/11`
in 0.11 seconds, including the immediately preceding null-first-key red and the
new immediate Tensor/cache/deep-metadata invariance checks.

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100038781927` stopped during pristine OpenFHE build and supplies
no project-build, API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity. The ZIP
passes `unzip -t`; uploaded artifact count is zero. Before commit, both the
retained directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted
credential scans, then every retained file except the manifest itself is bound
by `MANIFEST.sha256`.

The null-first-key guard is accepted green on Linux. The next behavior must
begin with a separately registered red whose nonnull first evaluation key
belongs to a different CryptoContext; no tag/subtype/digit-shape validation or
Relin2 arithmetic may be implemented ahead of that red.
