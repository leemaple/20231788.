#!/usr/bin/env bash
# Candidate only. Codex must review the entire package before running this script.
# This script has NOT been executed in the adjudication turn.
set -euo pipefail
umask 077
: "${ECD_REVIEW_APPROVED:?Set ECD_REVIEW_APPROVED=REPRODUCTION-ADJUDICATION-01 only after separate Codex review}"
test "$ECD_REVIEW_APPROVED" = REPRODUCTION-ADJUDICATION-01
if [ "$#" != 1 ]; then echo 'usage: run_reviewed_once.sh /ABSOLUTE/NEW/OUTPUT_PARENT' >&2; exit 2; fi
ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd -P)"
OUT="$1"
case "$OUT" in /*) ;; *) echo 'absolute output directory required' >&2; exit 2;; esac
case "$OUT" in "$ROOT"|"$ROOT"/*) echo 'output must be outside immutable delivery' >&2; exit 2;; esac
test "${GITHUB_ACTIONS:-}" = true
test "${RUNNER_ENVIRONMENT:-}" = github-hosted
test "${RUNNER_OS:-}" = Linux
test "${GITHUB_RUN_ATTEMPT:-}" = 1
python3 -B -c 'import sys; assert sys.version_info[:2] == (3,12)' 
python3 -B "$ROOT/tools/verify_delivery.py"
test ! -e "$OUT" && test ! -L "$OUT"
mkdir -- "$OUT"
printf '%s\n' 'Reserved one reviewed action; do not delete/retry after failure.' > "$OUT/ONCE_RESERVED.txt"
printf 'run_id=%s\nrun_attempt=%s\nrunner_os=%s\n' "$GITHUB_RUN_ID" "$GITHUB_RUN_ATTEMPT" "$RUNNER_OS" > "$OUT/runner-identity.txt"
python3 --version > "$OUT/python-version.txt" 2>&1
python3 -B "$ROOT/tools/run_scalar_checks.py" --output "$OUT/scalars.json" > "$OUT/scalars.log" 2>&1
python3 -B "$ROOT/candidate/check_rounding.py" --controls --allow-reviewed-transform --output-dir "$OUT/controls" > "$OUT/controls.log" 2>&1
set +e
python3 -B "$ROOT/candidate/check_rounding.py" --certify --allow-reviewed-transform --output-dir "$OUT/certificate" > "$OUT/certificate.log" 2>&1
code=$?
set -e
printf '%s\n' "$code" > "$OUT/certificate.exit"
case "$code" in
  0|3|4) python3 -B "$ROOT/tools/verify_rounding_result.py" "$OUT/certificate" > "$OUT/intake.log" 2>&1 ;;
  *) echo 'Oracle/infrastructure invalid; STOP, no retry and no production verdict.' >&2; exit "$code";;
esac
python3 -B "$ROOT/tools/verify_delivery.py" > "$OUT/delivery-after.log" 2>&1
# Nonzero REFUTED/INCONCLUSIVE is preserved; it must not be hidden by intake PASS.
exit "$code"
