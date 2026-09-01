# Relin2 downstream CI execution audit

Recorded: 2026-09-01 Asia/Shanghai

Verdict: **PASS**. Independent read-only audit found no P0, P1, or P2 issue
in the planned hosted execution sequence. This record authorizes no patch
application, push, workflow cancellation, build, test, or Fable5 invocation by
itself. The revised ChatGPT Pro delivery must first pass the separate receipt
and static gates.

## Workflow observations

- The current workflow has no `concurrency` or `cancel-in-progress` policy, so
  a later push will not automatically cancel an earlier run.
- The exact base intentionally does not yet list
  `agent/codex-relin2-01` under the push trigger. Patch 01 must add that branch;
  the first push then uses the workflow from that pushed commit, so there is no
  trigger bootstrap deadlock.
- Cancelling through the available Actions control cancels the whole workflow
  run, not only one job. Therefore a completed Linux job and its raw log must
  be captured before cancelling an unfinished Windows job.

## Required boundary outcomes

| Patch | Required hosted Linux evidence | Windows disposition |
| --- | --- | --- |
| 01 | Compile red: production library builds; the new API contract target fails on the three absent public symbols. | May be cancelled only after Linux evidence is complete. |
| 02 | Warning-clean green with the unchanged accepted 6/6 suite; this is scaffold green, not Relin2 green. | May be cancelled after Linux evidence is complete. |
| 03 | Build green and CTest red: registered Relin2 runtime/dependency reds remain attributable to the scaffold while the accepted 6/6 suite remains green. | May be cancelled after Linux evidence is complete. |
| 04 | The registered core suite is green; the Tensor2 lifecycle guard is still absent, so this boundary is not mergeable. | May be cancelled after Linux evidence is complete. |
| 05 | Build green with exactly the directed lifecycle runtime red and every other case green. | May be cancelled after Linux evidence is complete. |
| 06 | Complete Linux suite green with the minimal lifecycle guard active. | May be cancelled after Linux evidence is complete. |
| 07 | Final complete Linux suite green. | Must not be cancelled; Linux and Windows in the same push run must both succeed at the same exact SHA. |

## Serialized execution rule

For each accepted patch, in order:

1. Apply only that patch. Verify its staged diff, expected cumulative tree,
   parent, and delivery identity.
2. Create one semantic commit and fast-forward push only that commit. Do not
   create the next boundary first; never batch several new commits in one push,
   amend a pushed boundary, or force-push.
3. Verify the remote branch object ID equals the local commit. Resolve the
   Actions run by exact `head_sha`, `event=push`, and workflow path; never infer
   identity from the newest run.
4. Wait for `linux-gcc` to reach a terminal state and compare the actual build,
   named tests, counts, and failure shape with the table above. Infrastructure
   failures and unexpected semantic failures are not intended TDD reds.
5. Download and hash the completed Linux job log before deciding whether to
   cancel the remaining run. After any cancellation, wait for the run terminal
   state and retain the terminal API records. A completed Linux job may be
   called Linux red/green; a cancelled workflow may not be called workflow
   green.
6. Close the full evidence set for that SHA before applying the next patch.
7. For patch 07, do not cancel, modify, or amend. Require both jobs in the same
   push run to be `success` and bound to the final SHA. The Windows workflow's
   own explicit fetch/checkout comparison with `GITHUB_SHA` is part of that
   evidence.

## Evidence required per exact SHA

Retain sizes and SHA-256 values for:

- local/remote commit, ref, tree, parent, push receipt, and workflow source
  blob;
- raw run JSON, all-jobs JSON, check-runs JSON, run URL/ID/attempt/event/branch,
  and creation/completion timestamps;
- the complete Linux job log; for the final SHA, the complete Windows job log;
- the terminal complete-logs ZIP;
- raw artifacts-list JSON and every actual artifact. The current workflow has
  no `upload-artifact`, so a zero-result response must be retained as zero and
  not replaced by an invented artifact claim;
- for cancelled intermediate Windows work, terminal job/step state and any
  available partial log, explicitly labelled cancelled or pending;
- runner, pristine OpenFHE SHA, compiler/CMake versions, exact commands, exit
  codes, test names/counts, the boundary decision, and a checksum manifest for
  all retained raw records.

Any source or documentation change after patch 07 creates a new SHA and
invalidates the final same-commit dual-platform gate. The one authorized
terminal Fable5 substitution may bind only the first exact patch-07-or-later
commit that has passed this complete Linux/Windows gate.

## Authority cross-check

This audit was derived from the current `.github/workflows/dcp-rcb.yml`, the
seven-boundary and hosted-execution contract in
`coordination/tasks/chatgpt-pro-relin2-01.md`, the replacement boundaries in
`coordination/tasks/chatgpt-pro-relin2-remediation-02.md`, and the downstream
gate in
`coordination/reviews/relin2-remediation-02-receipt-checklist.md`.
