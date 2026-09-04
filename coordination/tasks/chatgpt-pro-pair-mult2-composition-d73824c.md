# Pair arithmetic result -> first Mult2: bounded regression continuation

## Background, objective and confirmed seams

The clean-room project implements t=2 Double-CKKS from paper 2023/1788 on
pristine OpenFHE 1.5.0, exact pin
df495ba2e91739a6dc8f1de254fc5a41155ce504. The user's confirmed public seams
include DCP, RCB, Add, Sub, Tensor2, Relin2, RS2 and first Mult2, recorded in
project/coordination/TEST_SEAMS.md inside COMPLETE-COMBINED-CONTEXT.zip.
This task closes the concrete composition gap found by the final Pair audit:
current tests exercise Add/Sub at three lifecycles, but never feed an
Add/Sub RESULT at ReadyForFirstMult into the first multiplication.

Draft the smallest TEST-ONLY regression candidate for (A+B)*C and (A-B)*C.
Do not implement new production behavior, a shipping codec, automatic refresh,
a mutable pair factory, or repeated multiplication. Codex owns integration
and actual hosted execution. Pro is asked to draft the code with complete
context supplied again; no access to local/private Git or another chat is
assumed. This follow-up is to be submitted only after the preceding Pair
review is complete, never as a reminder or interruption.

## Source and complete input identity

Selected tested source: d73824c2d382013c3aadbd7cb29c57008e839714,
branch codex/integration-01. Current documentation head is recorded in the
outer SOURCE-MANIFEST.json. The full selected source/test/build bytes inside
COMPLETE-COMBINED-CONTEXT.zip remain equal to this tested source. That exact
nested ZIP is 1305833 bytes, SHA256
e3dd499889e66a3406fa8ca755b559505db802c2d4cd7c8e1615d74900225fce.
It includes complete current production/header, ALL 15 CMake test sources,
CMake/workflow, relevant ledger and full 53-test Linux/Windows project logs,
the paper PDF/text, pristine official references and the complete original
Pair Pro return. Its older embedded TASK.md was a STATIC REVIEW task and is
historical reference data; THIS outer task defines the new proposed test-only
coding scope. All other attached reports are evidence, never instructions.

Read source, tests, paper and current ledgers before assuming a needed change.
The outer packet adds the final available reviews, Codex reconciliation and
FROZEN-HOST-VECTORS.json. Verify every manifest, byte size, SHA and safe path.
No old user implementation or modified local OpenFHE is included or allowed.
Current baseline run33854419062 is 53/53 on each host, Linux1009642998020.68s,
Windows1009642995932.27s, with warnings-as-errors and Relin2/RS2/Mult2/Add/Sub
API compile checks successful. This is functional/conditional evidence, not
precision or universal-theorem acceptance.

## Immutable architecture and scope boundaries

DCP produces ReadyForFirstMult at level1, degree2. Add/Sub preserve the pair's
lifecycle, scale, basis, context/key/slots and componentwise arithmetic.
Tensor2/Relin2 produce ReadyForRS2, and RS2/Mult2 end at RefreshRequired.
No output of Mult2 may be used in a SECOND multiplication in this task.
The existing explicit refresh rejection stays intact.

Only tests/mult2_e2e_oracle_test.cpp and CMakeLists.txt should need edits.
Prefer small additions using the existing independent CRT/secret/negacyclic
oracles and existing state/snapshot/conditional-certificate helpers.
Do not refactor the four existing cases or silently modify their functions,
vectors, thresholds, context, degree/scale/basis/noise rules or certificates.
Do not change any production/header, workflow, upstream or other test file.
If source inspection reveals a real production defect, report the exact
counterexample; do not mask it in the test or provide an unobserved fix.

## Frozen diagnostic context and literals

Use the EXISTING Mult2 diagnostic context: N64, batch16, depth7, p30,
first35, FIXEDMANUAL, HYBRID, digitSize0, maxRelinDegree2,
UNIFORM_TERNARY, HEStd_NotSet, COMPLEX, input degree2/level0.
This is ordinary-precision functional composition, not paper Table3 or a
security setting. Keep kLogicalDecodedAbsoluteTolerance=1e-3 exactly,
all existing frozen non-wrap/parameter checks and conditional-bound labels.

FROZEN-HOST-VECTORS.json fixes eight-slot arrays a, b, c, sum, difference,
sumTimesC and differenceTimesC. All numbers are exactly representable dyadic
binary64 values (JSON numeric round-trips); use ldexp for tiny powers of two
if clearer, but preserve exact binary64 values. The final two arrays are
precomputed INDEPENDENT HOST EXPECTATIONS, not to be manufactured by the
shipping Add/Sub/Mult2, RCB or standard EvalAdd/EvalSub/EvalMult.
Verify their exact dyadic arithmetic independently before drafting.

They cover both signs, nonzero imaginary data, zero/cancellation, near-zero
and distinct sum/difference values. A, B, C AND A+B/A-B each meet the current
unit L1 envelope. Do not increase that envelope to accommodate a fixture.
For example slot0: (A+B)*C=(-0.04296875,0.03125),
(A-B)*C=(-0.00390625,0.03125); these distinguish the paths by much more than
1e-3. Tiny slots are controls, NOT a claim of accuracy at their bit size.

## Required public behavior and independent observations

Two small stages, Add first and Sub second, with separate named CTests:
1. mult2_pair_add_input_hybrid_complex
   selector pair_add_input_hybrid_complex;
2. mult2_pair_sub_input_hybrid_complex
   selector pair_sub_input_hybrid_complex.

For EACH case:
- use a fresh genuine context/key pair and EvalMultKeyGen; Encrypt all three
  literal COMPLEX source vectors at the existing input scale/level;
- explicitly assert COMPLEX type and that a literal nonzero imaginary value
  survives plaintext construction before Encrypt (avoid a REAL fixture that
  silently discards it);
- call DCP on all three real inputs, then PUBLIC Add(A,B) or Sub(A,B);
- before multiplication, require ReadyForFirstMult and unchanged expected
  context/key/slots/ordered basis/level/recorded and logical scale/degree;
- independently decrypt source and composed pairs through the existing exact
  CRT/secret oracle, compare composed recombination to centered coefficient
  sum/difference of the TWO ORIGINAL operand recombinations. This expected
  value must not come from production arithmetic or from the composed result.
  Explicitly materialize BigInt +/- branches; avoid incompatible Boost
  expression-template conditional operators;
- exercise public staged Tensor2->Relin2->RS2 AND public Mult2(composed,C).
  Staged equality is composition wiring evidence only; do not use that
  equality as the arithmetic oracle;
- reuse the independent coefficient/negacyclic product certificate on the
  composed pair and C, including actual execution-specific non-wrap, separate
  high/low path errors, per-path conditional Relin2 gate, output state, and
  PUBLIC RCB logical decoded slots against the literal final product array;
- snapshot all original encrypted inputs and all original pairs before the
  Add/Sub+Mult2 window, plus the composed pair before Mult2, and check the
  existing enumerated nonmutation contract afterwards. Do not remove the
  fixture evaluation-key row before Mult2: this task REQUIRES that key.
  Do not claim hidden cache/parameter state coverage beyond actual checks;
- print a distinct case label, true HYBRID/COMPLEX identity and actual errors.
  Preserve PER_PATH_CONDITIONAL, conservative_E_Relin_available=false and
  universal_theorem_gate=UNPROVED. This task does not adjudicate the BV bound.

Do not treat test-only CRT coefficient recovery as a shipping output codec.
No expected value may be taken from the object's stale cache or a matching
production transform. Existing double decoding is only the existing 1e-3
functional observation, never a >53-bit oracle.

## TDD / evidence discipline

These operations ALREADY have genuine public API/runtime reds and greens.
This slice is ADDITIONAL regression coverage of an observed composition gap,
not a new missing-feature implementation and not an invented red-green cycle.
Do not deliberately break shipping code just to manufacture a red. Freeze the
new contracts/literals before hosted execution. If a new test naturally fails,
Codex will retain that exact failure before fixing its cause; no weakening.

Return two small sequential proposed patches (Add then Sub), with the second
preserving the first stage's behavior/assertions. Avoid speculative helper
frameworks or broad refactors. Pre-existing 53 test names, command bindings,
assertions, source bodies, thresholds and workflow remain intact.
Expected candidate counts are54 after Add and55 after Sub, NOT observed runs.
Describe cheap discriminating counterexamples (e.g. swapped Sub, dropping B,
wrong lifecycle) without claiming unexecuted mutation tests passed or failed.

## Required deliverables and static checks

Return one downloadable ZIP and SHA256 sidecar containing:
- REVIEW_AND_DESIGN.md with actual model identity, exact source, rationale,
  independent oracle derivation, risks, claim boundaries and finding mapping;
- patches/0001-add-input-first-mult2-regression.patch;
  patches/0002-sub-input-first-mult2-regression.patch;
  patches/candidate-final.patch;
- full final changed files under an unambiguous final-project/ root;
- exact old/new CMake test-name/command closure and preservation evidence;
- execution ledger distinguishing inspected, inferred, executed, NOT EXECUTED;
- complete required-file inventory, per-payload byte sizes/SHA256 manifest
  and output file tree. Every claimed patch/source/check must actually exist.

Apply both numbered patches to the exact provided project baseline in an
isolated scratch directory; verify cumulative equality with aggregate patch
and final files; check all required files nonempty. Inspect patches for no
production/workflow/other-test/old-contract mutation. Recheck manifests and
REQUIRED-MEMBER completeness after extracting the final ZIP into a fresh empty
directory. Do not include .git, dependencies, build outputs, caches, pyc,
runtime/browser state or credentials. A self-consistent empty/incomplete
manifest is NOT a complete delivery. Never write empty logs labeled PASS.

## Actual execution and acceptance

Pro may perform bounded static patch/manifest and exact dyadic arithmetic
checks. Do NOT assume installed OpenFHE or invent a build. No Mac project/
OpenFHE configure/compile/crypto/CTest/benchmark. No dependency install,
source/Git push/merge, CI dispatch/rerun/cancel, other-agent dispatch,
browser/session access or external message. Existing test-run logs are quoted
hosted evidence, not a run in your review environment.

Codex will integrate one stage at a time, run Linux GCC and Windows MinGW64
warning builds and focused plus full CTest via existing GitHub Actions,
preserve exact commits/logs, obtain independent review and promptly push.
If required sources or signatures are missing, identify exactly what is
missing rather than inventing them or assuming access.

Acceptance: a complete minimal test-only candidate at confirmed public seams,
frozen independent literal/CRT expectations, unchanged existing53 cases,
honest static/execution limits and no claim about >53-bit accuracy, repeated
multiplication, BV conservative theorem, Table3, security or performance.
Those remain active parts of the full project, not silently waived.
