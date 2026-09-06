# Task-based model routing

Apply this policy when assigning work, selecting a reviewer, or recovering from unavailable models. Use the user's latest instructions over an older task brief or allocation.

## Choose by task, then availability

Consult the local dated evidence at `coordination/MODEL_BENCHMARK_EVIDENCE_20260905.md` when changing the allocation; the [commit-pinned copy](https://github.com/leemaple/20231788./blob/688b3c406e268994efcc58ecb17faf9c611bf5bb/coordination/MODEL_BENCHMARK_EVIDENCE_20260905.md) is available when the local note is absent. Match the exact model, reasoning effort, evaluation version, agent harness, and fallback setting. Treat leaderboard results as evidence for a role preference, not proof of this implementation's correctness. A ChatGPT subscription or visible "Pro" label is not an API model identifier; record the actual UI label without assigning it another model's score.

Choose the relevant measures before comparing models: mathematical derivation and counterexamples use Reasoning/Mathematics and HLE; scientific code uses SciCode; paper/source synthesis uses AA-LCR; repository execution uses Terminal-Bench and Agentic Coding. Short coding scores do not establish multi-step repository reliability. Use the composite index only as secondary context, and avoid treating small score gaps or higher effort as guaranteed improvements. Deterministic checks are selected by reproducible acceptance, not by a model rank.

Recheck relevant primary results at a new major phase or when a newly available model could change a material decision. A daily ranking refresh is not a prerequisite to engineering. Prefer project-proven performance on the exact task when rankings are mixed or settings are not comparable.

## Allocation

| Work | Preferred owner | Practical boundary / fallback |
| --- | --- | --- |
| Critical-path execution, source integration, TDD, failure diagnosis, CI and Git | Codex lead | Own the result now. For a new difficult independent Codex subtask, prefer an available, explicitly selected GPT-6 Astra; record the actual model instead of assuming the lead session's identity. |
| Paper equations, scale/precision contracts, bounded complex code drafts, independent semantic final review | ChatGPT Pro in Ego Lite | Preserve the user's preference for Pro drafting. Give one complete sanitized task per independent conversation. Continue nonoverlapping execution while it thinks; Codex owns unassigned work and gaps released by a terminal return or unavailable author. |
| Difficult scientific code, competing mathematical conclusions, adversarial review of a high-risk boundary | Verified Fable 5.1 through the terminal | Consult promptly after targeted source/test investigation. If unavailable, Codex immediately owns the same question and asks an independent available reviewer to challenge it; do not wait for balance recovery. |
| Bounded repository work, multi-file/document review, test inventory and an additional implementation/review perspective | Prefer an available GPT-5.6 Sol subagent for substantial bounded work and long-document review | Keep an already effective owner. Terra is a balanced fallback for bounded engineering; Luna handles narrow extraction, formatting or deterministic-checkable first passes, not sole sign-off for precision mathematics. Public API price is not this subscription's marginal cost. Do not create extra tasks merely to use every model. |
| Windows execution and bounded auxiliary implementation/review | Windows ZCode/Zima when its identity, entry point and shared quota are verified | Reuse the shared-quota gate in [external-collaboration.md](external-collaboration.md). Restore it for appropriate subsequent work after observed recovery, without interrupting a fallback already making progress. |
| Builds, cryptographic tests, hashes, exact-source/log reconciliation | GitHub Actions / Windows and deterministic tools | Use actual test receipts as the correctness evidence. An agent opinion, benchmark score or "three models agree" is not a passing test. No sustained Mac builds. |

The split is a workload-based inference, not a claim that each owner wins every relevant benchmark. Fable's scientific-code evidence supports a targeted consultation role; Pro is retained for the requested drafting/review workflow and observed project contributions, without treating its browser harness as an independently benchmarked API configuration. Select only actually available workers; a leaderboard entry is not access authorization or a reason to purchase another service.

## Dispatch without duplicating work

Before dispatch, check active and completed work in the coordination ledger: exact scope/commit, owner, latest completed checkpoint and remaining deliverable. Assign a nonoverlapping deliverable; deliberate blind or adversarial review of the same boundary is allowed when recorded as independent review, not a second implementation. Reuse completed evidence instead of redoing work because the preferred model changed.

Record the requested selector/effort separately from the observed model identity, tool environment, fallback setting and availability for every model. Mark an unattested backend `requested-unverified`; a tool's configured model or a browser label alone cannot inherit an exact leaderboard score. This limits capability claims, not the ability to review a useful artifact against source and tests.

## Bound the deliverable and continue from returns

Before a complex handoff, choose one mathematical decision or one cohesive, independently checkable implementation slice. Name its owned files/interface, dependencies, exact deliverable and test/acceptance boundary. Split independent numerical helpers, evidence serialization and CI wiring only when their shared contracts can be fixed first; keep tightly coupled changes together. The complete-context requirement remains in force for every separate conversation. Do not reduce the final correctness scope to fit a smaller task.

Leave active Pro thinking uninterrupted and advance nonoverlapping work. A terminal partial or blocked answer is a return to triage, not continuing thought: retain its artifacts, verify any usable portion, record the missing deliverables, and give the critical-path gap to Codex with an independent reviewer. Continue from the latest verified point rather than resending the same whole package. Treat a download/transport problem separately from an algorithmic failure; no successful download or test is inferred from a displayed claim.

Judge role effectiveness by accepted, exact-source deliverables and reproducible tests on this project. A partial return can justify narrowing the next assignment, but does not establish a model-wide inability. Review the changed boundary and its affected dependencies; changing an owner does not reopen completed evidence by itself.

## Availability and review independence

For Fable, select a provider-advertised exact 5.1 identifier or the CLI's latest alias, disable fallback, and accept a review only when the emitted inference identity is verified as 5.1 and a usable answer exists. Use the terminal, never the browser. Record the requested model separately from the returned model. An init line alone, synthetic error message, empty usage, authentication failure or quota error is not a completed review.

On a definitive balance/quota failure from Fable or another worker, record the time and exact visible outcome once, preserve the brief, verified bundle and last completed checkpoint, and continue immediately under Codex ownership with an independent available review context. Do not repeatedly probe the same unavailable account. Retry only after an observed recovery signal or displayed reset, rechecking capacity at the next useful task boundary. Restore recovered models for subsequent work without moving an already progressing fallback. If no reset is supplied, record "unknown" rather than inventing one. Do not silently substitute Fable 5 or another model and label it 5.1.

For substantive code, retain three review responsibilities: Codex integration/standards, Pro semantic/paper review, and an additional adversarial review normally from Fable 5.1 or quota-available ZCode. When a reviewer is unavailable, assign its responsibility to a separate available Codex review context; preserve independent first-pass reasoning and report the reduction in provider diversity honestly. Two Codex contexts are not two different model providers. Do not hold an otherwise evidenced boundary open solely for unavailable model access, and do not dismiss unresolved correctness findings as an availability problem.

External reviewers receive a complete, sanitized, exact-commit bundle under [external-collaboration.md](external-collaboration.md). Save actual model/UI identity, task link or invocation receipt, scope, status, findings and disposition. An author's own review is not the independent semantic or adversarial seat: give that seat a separate context and require its first-pass judgment from source/spec/tests before showing the author's verdict. Keep Pro thinking uninterrupted: no stop, duplicate submission, destructive refresh or quota-driven reassignment of its active response.

## Acceptance and continuity

Use sufficient discriminating tests and an independent oracle for the agreed paper-scale correctness boundary. The user removed a 1,000-experiment requirement: do not restore it for statistics, benchmarking or model comparison. Keep frozen numerical limits and paper-parameter checks; change a disputed criterion only with recorded technical reasoning before the relevant run.

A completed role assignment names the current owner and fallback, preserves the pending task's brief and latest completed point, and updates the existing continuation automation when its saved routing is stale. Routine allocation and fallback decisions are autonomous; ask the user only for genuinely new authority or a material scope choice. No new recurring job is needed just to poll rankings or balances.
