# Claim-to-test matrix

The central separation is:

```text
exact implementation arithmetic gates
    != execution-specific noise/error certificate
    != universal paper theorem gate.
```

No one row may substitute for another.

| Claim | Independent/public-seam gate | Why it is not circular | Retained status | Candidate status / stop rule |
|---|---|---|---|---|
| Exact source identity | ZIP SHA-256, root manifest path/size/hash closure, Git/OpenFHE commit fields | Checks bytes and names, not asserted behavior | PASS | Must remain exact before patching |
| Tensor2 high arithmetic | `tensor2_valid_arithmetic_immutability`; independent tower/coefficient negacyclic multiplication oracle in `project/tests/tensor2_test.cpp:265-370,480-528` | Expected coefficients are computed independently from input components; no error bound is measured from Tensor2 output | PASS in retained matrices | Any failure blocks integration |
| Tensor2 low arithmetic and omitted low-low term | Same Tensor2 exact oracle and controlled witnesses | Expected cross terms are independently constructed; low-low omission is distinguishable | PASS | Any failure blocks integration |
| Relin2 wrapper/component arithmetic | `relin2_valid_arithmetic_state_immutability`; independent raised-high and public ordinary paths plus exact DCP quotient/remainder comparison in `project/tests/relin2_test.cpp:3702-3852` | Production Relin2 output is compared with a separately constructed path and exact centered decomposition, not bounded by its own error | PASS | Any failure blocks integration |
| BV key shape/basis support | `relin2_bv_zero_digit_valid_shapes`; key vector/basis/format checks | Validates the actual pinned BV shape rather than relabeling HYBRID as BV | PASS | Must remain PASS |
| New same-input Relin2 semantic identity | Patch 0001 coefficientwise comparison between independent high/low ordinary paths and independently recombined production pair | The reference errors come from separately constructed public calls; production DCP/RCB is not used to create the expected plaintext sum | NOT EXECUTED | Failure reopens production/oracle diagnosis; 0002 prohibited |
| Paper near-additivity on one execution | Exact `paper_additivity_residual = pair error - combined ordinary error`; compare with runtime `h` | Computes the proof residual directly; does not choose a threshold from the result | BV contradicted by retained norm lower bounds; HYBRID observed pass only | Diagnostic after 0002; `UNPROVED` universally |
| RS2 exact arithmetic | `rs2_valid_arithmetic_state_immutability`; independent centered CRT reconstruction and rounded division in `project/tests/rs2_test.cpp:534-609,635-665` | Expected high, recombined, and corrected low are computed without production RS2 | PASS | Any failure blocks integration |
| Mult2 named composition | `mult2_composition_contract` and e2e byte equality with `RS2(Relin2(Tensor2(...)))` | Direct public result comparison; no noise estimate | PASS | Must remain PASS |
| End-to-end input immutability | e2e snapshots of both pairs and encrypted inputs in `project/tests/mult2_e2e_oracle_test.cpp:940-987` | Byte/state identity, not an accuracy threshold | Reached and PASS before current fatal gate | Must remain PASS |
| Evaluation-key/cache immutability | Relin2 and RS2 state/immutability tests | Snapshot/equality checks independent of output error | PASS | Must remain PASS |
| Exact-execution Relin2 error bound | Patch 0001/0002: `pair error <= high-path max + low-path max` after exact coefficientwise path identity | Bound is not computed from the production pair error; it is derived from two independent ordinary-path results and triangle inequality | NOT EXECUTED | Conditional execution claim only |
| End-to-end coefficient relation after RS2 | Existing independent decrypt, independent pair recombination, schoolbook integer negacyclic input product, and corrected rational coefficient expression | Uses secret-key arithmetic isolated in the test and does not call production RCB for coefficient expected values | Current BV does not reach this check | Must pass after 0002; failure blocks integration |
| Decoded logical functionality | Existing production RCB/decrypt path with explicit logical/recorded ratio correction and frozen `1e-3` threshold | Separate from coefficient oracle and theorem gate | HYBRID passes; current BV not reached | Must pass, but proves neither bits nor theorem |
| Conservative `E_Relin` for BV/HYBRID | Backend-specific derivation or independently justified bound over stated input/key domain | Cannot be established by one or a few observed executions | NOT AVAILABLE | Remains `false`; no test in this patch claims otherwise |
| Lemma 4.4/Theorem 4.8 universal gate | Formal backend mapping plus conservative bounds and scale statement | Requires proof, not a self-measured execution | UNPROVED; retained BV contradicts paper near-additivity instantiation | Must remain `UNPROVED` |
| Security | Parameterized security analysis; `HEStd_NotSet` is explicitly functional-only | Not inferable from functional tests | NOT CLAIMED | No change |
| Precision bits / encoder quality | Dedicated precision and encoding analysis | `1e-3` absolute error is not a bit guarantee | NOT CLAIMED | Separate concurrent track |

## Independence limits

The Relin2 exact reference intentionally calls pinned OpenFHE ordinary `Relinearize`; it validates the Double-CKKS wrapper's raised-basis/DCP/recombination arithmetic, not the cryptographic correctness of OpenFHE's internal BV primitive. The paper-additivity diagnostic addresses that backend mapping separately.

Similarly, the per-path execution triangle is useful only after exact path identity passes. It cannot excuse a Tensor2, Relin2, or RS2 exact-oracle failure, and it cannot establish an input-domain theorem.
