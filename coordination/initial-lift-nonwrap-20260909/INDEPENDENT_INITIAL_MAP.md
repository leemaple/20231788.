# Independent source map: initial encoding, public encryption, and first DCP

## Scope and provenance

This is an independent, read-only map for production source checkpoint
`a4b815a733efe81897325e2a8e4c826a4ebfa439` in the coordination worktree at
`b86105b33294a84ce76d65f585eb16d25ae07156`. The only upstream implementation
read was the newly acquired permitted official OpenFHE mirror at
`/Users/lifeng/Documents/20231788-openfhe-parameter-atlas-20260908/artifacts/reference-sources/parameter-atlas-openfhe-df495ba2`, pinned to
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. No old or quarantined
implementation, build, test, checker, FHE operation, FFT/NTT, sampler, network,
or private runtime value was used.

For compact anchors below, `official/` denotes that permitted mirror root.

This map stops at the first DCP. It supplies the missing initial coefficient
interface for the already adopted Relin2/C25 recurrence; it does not repeat that
proof or infer nonwrap from any passing output.

Notation is coefficientwise in `R=Z[X]/(X^N+1)`:

- `N=32768`, `h=||s||_1=128`, and the exact logical input scale is `S=2^100`;
- `d=1099510054913` is the final full-basis prime;
- `Q=product(q_0,...,q_9)` is the ten-prime post-DCP modulus and `F=dQ` is the
  eleven-prime fresh modulus;
- `C_M(x)` is the coefficientwise centered representative modulo odd `M`;
- `||x||_c` is maximum absolute coefficient.

## Actual frozen call path

1. The fixed input is `paper_full_test::Inputs()`, not a generic annulus or a
   radius-only fixture (`tests/paper_full_eight_square_oracle.h:97–119`). The
   paper endpoint constructs it, creates the fixed plan/key pair, and calls
   `HighPrecisionClientIO::Encrypt` once at scale `2^100`
   (`tests/paper_full_eight_square_contract_test.cpp:258–288`). The evaluator
   then calls `DoubleCKKS::DCP` before the first multiplication
   (`tests/paper_full_eight_square_contract_test.cpp:183–201,302–304`).
2. `HighPrecisionClientIO::Encrypt` runs the same deterministic encoder as
   `InspectEncoding`, converts the exact signed coefficients to a full-`F`
   large polynomial and then to DCRT, and calls the scheme's element/public-key
   overload (`src/high_precision_client_io.cpp:597–623`). The official large
   polynomial conversion reduces every coefficient into every RNS prime
   (`official/src/core/include/lattice/hal/default/dcrtpoly-impl.h:59–68`).
3. `SchemeCKKSRNS` installs `PKECKKSRNS`, which inherits the `PKERNS` encryption
   implementation (`official/src/pke/lib/scheme/ckksrns/ckksrns-scheme.cpp:42–47`;
   `official/src/pke/include/scheme/ckksrns/ckksrns-pke.h:45–75`). The scheme
   wrapper delegates an element/public-key call directly to that implementation
   (`official/src/pke/include/schemebase/base-scheme.h:205–213`).
4. `PKERNS::Encrypt` creates an encryption of zero, changes the supplied message
   element to evaluation format, adds it only to coordinate zero, and returns
   two components (`official/src/pke/lib/schemerns/rns-pke.cpp:56–69`). The
   project then sets CKKS metadata to the frozen fresh state; these setters do
   not multiply the polynomial or alter its phase
   (`src/high_precision_client_io.cpp:623–635`).
5. `DoubleCKKS::DCP` validates the full-basis level/arity/metadata, applies the
   private quotient/remainder helper independently to both ciphertext
   coordinates, and returns high/low ciphertexts on `Q`
   (`src/double_ckks.cpp:370–463`).

## 1. What the encoder proves, and what it does not

For slot index `j`, let `t=floor(j/2)`. The actual frozen value is one of four
rotations/sign changes of the exact dyadic pair

```text
a_j = 1015/1024 - (t mod 16)/65536 + j*2^-75,
b_j = sign_j * (1 + ((t/16) mod 8))/1024.
```

This is the source-defined population of all 16384 slots
(`tests/paper_full_eight_square_oracle.h:97–112`). It has the public bounds

```text
|a_j| <= 1015/1024 + 16353*2^-75,
|b_j| <= 1/128,
R_frozen <= sqrt((1015/1024 + 16353*2^-75)^2 + (1/128)^2).
```

`R_frozen` is a convenient bound on the actual finite family, not a replacement
input distribution.

The encoder uses two separately constructed decimal multiprecision transform
tables (160 and 220 digits), an inverse special transform normalized by 16384,
paper nearest rounding with exact halves rounded downward, and a two-precision
agreement check (`src/high_precision_client_io.cpp:54–57,333–437`). For an
accepted call it establishes the following source facts:

- exact slot count and exact rational scale `S=2^100`;
- finite input and intermediate values;
- both precision paths select the same integer and the check path is farther
  from a half integer than its computed disagreement margin;
- every returned signed coefficient `m_i` satisfies `2|m_i|<F`;
- conversion of `m_i mod F` through the official big-integer type is exact.

The last three checks are at `src/high_precision_client_io.cpp:424–495`.
Therefore, with

```text
m = returned signed encoding polynomial,
M_enc = ||m||_c,
```

the implementation proves `2 M_enc < F` for every accepted call. It also has a
useful, transform-accuracy-independent range check. Write
`epsilon_160=std::numeric_limits<Primary>::epsilon()`. Before rounding each
primary coefficient `p`, `StableRound` requires

```text
max(1,|p|)*epsilon_160 < 2^-410                 (stored-scalar check),
```

and returns an integer whose `Primary` paper rounding agrees with the 220-digit
path (`src/high_precision_client_io.cpp:424–436`). Since paper nearest rounding
has distance at most `1/2`, exact-real interpretation of that check would give

```text
|m_i| < 2^-410/epsilon_160 + 1/2.              (E1-sharp)
```

The permitted project and official mirror do not vendor the Boost
`cpp_dec_float` implementation or a proof of its `numeric_limits`
specialization. A source-only claim should therefore state its scalar-library
assumption. Under the ordinary `cpp_dec_float<160>` contract
`epsilon_160=10^-159`, normal positive arithmetic/comparison, and relative
error below `epsilon_160` for each multiplication/division, a deliberately
coarse allowance for the multiplication and the 410 successive halvings in
`NegativePowerOfTwo` gives

```text
M_enc < 2^-408/epsilon_160 + 1/2 < 2^123.      (E1)
```

The factor-four relaxation from `2^-410` to `2^-408` is only an arithmetic
rounding allowance; no inverse-transform correctness is assumed. (Indeed,
`10^159=(10^3)^53<2^530`.) `m` is the actual centered pre-residue polynomial
used by encryption, not reconstructed from ciphertext or inferred from a
decode.

These guards do **not** constitute a rigorous analytic proof that the computed
roots/transform equal the exact canonical inverse, or that agreement at two
finite precisions implies correctly rounded exact coefficients. Both paths use
the same algorithm and can share systematic error. Thus a statement about the
intended slots needs a separately justified bound

```text
E_enc = ||can(m)/S - z||_slot.
```

If an exact/certified inverse transform is assumed and
`R=max_j |z_j|_2`, unit-root averaging and nearest rounding give the generic
coarse relation `M_enc <= S R + 1/2`; coefficient rounding alone gives canonical
error at most `N/(2S)`. Those are conditional mathematical bounds, not facts
proved by the current dual-precision guards. For the frozen input, the exact
aggregate `M_enc` from the already public deterministic encoding is the cleaner
initial bound; a generic radius must not silently replace it.

## 2. Exact public-encryption error consumers

### Honest public key

The fixed factory samples one sparse signed ternary secret with Hamming weight
128, then calls official private-key `EncryptZeroCore` to make the public key
(`src/paper_h128_client_keypair.cpp:193–217`). On the full basis, official RNS
PKE samples uniform `a` and Gaussian `e_pk` and returns

```text
pk_0 = a*s + e_pk,
pk_1 = -a
```

because the frozen noise scale is one
(`official/src/pke/lib/schemerns/rns-pke.cpp:111–145`; project profile guards at
`src/repeated_mult2.cpp:111–159`). The project validates basis, format, tag, and
secret Hamming structure, but an arbitrary structurally valid external key
would still need the honest hidden equation as a proof assumption.

### Public encryption

Official public-key `EncryptZeroCore` consumes exactly three fresh polynomial
objects relevant to phase error:

- dense ternary `v`, constructed with `TugType` and default `h=0`;
- Gaussian `e_0` in ciphertext coordinate zero;
- Gaussian `e_1` in ciphertext coordinate one.

It returns

```text
c_0 = m + pk_0*v + e_0,
c_1 = pk_1*v + e_1.
```

See `official/src/pke/lib/schemerns/rns-pke.cpp:148–196`. The no-argument TUG
constructor path calls `GenerateIntVector(N,0)`, so each coefficient of `v` is
in `{-1,0,1}` but its Hamming weight is not 128
(`official/src/core/include/lattice/hal/default/dcrtpoly.h:111`;
`dcrtpoly-impl.h:177–193`;
`math/ternaryuniformgenerator-impl.h:103–108`). Deterministically,
`||v||_1<=N`.

The three Gaussian polynomials `e_pk,e_0,e_1` all use the context DGG at the
guarded nominal `3.19F`; there is no OpenMP `private(dgg)` on these PKE paths.
For `std<300`, the official DGG builds a finite Peikert table of length
`ceil(std*12.00610553538285)` and either returns zero or a signed table index;
failure to find one throws (`official/src/core/include/math/discretegaussiangenerator.h:79–93`;
`discretegaussiangenerator-impl.h:52–66,75–114`). Hence every successful frozen
PKE call has

```text
||e_pk||_c, ||e_0||_c, ||e_1||_c <= 39.
```

Each DGG construction samples one signed coefficient vector and maps that same
vector into every RNS tower, so these are coherent small integer polynomials,
not independently chosen residues per prime
(`official/src/core/include/lattice/hal/default/dcrtpoly-impl.h:125–150`). No
independence or typical-size assertion is needed below.

### Fresh phase and deterministic support bound

The public-key `a` terms cancel exactly in the decryption phase. For compatible
full-basis integer lifts `C_0,C_1`, there is an integer polynomial `Gamma` with

```text
C_0 + s*C_1 = m + epsilon + F*Gamma,
epsilon = e_pk*v + e_0 + s*e_1.                 (I1)
```

Negacyclic convolution and `||s||_1=h` give the honest successful-return bound

```text
B_fresh := 39*N + 39 + 39*h
         = 39*(N+h+1)
         = 1,282,983,
||epsilon||_c <= B_fresh.                       (I2)
```

This is a deterministic implementation-support bound. It is not a historical
noise measurement, distribution-quality/security certificate, or claim that
payload/public-key noise used OpenMP's separate default-`1` evaluation-key
case. The bound applies only to the honest factory and successful PKE source
path just traced.

## 3. First DCP: exact representatives and bounds

For each full-basis ciphertext coordinate lift `C_i`, define its centered last
tower residue and quotient

```text
rho_i = C_d(C_i),
K_i   = (C_i-rho_i)/d.
```

`DropLastElementAndScale` coefficient-converts the `d` tower, uses centered
`SwitchModulus`, multiplies by the supplied `-d^-1` and `d^-1` factors, and
drops that tower
(`official/src/core/include/lattice/hal/default/dcrtpoly-impl.h:691–712`;
`official/src/core/lib/math/hal/intnat/mubintvecnat.cpp:98–122`). The project
forms the remainder as `sourcePrefix-d*high`
(`src/double_ckks.cpp:398–437`). Thus DCP returns, coordinatewise,

```text
high_i = [K_i]_Q,
low_i  = [rho_i]_Q,
d*high_i + low_i = [C_i]_Q.                    (I3)
```

Let

```text
rho = rho_0+s*rho_1,
R_d = (1+h)(d-1)/2.
```

Because `d` is odd, `||rho_i||_c<=(d-1)/2` and therefore
`||rho||_c<=R_d`. Importantly, `rho` is the phase of two independently rounded
ciphertext coordinates; it need not equal `C_d(C_0+sC_1)` as an integer lift.

Reducing (I1) modulo `d` proves coefficientwise divisibility
`d | (m+epsilon-rho)`. Define

```text
k = (m+epsilon-rho)/d in R.
```

Then the centered high/low phases after DCP can be written with explicit wrap
polynomials `alpha,beta` as

```text
h_0 = C_Q(k)   = k-Q*alpha,
l_0 = C_Q(rho) = rho-Q*beta,                   (I4)
d*h_0+l_0 = m+epsilon-Q*(d*alpha+beta).        (I5)
```

The original `F*Gamma` in (I1) has become `Q*Gamma` after division by `d` and
therefore disappears in `R_Q`; DCP's `rho` cancels exactly in recombination.
Neither cancellation requires nonwrap.

Centering cannot increase coefficient absolute value, so (I2)–(I4) give the
honest initial C25 interface without observing a secret or noise sample:

```text
H_0 := ||h_0||_c <= (M_enc+B_fresh+R_d)/d,
L_0 := ||l_0||_c <= R_d.                       (I6)
```

For the frozen public constants, `2R_d<Q` follows very coarsely from
`d<2^40`, `1+h=129<2^8`, and the ten retained primes; hence `beta=0` and
`l_0=rho`. This fact is not needed for (I6). The high bound also remains valid
when `alpha` is nonzero.

The DCP metadata records high logical scale approximately `S/d` and recombined
logical scale `S`, while retaining the compatibility scaling-factor field
(`src/double_ckks.cpp:440–457`). The metadata does not establish any of the
phase margins below.

## 4. Wrap ledger and sufficient margins

These are different propositions and must not be conflated:

| Object | Source-established statement | Sufficient centered-lift condition | Necessity |
|---|---|---|---|
| encoded `m` in full `F` | `2M_enc<F` and the stored-scalar range check underlying (E1) are enforced before residue conversion | already enforced | They say nothing about transform accuracy or encryption noise. |
| fresh full-basis phase | congruent to `m+epsilon (mod F)` | `2(M_enc+B_fresh)<F` gives `C_F(m+epsilon)=m+epsilon` | Not needed if the `F` wrap multiple is retained. |
| low DCP member | `l_0=C_Q(rho)` and `||l_0||_c<=R_d` | `2R_d<Q` gives `l_0=rho`; true from frozen public constants | Not needed for its centered coefficient bound. |
| high DCP member | `h_0=C_Q((m+epsilon-rho)/d)` | `2(M_enc+B_fresh+R_d)<F=dQ` guarantees `alpha=0` | Not needed for (I6); track `alpha` otherwise. |
| recombined first pair | `C_Q(dh_0+l_0)=C_Q(m+epsilon)` | `2(M_enc+B_fresh)<Q` selects `m+epsilon` itself | Sufficient, not necessary; it is stronger than the encoder's full-`F` guard. |
| intended CKKS slots | algebra uses `can(m)/S`; encryption adds `can(epsilon)/S` | additionally provide a valid `E_enc`; worst-case `||can(epsilon)||<=N B_fresh` | A passing final decode cannot retroactively prove these premises. |

Thus the full-modulus encoder guard alone does not certify the post-DCP
recombined phase: DCP has removed the factor `d` from the modulus. Conversely,
member nonwrap is not universally necessary for the modular compensation
identity or coefficient recurrence. If the proof selects each member's ordinary
integer quotient, it must establish the indicated member margin; if it selects
only the final recombined representative, it may keep `alpha,beta` until final
centering.

For this frozen profile, (E1) closes all four coefficient margins without
recomputing the encoding, conditional on the stated ordinary scalar semantics
and a successful encoder/public-encryption return. Specifically,
`B_fresh<2^21`, `R_d<2^47`, every retained prime is greater than `2^49`, and
there are ten retained primes, so

```text
M_enc+B_fresh+R_d < 2^124,
2*(M_enc+B_fresh+R_d) < 2^125 < Q < F.
```

The same chain implies the weaker fresh-phase, pair-phase, and low-member
conditions in the table. This is a magnitude/nonwrap implication only; it does
not turn the two finite transforms into a certified approximation of the ideal
canonical inverse.

## Minimal public/aggregate completion interface

No private key, sparse-secret positions, `v`, Gaussian coefficients, ciphertext
components, or reference answer is needed. There are two sufficient public
interfaces. If the ordinary `cpp_dec_float` scalar contract above is accepted,
a successful return plus (E1) supplies `M_bound=2^123` without recomputing or
publishing the encoding. Otherwise one deterministic encoding of the already
frozen public input can supply the exact `M_enc`. The common compact certificate
is:

```text
input_id / source hash; N,h,S,d,Q,F;
M_bound = 2^123;                     // conditional source bound (E1), or
M_bound = M_enc = max_i |m_i|;       // exact integer from InspectEncoding
E_enc;                               // rigorous outward upper bound, only if
                                     // intended-slot accuracy is claimed
B_fresh = 39*(N+h+1);
R_d = (1+h)*(d-1)/2;
H_0_bound = (M_bound+B_fresh+R_d)/d;
L_0_bound = R_d;
full_phase_margin_numerator = F-2*(M_bound+B_fresh);
high_member_margin_numerator = F-2*(M_bound+B_fresh+R_d);
pair_phase_margin_numerator = Q-2*(M_bound+B_fresh);
low_member_margin_numerator = Q-2*R_d.
```

The sign of each integer margin is an exact, auditable decision; no floating
threshold is required. `InspectEncoding` already returns the exact signed
coefficients (`include/openfhe_2023_1788/high_precision_client_io.h:70–80`), but
the proof consumer needs only their maximum, so retaining or publishing the
full vector is unnecessary. The exact maximum is tighter but is not a missing
input for these initial coefficient/margin propositions when (E1)'s scalar
contract is accepted. If `E_enc` is not supplied by a certified transform
analysis, the certificate still closes the initial coefficient bounds (I6) and
all modulus-margin questions, but it must not claim an error bound to the
intended complex slots.

For a generic-radius theorem, replace `M_enc` only after supplying a rigorous
transform-error bound, e.g. `M_enc<=S(R+delta_inv)+1/2`. The exact frozen input
certificate should use its actual deterministic `M_enc`; the annulus-125/128
condition, a different fresh draw, or a passing endpoint is not a substitute.

## Explicit residual assumptions / unknowns

- The ring/format primitives are interpreted according to their official
  source semantics; no low-level transform or modular-arithmetic execution was
  performed here.
- Honest factory generation is required for the public-key cancellation
  equation. Structural basis/tag validation cannot prove it for arbitrary key
  material.
- The finite bound 39 is conditional on the guarded nominal DGG consumer and a
  successful Peikert return. Actual historical coefficients and draws remain
  unknown and are not needed for the worst-case bound.
- `M_enc` for the actual frozen encoding was not computed in this read-only
  slice. StableRound nevertheless supplies the conditional coarse bound (E1),
  which is sufficient for the stronger post-DCP `Q` margin. The exact Boost
  specialization and arithmetic proof are not present in the permitted source;
  without the stated scalar-library contract, an exact aggregate `M_enc`
  remains the minimal substitute.
- Current dual-precision agreement is not a certified exact-transform error
  theorem. `E_enc` remains a separate public numerical-analysis obligation.
- No statement here certifies eight-step nonwrap or changes the original S100
  FAIL. This artifact's complete handoff is (I6), the exact margin ledger, and
  the compact public certificate fields above.

## Source anchor index

| Area | Project / official pristine source |
|---|---|
| Frozen constants and profile guards | `src/repeated_mult2.cpp:23–40,111–159,176–209,443–466` |
| Frozen input and exact scale | `tests/paper_full_eight_square_oracle.h:29–47,78–119`; `tests/paper_full_eight_square_contract_test.cpp:258–303` |
| Encoder, scalar types, and actual guards | `src/high_precision_client_io.cpp:54–57,73–88,315–495,503–506,597–635` |
| Fixed h128 key/public-key construction | `src/paper_h128_client_keypair.cpp:163–217` |
| Initial DCP and scale metadata | `src/double_ckks.cpp:241–279,370–463` |
| CKKS/RNS encryption dispatch | `official/src/pke/lib/scheme/ckksrns/ckksrns-scheme.cpp:42–47`; `official/src/pke/include/scheme/ckksrns/ckksrns-pke.h:45–75`; `official/src/pke/include/schemebase/base-scheme.h:195–213` |
| Private/public `EncryptZeroCore`, message add, phase | `official/src/pke/lib/schemerns/rns-pke.cpp:56–69,111–223` |
| Exact large-poly to DCRT conversion | `official/src/core/include/lattice/hal/default/dcrtpoly-impl.h:59–68` |
| Gaussian and ternary consumers | `official/src/core/include/lattice/hal/default/dcrtpoly.h:111`; `dcrtpoly-impl.h:125–150,177–193`; `official/src/core/include/math/discretegaussiangenerator.h:79–93`; `discretegaussiangenerator-impl.h:52–66,75–114`; `ternaryuniformgenerator-impl.h:103–142` |
| Centered dropped-tower semantics | `official/src/core/include/lattice/hal/default/dcrtpoly-impl.h:691–712`; `official/src/core/lib/math/hal/intnat/mubintvecnat.cpp:98–122` |
