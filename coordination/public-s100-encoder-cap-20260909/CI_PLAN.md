# Public S100 encoder-cap CI plan

## Frozen purpose and limits

This workflow provides two deliberately separate, tag-created CI stages for the
accepted public-encoder candidate.  It does not run on branch pushes, releases,
schedules, or manual dispatches.

- `public-s100-encoder-cap-red-20260909` establishes the intentional link RED.
  The build must fail, and one diagnostic line must contain both an
  `undefined reference`/`undefined symbol` marker and
  `InspectFixedS100PublicEncoding`.  Any successful link or unrelated failure is
  a failed RED job.  No transform or encoding is invoked in this stage.
- `public-s100-encoder-cap-once-20260909` is the separately reviewed one-shot
  GREEN stage.  It is eligible only after the implementation and this workflow
  have been reviewed.  Tag creation is a human action outside this slice.

Both the workflow trigger and job condition require an exact tag, a `push`
event, `created == true`, `deleted == false`, `forced == false`, and
`run_attempt == 1`.  The first executable gate repeats those checks against
`GITHUB_EVENT_PATH` without printing that file.  This static and synthetic gate
is defense in depth; it does not prove future GitHub scheduling or event
authenticity.

## Build and execution budget

Both stages use a fresh official `openfheorg/openfhe-development` checkout at
`df495ba2e91739a6dc8f1de254fc5a41155ce504`.  There is no dependency cache or
restore key.  The checkout and recursive submodules must be clean.  OpenFHE is
configured Release, native 64-bit, math backend 4, OpenMP on, reduced noise off,
with tests, examples, benchmarks, and extras disabled.  Builds use two workers.
The project build is isolated, enables only the public-encoder diagnostic, and
treats compiler warnings as errors; no default CTest or broad test target runs.

GREEN has this strict order:

1. Run the scalar/parser contract test, which performs zero transforms.
2. Build only `public_s100_encoding_dump` warning-clean.
3. Run `--metadata`; require the compiled source commit to equal
   `GITHUB_SHA`, with zero encoding and crypto calls.
4. Run `--api-negative`; require exactly
   `API_NEGATIVE_PASS encoding_calls=0`.  This banner does not independently
   attest cryptographic behavior.
5. Run the transform contract test, containing exactly four tiny transform
   models.
6. Record source/input/codec, compiler, Python, CMake, Boost package and
   compiler-referenced Boost-header, OpenFHE source/submodule/library, build
   flags, linked-library, and diagnostic-binary provenance.
7. Invoke the original public encoder exactly once, producing one new public
   JSON.  Require its source commit and counters: one encoding, zero crypto.
8. Invoke the interval certifier exactly once with the explicit reviewed
   transform permission.  Preserve its exact exit code, including REFUTED,
   INCONCLUSIVE, or rejected-input outcomes; no retry or second encoding is
   permitted.

The encoder and certifier each have their own timeout.  Their stdout, stderr,
and exit receipts remain in the unique owner-only evidence directory.  A final
`always()` upload targets only that non-hidden directory, named by GitHub run ID
and attempt; the binary, build trees, keys, credentials, and event payload are
not uploaded.

## Source binding and interpretation

The run binds the tag commit, `GITHUB_SHA`, reviewed ancestry
`5f94431d413c5b30cf0163719d4e17a29543b145`, and production baseline
`a4b815a733efe81897325e2a8e4c826a4ebfa439`.  It also requires the original
public input header to be byte-identical to the production baseline and records
the codec/reviewed-source diffs.  Labels, ancestry, and hashes identify the
inputs used by the run; they do not by themselves attest historical equivalence
or prove the mathematical claim.

A successful GREEN certificate can close only the adopted conditional public
encoding premise for the exact public polynomial and frozen implementation.  It
does not repair the original near-unit E80 failure, establish Table 3
statistics, prove security, or establish historical code/input pairing.  The
effective model identity remains **requested-unverified** unless the hosted
backend is independently attested.

## TDD and source-only receipts

The gate was written test-first.  Before the workflow existed, the bounded
suite was RED: three tests ran and the workflow-source test errored because
`.github/workflows/public-s100-encoder-cap.yml` was absent.  The exact-tag
positive cases and all synthetic negative event mutations already passed.

Final verification GREEN, after tightening the link RED to one diagnostic line:

```text
python3 -B coordination/public-s100-encoder-cap-20260909/test_ci_gate.py
Ran 3 tests in 0.458s
OK

python3 -B coordination/public-s100-encoder-cap-20260909/check_ci_gate.py source
{"encoding_calls": 0, "forward_transforms": 0, "status": "PASS_SOURCE_ONLY"}
```

The synthetic rejection set covers branch refs, wrong tags, reruns, forced
updates, deletions, non-created events, payload/ref disagreement, and non-push
events.  At this receipt point the checked files were:

```text
1f2c16761cb458229c64739ddd7e94f3c0099f766b81e53d9cfa2ed74e916f74  .github/workflows/public-s100-encoder-cap.yml
e97d8a8f0bc21428f844ecda68d258aea836bb52c9c9c26968aa83633b93f101  coordination/public-s100-encoder-cap-20260909/check_ci_gate.py
bc7d6285e404decb089d980e21bf6b7345610b554c671d190d2a3598ff75fcc1  coordination/public-s100-encoder-cap-20260909/test_ci_gate.py
```

These checks are non-cryptographic and source-only: zero encodings and zero
forward transforms.  This slice did not compile, dispatch a workflow, create or
push a tag, run either candidate contract test directly, invoke a transform or
encoder, or commit/push repository state.  Root review remains required before
the RED tag, and again before the distinct one-shot GREEN tag.
