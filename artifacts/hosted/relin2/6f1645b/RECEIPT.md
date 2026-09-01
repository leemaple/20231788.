# Relin2 API scaffold-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **Linux API scaffold green observed and closed**. This proves only the
minimal public API and immediate-throw scaffold. It is not Relin2 algorithm
green, runtime Relin2-contract green, whole-workflow green, or a Windows pass.

## Exact source identity

- implementation branch: `agent/codex-relin2-01`;
- commit: `6f1645b97ce5b2175530cde5bfd0929370997634`;
- parent: `557d2a331658a2cf16d47de36415c2d968e62b5f`;
- tree: `353a6f551deb186e6a01bb2fa6348b7ebb0021b6`;
- subject: `feat: add Relin2 API scaffold`;
- local HEAD, upstream, and `git ls-remote` branch SHA matched exactly after
  the non-force push.

The commit changes only `include/openfhe_2023_1788/double_ckks.h` and
`src/double_ckks.cpp`, with eight inserted lines. It appends
`PairLifecycle::ReadyForRS2`, appends
`PaperScaleDescriptor::approximateRecombinedLogicalScalingFactor` as an exact
`long double`, declares the public const `DoubleCKKS::Relin2` seam, initializes
the new DCP descriptor field to the fresh recombined recorded scale, and adds
an implementation that immediately throws exactly
`std::logic_error("DoubleCKKS: Relin2 is not implemented")`.

The commit adds no Relin2 validation, raw ciphertext access, evaluation-key
lookup, arithmetic, runtime Relin2 tests, CMake edits, or workflow edits.

## Hosted run identity

- workflow: `DCP RCB TDD`, workflow id `346498968`;
- run: `33529249978`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33529249978`;
- head branch/SHA: `agent/codex-relin2-01` /
  `6f1645b97ce5b2175530cde5bfd0929370997634`;
- run start: `2026-09-01T16:00:44Z`;
- terminal state: `completed/cancelled` at `2026-09-01T16:04:24Z`.

The run-level `cancelled` conclusion is expected for this intermediate
boundary and is not called workflow green.

## Linux observation

Job `linux-gcc` (`99927872443`) completed `success` on Ubuntu 24.04.4 with
CMake 3.31.6 and GCC 13.3.0. It checked out the exact implementation SHA and
pristine OpenFHE commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`.

The default warning-clean build succeeded. The separately invoked non-CTest
target `relin2_api_contract_test` then compiled and linked successfully,
closing the preceding compile-red boundary for all three public symbols.

CTest passed exactly these six inherited tests in 0.12 seconds:

1. `dcp_rcb`;
2. `tensor2_valid_arithmetic_immutability`;
3. `tensor2_result_scale_contract`;
4. `tensor2_right_input_validation`;
5. `tensor2_mutual_compatibility`;
6. `tensor2_prearithmetic_key_compatibility`.

These are inherited DCP/RCB/Tensor2 tests. No runtime Relin2 test existed at
this boundary, so the `6/6` result is not a claim that Relin2 works. The Node
20 deprecation annotation belongs to GitHub Actions dependencies and is not a
project compiler-warning result.

## Windows disposition

After the completed Linux job log was downloaded through the job-log API,
Codex submitted one cancellation request for the whole non-final run. The
Windows job `windows-mingw64` (`99927872804`) reached terminal
`completed/cancelled` while building pristine OpenFHE, at approximately 49%.
Project configure/build, the Relin2 contract target, and CTest were skipped.
No Windows pass, project-build, API-contract, or test result is claimed.

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

The minimal API scaffold boundary is accepted as authentic. The next
permitted implementation work is an independently observable runtime Relin2
contract red, followed by only the minimum production change needed to make
that one behavior green. Core arithmetic must not be implemented ahead of its
red test, and this receipt remains isolated on the evidence branch rather than
the implementation branch.
