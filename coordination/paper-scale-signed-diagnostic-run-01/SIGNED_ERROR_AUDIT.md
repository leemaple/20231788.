# Signed-error audit of the actual Linux and Windows diagnostic executions

2026-09-06. Independent reviewer selection: GPT-6 Astra, high; Codex fallback responsibility, not additional provider diversity. Earlier propagation/patch-spec audits were not rerun. Fable's reported definitive balance failure was not retried.

**Observed conclusion:** both executions fail the unchanged end-to-end contract. At every observed round/anchor, inherited fresh error I is substantially larger than added arithmetic error A. This establishes the attribution at the ten anchors; it does not establish an all-slot attribution or a new production defect.

## Evidence binding and executed checks

Root-supplied documentation HEAD: `a33d5f1fcfeb51cd63021b1ec1d24d6e76187a1c`. Both original BEGIN/COMPLETE markers independently identify tested source `9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e` and pristine OpenFHE pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`. Production is reported unchanged from `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89`; this audit verifies log mathematics, not Git provenance or the regression inventory, which are separate review responsibilities.

Run `33978202814`; Linux job `101338538686`, Windows job `101338538587`:

| Retained log | Bytes | SHA256 | Original first stream, inclusive lines |
| --- | ---: | --- | --- |
| LINUX_RAW.log | 906589 | `44eb5102695106bbe80a693ce4028b4160923f521d683c2d6af293bfc76040fa` | 7046–8024 |
| WINDOWS_LF.log | 915610 | `9ff1e0f25faafac5b57da07519b8f81b2a8b765f30ecec9e1c74d838395c1dea` | 7368–8346 |

The adjacent `verify_signed_error.py` validates the input hash, starts at the first `61: BEGIN`, and stops at its first `61: COMPLETE`. Subsequent CTest replay is excluded. Both final script executions exited 0, each validating 835 unique numeric fields, nine exact scale receipts, 20 fresh signed complex pairs (w0/E), and 320 round signed complex pairs (E/I/A/L at 8×10 anchors). Input expressions and powers are exact Fraction calculations; printed signed identities are recomputed with Decimal160. This uses only bounded scalar arithmetic on log data.

Reproduction, from the clean-room root:

```sh
/usr/bin/python3 artifacts/handoffs/paper-scale-signed-diagnostic-run-01/verify_signed_error.py
/usr/bin/python3 artifacts/handoffs/paper-scale-signed-diagnostic-run-01/verify_signed_error.py artifacts/handoffs/paper-scale-signed-diagnostic-run-01/WINDOWS_LF.log 9ff1e0f25faafac5b57da07519b8f81b2a8b765f30ecec9e1c74d838395c1dea
```

The script writes only stdout. JSON artifacts were saved separately with apply_patch. An initial successful scalar execution produced an overly long JSON response truncated by the command-output tool; output formatting was reduced to 50 decimal places, and the final scripts were rerun. Calculations still use Decimal160. Complete derived outputs are `LINUX_SIGNED_ERROR.json` and `WINDOWS_SIGNED_ERROR.json`.

Current test correspondence: `tests/paper_full_eight_square_oracle.h:78–90,153–179,283–313` and `tests/paper_full_eight_square_contract_test.cpp:270–301,317–338,366–392`. For each round the script independently verifies the reduced exact scale recurrence `S_r=S_(r-1)^2/(d*q[10-r])`, plus family/level/tower/terminal receipt metadata. This checks the emitted scale, not the absent ciphertext coefficients or a re-executed Horner normalization.

## Signed identities and print precision

For each anchor, the checker verifies fresh `w0=z+E0`, then reconstructs exact ideal `z_r=z^(2^r)` and checks:

```
I_r = w0^(2^r) - z_r
A_r = (z_r + E_r) - w0^(2^r)
L_r = (z_r + E_r) - (z_(r-1) + E_(r-1))^2
E_r = I_r + A_r
```

The tiny printed E0 carries more useful absolute accuracy than the near-unit printed w0, so power reconstruction uses exact z plus printed E0 after checking printed w0 agreement. Maximum w0 consistency discrepancy is `4.335e-102` (Linux), `4.294e-102` (Windows), within the 100-decimal output precision. Maximum subsequent identity discrepancy is `1.121e-124` and `1.113e-124`, respectively, below the fixed audit tolerance `1e-120`. Every E maximum, I/A/L aggregate, and final-versus-round8 anchor error also matches within the recorded output precision. These are scalar consistency checks, not an independent repeat of ciphertext decryption.

The emitted coefficient maximum divided by scale is nonnegative and never exceeds approximately `0.9647155084` in either execution. Thus the conventional Horner estimate `16*N^2*(B/S)*2^-512` is at most about `1.2362e-144`. Even allowing propagation through 256th powers leaves substantial margin below observed `1e-27` local residuals. This assumes ordinary arithmetic/root error behavior, not certified transcendental interval arithmetic. The previous arbitrary-full-Q conditioning concern is not supported by these observed coefficient magnitudes. Printed agreement at `1e-124` is limited by serialization, not evidence that internal binary512 precision is only 124 decimal places.

## Inherited versus added error: same anchor first

All comparisons below use max(real-component magnitude, imaginary-component magnitude). E=I+A is a **signed vector** identity: maxima from different anchors must not be subtracted as if they were one vector.

Across all 80 round/anchor observations, I/A is:

- Linux: at least `40.00094`, at most `714.2570`.
- Windows: at least `34.31801`, at most `3115.376`.

At round8 alone, the ranges are `41.0961–542.6158` and `39.2327–2280.7882`. Consequently added error is below 2.5% of inherited error at every sampled Linux point and below 2.92% at every sampled Windows point. This is actual signed-data evidence for inherited-error dominance, not merely a comparison of unrelated aggregate maxima.

| Round8 observation | Linux | Windows |
| --- | --- | --- |
| E anchor maximum | `2.8868984974e-24` at 0 | `1.9502151400e-24` at 257 |
| I at that same anchor | `2.8935248081e-24` | `1.9500042137e-24` |
| A at that same anchor | `6.6263107074e-27` | `2.3978020319e-27` |
| Same-anchor I/A | `436.6721` | `813.2465` |
| A maximum over ten anchors | `1.2496830180e-26` at 768 | `1.0458611893e-26` at 1 |
| I-maximum / A-maximum | `231.5407` | `186.4496` |

The last row is explicitly a ratio of maxima at different anchors. Small A is nonzero and does not certify each evaluator primitive. L1 includes any initial DCP departure; later L measures each complete Mult2's local departure. Neither separates Tensor2, Relin2, and RS2 internally.

Local residual maxima by round, in units of `1e-27`:

| Round | Linux | Windows |
| ---: | ---: | ---: |
| 1 | 0.699576 | 0.564145 |
| 2 | 1.071129 | 0.685735 |
| 3 | 1.021745 | 0.535020 |
| 4 | 1.189128 | 0.504202 |
| 5 | 0.840028 | 0.464402 |
| 6 | 1.250737 | 0.984419 |
| 7 | 2.419957 | 1.686313 |
| 8 | 2.619050 | 2.381938 |

Local maxima are not monotone early; they rise in the last two rounds but remain around `1e-27`. I and A aggregate maxima peak at round7 and decrease at round8, consistent with the subunit input powers' changing sensitivity. That consistency is not a theorem about later circuits or other keys. The two executions have separate random setup/encryption draws; differences between hosts cannot be attributed to platform arithmetic from these observations.

## Actual final acceptance and retained controls

| Observation | Linux | Windows |
| --- | --- | --- |
| Fresh full-slot E maximum | `3.8530898383e-25` | `3.4948573780e-25` |
| Final full-slot E maximum | `8.6166359880e-24` | `8.4890276422e-24` |
| Final full-slot E / 2^-80 | `10.4168737` | `10.2626047` |
| Final ten-anchor E / 2^-80 | `3.4900461` | `2.3576654` |
| Fresh codec disagreement | `4.3283507056e-165` | `4.3278994440e-165` |
| Final codec disagreement | `8.3504769944e-166` | `8.3530389814e-166` |
| Fresh oracle/production disagreement | `4.3349436876e-102` | `4.2935555568e-102` |
| Final oracle/production disagreement | `4.0494271141e-102` | `4.7616503864e-102` |
| Final observed real witness difference | `6.91166577443341608e-22` | `6.92503186287514950e-22` |
| Final witness difference error | `6.2456936406e-26` | `1.3990657806e-24` |
| Wrong nominal normalization error | `3.68743405013e-5` | `3.68743405013e-5` |

The exact ideal witness is approximately `6.91104120506935404e-22`. Both fresh and final witnesses satisfy their unchanged inequalities; the final permitted difference error is `2*2^-80≈1.65436e-24`. Both codec gates and independent/production agreement gates pass. The cleanup marker reports eight owned rows absent and two unrelated rows unchanged.

Both original streams finish with **seven numeric failures**: round4,5,6,7,8 independent-anchor gates, final full-slot gate, and final independent-anchor gate. The checker independently reconstructs this exact ordered failure list. The full-slot maxima are retained observations; per-slot full output and signed attribution were not logged, so they cannot be recomputed or attributed independently from this transcript. Reaching cleanup supports completion of preceding structural/domain checks in this code path; centered headroom alone is not a separate mathematical no-wrap proof.

## Inference and the next scientific adjudication

No new production defect follows from these logs. They show an actual failed frozen contract and decisively resolve the inherited-versus-added question **at the observed anchors**. Small A/L is compatible with the intended approximate algorithm; it neither proves that every arithmetic detail is correct nor transforms failed end-to-end E into a pass. Fresh E still combines encoding and encryption noise.

The next question is an independent technical adjudication of achievable end-to-end precision for the frozen OpenFHE public-encryption noise profile and frozen input domain, separately from correctness of the paper's multiplication operation. Keep the actual contract FAIL and the user's full implementation objective explicit. Paper empirical averages are not an every-round/all-key bound; no E-to-A acceptance substitution, changed parameter/distribution, or favorable-key retry is justified by this audit.

A small analytic budget calculation, included in the script, helps frame that question. With the input-domain radius upper bound R, first-order inherited component error is at most `sqrt(2)*k*R^(k-1)*E0`. For powers `k=2,4,...,256`, this conservative sensitivity peaks near `59.2285` at k=128. Ignoring added arithmetic and tiny nonlinear terms gives a conservative sufficient fresh budget near `1.3966e-26` (roughly 86 bits) for the existing 2^-80 target. This is not a necessary bound, new threshold, or proof that the profile is impossible. The technical adjudication needs an explicit noise/propagation budget, including encoding and ordinary public encryption, and an honest account of which paper experimental assumptions are specified or missing. It does not require another random key to answer the immediate question.

No C++, codec, transform, FHE, NTT, compiler, CI, network, browser, or Git operation was executed by this audit. Only the authorized audit script, two JSON results, and this report were written. This is not an implementation PASS or GREEN.
