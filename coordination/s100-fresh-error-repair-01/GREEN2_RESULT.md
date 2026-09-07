# Original-S100 fresh-error attribution: observed, not endpoint acceptance

Run https://github.com/leemaple/20231788./actions/runs/34110943783, attempt1, sourcea448b787399b43b6024d82c170add403969b493c. Linux101706800879 SUCCESS,5m16s; Windows101706802222 intentionally SKIPPED. GCC13.3/Boost1.83, official OpenFHE pin df495ba2e91739a6dc8f1de254fc5a41155ce504, OMP_NUM_THREADS2. Exact run/job/step receipt: GREEN2_RUN.json (SHA-25663f903260a3786b022d056e86b9e13f9b6ef82ff4756da84d4036cccec8755d4).

## Actual verification

The corrected diagnostic target compiled under unchanged warning-as-error flags. Keyless controls passed1/1 (CTest test0.09s, invocation0.10s). Fresh diagnostic passed1/1 in43.58s. Default60-test suite passed60/60 in2.91s; earlier focused1+2, legacy57, client1, repeated/h1282 invocations also passed. These overlap: do not claim125 distinct tests. The run executes62 unique CTests across125 invocations plus the public contract build steps listed in the run receipt. No old eight-square, S116, publication or Windows experiment ran. The watch terminated exit0; do not restart it or dispatch this completed source again.

Retained complete TWO diagnostic-step logs: remote-green2-diagnostic-steps.log, SHA-256cbd1d44bb8bdfbf7f9ff74bc94e950ce8a7602cd5e8bb43d98da6f403b2a874d. Provenance and all CTest summary lines: remote-green2-provenance-summary.log. The first attempted full-job console retrieval exceeded the output budget and is NOT retained or claimed complete; the two named step logs were re-fetched with exact step filtering and verified untruncated. This is log retrieval only, not test rerun. Raw CTest log has12timestamped blank/header lines ending in spaces; git diff whitespace check flags those evidence bytes. They are deliberately preserved for exact hash identity; no source whitespace defect or whitespace-policy relaxation is inferred. Markdown/JSON/source checks exclude only these immutable raw logs.

## Measurements from ONE actual public encryption

Original S100/h128/N32768/Q/input family was unchanged. Deterministic encoded m, independently sparse-decrypted centered p, and raw integer p-m were each observed at all16384slots,32768components, with binary512/768 transforms and ten direct Horner anchors per polynomial. No per-key noise vector or secret is printed.

| Quantity | Observed component maximum | Location |
| --- | --- | --- |
| A: deterministic encoding O(m)-z | 1.3048302662304672e-28 | slot5957 imaginary |
| B: chosen-lift aggregate public-encryption O(p-m) | 3.3701443335174188e-25 | slot6908 real |
| C: production decoder minus O(p) | 9.9998075384801244e-129 | slot4555 imaginary |
| E0: production fresh decryption minus z | 3.3704198850846009e-25 | slot6908 real |

At the SAME E0-extremal component (slot6908 real), A=-2.7555156718212400e-29, B=-3.3701443335174188e-25, C=6.2203252659843302e-129, E0=-3.3704198850846009e-25. Thus attribution does not subtract independent maxima at different locations. Full signed170-digit tuples and20anchor tuples remain in the retained log.

Same-component reconstruction and independent linearity residual maxima are7.3237828182197854e-154; tolerance exactly2^-300≈4.9090934653e-91. Largest m/p direct-Horner difference is about3.4885e-150; raw-difference Horner discrepancy about1.9184e-175. Legacy decimal100 bridge difference is4.99984e-102, also below the diagnostic tolerance. These observations do not prove arbitrary transcendental accuracy.

Inference: for THIS fixed profile/input and sampled key/encryption, aggregate fresh public-encryption error dominates encoding and decoder errors. Amax is roughly2600times smaller than Bmax, and the signed decomposition directly establishes dominance at the E0 maximum. Simply increasing deterministic encoder/decoder computation precision is not supported as a cure for the main error. This identifies the component, not a sampler implementation defect, statistical population result or exact HEaaN/OpenFHE equivalence.

## Paper/source boundary and next decision

Pinned paper text Section2.1 explicitly separates secret Hamming weight h from encryption distribution chi_enc and error distribution chi_err. Sections6.1–6.3 use HEaaN; Table3 fixes h128/N2^15/S100/~680bitQP and reports average infinity errors over1000runs, but inspected text does not specify the sampled input distribution or concrete chi_enc/chi_err. Do not invent those details or require1000runs. The supplied text contains a NUL later in the document, so text search must use `rg -a`; default binary-search termination is not evidence that the terms are absent.

Pinned official PKERNS::EncryptZeroCore public path (references/official-full/src/pke/lib/schemerns/rns-pke.cpp:148–196) samples v using unweighted ternary whenever secret mode is not GAUSSIAN, and Gaussian e0/e1. At noiseScale1 it gives c0=p0*v+e0, c1=p1*v+e1, hence aggregate (p0+p1*s)*v+e0+e1*s. The h128 secret adapter uses official private EncryptZeroCore to construct (a*s+e,-a), which yields the expected public-key noise e; changing h alone does not sparsify v. Current source validates sigma3.19/noiseScale1. B measurement does NOT separate these three terms, prove hidden sampler-wrap absence, or establish a mismatch with undocumented HEaaN choices.

Next root-owned discriminating step: use only the20already retained anchor scalars to assess ideal eight squarings of the observed fresh value, with same-slot complex arithmetic and explicit numerical bounds. No new encryption or ciphertext chain is needed for that narrow counterfactual. Independent /root/s100_candidate_math_review owns that bounded check. If inherited input error alone already exceeds2^-80, modifying Mult2 precision alone cannot restore exact-input accuracy without an additional justified correction or scientifically distinct conditions. Then reconcile paper experimental assumptions, not lower noise/change input silently.

Independent Pro source/paper review is already thinking in conversation6a9e9069-b06c-83ec-90fa-221abbb89274 with the exact corrected source and all references. It has not yet received these new results; do not interrupt to inject them. Retain its first-pass result, then provide complete updated evidence for any follow-up.

Original S100 eight-square FAIL remains retained; no new full chain was run. Diagnostic COMPLETE does not mean original precision repaired or full paper reproduction complete. S116 remains separately qualified, not substituted. No high-frequency scheduler restored; no Mac builds/crypto/FFT.
