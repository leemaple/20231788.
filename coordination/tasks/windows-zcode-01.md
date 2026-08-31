# Windows Z code/Zima 01 — independent clean-room implementation

## Objective and isolation

In a new dedicated empty folder on the Windows computer, independently implement the `t=2` Double-CKKS multiplication method from paper 2023/1788 for official pristine OpenFHE 1.5.0.

The destination is `https://github.com/leemaple/20231788`, but its existing implementation is wrong and excluded. Do not clone it as a starting point. Do not open, search, copy, read, adapt, build, or test any pre-existing 2023/1788 code or locally modified OpenFHE checkout on either computer. Do not read ChatGPT Pro output while implementing.

## Inputs

- The user-supplied paper PDF/text placed in this clean-room folder.
- This brief and the project workflow skill.
- A new official pristine OpenFHE 1.5.0 dependency checkout in a separate new folder; record tag, commit, and URL.

No other local code is an input.

## Method

1. Derive DCP, RCB, Tensor2, Relin2, RS2, and Mult2 directly from the paper, tracking plaintext meaning, RNS towers, coefficient/NTT form, level, scale, component count, rounding, and error.
2. Design the smallest cohesive API from scratch.
3. Write deterministic independent-oracle tests before implementation and retain the initial red output.
4. Implement the smallest one-multiplication vertical slice and prove a second-multiplication lifecycle or explicit tested refresh boundary.
5. Prefer public OpenFHE APIs. If a primitive is genuinely missing, prove the gap and isolate the smallest upstream patch with tests.
6. Apply KISS, YAGNI, fail-fast invariants, and no catch-all try/catch.

## Tests

Cover tower/prime roles; signed big-integer DCP/RCB reconstruction; encrypted real/complex slots with signs/zero/near-zero/magnitudes; coefficient/NTT domain; level/scale/basis/component metadata; Relin2 keys/components; independent RS2 rounding; end-to-end Mult2 precision; second-level lifecycle or refresh boundary; malformed/incompatible inputs; warning-enabled build.

## Deliverables

Create greenfield headers/source/CMake/tests plus `DESIGN.md` and `REVIEW.md`. Record exact commands and observed red/green outputs, OpenFHE identity, and findings as observed/inferred/pending. Commit in the new local clean-room Git repository or return a patch. Do not push, merge, open a PR, or claim unrun tests.

