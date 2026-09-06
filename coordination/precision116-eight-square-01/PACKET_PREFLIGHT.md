# EXPERIMENTAL-PRECISION116-EIGHT-SQUARE-01 packet preflight

## Findings

No actionable completeness, provenance, exclusion, or source-binding defect was found. The archive is suitable for root's separately controlled submission after this preflight; this is packet acceptance only, not a claim that it has been uploaded, submitted, compiled, run, or scientifically accepted.

## Exact packet identity and closure

- Archive: `artifacts/handoffs/precision116-eight-square-01/experimental-precision116-eight-square-2759fa90.zip`
- Observed size/SHA-256: `1,811,119` bytes / `16b3e3a5e986340d28b2499a754d2dcb432fec1881bbde41c2bb24288d8ef88f`
- Embedded self-excluding `MANIFEST.json` SHA-256: `b1169d85c54a99d13d601b625885ad8e5d86e08c99af1f2931f3101ce3edc234`
- Observed member closure: 189 unique regular files = 188 manifest payload rows + the manifest itself. Every payload's decoded byte count and SHA-256 matches its row; ZIP CRC validation passed. There are no directory entries, duplicate names, symlinks, absolute/traversal/backslash paths, or nested ZIPs.
- `PACKET_RECEIPT.json` independently matches the archive path, byte size/hash, manifest hash, ordered member list, 189/188 counts, packaging/source identities, and recorded scan dispositions.

## Source and provenance binding

- Packaging documentation HEAD is `80f1c53044e846bf6f0cf558b48732b915b9c116`; engineering source is the actually tested `2759fa90840946ef42957c7ba71ebea47e0e4995`; pristine OpenFHE is pinned to `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- All 101 locally sourced manifest rows were independently compared with their declared Git objects. Decoded bytes, file modes, computed Git blob IDs, recorded blob IDs, paths, and declared commits all agree. This covers 85 project files from the tested source plus 8 requirements, 2 task documents, and 6 retained evidence files.
- The 85 `project/` members are exactly the complete tracked `.github/workflows/dcp-rcb.yml`, `CMakeLists.txt`, `include/**`, `src/**`, and `tests/**` set at `2759fa90840946ef42957c7ba71ebea47e0e4995`; there are no missing or substituted files in that source/test/build closure.
- In particular, CMake's current paper target sources are all present: `paper_full_eight_square_contract_test.cpp`, `paper_endpoint_scaled_norm.cpp`, `paper_endpoint_exact_scalars.cpp`, `paper_endpoint_transform.cpp`, `paper_endpoint_diagnostics.cpp`, and `paper_endpoint_evidence_writer.cpp`. Their recursively used project headers/helpers are covered by the complete tracked include/src/tests selection. The current one-operation experimental seam and frozen original oracle are both supplied.
- `TASK.md` and `TASK_PREFLIGHT.md` match packaging HEAD and their accepted SHA-256 values `1fb9e2188ba8f4f90f3b4ac3d5fe7c4bdd248529363001366fe2e4cbfdf71a0d` and `643e7c872c2a7542fadbb966075c03bb96d3ee2d083c4e2c96c667fa1eead057`.

## Inherited references and evidence

- All 87 inherited members match the pinned prior input archive (`75c29c9f1305c44fc64679827dcfc1e8b3ba475b48ab46900f71849c255f9a7b`, manifest `40a4e8ee30121c85800b6eea4a541b52c11df4fd7b56cdd8da1c4da11207be91`) byte-for-byte, and each embedded prior-manifest row agrees exactly. The set is 77 official pristine OpenFHE files, 4 Boost 1.83 files, 2 paper files, and 4 workflow/instruction files.
- Eight source-bound requirements cover the accepted correctness scope, original profile/input/parameter boundaries, conditional budget, prior profile task, exact static certificate, and candidate identity. They are context, not permission to weaken the new `TASK.md`.
- The six evidence members are exactly RED/GREEN Linux/Windows raw job logs and their two terminal status records, all byte-bound to packaging HEAD. The RED status is terminal failure at `70c37679f4760c6dc2bebc35bdfd741c238f315b` on both hosts; the retained logs contain the intended missing-factory diagnostic after the legacy 60-test checkpoint. The GREEN status is terminal success at tested source `2759fa90840946ef42957c7ba71ebea47e0e4995` on both hosts; retained logs show the one-operation seam PASS with `full_eight_square_E80=NOT_TESTED`, `security=UNRESOLVED`, and observed durations 10.85 seconds on Linux and 11.75 seconds on Windows. These are retained-file observations, not a network revalidation or evidence of the pending eight-square result.

## Exclusions and review independence

- Name/member inspection found no `.git`, dependency/build/cache/browser/runtime-state/credential class, private-key file, nested archive, or unrelated full-slot/live endpoint capture.
- Prior Pro author design/verdict and prior independent acceptance/review documents are deliberately absent. The packet provides current source, task/specification, pristine references, and raw one-operation evidence for a new first-pass semantic review without priming it with old verdicts.
- `PACKET_PREFLIGHT.md` is an internal post-construction review artifact and therefore is not inside the already hashed archive; that is not a manifest-closure defect. The accepted task-level preflight is included.

## Verification boundary

Reviewer: `/root/endpoint_interop_workflow`, independent Codex agent context (GPT-5 family; no provider-diversity claim).

Performed read-only: repository state inspection; archive/member/CRC/hash/manifest checks; Git object comparisons; exact source-tree set comparison; prior-packet byte/manifest-row comparisons; terminal status parsing; and narrow retained-log/source-identity inspection. The checks were ephemeral and did not create another checker framework.

Not repeated: root's two Gitleaks 8.30.1 scans or targeted content scans. Their recorded zero-finding commands/results and environment are present and internally consistent in `PACKET_RECEIPT.json`, but this reviewer does not relabel them as independently executed scans.

Not performed: packet rebuild, source/test/CMake/CI edits, build, cryptography, full numerical replay, network/browser activity, upload, Pro submission, or Git mutation.

Disposition: **packet preflight PASS; no blocker to root's separate submission step. Runtime and scientific acceptance remain pending.**
