# Hosted precision116 execution

## RED dispatched once

- Source: `70c37679f4760c6dc2bebc35bdfd741c238f315b`.
- Ref: `codex/precision116-profile-seam-red-20260907` (also pushed to the safe working branch).
- Run: https://github.com/leemaple/20231788./actions/runs/34050734415
- Created: `2026-09-06T18:07:48Z`.
- Initial observed state: IN_PROGRESS, not RED accepted. Windows job `101533759804` was installing its official toolchain; Linux job `101533759903` was building pristine OpenFHE. No project compiler diagnostic was available yet.
- No dispatch, rerun, cancellation, or second activation-ref push. Production GREEN is not applied.

## Subsequent GREEN orchestration headroom

The unchanged new CTest timeout is 1200 seconds. Its outer workflow step previously also expired at 20 minutes, leaving no shutdown/reporting headroom. The working branch now increases only that GREEN-only step to 25 minutes on both hosts; the already running RED source and all cryptographic assertions remain unchanged.

Test-first evidence: changed the bounded CI checker expectation to 25, then ran it against the old workflow: 5 tests, 4 PASS / 1 FAIL, 0.019 seconds, with exact assertion `20 != 25`, exit 1. Changed only the two outer timeout values and reran: 5/5 PASS (exact duration retained in the command output). `git diff --check` passed. These are workflow source checks, not a cryptographic RED/GREEN result.

Current scientific claims remain original E80 FAIL, experimental operation NOT RUN, full-eight experimental E80 NOT TESTED, and security UNRESOLVED. Inspect the exact run's actual compiler diagnostics before accepting RED or applying GREEN.
