# EXECUTION NOTES

## Decision

`READY_FOR_CODEX_INTEGRATION`

This is a source-integration decision, not a compiled-test verdict.

## Actually executed in this container

1. Recomputed the input ZIP SHA-256 and byte size.
2. Ran ZIP member-integrity checking.
3. Recomputed every manifest-listed byte count and SHA-256 and checked for missing/extra files.
4. Read the complete `TASK.md`, complete paper text, relevant paper PDF pages, supplied project sources/tests, and pinned official OpenFHE reference files.
5. Constructed the 12 ordered red—green/coverage/CI patches.
6. Replayed all 12 patches with `git am` on a fresh copy of the exact supplied `project/` bytes.
7. Compared the replayed file bytes and Git tree with the final candidate.
8. Ran `git diff --check`, tree/status/diff checks, patch-state scans, and forbidden-operation scans.
9. Parsed the workflow YAML and checked branch/job/API-target/CTest wiring.
10. Performed a CMake dependency-availability probe.
11. Generated and reverified package-wide SHA-256 manifests.

## Dependency probe outcome

The container has GCC 14.2.0, Clang 17.0.0, CMake 3.31.6, and Git 2.47.3, but no pristine OpenFHE installation. CMake detected GNU C++ and then stopped at:

```text
Could not find a package configuration file provided by "OpenFHE"
(requested version 1.5.0)
```

Therefore:

- successful project configure: `NOT EXECUTED`;
- compilation: `NOT EXECUTED`;
- warning build: `NOT EXECUTED`;
- Add/Sub API target builds: `NOT EXECUTED`;
- runtime red/green tests: `NOT EXECUTED`;
- focused/full CTest: `NOT EXECUTED`;
- Windows/MinGW64: `NOT EXECUTED`;
- GitHub Actions: `NOT EXECUTED`.

The probe transcript is `checks/cmake-dependency-probe.txt`. No build was attempted on the user's Mac.

## Exact commands pending on Linux/GCC

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=<pristine-openfhe-1.5.0-install>
cmake --build build --parallel 2
cmake --build build --target pair_add_api_contract_test --parallel 2
cmake --build build --target pair_sub_api_contract_test --parallel 2
ctest --test-dir build -R '^pair_' --output-on-failure
ctest --test-dir build --output-on-failure
```

## Exact commands pending on Windows/MinGW64

```bash
prefix='<pristine-openfhe-1.5.0-install>'
build='build-mingw64'
cmake -S . -B "$build" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$prefix"
cmake --build "$build" --parallel 2
cmake --build "$build" --target pair_add_api_contract_test --parallel 2
cmake --build "$build" --target pair_sub_api_contract_test --parallel 2
export PATH="$prefix/bin:$prefix/lib:$PATH"
ctest --test-dir "$build" -R '^pair_' --output-on-failure
ctest --test-dir "$build" --output-on-failure
```

## Red—green evidence still required

A hosted executor should check out the exact baseline and stop after each patch 01–08 to capture the actual expected compile/runtime failure or success. Testing only the final tree is insufficient to prove that the red slices were observed.

## Historical CI boundary

The task's references to Actions runs `33833020685` and `33834861766` are packet-provided historical evidence. They are not execution of this candidate. In particular, no claim is made that the latter run's Windows job completed successfully.
