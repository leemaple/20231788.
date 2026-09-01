# RS2 preliminary paper/OpenFHE/TDD preflight

Recorded: 2026-09-01 Asia/Shanghai

Status: **preliminary pass for later task drafting, not implementation
authorization**. The mathematical formula and a minimal pristine-OpenFHE path
are viable. The exact task/base, final lifecycle predicates, diagnostic text,
fixture values, and execution claims must wait for accepted Relin2 patches and
exact Linux/Windows green evidence.

The revised document passed independent paper/TDD and pristine OpenFHE/API
read-only gates after correcting descriptor semantics, lifecycle naming,
metadata provenance/alias scope, public-RCB proof, exact API typing, and exact
integer multiplication. Those document gates do not replace the deferred
exact-Relin2 runtime gate below.

This is a clean-room algorithm/numeric-integration review. It uses the supplied
paper, accepted project contracts, and pristine OpenFHE 1.5.0 only. It does not
inspect or reuse any previous implementation and makes no build, test,
precision, performance, or network-security claim.

## Paper observations

### Ordinary rescale

Paper Section 2 and Definition 3.1 define the centered quotient operation:

```text
RS_q(ct) = round(q^-1 * ct) mod (Q_l/q)
         = (ct - Rem_q(ct)) / q.
```

The centered-quotient source anchors are extracted-text lines 237-245 and
345-361. The ordinary-RS decryption-polynomial error statement requires its
stated secret-weight/modulus hypothesis and is not a decoded-slot precision
guarantee.

### Definition 4.5

For a two-component ciphertext pair over `Q_l`, with active last prime
`q_l = Q_l/Q_(l-1)`, extracted-text lines 785-807 define:

```text
a = RS_q_l(high)
r = RS_q_l(q_div * high + low)

RS2_q_l(high, low) = (a, r - q_div * a).
```

The output belongs to
`R^2_Q_(l-1) x R^2_Q_(l-1)`. The second rescale applies to the complete
recombination. It cannot be distributed across `q_div*high + low`, because the
centered rounding operation is not linear.

### Exact recombination identity

Extracted-text lines 809-815 give the exact ciphertext identity modulo the
remaining basis:

```text
RCB_q_div(RS2_q_l(pair))
  = RS_q_l(RCB_q_div(pair))
  = RS_q_l(q_div * high + low).
```

RS2 consumes exactly `q_l`. It does not consume fixed divisor `q_div`. The
naive `DCP_q_div o RS_q_l o RCB_q_div` consumes both and is not an accepted
substitute.

### Lemma 4.6 boundary

Extracted-text lines 817-891 bound the recombined decryption-polynomial
rescale discrepancy by `(h+1)/2` under the paper's hypotheses. This is not a
separate bound for output high or low. In particular, the individual low member
contains both recombined and `q_div`-weighted high rounding terms. Do not claim
an analytic or decoded-slot precision result without instantiating every
required hypothesis.

### Mult2 relation

Definition 4.7, extracted-text lines 895-901, fixes
`Mult2 = RS2 o Relin2 o Tensor2`. This preflight does not implement or test the
wrapper.

## Mathematical inferences for the project state

Let accepted Relin2 output carry paper logical scales `H_in` and `R_in`:

| Stage | Ordered basis | Pair shape | Paper logical scales |
|---|---|---|---|
| RS2 input | `Q_l=[q0,...,q_l]` | two members, two RLWE components each | `H_in`, `R_in` |
| `a` and `r` | `Q_(l-1)` | two components each | divided by `q_l` |
| RS2 output | `Q_(l-1)` | two members, two components each | `H_in/q_l`, `R_in/q_l` |

For the first accepted multiplication:

```text
H_out = H_1 * H_2 / q_l
R_out = R_1 * R_2 / (q_div * q_l).
```

`q_l` is the input pair's exact last active modulus, not the context's fixed
`q_div` and not OpenFHE's real `baseSF`.

## Pristine OpenFHE 1.5.0 observations

The source is pinned at
`df495ba2e91739a6dc8f1de254fc5a41155ce504`.

- Public output-returning `CryptoContextImpl::Rescale` is an alias for public
  `ModReduce` and passes `GetCompositeDegreeFromCtxt()`:
  `src/pke/include/cryptocontext.h:2502-2531`.
- Output-returning RNS `ModReduce` clones the ciphertext before operating:
  `src/pke/lib/schemerns/rns-leveledshe.cpp:311-321`.
- On the project's supported FIXEDMANUAL path, public CKKS rescale increments
  level, decrements noise-scale degree, calls `DropLastElementAndScale` for
  every RLWE component, and updates the recorded factor; the public RNS entry
  gates that internal operation on FIXEDMANUAL:
  `src/pke/lib/schemerns/rns-leveledshe.cpp:317-321` and
  `src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp:172-190`.
- The coefficient/RNS arithmetic uses the actual active last prime selected by
  the current tower count. `DropLastElementAndScale` performs centered modulus
  switching and removes that tower:
  `src/core/include/lattice/hal/default/dcrtpoly-impl.h:691-711`.
- Under FIXEDMANUAL, `GetModReduceFactor` returns the fixed approximate scaling
  factor `baseSF=2^p`, not the exact prime:
  `src/pke/include/schemerns/rns-cryptoparameters.h:636-649`.
- `LevelReduce` merely removes towers and increments level; it is not the
  paper's centered RS:
  `src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp:197-201`.
- Public `EvalSub` can silently align different tower counts through level
  reduction, so project compatibility must be complete before subtraction:
  `src/pke/lib/schemerns/rns-leveledshe.cpp:393-407`.
- `CiphertextImpl::CloneEmpty` creates a new outer metadata-map container but
  shallow-copies its `shared_ptr<Metadata>` values:
  `src/pke/include/ciphertext.h:390-405`.
- Project RCB clones its high member and public Rescale clones its input. After
  the required complete compatibility recheck proves no alignment is needed,
  public EvalSub clones its first operand and changes only that result's
  elements; without that precheck it may level-reduce either working clone:
  `src/double_ckks.cpp:555-565`,
  `src/pke/lib/schemerns/rns-leveledshe.cpp:114-130,311-321,393-407`,
  `src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp:197-201`, and
  `src/pke/lib/schemebase/base-leveledshe.cpp:601-617`.

Therefore one valid public Rescale with composite degree one has the exact
metadata split:

```text
coefficient/paper division:  exact active q_l
recorded factor division:    baseSF = 2^p
level:                       +1
noise-scale degree:          -1
active basis:                remove exactly q_l
```

The paper logical fields and OpenFHE recorded factor must remain separately
validated.

## Required composite-degree gate

Public parameters expose `SetCompositeDegree`, while CKKS parameter validation
does not restrict that field for FIXEDMANUAL:

- setter: `src/pke/include/scheme/gen-cryptocontext-params.h:455-457`;
- validation: `src/pke/lib/scheme/gen-cryptocontext-params-validation.cpp:37-75`;
- protected context helper used by public Rescale:
  `src/pke/include/cryptocontext.h:452-460,2507-2510`.

If FIXEDMANUAL carries a composite degree greater than one, one public Rescale
can drop multiple towers. RS2 must reject `GetCompositeDegree()!=1` with a
stable project-owned diagnostic before cloning, non-validation raw arithmetic,
recombination, scalar multiplication, subtraction, or Rescale. Complete
validation may safely inspect members and towers after its null/shape gates.
This is an RS2-only precondition so DCP/RCB/Relin2 support is not silently
narrowed.

`CryptoContextImpl::GetCompositeDegreeFromCtxt()` is a protected helper, so
project production cannot and need not call it. The already-bound
`CryptoParametersCKKSRNS` is the same object from which that helper obtains the
value; RS2 must read `parameters_->GetCompositeDegree()` and require exactly
one. Public `Rescale` will then independently read the same value internally.
Do not copy the value into pair state or accept a caller-supplied override.

This guard cannot be replaced by a post-Rescale degree check. For composite
degree two, `ModReduceInternalInPlace` drops two towers while updating the
noise-scale degree by `levels/compositeDegree`, which is still one. A degree
`3 -> 2` observation can therefore look correct even though the basis, level,
and recorded factor are wrong. The pre-arithmetic guard and complete exact
basis/level/factor checks are independently required.

The deterministic negative fixture should construct FIXEDMANUAL parameters
with `SetCompositeDegree(2)` and enough full-Q towers to reach a valid public
`ReadyForRS2` input without invoking Rescale. It must snapshot the complete
pair, require exact `std::invalid_argument` text, and prove unchanged input.
Source order must prove that neither public RCB nor either Rescale call can run
before the rejection. The `DoubleCKKS` constructor and non-RS2 operations must
remain accepted for this fixture; moving the restriction into construction
would silently broaden the gate.

## Minimal candidate public seam

The later task should consider exactly:

```cpp
enum class PairLifecycle : std::uint8_t {
    ReadyForFirstMult,
    ReadyForRS2,
    AfterFirstRS2,
};

CiphertextPair RS2(const CiphertextPair& relinearized) const;
```

`q_l` is derived from the completely validated input's last active modulus;
callers do not supply it. No second pair type, public setter/friend, arbitrary
level argument, refresh implementation, or future multiplication seam is
needed.

`AfterFirstRS2` is a neutral project state, not paper terminology. It records
only what this bounded implementation has completed. The paper discusses
sequential multiplications, and the ordinary FIXEDMANUAL difference between an
exact prime and `baseSF` does not prove that refresh is mathematically required.
The current project has no second-multiplication contract, so RCB accepts this
state while Tensor2 and RS2 reject it before arithmetic. Any future transition
from it remains pending rather than being mislabeled as a refresh requirement.

This lifecycle name and its complete validation predicates remain provisional
until exact Relin2 green state is available.

## Candidate state table

| Stage | Basis | Level | Components/member | Degree | Recorded factor | Paper scales |
|---|---|---:|---:|---:|---:|---|
| `ReadyForRS2` input | `Q_l` | 1 | 2 | 3 | `SF_T` | `H_T`, `R_T` |
| rescaled high | `Q_(l-1)` | 2 | 2 | 2 | `SF_T/baseSF` | `H_T/q_l` |
| rescaled recombination | `Q_(l-1)` | 2 | 2 | 2 | `SF_T/baseSF` | `R_T/q_l` |
| `AfterFirstRS2` output | `Q_(l-1)` | 2 | 2 | 2 | `SF_T/baseSF` | `H_T/q_l`, `R_T/q_l` |

The minimal output descriptor candidate is:

```text
inputRecordedScalingFactor              = SF_T
divisor                                  = q_div
approximateLogicalScalingFactor          = H_T/exact_q_l
approximateRecombinedLogicalScalingFactor = R_T/exact_q_l
```

The existing first field retains its accepted meaning: the recorded factor at
the input to this pair transition. The pair's independent current recorded
factor is `SF_T/baseSF`. For the first supported parameter lifecycle, that
current value can numerically equal the fresh degree-two factor `baseSF^2`, so
factor and degree alone cannot distinguish states. `ValidatePair` must dispatch
on the complete lifecycle value and independently validate, for every state,
lifecycle, level, exact basis, degree, current recorded factor,
`inputRecordedScalingFactor`, and both logical scales. Its control flow must
cover every named state and reject every invalid enum value with a stable
project diagnostic; a particular `switch`/`if` spelling is not part of the
contract.

## Metadata source and alias boundary

With the required operand order, metadata provenance is fixed:

```text
RS2.high metadata  <- input.high
RCB result metadata <- input.high
RS2.low metadata   <- rescaled RCB result <- input.high
```

Input-low metadata does not propagate to either output. Each public clone has a
separate outer map container, but pristine OpenFHE shallow-copies the metadata
value pointers. To stay KISS-consistent with pristine OpenFHE and the existing
accepted project operations, this bounded RS2 slice provisionally accepts that
upstream alias behavior; the deferred exact Relin2 gate must still confirm its
actual output. RS2 promises only that the full input, including deep metadata
values, is unchanged during the call and immediately after return. It does not
promise that a later mutation through OpenFHE's mutable metadata pointer cannot
affect another ciphertext. Tests use different high/low sentinels to prove
value provenance and call-time non-mutation without claiming value-pointer
isolation. A project-wide deep-isolation change is outside this slice.

## Required fail-fast sequence candidate

1. Run complete pair validation before any non-validation getter use, raw
   arithmetic, cloning, recombination, or Rescale. Validation itself may read
   members and tower parameters after its existing null/shape checks.
2. Require exact `ReadyForRS2`, at least three active towers, and composite
   degree one. Capture and validate exact last active `q_l`.
3. Form `q_div*high+low` only from validated state, preferably through the
   accepted public RCB seam.
4. Bind named const ciphertext lvalues and call public non-in-place Rescale
   exactly once for high and exactly once for the complete recombination.
5. Completely validate both results: context, actual tag, slots, CKKS encoding,
   Evaluation format, exact ordered `Q_(l-1)` tower parameters, two components,
   level 2, degree 2, and exact recorded factor `SF_T/baseSF`.
6. Clone the already validated rescaled high and multiply every DCRT component
   directly with `DCRTPoly::operator*=(NativeInteger q_div)`. This is exact RNS
   integer arithmetic; CKKS scalar `EvalMult` is forbidden because it encodes an
   approximate scalar and can change scale/level. Before subtraction, recheck
   every compatibility field so public EvalSub cannot hide an error through
   auto-alignment.
7. Compute only `rescaledRecombined - q_div*rescaledHigh`; preserve the first
   rescaled high unchanged.
8. Construct the complete `AfterFirstRS2` pair, validate it before return, and
   prove the input plus every deep metadata-map value unchanged by the call.
   Assert both output maps have the specified high-member value provenance;
   do not claim deep pointer isolation.

Public Rescale is the trusted pristine primitive. Do not add project-private
rescale/CRT arithmetic to production merely to duplicate it. The independent
test oracle must not call public Rescale to produce expected coefficients.

## Independent oracle and witnesses

For every member component and coefficient, a test-owned `cpp_int` oracle
should:

1. clone every input and actual-output polynomial used by the oracle, convert
   each clone to Coefficient format, and prove the original Evaluation-format
   objects remain unchanged;
2. reconstruct and center the coefficient-format input under `Q_l`;
3. compute `RS(x)=(x-Center(x mod q_l,q_l))/q_l`;
4. set `A=RS(high)`;
5. center `Z=q_div*high+low` under `Q_l` and set `C=RS(Z)`;
6. set `B=C-q_div*A mod Q_(l-1)`;
7. compare `(A,B)` against every coefficient-format actual-output
   component/tower/coefficient;
8. bind the return from public `module.RCB(result)`, compare both components,
   every remaining tower, and every coefficient to independent `C`, and check
   its complete state and specified metadata source;
9. deep-snapshot the RS2 pair before public RCB, prove it unchanged immediately
   afterward, and separately prove `q_div*A+B=C`.

For the odd active prime `q_l`, define the centered residue mechanically:

```text
r = x mod q_l, normalized into [0, q_l-1]
Center_q_l(x) = r                 if 2*r <= q_l
                r - q_l           otherwise.
```

Because every supported RNS prime is odd, equality at an ambiguous exact half
cannot occur. Permanent boundary witnesses must include residues
`floor(q_l/2)` and `floor(q_l/2)+1` (plus their signed/normalized counterparts),
so an off-by-one or always-nonnegative remainder convention fails at a named
coordinate.

The fixed fixture must name coordinates and residues for positive and negative
rounding boundaries, quotient carry, nonzero high/low contributions, and a
case where `B != RS(low)`. Random search, decrypt-only comparison, or the RCB
identity alone is insufficient.

If a witness is injected in Coefficient format, the test-owned ciphertext must
be restored to Evaluation format before calling public RS2. Injection and
format conversion must never modify the original pair.

Also prove exactly one active prime was removed; level `1->2`; degree `3->2`;
recorded factor `/baseSF`; both paper logical scales `/exact q_l`; no evaluation
key dependency; exact input immutability; and stable pre-arithmetic rejection
for malformed manifest, wrong lifecycle, and composite degree other than one.

## Preliminary TDD sequence

The eventual external task should preserve separately observable boundaries:

1. compile red for only `AfterFirstRS2` and `RS2`; use an exact
   member-function-pointer `static_assert` for
   `CiphertextPair (DoubleCKKS::*)(const CiphertextPair&) const`, and check the
   enum underlying type and appended value;
2. immediate-throw, warning-clean non-mergeable scaffold;
3. complete-validation red, then minimal validation green;
4. wrong-lifecycle red, then minimal lifecycle green;
5. one valid arithmetic red must already assert complete `(A,B)` coefficients,
   public RCB acceptance and immutability on the RS2 result, and the exact
   recombination identity; then the complete formula makes that boundary green;
6. add only remaining after-first-RS2 boundary regressions, recording inherited
   greens honestly where an existing Tensor2 guard already supplies behavior;
7. final exact Linux/Windows green and documentation only afterward.

Every negative runtime case is a separately named CTest and requires exact
`std::invalid_argument`, `DoubleCKKS: ` attribution, stable field-specific
text, and whole-input immutability. Production `try`/`catch` is forbidden.

## Explicit exclusions

Do not implement or claim:

- independent rescaling of low;
- `DCP o RS o RCB`;
- `LevelReduce`, private `ModReduceInternal`, in-place Rescale, or manual
  metadata repair after Rescale;
- floating or CKKS-scalar `q_div`, arbitrary-level RS2, Add/Sub, Mult2, refresh,
  bootstrapping, repeated multiplication, serialization, or performance work;
- a Lemma 4.6 or decoded-slot precision bound without all hypotheses and
  measured quantities.

## Deferred exact-green gate

Before writing the RS2 engineering task, bind and independently audit:

- accepted Relin2 exact SHA/tree, full old/new CTest count, hosted Linux/Windows
  run, diagnostics, and review closure;
- actual `ReadyForRS2` manifest, factor, dual logical scales, and metadata-map
  inheritance behavior;
- deterministic public-seam rounding/boundary/carry fixture with explicit
  `q_l`, `q_div`, and `baseSF` inequality witnesses;
- public Rescale's exact observed basis/level/degree/factor behavior on that
  fixture;
- the final `AfterFirstRS2` name and every validation predicate.

Until that gate passes, no RS2 source patch, test patch, external implementation
request, build, or test result is claimed.
