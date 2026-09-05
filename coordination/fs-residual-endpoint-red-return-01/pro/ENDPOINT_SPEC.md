# FS-RESIDUAL-ENDPOINT-01 — corrected executable specification v1-r1

Status: **SPEC_READY for the bounded API/link RED only; candidate for Codex adoption.** Numerical assurance is explicitly **CONDITIONAL**, not an interval or numerical-library certificate. No implementation, compilation, numerical observer execution, new encrypted observation, or project acceptance is asserted by this specification. The two supplied proposals are superseded by the rules below where they differ. Source aliases and physical line references are in SOURCE_MAP.md.

## 1. Authority, frozen computation, and claims

The patch base is the supplied `project/` engineering bytes attributed to documentation commit `f1c33b7fdcc12741f40b96164c87a35f90090345`, tested source `9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e`, and unchanged production source `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89`. These attributions come from the authenticated packet; no private Git state was consulted. OpenFHE remains `df495ba2e91739a6dc8f1de254fc5a41155ce504`. Repository name is `leemaple/20231788.` including its final dot; branch is `codex/paper-scale-implementation-20260905`.

Retain N=32768, M=65536, 16384 slots, gap=1; native64/backend4; HYBRID/alpha1; STANDARD, PRE NOT_SET, COMPLEX, compositeDegree1, noiseScale1, SPARSE_TERNARY, HEStd_NotSet, FIXEDMANUAL nominal50/recorded2^100. Retain ordinary public-key encryption and all existing noise/features. A signed h128 root is sampled once for the paper setup and projected by complete modulus/root/phi identities into eight immutable families. The existing separate small foreign-identity fixture is not a second paper chain and remains unchanged. No secret enters the evaluator. There is one DCP and eight consecutive public Mult2 squares, no refresh/bootstrap or decrypt/re-encrypt. The public terminal RCB/receipt/binder/Poly* Decrypt route remains. [PC:11–15,53–88; H128:193–217; RP:412–432; T:176–190,242–341]

The ordered Q moduli are:

```
1125899904679937, 1125899903827969,
1152921504598720513, 1152921504597016577,
1152921504595968001, 1152921504595640321,
1152921504593412097, 1152921504592822273,
1152921504592429057, 1152921504589938689,
1099510054913
```

Their respective roots are:

```
26113207984, 150640639383, 100545759574150, 31693996050849,
88651361085495, 9679305630873, 24428769072221, 18776242964106,
5821397352863, 33888991361320, 121567553
```

P=1152921504606584833, P_root=4443670208963. Set d=1099510054913, and for i=1,...,8 consume m_i=Q[10-i] with zero-based Q indexing. Independently compute positive reduced rational scales

```
S0 = 2^100
Si = S(i-1)^2 / (d*m_i)
Si = 2^(100*2^i) / product[j=1..i]((d*m_j)^(2^(i-j)))
```

Both integer constructions must agree. Do not infer scales from metadata or replace S8 by S0. The paper's constructive Lemma 4.2 and scaling paragraph include d*m; the isolated Theorem 4.8 display does not justify deleting d. No sufficient nonwrap antecedent is newly established here. [O:32–45,76–90; SC:9–24; PAPER:608–660,903–960; PDF pp.7–8]

For slot s, t=floor(s/2), define exact dyadics

```
a = 1015/1024 - (t mod 16)/65536 + s/2^75
b = (1 + (floor(t/16) mod 8))/1024
b = -b when floor(t/512) mod 2 = 1
z_s = i^(floor(t/128) mod 4) * (a + i*b)
```

Use integer numerator/denominator construction, swaps and signs, not binary64 trigonometry. Keep the original z truth reference, 2^-75 witness, all original 2^-80 E gates and existing codec 2^-120 gate, wrong-S0 falsifier, nonfinite/fail-fast, ownership/foreign/cleanup controls. This task adds characterization, not a replacement green predicate. `A_disposition` is always `NOT_ADOPTED`. Small A cannot discharge original E80. [ID:7–20,38–69; O:97–108,134–195,315–334; T:324–385]

## 2. Observation seam and test-local API

Future GREEN must reuse the independent test-owned `freshPolynomial` and `finalPolynomial` at T:272 and T:319, not reconstruct them from production decoded slots. They arise from the existing sparse signed-secret integer negacyclic product and CRT. Official inverse NTT remains a disclosed shared dependency; this is not an independent NTT proof. The endpoint helper receives only `IntegerPolynomial` and positive exact `Scale`, never a context, key, ciphertext, evaluator callback, or producer transform/table. Do not change the existing Horner routine or its binary512 precision. [O:198–263,283–314; IO:664–716]

In future GREEN, collect the fresh/final observations inside the existing client-only RunPaper scope, then hand off only pure value data (signed residuals, exact norms/scales, comparisons and identity strings) to Run for post-cleanup publication. The handoff must retain no context, plan, key, ciphertext, result/binder ownership or evaluation-key row; otherwise it would invalidate the existing cleanup check. This handoff is not implemented in RED.

The new test-contract header declares seven undefined functions. Their future behavior is normative:

| Function | Required behavior |
|---|---|
| `Observe(polynomial,scale)` | Validate size N, positive odd modulus, centered coefficients and positive reduced scale; preserve input bytes; return all slots at own binary512 and binary768, exact C=sum(abs(c_j)), and K exponent. No global production objects. |
| `ScaledOneNormExponent(C,scale)` | C must be nonnegative. Return nullopt iff C=0. Otherwise the least integer e with C*scale.denominator <= 2^e*scale.numerator. Negative e uses exact cross-multiplication; no floating logarithm. |
| `DirectSparseReference768(terms,scale)` | Validate sorted unique degrees in [0,N), signed integer coefficients and positive reduced scale. Independently evaluate the fixed sparse controls at all powers-of-five roots, in binary768. No observer-table reuse. |
| `ExactAbsoluteDifference(left,right)` | Extract exact represented finite dyadics and return their absolute difference as reduced nonnegative Int/positive Int. Do NOT perform rounded binary768 subtraction first. |
| `AssessDifference(d,b,kind,modelSupported)` | Exact rational classifier in section 5; reject malformed/negative rationals. `modelSupported` describes compatibility with the declared conditional model, not a library accuracy proof. |
| `CanonicalDecimal(value)` | Exact nearest/ties-to-even 110-significant-digit spelling of a represented finite binary768 value, section 6. Reject exponent overflow and nonfinite values. Never zero a nonzero value. |
| `IsCanonicalDecimal(text)` | Independent C++ byte/semantic validator for section 6, not formatting-and-string-comparing. No locale, whitespace tolerance, Unicode digits or partial parse. |

`Binary<512>` and `Binary<768>` use `digit_base_2`, allocator-backed storage, explicit `et_off`. The new types' bit counts/radix and the old Real's 512-bit count are statically asserted. All array-sized scratch storage is heap-backed. Definitions of these functions, observer tables, live capture, formatter, status writer, packer, wrapper, and upload remain **absent from RED**. Invalid input must not be turned into expected unsupported-success.

The only RED executable change is a `--endpoint-observer-self-test` dispatch before `Run()`, hence before any context, key or encryption creation. Its fixtures are in `paper_endpoint_contract::synthetic`, in memory, and never use live evidence names. No-argument execution preserves the old body and completion logic exactly. Unknown arguments fail. The self-test is not a new CTest entry. The new assertions require real future behavior, not a stub or expected missing-feature exception.

## 3. Transform, direct roots, and computational envelope

For each p in {512,768}, independently let xi=exp(+2*pi*i/M), and form

```
u_j = (c_j/S) * xi^j
F_k = sum[j=0..N-1](u_j * exp(+2*pi*i*j*k/N))
e_s = powmod(5,s,M)                 # exact integer arithmetic
x_s = F_((e_s-1)/2)
```

This is an ordinary **positive, unnormalized** twisted radix-2 DFT. There is no factor 1/N. Bit-reverse the input and use 15 iterative radix-2 stages. The 16384 selected exponents are distinct, 1 mod 4, disjoint from their negatives; selected bins permute all even bins in [0,N), not the contiguous first half. The exact modular mapping was checked here without evaluating a transform. [PAPER:213–247; O:283–314; ODFT:96–161,209–268]

Generate every twist entry j=0,...,N-1 directly from angle pi*j/N at its own p, and every ordinary twiddle entry k=0,...,N/2-1 directly from 2*pi*k/N at its own p. Generate pi at p, multiply by the exact integer, and use exact power-of-two scaling. Never generate a root by recurrence, repeated complex multiplication, binary64, widening another precision, or the producer's special transform. Own same-precision immutable tables are reused across controls and fresh/final endpoints. Widening *final binary512 result values* exactly for comparison is allowed, not widening roots. Allocator-backed root generation follows the existing AnchorRoots storage precaution; it does not certify a larger precision table. [O:283–300; IO:329–400; BFWD:149–151]

A transform has 245760 butterflies, each with four real multiplications and six real additions/subtractions: 983040 real multiplications and 1474560 real additions/subtractions. Input scaling is exact integer t_j=c_j*scale.denominator, followed by rounding t_j and scale.numerator to p and one division (at most 98304 conversions/divisions total); multiplying a real scaled coefficient by a complex twist adds 65536 real multiplications. Indexing, permutations and copies are exact. Roots and inputs may be cached only as specified, not by changing this graph.

The four fixed full-N controls are 1, X, X^(N-1), and 3-2X+X^17-X^(N-1), at S=1 and synthetic modulus17. Their C are 1,1,1,7 and K are 1,1,1,8. A separate binary768 direct sparse reference reduces integer exponent e_s*j mod M first and evaluates its own direct sin/cos values. Cache up to M/2 odd-root pairs in a **separate** immutable reference table. For these at-most-four-term controls its error is conservatively J_768(K)=2^10*2^-768*K. Compare every slot/component with D_p+J_768, not only ten anchors. The constant catches normalization/scaling; X and X^(N-1) distinguish sign/map/orientation; the signed four-term polynomial adds cancellation and phase coverage. None proves the common NTT.

Source-only resource accounting: two DFT tables require 98304 complex root pairs total (196608 sin/cos calls). The separate reference cache adds at most 32768 pairs (65536 calls). Mantissa-only storage is at least 15 MiB for DFT tables and 6 MiB for the reference cache; allocator/object overhead and buffers are additional. A live process uses eight control DFTs plus four endpoint DFTs, twelve in total, reusing those tables. A separate explicit self-test process uses eight control DFTs and no crypto. This is O(N log N) plus bounded direct roots, not a timing guarantee. Keep the existing CTest 1200-second timeout, RUN_SERIAL, OMP_NUM_THREADS=2 and job limits. No benchmark campaign, root recurrence optimization, timeout increase or extra chain is authorized. A timeout is failure/unresolved evidence, not PASS. [CM:263–266; WF:32,162]

## 4. Conditional allowance model and derivation

### 4.1 Explicit premises, not source-proved library guarantees

Set u_p=2^-p. Assume finite, nonunderflowing/nonoverflowing nearest-rounded real addition/subtraction/multiplication/division and integer conversion with relative error <=u_p (exact zero stays zero). Use explicit p-bit temporaries; no reassociation, lower precision or fused graph substitutions. Assume relative pi error <=u_p and **absolute sin/cos error at the supplied angle <=8u_p**. That last premise, and complete decimal parsing/formatting behavior, are NOT established by the supplied Boost excerpts. Cross precision and controls can falsify a model but cannot certify it. All passing new numerical labels therefore retain `assurance=CONDITIONAL` and the named model below. [AP:7–19; BOOST:618–716,1110–1279,1972–1987,2029–2030; TRIG:25–76,159–350]

The four supplied Boost reference files are version1.83.0; the actual Windows historical package log records **1.92.0-3**. Do not describe that job as tested on pinned1.83. The future process records actual `BOOST_VERSION`; inspect its effective type/rounding configuration. The packet lacks complete `cpp_bin_float/io.hpp`, `cpp_bin_float/transcendental.hpp`, Boost.Math constants closure, and `cpp_dec_float.hpp`/number support needed for a complete parser/producer-library proof. This is an explicit assurance gap, not an invented implementation or permission to fetch substituted sources. Unsupported operation graph, precision, conversions or exponent range produces UNRESOLVED. Accepting the declared conditional premise is distinct from claiming it was tested or proved. [SW:359–375,514–515; WF:204–215]

For unchanged Horner, `R(Int)` uses a decimal string constructor, not a proven exact cast. Future GREEN must check each actual coefficient and scale integer's **represented result** against the exact integer (relative error <=u_512), plus exponent range, before applying H. This check wraps observations; it must not rewrite Horner. A checked direct binary conversion cannot silently be substituted for that source path. Likewise, the producer-to-observer comparison has a declared absolute transport allowance C_prod=2^-300 per complex value for ClientReal100-to-binary transport while |components|<=2; actual conversion range/finite status must be checked. This transport bound is conditional, NOT a zero-error bound on the producer's algorithm. Lack of applicable support is UNRESOLVED. [O:52,301–313; IOH:23–24; IO:701–716]

### 4.2 Exact conditioning and endpoint envelopes

For each independent integer polynomial, C=sum_j |c_j| and K=0 when C=0, else the least power of two >= C/S. Compute C and K by Int/rational comparisons; max_j |c_j| is not C. Negative K exponents are valid. Runtime C is mandatory, never substituted by a synthetic example or historical printed coefficient maximum. The formal terminal bound N*(Q_base-1)/(2*S8)<2^14 was checked here; it is only an upper cap, not an observed C. Fresh conditioning must be measured on the same next chain. [O:264–281; AP:21–44]

An angle has error <=2*pi*gamma_2, gamma_r=r*u/(1-r*u). Thus complex root error is at most 2*(2*pi*gamma_2+8u)<64u. For explicit complex multiply use <=16u|a||b| roundoff; addition <=4u(|a|+|b|). These dominate a per-dependency-layer factor (1+128u). Scaled-coefficient conversion contributes gamma_3. Set the following exact dyadic envelopes (valid also for zero C):

```
D_p(K) = 2^12*u_p*K
H_p(K) = 2^24*u_p*K
```

The ordinary DFT row-sum bound is K*((1+gamma_3)*(1+128u)^16-1)<D_p. The one-norm already counts summation weights; do not multiply by N again. Unchanged unscaled Horner, its coefficient conversions, N multiply/add steps, roots, inverse scale conversion and final multiply are dominated by K*((1+128u)^(N+4)-1)<H_p. H_512 is the bound actually used for the existing path. The rational inequalities were independently checked for both precisions, using geometric upper bounds rather than subtracting floating near-ones. They are proofs **under the premises**, not executions or interval certificates.

At both precisions require the exact sum of the *represented* endpoint component magnitudes <=5/4, and D_p<=2^-128. Use exact dyadic comparison, not a rounded sum at the threshold. Require the same guard for exact z. These conditions imply both exact and approximate endpoint moduli <2. A finite violated conditioning guard means UNRESOLVED for the observer; it changes neither the input nor original gates. The guard does not apply to the large sparse control's magnitude.

### 4.3 Powers and signed residuals

At each p independently compute z^256 and x0^256 by eight explicit complex squarings, each power requiring 32 real multiplies and 16 adds/subtracts. Never use production expected values or a generic pow implementation with a different operation graph. On the radius-2 disk,

```
2^256*((1+16u_p)^255-1) < P_p = 2^270*u_p
L = 256*2^255 = 2^263
Q_p = P_p + L*D_p(K_fresh)
R_p = 2^264*u_p
```

The exponent255 counts propagation weights, not255 executed multiplications. R bounds each subsequent complex subtraction by an absolute bound even under cancellation. Define signed vectors at all slots, independently at both precisions:

```
E0 = x0-z                 B_E0 = D_fresh+R
E8 = x8-z^256             B_E8 = D_terminal+P+R
I8 = x0^256-z^256         B_I8 = Q+P+R
A8 = x8-x0^256            B_A8 = D_terminal+Q+R
E8-I8-A8                  B_identity = B_E8+B_I8+B_A8+2R
```

These B bound complex absolute error, and hence each real/imaginary component error. Compare signed residuals, not differences of maxima. For a computed component-Linf maximum m, report diagnostic interval [max(0,m-B),m+B]. A measured argmax is not a certified unique true maximizer. Tie-breaking for *represented* maxima is least slot, then real before imag. Report E0/E8/I8/A8 maxima, their argmax components and the signed E/I/A tuple at each maximizer in the ordinary primary output; retain existing ten-anchor stage output unchanged. No A80 acceptance is introduced.

## 5. Concrete comparisons, ceiling, and dispositions

T_obs=2^-120 and T_budget=2^-128 are exact rationals. T_E=2^-80 remains exclusively the old acceptance threshold. Every named D,H,J,P,Q,R,B, transport/replay allowance and combined allowance that is actually used must be <=T_budget. Zero is allowed. Excess is UNRESOLVED, not rounding down or substituting a larger tolerance.

All differences below are maxima of exact absolute differences of represented finite components. Extract exact dyadics and compare integers/rationals, including 512-to-768 widening, rather than subtracting in a rounded float. Thus comparison-arithmetic error is zero; serialization is separately budgeted in section 6.

| Comparison | Scope | Combined allowance b | Kind |
|---|---|---|---|
| Each sparse control p vs direct768 | all slots/components | D_p(K)+J_768(K) | TwoBoundedPaths |
| Fresh/terminal512 vs768 | all slots/components | D_512(K)+D_768(K) | TwoBoundedPaths |
| Endpoint p vs unchanged Horner512 | ten frozen anchors/components | D_p(K)+H_512(K) | TwoBoundedPaths |
| Endpoint p vs corresponding producer decoded values | all slots/components | D_p(K)+C_prod | Producer |
| E0/E8/I8/A8 signed512 vs768 | all slots/components | respective B_512+B_768 | TwoBoundedPaths |
| E8-I8-A8 vs zero | all slots/components, each p | B_identity,p | TwoBoundedPaths |

The endpoint values compared with the producer must come from the corresponding fresh/final ordinary public client Decrypt, never a different chain or wrong scale. The producer is not presumed exact: do NOT require its discrepancy <=observer-only b.

Exact classifier, in precedence order:

1. Malformed identity/shape/order, nonfinite, invalid rational, source mutation or invalid semantic control: FAIL. Negative d or b is invalid, not UNRESOLVED.
2. Raw finite d>T_obs: FAIL, even when model support is unavailable.
3. Unsupported conditional model/preconditions, or b>T_budget: UNRESOLVED.
4. For TwoBoundedPaths, d>b: FAIL (contradiction of the adopted conditional model), even when d<T_obs. This does not identify which path/library is wrong.
5. Otherwise d+b<=T_obs: PASS with CONDITIONAL assurance. If d+b>T_obs: UNRESOLVED (threshold overlap).

Old E80 predicates and `numericFailures` are not passed through this classifier or reinterpreted using B. They execute unchanged. Only their finite numeric misses are deferred, as already implemented. Model FAIL/UNRESOLVED never becomes a new global PASS; absent or fatal evidence is not PASS.

## 6. Canonical decimals and scalar-only replay

Each real/imaginary field is exactly119 ASCII bytes. The sole zero is `+0.` followed by109 zero digits and `e+00000`. A nonzero field has explicit sign, leading digit1–9, decimal point, exactly109 fractional digits, lowercase e, exponent sign and exactly5 exponent digits. Nonzero grammar is `[+-][1-9]\.[0-9]{109}e[+-][0-9]{5}`, with full-string ASCII matching, plus the semantic prohibition of `e-00000`. `e+00000` is the only zero exponent. Negative zero, a leading0 with nonzero fractional digits, alternate zero exponents, spaces, CR, LF, tabs, Unicode digits, infinities, NaNs, excess/missing digits, exponent overflow and trailing bytes are rejected. A zero-spelling is valid only for an exact represented zero; no flush-to-zero formatting.

Round the exact represented dyadic to110 significant decimal digits, nearest/ties-to-even, with carry renormalization. This is an evidence spelling convention, not a change to the paper's coefficient rounding convention. The C++ implementation must decide integer remainder/tie comparisons exactly; printing to an unchecked stream and assuming the locale/rounding mode is not sufficient. The C++ scanner and independent Python reader must implement their own lexical checks; neither invokes the other or imports project code. Python uses ASCII `fullmatch` plus the zero/exponent semantic rules and exact Decimal/Fraction parsing. The RED includes positive/negative/zero, half and exact integer halfway fixtures and15 malformed strings. The returned Python script checks grammar and integer fixture arithmetic only; it does not implement a live formatter or codec.

Do not treat110 digits as zero serialization error. For a canonical nonzero with decimal exponent e, q=0.5*10^(e-109) bounds component rounding; for exact zero q=0. Per complex vector set t=2*max(q_real,q_imag), a conservative complex bound. Compute these as exact rationals. Derivative amplification must be counted when reconstructing fresh error. In particular, blindly combining110 digits with the radius-2 factor2^263 can exhaust a2^-128 allowance.

For scalar-only Python replay of the completed sidecar, use local Decimal precision256, ROUND_HALF_EVEN, ample checked exponent range and finite arithmetic. Reconstruct exact dyadic z by integer rational arithmetic (terminating decimal), parse the110-digit residuals exactly, and use only eight explicit scalar complex squarings per power, not a transform. Assume the specified Decimal nearest model with u_dec=10^-255; use P_dec=2^270*u_dec and R_dec=2^264*u_dec. Record Python/Decimal environment in the job log. These are conditional roundoff bounds, not a second encryption.

Both the exact reconstructed fresh value z+serialized_E0 and the represented Decimal reconstruction must have component one-norm <=3/2, tested by exact rational comparison. With the live5/4 guard and D<=T_budget, the exact fresh value also lies in this disk. Use L_replay=256*(3/2)^255<2^158, checked by integer arithmetic. This sharper fixed radius is not fitted to a measured residual. If the guard fails, replay is UNRESOLVED, not a request for more digits or trials. Let t0,t8 be the worst serialization bounds, and let

```
U0 = D_768,fresh + R_768 + t0 + R_dec
B_replay_E0 = B_E0,768 + t0
B_replay_E8 = B_E8,768 + t8
B_replay_I8 = L_replay*U0 + 2*P_dec + R_dec
B_replay_A8 = B_E8,768 + t8 + L_replay*U0 + 2*P_dec + 2*R_dec
```

Here replay constructs fresh=z+E0, terminal=z^256+E8, then its I/A. Comparing replay with retained primary live I/A metrics/tuples uses the sum of its B and the corresponding live B, plus any explicitly bounded rounding of the reported metric. Do not compare only maxima when a signed tuple is available. All such sums must satisfy T_budget. For maximum values, the same triangle bound applies; do not require identical argmax when maxima are indistinguishable within these bounds. The exact integer/rational compare and maxima over parsed sidecar values themselves have no rounding error. The Python packer must not import/execute project code, C++ formatter, production codec, FHE, FFT or NTT.

The four-row proposal illustration is not live evidence. No synthetic TSV/status/gzip is included in this return. Future file-system tests use fresh disposable temporary directories, a distinct `fs-endpoint-synthetic-` namespace and explicit `scope=synthetic`; none may enter a live artifact upload. A four-row illustration cannot satisfy a16384-row check.

## 7. Live canonical sidecar schema v1-r1

The completed uncompressed sidecar is ASCII, no BOM or CR, TAB-separated, exactly one LF per line including the final line, no blank/trailing/unknown/duplicate lines. Maximum uncompressed size16 MiB and maximum line length32768 bytes including LF; these accommodate exact S8 integers. Enforce byte limits before large integer/Decimal allocation and reject any numeric field outside the checked arithmetic exponent range. Integer tokens are `0` or `[1-9][0-9]*`; negative integer metadata only where explicitly permitted, with no -0. Rationals are reduced with positive denominator; no whitespace. Commit and SHA-256 strings are lowercase hex of40 and64 characters respectively.

Line1 is exactly `#fs-residual-endpoint-01.v1-r1`. Then emit `meta<TAB>key<TAB>value` in **exactly this order**, with no keys omitted:

| key | value/validation |
|---|---|
| scope | `live-single-chain` |
| source_commit | actual compiled PAPER_SOURCE_COMMIT, equals hosted checked-out SHA/GITHUB_SHA; never the old tested source merely copied |
| baseline_tested_source | `9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e` |
| production_source | `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89` |
| openfhe_pin | `df495ba2e91739a6dc8f1de254fc5a41155ce504` |
| host | `linux` or `windows`, matches actual job |
| github_run_id | positive canonical decimal, matches environment |
| github_run_attempt | positive canonical decimal, matches environment |
| test_name | `paper_full_eight_square_contract` |
| chain_count | `1` |
| n | `32768` |
| m | `65536` |
| slots | `16384` |
| gap | `1` |
| input_formula | `frozen-four-phase-exact-dyadic-v1` |
| primary_precision_bits | `768` |
| check_precision_bits | `512` |
| significant_digits | `110` |
| scale0_numerator | exact S0 numerator |
| scale0_denominator | exact S0 denominator |
| scale8_numerator | exact independent reduced S8 numerator |
| scale8_denominator | exact independent reduced S8 denominator |
| coefficient_l1_fresh | actual exact C_fresh |
| coefficient_l1_terminal | actual exact C_terminal |
| fresh_max_l1_numerator | maximum exact represented abs(real)+abs(imag) over both precisions, numerator |
| fresh_max_l1_denominator | its reduced positive denominator |
| terminal_max_l1_numerator | analogous terminal maximum numerator |
| terminal_max_l1_denominator | its reduced positive denominator |
| model | `conditional-binary-nearest-direct-trig8u-v1` |
| assurance | `CONDITIONAL` |
| boost_version | actual positive integer BOOST_VERSION, not a claimed pinned1.83 |
| root_policy | `direct-own-precision-v1` |
| norm | `max-real-imag-component` |
| observer_tolerance | `2^-120` |
| estimator_ceiling | `2^-128` |
| original_error_gate | `2^-80` |
| rounding | `decimal-nearest-ties-even` |
| row_count | `16384` |
| check_count | `24` |
| numeric_gate_failures | actual old finite failure count, not hardcoded7 |
| E80_disposition | `PASS` iff count0, otherwise `FAIL` |
| A_disposition | `NOT_ADOPTED` |
| observer_disposition | `PASS` (complete publication only after all conditional integrity checks pass) |

Then exactly24 comparison records, each

```
check<TAB>id<TAB>PASS<TAB>d_num<TAB>d_den<TAB>b_num<TAB>b_den<TAB>argmax_slot<TAB>argmax_component
```

Use reduced exact rational maxima d and prescribed b, positive denominators and component `real` or `imag`; argmax is in0..16383 and obeys the tie rule. The Horner records use only the fixed ten anchors. Record IDs in this exact order:

```
control.constant.512
control.constant.768
control.x.512
control.x.768
control.xNminus1.512
control.xNminus1.768
control.sparse.512
control.sparse.768
fresh.cross
terminal.cross
fresh.horner.512
fresh.horner.768
terminal.horner.512
terminal.horner.768
fresh.producer.512
fresh.producer.768
terminal.producer.512
terminal.producer.768
residual.E0.cross
residual.E8.cross
residual.I8.cross
residual.A8.cross
identity.512
identity.768
```

The reader derives K and each b from exact C/scales and the named model/control constants, not trusting an arbitrarily emitted b. It checks d, b and classification. Comparison receipts cannot independently prove the roots or NTT were correct; source/control correspondence and the assurance qualification remain essential.

Next is exactly `slot<TAB>E0.real<TAB>E0.imag<TAB>E8.real<TAB>E8.imag`. Follow with **16384** rows, in order0 through16383, five fields per row, no prefix: canonical integer slot then four canonical signed binary768 residual spellings. Duplicates, skips, reordered rows, premature EOF, extra rows and extra columns fail. The C++ writer formats all rows to staging on the same filesystem, closes and reopens them, then strictly validates these byte/schema/identity/order rules before atomically renaming the file to its previously absent canonical-ready path. Do not place the file's own hash or byte count in any line.

Primary stdout additionally retains the24 comparisons and all original output, plus max E0/E8/I8/A8 and signed maximizing tuples. For these new diagnostic metrics use the same canonical spelling and include their rounding quantum in replay comparisons. Preserve existing45/100-digit legacy observations verbatim; they are not silently substituted for the new110-digit values. The status writer consumes the actual primary CTest61 stream and ignores its unprefixed failure replay.

## 8. External status, compression, and failure-preserving publication

The identity stem is

```
fs-residual-endpoint-01.v1-r1.<source_commit>.<host>.<github_run_id>.<github_run_attempt>
```

Each invocation owns a newly created, previously nonexistent publication directory and separate staging directory. Refuse preexisting identity outputs, symlinks, path traversal, foreign run IDs and overwrites. Do not reuse a previous run's output. Only `<stem>.tsv.gz` and `<stem>.status.json` may be retained/uploaded for the live endpoint evidence; an incomplete run retains status only. The canonical `.tsv` is private staging, not uploaded. No secret keys, ciphertexts, raw integer polynomials or synthetic fixtures are exported.

The packer validates the completed canonical file independently, identity against the actual primary BEGIN/COMPLETE/cleanup/numeric/scale receipts, all9 exact scale receipts, and the scalar replay. It runs **after the single CTest invocation even when CTest fails**. Primary E80 count must equal the number of primary numeric-failure records. A nonzero count requires original COMPLETE FAIL with the accumulated-numeric-failure reason; zero requires original COMPLETE PASS. Any other fatal reason, timeout, absent completion or incomplete cleanup is not complete endpoint evidence even if a staging file exists. Numeric counts are observed, never assumed equal7. [T:379–394; SL:8022–8026; SW:8344–8348]

For complete evidence only, compress the validated bytes as one gzip member, DEFLATE level9, mtime0, no FNAME/FEXTRA/FCOMMENT, XFL2, OS255, fixed ten-byte header `1f8b08000000000002ff`. Validate trailer CRC/ISIZE, reject concatenated members or trailing bytes, and decompress with the16 MiB ceiling; the result must equal the canonical bytes. Compression-library versions may produce different DEFLATE streams: canonical byte identity is authoritative, and the *actual* gzip bytes receive an external hash. Do not falsely require cross-zlib gzip identity.

Status is a single ASCII JSON object, sorted keys, separators `,` and `:`, no indentation, duplicate keys, NaN, floats or BOM, ending with one LF. JSON Booleans are not accepted as integer fields. Every object key below is mandatory and no other key is permitted:

```
schema, source_commit, baseline_tested_source, production_source, openfhe_pin,
host, github_run_id, github_run_attempt, test_name, chain_count,
evidence_state, reason, assurance, model, boost_version, E80_disposition,
A_disposition, observer_disposition, numeric_gate_failures, row_count,
canonical_bytes, canonical_sha256, gzip_bytes, gzip_sha256, gzip_filename,
status_filename, ctest_exit_code, exit_code_convention, packer_disposition
```

`schema` is `fs-residual-endpoint-status-v1-r1`. Identity/model/assurance/name fields have the preceding values; run IDs are decimal **strings** (avoiding JSON range ambiguity), chain_count and byte/row/count/Boost/exit fields are JSON integers when known. `exit_code_convention` is `shell-status`; `ctest_exit_code` records the actual enclosing Bash shell status0..255, including its signal convention, never merely a Boolean. Windows uses the existing MSYS Bash workflow, not a newly invented PowerShell mapping. Status contains its filename for identity but **neither its own SHA nor its own byte count**.

Complete status: evidence_state=`COMPLETE`, reason=`NONE`, observer_disposition=`PASS`, packer_disposition=`PASS`, row_count=16384, valid non-null canonical/gzip hashes and sizes, filename exactly the stem, observed numeric_gate_failures, E80 PASS/FAIL consistent with both primary records and CTest status (PASS requires zero; E80 FAIL requires nonzero). Assurance remains CONDITIONAL. A remains NOT_ADOPTED. `packer_disposition=PASS` means complete evidence was validated, not CTest or E80 passed.

Incomplete status has chain_count=null: a BEGIN declaring a planned chain is not proof that a complete encrypted chain was observed. Incomplete status: evidence_state is `UNRESOLVED`, `FATAL` or `MISSING`; reason is one of `MODEL_UNSUPPORTED`, `ESTIMATOR_CEILING`, `CONDITIONING`, `NONFINITE`, `INTEGRITY`, `IDENTITY`, `FORMAT`, `REPLAY`, `CTEST_FATAL`, `TIMEOUT`, `NO_CANONICAL`, `IO_ERROR`. Choose the first detected cause in execution order; do not overwrite a concrete earlier cause with NO_CANONICAL. Model/ceiling/conditioning causes are UNRESOLVED; malformed/nonfinite/semantic/IO causes are FATAL; absent unexplained canonical evidence is MISSING. `packer_disposition=FAIL`; observer_disposition is UNRESOLVED, FAIL, or NOT_OBSERVED according to that cause. No gzip is published; canonical/gzip sizes, hashes and gzip_filename are null; row_count=0. Numeric count and E80 are retained only when the complete primary E80 record is available, otherwise count=null and E80=`NOT_OBSERVED`. Boost version is null if the process never reported it. No null is replaced by a fake zero hash or invented value. A status write failure remains a failing job, not a successful empty upload. The status is bounded to256 KiB.

Publish transactionally: validate the closed gzip and status in private staging, rename gzip first and validated status last as the commit marker. On failure, remove any orphan final gzip before publishing incomplete status. Upload selection must read and validate the final status: COMPLETE requires both matching files, incomplete selects status only, and absent/invalid status fails rather than uploading an orphan gzip. Filesystem/cleanup failures remain job failures; no claim of crash-proof durability is made.

Failure sequence in eventual GREEN:

1. The C++ normal path computes endpoints/residuals, runs the old downstream controls and observes old cleanup. Only finite old E80 misses are deferred. After those controls, close/reopen/validate the full canonical evidence **before** the old `Require(numericFailures==0,...)`. Then execute that Require unchanged. Fatal semantic/nonfinite or new integrity failures remain fail-fast and are not converted into complete sidecars.
2. Invoke the existing paper CTest exactly once, capturing its primary log and actual return code. Invoke the independent packer/status finalizer once irrespective of that code. Do not invoke the executable again to recover evidence. If CTest was nonzero, propagate that original nonzero status after finalization; if it was zero but packing failed, return a nonzero packaging failure. No success-forcing pipe, `continue-on-error` or unconditional zero exit may hide either failure.
3. Eventual always-run artifact upload runs even after finite E80 failure and targets only the exact intended gzip/status paths for this identity. Incomplete runs upload only the explicit status; absence of both files is an upload error. No recursive wildcard, temporary canonical, synthetic namespace, debug key material or broad working directory upload. A failed upload is not silently ignored. The workflow job remains nonzero when CTest or packing failed.

The status can be hashed by a later external delivery receipt; no recursive self-hash fixed point is requested. Publishing complete evidence is not guaranteed after process death; truthful incomplete status is the only permitted fallback.

## 9. TDD boundary and stopping rule

RED.patch contains only the existing paper executable's include/dispatch and one new test-contract/fixture header. Oracle, CMake, workflow, all production code, all legacy tests and API inventories are byte-identical. No CTest entry or workflow self-test invocation is added in this candidate. The missing functions should cause an honest paper-target linkage/API failure, **after** the existing old60/five explicit API checkpoint. Host compilation determines the actual failure; it was not run here. An unrelated earlier compiler failure is not the intended RED and must be reported as such.

Codex adopts the specification, integrates the exact patch, retains hosted Linux/Windows RED, and supplies a new complete handoff before any GREEN code is authored. Future GREEN first implements only the missing observer/evidence helpers and the failure-preserving wiring, runs the deterministic no-crypto self-test, then makes at most **one next single paper chain per host** in the existing normal test. Keep all old checks. See TEST_PLAN.md for the execution ledger/order and negative evidence fixtures. This spec does not authorize a dispatch or a current GREEN implementation.

Stop after those bounded observations and classify their evidence. A complete conditional observer result plus small A is diagnostic progress only, with original E80 separately retained. A discrepancy identifies a concrete slot/path to investigate, not automatic permission for another chain. Unsupported model, timeout, missing evidence or integrity failure stops with FAIL/UNRESOLVED. No quota, statistical replication, random retries, key selection, full-slot intermediate campaign, performance project or final acceptance follows automatically.
