# Eight-square hosted execution boundary

Root owns CI integration; Pro owns the pending new test/CMake draft. This is a read-only wiring inspection and integration constraint, **not a workflow change, test run or dispatch**. Do not dispatch the current workflow unchanged.

## Observed baseline

Inspected HEAD `448086c1c0735d65f2573d4b60cafa1bff525f9d`:

- `.github/workflows/dcp-rcb.yml` blob `fc7081398a56801de9ca6aa8deb27ba7f10ef334`.
- `CMakeLists.txt` blob `018748de9c68b2b3db86c567ef5f3f12919b58a3`.
- Exactly 62 registered CTest names. In each host, both legacy list/run exclusions select 57 names; both complete-suite exclusions select 60. These are source-derived counts, not new CTest execution.
- Current working ref `codex/precision116-eight-square-20260907` is not a push trigger. Documentation pushes do not activate this workflow.
- The existing experimental execution step selects only the completed single-operation GREEN ref, not this working ref.
- If the current working ref were manually dispatched unchanged, the old `Run and finalize paper endpoint once` condition evaluates true after preceding success. Its selector's `always()` condition also evaluates true after preceding failure; uploader additionally requires selector success. Therefore merely adding a new test step or skipping the old runner alone is insufficient isolation.

Root fully read the 627-line workflow and the relevant CMake registrations. A bounded isolated Python inspection reused the already read `check_ci_wiring.py` YAML parser and boolean evaluator (run name `ci_inspection_only`, so no unit-test entry point); it compared registration names, regex selections and exact-ref conditions for both jobs. Exit 0. No CMake configure, compilation, CTest, cryptography, GitHub API mutation or Mac heavy work occurred.

## Integration gate after the actual draft arrives

1. Bind the **actual returned** CMake target/test name and timeout only after code intake/review. Do not guess its name now or manufacture a missing-target RED. Keep it explicitly built and excluded from default/legacy selection.
2. Preserve original 57/60 selected names, five API targets, original test properties, native64/backend4/pristine pin, provenance checks, host setup and two-worker build limit. Add the actual new CTest name to both list/run exclusions in both jobs; a new registration otherwise reaches an unbuilt executable or repeats a costly chain in the legacy suite.
3. Choose a dedicated one-shot activation ref after review, leaving the working documentation ref untriggered. On that activation ref disable **all three** old endpoint runner/selector/uploader steps on both hosts, including `always()` paths. Do not rerun the old profile seam or original full chain as this new experiment.
4. Explicitly build the returned target, then run exactly the new anchored CTest name with `--no-tests=error`, `--verbose --output-on-failure`, `OMP_NUM_THREADS=2`, serial execution and an outer limit longer than the actual inner CTest timeout. Windows must retain its observed MSYS2 shell, project directory, native prefix/build conversion and runtime PATH.
5. Before pushing the one-shot ref, statically check both host branch conditions, nonempty exact test selection, unchanged original selection/steps and no endpoint activity after success/failure. Preserve any actual compile/setup/harness failure separately from numerical acceptance. The first valid full-eight numerical observation may PASS or FAIL; no expected-failure inversion or retry loop.

Raw hosted CTest/job output suffices for this bounded experiment. Do not create another endpoint publisher, serializer, upload framework or statistical batch. Record exact source/run/attempt/platform and retain failure output. The new test's numerical thresholds and oracle remain governed by `TASK.md`, not by CI convenience.

## Live dependency

At `2026-09-06T18:57:19.258Z`, the canonical Pro conversation in `SUBMISSION.md` still had one user message and an active Stop control, with no downloadable return. The page displayed a longer-thinking notice, not a terminal failure. Keep that same conversation uninterrupted; no restart or reassignment is justified by the notice.
