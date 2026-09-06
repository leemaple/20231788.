# Findings and dispositions

Severity concerns acceptance risk, not necessarily a code defect. All original paths below have exact copies under `evidence/`. The input and return manifests bind their bytes. Paper page numbers are physical PDF pages, visually inspected where mathematical layout mattered.

## F1 — BLOCKING: original-input precision remains failed

**Evidence.** Linux source-pinned log lines `6101,6198,6295,6392,6489,6586,6588,6592,6605` retain nine misses; Windows lines `6531,6628,6725,6822,6919,6921,6937` retain seven. Both terminal status files preserve CTest exit8/E80 FAIL. Independent full-slot E8 maxima exceed the limit by 11.0574x and 10.9593x. The original gate implementations are `project/tests/paper_full_eight_square_oracle.h:136–140,181–190` and `paper_full_eight_square_contract_test.cpp:241–248`.

**Disposition.** OPEN as an end-to-end acceptance blocker. No relabeling, failing-record removal, or project-complete verdict. Observer/packer success is not algorithm acceptance.

## F2 — HIGH: root endpoint attribution independently corroborated, but not generalized

**Evidence.** `project/coordination/fs-endpoint-live-run-01/LINUX_JOB.log:6706–6709`, `WINDOWS_JOB.log:7038–7041`; both canonical TSVs, all rows70–16453. The review's supplied full replay and separate integer-interval recurrence agree with all reported maxima and selected signed tuples. At Linux11656/imag and Windows5091/imag, I is negative and A positive. Both allowance-retaining reverse-triangle lower bounds exceed 2^-80. Global A upper bounds are smaller than 2^-80.

**Disposition.** CLOSED for the conditional full-slot endpoint question: INHERITED-DOMINANT / ADDED-ENDPOINT-SMALL. No subtraction of different-slot maxima, inference that every local operation added negligible error, or all-key claim. Small terminal A could conceal cancellation of local contributions. A remains NOT_ADOPTED as an acceptance criterion.

## F3 — HIGH: constructive source correspondence supported; no specific production defect evidenced

The reviewed path implements the compensations essential to the method, rather than ordinary independent operations on the high and low parts:

| Operation | Source locator | Checked correspondence |
|---|---|---|
| DCP | `project/src/double_ckks.cpp:399–437` | Centered last-divisor quotient; low part reconstructed as source minus d times high. |
| Pair Add/Sub | same file `699–807` | Componentwise polynomial operations, cloning ownership and checking compatible receipts/bases/scales. |
| Tensor2 | same file `826–881` | High-high and two cross products; discarded low-low term is the method's intended approximation. |
| Relin2 | same file `1011–1056` | Multiply high by d, append zero divisor residue, official relinearization, DCP, then add the low relinearization. |
| RS2 | same file `1137–1188` | Independently rescale high and recombined ciphertext; reconstruct low as rescaled recombination minus d times rescaled high. |
| Mult2/RCB | same file `1213–1243` | RS2(Relin2(Tensor2)) composition, and exact d*high+low recombination. |
| Reentry/receipts | `project/src/repeated_mult2.cpp:249–285,320–347,472–499` | Exact ordered family transition, same root secret projection, and exact S_r=S_(r-1)^2/(d*m_r); reentry is not another DCP/refresh. |
| Client I/O | `project/src/high_precision_client_io.cpp:333–447,541–596,645–716` | Own high-precision transform/rounding, official public encryption and Poly decryption, and exact logical receipt scale for decoding. |

Official DCP primitive: `references/official-full/src/core/include/lattice/hal/default/dcrtpoly-impl.h:693–711`. Public interfaces: `project/include/openfhe_2023_1788/double_ckks.h:153–164`.

**Disposition.** No production patch. This source correspondence plus bounded evidence supports the tested method; it is not an exhaustive formal code proof. Lack of a detected defect is not proof of absence. The printed sufficient nonwrap antecedent remains unproved for the final multiplication and cannot justify an all-lift theorem.

## F4 — HIGH: fresh error is not isolated encryption noise; h128 is not an ephemeral sparsity setting

**Evidence.** `project/src/paper_h128_client_keypair.cpp:193–217` samples the h128 secret and constructs its public key through the official secret-key zero-encryption primitive. `high_precision_client_io.cpp:559–584` encodes before calling the official public encryption API. `references/official-full/src/pke/lib/schemerns/rns-pke.cpp:111–145,148–193` gives the key/public-encryption algebra. At lines164–165 the public ephemeral polynomial uses the ternary generator without a Hamming-weight argument; `ternaryuniformgenerator.h:69–82` and `ternaryuniformgenerator-impl.h:51–67` document/implement dense ternary sampling when h=0.

With the adapter's sign convention, the decrypted error contribution is `e_pk*v + e0 + s*e1`, plus input encoding error. The adapter's `(a*s+e, -a)` public key is distributionally the usual `(-a'*s+e,a')` after a'=-a; that sign difference is not a defect. Fixed-noise decryption is enforced at `repeated_mult2.cpp:118` and `high_precision_client_io.cpp:188`; the official Poly path does not flood in that mode (`ckksrns-pke.cpp:71–95`).

**Disposition.** No sampler change. E0's measured components were not separated into these causes. Calling E0 purely encryption noise, blaming a particular term, replacing public by secret-key encryption, or sparsifying v would overstate/change the evidence. The paper separates chi_enc and chi_err from secret weight h on physical page4; Table3 alone does not identify an equivalent HEaaN/OpenFHE sampler/input distribution.

## F5 — HIGH: paper displays contain two algebraic inconsistencies; neither supports a production patch

**Scale display.** Physical pages7–8: Lemma4.2's constructive identity and page8's modulus-consumption paragraph require division by both q_div and q_l. The isolated Theorem4.8 comparison displays only q_l. The existing exact d*m receipt is consistent with the constructive method. Removing d is not a fix.

**Tensor sign (additional independently checked finding).** Physical page4 defines sk=(1,s), Dec(ct)=ct dot sk, and public key b=-a*s+e. Page5 Section2.2 nevertheless prints a negative Tensor cross component while asserting evaluation under (1,s,s^2) equals the product of the decryptions. With b=a=b'=a'=s=1, the asserted product is 4 but the printed tuple evaluates to 0. The positive cross component gives 4. The official implementation uses that positive component at `references/official-full/src/pke/lib/schemebase/base-leveledshe.cpp:620–636`, called by the production Tensor2 path.

**Disposition.** Preserve both as source-level paper discrepancies; retain the coherent constructive algebra and OpenFHE convention. The tiny counterexample is a scalar paper-display check in `test_independent_endpoint_check.py`, not a cryptographic test or a falsified production operation. No paper file was edited and no claim of author-issued errata is made.

## F6 — MEDIUM: numerical independence is improved, not absolute

**Evidence.** Live bounds are implemented at `project/tests/paper_endpoint_diagnostics.cpp:88–104`; residuals at `312–336`; direct same-precision roots at `paper_endpoint_transform.cpp:157–215`. The explicit conditional assumptions are in `project/coordination/paper-scale-precision-adjudication-return-01/OBSERVER_ALLOWANCE_PROPOSAL.md`. With K0/K8 the exact power-of-two upper bounds on coefficient l1 norm divided by scale, both captures give:

- B_E0 approximately 1.90934e-152; B_E8 approximately 1.24107e-150.
- B_I8 approximately 1.25376e-147; B_A8 approximately 1.25254e-147.

The independent checker rederives these rational values and retains serialization uncertainty; it does not infer them from agreement. Its grid intervals rigorously propagate the resulting scalar input intervals. The supplied reader/replay and the independent arithmetic were executed separately.

**Disposition.** Conditional assurance remains mandatory. Neither the original packet nor this return supplies interval-certified transcendental generation or a separate inverse NTT. Full endpoint captures do not provide per-round full-slot decompositions. No new universal correctness/security statement.

## F7 — MEDIUM: Linux witness survives as information, fails as accuracy

**Evidence.** Original source `paper_full_eight_square_contract_test.cpp:241–248`; Linux log `6590–6592`, Windows `6923–6924`; canonical rows70–71 on each host. Independently reconstructed Linux witness error magnitude is about 2.39390e-24 > 2*2^-80, while actual separation remains about 6.88710e-22 > 2^-76. Windows's error is about 2.68066e-25 and passes the difference-accuracy limit. Endpoint-to-producer allowances do not bridge either margin.

**Disposition.** Preserve the extra Linux miss. Neither “all sub-binary64 information was lost” nor “the witness passed because it was nonzero” is correct.

## F8 — MEDIUM: provenance is source-bound retained evidence, not recreated live provenance

**Evidence.** `evidence/MANIFEST.json`, `project/coordination/fs-endpoint-live-run-01/RUN_TERMINAL.json`, `ARTIFACTS_METADATA.json`, `LOG_TRANSPORT.md:1–8`. Actual diagnostic ref is `codex/endpoint-live-capture-20260906`, with tested source ed5fd192; the user-specified engineering root is `codex/paper-scale-implementation-20260905`. These are not claimed to be the same branch. Both retained jobs failed, while build, exact evidence selection and upload steps succeeded. The supplied receipt verifier checks groups [1,2,57,1,2,60], i.e. 123 invocations/60 unique regressions per host, not 123 distinct tests.

**Disposition.** Hash binding of retained bytes is verified. Git object history, unavailable runner primary.ctest.log bytes, original Windows CRLF log bytes, and outer GitHub artifact ZIP digests were not independently recreated. Timestamp-stripped `61:` records are selected hosted records, not the runner primary file. Secret scans were supplied as passed, not rerun here. No renewed remote-run claim.

## F9 — MEDIUM: unsupported shortcut from observed failure to a new scale/profile

**Evidence.** Exact recurrence `repeated_mult2.cpp:254–285`; paper physical page13, Section6.3/Table3. For unchanged d and m_r, changing S0 by 2^k changes Sr by 2^(k*2^r). The review's exact rational unit test verifies all eight stages for k=1 and k=4. The paper reports a 1,000-execution average; that is not this fixed input's per-stage/per-component guarantee.

**Disposition.** Reject “add four initial bits,” metadata-only scaling, favorable-key selection, statistical claims from two chains, and any automatic A80 replacement. The one selected next action is an explicit claim/profile separation; no replacement parameter profile is selected or applied. Original E80 remains an open requirement, not a passed test with a new name.
