# Mult2 and pair Add/Sub preflight

Status: preliminary read-only gate for later task drafting. This is not an
implementation task, patch, build, test result, precision result, or
authorization to begin either slice before the exact Relin2 and RS2 gates are
green.

Reviewed against:

- paper 2023/1788 PDF and text;
- accepted clean-room Tensor2 base
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`, tree
  `759d5195739684748d5a9664edabe3fa719e1acf`;
- pristine OpenFHE 1.5.0 commit
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`;
- `coordination/reviews/relin2-preflight.md`;
- `coordination/reviews/rs2-preflight.md`;
- three independent read-only paper, OpenFHE/API, and adversarial-TDD reviews.

The reviews did not edit source, build, test, browse for implementations, or
read any quarantined prior implementation.

## 1. Bounded conclusion

1. Paper Mult2 is exactly the composition
   `Tensor2 -> Relin2 -> RS2`. The public wrapper must not repeat, fuse, or
   replace any of those three project algorithms.
2. Paper pair Add/Sub is an explicit independent capability: add or subtract
   the two pair members componentwise. It is not an internal Mult2 stage.
3. Pair Add/Sub can use pristine OpenFHE's public output-returning `EvalAdd`
   and `EvalSub`, exactly once for high and once for low. No new production
   coefficient arithmetic is required.
4. OpenFHE's checks are not a safe pair-compatibility boundary. In
   `FIXEDMANUAL`, public Add/Sub may silently drop towers to align levels and
   may accept different RLWE component counts. The project must reject every
   mismatch before either public call.
5. The exact supported lifecycle set and output metadata-map contract cannot
   be frozen until accepted Relin2/RS2 establish the real pair state model.
   Cross-lifecycle arithmetic is prohibited regardless.

## 2. Paper facts

### 2.1 Pair and recombination

For `C = (h, l)` and fixed recombination divisor `q_div`, Definition 3.3 is

```text
RCB_qdiv(h, l) = q_div*h + l.
```

Paper anchors: PDF page 6; text lines 415-447. After homomorphic operations,
recombination is generally defined only over the current `Q_l`; no later lift
back to `Q_l*q_div` may be assumed (text lines 459-467).

### 2.2 Pair Add/Sub is explicit

The paper states that multi-precision addition and subtraction can be carried
out componentwise on ciphertext pairs (PDF page 7; text line 578). Therefore:

```text
Add((h1,l1), (h2,l2)) = (h1+h2, l1+l2)
Sub((h1,l1), (h2,l2)) = (h1-h2, l1-l2)

RCB(Add(C1,C2)) = RCB(C1) + RCB(C2) mod Q_l
RCB(Sub(C1,C2)) = RCB(C1) - RCB(C2) mod Q_l.
```

The paper does not define software lifecycle, metadata, auto-alignment,
diagnostics, or a separate error theorem for these operations. Those are
project contracts and must not be attributed to the paper.

### 2.3 Exact Mult2 pipeline

| Stage | Paper formula | Shape and basis | Anchor |
| --- | --- | --- | --- |
| Input | `Ci=(hi,li)` | each member in `R^2_Ql` | Definition 4.1, text 584-604 |
| Tensor2 | `h3=h1 tensor h2`; `l3=h1 tensor l2 + l1 tensor h2`; omit `l1 tensor l2` | two `R^3_Ql` members; no tower drop | text 590-606 |
| Relin2 | if `(u,v)=DCP_qdiv(Relin(q_div*h3))` and `w=Relin(l3)`, return `(u,v+w)` | two `R^2_Ql` members; no tower drop | Definition 4.3, text 664-683 |
| RS2 | `a=RS_ql(u)`; `c=RS_ql(q_div*u+v+w)`; return `(a,c-q_div*a)` | two `R^2_Q(l-1)` members; consume only `q_l` | Definition 4.5, text 785-815 |
| Mult2 | `RS2 o Relin2 o Tensor2` | pair over `Q_(l-1)` | Definition 4.7, text 895-901 |

The exact composition identities are:

```text
RCB(Relin2(T)) = Relin(q_div*h3) + Relin(l3) mod Q_l
RCB(RS2(P)) = RS_q_l(RCB(P)) mod Q_(l-1).
```

The first is Lemma 4.4, text 735-743; the second is the Definition 4.5
derivation, text 809-815. Centered rounding is not linear, so RS2 cannot be
replaced by independently rescaling high and low and adding afterward.

### 2.4 Paper ambiguity that must remain visible

Theorem 4.8's displayed formula at text 903-935 appears to divide the input
product only by `q_l`, while Lemma 4.2, the exact pipeline algebra, and the
following prose at text 949-958 require a total divisor `q_div*q_l`. Until an
author correction is available:

- coefficient identities and logical-scale contracts follow the definitions
  and lemmas;
- no report may say that Theorem 4.8's displayed formula is unambiguous;
- no decoded-slot precision theorem is verified without a concrete theorem
  instance and all of its error prerequisites.

## 3. Pristine OpenFHE 1.5.0 facts

Canonical source paths below are relative to official commit `df495ba...`.

### 3.1 Public symbols

- `CryptoContextImpl::EvalAdd(const ConstCiphertext&, const ConstCiphertext&)`:
  `src/pke/include/cryptocontext.h:1420-1423`.
- `EvalAddInPlace`: `cryptocontext.h:1431-1434`.
- `EvalAddInPlaceNoCheck`: `cryptocontext.h:1436-1442`.
- mutable Add variants: `cryptocontext.h:1451-1465`.
- `CryptoContextImpl::EvalSub(const ConstCiphertext&, const ConstCiphertext&)`:
  `cryptocontext.h:1639-1642`.
- `EvalSubInPlace`: `cryptocontext.h:1650-1653`.
- mutable Sub variants: `cryptocontext.h:1662-1676`.
- `ConstCiphertext` and `ReadOnlyCiphertext` aliases:
  `src/pke/include/ciphertext-fwd.h:47-54`.

There is no pristine `MakeCiphertextLike` or `CryptoContext::MakeCiphertext`.
The public object operations are `CiphertextImpl::CloneEmpty()` at
`src/pke/include/ciphertext.h:385-400`, `Clone()` at 402-405, and
`SetElements()` at 203-214. Pair Add/Sub does not need a project-built shell;
the output-returning public operations already clone the left operand.

### 3.2 What OpenFHE validates and what it does not

`CryptoContextImpl::TypeCheck` at `cryptocontext.h:263-288` checks nulls,
calling context, both ciphertext contexts, key tags, and encoding types. It
does not prove:

- ordered basis or per-tower parameter identity;
- equal tower count, level, slots, or Evaluation format;
- equal RLWE component count;
- equal noise-scale degree or recorded scaling factor;
- equal project divisor, lifecycle, or logical scales.

The output-returning Add/Sub paths clone the left operand and, outside
`NORESCALE`, clone the right operand before automatic alignment:
`src/pke/lib/schemerns/rns-leveledshe.cpp:46-62,114-130`. This protects the
original ciphertext objects but does not make mismatched inputs valid.

Under `FIXEDMANUAL`, `AdjustLevelsInPlace` uses member-zero tower counts. A
longer input may be silently `LevelReduceInternalInPlace`-reduced; equal tower
counts with different levels, order, or parameters are not detected:

- `src/pke/lib/schemerns/rns-leveledshe.cpp:393-447`;
- `src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp:197-201`.

The Add/Sub core verifies only member-zero tower counts and computes up to the
maximum RLWE component count instead of rejecting unequal shapes:
`src/pke/lib/schemebase/base-leveledshe.cpp:541-551,574-617`. DCRT `+=/-=`
iterates over the left tower count without proving format or parameter
identity: `src/core/include/lattice/hal/default/dcrtpoly-impl.h:374-407`.

Therefore project validation must precede every public Add/Sub call.

### 3.3 Successful strict-compatible behavior

For two-component, same-basis, same-format inputs, public non-in-place Add/Sub
is exact componentwise polynomial Add/Sub. The result keeps the corresponding
left member's context, key tag, encoding, slots, level, noise-scale degree,
recorded factor, hops, integer factor, and metadata map; OpenFHE does not
recompute them. Thus `result.high` inherits from `left.high` and `result.low`
inherits from `left.low`.

`CiphertextImpl::CloneEmpty()` creates a new metadata map containing the same
`shared_ptr<Metadata>` values, not deep-cloned metadata values
(`ciphertext.h:322-330,390-398`). Each output member has an outer map distinct
from every input map, while its metadata-value pointers shallow-alias the
corresponding left member; right metadata is not merged. The minimal contract
is therefore explicit left-member metadata inheritance plus input state that
is observably unchanged during and immediately after the call. It must not
claim metadata merge, exchange symmetry, deep metadata-value isolation, or
future independence after mutation through an aliased metadata pointer.

## 4. Preliminary public API boundary

The already-proposed KISS surface remains:

```cpp
CiphertextPair Add(const CiphertextPair& left,
                   const CiphertextPair& right) const;
CiphertextPair Sub(const CiphertextPair& left,
                   const CiphertextPair& right) const;
CiphertextPair Mult2(const CiphertextPair& left,
                     const CiphertextPair& right) const;
```

Do not add in-place variants, operator overloads, a generic binary-operation
framework, auto-align flags, plaintext/scalar variants, `t>2`, refresh, or a
second-multiplication API.

`CiphertextPair` must remain non-publicly constructible. Tensor three-component
objects are not accepted by Add/Sub.

## 5. Pair Add/Sub contract

### 5.1 Required validation order

Before any clone, public OpenFHE call, getter with unsafe raw assumptions, or
arithmetic:

1. fully validate `left`;
2. fully validate `right`;
3. validate mutual compatibility;
4. only then call the public primitive.

Mutual compatibility must include exact:

- crypto-context identity and operation context;
- `q_div`;
- ordered moduli, tower count, every tower parameter/root, and level;
- two RLWE components per member and Evaluation format;
- key tag, CKKS encoding, and slots;
- lifecycle;
- noise-scale degree and current OpenFHE recorded scaling factor;
- `PaperScaleDescriptor::inputRecordedScalingFactor` as an independent
  historical field;
- high and recombined logical scales;
- every accepted manifest/descriptor invariant.

The final set of same-lifecycle states supported by Add/Sub remains pending the
exact-green gate. Candidates are `ReadyForFirstMult`, `ReadyForRS2`, and the
neutral provisional `AfterFirstRS2`; listing a candidate does not accept it.
Cross-lifecycle Add/Sub, automatic lifecycle conversion, and automatic
level/scale/divisor repair are always rejected. `AfterFirstRS2` records only a
completed first RS2 and makes no paper-unsupported refresh claim.

### 5.2 Minimal production path

After all checks:

```text
result.high = context->EvalAdd(left.high, right.high)
result.low  = context->EvalAdd(left.low,  right.low)
```

or the same two calls with `EvalSub`. Use output-returning methods only; do not
use InPlace, Mutable, NoCheck, or a private DCRT component loop.

Construct the result with the proven-compatible left descriptor, explicit
left-member metadata semantics, and unchanged lifecycle, logical scales,
current recorded factor, `inputRecordedScalingFactor`, and degree. Fully
validate both current and historical factor fields before returning.

### 5.3 Independent oracle and fixed witnesses

For high and low, each RLWE component, tower, and coefficient:

1. clone and convert test inputs to COEFFICIENT;
2. reconstruct signed coefficients with independent `cpp_int` CRT over `Q`;
3. compute `(x+y) mod Q` or `(x-y) mod Q` independently;
4. reduce independently into every tower;
5. compare with the actual project output.

Also independently prove:

```text
q_div*(h1 +/- h2) + (l1 +/- l2)
    = RCB(C1) +/- RCB(C2) mod Q.
```

The expected path must not call project Add, Sub, or RCB. Separately bind the
real return from `module.RCB(actualResult)` and compare every component, tower,
and coefficient with the independently computed
`RCB(C1) +/- RCB(C2)`. Verify its complete state and its metadata inheritance
from `actualResult.high`, and snapshot `actualResult` before the call to prove
public RCB leaves it observably unchanged during and immediately after the
call. This checks the public RCB result; the displayed identity alone is not an
oracle for that implementation.

Fixed witnesses cover positive/negative modular wrap, carry/borrow, exact
cancellation, Sub operand direction, nonzero high/low, and a low term crossing
the centered `q_div/2` boundary so componentwise output differs from an
incorrect `DCP(RCB(left) +/- RCB(right))` implementation.

### 5.4 State and adversarial tests

Successful tests assert unchanged basis, level, lifecycle, component count,
format, context/tag/slots/encoding, degree, current recorded factor,
`inputRecordedScalingFactor`, divisor, and both logical scales. State snapshots
must prove both pairs, their distinct outer metadata maps, their map entries,
and immediately observable metadata values are unchanged; this is not a claim
that returned shallow-aliased metadata values are isolated from later mutation.

Every mismatch class is an independent negative test with exact
`std::invalid_argument`, `DoubleCKKS: `, and a stable field diagnostic:

- local left or right corruption;
- context, key tag, slots, encoding, or format;
- component count;
- tower count, order, modulus/root/parameter identity, or level;
- degree, current recorded factor, descriptor `inputRecordedScalingFactor`,
  high scale, recombined scale, divisor, or lifecycle.

Composite degree is not an Add/Sub precondition or negative-test dimension;
it is an RS2/Mult2 boundary. Add/Sub performs no rescale and must not be
artificially narrowed by the RS2-only `compositeDegree == 1` contract.

Tower-count and unequal-component cases must prove the project rejects before
OpenFHE's silent drop/maximum-shape behavior. Different-key-tag cases must
prove project attribution rather than an upstream `TypeCheck` exception.
Because a black-box failure cannot prove that a successful output-returning
OpenFHE call was not executed and discarded first, source-order review must
also establish that complete left, right, and mutual validation precede both
public Add/Sub calls.

## 6. Mult2 contract

### 6.1 Minimal production path

After its own full left, right, mutual, lifecycle, and composite-degree
preflight, Mult2 uses named temporaries only. Whether it also shares Relin2's
read-only evaluation-key preflight before Tensor arithmetic, and therefore the
exact missing-key failure position, remains an explicit Section 8 exact-green
decision rather than a fact in this preliminary gate:

```text
tensor       = Tensor2(left, right)
relinearized = Relin2(tensor)
result       = RS2(relinearized)
```

Do not call `EvalMult`, `EvalMultAndRelinearize`, `Relinearize`, or `Rescale`
directly inside this wrapper. Do not recompute coefficients or repair metadata.
Ordinary `EvalMultAndRelinearize` is a different algorithm; its pristine public
entry is `src/pke/include/cryptocontext.h:2051-2070`.

### 6.2 Derived final state

Subject to exact Relin2/RS2 confirmation:

| Field | Input -> Tensor2 -> Relin2 -> RS2/Mult2 output |
| --- | --- |
| basis | `Q_l -> Q_l -> Q_l -> Q_(l-1)` |
| level | `1 -> 1 -> 1 -> 2` |
| components/member | `2 -> 3 -> 2 -> 2` |
| noise-scale degree | `2 -> 3 -> 3 -> 2` |
| current OpenFHE recorded factor | `SF_i -> SF1*SF2/baseSF -> same -> SF1*SF2/baseSF^2` |
| `PaperScaleDescriptor::inputRecordedScalingFactor` | `SF_i -> N/A` (`TensorScaleDescriptor`) `-> SF_T=SF1*SF2/baseSF -> retained SF_T` |
| high logical scale | `H_i -> H1*H2 -> same -> H1*H2/q_l` |
| recombined logical scale | `R_i -> R1*R2/q_div -> same -> R1*R2/(q_div*q_l)` |
| lifecycle | `ReadyForFirstMult -> TensorCiphertextPair -> ReadyForRS2 -> AfterFirstRS2` |

The fixture must make `q_div`, exact active prime `q_l`, and `baseSF` distinct.
Context, key tag, slots, CKKS encoding, and Evaluation format remain fixed.
`q_div` remains the pair recombination divisor; RS2 consumes only `q_l`.
The RS2-relative metadata candidate is `final.high <- RS2 input.high` and
`final.low <- RS2 input.high`. Their outer maps are distinct, while metadata
value pointers shallow-alias that RS2 input member. Which original
Tensor2/Relin2 sentinel this ultimately denotes remains pending the exact-green
gate and must not be frozen here.

### 6.3 Independent composition oracle

The actual path calls only public project `Mult2`. The expected path must not
call project `Tensor2`, `Relin2`, `RS2`, `Mult2`, or their private helpers.

1. Reconstruct input coefficients with independent CRT and build the exact
   three-component Tensor polynomials using schoolbook negacyclic convolution;
   omit low-low.
2. Build test-owned valid ciphertexts for Definition 4.3's `q_div*h3`: multiply
   every existing `Q_l` residue of high by `q_div`, then append a `q_div` tower
   whose residue is zero. Only then call trusted pristine public `Relinearize`
   once for raised high and once for low.
3. Apply test-owned centered DCP to raised-high's result, obtaining `(u,v)`,
   then independently compute `v+w`.
4. Apply test-owned centered RS to `u` and `q_div*u+v+w`, obtaining
   `A`, `C`, and `B=C-q_div*A`.
5. Compare all actual `(A,B)` components, towers, and coefficients, and
   independently prove `q_div*A+B=C`.
6. Separately bind the real return from `module.RCB(actualResult)` and compare
   every component, tower, and coefficient with the independent RS oracle's
   `C`. Verify its complete state and metadata source from `actualResult.high`;
   snapshot `actualResult` first and prove public RCB leaves it observably
   unchanged during and immediately after the call.

This oracle independently verifies the project composition, not pristine
OpenFHE key-switch arithmetic; `Relinearize` remains an explicitly trusted
primitive.

Permanent fixed witnesses must cover:

- `X^(N-1)*X=-1` negacyclic wrap;
- visible omission of nonzero low-low;
- nonzero Tensor third component and nonzero relinearization `K0/K1` effects;
- centered DCP positive/negative boundaries and carry;
- centered RS positive/negative boundaries;
- `B != RS(low)`;
- a first real Mult2 output rejected on a second Mult2 before Tensor/key-cache
  access with the exact project-owned `ReadyForFirstMult` input-lifecycle
  diagnostic. The rejection proves only that a second multiplication is not
  implemented in this bounded project state, not that refresh is required.

Black-box tests prove the returned diagnostic and observable immutability;
source-order review proves that no discarded arithmetic or key-cache access
happened first.

Every valid and failing Mult2 path snapshots both input pairs, outer metadata
maps, entries, and immediately observable metadata values and proves them
unchanged without claiming future isolation from shallow-aliased outputs. Every
evaluation-key success, missing-key, or malformed-key fixture uses a test-owned
RAII guard that saves and restores the entire prior static evaluation-key map
even if an assertion throws; no test may rely on `EvalMultKeyGen` overwriting an
existing tag entry.

## 7. Minimal TDD patch order

### 7.1 Pair Add/Sub slice

1. Separate Add and Sub compile-only tests; retain both missing-API reds.
2. Add final declarations and unnamed-parameter immediate-throw scaffolds;
   preserve the complete old suite.
3. Add all named valid/oracle/negative/order CTests and retain independent
   scaffold reds.
4. Add the smallest full validation plus two-public-call implementation for
   each method; retain first complete green.
5. Only after green, make necessary local refactors/docs without changing the
   oracle.

### 7.2 Mult2 slice

1. Retain an independent missing-API compile red.
2. Add the final declaration and immediate-throw scaffold.
3. Retain independent validation/lifecycle/composite reds. Add the exact
   missing/malformed-key ordering reds only after accepted Relin2/RS2 close
   Section 8's key-preflight placement decision; this preliminary gate does
   not mandate a pre-Tensor missing-key order.
4. Implement only public preflight while valid input still reaches scaffold.
5. Add final exact HYBRID/BV composition-oracle reds.
6. Implement only the named `Tensor2 -> Relin2 -> RS2` composition and retain
   first complete green.
7. Add the actual-first-output to second-Mult2 regression. If the existing
   lifecycle guard already makes it green, record inherited green honestly;
   never manufacture a red.
8. Run full regression, strict warning build, and exact same-commit Linux and
   Windows evidence before documentation closure.

Every API/test/source/workflow/doc boundary is a separate small commit, pushed
immediately. Red Windows jobs are unnecessary after the corresponding exact
Linux red is retained; final Linux and Windows must bind the same commit.

## 8. Deferred exact-green gates

Do not draft an executable implementation task until accepted Relin2 and RS2
provide all of:

- exact source SHA/tree and complete old/new CTest count;
- exact Linux/Windows successful run and toolchain identities;
- accepted `ReadyForRS2` and `AfterFirstRS2` state fields, including the
  distinction between retained descriptor input factor `SF_T` and current
  recorded factor `SF_T/baseSF`;
- actual HYBRID/BV support and fixed key-switch/DCP/RS witness coordinates;
- exact `q_l`, level, degree, factor, and dual-scale observations;
- source-verified metadata-map inheritance and aliasing behavior;
- a decision on which same lifecycle values pair Add/Sub supports;
- whether Mult2 shares a project-private read-only key preflight with Relin2,
  and therefore exactly where missing-key failure occurs.

Any mismatch, auto-alignment, generic algebra framework, production
`try/catch`, mock/private test seam, refresh implementation, second
multiplication support, performance claim, or decoded-slot error-bound claim
is a reject reason.
