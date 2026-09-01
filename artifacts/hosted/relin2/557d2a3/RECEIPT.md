# Relin2 API compile-red hosted receipt

Recorded: 2026-09-01 Asia/Shanghai

Status: **expected Linux API compile red observed and closed**. This is not a
Relin2 implementation, runtime-test green, workflow green, or Windows pass.

## Exact source identity

- implementation branch: `agent/codex-relin2-01`;
- commit: `557d2a331658a2cf16d47de36415c2d968e62b5f`;
- parent: `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`;
- tree: `e338874b7e9324146e2774a2118e2904891ca3b5`;
- subject: `test: retain Relin2 API compile red`;
- local HEAD, upstream, and `git ls-remote` branch SHA matched exactly after
  the non-force push.

The commit changes only `.github/workflows/dcp-rcb.yml`, `CMakeLists.txt`, and
the new `tests/relin2_api_contract_test.cpp`. Production `include/` and `src/`
bytes are unchanged from the accepted `fb862a3` base.

## Hosted run identity

- workflow: `DCP RCB TDD`, workflow id `346498968`;
- run: `33527929014`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33527929014`;
- head branch/SHA: `agent/codex-relin2-01` /
  `557d2a331658a2cf16d47de36415c2d968e62b5f`;
- run start: `2026-09-01T15:47:48Z`;
- terminal state: `completed/cancelled` at `2026-09-01T15:51:24Z`.

The run-level `cancelled` conclusion is expected for this intermediate red
boundary and is not called workflow green.

## Linux observation

Job `linux-gcc` (`99923428497`) ran on Ubuntu 24.04.4 with CMake 3.31.6 and
GCC 13.3.0. It checked out the exact implementation SHA and pristine OpenFHE
commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

The default warning-clean build succeeded first, including these inherited
targets:

- `openfhe_2023_1788`;
- `tensor2_api_contract_test`;
- `dcp_rcb_test`;
- `tensor2_test`.

The separately invoked non-CTest target `relin2_api_contract_test` then failed
with exit code `2` for all three intended absent public symbols:

1. `PairLifecycle::ReadyForRS2`;
2. `PaperScaleDescriptor::approximateRecombinedLogicalScalingFactor`;
3. `DoubleCKKS::Relin2`.

The failure was confined to that contract target. CTest was skipped after the
compile red, so this receipt makes no `6/6` runtime-test claim. The Node 20
deprecation annotation belongs to GitHub Actions dependencies and is not a
project compiler-warning result.

## Windows disposition

After the completed Linux job log was downloaded through the job-log API,
Codex submitted one cancellation request for the whole non-final run. The
Windows job `windows-mingw64` (`99923428332`) reached terminal
`completed/cancelled` while building pristine OpenFHE, at approximately 49%.
Project configure/build, the Relin2 contract target, and CTest were skipped.
No Windows pass, red, project-build, or test result is claimed.

## Raw evidence and integrity

The directory retains pre-cancel and terminal run/jobs/check-runs/artifact API
responses, both Linux log retrievals, the cancelled Windows log, the terminal
complete-logs ZIP, exact workflow/source identities, and the cancellation
receipt. `complete-logs-terminal.zip` passed `unzip -t` and contains both job
logs plus per-step files. The artifacts API reported exactly zero uploaded
artifacts; this is retained as zero rather than replaced by an artifact claim.

Gitleaks 8.30.1 (binary SHA-256
`f414bc2fb952be6c9072b75cb411e3368614ef4b16d48dbd9ad238034afd2302`)
reported no leaks. The targeted credential-pattern scan returned no matches,
and the targeted forbidden-filename result is empty. `MANIFEST.sha256` lists
every retained file except itself.

## Boundary decision

The API compile-red boundary is accepted as authentic. The next permitted
implementation commit is the minimal public-declaration and immediate-throw
scaffold green. It must not add Relin2 arithmetic or runtime contract tests,
and it must preserve this red receipt on the separate evidence branch rather
than the implementation branch.
