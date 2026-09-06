# Complete finalizer GREEN slice

Base: c1b58dd8a03f5bcb11412f2d317a51843686a766.

The preceding turn made progress: actual dual-host RED was observed at source
e925c3ffcd670d33871ad585db13f1c1387468e8 / run34016221531, independently reviewed,
committed and pushed before this implementation began. Its accepted receipt is
../fs-endpoint-complete-hosted-01/12_dual_host_red_acceptance.json. Do not repeat it.

The same public finalize CLI seam and frozen hosted test now require the missing
complete behavior. Root edits only tests/paper_endpoint_finalizer.py plus the
isolated Python workflow's exact trigger. The test stays at SHA-256
d7784038f2bc2ee41b09bffd5665676d8f149a4f079055bb7c2c0447b7d30d89.

The pipeline must use the same actual parsed primary/sidecar: bind before full
scalar replay, pass that direct replay result into metric reconciliation, then
encode and independently verify gzip, compare decompressed canonical bytes,
and publish COMPLETE with actual row count, hashes, sizes, observed count/E80 and
Boost. Original CTest status, conditional assurance and A NOT_ADOPTED survive.
Known typed replay/reconciliation causes translate at the finalizer boundary to
truthful status-only failure; a known gzip verification error is INTEGRITY.
Unexpected errors are not swallowed or normalized into success.

The original incomplete paths and all underlying readers/math/transport modules
remain unchanged. The positive zero fixture does not alone discriminate omission
of an internal stage; independent source review and subsequent targeted negative
integration fixtures retain that obligation. This is not paper E80 acceptance.

The only GREEN trigger is codex/endpoint-finalizer-complete-green-20260906.
The prior corrected RED trigger is retired in this source. Other workflow bytes,
the existing dcp-rcb file, CMake and all paper/production sources stay unchanged.
No dispatch/rerun, crypto, CTest, C++ build, or artifact upload is added. The Mac
may run bounded existing incomplete/status/transport regressions, never this full
hosted fixture or full 16,384-row scalar replay. Hosted job limits remain 15 min,
child CLI 600 s, and original paper CTest timeout remains 1200 s.

An independently reviewed candidate followed by a scanned exact-source push may
activate the frozen positive test once per host. Preserve the actual result,
including a new failure or timeout; do not retry to select a passing run.
