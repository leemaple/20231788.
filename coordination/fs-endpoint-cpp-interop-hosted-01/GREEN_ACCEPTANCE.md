# Actual C++ to Python endpoint interop accepted

Observed and locally reconciled 2026-09-06 Asia/Shanghai. This accepts diagnostic interoperability, not the paper's original E80 precision contract.

Exact source c2c924bb68e9f34ada759140410bc85970c0f76f, ref codex/endpoint-cpp-interop-green-20260906, run https://github.com/leemaple/20231788./actions/runs/34037828392, attempt 1. Both jobs are terminal SUCCESS.

| Host / job | Actual new C++ target build (UTC) | Actual interop step (UTC) | Python complete / negative regressions |
| --- | --- | --- | --- |
| Linux / 101499026418 | 14:04:05–14:05:36 | 14:05:36–14:06:23 | 1 test 3.351 s / 3 tests 3.836 s, OK |
| Windows / 101499026504 | 14:06:30–14:08:40 | 14:08:40–14:09:47 | 1 test 4.289 s / 3 tests 4.668 s, OK |

Linux job completed14:06:28UTC; Windows14:09:53UTC. Each compiled the new header/entry and ran the actual producer exactly once. Actual producer exit0, empty stderr, 39 real FS endpoint records. Actual Boost values108300/109200 respectively. Capture/Write/Emit produced canonical16384-row TSV. The frozen hosted harness independently checked every E0 row against exact -z, twelve fixed E8 slots with exact Gaussian-integer arithmetic, and called the real independent finalizer once for full Decimal256 replay, reconciliation, gzip and exact publication. Both finalizers returned the supplied synthetic8 silently; complete status retained count2/E80 FAIL.

No real CTest or encryption chain ran in this interop experiment. Legacy framing and CTest exit8 are constructed test inputs; status chain_count1 is a fixture/schema shape, not an actual chain. Capture's high-precision transform and direct controls are diagnostic work on known zero endpoint polynomials. Same-backend C++ graph checks are not the independent oracle. The original frozen real E80 result remains FAIL.

## Exact artifacts and local checks

Each host uploaded exactly three diagnostic members (producer.stdout.txt, producer.stderr.txt, gate-receipt.json) plus exactly two publication members (identity-specific .tsv.gz and .status.json). GREEN_ARTIFACTS_METADATA.json retains GitHub's artifact IDs, byte counts and reported outer-ZIP digests. The publication outer ZIPs are not claimed independently rehashed locally: the first memory download timed out; final transport used gh run download without restarting CI.

All ten actual member files are retained under green/{linux,windows}-{diag,pub}; only their containing archival folders were shortened to preserve Windows checkout path length. Payload bytes and publication basenames are unchanged. No canonical TSV or scratch directory was uploaded. The canonical TSV can be recovered byte-for-byte from the retained gzip.

GREEN_LOCAL_VERIFICATION.json retains each exact local command and result. After transport and after archival-directory shortening, bounded independent checks verified:
- exactly five regular non-symlink files per host, with no extra payload;
- actual saved receipt equals the complete JSON emitted in that host's full job log;
- source/host/run/attempt, actual producer0, explicit synthetic framing, no actual crypto/CTest;
- all39 endpoint records and producer byte/hash receipts;
- reconstruction of test framing from the real output exactly matches the hosted primary SHA and the public primary parser returns COMPLETE;
- strict status schema and all receipt/status/count/Boost/disposition bindings;
- canonical gzip envelope/checksum, actual gzip/canonical sizes and SHA-256;
- every canonical row index0..16383 appears once in order.

These local checks did not rerun the full numerical replay, C++ computation, or cryptography.

| Host | Canonical bytes / SHA-256 | Gzip bytes / SHA-256 |
| --- | --- | --- |
| Linux | 7975314 / f85103a4302fe8c2ade50b2380ac4de900378b8e18b3bbf5924b1f2f7f443f1e | 2293045 / 3b824ad5f17ebdf67b2a7c86091369f79c2b29519464d6ba4a4a4e8c4a78d837 |
| Windows | 7975316 / 92f4b2ae761f155befbc19ae9164d6bfb1b5e835460ca26ba296dc6236ee8c07 | 2293048 / fe5dbc10e79f24698312235940325548305357455170fa3c9039bb64484aca5e |

Complete decoded CLI logs:
- GREEN_linux_job.log220652 UTF-8 bytes, SHA18061efe1ef07a6b4cb71f252796c361833fe8d55a4618900a493c41a49a1352.
- GREEN_windows_job.log222909 UTF-8 bytes, SHA0cbfcf393ce124fb0d41021727e822856da6d0c26f6b4fdb6a9feffa917d8886.
- These are decoded gh job log outputs, not raw GitHub log ZIPs. Original timestamp whitespace is preserved.

## Next boundary

The real no-argument C++ path still captures but does not write/emit endpoint evidence after cleanup. The existing once-wrapper and finalizer/select need minimal wiring into the existing live workflow. No new numerical criterion is authorized.

The independent source preflight from /root/endpoint_interop_workflow confirmed:
- add live Write → Emit → flush after the existing numeric count and before unchanged Require(numericFailures==0);
- existing wrapper already performs one real CTest pipeline and one finalizer, preserving CTest/capture/finalizer exit precedence; no rewrite needed;
- use pinned native Python3.12 and explicit Windows path identity/inherit;
- use actual fresh short RUNNER_TEMP/fl; preflight actual canonical staging and published status candidate paths <=247 on Windows;
- selector runs after failed wrapper, exact upload only after successful selector; exclude the synthetic self-test branch;
- preserve earlier57/60 suites, five APIs, CTest#61/noarg/1200s/RUN_SERIAL/OMP2 and original oracle.

That reviewer was read-only and did not run tests. It now owns a workflow-only draft in the separate worktree /Users/lifeng/Documents/20231788-openfhe-endpoint-live-workflow-20260906 at basee1f5c70c888f2d651d9dce01aec53924e24ce08f; its draft is not yet accepted or activated. Root owns the live C++ integration. Actual live publication remains pending.
