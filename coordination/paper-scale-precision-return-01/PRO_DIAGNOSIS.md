# Independent diagnosis of the first paper-scale precision failure

**Disposition: TEST-ONLY DIAGNOSTIC CANDIDATE; no source-proven production fix; no implementation PASS.**

Review date: 2026-09-05. Baseline: `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89`. Documentation checkpoint: `1853701d8862dabef804021d2dea1899776f38e2`. OpenFHE pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`. Branch: `codex/paper-scale-implementation-20260905`.

## 1. Conclusion and evidence status

The Windows failure is a real miss of the frozen **end-to-end** component-error gate, not a Linux compiler failure, a repeated experiment, or a demonstrated codec failure. At round 4, slot 512 has error `1.519830543686982243577737227091582941391260472e-24`, approximately **1.83736238570213248 times 2^-80**. The same fresh ciphertext already has a slot-512 perturbation whose ideal propagation to the sixteenth power is bounded below by approximately **1.49406650020095744e-24**, independently of the unknown signed direction of that perturbation. Thus inherited fresh error alone is sufficient to cross this gate even with otherwise ideal subsequent arithmetic. The actual round-4 error lies inside the inherited-error interval. This is strong support for the proposed explanation, but it does **not** establish that the actual accumulated arithmetic residual is small: unsigned error norms cannot resolve reinforcement or cancellation. [W:7449,7499–7508; analysis in §4 below.]

There is also evidence of a nonzero departure from pure fresh propagation. For example, round-1 slot 257 exceeds its pure-propagation upper bound by at least approximately `5.94017378694750e-28`. That is a **lower bound** on an aggregate residual, not its measured size, a noise-component estimate, or proof of a defective operation. The source contains ordinary discarded-low-product, relinearization, and rescale error mechanisms capable of producing additions. No specific incorrect production operation has been proved by this review. [W:7448,7464; P/src/double_ckks.cpp:826–891,1011–1056,1143–1189.]

The supplied public-key generation/encryption path explains why h128 does not by itself imply a correspondingly sparse fresh-noise path: the root secret has weight 128, but OpenFHE public encryption samples its ephemeral polynomial using **dense uniform ternary coefficients**. This is established from the pinned constructors and sampler, not inferred from the name of the secret distribution. The paper does not supply enough experimental detail to assert that HEaaN used a different ephemeral distribution. [P/src/paper_h128_client_keypair.cpp:193–217; O/src/pke/lib/schemerns/rns-pke.cpp:148–196; O/src/core/include/math/ternaryuniformgenerator-impl.h:103–108; T:223–237,1562–1590.]

The delivered patches therefore retain the production implementation and all frozen values. They add signed client-side observations of inherited, accumulated, and local errors to the **same one chain**; finite acceptance misses are counted and still cause exit failure after valid later observations and ownership cleanup. A separate allocator-backed `AnchorRoots` patch addresses the Linux compilation path. Its isolated scalar version compiled locally, but neither complete patched paper target nor hosted production CI was executed here.

### Citation convention

All line references in this report are to the **unmodified input snapshot**, unless explicitly identified as delivered files:

- **P/** = `project/`; **O/** = `official-full/`; **B/** = `boost-1.83.0/include/boost/`.
- **T** = `paper/PAPER-2023-1788.txt`; PDF references use the physical, 1-based page in `paper/PAPER-2023-1788.pdf`, plus section/definition/table.
- **W** = `evidence/WINDOWS_LF.log`; **L** = `evidence/LINUX_RAW.log`.
- Newly produced scalar evidence is under this delivery's `evidence/`, not the incoming evidence directory.

“Observed” below means present in retained raw evidence or actually executed in this review. “Derived” means a stated mathematical/source inference. “Pending” means no corresponding numerical or hosted validation exists.

## 2. Input identity, run identity, and reachability

### 2.1 Verified closure

The attached ZIP is **1,480,623 bytes**, SHA-256:

`28fa7ee1297af78ac4c2848b85d1fce1efdd81bbd1a13d15b0e75076c2719336`

CRC verification passed. All **134 members are unique safe regular files**, with **133 manifest payloads plus the self-excluded MANIFEST.json** and 4,548,367 expanded bytes. Every manifest size and SHA-256 matches; the extracted files remain byte-identical. The manifest records the baseline, checkpoint, branch, and official pin stated above. These checks establish internal archive/manifest integrity, not an independent remote attestation of a Git commit. See `evidence/input_verification.json` and `checks/verify_input.py`.

The review used the supplied four production modules and headers, both paper test files, required seam/acceptance/contract/audit documents, relevant pinned official files, paper, and raw logs. The initial view and independent scalar calculation were recorded before reading the separately labelled fresh-propagation audit; that ordering is recorded in `evidence/PRE_AUDIT_VIEW.md`. It is an execution record, not a cryptographically attested chronology. No private repository, old implementation, previous attachment, or earlier conversation supplied implementation evidence.

### 2.2 What actually ran

`evidence/RUN_TERMINAL_01.json` identifies run **33971779479**, push attempt **1**, head `b1b024e...`, and two completed FAILURE jobs. Its job/step records and raw logs agree:

| Host/job | Pre-paper validation | Paper target | Paper numerical observation |
|---|---|---|---|
| Linux / 101321455160 | Five API build steps successful; 60/60 regressions passed | Compilation failed | **None**; focused paper step skipped |
| Windows / 101321455226 | Five API build steps successful; 60/60 regressions passed | Compiled | **One** chain; client check stops at round 4 |

Linux's complete regression result is at L:5451–5453. The two `-Werror=array-bounds` failures and promoted binary-float conversion stack are at L:5454–5497, specifically L:5472 and L:5485. Windows' 60-test result is at W:5773–5775. The live paper stream begins at W:7380 and ends at W:7508. Later unprefixed CTest replay is not a second execution.

The first stream's measurements are:

| Quantity | Observed value | Location |
|---|---:|---|
| Fresh all-slot maximum component error | 3.380628068541526514072840073085484952608435963e-25 | W:7441 |
| Fresh 160/220 codec disagreement | 4.327899444029515408578455222124729267038817044e-165 | W:7442 |
| Fresh maximum over ten anchors | 1.081632465294796426394472742886068841698281297e-25 | W:7449,7455 |
| Fresh independent-anchor/production disagreement | 4.954005368621470060097734507696221211843730662e-102 | W:7456 |
| Round 1 anchor maximum | 2.148498437837611997103189667813446415046284698e-25 | W:7471 |
| Round 2 anchor maximum | 4.217839811308896046540254884767216034003689298e-25 | W:7483 |
| Round 3 anchor maximum | 8.144972185928475769270431936848113164028308842e-25 | W:7495 |
| Round 4 anchor maximum | 1.519830543686982243577737227091582941391260472e-24 | W:7507 |

The fresh and rounds 1–3 gates pass. Round-4 anchors **257, 512, and 768** exceed `2^-80 = 8.2718061255302767487140869206996285356581211090087890625e-25`; slot 512 is maximal. The original check throws after emitting the round-4 anchor maximum. [W:7497–7508.]

**Reachability matters.** `Evaluate` constructs all eight stages and the owned terminal RCB result before returning; only then does `RunPaper` perform the client-side checking loop. Reaching the observed round checks therefore indirectly establishes that evaluation returned, not that later ciphertexts were numerically accepted. Client round-5–8 checks, final exact-scale binder/decode, final full-slot/witness checks, and the explicit post-destruction ownership acceptance were **not reached**. No final numerical result or cleanup PASS may be inferred. [P/tests/paper_full_eight_square_contract_test.cpp:180–194,278–340,348–362.]

## 3. Error accounting and the discriminating observation

For each diagnostic slot, let `z` be the original exact dyadic input; `w0` the independent high-precision decryption of the fresh ciphertext at exact S0; and `wr` the independent decryption of the round-r recombined pair at exact Sr. Define:

```
Er = wr - z^(2^r)                  original end-to-end error
Ir = w0^(2^r) - z^(2^r)            inherited fresh error
Ar = wr - w0^(2^r)                 accumulated added residual
Lr = wr - w(r-1)^2                 one-step residual
```

These satisfy `Er = Ir + Ar`. With `vr = w0^(2^r)`, `A0 = 0`, and the corresponding previous value,

```
Ar = 2*v(r-1)*A(r-1) + A(r-1)^2 + Lr.
```

These are complex, signed quantities. Subtracting maxima such as `max|Er| - max|Ir|` does not recover `Ar`; even subtraction of unsigned norms at the same slot yields only inequalities. A large `Ar` can cancel a large `Ir`. Both real and imaginary components are needed, because squaring rotates as well as amplifies an input perturbation.

The proposed diagnostic retains the existing independent decryption, cpp_int CRT, exact scale calculation, and binary512 direct-Horner roots/oracle. It stores ten fresh anchors, ten previous anchors, and ten fresh-power baselines. For each round it emits **signed E, I, A, and L**, with 100 decimal places in scientific output, plus ten-anchor component maxima for I/A/L. It also emits signed fresh `w0` and fresh `E`. All calculations stay in the original binary512 `Real`; printed strings are not fed back into the computation. The unchanged original end-to-end norm observations and assertions remain separately visible.

Only original public seams remain inside `Evaluate`. There is no evaluator decryption, secret dependency, second encryption, extra chain, refresh, or instrumentation API. Computing `w0^(2^r)` is a diagnostic reference only; it is not substituted for the accepted truth `z^(2^r)`.

The diagnostic also emits the aggregate `max_j |centered coefficient_j| / Sr` for the already-decrypted polynomial. This provides an actual conditioning input for the direct-Horner oracle without logging coefficients or secret material. It is not itself an error gate or a no-wrap certificate.

The requested decomposition distinguishes inherited error from cumulative and local arithmetic contributions. It intentionally does **not** separately identify coefficient rounding versus each encryption-noise summand, or Tensor2 versus Relin2 versus RS2 inside a local residual. Existing observations and source bounds address those possibilities below. Adding new transform/secret plumbing merely to split those subcomponents would be a larger experiment than needed to answer the current leading alternative.

## 4. Independent propagation calculation and challenge to the supplied audit

### 4.1 Bounds available without signed errors

Write `w0 = z + epsilon`, `e = ||epsilon||component_inf`, and `D = p*z^(p-1) = a+i*b`, where `p=2^r`. Multiplication by D has real matrix `[[a,-b],[b,a]]`. Its induced component-infinity norm is `|a|+|b|`, and the inverse norm gives

```
(a^2+b^2)/(|a|+|b|) * e <= ||D*epsilon||component_inf
                           <= (|a|+|b|) * e.
```

A Taylor remainder bound is

```
R <= p*(p-1)/2 * (|z| + sqrt(2)*e)^(p-2) * (sqrt(2)*e)^2.
```

Subtract/add R at the two endpoints to bound `Ir`. This accounts for every direction consistent with the observed fresh component norm. `checks/analyze.py` independently reconstructs the frozen dyadic inputs using exact Fractions and evaluates these bounds using 120-decimal Decimal arithmetic, with a conservative `1e-42` relative allowance for the printed error norms. This is high-precision evaluation of analytic inequalities, **not directed-rounding interval arithmetic**. The decisive margins are vastly larger than the displayed rounding uncertainty.

The script parses only `61: OBS` lines, rejects duplicate field names, and finds 60 distinct numeric fields including exactly 50 fresh/round anchor norms. It does not interpret replay as new data. The separately re-executed supplied audit explicitly bounds the first BEGIN/COMPLETE interval to W:7380–7508 and confirms those same 50 anchor observations. [This delivery: `checks/analyze.py`, `evidence/independent_scalar.json`, `evidence/supplied_audit_reexecution.txt`.]

### 4.2 Slot 512

For the observed fresh slot-512 error, the derived inherited component-error intervals are:

| Round | Lower bound, approximately | Upper bound, approximately | Actual end-to-end norm |
|---|---:|---:|---:|
| 1 | 2.14214345509119122e-25 | 2.14636442331936166e-25 | 2.14849843783761200e-25 |
| 2 | 4.20105872668846105e-25 | 4.22589241871197645e-25 | 4.21783981130889605e-25 |
| 3 | 8.07902969199732728e-25 | 8.19046101487341575e-25 | 8.14497218592847577e-25 |
| 4 | **1.49406650020095744e-24** | **1.53821965758586599e-24** | **1.51983054368698224e-24** |
| 8, counterfactual only | 2.39531096370729711e-24 | 3.54885792737019448e-24 | **Not observed** |

The round-4 remainder bound is approximately `2.48141166488658333e-48`. The supplied audit's `2.80782909595127759e-48` is a valid looser bound obtained by replacing the radius factor with 1. Both produce the same material conclusion.

At round 4 even the inherited lower endpoint exceeds the gate. At round 8 the ideal-propagation interval is approximately **2.89575–4.29031 gates**, but this is emphatically **not** a prediction of the actual final error: later arithmetic can reinforce or cancel it. Nor does the round-4 match prove dominance in the measured error vector. The forthcoming signed A measurement is the discriminating evidence.

### 4.3 What unsigned data additionally proves—and does not

The reverse triangle inequality implies a lower bound on `||Ar||` whenever an observed end-to-end norm lies outside the inherited interval. At round 1 the independently calculated lower bounds include:

- slot 257: approximately `5.94017378694750e-28`;
- slot 512: approximately `2.13401451825034e-28`;
- slot 256: approximately `3.42366398597776e-28`.

These bounds apply to the recorded oracle values. Interpreting them as bounds on exact cryptographic residuals also assumes the corresponding oracle error is negligible; the conditioning observation and preserved oracle-validity checks address that qualification. They do not alone exclude oracle roundoff as part of the discrepancy. All are tiny relative to the failed gate, but **lower bounds being tiny does not prove the actual residual is tiny**. Values of zero in the accompanying lower-bound table mean “no positive lower bound from these unsigned observations,” not “no arithmetic error.” The script also derives selected local-residual lower bounds from consecutive unsigned errors; the same qualification applies.

I agree with the supplied audit's principal propagation interval and its public-encryption interpretation, and reproduced its saved script with exit 0. I do not upgrade its plausible “small extra arithmetic error” hypothesis to a measured magnitude, a particular error source, a final result, or a guarantee over keys. Those are exactly the claims requiring the signed diagnostic.

## 5. Initial encoding/encryption error: source-level separation

### 5.1 Encoding and codec

The client encoder independently constructs the 160- and 220-decimal transforms, performs stable nearest coefficient rounding, builds an integer DCRT plaintext, and invokes official **public-key** encryption. It does not encode through binary64 or use secret-key encryption for the user plaintext. [P/src/high_precision_client_io.cpp:52–55,329–358,420–433,541–596, particularly 559–584.]

Conditional on the correctly implemented/audited transform and nearest rounding, each unscaled polynomial coefficient contributes at most 1/2 of rounding error. Therefore a conservative slot-component bound for coefficient rounding alone is

```
N/(2*S0) = 2^-86 = 1.292469707114105741986576081359316958696581423282623291015625e-26.
```

The observed fresh all-slot maximum exceeds this by approximately `3.25138109783011594e-25`. Consequently ordinary nearest coefficient rounding alone cannot explain the fresh discrepancy. This argument does not rule out an arbitrary encoder bug; independent fresh anchor/production agreement around `4.95e-102`, and codec cross-precision disagreement around `4.33e-165`, strongly disfavor that alternative. [W:7441–7442,7456.]

Neither cross-precision agreement nor an error bound splits the actual signed initial perturbation into rounding and encryption summands. The returned I deliberately includes both. A future need to split them exactly would require observing the exact encoded plaintext through an independently justified client seam, not reusing production encoding as its own oracle or weakening encryption.

### 5.2 Actual public-key and ephemeral sampling path

`CreatePaperH128ClientKeyPair` constructs the root secret with the explicit Hamming weight argument 128. It then obtains public-key components using the official private-key `EncryptZeroCore` and moves the resulting elements into a public-key object. This is legitimate public-key generation, not substitution of secret-key encryption for the later user-data encryption. [P/src/paper_h128_client_keypair.cpp:193–217.]

The official key-generation primitive yields `(a*s + e_pk, -a)` at noiseScale 1. This is the usual RLWE public key after the harmless renaming `a'=-a`; it is not a sign error. Official public encryption subsequently forms

```
c0 = (a*s+e_pk)*v + e0 + m
c1 = (-a)*v + e1
c0+c1*s = m + e_pk*v + e0 + e1*s.
```

This follows directly from O/src/pke/lib/schemerns/rns-pke.cpp:56–69,111–145,148–196. The frozen profile supplies noiseScale 1 and Gaussian parameter 3.19F. [P/src/repeated_mult2.cpp:155–190.]

Crucially, at O/src/pke/lib/schemerns/rns-pke.cpp:164–165 the non-Gaussian-secret branch constructs `DCRTPoly(tug, elementParams, EVALUATION)` **without a weight argument**. The constructor defaults `h=0` [O/src/core/include/lattice/hal/default/dcrtpoly.h:109–112]; the implementation samples one logical coefficient vector and installs it consistently in all towers [O/src/core/include/lattice/hal/default/dcrtpoly-impl.h:177–192]; and `GenerateIntVector(...,0)` uses independent uniform values in `{-1,0,1}` [O/src/core/include/math/ternaryuniformgenerator-impl.h:103–108]. Thus the ephemeral v is dense ternary although the secret is h128.

Under a heuristic independent-coefficient model, the dominant `e_pk*v` contribution has per-component slot RMS approximately

```
sigma*N/(sqrt(3)*S0) ≈ 4.760805339e-26.
```

Including e0 and an averaged h128 secret contribution gives a rough complex-slot RMS `sigma*sqrt(N*(2*N/3+128+1))/S0 ≈ 6.752645305e-26`. This uses sigma approximately 3.19; the actual configured literal is 3.19F. These are model-scale calculations, **not measurements of this ciphertext**, independent-slot assumptions, failure probabilities, exact discrete-Gaussian variances, tail guarantees, or a security certification. They explain plausibility, not a precise attribution of the fresh maximum.

The client decode uses the official integer `Poly*` decryption path and the exact receipt scale. The official CKKS code adds flooding only under the NOISE_FLOODING_DECRYPT/EVALUATION branch; the frozen FIXED_NOISE configuration does not take it. Fresh codec disagreement is therefore not an indication of deliberate flooding introduced during this decryption. [P/src/high_precision_client_io.cpp:664–717; O/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp:70–95.]

## 6. Production mapping and ranked alternatives

### 6.1 What the source establishes

**DCP:** the high part is the rounded quotient after dropping d; the low part is constructed as the original prefix minus d times that high part. Recombination is exactly the original ciphertext prefix modulo that prefix. DCP is not a refresh or a mechanism to remove inherited encryption error. A semantic interpretation without prefix wrap is still required. [P/src/double_ckks.cpp:398–438; paper PDF p.6, Definitions 3.1/3.3 and Lemma 3.4.]

**Tensor2/Relin2/RS2:** Tensor2 retains high-high and the two high-low cross products, intentionally omitting low-low. Relin2 uses the d-lift/full-family relinearization and decomposes its remainder rather than naively treating the high ciphertext as independently scaled. RS2 rescales both the high and the recombined expression, then makes the low remainder by subtraction. Mult2 composes those operations. [P/src/double_ckks.cpp:826–891,1011–1056,1143–1189,1213–1224; paper PDF pp.7–8, Definitions 4.1/4.3/4.5/4.7.]

For intuition, under correct lifts/scales and nonwrap, if B denotes the previous decrypted low polynomial in canonical embedding, the discarded low-square contribution to a squared slot is `-(B/Sprev)^2`. Additional aggregate Relin2 and rescale errors contribute after their respective divisors. This is a mechanism decomposition, not a measured decomposition of L. Small local errors are expected even for a correct implementation, and their later propagation belongs in A.

**Exact scale:** the production receipt stores the integer-rational recurrence `Sr=Sprev^2/(d*mr)`; the test uses an independent closed-product integer expression. Nominal50 recorded metadata is deliberately distinct from this scale. The final binder/decrypt path uses the exact receipt, not a forced 2^100. [P/src/repeated_mult2.cpp:254–285; P/tests/paper_full_eight_square_oracle.h:76–91; P/src/high_precision_client_io.cpp:645–662,664–717.]

Official FIXEDMANUAL multiplication only aligns levels on the approved equal-state path; official rescale uses the actual CRT tail-prime tables. Metadata scaling is separate. There is no source evidence here for dividing polynomial coefficients by the nominal 2^50 instead of the actual Mult prime. [O/src/pke/lib/schemerns/rns-leveledshe.cpp:182–191,479–484; O/src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp:172–190; O/src/core/include/lattice/hal/default/dcrtpoly-impl.h:691–712; P/coordination/paper-scale-integration-01/NOMINAL_SCALE_AUDIT_01.md:9–24.]

For the frozen primes, `S8/S0 ≈ 1.000364850389603923`. Decoding a meaningful ~0.1 terminal value at nominal S0 would introduce an error around 10^-5, not 10^-24. This is a pending negative-control comparison in the original test, not an observed final decoding result. [P/coordination/paper-scale-integration-01/INPUT_DOMAIN_AUDIT_01.md:22–44; P/tests/paper_full_eight_square_contract_test.cpp:309–321.]

**Ownership and reentry:** one root secret is projected by matching full prime/root identities for client-generated key families; public plan/result objects contain evaluation material, not the secret. Reentry retains polynomials and exact scale, changing the family wrapper rather than refreshing. [P/src/repeated_mult2.cpp:226–240,315–347,412–432,460–499; P/coordination/TEST_SEAMS.md:23–37.]

No wrong prime order, inconsistent secret projection, duplicate fresh-noise addition, incorrect exact-scale division, or provably wrong Mult2 formula was established. This does not turn source review into proof of every arithmetic path.

### 6.2 Ranking and falsifiers

| Rank | Explanation/status | Evidence against or for it | Discriminating observation |
|---|---|---|---|
| 1 | **Inherited fresh error amplified by squaring: strongly supported** | Direction-independent round-4 lower bound exceeds gate; observed value is inside interval; actual public encryption noise path is source-consistent | Signed I and A: small A relative to I supports actual dominance; comparable/large A refutes the stronger “small added error” claim |
| 2 | **Added residual in recorded oracle values: evidenced at some anchors; ordinary Mult2 errors plausible** | Positive reverse-triangle residual lower bounds, subject to oracle accuracy; low-square omission, key switching, and rounding are real algorithmic mechanisms | L shows where additions enter; A measures their propagated aggregate without confusing them with I |
| 3 | **Small production arithmetic defect: unexcluded, not proved** | Unsigned ~10^-28 departures cannot identify a faulty operation; source mapping does not reveal one | Unexpected large/local jumps or repeated structured L motivate a narrowly targeted public Tensor2/Relin2/RS2 diagnostic/regression, not an immediate speculative fix |
| 4 | **Exact-scale/normalization defect: gross forms strongly disfavored** | Correct integer receipts and pinned rescale path; gross d/m/nominal errors have very different size | A common relative term `Lr ≈ eta*wr-1^2` across unrelated anchors, or receipt/coefficient inconsistencies, would revive this alternative |
| 5 | **Oracle/codec error: strongly disfavored at fresh and round 4, not globally certified** | Fresh independent agreement near 10^-102; codec agreement near 10^-165; round-4 modulus/precision conditioning ample | B/S conditioning aggregate, preserved finite/agreement checks, and original independent final anchor/production comparison constrain this alternative |

A conditioning qualification is important. The binary512 direct-Horner calculation has a documented arbitrary-fresh-modulus caveat: very large coefficients could lose accuracy by cancellation. At round 4 the active modulus is at most about 340 bits, S is about 2^100, and centered coefficients give B/S below roughly 2^239. A conventional conservative estimate `16*N^2*(B/S)*2^-512` is then around or below 2^-239, far below 2^-80. This is an ordinary-rounding scale estimate, **not a formal certification of Boost transcendental routines or every arbitrary fresh-Q polynomial**. The added observed B/S avoids assuming ideal conditioning when reading future data. [P/tests/paper_full_eight_square_oracle.h:31–41,188–248; P/coordination/paper-scale-integration-01/ORACLE_ADVERSARIAL_AUDIT_01.md.]

Similarly, positive headroom after centered reduction does not independently prove that earlier computation did not wrap: centered representatives necessarily lie in the chosen interval. Existing fail-fast state/nonwrap checks and end-to-end truth comparisons remain necessary. Ten intermediate anchors are not a full-slot intermediate theorem; the original final full-slot comparison remains essential. [P/coordination/paper-scale-integration-01/INPUT_DOMAIN_AUDIT_01.md:38–44,63–69.]

## 7. What Section 6.3 actually establishes

The supplied paper describes an HEaaN proof-of-concept implementation, then a high-precision experiment with depth 8 and nominal 100-bit plaintext precision. For Double-CKKS it gives N=2^15, h128, d approximately 2^40, eight Mult primes approximately 2^60, two Base primes approximately 2^50, P approximately 2^60, and maximum QL*P approximately 680 bits. The root encryption Q in this packet is about **620 bits**, with P accounting for the additional 60 bits; Q must not be called a 680-bit encryption modulus. [T:1464–1470,1562–1590; paper PDF p.13, §6.3/Table 3.]

Section 6.3 reports eight repeated squarings of one ciphertext and **average infinity norms of errors over 1,000 executions**, giving log2-error values approximately **-81.2 for t=1 and -81.8 for t=2**. The experiment does not use the recombine/decompose refresh strategy of Section 6.2. These are empirical average precision results, not a theorem that every encryption, key, slot, or intermediate stage meets 2^-80. The present user explicitly cancelled a 1,000-trial acceptance requirement; reporting the paper's sample count does not reinstate it. [T:1572–1576; P/coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md:3–6,26–35.]

The following distinctions are necessary:

| Item | What is actually supplied by the paper | What cannot be concluded from it |
|---|---|---|
| Input values | Depth 8 and nominal 100-bit plaintext precision | The exact slot generator, amplitudes/phases, probability law, or this packet's dyadic four-phase vector; “100 bits” does not specify a uniform input law or values near 2^100 |
| Secret / encryption randomness | Table 3 specifies secret weight 128; §2.1 parameterizes encryption/error distributions by chi_enc/chi_err | Actual Section-6.3 ephemeral weight, Gaussian sigma, detailed sampling method, or which keys/randomness were reused across 1,000 executions |
| Error reference | Formal multiplication sections compare with products of decrypted operands; full-circuit motivation discusses approximating plaintext computation | Whether the empirical Section-6.3 error specifically subtracts original z powers or fresh decrypted powers; no explicit subtraction procedure is supplied |
| Norm | §6.1 names infinity norm in the canonical embedding; §6.3 says average infinity norms | Exact measurement code for complex magnitude versus separately stored components |
| Aggregation | Average of error norms over the reported executions | Worst case, a per-key guarantee, each intermediate gate, or an all-key theorem |

The mathematical conventional infinity norm on complex slots is `max_s |error_s|`. For each vector,

```
max_s max(|Re error_s|,|Im error_s|)
  <= max_s |error_s|
  <= sqrt(2)*max_s max(|Re error_s|,|Im error_s|).
```

Thus a norm-convention change cannot rescue a component-error failure: the corresponding complex magnitude is at least as large. Nor does an average error below a threshold imply every observation is below it. [T:1503,1574–1576; the inequality follows directly from complex magnitude.]

The paper's formal one-multiplication correctness statements take decrypted inputs as operands; inherited fresh error enters a comparison with original input powers through subsequent error propagation. It would be incorrect to infer from this formal convention that the reported experiment definitely excluded fresh noise. Conversely, it would be incorrect to assert that it definitely included it when the empirical subtraction procedure is not specified. [T:256–278; PDF pp.5,7–8, §2.2 and Definitions 4.1–4.7; T:1574–1576.]

**Adjudication:** the frozen project criterion is a distinct, explicit end-to-end acceptance target. The paper's empirical mean does not certify it for the documented OpenFHE public-encryption path and current input. Missing experimental details are missing details, not proof of an HEaaN/OpenFHE distribution difference or authorization to redefine truth. The original failure remains a failure.

## 8. Delivered diagnostic and portability patches

### 8.1 DIAGNOSTIC.patch

Only these two repository files change:

```
tests/paper_full_eight_square_oracle.h
tests/paper_full_eight_square_contract_test.cpp
```

The patch adds small scalar helpers, observations, and a local failure counter. It does not change production files, public interfaces, CMake, workflow, root/prime tables, distributions, input generation, scales, anchors, precision, or test enumeration. The original evaluator, sparse decryption, CRT, roots, and Horner functions are byte-identical in the diagnostic-only variant. The count of `client.Encrypt` calls remains one. Existing independent final Horner and full-slot checks are not replaced.

**Failure handling:** only the original end-to-end 2^-80 maximum-error predicates and sub-binary64 witness predicate are converted from immediate throw to counted failures. Their inequalities and thresholds are unchanged. Finite validation, shape, state, roots, receipts, ownership, explicit domain/nonwrap checks, codec 2^-120 agreement, and anchor-versus-production agreement remain fail-fast. In particular an untrustworthy oracle/codec disagreement is not treated as permission to continue. No arbitrary exception is caught and ignored. The original outer catch-to-exit-failure is retained; it does not resume an invalid chain.

When only those finite numeric gates miss, client diagnostics continue through the already-evaluated round-5–8 states and the original final binder/decode/full-slot/witness checks. Objects then leave their original scope; the original ownership/cache cleanup checks run. **`Require(numericFailures==0, ...)` occurs after cleanup checks and before the sole COMPLETE PASS marker.** A new run in which a frozen numeric predicate fails cannot become green just because later residuals look small. A genuinely passing new run is not artificially forced to fail either.

The patch preserves original field names and adds, for each anchor:

```
diag.fresh.anchor_<slot>.w0.real/.imag
diag.fresh.anchor_<slot>.E.real/.imag
diag.round_<r>.anchor_<slot>.E.real/.imag
diag.round_<r>.anchor_<slot>.I.real/.imag
diag.round_<r>.anchor_<slot>.A.real/.imag
diag.round_<r>.anchor_<slot>.L.real/.imag
```

The I/A/L maxima are over **ten anchors only**, explicitly labelled `*_anchor_max_component`; they are not all-slot bounds. `numeric_gate=FAIL` names a failed original predicate, and the final `numeric_gate_failures` aggregate records retained misses. No secret, credential, raw key row, or per-coefficient plaintext dump is introduced. These logs concern the deliberately public deterministic test vector and its diagnostic errors, not a proposed production logging policy for private user data.

### 8.2 PORTABILITY.patch

The independent patch adds `<memory>` and changes only temporary arithmetic inside `AnchorRoots` to allocator-backed binary512 `RootReal` with explicit `et_off`. Final roots are converted back to the original fixed binary512 `Real`; anchor indices, angles, bit precision, exponent type/range, Horner precision, and all gates remain unchanged. No warning is suppressed.

In Boost 1.83, non-void allocator selection changes the integer significand storage strategy while retaining the chosen bit precision; trigonometric argument reduction instantiates a promoted 1536-bit intermediate for this 512-bit input. The retained Linux trace fails in the fixed-storage conversion path. [B/multiprecision/cpp_bin_float.hpp:93–99,254–277; B/multiprecision/detail/functions/trig.hpp:128 and following promoted-reduction block; L:5454–5497.]

An isolated probe using the exact candidate `AnchorRoots` body compiled under local **GCC 14.2 / Boost 1.83** with `-O3 -Wall -Wextra -Wpedantic -Werror`; ten root pairs agreed with a 160-decimal scalar reference to a maximum component difference of approximately **2.84418834937723e-154**. The original fixed-storage probe produced retained array-bounds compiler errors in an earlier timed-out batch. The original command's numeric exit status was not retained. These are bounded compiler/scalar observations, not a full-target GCC-13.3 reproduction, a formal proof that the warning is a false positive, or hosted Windows validation. See the execution ledger for the interrupted attempt and successful isolated commands.

### 8.3 Application and complete files

Both patches are against exact b1b024e source and use `a/tests/...` / `b/tests/...` paths. They apply independently and in either order. The final patch-order check and byte comparisons passed for all four cases. Complete files are supplied in three alternatives:

- `complete/diagnostic/`: DIAGNOSTIC only;
- `complete/portability/`: PORTABILITY only;
- `complete/combined/`: both together.

These directories are **alternatives**, not three repository trees to merge. In a clean disposable/integration checkout at the verified baseline, an application sequence is:

```sh
git apply --check /path/to/DIAGNOSTIC.patch
git apply /path/to/DIAGNOSTIC.patch
git apply --check /path/to/PORTABILITY.patch
git apply /path/to/PORTABILITY.patch
```

Only patch applicability was exercised here; integration into the user's repository was not performed. No production-fix patch is supplied because no production defect was proved.

## 9. Compatibility limits and alternatives outside this patch

The observed fresh perturbation and frozen near-unit input make an **otherwise ideal** subsequent chain miss the original round-4 gate. This refutes a blanket interpretation that the profile plus ideal Mult2 arithmetic automatically guarantees this gate. It does not prove that every valid sample must fail, that actual later arithmetic cannot cancel error, or that a bounded empirical correctness target is mathematically impossible. No cancellation or favorable key may be selected to manufacture acceptance.

The paper's lack of detailed empirical input/randomness information prevents an honest claim of exact experimental noise-profile equivalence. A different regime would need explicit separate adjudication; none is mixed into these patches:

| Possible change | Consequence outside the current authorization |
|---|---|
| Make the ephemeral polynomial sparse or reduce encryption/error noise | Changes the encryption distribution and possibly security assumptions; h128 secret alone does not authorize this. A security/parameter assessment and changed contract would be needed |
| Increase initial scale or alter the d/Mult chain | Changes modulus consumption, nonwrap margins, and potentially security. With fixed d and mr, multiplying S0 by 2^b multiplies Sr by 2^(b*2^r), so S8 changes by 2^(256b); “add a few initial bits” is not a harmless metadata fix |
| Shrink the input domain | Can attenuate fresh-error amplification but may invalidate meaningful nonzero z^256 outputs and the frozen witness/domain |
| Accept arithmetic error A instead of end-to-end E | Answers a different scientific question and excludes initial encoding/encryption error; E must still be reported as failed under the current contract |
| Secret-key/noiseless encryption, favorable-key choice, retry until precision passes | Explicitly excluded; no such change is proposed |

A small local L is useful evidence about an operation, not an end-to-end pass. Likewise, observing ideal nonwrap headroom without original-input precision does not establish the current target.

## 10. Decisive next observation and stopping boundary

The bounded next experiment is the **existing one paper chain per hosted platform**, after Codex independently reviews/integrates the patches. The five API builds, unchanged 60 regressions, paper target build, and live 61-test listing remain part of that hosted validation. No second encrypted chain is needed to observe the later stages, since evaluation already produces them before the client checks.

Read the resulting first live stream as follows. If I accounts for the failed end-to-end vector while A is much smaller, the stronger inherited-dominance claim becomes measured. If A is comparable or L exhibits a large jump, the simple dominance hypothesis is challenged and the identified round guides a smaller operation-specific investigation. In either case, E remains the acceptance quantity. A valid diagnostic run can end in FAIL while yielding decisive scientific evidence. If an invariant/nonfinite/oracle-validity failure occurs, stop at that point; do not interpret unavailable later observations.

**Pending:** hosted Linux/Windows compilation of the complete changed files; all production regressions/API builds after patch; actual signed I/A/L observations; client round-5–8 and final full-slot/witness/binder/cleanup outcomes; any production repair justified by those observations. This review produced no new FHE run, no final numerical pass, no all-key theorem, no 1,000-run statistics, and no independent approval of its own candidate.
