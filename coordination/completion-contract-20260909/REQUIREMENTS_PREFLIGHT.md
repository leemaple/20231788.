# Completion requirement-authority preflight

## Decision

The original fixed S100 E80 FAIL does **not**, by itself, prove that the paper's approximate multiplication algorithm was implemented incorrectly. It does prove that the exact tested implementation/input/key-noise instances did not meet the project's frozen numerical acceptance target. Both statements must survive the next handoff.

The paper supports its algorithm, mathematical premises and the nominal Section 6.3 experiment. It does not supply a universal E80 guarantee for the project's base1015 pressure vector, and it does not make recovery of unpublished HEaaN experiment details a prerequisite for judging an OpenFHE implementation. The user's explicit cancellation of the 1,000-trial requirement also excludes rebuilding it indirectly as a statistical/provenance completion gate.

The newly adopted Ecd result closes the previously selected current-public-polynomial rounding question. No concrete remaining production defect is established by the sources examined here. The immediate unfinished obligation is a **source- and authority-bound acceptance judgment**: specify which supported-domain engineering claim the existing evidence justifies, explicitly preserve the unmet original stress target, and identify only the validation coverage needed for that claim. A Windows run of the already declared S100 annulus contract is a concrete uncovered platform/profile/input cell if that domain is to support a two-platform S100 numerical acceptance claim. It is not automatically necessary merely because some historical artifact is unavailable, and it cannot establish a whole-annulus/all-key guarantee.

This review does not approve a relaxed threshold, redefine the user's objective, adopt annulus as a replacement, authorize a run, or declare the implementation complete.

## Authority and examined sources

Reviewer: separate Codex requirement/adversarial context; requested GPT-6 Astra / high, backend **requested-unverified**. No provider-diversity claim. Used `openfhe-2023-1788-workflow` to preserve clean-room inputs, frozen evidence and observed/inferred/pending distinctions.

Observed HEAD `a7f54de2701a1b9bc02660f66febeff707b56651` in `/Users/lifeng/Documents/20231788-openfhe-ecd-cell-20260909`; initial worktree clean. Only this report was written. No code, test, CI, transform, encryption, sampling, external handoff, browser or Git mutation was performed. Shell operations were read-only source/history inspection and hashes. Existing execution/adoption reports below were inspected, not rerun.

Authority hierarchy for this audit:

1. The user's current instruction and the recorded exact instruction “不需要 1000 次实验，能判断实现的正确就行” are the user authority. The full implementation goal remains.
2. The actual paper determines what its algorithm, conditional theorems and experiments claim. Its empirical result is evidence, not a universal application contract.
3. `coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md` is the root's operative engineering interpretation. Only its quoted sentence is a direct user quotation; its numbered implementation/validation requirements are project-selected operational requirements and should not be misquoted as verbatim user demands.
4. Historical tasks, agent findings, checklists and test definitions are engineering evidence/decisions. They do not acquire higher authority by being repeated in a later handoff. Pre-run tests still retain binding evidentiary meaning; “agent-selected” is not permission to change them after failure.

Principal sources:

- **U1:** `coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md:3–35`; recorded user quote, bounded representative validation, both-platform evidence, preserved vectors/thresholds, explicit exclusion of statistical/performance/universal/security certification gates. Git history attributes the scope document to `a193f592c868a0e978108e9637e99eb25af01b6a` and the subsequent frozen paper contract to `c6a46e0e89828cf46741f5183899038417a3f838`.
- **P1:** actual supplied `PAPER-2023-1788.txt`, read from the verified-input reference at `/Users/lifeng/Documents/20231788-openfhe-parameter-atlas-20260908/artifacts/returns/parameter-atlas-20260908/verified-input/references/paper/`. Lines 201/219 define ties-down nearest Ecd and Dcd; 903–947 describe a conditional local-error theorem, not an E80 endpoint theorem; 1464–1476 identify HEaaN/Linux experiment context; 1562–1590 are Section 6.3/Table 3. This audit used supplied text, not a guessed web quotation or quarantined implementation.
- **E1:** `coordination/paper-scale-integration-01/INPUT_DOMAIN_AUDIT_01.md`, opening and frozen-generator/threshold sections; `PRODUCTION_CONTRACT_01.md`, execution oracle and final paragraph before TDD. These freeze base1015, exact dyadic perturbation, component E80 and high-precision/independent checks before execution. They explicitly call E80 an actual-run target, not an all-Gaussian-key theorem.
- **E2:** `coordination/reproduction-adjudication-20260909/TASK.md:23–30,40–51` and `pro/DECISION.zh-CN.md:75–105`; prior interpretation and retained original/S116/annulus evidence. The prior Ecd “next action” is now historical, not pending.
- **E3:** `coordination/public-s100-ecd-cell-20260909/ADOPTED_RESULT.zh-CN.md`; run 34275429052, source `c5cf80bc31a47cf6e2aa7846d175c8b99d485d75`, all 32768 current-p coefficients certified, zero refuted/unknown, and unchanged historical E80.
- **E4:** `coordination/s100-annulus125-20260908/EXECUTION_CONTRACT.md`, `RESULT.zh-CN.md`, `INDEPENDENT_RESULT_REVIEW.md`; predeclared additive sample, actual Linux result, no Windows execution, unchanged production, conditional observer and source-to-binary provenance limits. `tests/s100_annulus125_eight_square_test.cpp:1–2,56–67,160–184` explicitly makes it additive, defines base999 and retains the evaluator-only eight-square path.

## Which obligations come from where?

| Proposition | Actual authority | Correct disposition |
| --- | --- | --- |
| Implement the t=2 high/low multiplication method on pristine OpenFHE, with correct client/evaluator boundary and meaningful evidence | User goal plus workflow; operation semantics from paper; detailed APIs/tests are project choices | Remains central. Source correspondence alone or low-N-only success is insufficient under U1. |
| Ecd is nearest integer rounding of scaled inverse embedding; Dcd uses the actual scale | Paper notation/Section 2.1, P1:201/219 | Current retained p now has all-coefficient certification (E3). Do not reschedule it or demand a generic formal proof of all Boost arithmetic. |
| Nominal S100 profile, N=2^15, h128, eight no-refresh squarings, Base50x2/Mult60x8/Div40/P60/QP680 | Paper Section 6.3/Table 3, P1:1562–1590 | Appropriate representative paper-scale target. Exact primes, library choices and reproducible key-family plumbing are this project's concrete realization, not values supplied by the paper. |
| Base1015 fixed vector, s/2^75 witness, per-stage/endpoint component max <=2^-80, codec agreement <=2^-120 | Project pre-run E1, not the user quotation or a paper theorem | Preserve the tests and their FAIL. Their scope may be adjudicated explicitly; neither this audit nor a later PASS changes their numerical outcome. |
| Every allowed input and every key/noise sample achieves E80 | No such authority in the inspected user instruction, P1 or E1; E1 explicitly disclaims it | Do not impose this universal guarantee or infer an algorithm bug from one contrary endpoint. |
| Reproduce a 1,000-execution mean, Table 3 timing/storage/NTT figures, exact HEaaN build/input/sampler | Paper reports an experiment; user/U1 remove statistical and performance completion gates | Record unreplicated empirical claims and missing details. Do not demand recovery as a hidden implementation-correctness gate. A future claim of identical empirical replication would require those details. |
| Independent security certification of this OpenFHE parameterization | Paper reports security estimates; U1 explicitly disclaims inferring deployment security | Keep security unestablished and avoid deployment claims. A new independent security proof/estimate is not a correctness-completion prerequisite under U1. |
| Linux and Windows source-bound regression/integration evidence | Operative U1 project requirement, not a Section 6.3 two-platform requirement | Keep existing evidence and identify an actual platform/claim coverage gap. Do not equate missing Windows annulus evidence with lack of all Windows integration evidence. |
| Historical p byte identity with current certified p | Necessary only to transfer this particular finite certificate to the historical run | Do not transfer the certificate without it. Its absence does not invalidate the observed old FAIL or obstruct judging the current implementation on independently retained current evidence. |

Section 6.3 explicitly reports average infinity norms over 1,000 executions (P1:1574–1576), not a maximum over all messages/randomness. Its input-generation formula and exact original library-version/sampler setup are not specified there. The project uses component max for the original E80 test, whereas the annulus result uses max complex modulus; the relation between these norms does not make those contracts identical. Neither successful nor failed project numbers should be presented as a reproduction of the paper's exact statistic.

The paper also leaves t>2 implementation and pair/tuple bootstrapping experimental work open (P1:1594–1607). They should not be introduced as new necessities for the established t=2/eight-square scope. Section 6.2's distinct 18-level refreshing experiment is not an unstated step in Section 6.3's expressly no-refresh computation.

## Concrete conflicts to remove from the next handoff

**C1 — Equivocation between preserving an unmet target and proving a code defect.** E1 explicitly says the E80 target is not a guarantee over all noise. E2's frozen FAIL must remain, but an instruction to “repair the implementation until that FAIL passes” would add a conclusion not supported by the experiment. Correct approximate arithmetic can propagate initial phase error beyond this target. The paper's local error theorem has magnitude/nonwrap hypotheses and an error term; it is not exact multiplication and is not a total endpoint bound of 2^-80 for this vector. Existing source/diagnostic evidence is compatible with correct approximate implementation, although compatibility alone is not proof that no bug exists.

**C2 — Treating every unachieved scientific claim as a user completion gate.** Prior Pro DECISION:101 distinguishes five propositions but says the last three cannot be erased. Read as a reporting obligation, that is sound. Read as “original stress E80, same-source empirical replication, and independent security certification must all be established before correctness completion,” it conflicts with U1:26–35 and lacks direct user authority. The next task must state the distinction explicitly, rather than carry that ambiguity forward. My earlier task preflight accepted preservation of these limitations; it did not establish new authority making all of them mandatory achievements.

**C3 — Promoting a scoped PASS into an input-domain theorem.** E4 proves one Linux input-and-key/noise sample under a predeclared additive contract. It does not prove every |x|<125/128 vector, nor all random samples, nor Windows. E4 itself forbids replacing the original task with an easier domain. Using the PASS as additional evidence for a carefully stated correctness claim is legitimate; silently replacing E1's vector or relabeling its FAIL is not.

**C4 — Reopening completed engineering work to chase provenance unknowns.** E3 closes current-p Ecd for the exact original input. The adopted nonwrap/Relin2 work and existing fresh full-slot analysis should not be rescheduled without a specific missed premise or counterexample. Historical coefficient/binary equivalence and unpublished author initialization restrict transfer/comparison claims; they are not automatically unresolved defects in current code.

## Is documenting the failure legitimate?

Yes. The faithful report is: the retained original pressure test completed and FAILed its unchanged E80 gate on Linux and Windows; no supported production fix has been established; the implementation has the separately enumerated algorithm/scale/ownership/oracle and scoped numerical evidence. This does not relax the test. It does mean the final report cannot claim “all frozen tests pass,” “original S100 E80 fixed,” or “Table 3 empirical result reproduced.”

The reporting judgment and acceptance judgment are separate. Documenting a failure is not by itself enough to accept implementation correctness: source findings, meaningful negative tests, independent numerical checks and representative paper-parameter execution must still meet U1. Conversely, a documented pressure-test limitation need not compel endless new samples, noise changes or unsupported algorithm edits. Any deliberate change to the role of the original target in final acceptance must be explicit, technically justified and traceable to the current authority—not hidden by renaming tests or narrowing inputs after seeing results.

## Minimum unclosed boundary, without inventing a new goal

The next Pro decision should close one finite **completion-contract adjudication**, using the now-complete Ecd result: state whether existing source/negative/regression/end-to-end evidence is sufficient for a bounded implementation-correctness claim, list the exact supported profile/input/operation/numerical scope, and identify a concrete missing engineering validation if not. It must not demand an unspecified HEaaN reconstruction or universal E80 theorem as its answer.

The clearest presently unfilled empirical cell is **Windows execution of the unchanged, already declared S100 `s100-annulus125-e80-v1` contract**, if a matched two-platform S100 E80 demonstration is needed for that final claim. Linux already supplies that sample; S116 supplies a different parameterization on both platforms; original near-unit S100 supplies observed failures on both. A bounded Windows counterpart would investigate a real platform/profile/input-oracle boundary, not retry Linux until its noise happens to pass. It would need the same declared numerical gates, exact source/dependency binding, all-slot independent endpoint checks, same-root family/scale/receipt assertions, and no retry/threshold change. This report does not authorize that execution or predict its result.

Critically, U1 requires Linux/Windows regression/integration evidence generally; it does **not explicitly require every numerical vector on both systems**. Therefore this missing cell is an engineering inference tied to a proposed two-platform S100 claim, not a newly discovered verbatim user demand. Pro should either justify that targeted validation from the final supported-domain claim, or explain why the existing cross-platform and Linux S100 evidence suffices while restricting the untested claim. “Windows annulus PASS alone completes everything” would be another unsupported jump.

Ordinary retained source-to-binary/log/artifact provenance and conditional high-precision observer evidence are admissible engineering assumptions when declared. Missing serialized copies of every internal receipt, a formal proof of every transcendental operation, or historical secret coefficient artifacts are not new mandatory work absent a specific claim that needs them. No identified code change or new noise experiment follows from this audit.

## Draft next-task preflight and packet requirements

I subsequently read the full `coordination/completion-contract-20260909/TASK.md`, SHA256 `5a110bc2fc4aa396db80afed49678190927be3261630d2b68a6c27c9534cbda7`. **Semantically suitable for the next handoff**, conditional on the stated complete packet and ordinary transfer/intake gates. It makes one nonduplicative authority/acceptance decision after materially new Ecd evidence; it does not request another Ecd proof, assume a production repair, automatically schedule Windows, restore 1,000 trials, or authorize altered truth/noise. It explicitly requires an honest final-domain/limitation judgment and a single discriminating next boundary only if necessary. No material task-semantic correction is required by this review.

The source organization correctly warns that historical “latest,” stop arrangements and agent verdicts are not current authority. This matters in the supplied `CHECK_AND_HANDOFF.zh-CN.md`: its 9 September successor should be judged against the current source and Ecd evidence, not its older branch labels/engineering-stop prose. `REPRODUCE.zh-CN.md` correctly identifies its S116 commands as historical, not a current S100 PASS. Neither document should be used to force an unauthorized default-branch change during this review; any final delivery action needs current scope and exact branch/source reconciliation.

Ensure the following exact context is present, reusing inherited verified files rather than making redundant inconsistent copies:

- `coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md`, and `coordination/paper-scale-integration-01/INPUT_DOMAIN_AUDIT_01.md`, `PRODUCTION_CONTRACT_01.md`, `NOMINAL_SCALE_AUDIT_01.md`. These are essential criterion-origin evidence: the first two prevent a reviewer from mistaking the root's base1015/E80 choice for a paper theorem or verbatim user condition.
- The original paper PDF/text; current project source/tests and exact dependency references; current adoption/review evidence for Ecd, cap, nonwrap and Relin2. Existing full context is sufficient; no new third-party source search is needed for this authority audit.
- `coordination/s100-annulus125-20260908/{EXECUTION_CONTRACT.md,RESULT.zh-CN.md,INDEPENDENT_RESULT_REVIEW.md}`; `coordination/annulus-independent-pro-review-20260908/{RETURN_DISPOSITION.md,ROOT_SCALAR_REVIEW.md,pro/REVIEW.md,pro/FIRST_PASS.md}` and its relevant frozen results. This distinguishes the predeclared sample from a post-hoc supported-domain theorem and separates current independent adoption from earlier author language.
- `coordination/precision116-eight-square-return-01/ACCEPTANCE.md` plus its retained hosted/runtime receipts; original two-platform S100 endpoint acceptance/audits; latest regression and negative-coverage dispositions. The S116 acceptance explicitly records 60/60 regression and the numerical PASS on each platform, so the next reviewer must not say Windows integration is wholly absent.
- Current `CHECK_AND_HANDOFF.zh-CN.md`, `REPRODUCE.zh-CN.md`, this preflight and the new task, with clear packet-relative locations and exact hashes. Their historical delivery requirements and dated statements are to be adjudicated, not replayed as instructions.

These are requested packet-presence checks, not a claim that this reviewer built, scanned, verified or uploaded the packet. No additional missing external dependency was found that must be recovered before the bounded Pro decision.

## Checked identities

| Source | SHA256 |
| --- | --- |
| supplied paper text | `60dd871a2769fddfe7ce7b2562d031d7c8d819a679eff3c2b6ebf3d7ea5769ae` |
| current correctness scope | `7c252d3d8693ce98fa4af5f1fe11a00dc9952663262c3e43906710ab2ba273cd` |
| adopted Ecd result | `c1134f928d2e01c09a9d81902a5523061ca6084a158cd9487214eb3cd9836d92` |
| prior adjudication TASK | `f0d29a4103bcd56f5ba2391f08721ec041c4c4e27193e0c48db944479fa9d4fb` |
| prior Pro DECISION | `d23f939b6cc9a333319d6dfd393f3546205d4be7fedd8ae368ea90d04636076a` |
| annulus execution contract | `6f78359e6ff3724a798927541dc9023f189d178aa4d537af43dd68a4aed2d70d` |
| annulus result | `871727b3c6b3de5017e29ea5df736ccd1b700996464fbd963ce4e7004f3d2c80` |
