# Primary reader root adoption — 2026-09-06

Base before intake: 30054c4fc02217b209c1b17e7701db3b5a292d38.
Root imported exactly the author's 26 owned files through apply_patch. The
source and test compared byte-for-byte with the isolated author worktree.
Root fully read source, tests, boundary and ledger; this is integration review,
not the author's own sign-off.

Accepted source SHA-256:
48ede879edd921e180ce3acfdc8ddab3ab141439e74ca0622ab350ec06e795e4.
Accepted test SHA-256:
2ed57cb2ee061716b8bf6a3058af07be99083c8b09580ee7aae1f74ea5bd7522.

Observed root intake command in the implementation root:
`cmp tests/paper_endpoint_primary_reader.py /Users/lifeng/Documents/20231788-openfhe-endpoint-primary-reader-20260906/tests/paper_endpoint_primary_reader.py && cmp tests/test_paper_endpoint_primary_reader.py /Users/lifeng/Documents/20231788-openfhe-endpoint-primary-reader-20260906/tests/test_paper_endpoint_primary_reader.py && /Users/lifeng/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/bin/python3 -B -m unittest discover -s tests -p test_paper_endpoint_primary_reader.py -v`

Actual result: both cmp operations succeeded; 32 tests PASS, 0.874 seconds,
exit 0. The tool output identifier was e554a1. This paragraph is an observed
summary of the returned output, not a retained lossless stdout transcript.
The author independently retained its own final 32-PASS/0.714s result.

Root's final transport finding was material: observed hosted Windows CTest
logs use CRLF. The reader now removes exactly one terminal CR only for a
Windows primary identity, while accounting for the original physical bytes.
Linux, embedded/doubled/bare CR and canonical TSV remain strict. The author's
actual focused RED and GREEN are retained as evidence 20/21; full 32-test
result and fixture hashes are evidence 22. A synthetic regression is not a
new hosted primary-pipeline success.

Earlier author receipts 01–08 are concise contemporaneous command/result
summaries, not all accompanied by source hashes or lossless stdout. Their
scope must not be retroactively enlarged. Receipt 15 is a fixture error and
17 a stale expected classification, not implementation regressions.

The complete-only parser preserves observed original E80 FAIL/count and CTest
exit code; it cannot establish primary-to-sidecar actual C or a replay result.
Root's separate binding slice now tests the actual two readers together.
Metric reconciliation, finalization, publication, hosted integration and
independent final Pro semantic review remain pending. No cryptographic result,
original E80 success, full 16K replay or implementation completion is claimed.
