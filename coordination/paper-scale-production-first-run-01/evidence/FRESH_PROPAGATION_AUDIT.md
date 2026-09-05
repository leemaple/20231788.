# Fresh-error propagation: independent challenge material

2026-09-05. Reviewer selection: GPT-6 Astra, high, independent Codex context. Fable's reported unavailable balance was not retried. This is a mathematical diagnostic, not an authoritative production verdict or independent provider review.

Inspected HEAD: `1853701d8862dabef804021d2dea1899776f38e2`; executed Windows source: `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89`; pristine OpenFHE: `df495ba2e91739a6dc8f1de254fc5a41155ce504`. The review's executed comparison found no changes from the executed source to inspected HEAD in `src/{high_precision_client_io,paper_h128_client_keypair,double_ckks,repeated_mult2}.cpp` or the two paper test files. No Git operation was used while saving this audit.

## Observed evidence

Source log: adjacent `WINDOWS_LF.log`, Windows job `101321455226`, workflow run `33971779479`. Original paper BEGIN is line 7380. The original `61:`-prefixed observations contain fresh plus rounds 1–4, ten anchors each. CTest repeats the failed process output after its original COMPLETE marker; that replay is not another execution. The companion script starts at the first `61: BEGIN`, verifies source/pin, stops at its first `61: COMPLETE`, and asserts exactly the expected 50 distinct observations. It also reads one fresh full-slot maximum.

Fresh full-slot error: `3.380628068541526514072840073085484952608435963e-25`; fresh anchor maximum at slot512: `1.081632465294796426394472742886068841698281297e-25`. Fresh codec disagreement is approximately `4.327899444e-165`; fresh independent oracle versus production disagreement is approximately `4.954005369e-102` (log lines 7441–7456). Round4 maximum is `1.519830543686982243577737227091582941391260472e-24`, or `1.83736238570213248 * 2^-80`, and causes the recorded failure.

`tests/paper_full_eight_square_contract_test.cpp:177–189` executes all eight Mult2 calls and RCB before returning. Its client checking loop at lines 278–284 throws at round4. Thus rounds 5–8/final I/O have no recorded numerical checks; successful evaluator return is not numerical acceptance. The separate Linux compile failure is not evidence about this Windows numerical result.

## Method and derived bounds

The accompanying `fresh_propagation_audit.py` faithfully consolidates the review's two interactive standard-library Decimal calculations and is re-executed after saving. Reproduce with `/usr/bin/python3 artifacts/handoffs/paper-scale-production-first-run-01/fresh_propagation_audit.py` from the clean-room root. It performs only a few dozen scalar anchor/power calculations. Decimal precision is 80 digits; it is not interval arithmetic. Displayed endpoints are approximate evaluations of the following analytical bounds, with margins vastly larger than decimal rounding.

Observed saved-script execution: exit code 0, original log interval `7380..7508`, exactly 50 anchor observations; numerical results below reproduced. A subsequent output-only adjustment prints exact zero as `0`; the final script was also re-executed.

Let `w0=z+epsilon`, `E=max(abs(Re epsilon),abs(Im epsilon))`, and `c=k*z^(k-1)=a+i*b`. The induced component-infinity norm of multiplication by c is `abs(a)+abs(b)`; applying the inverse map proves:

```
(a*a+b*b)/(abs(a)+abs(b))*E <= ||c*epsilon||component_inf
                               <= (abs(a)+abs(b))*E.
```

All examined inputs satisfy `abs(z)+sqrt(2)*E<1`. Taylor's integral remainder therefore gives `R <= k*(k-1)/2*abs(epsilon)^2 <= k*(k-1)*E^2`. Subtract/add R to the lower/upper linear endpoints. These bounds do not require knowing the signed error direction.

For slot512 and k=16, the pure-fresh propagation interval is approximately `[1.49406650020095744e-24, 1.53821965758586599e-24]`, with `R<=2.80782909595127759e-48`. Even its lower endpoint exceeds the frozen gate `2^-80=8.27180612553027675e-25`; the actual round4 value lies inside. Thus the existing fresh perturbation alone is sufficient to explain the threshold crossing if subsequent powers are otherwise ideal.

For k=256, the corresponding counterfactual interval is approximately `[2.39531096370729711e-24,3.54885792737019448e-24]`, or `[2.8957532700,4.2903059785]` times the gate. This is **not an actual final result**. Real later arithmetic errors could reinforce or cancel the inherited perturbation.

Pure-fresh propagation alone is nevertheless not the whole story: at round1, slot257 exceeds its pure-propagation upper endpoint by approximately `5.940173786947497e-28`, and slot512 by `2.1340145182503386e-28`. The nonlinear remainder is negligible at that scale. By the reverse triangle inequality these are lower bounds on departure from pure-fresh propagation, not estimates of a particular noise component. Later signed residuals are unavailable. Ordinary discarded-low-square, key-switching, and rescale rounding can create such additions; these values do not prove an implementation defect.

## Source-based interpretation

- `src/high_precision_client_io.cpp:559–584` computes the two high-precision inverse encodings, applies stable nearest coefficient rounding, constructs a DCRT plaintext, and calls official **public-key** Encrypt. Nearest coefficient rounding alone gives the conservative slot component bound `N/(2*S)=2^-86=1.29246970711410574e-26`. Assuming the reviewed transform is correct, the observed fresh full-slot maximum exceeds this bound by `3.25138109783011594e-25`; ordinary encoding rounding cannot account for the entire fresh discrepancy.
- `src/paper_h128_client_keypair.cpp:193–210` creates one signed-h128 root secret and obtains public-key elements `(a*s+e,-a)` through official EncryptZeroCore. Pristine `src/pke/lib/schemerns/rns-pke.cpp:56–69,111–145,148–196` therefore gives `Dec(Enc(m))=m+e*v+e0+e1*s` at noiseScale 1.
- The same official source at lines 164–165 samples temporary v with the default ternary constructor. Official `src/core/include/lattice/hal/default/dcrtpoly.h:111` defaults its h parameter to zero; `dcrtpoly-impl.h:177–191` calls `GenerateIntVector`; `src/core/include/math/ternaryuniformgenerator-impl.h:103–108` makes h=0 dense independent uniform ternary. Root h128 does not make v sparse.
- Under an approximate independent-coefficient noise model with variance sigma^2, the dominant e*v term has component RMS `sigma*N/(sqrt(3)*S)`, approximately `4.760805339e-26` for sigma=3.19. This is a plausible scale explanation, not a measured attribution or rigorous discrete-Gaussian tail guarantee.
- `src/high_precision_client_io.cpp:664–716` uses the official Poly* decryption route and exact receipt scale. Official `src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp:70–95` adds flooding only in the NOISE_FLOODING branch; the frozen FIXED_NOISE profile does not take it. Fresh oracle/codec agreement strongly disfavors an explanation based on codec precision alone.
- The exact scale recurrence is retained in `src/repeated_mult2.cpp:255–283`; DCP recombination is algebraically exact modulo the prefix (`src/double_ckks.cpp:415–423`), Tensor2 uses the high/high and two cross products (854–859), and RS2 performs the two independently rounded public rescale calls (1143–1154). Gross nominal/actual normalization errors would produce much larger discrepancies than observed. Small implementation defects remain unexcluded.
- Round4 has at most 340-bit centered coefficients; the original binary512 Horner's prior arbitrary-fresh-modulus conditioning caveat is not a plausible cause for a 1e-24 round4 discrepancy. A conventional conservative `16*N^2*(B/S)*2^-512` estimate with B<Q/2 is already roughly below 2^-238 there. This is an ordinary-rounding estimate, not a formal transcendental-library certification.

## What the paper establishes, and what remains pending

Only the supplied `/Users/lifeng/.zcode/workspace/default/2023.1788.txt` was used for paper claims. Section 2.1, lines 223–237, specifies chi_enc/chi_err as parameters and the public-key noise structure. Section 6, line 1466, identifies the HEaaN implementation. Section 6.1, line 1503, describes canonical-embedding infinity norms and 1,000-execution averages. Section 6.3, lines 1562–1580, reports eight squarings and about -81.8-bit average precision for t=2. The text does not freeze the actual experiment input generator, concrete encryption/noise distributions, or explicitly state whether reported experimental error subtracts the fresh error. Do not infer a secret-key encryption experiment from that omission.

These paper results do not prove a per-execution, all-slot, every-round 2^-80 bound for the current public-key path and frozen inputs. The usual complex-magnitude infinity norm differs from this test's component maximum by at most sqrt(2); that convention cannot explain the present gap. The project gate remains its independently frozen acceptance criterion.

The most discriminating next authorized diagnostic would retain signed complex anchor observations for the **same** evaluated chain and calculate `I_r=w0^(2^r)-z^(2^r)` (inherited error), `A_r=w_r-w0^(2^r)` (accumulated added error), and `L_r=w_r-w_(r-1)^2` (local residual). Separately compare the encoded integer plaintext to z, and fresh decryption to that encoded plaintext, to distinguish rounding from encryption noise. Current maximum-absolute-component logs cannot reconstruct these signed quantities.

No specific faulty operation has been established. Fixing a demonstrated duplicate noise addition, wrong distribution call, prime, formula, or rounding defect is a legitimate implementation repair. Switching to secret-key encryption, changing v's distribution/noise parameters/scale, selecting keys or inputs after failure, or redefining the expected result as a power of decrypted fresh data changes the frozen boundary. None is proposed as an authorized fix here. Gates remain 2^-80 and 2^-120, with no replacement 1,000-trial requirement.

No transform, FHE, NTT, compile, new encrypted chain, or CI was executed in this review or artifact preparation. No source/test file was changed. No GREEN or final all-slot success is claimed.
