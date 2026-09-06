# Application and one-shot hosted test plan

These are proposed root-owned commands, **not commands executed against a hosted repository in this response**. No CI dispatch, push, dependency installation or OpenFHE build is included. Use the already built pristine OpenFHE1.5.0 SDK at pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`, native64/backend4, without DEBUG_KEY. Its actual build/source provenance must be retained by root; the test's `required_openfhe_pin` string is a requirement, not an SDK attestation.

## Apply once

In the intended repository root, set `RETURN_DIR` to the extracted return directory. Do not apply on top of another test draft. The packet's `project/` prefix is not part of patch paths.

```bash
set -euo pipefail
: "${RETURN_DIR:?Set RETURN_DIR to the extracted return directory}"
BASELINE=2759fa90840946ef42957c7ba71ebea47e0e4995

git rev-parse HEAD
git status --short
git apply --check --whitespace=error "$RETURN_DIR/patches/precision116-eight-square.patch"
git apply --whitespace=error "$RETURN_DIR/patches/precision116-eight-square.patch"
git diff --check
cmp CMakeLists.txt "$RETURN_DIR/files/CMakeLists.txt"
cmp tests/experimental_precision116_eight_square_test.cpp \
    "$RETURN_DIR/files/tests/experimental_precision116_eight_square_test.cpp"
# The only permitted changes under these paths are the new test itself.
git diff --exit-code "$BASELINE" -- src include .github/workflows tests \
    ':(exclude)tests/experimental_precision116_eight_square_test.cpp'
```

The default option is OFF: unchanged legacy configurations do not register the new CTest. When ON, the new executable is still excluded from `all`; explicitly build its target. Use a **separate opt-in build directory**, not a legacy build whose old `ctest -E` selector might automatically include the new test. Do not invoke the old full-chain test or seam runner as part of this one-chain task.

## Platform preparation

Linux GCC, in the same kind of environment as the supplied GREEN log:

```bash
export HOST_LABEL=linux-gcc
: "${OPENFHE_PREFIX:?Use the existing pinned SDK installation}"
prefix="$OPENFHE_PREFIX"
export LD_LIBRARY_PATH="$prefix/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
```

Windows: use the existing **MSYS2 MINGW64 Bash** environment/toolchain from the supplied GREEN job, not an unrelated PowerShell/MSVC configuration. The supplied job used `OPENFHE_PREFIX=C:\openfhe-2023-1788\openfhe-install`; this is evidence, not an assertion that that path exists in a new host. Use the actual already provisioned prefix:

```bash
export HOST_LABEL=windows-mingw64
: "${OPENFHE_PREFIX:?Use the existing pinned MinGW64 SDK installation}"
prefix="$(cygpath -u "$OPENFHE_PREFIX")"
export PATH="$prefix/bin:$prefix/lib:$PATH"
```

## Common configure/build/exactly-one-test invocation

Run this block once on each prepared host, after separate review. Its attempt/build directories must not already exist. Change the attempt label only for a separately authorized, retained later attempt—not automatic retries.

```bash
set -euo pipefail
: "${HOST_LABEL:?Set the platform label above}"
: "${prefix:?Set the existing SDK prefix above}"
BASELINE=2759fa90840946ef42957c7ba71ebea47e0e4995
attempt_label=precision116-eight-square-01-attempt-1
build="$PWD/build-$attempt_label-$HOST_LABEL"
attempt="$PWD/evidence-$attempt_label-$HOST_LABEL"
test ! -e "$build"
test ! -e "$attempt"
mkdir "$attempt"

{
    git rev-parse HEAD
    git status --short
    git diff --check
    git diff --exit-code "$BASELINE" -- src include .github/workflows tests \
        ':(exclude)tests/experimental_precision116_eight_square_test.cpp'
    sha256sum CMakeLists.txt tests/experimental_precision116_eight_square_test.cpp \
        tests/paper_full_eight_square_oracle.h tests/experimental_precision116_profile_seam.h
    cmake --version
    c++ --version
} > "$attempt/identity.txt" 2>&1
# Includes any tracked test changes; hashes above also identify an untracked new test.
git diff --binary "$BASELINE" -- CMakeLists.txt tests > "$attempt/test-diff.patch"

cmake -S . -B "$build" -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_PREFIX_PATH="$prefix" \
    -DOPENFHE_2023_1788_ENABLE_EXPERIMENTAL_PRECISION116_EIGHT_SQUARE=ON \
    2>&1 | tee "$attempt/configure.log"
cmake --build "$build" --target experimental_precision116_eight_square_test --parallel 2 \
    2>&1 | tee "$attempt/build.log"
ctest --test-dir "$build" -N -R '^experimental_precision116_eight_square_contract$' \
    2>&1 | tee "$attempt/selection.log"
# The listing must contain exactly one test, with the exact name above.
# No --repeat, until-pass, retry wrapper, alternative vector, or seed selection.
set +e
ctest --test-dir "$build" --verbose --output-on-failure \
    -R '^experimental_precision116_eight_square_contract$' \
    2>&1 | tee "$attempt/ctest.log"
codes=("${PIPESTATUS[@]}")
set -e
printf 'ctest_exit=%s\ntee_exit=%s\n' "${codes[0]}" "${codes[1]}" > "$attempt/exits.txt"
cat "$attempt/exits.txt"
test "${codes[1]}" -eq 0
exit "${codes[0]}"
```

CTest sets OMP_NUM_THREADS=2, RUN_SERIAL and TIMEOUT1200. Runtime output is intentionally plain CTest/stdout. Retain the unmodified logs, identity/diff, CMake cache/SDK provenance on the host, and every attempted outcome. Nothing should be silently overwritten or published into the old endpoint record directory.

## Interpretation before a production decision

A full numerical observation requires a successful compile/link, a started named test, exact candidate/input establishment, all eight operations reaching the numerical predicates, valid physical/receipt/independent-oracle checks, successful ownership cleanup, and an unambiguous terminal COMPLETE result. Inspect the transcript, not just CTest's status or START fields. CTest may return a generic nonzero integer for executable failures.

`COMPLETE result=PASS` with executable exit0 is a bounded candidate PASS. `COMPLETE result=FAIL` with executable exit1 and valid structural/oracle checks is a valid numerical failure; retain every failed gate and measured value. `ABORT result=INVALID_OR_INCOMPLETE`, compiler/linker/setup failure, missing executable, timeout, crash, logging failure or no COMPLETE record is **not numerical RED/PASS**. Retain partial observations without promotion. START's squares/slots/requested-chain fields describe the contract, not completed operations.

Do not fix production from an invalid/harness observation. Isolate a demonstrated test defect, have its correction reviewed, retain the superseded attempt and exact corrected source, then run the corrected frozen test before deciding on production. The first valid test may pass. No manufactured missing-symbol RED is appropriate for this already implemented factory.
