# Actual C++ interop RED acceptance and minimal GREEN handoff

Observed 2026-09-06 Asia/Shanghai, after both jobs completed. This is a diagnostic integration boundary, not a numerical improvement or a passing paper implementation.

## RED evidence

- Exact source: `3eff15009abd02825b7e0da54c93f80d58f1be78`.
- Ref: `codex/endpoint-cpp-interop-red-20260906`.
- Run: https://github.com/leemaple/20231788./actions/runs/34037035615 (attempt 1).
- Linux job 101496883012 completed 13:49:37 UTC; Windows job 101496883194 completed 13:53:05 UTC. Both jobs failed only at the intended interop gate after successfully building pristine pinned OpenFHE and the existing producer target.
- Both actual producers returned 1; stdout was empty; stderr was exactly `paper_full_eight_square_contract: unexpected arguments` plus native LF (Linux) or CRLF (Windows). The unchanged harness asserted producer exit zero and failed.
- The new header was not included in this RED source. This proves missing entry behavior, not compilation or execution of the new header.
- Each diagnostic artifact has exactly producer.stdout.txt, producer.stderr.txt, gate-receipt.json. Both publication uploads were skipped. Actual crypto chain count and CTest invocation count were zero; supplied synthetic CTest exit 8 is explicitly a test input, not observed CTest status.

Affected Python regressions, observed in complete retained job logs:

| Host | Complete fixture | Three negative cases | Result |
| --- | --- | --- | --- |
| Linux | 1 test, 3.260 s | 3 tests, 3.757 s | Both commands OK |
| Windows | 1 test, 4.249 s | 3 tests, 4.696 s | Both commands OK |

The negative cases are count mismatch, signed A8 imaginary mismatch, and reconstructed fresh conditioning-disk excess. These executions validate the changed synthetic canonical namespace. They are not actual C++ interop.

## Retained transport

- RED_linux_job.log: full decoded `gh run view --job 101496883012 --log`, 211215 UTF-8 bytes, SHA-256 `afdda37ac29ec17bd89df3ddeca118a409da16e7102a1d5398df9cda4b8e02ed`.
- RED_windows_job.log: full decoded `gh run view --job 101496883194 --log`, 214493 UTF-8 bytes, SHA-256 `e849bd8a0cbb4851d7464a1448be3fecbb98151d196a0f9a30d8ef46d1a49495`.
- These are complete decoded CLI log outputs, not GitHub's raw log ZIPs. Timestamp whitespace is intentionally preserved.
- RED_ARTIFACTS.json retains exact ZIP command, archive bytes/hash and each member's decoded content, byte count and SHA-256. JSON escapes preserve native CRLF and empty stdout without normalizing the stored evidence.
- Linux artifact 9990555467: 1114 bytes, SHA-256 `c7368e4886a3999fff32abd54fae0678260d8da2776a9a8d56dca11927114387`.
- Windows artifact 9990607564: 1134 bytes, SHA-256 `f5f075249007ba39e472b0535879a819ce31a0dde69d2886417f833c97317ca5`.
- Archives were independently downloaded in memory through gh api, matched against GitHub's digest, required exactly three bounded members, and validated against source/host/run/attempt/exit/stage and stdout/stderr hashes. No artifact path was blindly extracted.

## Minimal GREEN delta

Base HEAD `88c2789c632410bd1977a4daf787a33a7b39d04b`.

Only include the frozen interop header and add the exact six-argument synthetic dispatch before existing self-test and normal Run. Emit the actual primary block, flush/check stdout, return zero only after success. Translate exceptions at this CLI boundary to a synthetic failure and retain EndpointFailure's typed reason; no legacy success/cleanup/CTest/chain records are emitted. Existing C++ code outside the include and inserted block is byte-preserved.

The dedicated workflow only changes RED to GREEN in its display name and exact fresh activation ref, plus two diagnostic upload labels. The two job execution bodies are unchanged. Header, harness, CMake, existing live workflow and diagnostic dependencies remain frozen (14 files in GREEN_STATIC.json).

Independent source reviewer: `/root/endpoint_cpp_interop`, requested GPT-5.6 Sol/high from existing task, backend requested-unverified. It reviewed root-authored dispatch/workflow changes and found no actionable findings. It authored the frozen header; therefore this is not represented as independent review of that header. Review confirmed argc/argv match, branch return before old modes, actual stream/error semantics, no normal cryptographic path, unchanged no-argument CTest, exact frozen header/harness hashes and workflow-body identity. No reviewer build/test/transform/CI was run.

Local work in this slice was bounded Git/source/static/hash/ZIP checks only: no Mac compilation, CTest, FHE chain or full numerical replay. GREEN build/runtime/publication remain pending until an actual hosted receipt is accepted.

Next: push the exact committed GREEN source once to `codex/endpoint-cpp-interop-green-20260906`, verify ref SHA, inspect its single run without rerun. Accept only actual C++ Capture/Write/Emit plus independent Python replay and exact validated publication. A failure must be diagnosed rather than relabeled GREEN.

Original paper E80 result remains FAIL (~10.4 times the unchanged threshold); no precision improvement is claimed.
