# PUBLIC-S100-ECD-CELL-01 CI source review

## Verdict

The dedicated workflow is suitable for root review before creation of the one
approved tag.  Its source matches the execution boundary in
`TASK.md`, the immutable author's `pro/NEXT_ACTION.md`, and the independent
`CANDIDATE_REVIEW.md`.  This is a static CI-wiring verdict, not evidence that the
workflow has run or that the unknown numerical result will certify.

## Admission and isolation

The only trigger is creation of the exact tag
`public-s100-ecd-cell-once-20260909`.  The job-level condition and the first
Python event gate independently require a push event, matching full tag ref,
`created=true`, `deleted=false`, `forced=false`, and `run_attempt=1`.  Branch
push, tag update, wrong tag, deletion, forced update, manual event, payload/ref
disagreement, and rerun are rejected.  Concurrency cancellation is disabled.

The job is limited to `ubuntu-24.04`, Python 3.12, and 45 minutes.  It installs
nothing and has no OpenFHE checkout, compiler/build, cache, CTest, encoding,
sampling, encryption, or other FHE step.  Checkout credentials are not
persisted and job permissions are `contents: read`.  The numerical candidate is
reached only through the separately owned reviewed harness.

The static checker cannot authenticate a future GitHub payload or prove that a
hosted job was scheduled exactly once.  In particular, Git tag deletion and
later recreation is an external owner action that source conditions alone
cannot remember.  Root must retain the one-shot ledger discipline and create
the exact tag only after the final source review.  There is no dispatch,
scheduled retry, precision retry, or alternate output path in this workflow.

## Frozen remote sequence

The workflow creates one new owner-only mode-0700 evidence directory below
`RUNNER_TEMP`, uniquely named by run ID and attempt.  Every shell that writes
evidence applies `umask 077`.

Before numerical admission it:

1. runs the scalar-only event/source gate tests and repeats the event check from
   `GITHUB_EVENT_PATH` without dumping the payload;
2. binds checked-out `HEAD`, the tag commit, and `GITHUB_SHA`, and requires a
   clean worktree;
3. records the exact ref, event, run identity, runner OS/architecture, Python
   executable/version/implementation/platform, and SHA256 identities for the
   workflow, integration harness/tests, original Pro manifest, candidate/core/
   rounding sources, and original scalar/delivery/intake tools; and
4. runs `test_harness_contract.py`, which is the root-owned scalar-only
   canonical-path/manifest/outcome admission suite.

It then invokes exactly once:

```text
python -B -I coordination/public-s100-ecd-cell-20260909/run_once.py --reviewed --output-directory "$EVIDENCE_DIR/execution"
```

The outer `-I` protects the root CLI import boundary; the reviewed root harness
uses `-B -E -s` for original child scripts so the immutable candidate's sibling
`interval_core` import remains available without `PYTHONPATH` or user-site
injection.  The workflow does not directly invoke a candidate transform.

The root harness, rather than the YAML, owns the frozen scientific ordering:
manifest/payload validation, scalar checks, four analytic tiny controls, at
most one full 224-bit inverse, then original table intake and declared/saved/
observed exit agreement.  A failed scalar/control stage prevents the full
inverse.  Numerical exits 0/3/4 retain their distinct meaning; observer,
control, or infrastructure failures are invalid evidence rather than production
verdicts.

The harness process's merged stdout/stderr is retained in `harness.log`; the
harness and `tee` exits are captured immediately and separately in
`harness.exit` and `harness-tee.exit`.  A successful logger returns the original
harness exit to the job.  A failed logger preserves that numerical exit but
marks CI evidence invalid with infrastructure exit 2, so a lost log cannot look
GREEN.  A 40-minute harness step timeout leaves margin inside the
45-minute job for the final `always()` upload.  Timeout remains an
infrastructure-invalid outcome; hosted-runner termination can never be treated
as a numerical result.  The upload path is exactly the unique evidence
directory, not the checkout or runner temp root.

## TDD receipt

The event gate was developed test-first without importing any candidate.
Initial RED:

```text
python3 -B coordination/public-s100-ecd-cell-20260909/test_ci_gate.py
Ran 3 tests in 0.001s
FAILED (errors=1)
FileNotFoundError: .github/workflows/public-s100-ecd-cell.yml
```

At RED, the exact fresh-tag positive and all mutated-event negative tests passed;
only the intentionally absent workflow-source contract errored.  After adding
the minimal workflow, the latest source-only GREEN at this review point was:

```text
python3 -B coordination/public-s100-ecd-cell-20260909/test_ci_gate.py
Ran 3 tests in 0.002s
OK

python3 -B coordination/public-s100-ecd-cell-20260909/check_ci_gate.py source
{"crypto_calls": 0, "full_transforms": 0, "status": "PASS_SOURCE_ONLY"}
```

These commands exercised only Python event/source logic.  They did not run the
root harness contract suite, import the numerical candidate, execute tiny or
full transforms, compile OpenFHE, encode, sample, encrypt, dispatch CI, create a
tag, or mutate external state.

## Interpretation limits

The recorded SHA256 for the immutable original Pro root manifest is
`8ab0982505dcdbe314a5ddf33aec0d46ae2ab440f6a31c535133c91659e2a2fb`.
The harness checks that pinned identity and all manifest payload bytes before
trusting the returned verifier; the workflow additionally records the exact
bytes used.  Hashes and labels establish byte identity, not mathematical truth
or historical pairing.

An eventual `ECD_ROUNDING_CERTIFIED`/0 applies only to the retained public
polynomial, exact original dyadic input, and reviewed interval argument.  A
REFUTED/3 result retains a finite witness; INCONCLUSIVE/4 exhausts the fixed
budget; invalid/timeout evidence proves neither.  No outcome changes the
original S100 E80 FAIL or proves a ciphertext repair, Table 3 reproduction,
historical identity, or security.  The review model identity remains
**requested-unverified** as recorded by the independent review.
