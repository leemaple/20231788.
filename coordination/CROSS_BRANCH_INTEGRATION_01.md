# Combined RS2 and Mult2 integration, first candidate

Observed2026-09-04. New dedicated worktree
/Users/lifeng/Documents/20231788-openfhe-codex-integration-01,
branch codex/integration-01. Neither this folder nor branch existed before the
checked worktree creation. No previous worktree or user edits were overwritten.
No old implementation or local modified OpenFHE was inspected or reused.

## Exact parents and evidence boundary

First parent1563cfe25998b8989f1b6b2be64ebdc1aad5d30a, codex/mult2-01:
source9bf86cb53a1bbae3a3627fe5efc385d2a29c89ce passed44/44 Linux/Windows in
run33846077283, with warning and Relin2/RS2/Mult2 API builds. It is only a
conditional execution certificate; BV near-additivity remains false in the
observed samples, conservative_E_Relin_available=false and theorem UNPROVED.
The independent ZCode audit remains live; no completed verdict is presumed.

Second parent06393e3a809208fa1a1d3af337ec1cd64ddbbe95, agent/codex-rs2-01:
source7928bb7634baa3603daf32806d70bd790938a353 passed43/43 both hosts in
run33837253564, including warning/API checks. Codex, Pro and authorized local
ZCode static reviews are reconciled in RS2_FOLLOWUP_7928BB7.md, PASS_WITH_GAPS
for that arithmetic/validation boundary, not the entire project. The remote
branch SHA and clean worktree were freshly verified before this merge.

These separate results do NOT certify this combined source. Add/Sub remains on
its separate branch and is NOT included in this first integration candidate.
Actual combined compile/test results are PENDING until a run on the merge SHA.

## Conflict resolution and exact merge scope

A read-only-worktree merge-tree preview and the real no-commit merge both found
one conflict: CMake's end-of-file test registrations. Following the
resolving-merge-conflicts workflow, the resolution retains ALL four incoming
RS2 entries and ALL five existing Mult2 entries. No test is disabled, renamed,
deleted or relaxed. Source auto-merges; relative to Mult2 it adds the previously
tested three-line per-native-tower format guard AND the nine-line declared-basis
validation. Those earlier RS2 changes were not yet in the Mult2 branch.

The RS2 test file and review/evidence records come unchanged from the second
parent. The Mult2 e2e oracle and production composition remain unchanged.
CI's push allowlist gains exactly codex/integration-01 so the combined SHA is
actually tested. No workflow cancel/rerun, dependencies, security settings,
backend, test parameters or thresholds are changed for this merge.

Static checks before commit: inspect parent histories and merged production diff,
verify exact source/test provenance, test-name union/no duplicates and conflict
marker absence, git diff --check, and secret scan of the staged change.
Mac compilation/crypto is forbidden: finish/push the merge after those static
checks, then use the existing hosted warning-as-error and complete48-test suite,
with explicit Relin2/RS2/Mult2 public API builds, on both platforms. Preserve and
diagnose any real combined failure before changing code or assertions.

The original goal is intact: high precision, repeated multiplication, the final
combined Add/Sub boundary, independent review reconciliation and final evidence
are not inferred from this preparatory merge. Owner Codex; no user decision is
needed for this isolated, reversible integration step.

## Pre-commit checks actually executed

The merged production differs from the RS2 parent only by the one public Mult2
declaration and literal composition body. RS2 tests exactly match the RS2 parent;
Mult2 behavior/e2e tests exactly match the Mult2 parent. A static parser verified
the exact union of both parents'48 CTest names AND command bindings, no duplicate
or missing registration. git ls-files -u is empty after resolving CMake.
Gitleaks8.30.1 staged-change scan:zero findings, about346KB inspected.

Unfiltered git diff --cached --check reports whitespace in TEN immutable incoming
evidence files only: nine archived RS2 CI logs and the original Pro REVIEW.md
(whose Markdown double spaces encode line breaks). Each file was independently
byte-compared to exact incoming06393e3 and preserved unchanged. Active code,
tests, CMake, workflow and authored coordination pass the check with only those
ten exact historical artifact paths excluded. This is not a code-format waiver,
nor a runtime test result. The preserved files are:

- artifacts/tdd/rs2-declared-basis/green.txt
- artifacts/tdd/rs2-declared-basis/red-linux.txt
- artifacts/tdd/rs2-declared-basis/red-windows.txt
- artifacts/tdd/rs2-deep-immutability/green.txt
- artifacts/tdd/rs2-mixed-tower-format/green.txt
- artifacts/tdd/rs2-mixed-tower-format/red.txt
- artifacts/tdd/rs2-prime-role-witness/green.txt
- artifacts/tdd/rs2-public-pipeline/green.txt
- artifacts/tdd/rs2-terminal-rejections/green.txt
- coordination/returns/rs2-pro-7928bb7/REVIEW.md
