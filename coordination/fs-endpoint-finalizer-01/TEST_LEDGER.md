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
