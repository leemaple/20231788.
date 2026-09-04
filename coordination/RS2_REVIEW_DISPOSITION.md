# RS2 independent review disposition

Observed 2026-09-04, Asia/Shanghai. This is an evidence ledger, not final sign-off.

## Exact completed independent review

Local ZCode task **Static review per LOCAL-REVIEW-TASK.md**, visible GLM-5.3 / Max,
completed after 14m27s at about12:02 CST. Native UI showed `Worked for`, final
`PASS_WITH_GAPS`, the two output paths and no running Stop button. It was a static
review, not a local build. Windows transfer had succeeded, but Windows task text
was not delivered; this is explicitly the authorized local ZCode fallback seat.

Exact return retained under `coordination/returns/rs2-zcode-a801e2c/`.
REVIEW.md SHA-256 `82c4be20e96a7d902a48cee91011b426c979c4a6646030076bc27143c877f951`.
Codex re-hashed all30 source-manifest entries after review: every length/hash still
matched. Return directory Gitleaks8.30.1 scan found no secrets.

## Finding reconciliation

| Item | Disposition and evidence | Owner |
| --- | --- | --- |
| F1: RS2/Tensor2 terminal rejection coverage | Closed for the requested cases by `b5f3d9f5848d7b39e8c3861e9a10d37a943e8d08`: repeated RS2 and terminal left/right/both Tensor2, fixture keys removed. Run33836142693 completed success: Linux43/43 (0.60s), Windows43/43 (0.97s), warning/API builds passed. Full result sections: `artifacts/tdd/rs2-terminal-rejections/green.txt`. This is coverage of existing behavior, not a newly observed production red-green cycle. | Codex |
| F2: untouched nonzero pipeline | Closed by `7041a489ae1afa98b75322ec334543f29f10b738`, run33834861766. Both warning/API builds and41/41 tests passed: Linux0.56s, Windows1.24s. New case runs HYBRID and BV0 genuine complex pipelines without coefficient replacement and without an evaluation key during RS2/RCB. Full result sections in `artifacts/tdd/rs2-public-pipeline/green.txt`. This is RS2 coefficient/state evidence, not decoded product accuracy. | Codex |
| F3: deep parameter/format immutability | Coverage extension authored: snapshot aggregate/declared/actual parameter identities and scalar values, individual native formats and every native value by value for all existing RS2 ciphertext/pair snapshots. Hosted execution pending. This does not claim an observed production mutation or cover every context-internal field. | Codex |
| F4: shallow key-cache comparison | Coverage extension authored for genuine retained relinearization keys: require both own and unrelated key tags, then check every cache row, object context/tag, and all A/B polynomial parameters/formats/values before/after RS2 and RCB. Hosted execution pending. The separate keyless pipeline remains intact; arbitrary malformed cache objects are not added to this fixture. | Codex |
| F5: missing ModReduce dispatcher source | Additional pinned official source inspected below; static definition gap closed, numerical accuracy claims remain separate. | Codex |
| F6: only first element/tower corrupted | Accurately scoped partial fixture coverage; no claim that every corruption position was executed. | Codex |
| F7: dotted repository URL | Already verified from actual fetch/push and CI; no change. | Codex |
| F8: minimal tower boundary | Existing deliberate boundary retained; no automatic relaxation. | Codex |
| F9: prime-role mutation evidence | Added deterministic positive/negative half-boundary coefficients for the smaller of q_div and q_l, with an independent incorrect-q_div quotient comparison required to differ in an actual output residue. Hosted execution pending. This is mathematical witness coverage, not a metadata-swap test or an executed production mutation; production mutation execution remains unclaimed. | Codex |

## Codex disagreement: declared versus actual RNS basis

ZCode's blanket statement that everything rejectable is rejected before arithmetic
is too strong. `ValidateCiphertext` checks actual tower moduli/roots/order/format,
but does not compare each DCRT element's declared aggregate parameter basis to
the actual active basis. A test-only DCRT object can declare the full4-tower basis
while retaining the genuine3 active native towers, without changing shared context
or tower parameters.

New public RS2 red test committed as `f8e976099f9bff5d1597b48c978c556e00c07652`.
Run33835497108, Linux job100907050619: warning/API builds passed;41/42 tests passed;
only `rs2_declared_basis_mismatch` failed with `RS2 invalid input did not fail fast`.
The malformed high-member input was accepted. This is a demonstrated validation
defect, not merely a coverage gap. Windows job100907050796 subsequently confirmed
the same failure:41/42, CTest exit8,1.12s. Both red logs are retained in
`artifacts/tdd/rs2-declared-basis/`. The red loop stops on the high member, so no
independent low-member red is claimed. Do not treat the earlier PASS_WITH_GAPS as
acceptance of this defect.

Minimal fix `68d0d985d3c17b7b3d1095f9142e38c0937544ad` constructs the expected
active prefix by copying pristine full parameters and dropping only the consumed
parameter suffix, then compares each declared parameter basis before arithmetic.
The prior per-tower diagnostics are preserved. Run33835813969 completed success
on both platforms: Linux42/42 (0.46s), Windows42/42 (1.06s), warning and API builds
passed. Full green sections are retained alongside the red logs. Both high/low
malformed members are now exercised in green. This does not claim exhaustive null-child
parameter corruption or deep immutability coverage. Source review follow-up is
still required for this validation addition.

The external review is preserved exactly, including minor inaccuracies: its scale
table reuses p for both30 and2^30, so the RS2 recorded scale must be read as
S^3/S=S^2 with S=2^30, not S^3/2^S. Its boundary list does not provide a dedicated
q_div-centered quotient witness. These wording/coverage issues do not change the
already tested two-rescale arithmetic.

## Additional official source for F5

All references pinned to pristine commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`; read from official GitHub blob API,
not a local OpenFHE checkout.

- [base-scheme.cpp](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-scheme.cpp#L94), blob `8e380bebd9dcab6d60ff70e49ccedc834f6f5c85`, lines94–99: SchemeBase ModReduce checks LeveledSHE enablement, calls the configured leveled-SHE object's ModReduce, restores input key tag, and returns the result.
- [rns-leveledshe.cpp](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-leveledshe.cpp#L311), blob `51d13290be8ae845915bb1d30326b06f8fcb7723`, lines311–320: LeveledSHERNS ModReduce clones the input then calls ModReduceInPlace; for FIXEDMANUAL this calls the virtual ModReduceInternalInPlace with the requested levels.
- [ckksrns-leveledshe.h](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/ckksrns-leveledshe.h#L48), blob `52bf074b2ee43ea1a300c715f108fd775edf32e4`: LeveledSHECKKSRNS derives from LeveledSHERNS. Its already-supplied internal implementation performs the native-tower drop, degree/level changes and GetModReduceFactor recorded-scale update.

These definitions remove the missing wrapper-source inference; the exact runtime
CRT/metadata tests remain necessary evidence for the exercised parameter regimes.
