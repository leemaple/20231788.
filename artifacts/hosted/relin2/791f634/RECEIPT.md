# Relin2 insufficient-active-basis runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the insufficient-active-basis Relin2 contract is green on Linux**.
This proves only the newly registered validation boundary. Relin2 arithmetic
and successful result construction remain intentionally unimplemented; this is
not whole-algorithm green or a Windows test result.

## Exact source identity

- implementation branch: `agent/codex-relin2-01`;
- commit: `791f634c7e29c9a4b9e465d7a092e94eb429a7ab`;
- parent/red commit: `8642a9450bd9315f1d228a537ccdce7b3d9614a5`;
- tree: `415548f752db312987688894e1207677ac39b46c`;
- subject: `feat: reject insufficient Relin2 active basis`;
- local HEAD, upstream, and remote branch SHA were all verified equal after a
  non-force push.

The commit changes only `src/double_ckks.cpp`, adding three lines. Immediately
after the existing complete `ValidateTensorResult(tensor)`, it compares the
active ordered-modulus count with the Tensor noise-scale degree and raises the
project-owned exact `std::invalid_argument` when the basis is too short. The
existing exact not-implemented `std::logic_error` remains unchanged for valid
adequate-basis input.

The change adds no public API, test, CMake, workflow, key-cache access, tower
construction, OpenFHE arithmetic/relinearization call, private DCP/addition,
result construction, state mutation, or `try`/`catch`. Independent Spec, TDD,
and Delivery/CI read-only reviews each returned `PASS` before commit.

## Hosted run identity

- workflow: `DCP RCB TDD`, workflow id `346498968`;
- run: `33555298067`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33555298067`;
- exact head SHA: `791f634c7e29c9a4b9e465d7a092e94eb429a7ab`;
- terminal state: `completed/cancelled` at `2026-09-01T20:28:02Z`.

The run-level cancellation was requested only after the completed Linux job log
had been downloaded, to stop the non-final Windows build. The run is therefore
not described as whole-workflow success.

## Linux observation

Job `linux-gcc` (`100014526797`) completed `success` on Ubuntu 24.04.4 with
CMake 3.31.6 and GCC 13.3.0 against pristine OpenFHE commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`.

The warning-clean default project build succeeded. The separately invoked
compile-only `relin2_api_contract_test` compiled and linked. CTest passed all
eight registered tests in 0.14 seconds:

1. `dcp_rcb`;
2. `tensor2_valid_arithmetic_immutability`;
3. `tensor2_result_scale_contract`;
4. `tensor2_right_input_validation`;
5. `tensor2_mutual_compatibility`;
6. `tensor2_prearithmetic_key_compatibility`;
7. `relin2_tensor_validation_order`;
8. `relin2_insufficient_active_basis`.

The exact aggregate was `100% tests passed, 0 tests failed out of 8`. This
closes the immediately preceding exact `7/8` red without implementing any later
Relin2 behavior.

## Windows disposition

Job `windows-mingw64` (`100014526531`) was cancelled while installing the
official Windows toolchain, before OpenFHE configure/build and before every
project step. No Windows pass, build, API-contract, or test claim is made.

## Raw evidence and integrity

This directory retains terminal run, jobs, check-runs, artifacts, per-job JSON,
both job logs, the terminal complete-logs ZIP, the exact workflow, source
identity, scan outputs, and a complete `MANIFEST.sha256` excluding itself. The
terminal ZIP passed `unzip -t`; the artifacts API reported exactly zero uploaded
artifacts. Gitleaks 8.30.1 and targeted credential-content/filename scans must
report no findings before this evidence branch is committed.

## Boundary decision

The insufficient-active-basis guard is accepted green on Linux. The next Relin2
behavior must begin with a separately registered red against an otherwise valid
adequate-basis Tensor that passes both existing validations and reaches the
unchanged not-implemented seam. No later arithmetic may be implemented ahead of
that red.
