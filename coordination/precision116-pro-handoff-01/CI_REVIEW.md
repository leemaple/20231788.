# Independent CI-only review

Date: 2026-09-07 Asia/Shanghai. Reviewer: `/root/endpoint_cpp_interop`, an independent Codex context; exact backend and effort are unattested, so this is not provider-diverse review. Fixed point and current `HEAD`: `50250a2f6563d8382fd4ae48bad9bb8de791f8d8`.

Reviewed exact files:

- baseline `.github/workflows/dcp-rcb.yml` SHA256 `8d6f0a2d13b891b46e1feaaaa68831549f8639dd2ed1ee84de6c5b55c9c97760`;
- working `.github/workflows/dcp-rcb.yml` SHA256 `476ee2d4fd3e6cd797406642f9c300121988a0e10348fc23509489a655a07891`;
- `coordination/precision116-pro-handoff-01/check_ci_wiring.py` SHA256 `8e1bd1da18857cdcc60e865b1975f21d083b220521fba1198bb81fda17d06317`;
- `coordination/precision116-pro-handoff-01/CI_PLAN.md` SHA256 `fc3fbcded98db502dddea414838592901a38bdea73da3aa8c714556bc9ef6f19`;
- unchanged `CMakeLists.txt` SHA256 `3c033adc985e9a883ce7a7c6d894e1b82c9db76436e4134aaa564658f8e2cc17`.

The checker was read before the plan, workflow diff, or its execution result. The final checker was then run once with `python3 -B -I coordination/precision116-pro-handoff-01/check_ci_wiring.py`: exit 0, five tests passed in 0.021 seconds. This is a static source check, not hosted compile RED or cryptographic execution.

## Standards

No material finding. The diff is confined to the workflow; source and CMake are unchanged. Existing steps, commands, job configuration, cache condition, action pins, five API builds, and original paper execution remain intact except for the explicitly required branch conditions and test-selection regexes. The duplicated Linux/Windows wiring follows the workflow's existing platform-specific command boundaries.

## Spec

No material finding. Exactly the RED and GREEN refs are appended. Both 57-test and 60-test selections exclude the new CTest in both jobs while preserving the prior selected-name sets. The paper target build precedes an exact-RED guard that exits 1 on unexpected success. Only the exact GREEN ref can run the new CTest, with `--no-tests=error`, exact-name selection, `OMP_NUM_THREADS=2`, and a 20-minute outer timeout. The endpoint runner, `always()` selector, and `always()` uploader are all false on both experimental refs, including after a compile failure. The two new steps are exact-ref predicates and therefore preserve prior behavior on pre-existing refs, whether invoked by push or `workflow_dispatch`; dispatch on a new experimental ref follows its new branch gates.

The checker is deliberately bounded rather than a complete GitHub-expression interpreter. Its five assertions cover the requested truth table and current diff; direct source comparison supplies the complementary proof that there is exactly one new guard/mode per job and that unchanged command bodies remain byte-preserved. No hidden endpoint upload, source/CMake mutation, full-chain invocation, security gate, or success translation was found.

Verdict: CI-only diff accepted for the later hosted RED/GREEN sequence. Hosted compilation, CTest execution, cryptography, and runtime behavior remain pending. No CI, browser, build, cryptography, Git write, or source edit was performed in this review; only this requested review file was added.
