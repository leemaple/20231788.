# Independent review of public encoder successor fixes

Disposition: **accept F1–F4 source fixes and the scalar classification coverage fix for the next remote RED/GREEN gate**. No unresolved code correctness finding was identified within this bounded review. Full execution acceptance remains pending; this is not a cap certificate or permission to skip the remaining gates.

## Exact scope and evidence

Reviewed in `/Users/lifeng/Documents/20231788-openfhe-public-encoder-cap-20260909`, branch `codex/public-encoder-cap-20260909`, HEAD `32d813042580f7306f78ea27662192a3e27ff430`, with the root's uncommitted diagnostic additions. Reviewer: independent Codex context `public_encoder_candidate_review`; requested GPT-6 Astra/high, backend **requested-unverified**. No provider-diversity claim.

Re-read the current project workflow, `coordination/public-s100-encoder-cap-20260909/TASK.md`, the adopted theorem, and both prior independent reviews. Reviewed the six files below completely where newly authored, the CMake delta and its existing commit-variable definition, and textual differences against the immutable Pro candidate. No workflow/CI review is claimed. The thin codec definition is intentionally absent for the planned real remote link RED; that absence is expected and is not a new finding.

SHA256 values below identify the final reviewed snapshot. Implementation hashes remained unchanged during review. Root added two CLI regression tests for CRLF and missing final LF after the initial nine-test check; those additions were read and the final eleven-test suite was rerun independently. The scalar test's initial hash was `715a4734609016f4cc7f61932e34c9c43c10783b510a1b89d811b8f3849e3bd6`; the table records its updated hash.

| File | SHA256 |
| --- | --- |
| `diagnostics/certify_public_encoder.py` | `2dcf566fdc41d250e7dd72899431cd2d262ef4fc275f282a0a139295d0120be8` |
| `diagnostics/public_s100_encoding_dump.cpp` | `1e6c379ced9665057765c433fa5b2a4447a9e73a5034dc589988341d1d90a80f` |
| `include/openfhe_2023_1788/public_s100_encoding_probe.h` | `ecbb493c5a14f91788f7c99b4967d8830abbefe487dbd2fe1f1ecb3d11bdbc7c` |
| `tests/public_encoder_scalar_contract_test.py` | `17fea1a33d47633b73b0afb9161f1f4578ad0a1b68da6a6845aa405e4673d784` |
| `tests/public_encoder_transform_contract_test.py` | `beef8a059ffe4ff15d3412416a49c478aecb37378d1ce454269e5f3d75ef50ef` |
| `CMakeLists.txt` | `80ab789552d829e6e6e2f3e2c69b3b673fd588d4c72e863c504bf7d1147a6ade` |

## Finding disposition

**F1 — source correction accepted; real build evidence pending.** CMake line 364 now expands `${LOSSLESS_IO_SOURCE_COMMIT}`, the value populated by the existing `git rev-parse HEAD` at lines 111–115. Driver lines 35–37 require exactly forty lowercase hexadecimal characters before reaching either input preparation or the diagnostic API. The encoding-free `--metadata` branch at lines 38–43 reports that same checked identity and returns without a probe call. The original input construction and full probe call remain at lines 57–58. Empty, unknown and malformed labels cannot spend the full encoding under this control flow. Actual compiled metadata must still be checked against the exact remote source commit before full encoding; source inspection is not a configure/link/binary receipt. Correct metadata does not itself attest a clean tree, dependencies or historical equivalence.

**F2 — resolved for the documented Unix host contract.** `read_public_bytes` at verifier lines 235–249 opens the final path using `O_NOFOLLOW|O_NONBLOCK`, checks the opened descriptor with `fstat` before reading, and requests at most 12,000,001 bytes. It rejects over-limit content before decoding/JSON parsing. The descriptor check closes the path-check/open race for the ordinary-file requirement; a FIFO cannot block at open because NONBLOCK is present, and the descriptor is rejected before any read if nonregular. Context management closes accepted descriptors on normal and validation-error paths. The actual accepted byte sequence is retained for hashing. This intentionally targets Linux/macOS; it does not provide a Windows Python fallback, whole-path component attestation, or a snapshot against a concurrent writer. Those are not required by the root-controlled unique-output workflow.

**F3 — resolved.** Verifier lines 262–265 translate `RecursionError` only around decoding/parsing. This becomes `ValueError` and is handled by the existing documented rejection/exit-4 boundary. It does not catch transform recursion or mask an algorithm failure. The actual CLI checks for deep arrays, duplicate fields, truncated JSON and invalid UTF-8 all passed independently, returned exit 4, emitted the rejection prefix without a traceback and created no certificate.

**F4 — resolved in the test oracle.** `assert_sqrt_enclosure` at transform-test lines 16–19 now requires `a<=L` and `b>=U`, where `L=10+5*t/D`, `U=10+5*(t+1)/D`, and `t=floor(sqrt(2D^2))`. Thus `[a,b]` contains an independently rigorous enclosure of the exact algebraic target. The former false point `[U,U]` is now rejected by an independently run scalar test. The same helper is called by the nontrivial transform model. Exactly four tiny transform invocations remain; none were executed in this review.

**Three-way boundary coverage — resolved.** The shared `classify_cap` at verifier lines 143–149 is used by the real transform at line 186. It preserves exactly `upper*16384<=16129*UNIT`, `lower*16384>16129*UNIT`, otherwise INCONCLUSIVE. The scalar checks exercise below, equal, above and straddling intervals. They passed without invoking a transform.

## Mathematical and execution boundary preservation

The immutable-candidate comparison shows no change to integer interval primitives, reciprocal-arctangent/Machin pi enclosure, Taylor polynomials/remainders, seed recurrence, coefficient twist, bit reversal, positive-exponent DIT, squared-modulus aggregation or frozen coefficients/metadata schema. The only transform-body change calls the algebraically equivalent classification helper. The earlier mathematical review therefore continues to apply to this source domain: all odd roots including conjugates are enclosed, forward polynomial evaluation has no inverse normalization, and the cap is exactly `(127/128)^2`, not the distinct fresh-phase `255/256` hypothesis.

The C++ delta adds checked metadata and a metadata-only return; it introduces no setup/context/key/sampler/encryption/decryption operation. The header is byte-identical to the reviewed candidate. The diagnostic target remains OFF, EXCLUDE_FROM_ALL, and absent from default CTest registration. A later GREEN thin-function addition must be checked against the already reviewed candidate and leave `ComputeEncoding` and the original input functions unchanged; it is not yet present in this snapshot.

The root-controlled output-path assumption from the prior review still applies to the C++ existence-check/file-open sequence. The bounded reader rejects final-component symlinks and nonregular descriptors, but it does not authenticate source provenance. Result fields correctly retain a separate root provenance gate, null historical pairing and `UNCHANGED_FAIL` for historical E80. A later result concerns the exact public polynomial whose bytes are hashed; adoption into the theorem additionally requires the specified source/path contract, and historical applicability requires separate numerical-source equivalence.

## Independently observed check

After reading both test modules and their import behavior, executed only:

```text
python3 -B -I tests/public_encoder_scalar_contract_test.py -v
```

Working directory was the exact successor worktree above. The initial nine-test run returned exit 0 in approximately 0.79 seconds (`Ran 9 tests in 0.638s`, `OK`). After the two read and reviewed regression-test additions, the final run returned exit 0 in approximately 0.90 seconds. Final captured output:

```text
test_cap_classification_below_equal_above_and_straddling (__main__.AlgebraicOracleBoundary) ... ok
test_false_point_above_sqrt_is_not_a_containment_certificate (__main__.AlgebraicOracleBoundary) ... ok
test_crlf_is_rejected (__main__.PublicEncoderParserContract) ... ok
test_deep_json_is_controlled_rejection (__main__.PublicEncoderParserContract) ... ok
test_duplicate_fields_are_rejected_by_actual_decoder (__main__.PublicEncoderParserContract) ... ok
test_invalid_utf8_is_rejected (__main__.PublicEncoderParserContract) ... ok
test_missing_final_lf_is_rejected (__main__.PublicEncoderParserContract) ... ok
test_truncated_json_is_rejected (__main__.PublicEncoderParserContract) ... ok
test_directory_is_not_regular_input (__main__.PublicFileBoundary) ... ok
test_exact_limit_is_read_but_one_more_byte_is_rejected (__main__.PublicFileBoundary) ... ok
test_symlink_is_not_accepted_as_regular_input (__main__.PublicFileBoundary) ... ok

----------------------------------------------------------------------
Ran 11 tests in 0.826s

OK
```

The module import creates constants/classes/functions; importing the transform-test file does not run its unittest main block or test methods. Its only invoked helper in this scalar run uses integer square root and rational comparisons. CLI subprocess inputs are all rejected before `canonical_bounds`. Thus this observed check contains no transform, C++/FFT/NTT/FHE, key generation, sampling or encryption. The reviewer did not recreate root's earlier RED stages; those remain root-reported evidence to retain and reconcile, not observations from this review.

## Remaining gates

No unresolved F1–F4 source finding remains. Still pending: actual remote missing-definition link RED, reviewed thin-definition GREEN, compiled valid/invalid metadata checks and original empty-input API check, the four tiny transform results, one original full encoding, exact provenance reconciliation, and one full interval certificate. This report does not certify that any of those occurred. No candidate files, Git state, workflows or immutable Pro artifacts were changed by the reviewer; this report is the only owned edit. Original S100 E80 FAIL and the separate full-paper reproduction gaps remain unchanged.
