# S100 annulus125 one-shot CI source plan

Recorded 2026-09-08 Asia/Shanghai. This document describes source controls for a
future tag-triggered run; it is not an execution receipt or authorization to create
the tag.

## Prior compile/control evidence

GitHub Actions run
<https://github.com/leemaple/20231788./actions/runs/34183786327> completed SUCCESS
at exact commit `def248a04b7212088e41a72e2239496dcd3e7027`. The retained run record
shows the warning-clean candidate build completed in 57 seconds, CTest performed
show-only selection, and the direct keyless controls entry completed in 13 seconds.
Its retained stdout reports `encrypted_runs=0`. That run compiled and checked the
candidate; it did not launch a payload or establish a numerical result.

## One-shot event boundary

`.github/workflows/s100-annulus125-once.yml` has only one trigger: creation by a
`push` event of the exact tag `s100-annulus125-once-20260908`. It has no branch,
path, release, schedule, or manual-dispatch trigger. Its single job requires all of:

- event name `push`;
- exact ref `refs/tags/s100-annulus125-once-20260908`;
- `created=true`, `deleted=false`, and `forced=false`; and
- GitHub run attempt 1.

The checked-out script repeats those facts from `GITHUB_EVENT_PATH` and the named
GitHub environment values without printing the event payload. A rerun attempt is
skipped by the job condition. Deletion is rejected. This is a stateless source
guard, not a durable remote ledger: operational authorization therefore requires
root to create and push this tag exactly once and never delete/recreate it.

All other checked-in workflows were parsed during the source check. Their push
triggers have branch allowlists, or they are manual-only, so this tag does not match
the legacy/full workflow paths.

## Exact source and build boundary

Checkout uses full history only so the workflow can resolve the reviewed compile
commit. Before dependencies or a payload, it requires:

```text
git diff --quiet def248a04b7212088e41a72e2239496dcd3e7027 HEAD -- src include tests CMakeLists.txt
```

Thus the tag may include reviewed workflow, coordination, finalizer, and receipt
changes, but its production/scientific source, test candidate, and CMake contract
must remain byte-identical to the successful compile/control commit. HEAD, tag ref,
and clean checkout are also checked.

The workflow retains the tested official OpenFHE pin
`df495ba2e91739a6dc8f1de254fc5a41155ce504`, clean source/submodule and sole-main-
remote checks, native64/backend4/OpenMP configuration, exact cache key, Python 3.12,
and two-worker build. It creates a fresh mode-0700 build directory, enables only the
annulus opt-in target, treats warnings as errors, and builds only
`s100_annulus125_eight_square_test`. No legacy, default, full-chain, Windows, or CTest
execution is present. The binary SHA-256 is recorded before the sample, while the
binary itself and build tree are outside the uploaded evidence directory.

Before encryption, `check_finalizer.py` runs its synthetic completed-evidence tests.
This adds no build, FFT, key, payload, or sample. The binary's own `Run` path retains
the keyless observer control before key creation, so there is no separate
`--controls` invocation in this workflow.

## Sole process and finalization boundary

The workflow has exactly one direct invocation of `run_once.py`:

```text
--executable "$S100_BUILD/s100_annulus125_eight_square_test"
--result-directory "$S100_EVIDENCE/sample"
--source-commit "$GITHUB_SHA"
```

It uses the helper's frozen 1200-second subprocess timeout. The result directory is
new and owner-only; program-start is durable before launch; stdout, stderr, raw TSV,
actual return code, timestamps, and timeout status remain in that directory. The
step propagates the wrapper exit normally: there is no `continue-on-error`, shell
masking, retry, loop, or second invocation.

After every attempted process, including numerical failure or timeout, an
`always()` conditional step calls `finalize_once.py`. It validates the frozen
receipts and banner, then replays the same realpath `raw.tsv` at decimal precisions
180 and 230. These are two scalar interpretations of one stored file, not two
encrypted samples. Invalid/incomplete evidence remains distinct from a valid
numerical FAIL. A final `always()` step uploads only the exact non-hidden,
owner-only evidence directory, including failure evidence; it does not upload the
binary, build tree, credentials, or keys. Workflow permissions are `contents: read`
and checkout credentials are not persisted.

## TDD and proof limits

`check_one_shot_gate.py` was added before the workflow. Initial command:

```text
python3 -B coordination/s100-annulus125-20260908/check_one_shot_gate.py self-test
```

RED: exit 1; three tests ran. The fresh exact tag/attempt-1 positive and all event
negatives passed; only the workflow-source test errored because the new workflow
file did not exist. The negative matrix covers branch ref, wrong tag, rerun, forced,
deleted, not-created, and non-push events.

After adding and hardening the workflow, final GREEN was 3/3 tests in 0.404 seconds.
The checker parses the actual YAML through the existing bounded Ruby/Psych helper,
requires the exact job/event/source/build/invocation/finalizer/artifact controls,
and confirms existing workflows cannot match the tag. These local checks ran no compiler, CTest, FFT,
OpenFHE, key generation, encryption, or GitHub event. They do not prove GitHub will
schedule or execute the YAML as intended. The actual one-shot run remains pending
independent review, final approval, root's workflow commit, and root's single tag
creation/push. Requested worker identity remains `requested-unverified`.

Regardless of the future result, this controlled one-sample annulus experiment does
not fix or supersede the original near-unit S100 E80 FAIL, prove the whole annulus,
prove Table 3 statistics/performance, certify security, or justify additional
samples after a FAIL or invalid record.
