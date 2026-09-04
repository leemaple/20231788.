# Independent algebra check for the printed Theorem 4.8 scale

Recorded 2026-09-04. Status: Codex mathematical inference pending Pro reconciliation;
not a corrected-paper claim, not an OpenFHE runtime test and not a security result.

The supplied original PDF page8 was visually inspected earlier; its displayed
comparison has 1/q_l times the input recombined plaintext product. The supplied
text's Definition4.7 and modulus-consumption paragraph instead state a Tensor2
division by q_div followed by RS2 division by q_l. Theorem4.8's proof also bounds
the recombined tensor plaintext with a denominator q_div.

## Small exact witness

Use constant polynomials in Z[X]/(X^2+1), q_div=13, q_l=17,
Q_(l-1)=12289, Q_l=208913; secret s=1 has h=1. Both input pairs have high
ciphertext (17,0) and low ciphertext (0,0). This is an ideal algebraic ciphertext
fixture, not an encryption or production parameter selection. All nonlinear
ciphertext components that need relinearization are zero; relinearization is
exact on this fixture, with a fixture-specific E_Relin=0. This is not an assertion
that the implementation has a zero universal relinearization error bound.

Each input recombines to 13*17=221. Tensor2 gives high (289,0,0), low zero;
the exact Relin2 boundary keeps high289 and low0. Every rescale division below is
integral, so centered rounding is unambiguous:

- RS2 high =289/17=17.
- RS2 low =(13*289)/17 -13*(289/17)=0.
- Output recombined plaintext =13*17=221.
- Input product / (q_div*q_l) =48841/221=221, exact error0.
- Input product / q_l =48841/17=2873, error2652.

The displayed magnitude condition is satisfied in this algebra fixture:
2 * (N*(M_high*q_div+M_low)^2 + E_Relin + h)
=2*(2*221^2+0+1)=195366 <208913=Q_l.
The displayed error expression with these fixture bounds is 1/17+1=18/17,
far below2652. All quantities fit without modulus wrap; Q_(l-1)/2 also exceeds
both compared output values. Integer arithmetic and divisibility were checked
with BigInt only; no Mac cryptographic computation or OpenFHE build was run.

## Interpretation and remaining gate

Under the visible, ordinary RCB and Tensor2 definitions, this witness supports
the missing-1/q_div interpretation. It is not evidence for a hidden normalization.
Pro is independently auditing the same discrepancy in the existing Mult2
conversation; do not interrupt or resend it. The narrowly scoped terminal
Fable5.1 request failed with403 before inference, so no Fable ruling exists.

Any implementation/test contract must explicitly distinguish the printed theorem
from the inferred normalized statement. Actual OpenFHE relinearization bounds,
non-wrap certificates, decoder recorded/logical scale bias and numerical accuracy
remain separate obligations; this ideal arithmetic check does not discharge them.
