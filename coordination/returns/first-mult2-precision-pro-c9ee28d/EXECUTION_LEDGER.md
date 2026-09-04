# Execution ledger

## Executed in the isolated review environment

| Action | Result | Evidence / limitation |
|---|---|---|
| Outer ZIP size/SHA-256, path safety, symlink, CRC | PASS | `evidence/INPUT_INTEGRITY.txt` |
| Outer 61-payload exact manifest closure | PASS | same |
| Nested precursor ZIP identity, safety, CRC, 33-entry manifest closure | PASS | same |
| 40-file project provenance verification | PASS | same |
| Source review of current production, fixture, oracle, official pinned OpenFHE, paper, and retained logs | PASS | `PRECURSOR_REVIEW.md`, `SOURCE_CLAIM_TEST_LEDGER.md` |
| Patch apply check against exact supplied `c9ee28d…` project snapshot | PASS | `evidence/PATCH_APPLY_AND_FINAL_EQUALITY.txt` |
| Changed-path closure (`CMakeLists.txt` plus one new test) | PASS | same |
| Applied-tree equality to delivered final changed files | PASS | same |
| Exact 54→55 CTest binding continuity | PASS | `evidence/CTEST_CONTINUITY.txt` and TSV files |
| Static source guards: no stale-cache getter, production decrypt, serialization, public/production/workflow edits, seed, relaxed threshold, or broad catch | PASS | `evidence/STATIC_SOURCE_GUARDS.txt` |
| Frozen contract serialization and hash | PASS | `evidence/FROZEN_CONTRACT.json`, `.sha256` |
| Standalone Boost-only input/product/rounding arithmetic compile with C++17 warnings-as-errors | PASS | `tools/standalone_first_mult2_contract_math.cpp`, `evidence/STANDALONE_FIRST_MULT2_MATH.txt` |
| Standalone binary64-collapse, fresh-`2^100` representation, frozen product delta, and `2^-80` separation checks | PASS | same; non-cryptographic only |

The standalone program is not linked to OpenFHE and does not execute Encrypt, DCP, Tensor2, Relin2, RS2, Mult2, or RCB. Its result is fixture arithmetic evidence only.

## Not executed

| Action | Status |
|---|---|
| Candidate CMake configure against pristine OpenFHE | NOT EXECUTED — no OpenFHE installation was available in this container |
| Candidate warning-as-error build | NOT EXECUTED |
| Focused first-Mult2 precision CTest | NOT EXECUTED |
| Full 55-test CTest suite | NOT EXECUTED |
| Linux GitHub Actions | NOT DISPATCHED |
| Windows MinGW64 GitHub Actions | NOT DISPATCHED |
| Mac project/OpenFHE compile or crypto runtime | NOT EXECUTED |
| Security estimation | NOT EXECUTED |
| Performance benchmark | NOT EXECUTED |
| Repeated multiplication / refresh | NOT IMPLEMENTED OR EXECUTED |
| Table 3 reproduction | NOT EXECUTED |

See `evidence/CMAKE_ENVIRONMENT_PROBE.txt`. No external message, git push/merge, CI dispatch/cancel/rerun, credential access, or browser-state use occurred.
