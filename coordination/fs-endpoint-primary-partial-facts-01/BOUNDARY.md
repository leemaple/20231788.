# Primary parse-error observation boundary

Status: public seam fixed before the first focused test on 2026-09-06.

`PrimaryLogError.observation` is always a frozen `PrimaryObservation`:

```python
PrimaryObservation(
    numeric_gate_failures: int | None,
    e80_disposition: str,
    boost_version: int | None,
)
```

The observation is derived from state already validated during the same
`parse_primary_log` call. It does not reparse a prefix, consult global state, or
infer missing output. Boost becomes observable only after a fully validated
positive Boost field in `FS_ENDPOINT_BEGIN`. Count and E80 become observable
only after a canonical legacy `COMPLETE` agrees with the previously validated
ordered labels and declared count, including the exact PASS/FAIL reason.

An error before those commit points retains `None`, `NOT_OBSERVED`, and/or the
already committed Boost value as applicable. The error's existing `reason`,
`detail`, and `first_failure` contracts remain unchanged. Successful
`PrimaryLog` results and all grammar, order, identity, numeric, transport, and
classifier rules are unchanged.

This seam only exposes reliable partial facts for a caller constructing a
truthful incomplete status. It does not authorize recovery, acceptance of the
malformed stream, prefix reparsing, finalization, publication, or a complete
evidence claim.
