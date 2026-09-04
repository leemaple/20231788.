# Integration04: reviewed high-precision precursor and first Mult2

## Frozen integration boundary,2026-09-04
Integration parent c9d7c7adeafe45a504c8bff491b6b7bdbcc7d3f4,
branch codex/integration-01, clean and remote-SHA verified.
Precision parent5cd9f37ca1361d22879aab1845b92341e4cfc34b,
branch codex/precision-01, remote-SHA verified.
Only committed source at that exact parent is merged; the parallel official
I/O research note being authored outside this tree is not an implicit input.

Git's actual common ancestor is0fc9632fe30619e160f8cd7456d333b9b2ae78a4.
An initial legacy three-argument merge-tree diagnostic mistakenly used8a4657
as the base and therefore displayed spurious removals of later BV documents.
It changed no worktree/index. Re-running automatic-base merge-tree succeeded
with tree63245d9358cde2f93e2c97c478281c329a0abd3b and no conflicts.
The actual no-ff/no-commit merge likewise applied cleanly. Codex explicitly
verified NO deleted paths. Do not treat the initial wrong-base diff as the merge.

## Exact executable delta and reviews
Production src/ and include/ unchanged. All15 prior test source files,
including the accepted fixed-key BV oracle, unchanged from integration parent.
Exactly four new precision files equal the reviewed5cd9f37 bytes:
tests/precision_dcp_rcb_fixture.h,
tests/precision_dcp_rcb_fixture.cpp,
tests/precision_dcp_rcb_contract_test.cpp,
tests/precision_first_mult2_contract_test.cpp.
CMake adds two executable registrations, linking/warning settings and exactly
two public-seam CTests; all53 prior name/COMMAND pairs preserved in order,
new total55. The new names are precision_dcp_rcb_high_precision_contract and
precision_first_mult2_high_precision_contract.
The complete workflow/CMake delta was read: only precision branch admission,
in-band source/run/attempt markers and focused first-Mult2 runs on both hosts;
existing toolchain/pin/resource/full-suite/five-API steps remain.

The main agent previously read the full candidate test/fixture, original Pro
return, actual precision logs and full independent ZCode reviews; reconciled
findings and source checks are retained without substituting chat summaries
for artifact bytes. Precision precursor has genuine runtime RED/GREEN;
first-Mult2 was honestly FIRST-OBSERVED GREEN for existing behavior.
Precursorbd141806 run33864080896 both54/54;
first-Mult2 source47907783a6141d0174da79eae264d779fc598f28/run33873114880
both55/55 with16 fresh-key slot/delta samples<=2^-80.
These are HISTORICAL source-branch results, not post-merge evidence.

Tri-party source/review records now include original Pro, Codex reconciliation,
and actual ZCode GLM-5.3 Max LOCAL STATIC final review. First-Mult2 narrow
ACCEPT has P0/P1=0 and all five P2 dispositions recorded in
FIRST_MULT2_PRECISION_ZCODE_FINAL_DISPOSITION.md. Do not copy its original
incorrect margin, scanner version or decrypted-low bound; corrections remain
separate from the byte-exact original.

## Explicit safeguard deferral, not claimed closure
P2-4 automated stale-cache guard was scheduled at the next integration boundary
but has NOT been implemented. Its new checker CLI is not among the user-
confirmed module seams in TEST_SEAMS.md. The tdd skill requires explicit user
confirmation before new tests at that seam; the nonblocking question and exact
proposed narrow scope are recorded in PRECISION_FIXTURE_GUARD_BOUNDARY.md.
No answer was observed before this merge. OwnerCodex explicitly defers that
automation until the seam is confirmed; this is unclosed technical debt, not
a shipped safeguard or a request to halt already-reviewed public-seam work.

Proceeding with these exact tests does not admit new unknown consumers:
the fixture remains TEST-ONLY in its two reviewed targets, current source
was independently checked for forbidden production Decode/Decrypt/cache use,
and no existing assertion/consumer/threshold is changed. Original plan timing
is revised transparently; future confirmation does not authorize unrelated
production interfaces. The separate production-client I/O work is design-only
until its own interface/seam is agreed. No test-only fixture is a shipping codec.

## Evidence preservation and verification obligations
Whole staged whitespace check reported only original archival whitespace.
Exact seven exceptions, all byte-identical to5cd9f37:
- coordination/evidence/first-mult2-precision/pro-visible-reply.txt
- coordination/returns/first-mult2-precision-pro-c9ee28d/FIRST_MULT2_TEST_DESIGN.md
- coordination/returns/first-mult2-precision-pro-c9ee28d/evidence/PRECURSOR_NAMESPACE_DIFF.txt
- coordination/returns/first-mult2-precision-pro-c9ee28d/patches/0001-add-first-mult2-high-precision-contract.patch
- coordination/returns/precision-reconstructed-bda8791/patches/0001-red-freeze-dcp-rcb-high-precision-contract.patch
- coordination/returns/precision-reconstructed-bda8791/patches/0002-green-replace-only-precision-fixture.patch
- coordination/returns/precision-reconstructed-bda8791/patches/candidate-final.patch

Scoped staged whitespace check excluding ONLY those original evidence files
passed. No source/config/new-note exception is allowed. Keep originals intact.
After final staged secret scan, commit/push without skip-CI and verify remote
SHA; require actual new BOTH-platform55/55, focused precision1/1, all five API
targets, warning-clean build, in-band source/run/attempt and16 actual precision
samples at the unchanged gate. Also inspect retained BV certificates in the
combined suite. Preserve any genuine failure; no retuning. Result PENDING.

No Mac OpenFHE/project build, crypto or benchmark occurred. Full paper repeated
eight-square40/60/h128 experiment, usable client precision I/O and statistics/
performance/security remain open. Repeated Mult2 Pro and Pair external review
continue in their own tasks; neither was stopped, refreshed or resubmitted.
