# Execution ledger — bounded source draft

## Outcome and authority

**Project NOT COMPILED / NOT RUN.** Complete applicable RED and GREEN drafts were produced from the supplied archive. The observed results below qualify patch/source/scalar checks only. They do not constitute a hosted missing-factory RED, a warning-clean project build, a passing encrypted integration test, numerical E80 evidence or independent review.

Engineering source: `dbbbee0d20d8a7ae3c138e42f633414db621a173`; documentation packaging: `21d94c23a7163c4770e8ec377219ea54d4428e2d`; official pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`. No external repository was read or modified. No live Git ancestry, branch cleanliness, original Gitleaks execution environment or hosted run status was independently attested. The input packet's historical qualifications and failures were not rewritten.

## Environment actually observed

The drafting container reported Linux `6.18.35`, x86_64, glibc2.41; Python3.13.5; Git2.47.3; CMake3.31.6; GCC14.2.0; Clang17.0.0. `results/environment.json` retains actual version commands, exits and output. Searching regular files under `/usr` and `/opt` for `OpenFHEConfig.cmake` or `openfhe.h` returned exit0 with no matches. No usable installed OpenFHE SDK was supplied or found. This is a scoped capability search, not proof about every possible filesystem location.

The optional Python parser modules `tree_sitter`, `tree_sitter_cpp` and `clang.cindex` were unavailable. No Gitleaks executable was located. No dependency installation, SDK build, CMake configure, project translation-unit compile, library link, crypto run or CI dispatch was attempted. Available compilers were used only for the small, dependency-free descriptor excerpt documented below.

## Actual input handling and source work

The Files read API was attempted against attachment `file_00000000b99081fba6d93fcb4d5b2f10`; it returned “No readable content was found” for the ZIP. The explicit mounted attachment path was then used with Python ZIP/hash routines. This was an archive-text retrieval limitation, not a failed project build.

The input ZIP has 1,543,581 bytes and SHA256 `75c29c9f1305c44fc64679827dcfc1e8b3ba475b48ab46900f71849c255f9a7b`. Its manifest SHA256 is `40a4e8ee30121c85800b6eea4a541b52c11df4fd7b56cdd8da1c4da11207be91`. Verification observed exactly 198 unique safe regular members, successful CRC checks and exact closure over 197 payloads. Every supplied byte count/SHA256 and all 110 supplied Git-blob IDs matched. The retained extracted input's 197 payloads were checked again after drafting and remained identical; that result is also in `results/environment.json`.

The controlling TASK, preflight, candidate/certificate, qualifications/failure boundaries, owned production headers/sources, test/oracle/build slice and relevant pinned official implementation interfaces were inspected before production editing. Paper citations use the supplied text transcription, not a claimed new PDF visual review. Historical tasks were treated as evidence, not current instructions. Source citations and the specific invariant decisions appear in `DESIGN.md`.

RED was drafted as the test/CMake-only tree. GREEN was drafted separately on top of RED. During production source review, an initially drafted namespaced `PlaintextModulus` cast was identified as inconsistent with the supplied global typedef (`references/official-full/src/core/include/utils/inttypes.h:59`) and corrected to `std::uint64_t` before patch generation. This was a drafting correction found by reading source, **not** a compiler-diagnosed failure, and it did not alter RED assertions. No post-GREEN RED correction patch is needed or included.

Unified source-diff inspection returned the normal `diff` status1 when changes were present; this is not a failed check. Patch construction and source-copy recovery files were generated from the separate draft trees. No implementation result was inferred from those construction steps.

## Actual bounded verification

The final verifier invocation was:

```bash
python3 -B /mnt/data/precision116_return/checks/verify_draft.py \
  /mnt/data/experimental-precision116-profile-seam-dbbbee0d.zip \
  --report /mnt/data/precision116_return/results/static_checks.json
```

**Observed exit0**, output `STATIC_DRAFT_CHECKS_PASS`, 15 check groups. The retained receipt is from the final repetition, after an additional full codec-block preservation comparison was added to the checker. An earlier prepackaging run also exited0 with 15 groups; no failed verifier run was observed. This repetition changed neither patch nor test assertions.

`results/static_checks.json` records exact argv, working directory, native exit, stdout and stderr for all 11 subprocesses in the final run. The disposable replay tree is a byte copy of the supplied project, not an external checkout. Its temporary paths in the receipt identify the actual execution location and are not prerequisites for reproducing the checks.

| Actual operation | Observed result | What it establishes |
|---|---|---|
| Git version query | Exit0, Git2.47.3 | Tool identity only |
| `git apply --check --whitespace=error-all RED.patch` and actual RED application | Both exit0, no diagnostics | RED applies to the supplied project bytes |
| RED `git apply --numstat` | Exit0; three paths | 8 CMake lines, 419 test-header lines and 10 original-test insertion lines |
| `git apply --check --whitespace=error-all GREEN.patch` and actual GREEN application | Both exit0, no diagnostics | GREEN applies after that RED |
| GREEN `git apply --numstat` | Exit0; four allowed production paths | No test/CMake/workflow changes in GREEN |
| GCC constants-only C++17 syntax check | Exit0, empty stdout/stderr | Syntax of the descriptor/constant excerpt only |
| Clang constants-only C++17 syntax check | Exit0, empty stdout/stderr | Same excerpt, second compiler only |
| Supplied `python3 -B -m unittest -v test_static_profile` | Exit0, seven tests `OK` | Existing scalar certificate tests, no encryption |
| Supplied `python3 -B -I static_profile.py` | Exit0; output exactly equals supplied certificate bytes | Reproduction of the accepted static certificate |

The two compiler commands used the actual extracted constant/descriptor declarations, standard integer/array headers and no project/dependency stubs:

```text
/usr/bin/g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -fsyntax-only <temporary>/descriptor_excerpt.cpp
/usr/local/swift/usr/bin/clang++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -fsyntax-only <temporary>/descriptor_excerpt.cpp
```

The excerpt was 2,209 bytes, SHA256 `61132c42b2b3e7fadccc4e1e06d03fa26d68480f98e44efaa82ac8fd9763ff48`. The delivered checker regenerates it. These commands produced no executable/object file and did not parse the full test, public factory, private-access seam or OpenFHE-dependent code. In particular, they **did not observe the intended missing-factory compile RED**.

In-process checks additionally verified exact candidate/test/production constant binding; Proth witnesses and native root orders; every family deletion; all nine exact rational scales by recurrence and independent closed product; full-file recovery copies; changed-path allowlists; RED/GREEN separation; original test body/modes/output byte preservation; original CMake prefix and all 61 old registrations; unchanged five API target definitions; fixed-Q adapter preservation; original factory equivalence; projection/cleanup code preservation; unchanged terminal-only adoption and codec blocks; and the single-expression-only evaluator change. The brace/delimiter pass is explicitly not a C++ grammar/type check. All non-allowlisted supplied files and the source archive remained unchanged.

The candidate scales agree with the certificate, including S8/S0≈1.0000152026932967 and final Q/S8≈0.9999834266523009. These numbers are exact-arithmetic-derived ratios, not measured numerical accuracy. No encrypted sample, selected key, endpoint result or trial retry was generated.

## Outstanding execution and review

| Required later evidence | Current status |
|---|---|
| Actual hosted RED-only missing-factory API compile receipt | **NOT RUN** |
| Warning-clean Linux and Windows GREEN project builds | **NOT COMPILED** |
| Candidate public Encrypt → DCP → one Mult2 integration test | **NOT RUN** |
| Relevant legacy57/60 execution and five API compile checks | **NOT RUN** |
| Candidate full-eight-square original-input E80 | **NOT TESTED**; outside this slice |
| Candidate security qualification / production adoption | **UNRESOLVED / NOT ADOPTED** |
| Independent final implementation review | **NOT PERFORMED** |

There is no missing draft artifact being deferred. Implementation acceptance remains contingent on the real hosted sequence in `TEST_PLAN.md`, then independent review. Root must exclude the appended experimental CTest from old57/60 checkpoints and avoid the original no-argument eight-square chain. The original paper-table E80 **FAIL** and incomplete full paper-reimplementation goal remain unchanged.

## Packaging boundary

The return includes only patches, complete changed-file copies, review/integration documents, a reproducible stdlib checker and its bounded-check receipts. No dependencies, build products, nested input archive, secrets, browser/session state or external workflow mutation is included. This author's self-check is not independent review. The internal manifest excludes itself; the finalized outer ZIP size/SHA256 is supplied outside that ZIP, without a circular self-hash claim.
