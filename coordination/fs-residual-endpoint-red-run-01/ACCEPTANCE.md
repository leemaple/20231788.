# Hosted endpoint API/link RED — accepted evidence

Decision2026-09-06 Asia/Shanghai: **ACCEPT_EXPECTED_API_LINK_RED_ONLY** for engineering source `2fe655d493dcde5f05aa1515f41ca6823bba30bd`. The actual [run33991083281](https://github.com/leemaple/20231788./actions/runs/33991083281), push/attempt1, remains FAILURE. This acceptance means the intended missing implementation has a preserved discriminating RED baseline; it does not relabel CI as green or establish numerical correctness.

| Evidence | Linux101373319837 | Windows101373319710 |
| --- | --- | --- |
| Old runtime checkpoint | 60 unique tests pass;123 actual invocations | 60 unique tests pass;123 actual invocations |
| Workflow partitions | 1,2,57,1,2,60 | 1,2,57,1,2,60 |
| Five explicit API builds | All completed | All completed |
| New paper target | Compiles, then expected link failure | Compiles, then expected link failure |
| Undefined helpers | Exactly seven endpoint functions,28 diagnostic lines | Exactly seven endpoint functions,28 diagnostic lines |
| New endpoint self-test / paper chain | Not executed | Not executed |

## Reconciled evidence

`RUN_TERMINAL.json` retains both full job-step lists and exact run/source/attempt identity. The run ended at2026-09-05T20:56:13Z; jobs ended at20:51:44Z and20:56:12Z. The only failed step on each host was the new paper-target build; its following paper execution was skipped. Both original decoded job logs were fetched exactly once at21:11:53–21:11:58Z and retained losslessly, with original BOM/Windows CRLF. See `RAW_PREFLIGHT.json` and the original ignored captures for explicit decoded-versus-HTTP and LF normalization boundaries.

The independent configured Sol/high context authored `verify_old_checkpoint.py` and `OLD_CHECKPOINT_AUDIT.md` without reading root's new-link audit. It independently derived the groups from exact tested CMake/workflow blobs, matched both live57/60 inventories including name/order/argv/add_test backtraces, and bound every actual Start/command/Passed record. The result retains246 total host invocation records, five explicit API targets, four other old explicit targets,12 default executables and the production library. Requested selector is not backend attestation or separate-provider validation.

After fully reading the new481-line checker and audit, root executed the unchanged checker on the tracked copy with explicit repo/coordination/capture/output paths at21:21:57.528714–21:21:57.789332Z. Exit0, stderr0. Its183,611-byte JSON is byte-identical to the independent result, SHA-256 `ded5d33cb69c76107b857a776bfcc04bd608b592d1f709dfc13a349de80525fe`. `ROOT_OLD_REEXECUTION.json` contains the actual command and timestamps; explicit paths are needed when running the archived copy outside its original ignored directory. No historical checker was re-executed.

Root separately inspected the new header/main, unchanged workflow and actual linker diagnostics, then ran `verify_link_red.py`. `ROOT_LINK_AUDIT.md` and `ROOT_LINK_VERIFICATION.json` preserve39 exact tested-source bindings, unchanged production, compiler/link ordering, all seven endpoint names and terminal errors. Both platforms reached actual linking, so the intended RED was not masked by dependency/syntax/warning failures. Linux's linker failure propagated workflow exit2; Windows exit1. No numerical observer result or new paper chain is present.

Independent old-checkpoint findings: none. Root new-link/evidence findings: none unresolved. The root link-audit's previously separate old-checkpoint gate is closed by the independent audit and root reexecution recorded here.

## Boundaries that remain open

- The seven helpers, deterministic observer execution, all-slot endpoint capture, conditional-budget checks, canonical evidence writer/reader, failure-preserving wrapper and artifact transport remain unimplemented or unexecuted in this RED slice.
- Original E80 remains FAIL from source9f. Small historical A remains diagnostic and NOT_ADOPTED. This new linkage result supplies no fresh E/I/A, codec120 or complete-project acceptance evidence.
- Linux GCC13.3/Boost1.83 and Windows GCC16.2/Boost1.92 compiled the declaration/assertion source. That is not proof of Boost trig/arithmetic error premises; future observer assurance remains conditional with explicit unsupported/UNRESOLVED handling.
- The final independent Pro semantic review of the completed project still lies ahead. Fable's unavailable receipt is not a review; Codex plus independent available context continue without waiting or probing repeatedly.

## Next owner and minimum action

This CI evidence stage is closed after scoped scan/commit/push. Do not fetch these logs again, rerun this CI, rerun the closed return/scientific checkers, or submit the old completed Pro task again.

Codex now owns a new complete sanitized exact-source handoff to a new Pro conversation for the minimal GREEN implementation under the adopted `../fs-residual-endpoint-red-return-01/pro/ENDPOINT_SPEC.md` and its full TEST_PLAN. Include the actual RED receipts, all required clean-room source, pinned pristine inputs and complete scientific/acceptance context; source-package gates still apply. Preserve current seven declarations/fixtures and old60/API/oracle/production boundaries. Add required malformed-helper/evidence/status/serialization/compression tests before claiming GREEN; separate synthetic checks from live evidence. If Pro becomes unavailable, Codex owns the same bounded work with an independent review context.

Only after that new patch is received and independently reviewed may its automatic exact-SHA hosted run execute the future observer controls and at most one unchanged next paper chain per host. Preserve original E80 failure propagation, full-slot observations, conditional allowances, truthful evidence status and no extra trial/key/input/noise selection. No1000-trial gate, default-branch merge, forced CI rerun, acceptance relaxation or Mac C++/FHE workload is introduced.
