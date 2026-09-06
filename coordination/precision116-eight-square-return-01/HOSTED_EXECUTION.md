# First candidate full-eight hosted observation

## Activation observed

- Exact source: `2b8b349edf5575556347082c1b725f6696c743b6`.
- Working ref `codex/precision116-eight-square-20260907` was pushed and verified first; it does not trigger this workflow.
- Dedicated ref `codex/precision116-eight-square-observation-20260907` was observed absent before activation, pushed once, and `git ls-remote` confirmed the exact source SHA.
- [Run 34055816234](https://github.com/leemaple/20231788./actions/runs/34055816234), created `2026-09-06T19:44:09Z` / September 7 03:44:09 Asia/Shanghai, workflow `OpenFHE 2023/1788 TDD`.
- Initial `gh run list` result: exact source, `in_progress`, empty conclusion. No rerun or manual dispatch.
- Subsequent `gh run view` confirmed attempt 1: Linux job `101547399114` started `19:44:13Z`, Windows job `101547399237` started `19:44:11Z`. At this checkpoint Linux was building pristine OpenFHE and Windows was installing its official toolchain. Neither had reached the new candidate target or numerical test.

This is one Linux observation and one Windows observation, not a trial batch. Compilation and eight-square numerical outcomes remain pending; do not infer PASS from workflow creation or static review. Fetch and bind the final run/attempt and full job logs before interpreting the outcome. Original S100 E80 FAIL and experimental security UNRESOLVED remain unchanged.
