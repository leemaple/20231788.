# Fable 5.1 decision task: Relin2 later evaluation-key semantics

Prepared: 2026-09-02 Asia/Shanghai

Status: completed by a verified, restricted, fresh-extraction
`claude-fable-5-1` run. The accepted controlling decision is recorded in
[`../reviews/fable51-relin2-later-key-decision-receipt.md`](../reviews/fable51-relin2-later-key-decision-receipt.md),
with complete raw evidence under
[`../handoffs/fable51-relin2-later-key-decision-01/`](../handoffs/fable51-relin2-later-key-decision-01/).

The retained handoff `TASK.md`, not this post-run status annotation, is the
byte-exact prompt bound by the packet manifest.

## Required model and response boundary

You must be the provider's current **Fable 5.1** model. Start your answer with
the exact model identifier you believe you are running. Do not claim access to
anything outside the supplied clean-room packet. Perform a read-only review:
do not edit files, build, run tests, access a network, or suggest weakening a
test merely to make it pass.

Return a concise engineering decision with four sections:

1. source/OpenFHE facts, with supplied file and line anchors;
2. which fixture is the semantically valid `ExtraValid` case;
3. whether the two cases should be scaffold-through characterization greens or
   full-Relin2 runtime reds at the current source, and why;
4. the exact minimal commit/test/mutation sequence that preserves honest TDD.

Clearly distinguish facts, inference, and remaining unknowns. If the packet is
insufficient, identify the exact missing material instead of guessing.

## Clean-room identity and scope

- public project: `https://github.com/leemaple/20231788.`;
- implementation branch: `agent/codex-relin2-01`;
- exact current source: `1e59e8b36d5119ceb2b463922f1053e03a029bd4`;
- source tree: `4e3a8b4857aeb8f5f7ef07dd2f01b5f74079ba77`;
- pristine OpenFHE: `df495ba2e91739a6dc8f1de254fc5a41155ce504`
  (release 1.5.0);
- paper method: 2023/1788 Double-CKKS, first-multiplication `Relin2` slice;
- Mac compilation is forbidden; this is a source-only decision.

The current project has 26 Linux/Windows-green tests. Relin2 validates the
Tensor and index-zero evaluation key completely and then throws the exact
terminal scaffold `DoubleCKKS: Relin2 is not implemented`. Arithmetic and
normal `ReadyForRS2` return are not implemented. HYBRID and both BV digit modes
now have independently tested A/B length, complete ordered basis, and
Evaluation-format guards.

## Frozen requirements still open

The supplied contract requires two independent configurations:

- `TestExtraLaterValid`, HYBRID/digitSize 0, `ExtraValid`, empty expected
  diagnostic, success true;
- `TestMalformedLaterIgnored`, HYBRID/digitSize 0, `MalformedLater`, empty
  expected diagnostic, success true.

The earlier task says:

- a valid vector with an additional later valid key must prove there is no
  erroneous `size()==1` restriction;
- a valid index-zero key followed by a malformed or null later key must
  successfully Relin2, proving later entries are not inspected or consumed;
- only index zero is validated and consumed for the current three-component
  input; extra later keys are permitted.

## Current observable helper

`CheckPassesCurrentScaffoldOrCompletes` accepts only either normal completion or
the exact current `std::logic_error("DoubleCKKS: Relin2 is not implemented")`.
It rejects every derived logic error, wrong text, and every other exception.
Existing valid-control tests call this helper and then immediately prove deep
Tensor, metadata, key-vector, context/tag, and global-cache invariance.

## Review disagreement to resolve

Two reviewers proposed one test-only commit per configuration, treating both as
existing-behavior characterization seams. They would call the scaffold helper,
label the result only as `preflight pass-through`, and rerun the same tests for
normal-return success after arithmetic. One suggested duplicating the valid
index-zero shared pointer as the second `ExtraValid` entry; both accept null as
the stable malformed later entry.

The API reviewer disagreed on two points:

1. A duplicate `s^2 -> s` pointer is not a semantically valid second key.
   OpenFHE's second generated entry is the `s^3 -> s` key. The reviewer proposes
   extending the test-only `MakeContext` helper with a defaulted
   `maxRelinSkDeg=2`, using `3` for these cases, and calling the public plural
   `EvalMultKeysGen` once to obtain real `[s^2 -> s, s^3 -> s]` entries.
2. Because the frozen table says `success=true`, the two tests should require a
   normal `ReadyForRS2` return now. They would therefore be honest runtime reds
   that remain red until the full Relin2 arithmetic commit; a scaffold-through
   result would not close the requirement.

The API reviewer agrees that the malformed case should generate the two real
keys and then replace only index one with null. The reviewer also states that
OpenFHE public `Relinearize` for a three-component ciphertext consumes only
`evalKeyVec[0]`, so a null index one should remain irrelevant after arithmetic.

## Decision constraints

- Do not manufacture a product red by adding an artificial production failure.
- Do not call a scaffold-through result full Relin2 success.
- Prefer one vertical slice at a time, but do not split tests from a production
  implementation if that would leave knowingly coupled long-lived reds without
  a useful intermediate behavior.
- The tests must use only public OpenFHE APIs and test-owned RAII restoration of
  the complete global evaluation-key map.
- Every production call must be followed immediately by deep Tensor and cache
  invariance checks. A normal-return test must also validate the full
  `ReadyForRS2` result.
- Any mutation proof is disposable evidence, not a production red. It must
  preserve and restore the exact source SHA and state which test alone it makes
  fail.
- KISS/YAGNI: do not add a production setter, compatibility layer, catch block,
  direct `KeySwitchCore` call, or OpenFHE modification.

## Supplied files

The packet contains the exact current project files:

- `project/CMakeLists.txt`;
- `project/include/openfhe_2023_1788/double_ckks.h`;
- `project/src/double_ckks.cpp`;
- `project/tests/relin2_test.cpp`.

It also contains the controlling contract documents:

- `contract/relin2-preflight.md`;
- `contract/chatgpt-pro-relin2-01.md`;
- `contract/chatgpt-pro-relin2-remediation-06.md`.

And the relevant pristine OpenFHE 1.5.0 files:

- `openfhe/src/pke/include/cryptocontext.h`;
- `openfhe/src/pke/include/scheme/gen-cryptocontext-params.h`;
- `openfhe/src/pke/lib/cryptocontext.cpp`;
- `openfhe/src/pke/lib/schemebase/base-leveledshe.cpp`.

Resolve the disagreement from those exact bytes. Do not rely on a former
implementation or any external repository state.
