# Initial independent source review

Review target: the exact six-file Pro candidate, patch SHA256 `efd57c459e3a843f05ac313b2cc1343ce75206bf5de0f19849f6fa2c771df3e6`, against engineering source `448e9d3067796b64656f0746f4be5c4153b1271d`. The intervening pre-application HEAD `901a9256dd3b495aefd93e26e49d3a5aa161fbb5` changes documentation only. After application, the observed diff command was `git diff 901a9256dd3b495aefd93e26e49d3a5aa161fbb5 -- src include`; all six applied files match the immutable returned manifest.

These are separate Standards and Spec review contexts, not two independent providers. Fable 5.1 remains unavailable after its definite balance failure. The Pro author self-review is not counted as an independent final semantic review.

## Standards

Selected reviewer: GPT-5.6 Sol, high; task `paper_pro_standards_review`.

No actionable Standards-axis finding. The reviewer read the complete six-file +475/-119 candidate, full changed translation units and headers, the frozen contract/build context and permitted pristine API snippets. The patch preserves the frozen tests/build/h128 adapter, uses fail-fast validation without broad catches, keeps secrets outside plan/evaluator/result state, revalidates live mutable OpenFHE context/basis/key-row boundaries, and limits the new abstractions to the fixed diagnostic and paper profiles. Its source-only `git diff --check 901a925 -- src include` passed.

Non-actionable judgment note: terminal constants recur in the I/O state validator and binder. The profile is deliberately fixed to eight rounds and two terminal towers; introducing a generic configuration solely to remove those literals would conflict with KISS/YAGNI without another supported profile.

No compile, tests, FHE/FFT/NTT, returned-script execution or CI/Windows validation was performed by this reviewer. Type/link portability and runtime mutation/ownership behavior remain pending hosted validation.

## Spec

Selected reviewer: GPT-6 Astra, high; task `paper_pro_spec_review`.

No source-proven P0/P1/P2 implementation defect identified. The reviewer read the full frozen test/oracle, paper section6.3/Table3, contract/audits, complete changed files and relevant pristine source. The following requirements have concrete source implementations:

- `MakeFamily` and `CreatePaperRepeatedMult2Setup` fix/check ordered Q/P/root identities, actual modes and partitions, nominal50, eight families and one h128 sampler call. Client-only key installation projects the same signed polynomial by full modulus/root/phi identities.
- The plan constructs exact rational scale receipts independently of recorded metadata; `Reenter` copies elements into the next family without decryption or refresh.
- `RCBWithReceipt` requires an owned terminal Rescaled receipt, recombines through the proper family, checks the root prefix and wraps unchanged elements at B0 absolute level9. The private result revalidates ownership, receipt, wrapper and actual basis.
- `BindRepeatedRcb`, bound cloning and public Poly* decryption enforce issuing-plan/live-state checks and final S8 normalization; heap-backed 160/220 transforms retain the pristine positive-forward/negative-inverse ordering.
- Original context-only N64/Q8 I/O remains restricted and the diagnostic setup remains two-family. Result/I/O/bound state keeps strong plan ownership, while public Data has no private key. The extra planned metadata guard does not contradict the diagnostic test's explicit empty-metadata assumption.

This clears source inspection for the prescribed hosted validation only. Full compilation, old regressions and the actual encrypted eight-square precision remain unresolved by source review. No execution or mutation was performed by this reviewer.

## Lead disposition

The lead read the entire patch and author review/findings/execution ledger, rechecked h128 caller compatibility, exact result wrapping/scale and transform changes, verified all resulting hashes and frozen paths, and found no concrete pre-run blocker. Preserve the existing API RED and publish this candidate for its first automatic hosted run; do not call it GREEN or a completed implementation yet.

Summary: Standards actionable findings0; Spec source-proven findings0. Both axes retain the unexecuted-production limitation. A separate fully briefed Pro final semantic review and exact-source dual-host execution remain required.
