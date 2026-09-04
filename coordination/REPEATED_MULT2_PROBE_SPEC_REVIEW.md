# Repeated-Mult2 Pro probe — independent Spec/correctness review

Input: `/private/tmp/repeated-mult2-return-review.QGVKRy/repeated-mult2-bounded-basis-routing-probe-774fe2d`; ZIP 63,963 bytes, independently rehashed SHA-256 `bee2b27ebf88c901b5b91bc3e79fe386231f07ea580b5228512bf380fdac2fd2`.
Baseline `774fe2dcfca47d7a08cab9c04b29c430e354cf9f`; live `8dcadb4544ed567c6de3a7d3825857f89470b29e` has identical src/include/tests/CMake. `T` below means repository `coordination/tasks/REPEATED_MULT2_PRO_DESIGN_AND_TDD.md`; other paths are relative to the input directory. Read workflow/engineering, actual delivery, pinned official source and supplied paper text §6.3/Table 3.

## Findings

1. **P1 — both new targets receive incompatible compiler flags.** `complete/project/CMakeLists.txt:214–216,226–228` unconditionally adds both `/W4 /WX` and GCC warning flags. The patches contain the same defect. GCC/MinGW will receive MSVC path-like arguments, blocking the required default build before either probe executes. Restore the repository's compiler-conditional structure. T:159–165 requires “Linux/Windows… -Wall -Wextra -Wpedantic -Werror” and default build. This is a static defect, not an observed compile RED.

2. **P2 — advertised identity/immutability checks are partial.** `DESIGN_DECISION.md:30` promises a “parameter fingerprint checked after factory lookup.” Probe `:124–140` instead validates/retains the newly allocated `cryptoParameters`, not parameters from the returned context. Its post-evaluation checks `:386–392` inspect Q moduli and key-row shapes, not root/table/key-value snapshots; row checks `:205–212` omit roots. These cannot substantiate T:123–124's “no mutation of caller inputs/shared contexts/key material.” Inspect the returned context and distinguish verified shape stability from full immutability; no actual mutation was observed.

3. **P2 — h128 ordering is contradictory.** `DESIGN_DECISION.md:5` withholds the second-Mult2 production patch until h128 executes, while `NEXT_PAPER_GATES.md:17–27` places semantic second Mult2 before h128. T:112–114 permits a “small homogeneous diagnostic first”; h128 must remain a paper-setup gate, not an added prerequisite to the frozen N64/UNIFORM_TERNARY diagnostic.

## Verified scope / pending evidence

No P0 found. Exact BigInt checks independently verified 128 canonical dyadic components, all 16 Z products, 16 W squares and both deltas. All 55 old CTest bindings remain ordered and unchanged. The second-contract C++ is the original first-Mult2 oracle plus an exact-rejection assertion (`:1062–1077`); it does not consume the depth-9 JSON or validate W. Its passing rejection is not semantic second-Mult2 RED/GREEN. This bounded delivery is expressly permitted by T:131–136,183–184, not final-goal acceptance.

Probe setup secret objects leave scope at `:340`; later relinearization uses family keys, with no evaluator secret access found. The random-tensor checks are structural, explicitly not an arithmetic oracle (`PROBE_ACCEPTANCE.md:23`). Public-key relation, actual table routing, second-operation arithmetic/precision and paper reproduction remain pending hosted evidence. No new all-key theorem gate is imposed.

Only this note written; no patch application, source/test edit, compilation, crypto, CI, staging, push or delegation. Existing h128-distribution work was preserved. The requested new client-setup/Mult2 seam remains unimplemented pending the user's decision.
