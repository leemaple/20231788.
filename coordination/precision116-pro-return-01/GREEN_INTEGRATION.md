# Precision116 GREEN integration

Root applied the reviewed GREEN patch only after inspecting and accepting the actual Linux missing-factory RED recorded in `HOSTED_EXECUTION.md`. The already running Windows RED job retains its original source SHA; applying this working-tree change cannot alter it.

Applied with `apply_patch` to exactly the four allowed production files. `cmp` succeeded against all four retained Pro GREEN complete copies, with hashes bound in `GREEN_REVIEW.md`. No RED test or CMake change was made: `git diff --exit-code -- tests CMakeLists.txt` succeeded. Ordinary active-source `git diff --check` passed. The production delta is 88 additions and 32 deletions. The earlier source-first independent GREEN review found no actionable source defect.

This adds only the explicitly experimental named factory, immutable profile-derived metadata/scale checks, and necessary private client binding. It preserves the original profile identities and evaluator arithmetic. This is an applied implementation, not yet a compiled or working implementation: GCC/MinGW compilation must resolve the RED errors without changing assertions, then the exact experimental one-operation CTest must pass along with the original 60-test/five-API checkpoints. The two GREEN-only outer timeouts are already 25 minutes; CTest remains 1200 seconds.

No local compiler, CTest or cryptographic operation was run. Next, push this exact GREEN implementation once to `codex/precision116-profile-seam-green-20260907` and bind the resulting run to its source SHA. No original full-eight or endpoint producer may run on that ref. Full experimental eight-square E80 and security remain unresolved; no final implementation-complete claim is authorized by this slice.
