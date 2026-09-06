# Hosted synthetic C++ endpoint interoperability RED boundary

Status: workflow-only RED draft authored from clean-room source
`84edbec31b36cfa01481d3f775607e784a0017f7` on 2026-09-06. It has not been
pushed, dispatched, compiled, or run on GitHub Actions.

## Purpose and exact trigger

This independent workflow connects the existing
`paper_full_eight_square_contract_test` executable to the separately authored
`tests/hosted_paper_endpoint_cpp_interop.py` gate. Only a push to the exact
branch `codex/endpoint-cpp-interop-red-20260906` can trigger it. There is no
`workflow_dispatch`, pull-request trigger, schedule, reusable-workflow entry,
or live-branch trigger.

The original `.github/workflows/dcp-rcb.yml`, `CMakeLists.txt`, production code,
and existing tests remain untouched. The workflow configures the project and
builds only the existing `paper_full_eight_square_contract_test` target with
parallelism two. It does not perform a default project build, enumerate or run
CTest, run the 60-test suite, build public-API targets, invoke a self-test, or
run the paper's encrypted chain.

## Hosted jobs

Ubuntu 24.04 and Windows Server 2022 MINGW64 jobs run independently, each with a
60-minute job limit. The single interop step has an additional ten-minute
limit. Both jobs set `OMP_NUM_THREADS=2`,
`RUN_ENDPOINT_COMPLETE_GATE=1`, `RUN_ENDPOINT_NEGATIVE_GATE=1`,
`RUN_ENDPOINT_CPP_INTEROP_GATE=1`, and the platform-matching
`ENDPOINT_TEST_HOST`.

Before the C++ interop call, each job uses native setup-python 3.12 to execute
`tests/hosted_paper_endpoint_finalizer_complete.py` once and
`tests/hosted_paper_endpoint_finalizer_negative.py` once. The job reports the
actual interpreter executable, version, implementation, platform, source path,
and exact source commit. On Windows, `path-type: inherit` exposes the
setup-python interpreter inside MSYS; the workflow converts both paths with
`cygpath -am` and requires the resolved `command -v python` path to equal the
action's `python-path` output.

The dependency boundary follows the proven DCP/RCB workflow: official pristine
OpenFHE 1.5.0 is fixed at
`df495ba2e91739a6dc8f1de254fc5a41155ce504`, checked for exact provenance, and
configured with tests, examples, benchmarks, and extras disabled. Linux reuses
the existing exact-key install cache; a miss builds and installs the pristine
dependency with parallelism two. Windows performs a fresh manual exact-SHA
checkout, recursive submodule initialization, build, and install under
`C:/root`, outside the repository's trailing-dot GitHub workspace. It does not
reuse a Windows OpenFHE cache.

## Native producer and scratch contract

The gate interface is exactly:

```text
python -B tests/hosted_paper_endpoint_cpp_interop.py \
  --producer ABS_NATIVE_EXISTING_EXECUTABLE \
  --output-root ABS_NATIVE_FRESH_PATH
```

Linux passes the real absolute executable and a descriptive fresh scratch root
`$RUNNER_TEMP/fs-cpp-$GITHUB_RUN_ID-$GITHUB_RUN_ATTEMPT`. Windows converts the
real MINGW-built `.exe` and `$RUNNER_TEMP/fi` to native absolute paths before
passing them to native Python. The short Windows leaf is deliberate: the
publisher repeats the long live identity stem in its private staging path, and
the gate preflights the actual deepest candidate against a conservative
247-character ceiling. Each hosted job has a fresh exclusive VM, the output
stem still binds source/run/attempt identity, and the gate refuses a
pre-existing scratch path. The workflow performs no deletion, pre-clean,
retry, or alternate-root fallback.

## Expected RED and retained diagnostics

The present executable is expected to reject the new producer arguments
`--endpoint-cpp-interop <parent> <host> <run> <attempt>` with exit code 1 because
that mode is not wired yet. That observed producer rejection—not dependency
checkout, bootstrap, configuration, compilation, Python regression, path, or
artifact failure—is the intended RED.

The Python gate creates `producer.stdout.txt`, `producer.stderr.txt`, and
`gate-receipt.json` before checking the producer return. It requires an actual
producer return of zero before independently replaying the actual data and
running the finalizer. The gate constructs `output-root/primary.ctest.log` as
explicit synthetic legacy framing for one test (nine scale receipts); it is not a real CTest execution
and deliberately differs from the actual producer exit. A COMPLETE result may
create only `published/<live-schema-stem>/<stem>.tsv.gz` and
`published/<live-schema-stem>/<stem>.status.json`; the corresponding
`canonical/<synthetic-stem>/...` tree remains private. Neither this workflow
nor the receipt may claim real CTest evidence.

An `if: always()` upload step selects exactly those three fixed diagnostic file
paths. The artifact is named
`fs-endpoint-synthetic-cpp-interop-<host>-<run>-<attempt>` with retention three
days, compression level zero, and `if-no-files-found: error`. No directory,
glob, canonical payload, manifest, source checkout, environment, or credential
path is uploaded. The earlier failing interop step is not converted into
success.

After that diagnostic upload, a separate step is conditional on the interop
step's own successful outcome. The gate validates exact publication identity,
status facts, and gzip round trip before returning zero; only then does this
step upload the two literal
`published/<live-schema-stem>/<stem>.{tsv.gz,status.json}` paths as
`fs-endpoint-synthetic-cpp-interop-publication-<host>-<run>-<attempt>`, with the
same three-day retention, compression level zero, and missing-file failure.
There is no directory or wildcard selection. On the expected absent-mode RED,
this publication upload is skipped.

The future C++ mode remains a no-crypto producer seam: it may apply the real
diagnostic/observer transforms to zero polynomials and call the real write/emit
path, but it must not create an FHE context, keys, ciphertexts, encryption,
decryption, multiplication, or paper chain.

## Action and ownership boundary

Action refs already proven by the endpoint protocol workflows are pinned:
checkout v4 `11d5960a326750d5838078e36cf38b85af677262`, setup-python v5
`a26af69be951a213d495a4c3e4e4022e16d87065`, setup-msys2 v2
`66cd2cce69caa17b53920067426061ca1de3a884`, and upload-artifact v4
`ea165f8d65b6e75b540449e92b4886f43607fa02`. The official cache v4 tag was
resolved read-only to `0057852bfaa89a56745cba8c7296529d2fc39830` for this draft.

This slice owns only:

- `.github/workflows/endpoint-cpp-interop-gate.yml`
- `coordination/fs-endpoint-cpp-interop-hosted-01/WORKFLOW_BOUNDARY.md`

No compile, test, CI, push, dispatch, artifact, or live-chain result is claimed.
