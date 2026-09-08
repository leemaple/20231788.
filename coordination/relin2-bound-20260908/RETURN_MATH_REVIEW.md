# Independent mathematical/source review of the Relin2 return

## Scope and disposition

This review compares the immutable Pro return (ZIP SHA-256
`68fbad707507d225e05f35ef4a2039b8ef693a615c2bcc68e7cd386f8dad6b4d`)
against the independently prepared `INDEPENDENT_SOURCE_MAP.md`, production
source checkpoint `a4b815a733efe81897325e2a8e4c826a4ebfa439`, and the permitted official
OpenFHE mirror at pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
`RELIN2_BOUND.zh-CN.md`, `CLAIMS.json`, `SOURCE_MAP.tsv`, and `NEXT_ACTION.md`
were read in full. No returned program, build, FHE operation, sampler, or
FFT/NTT operation was executed by this reviewer.

**Disposition: conditional acceptance of C03–C25 after the three bounded
corrections below.** No core equation in C03–C25 is rejected. In particular,
C13's zero extra-`d` digit and C25's coefficient recurrences survive independent
source and algebraic scrutiny. The original S100 result remains FAIL; this
review makes no production-defect claim.

## Claim-by-claim result

“Accept” below always retains the assumptions named in the return; it is not a
claim about an arbitrary HYBRID profile or an unauthenticated external key.

| Claim | Result | Independent reason / remaining condition |
|---|---|---|
| C03 | Conditional accept | `sOld=s^2` and HYBRID key generation give `B_j+A_js=P theta_j s^2+e_j (mod QP)` for an honestly generated key. Relin2's structural checks cannot authenticate this hidden equation. Sources S05–S08. |
| C04 | Accept for the frozen profile | With one prime per digit and the unsigned branch, the retained coefficient is exactly `U_q(c)` and the complementary towers receive that same integer reduced modulo their primes. Sources S09, S10, S29, S36. |
| C05 | Accept for single `P` | The two raw accumulators have independent nonnegative residues `r_i=U_P(z_i)` and `t_i=(z_i-r_i)/P=floor(z_i/P)`. This is Euclidean division, not nearest rounding. Sources S10–S14, S50. |
| C06 | Accept | OpenFHE adds the two key-switch outputs separately to ciphertext coordinates 0 and 1. Source S15. |
| C07 | Conditional accept | The numerator defining `nu_Q` is divisible by `P` by the honest-key congruence; it is not a new rounding hypothesis. |
| C08 | Conditional accept | The bound follows from negacyclic convolution, both component residues, and `||s||_1=h`; it correctly has `(1+h)(P-1)`. |
| C09 | Accept | Direct Euclidean division gives `delta_i=-g_i+eta_i (mod Q)` with two coordinate corrections. The sign and `eta_i in {-1,0,1}` range are correct. Sources S09, S13–S15, S50. |
| C10 | Conditional accept | Subtracting the three C07 identities gives the stated key-noise digit-carry term and the factor `2(P-1)` for each `Delta r_i`; no independence is used. |
| C11 | Accept as a sufficient special case, subject to correction 1 and correction 2 | `kappa=0` implies `G=g=0`, hence the stated `{0,1}` coordinate carries and decoded bound `1+h`. The converse is not proved. The returned M13 secret is not in the frozen factory sampler's sign-count support. |
| C12 | Accept only as the stated primitive model | An unsigned `k`-prime CRT sum can equal `U_M(c)+Ma`, `0<=a<=k-1`; the analogous `P beta` changes mod-down to `floor(z/P)-beta`. This is not a full theorem for arbitrary HYBRID settings. |
| C13 | Accept for the frozen profile and same key rows | Raising multiplies every active tower by `d` and appends a zero `d` tower (`src/double_ckks.cpp:1011–1023`). With `alpha=1`, the new digit is exactly zero. The remaining digit values and the shifted P-key index coincide, so both accumulators and both Q outputs coincide after restriction. Sources S08, S09, S13, S14, S36, S50. |
| C14 | Accept | `DropLastElementAndScale` uses the centered residue of the dropped odd `d` tower, giving the exact coordinate identity `d*high+low=A (mod Q)`. Sources S16–S18, S37. |
| C15 | Accept | DCP's two component remainders cancel algebraically upon RCB, leaving the sum of the two actual relinearizations. This modular coordinate identity needs no nonwrap assumption. Sources S08, S16, S19, S20, S42. |
| C16 | Conditional accept | C13 makes the high-call error `nu_Q(dH_2)`, not `d nu_Q(H_2)`; the independently reused key gives a second term `nu_Q(L_2)`. A worst-case sum, not independence, is appropriate. |
| C17 | Accept | The explicit integer wrap `w_2` correctly separates a centered phase difference from a modular error. The half-modulus margin is sufficient, not necessary. |
| C18 | Conditional accept, subject to correction 3 | For a successful Peikert call, source bounds returned coefficients by the finite table length: 13 for default `std=1` and at most 39 for nominal `3.19F`. This is implementation support, not an ideal-Gaussian tail theorem or an observation of historical samples. Sources S06, S07, S21, S39. |
| C19 | Conditional accept | The symbolic `B_K`, `B_2`, and normalized Relin term follow from C04, C08, C13, and C18. The eight exact numerical inequalities were intentionally not re-executed here; they require the separately recorded exact-arithmetic replay, not model agreement. |
| C20 | Conditional accept | DCP's component remainders survive in the low member as `rho_d0+s rho_d1`, with bound `R_d=(1+h)(d-1)/2`; they cancel only in RCB. Sources S08, S16–S18, S22. |
| C21 | Accept | Tensor2 has positive cross terms and the integer identity `dT*=M_aM_b-l_a l_b`; the input wrap multipliers must remain unless `alpha_a=alpha_b=0`. Sources S22, S23, S40, S41, S44. |
| C22 | Accept | RS2's recombination is exactly one ordinary rescale modulo `Q'=Q/m`; the displayed `w_pre` and `w_out` are distinct and both are needed for an equality between chosen centered lifts. Sources S17, S18, S24–S27, S42, S43, S49. |
| C23 | Accept | After choosing the stated integer lifts, the scale `S_next=S_aS_b/(dm)` and every multiplier in the canonical formula are correct. It is not division inside `R_Q`. |
| C24 | Conditional accept | The inequality requires centered input representatives (`alpha_a=alpha_b=0`) and `W=0`. The displayed NW margin is a sufficient, non-circular way to get both rescale wrap terms zero, but is not necessary. These conditions remain unverified on the historical chains. |
| C25 | Conditional accept | The coefficient recurrences are valid without assuming phase nonwrap, provided A10 supplies genuine current high/low coefficient bounds and compatible integer lifts are used as shown below. They are not a canonical-norm recurrence or an eight-step certificate. |

## Required bounded corrections

### 1. Do not state that zero digit carry is necessary

`RELIN2_BOUND.zh-CN.md:240`, `:380–381`, and `:589` use “only/才” wording
for `kappa_j=0`. C11 proves a sufficient special case, not a converse. Nonzero
digit carries can still yield the same simplified coordinate or decoded result
through special key/input cancellation. Replace those phrases by, for example:

> If every `kappa_j=0`, then the simplification is guaranteed. This is a
> sufficient condition; no necessity claim is made.

The general C09/C10 equations, which retain `G_i`, `g_i`, and both component
remainders, remain the adopted statements.

### 2. Qualify or replace M13 as a factory-supported witness

The returned M13 uses `s=sum_{i=0}^{127}X^i`
(`RELIN2_BOUND.zh-CN.md:275–285`). It is signed ternary of Hamming weight 128,
but it is outside the actual frozen factory sampler's sign-count support:

- `src/paper_h128_client_keypair.cpp:193–201` constructs the secret through
  `TugType` with `h=128`;
- official `dcrtpoly-impl.h:178–188` consumes exactly one TUG integer vector;
- official `ternaryuniformgenerator-impl.h:118–142` accepts only 63, 64, or 65
  positive coefficients when `h=128`.

Therefore M13 may remain an abstract `||s||_1=128` algebraic witness, but must
not be cited as a possible draw of the frozen factory. A minimal
factory-support-compatible replacement is

`s=sum_{i=0}^{63}X^i-sum_{i=64}^{127}X^i`,
`J=sum_{i=0}^{N-1}X^i`, and `A_0=((P+1)/2)J`, with the same zero errors and key
equation construction. It has 64 positive and 64 negative coefficients. In the
negacyclic ring,

`(sJ)_k=2(k+1)` for `0<=k<64`,
`(sJ)_k=2(127-k)` for `64<=k<=127`, and zero thereafter.

Thus the same no-digit-carry Euclidean-division calculation gives
`delta_1=J`, `delta_0=1` on coefficients 0 through 126 and zero thereafter, and
`||delta_0+s delta_1||_c=129=h+1` (attained at coefficient 63). This verifies
the proposed correction algebraically without sampling or execution. It is
still a support witness, not a statement about the historical secret.

### 3. Remove the circular C18 dependency

In `CLAIMS.json:49–53`, A07 already states the conclusion “successful
finite-Peikert key error coefficients <=39”; C18 then lists A07 as an assumption
at `CLAIMS.json:628–637`. The prose proof is sound, but the structured dependency
is circular. Either:

1. remove A07 from C18's assumptions and derive C18 directly from the fixed
   source path and successful-return condition; or
2. redefine A07 to contain only the preconditions (successful fixed Peikert
   consumer and `std` in the two source-selected cases), leaving `<=39` to C18.

C19, C24, and C25 may then depend on the derived support bound.

## Why C25 does not require intermediate nonwrap

The claim is correct, but `RELIN2_BOUND.zh-CN.md:551–563` should retain the
following lift bridge so its “no phase nonwrap required” conclusion is
auditable.

Let `A=(A_0,A_1)` be compatible full-basis integer lifts of the raised-high
relinearization and let `rho_{d,i}=C_d(A_i)`. Zero extra-`d` digit plus the same
P residues makes the full and restricted error integer the same `nu_H`; hence

`A_0+sA_1 = d h_a h_b + nu_H + dQ Gamma`.

Consequently `nu_H-rho_d` is coefficientwise divisible by `d`, where
`rho_d=rho_{d,0}+s rho_{d,1}`, and the DCP high member has the representative

`h_a h_b + (nu_H-rho_d)/d (mod Q)`.

Its coefficient norm is at most
`N H_a H_b+(B_K+R_d)/d`. Rescaling a representative modulo `Q` and then
centering modulo `Q'=Q/m` divides this bound by `m` and adds at most `R_m`.
Any prior `Q`-wrap becomes an integral multiple of `Q'` after division by `m`,
so it does not require `w=0`. This gives the first C25 recurrence.

Before RS2, the low member has representative

`h_a l_b+l_a h_b+nu_L+rho_d (mod Q)`.

Using the actual source identity
`low_next=RS(RCB)-d RS(high)` (`src/double_ckks.cpp:1143–1189`), the common
high contribution cancels modulo `Q'`; the two independently centered rescale
remainders contribute at most `(1+d)R_m`. Again, any compatible `Q`-representative
change becomes a `Q'` multiple. This gives the second recurrence. Nonwrap is
needed only when one wants C24's equality to a selected real/CKKS decoded
representative, not for these centered coefficient bounds in the quotient ring.

## Residual unknowns preserved

- A02 remains a primitive-semantics assumption; this slice did not re-audit or
  execute the underlying transform/arithmetic implementation.
- A05 is guaranteed by the honest key-generation path, not by Relin2's
  structural evaluation-key validation for an arbitrary supplied key.
- Historical `alpha_a`, `alpha_b`, `w_pre`, `w_out`, high/low phase margins, the
  actual DGG object state, and actual error coefficients remain unobserved.
- C18's finite support suffices for the local public bound but does not certify
  sampler distribution quality, security, or any payload/public-key noise.
- C25 supplies an analytic propagation interface only. Without an upstream A10
  bound it neither establishes per-step historical nonwrap nor changes the
  original S100 FAIL.

No further broad experiment follows from this review. The minimal adoption is
the corrected static contract above, with production patch `NONE` and the
historical result unchanged.
