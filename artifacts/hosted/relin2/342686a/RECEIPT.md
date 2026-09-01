# Relin2 empty-evaluation-key-vector runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the present-but-empty Relin2 evaluation-key-vector contract is green
on Linux**. Relin2 first-key validation and arithmetic remain intentionally
unimplemented; this is not whole-algorithm green or a Windows test result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `342686ae1badbcfd700eff0ec473cac4168e491a`;
- parent/accepted red commit:
  `e976626ebabf0aa40858ee97c830611e9c47c5f6`;
- tree: `4aeb255ee5b78b8b3f9f8c113c47cc09f1ba67d8`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `src/double_ckks.cpp` changed: five additions and one deletion. After full
Tensor validation and the accepted active-basis guard, production binds the
evaluation-key cache, stores the single `find(tensor.GetKeyTag())` result,
preserves the missing-row diagnostic, and then rejects an empty vector with
`DoubleCKKS: Relin2 evaluation-key vector is empty`. It uses no `operator[]`,
throwing key lookup, `try`/`catch`, key-element access, clone, basis change,
arithmetic, or cache/Tensor mutation. A nonempty vector still reaches the exact
old not-implemented scaffold. Spec, TDD, and Delivery/CI read-only reviews each
returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33560502555`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33560502555`;
- exact head SHA: `342686ae1badbcfd700eff0ec473cac4168e491a`;
- terminal run state: `completed/cancelled` after Linux evidence capture.

Linux job `100031552242` completed success against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest passed exactly `10/10`
in 0.11 seconds, including the immediately preceding empty-vector red.

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100031551913` stopped while installing its toolchain and supplies no
project-build, API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity. The ZIP
passes `unzip -t`; uploaded artifact count is zero. Before commit, both the
retained directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted
credential scans, then every retained file except the manifest itself is bound
by `MANIFEST.sha256`.

The empty-vector guard is accepted green on Linux. The next behavior must begin
with a separately registered red for a present evaluation-key vector whose
first element is null; no context/tag/subtype/shape validation or Relin2
arithmetic may be implemented ahead of that red.
