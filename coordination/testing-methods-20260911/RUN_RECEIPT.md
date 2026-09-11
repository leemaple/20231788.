# N256 initial-phase exact diagnostic — remote execution receipt

## Final observed result: bounded PASS

The sole job completed successfully at 10:47:35 Asia/Shanghai, in 4m13s.
Exactly one CTest passed: `initial_phase_exact_contract`, test time 0.19s,
total real CTest time 0.21s. The baseline passed and both same-ciphertext
controls were rejected with their exact expected classes:

```text
baseline=PASS profile=initial-phase-n256-h128-v1 N=256 towers=3 H=128 G=39 F=15015
mutation=one-tower-plus-one status=REJECTED reason=FRESH_CRT_COHERENCE
mutation=all-towers-plus-30031 status=REJECTED reason=FRESH_SUPPORT_BOUND
slice=PASS originalS100=UNCHANGED source_base=6d2ae1c0d0b4fa7259c6a08245a7ee004004faeb dependency_pin_declared=df495ba2e91739a6dc8f1de254fc5a41155ce504
```

This is one public encryption with the ordinary key-setup draws, not one RNG
draw and not three independent samples. No fault-only mode, resampling,
full-S100 suite or second attempt was executed. GitHub run and all job-step
conclusions are success; the event remains the one new tag push at X.

### Artifact and runtime verification actually completed

Root downloaded artifact10182556318 as raw ZIP, verified its 18,570 bytes and
SHA-256 `c5c28a71b631c6b2a80f69ca15938d182714c55aec0482b27e4c246c88a7a163`
against GitHub's API digest, then checked CRC, 20 unique flat regular members,
the exact 19-file manifest plus self-excluded manifest, every size and SHA-256.
Only after those checks were the files extracted under
`artifacts/initial-phase-34555701307/evidence/`. No artifact program was run.

Root separately compared all five `source-sha256.txt` values with the external
acceptance document in Y, replayed the already reviewed pure result/selection
guards, checked the literal 1/1 Passed and zero-failure summary, and checked
history/core-control/result marker files. All passed. CTest used exactly the
reviewed executable path and `--controls`, with OMP_NUM_THREADS=2 and timeout180.

Both actual CMake caches were parsed: fourteen requested dependency options
matched, including native64/backend4, OpenMP ON, reduced-noise/noise-debug OFF,
fresh shared Release build and disabled official test/benchmark/example suites.
Project OpenFHE_DIR resolves inside this run's fresh install prefix. The three
recorded OpenFHE dynamic-library paths are inside that prefix, no `not found`
entry exists, and the workflow hashed the actual resolved binaries. These
binary hashes were read from the verified remote evidence, not recomputed on
the Mac; binaries were deliberately not downloaded or uploaded as artifacts.

Observed toolchain: Ubuntu24.04/GCC13.3.0, Boost development package
1.83.0.1ubuntu2, CMake3.31.6, Python3.12.14. The runner annotated that pinned
Node20-targeting actions were forced to Node24; all steps still succeeded.
This is a non-blocking environment note, not a numerical failure.

Root's strict decoded Gitleaks8.30.1 directory scan of the verified evidence
returned exit0 and `[]` findings (about109140bytes). Working directory was
`/private/tmp`, clean environment, allow-comments/ignore-file bypass disabled,
decode depth5 and archive depth1. This is a scan result, not an absolute secrecy
guarantee. The test source and sealed artifact have no key/noise-vector dump.

Machine-readable observed result and exact manifest/log hashes are retained
in `RUN_RESULT.json`. Original raw evidence remains locally under the path
above and in the GitHub artifact (30-day artifact retention).

### Scientific disposition and next boundary

Accept only this fixed N256/h128 public-scheme initialization slice. It closes
a useful independent-transform coverage gap and demonstrates two non-vacuous
negative controls. No new production defect or fix was found. Coherent small
deviations within the support bound and sampler-distribution errors may pass;
N32768, the full encoder/wrapper and paper-scale chain were not exercised.

Original S100 FAIL is unchanged. Normal initialization-error amplification
remains a better-supported hypothesis, not a uniquely proven cause. The next
candidate is a reviewed, in-process, paper-scale same-sample observation at the
existing fresh-error seam, with only public booleans and run identity exported.
It is **not dispatched** by this one-shot closure and cannot recover the
unretained historical key/ciphertext state. No new approval request, automation,
merge or production patch is implied.

## Dispatch and fixed trust root

- Run: https://github.com/leemaple/20231788./actions/runs/34555701307
- Created by GitHub: `2026-09-11T02:43:19Z` = 10:43:19 Asia/Shanghai.
- Exact execution commit X: `6d2ae1c0d0b4fa7259c6a08245a7ee004004faeb`.
- External acceptance document commit Y: `1440a4f1ad0888641905aac6638e93fe69a0429f`.
  Its only changes are `EXECUTION_APPROVAL.json` and `STATUS.md`; GitHub confirms
  its parent is X. All five declared source SHA-256 values were compared with
  X's actual Git blobs before dispatch.
- Exact lightweight tag: `initial-phase-exact-once-20260911`. GitHub ref API
  confirmed object type `commit`, SHA X after the single tag push.
- Before dispatch, remote tag lookup was empty and the exact tag's GitHub run
  history had `total_count=0`. First observed run is `34555701307`, attempt 1;
  its `headSha` equals X. No rerun, tag recreation, workflow dispatch or timer
  was used. The older full-S100 workflow was not triggered.
- Official OpenFHE: `df495ba2e91739a6dc8f1de254fc5a41155ce504`; no local
  modified dependency or previous implementation is in scope.

## Historical initial observation, before the result

GitHub reports `in_progress`. The runner will build fresh pinned OpenFHE and
explicitly build only the new diagnostic. C++ compilation, CTest outcome,
classified controls, actual dependency/cache/loader provenance and artifact
manifest are still pending. A green badge alone will not be accepted.

The Mac has executed only source/manifest/scalar checks and six small runner
guard tests. No local build, FHE, sampling or FFT/NTT was run. This test does
not rerun original S100 or paper Table 3 and cannot change their status.
