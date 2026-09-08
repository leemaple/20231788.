# Independent root probe: where the coarse C25 sufficient bound loses usefulness

2026-09-09 Asia/Shanghai. This was written and run after the initial/nonwrap Pro task was submitted, before any Pro return, and was not sent into its live context. It probes a bound's strength, not an actual ciphertext or a newly detected production defect.

## Deliberately conditional starting point

Assume the fresh full-basis centered phase has coefficient norm `M0<=2^100` and the adopted initial DCP compatible-lift interface applies. This is a hypothetical, optimistic interface value, **not a numerical fact about the frozen original message's encoding or an actual historical sample**. With `Rd=(1+h)(d−1)/2`, start with upward-rounded `H0=(M0+Rd)/d`, `L0=Rd`. Source-specific primitive semantics and honest-key finite-support conditions of the adopted Relin2 contract remain assumptions.

Use the adopted C25 coefficient recurrences without a new initial-encryption draw, and use the basic negacyclic convolution bound

```text
||T*||c <= N*(d*H²+2*H*L).
```

For the sufficient NW test compare this bound plus `B2+m*Rm` against `Q/2`. All original S100 primes are read from the hash-pinned clean-room source. Each next H/L bound is rounded upward to an integer, which preserves a bound for integer coefficient norms and keeps the calculation small. We intentionally do not reinterpret a large upper bound as a lower bound or switch norms to make it smaller.

## Observed finite calculation

`check_coarse_recurrence.py` ran once using bundled Python `-B -I`, exit0. Its SHA256 is `16dea2b9dbdee0e3af62a2a703ef8e889aa171165455c53035c0d3ba5d8ca213`; exact output is ROOT_COARSE_RECURRENCE.json.

| Round | Q bit length | High bound bit length | Low bound bit length | Tensor coefficient bound bit length | This sufficient NW comparison |
| --- | --- | --- | --- | --- | --- |
| 1 | 580 | 61 | 47 | 176 | satisfied |
| 2 | 520 | 76 | 63 | 206 | satisfied |
| 3 | 460 | 106 | 94 | 266 | satisfied |
| 4 | 400 | 166 | 155 | 386 | satisfied |
| 5 | 340 | 286 | 276 | 626 | inconclusive |
| 6 | 280 | 526 | 517 | 1106 | inconclusive |
| 7 | 220 | 1006 | 998 | 2066 | inconclusive |
| 8 | 160 | 1966 | 1959 | 3986 | inconclusive |

An additional read-only exact-integer check of the retained rows tested the necessary input-lift bridge for this sufficient-certificate route: `2(dH+L)<Q` holds at inputs to rounds1–5, and `2(dH_next+L_next)<Q/m` holds at outputs of rounds1–4. Both fail as sufficient tests thereafter. Thus the first four rows can be chained **under the stated initialization and source-contract assumptions**; no hidden canonical-norm contraction is used. This second check ran exit0 and only read the script output and pinned public prime list, not an encrypted record.

The first failure is specifically round5. It says the recursively accumulated `N*H²` bound is too weak for the decreasing Q; it does not show that the true phase has that size. Even supplying the optimistic initial coefficient bound would not close all eight rounds through this coarse route alone. Do not cap the bound at `Q/2` and then claim this proves absence of wrap: a universal centered bound contains no such historical information.

## Consequence

This gives a precise independent challenge for the pending Pro derivation: preserve integer representatives and either use a sharper, justified norm/structure bound or name a minimal missing observable. It does not justify a production patch, changed parameter/input, new random sample, or an original E80 PASS claim. Original S100 FAIL remains unchanged; this is a mathematical bound check, not FHE RED/GREEN.
