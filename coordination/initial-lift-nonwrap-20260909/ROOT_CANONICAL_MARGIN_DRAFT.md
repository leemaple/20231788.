# Independent draft: margin-gated canonical induction for original S100

This draft was derived while the original Pro task remains live. It was not in its input and will not be injected into its ongoing thought. Independent source-first review conditionally accepts the bridge and induction; see CANONICAL_DRAFT_REVIEW.md. The normalization and notation findings are resolved below. This is an independently reviewed **conditional cross-check, not an unconditional eight-step proof or a historical numerical observation**. The final Pro return remains pending; the initial encoding-accuracy premise remains unproved.

## Why the coarse coefficient failure is not decisive

The original fixed input is near, but not on, the unit circle: its exact dyadic formula gives every complex slot modulus less than `127/128`. It is not necessary to substitute annulus125. The repeated factor N in the existing coefficient-convolution estimate can be avoided for multiplication in canonical norm, but **only if the integer representatives being bounded really are the centered phases used in the next step**. One may not silently assume canonical norm contracts under coefficient centering.

The source-specific Relin2 contract supplies compatible integer phase representatives, before taking the next centered representatives:

```text
X = h_a*h_b + (nu_H-rho_d)/d,
Y = h_a*l_b + l_a*h_b + nu_L + rho_d,
h_candidate = X/m + eps_H,
l_candidate = Y/m + eps_C - d*eps_H.
```

The full raised-high key equation ensures the divisibility in X. The rescale residues ensure the two final candidates are integer polynomials. Changes of full-Q lifts become Q/m multiples, as in the accepted coefficient C25 bridge. These are representatives modulo Q/m, not initially claimed to be its centered representatives.

Let H and L bound canonical norms of the current centered high/low integer phases; use `||can(nu_H)||,||can(nu_L)||<=N*BK`, `||can(rho_d)||<=N*Rd`, and `||can(eps_H)||,||can(eps_C)||<=N*Rm`. Submultiplicativity then bounds the candidates by

```text
H_candidate <= H_a*H_b/m + (N*BK+N*Rd)/(d*m) + N*Rm,
L_candidate <= (H_a*L_b+L_a*H_b+N*BK+N*Rd)/m + (1+d)*N*Rm.
```

**Normalization lemma:** for a real-coefficient polynomial `f` of degree less than N in `R[X]/(X^N+1)`, take all N roots `zeta_k=exp((2k+1)pi*i/N)`. Define `||f||can=max_k |f(zeta_k)|`. Orthogonality gives `f_j=(1/N) sum_k f(zeta_k)*zeta_k^(-j)`, so `|f_j|<=||f||can`. Conversely `||f||can<=sum_j |f_j|<=N*||f||coeff`. The stored CKKS half must contain one root from each conjugate pair; completing it by conjugacy leaves the same maximum magnitude. The lemma applies to the real/rational/integer candidates here, not arbitrary incomplete projections. Together with evaluation of products modulo `X^N+1`, it justifies both submultiplicativity and the retained N factors on coefficient-bounded error terms.

**Gate before induction:** require each candidate bound <(Q/m)/2. By the lemma, all candidate coefficients are centered. Only now identify them with actual next centered phases and propagate the canonical bounds. Also test `d*H_candidate+L_candidate<(Q/m)/2` to ensure the recombined representative is centered; individual members being centered alone is insufficient. At each input check the corresponding member and recombined margins. For the recombined C24-style sufficient NW margin use

```text
d*H_a*H_b + H_a*L_b + L_a*H_b + 2*N*BK + m*N*Rm < Q/2.
```

No direct canonical-norm bound is inferred by simply centering an overlarge candidate. If any gate fails, this route is inconclusive from that step.

## Explicit upstream condition, not an assumed E80 result

Let P0=M_enc+epsilon be the actual coherent integer fresh phase, with M_enc the encoded message polynomial (not the per-round Mult prime m). A sufficient input interface is

```text
||can(P0)|| <= S0*(255/256), S0=2^100.
```

This condition implies fresh and initial DCP representative margins via
`H0 <= (S0*(255/256)+N*Rd)/d`, `L0<=N*Rd`. It also follows from the actual original input formula, the honest PKE finite-support bound, and the separate hypothesis

```text
E_enc = ||can(M_enc)/S0-z|| <= 1/512,
```

because the fixed input radius is <127/128 and `N*39*(N+h+1)/S0<1/512`, leaving a total strictly below255/256. This demands only a coarse encoding-accuracy bound for a nonwrap claim; it is **not** an 80-bit precision assumption. The existing source-magnitude cap M_enc<2^123 does not itself prove it. Neither dual-precision agreement nor the historical observed E0 is relabeled a rigorous E_enc bound here.

## Check and counterexample

`check_canonical_margin_draft.py` uses original hash-pinned S100 primes and the frozen original-input source. It performs eight exact integer/Fraction iterations, rounding H/L bounds upward to integers for bounded arithmetic. It tests input/output member and recombined margins plus the recombined NW condition at every round. Any failure raises; a successful model result remains conditional on this draft's algebra and the upstream interface.

Its negative fixture has N=4,Q=5, `f=3+2X-X²+X³`; coefficient centering gives `f'=-2+2X-X²+X³`. At primitive eighth roots, `||can(f)||²=15`, while `||can(f')||²=10+5sqrt(2)>15`. This rejects the invalid general canonical-contraction shortcut without a numerical FFT or a square-root approximation.

This draft cannot repair initial encryption error, certify the original E80 threshold, or identify a production bug. A conditional nonwrap induction is a different assertion from reproducing the paper's reported precision. No parameter/input change, new ciphertext, FHE/FFT/NTT, CI or secret observation is involved.

Execution receipt: root ran the authored script once with bundled Python `-B -I`, exit0; SHA256 `5b4ccc33e215323c159e95460c79a25ab6d0466766d5988ee80b964c0a62a68a`. All five sufficient comparisons were positive at all8 rounds. At the final output the candidate recombined bound divided by Q_final/2 is at most734588/1000000 (an upward display bound), hence <3/4. Earlier seven displayed ratios round upward to1/1000000 because their moduli are much larger. Exact model output is ROOT_CANONICAL_MARGIN_DRAFT.json; it marks both E_enc and historical fresh-phase verification false and adoption pending. This numerical receipt verifies arithmetic, not the semantic bridge or an actual encrypted trajectory.

The original script and JSON are frozen, including their historical pending-review label. For repeatable verification without deleting or overwriting that evidence, run `python3 -B -I coordination/initial-lift-nonwrap-20260909/replay_canonical_margin_draft.py` from the repository root. It runs the exact original script and its two pinned public source inputs in a disposable directory, then compares the complete output bytes with the frozen JSON. See CANONICAL_REVIEW_DISPOSITION.md for the later review state and replay receipt.
