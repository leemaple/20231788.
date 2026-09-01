# Relin2 executed test record

Prepared: 2026-09-01 +08:00

All statuses below are from commands actually executed in this workspace. No Windows or hosted CI result is inferred.

## Environment

- Linux: `Linux localhost 6.18.35 ... x86_64 GNU/Linux`
- compiler: `g++ (Debian 14.2.0-19) 14.2.0`
- CMake: `3.31.6`
- CTest: `3.31.6`
- pristine OpenFHE source: supplied 1.5.0 tree bound to `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- local OpenFHE install prefix: `/mnt/data/relin2_openfhe_install`
- project configuration command used at every patch boundary:

```sh
cmake -S . -B /mnt/data/relin2_boundary_build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=/mnt/data/relin2_openfhe_install
```

The authoritative downstream form with `${OPENFHE_PREFIX}` is equivalent; the absolute local prefix above records what was actually executed.

## Input/package gate

Executed SHA-256 checks for all three uploads and matched the supplied values. ZIP central-directory count was 2,266; path-safety scan found zero unsafe paths. `unzip -t` passed. Fresh extraction followed by `sha256sum -c MANIFEST.sha256` verified all 1,997 listed files. Internal handoff, manifest, task, source/test, paper, OpenFHE identity, Tensor2 evidence, preflight, task-audit, and provenance hashes matched their binding records. A custom Git object-hash recomputation over `cleanroom-project/` produced tree `759d5195739684748d5a9664edabe3fa719e1acf`.

## Local pristine OpenFHE build

Configured Release OpenFHE with unit tests/examples/benchmarks/extras disabled and installed to `/mnt/data/relin2_openfhe_install`. Two early build invocations and the first build+install invocation were terminated by the execution wrapper time limit while making forward progress; they are not reported as compiler failures. Continuing the same build completed to 100%, and a separate `cmake --install` completed successfully. `OpenFHEConfig.cmake` was then present under the install prefix. No OpenFHE source was modified.

## Patch 01 — `01-red-relin2-api.patch`

Executed:

```sh
git apply --check --whitespace=error-all 01-red-relin2-api.patch
cmake -S . -B /mnt/data/relin2_boundary_build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/mnt/data/relin2_openfhe_install
cmake --build /mnt/data/relin2_boundary_build --target openfhe_2023_1788 --parallel 2
cmake --build /mnt/data/relin2_boundary_build --target relin2_api_contract_test --parallel 2
cmake --build /mnt/data/relin2_boundary_build --parallel 2
cmake --build /mnt/data/relin2_boundary_build --target dcp_rcb_test tensor2_test --parallel 2
ctest --test-dir /mnt/data/relin2_boundary_build -R '^(dcp_rcb|tensor2_)' --output-on-failure
```

Results: apply-check 0; configure 0; production library 0. The API target exited 2, specifically naming all three required missing symbols/contracts: `PairLifecycle::ReadyForRS2`, `PaperScaleDescriptor::approximateRecombinedLogicalScalingFactor`, and `DoubleCKKS::Relin2`. Full build also exited 2 solely when it reached that intended API-red target. The accepted six runtime CTests were then built and executed separately: 6/6 passed. This is the intended compile red; production library remained buildable.

## Patch 02 — `02-api-scaffold.patch`

Executed apply-check, the same configure command, `cmake --build ... --target relin2_api_contract_test --parallel 2`, full `cmake --build ... --parallel 2`, and full `ctest --test-dir ... --output-on-failure`.

Results: all apply/configure/build commands 0; 6/6 CTests passed. The API target compiled against the final declarations. `Relin2` remained the required immediate `logic_error("DoubleCKKS: Relin2 is not implemented")` scaffold; no Relin2 behavior was called green at this boundary.

## Patch 03 — `03-red-relin2-contract.patch`

Executed apply-check and configure. The first `relin2_test` target-build attempt was terminated by the execution wrapper during compilation; continuing the same target completed with status 0. Full build then completed with status 0. Full CTest was executed and returned 8: 6 accepted tests passed and every one of the 28 new Relin2/legacy-contract cases was independently executed and failed, for `18% tests passed, 28 tests failed out of 34`.

Observed red attribution:

- Relin2/key/valid-shape cases reached the scaffold and rejected its `logic_error`/non-result rather than silently passing.
- recombined-field RCB and Tensor2 cases failed because the legacy contract still did not reject the corrupted field.
- the deterministic DCP fixture printed `positive=(0,0), negative/carry=(0,1)` before reaching the scaffold, proving those witness preconditions were not hidden by the scaffold failure.

No single failure masked the remaining cases; CTest executed all 28.

## Patch 04 — `04-green-relin2-core.patch`

Executed apply-check, configure, `relin2_test` target build, full build, and full CTest. The first full-build invocation was terminated by the execution wrapper while compiling the updated accepted tests; a continuation of the same build completed with status 0. Full CTest status 0: 34/34 passed.

This boundary includes the complete Relin2 core, shared DCP seam, lifecycle/dual-scale validation, key preflight, basis restoration, two public relinearizations, pre-add rejection, exact `(u,v+w)` arithmetic/oracle, RCB support, legacy field regressions, HYBRID/BV adversarial key matrix, and deep immutability. It intentionally does not yet contain the Tensor2 `ReadyForFirstMult` lifecycle guard.

Directly executed final-core witness commands also returned 0:

```sh
/mnt/data/relin2_boundary_build/relin2_test valid_arithmetic_state_immutability
/mnt/data/relin2_boundary_build/relin2_test deterministic_dcp_boundary_witnesses
```

They printed:

```text
Relin2 K witness: component=0,tower=0,coefficient=0
Relin2 v/w witness: component=0,tower=0,coefficient=0
Relin2 deterministic DCP witnesses: positive=(0,0), negative/carry=(0,1)
```

## Patch 05 — `05-red-tensor2-lifecycle.patch`

Executed apply-check, configure, `relin2_test` target build, full build, and full CTest. Builds returned 0. Full CTest returned 8 with 34/35 passing; the sole directed red was `tensor2_rejects_ready_for_rs2`.

The case first used public Relin2 to construct a valid `ReadyForRS2`, then called Tensor2. Instead of the required lifecycle diagnostic, the unguarded implementation continued downstream and produced:

```text
DoubleCKKS: Tensor2 result scale metadata is invalid
```

Thus the red is specifically the missing lifecycle guard, not the earlier Relin2 scaffold.

## Patch 06 — `06-green-tensor2-lifecycle.patch`

Executed apply-check, configure, `relin2_test` target build, full build, and full CTest. All returned 0. Full suite: 35/35 passed. The production change is only the minimal pre-arithmetic `ReadyForFirstMult` guard.

## Patch 07 — `07-final-docs.patch`

Executed apply-check, configure, `relin2_test` target build, full build, and full CTest after the documentation-only change. All returned 0. Full suite: 35/35 passed.

## Ordered patch reapplication audit

On a fresh copy of the exact exported base tree, every final returned patch was independently checked and then applied in order with:

```sh
git apply --check --whitespace=error-all <patch>
git apply --whitespace=error-all <patch>
```

for patches 01 through 07. Every check/application returned 0. `diff -qr` between the resulting tree and the final stage7 tree produced no differences. No `.git` repository was created and no project commit/push/tag/PR/CI dispatch was performed, in accordance with the task's prohibition on those operations.

## Explicitly unrun / pending

- GitHub Actions on any generated patch/result: **pending; not dispatched**.
- final exact-commit `ubuntu-24.04` hosted run: **pending downstream**.
- final exact-commit `windows-2022` / MSYS2 MINGW64 run: **pending downstream**.
- any performance benchmark: **unrun / outside scope**.
- analytic precision/error-bound experiment: **unrun / not justified by the bounded proof contract**.
- RS2, Mult2 wrapper, pair Add/Sub, rotations, refresh/bootstrapping, repeated multiplication, serialization: **unimplemented and unrun by design**.
