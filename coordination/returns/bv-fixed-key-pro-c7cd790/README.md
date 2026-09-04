# BV fixed-key conservative error bound — independent follow-up

## Disposition

**AMEND — the fixed-key idea is mathematically usable, but the supplied packet does not close the one implementation contract needed to promote it to an unconditional accepted certificate.**

For OpenFHE 1.5.0 BV with `digitSize=0`, a deterministic bound uniform over **all ciphertexts on a declared basis for one already-generated evaluation key** can be built from the evaluation-key row residuals. The exact active-basis form is

\[
B_{Q_\ell}(K)=N\sum_{i=0}^{L-1}
\left\lfloor\frac{q_i}{2}\right\rfloor
\left\|\rho_i^{(Q_\ell)}\right\|_\infty,
\]

where

\[
\rho_i^{(Q_\ell)}=
\operatorname{Center}_{Q_\ell}
\bigl(b_i+a_i s-G_i(s^2)\bigr)
\]

is computed after restricting the fixed evaluation-key row to the active prefix basis. The residual already contains OpenFHE's key-generation noise-scale factor and sign:

\[
 b_i+a_i s-G_i(s^2)\equiv -n_s e_i.
\]

Accordingly, `n_s` must **not** be multiplied a second time when the measured quantity is `rho_i`. For odd RNS primes, `floor(q_i/2)=(q_i-1)/2` is exact; ZCode's `ceil(q_i/2)` is safe but one unit looser per row.

The raised-high call allocates eight BV digits on the full basis, while its appended `q_div` source tower is zero. Under the required `SwitchModulus(0)=0` mapping, the eighth digit contributes zero; the candidate checks that fact on the exercised raised-high component. The low call has seven digits. Subject to that mapping, both paths use the same seven active rows after projection to `Q_l`, and a direct pair bound is

\[
B_{\mathrm{pair}}=B_{\mathrm{high}}+B_{\mathrm{low}}
=2B_{Q_\ell}(K).
\]

No additional `+h` belongs in that direct two-path BV term. The paper's `+h` comes from its separate three-rounding near-additivity argument; the final RS2 term `(h+1)/2` remains.

## Exact unresolved premise

The supplied pinned `dcrtpoly-impl.h` shows that `CRTDecompose(0)` calls `NativePoly::SwitchModulus` to re-emit source-tower coefficients on every target tower, but the packet does not contain the implementation or a normative postcondition of that method. The nontrivial factor `floor(q_i/2)` requires the coefficientwise centered-lift contract

\[
L_i(r)=\begin{cases}
 r,&0\le r\le \lfloor q_i/2\rfloor,\\
 r-q_i,&\lfloor q_i/2\rfloor<r<q_i.
\end{cases}
\]

Without that contract, the only source-closed generic digit bound is the trivial active-modulus bound near `Q_l/2`, which cannot prove no-wrap and is not useful. The included candidate therefore remains a **PROBE**, not an adopted green or theorem gate. It checks the centered-lift boundary behavior at runtime, but a finite probe is not a proof for all residues.

## Candidate

Only one existing test file is changed:

- `candidate/0001-PROBE-fixed-key-bv-bound.patch`
- `candidate/project/tests/mult2_e2e_oracle_test.cpp`

The candidate:

1. computes the fixed-key row residuals before plaintext construction, encryption, or observation of any ciphertext error;
2. independently reconstructs each row in `Q_l` using `cpp_int` CRT and schoolbook negacyclic multiplication;
3. verifies the full raised-high digit count, prefix-low digit count, and zero `q_div` digit;
4. checks the existing independent high, low, pair, and final coefficient errors against the proposed key-conditioned bound;
5. adds explicit pre-RS and final integer-lift no-wrap conditions;
6. preserves the existing vectors, `1e-3` decoded functional tolerance, all 44 registered tests, production files, public seams, historical conditional certificate, and the labels `conservative_E_Relin_available=false` and `universal_theorem_gate=UNPROVED`.

No production patch is justified.

## Package contents

- `REVIEW.md` — source/evidence review and disposition of ZCode Q4 and Codex corrections.
- `BOUND-DERIVATION.md` — quantified fixed-key theorem, assumptions, proof, pair/final propagation, and Gaussian-key distinction.
- `TEST-PLAN.md` — independent probes, public-seam assertions, commands, fail-fast criteria, and limitations.
- `EXECUTION-LEDGER.md` — identities, retained evidence versus work actually executed here, and package-manifest semantics.
- `candidate/` — one test-only PROBE patch and the full changed file.
- `evidence/` — input verification, deterministic arithmetic witnesses, their source, and patch/static checks.
- `PACKAGE-MANIFEST.json` and `SHA256SUMS` — generated at packaging time.

## Execution status

Package/hash verification, source inspection, exact integer calculations, deterministic exhaustive toy witnesses, patch application, byte comparison, and static patch checks were executed in an isolated working directory.

OpenFHE configuration/build, project compilation, cryptographic runtime, CTest, Linux hosted CI, and Windows hosted CI for this candidate were **NOT EXECUTED**. The supplied run records are retained evidence, not executions performed in this review.

## Promotion boundary

Codex should not adopt the candidate as a green certificate unless all of the following hold:

1. the exact pinned implementation or an authoritative pinned contract for `NativePoly::SwitchModulus` proves the centered-lift mapping for every source residue;
2. warning-as-error builds and all 44 tests pass on Linux and Windows;
3. the candidate prints a nontrivial `B_path`, `B_pair < Q_l/2`, and both no-wrap conditions pass for fresh keys without changing vectors or thresholds;
4. the old conditional labels and universal-theorem `UNPROVED` status remain unchanged.

Even after those conditions, the result is a fixed-key, fixed-basis coefficient certificate. It is not a security proof, a Gaussian-key probability statement, a paper-wide theorem proof, or evidence of greater-than-53-bit precision.
