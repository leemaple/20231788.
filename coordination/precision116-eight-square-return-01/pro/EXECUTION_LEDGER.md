# Execution ledger

## Receipt and environment

Task: EXPERIMENTAL-PRECISION116-EIGHT-SQUARE-01. Dispatched ZIP `experimental-precision116-eight-square-2759fa90.zip`: **1,811,119 bytes**, SHA-256 `16b3e3a5e986340d28b2499a754d2dcb432fec1881bbde41c2bb24288d8ef88f`. Internal manifest SHA-256 `b1169d85c54a99d13d601b625885ad8e5d86e08c99af1f2931f3101ce3edc234`. Declared source `2759fa90840946ef42957c7ba71ebea47e0e4995`; packaging HEAD `80f1c53044e846bf6f0cf558b48732b915b9c116`. Source/commit association is the packet's supplied provenance; no remote Git lookup was performed.

Actual drafting container: Linux6.18.35 x86_64, Python3.13.5, CMake3.31.6, GNU g++14.2.0, Git2.47.3. `command -v gitleaks` produced no path. A bounded search under `/usr/local`, `/usr/lib`, `/opt` found no OpenFHE config/header. That search is not a universal filesystem inventory; the actual package-resolution failure below is the operative compilation limit.

## Actions actually performed

| Action | Actual outcome / scope |
|---|---|
| Files content retrieval | Two conversation-file search attempts returned no indexed content. Used the developer-provided mounted ZIP path; no Library, personal memory or GitHub/private-source lookup. |
| Archive verification | Verified dispatched size/hash, exactly189 safe regular unique members, CRC, self-excluding188-payload closure, every member size/SHA-256, 101 supplied Git-blob hashes, and extracted-byte equality. No extraction path/symlink exception. |
| Governing input | Read current TASK.md and TASK_PREFLIGHT.md before drafting. Source review used the four production .cpp files, relevant public headers, original full-chain test/oracle, candidate seam, requirements and selected pristine reference methods. Other project files were byte-verified, not represented as individually reviewed scientific proofs. |
| Paper access | Inspected supplied text and rendered supplied PDF pages7/8/13, including method definitions and section6.3/Table3. A public-paper web screenshot attempt was denied; local page rendering was the visual fallback. No OCR. Rendered images are scratch material, not return members. |
| Hosted evidence | Inspected supplied RED/GREEN status JSONs and raw job logs. These are earlier hosted outcomes, not local executions or newly dispatched CI. |
| Pure exact arithmetic | Python integers/Fraction checks of frozen Q/root/P/witness equality, new-prime Proth conditions, native/root-order constraints, eight-family deletion order, all nine closed-product scales vs frozen certificate and recurrence, all16,384 rational input-domain bounds, final expected witness interval and published slot0 scalar bounds. Passed. No secret or ciphertext exists in these calculations. |
| Source/static boundary checks | Checked excluded old-profile/nonterminal helper calls, single candidate/small factory call sites, one Encrypt, one DCP, one eight-iteration Mult2 call site, two endpoint Decrypt call sites, evaluator signature/body separation, opt-in CMake text and unchanged original CMake prefix. These are lexical/manual checks, NOT C++ type checking. |
| Patch applicability | `git apply --check --whitespace=error` and `git apply --whitespace=error` both exited0 on a scratch copy of the exact project snapshot; no stdout/stderr. Resulting full files equal the returned copies. Exactly CMake modified plus one new test; other84 original project files unchanged. |
| CMake configuration | One bounded local attempt exited1 at original CMakeLists.txt:9 because OpenFHE1.5.0 package configuration was not found. Raw command/stdout/stderr retained in `checks/CONFIGURE_ATTEMPT.txt`. |
| Candidate build/link | **NOT COMPILED / NOT LINKED.** Configuration stopped before target generation. CMake's generic compiler identification/ABI probe did run; that is not compilation of OpenFHE or the candidate test. |
| Candidate CTest / cryptography | **NOT RUN.** No encryption, decryption, NTT, keygen, encrypted squaring chain, SDK install/build, estimator or hosted test execution was performed here. |

`checks/STATIC_CHECKS.txt` retains actual detailed static results and patch commands. The pure S8/S0 and ideal witness/domain values there are parameter/input calculations, **not actual fresh/round/final ciphertext errors**. The actual-polynomial wrong-normalization control remains unexecuted.

The CMake attempt was a package-availability probe on the returned CMake change, not a candidate numerical attempt. No missing factory, fake symbol, stub SDK or alternate implementation was introduced. It does not establish numerical RED or syntax validity for the new source. No dependency installation was attempted after that failure.

## Self-review corrections during drafting

Before delivery, manual source inspection corrected a mixed-type multi-declarator `auto` in the newly drafted identity snapshots, added validation before ciphertext equality checks, made START identify a requested chain count rather than a completed count, and placed COMPLETE after summary output. Nonterminal negative checks were limited to Input and first Reentry. These are unexecuted author-draft corrections, not repairs of a hosted RED or evidence of independent review. No production source or original test was changed.

## Evidence separation

Earlier supplied RED: run34050734415, source70c37679f4760c6dc2bebc35bdfd741c238f315b, both jobs failed at the missing factory. Earlier supplied GREEN: run34051183115, source2759fa90840946ef42957c7ba71ebea47e0e4995, both jobs succeeded; final60-test checkpoints and the one-operation seam passed, with seam10.85s Linux /11.75s Windows. The seam explicitly says full-eight E80 NOT_TESTED and contains no result Decrypt. Those observations are not results of this draft.

Original profile recorded FAIL/run34039088536: preserved as required by the current task, not rerun. Candidate first-square E80 and full-eight E80: **NOT ESTABLISHED**. Candidate security: **UNRESOLVED**. New-test independent review, warning-clean compile/link and later hosted Linux/Windows execution: **PENDING, NOT CLAIMED**.

## Return integrity and prohibited actions

The output contains only the patch, complete test/CMake files, Markdown rationale/commands/ledger, bounded check logs and a self-excluding member manifest. No source-tree dependency copy, prior ZIP, build product, key material, ciphertext/state, browser state, runtime endpoint record, workflow edit, commit, push, CI dispatch or external conversation mutation is included or performed.

Input strict Gitleaks scans are supplied receipt claims attached to the verified input bytes, not scans rerun by this author. No Gitleaks executable was available here. A limited returned-text/key-token-pattern and filename screening is recorded separately; it is not a replacement Gitleaks certificate or a formal absence-of-secrets proof.

Final archive size/SHA-256 is computed after packaging and supplied externally in the response, not embedded inside that same ZIP.
