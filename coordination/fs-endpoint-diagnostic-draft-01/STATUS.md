# Endpoint diagnostic implementation draft - not GREEN

Base: `643bfac42fe2233fbda7f5676c3c2aece01e9496` on
`codex/paper-scale-implementation-20260905`. The existing hosted API/link RED
remains source `2fe655d493dcde5f05aa1515f41ca6823bba30bd`; original numerical
source `9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e` still has E80 FAIL.

## Actual draft state, 2026-09-06

- Seven missing helpers now have definitions: exact scaled one-norm exponent,
  independent binary512/768 twisted DFT, independent sparse reference, exact
  dyadic distance, classifier, canonical decimal formatter and byte validator.
- Root read the complete scalar and transform drafts before copying the six
  source/test/header files into this tree. Each copy was verified by `cmp` exit 0.
  The shared failure contract was independently compared by its author.
- CMake adds four test-local CPPs to the existing excluded paper target only.
  No CTest name/order/argv/timeout change, production change, legacy test-body
  change or oracle-header change is intended. The limited workflow change below
  is a separate source-reviewed early compile/self-test gate.
- Capture computes the prescribed 24 checks and full-slot signed residuals,
  exact norms, and measured maxima/tuples. It now preserves owned exact S0/S8
  values in the post-client-scope handoff. RunPaper reuses its existing fresh
  and terminal polynomials/decoded values and final Horner result; no new
  encryption or intermediate endpoint is introduced.
- Only tags and pure value evidence leave RunPaper. The existing unrelated-key
  ownership checks, cleanup receipt, numeric failure count and original
  `Require(numericFailures==0,...)` remain. This does not yet publish evidence.
- Finite-source producer conversion overflow is an unavailable comparison,
  not fabricated zero data or source NONFINITE. Available finite components
  are still checked for raw threshold excess before unsupported disposition.
  Actual source nonfinite remains fatal. Horner conversion support observes
  the unchanged decimal-string R(Int) result and does not replace that path.
- Checked arithmetic preserves each specified real operation. Nonfinite
  arithmetic is fatal; finite underflow/exponent applicability failure is
  MODEL_UNSUPPORTED. The explicit checked binary exponent envelope is
  [-400000,400000] in frexp convention. It bounds exact extraction and allocation
  and contains all legal five-digit canonical decimal exponents and the frozen
  scale integers; this is a conditional resource boundary, not a Boost proof.

## Tests and review - precise limits

The earlier hosted seven-undefined-functions RED is already retained. New C++
boundary assertions were authored before their corresponding corrections, but
were NOT executed on this Mac. The capture scale-output assertions and new
invalid-input tests existed at 01:52:30 UTC before the scale-retention fix:
`paper_endpoint_diagnostics_test.h` SHA-256
`8624594a9c9f451c50bf0243e15bc9a702af3b9cf16baa685ad137ec37b8044a`;
the then paper CPP SHA-256 was
`ab2909be7c220ac87ab527562b59eeed66c73972ced958ec27b7b3832993da98`.
Later includes/dispatch integration changed the paper CPP, not the evidence
that no C++ test was run. This is an authoring-order record, NOT behavioral RED.

The additional public-seam tests cover invalid norms/scales, fractional/exact K,
opposite-sign exact distances, canonical lower-exponent carry, allocation-range
rejection, malformed polynomials/sparse degrees, conversion/range rejection and
source-nonfinite capture. Frozen observer fixtures remain unchanged. Synthetic
negative capture tests reject before DFT; a supported-range transform rejection
may initialize a reusable 512-bit table but performs no additional completed DFT
or encrypted chain. The normal graph remains eight control and four endpoint
DFTs; self-test retains eight completed control DFTs and zero crypto.

Independent Codex contexts reviewed K/scalar semantics and transform/Capture
boundaries. Transform review found missing arithmetic checks and wrong range
classification; these were corrected and root inspected the complete changed
source. Capture review found lost scales and wrong finite-conversion
classification; root corrected them and requested a targeted follow-up review.
That review found two remaining conversion-boundary issues. The reviewer then
authored focused pure-value assertions and a correction; root read the complete
changed conversion paths, signatures and main wiring independently. Producer
components now carry separate availability bits, preserving an available raw
distance beside an unsupported conversion. Horner support flags are computed
before the unchanged endpoint Horner calls and actually passed to comparisons.
Conservative decimal/binary exponent preflights replace speculative catches;
actual source nonfinite remains fatal. Unrecognized parser exceptions still
propagate. Exact Boost parser exception behavior is not certified by this source
review and remains pending hosted evidence. These new C++ tests were authored
but NOT RUN; this is not behavioral RED/GREEN. These are independent Codex
contexts, not different-provider proof or final Pro review.

A separate Python reader slice was developed in
`/Users/lifeng/Documents/20231788-openfhe-endpoint-sidecar-20260906`. Its initial
seven tests passed on Python 3.9.6, but root's
actual Python 3.12.14 run failed six tests in fixture integer formatting (one
passed; 0.018 seconds, exit 1). Exact combined output is retained in that tree's
`coordination/fs-endpoint-sidecar-reader-01/ROOT_PYTHON312_BASELINE.json`.
Root additionally found missing Horner argmax constraints, an unstated integer
length cap, and duplicated allowance logic in the tests. These were corrected;
the focused behavioral RED and author GREEN are retained. Root independently
observed 11 tests PASS in 0.695 seconds, exit 0, using bundled Python 3.12.14 and
verified unchanged source hashes before/after. The exact output is retained in
`coordination/fs-endpoint-sidecar-reader-01/ROOT_PYTHON312_CORRECTED.json`.
The two Python files and their evidence were copied here with exact `cmp` checks.
This is strict synthetic reader evidence only: no independent scalar replay,
primary-log reconciliation, all-nine-scale validation or complete packer yet.

## Early remote draft-only compile/self-test decision

The execution order is deliberately split to obtain actual C++ feedback before
building the remaining writer/packer. A separate Codex reviewer inspected the
proposal and exact workflow diff with no findings; this is source review only.

- The only new push trigger is `codex/endpoint-diagnostic-draft-20260906`.
- Both hosts retain all existing old60/API steps and the paper-target build.
- Only that exact draft ref then runs the existing observer self-test once,
  with OMP_NUM_THREADS=2 and a 20-minute step limit. It performs the eight frozen
  synthetic control DFTs but no paper context/key/encryption or live chain.
- Only that draft ref skips the existing full paper CTest step. Every other
  existing branch retains the original CTest invocation and parameters.
- Failure remains a failing job. No workflow dispatch/rerun is requested.
- Retire the exact draft trigger before another push to that draft branch;
  do not use follow-up documentation commits to rerun the self-test implicitly.

At this pre-push checkpoint no remote result is claimed. `actionlint`, bundled
PyYAML and bundled Node YAML libraries were unavailable; those were tool
availability checks, not successful YAML parser checks. The small exact diff
was reviewed and `git diff --check` passed. GitHub will validate the workflow.

## Still required before a hosted live endpoint observation

1. Finish and independently review post-cleanup canonical writer/strict C++
   validator, primary diagnostic output, and the standalone Python reader.
2. Implement scalar replay, primary-log/all-nine-scale reconciliation, strict
   status/gzip and transactional exact-path publication, with actual synthetic
   filesystem and failure-preserving shell tests.
3. Wire one observer self-test, one primary paper CTest and one finalizer in each
   hosted job; preserve all limits, old 60/API gates and original nonzero exit.
4. Only then push an exact reviewed implementation to the existing live CI branch,
   retain its actual result and stop after at most one next chain per host.

No C++ configure/build, crypto, FFT/NTT, benchmark, CI dispatch/rerun or new paper
chain was performed for this draft. A backup branch must be labeled DRAFT and
must not be described as tested, complete, or accepted. No 1000-trial gate,
favorable-key selection, weakened E80 threshold or refresh is introduced.
