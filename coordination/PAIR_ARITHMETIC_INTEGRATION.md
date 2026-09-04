# Pair arithmetic returned-candidate integration

Observed2026-09-04, Asia/Shanghai. Starting baseline
5d8956b5e90e26168b842c1670f5a6db04b90c08, codex/pair-arithmetic-01,
clean and upstream matched. Add API already has independently observed
compile red/scaffold green; Add arithmetic still throws logic_error, Sub absent.

## Completed Pro return

Same independent conversation completed; Stop absent, final response actions
and READY_FOR_CODEX_INTEGRATION visible:
https://chatgpt.com/c/6a9a4269-665c-83ec-b130-8e40fd86f2d7
Title: 实现 Pair 加减补丁. It explicitly reports no installed OpenFHE and
NO compilation, runtime tests or CI execution. Its patch states are proposals,
not observed red/green outcomes. No live thinking was interrupted or task resent.

Returned pair-arithmetic-pro-7041a48-solution.zip:212032bytes,
SHA-256735dea4e6c164ced95c2829ea8eb5316201eb900fd5d77b1aad171e94e2676c4.
Input source7041a489ae1afa98b75322ec334543f29f10b738, original24-file packet
900942bytes/SHA50269f2a0f5198d5f4aee312808097370e6153783f7586cb1e9c0446da133c38.
Codex verified the actual download's size/hash against the visible response,
inspected all88 ZIP entries (66 regular files,22 directories; safe paths/no links),
and independently recomputed all65 FILE_HASHES entries with exact closure.
All12 patches match their declared hashes/lengths. Input manifest hash matches
a14252379ac595d22272c7daeff8b88c10e5099dc6e47c6aeb88623cf6b9a054.
Gitleaks8.30.1 archive-depth2 scan:zero. No supplied verification script executed.
Original ZIP and selected exact documents are retained under
coordination/returns/pair-arithmetic-pro-7041a48/.

## First runtime red boundary: Add

Codex read the complete returned README, source fact audit and96-line Add
behavior test. Only Add runtime test is being integrated now; the existing
add_api_contract_test name/history is retained (do not duplicate the returned
pair_add_api_contract_test or replay patches1/2 over a completed API boundary).

The returned first test checked fresh identity/state/RCB shape but did NOT check
an arithmetic expected value. Codex therefore added a small independent literal
host-sum oracle BEFORE runtime green: input slots(1.25,-2.5,0.75) and
(-0.5,3.0,1.5) must decrypt to(0.75,0.5,2.25). Fixed absolute tolerance1e-6,
with explicit finite-component guards. A clone-only implementation cannot pass.
No threshold is tuned after output; this is functional accuracy, not a bit claim.
Actual Encrypt/DCP/Add/RCB/Decrypt are used, without an Add evaluation key.

Production remains the existing throwing scaffold. Expected42-test suite:
41 inherited tests pass and only pair_add_runtime_behavior fails at the missing
Add body. Hosted execution PENDING; no Mac compilation or crypto test occurred.
Only after saving genuine red will Codex integrate the reviewed minimal Add body.

Later Sub API/behavior cycles, exact coefficient CRT oracle, three lifecycle
cases, negative matrix, ZCode/Pro/Codex reconciliation and cross-branch merge
remain pending. Do not apply the12 returned patches wholesale or claim their
selected-tree identity is the entire repository. The later RS2 declared-basis
fix and Mult2 are not yet present in this isolated branch.

## Observed runtime red before production edit

Source e22a2e1fb343731ca89cc0ea2e6444e7988bdc5e, run33839675559,
Linux job100919225214 completed the expected runtime red:41/42 passed,
only pair_add_runtime_behavior failed with
pair Add unexpected exception: DoubleCKKS: Add is not implemented.
Warning/default and Relin2/RS2 API builds passed; CTest exit8,0.56s.
Raw sections saved in artifacts/tdd/pair-add-runtime/red-linux.txt before any
production Add edit. Windows job100919225336 is still live; not restarted.
The independent literal expected sum is frozen in the red revision.

Codex has inspected the complete minimal Add patch and existing ValidatePair:
clone corresponding left high/low, direct matching DCRT additions, validated
manifest copy, no key access/rescale/alignment or production catch. Compatible
state checks precede cloning/arithmetic. After this red, the proposed minimal
body and its narrow compatibility check will be integrated; green is PENDING.

## Terminal runtime red/green on both hosts

Red e22a2e1, run33839675559: Windows job100919225336 confirms41/42,
same sole missing Add scaffold failure,1.01s. Warning/default and Relin2/RS2
API targets passed; the later Add API step was skipped after expected red.
Green a7681c2f02fe51dca80c9be51420788db9bde99c, run33839950608:
https://github.com/leemaple/20231788./actions/runs/33839950608
Linux100920033485:42/42,0.34s; Windows100920033616:42/42,0.98s.
Warning/default plus explicit Relin2/RS2/Add API targets all passed. All three
additional project-log sections are retained under artifacts/tdd/pair-add-runtime/.
The exact same literal host-sum test/threshold passed after the minimal Add
implementation; no test was weakened. This is the first Add behavior slice,
not yet the full coefficient/lifecycle/negative matrix or independent final review.
Next owner Codex: Sub API red, scaffold, independent literal subtraction runtime
red/green; then returned exact coefficient/lifecycle/negative coverage.

## Sub API: real compile red retained before the scaffold

Observed 2026-09-04 13:46 CST, source43bbe9d3ef0d4da262772b51a7b3b6a9102a5c14,
run https://github.com/leemaple/20231788./actions/runs/33841224322 completed failure.
Linux job100923761068:42/42 runtime tests,0.49s; Windows100923760882:42/42,1.03s.
Warning/default build, Relin2/RS2 API and Add API steps passed before the explicit
Sub API target failed: Sub is not a member of openfhe_2023_1788::DoubleCKKS.
The complete project configure/build/CTest/API log sections were saved under
artifacts/tdd/pair-sub-api/red-{linux,windows}.txt BEFORE any production Sub edit.

The seven-line public declaration/throwing scaffold is integrated from Pro's
reviewed patch06, retaining the existing std::logic_error classification for
missing greenfield behavior instead of misclassifying it as invalid user input.
The public const signature is unchanged. No subtraction arithmetic, validation
or speculative capability is implemented at this API-only boundary. Hosted API
green is pending. No Mac compilation or crypto execution occurred.

Next runtime slice will use Pro's explicit plaintext vectors(2.25,-1.5,4.0) and
(0.75,2.0,-3.5), and an independent literal expected difference(1.5,-3.5,7.5),
with a frozen functional1e-6 absolute tolerance and finite output guard before
any arithmetic green. The returned shape-only test is insufficient on its own.
Only after actual runtime red is saved may the inspected minimal Sub body land.

## API Linux green and the next runtime red test

Source43f6c469896a7945456d15230e53dd1e03791b04, run33842361373:
Linux100927075374 succeeded with42/42,0.63s, warning/default and all explicit
Relin2/RS2/Add/Sub API targets passing. Retained in pair-sub-api/green-linux.txt.
Windows100927075540 is still genuinely live building pristine OpenFHE; no
cancellation or restart. The workflow has no cancel-in-progress setting.

The runtime Sub test now follows the declared next slice: Pro patch07's public
fixture and state checks, plus the independent literal host difference and
finite checks described above. The unnecessary nested scaffold/invalid-input
catch was omitted; the top-level test harness reports unexpected failures.
No Sub production body was changed. Expected43 tests with only the new runtime
case failing at DoubleCKKS: Sub is not implemented. Actual runtime red pending.

## Sub API both-host green; actual runtime red before arithmetic

API source43f6c46 run33842361373 is now completed success on both platforms:
Windows100927075540:42/42,0.99s; all explicit API targets and warning build passed.
Windows project sections are retained under pair-sub-api/green-windows.txt.

Runtime-red source934a0950790eeb872f2c4ecb22ab37d1ffeaaafa,
run https://github.com/leemaple/20231788./actions/runs/33842680856:
Linux100928002005 completed42/43,0.44s; only pair_sub_runtime_behavior failed
with pair Sub unexpected exception: DoubleCKKS: Sub is not implemented.
The warning/default and Relin2/RS2 API steps passed; later Add/Sub API targets
were skipped after the expected runtime red, not relabelled passed.
Complete project sections were retained in pair-sub-runtime/red-linux.txt before
editing the production body. Windows100928002268 remains live and is not stopped.

After genuine red, Pro patch08's minimal body is now integrated: validate both
pairs and their mutual compatibility, clone corresponding left members, subtract
matching DCRT components, copy the validated manifest and validate the result.
Codex rechecked the pinned official base-leveledshe.cpp:594-617 and
dcrtpoly-impl.h:402-407 subtraction paths. No test/vector/tolerance change,
EvalSub convenience call, rescale, key access, catch or hidden normalization.
Runtime green is pending; no Mac compile or crypto execution occurred.

## Sub arithmetic red-green closed on both hosts

Observed 2026-09-04 14:15 CST. Runtime red934a095 run33842680856:
Windows100928002268 completed42/43,1.23s, with only the unchanged missing-Sub
exception. The project configure/build/test section is retained as
pair-sub-runtime/red-windows.txt. It was not canceled or rerun.

Green c3e68844a213ce68b69da8e56a312c511b4dce7f, run
https://github.com/leemaple/20231788./actions/runs/33842934325:
Linux100928737964:43/43,0.49s; Windows100928738258:43/43,0.99s.
Warning-as-error/default and explicit Relin2/RS2/Add/Sub API builds all passed.
Both exact project log sections are retained under pair-sub-runtime/green-*.
The red revision's literal difference(1.5,-3.5,7.5), finite checks and1e-6
tolerance were unchanged. This closes first Sub behavior, not full precision.

## Controlled coefficient coverage: first execution pending

Next isolated test slice integrates Pro patch09's754-line controlled CRT oracle,
not its later lifecycle/negative matrix or unrelated production code. It exercises
public Add/Sub and RCB on test-owned controlled coefficient fixtures. Thirteen
signed boundary witnesses include zero, both signs, centered half-modulus wraps,
reverse operands, self-add and self-subtract. Expected coefficients come from
independent cpp_int CRT and integer arithmetic, not production DCRT operators.
Each component and native tower is checked, alongside the enumerated snapshots
and metadata provenance. No claim is made for every hidden context/parameter field.

Codex found patch09 calls HasNonzeroValue but does not yet define it; the complete
returned final file defines it later. That small inspected helper is included now
to make this slice self-contained. ExpectedArithmetic's Boost conditional was
replaced with explicit materialized BigInt return branches as preflight advised.
These are static integration corrections, NOT observed compiler-red evidence.
No production arithmetic or existing test/vector/threshold was changed.

This is added regression coverage of already red-green Add/Sub behavior. Its
first hosted result is pending; do not manufacture a missing-behavior red or call
the newly registered44-test suite passed. Real COMPLEX lifecycle coverage and
negative-order/compatibility witnesses remain subsequent independent slices.
No local OpenFHE compilation or cryptographic execution occurred.

## Controlled CRT regression observed on both hosts

Source d4419afcd0818d3b122e91f57f0b1c43da8cbe32, run
https://github.com/leemaple/20231788./actions/runs/33843650508:
Linux100930842605:44/44,0.57s; Windows100930842388:44/44,1.35s.
The newly registered pair_arithmetic_controlled_oracle passed in0.04s and0.33s
respectively. Both warning-as-error/default builds and explicit Relin2/RS2/Add/Sub
API targets passed. Complete project log sections are retained under
artifacts/tdd/pair-controlled-oracle/. ANSI/CR and trailing whitespace only were
normalized; numerical values and actual outcomes are unchanged.

This is the first actual execution of the additional exact coefficient/CRT
coverage, not a new production red-green cycle. It does not cover the unintegrated
public-lifecycle keyless matrix or malformed-operand matrix, genuine canonical
precision, repeated multiplication, or merged later-RS2 fixes. Those remain
explicit next boundaries; no missing coverage is inferred from44 green tests.

## Public lifecycle/keyless regression: first hosted execution pending

Observed2026-09-04. Baseline915e7567cbd409794bd5d3397902ddfb28261d7f was clean.
Codex read the complete Pro patch0010 and inspected the touched current source,
agreed Add/Sub/RCB seams, preflight findings and pinned official plaintext
constructor/context data-type paths. Only this slice is integrated, not the
later malformed-operand matrix or any production change.

The test prepares untouched encrypted real/complex inputs through public DCP,
Tensor2, Relin2 and RS2, then exercises Add/Sub/RCB at ReadyForFirstMult,
ReadyForRS2 and RefreshRequired. It removes only this fixture's EvalMultKey row
AFTER state preparation and leaves an unrelated genuine BV row. Exact integer/
CRT checks cover each high/low component and native coefficient; metadata,
parameter provenance, enumerated context native values and retained key A/B
native values are snapshotted around the arithmetic window. These are specified
fields, not every hidden OpenFHE context/parameter/cache field.

Codex's preflight correction selects explicit COMPLEX for the public lifecycle
context, asserts GetCKKSDataType and checks the literal nonzero0.125 imaginary
component remains in the constructed plaintext cache before encryption.
Official ckkspackedencoding.h:89-102 shows the default REAL constructor would
discard it; cryptocontext.h:1195-1205 permits the real host vector in a complex
context. This is test-fixture correctness, not a claimed upstream defect.
The existing controlled REAL fixture remains REAL. The HasNonzeroValue helper
already integrated with patch09 is kept exactly once, not duplicated.

All prior44 registrations/vectors/thresholds and production Add/Sub code remain
unchanged; one public_lifecycles_keyless entry brings the proposed suite to45.
This is additional regression coverage on the retained genuine Add/Sub red-green
history, not an invented new missing-feature red. First actual hosted result is
pending; no local compilation/crypto, precision, universal theorem, repeated
multiplication or merged-branch acceptance is claimed. Owner Codex: preserve
any actual failure and investigate before changing assertions; otherwise retain
both-host logs, then proceed to negative contracts and independent final review.

## Public lifecycle/keyless regression observed on both hosts

Sourceaf35784e540d556a3af231f8f1a7bd374c49649d, run
https://github.com/leemaple/20231788./actions/runs/33850393475:
Linux10095161767045/45,0.53s; Windows10095161786545/45,1.65s.
The added pair_arithmetic_public_lifecycles_keyless case passed in0.04s and0.34s
respectively. Warning-clean default and explicit Relin2/RS2/Add/Sub API builds
passed on BOTH hosts. Complete project configure/build/CTest/API sections are
retained under artifacts/tdd/pair-public-lifecycles/. Only ANSI/CR/trailing
whitespace were normalized. The committed test SHA256 is
c680b795858edbc914d9fa1b08e7ab813a3d7db4e10a0665308cc87eb65a2034.
This closes the first hosted execution of this additional regression boundary;
no new production behavior, high-precision acceptance, repeated multiplication
or full hidden-state immutability proof is inferred. No Mac crypto/build ran.
