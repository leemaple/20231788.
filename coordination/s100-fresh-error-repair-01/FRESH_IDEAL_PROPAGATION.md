# Retained fresh phase error A+B under ideal squaring

The corrected GREEN2 calculation propagates the signed phase perturbation **A+B**, excluding readout error C. Under **eight error-free complex squarings**, it exceeds `T = 2^-80` at three of the ten preselected anchors. The largest retained-anchor component is slot 1023 real: approximately `+2.0832419873776521e-24`, or `2.5184850270 T`. The specifically requested slot-0 witness remains below T: its maximum component is about `0.3274255324 T`. This is a conditional scalar implication of the observed phase, **not execution or acceptance of a new ciphertext chain**.

The correction follows Pro's `NEXT_STEP.md` §4 and `CLAIM_BOUNDARY.md` §2 in `coordination/s100-independent-semantic-review-01/pro/`. The earlier version propagated E0. Its numerical conclusion is unchanged at the displayed precision, but its interpretation as ciphertext phase propagation required this correction.

## Exact evidence and bounded scope

- Remote source: `a448b787399b43b6024d82c170add403969b493c`.
- [GREEN2 run 34110943783, attempt 1](https://github.com/leemaple/20231788./actions/runs/34110943783), Linux job `101706800879`.
- Input file: `coordination/s100-fresh-error-repair-01/remote-green2-diagnostic-steps.log`.
- Independently checked log SHA-256: `cbd1d44bb8bdfbf7f9ff74bc94e950ce8a7602cd5e8bb43d98da6f403b2a874d`.
- The log records exactly one fresh public encryption, controls COMPLETE and fresh diagnostic COMPLETE. It explicitly retains the previous original S100 FAIL and says the original chain was not rerun.
- Only the 20 `tuple=anchor` rows were numerical inputs. No full-slot replay, FFT, cryptographic operation, build, CI dispatch, or sampler experiment was performed on the Mac.
- Independent-review context identity: `requested-unverified`; no additional model-provider diversity is claimed. The initial investigation preceded the author's verdict. This correction consumes the terminal Pro review; no active review was interrupted.

## Quantity computed

For each anchor `s`, pair its own real and imaginary entries:

`z_s = z_real + i z_imag`,

`e_s = (A_real+B_real) + i(A_imag+B_imag)`,

`d_s = (z_s + e_s)^256 - z_s^256`.

Eight repeated squarings give exponent 256. Each squaring uses `(x + i y)^2 = (x^2 - y^2) + i(2xy)`. Thus phase rotation and mixing of the two error components are retained. No maximum from one component or slot is substituted into another, and no maxima are subtracted. The frozen dyadic formula for every `z_s` is independently reconstructed and checked for exact equality to the log.

Here A is `encoding`, B is `encryption`, and C is `decoding` in each retained tuple. A+B approximates `O(p)-z`; E0 approximates A+B+C and is the production-readout error. C belongs to the readout and is excluded from the ciphertext's starting perturbation. E0 and C are retained only for same-component consistency checks. Those checks find maximum retained `|C| ≈ 9.21787115705e-129`, `|A+B−E0| ≈ 9.21787115705e-129`, and `|A+B+C−E0| ≤ 3.20437849810e-154` (the last decimal is an upward coarse bound).

The interval calculation encloses powers of printed A+B values. Applying the result to exact canonical phase values remains conditional on the independent observer's accuracy; multi-precision/Horner agreement is not a transcendental interval proof. This does not require or establish absence of hidden multiples of Q in an unobserved sampler lift.

## Directed arithmetic and rounding scope

The accompanying `check_fresh_ideal_propagation.py` uses only Python's standard-library Decimal, with 250 significant decimal digits. Every interval addition, subtraction, and multiplication rounds its lower endpoint downward and upper endpoint upward. Complex powers and their difference are enclosed by these operations; no linearization or floating-point trigonometry is used.

Each component of A+B is formed by directed interval addition at 250 digits, then enlarged by **plus or minus `1e-100` before propagation**. No A+B addition occurs in Decimal's default 28-digit context. The radius is a robustness enclosure for this offline calculation, not a change to any cryptographic acceptance threshold. The enclosure covers every exact complex input in that box. Its applicability to observed phase values assumes ordinary correct decimal formatting/conversion and sufficiently accurate binary512 observation; it is not a proof of arbitrary upstream numerical-library behavior. The retained readout contribution and tuple consistency residual are both verified below the radius, so replacing historical E0 with A+B is immaterial at the reported scale. The radius itself does not prove the unobserved exact phase lies inside the box.

The maximum width of any propagated signed-component interval was below `1.463385e-98`, negligible compared with the smallest reported excess over `T`. The short display values below are rounded summaries; the three explicit absolute intervals in the next table are coarser bounds rounded **outward**, and remain enclosing bounds.

## Results

`T = 8.2718061255302767487140869206996285356581211090087890625e-25` exactly.

| Anchor | Signed real error after ideal squaring | Signed imaginary error after ideal squaring | Largest component / T |
| ---: | ---: | ---: | ---: |
| 0 | `+8.8269878841e-27` | `+2.7084005245e-25` | `0.32742553` |
| 1 | `-6.9441679252e-25` | `-7.0552604625e-25` | `0.85292865` |
| 256 | `-6.5688703517e-27` | `-2.3633256430e-25` | `0.28570854` |
| 257 | `-1.7108610071e-25` | `-5.8445648250e-26` | `0.20683040` |
| 512 | `+3.3703142405e-25` | `+1.0990385494e-24` | `1.32865608` |
| 513 | `-4.2931017710e-25` | `+3.9135517142e-26` | `0.51900416` |
| 768 | `-2.3326331948e-25` | `-1.8060143613e-25` | `0.28199805` |
| 769 | `-1.2370525619e-24` | `+3.0007431478e-25` | `1.49550478` |
| 1023 | `+2.0832419874e-24` | `-7.2738450018e-25` | `2.51848503` |
| 16383 | `+5.7381221436e-25` | `-5.8561076728e-25` | `0.70795998` |

| Anchor / component | Enclosing interval for absolute error |
| --- | --- |
| 512 imaginary | `[1.09903854939e-24, 1.09903854940e-24]` |
| 769 real | `[1.23705256187e-24, 1.23705256188e-24]` |
| 1023 real | `[2.08324198737e-24, 2.08324198738e-24]` |

Every lower bound in the second table is strictly greater than `T`. Consequently, conditional on the starting phase lying in the analyzed box, the full-slot maximum of **ideal propagation of this observed phase** is at least the slot-1023 lower bound; no claim about its unretained maximum or location is needed. The other 17 retained components have interval upper bounds below `T`. Slot 0 is reported as the prespecified point witness, not replaced with a selected failing point; the ten-anchor set was already frozen before this correction.

## Implication for the next scientific action

Within the stated phase-observation enclosure, merely eliminating Mult2's additional numerical error while retaining this phase and the intended squaring map cannot remove the inherited error: exact evaluation itself already misses the frozen E80 condition. At slot 1023 real, an actual evaluator result within `T` of `z^256` would need an opposing net discrepancy relative to ideal phase propagation of **at least `1.25606137482e-24`**. This follows at the same component from `|r| >= |d| - T`, where `r = actual_output - (z + e)^256`.

Additional evaluator errors could cancel inherited error; a deliberate correction could also change this conclusion. Therefore this is not an impossibility theorem for every implementation, encryption draw, or parameter set, and it does not identify a production defect. The endpoint-pressure conclusion is unchanged by the A+B correction. This new fresh sample is not the historical S100 E8 ciphertext and cannot be paired with that E8 for causal attribution. A source-verified comparison of paper input distribution, fresh public-encryption conditions, and reported error metric remains the justified next investigation; this calculation itself executes no chain.

## Reproduction and retained outcome

Executed on the Mac with Python `3.9.6`, standard library only:

```sh
python3 coordination/s100-fresh-error-repair-01/check_fresh_ideal_propagation.py
```

The corrected checker exited 0. It validated the exact input hash, source identifier, complete fresh marker, all ten unique anchor pairs, finite A/B/C/E0 inputs, frozen inputs, and consistency within the robustness radius. It reported the same three exceedances and maximum signed-interval width `1.46338432085945257529734e-98`. A preceding bounded reader-contract RED exited 1 with `phase propagation lacks separate A/B/C inputs: current reader retains only z,E0`; it performed no powers or cryptography.

The scalar checker and this report are new evidence only. Production source and test predicates were not edited. Original S100 full-chain status remains the previously retained FAIL; no new S100 full-chain test was executed or claimed.

Historical E0 analysis: root independently reviewed and executed the earlier checker with SHA-256 `cde34b86a3446fddbdbb6a90cf2ce2d66338d2e1ba202bed4040388a4c27ddfe`. That receipt belongs to E0 propagation and is not relabeled A+B. The corrected checker SHA-256 is `9fbf1e6747c621400a6992ecdba0fee37724aebf5719466010b5fa92e23d734f`; its new execution used `python3 -B -I coordination/s100-fresh-error-repair-01/check_fresh_ideal_propagation.py`. No independent root rerun of this correction is claimed here.

Exact retained stdout summary from that correction (all 20 per-component rows reproduce the table above):

```text
source=a448b787399b43b6024d82c170add403969b493c log_sha256=cbd1d44bb8bdfbf7f9ff74bc94e950ce8a7602cd5e8bb43d98da6f403b2a874d
checker_sha256=9fbf1e6747c621400a6992ecdba0fee37724aebf5719466010b5fa92e23d734f
perturbation=A+B excluded_readout=C historical_E0=CONSISTENCY_ONLY
anchors=10 components=20 squarings=8 decimal_precision=250
input_component_radius=1E-100 threshold=8.2718061255302767487140869206996285356581211090087890625E-25
prespecified_slot0_witness component=real status=BELOW absolute_upper_bound=8.82698788412665017301662E-27 ratio_upper=1.06711735625462119385028E-2
prespecified_slot0_witness component=imag status=BELOW absolute_upper_bound=2.70840052450442799718872E-25 ratio_upper=3.27425532393120716477784E-1
exceeding_components=[(512, 'imag'), (769, 'real'), (1023, 'real')]
largest_anchor_lower_bound_slot=1023 component=real
largest_anchor_absolute_lower_bound=2.08324198737765211086267E-24
maximum_signed_interval_width=1.46338432085945257529734E-98
maximum_anchor_readout_abs=9.21787115704854228446977E-129
maximum_anchor_E0_minus_phase_abs_upper=9.21787115704854228446977E-129
maximum_anchor_ABC_minus_E0_abs_upper=3.20437849809351507366057E-154
largest_anchor_required_opposing_error_lower_bound=1.25606137482E-24
```
