# ChatGPT Pro task — Relin2 remediation 05 fail-closed audit repair

## Authority, background, and objective

This is a narrow replacement task for the clean-room `t=2` Relin2 slice of
paper 2023/1788 on pristine OpenFHE 1.5.0. It is not a new implementation task.
The remediation-04 production, public API, CMake/workflow, and executable tests
have already passed exact archive, input, patch, tree, protected-hash, and
tree-to-tree replay gates. Do not redesign or rewrite them.

Remediation 04 is rejected only because three patch-07 audit/evidence defects
can produce a false-green or cannot be replayed with the unversioned retained
command. Your objective is to return one minimal replacement seven-patch
archive whose patches 01-06 and executable candidate bytes are frozen, while
patch 07 repairs those exact audit/evidence defects and proves the repairs with
temporary negative fixtures.

Read every supplied attachment completely before acting. Do not assume access
to local files, private repositories, earlier browser state, or facts not
contained in the attachments. Treat the remediation-04 receipt as the
controlling independent review result. If any supplied size/hash identity does
not match the binding in the submission message, stop with `blocked`.

Your response must begin with exactly one of:

- `ready to apply`
- `changes needed`
- `blocked`

Only `ready to apply` may include a replacement archive.

## Exact attachment inventory

The submission must attach every file in this table with the exact local name,
size, and SHA-256 shown. Do not rely on an earlier conversation copy. The
ChatGPT UI may append exactly one browser-generated collision suffix of the
form ` (digits)` immediately before the extension. Print both the mounted name
and its normalized original basename; remove only that one final suffix for the
name comparison. No other rename or normalization is allowed, and byte size
plus SHA-256 remain controlling.

| # | Attachment | Bytes | SHA-256 |
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
| 10 | `chatgpt-pro-relin2-01-remediation-04-delivery.zip` | 206,292 | `61fa2b9ab16c79faf247338faeedf123d1329f0b905f75313709ff5154f28de8` |
| 11 | `relin2-remediation-04-receipt.md` | 8,188 | `82b56c5a5017caf3a59a53fa65524ce356b764e671b0b4388612c40514f65cd1` |

This task file, `chatgpt-pro-relin2-remediation-05.md`, is the twelfth
attachment. Its final size and SHA-256 cannot be self-bound here; the enclosing
submission message must bind both exact values. Before reading any content,
print and compare all twelve names, sizes, and SHA-256 values. Any mismatch
requires `blocked`.

## Fixed identities and non-negotiable boundaries

- Exact implementation base commit:
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`
- Exact base tree:
  `759d5195739684748d5a9664edabe3fa719e1acf`
- Pristine OpenFHE 1.5.0 commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- Rejected remediation-04 ZIP SHA-256:
  `61fa2b9ab16c79faf247338faeedf123d1329f0b905f75313709ff5154f28de8`
- Rejected remediation-04 final tree:
  `2b1a10ab6e4f776a620710e2c7dd2b214a237357`

Patches 01-06 must remain byte-identical:

| Patch | Required SHA-256 | Required cumulative tree |
|---|---|---|
| `01-red-relin2-api.patch` | `6296f34d9aafc62ea501c638189118393a5196f2e5f0f7ef552a9b94811d9f64` | `28d0294bce8844fa831951879020b353568c1c13` |
| `02-api-scaffold.patch` | `364a1689fa93cf98a65a69a70814d40e49650477668a0bf13d5fcaf63db5c57c` | `7ea2bf2db6f87ee357da2a68f198836c9fa30d4a` |
| `03-red-relin2-contract.patch` | `b68e182dfd3ada869af7c4a2941a7bb840030d15ed2a633733e2333fa5465e0a` | `ef19f3d7a55eee6767622c2b7210e00f7bdcfd54` |
| `04-green-relin2-core.patch` | `a6ad402d8d8dc38661916b2d1049ec7e034cea74948241922b9a95dbefff10e1` | `358215176f75e96af3dc3a1baa58842e9d33c2b5` |
| `05-red-tensor2-lifecycle.patch` | `c8d90f1a1e47056a8c4217fc58ccb3df5c54601f38edc3a1a866c375aee1101e` | `6ea2a0107e8cbc5239dcaafb6eb1d903ec6cac84` |
| `06-green-tensor2-lifecycle.patch` | `60bdfb21b007816ba626c5186c92ae4a909eae3360f85aecee80234a28ff396f` | `4f0d376c8be65ccfc3c9dded02921525bfec65b8` |

The following final candidate bytes must remain unchanged from remediation 04:

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

No production, header, workflow, CMake, or executable-test edit is authorized.
The exact paper composition remains `(u, v+w)` with exactly two public
output-returning `Relinearize` calls and the accepted lifecycle, scale,
metadata, Tensor immutability, deep-cache, HYBRID/BV, and diagnostic contracts.

Apply KISS and YAGNI. Do not add production `try`/`catch`, compatibility
wrappers, runtime audit hooks, alternative APIs, or generalized framework code.

## Required correction 1 — Python 3.9-compatible literal replay

`artifacts/tdd/relin2/audit_reference_scope.py:54` in remediation 04 embeds
backslash-bearing regular-expression expressions inside an f-string. It is a
`SyntaxError` under the acceptance host's Python 3.9 and 3.10, although it
passes under Python 3.14. The retained command is unversioned `python3`, and
`artifacts/tdd/relin2/14-environment.txt` omits Python identity/version.

Repair all retained audit programs so their syntax is compatible with Python
3.9. Do not merely change the command to a newer interpreter. At minimum,
precompute the regex/count outside the f-string or use a Python-3.9-safe
formatting expression; the rejected source line must be absent. Retain:

1. literal `python3 --version` output in the environment evidence;
2. `python3 -m py_compile` success for every shipped `audit_*.py` file, with
   bytecode/cache output directed outside the applied tree;
3. a source-regression assertion that the exact rejected backslash-bearing
   f-string construct is present in the remediation-04 input and absent from
   the replacement, with literal program/command, output, raw exit, and a
   separate assertion exit;
4. if an actual CPython 3.9, 3.10, or 3.11 interpreter exists in the supplied
   environment, its literal version plus compile and execution results; if none
   exists, say so and leave the pre-3.12 runtime gate pending for Codex rather
   than inferring it from a newer parser;
5. a fresh successful execution of every final shipped audit program against
   the exact final replay tree using the recorded source-agent interpreter.

Do not use `ast.parse(..., feature_version=(3, 9))` or a newer interpreter's
`py_compile` as proof of actual CPython-3.9 runtime compatibility: local
CPython 3.14 accepts the rejected construct under that nominal feature version.
No version-dependent syntax later than Python 3.9 may remain. Codex will run
the returned scripts with actual Python 3.9 and 3.10 before acceptance.

## Required correction 2 — enumerate every reference-closure call

The remediation-04 `audit_reference_scope.py:76-93` enumerates member calls,
`std` calls, and only four preselected free/constructor names. It does not
discover and reject every other non-member or namespace-qualified call. The
current `lbcrypto::NativePoly(...)` constructor is already unclassified.

Replace this with one C++ token/AST-aware, fail-closed call-expression audit
over the complete exact bodies of:

- `RaiseElement`;
- `RaiseHighReference`;
- `BuildReferenceRelinPaths`.

The audit must:

1. prove each named definition occurs exactly once and extract its exact
   brace-matched body without trusting fixed line ranges;
2. print the full extracted closure;
3. discover and print every call/construction expression in that closure,
   including member calls, free/helper calls, namespace-qualified calls, and
   constructors such as `DCRTPoly(...)` and `lbcrypto::NativePoly(...)`;
4. classify every discovered expression into an exact allow-list entry with an
   expected count and source location;
5. assert the discovered and classified multisets are equal in both directions;
6. retain exactly two allowed public `context->Relinearize(` calls and all
   existing forbidden private/production route checks;
7. fail on any unknown, missing, duplicate, indirect, local-callable, function
   pointer, macro-generated, or otherwise unclassified call route.

A fixed list that only counts desired names while ignoring undiscovered calls
is not acceptable. Prefer a Clang AST call-expression walk using the exact
compile command; a raw-token implementation is acceptable only if it proves
complete balanced ranges and classifies the entire callable token surface.

## Required correction 3 — exact Relin2 enclosing and containment proof

The remediation-04 `audit_relin2_calls.py:93-107,212-223` assigns enclosing
functions by the nearest hard-coded start line instead of exact function body
ranges. On failure paths it locates a prior
`CheckThrowsExactInvalidArgument` but never asserts that the Relin2 call token
occurs before that invocation's matching right parenthesis.

Repair the token/AST audit so it:

1. discovers all executable test/source scopes required by remediation 04;
2. derives every reported enclosing function/helper from exact balanced-brace
   or AST parent ranges, not nearest-start heuristics;
3. fails if any Relin2 call lies outside exactly one classified function body;
4. for every expected-failure site, identifies the exact
   `CheckThrowsExactInvalidArgument(...)` call range and proves
   `helper_open < Relin2_call < helper_matching_close`;
5. proves the Relin2 call is inside the diagnostic helper's lambda/callable
   argument, not merely elsewhere in the helper arguments or on a nearby line;
6. keeps the exact ten-call, six-success/four-failure, one unevaluated API node,
   no indirect route, pre/post observation, post-corruption snapshot, and exact
   diagnostic-helper semantics gates;
7. prints exact source ranges/locations and fails if discovered and classified
   sets differ in either direction.

## Required audit-mutation TDD

Use disposable copies outside the applied project tree. First run each repaired
audit unchanged and require green. Then make one minimal source-only mutation
for each case below, rerun the unchanged repaired audit, require the dedicated
named nonzero failure, restore byte-for-byte, and rerun green. Each mutated C++
fixture must first pass the exact Clang parse/AST command with zero diagnostics,
and the audit must print the mutated call's source location in its complete
discovered set before the named classification/containment assertion fails.

1. Declare a harmless probe type outside the closure and insert one
   syntactically valid unknown namespace-qualified construction/call, such as a
   temporary `audit_probe::UnknownCtor()`, into the extracted reference
   closure. Discovery must succeed; only the unknown-call/bijection assertion
   may fail.
2. Declare a harmless probe function outside the closure and insert one
   syntactically valid unknown unqualified helper call into that closure.
   Discovery must succeed; only the unknown-call/bijection assertion may fail.
3. Perform a count-preserving relocation of one existing successful Relin2 call
   and its required pre/post observations into a syntactically valid
   intervening helper placed where remediation 04's nearest-start heuristic
   attributes it to the former named function. Keep exactly ten calls, the
   six/four classification, and every non-target order/count gate green. The
   rejected remediation-04 audit must demonstrably false-green on this fixture;
   the repaired audit must fail only its exact enclosing-range assertion.
4. Perform a count-preserving relocation of one existing expected-failure
   Relin2 call from the diagnostic lambda to a nearby statement after both
   post-exception Tensor/cache comparison statements. Keep exactly ten calls,
   the six/four classification, all snapshot/corruption facts, and every
   non-target gate green. The rejected remediation-04 proximity audit must
   demonstrably false-green; the repaired audit must fail only its outer
   diagnostic-call containment assertion.
5. Perform a different count-preserving relocation that keeps one
   expected-failure Relin2 call inside the outer
   `CheckThrowsExactInvalidArgument(...)` argument range but moves it outside
   the lambda/callable body, for example into a syntactically valid comma
   expression in a later argument. Keep exactly ten calls and all non-target
   gates green. The repaired audit must fail only its lambda/callable-body
   containment assertion.

For each mutation retain the pre-mutation source and audit-program SHA-256,
clean-diff assertion, complete unified mutation diff, literal parse/discovery
and audit commands, raw stdout/stderr and exits, exact dedicated named failure,
restoration method, exact hash restoration, zero diff, and post-restoration
green. For mutations 3, 4, and 5 also retain the old remediation-04 audit's
exit-0 false-green result. Mutation helpers and temporary sources must not
become applied-project files.

Do not satisfy these tests by inserting deliberate failure text into the audit
output, changing the expected result during the mutated run, or selecting a
different audit program.

## Patch-07 and evidence scope

Build the replacement from the exact base by applying frozen patches 01-06,
then generate one replacement `07-final-docs.patch`. Relative to the complete
remediation-04 patch-07 result, only the following paths may differ:

- `artifacts/tdd/relin2/audit_reference_scope.py`;
- `artifacts/tdd/relin2/audit_relin2_calls.py`;
- directly affected `artifacts/tdd/relin2/*.txt` or exact raw-transcript files;
- `artifacts/tdd/relin2/INDEX.md`;
- `coordination/RELIN2_DESIGN.md` and `README.md` only where the audit or
  downstream-state wording must be made exact.

The replacement patch 07 must still reproduce every other remediation-04
documentation/evidence file that patch 07 added from the pre-07 tree. Do not
change the executable files frozen above. Do not rewrite unrelated mutation,
arithmetic, lifecycle, metadata, key-cache, or test-identity evidence. If an
existing retained file is unchanged, preserve it byte-for-byte.

The replacement `INDEX.md` must continue to bind only patches 01-06 and the
pre-07 tree, avoiding a patch-07 self-cycle. Its evidence-hash table must bind
exactly every regular file below `artifacts/tdd/relin2/`, sorted by relative
path, except `artifacts/tdd/relin2/INDEX.md` itself. External `TESTS.md` must
bind all seven patch hashes and the newly computed final tree.
`PATCHES.sha256` must bind the other nine archive files in exact order.

Retain a fresh exact-base, index-aware seven-patch replay with literal commands,
outputs, exits, and cumulative trees. Boundaries 01-06 must equal the fixed
trees above; independently compute the replacement patch-07 hash and final
tree. Run the complete final static audits after replay. Because executable
candidate bytes are frozen, the prior boundary red/green evidence remains
historical source-agent evidence; do not claim it was rerun unless it actually
was. If the existing build is available, also run one final warning-clean build
and unfiltered 37/37 CTest and record it accurately. Never substitute a partial
or filtered run.

## Required deliverables

Return exactly one ZIP named:

`chatgpt-pro-relin2-01-remediation-05-delivery.zip`

Its central directory must contain exactly these ten unencrypted regular
`100644` root files in this order:

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

`REVIEW.md` must state the bounded arithmetic/design conclusion, exact frozen
executable identities, corrected fail-closed audit design, mutation proof,
Python compatibility evidence, lifecycle/scale table, HYBRID/BV proof, and all
pending downstream work. `TESTS.md` must retain literal commands, raw outputs,
raw exits, separate assertion exits, hashes, cumulative trees, final replay,
and accurate executed-versus-retained distinctions.

First stage exactly the selected ten output files and scan that staged content.
Then create the ZIP, inspect its central directory, run archive integrity,
extract it into a fresh quarantine directory, recompute `PATCHES.sha256`, and
scan the actual freshly extracted contents again. Retain scanner/tool identity
and version, exact scopes, literal commands, raw outputs, and exits. Do not
include `.git`, build output, caches, database/runtime/browser state, `.env`,
API keys, tokens, cookies, private keys, credentials, source-agent helper
files, or temporary mutation material. If a dedicated secret scanner is
unavailable, state that accurately and retain targeted filename/content scan
commands/results; do not claim a tool you did not run.

In the response, report the ZIP size, SHA-256, replacement patch-07 SHA-256,
replacement final tree, frozen patch 01-06 hash results, frozen executable hash
results, audit-mutation matrix, Python compatibility result, final static audit
results, any actually rerun build/CTest result, and every pending environment.

## Required tests and acceptance criteria

Source-agent `ready to apply` eligibility requires all of the following except
the item explicitly identified as a downstream Codex receipt gate. Final
project acceptance additionally requires that downstream gate; do not present
it as source-agent evidence.

- every submitted input matches its message binding before inspection;
- patches 01-06 are byte-identical and reproduce their six fixed cumulative
  trees from the exact base;
- all eight frozen executable/project files match exactly;
- both repaired audit scripts use Python 3.9-compatible syntax;
- all shipped audit programs pass source-agent `py_compile` and fresh execution
  on the final tree; any available actual CPython 3.9-3.11 result is recorded
  without substitution, and actual Python 3.9/3.10 receipt execution remains a
  mandatory downstream Codex gate;
- the reference audit discovers, prints, exactly classifies, and bijectively
  accounts for every call/construction in the complete three-helper closure;
- the Relin2 audit uses exact enclosing ranges and proves exact diagnostic-call
  and lambda containment for every failure site;
- all five audit mutations produce their required unchanged-audit reds and
  restore byte-for-byte to green;
- final replay, hashes, docs, evidence index, external bindings, ZIP structure,
  archive integrity, and secret gates pass;
- production/tests remain unchanged, and no Windows, hosted same-SHA,
  project-commit/push/PR, Fable5, RS2, Mult2, pair Add/Sub, precision, or
  performance work is claimed.

Any unmet condition requires `changes needed` or `blocked`, never
`ready to apply`.

## Forbidden operations and forbidden claims

- Do not access the network, a Git remote, private repository, local user files,
  browser state, credentials, or hidden environment state.
- Do not commit, push, create a PR, dispatch CI, invoke Windows/Zcode/Zima, or
  invoke Fable5.
- Do not modify production, headers, workflow, CMake, or executable tests.
- Do not add runtime wrappers, fallback paths, `legacy_*`, production
  `try`/`catch`, or audit-only executable behavior.
- Do not alter an oracle, expected diagnostic, test dispatch, CTest
  registration, or mutation target.
- Do not claim Codex, Windows, hosted Linux/Windows, project integration,
  Fable5, RS2, Mult2, pair Add/Sub, plaintext precision, performance, or
  security results.
- Do not describe a finite desired-name count as a complete call-surface proof.
- Do not describe lexical proximity as exact function or diagnostic containment.
