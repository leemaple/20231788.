# RS2 downstream CI preflight

Recorded: 2026-09-01 Asia/Shanghai

Status: **provisional execution plan only**. This is not an RS2 engineering
task, source patch, test result, accepted lifecycle, or permission to start
RS2 before the deferred gate in `rs2-preflight.md` closes.

## Runtime bindings that must remain unknown for now

- `R`: the final accepted Relin2 commit and tree after one unchanged SHA has
  passed hosted Linux and Windows and all review findings are closed.
- `B`: the complete baseline CTest count independently read from
  `ctest --show-only=json-v1` at `R`.

No absolute RS2 test count may be frozen before both values exist. In
particular, the currently expected `B=37` is not evidence that the accepted
Relin2 result will still have that exact executable identity.

## Provisional semantic boundaries

RS2 should use an independent worktree and branch created from `R`:
`agent/codex-rs2-01`. Subject to the final Relin2 audit, the smallest useful
red-green series is:

| Patch | Bounded change | Required boundary observation |
|---:|---|---|
| 01 | workflow trigger, CMake, compile-only API contract | production library builds; API target fails only for missing `AfterFirstRS2` and `RS2` |
| 02 | appended enum value, declaration, immediate-throw scaffold | all `B` inherited tests pass |
| 03 | malformed-manifest, insufficient-basis, and composite-degree red tests | all `B` inherited tests pass; all three named new tests independently fail at their intended oracle |
| 04 | complete validation and composite-degree-one guard | `B+3` pass; valid input still reaches the scaffold |
| 05 | one wrong-lifecycle red | `B+3` pass and the one named new test independently fails |
| 06 | minimal `ReadyForRS2` lifecycle guard | `B+4` pass |
| 07 | one complete valid arithmetic/oracle red | `B+4` pass; the valid case contains the complete oracle but still fails at the unchanged immediate-throw scaffold before that oracle executes |
| 08 | minimal RS2 arithmetic and `AfterFirstRS2` construction | `B+5` pass |
| 09 | separately registered Tensor2/RS2 `AfterFirstRS2` regressions | `B+7` pass |
| 10 | final documentation and retained local TDD evidence only | `B+7` pass; no source, CMake, workflow, or test change |

Patch 07's valid test source must already contain the complete independent
`(A,B)` coefficient oracle, public RCB identity and immutability, exact state
and basis, metadata provenance, fixed rounding/carry witnesses, and whole-input
immutability. Its observed red remains the inherited scaffold exception;
patch 08 removes that scaffold and must be the first boundary to execute and
pass the unchanged complete oracle. Splitting those correctness obligations
into later test patches would weaken the arithmetic boundary.

If and only if the final Relin2 gate proves `B=37`, the corresponding expected
green totals become 37, 40, 41, 42, and 44. These numbers are illustrative
until then and must not appear as accepted claims.

## Source-agent and isolated-replay command shapes

These commands are for source-agent evidence and Codex's isolated replay. They
are not claims about the exact commands currently present in GitHub Actions.
Patch 01 uses them to keep the successful production build separate from the
intended API compile red:

```sh
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$OPENFHE_PREFIX"
cmake --build build --parallel 2 --target openfhe_2023_1788
cmake --build build --parallel 2 --target rs2_api_contract_test
```

All later source-agent/replay boundaries use unfiltered configure, build,
registration discovery, and execution:

```sh
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$OPENFHE_PREFIX"
cmake --build build --parallel 2
ctest --test-dir build --show-only=json-v1
ctest --test-dir build --output-on-failure
```

No filter, label selection, `--rerun-failed`, early-exit harness, or first
failure may stand in for execution of every registered red/green boundary.

The hosted jobs must be judged using the literal commands in the workflow blob
at each exact SHA. The accepted-base Linux job currently performs one full
`cmake --build build --parallel 2` and does not run
`ctest --show-only=json-v1`; the Windows job uses its separate MSYS2 and
`$PROJECT_BUILD` command sequence. Patch 01 is provisionally limited to adding
the RS2 branch trigger, so it does not silently replace those hosted commands.
If the accepted Relin2 workflow differs, reread it and revise this preflight
before dispatch. Source-agent/replay JSON proves executable identity; hosted
logs independently prove the commands and cases that the exact workflow
actually executed.

Each boundary is committed and pushed only after its local/static gates:

```sh
git diff --cached --check
git write-tree
git commit
git push --porcelain origin HEAD:refs/heads/agent/codex-rs2-01
git ls-remote --heads origin agent/codex-rs2-01
```

The hosted run must be selected by exact `head_sha`, `event=push`, and workflow
path, never by assuming the newest run is the intended one.

The sequence is strictly serialized. Do not create or commit the next
boundary, batch several semantic commits before a push, amend an already
pushed boundary, force-push, or otherwise move its branch ref until the prior
SHA's required Linux result, semantic review, downloadable evidence, hashes,
and (when applicable) terminal cancellation state are all closed. A later SHA
cannot retroactively prove an earlier red or green boundary.

## Linux/Windows execution policy

For patches 01 through 09, first wait for the Linux job to become terminal,
close its semantic and log/hash review, and then inspect the Windows job. If
Windows or the overall run is still nonterminal, cancel the whole run
immediately, wait until it reaches a terminal state, and retain all available
run/job/check/log evidence before pushing the next boundary. This prevents
overlapping hosted work while keeping the Mac idle.

Patch 10 must run to completion without cancel, amend, or follow-up commit. The
same patch-10 SHA must pass both Linux and Windows with identical executable
test identity and expected count before RS2 can be reviewed as a candidate.

## Evidence isolation

The implementation branch must contain exactly the ten semantic commits above.
Actions JSON, job logs, complete-logs ZIPs, checksums, and review receipts go
on a separate non-triggering branch:

```text
evidence/rs2-hosted-<patch10-short-sha>
```

Every evidence directory and manifest must name the implementation commit it
proves. Retain commit/ref/tree/parent, push receipt, workflow blob, complete
run/jobs/check-runs/artifacts-list JSON, Linux job log, final Windows log,
terminal complete-logs ZIP, OpenFHE SHA, runner/compiler/CMake identities,
literal commands and exits, test names/counts, and file size/SHA-256 manifest.
An empty artifacts list is evidence only when the exact API response is saved.

## Deferred hard stops

Before creating the RS2 branch or task, close all of
`rs2-preflight.md:396-411`, including:

- final accepted Relin2 SHA/tree, full test identity, hosted same-SHA result,
  diagnostics, and review closure;
- actual `ReadyForRS2` manifest, current/input recorded factor, both logical
  scales, and metadata provenance/alias behavior;
- fixed `q_l`, `q_div`, and `baseSF` witnesses plus observed public-Rescale
  basis/level/degree/factor behavior;
- final lifecycle name/predicates and exact project diagnostics.

At receipt time, reread the exact workflow blob. The current accepted-base
workflow does not trigger an RS2 branch, has no concurrency group, and uploads
no artifact; patch 01 must add only the required branch trigger, while strict
serial pushes and API log collection handle the other two facts without
speculative workflow machinery.
