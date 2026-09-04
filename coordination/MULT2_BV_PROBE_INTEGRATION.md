# BV same-input path probe: source review and integration

Observed 2026-09-04, Asia/Shanghai. Baseline branch codex/mult2-01,
HEAD2f2e30dbb124c336d041b33945d3661c50fd70c7, initially clean. Source/tests,
CMake and CI byte-diff against bda879104c8a8b1ba6ac9301385b5b1919bef440 is empty.
The current e2e test blob534dbd959afc36639461563eb1b57a0c661387e0 exactly
matches the returned probe's declared preimage. No quarantined code was read.

## Completed independent Pro return

Conversation: https://chatgpt.com/c/6a9a5824-3e5c-83ec-83ed-a73acf3dc062
Title: Diagnose BV Relin2 Failure. The Pro UI reports Worked for44m14s;
Stop is absent, the final response and download control are visible.
No interruption, refresh, reminder or duplicate submission occurred.

Archive /Users/lifeng/Downloads/mult2-bv-diagnosis-independent-candidate.zip:
66462bytes, SHA-25650b3a85159c0a8d860c301ecea84cd7831f9d4a48d2fbe0eaff06d707ac8b5c9.
Codex independently matched its size/hash to the final response, inspected all23
entries(18 regular files,5 directories; safe paths, no symlinks), verified all17
SHA256SUMS entries, all16 manifest payload sizes/hashes and exact18-file closure.
Gitleaks8.30.1 extracted-stage and archive-depth2 scans:zero findings.
No supplied script was executed. Twelve exact selected documents/patches are
retained under coordination/returns/mult2-bv-pro-bda8791/; the full original
archive also retains candidate/evidence files. The selected directory is not
represented as the complete18-file package.

The archived unified patches retain their exact single-space context-only blank
lines. Git's check flags those as trailing whitespace in the newly added archive
files; they are required patch syntax and are not stripped. Active source/docs
pass git diff --check with only those two immutable patch artifacts excluded.

Pro makes no production patch and explicitly marks OpenFHE builds, runtime,
CTest and candidate CI NOT EXECUTED. Its AMEND conclusion distinguishes the
invalid universal certificate, BV/backend proof-applicability gap and an
unproven production-wrapper defect. Its final green candidate remains conditional
and has NOT been applied. Fable5.1's403 was correctly not counted as a review.

## Codex review and corrections to the returned prose

Codex read the full diagnosis, source identity, patch series, test protocol,
execution ledger, claim matrix, entire current e2e test and complete probe patch.
The existing red loop is mult2_e2e_oracle_test bv_real/bv_complex under hosted
CTest; two revisions on two hosts produced8 observed BV failing fixtures.
No deterministic key seed or minimal-ring proof is claimed.

1. DIAGNOSIS section3 incorrectly calls the old norm inequality equivalent to
   the residual bound. Residual<=h is SUFFICIENT, not necessary: at Q=101,
   delta=-5, epsilon=5, h=1, norms satisfy5<=5+1 while centered residual has
   norm10>1. Therefore a passing old inequality does not prove near-additivity.
   The reverse-triangle lower bound used for the observed FAILURES is still
   valid and exceeds h. Original returned prose is preserved, not silently fixed.
2. TEST-PROTOCOL phaseB requests ordinary_combined_relin_execution_error, but
   patch0001 still calls that printed field empirical_E_Relin. Read the actual
   probe field; the later name belongs to the unapplied candidate. This is a
   documentation mismatch, not a runtime result.
3. Snapshot assertions cover their enumerated fields and windows. Existing
   shallow/identity checks are not relabelled full hidden-context or deep-key
   proof. This probe does not independently certify OpenFHE's internal primitive.

## Applied scope: probe only, original failure remains fatal

Applied only patch0001 to tests/mult2_e2e_oracle_test.cpp. The resulting Git blob
e392fa2d293b27a20754f82030819ff348c9ea70 exactly matches the returned postimage.
It reconstructs ordinary raised-high and low paths from public Tensor2 output,
uses independent secret-key/CRT decryption, and checks coefficientwise that
the two errors predict the public Relin2 pair error. It then prints high/low
errors, their triangle bound and the exact centered additivity residual.
The existing empirical_pair_error<=empirical_E_Relin+h assertion remains fatal.

All44 existing registrations, production code, vectors, p30/N64, actual BV and
HYBRID choices,1e-3 functional tolerance and original assertions are unchanged.
No test outcome was weakened to obtain green. This is the discriminating probe
for the ranked hypotheses already recorded in MULT2_BV_DIAGNOSIS.md.
No Mac compilation or cryptographic test was run.

Hosted result is PENDING. Require same-input identity and all independent exact
arithmetic regressions to pass on both hosts before considering any candidate
certificate change. Preserve the old BV failure and all residual values. Any
path/basis mismatch reopens production/oracle diagnosis; do not apply0002.
Even an eventual conditional execution green will not establish the full paper
precision, repeated multiplication, conservative E_Relin or universal theorem.

Owner Codex: retain probe CI, reconcile the backend proof gap, obtain the next
independent reviewer at a tested boundary; precision Pro is a separate live task.
