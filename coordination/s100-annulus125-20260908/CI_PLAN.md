# S100 annulus125 isolated CI plan

Recorded 2026-09-08 Asia/Shanghai against integration commit
`36e192390a67bb36d2b10fdd20d0c652a030ac47` on
`codex/s100-annulus125-20260908`.

## Frozen initial boundary

`.github/workflows/s100-annulus125.yml` is a new, dedicated Linux workflow.
It is push-enabled only for the exact branch above and only for project source,
tests, CMake, the workflow, and the named scalar/harness files. Documentation-only
pushes do not match. There is no `workflow_dispatch` fallback because a new
workflow file need not be registered on the default branch.

The checked-in mode is `compile-controls-only`. The workflow contains no encrypted
experiment step, no `--output` binary invocation, and no live TSV replay. It:

1. checks the exact project SHA and clean checkout;
2. runs synthetic-only receiver and harness checks;
3. checks out only official OpenFHE commit
   `df495ba2e91739a6dc8f1de254fc5a41155ce504`, including submodules, and checks
   the clean source and sole official main remote;
4. reuses the existing native64/backend4 cache key or builds OpenFHE with two
   workers, OpenMP enabled, and tests/examples/benchmarks/extras disabled;
5. creates a new isolated opt-in project build with owner-only permissions and
   warning-as-error, then builds only `s100_annulus125_eight_square_test`;
6. uses CTest `--show-only=json-v1` and requires the exact single registered name
   `s100_annulus125_eight_square_v1`; it never executes that CTest;
7. directly invokes only `--controls`, whose accepted stdout explicitly records
   `encrypted_runs=0`; and
8. uses an `always()` artifact step for whatever compile/control evidence exists.

The evidence directory originally used `runner.temp` in top-level `env`. Root
checked the GitHub context-availability documentation and identified that `runner`
is unavailable there. It now uses `github.workspace` under the ignored, non-hidden
`artifacts` directory so upload-artifact's default hidden-file exclusion does not
drop the evidence. The source guard rejects a future top-level `runner` context regression. Reference:
<https://docs.github.com/en/actions/reference/workflows-and-actions/contexts#context-availability>.
Hidden-directory exclusion was also checked against the
[pinned upload-artifact documentation](https://github.com/actions/upload-artifact/blob/v4.6.2/README.md#uploading-hidden-files).
Root's final dependency check added the actually invoked comprehensive-reassessment
agreement regression script to both trigger and guard allowlists.

Permissions are `contents: read`; checkout credentials are not persisted. No
secrets or credentials are passed to build or control commands.

## Future runner is intentionally unwired

`run_once.py` exists for a later, separately reviewed source change. The initial
workflow does not invoke it. Its narrow contract is one direct binary process,
1200-second default timeout, `OMP_NUM_THREADS=2`, an exclusive mode-0700 result
directory, a durable program-start receipt before launch, separate stdout/stderr,
raw TSV preservation, and a final receipt with the actual return code and timeout
flag. A timeout has no `CompletedProcess.returncode`, so the receipt records null
and `timed_out=true`; only the wrapper exits 124. The helper catches only
`subprocess.TimeoutExpired`. Existing result-directory creation fails before a
second process can start, so it has no retry/resume path.

Before any encrypted invocation, root must review and deliberately add a one-shot
workflow entry bound to the then-current accepted source. That future change must
call either the direct binary helper or CTest, never both, and must replay the same
raw TSV at decimal precisions 180 and 230. Those are two scalar reads of one file,
not two encryptions. Failure evidence must remain archived; no automatic retry is
permitted.

## Local TDD receipt

The source/harness checker was written first. Command:

```text
python3 -B coordination/s100-annulus125-20260908/check_execution_guards.py self-test
```

RED before adding the workflow and runner: exit 1, four tests discovered, three
errors. The missing workflow caused one error and the missing runner caused two;
the then-present transition-only unit test passed. No CMake, compiler, FFT, CTest,
key generation, encryption, or project binary ran.

After narrowing the design to the accepted compile/controls-only boundary, GREEN:
exit 0, three tests passed in 1.734 seconds. The tests parse the actual YAML, require
the exact branch/path/permission/job and non-execution contract, use a fake process
to preserve return code 7 plus stdout/stderr/raw output, prove a second start in the
same directory is rejected, and use a separate fake process to exercise the sole
timeout translation boundary.

After the final context/path and evidence hardening, the same three tests passed in
1.928 seconds. Integrated receiver self-test, LF-framing regression, and terminal
producer/observer disagreement regression each exited 0 and explicitly reported
zero live encrypted runs. AST parsing of the two owned Python files and
`git diff --check` also passed.

These are source and fake-process observations only. They do not prove GitHub YAML
acceptance, cache integrity, compilation, control-binary success, or cryptographic
correctness. Actual compile/control evidence remains pending the first authorized
push-triggered run. Requested model selector/worker identity is
`requested-unverified` because no independently attestable backend receipt was
available.

## Claim boundary

The annulus input is a controlled one-sample experiment candidate. It does not fix
or supersede the retained original near-unit S100 E80 FAIL, prove Table 3 statistics
or performance, certify security, establish a formal transcendental error bound,
or generalize from one future key/noise sample to the annulus. The initial workflow
does not produce even that one sample.
