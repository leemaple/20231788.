# S100 controls-only CI source receipt

- Recorded: 2026-09-07T18:45:16+08:00
- Baseline: `724a43cec317d08c6699fd89671b10cc4cd3e9cb`
- Branch: `codex/s100-fresh-error-repair-20260907`
- Worker identity: `requested-unverified` (the subagent runtime exposed no independently attestable model identifier or inference receipt)
- Scope: workflow source and its bounded source checker only

## Source contract

The workflow has one optional `workflow_dispatch` choice input, `s100_scope`, with
default `fresh` and choices `fresh` and `controls-only`. Only the existing
`Run S100 fresh-error diagnostic once` step changed its condition. On the exact
GREEN ref, an empty input or `fresh` retains the prior fresh run; `controls-only`
skips it. The existing encoding-inspection step stays byte-for-byte unchanged.

The checker parses the actual YAML with Ruby/Psych `safe_load` and uses a bounded
conjunction evaluator without Python `eval`. It proves the entire workflow equals
the baseline after normalizing only the new dispatch input and fresh-step condition.
This covers unchanged branches, jobs, steps, the eight legacy exclusions, the
Windows guard, other refs, and default success/failure behavior.

## TDD receipt

Command for every source-check run:

```text
python3 coordination/s100-fresh-error-repair-01/check_ci_ref_gates.py
```

RED before the workflow edit: exit 1; 12 tests ran in 0.016 seconds, with one
failure and one error. The failing checks were
`test_controls_stays_enabled_and_only_fresh_honors_controls_only` and
`test_one_optional_choice_input_defaults_to_fresh`. The ten pre-existing and
preservation checks passed.

The first post-edit run exposed one stale pre-existing assertion that required the
fresh step's whole condition to equal the former ref-only expression. The checker
was narrowed to retain that test's original semantic purpose (fresh scope is
enabled); the new controls-only tests independently require the exact appended
gate and whole-workflow preservation.

Final GREEN: exit 0; all 12 tests passed in 0.015 seconds. `git diff --check`
also passed.

## Limitations

No workflow was dispatched. No build, CTest, cryptographic operation, commit, or
push was performed for this source slice. GitHub runtime behavior and the planned
controls-only RED/GREEN runs remain pending external evidence; prior runtime claims
were not used as evidence for this receipt.
