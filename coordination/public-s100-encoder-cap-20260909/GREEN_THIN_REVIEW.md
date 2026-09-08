# Independent GREEN thin-definition review

Disposition: **accept the exact thin-definition source delta for the remote GREEN gate**. No new correctness finding in this bounded source review. This does not claim a successful GREEN build, execution of the public encoder, or certification of the canonical cap.

## Reviewed identity and scope

Worktree: `/Users/lifeng/Documents/20231788-openfhe-public-encoder-cap-20260909`; observed HEAD `a9d461aa458678387bd36ad0afd78dd42f036d22`, with only `src/high_precision_client_io.cpp` modified at review start. The root's changes were preserved. Reviewer is the independent Codex context `public_encoder_candidate_review`; requested selector GPT-6 Astra/high, backend **requested-unverified**. Provider diversity is not asserted.

Re-read the current workflow and TASK.md. Reused the prior independent interval-mathematics and F1–F4 fix reviews without rerunning their tests or reopening the adopted theorem. Reviewed this source delta, actual `ContextBinding`/`ComputeEncoding`/`FreshExactScale` call path, transform-table initialization and the original input header's geometry, eleven ordered moduli/roots and public input construction. No other implementation or modified OpenFHE tree was inspected.

Observed SHA256 of the complete successor `src/high_precision_client_io.cpp`:

```text
85b2b03954262344544dca96f84f8aa06044fb0fc74ff13f1a2317b67d666ac6
```

`cmp src/high_precision_client_io.cpp coordination/initial-lift-nonwrap-20260909/pro/candidate/full_files/src/high_precision_client_io.cpp` returned exit 0. The entire file therefore matches the originally reviewed immutable Pro full-file candidate, not merely the new function's name or a copied source label. The diff against production `a4b815a733efe81897325e2a8e4c826a4ebfa439` contains exactly the diagnostic header include and lines 759–777 defining the thin namespace/function. No existing algorithm body changed. `git diff --quiet` against that production commit for `tests/paper_full_eight_square_oracle.h` returned exit 0.

## Source conclusions

1. The size check at `src/high_precision_client_io.cpp:761–762` throws `std::invalid_argument` before the value binding, scale object or either transform table is constructed. In particular, the driver's planned empty-input API negative cannot reach any table or `ComputeEncoding` call through this function.
2. `ContextBinding binding{}` at line 765 value-initializes the aggregate defined at lines 25–44. Its context, crypto-parameter and plan members are empty shared pointers, not newly constructed contexts, parameters or plans. The diagnostic fills only geometry and fullBasis. It does not call `BindContext`, `ValidatePlan`, a factory, a key constructor or any evaluator method, and it does not return this dummy binding as a production receipt.
3. Geometry `{slots=16384,n=32768,m=65536,gap=1}` matches the frozen full S100 packing. All eleven moduli and all eleven roots at lines 768–769 were checked in order against `tests/paper_full_eight_square_oracle.h:35–45`, including final modulus 1099510054913 and final root 121567553. No S116 or annulus constants enter the definition.
4. The exact fresh scale is `(1<<100)/1` at line 770. With the null plan, `FreshExactScale()` takes its unchanged branch at lines 504–505 and returns the same rational. Thus `ComputeEncoding`'s unchanged scale check at lines 472–474 succeeds on precisely the intended scale without dereferencing a context or plan. The null-plan size diagnostic string inside `ComputeEncoding` remains irrelevant here because the thin function has already enforced the exact size and supplies the matching spec.
5. The primary/check tables retain the production decimal precisions 160/220 and deterministic table construction. The function makes one call to existing `ComputeEncoding` at line 773. That unchanged function performs its original two inverse transforms, stable integer rounding, full-modulus coefficient guard and exact upstream residue conversion at lines 463–496. It returns the exact pre-residue signed coefficients at lines 774–775; no extra rounding, recentering, scale conversion, projection filtering or coefficient substitution was introduced.
6. The original public slot formula and `ClientInputs(Inputs())` conversion in the original header are unchanged. This thin API intentionally accepts any correctly shaped public vector; the already reviewed standalone driver selects the ORIGINAL fixed input. That source path plus a root execution/provenance receipt, not the probe API name alone, binds a future output to the original input.
7. No context creation, PRNG, sampling, key generation, encryption, decryption, NTT, DCP, Tensor, Relin, RS or RCB call exists on this diagnostic path. The unchanged BigVector residue construction and upstream lazy PRNG definitions were addressed in the original independent candidate review and need no new upstream-source inference for this byte-identical implementation. Table construction and two inverse transforms are real numerical work on a valid call and must remain remote.

These are source-inferred control-flow/count conclusions. A literal `crypto_calls=0` or `encoding_calls=1` in later output is not independent runtime instrumentation. The real call count depends on the root launching the reviewed binary according to the fixed one-shot contract.

## RED evidence directly read

Read the retained artifact file `artifacts/public-s100-encoder-cap-red-34264640912-1/red-link-result.txt` directly:

```text
build_exit=2
expected_missing_symbol=InspectFixedS100PublicEncoding
transforms=0
encodings=0
crypto_calls=0
```

Also directly read `coordination/public-s100-encoder-cap-20260909/red-evidence/red-link-build.log.txt`; observed SHA256 `e47bd2e5326f05f0be464d9ce83b3f29df6ec407cf9ada286977950e8f0e28f7`. The saved log shows all four library translation units compiled, the static library linked successfully, the driver compiled, and executable linking then failed with two actual undefined references to `openfhe_2023_1788::client_io::diagnostic::InspectFixedS100PublicEncoding(...)`, from driver lines 58 and 46. It concludes with `collect2: error: ld returned 1 exit status` and the expected propagated make Error 2. This is a discriminating missing-definition RED, not a generic environment failure.

The retained `provenance.txt` records project SHA `230f59ed71fca5393041c0ba44227d354099e440`, tag `public-s100-encoder-cap-red-20260909`, stage red, run 34264640912, attempt 1, and official OpenFHE SHA `df495ba2e91739a6dc8f1de254fc5a41155ce504`. These are directly observed saved evidence files, not a build newly launched or supervised by this reviewer. Root owns transport/source/artifact reconciliation. The zero-call summary remains a recorded execution claim supported by this build-only failure path; no instrumented cryptographic/runtime trace is inferred from literal counters.

## Remaining runtime and provenance gates

- Preserve root's run/source/artifact reconciliation for the directly reviewed real missing-definition RED. The required error mechanism is evidenced by the saved raw log above; no repeat RED is called for by this review.
- Build the exact reviewed GREEN source remotely and retain complete compiler/linker receipts. Check that compiled `--metadata` is forty lowercase hexadecimal characters and equals the actual GREEN source commit; invalid metadata must fail before encoding. Run the one empty-input API negative with no tables/encoding.
- Run the four corrected tiny interval-transform models remotely; their previous source/scalar review is not a transform-test result.
- On the reviewed binary/source/input/dependency path, execute the original full public encoding once. Retain exact public JSON and its hash, geometry/scale/basis, compiler/Boost/OpenFHE source/build identities and zero-crypto call-path evidence.
- After the source/input/provenance and parser gates pass, perform one full outward-rounded canonical transform on those exact bytes. Retain both rational endpoints and CERTIFIED/REFUTED/INCONCLUSIVE outcome, plus failures if any. Do not change parameters, input, threshold, noise or samples, and do not retry the full operation on failure.
- Apply any resulting cap certificate only to its exact public polynomial and the adopted source/semantics contract. Historical applicability additionally requires numerical-source equivalence. Original S100 E80 FAIL, Table 3 reproduction, HEaaN provenance and security remain separate unresolved matters.

No C++ build/run, encoding, FFT/NTT, scalar test, sampling/FHE, CI/browser, commit/push/tag or external dispatch was performed by this reviewer. Only read-only identity/diff/hash/source/artifact checks and this report were performed. This report is the only reviewer-owned edit.
