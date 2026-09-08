# Independent root derivation — provisional, before Pro return

2026-09-08. Source a4b815a / official df495ba2. This note is NOT included in Pro's initial packet or sent during its thought. It is a separate root check for comparison with the eventual derivation, not accepted paper-scale correctness or a new precision result.

## A. Relin2 recombination does not itself need additivity

Write q for the **product of active Q towers**, d for Div, R=Z[X]/(X^N+1), and phi_k(c)=sum c_i s^i in R/kR. H,L are the three-component tensor outputs over q. The project constructs a raised Hstar over qd with Hstar mod q = d H and Hstar mod d = 0, calls R_hi=Relin_qd(Hstar) and R_lo=Relin_q(L), then DCP(R_hi)=(U,V) and returns (U,V+R_lo).

The implementation explicitly defines V=(R_hi mod q)-d U coordinatewise (`double_ckks.cpp:399–427`), regardless of which quotient rule produced U. Therefore, as an exact **mod-q identity**:

    d U + (V + R_lo) = (R_hi mod q) + R_lo.

If valid individual key-switch error lifts eta_hi, eta_lo satisfy the ordinary three-to-two phase contracts in their respective rings, the recombined phase is

    phi_q(RCB(output)) = d phi_q(H) + phi_q(L) + eta_hi + eta_lo (mod q).

Thus a compositional phase bound can start with **two separate ordinary key-switch error bounds**, not a claim that Relin is additive or that a carry affects only the second coordinate. This does NOT bound the low component by itself, prove the ordinary HYBRID error lifts, or identify the centered output difference as eta_hi+eta_lo without a nonwrap argument. High and low calls have different active bases; treating them as one identical linear map would be unjustified.

Source: project `double_ckks.cpp:1009–1060`; official `keyswitch-hybrid.cpp:308–438`. These source regions were actually read. No injected ciphertext was executed.

## B. Candidate ordinary HYBRID phase bound, specialized to this profile

Assumptions to check against Pro independently: one prime per partition (alpha=1); one auxiliary prime P; noiseScale=1 (thus t=0 in ApproxModDown); coherent exact modular/NTT arithmetic and common small-secret lift s; actual evaluation-key error polynomials e_j have a coefficient bound E. Let D_j be the coefficientwise [0,q_j) lift of the jth ciphertext digit. For alpha=1, ApproxSwitchCRTBasis has one summand; no multi-prime CRT quotient correction is needed to describe that digit's lift.

The key rows satisfy b_j+a_j*s = P*selector_j*s² + e_j in the extended ring; summing digit products gives the signal P*c2*s² modulo each Q prime and noise nu=sum D_j*e_j. Both coordinates undergo ApproxModDown. With a single P and the ordinary non-reduced-noise switch, let r0,r1 be their coefficientwise [0,P) residues. Candidate integer error lift:

    eta = (nu - r0 - r1*s) / P.

This numerator is coefficientwise divisible by P when the key/digit/secret consistency assumptions hold. It describes a phase error modulo active Q, not necessarily the centered numerical error. In particular it retains both coordinate remainders; replacing r0 by zero needs a separate argument.

For h=||s||_1 and negacyclic convolution, a conservative conditional bound is

    ||eta||_infinity,coeff <= [N E sum_j(q_j-1) + (1+h)(P-1)] / P.

At any canonical embedding, the further coarse bound is N times the coefficient bound. This is only an upper bound, not an equality, measured variance, failure probability, or Gaussian-security statement. The support assumption E=39 conservatively covers the source's normal completed Peikert draws at sigma3.19f or1; it must not be used if another sampler/configuration is active. `FindInVector` throws if no table entry is found, rather than returning an arbitrarily large integer.

Source: official `keyswitch-hybrid.cpp:98–125,314–438`, `dcrtpoly-impl.h:888–935,966–1012`, `discretegaussiangenerator-impl.h:62–114`. Need independently verify all preconditions, generated constant tables, secret extension and the concrete modular arithmetic branches before promoting this to an accepted implementation-specific theorem. No original E80 success follows from this draft.

## C. Why a small modular error is not an unconditional numerical bound

For scalar illustration only, q=101, intended centered value=50, perturbation=3 yields centered result=-48. The output is congruent to 50+3, but the observed integer difference is -98=3-101. A sufficient bound such as |target|+B<q/2 rules this out; failure of that sufficient condition does not itself prove a wrap. Ring/canonical bounds additionally need coherent coefficient lifts and scale normalization.

The companion small scalar script checks only these algebraic distinctions. It is not OpenFHE, a HYBRID simulation, a sampler, or evidence about any captured secret chain. Pro has been tasked with completing the source-level proof and deciding whether these ideas actually close a useful part of the paper's assumptions.
