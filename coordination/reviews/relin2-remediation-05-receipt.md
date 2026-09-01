# Relin2 remediation 05 receipt and review

Recorded: 2026-09-01 16:25 Asia/Shanghai

Decision: **CHANGES NEEDED — do not apply any remediation-05 patch to the real implementation branch.**

The production/API/test candidate remains byte-frozen and no arithmetic defect
was observed. The rejection is caused by reproduced fail-open audit behavior and
missing delivery evidence in replacement patch 07. This receipt does not treat
ChatGPT Pro local Linux evidence as Codex, hosted, or Windows evidence and makes
no new runtime claim.

## External response and recovered archive

- Saved conversation:
  <https://chatgpt.com/c/6a960223-f7d8-83ec-9ad1-ac404f614ba9>
- Ego Lite task space: `85`.
- ChatGPT Pro completed naturally after approximately `37m09s`; Codex did not
  interrupt, refresh, duplicate, or resubmit the request while it was running.
- The response began with `ready to apply` and supplied one archive named
  `chatgpt-pro-relin2-01-remediation-05-delivery.zip`.
- Page-declared and locally observed size: `178953` bytes.
- Page-declared and locally observed SHA-256:
  `06054658322b7bf6de1883a1c0cafb0ea118e452bd3a52f3e8d23be0a409bc45`.
- The unique UI download control was clicked exactly once and produced no local
  file. Codex did not click it again. The page's already-created attachment
  query was refetched exactly once and the same signed object was transferred
  once into a fresh quarantine directory. No further download/refetch occurred.

## Mechanical archive, handoff, and secret gates

Observed pass:

- The controlling task file was exactly `21525` bytes with SHA-256
  `2c5e9d92479665fc0c8f96de8b220ba3060f11d7b31410e0f0221aaa5f385c64`.
- All twelve submitted inputs matched their delivery-message name, size, and
  SHA-256 bindings before the response was inspected.
- `unzip -t` returned success.
- The central directory contained exactly the required ten unencrypted regular
  `100644` root entries, in the required order, without duplicates, directories,
  links, slash/backslash names, or unsafe paths.
- `PATCHES.sha256` contained exactly nine ordered records for the other nine
  files, and all nine recomputed hashes matched.
- Pinned Gitleaks `8.30.1` findings were reviewed rather than silently ignored.
  The archive scan's four findings and the applied-tree/decoded-transcript
  scan's eight findings were synthetic `key_cache` mutation records or frozen
  source/tree SHA-256 literals, not credentials. Exact-fingerprint reviewed
  rescans of both the final applied tree and all five freshly decoded Base64
  transcripts returned exit `0` and `no leaks found`.
- A separate targeted filename scan found no `.env`, credential, cookie, token,
  private-key, key-store, or browser-state file. A separate value-pattern scan
  found no private-key header, AWS/GitHub/OpenAI/Google/Slack token form, bearer
  token, or assigned API/access/secret value in either scope.

The incoming archive, extraction, scan reports, and disposable replays remain
outside the tracked implementation branch. No incoming binary was added to the
implementation history.

## Exact-base replays and frozen identities

The real implementation branch remained clean at exact local/remote commit
`fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`, tree
`759d5195739684748d5a9664edabe3fa719e1acf`. No patch was applied there.

Three different no-hardlinks/disposable replays were inspected:

- remediation 03 reproduced final tree
  `342436fab92fa2daf005ec557ee300cbe330122e`;
- remediation 04 reproduced final tree
  `2b1a10ab6e4f776a620710e2c7dd2b214a237357`;
- remediation 05 reproduced final tree
  `d9e6307f4fe3a278cb29727169c6396d26a897a0`.

Remediation-05 patch identities and cumulative trees independently reproduced:

| Boundary | Patch SHA-256 | Cumulative tree |
|---|---|---|
| 01 | `6296f34d9aafc62ea501c638189118393a5196f2e5f0f7ef552a9b94811d9f64` | `28d0294bce8844fa831951879020b353568c1c13` |
| 02 | `364a1689fa93cf98a65a69a70814d40e49650477668a0bf13d5fcaf63db5c57c` | `7ea2bf2db6f87ee357da2a68f198836c9fa30d4a` |
| 03 | `b68e182dfd3ada869af7c4a2941a7bb840030d15ed2a633733e2333fa5465e0a` | `ef19f3d7a55eee6767622c2b7210e00f7bdcfd54` |
| 04 | `a6ad402d8d8dc38661916b2d1049ec7e034cea74948241922b9a95dbefff10e1` | `358215176f75e96af3dc3a1baa58842e9d33c2b5` |
| 05 | `c8d90f1a1e47056a8c4217fc58ccb3df5c54601f38edc3a1a866c375aee1101e` | `6ea2a0107e8cbc5239dcaafb6eb1d903ec6cac84` |
| 06 | `60bdfb21b007816ba626c5186c92ae4a909eae3360f85aecee80234a28ff396f` | `4f0d376c8be65ccfc3c9dded02921525bfec65b8` |
| 07 | `e9e43e0ad568aa87652feebf17f9335ce717df322db4d92992e8767c16cd35e3` | `d9e6307f4fe3a278cb29727169c6396d26a897a0` |

Patches 01–06 were byte-identical to remediation 04. The eight protected final
production/header/CMake/workflow/executable-test hashes all matched the
controlling task. R4-to-R5 differences were confined to the eleven permitted
audit/evidence/documentation paths. R3-to-R5 executable differences were exactly
the three permitted pairs of metadata snapshot lines moved before the complete
Tensor/cache snapshots; every other protected byte was identical.

The applied `INDEX.md` listed exactly all 29 regular files below
`artifacts/tdd/relin2/` except itself, sorted by relative path. All 29 listed
SHA-256 values matched, no `__pycache__` or `.pyc` existed in the applied tree,
and `git diff --cached --check` returned success.

## Actual downstream Python receipt gates

Codex ran all five shipped audit programs on the exact final replay with both:

- CPython `3.9.6` at
  `/Library/Developer/CommandLineTools/Library/Frameworks/Python3.framework/Versions/3.9/bin/python3.9`;
- CPython `3.10.20` at
  `/Users/lifeng/.local/share/uv/python/cpython-3.10.20-macos-aarch64-none/bin/python3.10`.

`py_compile` and the five literal executions returned zero under both versions,
with bytecode disabled or redirected outside the project. The observed final
static outputs included 37 unique CTest command tuples, 31 distinct Relin2
routes, ten direct Relin2 call tokens classified six-success/four-failure, one
allowed unevaluated API node, 23 reference-closure calls, exactly two trusted
public `Relinearize` calls, frozen production hashes, and the exact R3-to-R5
reorder. These baseline-green results are real compatibility observations; they
do not prove that the audits fail closed under the required adversarial inputs.

## Frozen arithmetic/spec observations

The frozen production still implements the accepted paper composition:

`Relin2(CT) = DCP_qdiv(Relin(q_div * high3)) + (0, Relin(low3))`.

It returns exactly `(u, v+w)` on the unchanged working prefix, invokes the
public output-returning `Relinearize` exactly twice, uses the shared private DCP
seam, validates pre-add compatibility, preserves the accepted lifecycle/scale/
metadata/Tensor/cache contracts, and retains the accepted HYBRID/BV key-shape
logic. No production `try`/`catch`, rescale/mod-reduction, private key-switch,
or alternate legacy route was introduced. The external lifecycle/scale table
and HYBRID/BV proof are present. The review failures below concern audit and
delivery evidence, not an observed production arithmetic regression.

## Reproduced P1 fail-open behavior

### 1. Reference closure misses indirect/local callable routes

`audit_reference_scope.py:116-163,201-211` only classifies a call parenthesis
when its preceding token is a raw identifier or template `>`. Other potential
call surfaces return `None` and are silently skipped, although the controlling
task explicitly requires indirect, local-callable, function-pointer,
macro-generated, and otherwise unclassified routes to fail.

Codex inserted the following syntactically valid disposable mutation into
`RaiseElement` on an exact final-tree copy:

```cpp
auto auditProbe = [] {};
auto auditProbePtr = +auditProbe;
(*auditProbePtr)();
```

Literal command:

```text
PYTHONDONTWRITEBYTECODE=1 /Library/Developer/CommandLineTools/Library/Frameworks/Python3.framework/Versions/3.9/bin/python3.9 artifacts/tdd/relin2/audit_reference_scope.py --root <disposable-final-tree> --clang /usr/bin/clang++
```

Observed result: tokenization exit `0`, unchanged
`DISCOVERED_CALL_TOTAL=23`, all bijection/forbidden assertions printed green,
and whole audit exit `0`. The complete extracted closure printed the injected
function-pointer call while the discovered set omitted it. This is a concrete
false green, not a hypothetical parser limitation. Brace construction,
immediately invoked local callables, and object-like/token-paste macro routes
are the same unclosed class.

### 2. Failure-call audit accepts a lambda in the wrong helper argument

`audit_relin2_calls.py:169-187,327-339` proves only that a Relin2 token is in
some lambda somewhere inside the outer diagnostic-helper argument range. It
does not prove that the lambda is the helper's first callable argument.

Codex changed one disposable missing-key case to an empty first lambda and a
second-argument immediately invoked lambda that calls Relin2 and returns the
expected string. The call therefore executes while evaluating a later argument,
before `CheckThrowsExactInvalidArgument` can invoke or catch it.

Observed result under actual CPython 3.9 and Clang raw-tokenization: exact ten
direct calls, six-success/four-failure, one diagnostic container, one containing
lambda, all pre/post order assertions green, and whole audit exit `0`. This is a
second concrete false green and violates the exact callable-argument contract.

### 3. Raw literal scan does not close macro/wrapper/indirect Relin2 routes

`audit_relin2_calls.py:207-249` scans only raw source identifiers whose spelling
is literally `Relin2`. It has no expanded-token, AST, wrapper, callable-alias,
`std::invoke`, or indirect-route inventory. A token-paste macro such as
`object.Rel##in2(argument)` can add a real call while preserving the raw count
of ten. This violates the inherited no-indirect-route gate.

### 4. Required helper semantics and post-corruption anchors are not proved

The replacement audit does not extract and verify the exact
`CheckThrowsExactInvalidArgument` implementation: invocation of the first
callable, `std::invalid_argument`, and exact `what()` equality. Its failure-path
corruption proof reduces the anchor to the latest raw token spelled `cache`.
That is not a complete proof of Tensor tower/context mutation or key-map/pointee
mutation, and a differently named alias/direct cache expression can bypass it.

## Mutation-evidence and delivery P2/P1 failures

- `audit_relin2_calls.py` exits immediately at the target failure before the
  complete discovered records are printed. Mutation modes 3–5 therefore show
  only the target call and do not prove that all remaining nine calls and every
  non-target role/containment/order gate stayed green, as required.
- External `TESTS.md` ends after the patch-07 scope/index section. It does not
  retain the required outgoing selected-file staging scan, ZIP creation,
  central-directory/mode/order inspection, integrity test, fresh-extract
  `PATCHES.sha256` verification, second secret scan, scanner identity/version,
  commands, outputs, and exits. Codex's later quarantine scan cannot recreate
  missing source-agent evidence.
- The final configure/build/CTest-registration section records commands and
  asserted exits but omits raw configure/build stdout/stderr and the fresh raw
  `ctest --show-only=json-v1` output. The unfiltered 37/37 tail is present, but
  the broader retained-output requirement is not met.

## Required next action

Issue one narrow remediation 06 in the same saved ChatGPT Pro conversation.
Freeze patches 01–06 and every production/API/CMake/workflow/executable-test
byte. Replace only patch 07 audits and directly affected evidence/index/docs,
plus external `REVIEW.md`, `TESTS.md`, and `PATCHES.sha256`.

The replacement must at minimum:

1. inventory the complete raw and expanded/semantic callable surface without a
   silent `None` path, rejecting indirect function pointers, local/immediate
   callables, brace constructions, macro expansion/token pasting, wrappers,
   aliases, and unknown call routes;
2. prove that each failure Relin2 call is inside the exact first callable
   argument of the exact diagnostic invocation;
3. restore an exact fail-closed diagnostic-helper semantics proof and complete
   post-corruption snapshot anchors;
4. collect and print the complete call set and every non-target result before a
   dedicated mutation failure exit;
5. add disposable red-green mutations for every newly identified bypass and
   retain complete parse/diff/output/restore evidence;
6. retain complete build/registration raw output and the complete required
   outgoing archive/scan transcript.

Until remediation 06 passes mechanical and three-axis review, do not apply,
commit, push, dispatch hosted CI for, or spend the one authorized Fable5 review
on this Relin2 candidate.
