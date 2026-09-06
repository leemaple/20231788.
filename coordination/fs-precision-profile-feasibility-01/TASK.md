# FS-PRECISION-PROFILE-FEASIBILITY-01

State: active analytical feasibility, no new profile adopted or implementation authorized by this brief alone. Opened 2026-09-07 Asia/Shanghai at engineering HEAD `4bd885473a0494a7805374e45d2fd382c4baf6e4`.

## Objective and fixed boundary

Find one justified, finite next engineering slice toward original-input `2^-80` accuracy through eight no-refresh squarings at N32768 / 16384 slots with the same input domain, public encryption, h128 root secret and paper DCP/Tensor2/Relin2/RS2/RCB method on pinned pristine OpenFHE1.5.0. Retain the original exact-profile failure and all current regression evidence. No 1,000-run quota, random-key search, sampler substitution, known-plaintext correction, A-for-E substitution, or weakened precision predicate.

The input state is `../fs-endpoint-scientific-review-return-01/ACCEPTANCE.md`, its independent review qualifications, the actual live run `34039088536` at source `ed5fd192a89d6d4728ad295e87cf06a3f4abc832`, current clean-room source/tests and the verified packet's paper and official references. Do not read any quarantined local implementation.

## Bounded question

Current exact recurrence is `S_r=S_(r-1)^2/(d*m_r)`, with initial scale `2^100`, d near `2^40`, eight consumed moduli near `2^60`, and two final moduli near `2^50`. Increasing only S0 by k bits with divisors fixed multiplies S8 by `2^(256*k)`; that is not a coherent guard-bit patch.

Challenge this **unadopted hypothesis**, not a requested implementation: a second explicit family with S0 near `2^(100+k)`, d near `2^(40+k)`, final moduli near `2^(50+k/2)` and consumed 60-bit moduli unchanged. k=16 is one bounded candidate, not a search over many profiles. Determine exact-scale consistency, final/intermediate capacity, native modulus and auxiliary-P constraints, and an explicit initial public-encryption bound plus per-stage inherited/added budgets. A back-of-envelope rescaling of two observed errors is only a forecast, not proof or test evidence.

A replacement family changes the paper-table parameter experiment. Keep that distinction explicit; success would establish a separately supported parameter regime, not erase the original result. Do not introduce an unbounded universal-Gaussian or security-certification project; state noise-tail and security assumptions and quantify any changed modulus exposure. Fail closed if a necessary inequality does not hold.

## Actual ownership

- `/root/endpoint_cpp_interop`: mathematical feasibility derivation, exact scalar checks if useful; deliver one candidate with explicit inequalities and missing evidence, or one concrete blocking inequality. This owner authored neither the production code nor the Pro return checker; it has now moved from completed review to new design work and cannot independently approve its own new design.
- `/root/endpoint_interop_workflow`: read-only minimal source/test seam audit, hardcoded profile/scale/key/basis/OpenFHE constraints and smallest discriminating RED test. No generalized profile framework.
- Codex root: integrate the completed return, preserve Git checkpoints, challenge the derivation, and commission an independent review before adopting a candidate. Pro remains the preferred external complex-draft/review role; its previous conversation is terminal. Any new Pro conversation needs the complete sanitized exact-source bundle under the project skill, not presumed memory. Fable5.1 unavailable: immediate Codex fallback, no repeated balance probes or invented reset.

## Deliverable and verification gates

One short decision containing exact equations/source boundaries, observed vs conditional vs pending claims, minimal touched files/test interface, original-profile preservation, and the single next falsifiable test. Do not implement a report-contract subsystem. Tiny scalar arithmetic is allowed; no Mac build/crypto/FFT/full-slot replay, CI dispatch, browser manipulation, production edits, or claimed numerical pass in this analytical slice.

Before any accepted implementation: choose exact moduli/roots and exact recurrence (not rounded bit sizes), freeze precision and nonwrap budgets, obtain independent review, write/run the smallest RED for the new behavior, then implement with KISS/TDD. Sustained validation goes to GitHub Actions or verified Windows capacity. A failed feasibility inequality is useful evidence but not a universal impossibility result.
