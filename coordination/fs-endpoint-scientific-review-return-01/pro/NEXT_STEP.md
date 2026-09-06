# One next action: FS-PRECISION-CLAIM-SPLIT-01

**Owner:** integration lead. **State:** proposed, not applied. **Change class:** one documentation/report-contract slice. **No production patch and no encrypted rerun are authorized by this proposal.**

## Decision

Version the supported precision claim instead of rewriting Mult2 to compensate for an error already present at its input. Keep the current fixed public-encryption profile and its original E80 contract unchanged and visibly failing. Separately record the supported constructive correspondence and the two conditional realized-chain characterizations. Do not introduce a logical OR under which method correspondence or small A makes E80/project completion pass.

This is the selected minimal **explicit profile/claim separation**, not a claim to have found a parameter change that achieves E80. Under the unchanged combined requirement the full project remains unaccepted. `PROPOSED_PRECISION_BOUNDARY_V1.json` gives the exact proposed record; its `adoption_state` prevents it being mistaken for an integrated acceptance decision.

## Exact inputs

Use only the verified input identity in `DECISION.md`, source ed5fd192, production b1b024e3, official pin df495ba2, run34039088536/attempt1, both actual gzip/status files and retained logs. In this return they are under `evidence/project/coordination/fs-endpoint-live-run-01/`. Use the independently reproduced outputs `results/independent_linux.json` and `results/independent_windows.json`, binding their `bindings` values to those artifacts. The proposed profile continues to use the exact source-defined moduli/roots and S_r, not rounded scales from a report.

No replacement key, newly sampled chain, secret-state recovery, prior observer implementation or missing runner-primary-file bytes are inputs to this slice.

## Exact output

Integrate one versioned claim record and its short explanatory document under an integration-owned coordination directory, together with one deterministic report-contract regression. The record must have independent fields with the following meanings:

| Claim field | Required current value |
|---|---|
| constructive_method | SUPPORTED_TESTED_CORRESPONDENCE, not formal proof |
| realized_chain_endpoint | CONDITIONAL_CHARACTERIZATION_ONLY, two source-bound cases |
| original_input_E80 | FAIL |
| A_acceptance | NOT_ADOPTED |
| complete_implementation | NOT_ACCEPTED |
| replacement_numeric_profile | NONE_SELECTED |

The Linux case must retain nine misses and witness accuracy FAIL; Windows seven misses and witness accuracy PASS. Both retain CTest exit8 and observer/packer PASS. Preserve exact E/I/A identities and component norm. Do not edit an old status artifact to install this record.

The corresponding supported public claim is: “Constructive double-CKKS implementation with source-pinned bounded eight-square endpoint validation relative to the realized fresh plaintext, under the stated conditional observer model; the fixed public-encryption/original-input E80 profile has not passed.” It is not “80-bit end-to-end implementation completed.”

## TDD boundary and acceptance

The integration lead should first retain a minimal deterministic RED case for **conflation**: feed a report fixture with observer PASS, packer PASS and A below T, but the actual E80 FAIL/count/exit8; reject any proposed record claiming complete implementation or E80 PASS. A second assertion within the same test preserves the extra Linux witness failure. Then make only the smallest report-contract/document change necessary to emit the separated record. The test's GREEN means the record is honest, not that CTest61 passed. No such integration RED/GREEN is claimed by this review.

The review supplies an executable finite-data auditor and five passing unit/negative tests. They already check current identities, reject E80/A/count/exit-code rewrites, and reproduce the two full-slot decompositions. They are supporting regression specifications, not a replacement CI suite. Acceptance of the integration slice requires exact source/artifact binding, all original failures present, no unauthorized changes outside the report-contract scope, and fresh independent review of the resulting record.

Keep the existing 60 regressions and all five API targets untouched. Keep CTest61's no-argument path, registration, TIMEOUT1200, RUN_SERIAL and OMP_NUM_THREADS=2 (`project/CMakeLists.txt:266–271`). Do not change production src/include, distributions, exact scales, inputs, moduli, observers, packers or old artifacts. No general-purpose acceptance framework is needed.

## Precision boundary, not a hidden relaxation

For a given slot and component, if an inherited-error lower bound `L_I` and an added-residual upper bound `U_A` satisfy `L_I - U_A > T`, the old endpoint target is excluded for that captured endpoint. Both captures meet that condition. Even the ideal zero-added-error chain remains outside T because `L_I > T`.

For a genuinely different future profile, a sufficient endpoint budget would require `U_I + U_A <= T`; preserving all original per-stage predicates would also require the corresponding stage budgets. We have not proved those budgets for any replacement profile. A below T alone cannot establish them. The already observed fresh error must not be “corrected” in the evaluator using the known test plaintext.

Any later effort specifically to achieve original-input E80 therefore needs a separately versioned, coherent client/scale/modulus precision profile or a newly evidenced defect. That is **outside this one next slice**, not a request for another diagnostic chain now. In particular, initial-scale guard bits are not a valid isolated patch: with fixed d and m, their effect compounds as 2^(k*2^r). No new profile, input restriction, sampler substitution or acceptance threshold is silently adopted here.

## Done condition

This slice is done when the separated, source-bound claim record passes its deterministic anti-conflation regression and independent review, while the original E80 failures remain unchanged. It closes the ambiguity over what is and is not being delivered. It does not mark the user's still-unmet original end-to-end precision target complete.
