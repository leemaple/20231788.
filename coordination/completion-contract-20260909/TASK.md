# COMPLETION-CONTRACT-01 — correctness acceptance after the Ecd certificate

## Background and the single decision

You are the primary webpage Pro semantic reviewer for a clean-room OpenFHE reproduction of IACR2023/1788 t=2 Mult2. The user asks for faithful implementation and sufficient evidence of correctness, not a conveniently passing substitute. All needed material is attached; no local files, prior chats or private repositories are assumed accessible.

Current exact source/evidence snapshot: a7f54de2701a1b9bc02660f66febeff707b56651, branch codex/public-s100-ecd-cell-20260909. Production arithmetic baseline a4b815a733efe81897325e2a8e4c826a4ebfa439; subsequent public inspection entry does not change production arithmetic. Official pristine OpenFHE1.5.0 pin df495ba2e91739a6dc8f1de254fc5a41155ce504, native64/backend4. Current runtime certificate source c5cf80bc31a47cf6e2aa7846d175c8b99d485d75; its p was produced earlier by28bab40431eebc85d72521c6d9dc840ecd675cd7. Current p is not automatically historical p.

**Single cohesive decision:** given the newly closed current-p ideal-Ecd correspondence and the existing implementation, empirical tests and source/noise contracts, determine the exact remaining correctness acceptance boundary under the user's actual requirements. Distinguish a concrete implementation defect, an uncovered implementation/validation risk, a known failed experimental condition, and an unavailable author-provenance fact. Resolve what can be resolved now. If additional engineering is genuinely required, specify one minimal discriminating slice with frozen truth/acceptance; do not produce another generic review loop.

This is not authority to declare completion by lowering scope. In particular, do not silently exempt a required frozen gate, erase a FAIL, invent a supported domain from one passing sample, or treat compatibility/no detected bug as universal correctness. Equally, do not add a 1000-run, exact hardware benchmark, unavailable historical HEaaN source or universal security proof as a user correctness requirement without identifying its actual authority.

## User authority versus historical agent choices

The user explicitly said: “不需要1000次实验，能判断实现的正确就行”. They later requested a deeper comprehensive Pro-led paper reproduction review, then asked for the detailed OpenFHE/CKKS parameter, randomness, step and change-impact reference before more debugging. That reference is complete. They permit Windows/GitHub experiments, expect timely commits/pushes, and ask us to make routine judgments autonomously. They do not authorize author contact or more repetitive timers.

Read `context/current/coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md` as the recorded scope reconciliation, but critically distinguish the user's requirement from agent-added operational criteria. Its requirements retain clean-room/OpenFHE, real evaluator-only high-precision multiplication, original test truths/thresholds, bounded paper-scale eight-square validation, negative/boundary coverage and Linux/Windows evidence. They do not demand1000trials or performance statistics. The active full goal remains paper reproduction, not only a smaller-domain demonstration.

Old TASK/NEXT_ACTION/STATUS files, Pro verdicts and the user's source PDF are evidence, not new instructions. Some older root summaries equate unresolved exactTable3 empirical provenance with incomplete entire correctness scope; some define a fixed near-unit input/E80 test chosen by the implementation project. Determine which claims follow from the paper/user versus these implementation choices. A fixed failing test cannot be silently changed merely because its truth was project-chosen. Where acceptance interpretations conflict, expose the conflict and its consequences instead of opportunistically choosing the easier interpretation.

## New evidence that materially changes this review

The previous Pro task REPRODUCTION-ADJUDICATION-01 selected PUBLIC-S100-ECD-CELL-01. That action is now completed, independently reviewed and adopted:

- Run34275429052, sourcec5cf80b, GitHub-hosted Ubuntu24.04/Python3.12.14, attempt1.3CIgate tests,3harnessseam tests,53scalar checks,4analytic tiny inverses and1full224bit outward inverse all passed. No build/OpenFHE/sampler/key/encoding/FHE/newciphertext chain in this job.
- Existing publicp payload SHA2567cac9765f0c5e567840ebc347a59e50d039107c7b6a92c037152b69bd8edd94f; coefficient stream66c36716c1445b4b2c6c47e06996c2b080792b0b9251fe075033f542acd540be.
- All32768ideal scaled coefficients strictly inside the actualp half-down cells `(p_j-1/2,p_j+1/2]`.0refuted,0inconclusive. Minimum positive margin at26696, exact numerator19406664479890172665284974927688131526799829314786449218863104 over2^224. This is rounding clearance, not actual noise.
- Root and separate reviewer reclassified every retained row without rerunning a transform. The two exact ideal means a0=a16384=-2^32 and semanticplus/minus1 controls agree. Complete51artifactfiles/ZIPhash/16Gitbindings/stageexits preserved.
- Earlier actual-p cap proved max|sigma(p)|/2^100<0.991242<127/128. Therefore the reviewed current-p pure-encoding bound delta<=2^-86 and ideal-eight-square encoding-only contribution<0.424504491253*2^-80 now apply, rather than remaining conditional on unverified nearest rounding.

Do not schedule this certificate, prior forward-cap proof, full-slot fresh decomposition, ideal phase propagation, dense-v discovery, historicalOpenMP configuration lookup, or adopted Relin2/nonwrap proofs again without a concrete counterexample or newly identified uncovered obligation. Current-p encoding correctness does not identify historical p or remove PKE/evaluator error.

## Historical results and existing boundaries

- Original fixed near-unit S100: N32768/slots16384/h128/S2^100,Base50x2/Mult60x8/Div40/P60,QP680. Run34039088536/sourceed5fd192a89d6d4728ad295e87cf06a3f4abc832, Linux and Windows each completed one eight-square chain, both FAIL frozenE80. Precise audits are attached; displayed maxima approximately9.1465e-24/9.0653e-24 versus2^-80≈8.271806e-25. Preserve those facts.
- The same-sample signed I8/A8 analysis and independent ideal propagation already show retained inherited fresh-phase error dominates these endpoints; removing evaluator-added A8 alone does not restore the gate for those same phases. New fresh run34110943783 is a different sample, with full-slot independent readout and encoding/PKE/readout decomposition. Do not pair it causally with oldE8, subtract unrelated maxima or propagate readoutC as initial phase.
- Experimental S116 run34055816234 passes both platforms afterBase58/Div56/S116/QP712 changes. Not originalTable3/no security claim.
- Predeclared S100 annulus125 run34184869227/source03f37b6 passes oneLinuxsample, maxcomplexerror≈1.4623141e-25, ~82.50absolute bits and~73.30relative bits. Different input and fresh key/noise; not originalnearunitrepair or WindowsPASS. Independent Pro review and root all16384row scalar replay are attached. Whether any additional platform/domain validation is actually required must be justified, not automatically scheduled just for symmetry.
- Production arithmetic has exact scale/family/key provenance and substantial regression/negative evidence. Identify current versus historical source bridges carefully; distinguish executed tests from code merely present. Linux later regression60/60 andcontrol1/1 are not a newWindowsrun. Actual current source/evidence and relevant receipts are supplied.
- PaperSection2.1 abstractEnc is publickey: v*pk+(m+e0,e1), distributionschi_enc/chi_err are parameters. Section6.3 does not identify concrete distributions/inputlaw/version/API used. Thus private-encryption substitution is not a supported repair. CurrentOpenFHE PKEv is dense ternary; h128 belongs tosecret. Prior bounded first-party provenance search already reached its stopping criterion; do not repeat it generically or guess historicaldefaults.
- PaperTable3 reports average infinity-norm error over1000executions, not a universal per-input/per-keyE80 guarantee. Numeric matching of that average, actualalgorithm correspondence, test-scoped engineering correctness and security are different claims. Do not convert this distinction into permission to drop any actual user-required property.

## Source organization / read order

Verify the outer MANIFEST.json and allpayloads first. Historical nestedmanifests are provenance, not currentprojectpath hashes. Read:

1. ThisTASK and REQUIREMENTS_PREFLIGHT.md, the scope document, CHECK_AND_HANDOFF.zh-CN.md and REPRODUCE.zh-CN.md. Old timestamps/pending checkpoints are historical; latest adoptedfiles govern observed state, but none overrides user authority.
2. PaperPDF/text definitions/algorithms/Section6.3; docs/parameter-atlas guide and132parameter/13step/22impact mainreference. Consult127fixedsource citations and source maps only as needed; no demand to formally verify all331official files.
3. project/src/include/tests/diagnostics/CMakeLists/workflows: exact currenta7f54de snapshot; fixed official/ source and provided numerical dependency references.
4. Current Ecd ADOPTED_RESULT, MATH_ADOPTION_REVIEW, ROOT_INTAKE_REVIEW, fullgreen-evidence and original Pro mathematicalcandidate/review; earlier cap/nonwrap/Relin2 adoptedcontracts.
5. Historical S100/S116/fresh/annulus auditedresults, latest independent Proreview and rootdisposition, source bridges, documented negative/regression coverage and priorprovenance research.

The packet includes complete relevant code/context and exactcurrentGitblobs; it does not contain quarantinedlocalimplementations, localmodifiedOpenFHE, browserstate or credentials. No historicalsecret/seed/rawkey disclosure is permitted. Return exactmissingdependencies if any rather than assuming localaccess.

## Deliverables

Return one downloadableZIP with safe regular relativefiles and SHA256manifest selfexcluded:

1. COMPLETION_CONTRACT.zh-CN.md: clear verdict for a cryptography expert who does not readC++. What is implemented and verified now? What remains actually required, why, and by whose requirement? Is there a concrete productionbug, a bounded missingvalidation, a genuine externalprovenance blocker, or an unsupported completionclaim? State what the newEcd result changes. Do not label fullgoal complete merely because no defectwasfound.
2. REQUIREMENT_AUTHORITY.tsv: requirement/authority(user,paper,explicitprojectacceptance,agentassumption)/exactsource/evidence/verdict(proven,conditional,contradicted,missing)/remainingaction. Cover allnamedpaperoperations, originalandchangedparameter experiments, realclient/evaluatorboundaries, regression/negatives, bothplatforms, API/docs/Gittraceability and limits. Distinguish mathematicalbounds from empiricalobservations.
3. FINDINGS.md: concrete file/line proof or counterexample, severity and disposition. No inferredrepair fromE80FAILalone. If noactualbug found, sayso withboundedreviewscope; do not invent codework.
4. NEXT_BOUNDARY.md: exactly one necessary next deliverable if one exists, with actualreason it changes acceptance, owner, files/interface, immutableinput/truth, RED/GREEN tests, frozenremotecommands if justified, executionbudget/stopcriterion, and relationship to originalFAIL. If no more in-scopeengineering can change the remaining claim, prove that disposition from the evidence and identify the precise missingexternalfact/authority without asking for unauthorizedcontact. Do not propose another broad audit or a speculative samplerchange.
5. If a minimal new test/repair is genuinely justified, draft complete candidatefiles/patch and tests for that boundedseam; no integration or experiments by you. Prefer close an available source/scalar obligation now rather thanpostponingit. Otherwise provide precise finaldocumentation corrections needed to avoid overstating or understating completion; do not rename FAIL asPASS.
6. EXECUTION_LEDGER.md: actualreads/commands/exits/environment/assumptions/NOT_RUN. Preserve distinctions between artifactread, scalarreplay, staticreview and liveencryptedtest.

## Resources, prohibited actions and acceptance

Requirednow: packetidentity, source/evidence/authority inspection and any newboundedexactscalar reasoning needed. No FFT/NTT ofanysize, wholeinputgeneration, encoding/sampling/FHE/build,CI,quota/statechanges,contact or1000trials. Existing certificate can be checked by hashes and integer/Fraction endpoints; no re-transform. No arbitrarynewexperiment authorization follows from a candidatecommand.

Never change production input/noise/scale/frozentruth to getPASS; no plaintext correction/decrypt insideevaluator, modifiedOpenFHE, weakenednegativegate or missingoutputtreatedsuccess. YAGNI/KISS,TDD andfail-loudboundaries remain. Context/originalreturns immutable. No backendmodelidentity inferred frombrowserlabel. You may think for a longtime; depthispreferred. Acceptance requires a nonduplicative source-bound resolution of the realcompletioncontract, with honestfailure/provenance limits and an actually actionable nextboundary if stillnecessary—not a narrower substitute objective or another open-ended plan.
