# Model routing refinement — 2026-09-06

User request: allocate collaboration by measured task strengths, allow a project-skill update, and have Codex cover Fable 5.1 when quota is unavailable. This refines the existing September 5 policy; it does not restart the implementation or reopen completed reviews.

## Observed evidence

At 2026-09-06 02:26:56–02:27:49 Asia/Shanghai, the lead used the existing Ego project task space for read-only public-source checks. The selected AA rows and LiveBench CSV/category hashes match the September 5 note. Exact returned AA values, URLs, timestamps and recomputed LiveBench category means are in [BENCHMARK_RECHECK.json](BENCHMARK_RECHECK.json). No model benchmark was executed.

| AA exact displayed variant | SciCode | AA-LCR v1.1 | Terminal-Bench v2.1 |
| --- | ---: | ---: | ---: |
| Fable 5.1, max with fallback | 63.08% | 85.33% | 91.39% |
| GPT-6 Astra, max | 56.48% | 80.67% | 88.39% |
| GPT-5.6 Sol, max | 57.06% | 84.00% | 88.01% |
| GPT-5.6 Terra, max | 54.98% | 83.00% | 88.01% |
| GPT-5.6 Luna, max | 53.59% | 83.67% | 80.90% |

Sources: [AA SciCode](https://artificialanalysis.ai/evaluations/scicode), [AA-LCR](https://artificialanalysis.ai/evaluations/artificial-analysis-long-context-reasoning), [AA Terminal-Bench](https://artificialanalysis.ai/evaluations/terminalbench-v2-1). Astra high and medium are 89.89% and 89.51% on this terminal measure; more effort is not monotonically better, and small differences are not established as significant.

The same [LiveBench CSV](https://livebench.ai/table_2026_06_25.csv) and [category mapping](https://livebench.ai/categories_2026_06_25.json) yield Astra max Reasoning 92.65% versus Fable max 91.69%; Fable Mathematics 97.01% and Coding 86.38% versus Astra 96.81% and 80.36%. These selected comparisons support complementary roles, not a universal winner or OpenFHE competence proof. The release label remains 2026-06-25, not the observation date. Current official [Astra](https://developers.openai.com/api/docs/models/gpt-6-astra) and [Sol](https://developers.openai.com/api/docs/models/gpt-5.6-sol) pages were also opened; provider role descriptions are not independent scores.

Fable's AA variant enables fallback; the project's terminal requests disable it. Codex configured selectors and ChatGPT's visible `6 Pro` label are not inference attestations. No score is assigned to an unverified backend or different effort. API pricing does not establish subscription cost.

The latest retained Fable attempt was September 5 20:08:21–23 CST, HTTP 403, `账户余额不足`, with no usable inference and no reset supplied: [receipt](../paper-final-fable51-01/RECEIPT.md). This turn did not probe Fable again, inspect credentials, purchase capacity, or claim a current balance. No fresh ZCode quota or model-identity claim is made.

## Applied allocation and skill changes

- Codex lead owns the mathematical question now as well as orchestration, integration, necessary TDD, CI/Git and final disposition. A separate available Astra context challenges difficult derivations and counterexamples.
- ChatGPT Pro remains preferred for complex design/code drafts and paper-semantic review, with a complete sanitized packet each time and uninterrupted thinking. Keep its existing work and use completed returns.
- Sol is preferred for a new substantial bounded long-document/multi-file review. Terra is a balanced bounded-engineering fallback; Luna is for extraction, formatting and deterministic-checkable first passes, never sole precision sign-off.
- Fable 5.1 resumes targeted difficult scientific-code/adversarial consultation only after capacity recovery and verified identity. Codex owns the fallback immediately; restored availability does not move already progressing work.
- ZCode resumes suitable Windows supporting work only after the shared-quota and task-entry gates. Actions/Windows retain heavy builds and encrypted tests; the Mac remains low-load.

The skill now explicitly maps tasks to the relevant benchmark category, checks active/completed ownership before dispatch, records requested versus observed identity for every worker, and preserves checkpoints on quota fallback. Intentional blind/adversarial review remains allowed; duplicate implementation and repeat audits merely for a model change do not. The reference to historical benchmark evidence is now local-first and commit-pinned. Both the current engineering and task-discoverable skill copies received the same narrow routing edit; unrelated skill differences and dirty daily reports were preserved.

## Continuity of the current project

At this update's start: branch `codex/paper-scale-implementation-20260905`, clean HEAD/origin `2a4e7a17a7e0daca0b7660326674320bfc369656`; tested source remains `9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e`. Original E80 remains FAIL; this routing update changes no source, tests, precision gate or experimental count.

The independent Pro scientific return has already arrived in `artifacts/handoffs/paper-scale-precision-adjudication-return-01/`; its existing conversation is [Independent Scientific Review](https://chatgpt.com/c/6a9c4df6-b28c-83ec-8442-5ff28b80fb42). Do not poll, resend or redownload that completed task. The existing Astra mathematical review and Sol Standards review are complete in the same directory. Their reported remaining issues are a concrete numerical observer allowance/disposition, durable failed-CI sidecar transport/status, and eliminating self-referential sidecar hashes. Codex owns verification/disposition of these issues and the return checker's bounded reexecution; these steps are still pending at this routing checkpoint. No new encrypted run or acceptance change is authorized by this routing note.

## Verification performed for this change

- Both skill copies passed the skill-creator validator. The bundled Python initially lacked PyYAML; an isolated `uv run --no-project --with pyyaml` supplied it without altering project dependencies.
- A separate existing Sol context reviewed the old routing for gaps; a separate existing Astra context forward-tested the changed skill against four scenarios: Fable 403 while Pro thinks, mismatched max/high settings with an active owner, precision proof versus PDF transcription, and an unverified ZCode reset while a fallback progresses. All four selected nonblocking owners and preserved identity/test boundaries. This is qualitative skill validation, not model benchmarking or provider diversity.
- No C++ build, OpenFHE operation, FFT/NTT, crypto experiment, CI dispatch/rerun, external-agent submission or Telegram report was performed for this allocation change.

The final staged selection must pass strict secret scanning and diff checks before a documentation-only `[skip ci]` commit and push. Existing continuation scheduling and the 07:30 report branch are preserved; the saved prompt should point to this checkpoint and the current skill.
