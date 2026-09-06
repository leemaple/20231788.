# FS-ENDPOINT-WRITER-01 — bounded C++ evidence writer draft

Status: source-only draft on `codex/endpoint-writer-20260906` at base
`70ba4d74c7231f4a48e78381f2504e3c04dbcff2`. **NOT COMPILED / NOT RUN.** No
paper chain, transform, crypto, CTest, Python reader, packer, workflow, upload,
or CI dispatch was run. This is not observer GREEN, E80 resolution, or project
acceptance.

## Owned files and seam

Only these new test-local files are owned by this slice:

- `tests/paper_endpoint_evidence_writer.h`
- `tests/paper_endpoint_evidence_writer.cpp`
- `tests/paper_endpoint_evidence_writer_test.h`
- `coordination/fs-endpoint-writer-01/NOTES.md`
- `coordination/fs-endpoint-writer-01/TEST_LEDGER.md`

No existing source, CMake, workflow, paper test, oracle, diagnostics, or Python
reader was edited. The implementation depends on the current pure
`EndpointEvidence`, `CanonicalDecimal`, `IsCanonicalDecimal`,
`ScaledOneNormExponent`, `ExactAbsoluteDifference`, and `AssessDifference`
seams; it does not invoke Python and accepts no context, key, ciphertext, plan,
callback, raw polynomial, or environment-derived credential/identity.

Public API:

```text
WriteEndpointEvidence(EndpointEvidence, explicit identity,
                      caller numeric-failure/cleanup boundary,
                      exclusive trusted publication parent) -> ready path/bytes
ValidateEndpointEvidenceFile(path, expected pure evidence, identity, boundary)
EmitEndpointEvidencePrimary(stream, pure evidence, identity, boundary)
```

`EndpointEvidenceIdentity` explicitly carries `scope`, actual compiled source
commit, platform, GitHub run ID, run attempt, and actual `BOOST_VERSION`.
`EndpointPublicationBoundary` carries the caller-observed numeric failure count
and an owner-cleanup confirmation. Publication refuses a false cleanup flag.
The writer never assumes the historical count `7`; live identity rejects both
the original `9f6c8e...` observation SHA and accepted `2fe655...` link-RED SHA.

## Canonical file behavior

The writer validates before I/O:

- positive reduced exact S0/S8, with all nine frozen scales recomputed by both
  recursive and closed forms and compared;
- nonnegative exact coefficient L1 norms and reduced rational endpoint norms;
- both endpoint represented one-norm guards `<=5/4`;
- exactly 24 ordered receipt IDs, reduced nonnegative distances/allowances,
  rederived allowances, `<=2^-128`, PASS classification, argmax range, zero tie
  policy, and ten-anchor restriction for Horner receipts;
- exactly 16,384 E0 and E8 rows, finite components, four ordered maximum IDs,
  exact prescribed binary768 maximum allowances, signed tuple consistency, and
  exact E0/E8 full-row maximum/tie recomputation.

The sidecar is exactly the adopted schema: one v1-r1 header, 43 ordered metadata
records, 24 ordered PASS checks, the exact slot header, and 16,384 ordered rows
with four 119-byte canonical binary768 decimal fields. It has ASCII LF only,
one final LF, no BOM/CR/NUL/blank/unknown/extra line, maximum 16 MiB, and maximum
32,768 bytes per line including LF. Integer bit size and physical line/file
limits are checked before potentially unbounded decimal/integer work. The file
contains neither its own byte count/hash nor raw polynomial/key/ciphertext data.

The writer requires an existing absolute normalized non-symlink parent whose
ancestors are non-symlinks and which the caller owns exclusively for this
invocation. It atomically claims a previously absent identity directory,
restricts its and `.staging` permissions to owner access through portable
`std::filesystem::permissions`, writes `.candidate.tsv`, closes it, reopens and
strictly parses/binds it to the supplied values, checks the ready leaf remains
absent, then performs one `std::filesystem::rename` to the ready `.tsv`. Staging
and ready are beneath the same newly claimed directory and therefore on the
same filesystem.

This is an honest caller-owned-directory contract: C++17 does not prove hostile
parent race freedom, rename atomicity for every filesystem, permission privacy
on every Windows filesystem, or crash durability. No crash-proof claim is made.
Preexisting identity directories/destinations, symlink leaves/ancestors, dot or
dot-dot traversal, and overwrites are rejected. Every expected writer failure
uses the existing typed `EndpointFailure`/`FailEndpoint` boundary so the current
main can emit `FS_ENDPOINT_FAILURE`. The first detected cause is retained as
`FORMAT`, `IDENTITY`, `INTEGRITY`, `IO_ERROR`, `NONFINITE`, `CONDITIONING`,
`ESTIMATOR_CEILING`, or `MODEL_UNSUPPORTED`. Only the documented formatter
argument failure, filesystem error, and stream I/O failure are narrowly
translated. Unexpected formatter invariants, allocation failures, and other
unexpected exceptions are not caught or converted into success.

## Primary machine grammar

The separate output function does not alter or intercept legacy output. Future
integration may call it after the already-existing cleanup receipt and before
the unchanged `Require(numericFailures==0)`. Each record is one ASCII LF line,
with TAB-separated fixed-order `name=value` fields and no escaping because all
identities/IDs/components are closed validated token sets.

```text
FS_ENDPOINT_BEGIN<TAB>schema=fs-residual-endpoint-primary-v1-r1<TAB>scope=...<TAB>source_commit=...<TAB>host=...<TAB>github_run_id=...<TAB>github_run_attempt=...<TAB>boost_version=...
FS_ENDPOINT_SCALE<TAB>index=i<TAB>numerator=uint<TAB>denominator=positive_uint
FS_ENDPOINT_CHECK<TAB>id=check_id<TAB>result=PASS<TAB>distance_num=uint<TAB>distance_den=positive_uint<TAB>allowance_num=uint<TAB>allowance_den=positive_uint<TAB>argmax_slot=uint<TAB>argmax_component=real|imag
FS_ENDPOINT_MAX<TAB>id=E0|E8|I8|A8<TAB>magnitude=canonical119<TAB>magnitude_exact_num=uint<TAB>magnitude_exact_den=positive_uint<TAB>magnitude_quantum_num=uint<TAB>magnitude_quantum_den=positive_uint<TAB>allowance_num=uint<TAB>allowance_den=positive_uint<TAB>interval_lower_num=uint<TAB>interval_lower_den=positive_uint<TAB>interval_upper_num=uint<TAB>interval_upper_den=positive_uint<TAB>argmax_slot=uint<TAB>argmax_component=real|imag<TAB>tuple fields...
FS_ENDPOINT_COMPLETE<TAB>result=PASS<TAB>assurance=CONDITIONAL<TAB>row_count=16384<TAB>check_count=24<TAB>numeric_gate_failures=uint<TAB>E80_disposition=PASS|FAIL<TAB>A_disposition=NOT_ADOPTED<TAB>owner_cleanup_confirmed=true
```

There are exactly nine SCALE records (`index=0..8`), 24 CHECK records in sidecar
order, and four MAX records in `E0,E8,I8,A8` order. Each MAX record has all eight
signed tuple components in fixed order
`E0.real,E0.imag,E8.real,E8.imag,I8.real,I8.imag,A8.real,A8.imag`. Each component
is followed immediately by `<field>_q_num` and `<field>_q_den`. Magnitude has
both its canonical decimal and exact represented dyadic rational. All rational
fields are reduced with nonnegative numerator and positive denominator.

For a nonzero canonical component with decimal exponent `e`, its exact reported
serialization quantum is `q = 1/2 * 10^(e-109)`; canonical zero has `q=0`.
`interval_lower=max(0,magnitude_exact-allowance)` and
`interval_upper=magnitude_exact+allowance`. Thus the future independent packer
does not need to infer binary magnitude rounding or silently treat 110 decimal
digits as exact. The actual replay still owns sums of live allowance, scalar
replay allowance, metric quantum, and signed-tuple quanta.

## Deliberate unfinished boundaries

- No call site is added. In particular, cleanup ordering and placement before
  the unchanged numeric-failure Require are not yet wired.
- No CMake source/test registration, observer self-test dispatch addition,
  workflow wrapper, status JSON, gzip, upload selection, or Python replay is in
  this slice.
- `EndpointEvidence` retains full E0/E8 rows but only maximum receipts/tuples for
  I8/A8. The writer can rederive E0/E8 global maxima and validate every signed
  maximum tuple locally; it cannot independently rescan discarded full I8/A8
  vectors. That stronger check would require an explicitly reviewed change to
  the existing diagnostics value shape, which this ownership boundary forbids.
- The writer binds `sourceCommit` to an explicit caller value and validates its
  grammar/known-old exclusions; the future call site must bind it to the same
  compiled `PAPER_SOURCE_COMMIT` used by primary BEGIN and hosted checkout.
- The existing independent Python reader/packer must separately accept the
  grammar and rederive fractions/allowances. This C++ draft was not imported or
  executed by Python.
- Hosted Linux/Windows compilation under strict warnings and all behavioral
  tests for these unintegrated writer files remain pending. Root separately
  reported dual-host diagnostic compile/self-test SUCCESS at source `2af...`
  in run `34008596955` and latest main `0ff6159...`; neither report contains
  this writer, a live chain, or an E80 resolution.

Review corrections also enforce the global tie consequence available without
full I8/A8 vectors: any zero I8 or A8 maximum must be reported at slot 0, real.
The independent corrected-source pass exercises the slot and component rules
separately for both residuals. It also makes positive-infinity,
negative-infinity, and NaN fixtures internally coherent and classifies
finiteness before the separate nonnegative-magnitude integrity condition, so
each non-finite case reaches the typed `NONFINITE` boundary as its first cause.
For comparison receipts, malformed/model-mismatched rationals remain earlier
semantic failures, but the exact classifier's raw finite `d>2^-120` FAIL is now
mapped to `INTEGRITY` before testing the `2^-128` estimator ceiling. A
model-derived over-ceiling allowance can therefore no longer hide the spec's
higher-precedence raw observer failure; absent a raw FAIL, ceiling excess still
maps to `ESTIMATOR_CEILING`.
The nonzero primary synthetic fixture uses distinct signed tuple fields, an
exact negative-exponent `1/2` quantum/interval, the existing exact integer
half-even-down case, and an exact carry-renormalization case. Its expected
fractions and all 39 fields of the E0 MAX record are constructed independently
in the public test rather than counted by substring alone.

## Source identities

Final source-only draft identities before integration:

| Path | Bytes | SHA-256 |
| --- | ---: | --- |
| `tests/paper_endpoint_evidence_writer.h` | 2846 | `c251fab8d526b6734036f97e47d67f36fcaecfd692f9cc9f22dcf0e32efc42b9` |
| `tests/paper_endpoint_evidence_writer.cpp` | 41762 | `3fc99b052868b252aa925cb7c350e24036eaf4f54be8e9340c3bdd9a502e1ac9` |
| `tests/paper_endpoint_evidence_writer_test.h` | 24241 | `0621395297f16bc6fbfb85e17724741158563d0978baa7f65d8f743317adb1b0` |

These hashes describe uncompiled draft bytes in this isolated worktree, not an
integrated commit or hosted-tested source SHA.
