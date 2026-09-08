# Initial-source map review: conditional initialization certificate accepted

2026-09-09 Asia/Shanghai. This review closes the independent initial-source-map slice, not the pending Pro eight-step derivation or the complete reproduction goal. Final map SHA256: `8136366b7b75607234762f2c3290ab705f234dcd0edd2121afebf311db439591`.

## Read and reconciled

Root read all432 lines of the final `coordination/initial-lift-nonwrap-20260909/INDEPENDENT_INITIAL_MAP.md`, after the independent author confirmed it final. Root independently checked the original input formula, internal scalar types, range/rounding checks, shared encoding computation, initial DCP and official private/public `EncryptZeroCore` source consumers. The original372-line draft omitted a useful consequence of `StableRound`; root requested its investigation, and the final map explicitly adds the conditional scalar-library contract instead of claiming the encoder's full-modulus guard is its only available range constraint. Two editorial errors were corrected by the file's owner.

The adopted initial interface is

```text
epsilon = e_pk*v + e_0 + s*e_1
||epsilon||c <= 39*(N+h+1) = 1282983
H0 <= (M_enc + 1282983 + Rd)/d
L0 <= Rd,   Rd=(1+h)(d−1)/2.
```

It retains the full `F=dQ` lift, both ciphertext-coordinate d remainders, and explicit Q multiples. The initial DCP algebra does not assume cancellation of the uniform public-key term numerically; it cancels through the honest key equation. It does not confuse dense ephemeral `v` with the h128 secret.

## Independent finite arithmetic actually executed

Root authored/read `coordination/initial-lift-nonwrap-20260909/check_initial_margins.py` and ran it once with bundled Python `-B -I`; exit0. `ROOT_INITIAL_MARGINS.json` records the result and script SHA256 `e31c6c33ba8ee47d2822201500e4a673aa525f99554d737d32d4f875ee8e8277`.

Under the stated assumption `epsilon160=10^-159` and normal scalar relative-error bounds, the accumulated rounding multiplier is bounded without executing Boost:

```text
(1+epsilon160)^410 / (1-epsilon160)
  <= 1/[(1-410*epsilon160)(1-epsilon160)] < 4.
```

The geometric-series inequality follows because each binomial coefficient is at most the corresponding power of410. Therefore the guard plus nearest rounding yields

```text
M_enc < 4*2^-410/epsilon160 + 1/2 < 2^123.
```

Exact integer/Fraction checks confirm positive margins for full fresh phase, high DCP member, low DCP member and initial recombined phase. Their retained integer-margin bit lengths are620,620,580,580 respectively. The checker also rejects the implication from a full-F centered guard alone: `m=(Q+1)/2` fits F but not centered Q, and the additional range guard rejects this scalar fixture. It separately distinguishes the internal160-digit Primary from public100-digit ClientReal.

These are finite mathematical checks, not a production regression RED/GREEN. No Boost, inverse transform, FHE, random sampler or historical ciphertext was executed/read. No full source-level proof of the actual Boost version's arithmetic is claimed; its stated scalar behavior is an explicit dependency, just as the ring primitive semantics remain a dependency. If that contract is not admitted, an exact public `M_enc` aggregate is still an alternative initial interface, not a reason to expose secret/noise values.

## Disposition and remaining work

Accept the map as a **conditional source-specific initialization contract**. This removes the need to dispatch a new encoding run merely to establish these initial magnitude margins. It does not prove the intended-slot encoding error `E_enc`, or discharge the eight-step lift/precision conditions that Pro is currently deriving.

In particular, the earlier `ROOT_COARSE_RECURRENCE.md` used the hypothetical **fresh phase** bound `M0<=2^100`. The source-derived **encoding** cap `M_enc<2^123` does not establish that stronger hypothesis. Do not relabel its first four conditional rows as a historical or general accepted-path certificate. Nor does its fifth inconclusive comparison imply an actual wrap.

The main Pro conversation remains live and is not interrupted to inject these intermediate findings. Preserve this independent source map and root check for comparison with the terminal return. Original S100 E80 FAIL remains; no production/test/CMake/workflow change, new CI, encryption experiment, parameter change or author contact occurred.
