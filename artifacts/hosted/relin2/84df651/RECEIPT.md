# Relin2 Tensor-validation runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **Linux Tensor-validation-order green observed and closed**. This
proves only that public `Relin2` performs the existing complete Tensor result
validation before its retained immediate-throw scaffold. It is not Relin2
algorithm green, whole-workflow green, or a Windows pass.

## Exact source identity

- implementation branch: `agent/codex-relin2-01`;
- commit: `84df6518df47fc7e50b8f465e5aa294fe5fdf84d`;
- parent: `f2deacbb9b1f1a291d91b6d9ac9eec5f363f0082`;
- tree: `028033ad13b90710eaa98e6f1436bdb2d8f49b86`;
- subject: `feat: validate Relin2 Tensor input first`;
- local HEAD, upstream, and `git ls-remote` branch SHA matched exactly after
  the non-force push.

The commit changes only `src/double_ckks.cpp`, replacing the unnamed `Relin2`
parameter with `tensor` and calling the existing complete
`ValidateTensorResult(tensor)` before the unchanged exact immediate throw:

`std::logic_error("DoubleCKKS: Relin2 is not implemented")`

It changes no public API, test, CMake, workflow, evaluation-key logic,
insufficient-basis logic, tower arithmetic, relinearization, DCP/addition, or
result construction.

## Hosted run identity

- workflow: `DCP RCB TDD`, workflow id `346498968`;
- run: `33532645418`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33532645418`;
- head branch/SHA: `agent/codex-relin2-01` /
  `84df6518df47fc7e50b8f465e5aa294fe5fdf84d`;
- run start: `2026-09-01T16:35:00Z`;
- terminal state: `completed/cancelled` at `2026-09-01T16:37:45Z`.

The run-level `cancelled` conclusion is expected for this intermediate green
boundary after Linux evidence capture and is not called workflow green.

## Linux observation

Job `linux-gcc` (`99939294149`) completed `success` on Ubuntu 24.04.4 with
CMake 3.31.6 and GCC 13.3.0. It checked out the exact implementation SHA and
pristine OpenFHE commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`.

The warning-clean default build succeeded, including `relin2_test`. The
separately invoked compile-only `relin2_api_contract_test` compiled and linked.
CTest passed exactly all seven registered tests in 0.09 seconds:

1. `dcp_rcb`;
2. `tensor2_valid_arithmetic_immutability`;
3. `tensor2_result_scale_contract`;
4. `tensor2_right_input_validation`;
5. `tensor2_mutual_compatibility`;
6. `tensor2_prearithmetic_key_compatibility`;
7. `relin2_tensor_validation_order`.

The exact aggregate was `100% tests passed, 0 tests failed out of 7`. This
closes the immediately preceding exact red while leaving every valid Relin2
input on the not-implemented seam. The Node 20 deprecation annotation belongs
to GitHub Actions dependencies and is not a project compiler-warning result.

## Windows disposition

After the completed Linux job log was downloaded through the job-log API,
Codex submitted one cancellation request for the whole non-final run. The
Windows job `windows-mingw64` (`99939294452`) reached terminal
`completed/cancelled` while building pristine OpenFHE, at approximately 38%.
Project configure/build, the API contract, and CTest were skipped. No Windows
pass, project-build, API-contract, or test result is claimed.

## Raw evidence and integrity

The directory retains pre-cancel and terminal run/jobs/check-runs/artifact API
responses, both Linux log retrievals, the cancelled Windows log, the terminal
complete-logs ZIP, exact workflow/source identities, and the cancellation
receipt. `complete-logs-terminal.zip` passed `unzip -t`. The artifacts API
reported exactly zero uploaded artifacts; this is retained as zero rather than
replaced by an artifact claim.

Gitleaks 8.30.1 (binary SHA-256
`f414bc2fb952be6c9072b75cb411e3368614ef4b16d48dbd9ad238034afd2302`)
reported no leaks. The targeted credential-pattern scan returned no matches,
and the targeted forbidden-filename result is empty. `MANIFEST.sha256` lists
every retained file except itself.

## Boundary decision

The Tensor validation-order behavior is accepted green on Linux. The next
Relin2 behavior must begin with a new independently registered runtime red
against this exact valid-input not-implemented seam. No later production
behavior may be added before that red is observed and retained.
