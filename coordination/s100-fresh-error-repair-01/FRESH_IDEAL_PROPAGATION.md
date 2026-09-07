# Retained fresh input already exceeds E80 after ideal squaring

The GREEN2 fresh input, propagated through **eight error-free complex squarings**, exceeds `T = 2^-80` at three of the ten retained anchors. The largest retained-anchor component is slot 1023 real: approximately `+2.0832419873776521e-24`, or `2.5184850270 T`. This is a scalar implication of the observed fresh input, **not execution or acceptance of a new ciphertext chain**.

## Exact evidence and bounded scope

- Remote source: `a448b787399b43b6024d82c170add403969b493c`.
- [GREEN2 run 34110943783, attempt 1](https://github.com/leemaple/20231788./actions/runs/34110943783), Linux job `101706800879`.
- Input file: `coordination/s100-fresh-error-repair-01/remote-green2-diagnostic-steps.log`.
- Independently checked log SHA-256: `cbd1d44bb8bdfbf7f9ff74bc94e950ce8a7602cd5e8bb43d98da6f403b2a874d`.
- The log records exactly one fresh public encryption, controls COMPLETE and fresh diagnostic COMPLETE. It explicitly retains the previous original S100 FAIL and says the original chain was not rerun.
- Only the 20 `tuple=anchor` rows were numerical inputs. No full-slot replay, FFT, cryptographic operation, build, CI dispatch, or sampler experiment was performed on the Mac.
- Independent-review context identity: `requested-unverified`; no additional model-provider diversity is claimed. This subtask did not consult the author's verdict or interrupt Pro.

## Quantity computed

For each anchor `s`, pair its own real and imaginary entries:

`z_s = z_real + i z_imag`,

`e_s = E0_real + i E0_imag`,

`d_s = (z_s + e_s)^256 - z_s^256`.

Eight repeated squarings give exponent 256. Each squaring uses `(x + i y)^2 = (x^2 - y^2) + i(2xy)`. Thus phase rotation and mixing of the two error components are retained. No maximum from one component or slot is substituted into another, and no maxima are subtracted. The frozen dyadic formula for every `z_s` is independently reconstructed and checked for exact equality to the log.

`E0` is the diagnostic's signed production-decoded error, not the separately reported `encryption` maximum. This calculation therefore needs neither a hidden-sampler no-wrap claim nor a transcendental accuracy theorem about the independent observer.

## Directed arithmetic and rounding scope

The accompanying `check_fresh_ideal_propagation.py` uses only Python's standard-library Decimal, with 250 significant decimal digits. Every interval addition, subtraction, and multiplication rounds its lower endpoint downward and upper endpoint upward. Complex powers and their difference are enclosed by these operations; no linearization or floating-point trigonometry is used.

The printed `E0` at each component is additionally enlarged by **plus or minus `1e-100` before propagation**. This is a robustness enclosure for this offline calculation, not a change to any cryptographic acceptance threshold. The enclosure certifies the result for every exact complex input in that box. Its applicability to the observed production value assumes ordinary correct decimal formatting/conversion and binary512 arithmetic: at the retained inputs' magnitudes below 1, their binary512 rounding scale is below `1e-153`, and the 171-significant-digit printed E0 values lose far less than `1e-100`. The margin also exceeds the logged full-slot old-decimal100 bridge discrepancy, about `4.99984e-102`. It is not offered as a proof of arbitrary upstream numerical-library behavior.

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

Every lower bound in the second table is strictly greater than `T`. Consequently the full-slot maximum of **ideal propagation of this observed input** is at least the slot-1023 lower bound; no claim about its unretained maximum or location is needed. The other 17 retained components have interval upper bounds below `T`.

## Implication for the next scientific action

Merely eliminating Mult2's additional numerical error, while retaining this fresh input and the intended squaring map, cannot remove the inherited error: exact evaluation itself already misses the frozen E80 condition. At slot 1023 real, an actual evaluator result within `T` of `z^256` would need an opposing net discrepancy relative to ideal fresh-input propagation of **at least `1.25606137482e-24`**. This follows at the same component from `|r| >= |d| - T`, where `r = actual_output - (z + e)^256`.

Additional evaluator errors could accidentally cancel inherited error; a deliberate correction could also change this conclusion. Therefore this is not an impossibility theorem for every implementation, encryption draw, or parameter set, and it does not identify a production defect. It establishes that improving multiplication accuracy alone does not make this particular starting input meet the original criterion. A source-verified comparison of paper input distribution, fresh public-encryption contribution, and reported error metric is the justified next investigation; another full chain is unnecessary just to establish this inherited-error obstruction.

## Reproduction and retained outcome

Executed on the Mac with Python `3.9.6`, standard library only:

```sh
python3 coordination/s100-fresh-error-repair-01/check_fresh_ideal_propagation.py
```

Final checker source was rerun after adding outward-formatted interval summaries. Exit code was 0. It validated the exact input hash, source identifier, complete fresh marker, all ten unique anchor pairs, and frozen inputs; reported exceedances `(512, imag)`, `(769, real)`, `(1023, real)`; and reported maximum signed-interval width `1.46338432085945257529734e-98`.

The scalar checker and this report are new evidence only. Production source and test predicates were not edited. Original S100 full-chain status remains the previously retained FAIL; no new S100 full-chain test was executed or claimed.

Root independently read the entire checker, compared its frozen_input to Input(s) in the current C++ oracle, checked directed arithmetic/enclosure semantics, and executed `/Users/lifeng/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/bin/python3 -B -I coordination/s100-fresh-error-repair-01/check_fresh_ideal_propagation.py` with exit0 and the same three exceedances/intervals. Checker SHA-256: `cde34b86a3446fddbdbb6a90cf2ce2d66338d2e1ba202bed4040388a4c27ddfe`. This independent scalar replay is not another cryptographic sample.
