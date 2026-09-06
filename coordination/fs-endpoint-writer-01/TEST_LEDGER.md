# FS-ENDPOINT-WRITER-01 — test ledger

Base: `70ba4d74c7231f4a48e78381f2504e3c04dbcff2`

Branch: `codex/endpoint-writer-20260906`

Execution status: **ALL C++ TESTS NOT COMPILED / NOT RUN.** The task explicitly
prohibited local C++ configure/build/execution, OpenFHE/crypto/FFT/NTT work,
Python invocation, CI dispatch/rerun, push, and integration edits. No behavioral
RED or GREEN is claimed from source inspection.

## Test-first authoring order

Times below are the UTC wall-clock receipts printed immediately after the named
authoring boundary. File-system mtime is retained as an epoch because the host
`stat` display used the local timezone even when its format suffix said `Z`.

1. RED-DRAFT-1 — complete synthetic full-row publication assertion authored
   before the writer header/implementation existed.
   - Receipt time: `2026-09-06T02:46:05Z`
   - Test-header SHA-256:
     `d2f8c75ad83c15e95dfb5ce550e44414f91050815821a5fdfee3bc598089c49a`
   - Captured mtime display: `2026-09-06T10:45:59Z` (host-local rendering;
     suffix is not treated as UTC evidence).
   - Intended missing seam: `WriteEndpointEvidence` and
     `ValidateEndpointEvidenceFile` did not exist.
   - Execution: NOT COMPILED / NOT RUN; therefore not an observed RED.
2. GREEN-DRAFT-1 — minimal public value types plus writer/validator/primary
   implementation authored for the first slice.
   - Receipt time: `2026-09-06T02:50:26Z`
   - Header SHA-256:
     `656b6f0947299338fc2d1721c4712aab11ac3f67b7d83515b97c407aaa672929`
   - CPP SHA-256:
     `4873ca4aff12157363e5e3918bc5cab49733d4406c4b0155ea13303a8266d78c`
   - Test header remained
     `d2f8c75ad83c15e95dfb5ce550e44414f91050815821a5fdfee3bc598089c49a`.
   - Captured mtimes (Unix seconds): header `1788662782`, CPP `1788663013`,
     test header `1788662759`.
   - Execution: NOT COMPILED / NOT RUN; therefore not an observed GREEN.
3. RED-DRAFT-2 — overwrite refusal, malformed grammar/identity/scale/count,
   reordered/missing/duplicate/extra/full-row gates, byte/line limits,
   traversal/symlink, and primary grammar assertions authored before the final
   validation review pass.
   - Receipt time: `2026-09-06T02:51:34Z`
   - Test-header SHA-256:
     `71f3e10b88c350d090a0057ae8641e2b3b14c0838b12eb696372042b784e27dd`
   - Captured mtime (Unix seconds): `1788663083`.
   - Execution: NOT COMPILED / NOT RUN; therefore not an observed RED.
4. GREEN-DRAFT-2 — C++17 string-view portability adjustments, explicit
   primary output include, bounded line fixture, and final source review.
   - Final source hashes are in NOTES.md and the final-state section below.
   - Execution: NOT COMPILED / NOT RUN; therefore not an observed GREEN.
5. REVIEW-RED-DRAFT-3 — typed-reason, zero-I8/A8 tie, and nonzero primary
   assertions were authored before the corresponding writer corrections.
   - First receipt time: `2026-09-06T03:08:41Z`
   - Intermediate test-header SHA-256:
     `4aea66dcd62e53cc9d826372d5b611f91c20d341bd98d219e48a6d10c797b026`
   - Expanded typed NONFINITE/CONDITIONING/cleanup receipt time:
     `2026-09-06T03:09:26Z`
   - Expanded test-header SHA-256:
     `a668ac01e6562f28a3802c4147e96cfdf6d825e35bb80d1e795bbdf8b0536eb0`
   - Captured final pre-implementation-test mtime (Unix seconds): `1788664161`.
   - Execution: NOT COMPILED / NOT RUN; therefore not an observed RED.
6. REVIEW-GREEN-DRAFT-3 — expected failures changed to typed
   `EndpointFailure`, zero I8/A8 tie enforcement added, primary values/quanta
   retained unchanged, and the synthetic temporary parent canonicalized.
   - First implementation receipt time: `2026-09-06T03:14:04Z`; final
     source-identity receipt after assertion tightening: `2026-09-06T03:16:44Z`.
   - Header/CPP/test mtimes (Unix seconds): `1788664653`, `1788664627`,
     `1788664560`.
   - Execution: NOT COMPILED / NOT RUN; therefore not an observed GREEN.
7. REVIEW-RED-DRAFT-4 — coherent non-finite fixtures and independent zero-tie
   mutations were authored before the corresponding classification-order
   correction.
   - Receipt time immediately after test authoring: `2026-09-06T03:32:52Z`.
   - Test-header SHA-256:
     `61c36d534a700c65a8fe079b9061a9c3fb296a938614283d899d7fcabdada0c3`.
   - Test-header mtime: Unix seconds `1788665531`
     (`2026-09-06T03:32:11Z`).
   - Execution: NOT COMPILED / NOT RUN; therefore not an observed RED.
8. REVIEW-GREEN-DRAFT-4 — magnitude finiteness is now classified before the
   separate nonnegative integrity condition.
   - Receipt time immediately after implementation authoring:
     `2026-09-06T03:33:30Z`.
   - CPP SHA-256:
     `93bc73f9556356ad9550f776640c91d4c02328e220d4dec49fdf8737cbfb630e`.
   - CPP mtime: Unix seconds `1788665596` (`2026-09-06T03:33:16Z`).
   - Execution: NOT COMPILED / NOT RUN; therefore not an observed GREEN.
9. REVIEW-RED-DRAFT-5 — a coherent modeled over-ceiling allowance combined
   with a raw finite distance above `2^-120` was authored before the precedence
   correction.
   - Receipt time immediately after test authoring: `2026-09-06T03:36:01Z`.
   - Test-header SHA-256:
     `0621395297f16bc6fbfb85e17724741158563d0978baa7f65d8f743317adb1b0`.
   - Test-header mtime: Unix seconds `1788665753`
     (`2026-09-06T03:35:53Z`).
   - Execution: NOT COMPILED / NOT RUN; therefore not an observed RED.
10. REVIEW-GREEN-DRAFT-5 — the exact classifier result is inspected for raw
    FAIL before the separately typed estimator-ceiling disposition.
    - Receipt time immediately after implementation authoring:
      `2026-09-06T03:36:27Z`.
    - CPP SHA-256:
      `3fc99b052868b252aa925cb7c350e24036eaf4f54be8e9340c3bdd9a502e1ac9`.
    - CPP mtime: Unix seconds `1788665776` (`2026-09-06T03:36:16Z`).
    - Execution: NOT COMPILED / NOT RUN; therefore not an observed GREEN.

Independent corrected-source findings and exact dispositions for this cycle:

- Finding: the prior positive-infinity fixture changed `freshErrors[0].real`
  without changing the signed maximum tuple, so tuple inconsistency could be
  the first cause. Disposition: the fixture now changes the E0 row, E0 maximum
  magnitude, and matching E0 tuple component coherently; its only intended
  public failure is typed `NONFINITE`.
- Finding: a negative-infinite magnitude satisfied `magnitude < 0` before the
  finiteness guard and was therefore mislabeled `INTEGRITY`. Disposition:
  finiteness now precedes the separate negative-magnitude guard, and coherent
  positive infinity, negative infinity, and NaN fixtures each require typed
  `NONFINITE`.
- Finding: prior zero I8/A8 tie fixtures changed slot and component together.
  Disposition: both I8 and A8 now have distinct slot-only and imaginary-only
  mutations, each requiring typed `INTEGRITY` through the public writer seam.
- Finding: comparison allowance ceiling was classified before the exact
  comparison decision, hiding a raw finite `d>2^-120` behind
  `ESTIMATOR_CEILING`. Disposition: after rational and exact-model allowance
  validation, a classifier FAIL is mapped to typed `INTEGRITY` first; only a
  non-FAIL decision can reach the ceiling disposition. The focused fixture
  sets `C_fresh=2^473`, hence the exact fresh-cross allowance
  `2^-127+2^-383`, and distance `2^-119`; it requires `INTEGRITY`.

Pre-review handoff bytes remain recorded for comparison and were not relabeled:

| Path | Bytes | Pre-review SHA-256 |
| --- | ---: | --- |
| `tests/paper_endpoint_evidence_writer.h` | 2671 | `e463ba504f7d98d48a182dd5288e57a030a039d353e72b6662b3fa0c49e7ed87` |
| `tests/paper_endpoint_evidence_writer.cpp` | 39562 | `35613474129a2db4bbe1225b293626ad25b5df99b9605f8862ef15a33d14a353` |
| `tests/paper_endpoint_evidence_writer_test.h` | 13260 | `51749ae6ed7cd378da4e3cefba8bee0cdf802a17d1a86059c92a8e7c1baff3b1` |

## Source-only checks actually run

| Command | Result | Classification |
| --- | --- | --- |
| `pwd; git branch --show-current; git rev-parse HEAD; git status --short` (issued as one read-only inventory command) | Exact assigned worktree, branch and base confirmed; initially clean | OBSERVED source state, not a test |
| `git diff --check` | Exit 0/no output, while new files were still untracked | OBSERVED tracked-diff syntax only; it does not validate untracked source |
| `rg -n '[[:blank:]]+$'` over the three new C++ files | Exit 1/no matches | OBSERVED no trailing horizontal whitespace |
| `wc`, `shasum -a 256`, `stat` over new source | Exact bytes/hashes/mtimes recorded | OBSERVED source identity only |
| metadata source enumeration | 43 ordered entries visually/source-count verified | OBSERVED static count only |

No source-only check is promoted to behavioral PASS.

## Authored synthetic coverage — NOT RUN

`RunEndpointEvidenceWriterBoundaryTests()` contains:

- one complete 16,384-row zero-residual candidate using explicit
  `scope=synthetic`, a `fs-endpoint-synthetic-` disposable namespace, all 43
  metadata fields, all 24 prescribed receipts, and valid S0/S8;
- writer close/reopen validation and public-validator validation;
- preexisting identity/overwrite refusal;
- malformed canonical decimal (`nan`), wrong source identity, wrong exact S0,
  row-count metadata mismatch, reordered checks, reordered/duplicate/missing/
  extra rows, and a four-row candidate that cannot satisfy the full-row gate;
- >16 MiB file and >32,768-byte line rejection;
- dot-dot/non-normalized parent refusal and leaf-symlink refusal when the host
  permits creation of the symlink fixture;
- primary grammar counts: nine exact scale records, 24 checks, four maxima,
  caller numeric count `9` with no fabricated `7`, and tuple quantum fields.
- exact typed reason checks for identity, format, integrity, nonfinite and
  conditioning failures; an unexpected plain standard exception cannot satisfy
  these assertions;
- zero I8 and A8 maximum receipts rejected unless their argmax is slot 0/real,
  with separate slot-only and component-only public mutations for both
  residuals;
- coherent positive-infinity, negative-infinity, and NaN maximum/tuple
  fixtures requiring typed `NONFINITE` rather than an earlier tuple or sign
  classification;
- a coherent fresh-cross model with allowance `2^-127+2^-383` and raw distance
  `2^-119`, discriminating raw-threshold `INTEGRITY` precedence from
  `ESTIMATOR_CEILING`;
- a nonzero signed MAX fixture with all eight tuple value/quantum fields checked
  in order, exact `1/2 +/- 2^-504` interval fractions, negative decimal exponent,
  integer half-even-down, and carry-renormalized canonical cases.

These are deterministic file/value fixtures only. They contain no live evidence
filename, context, encryption, raw polynomial, key, or ciphertext.

## Required future hosted steps — not authorized or run here

After root review/integration, a future exact source should:

1. Compile this source with the repository's Linux and Windows C++17 strict
   warning settings. Any compile/link warning/error is a real failure, not RED
   or GREEN inferred by this author.
2. Wire the synthetic boundary function into the already-approved explicit
   observer self-test without adding a CTest entry or a paper chain.
3. Run the synthetic self-test and retain actual output.
4. Independently validate the C++ candidate using the Python reader/packer and
   its own malformed/hash/gzip/status/wrapper coverage. This slice does not
   claim those separate tests.
5. Wire both new C++ calls only after the existing owner-cleanup receipt and
   before the unchanged `Require(numericFailures==0)`, binding the explicit
   identity to compiled source/run/attempt/host/`BOOST_VERSION`.
6. Preserve old stdout and the original E80 gate. Then follow the project stop
   rule: at most one unchanged normal paper CTest chain per host on the new
   exact SHA, with failure-preserving finalization and no automatic rerun.

The known real E80 baseline at `9f6c8e...` remains FAIL. The accepted link RED at
`2fe655...` is separate evidence. Root's later dual-host diagnostic
compile/self-test SUCCESS at source `2af...`, run `34008596955`, and latest main
`0ff6159...` do not include this unintegrated writer or a live chain. This draft
does not alter or reinterpret any of them.

## Final source-only state

| Path | Bytes | SHA-256 | C++ status |
| --- | ---: | --- | --- |
| `tests/paper_endpoint_evidence_writer.h` | 2846 | `c251fab8d526b6734036f97e47d67f36fcaecfd692f9cc9f22dcf0e32efc42b9` | NOT COMPILED / NOT RUN |
| `tests/paper_endpoint_evidence_writer.cpp` | 41762 | `3fc99b052868b252aa925cb7c350e24036eaf4f54be8e9340c3bdd9a502e1ac9` | NOT COMPILED / NOT RUN |
| `tests/paper_endpoint_evidence_writer_test.h` | 24241 | `0621395297f16bc6fbfb85e17724741158563d0978baa7f65d8f743317adb1b0` | NOT COMPILED / NOT RUN |

No git commit, push, workflow dispatch, or integration was performed.
