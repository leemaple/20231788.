# Pair arithmetic result -> first Mult2: Pro return and staged hosted acceptance

## Returned artifact and byte integrity
Conversation title: 实现 Pair 加减补丁
https://chatgpt.com/c/6a9a4269-665c-83ec-b130-8e40fd86f2d7
Original task sent17:52:24 Asia/Shanghai2026-09-04; completion/download observed
by18:41, complete return independently inspected19:44-19:50.
Browser UI label Pro; returned metadata self-identifies GPT-5.6 Pro.
That metadata is not an independent routing-identity attestation.

ZIP /Users/lifeng/Downloads/pair-mult2-composition-d73824c-test-only-candidate.zip
Bytes54602, SHA256
aeacdbf5c5e66b873664e72bbd75391ab379226b37e8e4714e902f14f5dd3294.
CRC, unique safe relative names, no symlink/encryption and18 nonempty required
regular files PASS. MANIFEST.sha256 independently verifies all17 non-self
payloads; FILE-INVENTORY independently matches16 size/hash records, excluding
itself and that manifest as documented. Gitleaks8.30.1 extracted return scan
223841bytes, zero findings. Original18 files archived BYTE-EXACT under
coordination/returns/pair-mult2-composition-pro-d73824c/.
Scratch extraction /private/tmp/pair-mult2-composition-return.q7r0ZF.
No old implementation, modified OpenFHE or credentials were read or adopted.

Main independently read the full565-line design, execution ledger, complete new
code and both patch stages. Original production/public-header/CMake/test source
matches tested d73824c2d382013c3aadbd7cb29c57008e839714; current documentation
baseline8a465764044d8b1e1578f462ea4916f7123428a4 remains clean/pushed.
The confirmed seams in coordination/TEST_SEAMS.md include public Pair Add/Sub,
Mult2 and RCB; no new private seam is introduced. No tracked AGENTS.md or
CONTEXT.md is present in this source root.

## Independent Codex review and static checks
The genuine gap is composition: earlier tests check Pair Add/Sub and Mult2
separately but not an Add/Sub RESULT as a first-Mult2 operand.
The new input coefficient oracle decrypts ORIGINAL operand pairs independently
and compares centered exact BigInt sum/difference with the public composed
result; it does not use public RCB/production Add/Sub as its expected answer.
The product certificate is the existing independent integer-negacyclic/per-path
certificate. Staged/direct equality is wiring evidence ONLY. Final functional
slots use separately frozen dyadic literals with unchanged1e-3 tolerance.
COMPLEX is explicitly configured, and three pre-encryption imaginary witnesses
are checked. This ordinary fixture is unrelated to the precision test's stale
injected placeholder and may read its own normal packed-value cache.
All original inputs/pairs plus the composed pair are snapshotted at the relevant
window, first-input metadata checked, and final lifecycle remains RefreshRequired.
Evaluation-key assertions establish row PRESENCE only, not deep key immutability;
the return explicitly disclaims hidden-cache/future-mutation/concurrency proof.
No production defect or need to weaken guards/thresholds was found statically.

Codex independently converted the frozen exact dyadic values to BigInt multiples
of2^48, checked32 complex sum/difference/product identities and112 scalar literal
comparisons across all7 final C++ arrays. The5 arrays actually used by stage1
also match. All unit-L1 envelopes hold: A3/8,B3/16,C3/4,Add7/32,Sub9/16.
Record: coordination/evidence/pair-mult2-composition/codex-host-vectors.json.
This is small host arithmetic, not compiled C++/OpenFHE or cryptographic evidence.
For slot0,(A+B)*C=(-11/256,1/32), (A-B)*C=(-1/256,1/32), whereas droppingB
gives(-3/128,1/32); these exceed the functional tolerance by useful margins.
Tiny slots alone are not precision evidence.

## Stage1 frozen before first hosted observation
New isolated worktree/branch:
/Users/lifeng/Documents/20231788-openfhe-codex-pair-mult2-composition-01
codex/pair-mult2-composition-01, created from clean8a465764.
Raw patch0001 applies cleanly with git apply --check, then transferred through
apply_patch; actual stage1 files exactly match returned sizes/hashes:
CMakeLists.txt11269bytes,
05bacc8ce51ab6514da67f17a94dce68cdbc4db91613da1b58c654d36adce115;
tests/mult2_e2e_oracle_test.cpp73356bytes,
ba71c84b55d1fda70fce6434542ca6845912cc4b1941628eaaff659954e126cb.
Original patch0001 SHA
0fde6df49bb9c82a7353c11e5382cdc67b99b3a4733a08dad6b2acfece9492a0.
Only deleted old test line is the usage string needed to append the selector.
No original assertion/vector/tolerance/backend/body has been removed or changed.
All53 old CMake name/command bindings independently match in original order.
Exactly one new54th binding:
mult2_pair_add_input_hybrid_complex ->
mult2_e2e_oracle_test pair_add_input_hybrid_complex.
Production/header remain byte-identical to baseline.

Codex CI-only additions: one branch-admission line and explicit focused Add
CTest before the unchanged full CTest on each host,13 additions total.
Jobs, pin, warnings-as-errors, resource caps, platform setup and5 API targets
remain unchanged. Workflow SHA
e5537f73a0a4814c64a83eb74d9fc3164527b05ff9a6cd9810a879af1c88bf2d.
This workflow delta is NOT attributed to the Pro patch.
No Mac compilation/cryptographic run occurred.

The full staged whitespace check flags only original archival README Markdown
hard-break spaces and the three raw patch files' blank context/EOF lines.
Those exact four evidence files retain their original hashes; no archival
whitespace is silently rewritten. The scoped staged check excludes only those
four paths and still checks every active source/build/workflow and other record.

Stage2 is ARCHIVED ONLY. Raw0002 applicability against current stage1 was
checked without applying it. No Sub-input test is active yet, and55-test counts
in the original return are prospective. Adopt stage2 only after actual stage1
focused/full results, preserving any genuine failure before diagnosis.
This is additional regression coverage at already implemented behaviors.
If the first run passes, label FIRST-OBSERVED GREEN; never break production to
manufacture red. Original Add/Sub/Mult2 feature TDD history remains retained.

## Remaining acceptance and owners
Codex: push exact frozen stage1, observe hosted Linux/Windows warnings, focused
Add and full54 suite/API steps; preserve full logs and match actual names.
Then apply independently reviewed stage2 unchanged and repeat focused/full55.
Final ZCode/reviewer inspection of this actual new composition patch remains
pending; prior review identified the gap, not approval of this new code.
No merge into integration or full project completion is claimed.
The isolated BV final review and Pro high-precision first-Mult2 task remain live.
This p30,N64,HEStd_NotSet,1e-3 diagnostic is NOT high precision, repeated
multiplication, universal BV error proof, paper-scale security/performance or
the complete paper reproduction.

## Stage1 first observation: Linux GREEN; Windows still live
Frozen/pushed source cdc4711840c2d6bb7c390e8597d730ae335ee34e,
remote SHA verified. Run33869933158:
https://github.com/leemaple/20231788./actions/runs/33869933158
Linux job101013240028 completed SUCCESS at19:53:56 Asia/Shanghai.
Default warnings-as-errors, all5 explicit API builds and both CTest steps pass.
Focused Add:1/1 (case0.05s, total0.52s); full suite54/54,total0.51s.
Codex independently matched all54 unique actual names against current CMake;
the new Add case produced distinct certificate records in focused and full runs.
Original53 names preserved; this is FIRST-OBSERVED GREEN, not fabricated RED.
Full project configure/build/focused/full/API log retained as
artifacts/tdd/pair-mult2-composition/add-linux.txt,58414bytes,
SHA256 f09918a5b810ccaa00bb8dea1fb2490c45e6c0b66a2c2eebc427ec68e60ab34d.
Only ANSI/CR/trailing whitespace normalized in this new retained section.
Job identity/head/run and API-step outcomes are retained in
coordination/evidence/pair-mult2-composition/add-linux-cdc4711.json.
Windows job101013240269 remained live on this same source when observed;
no Windows result is inferred. Stage2 remains unapplied until that outcome.

## Stage1 GREEN confirmed on both platforms
Run33869933158 completed SUCCESS at the exact frozen cdc4711840c2d6bb7c390e8597d730ae335ee34e.
Windows job101013240269 completed20:00:01 Asia/Shanghai2026-09-04.
Its default warning build, all5 explicit API targets and both CTest steps PASS.
Focused Add1/1 (case0.09s,total0.10s), full54/54,total2.19s.
Actual54 unique names independently match unchanged current CMake; two genuine
Add certificates are present for its separate focused/full executions.
Retained full project configure/build/focused/full/API section:
artifacts/tdd/pair-mult2-composition/add-windows.txt,62048bytes,
SHA256 6dd616701d0ff0f5c4f0f7b6024b553347238f43f975591d64f7634bad7f7d0e.
Only ANSI/CR/trailing whitespace normalized. Metadata and records are retained
in coordination/evidence/pair-mult2-composition/add-windows-cdc4711.json.
The original Pro stage1 CMake/test hashes remain unchanged. No first-outcome
failure, threshold tuning, production patch or fake red was introduced.
This satisfies the stage1 execution boundary; the original archived stage2 may
now be applied unchanged, then frozen and observed separately. It does not
establish the still-unapplied Sub composition or high-precision/repeated use.

## Stage2 frozen before first hosted observation
After both stage1 jobs completed, Codex rechecked clean branch HEAD
2661ae3d8e279a3b4d67da5b6ed5648df9e062f7 and applied the original archived
0002 patch through apply_patch. Its SHA remains
3e34245127c8f8dbffbb3e6b680bf49e7fd04e027adaabcab822ea526785c861.
Active CMakeLists.txt and tests/mult2_e2e_oracle_test.cpp are byte-identical
to the Pro return's final-project files, confirmed with cmp and SHA256:
CMake11389bytes bf27cc6e2514049019671bbc8bc2461e65e0b63431dddd3a1b0194fa020180fa;
oracle75003bytes 7522b2e3aaf3f2730e52ae7fe0a45c77517bb45f973dc4eca6ed65da59e7be8d.

Exactly one additional CTest binding is active:
mult2_pair_sub_input_hybrid_complex ->
mult2_e2e_oracle_test pair_sub_input_hybrid_complex.
All54 prior name/command bindings are preserved in order, giving55 total.
The only code/build edits in this stage are that binding and the original
Pro Sub case: explicit public Sub dispatch, materialized BigInt difference
oracle, frozen difference/product literals and selector. The independently
verified7 final arrays/112 scalar literals remain unchanged.
No production/public-header changes, tolerance weakening, new private seam,
exception suppression or hypothetical repeated-use path were introduced.

Codex changed only the two existing focused CI step names and regexes to
select BOTH Add and Sub. Full CTest,5 API builds, warnings, pinned OpenFHE,
resource caps and platform setup are unchanged. Workflow SHA256:
f8549f97bff475990f19a5172af5a2f20412f6f12b71b356ebd3a828ac115511.
git diff --check and independent static CTest/changed-file closure PASS.
These are static checks, not a compile or runtime observation.

This is the frozen pre-execution state on2026-09-04. First hosted results
remain PENDING; preserve a real failure if observed, or label a first pass
FIRST-OBSERVED GREEN. Do not manufacture a production defect for a regression.
Required next evidence: focused2/2 and full55/55 on Linux and Windows,
warnings-as-errors and all5 explicit API builds, exact head/run/job metadata,
retained logs and independent actual-test-name closure. Final independent
review of the actual composition patch and integration remain pending.
This functional diagnostic does not replace the still-live first-Mult2
high-precision task or the complete paper parameter/repeated-use destination.

## Stage2 first-observed GREEN on both platforms
Frozen/pushed source b48b54e22f14bbfe988a6890f1b03eac9efb11a3, remote exact
SHA verified. Run33871723090:
https://github.com/leemaple/20231788./actions/runs/33871723090
Linux job101019032191 completed20:17:07 Asia/Shanghai2026-09-04:
focused Add/Sub2/2,total0.43s; full55/55,total1.06s.
Windows job101019032537 completed20:21:00:
focused Add/Sub2/2,total0.15s; full55/55,total1.68s.
Both warning-clean default builds and all5 explicit API builds passed.
Actual55 unique name sequences match current CMake in order on both hosts.
Each host has2 focused+2 full composition records, with real fresh fixture
keys. No tolerance/vector/production change followed an observed failure.
The final active CMake/test still exactly match the original Pro final copies.

Full configure/build/focused/full/API logs and exact job metadata are retained:
artifacts/tdd/pair-mult2-composition/add-sub-linux.txt,63386bytes,
SHA256 b4ea478c185fbd3f10f66b75425d897380a44066d5f42907ce7a9f958115cc02;
add-sub-windows.txt,67039bytes,
SHA256 fcd4fc79f708e8aceb20c1e6c97770a5e415ddee5b986e49f0e5335fe9e33148.
Corresponding JSON is in coordination/evidence/pair-mult2-composition/.
Only ANSI/CR/trailing spaces are normalized in these new retained log sections.
This closes staged Add/Sub-to-first-Mult2 hosted regression coverage.
Final independent review of this actual composition patch and integration
remain pending; functional1e-3,N64 coverage is not high-precision/paper proof.
