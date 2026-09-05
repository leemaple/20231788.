# First hosted parameter discovery dispatch

Source: `599fc158b6b67d3e752c39cb53069c29dc60fc6f`, branch
`codex/paper-table3-profile-probe-01`, exact public remote head confirmed by
`git ls-remote` after successful push. Local tree was clean.

Pre-push Gitleaks 8.30.1 command:
`gitleaks git . --log-opts='cfe2fbc..HEAD' --no-banner --redact`.
One commit, approximately 19,725 bytes scanned, zero findings. This included
the exact stable probe and its build/coordination changes; a clean scan is
evidence, not a guarantee.

Submitted **once** with
`gh workflow run 346498968 --repo leemaple/20231788. --ref codex/paper-table3-profile-probe-01`.
Returned URL: https://github.com/leemaple/20231788./actions/runs/33935796427.
The immediately following branch run listing was still empty, but the direct
run API confirmed the returned ID. No duplicate dispatch/retry occurred.

Run creation: 2026-09-05 01:19:12 UTC / 09:19:12 Asia/Shanghai.
Event: workflow_dispatch. Attempt: 1. Exact run head SHA matches source above.
Snapshot 09:19:26 Asia/Shanghai:

| Job | ID | Observed status |
| --- | --- | --- |
| linux-gcc | 101223371509 | in_progress, dependency installation |
| windows-mingw64 | 101223371775 | in_progress, exact-source checkout |

Both new probe steps were pending; **no new compile/test/probe success is
claimed**. Only the previously verified code baseline has dual-platform 57/57
evidence at this snapshot. Save exact job logs and reconcile all 18 family
records, collision control, old 57/57, warning-clean build and five API targets
when this run finishes. A parameter-probe pass is not production GREEN or
completion of the full paper implementation.
