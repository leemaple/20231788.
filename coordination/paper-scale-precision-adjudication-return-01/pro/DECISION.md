# Independent adjudication: paper-method correspondence and attainable precision

**Decision ID:** PS-PRECISION-ADJUDICATION-01. **Review date:** 2026-09-06, Asia/Singapore. **Scope:** supplied source, paper, raw evidence and bounded scalar arithmetic; no encrypted execution in this review.

## 1. Decision

**Retain the original E80 FAIL. Do not change production. Freeze one endpoint full-slot residual test, `FS-RESIDUAL-ENDPOINT-01`, for independent review before the next uniquely authorized automatic Linux/Windows push.** This is a targeted coverage closure, not a request for additional random-key trials.

The supplied implementation has strong source correspondence to the paper's constructive DCP/Tensor2/Relin2/RS2/RCB operations. The signed observations support initial encryption/encoding error amplified by repeated squaring, rather than a demonstrated multiplication defect. At the observed anchors, even setting all accumulated added arithmetic error to zero would leave the original-input E80 gate failed. That is a materially stronger diagnosis than merely observing a large final error. It is not a proof that every slot, key or intermediate lift is correct.

The full-slot terminal E80 target is **unmet**, not waived: Linux reports 10.4168737241 times the limit; Windows reports 10.2626047000 times the limit. Existing logs do not expose full-slot inherited and added residuals. Therefore neither “complete precision reproduction PASS” nor “full-slot added error is small” is accepted today. A claims-separation draft is supplied, but no gate is changed or adopted by this review. [SL:7939–7958,8022–8024; SW:8261–8280,8344–8346; CALC:/hosts]

**Source-proven production defects found: zero.** This is an evidentiary conclusion, not a proof of absence. No production patch or speculative regression/fix is supplied.

## 2. Ranked, falsifiable findings

| Rank | Finding and consequence | What would falsify or narrow it |
|---|---|---|
| F1 / P1 acceptance blocker | The frozen original-input target fails on both new complete chains. Completion of later checks does not make the test green. | A different authenticated interpretation of the first live raw streams and unchanged test predicates; none was found. A later passing chain cannot erase these failures. |
| F2 / P1 claim boundary | At all 80 anchor/stage pairs per host, inherited error dominates accumulated added error in the same-anchor component norm; the full-slot terminal extension remains unproved. | Independent full-slot endpoint evaluation finding a substantial added residual, codec mismatch, or different error maximizer unexplained by the recorded inherited error. |
| F3 / P1 proof boundary | Constructive operation correspondence is supported, but the printed Theorem 4.8 is not an unconditional certificate for this run. Its strong nonwrap antecedent cannot hold at the last multiplication using the observed coefficient size. | A valid different interpretation of that antecedent or a separately established tighter lift/error theorem—not agreement at ten anchors alone. |
| F4 / P2 causal constraint | Sparse h128 secret sampling does not make the official public-encryption ephemeral polynomial sparse. The unchanged public-encryption noise and input conditioning make a fresh E80 check non-compositional through eight squarings. | A different actual encryption dispatch or sampler in the supplied source, or signed data inconsistent with the stated propagation identities. Neither was found. |
| F5 / P2 rejected remedy | No evidence supports changing final scale metadata, increasing codec precision, or suppressing the Boost warning as a production precision fix. | A source-level scale/polynomial inconsistency or an independently evaluated codec discrepancy large enough to explain the misses. Current controls point the other way. |

Ranks indicate decision impact, not discovered exploitable vulnerabilities. F1 is a numerical acceptance failure, not itself a production-code defect.

## 3. Evidence identity, chronology and actual outcomes

The input's outer identity, safe unique regular paths, CRC, exact 153-payload manifest closure and every payload size/hash were independently checked. TASK was read first. The current production modules, headers, paper tests, public seams and frozen contracts were read before the prior diagnosis narratives. `CHECK_RESULTS.json` comes from the included new scalar/log reader, not from executing the supplied path- and HEAD-pinned audit scripts. Source/capture provenance remains attributed to the packet, not independently re-created Git or CI state. See `EXECUTION_LEDGER.md` and `SOURCE_MAP.md`.

**Original run 33971779479, b1 source.** Linux completed the legacy regression checkpoint, then failed paper-test compilation in the Boost fixed-storage 512-to-1536 conversion warning promoted to an error. It had no paper runtime. Windows reached fresh and round-1–4 observations and stopped at round 4: its anchor maximum was approximately 1.51983054369e-24. Later client checks were not reached. The evaluator itself builds its eight-stage result before the client performs per-round observations; that distinction must not be mistaken for validated round-5–8 outputs in the original Windows run. [OL:5451,5472–5485; OW:5773,7380–7508; TEST:177–189,284–301]

**New run 33978202814, 9f source.** The original live stream is SL:7046–8024 and SW:7368–8346: 979 live payload lines and 835 unique finite numeric fields per host, nine exact rational scale receipts, two identical 60-row profile passes, one paper chain. The subsequent unprefixed text is a CTest replay, byte-identical after timestamp removal and exclusion of one specifically identified Windows CTest stderr line at SW:8549. It is not a second experiment. The independent reader also checks 123 supplied Start/argv/PASS bindings per host against raw lines; these belong to groups 1,2,57,1,2,60 and are **not 123 distinct regressions**. [CALC:/hosts; LA and WA `actual_pass_bindings`, checked against their raw locations]

Each new chain retained exactly seven misses: the round-4–8 original-input independent-anchor gates, final original-input full-slot gate, and final original-input independent-anchor gate. The cleanup marker precedes a count of seven and `COMPLETE ... FAIL`. Numeric continuation exposed later valid states but did not bypass the final failure; nonfinite, shape, oracle-agreement and invariant failures remain immediate. [TEST:366–392; ORACLE:129–151,181–195,315–333; SL:7551,7648,7745,7842,7939,7941,7957,8022–8024; SW:7873,7970,8067,8164,8261,8263,8279,8344–8346]

| Quantity, maximum absolute real/imaginary component | Linux | Windows |
|---|---:|---:|
| Fresh full-slot E | 3.85308983831e-25 | 3.49485737803e-25 |
| Final full-slot E | 8.61663598801e-24 | 8.48902764217e-24 |
| Final full-slot E / 2^-80 | 10.4168737241 | 10.2626047000 |
| Round-8 ten-anchor I maximum | 2.89352480809e-24 | 1.95000421375e-24 |
| Round-8 ten-anchor A maximum | 1.24968301802e-26 | 1.04586118932e-26 |
| Final anchor/production disagreement | 4.04942711407e-102 | 4.76165038639e-102 |

The limit is exactly 2^-80 = 8.2718061255302767487…e-25. Final full-slot component precision, expressed only as −log2(observed maximum), is approximately 76.619 and 76.641 bits. That is not the paper's empirical mean metric. Entries in different rows can have different maximizers; this table is not vector subtraction. [SL:7107,7925–7927,7940–7958; SW:7429,8247–8249,8262–8280; CALC:/hosts]

Final exact-scale binding/decryption, independent anchors, wrong-scale control, meaningful-output/witness checks, foreign rejection, immutability and owner cleanup were reached on both new hosts. The wrong-2^100 scale produces approximately 3.68743405e-5 error, orders of magnitude larger than the genuine E miss. These controls substantiate the implemented client route; they do not establish a full-slot independent transform oracle. [TEST:303–341,366–384; SL:7942–7959,8022; SW:8264–8281,8344]

## 4. Three different correctness/precision claims

Let z_s be the original exact dyadic input; let x_0,s be the independently decrypted fresh value, and x_r,s the independently decoded recombined value after r squarings at the exact scale S_r. Write p=2^r and use the real/imaginary component maximum norm throughout unless a complex modulus is explicit:

    E_r = x_r − z^p              original-input end-to-end error
    I_r = x_0^p − z^p            inherited fresh error after ideal propagation
    A_r = x_r − x_0^p            accumulated added arithmetic residual
    L_r = x_r − x_(r−1)^2        local residual on decrypted operands
    E_r = I_r + A_r.

**Formal Mult2 correctness** concerns approximate multiplication of the decrypted operands, with explicit low-part, relinearization, rescaling and nonwrap assumptions. It is not a mechanism that recovers the original plaintext from encryption noise. **Original-input end-to-end correctness** includes I and is the frozen engineering gate. **Section 6.3** reports an empirical average infinity-norm precision over 1,000 repetitions, with Table 3's t=2 value −81.8. That reported average neither promises this input family's per-stage componentwise maximum nor removes its inherited error. The paper does not provide enough information to fix the exact input/noise distribution, key reuse law or empirical subtraction procedure as E versus A. No such settings are inferred here, and reproducing a 1,000-run mean is not proposed. [PAPER:256–322,584–951,1562–1590; PDF pp.5,7–8,13; PC:90–149]

If a paper infinity norm uses the complex modulus, then max-component ≤ complex modulus ≤ sqrt(2)·max-component. Changing that convention cannot turn the present component error above 2^-80 into a complex-modulus pass. The genuine semantic difference is the subtraction reference and sampling/averaging procedure, not a convenient norm rename.

The exact identities also give

    A_r = (x_(r−1) + x_0^(2^(r−1))) A_(r−1) + L_r,
    E_r = (x_(r−1) + z^(2^(r−1))) E_(r−1) + L_r.

These are scalar algebra, with no probabilistic assumptions. They are diagnostic decompositions only: z remains the truth reference for E. [ORACLE:153–179; TEST:289–301]

## 5. What the signed data actually prove

The new reader reconstructs exact dyadic z and exact rational anchor powers, and reconstructs the fresh operand from **z plus signed E0**. This avoids needlessly losing about 25 decimal digits by subtracting near-unit printed w0 values. It separately checks w0 consistency, reconstructs I and A, reconstructs L from consecutive actual values, and checks E−I−A and aggregate maxima. The largest signed identity discrepancy is 1.12085804e-124 on Linux and 1.11297614e-124 on Windows, consistent with 100-digit serialization; w0 consistency is about 4.3e-102. These are scalar rechecks of recorded numbers, not a new decrypt/CI execution. [ORACLE:145–179; CALC:/hosts/linux/signed_identity_max_discrepancies; CALC:/hosts/windows/signed_identity_max_discrepancies]

At Linux round 8, anchor 0, imaginary component:

    E = −2.8868984973815393612e-24
    I = −2.8935248080889027254e-24
    A = +6.6263107073633642054e-27.

At Windows round 8, anchor 257, real component:

    E = −1.9502151399694031505e-24
    I = −1.9500042137485291742e-24
    A = −2.1092622087397634271e-28.

Thus A partly cancels inherited error in the first example and reinforces it in the second. Both inherited components alone exceed E80. Eliminating A would not repair the original-input target for these retained fresh ciphertexts. This conclusion does not subtract maxima from different slots. [SL:7846–7850; SW:8191–8195]

Across all 80 anchor/stage pairs per host, the minimum ratio `||I_r,s||component / ||A_r,s||component` is 40.00094 and 34.31801, respectively. This is a same-anchor norm statement, not an assertion about every real/imaginary component ratio. It supports inherited-error dominance on those observations. However, ten anchors cannot bound A over 16,384 slots. In particular, the larger final full-slot E maxima have not been independently decomposed at their maximizers. [CALC:/hosts/linux/same_anchor_min_I_over_A; CALC:/hosts/windows/same_anchor_min_I_over_A]

## 6. Source correspondence, exact scale and limits of the theorem

The reviewed source implements the intended constructive sequence, with no identified causal discrepancy:

- **Client and key:** h128 is sampled explicitly for the root secret. The public key uses official secret-key EncryptZeroCore to generate the ordinary `(a·s+e_pk, −a)` public-key pair; message encryption then uses official **public-key** encryption of the high-precision encoded polynomial. This is not secret-key message encryption. Same-secret family projection matches modulus and root identity. [KEY:193–217; IO:541–596; REP:320–367,412–432]
- **DCP and Tensor2:** DCP constructs the quotient and exact remainder from the d tower. Tensor2 computes high×high and both cross products, intentionally omitting low×low; that omission is in the paper, not a newly discovered bug. The exact tensor scale divides by d. [DCK:398–438,826–891; PAPER:343–477,584–660]
- **Relin2:** the high tensor is multiplied by d and lifted before relinearization; DCP of that relinearized result supplies its remainder, which is added to the relinearized cross result. It is not the erroneous operation “relinearize high, then multiply its error by d.” [DCK:1011–1077; PAPER:664–743]
- **RS2, RCB and wrappers:** high and recombined values are rescaled using the actual consumed m; low is reconstructed as `RS(RCB) − d·RS(high)`. RCB is direct d·high+low. Reentry changes the context wrapper only after basis identity checks; it does not refresh or DCP again. [DCK:1080–1243; REP:472–531; LEVELED:172–190; CONTEXT:2073–2079]
- **Terminal client:** an owned receipt supplies the exact rational S8. Official Poly* integer decryption is used, followed by integer centering and independently generated 160-/220-decimal transforms; stock packed Decode is not the final oracle. [IO:645–716; DEC:70–95]

Nominal 50-bit scaling metadata, recorded 2^100 metadata and the true rational scale are distinct. The nine raw receipts independently match

    S0 = 2^100;  S_r = S_(r−1)^2 / (d·m_r).

For these exact primes, S8/S0 = 1.00036485038960392285…. A metadata-only division by 2^100 instead of S8 is measurably wrong. The source's internal recorded-scale normalization is not an extra polynomial division. [NOM:1–30; DCK:861–886; ORACLE:78–90; CALC:/hosts/*/receipt_checks; SL:7959; SW:8281]

### Conditional arithmetic bound, with units

Let H and C bound the high/low **decrypted integer-polynomial coefficient** norms at a stage. Let eta_H and eta_L be realized integer-polynomial decryption errors of the two actual relinearization routes, including their actual HYBRID effects. On consistent, nonwrapping integer lifts, the constructive identities give the local coefficient residual bound

    beta_r <= N·C²/(d·m_r)
              + (||eta_H||coef + ||eta_L||coef)/m_r + (h+1)/2.

The final term assumes coefficientwise nearest rescaling of two ciphertext components and ||s||1=h. To get a conservative decoded complex-slot bound multiply beta_r by N/S_r. Coefficient units must not be confused with canonical-slot units. The paper's Theorem 4.8 has the corresponding conditional low/relinearization/rescale terms; Theorem 4.9 separately controls canonical low-part growth. The actual supplied HYBRID path uses digit decomposition and ApproxModDown, so importing a one-polynomial `E_Relin` constant without bounding that path is not a proof. [PAPER:903–1076; HYBRID:51–129,308–434]

Two important qualifications prevent overclaiming:

**Printed normalization inconsistency.** The archived Theorem 4.8 display compares to a product divided by m alone, while Lemma 4.2 and the following modulus-consumption paragraph imply division by d·m. Its stated low-part error term also uses d·m. This review follows the constructive definitions and explicit scale discussion, not the inconsistent isolated display. The production recurrence agrees with that constructive interpretation; no source change to remove d is justified. [PAPER:614–660,903–951; PDF pp.7–8, inspected visually]

**The theorem's sufficient antecedent is too strong to certify this run.** It requires `N(dH+C)^2 + E_Relin + h < Q_active/2`. Before the last multiplication the observed recombined coefficient maximum is about `0.25768408958·S7`. Since centering cannot increase absolute value, `dH+C` must be at least that maximum. Taking E_Relin=0 already yields

    2N·(0.25768408958·S7)^2 / Q_active ≈ 4.78644166417e15,
    Q_active = 1125899904679937 · 1125899903827969 · 1152921504598720513.

Consequently that printed sufficient condition cannot be asserted. **Failure of a sufficient condition is not failure of the algorithm.** A tighter argument can use the divided product `(P²−C²)/d`, canonical norms and actual HYBRID errors, rather than the much larger undivided-product bound; those actual low-part/relinearization bounds are not in these logs. Observing centered coefficients inside Q/2 is itself tautological and is not a nonwrap proof. The requested finite implementation evidence need not be inflated into an all-key theorem, but it must be described honestly. [PAPER:907,935–945; ORACLE:264–280; SL:7747; SW:8069; exact S7 in CALC:/hosts/*/receipt_checks/7]

## 7. Public-encryption and input-conditioning constraint

### 7.1 The error that is actually encrypted

The official public-encryption route selects `DCRTPoly(tug, ..., EVALUATION)` without a Hamming-weight argument. Its default is zero, which selects dense uniform ternary coefficients. The root secret's explicit h=128 is a different constructor use. With noise scale one,

    pk = (a·s + e_pk, −a),
    c0 = pk0·v + e0 + m,   c1 = pk1·v + e1,
    c0 + c1·s = m + e_pk·v + e0 + e1·s.

This proves the source-level noise expression, not its realized distribution in every key. [PKE:111–145,148–196; DCRTH:111; DCRTI:178–192; TUG:50–64,103–108; KEY:199–210; REP:155–204]

For a realized v with w=||v||1≤N and integer coefficient bounds Bpk,B0,B1, a deterministic, loose bound is

    ||e_pk·v + e0 + e1·s||coef <= w Bpk + B0 + h B1;
    decoded slot modulus <= N(w Bpk+B0+h B1)/S0.

No missing realized Gaussian maxima are invented to turn this into a useful absolute all-key bound.

For a **moment-scale heuristic**, assume independent centered Gaussian-error coefficients of variance sigma_e², independent uniform ternary v, and a fixed h128 secret; approximate sigma_e by the configured 3.19. Conditional on v, each noise coefficient has variance `sigma_e²(w+1+h)`, with E[w]=2N/3. For a canonical root zeta, conditional on v and s,

    E|noise(zeta)|² = N sigma_e² (|v(zeta)|² + 1 + |s(zeta)|²).

Averaging over v and slots, Parseval gives mean |s(zeta)|²=h. Each real/imaginary component has half the complex second moment under these assumptions. At N=32768 and S0=2^100 this gives approximately **472.88 integer coefficient RMS**, **4.77484e-26 decoded component RMS**, or **6.75265e-26 complex RMS**. These are moment scales, not measured full-slot maxima, calibrated failure probabilities, exact discrete-Gaussian variances or independent-slot statistics. A fixed public key and the product e_pk·v induce correlations and nontrivial tails. [Source expression above; CALC:/analytic_scalars]

By comparison, ordinary nearest-integer encoding of N coefficients contributes at most `N/(2S0)=2^-86≈1.29247e-26` in complex slot modulus, conditional on a correct encoder. The observed fresh full-slot errors of about 3.5–3.85e-25 are too large to be explained by this rounding bound alone. This does not independently rule out every possible encoder defect, but it makes “add more transform digits” an unsupported causal fix given the independent anchor controls. [IO:412–448,541–596; SL:7107–7124; SW:7429–7446]

### 7.2 Propagation through the frozen input

The exact generator gives conservative domain bounds

    0.9909825368403 <= |z| <= 0.9912417251992,
    0.09837762981 <= |z^256| <= 0.10518920463.

These are meaningful nonzero outputs. For delta=x0−z and p=2^r,

    |I_r| <= p (|z|+|delta|)^(p−1) |delta|.

The complex derivative magnitudes p|z|^(p−1) are bounded as follows:

| Round | Derivative magnitude interval |
|---:|---:|
| 1 | 1.98197–1.98248 |
| 2 | 3.89276–3.89582 |
| 3 | 7.50848–7.52224 |
| 4 | 13.96722–14.02212 |
| 5 | 24.16551–24.36222 |
| 6 | 36.16913–36.76996 |
| 7 | 40.51279–41.88090 |
| 8 | 25.41384–27.16637 |

The peak at round 7 and decline at round 8 follow from the subunit inputs; a nonmonotone E sequence is not by itself anomalous. [ORACLE:97–107; CALC:/analytic_scalars/derivative_bounds]

For a componentwise bound, write the derivative D=u+iv. Its real 2×2 induced infinity norm is kappa=|u|+|v|, at most sqrt(2)|D|. A matching orientation-independent lower linear bound is `|D|²/(|u|+|v|)·||delta||component`. The nonlinear remainder in complex modulus is at most

    p(p−1)/2 · (|z|+sqrt(2)e0)^(p−2) · (sqrt(2)e0)^2,
    e0=||delta||component.

These bounds explicitly account for phase and units; an amplification factor must not be applied to an unrelated maximum without qualification. For an actual guarantee, add the propagated A/L budget and the remainder. Ignoring both only to illustrate the necessary engineering margin, the conservative **first-order sufficient** fresh component budget is about **1.39659e-26** for all eight stages, or **2.15305e-26** for the terminal stage alone. These are derived design diagnostics, not new acceptance thresholds and not necessary lower limits on attainable noise. The exact sufficient inequality uses `(rho+sqrt(2)e0)^(p−1)` instead of rho^(p−1); its correction here is negligible at the displayed digits. [CALC:/analytic_scalars]

This explains why “fresh error ≤2^-80” does not compose into “all subsequent error ≤2^-80.” Under the stated moment heuristic, derivative gains of roughly 25–42 already move a 4.77e-26 component noise scale into the 1e-24 range before taking maxima. The recorded matching-anchor I confirms the mechanism without relying on that heuristic. No claim is made that every permitted key must fail, or that a Gaussian impossibility theorem answers the engineering question. The defensible conclusion is narrower and sufficient: the current original-input guarantee is not supported, and its actual failures would remain at observed anchors even with A removed.

## 8. Is oracle conditioning an alternative explanation?

The independent oracle uses sparse-secret negacyclic arithmetic, cpp_int CRT and positive-root direct Horner. Official inverse NTT is shared explicitly; production special transforms and production Decrypt are not used to generate the independent polynomial. The allocator-backed binary512 type is confined to AnchorRoots and returns the original binary512 values. Boost's representation changes with allocator presence; that avoids the original fixed-storage conversion path without lowering precision or disabling warnings. [ORACLE:227–313; BOOST:93–99,254–277; OL:5472–5485]

The actual maximum coefficient/S over all reported states is approximately 0.964716, not the worst-case magnitude of an arbitrary centered element modulo the full 620-bit Q. With B/S at that observed level, a deliberately conservative ordinary Horner roundoff estimate `16 N²(B/S)·2^-512` is about **1.23612e-144**, assuming ordinary O(2^-512) root error, normal rounding and no overflow. Even a few hundredfold scalar propagation leaves an enormous margin below 2^-80. This is a conditioning estimate, not an interval proof of Boost transcendental accuracy. [ORACLE:264–272,301–313; CALC:/hosts/*/ordinary_Horner_rounding_estimate_16N2B_u]

The source-independent Horner/production agreements around 1e-102 and the 160-/220-decimal disagreements around 1e-165–1e-166 further weaken ordinary precision-loss explanations at the anchors. They do not exclude a full-slot ordering/transform bug outside the anchors, a shared inverse-NTT defect, or every possible correlated numerical defect. The first of those is the concrete, relevant uncovered risk addressed next; this review does not expand into a new NTT implementation or numerical-library certification project.

## 9. One next action and its stopping rule

Freeze **FS-RESIDUAL-ENDPOINT-01** as specified in `NEXT_TEST_SPEC.md`: extend only the client test to independently evaluate **fresh and terminal** integer polynomials at every slot on the same single chain, with a separately implemented ordinary twisted DFT, binary512/768 cross-check, exact slot-index map and the existing direct-Horner anchors. Compute all-slot E0,E8,I8,A8 against the unchanged original z; export enough signed per-slot scalar evidence to recompute the decomposition without production Decode or a new encrypted run.

The old chain's full-slot I/A cannot be recovered from its logs: the required all-slot fresh values or independently decrypted polynomial are not present. Do not claim the future test reproduces that original key/capture. On the next authorized push, each host still performs just the existing one encryption and one chain; the additional work is an independent observation of that chain, not another cryptographic trial. No run is authorized or dispatched by this package.

A small independently checked full-slot A8 would close the terminal coverage gap and support a precisely named added-arithmetic claim. E80 would still be evaluated against z and its historical failures retained. A substantial A8, production/independent discrepancy, or unreliable oracle would instead identify a concrete slot and risk for a minimal source-supported RED regression before any production implementation. There is no automatic “rerun again” branch. Endpoint closure does not claim full-slot intermediate bounds or instantiate the paper's universal nonwrap theorem.

## 10. Rejected alternatives and contract consequences

**Relabel E as A or fit a larger threshold:** rejected. A useful arithmetic metric is a different claim, not a repaired E80 test. The attached draft retains both names and the old failures; its proposed numerical added-error budget reuses the pre-existing 80-bit engineering budget, not the observed A maximum or a fitted pass rate.

**More keys, favorable keys or 1,000 repetitions:** rejected. They do not address the missing same-chain full-slot decomposition. Section 6.3's sampling exercise is not the user's present correctness obligation.

**Sparse ephemeral v, reduced sigma, secret-key message encryption:** explicit alternative cryptosystem/profile changes only. They alter the public-encryption noise/security contract and require a separate justified design and a minimal RED regression. h128 does not authorize them.

**Smaller inputs, larger primes, different d or chain, refresh/bootstrap:** explicit scope changes, not fixes to current compliance. Input attenuation changes conditioning and the meaningful-output/witness obligation. Section 6.2 refresh is a different experiment and does not recover unknown original-message error for free. Security/nonwrap and scale budgets would need to be revisited.

**Increase S0 by a few bits:** especially unsafe as a local suggestion. With d·m_r fixed, changing S0 by 2^b changes S_r by 2^(2^r b), hence S8 by **2^(256b)**. The current ideal terminal coefficient sufficient headroom calculation, `2 S8 max|z^256|/Qbase≈0.210455`, loses that simple bound at only b≈0.00878287; one extra initial bit multiplies terminal scale by 2^256. This headroom calculation is an ideal sufficient bound, not proof of an actual wrap threshold, but it decisively invalidates treating initial-scale bits as an isolated noise improvement. [ORACLE:78–90; CALC:/analytic_scalars]

**Claim security from h128, cross-platform agreement, or HEStd_NotSet:** rejected. None is a security estimate or all-key correctness guarantee. No security level is awarded, no sampler is weakened, and no new security-certification experiment is imposed here.

**Bottom line:** source correspondence and measured added-arithmetic accuracy can coexist with an unmet original-input precision target. Preserve both facts. The useful bounded next step is independent full-slot terminal residual closure—not a production patch, hidden gate change or new trial campaign.
