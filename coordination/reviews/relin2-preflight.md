# Relin2 clean-room preflight

Prepared: 2026-09-01 Asia/Shanghai

Status: read-only paper/OpenFHE preflight for the next bounded slice. No Relin2
source, test, build, execution, or external-agent delivery is claimed here.
The provisional implementation base is the Tensor2 exact source/test head
`fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`; a final external task must still
bind the accepted exact SHA after the active Tensor2 exact-current closure
review.

## Scope boundary

This slice implements only the first-multiplication Relin2 transition. It does
not implement RS2, the Mult2 wrapper, pair Add/Sub, rotations, bootstrapping,
repeated multiplication, precision/performance claims, or a network-security
assessment. The dependency order remains:

```text
Tensor2 -> Relin2 -> RS2 -> Mult2 wrapper
```

The preflight used only the user-supplied paper, pristine OpenFHE 1.5.0, and
the trusted exact Tensor2 branch. It did not read any former, local, private,
known-wrong implementation or author proof-of-concept.

## Paper contract

Definition 4.3, paper page 7 / extracted-text lines 664-674, gives a Tensor2
output

```text
CT = (high3, low3) in R^3_Ql x R^3_Ql
```

and defines

```text
Relin2(CT) = DCP_qdiv(Relin(q_div * high3)) + (0, Relin(low3)).
```

Writing

```text
(u, v) = DCP_qdiv(Relin(q_div * high3))
w      = Relin(low3)
```

the exact implementable pair formula is `(u, v + w)`. Both output members
have two RLWE components over the unchanged working modulus `Q_l`. Relin2 does
not consume the active multiplication tower.

The paper requires the high path to temporarily represent `q_div * high3`
over `q_div * Q_l`; ordinary relinearization occurs on that full basis and
private DCP returns it to `Q_l`. For the current first-lifecycle RNS layout,
the engineering derivation is to multiply every existing tower by `q_div`,
then append the `q_div` tower as exactly zero. The low path is relinearized
directly on `Q_l`. The append-zero-tower representation is a CRT/OpenFHE
mapping, not a step stated verbatim by the paper.

The following are not equivalent implementations:

- relinearizing high and low independently on `Q_l`; the paper says the high
  error is numerically destructive;
- `DCP o Relin o RCB`; the paper explains that this consumes an extra
  `q_div` factor.

Lemma 4.4, paper pages 7-8 / extracted-text lines 688-783, first gives the
exact recombination identity modulo `Q_l`:

```text
RCB_qdiv(Relin2(CT))
  = Relin(q_div * high3) + Relin(low3).
```

Its error statement has two distinct layers. Let

```text
A = RCB_qdiv(Relin2(CT)) * (1, s)
B = RCB_qdiv(CT)          * (1, s, s^2).
```

Without an additional centered-representative hypothesis, the lemma gives
the modular-difference bound

```text
|| [A - B]_Ql ||_infinity <= E_Relin + h.
```

Only if `||[B]_Ql||_infinity <= M` and
`2 * (M + E_Relin + h) < Q_l / 2` may it conclude the separately centered
bound

```text
|| [A]_Ql - [B]_Ql ||_infinity <= E_Relin + h.
```

A test may assert the exact coefficient identity above. It must not claim
either analytic bound unless `E_Relin`, the secret-key Hamming weight, the
centered norms, and every applicable precondition are explicitly computed for
that execution.

Relin2 preserves the Tensor2 paper-scale meanings. This is a mathematical
inference from the formula, not an OpenFHE metadata statement:

```text
H_after_Relin2 = H_after_Tensor2
R_after_Relin2 = R_after_Tensor2
```

`q_div` remains distinct from OpenFHE's real `baseSF`.
It is also distinct in role from the future `q_l` consumed by RS2; Relin2 does
not execute `RS_ql`.

## Observed OpenFHE 1.5.0 behavior

All paths below are relative to the pristine OpenFHE tree at
`df495ba2e91739a6dc8f1de254fc5a41155ce504`.

- `EvalMultKeyGen` generates the one `s^2 -> s` key needed for a three-member
  ciphertext and stores it in the static map keyed only by key tag
  (`src/pke/lib/schemebase/base-leveledshe.cpp:136-144`,
  `src/pke/lib/cryptocontext.cpp:87-103`).
- `GetEvalMultKeyVector` throws if the tag is absent
  (`src/pke/lib/cryptocontext.cpp:186-198`). `GetAllEvalMultKeys` exposes the
  map, so the project can perform a fail-fast read-only lookup without a
  production `try`/`catch`.
- Public output-returning `Relinearize` checks only non-null input and key-vector
  length. It does not validate ciphertext context, key context/tag, modulus
  identity, or tower order (`src/pke/include/cryptocontext.h:2021-2031`).
- The implementation clones the ciphertext, key-switches component two into
  components zero and one, and resizes to two. It sets evaluation format but
  does not change level, noise-scale degree, recorded scaling factor, slots,
  or key tag (`src/pke/lib/schemebase/base-leveledshe.cpp:319-341`).
- HYBRID and BV key switching infer the active level from tower count and
  assume the current basis is the exact ordered prefix of the full basis;
  neither independently proves tower identity
  (`src/pke/lib/keyswitch/keyswitch-hybrid.cpp:314-430`,
  `src/pke/lib/keyswitch/keyswitch-bv.cpp:245-277`).
- There is no suitable high-level append-zero-tower API. Constructing a
  `DCRTPoly` from an ordered NativePoly vector preserves that supplied order
  (`src/core/include/lattice/hal/default/dcrtpoly.h:99-106`,
  `src/core/include/lattice/hal/default/dcrtpoly-impl.h:109-123`).
- After installing the full elements, setting the temporary ciphertext level
  to zero makes its metadata consistent with the restored full basis
  (`src/pke/include/ciphertext.h:203-243`,
  `src/pke/lib/ciphertext-impl.cpp:38-53`).

Therefore no OpenFHE source change is needed, but module-owned input and key
validation is mandatory before calling public `Relinearize`.

## Minimal module design

### Public state seam

Keep the already reviewed public seam:

```cpp
enum class PairLifecycle : std::uint8_t {
    ReadyForFirstMult,
    ReadyForRS2,
};

CiphertextPair Relin2(const TensorCiphertextPair& tensor) const;
```

This is a KISS decision rather than a paper fact. Tensor2 needs a distinct type
because its members have three components and must never enter two-component
APIs. Relin2 returns the existing two-component shape; its existing lifecycle
field is the smaller state boundary than duplicating every pair member and
getter in a third type.

`CiphertextPair` validation must branch explicitly on lifecycle. Existing
`ReadyForFirstMult` predicates and diagnostics remain byte-for-byte stable.
`ReadyForRS2` requires the Tensor2 recorded factor/degree, level one, the exact
ordered first-pair basis, two components, and preserved context/divisor/tag/
slots/format. Relin2 additionally fails before key lookup or arithmetic unless
the Tensor input has at least as many active `Q_l` towers as its noise-scale
degree. For the current degree-three state this means at least three active
towers and therefore at least four towers in the full basis including
`q_div`. This Relin2-only precondition must not narrow the existing public
DCP/RCB contract for three-tower contexts.

The existing `PaperScaleDescriptor` should retain its three public fields and
gain one explicit recombined-logical-scale field. For `ReadyForFirstMult`, the
high logical scale is the current quotient scale and the recombined scale is
the input logical scale. For `ReadyForRS2`, Relin2 copies Tensor2's high and
recombined logical scales. No validation may derive either one by replacing
the integer `q_div` with `baseSF`.

### Private implementation seams

1. Keep public `DCP`'s fresh-input contract unchanged.
2. Extract its already-tested coefficient arithmetic into a private helper
   that accepts a fully validated two-component, full-basis ciphertext and
   returns the two raw decomposed ciphertext members with metadata preserved.
   Public DCP and Relin2 may share this helper; neither may bypass its own
   state-specific validation.
3. Validate the complete `TensorCiphertextPair` before raw access, cloning,
   multiplication, tower construction, or key lookup.
4. Read `GetAllEvalMultKeys` and fail with stable project-owned
   `std::invalid_argument` before arithmetic unless:
   - the Tensor key tag is present;
   - the vector contains at least one key; extra later keys are permitted, while
     only index zero is validated and consumed for this three-component input;
   - that first key is non-null;
   - the key context is the bound context;
   - the key tag matches;
   - only the first `s^2` key that this three-component input will consume is
     accepted;
   - before any A/B getter, the first entry dynamically casts without an
     exception to `EvalKeyRelinImpl<DCRTPoly>`; a different public-map
     `EvalKeyImpl` subtype receives the stable project-owned invalid-key
     diagnostic instead of reaching the base getter's OpenFHE exception;
   - for HYBRID, its A/B vector lengths exactly equal `GetNumPartQ()`, every
     entry is in evaluation format, and every entry has the exact bound
     context `ParamsQP` basis;
   - for BV, its A/B vector lengths equal the full-`Q` digit decomposition
     count derived from the bound context and digit size (`|Q|` when digit size
     is zero, otherwise the sum of `ceil(q_i.GetMSB() / digitSize)`), every
     entry is in evaluation format, and every entry has the exact full ordered
     `Q` basis.
5. Raise high by multiplying every active tower of all three components by the
   integer `q_div`, append an evaluation-format zero tower using the bound
   context's final tower parameters, install the elements, then set level zero.
   This is representation construction, not CKKS scalar multiplication; do
   not use a floating-point scalar API or change logical/metadata scales.
   Before key switching, validate all three raised components as the exact full
   ordered basis, level zero, degree three, factor `SF_T`, and bound
   context/tag/slots/CKKS encoding/evaluation format.
6. Call output-returning `context_->Relinearize` exactly once on raised high
   and once on low. Before private DCP, validate relinearized high as the exact
   full ordered basis, level zero, two components, degree three, factor `SF_T`,
   and the same bound context/tag/slots/encoding/evaluation format.
7. Privately DCP the validated relinearized high. Before adding, fail fast unless its
   remainder member and relinearized low both have two components, the same
   exact ordered `Q_l` prefix and level one, degree three, factor `SF_T`, and
   identical context/tag/slots/encoding/format. This prevents OpenFHE EvalAdd
   from silently aligning an incorrect basis or hiding bad metadata.
8. Add relinearized low only to the DCP remainder member. Validate the complete
   result before returning.

No production `try`/`catch`, key-cache mutation, upstream modification,
speculative compatibility path, lock/compatibility layer, or direct
`KeySwitchCore` call is needed. Public `Relinearize` reads the global evaluation
key cache again after the project preflight. This bounded slice therefore does
not promise thread safety against concurrent mutation of that cache.

## Provisional OpenFHE metadata mapping

This table is an engineering derivation from the current Tensor2 source and
observed pristine OpenFHE behavior, pending execution of the Relin2 tests. The
paper itself specifies the `Q_l -> q_div * Q_l -> Q_l` basis path, the
three-to-two component transition, and preservation of the working modulus.
Preservation of the logical high/recombined scales is a mathematical inference
from Definition 4.3. Neither category specifies OpenFHE level numbers, noise
degree, or recorded scaling-factor metadata.

| Stage | Ordered basis | Level | Components/member | Noise degree | Recorded factor |
|---|---|---:|---:|---:|---:|
| Relin2 input / Tensor2 output | `Q_l` prefix | 1 | 3 | 3 | `SF_T` |
| raised high | full `Q_l * q_div` | 0 | 3 | 3 | `SF_T` |
| relinearized raised high | full basis | 0 | 2 | 3 | `SF_T` |
| private DCP high pair | `Q_l` prefix | 1 | 2 each | 3 | `SF_T` |
| relinearized low | `Q_l` prefix | 1 | 2 | 3 | `SF_T` |
| Relin2 output | `Q_l` prefix | 1 | 2 each | 3 | `SF_T` |

Here `SF_T = SF_left * SF_right / baseSF` is OpenFHE's already-normalized
Tensor2 recorded factor. The paper high/recombined logical scales are copied
unchanged and remain separately derived with the integer `q_div`.

## Required red-green evidence

The eventual task must preserve the same ordered TDD discipline:

1. API compile red for the missing lifecycle/API/scale field, after the
   existing production library compiles.
2. A non-mergeable fail-before-access scaffold.
3. Independently registered complete runtime reds on that scaffold.
4. The smallest complete implementation green.
5. Documentation-only finalization, exact Linux/Windows hosted verification,
   and exact-current review.

At minimum, the independent runtime cases are:

- `relin2_valid_arithmetic_metadata_immutability`: generate the ordinary
  evaluation key; use a test-owned append-zero-tower construction, public
  OpenFHE relinearization primitive, and Boost `cpp_int` centered DCP oracle;
  compare every output component/tower/coefficient against `(u, v+w)` modulo
  its active `Q_l` residue; assert the exact per-residue recombination identity
  and deep input immutability.
- `relin2_result_state_scale`: assert exact basis/level/two-component shape,
  `ReadyForRS2`, degree/factor, context/divisor/tag/slots/format, and separately
  preserved paper high/recombined scales.
- `relin2_tensor_validation_order`: corrupt one Tensor manifest field and
  require the field-specific project diagnostic even when no evaluation key is
  installed, proving Tensor validation precedes key lookup or arithmetic.
- `relin2_insufficient_active_basis`: use the existing degree-three/full-three-
  tower fixture, whose Tensor state has only two active towers, and require a
  stable project-owned insufficient-basis diagnostic before key lookup or
  arithmetic. Existing DCP/RCB support for that fixture remains valid.
- `relin2_missing_eval_key`: a valid Tensor result without `EvalMultKeyGen`
  must fail with the project-owned missing-key diagnostic, not a downstream
  OpenFHE exception.
- `relin2_eval_key_prearithmetic_compatibility`: independently place a null
  first entry, wrong-context key, wrong actual key tag, wrong key subtype,
  malformed A/B vector length, wrong internal basis, or non-Evaluation-format
  A/B entry under the expected map key; require project-owned rejection before
  raising/key switching. Exercise the HYBRID- and BV-specific shapes separately.
  Any test that mutates the static key map must restore the prior map with a
  test-owned RAII guard; it must not rely on `EvalMultKeyGen` overwriting an
  existing entry.

For exact recombination, the test-owned reference restricts the full-basis
`Relin(q_div * high3)` result to `Q_l` by deleting only the appended `q_div`
residue before comparison with level-one `Relin(low3)`. It must not call
`ModReduce` or `Rescale`, which would divide, rescale, and change metadata.

The arithmetic fixture must contain a nonzero third RLWE component. Centered
quotient/remainder boundary values must be identified on the actual
full-basis `Relin(q_div * high3)` result after the trusted reference
relinearization and immediately before the test-owned DCP; record the named
component and coefficient for each boundary/carry witness. The permanent green
assertion is the complete per-residue coefficient equality modulo `Q_l`
against the test-owned full-basis reference. A correct-versus-naive inequality
may remain in the executable suite only if its fixed construction
algebraically guarantees a difference; otherwise preserve the naïve-path
failure as retained red evidence rather than a probabilistic test over a
randomly generated key. Tests may use OpenFHE `Relinearize` as the primitive
under the composition test, but must not share production tower-raising or DCP
helpers.

Existing DCP/RCB and Tensor2 assertions and diagnostics must remain green and
byte-for-byte stable where promised. Tests must be extended to cover the new
recombined-logical-scale field on `ReadyForFirstMult`: DCP propagation,
validation, snapshot/immutability, and Tensor2 pre-arithmetic rejection after
field corruption. The Mac is not the sustained build runner; intended reds
and final exact cross-platform green belong on GitHub Actions or the isolated
Windows environment.

## Remaining execution questions

These are tests to run, not reasons to broaden the design:

- prove on hosted Linux that one full-basis HYBRID key switch and one
  level-one-prefix HYBRID key switch succeed with the same exact generated
  `s^2` key;
- execute and retain the correct-versus-naive raised-high witness;
- observe exact failure attribution for corrupted public key-map cases;
- re-check the final exact commit on Windows 2022/MSYS2 MinGW64.

Second-multiplication bases such as `q_div * Q_(l-1)` are not the original
ordered prefix and remain explicitly out of this slice.
