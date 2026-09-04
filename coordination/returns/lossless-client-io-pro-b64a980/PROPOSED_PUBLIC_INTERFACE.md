# Proposed public interface

**Documentation shape only. This is intentionally not a compilable header and does not authorize implementation.**

## Declaration-shaped public seam

```cpp
namespace openfhe_2023_1788::client_io {

using ClientReal = boost::multiprecision::number<
    boost::multiprecision::cpp_dec_float<100>>;
using ExactInteger = boost::multiprecision::cpp_int;

struct ClientComplex final {
    ClientReal real;
    ClientReal imag;
};

// Both arguments must already be positive. Construction gcd-reduces them.
class PositiveRationalScale final {
public:
    static PositiveRationalScale FromPositive(
        ExactInteger numerator, ExactInteger denominator);

    const ExactInteger& Numerator() const noexcept;
    const ExactInteger& Denominator() const noexcept;
};

enum class CanonicalProjection {
    OpenFhePackedStride,
};

enum class ClientCiphertextOrigin {
    FreshClientEncoding,
    FirstMult2Rcb,
};

struct OrderedDcrtBasis final {
    std::uint32_t cyclotomicOrder;
    std::uint32_t ringDimension;
    std::vector<std::string> moduliDecimal;       // exact active order
    std::vector<std::string> rootsOfUnityDecimal; // exact matching order
};

struct ClientContextProfile final {
    const void* contextIdentity;       // process-local; never serialized
    const void* cryptoParamsIdentity;  // process-local; never serialized
    std::uint32_t requiredFeatureMask;
    std::uint32_t enabledFeatureMaskObserved;
    lbcrypto::ScalingTechnique scalingTechnique;
    lbcrypto::KeySwitchTechnique keySwitchTechnique;
    lbcrypto::ExecutionMode executionMode;
    lbcrypto::DecryptionNoiseMode decryptionNoiseMode;
    lbcrypto::CKKSDataType ckksDataType;
};

struct FreshEncodingSpec final {
    std::uint32_t slots;
    PositiveRationalScale logicalScale;
};

struct FirstMult2ScaleFactors final {
    ExactInteger qDiv;
    ExactInteger qL;
};

struct ClientCiphertextState final {
    ClientContextProfile contextProfile;
    std::string keyTag;
    OrderedDcrtBasis activeBasis;
    lbcrypto::PlaintextEncodings encodingType;
    lbcrypto::Format componentFormat;
    std::uint32_t slots;
    std::uint32_t strideGap;
    std::uint32_t level;
    std::size_t componentCount;
    bool metadataMapEmpty;             // required true in v1
    std::size_t noiseScaleDegree;
    double recordedScalingFactor;       // compatibility observation only
    lbcrypto::NativeInteger scalingFactorInt; // compatibility observation only
    PositiveRationalScale logicalScale; // authoritative normalization
    CanonicalProjection projection;
    ClientCiphertextOrigin origin;
    std::optional<FirstMult2ScaleFactors> firstMult2ScaleFactors;
};

class BoundCiphertext final {
public:
    // Returns an OpenFHE Clone() with a separate element vector/scalar state.
    // V1 requires an empty metadata map, avoiding shared Metadata-value aliases
    // between the returned clone and the private receipt snapshot.
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> CloneForEvaluation() const;

    const ClientCiphertextState& State() const noexcept;

    // No public constructor, raw snapshot getter, or metadata mutator.
};

struct DecodeDiagnostics final {
    ExactInteger activeCompositeModulus;
    ExactInteger maximumCenteredAbsoluteCoefficient;
    ExactInteger centeredHeadroom;
    ClientReal maximumCrossPrecisionDisagreement;
};

struct DecodedSlots final {
    std::vector<ClientComplex> values; // always exactly State().slots entries
    ClientCiphertextState state;
    DecodeDiagnostics diagnostics;
};

class HighPrecisionClientIO final {
public:
    explicit HighPrecisionClientIO(
        lbcrypto::CryptoContext<lbcrypto::DCRTPoly> context);

    BoundCiphertext Encrypt(
        const lbcrypto::PublicKey<lbcrypto::DCRTPoly>& publicKey,
        const std::vector<ClientComplex>& values,
        const FreshEncodingSpec& spec) const;

    // Narrow bridge only for the first retained DCP->Mult2->RCB transition.
    // All logical and physical output expectations are derived from the
    // immutable fresh parents plus the bound pinned context.
    BoundCiphertext BindFirstMult2Rcb(
        const lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly>& recombined,
        const BoundCiphertext& leftFresh,
        const BoundCiphertext& rightFresh) const;

    DecodedSlots Decrypt(
        const lbcrypto::PrivateKey<lbcrypto::DCRTPoly>& privateKey,
        const BoundCiphertext& input) const;
};

} // namespace openfhe_2023_1788::client_io
```

## Value and numerical contract

`ClientReal` is the sole public numeric scalar. Callers construct it from decimal text, exact integers, exact dyadic/rational operations, or another multiprecision value. The seam has no `double`, `long double`, `std::complex<double>`, or implicit binary64 overload. Every input and every internal root/transform result must be finite.

For each scaled encoding coordinate, the implementation computes independent 160-decimal-digit and 220-decimal-digit values, `x160` and `x220`, with π and all roots generated at the corresponding working precision—never from binary64 FFT/root tables. Define:

```text
d = abs(x220 - promote_to_220(x160))
h = distance(x220, { k + 1/2 : k is an integer })
```

The primary scaled coordinate must first satisfy:

```text
u160 = max(1, abs(x160)) * numeric_limits<WorkReal160>::epsilon()
u160 < 2^-410
```

This is the explicit supported-range guard: a large integer part may not consume the primary type's ability to resolve the tie margin. Both precisions must then give the same paper tie-down integer and:

```text
h > max(16*d, 2^-400)
```

Otherwise `Encrypt` raises a precision-ambiguity `range_error`. This is a conservative deterministic stability rule, not a proof of transcendental rounding. For decoding, the independently generated 160- and 220-digit slot results must agree in every real and imaginary component within `2^-120`; only then is the 220-digit result rounded once to `ClientReal`.

`PositiveRationalScale::FromPositive` requires `numerator > 0` and `denominator > 0`, rejects zero or either negative argument, and gcd-reduces. It does not accept a negative denominator and silently flip signs.

## `HighPrecisionClientIO(context)`

The constructor requires a non-null CKKS-RNS context with exact process-local context and crypto-parameter identity. It reads and records configuration but never calls `Enable`, changes a mode, creates keys, or mutates shared context state. It binds `S` from the crypto-parameters' configured nonzero batch size and builds instance-owned immutable multiprecision transform tables for the validated `(N,S,OpenFhePackedStride)` profile; it neither reads OpenFHE's binary64 root cache as numerical input nor installs a global mutable cache.

For the initial tracer it requires:

```text
requiredFeatureMask = PKE | KEYSWITCH | LEVELEDSHE
(GetEnabled() & requiredFeatureMask) == requiredFeatureMask
FIXEDMANUAL / HYBRID / COMPLEX / EXEC_EVALUATION
installed lbcrypto::BigInteger resolves to the inspected bigintdyn implementation
```

`enabledFeatureMaskObserved` records the complete mask at construction. Extra features already present may be recorded but do not become client functionality. Every later call requires the current complete mask to equal that immutable observation, so external feature-mask mutation is rejected; the module does not enable missing features. The accepted first tracer additionally requires `FIXED_NOISE_DECRYPT`. A context configured for polynomial flooding is never downgraded: it is explicitly unsupported by the first tracer, while the selected scheme-level Poly decryption route remains the route a separately accepted flooding-mode contract must use.

## `Encrypt(publicKey, values, spec)`

Requires:

- a non-null public key from the exact bound context and a nonempty key tag;
- the required feature subset still enabled;
- `values.size() == spec.slots`, with every component finite;
- `spec.slots == values.size() ==` the nonzero batch size bound from the context crypto-parameters, with power-of-two `N` and `S`, `1 <= S <= N/2`, and `N % (2*S) == 0`;
- v1 full ordered Q basis at level zero;
- v1 exact logical scale `2^100/1`.

The caller does **not** supply level, degree, recorded factor, format, encoding, or basis. The module derives those from the bound initial contract. In particular, the exact logical scale is the **total coefficient multiplier applied once**. OpenFHE standard Encode receives a base scale and stages the degree through additional CRT multiplication before recording `pow(base, degree)`; the direct client path must not feed an already-total `2^100` scale through that staged degree-two path. It directly rounds coefficients after one multiplication by `2^100`, then sets/verifies the compatibility state expected by retained `DoubleCKKS`: level 0, degree 2, recorded factor `baseScalingFactor * baseScalingFactor` (exactly `2^100` in the frozen context), and integer factor 1.

Effects:

1. Perform the multiprecision special inverse transform in the powers-of-five order.
2. Apply the exact rational scale and paper tie-down rounding with the two-precision guard above.
3. Reject any signed coefficient for which `2*abs(c) >= Q_full`.
4. Convert signed `cpp_int` coefficients to residues and then OpenFHE `BigInteger` using decimal strings only. The initial contract supports the supplied `-DMATHBACKEND=4` build configuration only when `lbcrypto::BigInteger` is compile-time-confirmed as the inspected dynamic implementation; another backend is unsupported rather than assumed equivalent.
5. Build a coefficient-format large `Poly`, then the official ordered `DCRTPoly`.
6. Call public `context->GetScheme()->Encrypt(element, publicKey)`.
7. Re-perform the high-level wrapper duties the lower public overload does not provide: key/context checks plus slots, level, degree, recorded factor, integer factor, and CKKS encoding metadata propagation.
8. Verify two evaluation-format components on the exact full ordered modulus/root basis and require a present, empty ciphertext metadata map.
9. Store an internal validated snapshot and return a `FreshClientEncoding` receipt.

It returns no `Plaintext`, packed-value cache, raw `Poly`, or mutable ciphertext handle.

## `BoundCiphertext::CloneForEvaluation()`

The receipt privately owns the validated ciphertext snapshot. `CloneForEvaluation()` calls public OpenFHE `CiphertextImpl::Clone()` and returns the separate ciphertext/element vector for `DCP` and the other retained evaluator APIs. OpenFHE's clone copies metadata-map entries whose `shared_ptr<Metadata>` values can remain shared; v1 therefore requires the map to be present and empty on receipt creation and every export. The receipt exposes neither its internal shared pointer nor a metadata setter. Under that explicit supported state, evaluator mutation cannot reach the receipt through ciphertext elements, scalar metadata, or metadata-value aliases.

## `BindFirstMult2Rcb(recombined, leftFresh, rightFresh)`

Requires both parents to be immutable `FreshClientEncoding` receipts from this module/context, with identical key tag, slots, context profile, full ordered modulus/root basis, and exact scale. It verifies `recombined` against the current pinned first-operation state and derives every expectation; there is no caller-supplied transition object or free scale. The caller contract requires `recombined` to be the immediate direct result of the retained `RCB(Mult2(DCP(leftClone), DCP(rightClone)))` call chain in the same process. The binder verifies state, not unforgeable operand lineage; it cannot distinguish another ciphertext with the same accepted key/context/basis/metadata state, and it does not claim to do so.

It derives:

```text
q_div = final modulus of the immutable full parent basis
q_l   = penultimate modulus of that basis
```

Both must be positive, odd, and distinct. The actual output modulus/root basis must be exactly the parent prefix after removing first `q_div` and then `q_l`, not merely a basis of the right length. Initial output must be level 2, two evaluation-format components, 16 slots, CKKS packed encoding, `COMPLEX`, degree 2, matching key/context, integer factor 1, and a present, empty metadata map.

The exact logical scale is calculated and gcd-reduced:

```text
left.logicalScale * right.logicalScale / (q_div*q_l)
= 2^200 / 1267650600226646386227681786497
```

The OpenFHE recorded `double` factor is independently recomputed in the same operation order used by the pinned evaluator and compared with exact `double ==` semantics:

```text
base          = parameters->GetScalingFactorReal(0)
freshRecorded = base * base
readyForRS2   = freshRecorded * freshRecorded / base
qLIndex       = fullParentBasisSize - 2
modReduce     = parameters->GetModReduceFactor(qLIndex)
expectedOut   = readyForRS2 / modReduce
```

Every intermediate must be finite and positive; the actual factor must equal `expectedOut`. This is a physical compatibility check, not the logical normalization. After validation, the method clones `recombined` into a new private snapshot under the empty-metadata-map rule and returns a `FirstMult2Rcb` receipt carrying derived `q_div` and `q_l`. It performs no evaluation and receives no secret key.

## `Decrypt(privateKey, input)`

Requires a non-null private key from the exact bound context whose nonempty key tag matches the private receipt snapshot. It revalidates the current required feature subset and all receipt invariants against the private ciphertext snapshot.

It then:

1. Calls public `context->GetScheme()->Decrypt(ciphertext, privateKey, &poly)`.
2. Checks `DecryptResult.isValid` before any client transform.
3. Requires coefficient format and verifies the returned `Poly` cyclotomic order, ring dimension, and exact composite modulus equal the receipt's active ordered-basis product.
4. Converts every residue through `BigInteger::ToString()` to `cpp_int`, checks `0 <= r < Q`, and centers with `r <= floor(Q/2) ? r : r-Q`.
5. Reads only coefficients `gap*j` and `gap*j+N/2`, normalizes by `scale.denominator/scale.numerator`, and applies the two-precision special forward transform.
6. Returns exactly `slots` owned values plus actual centered-representative diagnostics.

It never calls `CryptoContext::Decrypt(..., Plaintext*)`, ordinary `Decode`, or `DecryptCore`. Therefore it omits ordinary Decode's binary64 conversion, cache clearing, transform/error processing, and REAL-specific protection/postprocessing, and it makes no equivalent-output-protection claim. The public scheme Poly route is selected because it retains configured polynomial flooding; v1 does not disable protection to pass a precision gate. `REAL`, `EXEC_NOISE_ESTIMATION`, and all other unsupported profiles are rejected explicitly.

## Receipt invariants checked on every use

- process-local context and crypto-parameter identity;
- required feature mask, the complete enabled mask observed at construction, presence of the required subset, and exact absence of later mask drift (observed, never enabled by the client);
- scaling, key-switch, execution, decryption-noise, and CKKS data modes;
- nonempty matching key tag;
- cyclotomic order, ring dimension, and exact ordered modulus/root basis;
- CKKS packed encoding, uniform component format, and a present empty metadata map;
- context-configured batch size, slots, stride, level, component count, degree, recorded factor, and integer factor;
- exact positive reduced rational scale and state origin;
- first-operation factors and exact drop-two transition when present.

A level or tower count alone is never accepted as basis identity.

## Error surface

No internal catch/translation is proposed.

```text
invalid_argument : null/malformed caller data, nonpositive rational, invalid geometry
 domain_error    : unsupported mode/feature or context/key/state mismatch; invalid decrypt
 range_error     : nonfinite value, coefficient headroom, exact conversion, precision ambiguity
 OpenFHE errors  : propagate from official primitives
```

## Initial supported configuration

| Field | Supported value |
|---|---|
| OpenFHE | pristine 1.5.0 at `df495ba2e91739a6dc8f1de254fc5a41155ce504` |
| Integer backend | supplied `-DMATHBACKEND=4` configuration, with `lbcrypto::BigInteger` required to resolve to inspected `bigintdyn`; other backends rejected pending source/compile evidence |
| Required enabled features | `PKE | KEYSWITCH | LEVELEDSHE`; verified, never enabled by client |
| Scheme/scaling | CKKS-RNS / `FIXEDMANUAL` |
| Key switching | `HYBRID` |
| Data/execution | `COMPLEX` / `EXEC_EVALUATION` |
| Tracer decryption mode | `FIXED_NOISE_DECRYPT`; verified and never changed |
| Security setting | `HEStd_NotSet` — diagnostic only, not paper/security evidence |
| `N`, `M`, `S`, `gap` | 64, 128, 16, 2 |
| Fresh state | full basis, level 0, 2 components, degree 2, exact logical scale `2^100` |
| Fresh physical metadata | recorded factor `2^100`, integer factor 1, CKKS packed, evaluation format |
| First RCB state | exact drop-two basis, level 2, 2 components, degree 2 |
| Exact divisors | `q_div=1125899906843009`, `q_l=1125899906840833` |
| Output logical scale | `2^200/1267650600226646386227681786497` |
| Projection | exactly 16 `OpenFhePackedStride` slots |

Other ring dimensions and full packing are within the algorithmic design but are **unsupported until separate acceptance evidence exists**. Paper `N=32768,S=16384,h=128,dnum=11`, its modulus schedule, eight squarings, and 1,000-run evidence are not implied.

## Exact user-confirmation question

> **Do you explicitly confirm `PROPOSED_PUBLIC_INTERFACE.md` v1 as the public seam for the next smallest TDD/code slice: fixed `cpp_dec_float<100>` complex values; no public `Plaintext`; `Encrypt`, immutable receipt `CloneForEvaluation`, operation-specific `BindFirstMult2Rcb`, and `Decrypt`; exact reduced rational scale; public scheme-level Encrypt and `Decrypt(..., Poly*)`; the initial exact integer bridge restricted to the supplied build only after compile-time confirmation that `lbcrypto::BigInteger` is the inspected `bigintdyn` implementation; required features verified but never enabled; configured decryption protection never downgraded; `COMPLEX`/`EXEC_EVALUATION`; and `OpenFhePackedStride` output?**
>
> A precise affirmative record is: **`CONFIRM_LOSSLESS_CLIENT_IO_SEAM_V1`**.

Until that confirmation is recorded by the coordinating workflow, no test or implementation is authorized.
