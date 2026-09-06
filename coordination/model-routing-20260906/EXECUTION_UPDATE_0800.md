# Model allocation: bounded delivery and immediate fallback

Observed 2026-09-06, 07:59–08:00 Asia/Shanghai. This follows the user's renewed instruction to use capability leaderboards and let Codex cover unavailable Fable 5.1. It supplements the earlier [02:26 decision](DECISION.md); historical evidence is unchanged.

At entry, engineering root `/Users/lifeng/Documents/20231788-openfhe-paper-scale-implementation-20260905`, branch `codex/paper-scale-implementation-20260905`, was clean at `b16dfe886b6efcdd56242c9b7af591ac0ea577ed`, tracking the same origin commit. Repository is `leemaple/20231788.` including the final dot. The task-discoverable skill is in the catalog workspace, clean at `16034df` at entry; its no-dot remote is not a publish target. Only skill/coordination documentation is changed here.

## Observed benchmark evidence

The lead reused the existing Ego project space for read-only AA page data and LiveBench visible-table checks. AA's selected embedded `Dataset` score rows were read at 2026-09-05T23:59:12.561Z–23:59:14.597Z. Percentages below are those fractions times 100, rounded to two decimals; the previously archived selected scores are unchanged.

| Exact AA variant | SciCode | HLE | AA-LCR v1.1 | Terminal-Bench v2.1 |
| --- | ---: | ---: | ---: | ---: |
| Claude Fable 5.1 (max with fallback) | 63.08% | 59.13% | 85.33% | 91.39% |
| GPT-6 Astra (max) | 56.48% | 54.68% | 80.67% | 88.39% |
| GPT-5.6 Sol (max) | 57.06% | 49.49% | 84.00% | 88.01% |
| GPT-5.6 Terra (max) | 54.98% | 42.91% | 83.00% | 88.01% |
| GPT-5.6 Luna (max) | 53.59% | 39.48% | 83.67% | 80.90% |

Primary sources: [AA SciCode](https://artificialanalysis.ai/evaluations/scicode), [HLE](https://artificialanalysis.ai/evaluations/humanitys-last-exam), [AA-LCR](https://artificialanalysis.ai/evaluations/artificial-analysis-long-context-reasoning), [AA Terminal-Bench](https://artificialanalysis.ai/evaluations/terminalbench-v2-1). Astra high is 89.89% and medium 89.51% on the same terminal page; effort is not monotonically better, and these small gaps are not established as statistically meaningful. SciCode evaluates scientific coding subproblems, not this OpenFHE/C++ integration.

At 2026-09-06T00:00:18.873Z, the [LiveBench homepage](https://livebench.ai/) visibly selected release **2026-06-25**. Its current max-effort rows showed:

| Visible model row | Reasoning | Mathematics | Coding | Agentic Coding |
| --- | ---: | ---: | ---: | ---: |
| Claude Fable 5.1 Max Effort | 91.7 | 97.0 | 86.4 | 66.1 |
| GPT-6 Astra Max Effort | 92.7 | 96.8 | 80.4 | 57.3 |
| GPT-5.6 Sol Max Effort | 91.7 | 96.2 | 83.9 | 56.2 |
| GPT-5.6 Terra Max Effort | 90.6 | 94.9 | 78.2 | 54.9 |
| GPT-5.6 Luna Max Effort | 85.6 | 87.2 | 82.9 | 48.4 |

These are displayed one-decimal values, not newly recomputed CSV results. The prior raw-data/reproduction note remains [available](../MODEL_BENCHMARK_EVIDENCE_20260905.md). The live [OpenAI model catalog](https://developers.openai.com/api/docs/models) was also opened: its complex-work/balanced/high-volume descriptions inform available-role selection but are provider guidance, not another independent benchmark.

Inference: complementary roles are better supported than assigning every task to the highest composite score. Fable's AA fallback-enabled configuration is not the project's fallback-disabled terminal invocation. Browser `6 Pro`, Codex product names and requested subagent selectors do not attest a matching inference backend, effort or harness. No subscription cost, current Fable balance or exact deployment capability is inferred from public scores.

## Applied roles and operational change

| Responsibility | Owner and fallback |
| --- | --- |
| Critical-path mathematics, missing code, TDD, integration, CI/Git | Codex lead owns now; an available Astra context can independently challenge difficult mathematics. |
| Complex paper decisions and bounded code drafts | ChatGPT Pro remains preferred, with a complete sanitized task per independent conversation and uninterrupted thinking. |
| Difficult scientific-code/precision adversarial consultation | Verified terminal Fable 5.1 when usable; otherwise Codex immediately covers, with a separate available review context. |
| Bounded multi-file development and long-document/spec review | Prefer an available Sol context; Terra is a bounded-engineering fallback. |
| Mechanical extraction, formatting, inventory first pass | Deterministic tools; Luna may assist when independently checkable, never as sole precision sign-off. |
| Actual heavy build/cryptographic verification | GitHub Actions or dedicated Windows. ZCode is optional assistance only after shared-quota and actual-entry checks. |

This revision makes the existing bounded-deliverable instruction actionable: each handoff names one coherent decision/slice, owned paths/interface, dependencies, expected artifact and acceptance test. Dependent observer/serialization/CI pieces share a frozen contract before separation. A terminal partial return is received and triaged, not polled as ongoing thought or replaced with an identical giant submission. Codex owns the verified remaining gap. The author's self-review does not fill an independent review seat.

The latest retained Fable invocation, **September 5 20:08:21–23 CST**, failed with HTTP 403, `账户余额不足`, and no usable inference; no reset was supplied. [Receipt](../paper-final-fable51-01/RECEIPT.md). No recovery signal or new probe occurred in this turn. Operational allocation therefore keeps Fable out of the critical path; its current account balance is not claimed. ZCode was not dispatched or quota-probed for this change.

## Current work handoff, not implementation completion

At 2026-09-06T00:00:18.878Z, a read-only check of [Implement Diagnostic Green Package](https://chatgpt.com/c/6a9c8e43-15cc-83ec-ab14-40a2b4b99981) found one assistant response and no Stop button. Its final answer says **PARTIAL_NOT_GREEN**, claims two test-local Python files, and explicitly leaves all seven C++ helpers plus full-slot observer/evidence/CTest integration unfinished. It does not claim C++ compilation or OpenFHE execution. This is a statement of the observed return, not verification of its downloadable contents.

Codex owns artifact intake and the missing critical-path implementation; separate Standards and Spec contexts own independent review when the exact returned files are available. Pending intake must inspect safe ZIP paths/CRC/manifest/identity/secrets and actual source before accepting or executing anything. The prior ineffective download click is not a completed download. No new Pro submission, restart, interrupt or artifact acceptance occurred in this routing turn.

The next engineering boundary is the smallest testable missing endpoint-helper slice under the existing frozen contract, with independently reviewable evidence serialization kept separate where feasible. Do not rerun completed old audits or start a new full paper chain just to test a Python fragment. Original E80 remains FAIL; endpoint diagnostic GREEN and full-slot numerical attribution remain pending. No source, test, build file, CI workflow or numerical threshold was changed, and no new experiment was run. The user's no-1000-experiment scope and Mac low-load rule remain intact.

## Validation and continuation

Validation results are recorded after execution in [VALIDATION_0800.md](VALIDATION_0800.md). The current engineering and catalog routing references are kept byte-identical while their other skill differences are preserved. The existing continuation heartbeat is updated through the app tool to consume this new ownership/checkpoint; schedule and the complete 07:30 reporting branch are preserved. Today's September 6 report already has a Confirmed delivery-log row and is not recreated or sent again.
