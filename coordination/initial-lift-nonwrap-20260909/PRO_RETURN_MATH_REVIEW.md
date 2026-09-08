# Independent mathematical review of the Pro initial-lift/nonwrap return

## Scope and disposition

Reviewed at coordination `HEAD ae5686c` against unchanged production
`a4b815a733efe81897325e2a8e4c826a4ebfa439`, the permitted official OpenFHE
mirror at `df495ba2e91739a6dc8f1de254fc5a41155ce504`, and the previously adopted
Relin2 contract. I fully read `INITIAL_LIFT_NONWRAP.zh-CN.md`, `CLAIMS.json`,
`SOURCE_MAP.tsv`, and `checks/run_checks.py`; I also read the mathematically
necessary parts of `NEXT_ACTION.md` and `candidate/DIAGNOSTIC_DESIGN.zh-CN.md`
and rechecked the exact project/official source ranges cited below. I did not
execute returned code, a transform, sampler/FHE operation, build, browser, Git,
or CI action.

**Disposition: conditionally accept IL02--IL20 with the two adoption
qualifications in the next section. No core equation (5), (8), (11), (12),
(17), (18), (20), or (21) is rejected.** IL01 and the recorded executions are
accepted only as the verified intake/root receipts describe them, not as work
re-executed in this review. The result remains a conditional nonwrap theorem,
not an E80 PASS, historical-phase observation, production fix, or complete
paper reproduction.

## Material adoption qualifications

### 1. Pro's `127/128` cap and root's `255/256` cap are not interchangeable

Pro assumes

```text
K-CAP: ||sigma(p)||_inf/S0 <= 127/128,            (P-cap)
```

for the actual encoded integer polynomial `p` (`INITIAL_LIFT_NONWRAP.zh-CN.md:
297--326`). The reviewed root bridge instead assumes/bounds

```text
||sigma(p+f)||_inf/S0 <= 255/256,                 (fresh-cap)
```

where `f` is public-encryption error (`ROOT_CANONICAL_MARGIN_DRAFT.md:40--53`).
These are different objects and budgets.

- P-cap plus Pro's deterministic `||sigma(f)||<=E_sigma` implies fresh-cap,
  because `127/128+E_sigma/S0<255/256`.
- Fresh-cap does **not** imply P-cap: `f` may add or cancel, and root's separate
  route `R_frozen+E_enc+E_sigma/S0<255/256` with `E_enc<=1/512` allows an encoded
  magnitude larger than `127/128`. Numerically,
  `R_frozen+1/512` is already greater than `127/128` under the public upper
  envelope.
- Therefore Pro's final `<0.268651182804` margin and root's `<0.734588` margin
  are certificates under non-equivalent seeds. Agreement of their algebra is
  useful, but the numbers must not be presented as independent replays of the
  same hypothesis.

Pro states A04 as unproved and does not silently import root's premise, so this
is an adoption boundary, not a defect in its conditional calculation.

### 2. Preserve the stronger conditional StableRound magnitude implication

Pro deliberately evaluates a **centered-integer-guard-only** seed
`M_c=(F0-1)/2` and shows that this weak sufficient budget cannot certify fresh,
initial high, or initial recombined nonwrap
(`INITIAL_LIFT_NONWRAP.zh-CN.md:96,141--182`; `run_checks.py:361--371`). That
negative budget result is correct but does not exhaust the current encoder's
range checks, as Pro itself says.

The already reviewed independent source map obtains, conditional on ordinary
`cpp_dec_float<160>` `epsilon=10^-159` and the stated normal positive arithmetic
semantics,

```text
||p||c = M_enc < 2^123
```

from `StableRound`'s `max(1,|primary|)*epsilon<2^-410` check and nearest rounding
(`src/high_precision_client_io.cpp:417--436`;
`INDEPENDENT_INITIAL_MAP.md:103--136`). With `E_c<2^21`, `R_d<2^47`, and
`Q0>2^490`, this separately proves all initial coefficientwise fresh/DCP
member/recombined margins with ample room. Thus IL07 may be adopted only as a
failure of its named weak budget; it is not an unresolved initial-margin result
once that scalar-library contract is admitted.

This stronger coefficient cap still does **not** prove Pro's K-CAP. The generic
transfer gives only `||sigma(p)||<=N||p||c<2^138`, or a normalized bound below
`2^38`, far above `127/128`. Pro's proposed canonical certification therefore
answers a genuinely different remaining question.

## Algebra and source findings

### Initial public encryption and DCP

Accept equations (3)--(6), subject to A01--A03.

- Actual `ComputeEncoding` coefficients are shared by inspection and public
  encryption; the direct coefficient guard and exact residue conversion occur
  at `src/high_precision_client_io.cpp:462--495,597--623`.
- Official public encryption adds the message only to coordinate zero and uses
  dense ternary `v` plus `e0,e1`; honest private-key zero encryption supplies
  `pk0=a*s+e_pk, pk1=-a`
  (`official/src/pke/lib/schemerns/rns-pke.cpp:56--69,111--196`). Hence
  `f=e_pk*v+e0+s*e1` and `E_c=39(N+h+1)=1,282,983` follow without independence.
- The successful Peikert implementation returns zero or a signed table index
  no larger than `ceil(std*12.00610553538285)`
  (`official/.../discretegaussiangenerator-impl.h:61--114`), and a DCRT Gaussian
  polynomial maps one signed vector coherently to every tower
  (`official/.../dcrtpoly-impl.h:125--150`). The support-39 use is therefore a
  source-domain bound, not a historical draw or distribution/security claim.
- DCP applies the centered last-tower quotient separately to both coordinates
  and constructs `low=sourcePrefix-d*high`
  (`src/double_ckks.cpp:398--437`; official
  `dcrtpoly-impl.h:691--712`). For compatible lifts,
  `hat m0=(p+f-rho0)/d`, `check m0=rho0`, and `M0=p+f` are integral. A full
  `dQ0` lift multiple becomes a `Q0` multiple after division; no fresh nonwrap
  premise is needed for the modular representatives.

### Eight-step compatible lifts and integrality

Accept equations (8)--(12). They match the earlier independent reconstruction
in `CANONICAL_DRAFT_REVIEW.md:14--65`.

At active `Q_k=mu_k Q_{k+1}`, set

```text
X_k = hat_k^2 + (nu_H-rho_D)/d,
Y_k = 2*hat_k*low_k + nu_L+rho_D.
```

The adopted zero-extra-`d` digit and same-family key-row result is exactly what
makes `d | (nu_H-rho_D)`; `nu_H` is retained rather than spuriously multiplied
by `d` or discarded. Project Tensor2 has high×high and the two positive cross
terms (`src/double_ckks.cpp:826--890`), while Relin2 raises by `d`, relinearizes
both branches, DCPs high, and adds its remainder to low
(`src/double_ckks.cpp:1011--1077`).

Official rescale applies `DropLastElementAndScale` independently to both
ciphertext coordinates (`official/.../ckksrns-leveledshe.cpp:172--190`). Project
RS2 independently rescales high and RCB, then computes
`low_next=RS(RCB)-d*RS(high)` (`src/double_ckks.cpp:1137--1189`). Therefore

```text
hat_{k+1} = X_k/mu_k + epsilon_H,
low_{k+1} = Y_k/mu_k + epsilon_C-d*epsilon_H
```

are integral as complete expressions. A change by `Q_k*J` becomes
`Q_{k+1}*J` after the component quotient; it vanishes only in the new quotient
ring and is not asserted to be zero as an integer. Recombination leaves exactly

```text
M_{k+1}=(M_k^2-low_k^2)/(d*mu_k)
          +(nu_H+nu_L)/mu_k+epsilon_C.
```

No unnoticed `Q`-lift term, second DCP, or extra same-family `nu_H` term
survives. `Reenter` only copies elements/metadata into the next family wrapper
(`src/repeated_mult2.cpp:509--536`).

### Canonical recurrences and gates

Accept equations (17)--(21). With
`a_k=d||sigma(hat_k)||/S_k`, `b_k=||sigma(low_k)||/S_k`, and
`c_k=||sigma(M_k)||/S_k`, direct normalization of the compatible lifts gives

```text
a_{k+1} <= a_k^2 + U_k+d*V_k,
b_{k+1} <= 2*a_k*b_k + U_k+(1+d)*V_k,
c_{k+1} <= c_k^2+b_k^2+2*N*B_k/(mu_k*S_{k+1})+V_k.
```

The low-square term remains in the combined recurrence, each Relin error is
counted once, both member-rescale errors remain where required, and the exact
scale identity is `d*mu_k*S_{k+1}=S_k^2`. The checker implements these formulas
with exact `Fraction` arithmetic and upward `2^-192` rounding
(`run_checks.py:186--249`).

The proof is non-circular. It bounds unreduced compatible integer candidates in
canonical norm, uses the inverse canonical formula
`||u||c<=||sigma(u)||inf`, checks strict half-modulus conditions, and only then
identifies candidates with centered phases. Equations (20)--(21) separately
cover pre-rescale RCB, post-rescale high/low/RCB, and next recursive member and
combined representatives. It never invokes canonical contraction under
coefficient centering. Some auxiliary pre-RS member gates are stronger than
strictly necessary, but passing stronger sufficient gates is sound.

### Frozen input and negative control

- The public loop in `run_checks.py:120--141` implements the exact source input
  formula from `tests/paper_full_eight_square_oracle.h:97--107`. Sign/quadrant
  operations preserve magnitude. The exact maximum at slot 16353 and the bound
  `<127/128` concern the unchanged original input, not annulus-125/128. They do
  not imply K-CAP for the actual finite-precision encoded polynomial.
- K=1 is an analysis-only negative control. Static inspection of its frozen
  result shows round-eight Relin2/RS/recursive recombined gates fail while the
  high/low member gates remain true. It validly refutes “member gates imply the
  recombined gate,” but is neither a new plaintext run nor evidence of actual
  wrap (`run_checks.py:355--359`; `K1_negative_control.json:1757--1773`).
- The ideal-final bound is valid only for the separately defined ideal nearest
  encoding. It cannot identify the actual RCB lift or prove K-CAP/E80.

## Claim-by-claim adoption

| Claims | Disposition |
|---|---|
| IL02--IL06 | Accept in their stated source domain. IL03 remains a deterministic successful-return support bound, not a sample/security statement. |
| IL07 | Accept only with qualification 2 above: the deliberately weak direct-centered-guard budget fails, while the separately adopted conditional StableRound range implication closes initial coefficient margins. |
| IL08--IL10 | Accept under A01--A03 and the adopted Relin2 amendments. Compatible-lift integrality, exact scale, and canonical recurrences are sound. |
| IL11 | Conditional accept exactly under A01--A04. A04 remains null/unproved. Do not substitute root's fresh-cap premise. |
| IL12 | Accept as a conditional sufficient-budget failure at step five, not wrap evidence. |
| IL13 | Accept as an analysis-bound negative control only. |
| IL14 | Accept under A06's ideal encoding definition only. |
| IL15--IL16 | Accept the separation of nonwrap, E80, S100/S116, and annulus histories. Nothing here changes the original S100 FAIL. |
| IL17 | Mathematical status only: the candidate has not executed C++ or a transform. Engineering/code acceptance is outside this review. |
| IL18--IL19 | Accept as explicit unknown/incomplete statuses. Historical use additionally needs A05. |
| IL20 | Accept as the selected bounded way to decide Pro's A04 for one public `p`; “unique” is operational scope, not mathematical necessity. Other sufficient premises, including root's different fresh-cap theorem, remain logically possible. |

## Next-action necessity and historical boundary

The proposed public encoder-cap diagnostic is mathematically sufficient to
instantiate Pro's certificate because it measures the exact missing object
`max |sigma(p)/S0|` and requires neither a key nor ciphertext. Its three-way
CERTIFIED/REFUTED/INCONCLUSIVE interpretation is correct. If it refutes
`127/128`, only this chosen sufficient seed fails; root's looser fresh-cap route
or another sound bound is not thereby refuted.

The diagnostic is not necessary to establish the **initial** coefficient
margins when the conditional StableRound scalar contract is accepted. It is a
targeted way to close Pro's stronger spectral seed for all eight steps. Applying
a future result to an old run additionally requires A05's source/build/input
equivalence; identical profile names or self-reported commits are insufficient.

## Final retained unknowns

1. A04 (`||sigma(p)||/S0<=127/128`) is unproved in this return.
2. A05 historical encoding/build equivalence is unproved.
3. A01's low-level exact ring semantics and honest-key scope remain adopted
   dependencies, not newly formalized guarantees for arbitrary binaries/keys.
4. No actual historical wrap integer or high/low phase was observed.
5. Encoding accuracy relative to the ideal inverse, full-chain error
   propagation tight enough for E80, paper Table 3 provenance/statistics, and
   security remain outside this conditional nonwrap result.

No production or Pro artifact is changed by this review.
