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
| 03 | all 18 reachable complete-validation runtime reds: composite basis, ordered manifest/descriptor, and independent high/low member-state cases; add `short_active_basis_source_gate` | all `B` inherited tests pass; all 18 named new cases execute independently and fail at their unchanged exact oracle; the static short-basis gate independently reds on the exact missing or misordered guard |
| 04 | complete validation, short-active-basis guard, and composite-degree-one guard | `B+18` pass; the unchanged short-basis source gate exits zero on the staged `ValidatePair -> short basis -> composite degree` order while no exact RS2 lifecycle guard exists; retain the first-green `S01`-`S04`, `S06`, `S11`-`S12`, and per-construction-route proof; source identity proves the immediate-throw scaffold and absence of RS2 arithmetic remain unchanged |
| 05 | one `ReadyForFirstMult` lifecycle runtime red; execute the separate invalid-enum source audit without forcing its outcome | `B+18` runtime tests pass and the one named runtime case independently fails; record the source audit as an authentic red or inherited green, retaining `L01`-`L03` now only for inherited green |
| 06 | minimal `ReadyForRS2` lifecycle guard; add an invalid-enum branch only if patch 05 proved it missing | `B+19` runtime tests pass; the invalid-enum audit exits zero without an audit-only production change and retains `L01`-`L03` here if this is its first green; the unchanged short-basis audit reruns green only on final `ValidatePair -> exact lifecycle -> short basis -> composite degree` order and retains `S05` |
| 07 | one complete valid arithmetic/coefficient/state/public-RCB oracle red plus the complete `rs2_call_order_source_gate` | `B+19` pass; the valid-path case reaches the unchanged immediate-throw scaffold and fails with its complete oracle already present; the source gate independently reds on the exact missing trusted-call/def-use set |
| 08 | minimal RS2 arithmetic, complete post-Rescale/compatibility/final-pair validation, and `AfterFirstRS2` construction | `B+20` pass with the unchanged complete valid-path oracle executing; all three source gates exit zero, and byte-frozen audits/drivers retain full `S01`-`S12`, every construction-route mode, `R01`-`R53`, and expanded `V`/`K`/`P` mutation/restore proof |
| 09 | metadata provenance, no-evaluation-key dependency, and the Tensor2/RS2 `AfterFirstRS2` regressions | `B+24` pass; all three static source gates rerun green; record each newly registered runtime case as an inherited green only if the existing implementation actually supplies it |
| 10 | final documentation and retained local TDD evidence only | `B+24` pass; all three static source gates rerun green on the final exact SHA, with complete `L01`-`L03`, `S01`-`S12`, every construction-route mode, `R01`-`R53`, and expanded `V`/`K`/`P` mutation/restore evidence retained; no source, CMake, workflow, or test change |

Patch 07's valid arithmetic test must already contain the complete independent
`(A,B)` coefficient oracle, public RCB identity and immutability, exact state
and basis, fixed rounding/carry witnesses, and whole-input immutability. Its
observed red remains the inherited scaffold exception; patch 08 removes that
scaffold and must be the first boundary to execute and pass the unchanged
complete oracle. Splitting those correctness obligations into later test
patches would weaken the arithmetic boundary.

Patch 07 must introduce the complete call-order/def-use audit unchanged from
its red through final green. Its scaffold red must print the complete discovered
call/result set and the exact missing dataflow gates. Patch 08 may change only
production needed for the formula and its mandatory validation; the same audit
must then prove that input high feeds the first public Rescale, accepted RCB
output feeds the second, both
returns feed exact NativeInteger multiplication/EvalSub, and the returned pair
is exactly `(A,B)`. It must also prove complete validation of both Rescale
returns before their first dependent operation, every pre-EvalSub compatibility
field, and complete validation of the exact final pair before return.
Counted-but-discarded trusted calls plus a separate manual implementation must
remain red. Rerun this audit at patches 08, 09, and 10 and
after any inserted production-source boundary. At patch 08 retain the complete
byte-frozen audits/drivers, `S01`-`S12`, every construction-route mode,
`R01`-`R53`, expanded `V`/`K`/`P` expected-versus-actual failure-code tables,
raw mutation logs, and pre/mutated/restored hashes required by
`rs2-tdd-preflight.md`. Rerun that complete mutation suite on the final exact
source; a green unmutated audit alone is insufficient. It never changes `B`.

Patch 09's metadata, cache, and two lifecycle regressions are expected to be
inherited greens. If any exposes genuinely missing behavior, retain its
authentic red, insert a minimal green implementation boundary, renumber the
documentation boundary, and recompute every later count; never bury a new red
in the documentation patch or manufacture one by weakening the fixture.

The invalid-enum source audit is likewise not entitled to an artificial red
or to a required `switch` spelling. It must prove the complete lifecycle
validation/dispatch rejects every invalid enum value on all paths with the
rebound exact `std::invalid_argument` type and complete project diagnostic. Run it
against the exact accepted Relin2 base, at patches 05 and 06, after every later
source-changing boundary, and again on the final exact SHA. If the transitive
accepted validator already supplies the semantic guard, record an inherited
green and leave it unchanged. Only an authentic missing guard may produce the
patch-05 red and authorize the smallest patch-06 fix. Patch 08's
`AfterFirstRS2` construction/validation must not delete or bypass that guard.

The accepted public construction routes cannot produce an otherwise valid
`ReadyForRS2` pair with only two active towers: the pair state is private, and
Relin2 rejects the short basis before producing that lifecycle. Therefore the
short-active-basis defense is a static source invariant, not a CTest fixture.
Introduce its token/AST gate as an authentic red at patch 03, make it green at
patch 04 with `ValidatePair -> short basis -> composite degree` while no exact
RS2 lifecycle guard exists. At patch 06 the unchanged gate must tighten to and
prove `ValidatePair -> exact lifecycle -> short basis -> composite degree`.
It must also bind the rebound exact `std::invalid_argument` type and complete
short-basis diagnostic. Rerun it after every later production-source boundary
and on the final exact SHA. Do not add a friend, setter, layout/UB hack, or
weakened validator merely to manufacture the unreachable runtime state. The
final exact source must retain the complete `L01`-`L03`, `S01`-`S12`, and every
per-construction-route mutation/restore proof. None of the three static audits
changes `B`.

If and only if the final Relin2 gate proves `B=37`, the corresponding expected
green totals become 37, 55, 56, 57, and 61. These numbers are illustrative
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
the `Deferred exact-green gate` section of `rs2-preflight.md`, including:

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
