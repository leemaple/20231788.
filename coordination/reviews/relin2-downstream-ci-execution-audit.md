# Relin2 downstream CI execution audit

Recorded: 2026-09-01 Asia/Shanghai

Verdict: **PASS after the evidence-ref, exact-count, and cancellation rules
below were made binding**. This record authorizes no patch
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
| 01 | Production library green; the separately built API-contract target is the intended compile red on the three absent public symbols. This is not a CTest count. | After Linux evidence and its raw log are captured, cancel the whole run immediately if Windows is not terminal; wait for terminal cancellation before advancing. |
| 02 | Warning-clean accepted suite exactly `6/6`; this is scaffold green, not Relin2 green. | Same mandatory intermediate-run cancellation rule. |
| 03 | Exactly six accepted tests pass and all 30 independently named Relin2 cases fail on their intended scaffold/dependency reds: 36 registered/executed total, never one failure standing for later unexecuted cases. | Same mandatory intermediate-run cancellation rule. |
| 04 | Exact complete core suite `36/36` green; the Tensor2 lifecycle guard is still absent, so this boundary is not mergeable. | Same mandatory intermediate-run cancellation rule. |
| 05 | Exactly 36 pass plus the one directed lifecycle failure: 37 registered/executed total. | Same mandatory intermediate-run cancellation rule. |
| 06 | Exact complete suite `37/37` green with the minimal lifecycle guard active. | Same mandatory intermediate-run cancellation rule. |
| 07 | Final complete Linux suite exactly `37/37` green. | Must not be cancelled; Linux and Windows in the same push run must both succeed with exact `37/37` at the same exact SHA. |

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
5. Download and hash the completed Linux job log. For patches 01-06, if the
   Windows job or run is still nonterminal, immediately cancel the whole run,
   wait for terminal cancellation, and retain the terminal API records before
   applying the next patch. A completed Linux job may be called Linux
   red/green; a cancelled workflow may not be called workflow green. Patch 07
   is the sole run allowed to continue through complete Windows execution.
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

## Evidence ref isolation

The implementation branch `agent/codex-relin2-01` must contain exactly the
seven accepted semantic patch commits over `fb862a3...`; its final verified
head is the patch-07 commit. Do not commit downloaded logs, run JSON, checksum
manifests, review receipts, or any other evidence file onto that branch before,
between, or after those commits.

Persist hosted raw records in a separate worktree and a non-triggering branch
named `evidence/relin2-hosted-<patch07-short-sha>` (or an equivalently isolated
immutable evidence ref). Every directory and manifest there must name the
implementation commit it proves. Push and remotely verify the evidence ref
without advancing or rewriting `agent/codex-relin2-01`. A commit containing
artifacts/evidence is still a commit: if placed on the implementation branch it
would change the SHA and invalidate the patch-07 Linux/Windows result just as a
source or documentation change would.

The final Fable5 bundle and review must bind the unchanged patch-07
implementation SHA plus the separately hashed evidence-ref files. Fable5
output and its receipt likewise stay off the implementation branch.

## Authority cross-check

This audit was derived from the current `.github/workflows/dcp-rcb.yml`, the
seven-boundary and hosted-execution contract in
`coordination/tasks/chatgpt-pro-relin2-01.md`, the replacement boundaries in
`coordination/tasks/chatgpt-pro-relin2-remediation-02.md`, and the downstream
gate in
`coordination/reviews/relin2-remediation-02-receipt-checklist.md`.
