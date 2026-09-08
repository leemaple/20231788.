# Independent Relin2 / HYBRID source map

## Scope and provenance

This is an independent, read-only trace of the project at the assigned
source/evidence checkpoint
`bbd4e73af74d1b072e3beb588cf9c7c4de3117cc` (the repository `HEAD` later
advanced through root-authored documentation only) and the newly acquired,
permitted official pristine OpenFHE 1.5.0 source mirror pinned as `df495ba2` under
`artifacts/reference-sources/parameter-atlas-openfhe-df495ba2`. It is a source
map and a list of proof obligations, not a numerical verdict. No build, test,
sampling, FFT experiment, network access, or other OpenFHE tree was used.

Notation below: `d = q_div` is the final full-basis Q prime, `Q_l` is the
active prefix after dropping `d`, `P` is the HYBRID auxiliary product, and
`Dec_s(c0,c1,...) = c0 + c1*s + ...` in the relevant quotient ring.

## Actual call graph

1. `DoubleCKKS::Mult2` calls `Tensor2`, `Relin2`, then `RS2`
   ([project source](../../src/double_ckks.cpp#L1213-L1224)). `Relin2` is also a
   public operation ([declaration](../../include/openfhe_2023_1788/double_ckks.h#L147-L161)).
2. `Tensor2` produces two degree-2/three-component ciphertexts:
   `T_H = H1*H2` and `T_L = H1*L2 + L1*H2`, with no relinearization
   ([project source](../../src/double_ckks.cpp#L826-L890)).
3. `Relin2` invokes public OpenFHE `Relinearize` exactly twice: once on the
   raised `d*T_H`, once on `T_L`
   ([project source](../../src/double_ckks.cpp#L1011-L1038)). Public
   `CryptoContext::Relinearize` selects the eval-mult-key row by ciphertext tag
   and delegates to the scheme
   ([upstream source](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/pke/include/cryptocontext.h#L2021-L2032)).
4. Upstream relinearization clones the ciphertext, then for its sole `c2`
   component calls `KeySwitchCore(c2, evalKey[0])`, adds the two returned
   polynomials to `c0,c1`, and truncates to two components
   ([upstream source](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/pke/lib/schemebase/base-leveledshe.cpp#L319-L341)).
   The scheme installs `KeySwitchHYBRID` when the context selects HYBRID
   ([upstream dispatch](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/pke/include/schemerns/rns-scheme.h#L68-L76)).
5. Eval-mult-key generation supplies `oldKey=s^2, newKey=s`
   ([upstream source](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/pke/lib/schemebase/base-leveledshe.cpp#L135-L144)).
   The repeated evaluator actually installs one such key in every modulus
   family ([project source](../../src/repeated_mult2.cpp#L347-L377)).

## Relin2 transformations and exact identities

### 1. Raise the high tensor branch from `Q_l` to `Q_l*d`

For every one of the three ciphertext components, the project multiplies all
existing `Q_l` towers by `d`, appends an all-zero evaluation-format tower for
`d`, and changes only level metadata from 1 to 0
([project source](../../src/double_ckks.cpp#L1011-L1028)). Thus the raised
component has residues

`C^(up) mod q_i = d*C mod q_i` for `q_i | Q_l`, and
`C^(up) mod d = 0`.

This is the exact CRT embedding of `d*C` into `R/(Q_l*d)`. Its use as an
integer (rather than modular) lift additionally needs the no-wrap conditions
listed below.

### 2. HYBRID digit expansion (`Q_l -> Q_l*P`)

`KeySwitchCore` is exactly
`EvalFastKeySwitchCore(EvalKeySwitchPrecomputeCore(c2), key, paramsQl)`
([upstream source](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L308-L312)).
The precompute routine:

- partitions the active Q-prefix into `alpha = GetNumPerPartQ()` consecutive
  digits and truncates the final digit at the current level;
- copies the original `c2` residues on that digit;
- changes the digit to coefficient format;
- calls `ApproxSwitchCRTBasis` from the digit basis to all other active-Q and P
  towers; and
- assembles each result in ordered `Q_l || P` and returns it to evaluation
  format
  ([upstream source](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L314-L378)).

Coefficientwise, `ApproxSwitchCRTBasis` computes

`S_B(x) = sum_i ([x_i * (B/b_i)^(-1)]_{b_i}) * (B/b_i)`,

then reduces `S_B(x)` into each target modulus
([implementation](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/core/include/lattice/hal/default/dcrtpoly-impl.h#L888-L931)).
Consequently `S_B(x) = x + alpha_x*B` for a chosen lift; upstream explicitly
describes the returned value this way, with a “small alpha,” but does not
compute a correcting `alpha_x`
([interface contract](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/core/include/lattice/hal/dcrtpoly-interface.h#L905-L931)).
This is deterministic approximate basis extension, not nearest rounding.

### 3. Eval-key product and approximate mod-down (`Q_l*P -> Q_l`)

For each full-Q partition `j`, HYBRID key generation samples uniform `a_j` and
Gaussian `e_j` and stores, at a Q tower inside partition `j`,

`b_j = -a_j*s + e_j + P*s^2`,

while at every other Q/P tower it stores

`b_j = -a_j*s + e_j`.

For CKKS the noise scale multiplying `e_j` is one
([key generation](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L85-L129),
[CKKS setup](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-internal.h#L124-L126)).

The expanded digit `D_j` is multiplied by `(b_j,a_j)` and accumulated as
`(W,V)` in `Q_l || P`. For a reduced active prefix, Q indices are used directly
and P indices are shifted past the dropped full-Q suffix
([upstream source](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L402-L435)).
Because the original `c2` residue is retained on its own partition, decrypting
the accumulator gives the exact modular structure

`W_Q + V_Q*s = P*c2*s^2 + E_Q`,

`W_P + V_P*s_P = E_P`, where `E = sum_j D_j*e_j` and `s_P` is the
key-generation extension of the secret to P.

Each of the two accumulated components is then independently passed through
`ApproxModDown`
([upstream calls](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L381-L400)).
For CKKS (`t=0`), that routine coefficient-converts the P part, approximately
extends it to Q, and returns

`AMD(X) = (X_Q - S_P(X_P)) * P^(-1) mod Q`

([implementation](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/core/include/lattice/hal/default/dcrtpoly-impl.h#L966-L1004)).
Because OpenFHE applies `AMD` separately to `W` and `V`, HYBRID
relinearization satisfies the modular relation

`Dec_s(Relin_Q(c)) = c0 + c1*s + c2*s^2 + eta_Q (mod Q)`,

with

`eta_Q = P^(-1)*(E_Q - S_P(W_P) - S_P(V_P)*s) mod Q`.

In general this must **not** be simplified to
`P^(-1)*(E_Q-S_P(E_P))`: the approximate basis switch chooses lifts
componentwise and need not commute with addition or multiplication by `s` as
an integer-lift operation. The exact definition exposes both sources that a
bound must cover: the digit lifts inside `W,V,E` and the two independently
chosen P-to-Q lifts. The upstream interface promises only an approximate
`X/P` result ([contract](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/core/include/lattice/hal/dcrtpoly-interface.h#L962-L996)); it contains no executable magnitude bound.

There are two digit-expansion sequences in Relin2 (full `Q_l*d` for the raised
high branch, reduced `Q_l` for the low branch) and four `ApproxModDown` calls
(two output components per relinearization). The digit partitions, final digit,
and deterministic lift errors can differ between those two levels.

### 4. Componentwise centered quotient/remainder (“carry”)

After relinearizing the raised high branch on the full basis, Relin2 calls the
same private DCP helper used by public DCP
([call](../../src/double_ckks.cpp#L1035-L1052),
[helper](../../src/double_ckks.cpp#L398-L438)). For every ciphertext component
`A_j` it uses factors `d^(-1) mod q_i` and `-d^(-1) mod q_i`, invokes
`DropLastElementAndScale`, and then constructs

`h_j = (A_j - r_j)/d mod Q_l`, `r_j = centered(A_j mod d)`,

so `d*h_j + r_j = A_j mod Q_l` exactly. The equivalence of the supplied
`q_i-d^(-1)` factor to upstream’s standard rescale factor follows from the
upstream precomputation
([precomputation](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/pke/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp#L62-L83)).
`DropLastElementAndScale` coefficient-converts the last tower, switches its
modulus, multiplies by those two factors, and drops the tower
([implementation](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/core/include/lattice/hal/default/dcrtpoly-impl.h#L691-L712)).
`SwitchModulus` interprets residues strictly above `floor(d/2)` as negative
([implementation](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/core/lib/math/hal/intnat/mubintvecnat.cpp#L98-L122)).
Because Relin2 requires odd `d`, each coefficient has the unique bound
`|r_j| <= (d-1)/2` with no tie
([constructor check](../../src/double_ckks.cpp#L255-L271)).

Relin2 sets `out.high=(h_0,h_1)` and
`out.low=(r_0+B_0,r_1+B_1)`, where `B=Relin_Ql(T_L)`
([project source](../../src/double_ckks.cpp#L1040-L1077)). RCB later computes
`d*out.high + out.low` componentwise
([project source](../../src/double_ckks.cpp#L1227-L1243)). Hence the exact
compensation identity is

`RCB(Relin2(T_H,T_L)) = Relin_(Q_l*d)(raise_d(T_H))|_(Q_l) + Relin_(Q_l)(T_L) (mod Q_l)`.

Writing the two HYBRID errors as `eta_H, eta_L`, decryption gives

`Dec_s(RCB(out)) = d*Dec_s(T_H) + Dec_s(T_L) + eta_H + eta_L (mod Q_l)`.

At pair level define `rho = r_0 + r_1*s`. Then, for compatible chosen lifts,

`Dec_s(out.high) = Dec_s(T_H) + (eta_H-rho)/d`,

`Dec_s(out.low)  = Dec_s(T_L) + eta_L + rho`.

This is the actionable carry-compensation identity. The `rho` terms cancel
only after recombination.

## Preconditions a decrypted error/nonwrap lemma must justify

These are stronger than what runtime validation proves:

1. **Honest eval key and secret lift.** Relin2 checks tag, context, subtype,
   vector length, full ordered `Q||P` basis, and evaluation format
   ([project checks](../../src/double_ckks.cpp#L905-L1009)); it cannot verify
   `b+a*s=P*s^2+e`. Key generation extends `s` to P by coefficient-converting
   only its first Q tower and centered modulus-switching it
   ([upstream source](../../artifacts/reference-sources/parameter-atlas-openfhe-df495ba2/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L54-L83)).
   A proof must assume an honestly generated small secret whose centered
   coefficients are represented consistently in every Q/P prime, plus an
   explicit tail event/norm bound for every sampled `e_j`.
2. **No independence shortcut.** Both Relin2 calls reuse the same eval key.
   Their `eta_H` and `eta_L` are deterministic functions of correlated tensor
   inputs and the same `e_j`; neither independence nor cancellation follows
   from source. Bound their sum jointly or by a worst-case norm inequality.
3. **Component carry versus decrypted remainder.** DCP divides `A_0` and `A_1`
   separately. Thus `rho=r_0+r_1*s`, not generally
   `centered(Dec_s(A) mod d)`. A coefficient sup-norm bound available directly
   from source is
   `||rho||_inf <= (d-1)/2 * (1 + ||s||_1)` (using the negacyclic convolution
   inequality). Any lemma using only `(d-1)/2` after decryption needs an
   additional argument. For a ternary secret of Hamming weight `h`, the stated
   bound specializes to `(d-1)(1+h)/2`.
4. **Approximate basis-conversion constants.** Prove concrete coefficient/canonical-
   embedding bounds for every digit lift `D_j`, the accumulators `W,V`,
   `E=sum D_j e_j`, and the two separate lifts `S_P(W_P),S_P(V_P)`. The source
   description “small alpha” is not a quantified theorem or runtime check.
   Linearity of the chosen integer lifts must not be assumed. The current
   active-Q prefix and truncated last partition must be used, not automatically
   the full-Q partition count.
5. **Integer lifts and nonwrap.** All identities above are unconditional only
   modulo their rings. A sufficient route to the displayed centered-lift
   equations for each pair member is to select compatible centered lifts and
   establish nonwrap for `d*Dec(T_H)+eta_H` in `Q_l*d` and
   `Dec(T_L)+eta_L` in `Q_l`; componentwise integer quotient claims additionally
   need component-lift control. Those intermediate nonwrap conditions are not
   universally necessary if a proof instead tracks the relevant wrap multiples
   explicitly and selects only the final representative. The recombined modular
   identity needs no nonwrap assumption; interpreting its final representative
   as a CKKS decrypted-error inequality normally requires a strict half-modulus
   bound for `d*Dec(T_H)+Dec(T_L)+eta_H+eta_L` in `Q_l`, in the norm used by the
   decoder.
6. **No runtime noise-budget guard.** The check
   `active_towers >= noiseScaleDegree`
   ([project source](../../src/double_ckks.cpp#L901-L904)) is structural. Neither
   Relin2 nor upstream HYBRID code measures ciphertext/error norms, basis-lift
   errors, or half-modulus distance. Any nonwrap conclusion is therefore a
   theorem precondition, not an observed runtime property.

## Explicit unknowns / next proof inputs

- A quantified norm bound (with failure probability) for `eta_H+eta_L` under
  the exact OpenFHE discrete-Gaussian sampler and the two active digit layouts
  is not present in the traced source.
- The norm-transfer constant from coefficient/canonical-embedding error to the
  project’s final CKKS slot error is not established here.
- The intended lemma must state whether it bounds only the recombined output or
  each pair member. The former eliminates `rho` exactly; the latter must retain
  and bound `rho`.
- The lemma should name the exact stage(s) at which centered no-wrap is needed;
  a single final-modulus assumption does not by itself justify intermediate
  integer quotient interpretations.
- Actionable proof order: (i) formalize the modular HYBRID `eta_Q` identity
  above for a current Q-prefix, (ii) quantify digit/base-extension and Gaussian
  terms without independence between the two calls, (iii) add the exact `rho`
  compensation, and (iv) discharge intermediate and final half-modulus bounds.
