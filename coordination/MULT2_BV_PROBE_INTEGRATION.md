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

## Observed Linux probe, Windows still live

Source2936b5b5ac485fd528c41cb12577b50d38c2be57,
https://github.com/leemaple/20231788./actions/runs/33844736013,
Linux100934063985:42/44,0.93s. Only the two original BV assertions fail;
all preceding coefficientwise high+low path identities passed in all4 cases.
No path, basis or independent exact-arithmetic regression failed. Warning/default
and Relin2/RS2 API targets passed; later Mult2 API step was skipped after red.

BV REAL: ordinary E179278841604, pair206139699899, high180585569748,
low244286890828, triangle424872460576, exact residual256624272918, h41.
BV COMPLEX: ordinary E175017062761, pair264268905321, high163197013779,
low265247037367, triangle428444051146, exact residual283467857424, h44.
Both exact residuals exceed h while the independent path identities agree.
HYBRID REAL residual9/h38 and COMPLEX residual16/h42 stayed within h for these
executions only. Raw project sections retained in mult2-bv-path-probe/linux.txt.

This observed result supports the backend-to-proof gap for these executions;
it does not prove all production behavior, a conservative bound or precision.
Windows100934063875 is still live building pristine OpenFHE. Patch0002 remains
unapplied until both-host probe adjudication. No job cancellation/restart.

## Windows terminal probe and conditional-certificate decision

The same run33844736013 is now terminal on both platforms. Windows100934063875:
42/44,1.19s, same two original BV assertions; every new coefficientwise high+low
path identity passed. Warning/default and Relin2/RS2 APIs passed; later Mult2 API
was skipped after red. Raw project sections saved BEFORE candidate edits.

Windows BV REAL: ordinary187089566090, pair252147500187, high201960661565,
low209725412505, triangle411686074070, residual287762848148, h43.
Windows BV COMPLEX: ordinary207005538868, pair269853904025, high206666010309,
low254774839099, triangle461440849408, residual383325805072, h47.
HYBRID residuals16/h43 and19/h44 stayed within h for these executions.

Codex independently reread pinned paper text280-312 and690-775, official
base-leveledshe.cpp:319-340, keyswitch-bv.cpp:49-103/245-277,
dcrtpoly-impl.h:230-250 and production Relin2's actual raised-high/low calls.
BV CRT digits and noisy key rows do not justify the paper's single-rounding
near-additivity transfer. Both-host probes discriminate this from a demonstrated
wrapper composition defect. A conservative backend/domain theorem remains OPEN.

Only after retaining those probes, candidate0002 is accepted for hosted TESTING:
replace the invalid cross-execution sample bound with independent per-path
triangle/error and non-wrap checks, retain the exact residual as a reported proof
gap, and keep all44 named cases, vectors, parameters and1e-3 tolerance intact.
The same-input coefficient identities and exact arithmetic regressions remain
mandatory. No production file changes. The temporary debug prefix will be renamed
to a permanent RELIN2-EXECUTION certificate label; numerical checks are unchanged.
This is a corrected execution-test contract, NOT proof that the old inequality
passed, a precision result, a universal paper proof, or full project completion.
Conditional green execution and independent final review remain pending.

Candidate after the diagnostic-label rename, before commit:
tests/mult2_e2e_oracle_test.cpp SHA256
b27c15ceb2ab886077701187cd9700d89aad9bf8feb3904cd0dfccd1c78e1b26.
Retained probe logs SHA256: Linux
a626b5f12e66a618952390492fdc6f42dea6d4d6412bda5887102cecfa79c180;
Windows045f1c7faa8e5a2635aa4746b00dfd42e112a3d44bd7750c2950fc9191889f4a.
