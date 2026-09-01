# Relin2 missing-evaluation-key runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the public-API-reachable missing-evaluation-key contract is red on
Linux and uniquely reaches the retained not-implemented seam**. This is not
Relin2 algorithm green or a Windows test result.

## Source and test boundary

- branch/commit: `agent/codex-relin2-01` /
  `b0196dd5349e94a0df0cd0f0750fa7f6819a3498`;
- parent: `791f634c7e29c9a4b9e465d7a092e94eb429a7ab`;
- tree: `5dd4c479e77e60cee399befc89c5aca06271cf2d`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `CMakeLists.txt` and `tests/relin2_test.cpp` changed. The new ninth test
uses depth three and public APIs only: four full-basis towers become a valid
Tensor with three active towers and noise-scale degree three. It neither calls
`EvalMultKeyGen` nor clears the process cache; it proves the complete cache is
empty before the call and remains empty afterward. It requires exact
`std::invalid_argument` text:

`DoubleCKKS: Relin2 evaluation key is missing for the Tensor key tag`

Production `include/` and `src/` bytes are unchanged. Spec, TDD, and
Delivery/CI read-only reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `DCP RCB TDD` / `33557301667`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33557301667`;
- exact head SHA: `b0196dd5349e94a0df0cd0f0750fa7f6819a3498`;
- terminal run state: `completed/cancelled` at `2026-09-01T20:48:36Z`.

Linux job `100021153304` built the warning-clean project and compile-only
Relin2 API target against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The inherited eight tests passed;
only `relin2_missing_eval_key` failed. Aggregate: `8/9`, CTest exit `8`. Exact
attribution:

`Relin2 test failure: Relin2 missing evaluation key threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

After the Linux log was captured, the intermediate run was cancelled. Windows
job `100021152934` stopped during toolchain setup and supplies no project-build
or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity. The ZIP
passed `unzip -t`; uploaded artifact count is zero. Before commit, the retained
directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted credential
scans, then every retained file except the manifest itself is bound by
`MANIFEST.sha256`.

The red is accepted. Green may only perform a read-only `find` in
`GetAllEvalMultKeys()` after the existing validations, emit the stable missing
row diagnostic, and otherwise preserve the old not-implemented seam.
