# Task-based model routing

Apply this policy when assigning work, selecting a reviewer, or recovering from unavailable models. Use the user's latest instructions over an older task brief or allocation.

## Choose by task, then availability

Consult the dated [benchmark evidence](https://github.com/leemaple/20231788./blob/codex/three-track-integration-20260905/coordination/MODEL_BENCHMARK_EVIDENCE_20260905.md) (local repository path: `coordination/MODEL_BENCHMARK_EVIDENCE_20260905.md`) when changing the allocation. Match the exact model, reasoning effort, evaluation version, agent harness, and fallback setting. Treat leaderboard results as evidence for a role preference, not proof of this implementation's correctness. A ChatGPT subscription or visible "Pro" label is not an API model identifier; record the actual UI label without assigning it another model's score.

Recheck relevant primary results at a new major phase or when a newly available model could change a material decision. A daily ranking refresh is not a prerequisite to engineering. Prefer project-proven performance on the exact task when rankings are mixed or settings are not comparable.

## Allocation

| Work | Preferred owner | Practical boundary / fallback |
| --- | --- | --- |
| Critical-path execution, source integration, TDD, failure diagnosis, CI and Git | Codex lead | Own the result now. For a new difficult independent Codex subtask, prefer an available, explicitly selected GPT-6 Astra; record the actual model instead of assuming the lead session's identity. |
| Paper equations, scale/precision contracts, bounded complex code drafts, independent semantic final review | ChatGPT Pro in Ego Lite | Preserve the user's preference for Pro drafting. Give one complete sanitized task per independent conversation. Continue nonoverlapping execution while it thinks; Codex writes needed critical-path code when availability or response latency would otherwise block progress. |
| Difficult scientific code, competing mathematical conclusions, adversarial review of a high-risk boundary | Verified Fable 5.1 through the terminal | Consult promptly after targeted source/test investigation. If unavailable, Codex immediately owns the same question and asks an independent available reviewer to challenge it; do not wait for balance recovery. |
| Bounded repository work, multi-file/document review, test inventory and an additional implementation/review perspective | Available Codex subagent; GPT-5.6 Sol is a candidate for substantial bounded work and long-document review | Use the actual selectable model and task evidence. Terra/Luna are candidates for simpler extraction, report formatting or checks with deterministic acceptance; not sole sign-off for precision mathematics. Public API price is not this subscription's marginal cost. Do not create extra tasks merely to use every model. |
| Windows execution and bounded auxiliary implementation/review | Windows ZCode/Zima when its identity, entry point and shared quota are verified | Reuse the shared-quota gate in [external-collaboration.md](external-collaboration.md). Restore it for appropriate subsequent work after observed recovery, without interrupting a fallback already making progress. |
| Builds, cryptographic tests, hashes, exact-source/log reconciliation | GitHub Actions / Windows and deterministic tools | Use actual test receipts as the correctness evidence. An agent opinion, benchmark score or "three models agree" is not a passing test. No sustained Mac builds. |

The split is a workload-based inference, not a claim that each owner wins every relevant benchmark. Fable's scientific-code evidence supports a targeted consultation role; Pro is retained for the requested drafting/review workflow and observed project contributions, without treating its browser harness as an independently benchmarked API configuration.

## Availability and review independence

For Fable, select a provider-advertised exact 5.1 identifier or the CLI's latest alias, disable fallback, and accept a review only when the emitted inference identity is verified as 5.1 and a usable answer exists. Use the terminal, never the browser. Record the requested model separately from the returned model. An init line alone, synthetic error message, empty usage, authentication failure or quota error is not a completed review.

On a definitive balance/quota failure, record the time and exact visible outcome once, preserve the brief and verified bundle, and continue immediately under Codex ownership. Do not repeatedly probe the same unavailable account. Retry only after an observed recovery signal or displayed reset, rechecking capacity at the next useful task boundary. If no reset is supplied, record "unknown" rather than inventing one. Do not silently substitute Fable 5 or another model and label it 5.1.

For substantive code, retain three review responsibilities: Codex integration/standards, Pro semantic/paper review, and an additional adversarial review normally from Fable 5.1 or quota-available ZCode. When a reviewer is unavailable, assign its responsibility to a separate available Codex review context; preserve independent first-pass reasoning and report the reduction in provider diversity honestly. Two Codex contexts are not two different model providers. Do not hold an otherwise evidenced boundary open solely for unavailable model access, and do not dismiss unresolved correctness findings as an availability problem.

External reviewers receive a complete, sanitized, exact-commit bundle under [external-collaboration.md](external-collaboration.md). Save actual model/UI identity, task link or invocation receipt, scope, status, findings and disposition. Keep Pro thinking uninterrupted: no stop, duplicate submission, destructive refresh or quota-driven reassignment of its active response.

## Acceptance and continuity

Use sufficient discriminating tests and an independent oracle for the agreed paper-scale correctness boundary. The user removed a 1,000-experiment requirement: do not restore it for statistics, benchmarking or model comparison. Keep frozen numerical limits and paper-parameter checks; change a disputed criterion only with recorded technical reasoning before the relevant run.

A completed role assignment names the current owner and fallback, preserves the pending task's brief and latest completed point, and updates the existing continuation automation when its saved routing is stale. Routine allocation and fallback decisions are autonomous; ask the user only for genuinely new authority or a material scope choice. No new recurring job is needed just to poll rankings or balances.
