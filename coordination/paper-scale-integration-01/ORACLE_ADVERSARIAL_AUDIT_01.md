# Independent adversarial oracle audit 01

Date: 2026-09-05. Reviewer selection: GPT-6 Astra, high, independent Codex review context. This takes over the adversarial responsibility after Fable 5.1's reported definitive insufficient-balance failure; it does not restore provider diversity.

Inspected clean-room HEAD: `71864006ee1fc3b015235200a755a9657a899fac`. Frozen engineering source: `448e9d3067796b64656f0746f4be5c4153b1271d`. The executed `git diff 448e9d3067796b64656f0746f4be5c4153b1271d HEAD -- tests/paper_full_eight_square_oracle.h tests/paper_full_eight_square_contract_test.cpp` returned no differences; inspected worktree status was clean. These observations precede creation of this report.

## Scope and method

Read the project workflow and engineering/model-routing references, both complete paper test sources, and `PRODUCTION_CONTRACT_01.md`, `INPUT_DOMAIN_AUDIT_01.md`, and `NOMINAL_SCALE_AUDIT_01.md`. Independently checked the original paper text, especially §6.3/Table 3, and exact pristine OpenFHE pin `df495ba2e91739a6dc8f1de254fc5a41155ce504` via `git show` of `src/core/lib/math/dftransform.cpp`, `src/pke/lib/schemerns/rns-pke.cpp`, and `src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp`.

Executed one bounded `/usr/bin/python3` standard-library calculation using `Fraction` for exact comparisons and `Decimal` at 65 digits only for display. It calculated slots 0 and 1 by eight exact complex squarings; both published slot-zero constant errors; all eight scale recurrences and closed products; analytic lower/upper magnitude bounds; ideal terminal nonwrap; modulus bit lengths; and indicative scalar Horner roundoff estimates. No full-slot Fraction expansion was performed.

The reproducible mathematical operations were:

- `mul((a,b),(c,d)) = (a*c-b*d, a*d+b*c)` with the frozen dyadic generator, applied eight times to slots 0 and 1.
- `S_i = S_(i-1)^2 / (d*q[10-i])`, independently compared for every `i=1..8` with `2^(100*2^i) / product((d*q[10-j])^(2^(i-j)), j=1..i)`.
- `Amin=1015/1024-15/65536`, `Amax=1015/1024+16383/2^75`; `L=(Amin^2+(1/1024)^2)^128`, `U=(Amax^2+(8/1024)^2)^128`; exact tests `.098<L<=U<.106` and `2*S8*U<q[0]*q[1]`.

One attempted engineering-source `git diff` was mistakenly issued from the pristine upstream repository and failed with `bad object`; it yielded no review evidence. The successful comparison above was subsequently run in the clean-room worktree. No FHE, FFT, NTT, compile, or cryptographic test was executed. No GREEN result is claimed.

## Findings

No blocking oracle arithmetic defect was found in the inspected scope.

- **Dyadic conversion:** oracle header lines 96–117 form exact dyadics with denominator at most `2^75`. Their finite decimal expansions fit within the 100-digit string conversion and declared 100-digit `ClientReal` precision. Subsequent decimal conversions have ample margin relative to the frozen gates.
- **Sparse signed decryption:** lines 202–207 correctly apply the negacyclic sign at `x^32768=-1`. Secret indices are supplied by the verified signed-h128 extraction. Each accumulator remains in `[0,q)`; every unsigned sum is below `2*q<2^61`, so the 128 terms do not create unchecked accumulation overflow.
- **CRT and centering:** lines 209–222 correctly perform exact CRT, center, then compute `Center(d*high+low,Q)`. Centering each operand first preserves the recombination congruence.
- **Slot roots and polynomial evaluation:** lines 225–247 use positive `exp(2*pi*i*5^s/65536)` and descending Horner iteration over ascending coefficient storage. This matches the pristine upstream powers-of-five rotation group and positive special-forward convention.
- **Exact scale:** every closed product matched its independent recurrence. `S8/S0 = 1.0003648503896039228505883078640327851759075371099059794569317001`.
- **Discriminating witnesses:** both slot-zero literals were strictly within `2^-150` of the exact rational result. The terminal slot-zero/one real difference was `6.9110412050693540435877164095632187602085666714774410079567791030e-22`, or `835.49361532289433411773948813258879664019921447399802827914883979 * 2^-80`, strictly between `2^-71` and `2^-70`. Wrong nominal normalization at slot zero gives real-component error approximately `0.000036874340501313190703278184251124024006924617453708096950833465982`.
- **Domain and ideal nonwrap:** exact comparisons passed. Displayed bounds were `L=0.098377629809948652158753081942234538430906182086691224458128240473` and `U=0.10518920462622239664533492597680046302447254411951175698983340998`. This establishes an ideal-message bound, not a Gaussian-noise guarantee.

## Two assurance limits

1. **Conditioning is not uniformly certified for arbitrary corrupt coefficients.** Binary512 Horner has substantial margin for intended coefficients of size `O(S)`. Under ordinary rounding/root-error assumptions, a conservative indicative estimate `16*N^2*(B/S)*2^-512` with `B<=2*S` is `2^-477`. This is an analytical estimate, not an executed binary512 error measurement or a rigorous library transcendental bound. The inspected code at header lines 236–247 has no dynamic conditioning guard. Fresh and first-round moduli have 620 and 520 bits; substituting their worst centered coefficient bounds into the same estimate does not certify even 120-bit absolute accuracy. No concrete false pass or false rejection was demonstrated. This limits the assurance statement; it is not grounds to change the frozen thresholds.
2. **Intermediate coverage is limited to ten anchors.** Contract-test lines 278–284 check each returned round at those anchors. A transient error at an unobserved slot that disappears under a later squaring can escape these intermediate observations. Fresh/final checks cover all slots through production decoding, with independent ten-anchor controls. This is not a proof of full-slot independent correctness at every intermediate round, nor a demonstrated defective production execution.

The frozen `2^-80` correctness and `2^-120` codec gates remain unchanged. Final production integration, actual hosted execution, and final-code review remain outside this audit's completed evidence.

## Lead recheck

The lead read both complete test sources and this report, then independently re-executed a bounded `/usr/bin/python3` Fraction calculation of all eight scale equalities, both scalar literals, the witness interval, the ideal domain and nonwrap inequality. All assertions passed; the 65-digit displays for S8/S0, witness separation, wrong nominal real error and domain bounds matched this report. This was scalar-only computation, not a transform or encrypted-chain run. A separate `git diff --exit-code 448e9d3067796b64656f0746f4be5c4153b1271d -- src include tests CMakeLists.txt .github/workflows` exited zero. No production, test, threshold, build or workflow change is made by this documentation checkpoint.
