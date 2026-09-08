# Independent root coverage baseline for the forthcoming atlas

Source-only inspection on2026-09-08, while browser Pro's document is still being authored. This checklist is not the final atlas or a numerical test result. It is deliberately not sent into the author's active thought.

## Parameter API coverage baseline

Root inspected fixed OpenFHE1.5.0 `src/pke/include/scheme/gen-cryptocontext-params.h` at commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`. A bounded Python standard-library lexical extraction of lines matching `^\s*virtual\s+void\s+(Set\w+)\(` found **32 base Params setter declarations**, lines368–461, excluding the commented examples. They are:

```text
SetPlaintextModulus             368
SetDigitSize                    371
SetStandardDeviation            374
SetSecretKeyDist                377
SetMaxRelinSkDeg                 380
SetPREMode                      383
SetMultipartyMode               386
SetExecutionMode                389
SetDecryptionNoiseMode          392
SetNoiseEstimate                395
SetDesiredPrecision             398
SetStatisticalSecurity          401
SetNumAdversarialQueries         404
SetThresholdNumOfParties         407
SetKeySwitchTechnique           410
SetScalingTechnique             413
SetBatchSize                    416
SetFirstModSize                 419
SetNumLargeDigits               422
SetMultiplicativeDepth          425
SetScalingModSize               428
SetSecurityLevel                431
SetRingDim                      434
SetEvalAddCount                 437
SetKeySwitchCount               440
SetEncryptionTechnique          443
SetMultiplicationTechnique      446
SetPRENumHops                   449
SetInteractiveBootCompressionLevel 452
SetCompositeDegree              455
SetRegisterWordSize             458
SetCKKSDataType                 461
```

This is a checkable API inventory, **not** proof that all32 apply to CKKS or that the project uses their values. The final atlas must match each against CKKS-specific disabled setters, upstream defaults/parameter generation, and the project's actual manual context path. Constructor-only, derived, compile-time, scale/basis, key-family, runtime-metadata and diagnostic parameters require additional entries; a complete32-row setter table alone is insufficient.

Pinned source link: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/gen-cryptocontext-params.h#L368

## Root-read randomness anchors for review, not new runtime claims

Root read the complete `src/core/include/math/distributiongenerator.h`, `src/core/lib/math/distributiongenerator.cpp` and `src/core/lib/utils/prng/blake2engine.cpp` from the newly hash-verified official mirror, plus targeted ternary/Gaussian/PKE ranges. The independent randomness worker separately traces the full consumer closure. Require the returned document to distinguish:

- Built-in BLAKE2Xb generation, a seed array and counter, from `std::random_device` seed acquisition plus the fallback-derived contribution. Comments about entropy/security are not independently demonstrated guarantees.
- `WITH_OPENMP`/`FIXED_SEED` preprocessing branches and thread-local/threadprivate state. The external PRNG loader's Linux/GCC-only branch is not a cross-platform API guarantee. The atlas must not infer exact compiled branches solely from defaults or claim executed sampler behavior.
- Fixed-seed debugging from production randomness; thread/scheduling/library implementation may matter for reproducibility. No actual seed is required or retained.
- Gaussian `SetStd`/threshold/Initialize and the active small-sigma inversion path versus other available sampling algorithms. A list of algorithm names without the actual call branch is insufficient.
- Sparse secret-weight selection versus public-encryption temporary `v`: upstream non-GAUSSIAN public `EncryptZeroCore` constructs the latter without an explicit h. Defaults and the project's h128 adapter must be reconciled, not equated by naming.
- Small signed polynomial sampling before RNS projection versus independent uniform residue sampling; representation and shared-root secret reuse must be source-bound.

These are review obligations. Any new defect hypothesis must be stated separately with exact reachability and evidence; no upstream sampler or project algorithm was executed or changed here.

## Acceptance checklist when Pro returns

1. Retain/hash/scan the original returned archive; do not execute supplied code before inspection.
2. Check the main document, parameter dictionary, randomness paths, change-impact matrix and source-coverage records all exist and are substantive.
3. Reconcile this32-setter inventory and the independent project/RNG maps with the author's coverage; explicitly classify inactive/disabled/default-versus-overridden fields.
4. Check source lines and constant/scale tables against fixed source, not against model agreement. Distinguish exact logical scale from compatibility metadata.
5. Ensure every proposed parameter change lists affected contexts/keys/precomputations/ciphertexts/oracles/tests and which historical evidence becomes inapplicable.
6. Preserve three distinct scientific results and the original HEaaN unknowns. Do not require unavailable HEaaN information to finish accessible-source documentation.
7. Finish the reference document before choosing a new diagnostic slice. No FHE run, source fix, author contact or new timer is implied by an atlas finding.
8. Distinguish the family context's Q (which retains Div as its final tower for the lifted relinearization path) from the active pair's Q (from which initial DCP already removed Div). For family f, sizes are family `11-f`, input pair `10-f`, output pair `9-f`; the terminal pair/RCB has only the two Base towers. An unqualified statement that “RS2 retains Div” is misleading unless it explicitly refers to the next family template, not an active pair tower. Reentry is rebinding to the next validated family, not an extra arithmetic DCP/error refresh.
