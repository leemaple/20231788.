# Test and integration plan

## Status and recovery

**Project NOT COMPILED / NOT RUN in the drafting container.** Only source/patch/scalar checks and a constants-only descriptor excerpt syntax check ran. A real missing-factory compile RED, GREEN project builds, the encrypted one-operation test, legacy60/five API checks and independent review are pending.

Apply patches from a repository whose engineering files match `dbbbee0d20d8a7ae3c138e42f633414db621a173`. Documentation-only HEAD changes, including packaging commit `21d94c23a7163c4770e8ec377219ea54d4428e2d`, are not a different engineering base. Preserve a clean root-owned integration branch and record its actual source commit. `RED/` and `GREEN/` contain lossless complete changed-file copies, not whole repositories: copy RED files onto the baseline, then GREEN files onto that result, only as recovery from unavailable patch tooling.

No workflow is supplied or modified. The commands below are for the integration lead's hosted Linux and Windows jobs with the already provisioned pristine OpenFHE pin, native64/backend4, DEBUG_KEY disabled. Do not run heavy builds/cryptography on the Mac. Root owns separate exact-source RED and GREEN commits, job wiring and retained run receipts; this packet does not dispatch them.

## 1. RED-only application and actual compile receipt

In the root-owned integration checkout, before applying either patch:

```bash
set -euo pipefail
: "${RETURN_DIR:?Path to unpacked return package}"
test -z "$(git status --porcelain=v1)"
git rev-parse HEAD
git diff --exit-code dbbbee0d20d8a7ae3c138e42f633414db621a173 -- \
  CMakeLists.txt include src tests .github/workflows
git apply --check --whitespace=error-all "$RETURN_DIR/RED.patch"
git apply --whitespace=error-all "$RETURN_DIR/RED.patch"
git diff --check
```

Root then binds and publishes only that RED tree through its existing hosted process. GREEN is not applied before the hosted failure has been retained. A successful default build is not the required RED because the paper executable is `EXCLUDE_FROM_ALL`.

Use the following job-local variables on Linux:

```bash
: "${OPENFHE_PREFIX:?Existing installation at the required pristine pin}"
PREFIX="$OPENFHE_PREFIX"
BUILD="$PWD/build-precision116"
EVIDENCE="$PWD/precision116-job-evidence"
```

On Windows, use the existing MSYS2 shell and provisioned Windows dependency. Equivalent path setup is:

```bash
: "${OPENFHE_PREFIX:?Existing pinned Windows installation}"
PREFIX="$(cygpath -u "$OPENFHE_PREFIX")"
BUILD="$PWD/build-precision116"
EVIDENCE="$PWD/precision116-job-evidence"
```

Both hosts then execute this RED compile, not a CTest:

```bash
mkdir -p "$EVIDENCE"
cmake -S . -B "$BUILD" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="$PREFIX"
set +e
cmake --build "$BUILD" --target paper_full_eight_square_contract_test --parallel 2 \
  > "$EVIDENCE/red-build.log" 2>&1
red_rc=$?
set -e
printf 'RED_BUILD_EXIT=%s\n' "$red_rc" | tee "$EVIDENCE/red-build-exit.txt"
cat "$EVIDENCE/red-build.log"
test "$red_rc" -ne 0
grep -F 'CreateExperimentalPrecision116Setup' "$EVIDENCE/red-build.log"
```

Expected leading API diagnostic is that `CreateExperimentalPrecision116Setup` is not a member of `openfhe_2023_1788` (compiler wording varies). The grep is a locator, not a complete adjudicator: retain the native exit, full compiler output, exact project/dependency identities and confirm that this is the missing-factory API failure, not a dependency/configuration error, an earlier unrelated test error or a linker-only failure. No placeholder declaration/definition may be added. This expected failure has **not** been observed in this draft environment.

## 2. GREEN after the real RED receipt

Only after accepting the hosted compile RED, apply GREEN on top of the exact RED tree:

```bash
git apply --check --whitespace=error-all "$RETURN_DIR/GREEN.patch"
git apply --whitespace=error-all "$RETURN_DIR/GREEN.patch"
git diff --check
```

Root binds the resulting GREEN source to its next hosted run. In each host job, use the same path setup above, reconfigure at that actual source commit, and preserve the existing warning-as-error settings:

```bash
set -euo pipefail
export OMP_NUM_THREADS=2
mkdir -p "$EVIDENCE"
cmake -S . -B "$BUILD" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="$PREFIX"
cmake --build "$BUILD" --parallel 2
cmake --build "$BUILD" --target relin2_api_contract_test rs2_api_contract_test --parallel 2

exclude57='^(precision_client_io_first_mult2_contract|repeated_mult2_semantic_two_square_contract|paper_h128_client_keypair_contract|paper_full_eight_square_contract|experimental_precision116_profile_seam)$'
ctest --test-dir "$BUILD" --show-only=json-v1 -E "$exclude57" > "$EVIDENCE/legacy57-selection.json"
ctest --test-dir "$BUILD" --verbose --output-on-failure -E "$exclude57" \
  2>&1 | tee "$EVIDENCE/legacy57.log"

cmake --build "$BUILD" --target mult2_api_contract_test add_api_contract_test sub_api_contract_test --parallel 2
cmake --build "$BUILD" --target precision_client_io_first_mult2_contract_test \
  repeated_mult2_semantic_two_square_test paper_h128_client_keypair_contract_test --parallel 2

exclude60='^(paper_full_eight_square_contract|experimental_precision116_profile_seam)$'
ctest --test-dir "$BUILD" --show-only=json-v1 -E "$exclude60" > "$EVIDENCE/legacy60-selection.json"
ctest --test-dir "$BUILD" --verbose --output-on-failure -E "$exclude60" \
  2>&1 | tee "$EVIDENCE/legacy60.log"

cmake --build "$BUILD" --target paper_full_eight_square_contract_test --parallel 2
ctest --test-dir "$BUILD" --show-only=json-v1 -R '^experimental_precision116_profile_seam$' \
  > "$EVIDENCE/experimental-selection.json"
ctest --test-dir "$BUILD" --verbose --output-on-failure -R '^experimental_precision116_profile_seam$' \
  2>&1 | tee "$EVIDENCE/experimental-one-operation.log"
```

The five API targets are compile-only and must not be counted as five additional CTests. The selection files must contain exactly 57, 60 and 1 tests respectively. Retain native build/CTest exits, source/pin/mode identities and warning-clean output. The existing `--endpoint-observer-self-test` and `--endpoint-cpp-interop` modes are byte-preserved but are not invoked by this new seam; additional legacy job checks remain root-owned.

**Do not run unfiltered CTest.** Appending the new test makes 62 registrations, not a new 60-test default suite. Always exclude the experimental entry from old57/60 checkpoints, even after explicitly building the target, and always exclude the old no-argument `paper_full_eight_square_contract`. Do not invoke the executable without arguments, an endpoint publisher/finalizer, or the old full-chain wrapper for this task. Merely listing CTests is not executing them.

The new CTest has `TIMEOUT=1200`, `RUN_SERIAL=TRUE` and `OMP_NUM_THREADS=2`. Successful machine output is uniquely:

```text
EXPERIMENTAL_PRECISION116_PROFILE_SEAM result=PASS profile=experimental-s116-d56-b58-v1 squares=1 full_eight_square_E80=NOT_TESTED security=UNRESOLVED
```

The FAIL line carries the same experiment scope and a reason; its `squares=1` describes the requested test, not proof that an operation completed before an early failure. No original endpoint schema or live-chain disposition is emitted. A successful line qualifies only this integration seam, not numerical eight-square accuracy, security or projected-key correctness.

## 3. Actual bounded checks and reproduction

From this unpacked return package, with the original supplied ZIP available:

```bash
python3 -B checks/verify_draft.py /path/to/experimental-precision116-profile-seam-dbbbee0d.zip \
  --report /path/to/a-new-static-check-receipt.json
```

Use a new report path rather than overwriting the delivered receipt or manifest. This command verifies input identities/CRCs and closure, actually applies RED then GREEN in a disposable source tree, checks changed-path allowlists and complete-file copies, proves byte preservation of the old test/CMake prefix and critical old source bodies, parses both production/test constants against the frozen candidate, and independently compares every exact scale using recurrence and closed product. It runs the supplied seven stdlib scalar tests and verifies standalone certificate bytes. It also checks C++ delimiter balance and, when available, syntax-checks **only** the exact constants/descriptor excerpt using GCC/Clang; it does not compile the project or fabricate dependency stubs.

Observed here: 15 static check groups PASS, supplied scalar tests 7/7 PASS, descriptor-only C++17 syntax checks PASS on GCC14.2 and Clang17. See `results/static_checks.json` for every executed subprocess argv/cwd/exit/stdout/stderr and exact scale digest. These are not a hosted TDD receipt, a warning-clean project build, a numerical pass or independent sign-off.

## 4. Independent acceptance

After real Linux/Windows execution, independent review must inspect descriptor binding, old-profile equivalence, plan-only S116 I/O, destroyed projected-secret handling, receipt parents/reentry, all-family ownership seals, test completeness and negative rejection semantics. Any actual RED-test defect must receive a separately explained correction rather than weakening assertions inside GREEN. No post-GREEN change to the delivered RED assertions was made here.

Full numerical eight-square verification is a later, separately authorized slice. Original failures, all-key/nonwrap uncertainty and unresolved candidate security remain unchanged throughout this handoff.
