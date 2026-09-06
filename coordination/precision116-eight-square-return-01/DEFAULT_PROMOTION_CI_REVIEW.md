# Default-promotion CI review

## Disposition

**Accepted; no scoped blocker found.** Against HEAD `bee9cc9b78b841b0a876430238b811921468dca7`, `.github/workflows/dcp-rcb.yml` changes exactly eight condition lines: on both Linux and Windows, the default ref `refs/heads/cleanroom/reimplement-mult2-20260831` is added as an exclusion for the old paper-target build, endpoint runner, `always()` selector, and `always()` uploader. No step command or body changed.

On default, the standard 60-test suite, public API builds, and focused client/repeated/h128 checks remain enabled on prior-step success. The old paper full target and endpoint run/select/upload are disabled even on failure paths. The RED/one-operation/eight-square observation steps remain exact-ref-only and therefore disabled on default; the dedicated eight-square observation-ref behavior is unchanged. Because the only Boolean delta is `&& github.ref != DEFAULT_REF`, behavior is preserved for every non-default ref, including workflow dispatch.

The updated checker explicitly tests these default success/failure/selector states and excludes only this intentional default exception from its prior-branch equivalence audit. Retained TDD evidence is coherent:

- `DEFAULT_CI_RED.txt`, SHA-256 `f446eece2db91516410fbaddda52b38a0ffad9d936911f6d3b00b2016b57e4c8`: 6 tests, 5 PASS / 1 expected FAIL because the old paper target was still enabled.
- `DEFAULT_CI_GREEN.txt`, SHA-256 `d99b7286f7f9b3cc57caaab5a58a737014e83bca6c44df16fb7f9707ec0a98e4`: 6/6 PASS.
- Current workflow SHA-256 `a647ce82e9e7c9658706d6ef1af09611640123df2eadebcad69c114818dcba11`; checker SHA-256 `1f1267f046a4a45ea8ebae25b025ac91358542bb8f5c6e78594c8c9f3fe54c25`.

`git diff --quiet 2b8b349edf5575556347082c1b725f6696c743b6 -- include src tests CMakeLists.txt` returned 0: production, CMake, and tests are unchanged. This is a source/evidence review only; I ran no checker, compiler, CTest, cryptography, CI, network, or push operation.
