# FS endpoint wrapper and finalizer integration contract

Status: design only, authored 2026-09-06. No wrapper, finalizer, C++ live
wiring, workflow change, build, CTest run, or upload is implemented or claimed by
this document.

This contract is deliberately one-chain-specific. It connects the existing
`paper_full_eight_square_contract` CTest to the independent endpoint readers,
replay, status, gzip, reconciliation, and publication seams. It is not a
general-purpose test runner.

## 1. Frozen integration boundary

The integration must preserve all of these properties:

- The existing CTest name, command, executable argv, `TIMEOUT 1200`,
  `RUN_SERIAL TRUE`, and `OMP_NUM_THREADS=2` remain unchanged.
- The normal paper CTest is invoked exactly once per authorized host. Remove the
  adjacent `ctest --show-only=json-v1` from this one workflow step; it is not
  needed by the wrapper and would violate the deliberately literal one-CTest
  process boundary. No recovery rerun is permitted.
- Existing earlier builds, the 60-test checkpoint, and the synthetic observer
  self-test branch remain separate and unchanged. The wrapper replaces only the
  current normal `Run paper full eight-square contract once` command body.
- A nonzero CTest status remains the job's status after finalization. A zero
  CTest status does not hide a capture, validation, packaging, selection, or
  upload failure.
- The finalization operation is attempted exactly once after the CTest pipeline,
  regardless of its status. A later `select` subcommand is read-only upload
  selection and is not another finalization.
- The wrapper must not use `|| true`, `continue-on-error`, an unconditional zero
  exit, or a success-forcing pipe.
- Process death of the runner or wrapper can prevent finalization. This design
  does not claim crash-proof publication or the ability to run cleanup after
  `SIGKILL` or host loss.

The invocation owns a fresh, exclusive scratch tree with this exact layout:

```text
scratch/
  primary.ctest.log
  canonical/
    <stem>/
      <stem>.tsv
  published/
    <stem>/
      <stem>.status.json
      <stem>.tsv.gz        # COMPLETE only
```

`<stem>` is exactly:

```text
fs-residual-endpoint-01.v1-r1.<GITHUB_SHA>.<host>.<GITHUB_RUN_ID>.<GITHUB_RUN_ATTEMPT>
```

The C++ writer receives only `scratch/canonical`. The publisher receives only
the distinct `scratch/published`. Neither component may be pointed at `scratch`
itself or at the other component's parent. The canonical TSV remains private and
must never be included in an upload path.

## 2. C++ live call

The live no-argument path in `paper_full_eight_square_contract_test.cpp` must
construct the writer inputs only after all eight paper-owner rows are confirmed
absent, both unrelated rows are confirmed unchanged, and the existing
`OBS numeric_gate_failures=<n>` value has been emitted.

The exact call order is:

```cpp
const paper_endpoint_contract::EndpointEvidenceIdentity identity{
    "live-single-chain",
    PAPER_SOURCE_COMMIT,
    RequiredEndpointEnvironment("PAPER_ENDPOINT_HOST"),
    RequiredEndpointEnvironment("PAPER_ENDPOINT_RUN_ID"),
    RequiredEndpointEnvironment("PAPER_ENDPOINT_RUN_ATTEMPT"),
    BOOST_VERSION,
};
const paper_endpoint_contract::EndpointPublicationBoundary boundary{
    numericFailures,
    true,
};
const auto endpointFile = paper_endpoint_contract::WriteEndpointEvidence(
    paper.endpoint,
    identity,
    boundary,
    std::filesystem::path(
        RequiredEndpointEnvironment("PAPER_ENDPOINT_CANONICAL_PARENT")));
paper_endpoint_contract::EmitEndpointEvidencePrimary(
    std::cout, paper.endpoint, identity, boundary);
std::cout << std::flush;
```

`endpointFile` must not be used to invent an identity or receipt; it only proves
that this call returned after the writer's close/reopen validation. A compiler
warning for an intentionally unused return must be avoided by either checking
its returned path against the writer-derived expected path or omitting the local
variable.

The existing line below remains in the same position after those two calls and
is not weakened:

```cpp
Require(numericFailures == 0,
        "accumulated numeric acceptance failures: " +
            std::to_string(numericFailures));
```

The existing legacy `COMPLETE ... PASS` follows only if that Require passes.
Consequently, a finite E80 failure may still have a closed canonical sidecar and
complete endpoint-primary records before the unchanged Require produces the
legacy `COMPLETE ... FAIL`; fatal observer/writer errors still fail before an
endpoint-primary COMPLETE record.

`RequiredEndpointEnvironment` is a narrow test-local reader. It must reject a
missing or empty value and must not supply defaults. The writer remains the
authority for identity/path grammar. These four values are required only in the
normal `argc == 1` live path. The existing self-test dispatch must stay before
the reads, so `--endpoint-observer-self-test` neither needs live variables nor
creates a live filename. No credential, repository, key, ciphertext, OpenFHE
owner, or environment-derived source SHA is passed to the writer.

`PAPER_SOURCE_COMMIT` remains the configure-time compile definition from CMake,
and the finalizer independently expects `GITHUB_SHA`. A mismatch must fail
identity validation; the environment may not override the compiled source.
`BOOST_VERSION` is the actual compiled Boost value.

No CMake change is required for this call: the writer source is already in the
existing executable and the CTest properties already carry the required test
timeout, serialization, and OpenMP setting.

## 3. Host path and environment spelling

Both jobs must first provision an explicit Python 3.12 runtime, report
`python --version`, and fail if it is not Python 3.12. No dependency installation
is performed by the wrapper itself.

The proposed workflow prerequisite in each job is concrete:

```yaml
- name: Set up endpoint Python
  id: endpoint-python
  uses: actions/setup-python@v5
  with:
    python-version: '3.12'

- name: Report endpoint Python
  # Linux uses bash; Windows uses the existing msys2 {0} shell.
  run: |
    python --version
    python -c 'import sys; assert sys.version_info[:2] == (3, 12)'
```

The Windows visibility of this action-provided `python` inside `msys2 {0}` is a
hosted-check item in section 8, not an already observed fact.

### Linux

The workflow step runs from the checkout root and supplies:

```bash
host=linux
build_shell="$PWD/build"
scratch_shell="$RUNNER_TEMP/fs-endpoint-live-${GITHUB_RUN_ID}-${GITHUB_RUN_ATTEMPT}-linux"
test ! -e "$scratch_shell" && test ! -L "$scratch_shell"
umask 077
mkdir -- "$scratch_shell"
scratch_shell="$(cd "$scratch_shell" && pwd -P)"
scratch_native="$scratch_shell"

export PAPER_ENDPOINT_HOST="$host"
export PAPER_ENDPOINT_RUN_ID="$GITHUB_RUN_ID"
export PAPER_ENDPOINT_RUN_ATTEMPT="$GITHUB_RUN_ATTEMPT"
export PAPER_ENDPOINT_CANONICAL_PARENT="$scratch_native/canonical"
```

The wrapper creates the `canonical` and `published` children after claiming the
fresh scratch root. Linux shell and native paths are identical absolute paths.

### Windows MSYS2/MINGW64

The workflow step remains `shell: msys2 {0}` with working directory
`C:/openfhe-2023-1788/cleanroom`. It supplies:

```bash
host=windows
prefix="$(cygpath -u "$OPENFHE_PREFIX")"
build_shell="$(cygpath -u "$PROJECT_BUILD")"
runner_temp_shell="$(cygpath -u "$RUNNER_TEMP")"
scratch_shell="$runner_temp_shell/fs-endpoint-live-${GITHUB_RUN_ID}-${GITHUB_RUN_ATTEMPT}-windows"
test ! -e "$scratch_shell" && test ! -L "$scratch_shell"
umask 077
mkdir -- "$scratch_shell"
scratch_shell="$(cd "$scratch_shell" && pwd -P)"
scratch_native="$(cygpath -am "$scratch_shell")"
export PATH="$prefix/bin:$prefix/lib:$PATH"

export PAPER_ENDPOINT_HOST="$host"
export PAPER_ENDPOINT_RUN_ID="$GITHUB_RUN_ID"
export PAPER_ENDPOINT_RUN_ATTEMPT="$GITHUB_RUN_ATTEMPT"
export PAPER_ENDPOINT_CANONICAL_PARENT="$scratch_native/canonical"
```

MSYS tools (`ctest`, Bash, and `tee`) use `build_shell`, `scratch_shell`, and
`$scratch_shell/primary.ctest.log`. The native MINGW C++ process and the native
Python 3.12 process receive the mixed native spelling returned by
`cygpath -am`, for example `C:/.../canonical`. Never pass `/c/...` through
`PAPER_ENDPOINT_CANONICAL_PARENT`; native `std::filesystem` is not required to
interpret an MSYS virtual path.

The wrapper must require
`$(cygpath -am "$scratch_shell") == "$scratch_native"`. On Linux it must require
`$scratch_shell == $scratch_native`. These checks prevent the two spellings from
silently naming different scratch trees; they do not claim protection from a
hostile concurrent parent mutation.

## 4. One-shot Bash wrapper

The future narrow script is `tests/run_paper_endpoint_once.sh`. Its production
CLI is:

```text
tests/run_paper_endpoint_once.sh \
  --scope live-single-chain|synthetic \
  --host linux|windows \
  --build-dir-shell ABS_SHELL_PATH \
  --scratch-shell ABS_SHELL_PATH \
  --scratch-native ABS_NATIVE_PATH \
  --python PYTHON_EXECUTABLE \
  --ctest CTEST_EXECUTABLE
```

It requires the scratch root to exist, be the newly claimed empty real directory
for this invocation, and requires the expected GitHub identity variables. It
creates exactly `canonical` and `published`, then runs this core once:

```bash
primary_log_shell="$scratch_shell/primary.ctest.log"
primary_log_native="$scratch_native/primary.ctest.log"
canonical_native="$scratch_native/canonical"
published_native="$scratch_native/published"

mkdir -- "$scratch_shell/canonical" "$scratch_shell/published"

set +e
set -o pipefail
"$ctest_executable" \
  --test-dir "$build_dir_shell" \
  --verbose \
  --output-on-failure \
  -R '^paper_full_eight_square_contract$' \
  2>&1 | tee -- "$primary_log_shell"
pipeline_status=("${PIPESTATUS[@]}")
ctest_status="${pipeline_status[0]}"
capture_status="${pipeline_status[1]}"

"$python_executable" -B tests/paper_endpoint_finalizer.py finalize \
  --primary-log "$primary_log_native" \
  --ctest-exit-code "$ctest_status" \
  --capture-exit-code "$capture_status" \
  --scope "$scope" \
  --source-commit "$GITHUB_SHA" \
  --host "$host" \
  --github-run-id "$GITHUB_RUN_ID" \
  --github-run-attempt "$GITHUB_RUN_ATTEMPT" \
  --canonical-parent "$canonical_native" \
  --published-parent "$published_native"
finalizer_status=$?
set -e

if (( ctest_status != 0 )); then
  exit "$ctest_status"
fi
if (( capture_status != 0 )); then
  exit "$capture_status"
fi
exit "$finalizer_status"
```

`PIPESTATUS` must be copied immediately after the pipeline, before any test,
assignment command substitution, logging, or finalizer call. Bash status is
already in 0..255. If the `ctest` process itself is terminated by a signal, Bash
supplies `128 + signal`; the wrapper passes and returns that value without
Boolean conversion. A signal delivered only to the test child may instead be
translated by CTest to CTest's own nonzero status; the wrapper records the
actual enclosing CTest status and does not reconstruct the child's status.

The exit precedence is CTest, then capture, then finalizer. Thus an earlier
nonzero CTest result is never replaced by a later packaging error, while a
capture/finalizer failure still makes an otherwise-zero run fail. The finalizer
must also retain `ctest_exit_code` in status. The wrapper makes exactly one
`finalize` call even if CTest could not be launched and returned 126/127.

Fresh-scratch creation, path normalization, and Python/tool availability are
preconditions checked before starting the one authorized chain. If those
preconditions fail, the workflow stops before CTest; it must not start a chain
that cannot be captured/finalized. Once CTest starts, ordinary nonzero exits use
the explicit sequence above. Host death, Bash death, or inability to execute
the already-checked Python binary remains outside the recoverable guarantee and
must not be described as a committed incomplete status.

The production workflow must pass `--scope live-single-chain`. The
`--scope synthetic` value exists only for the disposable subprocess harness;
the wrapper must reject any other spelling. The production host invocations are
exactly:

```bash
bash tests/run_paper_endpoint_once.sh \
  --scope live-single-chain \
  --host "$host" \
  --build-dir-shell "$build_shell" \
  --scratch-shell "$scratch_shell" \
  --scratch-native "$scratch_native" \
  --python "$(command -v python)" \
  --ctest "$(command -v ctest)"
```

A nonzero `tee` status makes the transcript incomplete even if its captured
prefix happens to contain syntactically complete records. Finalization must not
publish complete evidence in that case. If the prefix already contains an
earlier typed `FS_ENDPOINT_FAILURE`, that observed earlier reason is retained;
otherwise the incomplete reason is `IO_ERROR`. Reliable complete E80/count/Boost
facts present in the prefix may be retained as the status schema permits, but
missing bytes are never inferred.

## 5. Finalizer CLI and state machine

`tests/paper_endpoint_finalizer.py` has exactly two subcommands:

```text
finalize --primary-log PATH --ctest-exit-code 0..255
         --capture-exit-code 0..255
         --scope live-single-chain|synthetic
         --source-commit HEX40 --host linux|windows
         --github-run-id POSITIVE_DECIMAL
         --github-run-attempt POSITIVE_DECIMAL
         --canonical-parent ABS_NATIVE_PATH
         --published-parent ABS_NATIVE_PATH

select   --source-commit HEX40 --host linux|windows
         --github-run-id POSITIVE_DECIMAL
         --github-run-attempt POSITIVE_DECIMAL
         --published-parent ABS_NATIVE_PATH
         --manifest ABS_NATIVE_PATH
```

`finalize` imports only the independent test-local diagnostic modules:

- `paper_endpoint_primary_reader`
- `paper_endpoint_sidecar_reader`
- `paper_endpoint_sidecar_replay`
- `paper_endpoint_reconcile`
- `paper_endpoint_gzip`
- `paper_endpoint_status`
- `paper_endpoint_publication`

It must not import OpenFHE, the C++ writer, production project code, a shell
runner, or a crypto helper. It parses only CTest-prefixed primary records through
`parse_primary_log`; unprefixed CTest failure replay is not accepted as primary
evidence.

The finalization order is:

1. Validate all scalar arguments and both distinct, absolute, normalized,
   non-symlink parents before derived path concatenation. Read
   `primary.ctest.log` as bounded immutable bytes.
2. Determine the CTest timeout observation from the captured CTest-owned
   transcript and pass the resulting Boolean to `parse_primary_log` together
   with the exact shell status and expected identity/scope. Exit status alone is
   not sufficient to distinguish `TIMEOUT` from `CTEST_FATAL`.
3. If capture failed, prohibit COMPLETE. Preserve a typed primary failure that
   is visibly earlier in the retained prefix; otherwise construct truthful
   incomplete `IO_ERROR` facts from the parser's reliable partial context.
4. If the primary result is incomplete, construct the exact incomplete status
   from that result and call `publish_endpoint_evidence` with both payloads
   `None`. Return a nonzero code. Do not inspect or package a stale canonical
   file.
5. For a complete primary result, derive the exact canonical path as
   `canonical_parent / stem / (stem + ".tsv")`. Reject any missing, extra,
   symlinked, foreign, or non-regular candidate. Read bounded bytes, call
   `read_sidecar` with the same expected identity, then reread bounded bytes and
   require byte equality before packaging. This close/reopen equality binds the
   bytes that are hashed and gzipped to the validated path within the explicit
   exclusive-scratch contract; it is not a hostile-race guarantee.
6. Call `validate_primary_binding(primary, sidecar)`, then
   `replay_sidecar(sidecar)`, then `reconcile_replay(primary, sidecar, replay)`.
   All must succeed before compression.
7. Call `encode_gzip(canonical_bytes)` and immediately `verify_gzip` against
   actual SHA-256 hashes and byte counts computed from the actual canonical and
   gzip bytes. Construct the COMPLETE status from the parsed/replayed facts and
   those actual receipts; never trust a predeclared status hash or filename.
8. Call `publish_endpoint_evidence` exactly once with the status and the same
   canonical/gzip byte objects. Propagate its `required_exit_code`. If it raises
   `PublicationError`, propagate its nonzero `required_exit_code` even when it
   committed a truthful fallback status.

Complete E80 PASS requires count zero and CTest status zero. Complete E80 FAIL
requires a positive observed count and the actual nonzero CTest status. The
finalizer may successfully validate and publish the latter, but its required
exit remains the original CTest status. Any other CTest/count/legacy COMPLETE
contradiction is incomplete `INTEGRITY`; it must not be normalized.

The finalizer catches only documented boundary errors in order to publish a
truthful incomplete status or return their required failure code. Unexpected
exceptions retain a traceback and nonzero exit; they are not converted into
success or an invented reason. A failure to write/validate incomplete status is
itself a nonzero finalizer failure and produces no empty-success selection.

### Required typed-error prerequisite

The current primary reader's `PrimaryLogError.reason` and reconciliation
`ReconcileError.reason` are usable typed first-cause seams. The current
`SidecarError` and `ReplayUnresolved`/`ReplayError` do not expose enough typed
reason data for the finalizer to distinguish, without parsing human exception
text, at least:

- sidecar `FORMAT` vs `IDENTITY` vs `INTEGRITY`;
- replay `CONDITIONING` vs `ESTIMATOR_CEILING` vs `REPLAY`.

Before implementing the finalizer, those independent readers must expose a
stable `.reason` token (or equivalent typed subclasses) from the adopted reason
set. The finalizer must not classify by substring matching `str(error)`. This is
a real interface prerequisite, not authorization to relax any numerical test.

Similarly, exact `TIMEOUT` classification requires an observed, stable CTest
transcript marker on both hosted shells. Until that format is established,
nonzero incomplete execution can truthfully be classified as `CTEST_FATAL`, but
must not be guessed to be `TIMEOUT`.

## 6. Exact upload selection after a failed wrapper step

The wrapper step has an `id`, has no `continue-on-error`, and may finish
nonzero. A distinct selector step uses `if: always()` and the deterministic
scratch path, so it still runs after wrapper failure. Its only operation is the
read-only `select` subcommand, which calls `select_endpoint_uploads`, writes the
returned allowlist to a new manifest outside `published/<stem>`, and fails on
missing/invalid status, orphan gzip, extra paths, or any identity/hash mismatch.

The manifest contains exactly one native absolute path per LF-terminated line,
in selector order: gzip then status for COMPLETE, status only for incomplete.
It must reject CR, blank lines, duplicates, relative paths, and any path outside
the expected identity directory. It is not itself uploaded.

Linux selection step:

```bash
scratch_shell="$RUNNER_TEMP/fs-endpoint-live-${GITHUB_RUN_ID}-${GITHUB_RUN_ATTEMPT}-linux"
scratch_native="$(cd "$scratch_shell" && pwd -P)"
manifest_native="$scratch_native/upload-paths.txt"
python -B tests/paper_endpoint_finalizer.py select \
  --source-commit "$GITHUB_SHA" \
  --host linux \
  --github-run-id "$GITHUB_RUN_ID" \
  --github-run-attempt "$GITHUB_RUN_ATTEMPT" \
  --published-parent "$scratch_native/published" \
  --manifest "$manifest_native"
{
  echo 'paths<<FS_ENDPOINT_PATHS'
  cat -- "$manifest_native"
  echo 'FS_ENDPOINT_PATHS'
} >> "$GITHUB_OUTPUT"
```

Windows MSYS2 selection step:

```bash
runner_temp_shell="$(cygpath -u "$RUNNER_TEMP")"
scratch_shell="$runner_temp_shell/fs-endpoint-live-${GITHUB_RUN_ID}-${GITHUB_RUN_ATTEMPT}-windows"
scratch_shell="$(cd "$scratch_shell" && pwd -P)"
scratch_native="$(cygpath -am "$scratch_shell")"
manifest_native="$scratch_native/upload-paths.txt"
manifest_shell="$scratch_shell/upload-paths.txt"
github_output_shell="$(cygpath -u "$GITHUB_OUTPUT")"
python -B tests/paper_endpoint_finalizer.py select \
  --source-commit "$GITHUB_SHA" \
  --host windows \
  --github-run-id "$GITHUB_RUN_ID" \
  --github-run-attempt "$GITHUB_RUN_ATTEMPT" \
  --published-parent "$scratch_native/published" \
  --manifest "$manifest_native"
{
  echo 'paths<<FS_ENDPOINT_PATHS'
  cat -- "$manifest_shell"
  echo 'FS_ENDPOINT_PATHS'
} >> "$github_output_shell"
```

The workflow shape is:

```yaml
- name: Run and finalize paper endpoint once
  id: endpoint-run
  if: github.ref != 'refs/heads/codex/endpoint-writer-selftest-20260906'
  # host-specific Bash/MSYS body invokes tests/run_paper_endpoint_once.sh

- name: Select exact endpoint upload
  id: endpoint-select
  if: ${{ always() && github.ref != 'refs/heads/codex/endpoint-writer-selftest-20260906' }}
  # host-specific Bash/MSYS body above

- name: Upload exact endpoint evidence
  if: ${{ always() && steps.endpoint-select.outcome == 'success' && github.ref != 'refs/heads/codex/endpoint-writer-selftest-20260906' }}
  uses: actions/upload-artifact@v4
  with:
    name: fs-residual-endpoint-${{ runner.os }}-${{ github.run_id }}-${{ github.run_attempt }}
    path: ${{ steps.endpoint-select.outputs.paths }}
    if-no-files-found: error
    compression-level: 0
```

`always()` only controls later-step scheduling; it does not erase the earlier
wrapper failure. If selection succeeds after finite E80 failure, the evidence is
uploaded and the job remains nonzero. If selection fails, upload is skipped and
the selector failure remains visible. There is no wildcard, directory upload,
recursive search, temporary TSV, synthetic path, or fallback to “whatever
exists.”

## 7. Tiny synthetic subprocess harness

The wrapper receives `--ctest` and `--python` as executable boundary arguments
so a stdlib-only subprocess test can exercise the actual shell control flow
without CTest, OpenFHE, crypto, or 16,384-row replay. Tests use only a disposable
`fs-endpoint-synthetic-*` directory and `scope=synthetic`; they never use a live
artifact directory or upload action.

The fake CTest executable validates the fixed argv, increments a counter file,
prints a minimal synthetic primary stream, and exits with a requested shell
status. The fake Python executable validates that it was called once with
`paper_endpoint_finalizer.py finalize` and the exact two pipeline statuses,
increments a second counter, optionally writes a schema-valid synthetic status,
and exits with a requested finalizer status. A fake `tee` is injected only by a
temporary leading `PATH` entry to exercise an OS-command capture failure; no
internal Python helper is mocked.

Required vectors are:

| Fake CTest | Capture | Finalizer | Required observation |
|---:|---:|---:|---|
| 0 | 0 | 0 | wrapper 0; CTest count 1; finalize count 1 |
| 8 | 0 | 0 or 8 | wrapper 8; finalize still count 1 |
| 137 | 0 | 0 or 137 | wrapper 137 unchanged |
| 0 | nonzero | nonzero | incomplete `IO_ERROR`; wrapper uses capture status |
| 8 | nonzero | nonzero | wrapper 8; capture failure is not hidden |
| 8 | 0 | 1 | wrapper 8, not the later 1 |
| 0 | 0 | 1 | wrapper 1 |

Additional finalizer/selector fixtures use small disposable files:

- malformed or truncated primary with status zero cannot become COMPLETE;
- complete-looking captured primary plus nonzero capture status becomes
  incomplete and has no gzip;
- missing canonical after otherwise complete primary is `NO_CANONICAL`, unless
  an earlier concrete cause already exists;
- a complete finite E80-failing primary retains its count/disposition and CTest
  status rather than becoming success;
- after a wrapper failure, status-only selection returns exactly the status
  path; missing/invalid status, orphan gzip, extra temp file, foreign identity,
  or any symlink fails selection.

The harness proves process-count and status-precedence wiring only. It is not a
paper chain, hosted CTest result, gzip/replay proof, or workflow upload result.

## 8. Hosted facts still requiring observation

The following are availability/behavior questions for one bounded hosted
integration check; this document does not assert their answers:

1. `actions/setup-python@v5` (or the then-adopted pinned action) makes the selected
   Python 3.12 executable visible as `python` inside both Ubuntu Bash and the
   existing Windows `msys2 {0}` shell, while the Windows process correctly opens
   the `C:/...` native paths supplied above.
2. The exact CTest timeout line and line endings on Ubuntu 24.04 and Windows
   2022/MSYS2 are stable enough for a strict timeout classifier. This must be
   established with a tiny synthetic timeout, not another paper chain.
3. The captured CTest paper stream retains the expected `61: ` prefix on both
   hosts and Windows line endings are handled exactly by the current primary
   reader. The real test number must not be inferred from the source inventory
   if hosted discovery differs.
4. `actions/upload-artifact@v4` accepts the selector's multiline absolute native
   path output on both hosts, including a one-line status-only list after a
   failed wrapper step, while excluding the manifest and canonical TSV.
5. An `if: always()` selector and conditional upload actually execute after the
   preceding nonzero wrapper step in both jobs without changing the job's final
   failure disposition.

These checks may use disposable synthetic subprocess/status fixtures. They do
not authorize a new CTest entry, altered paper timeout, extra encrypted chain,
rerun, or broad artifact upload.
