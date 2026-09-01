# ChatGPT Pro task — Relin2 remediation 06 complete callable-surface repair

## Authority, background, and objective

This is one narrow replacement task for the clean-room `t=2` Relin2 slice of
paper 2023/1788 on pristine OpenFHE 1.5.0. It is not a new implementation task.
The remediation-05 production, public API, CMake/workflow, and executable tests
passed exact archive, hash, tree, protected-byte, Python 3.9/3.10 baseline, and
bounded arithmetic gates. Do not redesign or rewrite them.

Remediation 05 is rejected because its patch-07 audits can still false-green on
indirect/local/macro call routes and on a failure call placed in the wrong
diagnostic-helper argument. It also dropped exact helper/corruption gates,
terminates before printing the complete non-target mutation result, and omits
required source-agent archive/build transcripts. Your objective is to return
one minimal replacement seven-patch archive whose patches 01–06 and all
executable candidate bytes are frozen, while patch 07 closes the complete
callable surface and repairs the exact evidence omissions.

Read every supplied attachment completely before acting. Do not assume access
to local files, private repositories, prior browser state, earlier conversation
attachments, or facts not present in this submission. Treat
`relin2-remediation-05-receipt.md` as the controlling independent review. If any
supplied identity does not match the binding below or in the submission
message, stop with `blocked`.

Your response must begin with exactly one of:

- `ready to apply`
- `changes needed`
- `blocked`

Only `ready to apply` may include a replacement archive.

## Exact attachment inventory

The submission must attach every file in this table with the exact local name,
size, and SHA-256 shown. The ChatGPT UI may append exactly one browser-generated
collision suffix of the form ` (digits)` immediately before the extension.
Print both the mounted name and normalized original basename; remove only that
one final suffix for name comparison. No other rename or normalization is
allowed. Byte size and SHA-256 remain controlling.

| # | Attachment | Bytes | SHA-256 |
|---:|---|---:|---|
| 1 | `20231788-cleanroom-relin2-base-fb862a3-ci33436252725.zip` | 9,115,214 | `3e839a6b88a81107657442a2bb4f6b08385f6a24685cab11968db540436750f6` |
| 2 | `20231788-cleanroom-relin2-base-fb862a3-ci33436252725.binding.md` | 3,640 | `3320efa8723f0c519da453a006617c328de5bfa2aca72392a6161c66a0489d2f` |
| 3 | `chatgpt-pro-relin2-01.md` | 32,866 | `9323d631de4c6f6418ba1aa597332206a765846d0510a275df42a7a64b0a7513` |
| 4 | `chatgpt-pro-relin2-01-remediation-03-delivery.zip` | 49,641 | `cd3c5c43214023c997ee2a1ab7802cc096c293314349c392b509fe92330ca2a0` |
| 5 | `chatgpt-pro-relin2-remediation-04.md` | 25,618 | `23bd11960f688ce613e6f043e1e667b82d958db943d1055f1c3e1bc2cdfd824d` |
| 6 | `chatgpt-pro-relin2-01-remediation-04-delivery.zip` | 206,292 | `61fa2b9ab16c79faf247338faeedf123d1329f0b905f75313709ff5154f28de8` |
| 7 | `relin2-remediation-04-receipt.md` | 8,188 | `82b56c5a5017caf3a59a53fa65524ce356b764e671b0b4388612c40514f65cd1` |
| 8 | `chatgpt-pro-relin2-remediation-05.md` | 21,525 | `2c5e9d92479665fc0c8f96de8b220ba3060f11d7b31410e0f0221aaa5f385c64` |
| 9 | `chatgpt-pro-relin2-01-remediation-05-delivery.zip` | 178,953 | `06054658322b7bf6de1883a1c0cafb0ea118e452bd3a52f3e8d23be0a409bc45` |
| 10 | `relin2-remediation-05-receipt.md` | 12,925 | `0ac897e1cfdd93d697c2dbd1572e2142d1b2687338084e0c236a8ea4564efde7` |

This task file, `chatgpt-pro-relin2-remediation-06.md`, is attachment eleven.
Its final size and SHA-256 cannot be self-bound here; the enclosing submission
message must bind both exact values. Before reading substantive content, print
and compare all eleven names, sizes, and SHA-256 values. Any mismatch requires
`blocked`.

## Fixed identities and non-negotiable boundaries

- Exact implementation base commit:
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`
- Exact base tree:
  `759d5195739684748d5a9664edabe3fa719e1acf`
- Pristine OpenFHE 1.5.0 commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- Rejected remediation-05 ZIP SHA-256:
  `06054658322b7bf6de1883a1c0cafb0ea118e452bd3a52f3e8d23be0a409bc45`
- Rejected remediation-05 patch-07 SHA-256:
  `e9e43e0ad568aa87652feebf17f9335ce717df322db4d92992e8767c16cd35e3`
- Rejected remediation-05 final tree:
  `d9e6307f4fe3a278cb29727169c6396d26a897a0`

Patches 01–06 must remain byte-identical:

| Patch | Required SHA-256 | Required cumulative tree |
|---|---|---|
| `01-red-relin2-api.patch` | `6296f34d9aafc62ea501c638189118393a5196f2e5f0f7ef552a9b94811d9f64` | `28d0294bce8844fa831951879020b353568c1c13` |
| `02-api-scaffold.patch` | `364a1689fa93cf98a65a69a70814d40e49650477668a0bf13d5fcaf63db5c57c` | `7ea2bf2db6f87ee357da2a68f198836c9fa30d4a` |
| `03-red-relin2-contract.patch` | `b68e182dfd3ada869af7c4a2941a7bb840030d15ed2a633733e2333fa5465e0a` | `ef19f3d7a55eee6767622c2b7210e00f7bdcfd54` |
| `04-green-relin2-core.patch` | `a6ad402d8d8dc38661916b2d1049ec7e034cea74948241922b9a95dbefff10e1` | `358215176f75e96af3dc3a1baa58842e9d33c2b5` |
| `05-red-tensor2-lifecycle.patch` | `c8d90f1a1e47056a8c4217fc58ccb3df5c54601f38edc3a1a866c375aee1101e` | `6ea2a0107e8cbc5239dcaafb6eb1d903ec6cac84` |
| `06-green-tensor2-lifecycle.patch` | `60bdfb21b007816ba626c5186c92ae4a909eae3360f85aecee80234a28ff396f` | `4f0d376c8be65ccfc3c9dded02921525bfec65b8` |

The following final candidate bytes must remain unchanged from remediation 05:

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
metadata, Tensor immutability, deep-cache, HYBRID/BV, diagnostic, and oracle
contracts. Apply KISS and YAGNI. Do not add production `try`/`catch`, runtime
audit hooks, compatibility wrappers, alternate APIs, or generic framework code.

## Required correction 1 — complete reference callable/construction surface

Remediation 05's `audit_reference_scope.py` recognizes a parenthesized call only
when `(` is preceded by an identifier or template `>`. Every other shape is
silently returned as `None`. Codex reproduced a false green by inserting this
valid local function-pointer call into the printed `RaiseElement` body:

```cpp
auto auditProbe = [] {};
auto auditProbePtr = +auditProbe;
(*auditProbePtr)();
```

The unchanged audit printed the injected body, still discovered exactly 23
calls, and exited zero under actual CPython 3.9 plus Clang raw tokenization.

Replace the audit with one complete, fail-closed raw-plus-expanded/semantic
callable and construction inventory over the exact balanced bodies of:

- `RaiseElement`;
- `RaiseHighReference`;
- `BuildReferenceRelinPaths`.

The repaired audit must:

1. prove every named definition occurs exactly once and print its exact source
   and balanced token/AST range;
2. print the complete extracted closure before any assertion can terminate;
3. define and print one exact AST/source taxonomy before counting: every
   source-spelled `CallExpr`, `CXXMemberCallExpr`, `CXXConstructExpr`,
   `CXXOperatorCallExpr`, `CXXNewExpr`, callable macro expansion, and equivalent
   explicit callable node is in scope; compiler-injected implicit nodes may be
   excluded only when the AST marks them implicit and the exclusion is printed;
4. discover every explicit call and construction form, including member,
   free/helper, namespace-qualified, ordinary constructor, brace construction,
   function object, immediately invoked lambda/local callable, function pointer,
   pointer-to-member, overloaded `operator()`, `new`, macro-generated call, and
   token-paste expansion;
5. inventory and classify every potential call/control/grouping token surface;
   there may be no silent `None`, ignored parenthesis, ignored call-like brace,
   or unexamined preprocessor expansion path;
6. print both raw-source and expanded/semantic source locations and prove the
   discovered and classified multisets equal in both directions;
7. retain the legacy audit's 23 explicit parenthesized calls/constructions as a
   separately labelled cross-check, not as the complete semantic total. Derive,
   print, and bind the exact complete baseline multiset under the taxonomy above,
   including source-spelled overloaded operators such as `tower *= divisor`;
   retain exactly two trusted public `context->Relinearize` calls;
8. reject every indirect/local/macro/wrapper/alias/function-pointer route even
   if it preserves the allowed-name count;
9. retain all existing forbidden private/production route and unique-definition
   checks.

A fixed desired-name list combined with ignored syntactic forms is not a
complete inventory. Prefer a Clang AST/preprocessing-record walk using the exact
compile command. A raw-token solution is acceptable only if it mechanically
accounts for the entire potential callable and macro surface, not only the
current desired spellings. A frozen exact closure identity may be an additional
defense but may not replace discovery and classification.

## Required correction 2 — complete Relin2 route and exact callable argument

Remediation 05 scans only raw identifiers spelled literally `Relin2`. It does
not close token-paste macros, wrappers, aliases, `std::invoke`, member pointers,
or other expanded/indirect routes. It also accepts a Relin2 call in any lambda
anywhere inside the diagnostic helper's outer argument range.

Codex reproduced the second false green by replacing the missing-key case's
first callable with an empty lambda and moving `module.Relin2(tensor)` into an
immediately invoked lambda in the later expected-message argument. The call is
evaluated before the helper can invoke or catch it, yet the audit still reported
ten calls, six success/four failure, one diagnostic container, one containing
lambda, all observation gates green, and exit zero under actual CPython 3.9.

Repair `audit_relin2_calls.py` so it:

1. discovers and prints all raw and expanded/semantic Relin2 routes in every
   relevant test/source file, including macro expansion/token paste, wrapper,
   alias, `std::invoke`, function object, member pointer, and indirect call;
2. permits exactly ten direct executable `module.Relin2(tensor)` calls in the
   ten frozen sites, one exact unevaluated API node, and no other route;
3. derives each enclosing function from exact balanced/AST ranges and retains
   the exact six-success/four-failure classification;
4. parses each `CheckThrowsExactInvalidArgument(...)` invocation into exact
   top-level argument ranges, identifies the exact first callable argument, and
   proves the Relin2 call is inside that callable's exact body;
5. proves no Relin2 token/call/expansion occurs in any later helper argument,
   default expression, nested unrelated lambda, wrapper, or pre-call evaluation;
6. extracts and verifies the exact helper definition and semantics: the first
   callable is invoked, only `std::invalid_argument` is accepted, the actual
   `what()` string is compared for exact equality with the expected message,
   wrong exception types fail, and no-throw fails;
7. preserves exact success pre/post Tensor/cache order, failure pre/post order,
   post-corruption snapshots, one API node, no legacy route, and all prior
   semantic gates.

## Required correction 3 — complete corruption anchors and non-target evidence

The remediation-05 failure audit treats the latest raw token spelled `cache` as
the corruption anchor. That is not a semantic proof: a direct cache expression,
differently named alias, pointee mutation, context change, tower mutation, or
removed mutation can false-green while leaving an earlier `cache` token.

For every one of the four failure-call sites, derive and print exact semantic
ranges for the required deliberate corruption and both subsequent snapshots.
Prove that:

1. the required Tensor tower/basis or evaluation-key map/vector/pointee/context
   corruption occurs before both complete snapshots;
2. the snapshots occur in exact Tensor-then-deep-cache order after the final
   mutation that establishes the failure fixture;
3. no mutation of the Tensor, context, key map, vector, or pointee occurs after
   either snapshot and before the diagnostic callable executes;
4. shared `RunKeyMutation` branches bind each named mutation mode to its exact
   expected diagnostic without relying on a variable-name substring.

The four failure-call sites are exactly `TestTensorValidationBeforeKey`,
`TestInsufficientActiveBasis`, `TestMissingEvalKey`, and the shared negative
branch of `RunKeyMutation`. Bind the first site's basis corruption and cache erase
as a two-part validation-before-key witness; bind the second site's exact
two-tower context fixture and cache erase as a two-part precedence witness; bind
the third site's cache erase; and bind each of the nine negative `KeyMutation`
enum branches (`Empty`, `NullFirst`, `WrongContext`, `WrongTag`, `WrongSubtype`,
`ALength`, `BLength`, `Basis`, `Format`) to its concrete corruption operation.
Counting four call expressions, recognizing only one branch, or recognizing only
direct `cache` spellings is insufficient.

The audit must derive and bind this exact frozen `RunKeyMutation` configuration
table, not merely count wrapper functions or accept any enum/string combination:

| Wrapper | Technique | Digit size | Mutation | Expected diagnostic | Success |
|---|---|---:|---|---|---|
| `TestKeyEmpty` | `HYBRID` | 0 | `Empty` | `DoubleCKKS: Relin2 evaluation-key vector is empty` | false |
| `TestKeyNullFirst` | `HYBRID` | 0 | `NullFirst` | `DoubleCKKS: Relin2 first evaluation key is null` | false |
| `TestKeyWrongContext` | `HYBRID` | 0 | `WrongContext` | `DoubleCKKS: Relin2 first evaluation key belongs to a different context` | false |
| `TestKeyWrongTag` | `HYBRID` | 0 | `WrongTag` | `DoubleCKKS: Relin2 first evaluation key tag does not match the Tensor key tag` | false |
| `TestKeyWrongSubtype` | `HYBRID` | 0 | `WrongSubtype` | `DoubleCKKS: Relin2 first evaluation key has the wrong concrete subtype` | false |
| `TestHybridALength` | `HYBRID` | 0 | `ALength` | `DoubleCKKS: Relin2 evaluation key HYBRID A vector length mismatch` | false |
| `TestHybridBLength` | `HYBRID` | 0 | `BLength` | `DoubleCKKS: Relin2 evaluation key HYBRID B vector length mismatch` | false |
| `TestHybridBasis` | `HYBRID` | 0 | `Basis` | `DoubleCKKS: Relin2 evaluation key HYBRID entry basis mismatch` | false |
| `TestHybridFormat` | `HYBRID` | 0 | `Format` | `DoubleCKKS: Relin2 evaluation key HYBRID entry must be in evaluation format` | false |
| `TestBv0ALength` | `BV` | 0 | `ALength` | `DoubleCKKS: Relin2 evaluation key BV A vector length mismatch` | false |
| `TestBv0BLength` | `BV` | 0 | `BLength` | `DoubleCKKS: Relin2 evaluation key BV B vector length mismatch` | false |
| `TestBv0Basis` | `BV` | 0 | `Basis` | `DoubleCKKS: Relin2 evaluation key BV entry basis mismatch` | false |
| `TestBv0Format` | `BV` | 0 | `Format` | `DoubleCKKS: Relin2 evaluation key BV entry must be in evaluation format` | false |
| `TestBvNzALength` | `BV` | 10 | `ALength` | `DoubleCKKS: Relin2 evaluation key BV A vector length mismatch` | false |
| `TestBvNzBLength` | `BV` | 10 | `BLength` | `DoubleCKKS: Relin2 evaluation key BV B vector length mismatch` | false |
| `TestBvNzBasis` | `BV` | 10 | `Basis` | `DoubleCKKS: Relin2 evaluation key BV entry basis mismatch` | false |
| `TestBvNzFormat` | `BV` | 10 | `Format` | `DoubleCKKS: Relin2 evaluation key BV entry must be in evaluation format` | false |
| `TestExtraLaterValid` | `HYBRID` | 0 | `ExtraValid` | empty string | true |
| `TestMalformedLaterIgnored` | `HYBRID` | 0 | `MalformedLater` | empty string | true |

Require exactly these nineteen wrappers/configurations: seventeen negative and
two success configurations, with no duplicate, missing, extra, reordered-field,
or transitive alias binding. Print the resolved wrapper, technique, digit size,
mutation enum, exact diagnostic, and success boolean for every row.

Do not terminate at the first target mutation failure. Always finish discovery
and print all ten sites, all raw/expanded routes, all role assignments, and every
independent per-site gate. Then print a matrix showing the target gate failed
with its dedicated code while every non-target gate remained green; only after
that complete output may the program exit nonzero.

## Required audit-mutation TDD

Before creating any disposable fixture, byte-finalize one Python-3.9-compatible
mutation driver that contains the complete fixed table for all five inherited
plus sixty-three new modes: exact source transform, exact target audit, exact
declared stable failure-code set, required non-target gates, parse command, and
restore check. Print and retain its complete source plus SHA-256 before the first
fixture. The driver must never derive, widen, or rewrite an expected set from
actual audit stdout/stderr. Every clean, red, restore, and post-restore record
must print the same driver SHA-256, and the driver bytes must match after all
sixty-eight modes.

Retain the exact driver source as Base64 evidence at
`artifacts/tdd/relin2/68-mutation-driver.py.b64`, together with its decoded byte
size and SHA-256 in the evidence index and raw transcript. This is evidence data,
not an applied-project executable: the decoded driver must exist only in fresh
directories outside the applied tree. The final decode/scanner gate below must
decode and scan it. No alternate per-mode helper may supply or change expected
failure codes.

Use fresh disposable copies outside the applied project tree. Each fixture must
first pass the exact Clang parse/AST command with zero diagnostics. Run the
unchanged repaired audit on the clean source and require green; apply one minimal
source-only mutation; rerun the same audit; require the named red; print the
complete discovered set and non-target matrix; restore byte-for-byte; prove zero
diff and original SHA-256; rerun green.

Rerun and retain all five remediation-05 mutation modes so the repair does not
regress them. In addition, execute all sixty-three rows below as separate
mandatory disposable mutations. A combined fixture, an alternative within one
row, or one representative standing in for several rows is not acceptable.

Reference callable-surface mutations:

1. insert the exact function-pointer/local-callable reference mutation reproduced
   by Codex above;
2. insert an immediately invoked lambda call in the reference closure;
3. insert a stateful function object whose overloaded `operator()` is invoked in
   the reference closure;
4. insert one unknown brace-form construction in the reference closure;
5. insert exactly `new AuditProbe()` as an unknown explicit construction in the
   reference closure;
6. insert one pointer-to-member invocation in the reference closure;
7. insert one ordinary function-like macro invocation whose expansion generates a
   call in the reference closure;
8. insert one object-like macro identifier, with no parameter list and no `##`,
   whose expansion generates a call in the reference closure;
9. insert one function-like token-paste macro invocation whose expansion
   generates a call in the reference closure;
10. replace one existing allowed reference call with an allowed-name-preserving
    function object named for that destination; define its forwarding
    `operator()` outside the three audited bodies. The legacy total, complete
    semantic total, and ordinary allowed-name counts must remain unchanged.

Relin2 route and diagnostic-containment mutations:

11. replace one existing Relin2 call with an ordinary function-like macro
    expansion, keeping exactly ten semantic Relin2 routes;
12. replace one existing Relin2 call with an object-like macro identifier, with
    no parameter list and no `##`, whose expansion executes that call; keep
    exactly ten semantic Relin2 routes;
13. replace one existing Relin2 call with a token-paste macro expansion, keeping
    exactly ten semantic Relin2 routes;
14. replace one existing Relin2 call with a named forwarding wrapper, with the
    sole direct Relin2 call for that site in the wrapper body; keep exactly ten
    semantic Relin2 routes;
15. replace only the receiver expression `module` with a direct local
    `DoubleCKKS&` receiver alias at one frozen site; do not add a lambda, member
    pointer, wrapper, macro, or function object;
16. replace one existing Relin2 call with the exact route chain
    `&DoubleCKKS::Relin2` member pointer followed by `(module.*member)(tensor)`;
17. replace one existing Relin2 call with the exact route chain
    `&DoubleCKKS::Relin2` member pointer followed by
    `std::invoke(member, module, tensor)`;
18. replace one existing Relin2 call with a stateful function object whose
    `operator()` body directly calls `module.Relin2(tensor)`; keep exactly ten
    semantic Relin2 routes;
19. reproduce Codex's wrong-argument fixture: make the diagnostic helper's first
    callable empty and move the Relin2 call to an immediately invoked lambda in a
    later top-level argument;
20. move the Relin2 call into the first callable's init-capture evaluation and
    leave that callable's body empty, keeping exactly ten semantic route records;
21. move the Relin2 call into an uncalled nested lambda inside the first callable
    body, keeping the outer callable body and exactly ten semantic route records;
22. replace one diagnostic Relin2 call with a parse-valid global/static fixture
    whose callable `operator()` has an actually evaluated default argument that
    executes the call. The first diagnostic callable invokes that operator with
    the argument omitted; the AST must contain `CXXDefaultArgExpr`; keep exactly
    ten semantic route records.

Exact helper-semantics mutations:

23. remove the helper's invocation of its first callable;
24. change only the accepted `std::invalid_argument` catch type to
    `std::runtime_error`;
25. replace only exact `what()` equality with a substring comparison;
26. replace only the final no-throw `throw TestFailure(...)` with `return;`;
27. replace only the wrong-exception catch branch's `throw TestFailure(...)` with
    `return;`.

Exact failure-fixture anchor mutations:

28. in `TestTensorValidationBeforeKey`, remove the exact
    `moduli.front() += NativeInteger(2)` basis corruption while retaining the
    pre-snapshot key-cache erase;
29. in `TestTensorValidationBeforeKey`, remove the pre-snapshot key-cache erase
    while retaining the exact basis corruption;
30. in `TestInsufficientActiveBasis`, change only the exact short-basis context
    fixture's third argument from `2` to `3` while retaining the pre-snapshot
    key-cache erase;
31. in `TestInsufficientActiveBasis`, remove the pre-snapshot key-cache erase while
    retaining the exact two-tower context fixture;
32. in `TestMissingEvalKey`, remove its pre-snapshot key-cache erase;
33. neutralize only the `KeyMutation::Empty` branch's `cache[tag].clear()` anchor;
34. replace only the `KeyMutation::NullFirst` branch's `{nullptr}` row with
    `{valid}`;
35. make only the `KeyMutation::WrongContext` branch install the original valid
    context identity;
36. make only the `KeyMutation::WrongTag` branch set the original `tag`;
37. make only the `KeyMutation::WrongSubtype` branch install `valid` at
    `cache[tag]`;
38. remove only the `KeyMutation::ALength` branch's `a.pop_back()` anchor;
39. remove only the `KeyMutation::BLength` branch's `b.pop_back()` anchor;
40. remove only the `KeyMutation::Basis` branch's `bad.DropLastElement()` anchor;
41. change only the `KeyMutation::Format` branch's bad format from
    `Format::COEFFICIENT` to `Format::EVALUATION`.

Post-snapshot mutation-surface mutations must be inserted after both complete
snapshots and before the diagnostic callable in a failure fixture whose required
pre-snapshot corruption remains intact:

42. mutate the Tensor ordered-moduli manifest through the direct Tensor access
    expression;
43. mutate the Tensor ordered-moduli manifest through a differently named local
    reference alias;
44. replace an evaluation-key entry with a same-tag, same-A/B
    `EvalKeyRelinImpl` constructed from a different context, reaching the entry
    through the direct static cache expression;
45. perform the same context-identity mutation through a differently named local
    key alias;
46. erase the active evaluation-key map row through the direct static cache
    expression;
47. erase the active evaluation-key map row through a differently named local map
    alias;
48. append a duplicate key to the active evaluation-key vector through a direct
    `.at(tag)` expression;
49. append a duplicate key to the active evaluation-key vector through a
    differently named local vector alias;
50. change the first evaluation-key pointee's key tag through a direct
    `.at(tag).front()` expression;
51. change the first evaluation-key pointee's key tag through a differently named
    local pointee alias.

Exact nineteen-row configuration-field value mutations:

52. change one negative wrapper's expected diagnostic while its other five frozen
    fields remain unchanged;
53. change one wrapper's mutation enum while its other five frozen fields remain
    unchanged;
54. change one wrapper's technique while its other five frozen fields remain
    unchanged;
55. change one wrapper's digit size while its other five frozen fields remain
    unchanged;
56. change one wrapper's `expectSuccess` boolean while its other five frozen
    fields remain unchanged;
57. rename one wrapper and update its test-dispatch `ResolveTest` function pointer
    to the renamed wrapper, preserving the exact command, configuration tuple,
    nineteen-row count, parse, and every non-wrapper gate.

Exact nineteen-row transitive-alias mutations:

58. replace one wrapper's direct expected-diagnostic string with a local named
    constant of the same value;
59. replace one wrapper's direct mutation enum with a local named constant of the
    same value;
60. replace one wrapper's direct technique enum with a local named constant of the
    same value;
61. replace one wrapper's direct digit-size literal with a local named constant of
    the same value;
62. replace one wrapper's direct `expectSuccess` literal with a local named
    constant of the same value;
63. replace one direct `ResolveTest` wrapper pointer with a local named function-
    pointer alias of the same value.

The mutation driver must declare the following exact target-red sets before it
runs a fixture, then prove the actual stable failure-code set equals the declared
set. These are target gates, not non-target failures:

| Mode | Required target-red set and legacy-count result |
|---:|---|
| 1 | `REF_FUNCTION_POINTER_ROUTE`; legacy cross-check remains 23 |
| 2 | `REF_IIFE_ROUTE`; legacy cross-check remains 23 |
| 3 | `REF_FUNCTION_OBJECT_ROUTE`, `REF_LEGACY_COUNT`; legacy count is exactly 24 |
| 4 | `REF_BRACE_CONSTRUCTION`; legacy cross-check remains 23 |
| 5 | `REF_NEW_CONSTRUCTION`, `REF_LEGACY_COUNT`; legacy count is exactly 24 |
| 6 | `REF_MEMBER_POINTER_ROUTE`; legacy cross-check remains 23 |
| 7 | `REF_FUNCTION_MACRO_ROUTE`, `REF_LEGACY_COUNT`; legacy count is exactly 24 |
| 8 | `REF_OBJECT_MACRO_ROUTE`; legacy cross-check remains 23 |
| 9 | `REF_TOKEN_PASTE_ROUTE`, `REF_LEGACY_COUNT`; legacy count is exactly 24 |
| 10 | `REF_ALLOWED_NAME_ROUTE_FORM`; legacy count and allowed-name counts remain unchanged |
| 11 | `RELIN_FUNCTION_MACRO_ROUTE`, `RELIN_DIRECT_SITE_SHAPE` |
| 12 | `RELIN_OBJECT_MACRO_ROUTE`, `RELIN_DIRECT_SITE_SHAPE` |
| 13 | `RELIN_TOKEN_PASTE_ROUTE`, `RELIN_RAW_DIRECT_LITERAL_ROLE`, `RELIN_DIRECT_SITE_SHAPE` |
| 14 | `RELIN_WRAPPER_ROUTE`, `RELIN_SITE_ENCLOSURE` |
| 15 | `RELIN_RECEIVER_ALIAS` |
| 16 | `RELIN_MEMBER_POINTER_ROUTE`, `RELIN_DIRECT_SITE_SHAPE` |
| 17 | `RELIN_STD_INVOKE_ROUTE`, `RELIN_MEMBER_POINTER_ROUTE`, `RELIN_DIRECT_SITE_SHAPE` |
| 18 | `RELIN_FUNCTION_OBJECT_ROUTE`, `RELIN_SITE_ENCLOSURE` |
| 19 | `DIAGNOSTIC_FIRST_CALLABLE_ARGUMENT` |
| 20 | `DIAGNOSTIC_CALLABLE_BODY_EXECUTION` |
| 21 | `DIAGNOSTIC_UNCALLED_NESTED_LAMBDA` |
| 22 | `DIAGNOSTIC_DEFAULT_ARGUMENT`, `RELIN_INDIRECT_ROUTE`, `RELIN_FUNCTION_OBJECT_ROUTE`, `RELIN_SITE_ENCLOSURE`, `RELIN_DIRECT_SITE_SHAPE` |

For modes 1–10, every new semantic construction/call must print in the complete
reference set before the declared red. Modes 11–22 must print the complete raw
and expanded Relin2 route chain before the declared red. For modes 23–63, the
complete ten-site, helper, four-site anchor, nine-enum anchor, post-snapshot
surface, or nineteen-row configuration matrix must print before exit; each has
one dedicated target code. Every gate absent from the exact declared target set
must remain green. Modes 52–63 require distinct stable field-level value and
alias-form gates; one aggregate mismatch cannot satisfy them.

For every mode retain: pre source and audit hashes; clean diff; complete unified
mutation diff; literal parse, raw-token, preprocessing/AST, and audit commands;
raw stdout/stderr and exits; complete discovered/classified sets; target and
non-target assertion matrix; the unchanged driver SHA-256; restoration command,
exact hash restoration, zero diff, and post-restore green. Unencoded mutation
helpers, executable scripts, AST scratch files, and temporary sources must not
become applied-project files; only the required indexed Base64 driver evidence is
retained.

Do not manufacture failure text, edit the repaired audit's frozen expected table,
change the mutation driver's declared target codes, or use a different audit
program for the red. The exact test-source edits prescribed by modes 52 and 56
are the only permitted mutations of an expected diagnostic or `expectSuccess`;
they may not be accompanied by any masking expected-result change.

## Required correction 4 — complete retained build and non-cyclic archive evidence

Rerun the final configure, warning-clean build, registration query, and CTest on
the exact source-agent final tree. Retain the complete raw stdout/stderr and raw
exit for:

- configure;
- warning-clean build;
- `ctest --show-only=json-v1` including the complete JSON;
- unfiltered `ctest --output-on-failure` including all 37 results.

The source-agent final response and `TESTS.md` must bind these literal rerun
commands and outputs to the exact final source tree; remediation-05 summaries are
not substitutes. Baseline Python/audit commands must likewise retain complete raw
output, not only summaries. Hosted Linux/Windows remains a downstream Codex gate
and must not be claimed here.

After the replacement patch 07 is byte-final, perform a fresh exact-base
seven-patch replay. Decode every Base64 file below `artifacts/tdd/relin2/` into
a fresh directory outside the applied tree. Scan both the exact applied final
tree and all decoded bytes with the identified secret scanner plus the targeted
filename/content scan. Retain the complete commands, decoded-file manifest,
sizes/hashes, redacted findings and review disposition, raw reports, and exits
in external `TESTS.md`. If this gate changes patch 07, repeat it against the new
exact final patch/tree; evidence from an earlier tree is not sufficient.

Final ZIP construction and fresh-extract evidence is necessarily produced
after external `TESTS.md`, `PATCHES.sha256`, and the ZIP bytes are immutable. It
must not be written back into any file inside that ZIP, because doing so would
change the recorded hashes and create a self-cycle. Instead, after the ten files
are final, stage/select exactly those files, execute the following post-final
sequence, and place the literal complete transcript in the final ChatGPT
response after the required verdict line:

1. exact selected-file manifest, modes, sizes, SHA-256 values, and source paths;
2. scanner identity/path/version, exact staged-content Gitleaks command, full
   redacted output/report, raw exit, and targeted filename/content scan;
3. literal ZIP construction command and raw exit;
4. complete central-directory order/mode/encryption/path inspection and
   duplicate/unsafe-path assertions;
5. `unzip -t` command/output/exit;
6. extraction into a fresh quarantine directory;
7. fresh-extract `PATCHES.sha256` command/output/exit;
8. scanner identity/version plus complete fresh-extract Gitleaks and targeted
   scans with literal commands, outputs/reports, and raw exits;
9. final ZIP byte size and SHA-256.

The post-final response transcript must bind the immutable external `TESTS.md`
and `PATCHES.sha256` hashes it scanned. If any post-final check fails, do not
modify a file and reuse the old transcript: rebuild from the corrected immutable
ten-file set and rerun the entire post-final sequence.

Codex will save and hash the complete final response as the source-agent
packaging sidecar. Codex's later quarantine scan cannot substitute for missing
source-agent evidence. Review false positives explicitly; do not suppress broad
rules or claim `no findings` when findings were merely allow-listed.

## Patch-07 and evidence scope

Build the replacement from the exact base by applying frozen patches 01–06,
then generate one replacement `07-final-docs.patch`. Relative to the complete
remediation-05 patch-07 result, only these paths may differ:

- `artifacts/tdd/relin2/audit_reference_scope.py`;
- `artifacts/tdd/relin2/audit_relin2_calls.py`;
- `artifacts/tdd/relin2/68-mutation-driver.py.b64`, containing only the required
  byte-frozen driver evidence;
- directly affected `artifacts/tdd/relin2/*.txt`, JSON, or exact raw-transcript
  files;
- `artifacts/tdd/relin2/INDEX.md`;
- `coordination/RELIN2_DESIGN.md` and `README.md` only where audit/downstream
  wording must be corrected.

One additional narrow audit program or retained machine-readable AST/token
manifest is allowed only if it materially simplifies and deepens the complete
proof. Do not create a general audit framework. Preserve every unchanged
remediation-05 evidence file byte-for-byte.

The replacement `INDEX.md` must bind exactly every regular file below
`artifacts/tdd/relin2/`, sorted by relative path, except itself. It must continue
to bind only patches 01–06 and the pre-07 tree, avoiding a self-cycle. External
`TESTS.md` must bind all seven patch hashes and the new final tree.
`PATCHES.sha256` must bind the other nine archive files in exact order.

Retain a fresh exact-base, index-aware seven-patch replay with literal commands,
outputs, exits, and cumulative trees. Boundaries 01–06 must equal the fixed
trees above. Independently compute the replacement patch-07 hash and final tree.
Run every final shipped audit after replay under the recorded source-agent
interpreter. All scripts must remain Python-3.9-syntax-compatible. Codex will
again run `py_compile` and every shipped audit under actual CPython 3.9 and 3.10.

## Required deliverables

Return exactly one ZIP named:

`chatgpt-pro-relin2-01-remediation-06-delivery.zip`

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
identities, corrected complete callable/route/helper/corruption design,
mutation matrix, Python compatibility, lifecycle/scale table, HYBRID/BV proof,
and every pending downstream item. `TESTS.md` must retain all literal commands,
raw outputs/stderr, raw exits, separate assertion exits, hashes, trees, complete
mutation matrices, the byte-frozen driver source/hash proof and decoded identity,
final replay, complete final build/registration output, and the exact applied-tree
plus decoded-Base64 scans. It must state that immutable ZIP construction and
fresh-extract evidence follows in the post-final response rather than claim
self-cyclic in-archive evidence.

In the response report the ZIP size/SHA-256, replacement patch-07 SHA-256,
replacement final tree, frozen patches/files, complete old-plus-new mutation
matrix, Python result, every final audit result, complete final build/CTest,
applied-tree/decoded-Base64 scan result, the complete post-final selected-file/
ZIP/fresh-extract transcript, and all pending environments.

## Acceptance criteria

`ready to apply` requires every item below:

- all eleven submitted inputs match before substantive inspection;
- patches 01–06 and all eight executable/project files remain byte-identical;
- exact-base replay reproduces all six frozen trees and the new final tree;
- no silent callable/constructor/macro/indirect surface remains in either audit;
- both Codex-reproduced false-green fixtures now produce dedicated reds;
- one byte-frozen driver contains all sixty-eight transforms and declared target
  sets before execution; every mode binds the same driver SHA-256; its exact
  Base64 evidence, decoded identity, INDEX binding, and decoded scan are retained;
- every inherited and new mutation produces an actual stable failure-code set
  exactly equal to its declared target-red set (a singleton for modes 23–63), a
  complete printed discovered set, explicit non-target greens, byte restoration,
  and final green;
- exact diagnostic callable argument, helper semantics, and corruption anchors
  are proved mechanically;
- all shipped audits compile with Python-3.9-compatible syntax and execute
  successfully on the final source-agent tree;
- INDEX, external docs, complete final build/registration raw output, applied-tree and decoded-Base64
  evidence, immutable post-final selected-file/archive evidence, fresh-extract
  verification, hashes, scans, and ZIP structure are complete without a
  self-cycle;
- no production/test change and no Windows, hosted same-SHA, project commit/
  push/PR, Zcode/Zima, Fable5, RS2, Mult2, pair Add/Sub, plaintext precision,
  performance, or security result is claimed.

Any unmet item requires `changes needed` or `blocked`, never `ready to apply`.

## Forbidden operations and claims

- Do not access the network, Git remote, private repository, local user files,
  browser state, credentials, or hidden environment state.
- Do not commit, push, create a PR, dispatch CI, invoke Windows/Zcode/Zima, or
  invoke Fable5.
- Do not modify production, headers, workflow, CMake, or executable tests in the
  delivered patches or final applied tree. Only fresh disposable copies may
  receive the five inherited remediation-05 mutations plus the sixty-three new
  mutations prescribed here, exactly sixty-eight fixtures total; each must be
  restored byte-for-byte and no mutated source may enter the archive.
- Do not add runtime wrappers, fallback paths, `legacy_*`, production
  `try`/`catch`, or audit-only executable behavior.
- In delivered/final applied bytes, do not alter an oracle, expected diagnostic,
  test dispatch, CTest registration, lifecycle, scale, metadata, cache, key-shape,
  or arithmetic target. In disposable copies, only the five inherited and
  sixty-three new mutations (sixty-eight total) are allowed; do not retain,
  broaden, combine, or mask them.
- Do not claim Codex, hosted Linux/Windows, Windows, integration, Fable5, RS2,
  Mult2, pair Add/Sub, plaintext precision, performance, or security results.
- Do not describe raw literal-name counting, ignored parentheses/braces, lexical
  proximity, any containing lambda, or variable-name heuristics as a complete
  semantic proof.
