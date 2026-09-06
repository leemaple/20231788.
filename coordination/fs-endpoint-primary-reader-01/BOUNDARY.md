# FS-ENDPOINT-PRIMARY-READER-01 — bounded public seam

This slice owns a standalone, standard-library-only reader for the primary
CTest entry 61 stream. It does not read a canonical TSV, perform scalar replay,
compress data, create status JSON, publish files, run C++, or interpret an
observer result as satisfying the unchanged E80 gate.

Requested worker selection was Sol/high. The backend identity is
`requested-unverified`; no model benchmark identity is inferred from it.

## Public interface

```python
parse_primary_log(
    log_bytes: bytes,
    expected_identity: PrimaryIdentity,
    *,
    ctest_exit_code: int,
    timed_out: bool = False,
    expected_scope: str = "live-single-chain",
) -> PrimaryLog
```

`PrimaryIdentity` is frozen and carries the expected compiled source SHA, host,
GitHub run ID, and run attempt. The parser accepts only exact immutable `bytes`,
an actual shell status integer in `0..255` (a Boolean is not an integer here),
and an actual Boolean timeout flag.

The scope defaults strictly to `live-single-chain`. The only alternate value is
an explicitly requested `synthetic`, used by the cross-language fixture; the
observed, matched scope is returned as `PrimaryLog.endpoint.scope`. Scope is
never auto-detected. The original live seam was fixed before tests; this
explicit synthetic option was added through a later focused RED/GREEN after
the cross-language harness requirement became concrete.

Only lines beginning with the exact CTest prefix `61: ` are primary process
records. Unprefixed CTest failure replay is ignored. The complete input is
bounded to 16 MiB and each selected primary physical line to 32,768 original
bytes including its LF or CRLF. Selected records must be ASCII with no NUL.
For an expected Windows identity only, the reader removes exactly one transport
CR immediately before a terminating LF; embedded CR, doubled CR, and a final
bare CR remain invalid. Linux remains strictly LF-only. This transport handling
does not change the canonical TSV ASCII-LF artifact schema.

The returned frozen `PrimaryLog` distinguishes:

- `COMPLETE`: all legacy and endpoint records are complete and consistent;
  E80 may independently be `PASS` with shell status zero or `FAIL` with a
  nonzero shell status.
- `UNRESOLVED`, `FATAL`, or `MISSING`: no complete endpoint-evidence claim is
  made. A first typed `FS_ENDPOINT_FAILURE` is retained before the generic
  legacy failure record. Timeout is `FATAL`; unexplained missing-primary cases
  remain explicit. Unexplained absence uses the adopted `NO_CANONICAL` status
  reason; this primary-only slice does not invent a second missing-evidence
  token. A validated legacy numeric count and E80 disposition are retained in
  an incomplete result only when the corresponding legacy COMPLETE record is
  present and consistent. Merely observing the count line is insufficient.

`PrimaryLogError` rejects malformed, duplicate, reordered, foreign-identity, or
contradictory records. It carries a stable reason and, when already observed,
the first reliable typed endpoint failure so a later status finalizer does not
replace that earlier cause with a generic error. A caller handling such an
exception must prefer `error.first_failure.reason` when present; the later
parser error describes corruption encountered after that earlier execution
cause and does not supersede it.

For complete evidence the parser requires the exact legacy BEGIN and cleanup,
the exact observed numeric-failure count, all nine legacy `RECEIPT` scales, one
strict endpoint BEGIN, nine endpoint scales, 24 ordered checks, four ordered
39-field maxima, one endpoint COMPLETE, and one legacy COMPLETE. It derives all
nine frozen rational scales independently from the fixed Q/divisor constants.
It validates canonical unsigned/reduced rationals, PASS classifier ceilings,
canonical 119-byte decimals, exact decimal quanta and maximum intervals, tuple
field order, selected signed magnitude, identity, ordering, counts, E80
disposition, and shell status. The four ordered MAX allowances recover a
self-consistent fresh/terminal/propagated/residual/identity bound tuple; the
reader uses that tuple to verify internal agreement with all 24 emitted check
allowances rather than accepting arbitrary under-ceiling values. This proves
only consistency among primary records. Because the primary grammar contains
no coefficient one-norm C, it cannot prove that the recovered K came from the
actual C/S. A later reconciler must bind every CHECK and MAX allowance to the
independently validated sidecar C/scales/model before publication.

Partial endpoint records are retained only after their individual grammar and
order checks. They are not promoted to complete evidence and are not compared
with sidecar or replay values in this slice.
