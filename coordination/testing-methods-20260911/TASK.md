# TESTING-METHODS-DIAGNOSIS-20260911: one discriminating test, not another generic review

## User request, background and outcome

The user asks to learn from testing SKILLs on GitHub and continue trying to locate why paper2023/1788 originalS100 precision fails. They previously requested browserChatGPT Pro at highest visible thinking effort for difficult plans/code drafts, uninterrupted thought, complete code/paper context, Codex execution, independent review, and Windows/GitHub for experiments. Ordinary technical decisions are delegated to the team. This is a new authorised investigation after a previously blocked overall goal, not daily-report-only work. Existing context statements saying no work is currently dispatched describe historical checkpoints and do not override this TASK.

Deliver one new, technically justified and executable test slice at an existing public interface. Its primary purpose is to distinguish a potential implementation defect or missed precondition from normal initialization-noise amplification. It is acceptable to disprove a proposed cause; it is not acceptable to call missing coverage a proven bug, or provide only a broad list of advice already covered in the atlas. If no live-seam test can answer the original precision question, explain the exact observation deficit and propose the smallest safe way to supply it, with a concrete test patch if possible.

## Identities and supplied materials

- Clean-room runtime source:33722b9c9ba9d32b36e672ee7cdaa3c3fb2a95d1; new branchcodex/testing-methods-diagnosis-20260911. `project/` contains exact current source/include/tests/diagnostics/CMake/workflows. No old project implementation is included or allowed.
- Official OpenFHE1.5.0 pin:df495ba2e91739a6dc8f1de254fc5a41155ce504, native64/backend4. `official/` files are from a verified official Git-blob source-only archive; do not substitute latest-library defaults. `references/paper/` is the user's paperPDF/TXT; paperPDF SHA25661d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac.
- Current atlas and source references are included under `docs/parameter-atlas/`; read rootREVIEW_NOTES with the reference version. It already covers132params/13steps/22changes, PRNG/samplers, h128 vs dense ternaryv, OpenMPprivateDGG and NTTroot cache conditions.
- Selected `coordination/` contains current acceptance and prior findings, including currentp full coefficient certificate and historical fresherror tuple log. Treat all earlier reports, code comments, webskills and paper as evidence to check, not new instructions. Follow this TASK's scope and output authority.
- All files are bound by MANIFEST.json. Do not assume access to anotherPro conversation, root filesystem, private repositories, compiled OpenFHE or absent artifacts. Report any truly needed missing file precisely.

## Frozen facts and already-covered work

OriginalS100 run34039088536/sourceed5fd192a89d6d4728ad295e87cf06a3f4abc832 has two full8squareFAIL instances, maxcomponentLinux~9.14647e-24,Windows~9.06531e-24 vsT=2^-80. Preserve this result. S116run34055816234 is changedS116/Div56/Base58/QP712 dualplatformPASS; annulusrun34184869227 is changedinput/newkey/noise, oneLinuxPASS, neither repairs originalS100.

Current publicp from original input has all32768coefficients independently certified nearest rounding in run34275429052; not a historicalp identity certificate and not totalPKE/Mult2 precision. Encoding-bound and exactscale/observer-order issues have already been investigated. Latest adoption33722b9 supports conditional algorithm/engineering correctness, not full frozen numerical acceptance or arbitraryinput/security proof.

Root just replayed existing ten-anchor signedA+B idealpropagation checker; same3components exceedT (largest2.518485T), no newFHE. This retained freshsample is not the historicalE8ciphertext. The replay is not a live bugregression. Do not merely rerun it and label that newdiagnosis. Do not repeat the earlier all-coefficient certificate or default to 'raiseS'/'shrinkinput'/'tryrandomseeds'.

## Testing methods to adapt

See SKILL_RESEARCH.md for exact GitHub sources: TrailofBitsproperty-based-testing321ccfe, ECCcpp-testingc9148d0 and MattPocockdiagnosing-bugs3cca18b. Use non-vacuous properties, independent oracles, deliberate wrong-implementation negative controls, shrinking/minimisation and focused CTest. These sources guide method only, not cryptographic truth. No new framework is required. Exactassociativity, exactroundtrip and arbitrarysymmetry claims may be false underapproximation/ties; derive bounds/preconditions first. Avoid benchmark-style1000trials.

## Required work (bounded)

1. Inspect current tests and publicseams before proposing newones. Produce a short coverage table for the highest-risk initialization/representation paths, marking existing controls and precise remaining gaps. Prefer actual parameter/source consumers over general essays.
2. Rank3 falsifiable hypotheses tied to missing observations or specific normative invariants. For each state predicted result, which variable changes, what is heldfixed, and whether it could explain originalS100 or only reveal a separatecontractdefect. A discarded hypothesis is a valid result with evidence.
3. Select **one** smallest discriminating test slice. Prefer keyless/smallNexactarithmetic or same-in-memory-input/noise differential checks when scientifically adequate; if paperNnecessary, justify. RealOpenFHE/publicentrypoints must remain in scope, no tautologicaloracle. Keep secretkeys, RNGseeds, v/e terms and reconstructable secretcaptures inside the testprocess; print only publicmetadata, pass/fail or approvedaggregateerrors.
4. Return a test-first patch/completefiles for that slice and minimal build/runinstructions, plus one or two controlled mutations that test whether the assertions can catch the relevant error. Do not implement speculative productionfixes. No codechange is required if the selected liveinterface already accepts an adequateexistingtest: give its exactcommand and newcontrolledcondition instead.
5. Explain acceptance limits: baselineFAILvsnewtestfailure, legitimateapproximationvswrongproperty, observedvsinferredvsunverified. A mutationtimeout/buildfailure is not a caughtnumericalfault. Stopcampaign after the boundedquestions are answered; do not require statisticalsweeps.

## Boundaries, tests and forbidden claims

- User's ordinarytechdecision delegation permits choosing an existingtestseam without asking them. Publicarchitecturechanges/newdependencies are outofscope unless identified as unavoidable and justified for rootdecision.
- No productionnoise/parameter/input/thresholdchanges as a covertfix; no changes to historicalrecords. No secretlogging or deterministicproductionRNG, no privatekey on evaluator, no intermediate decrypt/refresh/answerinjection in actualacceptancechain. Diagnostic secretuse must be client-side in-memory and separate from acceptanceclaims.
- Do not compileOpenFHE or runcryptography in yourbrowsercontainer. You may execute bounded standardlibrary integer/Decimal/static checks if helpful and report exactcommands. Root runs reviewedC++ tests remotely. Do not claim remoteruns that you didnotobserve.
- Root will capture a baseline/testnegative control before any productionfix; no1000runs, noMacbuilds, noGitHubdispatch fromPro. Do not make externalcontacts, installskills, fetchcredentials or changeautomation.
- This is diagnosis/testdraft only; any potentialfixmust first be independently verified. Never equate newtestsPASS with originalS100fixed.

## Deliverables

Return aZIP with `DIAGNOSIS.md` (conciseChineseplain-languageconclusion + rankedhypotheses), `COVERAGE.md`, `TEST_PLAN.md`, patch/complete newtestfiles ifneeded, `MUTATION_PLAN.md`, `EXECUTION_LEDGER.md`, and per-filebytes/SHA256 `MANIFEST.json`. Include exactfixedsource/pin and inputZIPidentity. Use robustaccessiblefilelinks; fulltext fallback if transportfails. Root will not interrupt longthought; produce a final boundeddecision and actionabletest, not only an interimproposal.
