# Relin2 missing-evaluation-key runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the missing-evaluation-key Relin2 contract is green on Linux**.
Relin2 key-shape validation and arithmetic remain intentionally unimplemented;
this is not whole-algorithm green or a Windows test result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `7c0e94de99c0d6a966f73d78bf50b8c590cfce8c`;
- parent/red commit: `b0196dd5349e94a0df0cd0f0750fa7f6819a3498`;
- tree: `a82bcc806b99b09e9b7a8199271a56cf3426b7d3`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `src/double_ckks.cpp` changed, adding four lines. After complete Tensor
validation and the accepted active-basis guard, production binds a const
reference to `GetAllEvalMultKeys()`, uses `find(tensor.GetKeyTag())`, and emits
the stable project diagnostic when the row is absent. It uses no `operator[]`,
`GetEvalMultKeyVector`, `try`/`catch`, key/vector access, clone, basis change,
arithmetic, or state mutation. A present row still reaches the exact old
not-implemented `std::logic_error`. Spec, TDD, and Delivery/CI read-only
reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `DCP RCB TDD` / `33557603825`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33557603825`;
- exact head SHA: `7c0e94de99c0d6a966f73d78bf50b8c590cfce8c`;
- terminal run state: `completed/cancelled` at `2026-09-01T20:52:42Z`.

Linux job `100022130903` completed success against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest passed exactly `9/9` in
0.30 seconds, including the immediately preceding red.

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100022130681` stopped during pristine OpenFHE build near 10% and supplies
no project-build, API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity. The ZIP
passed `unzip -t`; uploaded artifact count is zero. Before commit, the retained
directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted credential
scans, then every retained file except the manifest itself is bound by
`MANIFEST.sha256`.

The missing-row guard is accepted green on Linux. The next behavior must begin
with a separately registered red for a present but empty evaluation-key vector;
no key dereference, shape validation, or Relin2 arithmetic may be implemented
ahead of that red.
