# Relin2 insufficient-active-basis runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the next public-API-reachable Relin2 contract is red on Linux, with
the failure uniquely attributed to the retained not-implemented seam**.
Production at this boundary still performs only complete Tensor validation and
then throws the exact not-implemented `std::logic_error` for otherwise valid
input. This is not Relin2 algorithm green or a Windows test result.

## Exact source identity

- implementation branch: `agent/codex-relin2-01`;
- commit: `8642a9450bd9315f1d228a537ccdce7b3d9614a5`;
- parent: `84df6518df47fc7e50b8f465e5aa294fe5fdf84d`;
- tree: `f7cf880d6682f5ab194aefd2f5df59120e7e034b`;
- subject: `test: retain Relin2 insufficient-basis red`;
- local, upstream, and remote implementation branch SHA matched exactly after
  the non-force push.

The commit changes only `CMakeLists.txt` and `tests/relin2_test.cpp` (35
insertions, two deletions). It parameterizes the existing public context
fixture, registers `relin2_insufficient_active_basis`, and uses only public
OpenFHE/project APIs. With multiplicative depth two, the encrypted inputs begin
with exactly three towers; DCP and Tensor2 leave exactly two active towers while
the Tensor noise-scale degree is exactly three. The same minimum fixture first
passes RCB, proving it is otherwise supported. No evaluation multiplication key
or private state setter is used.

The test requires `std::invalid_argument` and full-string equality with:

`DoubleCKKS: Relin2 requires at least as many active Q_l towers as the Tensor noise-scale degree`

Production `include/` and `src/` bytes are unchanged from the parent.

## Hosted run identity

- workflow: `DCP RCB TDD`, workflow id `346498968`;
- run: `33554777953`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33554777953`;
- exact head SHA: `8642a9450bd9315f1d228a537ccdce7b3d9614a5`;
- terminal state: `completed/cancelled` at `2026-09-01T20:23:36Z`.

The run-level cancellation was requested only after the complete Linux failure
log had been downloaded, to stop the non-final Windows build. It does not turn
the completed Linux red into a cancellation claim.

## Linux observation

Job `linux-gcc` (`100012796036`) completed `failure` on Ubuntu 24.04.4 with
CMake 3.31.6 and GCC 13.3.0 against pristine OpenFHE commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`.

The warning-clean project build and the separately invoked compile-only
`relin2_api_contract_test` both succeeded. CTest ran all eight registered tests:
the seven inherited tests passed, and only
`relin2_insufficient_active_basis` failed. The exact aggregate was `7/8`, CTest
exit `8`. The attribution line was:

`Relin2 test failure: Relin2 insufficient active basis threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

This proves that the public fixture passed complete Tensor validation and
reached the pre-existing scaffold, while the required basis guard was absent.

## Windows disposition

Job `windows-mingw64` (`100012795842`) was cancelled during the pristine
OpenFHE build, at approximately 9%. It did not reach project configure/build,
the Relin2 API contract, or CTest. No Windows pass or project-test claim is made.

## Raw evidence and integrity

This directory retains terminal run, jobs, check-runs, artifacts, per-job JSON,
both job logs, the terminal complete-logs ZIP, the exact workflow, source
identity, scan outputs, and a complete `MANIFEST.sha256` excluding itself. The
terminal ZIP passed `unzip -t`; the artifacts API reported exactly zero uploaded
artifacts. Gitleaks 8.30.1 and targeted credential-content/filename scans must
report no findings before this evidence branch is committed.

## Boundary decision

The red is accepted. The only authorized production change is a fail-fast
active-tower-count check after complete Tensor validation and before any later
evaluation-key lookup, basis raise, arithmetic, or result construction. Valid
adequate-basis input must continue to reach the exact not-implemented seam.
