# Model benchmark evidence — 2026-09-05

## Scope, observation and authority

Observed 2026-09-05, 12:03:43–12:09:25 UTC (20:03:43–20:09:25 Asia/Shanghai). This is a bounded, read-only primary-source investigation, not a new model evaluation. The user authorizes evidence-informed role allocation and requires Codex to take over when Fable 5.1 lacks quota. That availability rule is user authority, not a benchmark conclusion. At handoff, the parent reports its separate single terminal attempt returned HTTP 403 for insufficient balance; this researcher did not repeat that attempt or inspect quota.

Repository at start: `codex/three-track-integration-20260905`, HEAD `07d0d1f130b2c9b041a61b97d4546e7058066587`, clean. Only this note is owned by the researcher. No source/skill edits, builds, cryptographic experiments, external-agent dispatches, browser operations or CI runs were performed. The research skill is implemented by this background research task; another nested research worker could not start because the team slot limit was reached.

## Observed: useful primary benchmark evidence

### Artificial Analysis: compare capabilities, not only a composite badge

AA currently labels its Intelligence Index **v4.2**. It combines ten evaluations; category weights are Agents 30%, Coding 20%, Scientific Reasoning 20%, General 30%. Its scientific coding score is unit-tested Python subproblem performance, not a C++ repository integration test. AA-LCR v1.1 tests reasoning across long documents and is not directly comparable with v1.0. For Terminal-Bench v2.1, AA uses 89 tasks, the Terminus 2 harness, pass@1 averaged across three repeats, a 250-episode limit, and a two-hour-or-longer task timeout. These are benchmark settings, not instructions to expand this project's tests. [AA methodology](https://artificialanalysis.ai/methodology/intelligence-benchmarking)

The publicly readable model pages report Fable 5.1 at index 57 and class rank 1/202; its full configuration explicitly includes adaptive reasoning, maximum effort and **default fallback**. Astra at maximum effort reports 55 and class rank 2/202. These are displayed class comparisons, not an assertion that every deployed model or agent was ranked. [Fable 5.1 model page](https://artificialanalysis.ai/models/claude-fable-5-1), [Astra model page](https://artificialanalysis.ai/models/gpt-6-astra)

The following percentages were read from each official evaluation page's embedded `application/ld+json` Dataset named `…: Score`; fractions were multiplied by 100 and rounded to two decimals. Rows are selected project-relevant variants, not an exhaustive leaderboard.

| AA public variant | SciCode | HLE | AA-LCR v1.1 | Terminal-Bench v2.1 |
|---|---:|---:|---:|---:|
| Fable 5.1, max, with fallback | 63.08 | 59.13 | 85.33 | 91.39 |
| GPT-6 Astra, max | 56.48 | 54.68 | 80.67 | 88.39 |
| GPT-5.6 Sol, max | 57.06 | 49.49 | 84.00 | 88.01 |
| GPT-5.6 Terra, max | 54.98 | 42.91 | 83.00 | 88.01 |
| GPT-5.6 Luna, max | 53.59 | 39.48 | 83.67 | 80.90 |

Column sources: [SciCode](https://artificialanalysis.ai/evaluations/scicode), [HLE](https://artificialanalysis.ai/evaluations/humanitys-last-exam), [AA-LCR](https://artificialanalysis.ai/evaluations/artificial-analysis-long-context-reasoning), [Terminal-Bench v2.1](https://artificialanalysis.ai/evaluations/terminalbench-v2-1).

Two important counterexamples to simplistic routing:

- AA-LCR's leading published row is Kimi K3 max, 88.67%; the selected Astra max row is below Sol/Luna on this particular long-document measure. A large advertised context window alone is not evidence of successful document reasoning. [AA-LCR](https://artificialanalysis.ai/evaluations/artificial-analysis-long-context-reasoning)
- Astra **high** scores 89.89% and **medium** 89.51% on AA's Terminal-Bench, versus max 88.39%. Do not assume more effort guarantees a higher score; the page does not establish that these small differences are statistically decisive or transfer to this repository. [Terminal-Bench v2.1](https://artificialanalysis.ai/evaluations/terminalbench-v2-1)

Luna max's AA page lists input/output API prices of $0.20/$1.20 per million tokens and measured output speed 133 tokens/s. This supports investigating it as a cheaper first-pass option; it does **not** establish the marginal price, quota or latency of the user's Codex subscription. [Luna model page](https://artificialanalysis.ai/models/gpt-5-6-luna)

### LiveBench: independent category separation

The current site's JavaScript and owner release constants identify the latest dataset release as **2026-06-25**. This is the question-release label, not the date every subsequently added model was tested. The official CSV fetched today contains 54 model rows. Its category JSON separates Reasoning, Mathematics, Coding, Agentic Coding, Data Analysis, Language and IF. Scores below were independently recomputed from raw subtask columns using the owner's category-average rule; overall is the mean of the seven category means. They are **not** a claim about an unseen UI filter or deployment. [CSV](https://livebench.ai/table_2026_06_25.csv), [categories](https://livebench.ai/categories_2026_06_25.json), [release constants](https://raw.githubusercontent.com/LiveBench/new-livebench/main/src/lib/constants.js), [averaging code](https://raw.githubusercontent.com/LiveBench/new-livebench/main/src/Table/Averaging.js)

| Exact CSV model ID | Overall | Reasoning | Mathematics | Coding | Agentic Coding |
|---|---:|---:|---:|---:|---:|
| `claude-fable-5-1-max-effort` | 83.41 | 91.69 | 97.01 | 86.38 | 66.06 |
| `gpt-6-astra-max` | 82.16 | 92.65 | 96.81 | 80.36 | 57.32 |
| `gpt-5.6-sol-max` | 81.05 | 91.65 | 96.20 | 83.94 | 56.21 |
| `gpt-5.6-terra-max` | 77.94 | 90.63 | 94.91 | 78.25 | 54.95 |
| `gpt-5.6-luna-max` | 73.56 | 85.64 | 87.20 | 82.92 | 48.43 |

Within the 54 fetched rows under that computation, Astra is first on Reasoning; Fable 5.1 is first on Mathematics, Coding, Agentic Coding and overall. Luna's relatively strong short Coding score does not imply equivalent agentic repository performance. These are observed/recomputed aggregate results, not measured OpenFHE aptitude. [Official CSV](https://livebench.ai/table_2026_06_25.csv)

The owner describes objective ground-truth grading and periodic new questions to reduce contamination. Its README contains historical release text, so that older prose is not used as the current release authority. The new leaderboard's model metadata explicitly groups effort variants; public table/model labels alone do not prove API endpoint snapshots or fallback behavior. [LiveBench benchmark README](https://github.com/LiveBench/LiveBench/blob/main/README.md), [leaderboard data conventions](https://github.com/LiveBench/new-livebench)

### SWE-bench: agent/harness and dataset controls matter

The official site defines % Resolved as the share of issue instances solved. Verified has 500 human-filtered instances; its Bash Only view places models in mini-SWE-agent. The original SWE-bench uses Python repositories; Multilingual is a separate 300-instance, nine-language dataset. These are not tests of paper derivations, CKKS precision, or this project's Windows/OpenFHE ABI. [Official SWE-bench](https://www.swebench.com/)

The site's actual `script#leaderboard-data` JSON includes historical Verified entries with exact harness/configuration metadata. For example, the 2026-02-17 mini-SWE-agent 2.0.0 entries report Claude 4.5 Opus high at 76.8% and GPT 5.2 high at 72.8%, both one-attempt labels; their `checked` field is null, so they are not described here as team-checked. These old entries demonstrate why model, effort, harness version, attempt policy and submission date must travel with a score; they do **not** rank today's Astra/Fable 5.1. [Official embedded leaderboard data](https://www.swebench.com/)

The official Terminal-Bench landing/2.1 route was opened, but the text reader did not expose usable leaderboard rows. No ranking is attributed to an unseen Terminal-Bench owner table; the terminal scores above are specifically AA's independently run configuration. [Terminal-Bench owner](https://www.tbench.ai/?version=2.1)

### OpenAI model catalog: provider guidance, not an independent rank

The lead separately opened the current official model catalog and latest-model guide. The catalog identifies `gpt-6-astra` for complex reasoning/coding, `gpt-5.6-terra` for an intelligence/cost balance, and `gpt-5.6-luna` for cost-sensitive high-volume work; it also lists `gpt-5.6-sol`. These are provider role descriptions, not an additional independent benchmark. The current tool catalog exposes these selectable IDs, but does not attest the main session's response model or the user's marginal subscription costs. [OpenAI model catalog](https://developers.openai.com/api/docs/models), [current model guidance](https://developers.openai.com/api/docs/guides/latest-model)

## Identity gaps — do not collapse these distinctions

1. **Public names now exist:** AA and LiveBench explicitly contain Astra and Fable 5.1 labels. It would be inaccurate to say no primary benchmark mentions them. That does not, by itself, attest which backend a particular private CLI alias or app conversation actually used.
2. **Fable's measured fallback differs:** AA's Fable row includes default fallback; this project requires disabled fallback and recorded emitted model identity. Those are different execution configurations. LiveBench's CSV effort ID does not settle its fallback policy. Do not copy a with-fallback score onto a no-fallback CLI result.
3. **Codex/ChatGPT Pro are delivery environments:** a product/subscription name is not a benchmark row. Before claiming correspondence, retain actual emitted model ID/version, reasoning effort, harness/tool environment and fallback status. The runtime catalog name `gpt-6-astra` resembles the public name, but no runtime response attestation or exact effort for the current task was collected in this research.
4. **Independent task versus independent model:** a separate Codex review can be blind and independently performed, but two agents using the same model are not diverse-model validation. ChatGPT Pro on another surface may also use the same underlying family.
5. No evidence here authorizes alias remapping, quota probing, a purchase, a new external dispatch, or relaxed test acceptance.

## Recommended project allocation — inference, not leaderboard fact

| Work boundary | Recommended primary / check | Why and limits |
|---|---|---|
| Paper mathematics, algorithm choice, unresolved invariants | Codex/Astra takes ownership now while Fable is unavailable; Pro remains a fully briefed complex-design consultant; reserve one exact-context, high-effort Fable 5.1 consultation when capacity returns and identity is verified | Astra's independent reasoning/math evidence supports substantive work, not merely scheduling. Fable's cross-benchmark strengths support a targeted scientific-code or mathematical-counterexample review, not a blocking dependency or authority over tests. |
| Long-context paper/source review | Preserve the already-running ChatGPT Pro review; for a new bounded review, prefer an available verified strong long-document variant such as Sol, with Codex checking every finding | AA-LCR differentiates this from composite rank. Do not interrupt/restart a long Pro answer or assume Pro equals an AA model/configuration. |
| Repository implementation and diagnosis | Codex owns the exact worktree, TDD loop and integration; Pro may draft bounded changes from a complete exact-source packet; Sol is a reasonable alternative worker when actually available; separate review before acceptance | AA terminal scores and LiveBench category evidence support agentic capability, but this repository's actual RED/GREEN and dual-host results are the acceptance evidence. Avoid wholesale reassignment based on tiny score gaps. |
| Mechanical CI/log/hash/manifest verification | Codex orchestration plus deterministic parsers; a cheaper agent may extract/check bounded records | Reproducible commands and exact assertions matter more than frontier rank. No model can turn a planned test into an observed pass. |
| Low-cost independent first pass | Verified Luna for narrow naming/spec coverage, missing references, and small diff checks; escalate substantive numeric/lifecycle/API findings to the primary and another reviewer | AA cost and LiveBench Coding/Agentic Coding split support a bounded cheap pass, not sole approval of difficult crypto or architecture. Same-model workers remain task-independent only. |
| Windows/ZCode assistance after quota recovery | Restore bounded supporting work only after the established quota gate verifies availability; record the actual backend model before using benchmark-based role claims | ZCode is an agent product, not a model score. No fresh ZCode availability or backend identity was inspected in this research, and recovery is not a reason to move an already-progressing critical task. |

The allocation should change on meaningful capability evidence, verified availability, or a concrete task failure—not on every leaderboard movement. Fable quota exhaustion triggers immediate Codex takeover; a later reset time alone is not proof of restored capacity. Keep the user's no-1000-trial correctness scope, exact commits, independent oracles, substantive reviews and bounded representative paper-parameter execution unchanged.

## Retrieval and reproducibility notes

- The web text extractor could not render LiveBench's dynamic homepage table, rejected the CSV content type, and exceeded its size limit on the AA all-model leaderboard. Read-only HTTPS GET of the **same public owner resources** was therefore used to parse LiveBench CSV/JSON, AA embedded Dataset JSON and SWE-bench embedded leaderboard JSON in memory. No authenticated browser state was used.
- LiveBench CSV: 9,100 bytes, SHA-256 `f23992acb9c01d4904b8cb2c5714a0e9ab15694e9ee0cdc4a2b525167033ba87`. Category JSON: 725 bytes, SHA-256 `dad300ad18655b69db720e1b88fc5a5eac06c5b2f0e52c2bf50f10ff057674f3`.
- Observed AA HTML hashes: AA-LCR `851a1cb354a3a86746d9f9fc50d8a2fee633f5c3d2f11e949564db6bce74a21c`; Terminal-Bench `dd0eb7a7507d919e5e36be6a22fab2a0402e6e52bd69146ddc610f6e96251a01`; HLE `cde8faca46dbea4ac61c7dfa8a5e8252f6e19ef774dfd3d46fad7d9be0cae89f`. Dynamic pages may change bytes between requests; hashes identify observed responses, not immutable benchmark releases. Raw HTML is not archived by this single-note task.
- Category reproduction: read each row's listed subtask values as decimals, average within each category, then average all seven category means. Preserve model IDs and dataset date; do not merge old/new benchmark versions or assume official API costs equal subscription costs.
