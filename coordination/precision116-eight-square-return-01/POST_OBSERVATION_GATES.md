# Minimal post-observation gates

Planning state: run `34055816234`, attempt 1, was progressing at exact source `2b8b349edf5575556347082c1b725f6696c743b6` when requested. I did not poll it. This is a conditional plan, not a result. Reviewer is a separate Codex context with unattested backend identity, not provider-diverse evidence.

## Closed evidence

- The current authority requires bounded correctness, not 1,000 trials: `coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md`, SHA-256 `7c252d3d8693ce98fa4af5f1fe11a00dc9952663262c3e43906710ab2ba273cd`.
- The original S100 profile remains dual-host E80 **FAIL** at run `34039088536`: `coordination/fs-endpoint-live-run-01/ACCEPTANCE.md`, SHA-256 `e99b26f5421be3fa205cbd688836ae3902ef5cf74dd8f46ceefa8f5c987ba655`.
- The exact S116 candidate's primes, roots, family deletion, scales, native capacity, and metadata have an accepted static certificate: `coordination/fs-precision-profile-feasibility-01/STATIC_ACCEPTANCE.md`, SHA-256 `13726eeb8a77da30222bcf01200de16ec6925fcaa003067ece7760a6d6f56d0e`.
- Run `34051183115` established dual-host one-operation integration at production source `2759fa90840946ef42957c7ba71ebea47e0e4995`: `coordination/precision116-pro-return-01/FINAL_RUNTIME_REVIEW.md`, SHA-256 `ea7906455dfadf9ae195538fba2eadb2a0fa47c3fcc7f154b6c1dbd0f3888785`.
- The active eight-square test SHA-256 is `478954a08580d3d342a47ad1e54900f4dce455af1964a3d3e2d231cf61b748d2`; its numerical review is `coordination/precision116-eight-square-return-01/NUMERICAL_REVIEW.md`, SHA-256 `2be8b125ce19dcb25ba2e579ad5838dc4bb5f7ad3e3da483c130b7fbd91f820c`.

## If both hosts yield a valid PASS

Only three closure gates remain:

1. Retain and hash both raw logs plus final status, binding source `2b8b349...`, attempt 1, pristine OpenFHE `df495ba2e91739a6dc8f1de254fc5a41155ce504`, native64/backend4, and unchanged test bytes. Each host must show prerequisites PASS, exactly one candidate/frozen-input chain, all eight squares and structural/oracle checks reached, zero numeric misses, cleanup PASS, and CTest PASS.
2. Perform one bounded source/log reconciliation of scales, errors, receipt count, wrong-scale witness, chain count, cleanup, and result classification. No new cryptographic run is needed.
3. Record the parameter-deviation disposition: accept `experimental-s116-d56-b58-v1` only for the tested N32768/h128/full-16384/eight-no-refresh/E80 domain. Preserve its experimental name, the original S100 FAIL, and “not exact Table 3 replication.”

Recommended next task: `PRECISION116-EIGHT-SQUARE-RUNTIME-ADJUDICATION-01`, combining those three documentation/evidence gates. A valid dual-host PASS can then close the correctness-focused implementation with stated limits. Ten intermediate anchors do not prove all-slot intermediate nonwrap, and one chain per host is not an all-key theorem; neither is a new completion gate. No extra trial, endpoint protocol, benchmark, or 1,000-run batch is required.

## If either host yields a valid finite FAIL

A valid numerical FAIL must still complete all eight operations, structural/oracle checks and cleanup, then emit `result=FAIL`. Build/setup/timeout/nonfinite/oracle/state failures are `INVALID_OR_INCOMPLETE`. Diagnose the concrete cause before repairing it; it may be a harness, implementation or parameter defect and must not automatically be dismissed as a harness problem.

Retain a valid FAIL without retrying for another key or changing input/noise/gates. Classify whether fresh already fails or the first miss appears later. Then perform one test-only `PRECISION116-FAILURE-LOCALIZATION-01` slice: emit signed actual/ideal/inherited/added values at the first failing anchor and, for an off-anchor final maximum, at that exact slot using the already available fresh/final decoded values. Only an established production defect gets a minimal RED/fix; an insufficient profile margin requires a separately named, statically certified candidate. Do not overwrite either S116 or original results.

## Security and authority

Security remains **UNRESOLVED**: QP exposure is about 712 bits at N32768/h128 with `HEStd_NotSet`; see `coordination/fs-precision-profile-feasibility-01/FEASIBILITY_REVIEWS.md`, SHA-256 `8a8f6473bee72401e1adc78e2b7b45cc682825640f7e704648140024a7b2a2f0`. This is not a bounded-correctness blocker.

Existing delegated technical authority covers the steps above. Replacing the original profile/E80 gate or beginning a new statistical/formal certification project would expand the accepted scope. Claims of exact Table 3 reproduction or 128-bit/deployment security require supporting evidence, not merely permission. Do not make the experimental profile an unlabeled default. None of those expansions is on the minimal path.

Root intake: verified all six referenced evidence hashes, corrected the transcribed numerical-review hash, and clarified that invalid execution can indicate a production/parameter defect rather than only a harness issue. This is a conditional closure plan, not current acceptance.
