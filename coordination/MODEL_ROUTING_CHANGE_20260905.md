# Model allocation change — 2026-09-05

User instruction: use model capability benchmarks such as Artificial Analysis to revise collaboration; when Fable 5.1 lacks quota, Codex takes over.

## Applied policy

The project workflow now routes allocations through `.agents/skills/openfhe-2023-1788-workflow/references/model-routing.md`. The same routing policy is installed in the current task's discoverable project skill; unrelated differences between the catalog and clean-room skill copies are preserved. The catalog copy also now carries the existing clean-room ZCode shared-quota gate.

- Codex: critical-path reasoning, needed implementation, TDD, integration, CI, Git and final accountability.
- ChatGPT Pro: preferred bounded complex design/code drafts plus semantic/paper review, with a complete sanitized packet and uninterrupted thinking.
- Fable 5.1: targeted high-risk scientific-code/mathematical challenge when usable; unavailable now, with immediate Codex takeover.
- Sol: candidate for substantial bounded work or long-document review. Terra/Luna: simpler deterministic-checkable extraction/formatting/first passes, not precision sign-off.
- ZCode: later bounded Windows assistance only after actual shared-quota and task-entry verification.
- GitHub Actions/Windows: actual heavy computation. Deterministic source/hash/log checks stay tool-driven.

Reasoning for this split, exact benchmark variants, limitations and source URLs are in [the benchmark note](MODEL_BENCHMARK_EVIDENCE_20260905.md). It is an allocation inference, not a claim that a product label attests the lead session's backend or that benchmark scores prove OpenFHE correctness.

## Current availability evidence

A single terminal attempt at 2026-09-05 20:08:21–20:08:23 Asia/Shanghai returned HTTP 403, “账户余额不足”. Init requested `claude-fable-5-1`; only a synthetic error was returned, no model usage/inference or source reads occurred. Status: **NO_ACCEPTED_FABLE51_REVIEW**. No repeat was made; no reset time was supplied. [Exact receipt](paper-final-fable51-01/RECEIPT.md), [preserved question](paper-final-fable51-01/TASK.md).

Codex owns the preserved paper-scale interface/scale/oracle questions immediately. No purchase, credential change or external-agent interruption is authorized by this allocation change. No fresh ZCode quota claim is made here.

## Verification and preserved boundaries

- Both discoverable/catalog and clean-room skills passed the official skill-creator `quick_validate.py` (a disposable uv environment supplied its missing PyYAML dependency; no project dependency change).
- An independent read-only subtask requested with `gpt-5.6-sol`, medium effort, evaluated four realistic scenarios: Fable 403 while Pro thinks; dual-host CI evidence plus formatting; unconfirmed ZCode reset at weekly91%; benchmark fallback/product identity mismatch. All four selected nonblocking owners and preserved identity, quota, oracle and test requirements. This is behavioral review, not a model benchmark or cryptographic test.
- Role policy content matches across both copies. Clean-room isolation, OpenFHE pin, TDD, source-handoff scans, Pro conversation discipline, the no-1000-run scope and daily reporting boundaries are unchanged.
- The initial staged gitleaks scan stopped on a generic-api-key false positive spanning two zero-usage receipt fields. The fields were verified against the structured terminal receipt and rephrased as readable sentences; no secret value, scanner rule, baseline or exclusion was changed. The final staged selection must pass a fresh scan before publishing.
- Only skill/coordination documentation is changed by this checkpoint; no implementation, test, build or workflow edit and no new CI run is part of the allocation update.
- Existing continuation automation is to consume this routing and the latest engineering status; its 07:30 report/delivery branch and schedule are preserved.
