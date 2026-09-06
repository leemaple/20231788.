# Routing update validation — 2026-09-06 morning

Scope: skill and coordination documentation only, based on engineering HEAD `b16dfe886b6efcdd56242c9b7af591ac0ea577ed`. This is not an implementation review, model benchmark, C++ build, or cryptographic test.

## Executed checks

- Both current engineering and catalog skill folders passed `uv run --no-project --with pyyaml /Users/lifeng/.codex/skills/.system/skill-creator/scripts/quick_validate.py <exact-skill-folder>`. The final check after clarifying active Pro ownership returned `Skill is valid!` twice, exit 0. Isolated uv tooling changed no project dependencies.
- `cmp` confirmed the two `references/model-routing.md` files are byte-identical, exit 0. Their other skill files were preserved.
- `git diff --check` passed in both workspaces. `git diff --quiet b16dfe886b6efcdd56242c9b7af591ac0ea577ed -- src include tests CMakeLists.txt .github/workflows` returned 0 in the engineering root: no engineering changes from this update.
- A separate read-only audit context `/root/model_routing_review_20260906` inspected the existing policy. It found the broad model allocation already supported; improvements were bounded acceptance, explicit terminal-partial recovery, nonoverlapping active ownership, author/reviewer independence, and separate dated evidence. Those specific changes were incorporated.

## Independent forward test

A new context `/root/routing_forward_test_0800`, requested selector `gpt-5.6-sol`, effort `medium`, received the changed skill and four scenarios without the intended answers or prior audit conclusions. Selector is **requested-unverified**: no emitted inference identity was attested, and this does not establish provider diversity.

| Scenario | Observed reviewer action | Disposition |
| --- | --- | --- |
| Fable 403 with no recovery; Pro actively thinking; independent serializer task available | Codex covers the Fable question, leaves Pro uninterrupted, assigns only the nonoverlapping bounded work; retains receipt/checkpoint. | PASS for intended ownership and continuity. |
| Pro terminal partial, two files claimed but no verified download, seven helpers missing, old RED closed | Verify artifact transport/source before acceptance; Codex owns missing helpers with independent review; preserve old evidence without calling it GREEN or using it for unrelated new tests. | PASS. |
| AA max-with-fallback versus CLI no-fallback init only; author self-review says PASS | No accepted Fable review; no transfer of score/configuration; separate independent review from source/spec/tests is required. | PASS. |
| ZCode reset time has passed, weekly last seen 91%, fallback already making progress | Read actual shared quota before dispatch; keep progressing fallback; restore only subsequent suitable work; no 1,000-experiment gate. | PASS. |

The reviewer found no policy conflict. It noted that non-quota retry details, inbound download mechanics, and the choice between two available bounded-task owners remain contextual. These are deliberately not converted into extra universal skill machinery: task-specific recovery evidence and existing external-collaboration rules still apply. Source/test correspondence, not an old CI item's label, determines whether a RED baseline is relevant. No numerical sign-off is claimed by these scenarios.

## Existing automation readback

The app's `automation_update` tool successfully updated the existing heartbeat `2023-1788-openfhe-07-00-pdf`; no new job was created. Full TOML readback matched the expected prompt (37,927 characters) and preserved ID, kind, name, status, schedule, target task and creation timestamp. Observed updated timestamp: `1788653071247` milliseconds since Unix epoch.

An exact substring comparison confirmed the complete `07:30 日报分支` remains byte-identical. The new engineering checkpoint identifies the terminal Pro partial, pending artifact verification, Codex ownership of the missing implementation, and independent reviews. It explicitly preserves the no-1000 scope, Fable no-reprobe rule, low Mac load and frozen scientific criteria. The September 6 Confirmed report delivery is preserved; no PDF or Telegram action was performed in this turn.

## Publication boundary

Publish only the routing reference and these two new coordination notes on the existing engineering branch after a clean exact-selection secret scan. Use a documentation-only `[skip ci]` commit, not a new numerical/CI claim. Retain the identical catalog skill edit in its local Git history; do not push its no-dot remote. Final commit and remote readback are the publication receipt, not a test-source upgrade.
