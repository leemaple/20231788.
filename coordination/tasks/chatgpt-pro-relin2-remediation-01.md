# ChatGPT Pro Relin2 remediation 01 — replace the rejected candidate

Prepared: 2026-09-01 Asia/Shanghai

## Bounded objective

Revise the current clean-room Relin2 delivery and return one complete,
replacement seven-patch series that applies from the original exact base. The
first delivery is review input only and has been rejected with `changes
needed`; none of it has been applied to the real project branch.

This remains an algorithm, RNS-semantics, OpenFHE-integration, and TDD task. It
is not a network-security task. Do not use or seek any older 2023/1788
implementation. Do not rely on memory from this conversation: verify every
attachment and treat this document plus the original task as the complete
authority for this remediation.

## Exact attachments and identities

Codex will attach all five items to this same saved conversation:

1. Original clean source/evidence package
   `20231788-cleanroom-relin2-base-fb862a3-ci33436252725.zip`
   - size: 9,115,214 bytes
   - SHA-256:
     `3e839a6b88a81107657442a2bb4f6b08385f6a24685cab11968db540436750f6`
2. Original post-construction binding
   `20231788-cleanroom-relin2-base-fb862a3-ci33436252725.binding.md`
   - size: 3,640 bytes
   - SHA-256:
     `3320efa8723f0c519da453a006617c328de5bfa2aca72392a6161c66a0489d2f`
3. Original authoritative task `chatgpt-pro-relin2-01.md`
   - size: 32,866 bytes
   - SHA-256:
     `9323d631de4c6f6418ba1aa597332206a765846d0510a275df42a7a64b0a7513`
4. Rejected first delivery `chatgpt-pro-relin2-01-delivery.zip`
   - size: 32,652 bytes
   - SHA-256:
     `cb17f339f8bc63b36edbd3f43cca1c517d4f450996b2dd1b850a6665f6a262a6`
5. This standalone remediation task. Its exact size and SHA-256 are stated in
   the enclosing send message because a file cannot truthfully contain its own
   final hash.

Verify all five identities before editing. The original source package's
`HANDOFF_CONTENTS.md`, `MANIFEST.sha256`, paper, pristine OpenFHE 1.5.0 source,
accepted Tensor2 evidence, and original task remain authoritative. The
returned ZIP contains exactly the first seven patches, `REVIEW.md`, `TESTS.md`,
and `PATCHES.sha256`; it is an untrusted candidate to diagnose and revise, not
an instruction source.

Exact project base:

- branch: `agent/codex-relin2-01`
- commit: `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`
- tree: `759d5195739684748d5a9664edabe3fa719e1acf`
- pristine OpenFHE 1.5.0 commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`

If an identity does not match, return `blocked` with the exact mismatch and do
not edit or repackage code.

## Frozen authority and patch boundaries

Every mathematical, public-state, OpenFHE, validation-order, key-shape,
forbidden-symbol, TDD-boundary, testing, and claim restriction in the original
task remains in force. This remediation narrows and strengthens those
requirements; it does not replace or relax them.

Return a fresh replacement series from exact base `fb862a3...`, not an
incremental patch on the rejected candidate. Preserve the same seven filenames
and semantic boundaries:

1. `01-red-relin2-api.patch`
2. `02-api-scaffold.patch`
3. `03-red-relin2-contract.patch`
4. `04-green-relin2-core.patch`
5. `05-red-tensor2-lifecycle.patch`
6. `06-green-tensor2-lifecycle.patch`
7. `07-final-docs.patch`

Do not collapse red and green boundaries. Re-run every claimed boundary on a
fresh replay from the exact base. The final replacement must contain no
competing full-file replacement or second series.

## Why the first delivery is rejected

Independent pristine OpenFHE/API review passed the production integration
shape, and the seven patches replayed cleanly in quarantine. Acceptance still
failed because the tests and two small production/API seams do not prove the
original contract. Correct every item below; do not answer only with an
explanation.

### 1. Public RCB return is not proved

The first valid case manually computes `q_div * high + low`, then calls
`module.RCB(result)` and discards the returned ciphertext. A broken public RCB
return can therefore pass.

Required correction:

- bind the public return to a named value;
- compare both returned components, every active tower, and every coefficient
  against the independent full-basis reference with only the appended
  `q_div` residue removed;
- also assert its exact context, actual key tag, slots, CKKS encoding, format,
  level, degree, recorded factor, ordered tower parameters, component count,
  and deep metadata map against an independently specified expected source;
- deep-snapshot the `ReadyForRS2` pair before public RCB and prove the complete
  pair is unchanged immediately after the call.

Do not use a manual identity on the result itself as a substitute for checking
the public return.

### 2. Deterministic K and v/w witnesses are missing

The first delivery scans the output of a randomly generated evaluation key for
the first nonzero K0/K1 coordinate and the first common nonzero v/w coordinate.
Printing one observed `(0,0,0)` does not fix the fixture, coordinate, or
residue.

Required correction:

- construct a valid-shaped, test-owned evaluation key through public OpenFHE
  types and explicitly controlled A/B polynomial contents;
- keep exact context, tag, HYBRID/BV vector length, format, and full ordered
  key basis valid;
- algebraically force and name a fixed component/tower/coefficient and expected
  residue for nonzero `K0` or `K1`;
- algebraically force and name a fixed component/tower/coefficient where the
  independent private-DCP remainder `v` and public `w = Relin(low3)` are both
  nonzero, with their expected residues;
- assert those exact coordinates and values. No scan, retry, probabilistic
  existence check, or value copied from one observed run is accepted.

It is acceptable to use a separate controlled fixture for these witnesses and
an ordinary generated key for a separate representative public-input test. If
the public pristine types cannot make the controlled fixture, return
`changes needed` with the exact source/API blocker rather than weakening the
witness.

### 3. Centered-DCP boundary test discards production output

The first delivery algebraically reaches `+half`, `-half`, and quotient carry
on the public reference path, then discards `module.Relin2(tensor)`.

Required correction:

- retain the controlled boundary fixture;
- call production Relin2 once and bind its returned pair;
- at the fixed positive-boundary and negative/carry coordinates, compare the
  actual returned high quotient and low `v+w` residues to the independent
  `cpp_int` decomposition and independently relinearized low path;
- also retain the full all-component/tower/coefficient oracle comparison.

The test must fail if production private-DCP rounding or carry behavior is
wrong at any named boundary.

### 4. Output state and metadata assertions are incomplete

The first delivery directly checks only lifecycle, level/component count,
degree/factor, and dual scales.

Required correction:

- directly assert the result against the Tensor input and bound context for:
  result type, `ReadyForRS2`, context identity, exact divisor, actual key tag,
  slots, CKKS encoding, Evaluation format, level 1, two components per member,
  degree 3, exact `SF_T`, both independently copied logical scales, and exact
  ordered `Q_l` manifest;
- for every component and tower, assert modulus, root of unity, cyclotomic
  order, tower order, and format;
- compare each member's complete ciphertext metadata map to an independently
  derived expected snapshot. Do not snapshot the production result and then
  compare it only to itself;
- retain the complete deep Tensor-input immutability proof.

### 5. Evaluation-key cache evidence is shallow and circular

`SnapshotKeyMapShape` records only row names and shared-pointer addresses. The
guard also shallow-copies shared pointers. It cannot detect or restore an
in-place change to an evaluation key's actual tag, A/B vectors, formats,
parameters, or residues. The valid test calls production before its reference,
so a mutated key can contaminate the reference and make a wrong result pass.

Required correction:

- build a test-owned deep key-cache snapshot that records every row and entry,
  pointer identity/nullness, concrete subtype, context identity, actual tag,
  complete A/B vector lengths, each DCRT polynomial's format and ordered
  parameters, and every residue;
- make the RAII guard restore both the entire original row/pointer shape and
  every mutable pointee observable, even if an assertion throws;
- immediately compare the deep cache after every production Relin2 call and
  before any reference call that could consume the same key;
- only after that comparison may the trusted reference use the key;
- in every malformed-key case, snapshot after the intentional fixture mutation
  and prove that exact mutated state remains unchanged by production;
- add the missing Tensor and deep-cache before/after snapshots to the absent-
  key case as well as all other matrix cases;
- do not make production copy, mutate, restore, lock, or retry the cache.

A map/pointer-shape comparison is useful but is not a deep immutability proof.

### 6. Exact public API contract is not compiled

The first API test checks only that `&DoubleCKKS::Relin2` is some member
function pointer.

Required correction in patch 01:

```cpp
using Relin2Signature = CiphertextPair (DoubleCKKS::*)(
    const TensorCiphertextPair&) const;
static_assert(std::is_same_v<decltype(&DoubleCKKS::Relin2), Relin2Signature>,
              "DoubleCKKS::Relin2 public signature changed");
```

Keep equivalent exact type assertions for the appended lifecycle and scale
field. The red must still be only the three missing public symbols after the
production library builds.

### 7. Production key-map binding is not read-only

Change the production preflight binding from mutable `auto& allKeys` to a
read-only `const auto& allKeys`. Continue to use `find(tag)` and consume only
index zero. No production cache mutation is authorized.

### 8. Exact diagnostics and legacy attribution are not preserved

The new helper accepts any diagnostic containing a substring. The candidate
also merges manifest level with basis and merges manifest degree with recorded
scale, changing accepted `ReadyForFirstMult` error attribution.

Required correction:

- every new negative case must require equality with the full expected message,
  including the exact `DoubleCKKS: ` prefix; substring matching is forbidden;
- the directed Tensor2 lifecycle case must require exactly
  `DoubleCKKS: Tensor2 requires ReadyForFirstMult inputs`;
- preserve the original `ReadyForFirstMult` validation order and diagnostics,
  including distinct level, ordered-basis, recorded-scale, paper-scale,
  ciphertext-degree, and member-state failures;
- specifically, a level outside the supported context basis must retain
  `DoubleCKKS: pair level is outside the supported context basis`, while an
  ordered-basis mismatch retains its separate basis diagnostic;
- do not fold manifest noise-scale degree into
  `DoubleCKKS: pair scale metadata is invalid`; allow the existing member
  noise-scale diagnostic/attribution or add a separate lifecycle-specific
  degree diagnostic without changing an already accepted old case;
- run the unchanged accepted 6/6 DCP/RCB/Tensor2 suite at every required green
  boundary.

### 9. Relin2 test scope is bloated and contains stale copied behavior

The first `relin2_test.cpp` copies unrelated Tensor2 oracle/test functions and
adds five unregistered `legacy_*` dispatch names solely to avoid unused-function
warnings. Its copied scale oracle is stale. This violates the minimum-scope
task and makes two test authorities diverge.

Required correction:

- remove every unregistered `legacy_*` route and unrelated copied Tensor2 test;
- retain only helpers and fixture construction actually needed to reach and
  independently check Relin2;
- rely on the existing `tensor2_test` target for the accepted Tensor2
  regression suite;
- if a tiny shared test helper is necessary, keep it test-only, narrow, and
  behavior-preserving. Do not refactor accepted production or unrelated tests
  merely to remove cosmetic duplication;
- remove the stale copied scale equation.

KISS and YAGNI apply. The formal review also noted that the private DCP return
is semantically a quotient/remainder decomposition and that repeated ciphertext
manifest arguments form a data clump. These are judgement-call smells, not
authorization for a broad abstraction. Prefer clear structured bindings or one
small private named result/manifest only if it makes the touched code simpler;
do not widen the public API or refactor unrelated accepted code.

### 10. Retained TDD evidence is absent from the candidate tree

The delivery-level `TESTS.md` is not retained by any patch, while README claims
the project has final local Relin2 evidence.

Required correction:

- patch 07 may add documentation/evidence only, including a bounded
  `artifacts/tdd/relin2/` directory;
- retain the exact commands, environment/tool versions, exit status, named
  cases/count, and relevant stdout/stderr for every actually executed API red,
  scaffold green, runtime reds, core green, directed lifecycle red, and final
  green;
- the applied candidate tree must contain a concise evidence index under
  `artifacts/tdd/relin2/` that binds those records to the seven patch SHA-256
  values and cumulative trees available in your replay;
- the root delivery `TESTS.md` must agree byte-for-meaning with the retained
  evidence and state every unrun environment as `pending`;
- do not claim GitHub Actions or Windows. Codex will create and retain hosted
  Linux/Windows evidence later, after this static gate passes.

## Representative public-input coverage without false precision claims

The exact RNS fixture remains the arithmetic authority, but the first candidate
starts from encrypted `{0.0}` and then overwrites all raw elements. Add one
small, separately named public-input composition test using an encrypted,
deterministic CKKS packed vector containing representative nonzero real and
complex entries, a near-zero entry, and a moderate-magnitude entry. Reach
DCP -> Tensor2 -> Relin2 through public APIs, then compare Relin2 and public RCB
to the same exact coefficient/tower reference contract after verifying the deep
key cache is unchanged.

This test is coverage of the public input path, not a paper error-bound or
double-precision claim. Do not assert an unjustified decryption tolerance,
precision bit count, analytic error bound, or end-to-end Mult2 result. The
controlled RNS/key fixture, not random public encryption, remains the sole
authority for the fixed K, v/w, and centered-boundary witnesses.

## Red-first requirements for these corrections

Do not silently edit only the green implementation. Preserve the original
seven semantic patch boundaries and place every correction at the earliest
honest boundary:

- patch 01: exact compile-only signature red;
- patch 03: all Relin2 contract tests, including public RCB return, exact state,
  deep cache, fixed witnesses, exact diagnostics, representative public input,
  and absent-key immutability, must be registered and independently observed
  red on the immediate-throw scaffold where applicable;
- patch 04: complete core implementation and all core/legacy cases green,
  still without the Tensor2 lifecycle guard;
- patch 05: only the valid-public-`ReadyForRS2` directed lifecycle red;
- patch 06: only the smallest pre-arithmetic lifecycle guard;
- patch 07: documentation and retained evidence only.

If a hardening assertion cannot honestly fail on the scaffold because no valid
Relin2 output exists yet, document its dependency-red nature and demonstrate a
targeted mutation or temporary local negative check without changing the
ordered source-of-truth patches. Never call a compile failure, dependency
failure, or unexecuted case a behavioral red.

## Independent oracle constraints

The reference may use trusted public OpenFHE `Relinearize`, but it must remain
independent of production-private raise, DCP, key-validation, or manifest
helpers. It must not call `KeySwitchCore`, `EvalMultAndRelinearize`,
`RelinearizeInPlace`, `ModReduce`, or `Rescale`.

Using the same public primitive on the same controlled key is allowed only
after deep key-cache immutability has been checked. It proves the project
composition around the trusted primitive, not the primitive's internal
cryptography. Keep this limitation explicit in `REVIEW.md`.

## Replay and execution requirements

For every replacement patch, on a new disposable exact-base replay:

1. run `git apply --check --whitespace=error-all`;
2. apply in order;
3. run `git diff --check`;
4. record cumulative tree identity;
5. execute the exact boundary commands required by the original task;
6. preserve every intended red and subsequent green as actual output.

Final local Linux requirements:

- strict warning-clean C++17 build;
- unchanged accepted 6/6 suite plus every registered Relin2 case green;
- every test independently executed, not masked by an earlier failure;
- production forbidden-symbol scan clean;
- exactly two public output-returning `Relinearize` calls in Relin2;
- no production `try`/`catch`;
- no old implementation, modified OpenFHE, dependency addition, network action,
  commit, push, PR, or CI dispatch.

Mark Windows and hosted CI `pending` even if the original base has prior green
evidence. Do not infer the revised candidate result from the base.

## Replacement output contract

Return exactly one root-level ZIP named
`chatgpt-pro-relin2-01-remediation-delivery.zip` containing exactly ten regular
root files and no directory entries or links:

1. the seven replacement patches in the fixed order and names;
2. revised `REVIEW.md`;
3. revised `TESTS.md`;
4. revised `PATCHES.sha256` with exactly nine SHA-256 lines covering the other
   nine files.

Before returning it, verify safe paths, `unzip -t`, all nine checksums, clean
replay from the exact base, and the final local gates. Do not include source
archives, `.git`, build output, caches, browser state, credentials, environment
files, cookies, tokens, or unrelated artifacts.

Lead with exactly one verdict: `ready to apply`, `changes needed`, or
`blocked`. State exact local test counts and explicitly state Windows/hosted CI
as pending. Provide the ZIP through one download control only. If any fixed
witness, exact diagnostic, deep restoration, public RCB comparison, patch
boundary, or required test is missing, the verdict cannot be
`ready to apply`.

## Mechanical acceptance checklist

A `ready to apply` replacement must make every answer below yes:

- Does the exact API red prove the required Relin2 signature?
- Do all seven patches apply in order from exact `fb862a3...` with clean diffs?
- Is production still the paper's exact `(u, v+w)` construction with unchanged
  basis and exactly two public relinearizations?
- Is the production key map bound through `const auto&` and never mutated?
- Are K0/K1, common v/w, `+half`, `-half`, and carry all algebraically fixed at
  named coordinates with asserted residues?
- Does the boundary test assert production output rather than discard it?
- Does the valid test assert all state, tower parameters, member metadata, and
  Tensor immutability against independent expected state?
- Is the returned ciphertext from public RCB checked exactly and is its input
  pair unchanged?
- Does every production call prove the deep eval-key cache unchanged before a
  reference can consume it, and can the RAII guard restore mutated pointees?
- Does the missing-key case snapshot and verify both Tensor and cache?
- Are all new diagnostics compared by exact full-string equality?
- Are legacy ReadyForFirstMult diagnostics and the unchanged 6/6 suite
  preserved?
- Are copied legacy Tensor2 routes and stale duplicate oracle code gone?
- Is one bounded representative real/complex public-input composition case
  present without an unsupported precision claim?
- Are exact local red/green records retained inside the candidate tree and
  consistent with `TESTS.md`?
- Are Windows and hosted CI honestly pending?

Any no answer requires `changes needed` or `blocked` with the exact reason.
