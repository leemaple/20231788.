# BV zero-digit centered lift: additional official source and Codex derivation

Status: SOURCE-BACKED DERIVATION, pending independent cross-check and hosted
OpenFHE witnesses. Observed 2026-09-04 Asia/Shanghai, clean integration source
d73824c2d382013c3aadbd7cb29c57008e839714; documentation head before this note
7f3aa3adc42ac8c2d80dac904641efa6dc36de0b. No production, CTest, threshold,
backend, security or universal-theorem label is changed by this note.

## New source closes a specific missing premise

The active Pro fixed-key task had isolated whether CRTDecompose(0) actually
uses centered coefficient lifts. Its original packet contains keyswitch-bv.cpp
and dcrtpoly-impl.h, but not the lower-level SwitchModulus implementation.
Codex independently obtained FOUR additional pristine files through the
official GitHub tree/blobs at exact OpenFHE commit
df495ba2e91739a6dc8f1de254fc5a41155ce504. Bytes, Git blob SHA1 and SHA256
were recomputed, not inferred from filenames. They are retained unchanged
with licenses under coordination/official-references/bv-centered-lift/;
OFFICIAL-PROVENANCE.json records paths, URLs, lengths and both hashes.
No locally modified OpenFHE or old user implementation was inspected.

Load-bearing source chain:
- Original pinned dcrtpoly-impl.h:230-253: zero-digit CRTDecompose obtains
  coefficient and evaluation copies. Digit i retains original tower i;
  every other tower is made from a coefficient copy of tower i, using
  Poly::SwitchModulus, then changed back to evaluation format.
- New poly-impl.h:400-406 forwards to m_values->SwitchModulus and replaces
  the modulus/root parameters.
- New mubintvecnat.h:325 declares NativeVectorT::SwitchModulus.
- New mubintvecnat.cpp:109-122 uses halfQ = oldQ >>1 and STRICT v>halfQ:
  for newQ>oldQ, add newQ-oldQ only on the negative half;
  otherwise apply modular subtraction of oldQ-newQ on the negative half.
  It is NOT LazySwitchModulus (:125-129), which merely reduces unsigned v.
- New ubintnat.h:891-902 implements ModSubEq, first reducing both operands
  modulo the target. :325-327 gives AddEqFast. In the increasing-modulus
  branch v+(newQ-oldQ) <=newQ-1, so this addition stays representable.

For odd p=oldQ, canonical 0<=v<p, define d=v when v<=floor(p/2),
and d=v-p otherwise. Then |d|<=floor(p/2)=(p-1)/2.
For new r>p, the source returns v or v+r-p, exactly d mod r.
For r<=p, it returns v mod r or (v-p+r) mod r, again d mod r.
Thus the copied coefficient in EVERY tower represents the SAME small signed
integer d. The unchanged tower i is congruent to d modulo p as well.
The inverse/forward NTT operations change representation, not this ring fact.

This supplies a source-backed centered-lift premise for Native DCRT's
zero-digit branch with valid canonical residues/generated odd moduli.
It does not establish runtime execution, malformed-input behavior, the
nonzero-digit branch, HYBRID, or integer no-wrap for complete multiplication.

## Fixed-key modular bound (derived, not yet a shipping/test gate)

Let Q=product_i q_i be the ordered active PREFIX modulus, with odd pairwise
coprime q_i, ring R_Q=Z_Q[X]/(X^N+1), and a fixed same-context secret s.
Let g_i be the CRT idempotent that is one at tower i and zero elsewhere.
For fixed BV degree-two evaluation-key rows (a_i,b_i) restricted to Q, define
the actual key residual r_i=[b_i+a_i*s-g_i*s^2]_Q, coefficientwise centered.

The keygen sign is verified by original keyswitch-bv.cpp:85-96:
b_i=g_i*sOld-a_i*sNew-ns*e_i. Original base-leveledshe.cpp:136-143 constructs
sOld=s^2 and sNew=s for EvalMultKeyGen. Therefore r_i is the centered modular
residue of -ns*e_i for these keys, NOT a ciphertext-measured error.
If r_i is measured directly, ns is already included; do not multiply it again
while claiming it is a newly derived exact expression. Recovering a particular
unwrapped Gaussian e_i from a residue needs additional assumptions.

For any valid THREE-component ciphertext at this domain, CRTDecompose(0)
gives digits d_i with ||d_i||_infinity<=k_i=floor(q_i/2).
The gadget identity sum_i g_i*d_i=c2 holds towerwise, independently of key
errors. Original base-leveledshe.cpp:327-341 and keyswitch-bv.cpp:245-277
therefore give modular decryption error:

  epsilon_Q(c2) = [sum_i d_i*r_i]_Q.

For coefficientwise centered modular infinity norm, integer negacyclic
convolution and subsequent centered reduction imply:

  ||epsilon_Q(c2)||_infinity
       <= B_Q := sum_i k_i * ||r_i||_1.

This bound is computed from fixed KEY, SECRET, BASIS and declared domain
BEFORE choosing the checked ciphertext. It is uniform over ciphertexts for
that fixed key/domain. It is not fitted to the particular observed error.
The coarser N*sum_i k_i*||r_i||_infinity also follows. ZCode's
ns*N*sum_i ceil(q_i/2)*||e_i||_infinity can be a coarser sufficient expression
only with an explicit suitable e_i definition and noise/lift premises; it
must not silently mix raw Gaussian noise, scaled row residuals and empirical
ciphertext errors. floor is exact for these odd centered digits; ceil is
looser, not inherently unsound.

This is a MODULAR statement. It does not identify the unwrapped random
Gaussian draw or give a uniform unwrapped-noise bound over all keys.
The finite ring also admits a trivial universal floor(Q/2) cap; that is not
a useful precision theorem. Key-specific B_Q can still be too loose.

## Full raised-high versus prefix-low

Current Relin2 multiplies every existing high tower by q_div, appends a ZERO
q_div tower, relinearizes at Q*q_div, separately relinearizes low at Q,
then exactly decomposes high and adds the high remainder to low.

The final CRT digit of raised-high is identically zero, including after
coefficient conversion and centered lift; its key row contributes nothing.
For each earlier digit, reducing the full key-switch result to Q commutes
with ring operations and gives the same restricted key rows r_i above.
The digit coefficients still obey k_i; multiplication by q_div does not
remove the centered source-residue bound. Thus B_Q also bounds the
RESTRICTED full-basis high-path error. It need not include the inactive final
row or incur an extra q_div multiplier on an already raised-path error.

By the independently retained Relin2 high+low pair identity, a sufficient
fixed-key bound for the recombined PAIR modular error is 2*B_Q (or B_H+B_L
if separately tighter valid domain bounds are supplied). No extra +h comes
from exact decomposition/recombination itself. This does NOT resurrect BV's
disproved combined-call near-additivity premise or prove the paper's literal
bound using a single ordinary Relinearize execution.

Using any such bound in the final RS2/Mult2 INTEGER coefficient argument
still requires its separate non-wrap/lift assumptions and the RS2 rounding
bound. Conditional on those, the existing normalized expression is:

  N*m_low^2/(q_div*q_l) + B_pair/q_l + (h+1)/2.

No extra +h should be inserted twice. The existing normalization correction
remains an algebraic inference about the paper, not a confirmed author erratum.
This note does not silently replace the current execution-specific gate,
alter frozen tests, or set universal_theorem_gate=PROVED.

## Bounded arithmetic checks actually executed

Command:
 node coordination/evidence/bv-centered-lift/source-branch-witness.mjs

This is Codex-authored exact BigInt arithmetic modeling the cited branches
and a toy degree-two ring, NOT compiled OpenFHE, encryption/key generation,
CKKS, CTest, cryptographic execution or a parameter benchmark.
No dependencies/network/filesystem writes occur in that script.
Observed exit0, ~0.020s wall time:
- 3627 scalar (old prime,new prime,residue) cases over 13 odd primes;
  every modeled SwitchModulus result equals independent centered lift;
- 1596 cases distinguish this result from a lazy unsigned reduction;
- Q15 / fullQ105 / q_div7 / N2 toy gadgets: all225 low polynomials and all225
  raised-high polynomials satisfy gadget/restriction/zero-final-digit facts;
- fixed residuals r0=(1,0), r1=(0,1) give predeclared B_Q=3;
  observed per-path maximum3, and all50625 independent high/low pairs obey
  2*B_Q=6, with observed maximum6;
- the Q101 example 40+40 -> -21 still demonstrates that modular triangle
  acceptance alone does not imply no-wrap.

The toy uses fixed algebraic residuals, NOT a generated evaluation key.
Finite enumeration is supporting discrimination evidence, not a proof of
OpenFHE correctness for every input. The general statement is the explicit
source/algebra derivation above, pending independent review and hosted tests.

## Next owner, missing verification and non-interruption

Codex owns reconciling this new source note with the currently LIVE Pro
fixed-key derivation at:
https://chatgpt.com/c/6a9a5824-3e5c-83ec-83ed-a73acf3dc062.
No mid-response message, upload, reminder, stop or refresh was sent.
After completion, retain its return, compare quantifiers/lifts/domains,
and supply these exact additional official references in a complete
follow-up if its result leaves this premise unresolved or disagrees.

Remaining: independent review of this derivation; actual key-residual and
digit-domain witnesses on pinned hosted OpenFHE; useful tightness at the
intended parameters; final integer non-wrap and high-precision/repeated
lifecycle experiments. Fable5.1's already recorded403-before-inference is
not a review and does not block those safe steps. Full goal remains active.
