# Real CTest timeout transport: accepted on both hosts

Evidence reviewed 2026-09-06 (Asia/Shanghai). Source
`3897832bdaf95c05062f1bcec161b929e60d1ede`, attempt 1,
[run 34019353118](https://github.com/leemaple/20231788./actions/runs/34019353118).

## Observed

Both jobs are terminal **failure by design**. The real wrapper ran Test 61 once,
CTest terminated the deliberately sleeping emitter at 2.00 seconds, and the
wrapper preserved shell exit 8. This is not a green workflow badge.

The preparation, bounded actual-byte inspection, status selection, exact
allowlist validation, upload, download, and final "Require TIMEOUT after status
transport" steps all succeeded on both hosts. Linux job 101448928359 completed
07:31:04 UTC; Windows job 101448928307 completed 07:32:10 UTC.
Actual primary streams were LF-only: Linux 28 lines / 1515 bytes, Windows 26
lines / 1240 bytes. Constructed CRLF unit cases remain separate from this fact.
Windows' deepest native status path was 242 characters.

The independent read-only download in 07 checks GitHub's exact artifact ID,
source/run binding, ZIP byte size and SHA-256, and an exact single-member ZIP
allowlist without extraction. It strict-decodes each retained raw status and
matches its SHA-256 to the pre-upload primary log receipt. Both are FATAL /
TIMEOUT / NOT_OBSERVED, with ctest_exit_code 8, zero rows, and null numerical
count and payload fields. No canonical data or numerical outcome is invented.

| Host | Artifact | ZIP bytes | Raw status bytes |
| --- | ---: | ---: | ---: |
| Linux | 9984951229 | 1351 | 1033 |
| Windows | 9984966893 | 1359 | 1037 |

04/05 preserve complete gh-decoded job text, not the raw GitHub log archive.
Original timestamp-line trailing whitespace is intentionally retained.
06 retains terminal metadata. Raw status bytes and complete primary-line
inspectors are in green-artifacts/{host}/.

## Interpretation and limits

The prior exact protocol RED (source 0e3b82b, run 34018316144) classified the same
actual timeout grammar as CTEST_FATAL. Source 3897832 changes only the bounded
timeout observation in the finalizer and the isolated workflow activation ref;
the protocol fixture, wrapper and downstream verifier stay frozen.
The local first-failure/zero-exit/duplicate/truncated/CRLF cases and 91-method
affected regression are retained in 01. The existing independent source review
is in 02. This closes the timeout integration gap, including transport after
failure; it does not prove C++ writer-to-Python numerical interoperability.

No new encrypted chain ran. The original paper-scale run 33978202814 remains
E80 FAIL (~10.4x Linux, ~10.3x Windows the 2^-80 limit), A NOT_ADOPTED.
No local compilation, FFT/NTT, FHE, or full 16384-slot replay ran during this
evidence intake. No run was rerun, cancelled or dispatched.

## Next concrete seam

C++ WriteEndpointEvidence uses fs-endpoint-synthetic-<identity> for synthetic
canonical files, but Python finalization currently looks only for
fs-residual-endpoint-01.v1-r1.<identity>. The hosted handwritten fixture cannot
prove this real interface. Root owns a minimal test-first scope-specific
canonical lookup (published status naming stays schema-defined), then actual
C++ Capture -> Write/Emit -> independent Python validation/replay/reconciliation.
Do not silently rename synthetic artifacts or change the original E80 oracle.
Only after actual interoperability is accepted may normal post-cleanup writer
and once-wrapper wiring proceed.

Fable 5.1 has no observed recovery from its last definitive balance 403. A prior
Codex review worker's next interop audit terminated on usage limit after reporting
the namespace mismatch; its complete new audit was not delivered. Root owns the
gap, with a new independent bounded C++ draft context; this is not multiple
providers or a completed semantic review.
