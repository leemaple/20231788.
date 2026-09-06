# FS-ENDPOINT-PRIMARY-PARTIAL-FACTS-01 — test ledger

## Boundary and provenance

- Branch: `codex/endpoint-primary-partial-facts-20260906`
- Exact base/HEAD: `b0fae5dcfb70a71d462e6354d370fa385fdb5475`
- Runtime: bundled CPython 3.12.14, standard library only
- `BOUNDARY.md` fixed the public seam before the accepted focused RED.
- The OpenFHE workflow and global TDD instructions were read in full before
  implementation.

The immutable public value is
`PrimaryObservation(numeric_gate_failures, e80_disposition, boost_version)`.
Every `PrimaryLogError` exposes it through `observation`. Facts are accumulated
only inside the one parse: Boost commits after a fully validated positive
endpoint-BEGIN field; count/E80 commit only after the canonical legacy COMPLETE
agrees exactly with the already validated labels, count, result, and reason.
The existing `reason`, `detail`, and `first_failure` fields are unchanged.

## Actual TDD receipts

1. `evidence/01_partial_facts_red.json`: two focused tests produced two errors
   against the unchanged reader (exit 1, 0.017s). The immutable observation
   type and error attribute did not exist.
2. `evidence/02_partial_facts_green.json`: the final focused test source passed
   2 tests (exit 0, 0.084s). A complete valid nonzero-E80 primary followed by
   an unknown selected endpoint record retains `(2, FAIL, 108300)`. Early
   failure, a post-BEGIN malformed scale, and an invalid legacy COMPLETE retain
   only facts whose validation boundary was reached.
3. `evidence/03_primary_regression_green.json`: all 32 unchanged primary-reader
   tests passed at the final source hash (exit 0, 0.726s).
4. `evidence/04_combined_green.json`: the focused and regression modules passed
   together, 34 tests total (exit 0, 1.124s).

An earlier exploratory invocation contained a transient unary-plus typo in a
new test fixture. It is not used as the accepted RED and no hash or behavioral
claim is reconstructed for it. The retained RED above was rerun after correcting
that test-source defect and has contemporaneous source/test hashes.

## Final identities

- `tests/paper_endpoint_primary_reader.py`:
  `9b040555ad39afa6ee99eb5e78a249fcaa865da010ec7118f03bded37e92af78`
- `tests/test_paper_endpoint_primary_reader.py` (unchanged):
  `2ed57cb2ee061716b8bf6a3058af07be99083c8b09580ee7aae1f74ea5bd7522`
- `tests/test_paper_endpoint_primary_partial_facts.py`:
  `85973db58b6bc3fb8dbe75402baa53a57625c099fae7df6c2e76eba56fd4aa46`
- `coordination/fs-endpoint-primary-partial-facts-01/BOUNDARY.md`:
  `2ff24ea2ea4e4138da062551fe1d15f6029f931d677179ecf8433cb6bb6512fe`

`git diff --check` exited zero, and the owned source/test/boundary files had no
trailing horizontal whitespace.

## Scope limits

These are bounded synthetic Python parser tests. No C++, OpenFHE, crypto,
16,384-row replay, production pipeline, hosted Linux/Windows chain, CI,
commit, or push was run. The new observation does not accept a malformed log;
it only lets the finalizer preserve reliable partial facts while honoring the
existing first-failure precedence. Original E80 and all scientific acceptance
gates are unchanged.
