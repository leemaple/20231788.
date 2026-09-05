# FS-RESIDUAL-ENDPOINT-01: concrete observer allowance proposal

Status: **PROPOSAL, not adopted/frozen/executed.** Prepared against supplied HEAD `75f78f336cc068852b2306281b54c774a0660549`; production and original E80 requirements are unchanged. Requested reviewer selector: GPT-6 Astra high; backend identity is not independently attested (`requested-unverified`). Fable remains unavailable, without retry. This is the mathematical allowance deliverable only; sidecar/schema/CI ownership remains separate.

## 1. Fixed mathematical and arithmetic model

Use N=32768, L=15; p=512 and 768 **binary significant bits**, `u_p=2^-p`. Test-owned centered integer coefficients c_j and independent exact positive rational scale S=n/d define a_j=c_j*d/n. Define **exact** C=sum_j abs(c_j), and K as the least power of two not smaller than C*d/n, using integer comparisons/bit lengths (K=0 when C=0). Do not calculate this upper bound by an unchecked floating-point sum or logarithm. Log C*d/n and K, in addition to the maximum coefficient/scale. All bounds below are complex-modulus bounds and therefore also bound each component.

Conditional assumptions, explicitly not interval-certified Boost guarantees:

- Every finite normal real add/subtract/multiply/divide and integer-to-binary-p conversion is rounded with relative error at most u_p; exact zero remains zero. No underflow, overflow, reassociation, fused operations or intermediate lower precision. Use explicit temporaries/`et_off` for the new helper.
- Pi at each precision has relative error at most u_p. At a supplied angle in [-2*pi,2*pi], each sin/cos result has absolute error at most 8*u_p. This is a stated transcendental assumption to source-review and challenge with independent precision and controls, **not a theorem supplied by dual-precision agreement**.
- All integer indexes, modular powers, coefficient products, scale integers, K and allowance exponents are exact. Root generation uses allocator-backed p-bit types with the same precision/exponent semantics; no lower-precision constants or widening of the other precision's table.

If the implementation cannot satisfy these hypotheses, or finite/exponent checks fail, do not use these bounds. Structural/nonfinite errors FAIL immediately; an unsupported numerical model is UNRESOLVED and cannot produce diagnostic acceptance.

## 2. Fixed helper and root construction

Generate each needed root directly at its own precision; **no recurrence for roots or twiddles**. Form angles as `pi_p * integer_k / power_of_two`, with at most one non-exact multiplication (power-of-two scaling exact). Use twist angle pi*j/N and DFT angle 2*pi*k/N. These angle/root assumptions imply complex root error <64*u_p and root magnitude <=1+64*u_p. Direct evaluation means no root error proportional to a hidden recurrence length.

Use an ordinary positive-sign unnormalized in-place radix-2 DFT after an exact bit-reversal permutation. Each of the L stages has N/2 butterflies:

    t = complex_multiply(w, b)
    out0 = a + t
    out1 = a - t

Implement complex multiplication with four real multiplications and two real additions/subtractions; each butterfly consequently has four real multiplications and six real additions/subtractions. There are exactly 245760 butterflies, 983040 real multiplications and 1474560 real additions/subtractions per endpoint/precision, besides the twist and coefficient conversion. Use all N twist roots and at most N/2 ordinary twiddle roots, shared across stages but not across precision. Endpoint reuse of a same-precision immutable root table is permitted. The counts describe arithmetic, not a Mac execution request.

Convert each coefficient using an **exact integer** product t_j=c_j*d, then `Real_p(t_j)/Real_p(n)`: at most three rounded operations, relative error <=gamma_3=3u/(1-3u). Twist using the explicit complex multiply (even though the coefficient is real). No preliminary double/decimal approximation of S, no production scale receipt as oracle truth, and no normalization by N in the forward DFT.

Select `F[(e_s-1)/2]`, where e_s=5^s mod65536 is computed exactly. Selected exponents are the 16384 residues 1 mod4, with negatives the disjoint 3 mod4 set; selected DFT indexes are a permutation of even indexes. Retain explicit uniqueness/range/conjugacy checks and the original direct-Horner anchor convention.

## 3. Fixed endpoint and Horner envelopes

For the structure above, freeze

    D_p(K) = 2^12 * u_p * K       ordinary twisted DFT endpoint error
    H_p(K) = 2^24 * u_p * K       existing direct-Horner endpoint error

These intentionally loose constants are not fitted to future residuals. Derivation: explicit complex multiply has rounding error <=16u|a||b|; complex addition has error <=4u(|a|+|b|). Combining root error <=64u with these bounds permits a multiplicative error-growth majorant `(1+128u)` per twist/butterfly dependency layer. Exact coefficient conversion contributes at most gamma_3. The DFT's row-sum bound is therefore `K*((1+gamma_3)*(1+128u)^16-1) < 4096uK` for both p. This uses the coefficient one-norm, not 2^15 times that norm: exact row-sum weights already account for the butterfly sums.

For Horner, each coefficient conversion, N multiply/add steps at a root of magnitude <=1+64u, and the existing three-operation rational inverse-scale conversion plus final multiplication are conservatively covered by `K*((1+128u)^(N+4)-1) < 2^24*u*K`. The original path evaluates unscaled integer coefficients first; its intermediate exponent range must remain valid. Do not silently substitute a different Horner algorithm and claim this source correspondence without review. The bounds remain conditional on the root/arithmetic model above.

Compute D,H as exact dyadic numbers from K, not as cancellation-prone evaluations of `(1+epsilon)^n-1`. The displayed majorants justify the closed constants; they need not run per slot.

## 4. Powers, residuals and subtraction allowance

Generate original z from the frozen integer/dyadic formula; its components have fewer than 512 significant bits, so conversion is exact. Compute z^256 and x0^256 separately by **eight explicit complex squarings**, at each p: 32 real multiplications and 16 additions/subtractions per power. Do not reuse production expected values.

A deliberately conservative fixed norm condition makes a simple usable power bound possible. For both independently computed endpoints, require the computed p-bit sum `abs(re)+abs(im) <= 5/4`, and require D_p(K)<=2^-128. With the rounding model this implies the exact endpoint modulus is <2. Require the same sum bound for exact z. These are observer-conditioning conditions, not changes to the input profile or original acceptance; if a finite value violates them the new observer is UNRESOLVED. Existing invariants/E gates still apply. Actual fixed input domain is comfortably within this guard, but no future measurement is assumed.

Both the exact fresh value and its computed approximation then have modulus <=2. Squaring-roundoff accumulation is bounded by

    2^256 * ((1+16u_p)^255 - 1) < P_p = 2^270*u_p.

The factor 255 counts repeated propagation of eight squaring errors, not 255 executed multiplies. The power Lipschitz bound on the radius-2 disk is `256*2^255=2^263`. Set

    Q_p = P_p + 2^263*D_p(K_fresh)
    R_p = 2^264*u_p.

P bounds z-power arithmetic error; Q bounds computed x0-power versus the exact decrypted fresh value to power 256. R conservatively bounds each subsequent complex subtraction, including the two subtractions forming E-I-A: powers and their differences under this model have magnitude far below 2^260. No relative-error assumption on a small residual is used.

For endpoint observations at each p, use the following absolute envelopes:

    B_E0 = D_fresh + R
    B_E8 = D_final + P + R
    B_I8 = Q + P + R
    B_A8 = D_final + Q + R
    B_identity = B_E8 + B_I8 + B_A8 + 2*R.

Here E0=x0-z, E8=x8-z^256, I8=x0^256-z^256, A8=x8-x0^256. The reference for E never changes. Evaluate each signed field at both precisions; compare all slots, not only maxima. E-I-A must have component norm <=B_identity (plus its explicitly counted comparison/serialization error, if applicable); a larger discrepancy FAILs the new integrity check. Common intermediates do not invalidate these conservative triangle bounds.

The same error bound on every slot bounds the uncertainty of a full-slot maximum. Near-ties in argmax should be reported as measured maximizers, not claims that one exact maximizer has been certified.

## 5. Concrete comparison allowances and decisions

Keep **T_obs=2^-120** and **T_E=2^-80** exactly. Introduce a fixed *estimator budget ceiling* **T_budget=2^-128** (one 256th of T_obs). This is an observer trust requirement, not a relaxed agreement tolerance or new E threshold.

Use exact dyadic/rational arithmetic for allowance sums and threshold comparisons, or independently reviewed outward upper bounds. Comparing already represented binary numbers by subtraction must not introduce an uncounted roundoff; exact extraction of their binary significands/exponents is a suitable small comparison helper. This is not a new transform or interval library.

Required comparison envelopes:

- 512 versus 768 endpoint: D512+D768.
- Ordinary endpoint versus unchanged binary512 Horner anchor: D_p+H512.
- 512 versus 768 signed E/I/A: corresponding B512+B768.
- Ordinary endpoint versus production endpoint: D_p+C_prod. For the **new comparison only**, format each finite production component with >=100 significant decimal digits, parse the literal exactly as a rational, and require its absolute value <=2. Under ordinary correctly rounded decimal formatting the production-to-literal error is <2^-300 per complex value; freeze C_prod=2^-300. Compare that exact literal to the exact represented new observer output. No claim is made that C_prod bounds production's algorithmic error; it bounds only this observation conversion. Preserve the existing production checks unchanged.

All D,H,B, B_identity and every comparison envelope used for acceptance must be <=T_budget. If not, the observer is **UNRESOLVED**, not rescued by a smaller observed discrepancy. The two precisions must also agree within their stated envelope; any exact measured difference greater than that envelope is **FAIL** of the conditional numerical model/helper, even if below T_obs. This stricter diagnostic check does not widen the original tolerance.

For every required observer comparison with measured component discrepancy d and envelope b, require both `d<=b` where both paths have the error envelopes above (excluding production, whose numerical algorithm has no such bound), and `d+b<=T_obs`. For production require `d+b<=T_obs` only. If raw d>T_obs: **FAIL immediately**, preserving the frozen scale. If d<=T_obs but d+b>T_obs: **UNRESOLVED**. Unsupported numerical assumptions/conditioning: UNRESOLVED; structural, nonfinite, wrong mapping, missing coverage or invariant failures: FAIL immediately. Neither UNRESOLVED nor FAIL can produce a diagnostic PASS.

Do not reinterpret old E predicates: every existing finite E80 miss retains its count and forces the original test FAIL, exactly as before. New E envelopes may explain numerical uncertainty but cannot erase such a miss. If optional A80 is separately adopted, classify with exact `a=max_component|A8_computed|`, b=B_A8: PASS only if a+b<=2^-80; FAIL if max(0,a-b)>2^-80; otherwise UNRESOLVED. If not adopted, report those bounds as characterization only. Define “small terminal added residual closes this diagnostic” as upper bound a+b<=2^-80 with all observer prerequisites satisfied, never as completion of E80 or the full user goal.

## 6. Controls and practical checks

Keep the constant, X, X^(N-1), fixed signed sparse-polynomial controls and ten Horner anchors from NEXT_TEST_SPEC. Freeze the signed control to `3-2X+X^17-X^(N-1)`. Apply the same scale conversion/DFT envelopes to each control's exact coefficient one-norm. Independently evaluate each nonzero control term at p768 using the exact modular angle index `(e_s*degree) mod65536`, a directly generated root at that angle, and the exact-integer coefficient/scale conversion above. Sum at most four terms with three complex additions; no repeated root powering or shared transform table. This is at most four coefficient conversions, four roots, four explicit complex multiplications, and three complex additions per reference. Freeze its reference envelope `J768(K_control)=2^10*u768*K_control`: gamma_3 conversion, 64u root error, 16u multiply rounding and three 4u additions fit well within this constant. Compare using `D_p+J768`, with the same budget and T_obs rules. Do not assign zero uncertainty to a computed sin/cos reference. Root sign/map checks and exact integer expectations are separate from numerical closeness; a generic unbounded helper is not authorized.

Illustrative scalar sanity check, **not a prediction of future ciphertexts**: K=2^15 covers a coefficient-one-norm/scale at most 32768. At p512 this gives D=2^-485, H=2^-473 and propagated term 2^263 D=2^-222; P=2^-242 and R=2^-248. Even K=2^32 gives propagated term 2^-205. Thus the deliberately loose power allowance still leaves >70 bits of margin below T_budget for these examples. Actual K is measured once per endpoint and tested against the pre-frozen formulas; no allowance constants change afterward.

The sole local numerical self-check was the following bounded Fraction arithmetic. The complete authored document/helper was read before executing this block verbatim through inline `python3`. It completed successfully and printed `512 15 budget example OK`, `512 32 budget example OK`, `768 15 budget example OK`, `768 32 budget example OK`. These are scalar inequality checks, not an observer or cryptographic test PASS; no vector/transform/crypto work was included:

```python
from fractions import Fraction as F
for p in (512, 768):
    u = F(1, 2**p)
    assert (1+128*u)**16*(1+3*u/(1-3*u))-1 < 4096*u
    # Avoid a huge exact N-term Fraction power; Bernoulli/binomial majorant:
    # (1+x)^n <= 1/(1-n*x) for nonnegative x with n*x<1.
    t = (32768+4)*128*u
    assert t/(1-t) < 2**24*u
    t = 255*16*u
    assert 2**256*t/(1-t) < 2**270*u
    for k in (15, 32):
        D = 2**12*u*2**k
        P, R = 2**270*u, 2**264*u
        Q = P + 2**263*D
        E, I, A = D+P+R, Q+P+R, D+Q+R
        assert E+I+A+2*R <= F(1, 2**128)
        print(p, k, 'budget example OK')
```

This proposal does not certify Boost transcendental accuracy, shared official inverse NTT, universal nonwrap or cryptographic correctness. It supplies a checkable conditional arithmetic model plus falsification controls. No new encrypted execution, profile change, gate weakening or production patch is authorized.
