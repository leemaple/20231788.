# Independent Standards review

## Identity and scope

- Repository: `/Users/lifeng/Documents/20231788-openfhe-paper-scale-implementation-20260905`
- Branch / reviewed HEAD: `codex/paper-scale-implementation-20260905` / `797e86dc0ff625c1847543fcbab295b2a330de76`
- Fixed implementation source: `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89`
- Prospective inputs: `DIAGNOSTIC.patch` (`9c7ba12d03bc39cfb8c4103c5cad854e743c5ec8796189c62a7b3a45032a6bb7`) and `PORTABILITY.patch` (`3361757fe975563082ef51d90760c44e69fbe5a18b316e83448efcdee8f90b6b`), checked against the two original and combined complete test files.
- Axis: repository engineering standards, fail-fast boundaries, type/portability assumptions, KISS/YAGNI and supplied smell heuristics. Paper mathematics/spec correctness was excluded.

## Finding

- **Judgment smell — Mysterious Name (non-blocking):** prospective `tests/paper_full_eight_square_oracle.h:169-172` names the inherited, added, and local component errors `i`, `b`, and `l`. In particular `b` represents the task's `A` residual, making the most important diagnostic distinction unnecessarily easy to misread. Rename them to `inheritedError`, `addedError`, and `localError` (or equivalent). This is readability/maintenance feedback, not a correctness violation.

## Hard-violation result

No hard Standards violation found. The patch remains test-only; retains all original `2^-80`/`2^-120` acceptance gates and final failing exit; accumulates only finite acceptance misses; preserves fail-fast structural, nonfinite, codec/oracle and nonwrap checks; adds no broad catch; and confines allocator-backed Boost storage to the root-construction portability seam before conversion to the unchanged binary512 `Real`.

No compiler, CMake, OpenFHE, FHE, FFT, NTT, numeric execution, CI, browser, or network action was run in this review. Hosted Linux/Windows integration remains pending.
