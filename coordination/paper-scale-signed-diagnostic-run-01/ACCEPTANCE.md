# Full signed-diagnostic run: evidence accepted, precision contract FAIL

2026-09-06 Asia/Shanghai. Run [33978202814](https://github.com/leemaple/20231788./actions/runs/33978202814), automatic push attempt 1, tested source `9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e`. Documentation baseline `a33d5f1fcfeb51cd63021b1ec1d24d6e76187a1c`. Production remains byte-identical to `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89`; pristine OpenFHE pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

## Observed outcome

Both hosted jobs completed with FAILURE: Linux 101338538686 at 2026-09-05T16:39:49Z, Windows 101338538587 at 16:41:43Z. Both compiled the paper target and executed one complete eight-square chain. Linux's earlier Boost warning/compile blocker is resolved by the test-only allocator-backed root temporaries on actual GCC 13.3.0. No production arithmetic changed.

Each host passed all 123 earlier accepted test invocations in groups [1,2,57,1,2,60]; these are repeated workflow groups, not 123 unique tests. Live inventories contain 57, 60 and 61 tests. Five public API targets, the three accepted integration targets and production library built. Each host's sole paper execution logged all 835 unique numeric fields, nine exact-scale receipts, final binder/decryption/full-slot/witness checks, wrong-scale and foreign rejection controls, and ownership cleanup. CTest's duplicate text is a replay, not a second experiment.

The unchanged numerical contract still has exactly seven misses on each host: round 4–8 independent end-to-end anchors, final full-slot E, and final independent E. The final `numericFailures==0` assertion is reached and throws; only `COMPLETE result=PASS` is not reached. Final full-slot component error is 8.6166359880052555e-24 on Linux and 8.4890276421661587e-24 on Windows, respectively 10.4169 and 10.2626 times 2^-80. Witness, codec disagreement and independent/production agreement gates pass. These passing controls do not cancel the failed precision gates.

## Independent review and root reproduction

GPT-5.6 Sol (standards/evidence) checked exact source, raw-log provenance, inventory/argv/result binding, build order, single-run/replay separation, numeric gate outcomes and lifecycle reachability. GPT-6 Astra (mathematics) independently recomputed signed identities and exact scale receipts from the original first streams. Root completely read both scripts and all audit reports, then actually re-executed both scripts for both hosts while HEAD was a33d5f1 and Git clean. All four exits were 0 and full stdout was byte-identical to the retained JSON. See ROOT_REEXECUTION.json for hashes and limits. These are independent contexts within Codex, not additional provider diversity or Fable attestations.

At every measured round/anchor, inherited initial error I exceeds added arithmetic error A: the minimum same-anchor ratio is 40.0009 on Linux and 34.3180 on Windows. This is a ten-anchor observation, not full-slot attribution or a universal correctness theorem. The observed coefficient/scale magnitudes make ordinary binary512 Horner rounding an implausible explanation at the measured scale, subject to the audit's explicit non-interval assumptions. No definite new production defect is proved. Full E remains referenced to original plaintext powers, never to fresh decryption.

The raw files preserve BOM and original trailing whitespace. Windows' tracked LF copy has an explicitly recorded original decoded CRLF hash. RAW_PREFLIGHT.json retains strict zero-finding gitleaks evidence on both original decoded logs. Original connector capture JSON is ignored local provenance, not HTTP byte attestation. The tracked scripts retain their original capture/location and frozen-HEAD guards; a later documentation HEAD must not silently weaken those guards.

## Decision and next action

Accept the new evidence and compiler-portability result; do NOT mark implementation or frozen precision acceptance complete. Preserve both historical failures, source and gates. Do not change thresholds, encryption distribution, scale, input, or keys to obtain a favorable run.

Next is a new independent ChatGPT Pro scientific adjudication of the frozen public-encryption noise/input profile and achievable end-to-end precision, separately from evidence of correct paper multiplication. Supply the full exact-source/paper/official-source/archive and both original hosted runs plus full audits. Ask for one technically justified next decision, with any versioned criterion proposal explicit and independently reviewable. Do not request 1,000 trials or repeat the same random experiment. Fable 5.1 remains unavailable after the verified balance failure; Codex and Pro continue without quota polling or ZCode dispatch.

This checkpoint changes only evidence/documentation. No Mac compile, FHE, NTT, FFT or codec execution; no CI dispatch/rerun, merge or source modification occurred. Later documentation pushes use [skip ci].
