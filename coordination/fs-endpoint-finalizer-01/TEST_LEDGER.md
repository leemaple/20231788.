# Incomplete finalizer draft — 2026-09-06

Root owns the public finalizer/CLI slice. Numbered evidence retains actual
command output and pre-run source/test hashes. No C++, FHE or full scalar replay
was run locally and no hosted pipeline is claimed.

- 01/02: missing module RED, then missing-primary IO_ERROR status-only GREEN.
- 03/04: complete primary/missing canonical RED, then retention GREEN (5 tests).
- 05/06: malformed and symlink canonical RED, then validation GREEN (7 tests).
- 07/08: missing CLI RED, then actual incomplete finalize/select CLI GREEN
  (8 tests, 0.305s). Exact manifest is LF-delimited and cannot overwrite a file.
- 09/10: malformed identity types caused three unexpected TypeErrors; bounded
  exact string type/size/grammar preflight now occurs before stem allocation.
  9 tests PASS in 0.268s. Oversized strings were already rejected; no allocation
  measurement or separate oversized-input RED is claimed.
- 11/12: foreign canonical-parent sibling was misclassified as missing/format.
  Bounded os.scandir enforces exactly one expected identity/leaf; explicit lstat
  distinguishes absence from I/O. 10 tests PASS in 0.330s.
- 13: additional filesystem-boundary EACCES/EIO injection was first-pass GREEN,
  11 tests PASS in 0.328s. It is not a new RED/GREEN cycle. Readers, status and
  publication remain real, and canonical/foreign input bytes remain unchanged.

Independent review by /root/endpoint_writer_review found identity preflight,
reliable primary partial facts, canonical-parent isolation and Boolean stat
conveniences. Root corrected preflight/isolation/stat handling with the receipts
above. The primary partial-facts reader is assigned separately to that author
in its isolated worktree; finalizer must consume the reviewed immutable facts.
A second independent source-first review of current incomplete CLI is active
under /root/endpoint_canonical_writer.

Complete validated canonical input still raises NotImplementedError deliberately.
Complete binding/full replay/reconciliation/gzip/publication needs a genuine
hosted RED before implementation. CLI timeout recognition needs observed
CTest-owned markers; explicit timed_out in data tests is not a classifier.
No live wrapper wiring, next chain, scientific accuracy improvement or project
completion is asserted.

## Subsequent source-first review and correction

/root/endpoint_canonical_writer reviewed the exact incomplete-finalizer source
and found empty/launch-failure primary classification, swapped scratch roles,
and unverified closed manifest bytes. Root added public tests before each fix:

- 16/17: actual five-subcase RED; empty primary now reaches the real parser.
  Capture IO_ERROR and explicit observed TIMEOUT survive; a nonzero CTest with
  no reliable numeric completion is CTEST_FATAL. Canonical remains nonempty.
  No timeout is inferred from shell status. 12 tests PASS / 0.396s.
- 18/19: three role-mismatch subcases RED; exact canonical, published and
  primary.ctest.log siblings required before publication. 13 PASS / 0.449s.
- 20/21: actual filesystem-boundary same-length manifest corruption RED;
  closed manifest is boundedly reopened and compared byte-for-byte with the
  exact selected LF list. 14 PASS / 0.942s. Failure remains nonzero and existing
  status is preserved; this is not crash-proof durability.
- 22/23: actual loss of complete FAIL/PASS and Boost-only facts RED. Adopted
  immutable PrimaryLogError.observation from the independently authored reader;
  status now consumes its actual validated facts, without reparsing a prefix.
  15 PASS / 0.419s, including early-error unknown facts and no gzip publication.

Reader source 9b040555ad39afa6ee99eb5e78a249fcaa865da010ec7118f03bded37e92af78;
finalizer 553103c8b4f2277df2b84fb958dc8ed908c69bd5b9a9514493832a23fd882de9;
test c385f4cd7cc591bb99935a519b891a4ddaf7991314391f10e0007322f132954c.
Independent delta review is assigned to /root/endpoint_canonical_writer.
All complete-pipeline/hosted gates stated above remain pending.

Affected integration regression after these exact deltas: 64 PASS / 8.777s,
exit 0, across primary reader, partial observations, finalizer, binding and
metric reconciliation. Both actual tool chunks are retained in evidence/25.
The prior whole endpoint regression at checkpoint 8a186ca was 133 total:
132 PASS and one hosted full-replay SKIP / 20.683s (evidence/14). These two
runs overlap and must not be summed as unique test coverage.

## Independent corrected-slice closure

/root/endpoint_canonical_writer independently read the exact finalizer
553103c8b4f2277df2b84fb958dc8ed908c69bd5b9a9514493832a23fd882de9,
test c385f4cd7cc591bb99935a519b891a4ddaf7991314391f10e0007322f132954c,
and primary reader 9b040555ad39afa6ee99eb5e78a249fcaa865da010ec7118f03bded37e92af78.
It found no remaining actionable defect in this incomplete/CLI slice. Review
confirmed primary-only empty allowance, restricted nonzero no-facts CTest
classification, exact roles, closed-manifest equality and single-parse fact
commit/propagation. Reviewer did not run tests, modify files, or perform CI.
Complete pipeline, actual timeout recognition and hosted integration remain
pending gates, not implicitly accepted by this closure.
