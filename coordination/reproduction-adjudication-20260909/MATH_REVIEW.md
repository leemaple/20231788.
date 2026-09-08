# Independent mathematical review of the reproduction-adjudication return

## Disposition

**Accept the new scalar mathematics and the identification of the remaining
Ecd coefficient-correspondence gap. No mathematical defect was found in the
proposed inverse-mapping contract.** The return closes two of 32,768 ideal
coefficient identities, proves that the already adopted canonical-radius cap
does not imply correct encoding, and gives a sound conditional bound for the
pure encoding contribution after eight ideal squarings. It does not close the
other 32,766 rounding cells and therefore does not establish a new E80 PASS or
a production defect.

The proposed `PUBLIC-S100-ECD-CELL-01` is a genuine, bounded, nonduplicative
next requirement for literal paper-Ecd correspondence of this retained public
polynomial. Its candidate is still **unrun**. A future GREEN would prove the
correspondence only for this hash-bound current `p`; it would not identify the
historical `p`, remove public-encryption/evaluator/readout error, reproduce
Table 3, or certify security.

Reviewer context/model identity is requested-unverified. Worktree identities
`31e24bec...` (clean source baseline) and `278bc5d...` (current documentation/
intake state) were supplied by root; I did not invoke Git to re-attest them.

## Independent source derivation before reading the verdict

The frozen input is defined at
`tests/paper_full_eight_square_oracle.h:97--119`. For slot index `s`, put
`t=floor(s/2)`. Apart from the dyadic perturbation `s*2^-75`, the magnitude
patterns repeat through four 256-slot rotations. Write

```text
s = 1024*b + 256*r + k,
b=0,...,15, r=0,...,3, k=0,...,255.
```

The rotation multiplier is `i^r`; the sign of the small imaginary magnitude
is constant through the four quarters of one block. Hence every nonperturbation
term is multiplied by `1+i-1-i=0`. For the perturbation,

```text
sum_{k=0}^{255} (1024*b+256*r+k)
  = 262144*b + 65536*r + 32640,

65536 * sum_{r=0}^{3} r*i^r
  = -131072*(1+i).
```

Sixteen blocks therefore give exactly

```text
sum_s z_s = -2^-54*(1+i).
```

This reasoning does not enumerate or reconstruct the full slot vector. The
dyadic inputs have denominators no larger than `2^75`; under the admitted
ordinary Boost formatting/construction semantics, the source's binary512 to
decimal100 bridge carries them exactly.

The inverse transform's index-zero output is the average: in the inverse
butterflies, the zero branch repeatedly adds both halves, index zero is fixed by
bit reversal, and `Transform` divides by 16,384
(`src/high_precision_client_io.cpp:375--405`). Thus, with `S=2^100` and
`m=N/2=2^14`,

```text
a_0 = (S/m) sum_s Re(z_s) = -2^32.
```

For the middle coefficient, with `xi=exp(pi*i/N)`, every `5^s` is `1 mod 4`,
so

```text
xi^(-5^s*N/2) = -i,
Re(-i*z_s) = Im(z_s),
a_{N/2} = (S/m) sum_s Im(z_s) = -2^32.
```

The sign is positive relative to the imaginary mean, not its negative. Reading
only the two corresponding entries from the retained public JSON gives

```text
p[0] = p[16384] = -4294967296,
```

so those two coefficients are exact ideal-rounding matches.

## Half-down cells and the semantic negative

Production `PaperRound` chooses `floor(x)` when the fractional part is at most
one half (`src/high_precision_client_io.cpp:417--423`). The exact preimage of an
integer `k` is therefore

```text
(k-1/2, k+1/2].
```

Accordingly, a closed outward enclosure `[l,u]` certifies `k` only when
`l>k-1/2` and `u<=k+1/2`; it refutes only when `u<=k-1/2` or `l>k+1/2`.
Overlap is inconclusive. `rounding_contract.py:61--78` implements exactly these
open-left/closed-right directions, including negative ties. Requiring a
strictly positive margin in the future runner is stronger than mathematically
necessary at an exact right tie, but it can only turn such a case into
INCONCLUSIVE, not produce a false GREEN.

Let `p'=p+1`, meaning increment only the constant coefficient. The adopted cap
gives `||sigma(p)||/S < R=495621/500000`; exact arithmetic gives

```text
R + 1/2^100 < 127/128.
```

Thus `p'` still satisfies the nonwrap seed. But its constant coefficient is
`-2^32+1`, whose rounding cell begins at `-2^32+1/2`, so the exact ideal value
`-2^32` is wholly outside it. The negative is genuinely semantic after fixture
binding, not merely a hash rejection. It proves that the cap cannot establish
Ecd correspondence; it is not evidence that production emitted `p'`.

## Conditional error propagation

If all 32,768 scaled ideal inverse coefficients lie in their actual `p_j`
cells, then each real coefficient error has absolute value at most `1/2`.
The canonical triangle inequality yields the conservative complex-modulus bound

```text
delta = ||sigma(p)/S-z||_inf <= N/(2S) = 2^-86.
```

No `sqrt(2)` factor is missing: this sums the `N` real polynomial coefficient
errors directly. Grouping real/imag halves could give a tighter bound, but the
stated `N/(2S)` bound is valid.

With `x=sigma(p)/S`, `|x|<R`, and consequently `|z|<=R+delta`, the exact
factorization

```text
x^256-z^256 = (x-z) sum_{j=0}^{255} x^(255-j) z^j
```

gives

```text
|x^256-z^256| <= 256*(R+delta)^255*delta.
```

Since `delta/2^-80=1/64`, its ratio to the E80 threshold is
`4*(R+delta)^255`. My independent `Fraction` calculation verifies

```text
424504491252/10^12
  <= 4*(R+2^-86)^255
  < 424504491253/10^12
  < 1/2.
```

This is a valid conditional **encoding-only, ideal-squaring** budget. It omits
PKE error, Tensor2/Relin2/RS2 error, LL omission, readout error, and any
historical phase. It cannot change the retained two-platform S100 E80 FAIL.

## Review of the proposed full inverse-mapping obligation

The mathematical map in `candidate/check_rounding.py:33--87` is correct. Put
`y_k=z_s` at `k=(5^s mod 2N-1)/2` and put its conjugate at `N-1-k`. The
powers-of-five orbit covers the roots congruent to 1 modulo 4 and the conjugate
placement covers those congruent to 3 modulo 4, with no missing factor of two.
For real coefficients,

```text
y_k = sum_j (a_j/S) xi^j exp(2*pi*i*j*k/N),
a_j = (S/N) xi^(-j) sum_k y_k exp(-2*pi*i*j*k/N).
```

The candidate implements the negative-exponent DFT, division by `N`, and
`xi^(-j)` untwist in that order (`:54--83`). Its orbit collision/full-coverage
checks and the two analytic mean anchors are appropriate. The exact dyadic
slot transcription in `rounding_contract.py:81--91` agrees with the frozen
input, rather than annulus or S116 input.

The integer-grid interval operations, Machin-pi enclosure, and Taylor remainder
construction are outward by inspection (`candidate/interval_core.py:15--116`).
They are copied from the already reviewed cap checker, so this is not a second
independent validation of those primitives. It is nevertheless independent of
the production Boost special inverse, and its new Hermitian completion,
direction, normalization, twist, cells, and source-to-`p` comparison are the
relevant independent seam. The N4/N8 analytic spectra and X/X^3 controls are
reasonable sign/normalization checks, but they and the full inverse have not
run.

Outcome interpretation is sound:

- all 32,768 enclosures contained in their cells with positive margin can
  certify this current `p` and activate the conditional bound above;
- an enclosure wholly outside a cell can refute correspondence only after the
  source, orbit, tiny controls, imaginary-zero condition, and analytic anchors
  remain valid;
- overlap, zero conservative margin, excess interval width, or a control
  failure is not a production refutation.

The full coefficientwise check is sufficient and directly resolves literal
Ecd equality for this `p`; it is not the only conceivable way to obtain a
canonical encoding-error bound. A direct independent bound on
`sigma(p)/S-z` could suffice for that narrower inequality without proving each
rounding cell. Pro's “unique next action” is therefore accepted as an
operational prioritization, not a theorem of mathematical necessity.

One minor wording boundary should be preserved during integration: the tiny
control code at `check_rounding.py:109--113` enforces the `2^-100` width only
for the real enclosure; for the imaginary enclosure it checks containment of
zero. This does not invalidate the inverse formula or any future full result,
but prose should not claim both component widths were tested unless the
separate harness review adds that assertion.

## Claim disposition

| Pro claim | Disposition |
| --- | --- |
| Closed forms `a_0=a_{N/2}=-2^32` and match to retained `p` | Accepted. |
| Half-down cell endpoints and signed boundary tests | Accepted. |
| `p+1` remains under the cap but violates Ecd | Accepted as a semantic counterexample to implication, not a production counterexample. |
| Conditional `delta<=2^-86` and eighth-square ratio `<0.424504491253` | Accepted only under all-coefficient ideal nearest-rounding correspondence. |
| Current cap instantiates the adopted nonwrap theorem | Accepted under the already adopted A01--A03 and root provenance receipt; still current-`p` only. |
| No evidenced Tensor2/Relin2/RS2 repair | No contrary mathematical/source finding in this review; this is not a universal proof of production correctness. |
| Full inverse candidate proves Ecd | Not yet: mathematically suitable candidate, but tiny/full transforms are unrun and source/runner adoption is pending. |
| Original S100/Table 3/security completion | Not established; historical E80 FAIL is unchanged. |

## Evidence identities and commands

Principal reviewed SHA256 values:

| File | SHA256 |
| --- | --- |
| `pro/SCALAR_PROOF.zh-CN.md` | `efd8e7efaec3bcb7898b782ef7f682091f2fac082ebd6c92b8d8925e5dde1502` |
| `pro/tools/run_scalar_checks.py` | `08db7890fa88fe67dc014d5f759567d0bf7d146c8e5f0552704913f183e9e537` |
| `pro/candidate/rounding_contract.py` | `4d9daa799c808f6879b26555d6de4cac6875dfa713531a284d97b79eafef2b7e` |
| `pro/candidate/interval_core.py` | `0167613bff7d314a12d17b49e6cb594069c6dcb992385289bcf9fc861500c412` |
| `pro/candidate/check_rounding.py` | `e1a243ec8246383cc3776efc86a153d82217a9b60d8cafd36acb98fcb11235e9` |
| `pro/DECISION.zh-CN.md` | `d23f939b6cc9a333319d6dfd393f3546205d4be7fedd8ae368ea90d04636076a` |
| `pro/FINDINGS.md` | `a5d5d9bce5c41f067c64892f2fded4de18a17f5fef3fa2cb6ac2ca607f65c4ab` |
| `pro/NEXT_ACTION.md` | `58888344b11ab9e2b097d58c1a5680d1d73e49115af73dc49b10e3df8533066a` |
| `pro/evidence/SCALAR_CHECKS_COMPLETE.json` | `1ed7ac9a454e03c585e620c349e0080cf582b71495900b4f2e577bd4b9310865` |
| retained `pro/fixtures/public_s100_encoding.json` | `7cac9765f0c5e567840ebc347a59e50d039107c7b6a92c037152b69bd8edd94f` |
| retained `pro/source/paper_full_eight_square_oracle.h` | `08d6c4dfe6761b84d893d90c120418a29d8a2b96aa05304e95728d72e4c3e203` |
| retained `pro/source/high_precision_client_io.cpp` | `85b2b03954262344544dca96f84f8aa06044fb0fc74ff13f1a2317b67d666ac6` |

I read source/specification before Pro's `DECISION` and `FINDINGS`. Read-only
commands were `sed`, `nl`, `rg`, `find`, `wc`, and `shasum`. My independent
bounded Python command used only `fractions.Fraction` and constant-size
four-quarter arithmetic to verify the two closed forms, cell separation,
`p+1` cap slack, `delta=2^-86`, and the exact 12-decimal propagation bracket.
It selected only `p[0]` and `p[N/2]` from the retained JSON and did not
reconstruct all input slots.

I did not execute Pro's scalar checker, candidate code, tiny/full transforms,
FFT/NTT, an encoder, C++, build, FHE, sampler, CI, browser, or Git. Root reports
that transport and its independent 53-check scalar replay passed; this review
does not duplicate or re-attest that intake. No production, Pro artifact,
threshold, input, or evidence file was modified; this report is the only edit.
