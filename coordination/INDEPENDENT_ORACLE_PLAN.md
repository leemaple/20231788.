# Independent oracle and retained TDD evidence plan

Recorded: 2026-08-31

This plan is derived only from paper 2023/1788 and pristine OpenFHE 1.5.0. It does not use a prior implementation. It defines how expected values remain independent from the production module.

## Evidence contract

For every public seam, retain the first failing command, exit code, and complete relevant output before adding production behavior. Store it under `artifacts/tdd/<slice>/red.txt`. After the smallest implementation passes, store the same command and its complete relevant output under `artifacts/tdd/<slice>/green.txt`. Every evidence file records source commit, OpenFHE commit, runner OS/compiler, and command line.

A compile failure is acceptable red evidence only for the first public-interface slice. Later slices need a behaviorally meaningful failure. No test may call a private production helper to calculate its expected result.

## Exact integer/CRT oracle

Use a test-only adapter based on `boost::multiprecision::cpp_int`:

1. Reconstruct each coefficient from its ordered RNS residues by textbook CRT.
2. Center it in `(-Q/2, Q/2]`.
3. For an odd divisor `q`, compute the centered remainder `r` in `(-q/2, q/2]` and exact quotient `(x-r)/q`.
4. Convert expected signed coefficients back to each target tower independently.
5. For polynomial products, use a direct schoolbook negacyclic convolution modulo `X^N + 1`, then reduce/center modulo the requested composite modulus.

This oracle shares neither `DropLastElementAndScale`, production tower-copy code, production tensor loops, nor production metadata helpers.

Boundary coefficients include `0`, `+/-1`, `+/-(q-1)/2`, `+/-(q+1)/2`, values one step around those points, and values one step around `+/-Q/2`. OpenFHE RNS primes are odd, so there is no integer exactly at `q/2`; tests must not invent an even-modulus tie case.

## Slice matrix

### DCP

- Feed a public DCP call ciphertext components whose coefficients are constructed from deterministic signed integers.
- Compare every quotient and remainder coefficient/tower with the independent oracle.
- Assert `x = q_div * quotient + remainder` over the integers before reduction and modulo the retained prefix.
- Assert last-tower consumption, ordered-prefix identity, component count, key tag, logical scale, noise-scale degree, and level transition.
- Negative cases: wrong level, wrong last modulus/order, mixed formats, wrong component count, and mismatched context/key tag.

### RCB and pair Add/Sub

- Compare RCB coefficients to independent `q_div * high + low (mod Q_l)` arithmetic.
- Compare Add/Sub to independent componentwise signed polynomial arithmetic.
- Assert metadata preservation and reject incompatible basis, level, logical scale, context, format, component count, or key tag.

### Tensor2

- Use independently generated two-component pairs and the schoolbook negacyclic oracle.
- Check both three-component results against `high1 tensor high2` and `high1 tensor low2 + low1 tensor high2`.
- Separately prove that the low-low product is absent by choosing inputs for which it is nonzero.
- Assert the logical-scale transition `S1 * S2 / q_div`, noise-scale degree 3, unchanged level, and exactly three RLWE components in each output member.

### Relin2

- Generate the official OpenFHE evaluation key, but treat production Relin2 as a black box.
- Assert the public result has two RLWE components per pair member and reconstructs to the same decrypted polynomial as the three-component input up to the paper's relinearization bound.
- Independently measure the standard OpenFHE relinearization discrepancy on the recombined three-component input using the test secret key; report this as an observed execution value, never as a universal bound.
- Verify that the high path really executes on `[q0, ..., q_l, q_div]` and the low path on the exact prefix `[q0, ..., q_l]`. A tower-count-only check is insufficient.
- Run the slice for each explicitly supported key-switch technique; do not claim HYBRID and BV support from a run of only one.

### RS2

- Compute ordinary centered rescaling independently for `high` and `q_div * high + low`.
- Assert exact pair identity `RCB(RS2(pair)) = RS(RCB(pair))` coefficientwise modulo `Q_(l-1)`.
- Assert consumption of exactly `q_l`, level increment, logical-scale division by `q_l`, and noise-scale degree reduction from 3 to 2.

### Mult2

- Deterministically encode two real/complex slot vectors at `noiseScaleDeg == 2`, encrypt, DCP, Mult2, RCB, and decrypt.
- Compare with host-language plaintext multiplication using an error threshold recorded before the green run.
- Also compute the paper Theorem 4.8 quantities from the actual test instance: `M_high`, `M_low`, secret-key Hamming weight `h`, `Q_l`, `q_div`, and `q_l`. Assert the correctness precondition `N * (M_high*q_div + M_low)^2 + E_Relin + h < Q_l/2` using a documented conservative `E_Relin`; if only an observed execution discrepancy is available, label the result an empirical certificate rather than a proof.
- Record and check the theorem error expression `(N*M_low^2/q_div + E_Relin + h)/q_l + (h+1)/2` alongside the decoded CKKS error.
- Assert the measured ratio between `2^(2p)` and `q_div*q_l` instead of assuming exact equality.
- Assert that a second Mult2 attempt fails at the documented refresh boundary before any key-switch table is indexed.

## Runner matrix

- Windows ZCode/Zima runner: warning-enabled build plus the exact test filter for each red/green slice.
- GitHub Actions: clean checkout, pinned OpenFHE 1.5.0 source commit, warning-enabled build, and the complete test suite.
- The Mac may perform source inspection and small file checks only; it is not the sustained compile/test runner.

No passing test, build, or theorem certificate is claimed by this plan.
