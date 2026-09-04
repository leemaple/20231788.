# Combined Mult2 and pair Add/Sub integration candidate

Observed 2026-09-04. Work remains in the dedicated codex/integration-01
worktree. Clean status and exact parents were checked; no user change or old
implementation was read, replaced or discarded.

## Source and accepted parent boundaries

First parent 7afb77d496e606efcaca71767913ef51221ced09 includes the accepted
RS2+Mult2 merge and the latest BV review/proof-handoff documentation. Production
and tests still match tested merge 1e2487fb0539d4659e953ef232020bb800968f8e:
run 33851712076, Linux 100955780944 48/48 0.78s, Windows 100955781223
48/48 1.34s, all warning and Relin2/RS2/Mult2 API builds passed.

Incoming parent 613064117e980d30244dfd7c53915d0869a54a89, pair arithmetic:
source 4b170183f29b415329c232a17ea1924acdd0d954 passed run 33852796677,
Linux 100959175670 46/46 0.52s, Windows 100959175902 46/46 1.69s,
with warning and explicit Relin2/RS2/Add/Sub API builds. Add/Sub genuine
red-green histories, controlled CRT, public lifecycle/keyless and malformed
operand regressions are retained in the incoming evidence and ledger.
The final independent Pair static review is still pending, not presumed.

Merge base 7041a489ae1afa98b75322ec334543f29f10b738. Both parents and remote
tips were verified. Their separate passing runs do NOT certify this merge;
first combined 53-test execution remains pending until its exact SHA is tested.

## Conflict resolution and static verification

Read-only merge-tree preview and actual no-commit merge agreed on two conflicting
files: CMakeLists.txt and .github/workflows/dcp-rcb.yml. Header/source auto-merged.
The resolving-merge-conflicts workflow was followed: inspect histories, compare
the two documented intents, preserve both, introduce no new behavior, then
verify and finish the merge. No abort/reset or side-switch discarded changes.

CMake retains Mult2 AND all five incoming Pair executables, warning-as-error
options for both compiler families, and all API targets. CI retains both branch
allowlists, verbose CTest, and explicit Mult2/Add/Sub API builds on both hosts.
No backend, prime, vector, tolerance, security flag, dependency or CI cancellation
policy changed. Existing tests were not renamed, removed, disabled or relaxed.

Checks actually executed before commit:
- Static CTest parser: exact union of both parents' 48 and 46 registrations is
  53 distinct names with identical command bindings, no duplicates or omissions.
- Source byte construction: merged production equals the first parent plus
  the incoming EXACT 106-line Add/Sub/ValidatePairCompatibility block; header
  differs only by the four incoming declaration lines. RS2 fixes remain intact.
- All ten preexisting test files are byte-identical to the first parent;
  all five added test files are byte-identical to the incoming parent.
- Ruby YAML parse succeeded. For each platform, the merged step list is exactly
  the first parent's steps plus the other parent's previously absent steps:
  Linux 16 and Windows 14, no continue-on-error, verbose CTest preserved.
- Resolved files contain no conflict markers; git ls-files -u is empty.
- Active source/tests/CMake/workflow/coordination pass git diff --cached --check.

The unfiltered whitespace check reports 44 diagnostic lines in SIX historical
incoming log files. Each is byte-identical to the incoming parent and is
preserved, not silently normalized. Only these exact archival paths were excluded
from the active-change whitespace check:

- artifacts/tdd/pair-add-public-api/green.txt
- artifacts/tdd/pair-add-public-api/red.txt
- artifacts/tdd/pair-add-runtime/green-linux.txt
- artifacts/tdd/pair-add-runtime/green-windows.txt
- artifacts/tdd/pair-add-runtime/red-linux.txt
- artifacts/tdd/pair-add-runtime/red-windows.txt

Staged secret scan and the incoming Pro ZIP's content/hash verification precede
commit. Mac builds/crypto are forbidden: actual combined warning/API/CTest
validation will run only on GitHub Actions/Windows after commit and push.
Any real failure must be retained and diagnosed before assertions are changed.

## Remaining acceptance boundaries

This is integration, not a new production behavior or fabricated red-green cycle.
Public seams are now combined in one candidate; this alone proves no >53-bit
precision, repeated multiplication, security or paper-scale performance.
BV certificate remains PER_PATH_CONDITIONAL and universal E_Relin UNPROVED.
The independent Pair review, high-precision public behavior and repeated-use
boundary remain open. Owner Codex: verify combined hosted run, retain exact logs,
then submit a complete sanitized combined snapshot for independent Pair review.
No user decision is required for this reversible in-scope step.
