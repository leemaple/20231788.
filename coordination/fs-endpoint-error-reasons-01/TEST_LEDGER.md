# Typed reason TDD ledger

Base: `2e14ecb7e280e23e6aa64db13fc580a408d8bcdd`

The public seams and reason mapping were frozen in `BOUNDARY.md` before tests or
implementation were edited. All commands use the bundled Python 3.12 runtime,
`-B`, and omit `RUN_FULL_ENDPOINT_REPLAY`. Evidence files record literal command
output; no C++, crypto, full 16,384-row replay, CI, commit, or push is performed.

Cycles will be appended in authoring order.

## Cycle 1 — public exception compatibility seam

- Authored the constructor/message/default-reason assertion first.
- Test SHA before implementation:
  `0c48774d1d88315da1d5c2b98c13709d5c3d3d281166758bfd5488875d9683d1`.
- Actual RED: exit 1, all three classes lacked `.reason`; see
  `evidence/01-constructor-red.txt`.
- Minimal implementation added message-preserving constructors with compatible
  one-positional-message calls.
- Actual GREEN: 1 test PASS in 0.000s; see
  `evidence/02-constructor-green.txt`.

## Cycle 2 — sidecar trusted identity mismatch

- Added a public `read_sidecar` test distinguishing malformed expected SHA
  (`FORMAT`) from a valid but different artifact SHA (`IDENTITY`).
- Test SHA before implementation:
  `a5a343f3bde85f9aae9839eb5842ea392c89cad4a15fe5c3cbe64c50e1a10dfb`.
- Actual RED: exit 1; artifact mismatch incorrectly retained the default
  `FORMAT`; see `evidence/03-sidecar-identity-red.txt`.
- Minimal implementation added a keyword reason to `_require` and marks only
  trusted identity-field mismatches as `IDENTITY` in this cycle.
- Actual GREEN: 2 tests PASS in 0.040s; see
  `evidence/04-sidecar-identity-green.txt`.

## Cycle 3 — sidecar grammar before identity comparison

- Added a malformed artifact SHA counterexample to the public identity test.
- Test SHA before implementation:
  `8e9bdb961361d7d721b382ba9e73423c28e2aa59a66c4dfe1e15368413d8389f`.
- Actual RED: exit 1; valid mismatch handling masked malformed artifact grammar
  as `IDENTITY`; see `evidence/05-sidecar-grammar-precedence-red.txt`.
- Minimal implementation validates the five artifact identity scalars before
  comparing them with trusted expected values.
- Actual GREEN: focused test PASS in 0.063s; see
  `evidence/06-sidecar-grammar-precedence-green.txt`.

## Cycle 4 — sidecar semantic vs conditioning failures

- Added public wrong-model and endpoint-radius counterexamples.
- Test SHA before implementation:
  `c7c4656415133a1be357e043b21d5e12f25bcf976a592e054a747c34741bb469`.
- Actual RED: exit 1; wrong model was still `FORMAT`; the first assertion
  correctly prevented a claim about the not-yet-reached radius assertion. See
  `evidence/07-sidecar-semantic-conditioning-red.txt`.
- Minimal implementation marks fixed nonidentity metadata contradictions as
  `INTEGRITY` and the explicit endpoint radius guard as `CONDITIONING`.
- Actual GREEN: focused test PASS in 0.052s; see
  `evidence/08-sidecar-semantic-conditioning-green.txt`.

## Cycle 5 — sidecar estimator ceiling

- Added a coherent `Cfresh=2^473`, `S0=2^100`, `Kfresh=2^373` fixture whose
  `fresh.cross` allowance is exactly `2^-127 + 2^-383`.
- Test SHA before implementation:
  `23b448cad5660a6a87f470765af930aaddff240c5331e7a34b49081f55fbeae2`.
- Actual RED: exit 1; the ceiling failure retained default `FORMAT`; see
  `evidence/09-sidecar-ceiling-red.txt`.
- Minimal implementation marks the exact `2^-128` derived allowance gate as
  `ESTIMATOR_CEILING`.
- Actual GREEN: focused test PASS in 0.042s; see
  `evidence/10-sidecar-ceiling-green.txt`.

## Cycle 6 — §5 raw-excess precedence

- Added the same coherent over-ceiling fixture with raw distance `2^-119`.
- Test SHA before implementation:
  `76579a0dae32ceff401782d9ac6bfe71bd420b286b97b9be1e80b373542f8ab9`.
- Actual RED: exit 1; ceiling was reported before raw finite failure; see
  `evidence/11-sidecar-raw-priority-red.txt`.
- Minimal implementation checks raw `d > 2^-120` before the estimator ceiling.
- Actual focused GREEN: ceiling and raw-priority tests both PASS in 0.066s.

## Cycle 7 — well-formed wrong allowance

- The first draft changed only the numerator and accidentally created an
  unreduced rational. Its observed `FORMAT` was not accepted as a semantic RED;
  the speculative source line was reverted.
- Corrected the fixture to encode the reduced rational exactly twice the frozen
  allowance, then reran before implementation.
- Corrected test SHA:
  `c02f5f934d5f628674f0acf5fbe2d41ad37c4c8b993284ce72ae78a75040dbf4`.
- Actual meaningful RED: exit 1, reduced wrong allowance still returned
  `FORMAT`; see `evidence/12-sidecar-allowance-red.txt`.
- Minimal implementation marks rederived-allowance mismatch as `INTEGRITY`.
- Actual focused GREEN: 1 test PASS in 0.040s.

## Cycle 8 — precise sidecar I/O translation

- Added a public `read_sidecar` test with an `OSError` at the filesystem
  `Path.open` boundary, not an internal helper mock.
- Actual RED: the `OSError("synthetic read fault")` escaped directly; 1 test
  ERROR in 0.014s.
- Minimal implementation catches only `OSError` around explicit `stat` and
  read operations, preserves it as `__cause__`, and emits `IO_ERROR`.
- Actual GREEN: 1 test PASS in 0.013s.

## Cycle 9 — replay conditioning vs estimator ceiling

- Added public disk-excess and serialization-ceiling assertions.
- Actual RED: ceiling returned the backward-compatible unresolved default
  `CONDITIONING` instead of `ESTIMATOR_CEILING`; 1 failure in 0.001s.
- Minimal implementation explicitly labels both derived-bound and early
  serialization budget excess as `ESTIMATOR_CEILING`; disk excess retains
  `CONDITIONING`.
- Actual GREEN: 1 test PASS in 0.001s.

## Cycle 10 — replay grammar, record integrity, and arithmetic

- Added public malformed canonical text, missing validated metadata, and
  nonterminating exact-input counterexamples.
- Actual RED: grammar and record cases both returned default `REPLAY`; the
  arithmetic case already returned `REPLAY`. One test had 2 failing subtests.
- Minimal implementation adds explicit `FORMAT` to grammar/direct-argument
  checks and `INTEGRITY` to validated-sidecar metadata/shape/order checks;
  numerical conversion/arithmetic retains `REPLAY`.
- Actual GREEN: all 3 subtests PASS in 0.001s.

## Cycle 11 — nonfinite and unexpected-fault escape

- Added a public synthetic summary containing Decimal infinity plus a separate
  `TypeError` programming-fault counterexample.
- Actual RED: infinity returned default `REPLAY`; 1 failure in 0.001s.
- Minimal implementation labels represented/arithmetic nonfinite values
  `NONFINITE`; it adds no broad catch.
- Actual GREEN: infinity is typed and the unrelated `TypeError` still escapes;
  1 test PASS in 0.000s.

## Semantic receipt sweep

- Added well-formed scale, E80, comparison-decision, Horner-argmax, and zero-tie
  contradictions through `read_sidecar`.
- Actual RED: all 5 subtests returned `FORMAT` instead of `INTEGRITY`; test
  source at RED and final GREEN is
  `0e071d53dc55a6456f115634b636e09be716ba92449cbf3830c7c523c8d5c902`.
- Minimal implementation labels only those semantic gates `INTEGRITY`; malformed
  component grammar remains `FORMAT`.
- Actual GREEN: all 5 subtests PASS in 0.144s.

## Final bounded regression

Exact final source identities:

```text
paper_endpoint_sidecar_reader.py 40d00c340f40504722e000d450a6883c01fadf99ebb05678018c01b7267008a0
paper_endpoint_sidecar_replay.py 4f44b8012171fc79d5edcae8455f270b161dd354997be0b7bcc96d15b9574d46
test_paper_endpoint_error_reasons.py 0e071d53dc55a6456f115634b636e09be716ba92449cbf3830c7c523c8d5c902
```

Actual combined result: 33 tests in 1.159s, 32 PASS and the existing hosted
full-replay test SKIP. See `evidence/13-final-regression-green.txt`.

Intermediate cycle outputs above are actual observed results. Per-cycle file
hashes were retained where captured before implementation; the final regression
records all exact delivered source/test hashes. No missing intermediate hash is
retroactively fabricated.
