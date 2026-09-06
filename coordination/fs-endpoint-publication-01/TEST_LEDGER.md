# Endpoint publication test ledger

Base: `891f58e2cbefa816416631fe53c1bc5020ba4545`

Scope: new failure-preserving publication and exact upload-selection seam only.
No C++, CMake, workflow, status codec, gzip codec, primary reader, replay,
encryption, upload, CI, commit, or push was performed.

## Authoring order

Filesystem birth times establish the required design/test/source order:

1. `BOUNDARY.md`: 2026-09-06 12:10:08 +0800.
2. `test_paper_endpoint_publication.py`: 2026-09-06 12:10:38 +0800.
3. `paper_endpoint_publication.py`: 2026-09-06 12:12:00 +0800.

Each numbered JSON receipt under `evidence/` records the exact command,
environment, source hash, test hash, exit, and concise actual output. Receipt 00
is retained but explicitly excluded from TDD evidence because the first command
used the wrong working-directory import path. Meaningful cycles were:

- 01 RED (absent public seam) -> 02 GREEN (COMPLETE/failing-E80 publication).
- 03 RED (incomplete unsupported) -> 04 GREEN (status-only incomplete).
- 05 RED (orphan remained/no fallback) -> 06 GREEN (status-last fallback).
- 07 RED (no structured first cause) -> 08 GREEN (fallback/cleanup failures).
- 09 RED (raw regex `TypeError`) -> 10 GREEN (typed bounded identity rejection,
  filesystem and selection binding).
- 11 RED (payload read before size preflight) -> 12 GREEN (bounded stat-first
  selection).
- 13 records final nine-test publication coverage; 14 records the unchanged
  status/gzip codecs plus publication regression.
- 15 RED (COMPLETE still required arbitrary caller fallback) -> 16 GREEN
  (fallback derived from observed COMPLETE facts with IO_ERROR first cause).
- 17 RED (a valid altered reopened status was accepted) -> 18 GREEN (closed
  staged/fallback bytes must exactly equal the validated bytes).
- 19 RED (four partial write/reread cases left owned files untracked) -> 20 GREEN
  (ownership recorded immediately after exclusive open, before write/read).
- 21 corrects the size-preflight negative control to intercept the actual
  `Path.open` boundary for both status and gzip. Receipts 22 and 23 are the final
  original-author Python 3.12 publication and three-module regressions.
- 24 RED -> 25 GREEN moves COMPLETE immutable-payload/gzip/hash/canonical
  validation after exclusive identity/staging claim so a recoverable mismatch
  commits derived `INTEGRITY` status while retaining observed facts and failure.
- 26 RED -> 27 GREEN proves final status is not visible before owned staging
  cleanup succeeds; a recoverable cleanup interruption instead produces the
  truthful status-only fallback.
- 28 RED -> 29 GREEN tracks and removes an incomplete publication's exclusively
  created partial status candidate and newly owned empty directories.
- 30 RED -> 31 GREEN does the same for a partial fallback-status write while
  preserving the original publication cause rather than the later fallback
  failure.
- 32 records two stale suite assertions that expected no status after COMPLETE
  payload failure. After separating invalid incomplete input from valid-COMPLETE
  failure, 33 records all 15 publication tests green.
- 34 is the final exact-source publication receipt. 35 runs the unchanged gzip
  and status codec tests with publication: all 36 pass under bundled Python
  3.12.14.

## Covered public behavior

- exact explicit identity and canonical status encode/decode round trip;
- COMPLETE gzip/status byte-count, SHA-256, decompression, and explicit
  canonical-byte correspondence;
- COMPLETE payload type/hash/canonical mismatches occur after exclusive claim,
  commit derived `INTEGRITY` status-only evidence when recoverable, and retain
  CTest/count/E80/Boost facts plus a nonzero required exit;
- complete failing E80 retains required exit 8;
- incomplete status-only publication with a nonzero required disposition;
- gzip-first/status-last commit order: validated status remains a private root
  candidate until the owned staging directory is removed, and its final rename
  is the last filesystem mutation;
- removal of only the owned orphan gzip, internally derived truthful incomplete
  fallback, and a raised failure even after fallback commits;
- fallback retains actual CTest exit, numeric count/E80 and Boost facts; an
  `OSError` becomes IO_ERROR rather than an invented scientific failure;
- exact reopened gzip/status bytes must equal the validated input bytes;
- successful exclusive creation establishes ownership before a partial write or
  reread can fail, so all four gzip/status fault positions can cleanly fallback;
- incomplete and fallback partial status creation tracks ownership immediately,
  attempts exact cleanup of only those candidates/new empty directories, and
  preserves the original first cause when fallback or cleanup cannot complete;
- fallback-status failure and orphan-cleanup failure both remain failures and
  retain the first publication cause;
- preexisting identity, repeat/overwrite, non-normal traversal, symlink parent,
  symlink identity, symlink status leaf, malformed identity type, mixed identity,
  wrong gzip hash, canonical/gzip mismatch, and invalid COMPLETE/incomplete input
  shapes fail closed;
- missing status/orphan gzip, modified gzip, and any extra identity-directory
  entry fail selection; sibling foreign files are not selected;
- status/gzip sizes are checked before bounded reads; returned paths are the
  exact nonrecursive allowlist in gzip-then-status or status-only order.

The fault tests mock only `pathlib` filesystem boundaries and assert public
outcomes; they do not assert calls between internal module functions.

## Final source facts and limits

- publication source SHA-256:
  `7fbfb9329817a370db37ac6483f6320ac985a2caaf7286a0cec9085954d4ed3d`
- publication test SHA-256:
  `831b70ef949bd9652b18a75b1953d3c5ed1dd78d9d888207dd66c6a9ba50a709`
- boundary SHA-256:
  `c1c9dcdd12c8e6f88ea58570d794ae977602ba480f30f07d404576f59e2e37f7`
- unchanged status codec SHA-256:
  `f443ff51dbe4aa24b21bfd5d1eff5986a7bcfd064ee1d7bae55ffa6cce283c34`
- unchanged gzip codec SHA-256:
  `d5a6825f5a1092b52f07da70f2a09d45065c283b1fe6e6c6d3d6dcf4f199839e`

Initial receipts 01–14 actually ran under Homebrew Python 3.14.5. Correction
receipts 15–23 actually ran with bundled Python 3.12.14 at the exact recorded
path; no prior receipt was relabeled. There was no C++ compilation, crypto,
16,384-row replay, primary provenance binding, real evidence publication,
upload, hostile-parent race test, crash recovery test, or dual-host run. The
cooperative fresh/exclusive scratch-parent contract and lack of crash-proof
guarantees are explicit in `BOUNDARY.md`.
