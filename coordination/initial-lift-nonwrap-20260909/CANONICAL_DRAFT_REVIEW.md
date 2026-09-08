# Independent adversarial review: canonical-margin draft

## Scope and order of review

This is a bounded mathematical/source review at coordination checkpoint
`e6b21a6327546c219e5f32057eeb8e2d3e465ab8`, against unchanged production
source `a4b815a733efe81897325e2a8e4c826a4ebfa439` and the permitted official
OpenFHE mirror at `df495ba2e91739a6dc8f1de254fc5a41155ce504`. I first reconstructed the
integer bridge below from the adopted Relin2 contract and source, before
opening `ROOT_CANONICAL_MARGIN_DRAFT.md` or its checker. I did not execute that
checker, a build, FHE/FFT/NTT, sampling, or cryptography. This review is not an
S100/precision disposition.

## A. Independent pre-draft reconstruction

Work coefficientwise in `R=Z[X]/(X^N+1)`. At one multiplication let the active
input modulus be `Q=m Q'`, where `m` is the last active Mult prime removed by
RS2. Let `d` be Div. Choose the current centered integer phase representatives
`h_a,l_a,h_b,l_b` of the two pair members and put

```text
H_t = h_a*h_b,
L_t = h_a*l_b+l_a*h_b.
```

For the two Relin2 calls, let `nu_H,nu_L` be compatible integer HYBRID error
representatives, and let `rho_d` be the decrypted phase of the two centered
component remainders modulo `d`. The adopted zero-extra-`d`-digit/same-key-row
argument gives the integral representatives

```text
X = H_t + (nu_H-rho_d)/d,
Y = L_t + nu_L+rho_d.                              (A1)
```

Here `d | (nu_H-rho_d)` coefficientwise. `X,Y` represent the actual Relin2
high/low phases modulo `Q`; moreover

```text
dX+Y = d H_t+L_t+nu_H+nu_L.                       (A2)
```

Thus the DCP carry cancels only in recombination. The `nu_H` term does not
disappear: it is divided by `d` in `X`, and appears once in (A2).

For RS2 choose compatible component lifts of the high ciphertext and of its
recombination with low. If `r_H,0,r_H,1` and `r_C,0,r_C,1` are their centered
component residues modulo `m`, define the phase rounding terms

```text
epsilon_H = -(r_H,0+s*r_H,1)/m,
epsilon_C = -(r_C,0+s*r_C,1)/m,
R_m       = (1+h)(m-1)/(2m).
```

Then `||epsilon_H||_c,||epsilon_C||_c<=R_m`. Official CKKS rescale applies
`DropLastElementAndScale` independently to each ciphertext coordinate, and
project RS2 computes `low_next=RS(RCB)-d RS(high)`. Therefore integral
representatives of the two output-member phases modulo `Q'` are

```text
x = X/m + epsilon_H,
y = Y/m + epsilon_C-d*epsilon_H.                  (A3)
```

The displayed rational expressions are integral as wholes because each
ciphertext component minus its centered `m` residue is divisible by `m`.
Changing a compatible pre-rescale phase lift by `Q` times an integer changes
(A3) by `Q'` times an integer; it introduces no surviving term after reduction
modulo `Q'`. This is why an old `Q`-lift multiple need not be bounded. It is not
a license to omit `nu_H`, `nu_L`, or either independent RS2 remainder.

### Canonical candidate bounds and the identification gate

Let `||.||_can` be the maximum over the canonical embeddings. Before modular
centering, it is multiplicative/submultiplicative in the needed direction:

```text
||uv||_can <= ||u||_can ||v||_can.
```

Consequently, given canonical member bounds `PH_a,PL_a,PH_b,PL_b` and explicit
canonical error/carry bounds `K_H,K_L,D_d,D_m`, (A1)--(A3) yield

```text
||X||_can <= PH_a*PH_b+(K_H+D_d)/d,
||Y||_can <= PH_a*PL_b+PL_a*PH_b+K_L+D_d,
||x||_can <= ||X||_can/m+D_m,
||y||_can <= ||Y||_can/m+(1+d)D_m.                (A4)
```

This legitimately removes the coarse coefficient-convolution factor `N` from
the **multiplication terms**. It does not show that modular centering contracts
canonical norm. To transfer (A4) to the actual centered output phases, one must
first establish the member gates

```text
||x||_c < Q'/2,       ||y||_c < Q'/2.             (A5)
```

Then the actual coefficientwise centered phases equal the integer candidates
`x,y`, so their canonical norms equal the quantities bounded in (A4). A5 may
be proved by an independent coefficient bound, or by the inverse canonical
transform inequality `||u||_c<=||u||_can` after that inequality is explicitly
established for this power-of-two cyclotomic normalization and applied to the
unreduced candidates. Either route is non-circular: the candidate exists and
is bounded before it is identified with the ciphertext's centered phase.
Applying a claimed canonical contraction to `C_Q(u)` would instead be circular
and invalid.

Member gates are not the same as the recombined gate

```text
||d*x+y||_c < Q'/2.                               (A6)
```

A6 is additionally needed when the next semantic step identifies the pair's
recombined centered phase with the ordinary integer `d*x+y` (equivalently,
when setting the next input wrap `alpha=0`). A5 does not imply A6. Conversely,
A6 alone does not identify each member and so cannot by itself justify an
inductive canonical bound on `x` and `y`.

### Honest initial canonical interface

For the actual frozen input from
`tests/paper_full_eight_square_oracle.h:97--112`, not an annulus substitute,

```text
R_frozen <= sqrt((1015/1024+16353*2^-75)^2+(1/128)^2).
```

If the separate, presently unproved premise `E_enc<=1/512` is supplied, the
fresh phase has the public candidate bound

```text
P_0 <= S*(R_frozen+E_enc)+N*B_fresh.              (A7)
```

This implies `P_0<S*(255/256)`: use
`sqrt(a^2+b^2)<=a+b^2/(2a)`, `a>1/2`,
`16353*2^-75<2^-61`, hence

```text
R_frozen < 1015/1024+2^-14+2^-61,
N*B_fresh/S < 2^-64,
```

leaving far more than those two tiny terms below `255/256` after adding
`1/512`. This conclusion is conditional on E_enc and the honest successful
PKE support bound; it is not inferred from an endpoint result or from the
different annulus-125/128 experiment.

## B. Draft/checker findings

### Disposition

**Conditional acceptance; no material equation is rejected.** The draft uses
the same compatible candidates as (A1)--(A3), bounds those unreduced candidates
in canonical norm, and checks strict half-modulus gates before identifying them
with actual centered phases. This is a non-circular way to replace the repeated
coefficient-convolution factor `N` on the multiplication terms. It does not
claim that coefficient centering contracts canonical norm.

| Item | Finding | Reason / required qualification |
|---|---|---|
| `X,Y -> h_candidate,l_candidate` bridge | Accept | It matches (A1)--(A3) and `src/double_ckks.cpp:1011--1077,1137--1189`. Both rescale remainder phases remain, and low has `epsilon_C-d epsilon_H`. |
| Removal of multiplication `N` factors | Conditional accept | Canonical evaluation is submultiplicative before reduction. Transfer to actual phases is valid only after the strict candidate member gates. |
| Coefficient-from-canonical gate | Conditional accept | The assertion `||u||_c<=||u||_can` is correct for the full power-of-two canonical embedding: inverse DFT gives each coefficient as a `1/N` average of all embeddings. With CKKS's stored half, conjugacy supplies the omitted half with equal magnitudes. The draft should state this normalization lemma rather than leave “each coefficient is at most” unsupported. |
| Output member gates | Accept | `H_candidate,L_candidate<Q'/2` identify the two integer candidates separately. No canonical-centering contraction is used. |
| Output recombined gate | Accept | `d H_candidate+L_candidate<Q'/2` is separately checked; member nonwrap alone would not imply it. |
| Recombined NW gate | Accept | `dH^2+2HL+2N BK+mN Rm<Q/2` first puts `T*+nu_2` inside `Q/2`, and after division by `m` plus the recombined RS remainder puts its output inside `Q'/2`. Thus both C22 wrap terms are zero by a sufficient, forward bound. |
| Old `Q`-lift terms | Accept as absent after reduction | A compatible lift change is `Q gamma=mQ' gamma`; after the exact component quotient it is `Q' gamma`, so it vanishes modulo `Q'`. It must not be interpreted as literal equality of arbitrary pre-rescale lifts, but no extra bound term survives in the centered output. |
| Same-family `nu_H` | Accept as retained | It appears in `X` as `(nu_H-rho_d)/d`, in recombination once, in `H_candidate` through `N BK/(dm)`, and in the NW gate through one of the two `N BK` terms. No independence or cancellation between `nu_H` and `nu_L` is assumed. |
| Initial `P_0` condition | Conditional accept | The implication from the actual frozen formula, honest PKE support, and `E_enc<=1/512` is sound. `E_enc` is not proved here. Rename the encoded message polynomial in `P_0=m+epsilon`, because `m` is also the per-round Mult prime. |
| Original-input scope | Accept | The checker uses an upper envelope from the source formula and proves it below `127/128`; it does not substitute the annulus-125/128 fixture. Its `16383*2^-75` is looser than the exact maximum `16353*2^-75` but remains a valid bound. |
| Eight Fraction iterations | Accept by static inspection, not independently replayed | Prime consumption, `BK`, the two candidate recurrences, five strict gates, and the final ratio are encoded consistently with the draft. Per instruction this review did not execute the checker, so its recorded exit and JSON numbers remain root-observed evidence. |
| Canonical-centering counterexample | Accept | For the stated `N=4,Q=5` vectors, the supplied exact formula gives squared norms `15` and `10+5 sqrt(2)>15`; it correctly refutes a general contraction shortcut. |

### Why the recipe is non-circular

At round `f`, the induction hypothesis concerns already identified centered
integer phases `h_f,l_f`. Their canonical bounds produce bounds on the ordinary
integer polynomials `X,Y,x,y`; none of those estimates applies a modular
centering map. The inverse-canonical inequality then turns the candidate bounds
into coefficient bounds. Only after comparison with `Q'/2` does the proof use
uniqueness of the centered representative to set the next actual phases equal
to `x,y`. The separately checked `d x+y` gate permits the next semantic
recombined identification. This ordering is forward and does not assume the
conclusion it is trying to prove.

The initial step has the same structure. Take the coherent candidate
`P_0=message+epsilon`, bound its canonical norm by (A7), form the initial DCP
candidates, and use their strict bounds to identify the post-DCP centered
members. An arbitrary full-modulus lift contributes a multiple of the retained
modulus and vanishes after DCP; it is not silently declared zero as an integer.

### Checker-specific limitation

`check_canonical_margin_draft.py:92--94` creates its JSON with mode `"x"`.
Once the recorded result exists, the same checker cannot be replayed without
first moving or deleting that artifact. This does not affect the inspected
algebra, but it is a reproducibility defect in the checker. A minimal correction
is to construct the prospective serialization in memory and either (a) verify
that an existing file is byte-identical, or (b) write only when absent. This
review did neither because it owns only this file and destructive/result edits
were out of scope.

## Remaining assumptions and exact scope

1. `E_enc<=1/512` remains an unproved numerical-analysis premise. The source
   StableRound magnitude guard and two-precision agreement do not imply it.
2. Honest PKE/evaluation keys, coherent small integer lifts, successful finite
   Peikert consumers, and the adopted `BK`, DCP, and Relin2 contracts remain
   premises. Structural validation alone does not establish honest-key
   equations for arbitrary keys.
3. The canonical norm must mean the maximum over the full embedding (or one
   conjugate from every pair with the same magnitudes), with the inverse-DFT
   normalization used in the coefficient inequality above.
4. Every checker comparison is a sufficient strict gate, not a historical
   observation. Failure of such a gate would make this proof route
   inconclusive, not prove production wrap; success remains conditional on the
   algebra and initial premise.
5. The result is a conditional eight-step coefficient/member/recombined
   nonwrap induction. It does not bound the ideal encoding error, prove the
   original E80 accuracy threshold, turn the root JSON into a production test,
   or change the original S100 FAIL disposition.

## Source anchors

- Frozen input: `tests/paper_full_eight_square_oracle.h:97--112`; actual eight
  self-products: `tests/paper_full_eight_square_contract_test.cpp:183--201`.
- Tensor, Relin2, RS2, and RCB:
  `src/double_ckks.cpp:826--890,894--1077,1080--1205,1227--1243`.
- Re-entry performs no arithmetic:
  `src/repeated_mult2.cpp:509--536`.
- Official `Rescale -> ModReduce`:
  `official/src/pke/include/cryptocontext.h:2501--2510`;
  `official/src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp:172--190`.
- Official centered component quotient:
  `official/src/core/include/lattice/hal/default/dcrtpoly-impl.h:691--712` and
  centered `SwitchModulus` as indexed in
  `coordination/relin2-bound-20260908/INDEPENDENT_SOURCE_MAP.md`.
- Adopted Relin2/C25 bridge and qualifications:
  `coordination/relin2-bound-20260908/ADOPTED_CONTRACT.md:39--72`;
  `coordination/relin2-bound-20260908/ROOT_RETURN_REVIEW.md:24--44`;
  `coordination/relin2-bound-20260908/RETURN_MATH_REVIEW.md:110--151`.
