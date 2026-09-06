# Live endpoint workflow draft — 2026-09-06

Status: workflow-only draft from exact clean-room base
`e1f5c70c888f2d651d9dce01aec53924e24ce08f`. No local build, CTest,
OpenFHE/FHE execution, numerical replay, GitHub Actions run, dispatch, artifact,
or push is claimed. Live runtime remains pending root's separately owned C++
no-argument integration and one authorized hosted activation.

Root reported that synthetic C++ interoperability run 34037828392 completed
successfully on both hosts and that its exact diagnostics and publication were
independently checked. This draft treats that report as the prerequisite to
workflow authoring; it does not rerun or independently re-accept that evidence.

## Exact owned delta

Only `.github/workflows/dcp-rcb.yml` and this document change.

The workflow trigger gains exactly one branch:
`codex/endpoint-live-capture-20260906`. Existing branches and
`workflow_dispatch` remain byte-for-byte present; this draft does not activate
either trigger.

Both jobs provision pinned native setup-python 3.12 at
`a26af69be951a213d495a4c3e4e4022e16d87065`. Linux requires the resolved
`command -v python` to equal the action's `python-path` output and reports the
actual executable, version, and platform. Windows provisions Python before
MSYS2, adds `path-type: inherit`, converts both interpreter spellings with
`cygpath -am`, requires exact equality, and requires native `os.name == "nt"`.
No Python package install or cache is added, and the completed synthetic
complete/negative/timeout/interop gates are not rerun.

The existing project-source provenance step moves before project CMake
configuration on each host and additionally requires a clean checkout. Thus
CMake's existing configure-time `git rev-parse HEAD` compile definition is
formed only after `HEAD == GITHUB_SHA` is established. The Windows manual
checkout's earlier exact-SHA and clean-tree checks remain unchanged.

Only the final normal paper command is replaced. The wrapper invocation is
exactly one real CTest path with `--scope live-single-chain`; it receives the
existing build directory, fixed host, fresh shell/native scratch spellings,
setup-python executable, and actual CTest executable. The adjacent unfiltered
`ctest --show-only=json-v1` is removed. No synthetic CTest framing, wrapper
retry, `continue-on-error`, or success-forcing exit is introduced.

## Fresh path and Windows ceiling

Each exclusive hosted VM uses the fixed fresh child `$RUNNER_TEMP/fl`. The
workflow rejects any existing file, directory, or symlink at that name and
never removes, pre-cleans, retries, or chooses a fallback. It creates the root
with `umask 077`; the existing wrapper requires the normalized directory to be
empty and creates only `canonical` and `published` beneath it.

Before the Windows root is created or the paper chain starts, native Python
constructs the actual identity stem from `GITHUB_SHA`, host `windows`, run ID,
and attempt. It rejects a `\\?\` spelling or either exact candidate exceeding
247 characters:

```text
<scratch>/canonical/<stem>/.staging/.candidate.tsv
<scratch>/published/<stem>/.staging/<stem>.candidate.status.json
```

The second is the deepest publication transaction path; checking only final
leaves would be insufficient. Linux uses the same short `fl` leaf and POSIX
native/shell identity but does not import the Windows `MAX_PATH` ceiling.

## Failure-preserving selection and upload

After the wrapper step, each job runs the actual finalizer `select` subcommand
under `if: always()`. It passes the same exact source/host/run/attempt identity,
the normalized native `published` parent, and a new manifest at the scratch
root. Windows passes native paths to Python and appends the manifest through
the MSYS spelling of `GITHUB_OUTPUT`.

Upload uses pinned upload-artifact v4
`ea165f8d65b6e75b540449e92b4886f43607fa02` only when selection succeeds. Its
`path` is exactly `steps.endpoint-select.outputs.paths`, with
`if-no-files-found: error`, compression level zero, and retention three days.
The selector validates the committed identity directory and returns gzip then
status for COMPLETE or status only for incomplete. No glob, scratch directory,
canonical TSV, primary CTest log, manifest, credential, or fallback path is
uploaded. `always()` schedules recovery without erasing the wrapper's nonzero
status.

The normal wrapper, selector, and upload conditions all exclude the existing
`codex/endpoint-writer-selftest-20260906` branch. Its synthetic self-test command
is unchanged.

## Frozen behavior

The earlier 57-test checkpoint, complete 60-test suite, five public-API build
commands, OpenFHE setup, paper executable build, and every other focused command
remain unchanged. No CMake or test source changes occur here, so CTest
`paper_full_eight_square_contract` remains no-argument Test 61 with
`TIMEOUT 1200`, `RUN_SERIAL TRUE`, and `OMP_NUM_THREADS=2`.

The historical live E80 failure may yield a validated COMPLETE gzip/status and
still leave the wrapper and job nonzero; that is truthful failure-preserving
transport, not a green numerical claim. Conversely, path, capture, finalizer,
selector, or upload failure cannot become success.

## Static verification

Bounded source-only checks before the local `[skip ci]` commit passed:

- Ruby parsed the YAML and confirmed the exact one-branch trigger delta, two
  unchanged jobs, endpoint step identities/conditions, pinned new actions, and
  selector-only upload paths;
- `bash -n` accepted every Bash/MSYS run body, and Python `compile()` accepted
  the embedded Windows path preflight without executing it;
- exact source comparison preserved the earlier 57/60/five-API/self-test
  command bodies and found no remaining adjacent unfiltered paper
  `--show-only` invocation;
- a representative `D:/a/_temp/fl` calculation using accepted 11-digit run
  34037828392 produced writer/status candidate lengths 140/240, with the latter
  within 247; the future job still checks its actual native runner path;
- `git diff --check` passed, CMake/tests/headers/production were unchanged, and
  the final scope contained only the two owned files.

These checks are source validation only. Hosted interpreter visibility, actual
runner-temp spelling and path lengths, wrapper/finalizer behavior, artifact
selection, and the live chain remain runtime-pending.
