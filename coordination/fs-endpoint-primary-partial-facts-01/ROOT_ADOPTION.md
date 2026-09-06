# Root intake of immutable primary observations — 2026-09-06

Root read the complete changed parser source and focused tests, including all
post-BEGIN helper error paths and legacy numeric commit points. Facts are
committed only after the applicable record's full validation. Earlier failures
carry the immutable empty/Boost-only observation; complete old numeric facts
survive a later malformed selected record without making that record accepted.
Successful parse decisions and the original 32 tests are unchanged.

Exact source 9b040555ad39afa6ee99eb5e78a249fcaa865da010ec7118f03bded37e92af78;
new tests 85973db58b6bc3fb8dbe75402baa53a57625c099fae7df6c2e76eba56fd4aa46.
Root actual regression: 34 PASS / 2.548s / exit 0, evidence/05_root_intake.json.
Finalizer public consumer separately produced actual RED then GREEN for retained
FAIL, PASS, Boost-only and entirely unobserved facts; its evidence is in
coordination/fs-endpoint-finalizer-01/evidence/22 and 23.

No C++, crypto, full scalar replay, CI or scientific accuracy claim. An additional
independent source review of reader commit points and finalizer integration is
assigned to /root/endpoint_canonical_writer; author review is not counted as an
independent seat.

The additional /root/endpoint_canonical_writer source-first review has now
closed at the exact reader and finalizer hashes above with no actionable issue.
It checked post-commit helper propagation and the retained-facts consumer; it
did not rerun tests or count the author's own review as an independent seat.
