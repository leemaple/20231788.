# Relin2 delivery receipt and isolated replay gate

Recorded: 2026-09-01 08:08 CST

Status: **archive and replay gates passed; candidate acceptance failed with
changes needed**. No returned patch has been applied to
`agent/codex-relin2-01`, compiled on the Mac, committed to the real branch, or
sent to CI.

## Received artifact identity

- Conversation:
  `https://chatgpt.com/c/6a960223-f7d8-83ec-9ad1-ac404f614ba9`.
- Packaging-only response: `ready to apply`, page-reported `Worked for 57s`.
- Download event GUID: `a09fb628-cc93-4c1c-b0c0-14ebc6d7a354`.
- The download control was clicked exactly once. The completed 32,652-byte
  browser download was moved from the default Downloads folder into the new
  quarantine directory; it was not downloaded again.
- File:
  `artifacts/incoming/chatgpt-pro-relin2-01/chatgpt-pro-relin2-01-delivery.zip`.
- Observed byte size: 32,652.
- Observed SHA-256:
  `cb17f339f8bc63b36edbd3f43cca1c517d4f450996b2dd1b850a6665f6a262a6`.
- The locally recomputed byte size and SHA-256 exactly match the page report.

## Archive and checksum gate

The central directory contains exactly ten regular root files, no duplicate,
directory, link, absolute path, backslash path, or `..` component:

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

`unzip -t` passed all ten entries. Fresh extraction into
`/tmp/20231788-relin2-delivery.PdyITd` followed by
`shasum -a 256 --check PATCHES.sha256` passed all nine listed files. The same
check passed again on the retained `output/` copy. Gitleaks 8.30.1 scanned both
the fresh extraction and retained output, about 120.54 KB each, with no leaks
found. Two earlier command attempts used the wrong relative working directory
and read zero files or could not find the listed files; those outputs are
explicitly excluded from evidence and were replaced by the successful
absolute-path/in-directory checks above.

## Isolated patch replay

A disposable `git clone --no-hardlinks` at
`/tmp/20231788-relin2-replay.q1BjZE/project` was detached at exact base
`fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`, tree
`759d5195739684748d5a9664edabe3fa719e1acf`.

For every patch in order 01 through 07, both
`git apply --check --whitespace=error-all` and actual `git apply` succeeded;
`git diff --check` remained clean. The fully replayed candidate tree is
`bd2edcaec7adaf7556c9fbfa7b65502a70f27b4a`. A disposable review-only commit
`49adc5baa5275f555998e3d04065582d241e396d` binds that tree for the formal
two-axis diff review. It is not a project commit or test result.

The final production tree contains none of the task's forbidden calls or
production `try`/`catch`. Production contains exactly two public
`Relinearize` call sites in `Relin2`; the independent reference contains no
`KeySwitchCore`, `EvalMultAndRelinearize`, `ModReduce`, or `Rescale` call.

When the exact returned patch files were staged as immutable receipt evidence,
repository-level `git diff --cached --check` reported trailing whitespace on
blank unified-diff context lines such as a single leading context marker. The
artifact bytes were deliberately not rewritten because that would break the
sender checksum. A path-scoped check of every coordination file passed, and
the isolated replay's `git apply --check --whitespace=error-all` plus
post-application `git diff --check` passed at every patch boundary. This is an
artifact-container formatting exception, not an accepted source-tree
whitespace error.

## Acceptance blockers

The returned `ready to apply` verdict is rejected pending a revised bundle:

1. **P1 — public RCB result is not tested.** The valid case manually rebuilds
   the recombination identity, then discards the return from
   `module.RCB(result)`. An RCB implementation that accepts the lifecycle but
   returns the wrong ciphertext would pass. The public result must be compared
   component/tower/coefficient against the independent oracle.
2. **P1 — required witnesses are not deterministic.** The K0/K1 and common
   nonzero v/w coordinates are found by scanning output from a randomly
   generated evaluation key. One observed run printed `(0,0,0)`, but the test
   neither fixes that coordinate nor algebraically forces its residue. The
   controlled A/B fixture only fixes centered DCP boundary/carry witnesses.
3. **P1 — the boundary test does not check production output.** It proves the
   public reference path reaches `+half`, `-half`, and carry, then discards the
   actual `Relin2` return. Wrong production residues at those boundaries would
   still pass.
4. **P1 — valid output state assertions are incomplete.** The test directly
   checks lifecycle, level/component count, degree/factor, and dual scales, but
   not exact context, divisor, key tag, slots, encoding, format, ordered basis,
   root/cyclotomic tower parameters, or member metadata. Reusing production
   validation is not an independent assertion.
5. **P1 — key-cache immutability is shallow.** The cache snapshot records only
   row names and shared-pointer addresses. It cannot detect or restore an
   in-place change to a key tag, A/B vector, format, parameters, or residues;
   a later reference call could then consume the same corrupted key and mask
   the mutation.
6. **P2 — two contract seams are weaker than specified.** Production binds the
   global key map as mutable `auto&` instead of a read-only reference, even
   though it does not currently mutate it. The compile-only Relin2 API test
   checks only that `&DoubleCKKS::Relin2` is some member-function pointer, not
   the exact required return, argument, and `const` signature.

Independent pristine OpenFHE/API review returned PASS. Independent paper and
adversarial-TDD reviews independently reproduced blockers 1 through 5.

## Formal two-axis review

Fixed point: `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.

Review-only candidate: `49adc5baa5275f555998e3d04065582d241e396d`.

Diff command:

```text
git diff fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9...49adc5baa5275f555998e3d04065582d241e396d
```

No build or test was run by either formal review agent.

### Standards

Hard violations:

- `README.md:17,29` / implementation-test hunks: the candidate adds Relin2
  production code and 29 CTest cases but contains no `TESTS.md`,
  `artifacts/tdd/relin2`, exact commands, environment, red output, or green
  outcome; the commit message records none either. This violates
  `engineering.md` Red-green-refactor rules 1/3 and Review's evidence rule,
  plus `GIT_CHECKPOINT_POLICY.md` rule 4/final retained-evidence requirement.
  README's reference to evidence "in the review delivery" does not make it
  part of this candidate.
- `tests/relin2_test.cpp:276,786,834-937`: the valid-path oracle starts from
  encrypted `{0.0}`, overwrites raw RNS elements, mirrors production's
  basis-raising steps, and invokes the same `Relinearize` API to form expected
  values. It never decrypts representative real/complex, near-zero,
  magnitude, or precision vectors. This falls short of `engineering.md`
  Derive-target-behavior rule 3 and README's "red-first independent-oracle
  tests" claim.
- `tests/relin2_test.cpp:237-711,1127-1136`: a copied Tensor2 harness and five
  unregistered `legacy_*` dispatch routes are retained solely to avoid
  unused-function warnings. That conflicts with `engineering.md`'s "smallest
  test" and "narrow interfaces / current acceptance test only" rules.

Judgement-call smells:

- **Duplicated Code / Divergent Change / Speculative Generality** — the same
  `tests/relin2_test.cpp` hunk embeds unrelated legacy Tensor2 tests,
  acknowledged by its own comment, instead of shared test support.
- **Mysterious Name** — `src/double_ckks.cpp:388-399,698-723` returns a semantic
  quotient/remainder decomposition as `std::pair` and accesses
  `.first`/`.second`; a named decomposition type would expose meaning.
- **Data Clumps** — `src/double_ckks.cpp:688-703` repeatedly passes basis,
  level, degree, scale, tag, slots, component count, and labels to
  `ValidateCiphertext`; bundle the expected ciphertext manifest.

### Spec

Missing or partial:

1. Exact diagnostics are not proved. Spec lines 480-482 require "a
   field-specific stable message," and lines 387-390 require the exact
   lifecycle diagnostic. `tests/relin2_test.cpp:51-55` accepts any message
   merely containing a substring.
2. Spec lines 450-455 require assertions for "result type/lifecycle, exact
   basis/tower parameters...context/tag/slots/encoding/format" and proof that
   public RCB "yields the exact recombined result."
   `tests/relin2_test.cpp:953-960` checks only a subset;
   `tests/relin2_test.cpp:963` discards `module.RCB(result)`. The exact check at
   `tests/relin2_test.cpp:917-936` manually recombines instead, so it cannot
   verify RCB's returned value.
3. Spec lines 507-508 say "Each malformed-key case must prove...Tensor inputs
   and static key map are unchanged." The absent-key case at
   `tests/relin2_test.cpp:985-990` takes neither snapshot.

Scope creep:

4. Spec lines 160-163 require "only Relin2 and the minimum changes needed."
   `tests/relin2_test.cpp:329-711` duplicates the existing Tensor2
   oracle/tests, then exposes five unregistered legacy commands at
   `tests/relin2_test.cpp:1128-1136` solely to suppress unused-function
   warnings. This is unrelated duplicate coverage; its copied scale oracle is
   also stale at `tests/relin2_test.cpp:605-606`.

Looks implemented but wrong:

5. Spec lines 192-195 require `ReadyForFirstMult` to keep "stable error
   texts." `src/double_ckks.cpp:420-421` merges wrong-level and wrong-basis
   failures, so a legacy wrong level now reports `"pair ordered RNS basis does
   not match its level"` instead of the prior level diagnostic.
   `src/double_ckks.cpp:430-432` likewise folds manifest degree into the
   generic scale diagnostic, changing legacy failure attribution.

Summary: Standards has six findings (three hard violations and three
judgement-call smells); its worst issue is the absence of retained red/green
evidence and an adequate independent public-input oracle in the candidate.
Spec has five findings; its worst issue is incomplete exact-output/public-RCB
proof, compounded by non-exact diagnostics and shallow key-cache evidence.

The two axes are reported independently and are not merged or reranked. Their
combined decision is `changes needed`; the candidate remains quarantined and
must not be applied to the real Relin2 branch.
