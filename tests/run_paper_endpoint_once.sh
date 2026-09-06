#!/usr/bin/env bash
# One existing paper CTest, then one independent finalization. No retry.
set -eu
set -o pipefail

fail() { echo "endpoint wrapper: $*" >&2; exit 1; }

[[ $# == 14 ]] || fail 'expected exactly seven option/value pairs'
# Fixed option order keeps this one-task interface explicit and nonextensible.
[[ $1 == --scope && $3 == --host && $5 == --build-dir-shell &&
   $7 == --scratch-shell && $9 == --scratch-native &&
   ${11} == --python && ${13} == --ctest ]] || fail 'unexpected option order'
scope=$2
host=$4
build_shell=$6
scratch_shell=$8
scratch_native=${10}
python_executable=${12}
ctest_executable=${14}

[[ $scope == live-single-chain || $scope == synthetic ]] || fail 'invalid scope'
[[ $host == linux || $host == windows ]] || fail 'invalid host'
[[ ${GITHUB_SHA-} =~ ^[0-9a-f]{40}$ ]] || fail 'invalid source identity'
[[ ${GITHUB_RUN_ID-} =~ ^[1-9][0-9]*$ ]] || fail 'invalid run identity'
[[ ${GITHUB_RUN_ATTEMPT-} =~ ^[1-9][0-9]*$ ]] || fail 'invalid run attempt'
[[ -x $python_executable && -x $ctest_executable ]] || fail 'missing executable'
[[ $build_shell == /* && -d $build_shell ]] || fail 'invalid build directory'
[[ $scratch_shell == /* && $scratch_shell != / && -d $scratch_shell &&
   ! -L $scratch_shell ]] || fail 'invalid scratch directory'
[[ $(cd -- "$scratch_shell" && pwd -P) == "$scratch_shell" ]] || fail 'scratch must be real normalized path'
if [[ $host == windows ]]; then
    [[ $(cygpath -am "$scratch_shell") == "$scratch_native" ]] || fail 'Windows path spellings disagree'
else
    [[ $scratch_native == "$scratch_shell" ]] || fail 'Linux path spellings disagree'
fi
shopt -s nullglob dotglob
scratch_entries=("$scratch_shell"/*)
[[ ${#scratch_entries[@]} == 0 ]] || fail 'scratch directory is not empty'
shopt -u nullglob dotglob
script_directory=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)
finalizer="$script_directory/paper_endpoint_finalizer.py"
[[ -f $finalizer && ! -L $finalizer ]] || fail 'missing real finalizer'
if [[ $host == windows ]]; then
    finalizer=$(cygpath -am "$finalizer")
fi
"$python_executable" -c 'import sys; sys.exit(0 if sys.version_info[:2] == (3, 12) else 1)'
command -v tee >/dev/null || fail 'missing capture executable'

umask 077
mkdir -- "$scratch_shell/canonical" "$scratch_shell/published"
export PAPER_ENDPOINT_HOST="$host"
export PAPER_ENDPOINT_RUN_ID="$GITHUB_RUN_ID"
export PAPER_ENDPOINT_RUN_ATTEMPT="$GITHUB_RUN_ATTEMPT"
export PAPER_ENDPOINT_CANONICAL_PARENT="$scratch_native/canonical"

set +e
"$ctest_executable" --test-dir "$build_shell" --verbose --output-on-failure \
    -R '^paper_full_eight_square_contract$' 2>&1 | tee -- "$scratch_shell/primary.ctest.log"
pipeline_status=("${PIPESTATUS[@]}")
ctest_status=${pipeline_status[0]}
capture_status=${pipeline_status[1]}
"$python_executable" -B "$finalizer" finalize \
    --primary-log "$scratch_native/primary.ctest.log" \
    --ctest-exit-code "$ctest_status" --capture-exit-code "$capture_status" \
    --scope "$scope" --source-commit "$GITHUB_SHA" --host "$host" \
    --github-run-id "$GITHUB_RUN_ID" --github-run-attempt "$GITHUB_RUN_ATTEMPT" \
    --canonical-parent "$scratch_native/canonical" \
    --published-parent "$scratch_native/published"
finalizer_status=$?
set -e
if (( ctest_status != 0 )); then exit "$ctest_status"; fi
if (( capture_status != 0 )); then exit "$capture_status"; fi
exit "$finalizer_status"
