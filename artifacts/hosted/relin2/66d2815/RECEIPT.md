# Relin2 null-first-evaluation-key runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the present-vector/null-first-evaluation-key contract is red on
Linux**. The warning-clean project build and compile-only Relin2 API contract
pass; exactly the newly registered runtime case remains red against the old
scaffold. This is not a whole-algorithm result or a Windows test result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `66d28156a698df78ecdfdd71ce8ddeb513308a7a`;
- parent: `342686ae1badbcfd700eff0ec473cac4168e491a`;
- tree: `e7dda469c03a325f7af87c2636ff164b68f03fa3`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `CMakeLists.txt` and `tests/relin2_test.cpp` changed; production
`include/` and `src/` are byte-unchanged. The eleventh public fixture reaches
an otherwise-valid Tensor with a matching tag row containing exactly one null
`EvalKey<DCRTPoly>` shared pointer. It uses no `EvalMultKeyGen`, cache clear,
throwing key lookup, `operator[]`, key dereference, or Relin2 arithmetic. RAII
restores the global map. Tensor/cache/deep-metadata invariance assertions sit
immediately after the diagnostic helper; this red stops inside the helper on
the wrong exception type, so their execution is demonstrated by the paired
green checkpoint. Spec, TDD, and Delivery/CI read-only reviews each returned
`PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33562346015`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33562346015`;
- exact head SHA: `66d28156a698df78ecdfdd71ce8ddeb513308a7a`;
- terminal run state: `completed/cancelled` after Linux evidence capture.

Linux job `100037552108` ran against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest reported exactly `10/11`
passing in 0.20 seconds. Tests 1 through 10 passed; only
`relin2_key_null_first` failed, with the exact observation:

`Relin2 test failure: Relin2 null first evaluation key threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100037552072` stopped during pristine OpenFHE build and supplies no
project-build, API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity. The ZIP
passes `unzip -t`; uploaded artifact count is zero. Before commit, both the
retained directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted
credential scans, then every retained file except the manifest itself is bound
by `MANIFEST.sha256`.

The null-first-key contract is accepted red on Linux. The green change may only
bind the first nonempty-vector element and reject a null shared pointer with the
exact diagnostic; it must not dereference the key or inspect context, tag,
subtype, digit vectors, basis, format, or begin arithmetic.
