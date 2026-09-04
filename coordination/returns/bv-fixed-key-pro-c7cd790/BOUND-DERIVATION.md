# BOUND DERIVATION — fixed-key BV `digitSize=0`

## 1. Status and scope

This document derives a bound for the exact pinned OpenFHE 1.5.0 BV key-switch structure supplied in the packet. It does **not** import the paper's Relin near-additivity premise into BV.

The result has three distinct quantifiers:

1. **Fixed-key, ciphertext-uniform result:** deterministic for one already-generated evaluation key, one ordered basis, and every ciphertext third component in the declared domain.
2. **Across-key deterministic result:** only the trivial centered modular bound is unconditional; there is no finite probability-one raw integer-noise bound for an unbounded discrete Gaussian.
3. **Across-key probabilistic result:** possible only after fixing a tail function and failure probability. No numerical DGG tail parameters sufficient for that calculation are supplied here.

The nontrivial fixed-key result is conditional on the exact all-residue semantics of `NativePoly::SwitchModulus`. The packet shows the call site but not that implementation or a normative postcondition. The candidate therefore remains a probe.

## 2. Notation and exact fixture

Let

\[
R_M=\mathbb Z_M[X]/(X^N+1).
\]

The current fixture has `N=64`, active basis

\[
I=\{0,1,\ldots,6\},\qquad Q_\ell=\prod_{i=0}^{6}q_i,
\]

and full raised-high/key basis

\[
Q^+=Q_\ell q_{\mathrm{div}}.
\]

Exact ordered values:

| row | modulus `q_i` | exact centered digit radius `D_i=floor(q_i/2)` | `N*D_i` |
|---:|---:|---:|---:|
| 0 | 34,359,736,577 | 17,179,868,288 | 1,099,511,570,432 |
| 1 | 1,073,744,257 | 536,872,128 | 34,359,816,192 |
| 2 | 1,073,738,753 | 536,869,376 | 34,359,640,064 |
| 3 | 1,073,742,721 | 536,871,360 | 34,359,767,040 |
| 4 | 1,073,739,649 | 536,869,824 | 34,359,668,736 |
| 5 | 1,073,742,209 | 536,871,104 | 34,359,750,656 |
| 6 | 1,073,741,441 (`q_l`) | 536,870,720 | 34,359,726,080 |

Additional exact values:

```text
q_div = 1073741953
Q_l = 52656049226897061758347970843194892279389197066160739197584863617
bit_length(Q_l) = 215
q_div*q_l = 1152921231876374273
2^60/(q_div*q_l) = 1.000000236556032764816331812324746...
sum_i D_i = 20401092800
N*sum_i D_i = 1305669939200
```

Every active modulus and `q_div` is odd, and `gcd(q_div,q_i)=1` for all seven active rows. These values were recomputed by `evidence/static_witnesses.py`; output is in `evidence/static-witnesses.txt`.

The parameter fixture is `p=30`, first modulus size 35, multiplicative depth 7, `FIXEDMANUAL`, `digitSize=0`, `maxRelinSkDeg=2`, `UNIFORM_TERNARY`, and `HEStd_NotSet`. The actual secret Hamming weight `h` is key-dependent; retained green values were 48/46 on Linux BV REAL/COMPLEX and 46/48 on Windows BV REAL/COMPLEX. The candidate measures `h` for each new key.

### 2.1 Assumption ledger

The derivation uses the following assumptions explicitly; none is hidden inside the symbol `E_Relin`.

- **A1 — CRT field/ring basis.** The listed moduli are positive, odd, and pairwise coprime; coefficientwise CRT identifies the tower representation with `R_{Q_I}`. The exact pairwise gcds were checked.
- **A2 — ring law.** Polynomial multiplication is negacyclic modulo `X^N+1`; an output coefficient is a signed sum of exactly `N` products.
- **A3 — representation conversion.** `SetFormat`/NTT conversion preserves the represented ring element, so coefficient-form independent multiplication can be compared with evaluation-form OpenFHE operations.
- **A4 — basis projection.** Dropping the final DCRT towers is the canonical ring projection from the full ordered CRT basis to its active prefix, with no independent rounding term.
- **A5 — fixed key and identity.** The secret key, key tag, context identity, and evaluation key are fixed throughout one certificate; the eval-key rows implement `s^2 -> s` and are not mutated by the oracle.
- **A6 — centered BV digit lift.** For `digitSize=0`, each source-tower residue is re-emitted on every target tower as the same centered integer in `[-floor(q_i/2),floor(q_i/2)]`. This is the only material premise not closed by the supplied source set.
- **A7 — zero lift.** `SwitchModulus(0)=0`, so the appended zero `q_div` tower creates an allocated but noncontributing digit. The candidate checks this on the actual raised-high third component.
- **A8 — finite modular residual versus raw noise.** The measured `rho_i` is a centered representative modulo the declared basis. It is not identified with the unbounded raw DGG sample unless a separate raw-noise/no-wrap event is established.
- **A9 — integer-lift use.** Any step that treats a modular congruence as an ordinary integer/rational equality is conditioned on the separate half-modulus inequalities in Section 13. Modular triangle inequalities alone are not used for that purpose.
- **A10 — exact test arithmetic.** The candidate's `cpp_int` CRT and schoolbook products do not overflow; conversions to native integers are used only for fixture residues that fit the pinned native moduli.

## 3. Source-derived evaluation-key relation

### 3.1 Key direction

`EvalMultKeyGen` creates `s^2` and invokes key switching from that old key to the original key `s` (`official-openfhe/base-leveledshe.cpp:136-144`). Thus, for multiplication relinearization,

\[
s_{\mathrm{old}}=s^2,\qquad s_{\mathrm{new}}=s.
\]

### 3.2 BV row construction and sign

For `digitSize=0`, OpenFHE creates one evaluation-key row for every old-key tower. At row `i` it:

- samples `a_i` uniformly;
- initializes `b_i` to zero;
- puts the old-key tower at index `i`;
- subtracts `a_i*s + ns*e_i`.

See `official-openfhe/keyswitch-bv.cpp:49-103`, especially lines 86-95.

Define the CRT gadget row `G_i(s^2)` by

\[
G_i(s^2)_j=
\begin{cases}
s_j^2,&j=i,\\
0,&j\ne i.
\end{cases}
\]

Then, in `R_{Q^+}`,

\[
b_i=G_i(s^2)-a_i s-n_s e_i,
\]

so

\[
\boxed{b_i+a_i s-G_i(s^2)\equiv -n_s e_i\pmod {Q^+}.}
\]

The key-switch error sign is negative relative to the raw DGG sample. The sign disappears under an absolute norm, but the factor `n_s` does not: it is already included if the row residual itself is measured.

## 4. Source-derived `digitSize=0` key switch

`CryptoContext::Relinearize` retrieves the evaluation multiplication key and dispatches to scheme relinearization (`official-openfhe/cryptocontext.h:2021-2031`). Scheme relinearization invokes `KeySwitchCore` on component `c2`, adds the two returned polynomials to `c0,c1`, and truncates the ciphertext to two components (`official-openfhe/base-leveledshe.cpp:327-340`).

BV `KeySwitchCore` does the following (`official-openfhe/keyswitch-bv.cpp:245-278`):

1. compute `digits = c2.CRTDecompose(digitSize)`;
2. copy evaluation-key rows;
3. drop each row's final towers until its basis matches the input basis;
4. multiply row `i` by digit `i`;
5. sum all contributing rows.

For `digitSize=0`, `CRTDecompose` returns exactly one digit per input tower. Digit `i` retains source tower `i`; every other tower is produced by calling `SwitchModulus` on the coefficient-form source tower (`official-openfhe/dcrtpoly-impl.h:230-250`).

Let `d_i(x)` denote digit `i` for input third component `x`. At target tower `j`, only gadget row `G_j` is nonzero, and digit `j` at its own source tower is exactly `x_j`. Therefore the gadget recomposition is exact:

\[
\sum_i d_i(x)G_i(s^2)=x s^2\quad\text{in }R_{Q_I}.
\]

If `(u_0,u_1)` is the output of `KeySwitchCore(x,K)`, then the relinearization error is

\[
\begin{aligned}
\operatorname{Err}_{K,I}(x)
&=(u_0+u_1s)-xs^2\\
&=\sum_i d_i(x)(b_i+a_i s-G_i(s^2))\\
&\equiv -n_s\sum_i d_i(x)e_i\pmod {Q_I}.
\end{aligned}
\]

This is the actual BV error equation. It is not the paper's rounded-Relin near-additivity equation.

## 5. Basis-specific measured row residual

For an active basis `I`, let `pi_I` be the CRT projection that drops all later towers. Define

\[
\rho_i^{(I)}=
\operatorname{Center}_{Q_I}
\left(
\pi_I(b_i)+\pi_I(a_i)\pi_I(s)-\pi_I(G_i(s^2))
\right).
\]

`Center_{Q_I}` is applied coefficientwise after exact CRT reconstruction into the interval

\[
[-\lfloor Q_I/2\rfloor,\lfloor Q_I/2\rfloor].
\]

Then

\[
\rho_i^{(I)}\equiv -n_s\pi_I(e_i)\pmod {Q_I}.
\]

Set

\[
R_i^{(I)}=\|\rho_i^{(I)}\|_\infty.
\]

This is a **finite centered modular residual**. It is not necessarily the unbounded raw integer Gaussian polynomial. It is nevertheless sufficient for a fixed-key modular error bound when combined with an established digit lift and a total no-wrap condition.

The residual is basis-specific. In particular,

\[
\operatorname{Center}_{Q^+}(r)\quad\text{and}\quad
\operatorname{Center}_{Q_\ell}(\pi_I r)
\]

need not be the same integer polynomial. The relevant candidate therefore drops the `q_div` tower first and reconstructs each row in `Q_l`.

## 6. The centered-digit premise

The useful bound requires the following contract for every source modulus `q_i`, every source residue `r`, every coefficient, and every target tower:

\[
L_i(r)=
\begin{cases}
r,&0\le r\le \lfloor q_i/2\rfloor,\\
r-q_i,&\lfloor q_i/2\rfloor<r<q_i,
\end{cases}
\]

and the target-tower residue is `L_i(r) mod q_j`.

Under this contract,

\[
\|d_i(x)\|_\infty\le D_i,
\qquad
D_i=\left\lfloor\frac{q_i}{2}\right\rfloor.
\]

All fixture moduli are odd, so `D_i=(q_i-1)/2`. ZCode's `ceil(q_i/2)=(q_i+1)/2` is safe but not exact.

### 6.1 Why the residual alone is insufficient

Take `q_i=3` and residue `r=2`.

- centered lift: `-1`, whose magnitude is at most `floor(3/2)=1`;
- canonical nonnegative lift: `2`, whose magnitude exceeds 1.

Both are congruent to residue 2 modulo 3. Thus the evaluation-key residual does not determine the digit magnitude. The `SwitchModulus` lift semantics are a separate premise.

The supplied source contains the call at `official-openfhe/dcrtpoly-impl.h:243-246`, but not `NativePoly::SwitchModulus` itself. This is the precise source gap.

## 7. Fixed-key ciphertext-uniform theorem

### Theorem 1 — basis-specific BV key-switch bound

Fix:

- an ordered active basis `I` of pairwise-coprime odd moduli;
- ring dimension `N`;
- secret key `s` and BV evaluation key `K` generated for `s^2 -> s`;
- `digitSize=0`;
- the centered-digit contract in Section 6.

For each effective row `i`, let

\[
D_i=\lfloor q_i/2\rfloor,
\qquad
R_i=\|\rho_i^{(I)}\|_\infty.
\]

Define

\[
\boxed{
B_I(K)=N\sum_{i\in I}D_iR_i.
}
\]

Then for **every** `x in R_{Q_I}`,

\[
\left\|
\operatorname{Center}_{Q_I}(\operatorname{Err}_{K,I}(x))
\right\|_\infty
\le
\min\left(\left\lfloor\frac{Q_I}{2}\right\rfloor,B_I(K)\right).
\]

If

\[
B_I(K)<Q_I/2,
\]

then the constructed integer error representative is unique and no key-switch-error wrap occurs.

### Proof

For every row `i`, choose the centered coefficient representatives of `d_i(x)` and `rho_i`. The centered-digit premise gives

\[
\|d_i(x)\|_\infty\le D_i,
\qquad
\|\rho_i\|_\infty=R_i.
\]

In the negacyclic ring, each output coefficient of a product is a signed sum of exactly `N` coefficient products. Therefore

\[
\|d_i(x)\rho_i\|_\infty\le ND_iR_i.
\]

Summing rows gives an integer representative `z(x)` of the modular key-switch error with

\[
\|z(x)\|_\infty
\le N\sum_iD_iR_i=B_I(K).
\]

The centered representative of any residue has norm at most `floor(Q_I/2)`, yielding the stated minimum. If `B_I(K)<Q_I/2`, every coefficient of `z(x)` already lies strictly inside the centered interval, so reduction and centering return the same integer coefficient. The representative is then unique. QED.

The factor `N` is necessary in general. The included exact exhaustive toy witness uses `N=2`, moduli 3 and 5, and two fixed residual rows. It attains the bound 6, while omitting `N` gives 3 and fails (`evidence/static-witnesses.txt`).

## 8. Raised-high and prefix-low specialization

### 8.1 Low path

The low third component is in `R_{Q_l}`. `CRTDecompose(0)` returns seven digits; `EvalFastKeySwitchCore` projects the full key rows to the same seven-tower prefix (`official-openfhe/keyswitch-bv.cpp:257-276`). Therefore Theorem 1 applies directly:

\[
B_L=B_{Q_\ell}(K).
\]

### 8.2 Raised-high path

Production multiplies the high third component by `q_div` in every active tower and appends an all-zero `q_div` tower (`project/src/double_ckks.cpp:807-818`). It then relinearizes on `Q^+` (`project/src/double_ckks.cpp:821-829`).

The full decomposition has eight digits. The eighth source tower is zero, so its digit is zero if `SwitchModulus(0)=0`; the candidate checks this on the actual raised-high `c2`. After projection back to `Q_l`, only rows 0 through 6 remain:

\[
\pi_I(\operatorname{Err}_{K,Q^+}(x_H))
=\sum_{i=0}^{6}d_i(x_H)\rho_i^{(Q_\ell)}.
\]

Since `gcd(q_div,q_i)=1`, multiplication by `q_div` is bijective modulo every active `q_i`. Thus the raised-high active digits range over the same worst-case domains as arbitrary active residues; no smaller `D_i` is justified.

Hence

\[
B_H=B_{Q_\ell}(K).
\]

This is a bound on the full-call error **after projection to the active basis**, which is the error relevant to the recombined pair. It is not a claimed centered-integer bound on the full `Q^+` error including the final tower.

## 9. ZCode formula: exact interpretations

### 9.1 If `e_i` means raw DGG integer noise

If the actual raw integer DGG samples are available and satisfy

\[
E_i=\|e_i\|_\infty,
\]

then

\[
B_I^{\mathrm{raw}}=n_sN\sum_iD_iE_i
\]

is a raw representative bound. If it is below `Q_I/2`, it also establishes no-wrap. This is the closest exact version of ZCode's formula; use `floor`, not `ceil`, for the tight centered digit radius.

The supplied test inputs do not expose or replay the raw Gaussian samples, so this form cannot be instantiated independently here.

### 9.2 If “measured error polynomial” means the key row residual

If the measured object is

\[
\rho_i=b_i+a_i s-G_i(s^2),
\]

then `rho_i` already contains `-n_s e_i` modulo the basis. The exact form is

\[
B_I(K)=N\sum_iD_i\|\rho_i\|_\infty.
\]

Applying `n_s` again double-counts the key-generation scale. It may remain numerically conservative when `n_s>=1`, but it is not the exact derivation and obscures whether the norm refers to raw integer noise or a centered modular residual.

## 10. Fixed key versus Gaussian-key quantification

### 10.1 Deterministic fixed-key statement

After `K` is generated, all `rho_i`, `R_i`, and `B_I(K)` are fixed. Subject to the digit contract, the bound holds for every ciphertext third component in the declared domain. This is an a-posteriori-on-key, a-priori-on-ciphertext certificate.

### 10.2 No nontrivial unconditional raw-noise bound

A discrete Gaussian has unbounded integer support. Consequently, no finite `T` can satisfy

\[
\Pr[\|e_i\|_\infty\le T]=1
\]

for all generated keys. A finite probability-one raw-noise bound therefore does not exist.

There is always the trivial centered modular statement

\[
\|\operatorname{Center}_{Q_I}(z)\|_\infty\le\lfloor Q_I/2\rfloor,
\]

but that cannot establish no-wrap or the intended integer lift.

### 10.3 Generic probabilistic statement

Let a single raw DGG coefficient `E` have tail

\[
\tau(T)=\Pr[|E|>T].
\]

For `L` effective rows and `N` coefficients per row, the union bound gives

\[
\Pr\left[\max_{i,k}|e_{i,k}|\le T\right]
\ge 1-LN\tau(T).
\]

No independence assumption is required for this inequality. For a desired failure probability `delta`, choose `T` such that

\[
LN\tau(T)\le\delta.
\]

On that event, if `n_sT<Q_I/2`,

\[
R_i\le n_sT,
\]

and

\[
B_I(K)\le n_sNT\sum_iD_i.
\]

A useful theorem additionally requires this bound and the downstream target-plus-error bounds to remain below their half-modulus limits. The packet does not supply a pinned numerical tail function, sampler parameter interpretation, or target `delta`, so no numerical probability is claimed.

## 11. Pair bound and the role of `h`

Let `epsilon_H` and `epsilon_L` be the projected high-path and low-path key-switch errors. Production's private DCP/remainder step preserves recombination exactly, so

\[
\epsilon_{\mathrm{pair}}
\equiv\epsilon_H+\epsilon_L\pmod {Q_\ell}.
\]

The modular centered norm is subadditive, and the direct raw bounds give

\[
\boxed{
B_{\mathrm{pair}}=B_H+B_L=2B_{Q_\ell}(K).
}
\]

For a unique integer pair-error lift, require

\[
B_{\mathrm{pair}}<Q_\ell/2.
\]

No additional `+h` belongs in this expression. Paper Lemma 4.4 obtains `+h` by comparing two separate Relin calls with one combined Relin call and introducing an integral rounding residual `e` with `||e||<=1`, then bounding `||s e||<=h` (`PAPER-2023-1788.txt:735-777`). The direct BV path bound never makes that comparison.

Adding `+h` to `B_H+B_L` would be harmless positive slack, but it would double-count no derived BV operation and should not be presented as required.

## 12. Final Mult2 coefficient bound

For each input pair, write

\[
m_j=q_{\mathrm{div}}\widehat m_j+\check m_j,
\]

with

\[
\|\widehat m_j\|_\infty\le M_{\mathrm{high}},
\qquad
\|\check m_j\|_\infty\le M_{\mathrm{low}}.
\]

Tensor2 omits the low-low product, so its recombined decryption is

\[
t=\frac{m_1m_2-\check m_1\check m_2}{q_{\mathrm{div}}}.
\]

After direct fixed-key Relin2 bounding,

\[
r=t+\epsilon_{\mathrm{pair}}.
\]

RS2 introduces the independent rounding term `zeta` with

\[
\|\zeta\|_\infty\le\frac{h+1}{2}
\]

from Lemma 4.6 (`PAPER-2023-1788.txt:817-891`). Therefore

\[
\begin{aligned}
y-\frac{m_1m_2}{q_{\mathrm{div}}q_\ell}
&=-\frac{\check m_1\check m_2}{q_{\mathrm{div}}q_\ell}
+\frac{\epsilon_{\mathrm{pair}}}{q_\ell}+\zeta,
\end{aligned}
\]

and

\[
\boxed{
\left\|y-\frac{m_1m_2}{q_{\mathrm{div}}q_\ell}\right\|_\infty
\le
\frac{NM_{\mathrm{low}}^2}{q_{\mathrm{div}}q_\ell}
+
\frac{B_{\mathrm{pair}}}{q_\ell}
+
\frac{h+1}{2}.
}
\]

In exact integer numerator form,

\[
2q_{\mathrm{div}}q_\ell\cdot\mathrm{error}
\le
2NM_{\mathrm{low}}^2
+2q_{\mathrm{div}}B_{\mathrm{pair}}
+q_{\mathrm{div}}q_\ell(h+1).
\]

The normalization is explicitly `1/(q_div*q_l)`. The apparent missing `1/q_div` in the printed Theorem 4.8 target remains an independently derived algebraic discrepancy, not an author-confirmed erratum (`PAPER-2023-1788.txt:899-945`).

## 13. Separate no-wrap conditions

A centered modular triangle inequality is not a no-wrap proof. Exact witness:

```text
Q = 101
Center_Q(40 + 40) = -21
|-21| = 21 <= 80
```

The triangle inequality passes despite wrap.

For the intended integer/rational proof, use separate conditions.

### 13.1 Key-switch error lift

\[
B_H<Q_\ell/2,\qquad B_L<Q_\ell/2,
\qquad B_{\mathrm{pair}}<Q_\ell/2.
\]

### 13.2 Pre-RS target plus pair error

Let

\[
A=M_{\mathrm{high}}q_{\mathrm{div}}+M_{\mathrm{low}}.
\]

A tight sufficient form is

\[
\frac{NA^2}{q_{\mathrm{div}}}+B_{\mathrm{pair}}<Q_\ell/2.
\]

The candidate deliberately retains the stronger existing paper-style preselection condition

\[
NA^2+B_{\mathrm{pair}}<Q_\ell/2.
\]

### 13.3 Final output target plus error

Since `Q_{l-1}=Q_l/q_l`, require

\[
\frac{NA^2}{q_{\mathrm{div}}q_\ell}
+
\left(
\frac{NM_{\mathrm{low}}^2}{q_{\mathrm{div}}q_\ell}
+
\frac{B_{\mathrm{pair}}}{q_\ell}
+
\frac{h+1}{2}
\right)
<\frac{Q_{\ell-1}}{2}.
\]

After multiplying by `2*q_div*q_l`, this is exactly

\[
2NA^2+
\left(
2NM_{\mathrm{low}}^2+2q_{\mathrm{div}}B_{\mathrm{pair}}
+q_{\mathrm{div}}q_\ell(h+1)
\right)
<q_{\mathrm{div}}Q_\ell.
\]

The candidate checks this exact integer inequality.

## 14. What this proves and does not prove

Subject to the centered-digit contract and successful hosted validation, the candidate can prove:

- a key-conditioned BV error bound uniform over every third component on the specified active basis;
- correct row count and basis projection for the current high/low Relin2 paths;
- no-wrap for the bounded key-switch and final coefficient argument when the explicit inequalities pass;
- a final coefficient inequality using `1/(q_div*q_l)` and no extra Relin2 `+h`.

It does not prove:

- the centered-digit contract from the supplied source set;
- a probability-one or numerical-tail bound over newly generated Gaussian keys;
- a paper-wide universal `E_Relin` theorem;
- high-precision encoding or more than 53 accurate bits;
- secure parameters, repeated multiplication, refresh, canonical-embedding growth, or performance.
