# Hosted finalizer negative gate boundary

## Purpose

This slice adds exactly three hosted-only, public `paper_endpoint_finalizer.py
finalize` CLI regression cases.  They start from the frozen, parser-valid
synthetic complete fixture in `hosted_paper_endpoint_finalizer_complete.py` and
change only frozen input-evidence bytes (canonical TSV or primary log). They exercise three independently required
stages of the already implemented complete finalizer path:

1. primary-to-sidecar binding;
2. actual scalar replay conditioning;
3. primary signed-metric reconciliation.

This is discriminating regression coverage of implemented behavior.  It is not
a new-behavior RED and does not rerun or replace the genuine missing-complete
path RED at source `e925c3ffcd670d33871ad585db13f1c1387468e8`.

## Public seam and common assertions

Every case:

- obtains the actual hosted identity through the same Python 3.12, GitHub-hosted,
  source SHA, run/attempt and native-platform gates as the accepted positive
  hosted test, with only the workflow-specific opt-in name changed;
- calls the frozen `_matching_full_inputs(identity)` fixture builder;
- writes the mutated canonical TSV and unchanged or mutated primary CTest log
  beneath a fresh `fs-endpoint-synthetic-*` directory in the real
  `RUNNER_TEMP`;
- invokes the public finalizer CLI once with CTest exit `8`, capture exit `0`,
  and `scope=synthetic`;
- requires CLI exit `8`, empty stdout/stderr, unchanged input bytes, and the
  public upload selector returning exactly one status file and no gzip;
- decodes that file through the public status codec and checks the exact reason,
  evidence/observer/packer dispositions, null canonical/gzip fields, row count
  zero, and retained primary facts: count `2`, `E80=FAIL`, Boost `108300`,
  `A=NOT_ADOPTED`, and CTest exit `8`.

No assertion extracts a reason from exception or subprocess message text.
Status publication is the machine-readable failure evidence; finalizer exit `8`
continues to preserve the original CTest status.

## Exact mutations

### Binding mismatch (`INTEGRITY`)

Replace exactly one sidecar metadata line
`meta\tnumeric_gate_failures\t2\n` with
`meta\tnumeric_gate_failures\t3\n`.  The canonical sidecar reader accepts the
record and the primary remains complete with count `2`; binding must reject the
observed-count disagreement before scalar replay.

This black-box case proves that the complete path does not omit binding.  Because
`reconcile_replay` also revalidates binding, it cannot prove the precise first
call location without forbidden internal instrumentation; source review owns
that ordering claim.

### Scalar replay conditioning (`CONDITIONING`)

Replace exactly the E0 real component of sidecar slot zero with canonical
`+1.` followed by 109 zeros and `e+00000`.  All other components and rows remain
unchanged.  The replacement token includes the preceding LF and the complete
slot-zero row, so decimal slot suffixes such as `10`, `20`, or `16380` cannot be
mistaken for slot zero.  The sidecar is still canonical and model preflight remains below the
estimator ceiling, but the frozen slot-zero value `(1015/1024, 1/1024)` produces
a reconstructed fresh-component one-norm of `255/128`, exceeding the exact
`3/2` disk guard.  The expected status is `UNRESOLVED`, reason
`CONDITIONING`, observer `UNRESOLVED`.

### Signed metric mismatch (`INTEGRITY`)

In each of the four primary `FS_ENDPOINT_MAX` records, replace the shared,
nonselected `A8.imag` tuple component with canonical
`+1.` followed by 109 zeros and `e-00080`, and replace its quantum with
`q_num=1`, `q_den=2` followed by 189 zeros.  The denominator is emitted as the
actual canonical decimal integer string `2 * 10^189`, never as an expression.
Exactly four records must change so the primary reader's shared-slot tuple
invariant remains true.  The all-zero sidecar replay completes, while metric
reconciliation rejects the signed value difference as `INTEGRITY`.

All zero maxima select real, so `A8.imag` is nonselected in every record.  The
primary reader also checks that `1/(2*10^189)` is the exact serialization quantum
of the mutated decimal.  This case distinguishes omission of signed-component
reconciliation; it is not an independent q-consumption test.  A q-only mutation
would fail the primary reader, while a valid near-boundary q discriminator would
require a different positive fixture and is outside this three-case slice.

## Hosted workflow boundary

The new workflow has exactly two Python-only jobs (`ubuntu-24.04` and
`windows-2022`), pins the accepted checkout/setup-python action SHAs, verifies
Python 3.12 and exact source identity, and runs only the new hosted test with a
600-second child timeout under a 15-minute job timeout.  It triggers only on a
push to `codex/endpoint-finalizer-negative-gate-20260906`.

It does not build or run C++, CTest, OpenFHE, crypto or transforms; it does not
upload artifacts and cannot trigger the existing DCP/full-chain workflow by its
own YAML.  The 16,384-row fixture and complete scalar replay are permitted only
on these hosted jobs.

## Excluded claims

This slice does not prove production/FHE provenance, live-chain evidence,
cryptographic correctness, hostile-parent or crash-proof publication, CI service
availability, or a local/full-suite result.  Root review, secret scanning,
activation and final acceptance remain external to this authoring worktree.
