# Root audit — endpoint declaration/link RED

Scope: new automatic push run [33991083281](https://github.com/leemaple/20231788./actions/runs/33991083281), exact engineering source `2fe655d493dcde5f05aa1515f41ca6823bba30bd`, attempt1. This is not a replay of a closed historical CI audit. The worktree's documentation HEAD at intake was `82bdccda3b67301bf2c9fa41d14946391c00bb88`.

## Observed new result

The run completed FAILURE at2026-09-05T20:56:13Z. Linux job101373319837 completed at20:51:44Z; Windows job101373319710 at20:56:12Z. Both metadata records identify only the paper-target build as a failed step; both subsequent paper-run steps are SKIPPED. Metadata alone does not certify every old test invocation; the independent checkpoint audit is a separate gate.

Each complete job log was fetched once, within21:11:53–21:11:58Z. Original decoded text, BOM and Windows CRLF are retained losslessly in ignored capture JSON. `RAW_PREFLIGHT.json` records hashes, sizes and a strict Gitleaks8.30.1 scan of937,673 framed original decoded bytes: exit0, zero findings. The captures preserve connector-decoded text, not HTTP transport bytes. Linux checked-in bytes are unchanged; Windows's checked-in copy normalizes only CRLF to LF.

| Host | C++ compilation / link lines | Missing endpoint diagnostics | Final error |
| --- | --- | --- | --- |
| Linux | L5462 / L5463 | All seven function names;28 diagnostic lines | `ld returned 1 exit status`; build/workflow exit2 |
| Windows | L5776 / L5777 | All seven function names;28 diagnostic lines | `ld returned 1 exit status`; build/workflow exit1 |

Both sets are exactly Observe, ScaledOneNormExponent, DirectSparseReference768, ExactAbsoluteDifference, AssessDifference, CanonicalDecimal and IsCanonicalDecimal. All refer to the new test header after the link line. There is no earlier compiler/fatal error or unrelated missing symbol in these logs. This realizes the intended missing-definition API/link RED, not a numeric failure or a missing-header test.

The exact new test source compiled far enough to reach linkage on Linux GCC13.3.0/CMake3.31.6 and Windows MSYS2 GCC16.2.0/CMake4.4.2. Actual installation logs retain Linux Boost1.83 packages and Windows Boost1.92.0-3, not a claim that future arithmetic accuracy premises have been certified. Linux cache was a miss; both official build/install steps actually succeeded. Native64/backend4 and the pristine pin are present in the retained run.

No new paper CTest Start61, endpoint self-test result, paper COMPLETE marker, numeric-failure count or paper cleanup output exists. With failed linkage and skipped execution, no new paper chain or observer computation ran in this run. The old low-N checks are distinct from a new paper-scale chain.

## Root verification actually executed

Root wrote and ran the read-only `verify_link_red.py` using `/usr/bin/python3 -I`. Its execution returned exit0 and the JSON retained as `ROOT_LINK_VERIFICATION.json`; the immediate post-execution clock observation was2026-09-05T21:15:58Z. No precise start time was recorded. The script binds39 current engineering files to exact Git blobs at2fe, verifies the two-path engineering delta since9f, and confirms production src/include unchanged fromb1. It verifies both run/job identities, original/LF hashes, actual compiler/linker ordering, seven missing function names, error placement and skipped/absent new runtime evidence.

This was local Git/JSON/text verification only: no C++ configure/build, OpenFHE, FHE, codec, FFT/NTT, benchmark, historical checker rerun, CI dispatch or rerun. The checker tolerates a later documentation-only HEAD by binding exact engineering bytes, not by loosening tested-source identity.

## Disposition boundary

Accept the **link-failure component** of diagnostic RED. The complete RED gate additionally needs the independent old60/five-API checkpoint reconciliation and final evidence review. Numerical observer GREEN remains unimplemented; original E80 remains FAIL from9f. Neither a compile/link RED nor small historical A establishes project completion. Do not start another paper chain, alter acceptance or rerun this CI to seek a favorable result.
