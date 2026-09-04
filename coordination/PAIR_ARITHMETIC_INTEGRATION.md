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
