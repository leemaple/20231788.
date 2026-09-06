# EXPERIMENTAL-PRECISION116-PROFILE-SEAM-01

State: ready to prepare exact-source handoff; **not yet submitted to Pro, implemented, compiled or tested**. This task follows the accepted experimental static certificate, not a passing encrypted profile.

## Objective

Add exactly one explicit factory, proposed name `CreateExperimentalPrecision116Setup()`, for the immutable `experimental-s116-d56-b58-v1` candidate. Prove the actual OpenFHE context/key/client/one-Mult2 seam works with this profile. Preserve `CreatePaperRepeatedMult2Setup()` and the existing no-argument paper contract unchanged and still honestly failing its old E80 criterion.

## Scope and constraints

Use a dedicated new clean-room worktree from the next committed static-certificate checkpoint; check existing worktrees/owners before creating it. No quarantined code or local modified OpenFHE. Same pinned official OpenFHE1.5.0, N32768, h128, dense ephemeral ternary, sigma3.19, public encryption, DCP/Tensor2/Relin2/RS2/RCB and original input semantics. No runtime k, public generic configuration, known-plaintext compensation, key selection, noise changes or relaxed E80 limit. Mark the new factory/profile experimental and security-unresolved.

Expected minimal production files: `include/openfhe_2023_1788/repeated_mult2.h`, `src/repeated_mult2.cpp`, `src/high_precision_client_io.cpp`, `src/double_ckks.cpp`. One private immutable descriptor should supply exact Q/roots, P, base58 and S0=2^116 to the existing paper-geometry path. Keep small diagnostic/context-only S100 behavior intact. No keypair-adapter change is currently justified. Validate actual P/root/QP/HYBRID tables, metadata F=2^58, fresh/RS recorded F^2 and tensor F^3, actual rational scale receipts and ownership/cleanup. Do not change endpoint writers/readers: their schema intentionally names the original profile.

## RED, GREEN and acceptance

1. Draft a separate mode on the existing paper-contract executable plus a separately named CTest. Calling the missing factory should give an honest API compile RED before production code is applied. Return this as a separable RED patch and execution instructions, not fabricated compiler output.
2. The test checks exact candidate moduli/roots/P, all eight ordered-family deletions, h128/key-row and no-alias ownership, base58 metadata, rejection of old S100 inputs, and actual Encrypt→DCP→one Mult2 input/output logical scales. It must explicitly report **profile seam only, E80 not tested**. Keep the original no-argument mode unchanged.
3. After root records actual hosted RED, apply only the cohesive GREEN patch. Build and run the new seam test on Linux and Windows using bounded CI/Windows execution, then relevant legacy regression/API checks. Reuse existing cache/build workflows where safe; no heavy Mac work. Do not run another full encrypted eight-square chain in this slice.
4. Independently review the changed interface, exact constants/receipt transitions and client/evaluator separation. Acceptance needs exact commit/run/host receipts and actual results; a Pro draft or green static certificate is not a build/test result.

## Handoff and continuity

Codex root owns the critical path. Pro is the preferred bounded code drafter: prepare one complete sanitized ZIP containing the exact current clean-room source/tests/build metadata, paper/official references needed for the six touched files, current accepted failure/review context, static candidate/certificate and this task. Follow workflow scanning, exclusion, manifest, commit/size/SHA gates, save the new conversation URL and submit once. Do not reuse the terminal scientific review as if it is still thinking or assumes these materials. If Pro is unavailable/terminal partial, retain its usable output and let Codex own the missing implementation with independent review; do not block on Fable balance.

Following a successful one-operation seam, decide the smallest discriminating original-input eight-square validation on the experimental profile. Original E80 and security boundaries remain distinct. No1,000-experiment batch or reporting subsystem is needed.
