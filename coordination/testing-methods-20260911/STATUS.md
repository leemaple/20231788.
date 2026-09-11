# New user-authorised testing-method diagnosis

## Latest checkpoint, 09:35 CST

The verified 486-member packet was submitted once at 09:26:48.871 CST to
https://chatgpt.com/c/6aa358da-b52c-83ec-9356-f0d8d742a0d9 (`执行诊断测试`).
The latest read-only browser check still shows active Pro thought and the Stop
answering control. Pro has visibly progressed through packet validation,
source/test inspection, and DCRT/NTT implementation inspection. These are UI
progress observations, not an adopted final answer. No stop, refresh, duplicate
prompt or restart was issued.

Commits `de814566a85412910738384a4a01eaa16000a8d9` and
`5a36ecb5d931abf442d4b255320756403370486b` are pushed. The independent coverage
map is complete; root requested a focused second review of its proposed
encoder fixtures. That review caught an **invalid proposed test input**, not
a production bug: N64/S16 can represent the 32 even coefficient lanes, not an
arbitrary dense 64-coefficient vector. The note now preserves this restriction
and requires checking the actually stored near-half input before asserting a
rounding result. This review remains independent of the live Pro conversation.

See `ROOT_RUNNER_PREFLIGHT.md` for target-selection and build-provenance gates.
No candidate test has been adopted, no new C++ test or production change has
been executed, and no CI or automation has been dispatched. Fable and ZCode
remain on the documented availability fallback; their absence does not block
Pro/Codex review. Historical preparation details follow for provenance.

2026-09-11. Branch `codex/testing-methods-diagnosis-20260911` starts at33722b9; originalworktree preservedclean. This is engineeringinvestigation authorised by latestuser request, not a dailyheartbeat. No newtimer, productionchange or CIrun yet.

GitHubskills discovery completed; see SKILL_RESEARCH.md. Existingten-anchor scalarreplay executed, same3exceedances. It is not anewciphertextresult or a newlyidentifiedproductionbug.

Bounded coverage researcher `/root/testing_coverage_gap_map` owns COVERAGE_GAPS.md; requestedgpt-5.6-sol/high, backendunattested. First attempt withhistoryfork failed inthreadstore; retry withcompletefork-noneprompt started successfully. No externalproviderdiversity claim.

Browser taskspace244 created after inventory confirmed noexistingOpenFHEspaces; newProcomposer shows6/Pro, highestPower5of5, slider0..4now4. Not submittedyet. Root preparing exactsource/official/papercontextZIP; no assumption of Pro localaccess. Latestarchivedreview not reopened.

Next: verifiedsanitizedpacket → oneProsubmission → uninterruptedwait plus independentcoverage/checkerwork → root triage/testdraftreview → only then justifiedremoteexperiment. Fable5.1noobservedrecovery; ZCodequotaunavailable atmorningcheck. Neither blocks localorProresearch; no repeatedquotaattempts.
