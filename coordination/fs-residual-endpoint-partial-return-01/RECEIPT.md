# Terminal partial return: retained, not integrated

Observed September 6, 2026, Asia/Shanghai. Source at intake: `61ad881372f219e0349951509ac133fd250985fc`, branch `codex/paper-scale-implementation-20260905`, clean; origin is the trailing-dot repository `leemaple/20231788.`.

## Artifact and transport

The existing [Implement Diagnostic Green Package](https://chatgpt.com/c/6a9c8e43-15cc-83ec-ab14-40a2b4b99981) conversation visibly uses `6 Pro`; its inference backend is unattested. It was submitted once and returned **PARTIAL_NOT_GREEN**. No interruption, refresh, duplicate prompt or resubmission was used.

The initial offscreen ZIP click did not produce a verified download. After observing the actual button in the viewport, the recovery click ran from 2026-09-06T00:11:40.229Z to 00:11:40.610Z. The completed file was subsequently found at `/Users/lifeng/Downloads/fs-residual-endpoint-partial-return-01.zip`, not the requested intake download directory. Exact completion time was not observed. There were two ZIP clicks across these recovery attempts, one observed complete ZIP, and no repeated download after verification.

The separate visible outer-identity JSON preview agreed with the actual ZIP: **66,805 bytes**, SHA-256 `adec7ffa30e284e1de77633d7f257b8058ab075fd4844c911bdef49016d153eb`, 21 regular members, 20 manifest payloads, disposition PARTIAL_NOT_GREEN. The ZIP is retained byte-exact alongside this receipt. The 21 `pro/` files are its unmodified text members, not accepted project implementation.

Safe unique relative paths, regular-file types, no encryption, CRC, exact manifest closure, all member byte counts and hashes, UTF-8/LF/no CR/no NUL were checked. The complete framed member stream passed strict gitleaks 8.30.1 at 00:13:49Z, without custom environment configuration, allowlists or baseline. Exact bytes, scanner arguments and stdout/stderr are in [SCAN_AND_EXTRACTION.json](root/SCAN_AND_EXTRACTION.json) and [ZIP_PREFLIGHT.json](root/ZIP_PREFLIGHT.json). Text extraction used apply_patch and byte comparisons. The returned `evidence/package_partial.py` builder was not executed and is retained as untrusted provenance only.

## Source binding already executed

The supplied PATH_IDENTITIES document's 39 unchanged engineering paths were independently checked against API/link RED commit `2fe655d493dcde5f05aa1515f41ca6823bba30bd`, dispatch-doc commit `29e12670150f083be396686f0f4b92136758956f`, intake HEAD and the current clean filesystem. Each byte count, SHA-256 and Git blob agreed. Both new test-local paths were absent from intake HEAD and filesystem and matched the supplied result identities. Both patch add-file hunks reconstructed exactly the supplied complete source files. Both files parsed with Python AST; the author run log identities also reconciled. These checks establish provenance, not numerical correctness.

## Actually executed root replay

Only the two supplied Python files ran, in fresh synthetic namespace `/private/tmp/fs-endpoint-synthetic-primitives.VEJ6fk/tests`, without project imports or integration. The root had read both complete files first. Command, times, runtime and exact raw stream identities are in [PRIMITIVE_RUNS.json](root/PRIMITIVE_RUNS.json).

- Test-only baseline: exit 1, missing `endpoint_evidence_primitives` import, one unittest loader failure. This is **not 23 behavior-specific RED observations**, and replay cannot retroactively supply the author's missing TDD history.
- Adding the unchanged supplied implementation: exit 0, **23 tests passed**, unittest elapsed 0.053 seconds; observed run 00:19:18.008805Z–00:19:18.109300Z.
- Actual root runtime: Python 3.12.14, macOS 26.3.1 arm64, compile/runtime zlib 1.2.12. It differs from the author's environment; no cross-version gzip byte identity is claimed.
- No C++ configuration/build, OpenFHE execution, FFT/NTT, encryption, benchmark, CI rerun/dispatch or paper-chain experiment occurred. Scalar loops are not cryptographic experiment counts.

## Review and disposition

See [STANDARDS_REVIEW.md](STANDARDS_REVIEW.md). An independent Sol-requested review found missing per-test RED history, formula tests sharing the implementation's expressions, one Fraction-only test that bypasses the public seam, and unused constants. Preserve these findings for disposition; do not claim 23 passing primitive tests resolve them.

A requested separate Astra Spec review could not start (`agent thread limit reached`); attempting to reuse a completed review context had the same result. This is session capacity, not a Fable/ZCode quota diagnosis. No independent Spec answer was received. Codex owns the remaining source/spec adjudication, with its independence limits reported honestly.

Disposition: **RETAINED_REFERENCE_NOT_INTEGRATED**. Transport and primitive replay are verified; integration acceptance and the full diagnostic GREEN task are not. The seven C++ helper definitions, full-slot controls/observer, Horner representation check, replay/status publication and CTest/workflow wiring remain unfinished. Original E80 remains FAIL; A is not adopted. None of these review findings alone demonstrates an algorithmic impossibility or a new numerical regression.

## Ownership and next checkpoint

Codex owns the missing critical path under the updated model-routing skill. Fable remains out of the critical path after its last retained insufficient-balance result; no new balance probe was made. Pro's terminal response is no longer polled as active thinking, and the verified ZIP must not be downloaded again.

Next: resolve the partial's source/spec and independent-oracle gaps before integrating any reusable portion; use the frozen endpoint specification and existing exact-source RED for a small missing C++ helper slice. Capture genuine RED for newly authored behavior tests rather than manufacturing historical RED. An unavailable review context does not stop nonoverlapping engineering, but unresolved correctness findings are not waived. Existing completed audits and the September 6 Confirmed daily report stay closed.

This checkpoint publishes evidence only. Production source, active tests, build metadata, CI and numerical criteria are unchanged.
