# FS-ENDPOINT-PRIMARY-READER-01 — test ledger

## Boundary and provenance

- Branch: `codex/endpoint-primary-reader-20260906`
- Exact base/HEAD during this slice:
  `0ff6159f9ca28d9adb9c0470d6f9f3cff6e6e9ab`
- Requested worker: Sol/high; backend identity: `requested-unverified`
- Runtime: bundled Python 3.12.14, standard library only
- Public seam was documented in `BOUNDARY.md` before the first test run.
- Owned files only were created or edited. No C++, crypto, 16K replay, CI,
  commit, or push was run.

## TDD evidence

1. `01-import-red.json`: the first public complete-E80 tracer failed because
   the reader module did not exist (`ModuleNotFoundError`).
2. `02-complete-e80-green.json`: the first complete nonzero-E80 path passed
   after the minimum reader implementation.
3. Later small tests covered zero/pass, typed/untyped/timeout/missing failure
   preservation, prefix replay, identity/order/count/scale/status corruption,
   the classifier distinction, and the full MAX grammar. These were already
   green under the incrementally expanded implementation; no RED is claimed.
4. `03-post-complete-replay-red.json`: a focused source-review tracer exposed
   that a prefixed RECEIPT after legacy COMPLETE was accepted.
5. `04-post-complete-replay-green.json`: the post-COMPLETE ordering guard
   closed that defect.
6. `05-full-python-green.json`: the then-current bounded suite ran 20 tests,
   all passing in 0.371 seconds with exit code zero.
7. `06-exact-allowance-red.json`: an under-ceiling but model-inconsistent
   check allowance was accepted, proving that ceiling/classifier checks alone
   were insufficient.
8. `07-exact-allowance-green.json`: exact recovery of a bound tuple from the
   four MAX allowances closed the primary-internal gap and binds all 24
   allowances to that tuple. Later review correctly limited this to
   self-consistency because primary does not contain actual C.
9. `08-final-python-green.json`: the first-pass bounded suite ran 21 tests, all
   passing in 0.402 seconds with exit code zero. It predates the independent
   review corrections below and is not the final acceptance receipt.
10. `09-incomplete-retention-red.json` / `10-incomplete-retention-green.json`:
    three focused cases established FATAL timeout classification, preservation
    of a complete legacy E80 record, and refusal to infer E80 from the OBS count
    alone.
11. `11-grammar-dyadic-red.json` / `12-grammar-dyadic-green.json`: focused
    public-seam tests closed arbitrary/reordered gate labels, unknown selected
    endpoint records, and non-dyadic represented-distance/magnitude records.
12. `13-explicit-scope-red.json` / `14-explicit-scope-green.json`: the parser
    retained strict live default behavior while adding explicitly requested,
    strictly matched synthetic scope and returning that observed scope.
13. `15-fixture-correction-red.json` truthfully records two test-fixture errors:
    the carry-renormalized decimal required quantum 50, and the first alternate
    bound exceeded the observer classifier after amplification. Neither is
    reported as a reader defect.
14. `16-realizable-max-alternate-model-green.json` verifies writer-realizable
    dyadic MAX values for every residual, alternating signed real/imaginary
    selections, all eight tuple fields, the integer `10^111-5` half-even carry,
    and an under-ceiling self-consistent alternate K.
15. `17-full-suite-stale-expectation-red.json` records the old missing-COMPLETE
    test expecting MISSING despite supplying a nonzero status after legacy PASS;
    the corrected expectation separates that late CTest failure.
16. `18-review-corrections-green.json`: the corrected bounded suite ran 29
    tests, all passing in 0.627 seconds.
17. `19-final-python-green.json`: after the final observed-scope and timeout
    retention checks, all 29 tests passed in 0.650 seconds with exit code zero.
18. `20-windows-crlf-transport-red.json`: the focused public-bytes seam ran
    three tests. The complete Windows CRLF stream and exact-limit CRLF record
    both failed at the blanket CR guard; invalid CR forms were already rejected.
19. `21-windows-crlf-transport-green.json`: after removing exactly one terminal
    transport CR for an expected Windows identity, all three focused tests
    passed in 0.045 seconds. Embedded, doubled, final bare, and Linux CR remain
    rejected; line limits count the original bytes including CRLF.
20. `22-windows-crlf-full-green.json`: all 32 bounded reader tests passed in
    0.714 seconds with exit code zero. The receipt retains SHA-256 and byte sizes
    for the complete Windows CRLF input and each invalid transport fixture.

The synthetic complete records use independently derived frozen S0..S8 and
the exact zero-norm conditional allowance model. The nonzero MAX fixtures use
writer-realizable dyadic exact values, positive and negative selected tuple
components, a literal integer `10^111-5` half-even carry to
`+1.000...e+00111`, the carry-renormalized quantum 50, and exact lower/upper
rational intervals.

The alternate-K fixture is intentionally accepted because all primary
allowances are mutually consistent and under the ceilings. It demonstrates a
scope boundary, not actual-C correctness: the primary grammar has no C. The
mandatory future primary↔sidecar reconciler must bind these values to the
sidecar's independently validated C/scales/model before publication.

## Final source hashes

- `tests/paper_endpoint_primary_reader.py`:
  `48ede879edd921e180ce3acfdc8ddab3ab141439e74ca0622ab350ec06e795e4`
- `tests/test_paper_endpoint_primary_reader.py`:
  `2ed57cb2ee061716b8bf6a3058af07be99083c8b09580ee7aae1f74ea5bd7522`

## Remaining gates

- The reader has not consumed a hosted Linux/Windows live primary CTest log;
  the Windows CRLF evidence is bounded synthetic transport input. No C++
  command was run from this worktree.
- It does not read or compare the canonical sidecar, execute the 16,384-row
  scalar replay, gzip evidence, finalize status, or publish artifacts.
- No C++ test, full OpenFHE run, or CI result is implied by the Python result.
  The unchanged original E80 gate remains outside this parser slice and is
  still reported FAIL in the surrounding project evidence.
