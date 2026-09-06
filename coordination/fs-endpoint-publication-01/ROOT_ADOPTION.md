# Root publication intake — 2026-09-06

Root fully read final source, test, boundary and ledger before executing tests.
Final source 7fbfb9329817a370db37ac6483f6320ac985a2caaf7286a0cec9085954d4ed3d;
tests 831b70ef949bd9652b18a75b1953d3c5ed1dd78d9d888207dd66c6a9ba50a709.
Exactly 40 author-owned files were imported via apply_patch. Source/test cmp
succeeded against the isolated author tree. Root actual 15 PASS in 0.095s,
exit 0, retained in evidence/36_root_intake.json (not a hosted test).

Independent findings and disposition:
1. Arbitrary caller fallback could rewrite known E80/count/Boost and invent a
   scientific reason. Closed: fallback now derives solely from validated
   COMPLETE, preserving observed facts; IO vs integrity is explicit.
2. Reopened schema-valid status could differ from the bytes that were validated.
   Closed: closed bytes must match, including status and fallback.
3. Partial write/read failures could leave exclusively created paths untracked.
   Closed: ownership is recorded immediately after exclusive open.
4. COMPLETE payload preflight failed before a recoverable fallback transaction.
   Closed: type/hash/canonical checks are now inside the claimed private tree.
5. Final status could appear before staging cleanup completed, and incomplete/
   fallback partial files were not cleaned precisely. Closed: root private
   candidate, staging removal, final status rename last; explicit owned cleanup.
   Secondary failure retains the original cause and nonzero result.

Original author /root/endpoint_canonical_writer wrote the first slice; independent
/root/endpoint_writer_review made the last bounded corrections after its own
source-first findings. Root supplied independent final integration review.
Both are separate Codex contexts, not different model providers; requested
Sol/high backend identity is requested-unverified. Final Pro semantic review of
the complete scientific project remains pending.

No blocker remains for this cooperative, exclusive-parent filesystem seam.
The publisher does not validate scientific TSV contents or replay provenance;
the finalizer must do that before passing complete status/payloads. These tests
use tiny synthetic files, not a live 16,384-slot artifact. Windows filesystem
execution, hosted exact upload, full pipeline, C++ live call and next authorized
chain remain pending. No crash-durability or hostile-parent-race guarantee.
