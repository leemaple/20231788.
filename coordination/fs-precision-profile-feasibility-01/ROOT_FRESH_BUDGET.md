# Conditional fresh-error feasibility calculation, not a passed profile

Root computed this bounded scalar check on 2026-09-07 Asia/Shanghai at `fb68da960d2bf741fa5ad8643f4a8767268af356`. Bundled Python3.12.14 `-B -I`, standard-library exact `Fraction` arithmetic; 0.00063s reported by the script. No encryption, FFT, full-slot numerical replay, source change, or candidate-profile test. The mathematical owner was sent the result for challenge; independent design sign-off remains pending.

## Explicit conditional model

Let N=32768, h=128, T=2^-80, and rho=127/128. The frozen formula has `|a| <= 1015/1024 + 16383/2^75` and `|b| <= 8/1024`; exact rational comparison confirms `a_max^2+b_max^2 < rho^2`. Four-phase rotations preserve this magnitude.

Assume each of the three public-encryption error polynomials `e_pk,e0,e1` has canonical **complex-modulus** norm at most B=8192. This is a conditional event, **not** a measured event, an established sampler tail probability, or a universal bound on Gaussian samples. Dense ternary ephemeral v has `|can(v)| <= N`; the h128 secret has `|can(s)| <= h`. Thus the canonical public error of `e_pk*v+e0+s*e1` is bounded by `B*(N+h+1)`. For mathematically exact encoding followed by nearest rounding of N real coefficients, the added canonical error is at most N/2. Finite-precision transform error, before coefficient rounding, needs its own allowance for an implemented profile; it is not silently proved by this calculation.

Under that ideal-encoding/noise event, `u0 = (N/2 + B*(N+h+1))/2^116 = 269508608/2^116`. For each r=0..8, n=2^r, the power difference obeys the complex-norm bound

`|(z+e)^n-z^n| <= n*(rho+u0)^(n-1)*u0 = U_I,r`.

Complex modulus bounds each real/imaginary component, so no extra factor of two is needed here. Exact comparisons put every `U_I,r <= T/4`; the greatest bound is about 0.18540T at r7, and the r8 bound is about 0.13587T. These numbers are conditional **upper envelopes**, not measured new-profile errors. Applying the same loose envelope at S100 fails this sufficient-budget check; that alone proves neither actual error nor impossibility. The old actual CTest failure is separate retained evidence.

## Reproducible exact scalar expressions

```python
from fractions import Fraction as F
N, h, B = 32768, 128, 8192
rho, T = F(127,128), F(1,2**80)
amax, bmax = F(1015,1024)+F(16383,2**75), F(8,1024)
assert amax*amax+bmax*bmax < rho*rho
u = F(N//2+B*(N+h+1),2**116)
for r in range(9):
    n = 2**r
    bound = n*(rho+u)**(n-1)*u
    assert bound <= T/4
```

## Unclosed dependencies

This addresses only ideally propagated fresh error, not A_r. Raising S0 and d together keeps the high-part scale approximately fixed; omitted low-times-low error need not shrink. No claim that observed A from the old profile scales down is allowed. Candidate exact moduli/roots, coherent actual scale drift, intermediate/final lift and capacity, auxiliary-P compatibility, finite-precision encoding, added-error budget and the noise-event justification or observation remain to be resolved. A changed profile also changes the paper-table parameter experiment and must not relabel the original profile as passed.

The useful conclusion is a finite conditional budget worth challenging, not authorization to skip TDD or run a new full chain yet.
