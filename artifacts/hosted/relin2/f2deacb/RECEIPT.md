# Relin2 Tensor-validation runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **expected Linux runtime red observed and closed**. The strict default
build and API contract succeeded, all six inherited tests stayed green, and
the new independently registered Relin2 validation-order test failed for the
intended immediate-throw scaffold. This is not Relin2 algorithm green,
whole-workflow green, or a Windows pass.

## Exact source identity

- implementation branch: `agent/codex-relin2-01`;
- commit: `f2deacbb9b1f1a291d91b6d9ac9eec5f363f0082`;
- parent: `6f1645b97ce5b2175530cde5bfd0929370997634`;
- tree: `d1231cb5fb080a0eac65091fe1f5db539779d4b2`;
- subject: `test: retain Relin2 Tensor validation red`;
- local HEAD, upstream, and `git ls-remote` branch SHA matched exactly after
  the non-force push.

The commit changes only `CMakeLists.txt` and the new
`tests/relin2_test.cpp`. Production `include/` and `src/` bytes are unchanged
from the accepted API scaffold. The new test creates a valid public Tensor2
result without calling `EvalMultKeyGen`, corrupts only its high logical-scale
manifest field, calls public `Relin2`, and requires exact full-string equality
with the project-owned `std::invalid_argument` diagnostic:

`DoubleCKKS: Tensor2 result paper-scale descriptor is inconsistent`

The test has its own `relin2_tensor_validation_order` CTest registration and
uses strict warning-as-error options on supported toolchains.

## Hosted run identity

- workflow: `DCP RCB TDD`, workflow id `346498968`;
- run: `33531734269`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33531734269`;
- head branch/SHA: `agent/codex-relin2-01` /
  `f2deacbb9b1f1a291d91b6d9ac9eec5f363f0082`;
- run start: `2026-09-01T16:25:57Z`;
- terminal state: `completed/cancelled` at `2026-09-01T16:29:35Z`.

The run-level `cancelled` conclusion is expected for this intermediate red
boundary and is not called workflow green.

## Linux observation

Job `linux-gcc` (`99936280030`) ran on Ubuntu 24.04.4 with CMake 3.31.6 and
GCC 13.3.0. It checked out the exact implementation SHA and pristine OpenFHE
commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

The warning-clean default project build succeeded, including the new
`relin2_test` target. The separately invoked compile-only
`relin2_api_contract_test` also compiled and linked. CTest then observed:

- inherited DCP/RCB/Tensor2 tests: exactly `6/6` passed;
- new `relin2_tensor_validation_order`: failed alone;
- aggregate: `86%`, exactly `1` failure out of `7`, CTest exit `8`.

The new test's exact failure was:

`Relin2 test failure: Relin2 Tensor manifest validation before evaluation-key lookup threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

This proves the fixture reached the current production scaffold and the red is
attributed to its `std::logic_error` rather than a compile failure, an inherited
regression, or the required project-owned invalid-argument diagnostic. The
Node 20 deprecation annotation belongs to GitHub Actions dependencies and is
not a project compiler-warning result.

## Windows disposition

After the complete Linux job log was downloaded through the job-log API,
Codex submitted one cancellation request for the whole non-final run. The
Windows job `windows-mingw64` (`99936279878`) reached terminal
`completed/cancelled` while building pristine OpenFHE, at approximately 48%.
Project configure/build, the API contract, and CTest were skipped. No Windows
pass, red, project-build, or test result is claimed.

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

The Tensor-manifest validation-order red is accepted as authentic. The next
permitted production change is only to call the existing complete
`ValidateTensorResult(tensor)` at the start of `Relin2`, then retain the exact
immediate `std::logic_error("DoubleCKKS: Relin2 is not implemented")` for valid
input. It must not add evaluation-key access, insufficient-basis logic, tower
raising, relinearization, private DCP, addition, or result construction.
