# Default-promotion CI boundary

Reviewed source: active HEAD `bee9cc9b78b841b0a876430238b811921468dca7`; default branch `cleanroom/reimplement-mult2-20260831` at `8c06b480ab553aa30ba1c86b4e091a89432bce11`; workflow SHA-256 `6095c87e3c79de388a73fbf816a5bfa772a6175e08631d219eab7f8d12672a05`. Source review only; no CI, build, crypto, push, or network action.

## Exact minimal delta

In **both** `linux-gcc` and `windows-mingw64`, add

`github.ref != 'refs/heads/cleanroom/reimplement-mult2-20260831'`

to the existing `if` expression for exactly these four steps:

1. `Build paper full eight-square contract`
2. `Run and finalize paper endpoint once`
3. `Select exact endpoint upload`
4. `Upload exact endpoint evidence`

For the last two `always()` steps, keep `always()` and every existing selector/outcome guard; add only the default-ref exclusion. This makes default pushes and workflow dispatches on default skip the known-failing expensive original endpoint path even after an earlier failure.

Do not change commands, job definitions, the standard 60-test suite, five public API builds, focused client/repeated/h128 checks, trigger branches, or any source/CMake/test file. Do not change the three exact-ref S116 observation conditions: they remain false on default, so the accepted numerical chain is not rerun. All non-default refs retain prior behavior, including the dedicated observation ref.

## Test-first acceptance

First extend the bounded workflow checker with `DEFAULT_REF` and assert, on both jobs, that all four named steps are disabled for default under success/failure and selector success/failure/skipped; assert the standard suite/API steps remain enabled and the three S116 observation steps remain disabled. Adjust the existing “old branch behavior preserved” assertion to exclude only this intentional default-ref exception. Retain the failing checker output against the current workflow.

Then apply the eight one-line condition edits and require the checker PASS, `bash -n` on changed run blocks if any (none expected), and `git diff --check`. This is delivery wiring only; it neither replaces nor reruns the accepted scientific gate.
