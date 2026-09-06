# Experimental static profile accepted for a bounded integration test

Date: 2026-09-07 Asia/Shanghai. Root baseline `fb68da960d2bf741fa5ad8643f4a8767268af356`. **This accepts a static certificate, not a new production profile, E80 result or security claim.**

Exact reviewed artifacts:

| File | SHA-256 |
| --- | --- |
| candidate.json | 94189f13c259778f5c48a0e18d485aceade79645a620c5e223cea6c96a498f3f |
| static_profile.py | 6892fb6cf1369bc07b8514f606c8920217a0561f49c527ea7d89cd4c49a7710f |
| test_static_profile.py | ba7985a12b09d8f55f5f04e2ff1f51f15a4a1cfb51fc92188aa548177c6262eb |
| STATIC_CERTIFICATE.json | b7e8590a5ece57c187d01b1aabe2b9f685744f544c1f150d4fcba66acebd1b2d |

## What is established

The three new moduli have deterministic primality certificates and exact-order65536 roots. The candidate is bound to those exact triples. The eight original consumed moduli and reserved P are retained exactly, with no Q/P collision, and all native towers are at most60 bits. All eight family bases are derived by deleting the actual second-last tower. Every exact scale matches an independently stated closed-product oracle. Final S8/S0≈1.00001520 and Q8/S8≈0.99998343; metadata58 is consistent with fresh scale116.

Root's initial missing-module RED and review-driven binding/family RED are retained in `STATIC_EXECUTION.md`. Final7 tests PASS in0.005s and standalone output equals the stored certificate byte-for-byte. Independent `/root/endpoint_interop_workflow` closed all three findings and independently ran7 PASS/0.012s with byte equality. Independent `/root/endpoint_cpp_interop` first checked the mathematical expressions with a separate exact Python calculation, then source-only checked the final delta and closed its wording concern. Both final reviews are tied to the hashes above. They are separate Codex contexts, not independent providers; no new Pro or Fable review of this static script is claimed.

## What is not established

- No actual OpenFHE context for the candidate has been constructed, compiled or encrypted. Library P selection and table construction still need real execution.
- New-profile original-input E80 is **NOT_TESTED**, and the old profile remains **FAIL**. The candidate is **NOT_ADOPTED**. No old source/test/status is changed.
- Security is **UNRESOLVED**: total root QP≈711.99999794bits versus about680 for the original. This is not the paper-table experiment and cannot inherit its128-bit security claim. Security certification is not an invented prerequisite for a clearly labeled bounded correctness experiment.
- The fresh-noise envelope is conditional; actual noise, finite-precision encoding and added residuals are not measured. Raising d does not imply that normalized multiplication omission error shrinks.
- The prospective recombined-coefficient capacity premise covers every canonical slot and both components at all r0–8. Existing ten-anchor observations do not establish it. It neither proves intermediate Tensor/Relin lifts nor adds an all-slot-per-stage observer framework to the next slice.

## Go decision and owner

Proceed only to a minimal, separately isolated **one-operation experimental profile integration**. Keep the old no-argument paper test/profile and all old records unchanged. Prefer a complete-context ChatGPT Pro draft of separable RED and GREEN patches; root owns actual RED execution, integration and hosted verification. Fable5.1 remains unavailable; independent Codex review is the fallback. See `NEXT_ENGINEERING_TASK.md`. This is a go-to-test decision, not an E80 promise or project completion.
