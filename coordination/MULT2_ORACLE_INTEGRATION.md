# Mult2 independent coefficient-oracle integration

Observed 2026-09-04, Asia/Shanghai. Starting baseline cb28b969e380ff2ad085397dff24878fc3c35040,
branch codex/mult2-01, clean and exact upstream SHA verified before edits.
The Mult2 seam already has real host-product behavior red at30d6d0e and
minimal-composition green at5914302 on both Linux/Windows (40-test suite).
This is additional coverage of that existing behavior, not a newly discovered
production regression and not fabricated historical coefficient-oracle red.

## Input provenance and audit

Candidate: the final, patch7-hardened tests/mult2_e2e_oracle_test.cpp from
the verified ChatGPT Pro return in coordination/returns/mult2-pro-a3a6a17/.
The original ZIP hash and conversation are recorded in MULT2_BEHAVIOR_INTEGRATION.md.
Codex read all1047 lines before integration. No returned script was executed.
One Codex hardening: explicitly reject non-finite decoded real/imaginary values
before std::max accumulation, so a NaN cannot silently escape an error threshold.
No production source, formula, host vector, prime parameter or tolerance changed.

Public seams: genuine Encrypt/DCP/Mult2/RCB/Decrypt; staged Tensor2/Relin2/RS2
is checked separately for composition consistency, never used as the numerical
expected answer. Coefficient decryption uses per-native-tower c0+c1*s+c2*s^2,
schoolbook negacyclic multiplication, cpp_int CRT and centered representatives.
Final output high/low are independently decrypted/recombined without production
RCB. Standard OpenFHE Relinearize is used only to measure this execution's
comparison error, not to declare a universal E_Relin bound.

## First hosted coverage boundary

Only HYBRID/REAL is registered initially. The supplied generic runner retains
the later real/complex HYBRID/BV selectors, which are not yet executed/accepted.
Fixed N64, batch16, p30, first35, depth7, digit0, degree2/level0 encoding,
UNIFORM_TERNARY, FIXEDMANUAL, HEStd_NotSet. Eight frozen host slots include
signs, zero and near-zero; logical decoded absolute tolerance remains1e-3.
The actual active tower count is printed from runtime; none is claimed until
execution. Preselection reserve is32bits, not a security proof.

The certificate measures M_high/M_low/h, empirical standard/pair relinearization
errors, checks an execution-specific non-wrap inequality, and compares the exact
coefficient product against the corrected1/(q_div*q_l) target using integer
cross-products. It also checks the visible uncorrected decoder's deterministic
bias and a separately corrected logical-slot diagnostic. Its printed
conservative_E_Relin_available=false and universal_theorem_gate=UNPROVED
must remain visible. The claimed missing factor is an inference, not an
author-confirmed erratum. These tests cannot establish53/106-bit precision,
secure parameters, a universal theorem, repeated multiplication or performance.

CI CTest is now verbose on both hosts so successful numeric certificates can be
retained. Compilation/CTest are PENDING; no Mac build or crypto test was run.
The later RS2 validation fixes still require reconciliation before main merge.

Next owner Codex: inspect the exact hosted first result; preserve any failure
before deciding a minimal correction. Do not tune thresholds to fit output.
Then advance the other supported key-switch/data-type cases and negative cases,
and submit the verified arithmetic boundary for independent review.
