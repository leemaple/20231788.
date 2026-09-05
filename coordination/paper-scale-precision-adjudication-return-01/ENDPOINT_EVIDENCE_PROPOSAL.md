# FS-RESIDUAL-ENDPOINT-01 evidence and TDD freeze proposal

**Status: PROPOSAL ONLY — not frozen, implemented, compiled, or run.** This document resolves only the evidence-retention and self-referential-hash gaps identified in `SOL_ADJUDICATION_REVIEW.md`. It deliberately does not choose the observer/error bounds being adjudicated separately by Astra.

Review basis: clean repository `codex/paper-scale-implementation-20260905` at `75f78f336cc068852b2306281b54c774a0660549`; tested-source baseline `9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e`; unchanged production attribution `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89`; pristine OpenFHE pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`. No local build, transform, codec, FHE, CI, network, or external-agent execution supports this proposal.

## 1. Minimal implementation seam

Limit the future patch to:

1. `tests/paper_full_eight_square_contract_test.cpp`: retain the existing default execution and final `Require(numericFailures==0, ...)`; add a no-crypto observer/evidence self-test mode and commit evidence after owner/foreign/source/key checks but immediately before that final numeric assertion.
2. `tests/paper_full_eight_square_oracle.h` (or one new test-only header if size requires it): ordinary endpoint observer, canonical decimal formatter, evidence writer, and strict read-back validator. No symbol enters `include/` or production `src/`.
3. One small standard-library-only Python program, proposed name `tests/package_fs_residual_endpoint_evidence.py`, that strictly validates the ready TSV, compresses it, writes the external status record, and has a synthetic self-test. It performs no FHE/FFT/NTT or numerical-oracle work.
4. `.github/workflows/dcp-rcb.yml`: one deterministic self-test invocation, one failure-preserving CTest wrapper, and one always-run upload per host.

Do not add a CTest entry or change `CMakeLists.txt`. The existing paper executable remains `EXCLUDE_FROM_ALL`; CTest entry 61 keeps the same name, executable argv, order, timeout, serial property, and `OMP_NUM_THREADS=2`. The old 60 names/argv/order and all five API targets remain byte-for-byte/inventory-identical. Production `src/` and `include/` must remain byte-identical to `b1b024e…`.

The existing reusable points are exact: fresh encryption and production decode at `paper_full_eight_square_contract_test.cpp:264–275`, eight-stage loop at `284–301`, final production decode and independent `finalPolynomial` at `317–323`, semantic/lifecycle checks through `340–380`, and the effective E80 failure at `381`. The future observer reuses `freshPolynomial` and `finalPolynomial`; it creates no second key, encryption, DCP, multiplication, or chain.

## 2. Canonical uncompressed evidence bytes

The canonical object is an uncompressed TSV byte string. Compression is transport; both hashes are external.

- Encoding: UTF-8 restricted to ASCII; no BOM.
- Newlines: LF (`0x0a`) only, including exactly one final LF; CR is forbidden.
- Fields: separated by one TAB; no TAB/LF/CR inside a field; no blank lines.
- Metadata and row order are fixed below. Unknown or duplicate fields fail validation.
- Integers are unsigned base-10 without leading zeroes except `0`.
- Commit values are exactly 40 lowercase hexadecimal characters.
- Every residual is the binary768 result serialized as exactly 110 significant decimal digits using
  `^[+-][0-9]\.[0-9]{109}e[+-][0-9]{5}$`. Normalize every zero to `+0.` followed by 109 zeroes and `e+00000`; negative zero is forbidden. Reject nonfinite values. The fixed explicit sign preserves signed evidence; 110 significant digits exceeds the requested 100.

The lines are exactly:

```text
FS-RESIDUAL-ENDPOINT-01<TAB>v1
meta<TAB>source_commit<TAB><actual PAPER_SOURCE_COMMIT>
meta<TAB>baseline_tested_source<TAB>9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e
meta<TAB>production_source<TAB>b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89
meta<TAB>openfhe_pin<TAB>df495ba2e91739a6dc8f1de254fc5a41155ce504
meta<TAB>host<TAB><linux|windows>
meta<TAB>github_run_id<TAB><positive decimal>
meta<TAB>github_run_attempt<TAB><positive decimal>
meta<TAB>test_name<TAB>paper_full_eight_square_contract
meta<TAB>chain_count<TAB>1
meta<TAB>n<TAB>32768
meta<TAB>m<TAB>65536
meta<TAB>slots<TAB>16384
meta<TAB>gap<TAB>1
meta<TAB>input_formula<TAB>frozen-four-phase-exact-dyadic-v1
meta<TAB>residual_precision_bits<TAB>768
meta<TAB>significant_decimal_digits<TAB>110
meta<TAB>scale0_numerator<TAB>1267650600228229401496703205376
meta<TAB>scale0_denominator<TAB>1
meta<TAB>scale8_numerator<TAB><canonical positive decimal>
meta<TAB>scale8_denominator<TAB><canonical positive decimal>
meta<TAB>row_count<TAB>16384
meta<TAB>numeric_gate_failures<TAB><canonical nonnegative decimal>
meta<TAB>e80_disposition<TAB><PASS|FAIL>
meta<TAB>a_claim_disposition<TAB><value from the separately approved criterion, or NOT_ADOPTED>
meta<TAB>observer_disposition<TAB>PASS
slot<TAB>E0.real<TAB>E0.imag<TAB>E8.real<TAB>E8.imag
```

Exactly 16,384 data lines follow, with slots `0,1,…,16383` in strictly increasing order and no omissions or duplicates. Thus the file has exactly 16,411 lines. `E0` and `E8` retain their original-z meanings from `NEXT_TEST_SPEC.md`; no I/A values or near-unit x0 values substitute for them. The approved offline checker may reconstruct x0, x8, I8, A8, maxima, and signed maximizing tuples. Binary512 comparisons and summary maxima remain parseable log/status observations; they do not replace the binary768 all-slot rows.

`numeric_gate_failures` is the unchanged existing original-input E80 accumulator printed today at `paper_full_eight_square_contract_test.cpp:380–381`. Do not add an A claim to that counter or overwrite its history; the separately reviewed A disposition has its own field.

At runtime, validate `PAPER_SOURCE_COMMIT == GITHUB_SHA`; map only `RUNNER_OS=Linux|Windows` to `linux|windows`; require decimal `GITHUB_RUN_ID` and `GITHUB_RUN_ATTEMPT`. Do not accept caller-provided filename fragments. These identity fields, plus the fixed baseline/production/pin, must agree independently in the C++ writer and Python packer.

## 3. Filename, atomic publication, compression, and external hash

Use this exact stem:

```text
fs-residual-endpoint-01.v1.<source_commit>.<host>.<github_run_id>.<github_run_attempt>
```

The test writes `<stem>.canonical.tsv.partial` under a workflow-supplied, build-local `PAPER_EVIDENCE_DIR`. After all 16,384 rows are in memory and all oracle/nonfinite/map/shape/source/key/foreign/ownership checks have passed, `Run()` must:

1. write the complete partial file;
2. flush and close it, reopen it, and run the same strict structural/value/identity validation used by the self-test;
3. rename it to `<stem>.canonical.tsv` only if validation succeeds;
4. emit one `OBS evidence_ready=... rows=16384` marker;
5. execute the unchanged final `Require(numericFailures==0, ...)`.

This order makes a finite E80 miss durable while retaining the existing `COMPLETE ... FAIL` and nonzero process result. Fatal oracle/invariant failures before publication remain fail-fast and must not manufacture a ready file.

After the single CTest process returns, the Python packer validates the ready TSV again and creates `<stem>.tsv.gz.partial`. Compression is one RFC 1952 gzip member using DEFLATE level 9, `mtime=0`, empty original filename, no extra/comment fields, and OS byte 255; rename to `<stem>.tsv.gz` only after close and successful decompression/byte-for-byte comparison with the canonical TSV. Different zlib implementations need not produce equal compressed bytes; the decoded canonical bytes are the comparison authority.

The packer then writes `<stem>.status.json.partial` and atomically renames it to `<stem>.status.json`. Canonical status encoding is UTF-8 ASCII, no BOM, JSON object keys sorted lexicographically, separators `,` and `:` with no optional whitespace, no floats/NaN, and one final LF. Reject unknown or missing keys. The exact keys and types are:

```text
a_claim_disposition       string: NOT_ADOPTED|PASS|FAIL|UNRESOLVED
baseline_tested_source    string: fixed 9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e
canonical_tsv_bytes       nonnegative JSON integer
canonical_tsv_sha256      string: 64 lowercase hex
ctest_process_disposition string: PASS|FAIL (PASS iff exit code is zero)
ctest_process_exit_code   nonnegative JSON integer
e80_disposition           string: PASS|FAIL
github_run_attempt        positive JSON integer
github_run_id             positive JSON integer
gzip_bytes                nonnegative JSON integer
gzip_sha256               string: 64 lowercase hex
host                      string: linux|windows
numeric_gate_failures     nonnegative JSON integer
observer_disposition      string: PASS
openfhe_pin               string: fixed df495ba2e91739a6dc8f1de254fc5a41155ce504
production_source         string: fixed b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89
row_count                 JSON integer: 16384
schema                    string: FS-RESIDUAL-ENDPOINT-01/status-v1
sidecar_filename          string: <stem>.tsv.gz
source_commit             string: actual 40-lowercase-hex PAPER_SOURCE_COMMIT
status_filename           string: <stem>.status.json
test_name                 string: paper_full_eight_square_contract
```

Copy the dispositions/count only after matching the TSV. Compute `canonical_tsv_bytes`/SHA-256 over the exact uncompressed bytes and `gzip_bytes`/SHA-256 over the final gzip bytes.

The status file deliberately contains no hash or size of itself. The TSV deliberately contains no hash or size of itself. GitHub's upload receipt/digest and the later root download preflight bind the status file and outer artifact without recursion. No secret, key, ciphertext, CRT coefficient, or environment dump is included.

## 4. Failure-preserving workflow

Keep every existing step through the 60-test checkpoint and five API builds unchanged. Use `${{ github.workspace }}/build/paper-evidence` on Linux and `${{ env.PROJECT_BUILD }}\paper-evidence` on Windows as `PAPER_EVIDENCE_DIR`; do not fall back to the current directory or a user home. The paper run shell must disable immediate exit only around the existing single CTest invocation, capture its actual code, restore fail-fast behavior, run the packer once with expected source/host/run/attempt arguments, then return the original CTest code when packaging succeeds. If packaging fails, return nonzero. Do not use `continue-on-error`, do not execute the test binary a second time, and do not translate a failing CTest result to success.

Add a following `actions/upload-artifact@v4` step with `if: always()`, an artifact name containing runner OS, source SHA, run ID and attempt, an exact path list containing only `<stem>.tsv.gz` and `<stem>.status.json`, and `if-no-files-found: error`. A missing artifact is therefore an additional visible failure, not an inferred success. Record the upload action's artifact ID, URL, and digest in the eventual run evidence.

Acceptance must bind four independent facts: raw log has exactly one live paper `BEGIN`/chain and the final `COMPLETE` outcome; status records the same source/run/host and actual nonzero CTest code; decompressed TSV matches its external byte/hash fields and all 16,384 rows; the GitHub job remains failed when E80 remains failed. CTest's output-on-failure replay is not another chain.

## 5. TDD sequence

### RED — no new cryptographic execution

Freeze the schema, formatter examples (positive, negative, normalized zero, exponent padding), four synthetic rows, filename validation, gzip/status rules, and malformed cases before helper implementation. Add an explicit `--endpoint-observer-self-test` dispatch to the existing paper executable that reaches only deterministic constant/monomial/sparse-polynomial observer controls and evidence formatting—before any setup, key generation, encryption, or transform of ciphertext data. The first test patch references the absent test-local observer/evidence seam, so the paper target has an honest compile/link RED after the unchanged old-60/API checkpoints. Retain that hosted RED. Do not introduce a stub success, unsupported-exception expectation, new CTest entry, or paper chain.

### GREEN — observer-specific, not overall E80 GREEN

Implement only the test-local observer/evidence seam needed by the frozen RED. The self-test must pass without constructing OpenFHE contexts or keys. In the normal no-argument path, run the same deterministic helper controls before encryption, then perform exactly the existing one chain and endpoint observation. A trustworthy observer and valid retained artifact are the GREEN target for this cycle; the existing paper contract is still expected to be `FAIL` if E80 fails.

Hosted acceptance, on the exact candidate commit, requires: production `src/include` hashes unchanged; exact old-60 inventory/argv/order and five API build receipts unchanged; paper inventory still entry 61; deterministic self-test PASS; exactly one live cryptographic chain per host; observer controls and all-slot producer agreement reaching their separately frozen bounds; ready sidecar/status uploaded on both hosts; root's independent pure parser reproducing identities, hashes, rows and scalar decompositions; and the original E80 failure/count/`COMPLETE FAIL` retained if observed. No criterion may be changed after seeing that run.

## 6. Non-goals

No production patch, public API, new general instrumentation framework, intermediate-stage full-slot dump, second encryption/chain, favorable-key selection, repeat, 1,000-trial study, parameter/threshold adjustment, Mac compilation or numeric execution is proposed. This document does not assert that the observer, A claim, or paper implementation passes; it supplies an implementable evidence boundary for independent review before any freeze or hosted run.
