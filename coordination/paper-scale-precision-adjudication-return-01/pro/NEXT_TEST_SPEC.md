# FS-RESIDUAL-ENDPOINT-01 — one bounded coverage-closure test

**Status:** specification only; NOT IMPLEMENTED or RUN here. Requires independent review before Codex integration and the separately authorized unique automatic Linux/Windows push. No production change is recommended.

## 1. Exact uncovered risk and hypothesis

The existing source and signed ten-anchor observations support inherited-error amplification, but the terminal full-slot maximum E is larger than the anchor maximum. The current evidence cannot exclude a localized added-arithmetic defect or a production transform/order discrepancy outside those ten anchors. There are no full-slot fresh independent values or independent integer-polynomial dumps in the supplied logs from which the missing I/A values can be recovered retrospectively.

Test the falsifiable hypothesis: **on the same one fresh encryption and eight-square chain, the independent terminal full-slot error decomposes into inherited fresh error plus a small added residual, and the production endpoints agree with independently evaluated integer polynomials at all slots.** Do not state this hypothesis as an already proved full-slot conclusion.

Only the fresh and terminal endpoints need a new full-slot transform. They answer the missing terminal claim while existing ten-anchor observations retain all eight stages. This test does not claim full-slot intermediate bounds, a certified HYBRID error theorem, or all-key correctness. [TEST:264–301,309–323; DECISION §§5–9]

## 2. Frozen scope

Retain the exact existing public-key client Encrypt, one DCP, eight public Mult2 squarings, owned terminal RCB, exact-scale binder/Decrypt, original input generator, h128 root secret projection, all ordered Q/P/d/Mult constants, and metadata. Evaluator interfaces receive no secret. No production implementation, encryption distribution, noise parameter, chain, warning flags, API regression or frozen gate changes.

Use the existing test-owned `freshPolynomial` from TEST:272 and `finalPolynomial` from TEST:319, obtained by the current independent sparse-secret negacyclic decryption and cpp_int CRT. Reuse them; do not produce an oracle polynomial by calling production Decrypt, its Forward/Special transforms, stock packed Decode or production CRT interpolation. Official inverse NTT remains the declared shared dependency. Compute exact S0/S8 with the independent closed-product integer formula. The source ciphertext, public/secret keys, plan and owned terminal result remain immutable.

The future run may have a different fresh key from the supplied run. It is a new, more discriminating observation of its own single chain, not a reproduction of the unavailable original capture/key. Do not request or choose favorable keys and do not add a second encryption or second chain.

## 3. Independent full-slot evaluation

Implement one small **test-local ordinary complex radix-2 DFT** helper, not a generic instrumentation framework. It must not reuse the producer's special-transform table, split-complex butterflies, index table, codec implementation or returned decoded slots as its reference.

For real integer polynomial coefficients c_j, j=0…N−1, N=32768, let

    xi = exp(2*pi*i/(2N)),
    u_j = (c_j / S) * xi^j,
    F_k = sum_{j=0}^{N-1} u_j * exp(2*pi*i*j*k/N).

This is the **positive-sign, unnormalized** ordinary DFT. For slot s compute the exact modular exponent `e_s = 5^s mod 2N` and select

    x_s = F_((e_s−1)/2).

This evaluates c(xi^(5^s))/S without the producer's packed special-FFT layout. Complex conjugacy supplies the other roots but does not require a hidden sign change. Check by integer arithmetic that the 16,384 selected exponents/indices are distinct, odd/in-range as appropriate, and disjoint from their conjugates. Do not replace this by the contiguous first half of an ordinary DFT. [PAPER:197–247; ORACLE:283–313; IO:329–400 is the implementation to remain independent from]

Evaluate both endpoints at **binary512 and binary768**, independently generating each precision's roots and temporaries. Use allocator-backed multiprecision types for transcendental root generation to avoid reintroducing the already diagnosed fixed-storage warning; keep warning flags and precision intact. Neither root table may be formed by widening the other. The helper can share its own precision-generic mathematical implementation; independence from production, not duplicating every line of test code, is the requirement.

At the existing ten anchors, compare both full-slot results with the unchanged binary512 direct-Horner path. Also compare binary512/768 at every slot and compare the independently evaluated endpoints to the corresponding production outputs at every slot. Use the already established **2^-120 observer-agreement scale**, not a tolerance tuned to future E/A values. Source review must check the sign, normalization, rational scaling, input conversion, selected-slot map and root generation. Log the actual coefficient/S and coefficient-one-norm/S conditioning values. Dual-precision agreement is a check, not a certified interval proof; shared inverse NTT and multiprecision-library dependence must remain disclosed.

Before integration, freeze the ordinary-Horner/DFT error-estimator formulas using fixed operation counts and explicit root-rounding assumptions; evaluate those formulas on the actual observed coefficient norms of each endpoint. The estimates must be comfortably below 2^-120 or the oracle is UNRESOLVED, not accepted by rounding a convenient result. This is scalar conditioning work on the same observations, not a new cryptographic experiment.

## 4. Independent helper controls, frozen before encryption

Test the ordinary transform/map using exact sparse polynomials and the existing direct-Horner anchors, without production Decode:

- Constant polynomial 1: every selected slot equals 1/S; catches normalization and scaling mistakes.
- Monomials X and X^(N−1): compare to independently generated positive-root powers at all selected indices; catches sign, selection and negacyclic orientation. Include exact integer checks of the index permutation.
- A fixed real signed sparse polynomial such as `3 − 2X + X^17 − X^(N−1)`: compare against direct closed-form scalar evaluation at every slot at higher precision. Fix coefficients and positions before the encrypted run.

These are bounded controls for the new observer, not a second encrypted trial. Keep exact rational z generation independent of this helper. Preserve the existing wrong-S0 terminal control, original-input phase/witness controls, and immutable/foreign/cleanup checks. These controls cannot prove the shared inverse NTT correct; no new NTT implementation is requested.

## 5. Values to compute and retain

At each of 16,384 slots, generate the original exact dyadic z using integer/rational formulas, evaluate z^256 by independent binary768 repeated scalar squaring, and use the independent endpoint values x0,x8 to compute

    E0 = x0−z,
    E8 = x8−z^256,
    I8 = x0^256−z^256,
    A8 = x8−x0^256,
    E8−I8−A8.

Also compute the binary512 versions to bound sensitivity of these residual calculations, not just the raw values. **Do not redefine E using x0.** The A reference is deliberately different and must always be named A. Preserve the existing original-input E predicates and numeric-failure accumulation/fail-fast boundary.

Report full-slot maxima of E0,E8,I8,A8; argmax slot and real/imaginary component for each; and the signed E/I/A tuple at **each** maximizer, not just a subtraction of maxima. Report independent/production discrepancy maxima and their maximizers. Report the worst signed decomposition discrepancy and fixed-input witness values. Keep all current ten-anchor stage E/I/A/L output unchanged.

Retain one canonical, compressed test-evidence sidecar with **all 16,384 rows**:

    slot, E0.real, E0.imag, E8.real, E8.imag

Use at least 100 decimal digits for these residuals. Do not serialize only near-unit x0 with insufficient digits. A metadata header identifies schema version, source and OpenFHE pin, host/run/attempt, input formula, precision, exact scales, row count and sidecar hash/size. No secret, key material, ciphertext or intermediate integer coefficients need be exported. The small known plaintext-error vectors suffice for scalar-only checking.

An offline checker can reconstruct `x0=z+E0`, `x8=z^256+E8`, then recompute I8,A8, all maxima, signed maximizing tuples and the witness using exact dyadic input and independent high-precision scalar arithmetic. This checks the reported decomposition and coverage without using production Decode as its own oracle and without FHE/NTT/FFT on the user's Mac. It does not by itself certify that the sidecar was produced by the correct transform; the source, test controls and logged oracle agreements establish that separate link.

## 6. Pre-frozen dispositions and stopping rule

**Oracle inconsistency, nonfinite value, wrong shape/map, missing sidecar/rows, source/key mutation, or existing semantic/invariant failure:** FAIL. Do not continue past these errors to manufacture numeric results.

**Finite E80 miss:** retain it under the original label and force the original failure outcome before COMPLETE PASS, exactly as now. Additional A reporting cannot remove that failure. Future miss counts are measured, not assumed to equal seven; the supplied seven-miss histories remain immutable.

**Proposed A80 terminal predicate:** only if the separately named adjudication draft is independently adopted, evaluate `max_component |A8| ≤2^-80` as a distinct claim. The bound is inherited from the original engineering budget, not selected from the future data. If a result lies so near a comparison boundary that the pre-frozen observer error analysis cannot distinguish sides, mark that new claim UNRESOLVED rather than widening a gate. A80 success is never E80 success.

**Full-slot A small with trustworthy oracle:** stop this diagnostic workstream. Record terminal added-arithmetic evidence alongside the original-input failure; do not request more keys or a full-slot intermediate campaign merely to strengthen rhetoric.

**Substantial added residual or production/independent discrepancy:** record the exact bad slot/component and whether it is already present fresh, only terminal, or in producer/independent agreement. That result falsifies the relevant global explanatory claim. It does not automatically prove which operation is wrong. Use the existing public seams and source to construct a minimal RED regression for that concrete defect hypothesis before proposing any production change. No automatic random repeat or broad instrumentation framework follows.

## 7. Explicit non-goals

No attempt to reproduce Section 6.3's 1,000-run empirical average; no assertion that small endpoint A certifies all intermediate slots, all keys, security, or printed theorem nonwrap assumptions; no parameter tuning, key selection, encryption weakening, refresh/bootstrap, gate patch, network dispatch, or local Mac compilation/cryptographic/transform work. This document authorizes no execution by itself.
