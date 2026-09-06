# Live full-slot endpoint evidence accepted; original precision FAIL

Observed 2026-09-06 Asia/Shanghai. Exact source `ed5fd192a89d6d4728ad295e87cf06a3f4abc832`, run [34039088536](https://github.com/leemaple/20231788./actions/runs/34039088536), attempt 1, ref `codex/endpoint-live-capture-20260906`. Production src/include remain byte-identical to `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89`; official pristine OpenFHE pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

## Actual execution

Both jobs are terminal FAILURE, deliberately preserving the actual CTest exit 8. Each ran one real paper chain: N32768, 16384 slots, fresh public encryption, one DCP and eight Mult2 squares, unchanged input/key/noise/scales and original acceptance predicates. There was no recovery rerun or extra chain. The unprefixed CTest failure text is a replay, not a second experiment.

| Host / job | Paper target build UTC | Actual CTest61 duration | Wrapper/finalizer UTC | Numerical misses |
| --- | --- | --- | --- | ---: |
| Linux / 101502439156 | 14:31:29–14:32:32, success | 85.24 s | 14:32:32–14:34:02 | 9 |
| Windows / 101502439304 | 14:35:37–14:37:14, success | 118.06 s | 14:37:14–14:39:20 | 7 |

Each host passed earlier groups [1,2,57,1,2,60]: 123 test invocations, not 123 unique tests. The five API targets and unchanged full paper target compiled. The actual no-argument path reached owner cleanup, original numerical count, Write/Emit/flush, 39 endpoint records and unchanged failing Require. Each independent Python finalizer completed full Decimal256 replay/reconciliation and exact publication; its COMPLETE status can only be produced after these calls in the tested source. The run does not retain a separate per-replay timing/trace, so no such measurement is claimed.

Both always-run selectors and exact two-file uploads succeeded after the failed wrapper. Status is `evidence_state=COMPLETE`, `observer_disposition=PASS`, `packer_disposition=PASS`, `E80_disposition=FAIL`, `A_disposition=NOT_ADOPTED`, `ctest_exit_code=8`. Diagnostic completion is not numerical completion.

Linux failed the round3–8 anchor gates, terminal full-slot and terminal anchor gates, and the final retained-difference accuracy gate. Windows failed round4–8 anchors plus terminal full-slot and terminal anchors. Linux's final difference is still nonzero and above the witness-size minimum; its discrepancy from the ideal difference exceeds 2×2^-80. Do not report this as loss of all sub-binary64 information or suppress the extra miss. Historical seven-miss runs remain unchanged.

## Full-slot numerical characterization

All displayed values below are rounded summaries of retained exact/canonical records. Norm: maximum absolute real/imaginary component, not complex modulus.

| Maximum | Linux | Windows |
| --- | ---: | ---: |
| Fresh aggregate E0 | 3.20401753e-25 | 3.36907051e-25 |
| Final original-input E8 | 9.14647363e-24 | 9.06530517e-24 |
| Ideally propagated fresh error I8 | 9.14836661e-24 | 9.06562720e-24 |
| Accumulated post-fresh residual A8 | 4.56710917e-26 | 8.30355460e-26 |

The original limit is 2^-80 ≈8.27180613e-25: terminal E8 is approximately 11 times the limit on both hosts. The observed full-slot A8 upper bounds are below that magnitude, but no A80 acceptance criterion has been adopted. Different maxima must not be subtracted as if they belonged to one slot.

At the actual E8 maximum, Linux slot11656/imag has E=-9.14647363e-24, I=-9.14836661e-24, A=+1.89297357e-27; Windows slot5091/imag has E=-9.06530517e-24, I=-9.06562720e-24, A=+3.22027530e-28. In both, A partly cancels I. Including the frozen conditional allowances and serialization quantum, |I|'s lower bound alone exceeds 2^-80, and its lower bound minus |A|'s upper bound still exceeds the limit. Eliminating post-fresh added residual cannot repair these realized fresh ciphertexts' original-input endpoint failures.

Disposition for this bounded endpoint question: **INHERITED-DOMINANT / ADDED-ENDPOINT-SMALL**, conditional on the specified observer model. This now covers the full-slot endpoint rather than only ten anchors. It does not separate E0 into encoding versus encryption causes, establish per-round full-slot local residuals, prove all intermediate lifts/nonwrap assumptions, supply an all-key theorem, or reproduce the paper's statistical mean. Original E80 and the complete project remain unaccepted.

## Retained evidence and independent checks

The four actual artifact members are retained unchanged under linux/ and windows/. Canonical TSVs are recoverable from the gzip members. ARTIFACTS_METADATA.json contains GitHub-reported outer ZIP digests only; no independent outer-ZIP rehash is claimed. Download delays were transport-only and did not trigger CI restart.

`verify_live_evidence.py` uses unchanged public status, gzip, sidecar and primary-binding readers, validates all ordered rows and exact serialized E0/E8 maxima, selected same-slot fields, precise counts, source/job/step identities, one actual CTest and one failure replay. It performs no Mac build, encryption, FFT or full numerical replay. It temporarily decodes one artifact for the public file reader; that local path does not validate the original runner's namespace/race behavior. Exact Fraction strings preserve mathematical decision inputs.

The selected timestamp-stripped `61:` records are genuine hosted-log payloads, not constructed test framing. Their reported hash is **not** the byte identity of the runner's unavailable `primary.ctest.log`, which also contains CTest framing. Linux's complete decoded gh log is retained as received. WINDOWS_JOB.log is the LF normalization of the complete decoded CRLF gh output; LOG_TRANSPORT.md records exact original and saved hashes. Neither is a raw HTTP log ZIP attestation.

Separate Codex contexts supplied a source-only mathematical decision checklist and adversarial review of this root-authored audit. AUDIT_REVIEW.md records findings and corrections. These are independent contexts, not different providers; Fable5.1 remains unavailable after the retained balance failure. No new Pro semantic/precision sign-off is claimed here.

## Next action

Give ChatGPT Pro one complete sanitized exact-source/paper/official-reference/log/gzip/status package for a final scientific disposition and one bounded next engineering decision. Ask it to distinguish paper-method correctness from the original-input stress criterion and propose a mathematically justified next step, without changing a gate or source in this evidence checkpoint. If a production correction is established, require a minimal falsifying test before implementation. Do not simply repeat the same random experiment, change keys/noise/input, or rename A as E.
