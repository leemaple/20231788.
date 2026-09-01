# Relin2 remediation 04 mechanical receipt checklist

Recorded: 2026-09-01 Asia/Shanghai

Status: **prepared and independently checked before receipt**. This document is
a deterministic acceptance checklist, not a claim that ChatGPT Pro has
finished, that a remediation-04 ZIP exists, or that any build/test passed.

## Fixed identities

- Real implementation branch: `agent/codex-relin2-01`
- Exact untouched base commit:
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`
- Exact base tree: `759d5195739684748d5a9664edabe3fa719e1acf`
- Pristine OpenFHE 1.5.0 commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- Rejected remediation-03 final tree:
  `342436fab92fa2daf005ec557ee300cbe330122e`
- Controlling remediation-04 task: `25,618` bytes, SHA-256
  `23bd11960f688ce613e6f043e1e667b82d958db943d1055f1c3e1bc2cdfd824d`

All nine submitted inputs must match before any returned work is inspected:

| # | Input | Bytes | SHA-256 |
|---:|---|---:|---|
| 1 | `20231788-cleanroom-relin2-base-fb862a3-ci33436252725.zip` | 9,115,214 | `3e839a6b88a81107657442a2bb4f6b08385f6a24685cab11968db540436750f6` |
| 2 | `20231788-cleanroom-relin2-base-fb862a3-ci33436252725.binding.md` | 3,640 | `3320efa8723f0c519da453a006617c328de5bfa2aca72392a6161c66a0489d2f` |
| 3 | `chatgpt-pro-relin2-01.md` | 32,866 | `9323d631de4c6f6418ba1aa597332206a765846d0510a275df42a7a64b0a7513` |
| 4 | `chatgpt-pro-relin2-remediation-01.md` | 22,286 | `fda97960fa60942f255f3d43195fe3deb31b68d7709c9529ae90c1bb7dea1548` |
| 5 | `chatgpt-pro-relin2-remediation-02.md` | 16,329 | `6654a10f45b080ca6e5f3b271c474ea404d8077cfe30534f40eea5256054261b` |
| 6 | `chatgpt-pro-relin2-remediation-03.md` | 15,824 | `712f48dceb73eea675f035c3cbefa9b7a9e730c3264f60bd62bed33f8a05aa0c` |
| 7 | `chatgpt-pro-relin2-01-remediation-03-delivery.zip` | 49,641 | `cd3c5c43214023c997ee2a1ab7802cc096c293314349c392b509fe92330ca2a0` |
| 8 | `relin2-remediation-03-receipt.md` | 11,626 | `ef4ffe0b89976f55f8ee4d05fe19f6df391b3f203fe26c843e20c0f18ae4b8a9` |
| 9 | `chatgpt-pro-relin2-remediation-04.md` | 25,618 | `23bd11960f688ce613e6f043e1e667b82d958db943d1055f1c3e1bc2cdfd824d` |

Any input mismatch requires the Pro response to be `blocked`; a candidate that
continued after a mismatch is rejected.

Before reading a returned patch, require the real branch, base commit/tree, and
clean worktree to match. The response must begin with exactly one of `ready to
apply`, `changes needed`, or `blocked`. Only `ready to apply` may supply a
candidate for acceptance. Its single download must be named exactly
`chatgpt-pro-relin2-01-remediation-04-delivery.zip`; local size/SHA-256 must
first match the page statement. One completed download event is allowed.

## Archive and secret gate

Inspect the central directory before extraction. It must contain exactly these
ten unencrypted regular `100644` root entries, in this order, with no duplicate,
directory, link, slash, backslash, or unsafe path:

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

Run `unzip -t`, extract to a fresh quarantine directory, and require
`PATCHES.sha256` to contain exactly nine ordered records for the first nine
files, excluding itself. Recompute all nine hashes. Run pinned Gitleaks 8.30.1
on the extraction plus targeted credential/private-key/browser-state filename
and content scans. If generic hash-like findings occur, review every finding
and create an R4-specific exact-fingerprint allow-list only for proved
patch/tree/hash identities; never reuse remediation-03's allow-list.

## Exact-base index-aware replay

First replay the rejected remediation-03 ZIP independently from the fixed base
and require its exact final tree `342436fab92fa2daf005ec557ee300cbe330122e`.
Then use a separate fresh `git clone --no-hardlinks` detached at the fixed base
for remediation 04. For each patch, retain command/output/exit for:

```text
git apply --check --whitespace=error-all <patch>
git apply --index --whitespace=error-all <patch>
git diff --check
git diff --cached --check
git write-tree
```

The cumulative trees must be:

| Boundary | Required tree |
|---|---|
| base | `759d5195739684748d5a9664edabe3fa719e1acf` |
| 01 | `28d0294bce8844fa831951879020b353568c1c13` |
| 02 | `7ea2bf2db6f87ee357da2a68f198836c9fa30d4a` |
| 03 | `ef19f3d7a55eee6767622c2b7210e00f7bdcfd54` |
| 04 | `358215176f75e96af3dc3a1baa58842e9d33c2b5` |
| 05 | `6ea2a0107e8cbc5239dcaafb6eb1d903ec6cac84` |
| 06 | `4f0d376c8be65ccfc3c9dded02921525bfec65b8` |
| 07 | Independently computed at receipt; must then equal external `TESTS.md` |

Codex independently reproduced the base through boundary 06 in disposable
`/tmp/relin2-r4-expect.3KAkOG/repo` without compiling. The values above were
computed before any remediation-04 response was available; they are not copied
from a future Pro claim.

After R4 replay, create at most one disposable never-pushed review commit and
retain the literal full command/output for the complete
remediation-03-final-to-R4-final diff. Outside docs/evidence, that diff must be
the exact six-line reorder described below. A selected-file summary is not a
substitute for the full diff.

## Patch identity and scope

These four patches must be byte-identical to remediation 03:

| Patch | Required SHA-256 |
|---|---|
| 01 | `6296f34d9aafc62ea501c638189118393a5196f2e5f0f7ef552a9b94811d9f64` |
| 02 | `364a1689fa93cf98a65a69a70814d40e49650477668a0bf13d5fcaf63db5c57c` |
| 04 | `a6ad402d8d8dc38661916b2d1049ec7e034cea74948241922b9a95dbefff10e1` |
| 06 | `60bdfb21b007816ba626c5186c92ae4a909eae3360f85aecee80234a28ff396f` |

Applying the only allowed three snapshot reorderings produced these independent
reference identities:

- patch 03 test blob:
  `e422c61bfd1cc210a6a7d0f3dab8a18d66985ffe`;
- patch 05 SHA-256:
  `c8d90f1a1e47056a8c4217fc58ccb3df5c54601f38edc3a1a866c375aee1101e`;
- patch 05 test blob:
  `7f04970cf56cce051336c63abc315c9a22261ff8`.

For information only, the local calibrated generator serialized patch 03 as
SHA-256
`b68e182dfd3ada869af7c4a2941a7bb840030d15ed2a633733e2333fa5465e0a`.
The controlling task does not freeze that serialization: a different patch-03
hunk context/header is acceptable only if exact-base application produces the
required blob/tree and the complete semantic diff contains precisely the three
allowed reorderings. After normalizing only its full-index line, the
independently generated patch 05 was byte-identical to remediation 03; any
other patch-05 textual difference is a hard stop.

Required patch paths:

- 01: workflow, CMake, API contract test;
- 02: public header and production source;
- 03: CMake and `tests/relin2_test.cpp`;
- 04: header, production, DCP/RCB test, Tensor2 test;
- 05: CMake and Relin2 test;
- 06: production source only;
- 07: README, `coordination/RELIN2_DESIGN.md`, and
  `artifacts/tdd/relin2/**` only.

## Protected final content

Require these final SHA-256 values:

```text
96db76b6911bea7f7c1481efdadbb051f1c7b303a694be878bbb1853a8639e3f  src/double_ckks.cpp
0236736b8e34f71fdd871fb8fc5e338eb48d713eef5dd20258633a67f788d1f1  include/openfhe_2023_1788/double_ckks.h
1fa5c59a1563ea0bac08165cfebf13485af6aa2f7ae7087c89bcec1927b02578  CMakeLists.txt
a4fb9d364a767c42edeb4e0b326c01ae1e82ac4c9c3a01808a73592483616d9e  .github/workflows/dcp-rcb.yml
beb666576882be8f8eefc7224be33c95d9e82378e87a5d3c239eb03df3c35396  tests/relin2_api_contract_test.cpp
01f3fa263f8f411bc6f4a8f6e7b8e6b1dbec1eb4c7609260105d388250892c85  tests/dcp_rcb_test.cpp
8f8a19165134e2c984e3fa46a480eefe3b835db2cf332fc15a1ad69a914f17c2  tests/tensor2_test.cpp
c7b3f36b4da29c2a57e41ef9d7f6a4840ff71b30cf04af722e3689065ee46e43  tests/relin2_test.cpp
```

The remediation-03-to-04 final diff outside docs/evidence must contain only
the same six metadata-snapshot lines moved in exactly these functions, with no
line addition, deletion, or textual change:

- `TestValidArithmeticStateImmutability`;
- `TestControlledWitnessesAndBoundaries`;
- `TestRepresentativePublicInput`.

This must mechanically preserve `ResolveTest`, `main`, every other test body,
CMake registration, workflow, public API, and production behavior.

## Fail-closed evidence review

Require retained literal programs/commands, raw output/exit, and separate
assertion exits proving:

- all ten executable Relin2 member-call sites are discovered over the complete
  executable C++/test scope by a token/AST-aware method that strips comments,
  ordinary/character/raw strings and does not assume a receiver name. The
  retained output prints every site's line, enclosing test/helper, and
  success/failure classification;
- the discovered/classified sets are mutually equal: six success and four
  failure. Every success has complete Tensor/cache snapshots as the final two
  observations before production and their comparisons as the first two
  statements after return. Every failure snapshots after intentional fixture
  corruption, invokes production inside the exact `std::invalid_argument`
  diagnostic assertion, then performs the two immutability comparisons as the
  first two post-exception observations;
- no runtime pointer-to-member/alias/wrapper/macro/`std::invoke` route, with
  only the exact frozen unevaluated API `decltype(&DoubleCKKS::Relin2)` node
  allowed. That node must occur exactly once inside the frozen `std::is_same_v`
  static assertion, create/store no callable value, and be the only
  address-of/member-pointer reference;
- the independent-reference extraction contains the complete bodies of
  `RaiseElement`, `RaiseHighReference`, and `BuildReferenceRelinPaths`, each
  definition exactly once, no reference helper outside the closure, zero
  inherited forbidden primitive/path, and exactly two allowed public
  `context->Relinearize(` calls. Retain the full extracted scope and discover
  every call expression in it; print and classify every call, assert exact
  per-class counts, and require the discovered and classified sets to be equal
  in both directions. Any unknown public primitive, local callable, indirect
  route, or other unclassified call fails the gate;
- frozen production hash plus complete key-validator/Relin2 bodies and the
  exact read-only cache binding/find/index-zero proof, not a partial blacklist;
- exactly two public output-returning `Relinearize` calls, no production
  `try`/`catch`, no forbidden primitive, and no `legacy_*` route;
- all six test modes (`key_context_restore`, `metadata_equal_clone`,
  `native_tower_format`, `result_state`, `fixed_k`, `fixed_w`) and all three
  production modes (`dcp_sign_carry`, `public_rcb`, `key_cache`) with the real
  unified diff, proved underlying state/expression change, unchanged named
  oracle failure, exact restoration hash/zero diff, and post-restoration
  warning-clean final green;
- `key_context_restore` disables only the guard's base/context restoration and
  is detected by the unchanged post-scope cache oracle, with no post-scope
  injected mutation;
- `metadata_equal_clone` changes actual map/value identity while preserving
  deep value; `native_tower_format` changes one actual tower but not aggregate
  format; `result_state` corrupts named actual result state; `fixed_k`/`fixed_w`
  perturb named observed residues, not expected values; and the three
  production modes change only their named actual production expression/path;
- no mutation inserts a deliberate throw/failure text, changes an oracle,
  expected value, diagnostic, dispatch, registration, or test selection, or
  empties/aliases a test. Before any mode is run for each mutated
  test/production binary, that binary first passes the unfiltered
  no-environment 37/37 suite. Each mode prints a distinct
  `HARDENING_MUTATION_APPLIED` before/after state record before the unchanged
  named oracle fails;
- every mutation record starts with pre-mutation source SHA-256 and clean-diff
  assertion, embeds the complete nonempty unified diff plus an assertion of the
  exact changed file, and retains the literal restoration command/method,
  original byte hash, and zero-diff assertion. Temporary injection programs or
  mutation patches must not become patch-07 project files; the retained logs
  embed their complete reproducible effects;
- README names exact candidate branch `agent/codex-relin2-01` and accepted base
  `fb862a3...`, calls Relin2 a not-yet-integrated candidate, labels Pro Linux
  only source-agent evidence, leaves project commits/pushes, hosted
  Linux/Windows and Fable5 pending, and keeps RS2/Mult2/pair Add/Sub outside the
  slice. It removes the mutually contradictory old claims that Tensor2 is the
  current unverified slice, Relin2 is unimplemented, and a Relin2 candidate
  simultaneously exists;
- the preserved lifecycle/scale and HYBRID/BV proof remains exact.

Fresh boundary evidence must also retain the exact red/green matrix rather than
only a final run:

| Boundary | Required local Linux observation |
|---|---|
| 01 | Production library builds; API contract is the exact compile red |
| 02 | Accepted suite `6/6` green on immediate-throw scaffold |
| 03 | Accepted six green plus all 30 named Relin2 scaffold/dependency reds, independently executed |
| 04 | Core suite `36/36` green without lifecycle guard |
| 05 | Exactly `36/37`, only the directed lifecycle red |
| 06 | Final implementation `37/37` green |
| 07 | Final replay `37/37` green with docs/evidence applied |

Each boundary must retain its exact commands, raw output/exit, environment,
patch/tree identity, and warning-clean C++17 build status. A dependency failure
or the first runtime failure cannot stand in for later unexecuted cases.

## Executable test identity

Parse the retained complete `ctest --show-only=json-v1` record and require:

- 37 tests, 37 unique names, and 37 unique complete command tuples;
- every actual name/command pair exactly equals the frozen `CMakeLists.txt`
  registration;
- 31 distinct `relin2_test` arguments bijective with the unchanged 31
  `ResolveTest` targets;
- no `DISABLED`, `SKIP_RETURN_CODE`, skip regex, `WILL_FAIL`, alias, or
  substitute route;
- the literal assertion program, full JSON/raw output, and exit are retained;
  both the executable-test identity assertion and the complete
  remediation-03-to-04 diff assertion are rerun after all mutation restoration
  and before packaging;
- final unfiltered `ctest --test-dir build --output-on-failure` exactly 37/37.

Patch 07's `artifacts/tdd/relin2/INDEX.md` must bind patches 01-06, their
hashes, cumulative trees through pre-07 tree `4f0d376...`, and every retained
evidence file without binding patch 07 or creating a self-cycle. External
`TESTS.md` must independently bind all seven patch hashes and the final tree;
`PATCHES.sha256` binds the other nine ZIP files.

External `REVIEW.md`/`TESTS.md` must preserve remediation-03's exact
arithmetic/state, metadata identity/provenance, deep key-cache, per-tower
format, exact diagnostic, public RCB, representative-input, lifecycle/scale,
HYBRID/BV, and accepted-regression gates. They must not claim a partial regex
proves universal C++ cache immutability and must not claim independent
verification of OpenFHE relinearization when the allowed reference uses that
same trusted public primitive.

The Pro Linux build/result remains source-agent evidence. Do not call it Codex,
hosted, or Windows evidence. Only after every receipt/static gate passes may
the seven patches enter the real implementation branch. Follow
`coordination/reviews/relin2-downstream-ci-execution-audit.md`: keep exactly
seven semantic commits on the implementation branch, retain hosted records on
a separate non-triggering evidence ref, cancel intermediate Actions only after
Linux evidence is closed, and require the patch-07 SHA to pass Linux and
Windows together before the one authorized terminal Fable5 review.
