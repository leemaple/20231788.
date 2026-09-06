# CI wiring implementation — 2026-09-07 Asia/Shanghai

This is a narrow workflow change, not the experimental algorithm RED/GREEN. Engineering baseline `dbbbee0d20d8a7ae3c138e42f633414db621a173`; starting documentation HEAD `50250a2f6563d8382fd4ae48bad9bb8de791f8d8`. Pro still owns the unreturned test/CMake and production drafts. Neither has been applied. No source, CMake, original oracle or endpoint publisher changed.

## Actual local checks

Root read CI_PLAN.md and the entire affected workflow. The author of the plan did not edit the workflow. Root added the bounded source-check script before changing the workflow. `actionlint` was absent and bundled isolated Python could not import `yaml`; no packages were installed. The available system Ruby standard YAML parser successfully parsed the baseline and was used by the Python checker. This is YAML/source validation, not execution by GitHub's scheduler.

Command: `/Users/lifeng/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/bin/python3 -B -I coordination/precision116-pro-handoff-01/check_ci_wiring.py`.

- Initial checker on unchanged workflow: 5 tests, 3 failures and 2 errors. One error was the checker's intentionally restricted expression evaluator encountering an unchanged cache condition; root corrected the checker to compare unchanged conditions byte-for-byte and evaluate only edited conditions. The other was the genuinely absent experimental step.
- Corrected checker on unchanged workflow: **1 pass, 3 failures, 1 missing-step error**, exit1, unittest0.002seconds. Exact output retained in CI_STATIC_RED.txt. This proves only missing CI wiring, **not** the missing C++ factory compile RED.
- Root then changed only `.github/workflows/dcp-rcb.yml`, +53/-14 lines. The same corrected checker: **5 PASS**, unittest0.018seconds. `git diff --check` also succeeded. CI_STATIC_GREEN.txt retains the actual combined command output, including diff stat. No hosted run or cryptographic process started.
- Root additionally parsed the actual YAML and passed each of the four new run blocks to `/bin/bash -n`: all four syntax checks passed without executing their contents. This does not establish Windows runtime behavior or hosted scheduling.

Reviewed-before-independent-review hashes:

- Workflow: `476ee2d4fd3e6cd797406642f9c300121988a0e10348fc23509489a655a07891`.
- Checker: `8e1bd1da18857cdcc60e865b1975f21d083b220521fba1198bb81fda17d06317`.
- Plan: `fc3fbcded98db502dddea414838592901a38bdea73da3aa8c714556bc9ef6f19`.

## Actual delta and boundaries

Two future exact activation refs were appended: `codex/precision116-profile-seam-red-20260907` and `codex/precision116-profile-seam-green-20260907`. The current working branch remains outside the push allowlist. Both jobs preserve old57/60 selections while excluding the prospective new test from unbuilt-target checkpoints. Five API builds, default build, original focused commands, toolchains, pristine pin, cache policy and job limits remain unchanged.

The existing paper target builds explicitly. On RED, a successful build now deliberately fails at an explanatory guard: it does not count as the expected missing-factory diagnostic and cannot produce a misleading green workflow. On GREEN only, one exact named CTest uses `--no-tests=error`, OMP2 and a20-minute outer step bound. The test's own timeout/serial settings await the Pro RED CMake patch and will be checked at intake, not fabricated here.

All six endpoint run/select/upload conditions exclude both new refs, including `always()` selection after earlier failure. The original synthetic observer step still selects only its original exact ref. Static checks exercise success/failure and selection outcomes and preserve original-branch condition behavior.

Independent `/root/endpoint_cpp_interop` review is accepted with no material finding, bound to the exact hashes above in CI_REVIEW.md; its one independent bounded checker run passed5 tests in0.021seconds, exit0. Root read the complete review. This separate Codex context is not another provider, and is not a Pro scientific sign-off. Do not push either activation ref until the actual RED test has been reviewed/applied. Push only the untriggered working branch for this preparatory checkpoint. No dispatch/rerun is authorized by this document; future actual engineering runs follow the task's exact-source RED-then-GREEN sequence.

Overall project remains incomplete; original E80 FAIL, experimental one-operation NOT RUN, experimental eight-square E80 NOT TESTED, security UNRESOLVED.
