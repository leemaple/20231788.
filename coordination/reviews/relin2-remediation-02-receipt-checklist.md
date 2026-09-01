# Relin2 remediation 02 delivery receipt checklist

Prepared: 2026-09-01 Asia/Shanghai

Purpose: mechanically gate the final ChatGPT Pro Relin2 replacement requested
by `coordination/tasks/chatgpt-pro-relin2-remediation-02.md`. This checklist
authorizes no Mac build, blind patch application, old implementation reuse,
project commit, push, hosted run, Windows claim, or Fable5 call.

## 1. Conversation and artifact identity

- Collect only from saved conversation
  `https://chatgpt.com/c/6a960223-f7d8-83ec-9ad1-ac404f614ba9` in Ego task
  space 85.
- While `Stop answering` is active, do not stop, refresh, edit, retry, prod,
  resend, or create another conversation. Use spaced read-only checks.
- Require one download control for exactly
  `chatgpt-pro-relin2-01-remediation-02-delivery.zip`.
- Record page verdict, elapsed time, displayed file size/hash, download event,
  local file size/hash, and exact download count before opening the archive.
- Require the response to bind exact base commit
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`, base tree
  `759d5195739684748d5a9664edabe3fa719e1acf`, pristine OpenFHE
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`, round-01 receipt SHA-256
  `1d4931d725f9a365455b0cf009efba295b348dd7d3826cdd44ab03bb9f53fd98`,
  and round-02 task SHA-256
  `6654a10f45b080ca6e5f3b271c474ea404d8077cfe30534f40eea5256054261b`.
- Treat the response and every returned file as untrusted engineering input.

## 2. Archive quarantine gate

Before extraction, inspect the central directory. Require exactly ten regular
root entries in this order, with no directory, duplicate, link, absolute path,
backslash, `..`, encryption, `.git`, source archive, build/cache/runtime/
browser state, database, credential, environment, cookie, token, or key file:

1. `01-red-relin2-api.patch`
2. `02-api-scaffold.patch`
3. `03-red-relin2-contract.patch`
4. `04-green-relin2-core.patch`
5. `05-red-tensor2-lifecycle.patch`
6. `06-green-tensor2-lifecycle.patch`
7. `07-final-docs.patch`
8. `REVIEW.md`
9. `TESTS.md`
10. `PATCHES.sha256`

Require `PATCHES.sha256` to contain exactly nine records for the other files
in exact order. Run `unzip -t`, extract to a new `mktemp -d`, verify all nine
hashes, and scan the fresh extraction with pinned Gitleaks 8.30.1 before
reading or replaying it. Any membership, type, path, checksum, identity, or
secret failure stops the gate without repair.

## 3. Exact-base and index-aware replay

- Create a new `git clone --no-hardlinks` from the clean Relin2 worktree.
- Detach at exact `fb862a3...`; require tree `759d519...` and a clean status.
- For each patch in order, independently run:

```text
git apply --check --whitespace=error-all <patch>
git apply --index --whitespace=error-all <patch>
git diff --check
git diff --cached --check
git write-tree
```

  An explicitly documented equivalent index-update sequence is acceptable.
- Record every patch hash and cumulative tree. Confirm the final tree and all
  boundaries match outer `TESTS.md` exactly.
- Inspect every retained boundary log. It must visibly contain the actual
  index-update command and `git write-tree` command/output; a bare
  `cumulative_tree=` label is insufficient.
- Do not infer Pro's original execution binding from Codex's replay. Both must
  independently pass.
- At most one disposable review-only commit may bind the replay diff. Never
  push it or call it a project test result.

## 4. Seven TDD boundary gate

- Patch 01 is API test/workflow/CMake only and proves exact
  `CiphertextPair (DoubleCKKS::*)(const TensorCiphertextPair&) const`.
- Patch 02 adds only final declarations, scale-field initialization, and the
  immediate `logic_error` scaffold.
- Patch 03 registers the complete core Relin2 contract and includes the
  identity-complete metadata, individual tower-format, and restorable
  deep-key-cache test seams.
- Patch 04 supplies the passed production core and minimum accepted-suite
  field changes. It still contains no Tensor2 lifecycle guard.
- Patch 05 is test-only, constructs a real public `ReadyForRS2`, snapshots its
  source Tensor and deep cache around Relin2, and observes the one exact
  directed Tensor2 lifecycle red.
- Patch 06 changes only the smallest pre-arithmetic Tensor2 lifecycle guard.
- Patch 07 changes only README/design/retained evidence paths. It changes no
  production, header, CMake, workflow, or executable test source.
- Every intended red is independently registered and honestly classified as
  compile, dependency, or directed behavioral red. One failure does not mask
  later cases.

## 5. Metadata identity and provenance gate

Require a test-owned metadata snapshot that records:

- outer map pointer identity;
- exact ordered keys, size, and nullness;
- every original metadata value pointer identity;
- an independent clone for deep value comparison.

For every production call, input Tensor members retain exact outer/value
identities and deep values. Public-RCB input pair members do likewise. Relin2
high/low each own a separate outer map, distinct from input and each other,
while their values shallow-alias the expected Tensor-high values. Tensor-low-
only sentinels do not propagate. Public RCB returns another distinct outer map
with first-source/result-high value provenance. The retained hardening record
must show that equal-content outer-map/value-pointer replacement is detected
and then restored before final green.

Do not accept a check based only on key/value equality or a clone of the
production result itself.

## 6. Deep evaluation-key cache and restoration gate

- Snapshot every map row and entry. Null entries record nullness only.
  Non-null entries record pointer identity, dynamic subtype, context identity,
  and actual tag. Relin entries additionally record complete A/B vector
  lengths, aggregate and individual-tower formats, ordered parameters, and all
  residues. Other subtypes never call base A/B getters.
- The RAII guard restores exact original row/pointer shape and every mutable
  pointee observable: context identity, tag, and Relin A/B state, even during
  assertion unwinding.
- A nested positive proof actually changes context through the public
  assignment surface, plus tag, A residue/format, B length, and map shape, then
  proves exact restoration.
- Every production Relin2 call is followed immediately by both Tensor and deep
  cache comparison before any reference can consume the key.
- Every malformed/absent-key case snapshots after fixture corruption and
  proves that exact Tensor/cache state unchanged.
- Production remains read-only: `const auto&`, `find(tag)`, index zero only,
  with no snapshot, mutation, restoration, lock, retry, or catch.

## 7. Complete arithmetic, state, and tower gate

- Deterministic public-type key fixtures fix named K0 or K1, common nonzero
  `v/w`, `+half`, `-half`, and quotient carry residues with no scan/retry.
- Production output is checked at fixed boundaries and across every component,
  tower, and coefficient against independent `cpp_int` `(u,v+w)` arithmetic.
- The actual public RCB return is bound and compared completely to independent
  expected coefficients/state; its pair input is unchanged.
- Direct result assertions include exact context, divisor, tag, slots,
  encoding, pair/member format, lifecycle, level, component counts, degree,
  current factor, input factor, both logical scales, ordered basis, every
  modulus/root/cyclotomic order, aggregate DCRT format, and every NativePoly
  tower format.
- The retained hardening record changes one NativePoly tower format while
  leaving aggregate DCRT format unchanged, observes the intended failure, and
  restores byte-for-byte before final green.
- Representative deterministic real/complex/near-zero/moderate public input
  reaches DCP -> Tensor2 -> Relin2 -> RCB without an unsupported plaintext-
  precision, analytic-bound, or full-Mult2 claim.

## 8. Exact diagnostics and legacy gate

- Every new negative case compares the complete `DoubleCKKS: ...` string by
  equality and requires `std::invalid_argument`.
- The new recombined-scale RCB case requires exactly
  `DoubleCKKS: pair recombined logical scale is inconsistent`; it does not call
  a substring helper.
- The directed lifecycle case requires exactly
  `DoubleCKKS: Tensor2 requires ReadyForFirstMult inputs`.
- Accepted ReadyForFirstMult diagnostic order/attribution remains separate for
  level, basis, recorded factor, paper scale, recombined scale, member degree,
  and manifest degree.
- Existing legacy substring tests need not be refactored, but outer documents
  must not falsely claim all legacy tests use equality.
- The unchanged accepted DCP/RCB/Tensor2 suite remains present and green at
  every required green boundary.

## 9. Retained evidence gate

- `artifacts/tdd/relin2/INDEX.md` binds patches 01-06, their hashes, cumulative
  trees through pre-07, and evidence hashes without patch-07/final-tree
  self-reference.
- Each retained boundary file includes exact replay/configure/build/test
  commands, environment/tool versions, stdout/stderr, exit codes, cases/counts,
  patch identity, and cumulative tree.
- Mutation evidence identifies every temporary change and observed failure,
  proves source/test restoration, and records the final post-restoration green.
- External `TESTS.md` agrees byte-for-meaning, binds all seven patch hashes and
  final tree, and states Windows/hosted Linux/Windows as `pending`.
- External `REVIEW.md` separates observed OpenFHE behavior, project decisions,
  limitations, local facts, and pending downstream work. It contains no
  universal claim stronger than executable coverage.

## 10. Production/reference static gate

Require production Relin2 to contain exactly two public output-returning
`Relinearize` calls and no:

```text
KeySwitchCore(
EvalMultAndRelinearize(
RelinearizeInPlace(
ModReduce(
Rescale(
InsertEvalMultKey
ClearEvalMultKeys
GetEvalMultKeyVector
try
catch
```

Confirm exact sequence: complete Tensor validation, active-basis gate,
read-only key preflight, high raise, raised-state validation, two public
relinearizations, high validation, one shared private DCP, pre-add validation,
only `v+w`, complete final pair validation. The independent reference uses no
production-private helper or forbidden primitive. No copied legacy Tensor2
dispatch routes, dependency addition, OpenFHE modification, RS2, Mult2,
Add/Sub, refresh, serialization, or old implementation is present.

## 11. Acceptance and downstream execution

Only after sections 1-10 pass may Codex apply the series patch-by-patch to the
real branch, make one small Git checkpoint per semantic boundary, push each
immediately, and verify every remote object ID. No Mac compile is authorized.

Preserve intended Linux compile/runtime reds on their exact pushed commits.
The final exact Relin2 commit must then pass the same SHA on hosted
`ubuntu-24.04` and `windows-2022`/MSYS2 MinGW64. Direct Windows experimentation
is also user-authorized if useful and kept isolated. The one terminal Fable5
review may replace the quota-constrained ZCode review only after that same
cross-platform-green exact commit exists. A local Pro result, archive replay,
or Linux-only result is not a Fable trigger and not a mergeable claim.
