# EXPERIMENTAL-PRECISION116-PROFILE-SEAM-01 CI plan

Plan only: no workflow/CMake/source edit, CI dispatch, build, cryptography, or Git mutation was performed here.

## Source and scope

- Engineering baseline: `dbbbee0d20d8a7ae3c138e42f633414db621a173`.
- Inspected documentation HEAD: `50250a2f6563d8382fd4ae48bad9bb8de791f8d8`.
- Inspected `.github/workflows/dcp-rcb.yml` Git blob: `059dbf3cacfef2286e22a95a982fce3c5d7bc136`.
- Inspected `CMakeLists.txt` Git blob: `0bd6cf1aba78d4d0515e145cbdccd119a9ea6ca5`.
- Exact activation branches: `codex/precision116-profile-seam-red-20260907` and `codex/precision116-profile-seam-green-20260907`. The current documentation/source branch `codex/precision116-profile-seam-20260907` stays untriggered.
- Pro owns the test/CMake RED and production GREEN. Root owns the later workflow edit and actual hosted runs.

Use the same workflow wiring on both activation branches. Push the RED branch once to retain the actual compile failure, then apply GREEN without weakening the RED test and push the GREEN branch once.

## Expected CMake registration from RED

The shared `paper_full_eight_square_contract_test` stays `EXCLUDE_FROM_ALL`. Add one CTest mode:

```cmake
add_test(NAME experimental_precision116_profile_seam
         COMMAND paper_full_eight_square_contract_test
                 --experimental-precision116-profile-seam)
set_tests_properties(experimental_precision116_profile_seam PROPERTIES
    TIMEOUT 900
    RUN_SERIAL TRUE
    ENVIRONMENT "OMP_NUM_THREADS=2")
```

Keep the existing `paper_full_eight_square_contract` command and its `TIMEOUT 1200`, `RUN_SERIAL TRUE`, and `OMP_NUM_THREADS=2` properties unchanged (`CMakeLists.txt:266-271`). Separate property blocks prevent an accidental timeout change to the old test.

This raises the registered CTest count from 61 to 62 before the excluded executable is explicitly built.

## Minimal workflow delta

1. Append only these entries to `on.push.branches`:

```yaml
      - codex/precision116-profile-seam-red-20260907
      - codex/precision116-profile-seam-green-20260907
```

Keep existing triggers and `workflow_dispatch` unchanged; do not dispatch it for this evidence.

2. In both jobs, change both `Run legacy 57-test checkpoint` commands to use this exact exclusion:

```sh
-E '^(precision_client_io_first_mult2_contract|repeated_mult2_semantic_two_square_contract|paper_h128_client_keypair_contract|paper_full_eight_square_contract|experimental_precision116_profile_seam)$'
```

With 62 registrations, excluding five retains 57 tests and avoids the unbuilt shared executable.

3. In both jobs, change both `Run complete 60-test three-track suite` commands to:

```sh
-E '^(paper_full_eight_square_contract|experimental_precision116_profile_seam)$'
```

Excluding the two modes backed by the excluded target retains the old 60-test suite. Preserve all five public-API target builds and all existing focused tests unchanged.

4. Keep the existing explicit `paper_full_eight_square_contract_test` target build immediately after the 60-test suite.

- RED: its new test source references missing `CreateExperimentalPrecision116Setup()` and the target build must fail with that real compiler diagnostic.
- GREEN: the same target builds against the new production API.

Do not add `continue-on-error`, invert the exit, or manufacture a passing RED. A bootstrap, dependency, configuration, legacy-test, warning, timeout, or missing-executable failure is not the intended RED.

5. Immediately after that target build, add one exact-branch step per host.

Linux:

```yaml
      - name: Run experimental precision116 one-operation profile seam
        if: github.ref == 'refs/heads/codex/precision116-profile-seam-green-20260907'
        timeout-minutes: 20
        env:
          OMP_NUM_THREADS: 2
        run: >-
          ctest --test-dir build --verbose --output-on-failure
          --no-tests=error -R '^experimental_precision116_profile_seam$'
```

Windows MINGW64:

```yaml
      - name: Run experimental precision116 one-operation profile seam
        if: github.ref == 'refs/heads/codex/precision116-profile-seam-green-20260907'
        timeout-minutes: 20
        env:
          OMP_NUM_THREADS: 2
        shell: msys2 {0}
        working-directory: C:/openfhe-2023-1788/cleanroom
        run: |
          prefix="$(cygpath -u "$OPENFHE_PREFIX")"
          build="$(cygpath -u "$PROJECT_BUILD")"
          export PATH="$prefix/bin:$prefix/lib:$PATH"
          ctest --test-dir "$build" --verbose --output-on-failure \
            --no-tests=error -R '^experimental_precision116_profile_seam$'
```

CTest's 900-second property is the inner test bound; 20 minutes is the outer step bound. `--no-tests=error` prevents a zero-selection false green.

6. Exclude this branch from all three endpoint steps in both jobs:

- `Run and finalize paper endpoint once`
- `Select exact endpoint upload`
- `Upload exact endpoint evidence`

The runner condition becomes:

```yaml
if: github.ref != 'refs/heads/codex/endpoint-writer-selftest-20260906' && github.ref != 'refs/heads/codex/precision116-profile-seam-red-20260907' && github.ref != 'refs/heads/codex/precision116-profile-seam-green-20260907'
```

The selector and uploader keep their current terms and add the same final branch inequality:

```yaml
if: ${{ always() && github.ref != 'refs/heads/codex/endpoint-writer-selftest-20260906' && github.ref != 'refs/heads/codex/precision116-profile-seam-red-20260907' && github.ref != 'refs/heads/codex/precision116-profile-seam-green-20260907' }}
```

```yaml
if: ${{ always() && steps.endpoint-select.outcome == 'success' && github.ref != 'refs/heads/codex/endpoint-writer-selftest-20260906' && github.ref != 'refs/heads/codex/precision116-profile-seam-red-20260907' && github.ref != 'refs/heads/codex/precision116-profile-seam-green-20260907' }}
```

All six condition edits are required, and each must exclude both activation refs. In particular, `always()` would otherwise run the selector after the intended RED compile failure, fail on an absent scratch publication, and obscure the primary result. The observer self-test already selects only its old branch and needs no change. Add no finalizer, selector, artifact upload, synthetic self-test, or original no-argument full-chain run for this experiment.

## Reuse unchanged

- Linux: Ubuntu 24.04, the existing OpenFHE cache key, pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`, native64/backend4, `--parallel 2`, and the 45-minute job limit.
- Windows: Windows 2022, fresh checkout under `C:/openfhe-2023-1788`, MINGW64 with `path-type: inherit`, native Python identity check, fresh OpenFHE build, native64/backend4, `--parallel 2`, and the 60-minute job limit. Add no cache.
- Keep provenance before project configuration. Live `PAPER_SOURCE_COMMIT` remains the checked-out `GITHUB_SHA`, not the older engineering baseline literal.

## Acceptance and hazards

RED requires both hosts to reach the explicit paper-target build and fail specifically for the absent factory. GREEN requires both hosts to retain the default warning-clean build, old 57 and 60 suites, five API target builds, focused tests, explicit paper target build, and exactly one selected experimental CTest pass.

The new result must report `squares=1`, `full_eight_square_E80=NOT_TESTED`, and `security=UNRESOLVED`. It is not an eight-square precision or security result.

Review the eventual diff for these concrete hazards:

- failing to add the new name to the 57 exclusion produces 58 selected tests and reaches an unbuilt executable;
- excluding only the old paper test from the complete suite produces 61 selected tests and the same unbuilt command;
- focused `ctest -R` without `--no-tests=error` may pass with zero selected tests;
- skipping the endpoint producer but not its `always()` selector creates a misleading secondary failure; and
- direct executable invocation bypasses CTest's timeout, serial, and environment contract.

Before a push, statically verify the exact branch trigger, both checkpoint regexes in both jobs, all six endpoint exclusions, unchanged original paper-test properties, and unchanged five API commands. Those checks and all runtime outcomes remain pending.
