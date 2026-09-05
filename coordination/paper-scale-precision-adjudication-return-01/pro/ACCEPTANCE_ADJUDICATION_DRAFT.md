# Acceptance adjudication draft — PS-CLAIMS-v1

**Status: DRAFT FOR INDEPENDENT REVIEW. Not adopted. No test/production patch is included.**

This document separates claims that the paper and evidence distinguish. It does not replace, weaken, or declare success under the existing `PRODUCTION_CONTRACT_01.md`. Citation aliases are defined in `SOURCE_MAP.md`; the reasoning and limitations are in `DECISION.md`.

## 1. Original contract and failure history remain immutable

The original contract requires fresh/full-terminal and per-round ten-anchor original-input component error ≤2^-80, with exact scales, independent oracle checks, the witness, immutable source/keys, binding/foreign rejection and cleanup. That remains **E80**, with no change to operands, norms, thresholds, input, sampler, sigma, scale, primes, chain or count of squarings. [PC:90–149]

Preserve the following records verbatim rather than relabeling them:

| Source/run | Original disposition |
|---|---|
| b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89 / 33971779479 | Linux: paper compile failure, no paper runtime. Windows: round-4 E80 failure, later client gates not reached. |
| 9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e / 33978202814 | Linux and Windows: one complete observed chain each; seven retained numeric misses each; final COMPLETE FAIL. |

No aggregated “implementation PASS” may hide these dispositions. Completion of the later observations is accepted as increased diagnostic coverage, not as E80 success. [SL:8022–8024; SW:8344–8346]

## 2. Claims and their present status

**C1 — Constructive paper-method source correspondence: SUPPORTED, limited.** The reviewed source follows DCP, Tensor2 with omitted low×low, high-lift-before-Relin2, RS2 recombination identity, and terminal RCB/exact-scale decoding. This is a source-review conclusion with regression support, not an unconditional formal proof or a security claim. Printed theorem normalization and nonwrap qualifications in DECISION §6 apply.

**C2 — Measured added arithmetic at fixed anchors: SUPPORTED for these recorded observations.** At ten fixed anchors and eight stages on each supplied new chain, signed I/A/L reconstruct consistently; A is small and I dominates in the same-anchor norm. This does not say “all slots,” “all keys,” or “original plaintext correct to 80 bits.”

**C3 — Original-input E80: FAIL on the supplied runs.** It remains the original requirement. It is neither accepted nor made pending merely because C1/C2 are supported. Satisfying the user's stronger original-input target in general remains unresolved under the unchanged profile.

**C4 — Full-slot terminal added-arithmetic A80: PENDING; separately proposed.** This is a proposed named engineering claim, not a replacement for C3. After independently reviewed, pre-frozen endpoint tests, the predicate would be

    A80_TERMINAL:
      max over all 16,384 slots and real/imaginary components of
      |x8 − x0^256| <= 2^-80,

where x0 and x8 come from the independent integer-polynomial/ordinary-DFT test path at exact S0 and S8, not from production Decrypt. Original z remains the reference for E0/E8. Oracle integrity and all frozen semantic/invariant checks are prerequisites. A pass establishes only terminal added-arithmetic precision for the observed chain/profile; intermediate full-slot A and all-key bounds are not included.

**C5 — Section 6.3 empirical precision reproduction: NOT CLAIMED.** The paper reports an average norm result and omits enough distribution/subtraction detail that it does not define the current deterministic per-stage test. This draft creates no statistical replication obligation. [PAPER:1562–1590; PC:147–149]

**C6 — Security level or universal nonwrap theorem: NOT ASSESSED / NOT ESTABLISHED.** HEStd_NotSet is not a security assertion, and measured agreement does not instantiate the paper's nonwrap antecedents. No new security-certification gate is added to this bounded engineering task.

## 3. Why the proposed criterion is not fitted to the observations

The *semantic split* comes from the paper's arithmetic on decrypted operands and the algebraic identity E=I+A. The number 80 is inherited from the existing engineering precision budget. It is **not** derived as a universal theorem from Table 3, nor selected just above the observed 1e-26 added errors. This distinction is mandatory in any resulting report. [PAPER:584–951; PC:147–149; ORACLE:153–179]

Calling C4 “80-bit added arithmetic” is meaningful precisely because it excludes inherited fresh error and says so. Calling C4 a pass for original-input 80-bit precision would be false. A80 can pass while E80 fails, and both outcomes must appear adjacent to one another. The report must still disclose fresh E, full terminal E, inherited I, added A, exact scales, original run IDs and signed maximizing-slot evidence.

The proposed C4 is optional as a named acceptance claim; the endpoint evidence is still useful as a characterization if independent reviewers decline to adopt the numerical A80 predicate. In either case C3 is untouched. This review recommends the bounded endpoint observation, not silently adopting a new green gate.

## 4. Adoption and execution boundary

Before any new encrypted execution, the available independent reviewers must assess this draft and `NEXT_TEST_SPEC.md`; Codex then freezes the exact test source, scalar controls, output schema and numerical comparisons. No decisions based on that future chain's results may change the generator, gates, key selection or required outputs.

The test remains the existing one-chain-per-host test. No replay is counted as a new experiment. There is no favorable-key, repeat-until-green, extra trial-count or 1,000-repetition acceptance branch. No new run is authorized or dispatched by this artifact itself.

The existing E80 final-failure logic remains effective. Additional oracle-integrity failures must also fail the run; additional claim reporting cannot mask any legacy numeric/invariant failure. A later C4 pass permits only the named C4 claim and the associated evidence statement. It does not rewrite a historical FAIL or approve the original precision-reproduction target.

## 5. Numerical, semantic and security consequences

Nothing in production or the cryptographic profile changes. The diagnostic distinguishes uncertainty already introduced by public encryption from subsequent arithmetic residuals. It does not claim to remove either error from the ciphertext.

Changing ephemeral sampling, sigma, encryption mode, input domain, S0, d, primes or chain length requires a separate contract/design review and a concrete causal RED regression. With fixed d·m, S0×2^b implies S8×2^(256b); an initial-scale adjustment is not a local metadata repair.

On successful endpoint coverage closure, stop this diagnostic workstream and publish the limited C1/C2/C4 evidence beside the retained C3 failure. On a new discrepancy, identify the exact slot/component/operation risk and design its minimal RED before considering production changes. There is no automatic escalation to a generic instrumentation or sampling project.
