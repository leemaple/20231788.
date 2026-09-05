#include "openfhe_2023_1788/high_precision_client_io.h"
#include "scheme/ckksrns/ckksrns-cryptoparameters.h"

#include <boost/math/constants/constants.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace openfhe_2023_1788::client_io {
namespace detail {

// Opaque, immutable value snapshot shared by this client's receipts. Context
// and Params remain upstream-owned shared objects, not transitive deep copies.
struct ClientContextBinding final {
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> context;
    std::shared_ptr<lbcrypto::CryptoParametersCKKSRNS> parameters;
    ClientContextProfile profile;
    OrderedDcrtBasis fullBasis;
    OrderedDcrtBasis pBasis;
    OrderedDcrtBasis qpBasis;
    OrderedDcrtBasis pkBasis;
    std::vector<OrderedDcrtBasis> partitionBases;
    std::uint32_t numPartQ;
    std::uint32_t numPerPartQ;
    std::vector<lbcrypto::NativeInteger> pModq;
};

}  // namespace detail
namespace {

using lbcrypto::Ciphertext;
using lbcrypto::CryptoContext;
using lbcrypto::DCRTPoly;
using Parameters = lbcrypto::CryptoParametersCKKSRNS;
using ContextBinding = detail::ClientContextBinding;
template <unsigned Digits>
using WorkReal = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<Digits>>;
using Primary = WorkReal<160>;
using CheckReal = WorkReal<220>;

constexpr std::size_t kSlots = 16;
constexpr std::uint32_t kN = 64;
constexpr std::uint32_t kM = 128;
constexpr std::uint32_t kGap = 2;
constexpr std::uint32_t kRequiredFeatures = lbcrypto::PKE | lbcrypto::KEYSWITCH | lbcrypto::LEVELEDSHE;
static_assert(NATIVEINT == 64 && MATHBACKEND == 4, "client I/O requires native64/backend4");
static_assert(std::is_same_v<lbcrypto::BigInteger, bigintdyn::BigInteger>,
              "the inspected exact decimal bridge requires official bigintdyn");

void Require(bool condition, const char* message) {
    if (!condition) throw std::domain_error(std::string("HighPrecisionClientIO: ") + message);
}

template <class Real>
void RequireFinite(const Real& value) {
    if (!boost::math::isfinite(value))
        throw std::range_error("HighPrecisionClientIO: nonfinite numerical value");
}

template <class Real>
Real Absolute(const Real& value) {
    return value < 0 ? Real(-value) : value;
}

template <class Real>
Real NegativePowerOfTwo(unsigned bits) {
    Real value = 1;
    for (unsigned bit = 0; bit < bits; ++bit) value /= 2;
    return value;
}

ExactInteger Gcd(ExactInteger left, ExactInteger right) {
    while (right != 0) {
        ExactInteger remainder = left % right;
        left = std::move(right);
        right = std::move(remainder);
    }
    return left;
}

ExactInteger CompositeModulus(const OrderedDcrtBasis& basis) {
    ExactInteger result = 1;
    for (const auto& modulus : basis.moduliDecimal) result *= ExactInteger(modulus);
    return result;
}

bool SameBasis(const OrderedDcrtBasis& left, const OrderedDcrtBasis& right) {
    return left.cyclotomicOrder == right.cyclotomicOrder && left.ringDimension == right.ringDimension &&
           left.moduliDecimal == right.moduliDecimal && left.rootsOfUnityDecimal == right.rootsOfUnityDecimal;
}

OrderedDcrtBasis ReadBasis(const std::shared_ptr<DCRTPoly::Params>& params) {
    Require(params != nullptr, "null context basis");
    Require(params->GetCyclotomicOrder() == kM && params->GetRingDimension() == kN &&
            !params->GetParams().empty(), "unsupported context basis geometry");
    OrderedDcrtBasis result{kM, kN, {}, {}};
    for (const auto& tower : params->GetParams()) {
        Require(tower != nullptr, "null context tower parameters");
        Require(tower->GetCyclotomicOrder() == kM && tower->GetRingDimension() == kN &&
                tower->GetModulus() > lbcrypto::NativeInteger(2) &&
                tower->GetRootOfUnity() > lbcrypto::NativeInteger(0) &&
                tower->GetRootOfUnity() < tower->GetModulus(), "invalid context tower parameters");
        result.moduliDecimal.push_back(tower->GetModulus().ToString());
        result.rootsOfUnityDecimal.push_back(tower->GetRootOfUnity().ToString());
    }
    Require(params->GetModulus().ToString() == CompositeModulus(result).convert_to<std::string>(),
            "inconsistent context composite modulus");
    return result;
}

// Unlike ReadBasis, a live comparison must turn every structural mismatch
// into the one boundary diagnostic before using an upstream primitive.
bool MatchesBasis(const std::shared_ptr<DCRTPoly::Params>& params, const OrderedDcrtBasis& expected) {
    if (!params || params->GetCyclotomicOrder() != expected.cyclotomicOrder ||
        params->GetRingDimension() != expected.ringDimension ||
        params->GetParams().size() != expected.moduliDecimal.size() ||
        params->GetModulus().ToString() != CompositeModulus(expected).convert_to<std::string>()) return false;
    for (std::size_t index = 0; index < expected.moduliDecimal.size(); ++index) {
        const auto& tower = params->GetParams()[index];
        if (!tower || tower->GetCyclotomicOrder() != expected.cyclotomicOrder ||
            tower->GetRingDimension() != expected.ringDimension ||
            tower->GetModulus().ToString() != expected.moduliDecimal[index] ||
            tower->GetRootOfUnity().ToString() != expected.rootsOfUnityDecimal[index]) return false;
    }
    return true;
}

// This guard cannot use DCRT IsEmpty() (which only detects all-empty towers),
// DCRT operator== (which dereferences Params), or a NativePoly parameter/value
// getter before proving that its pointer/value storage exists.
// Upstream unchecked uses: rns-pke.cpp:172-192 and 199-223, at the pinned commit.
bool MatchesElement(const DCRTPoly& element, const OrderedDcrtBasis& expected) {
    const auto& params = element.GetParams();
    if (!params || element.GetFormat() != Format::EVALUATION ||
        params->GetParams().size() != expected.moduliDecimal.size() ||
        element.GetAllElements().size() != expected.moduliDecimal.size() ||
        params->GetCyclotomicOrder() != expected.cyclotomicOrder ||
        params->GetRingDimension() != expected.ringDimension ||
        params->GetModulus().ToString() != CompositeModulus(expected).convert_to<std::string>()) return false;
    for (std::size_t index = 0; index < expected.moduliDecimal.size(); ++index) {
        const auto& declared = params->GetParams()[index];
        const auto& tower = element.GetAllElements()[index];
        const auto& actual = tower.GetParams();
        if (!declared || !actual || tower.IsEmpty() || tower.GetFormat() != Format::EVALUATION) return false;
        for (const auto& part : {declared, actual}) {
            if (part->GetCyclotomicOrder() != expected.cyclotomicOrder ||
                part->GetRingDimension() != expected.ringDimension ||
                part->GetModulus().ToString() != expected.moduliDecimal[index] ||
                part->GetRootOfUnity().ToString() != expected.rootsOfUnityDecimal[index]) return false;
        }
        if (tower.GetValues().GetLength() != expected.ringDimension ||
            tower.GetValues().GetModulus() != actual->GetModulus()) return false;
    }
    return true;
}

ContextBinding BindContext(CryptoContext<DCRTPoly> context) {
    Require(context != nullptr, "null context");
    const auto cp = std::dynamic_pointer_cast<Parameters>(context->GetCryptoParameters());
    Require(cp != nullptr && context->GetScheme() != nullptr && context->getSchemeId() == lbcrypto::CKKSRNS_SCHEME,
            "context is not CKKS-RNS");
    const auto enabled = context->GetScheme()->GetEnabled();
    Require((enabled & kRequiredFeatures) == kRequiredFeatures, "required features are not enabled");
    Require(cp->GetScalingTechnique() == lbcrypto::FIXEDMANUAL && cp->GetKeySwitchTechnique() == lbcrypto::HYBRID &&
            cp->GetEncryptionTechnique() == lbcrypto::STANDARD && cp->GetMultiplicationTechnique() == lbcrypto::HPS &&
            cp->GetPREMode() == lbcrypto::NOT_SET && cp->GetCKKSDataType() == lbcrypto::COMPLEX &&
            cp->GetExecutionMode() == lbcrypto::EXEC_EVALUATION &&
            cp->GetDecryptionNoiseMode() == lbcrypto::FIXED_NOISE_DECRYPT && cp->GetNoiseScale() == 1 &&
            cp->GetSecretKeyDist() == lbcrypto::UNIFORM_TERNARY && cp->GetStdLevel() == lbcrypto::HEStd_NotSet &&
            cp->GetDigitSize() == 0 && cp->GetMaxRelinSkDeg() == 2 && cp->GetMultiplicativeDepth() == 7 &&
            cp->GetMultipartyMode() == lbcrypto::FIXED_NOISE_MULTIPARTY && cp->GetThresholdNumOfParties() == 1,
            "unsupported diagnostic profile");
    Require(cp->GetEncodingParams() != nullptr && cp->GetBatchSize() == kSlots && cp->GetPlaintextModulus() == 50,
            "unsupported encoding parameters");
    const auto q = ReadBasis(cp->GetElementParams());
    Require(q.moduliDecimal.size() == 8 &&
            cp->GetElementParams()->GetParams().front()->GetModulus().GetMSB() == 55 &&
            q.moduliDecimal.back() == "1125899906843009" &&
            q.moduliDecimal[6] == "1125899906840833", "unsupported diagnostic Q basis");
    const auto pk = ReadBasis(cp->GetParamsPK());
    Require(SameBasis(pk, q), "public-key parameters are not full Q");
    const auto p = ReadBasis(cp->GetParamsP());
    auto expectedQP = q;
    expectedQP.moduliDecimal.insert(expectedQP.moduliDecimal.end(), p.moduliDecimal.begin(), p.moduliDecimal.end());
    expectedQP.rootsOfUnityDecimal.insert(expectedQP.rootsOfUnityDecimal.end(), p.rootsOfUnityDecimal.begin(), p.rootsOfUnityDecimal.end());
    const auto qp = ReadBasis(cp->GetParamsQP());
    Require(SameBasis(qp, expectedQP), "inconsistent HYBRID QP basis");
    Require(cp->GetNumPartQ() == 3 && cp->GetNumPerPartQ() == 3 && cp->GetNumberOfQPartitions() == 3,
            "unsupported HYBRID partition profile");
    OrderedDcrtBasis partitions{kM, kN, {}, {}};
    std::vector<OrderedDcrtBasis> partitionBases;
    for (std::uint32_t part = 0; part < 3; ++part) {
        const auto basis = ReadBasis(cp->GetParamsPartQ(part));
        Require(basis.moduliDecimal.size() == (part == 2 ? 2U : 3U), "inconsistent HYBRID partition length");
        partitions.moduliDecimal.insert(partitions.moduliDecimal.end(), basis.moduliDecimal.begin(), basis.moduliDecimal.end());
        partitions.rootsOfUnityDecimal.insert(partitions.rootsOfUnityDecimal.end(), basis.rootsOfUnityDecimal.begin(), basis.rootsOfUnityDecimal.end());
        partitionBases.push_back(basis);
    }
    Require(SameBasis(partitions, q) && cp->GetPModq().size() == q.moduliDecimal.size(), "uninitialized HYBRID basis tables");
    const ExactInteger pModulus = CompositeModulus(p);
    for (std::size_t index = 0; index < q.moduliDecimal.size(); ++index) {
        Require(ExactInteger(cp->GetPModq()[index].ToString()) == pModulus % ExactInteger(q.moduliDecimal[index]),
                "inconsistent HYBRID PModq table");
    }
    Require(cp->GetScalingFactorReal(0) == std::ldexp(1.0, 50), "unsupported base scaling factor");
    ClientContextProfile profile{context.get(), cp.get(), kRequiredFeatures, enabled,
        cp->GetScalingTechnique(), cp->GetKeySwitchTechnique(), cp->GetExecutionMode(),
        cp->GetDecryptionNoiseMode(), cp->GetCKKSDataType()};
    return {std::move(context), cp, profile, q, p, qp, pk, std::move(partitionBases),
            cp->GetNumPartQ(), cp->GetNumPerPartQ(), cp->GetPModq()};
}

void CheckSharedBasis(const ContextBinding& binding) {
    const auto& cp = binding.parameters;
    constexpr auto diagnostic = "shared context basis changed";
    Require(MatchesBasis(cp->GetElementParams(), binding.fullBasis) &&
            MatchesBasis(cp->GetParamsP(), binding.pBasis) &&
            MatchesBasis(cp->GetParamsQP(), binding.qpBasis) &&
            MatchesBasis(cp->GetParamsPK(), binding.pkBasis) &&
            cp->GetNumPartQ() == binding.numPartQ && cp->GetNumPerPartQ() == binding.numPerPartQ &&
            cp->GetNumberOfQPartitions() == binding.partitionBases.size() &&
            cp->GetPModq() == binding.pModq, diagnostic);
    // GetParamsPartQ indexes without bounds checks in the pinned upstream;
    // the live partition count was established above before every access.
    for (std::uint32_t part = 0; part < binding.partitionBases.size(); ++part)
        Require(MatchesBasis(cp->GetParamsPartQ(part), binding.partitionBases[part]), diagnostic);
}

void CheckProfile(const ContextBinding& binding) {
    const auto& context = binding.context;
    const auto& cp = binding.parameters;
    const auto& profile = binding.profile;
    Require(context->GetCryptoParameters().get() == profile.cryptoParamsIdentity &&
            context->GetScheme() != nullptr && context->getSchemeId() == lbcrypto::CKKSRNS_SCHEME &&
            context->GetScheme()->GetEnabled() == profile.enabledFeatureMaskObserved &&
            cp->GetScalingTechnique() == profile.scalingTechnique && cp->GetKeySwitchTechnique() == profile.keySwitchTechnique &&
            cp->GetExecutionMode() == profile.executionMode && cp->GetDecryptionNoiseMode() == profile.decryptionNoiseMode &&
            cp->GetCKKSDataType() == profile.ckksDataType, "context profile changed");
    CheckSharedBasis(binding);
}

bool SameProfile(const ClientContextProfile& left, const ClientContextProfile& right) {
    return left.contextIdentity == right.contextIdentity && left.cryptoParamsIdentity == right.cryptoParamsIdentity &&
        left.requiredFeatureMask == right.requiredFeatureMask && left.enabledFeatureMaskObserved == right.enabledFeatureMaskObserved &&
        left.scalingTechnique == right.scalingTechnique && left.keySwitchTechnique == right.keySwitchTechnique &&
        left.executionMode == right.executionMode && left.decryptionNoiseMode == right.decryptionNoiseMode &&
        left.ckksDataType == right.ckksDataType;
}

void CheckCiphertext(const lbcrypto::ConstCiphertext<DCRTPoly>& ciphertext, const ClientCiphertextState& state) {
    Require(ciphertext != nullptr && ciphertext->GetCryptoContext().get() == state.contextProfile.contextIdentity &&
            ciphertext->GetCryptoParameters().get() == state.contextProfile.cryptoParamsIdentity &&
            !state.keyTag.empty() && ciphertext->GetKeyTag() == state.keyTag, "ciphertext context or key mismatch");
    Require(ciphertext->GetEncodingType() == state.encodingType && ciphertext->GetSlots() == state.slots &&
            ciphertext->GetLevel() == state.level && ciphertext->GetElements().size() == state.componentCount &&
            ciphertext->GetNoiseScaleDeg() == state.noiseScaleDegree &&
            std::isfinite(ciphertext->GetScalingFactor()) && ciphertext->GetScalingFactor() == state.recordedScalingFactor &&
            ciphertext->GetScalingFactorInt() == state.scalingFactorInt &&
            ciphertext->GetMetadataMap() && ciphertext->GetMetadataMap()->empty(), "ciphertext state mismatch");
    for (const auto& element : ciphertext->GetElements())
        Require(MatchesElement(element, state.activeBasis), "ciphertext element mismatch");
}

double FreshRecorded(const ContextBinding& binding) {
    const double base = binding.parameters->GetScalingFactorReal(0);
    const double fresh = base * base;
    Require(std::isfinite(base) && base > 0 && std::isfinite(fresh) && fresh > 0, "invalid fresh recorded factor");
    return fresh;
}

ClientCiphertextState FreshState(const ContextBinding& binding, const std::string& tag) {
    return {binding.profile, tag, binding.fullBasis, lbcrypto::CKKS_PACKED_ENCODING,
        Format::EVALUATION, kSlots, kGap, 0, 2, true, 2, FreshRecorded(binding), lbcrypto::NativeInteger(1),
        PositiveRationalScale::FromPositive(ExactInteger(1) << 100, 1), CanonicalProjection::OpenFhePackedStride,
        ClientCiphertextOrigin::FreshClientEncoding, std::nullopt};
}

template <class Real>
struct Complex final { Real real; Real imag; };

template <class Real>
Complex<Real> Add(const Complex<Real>& left, const Complex<Real>& right) {
    return {left.real + right.real, left.imag + right.imag};
}

template <class Real>
Complex<Real> Subtract(const Complex<Real>& left, const Complex<Real>& right) {
    return {left.real - right.real, left.imag - right.imag};
}

template <class Real>
Complex<Real> Multiply(const Complex<Real>& left, const Complex<Real>& right) {
    return {left.real * right.real - left.imag * right.imag, left.real * right.imag + left.imag * right.real};
}

// Independently generated immutable roots at EACH working precision. No
// OpenFHE binary64 root/cache data and no test oracle enters this module.
// Butterfly/order source: official dftransform.cpp:50-69,209-268 at df495ba.
template <class Real>
struct TransformTable final {
    std::array<std::uint32_t, kSlots> powersOfFive{};
    std::array<Complex<Real>, kM + 1> roots{};

    TransformTable() {
        std::uint32_t power = 1;
        for (auto& exponent : powersOfFive) { exponent = power; power = (5 * power) % kM; }
        const Real pi = boost::math::constants::pi<Real>();
        for (std::uint32_t exponent = 0; exponent < kM; ++exponent) {
            const Real angle = Real(2) * pi * Real(exponent) / Real(kM);
            roots[exponent] = {boost::multiprecision::cos(angle), boost::multiprecision::sin(angle)};
            RequireFinite(roots[exponent].real); RequireFinite(roots[exponent].imag);
        }
        roots[kM] = roots[0];
    }
};

template <class Real>
void BitReverse(std::array<Complex<Real>, kSlots>& values) {
    for (std::size_t index = 1, reversed = 0; index < kSlots; ++index) {
        std::size_t bit = kSlots / 2;
        while ((reversed & bit) != 0) { reversed ^= bit; bit >>= 1; }
        reversed ^= bit;
        if (index < reversed) std::swap(values[index], values[reversed]);
    }
}

template <class Real>
void Transform(std::array<Complex<Real>, kSlots>& values, const TransformTable<Real>& table, bool inverse) {
    if (!inverse) BitReverse(values);
    for (std::size_t length = inverse ? kSlots : 2; length >= 2 && length <= kSlots;
         length = inverse ? length / 2 : length * 2) {
        const std::size_t half = length / 2, quarter = length * 4, gap = kM / quarter;
        for (std::size_t offset = 0; offset < kSlots; offset += length) {
            for (std::size_t index = 0; index < half; ++index) {
                const auto left = values[offset + index], right = values[offset + index + half];
                const auto exponent = table.powersOfFive[index] % quarter;
                if (inverse) {
                    values[offset + index] = Add(left, right);
                    values[offset + index + half] = Multiply(Subtract(left, right), table.roots[(quarter - exponent) * gap]);
                }
                else {
                    const auto rotated = Multiply(right, table.roots[exponent * gap]);
                    values[offset + index] = Add(left, rotated);
                    values[offset + index + half] = Subtract(left, rotated);
                }
                RequireFinite(values[offset + index].real); RequireFinite(values[offset + index].imag);
                RequireFinite(values[offset + index + half].real); RequireFinite(values[offset + index + half].imag);
            }
        }
    }
    if (inverse) {
        BitReverse(values);
        for (auto& value : values) { value.real /= Real(kSlots); value.imag /= Real(kSlots); }
    }
}

template <class Real>
std::array<Complex<Real>, kSlots> Inverse(const std::vector<ClientComplex>& values,
                                         const TransformTable<Real>& table) {
    std::array<Complex<Real>, kSlots> result{};
    for (std::size_t slot = 0; slot < kSlots; ++slot) result[slot] = {Real(values[slot].real), Real(values[slot].imag)};
    Transform(result, table, true);
    return result;
}

template <class Real>
ExactInteger PaperRound(const Real& value) {
    const Real floor = boost::multiprecision::floor(value);
    // Paper notation: nearest integer, downward at an exact half-integer.
    const Real rounded = value - floor <= Real("0.5") ? floor : Real(floor + 1);
    return rounded.template convert_to<ExactInteger>();
}

ExactInteger StableRound(const Primary& primary, const CheckReal& check) {
    RequireFinite(primary); RequireFinite(check);
    const Primary unit = std::max(Primary(1), Absolute(primary)) * std::numeric_limits<Primary>::epsilon();
    if (!(unit < NegativePowerOfTwo<Primary>(410)))
        throw std::range_error("HighPrecisionClientIO: encoding supported range exceeded");
    const CheckReal disagreement = Absolute(CheckReal(check - CheckReal(primary)));
    const CheckReal fraction = check - boost::multiprecision::floor(check);
    const CheckReal halfDistance = Absolute(CheckReal(fraction - CheckReal("0.5")));
    const CheckReal margin = std::max(CheckReal(16 * disagreement), NegativePowerOfTwo<CheckReal>(400));
    const ExactInteger primaryInteger = PaperRound(primary), checkInteger = PaperRound(check);
    if (!(halfDistance > margin) || primaryInteger != checkInteger)
        throw std::range_error("HighPrecisionClientIO: ambiguous encoding precision");
    return checkInteger;
}

template <class Real>
std::array<Complex<Real>, kSlots> Forward(const std::vector<ExactInteger>& coefficients,
                                         const PositiveRationalScale& scale, const TransformTable<Real>& table) {
    const Real normalization = Real(scale.Denominator().convert_to<std::string>()) /
                               Real(scale.Numerator().convert_to<std::string>());
    std::array<Complex<Real>, kSlots> result{};
    for (std::size_t slot = 0; slot < kSlots; ++slot) {
        result[slot] = {Real(coefficients[kGap * slot].convert_to<std::string>()) * normalization,
                        Real(coefficients[kGap * slot + kN / 2].convert_to<std::string>()) * normalization};
        RequireFinite(result[slot].real); RequireFinite(result[slot].imag);
    }
    Transform(result, table, false);
    return result;
}

}  // namespace

PositiveRationalScale::PositiveRationalScale(ExactInteger numerator, ExactInteger denominator)
    : numerator_(std::move(numerator)), denominator_(std::move(denominator)) {}

PositiveRationalScale PositiveRationalScale::FromPositive(ExactInteger numerator, ExactInteger denominator) {
    if (numerator <= 0 || denominator <= 0)
        throw std::invalid_argument("PositiveRationalScale: numerator and denominator must be positive");
    const auto divisor = Gcd(numerator, denominator);
    numerator /= divisor;
    denominator /= divisor;
    return PositiveRationalScale(std::move(numerator), std::move(denominator));
}

const ExactInteger& PositiveRationalScale::Numerator() const noexcept { return numerator_; }
const ExactInteger& PositiveRationalScale::Denominator() const noexcept { return denominator_; }

BoundCiphertext::BoundCiphertext(Ciphertext<DCRTPoly> snapshot, ClientCiphertextState state,
                                 std::shared_ptr<const detail::ClientContextBinding> binding)
    : snapshot_(std::move(snapshot)), state_(std::move(state)), binding_(std::move(binding)) {}

Ciphertext<DCRTPoly> BoundCiphertext::CloneForEvaluation() const {
    CheckProfile(*binding_);
    CheckCiphertext(snapshot_, state_);
    return snapshot_->Clone();
}
const ClientCiphertextState& BoundCiphertext::State() const noexcept { return state_; }

struct HighPrecisionClientIO::Impl final {
    const std::shared_ptr<const ContextBinding> binding;
    const TransformTable<Primary> primary;
    const TransformTable<CheckReal> check;
    explicit Impl(CryptoContext<DCRTPoly> context)
        : binding(std::make_shared<const ContextBinding>(BindContext(std::move(context)))) {}
};

HighPrecisionClientIO::HighPrecisionClientIO(CryptoContext<DCRTPoly> context)
    : impl_(std::make_shared<const Impl>(std::move(context))) {}

BoundCiphertext HighPrecisionClientIO::Encrypt(const lbcrypto::PublicKey<DCRTPoly>& publicKey,
                                               const std::vector<ClientComplex>& values,
                                               const FreshEncodingSpec& spec) const {
    const auto& binding = *impl_->binding;
    CheckProfile(binding);
    if (!publicKey || publicKey->GetCryptoContext().get() != binding.context.get() || publicKey->GetKeyTag().empty() ||
        publicKey->GetPublicElements().size() != 2 ||
        !MatchesElement(publicKey->GetPublicElements()[0], binding.fullBasis) ||
        !MatchesElement(publicKey->GetPublicElements()[1], binding.fullBasis))
        throw std::invalid_argument("HighPrecisionClientIO: malformed public key");
    if (values.size() != kSlots || spec.slots != kSlots)
        throw std::invalid_argument("HighPrecisionClientIO: expected exactly 16 slots");
    Require(spec.logicalScale.Numerator() == (ExactInteger(1) << 100) && spec.logicalScale.Denominator() == 1,
            "unsupported fresh exact scale");
    for (const auto& value : values) { RequireFinite(value.real); RequireFinite(value.imag); }
    const auto primary = Inverse(values, impl_->primary);
    const auto check = Inverse(values, impl_->check);
    const Primary primaryScale(spec.logicalScale.Numerator().convert_to<std::string>());
    const CheckReal checkScale(spec.logicalScale.Numerator().convert_to<std::string>());
    std::vector<ExactInteger> coefficients(kN, 0);
    for (std::size_t slot = 0; slot < kSlots; ++slot) {
        coefficients[kGap * slot] = StableRound(Primary(primary[slot].real * primaryScale), CheckReal(check[slot].real * checkScale));
        coefficients[kGap * slot + kN / 2] = StableRound(Primary(primary[slot].imag * primaryScale), CheckReal(check[slot].imag * checkScale));
    }
    const ExactInteger modulus = CompositeModulus(binding.fullBasis);
    const lbcrypto::BigInteger officialModulus(modulus.convert_to<std::string>());
    lbcrypto::BigVector residues(kN, officialModulus);
    for (std::size_t coefficient = 0; coefficient < kN; ++coefficient) {
        if (2 * Absolute(coefficients[coefficient]) >= modulus)
            throw std::range_error("HighPrecisionClientIO: encoding coefficient would wrap");
        const ExactInteger residue = coefficients[coefficient] < 0 ? ExactInteger(coefficients[coefficient] + modulus) : coefficients[coefficient];
        residues[coefficient] = lbcrypto::BigInteger(residue.convert_to<std::string>());
        if (ExactInteger(residues[coefficient].ToString()) != residue)
            throw std::range_error("HighPrecisionClientIO: exact integer conversion failed");
    }
    // Official large-Poly/DCRT constructor, not test-fixture tower injection.
    // poly.h:81-91, dcrtpoly-impl.h:59-68; the PKE implementation switches format.
    lbcrypto::Poly polynomial(binding.parameters->GetElementParams(), Format::COEFFICIENT);
    polynomial.SetValues(std::move(residues), Format::COEFFICIENT);
    const DCRTPoly element(polynomial, binding.parameters->GetElementParams());
    auto ciphertext = binding.context->GetScheme()->Encrypt(element, publicKey);
    Require(ciphertext != nullptr, "official encryption returned null");
    auto state = FreshState(binding, publicKey->GetKeyTag());
    // Duties of the official high-level wrapper (cryptocontext.h:1250-1266),
    // without a Plaintext, binary64 slot cache, or an extra scale multiplication.
    ciphertext->SetEncodingType(state.encodingType);
    ciphertext->SetSlots(state.slots);
    ciphertext->SetLevel(state.level);
    ciphertext->SetNoiseScaleDeg(state.noiseScaleDegree);
    ciphertext->SetScalingFactor(state.recordedScalingFactor);
    ciphertext->SetScalingFactorInt(state.scalingFactorInt);
    CheckCiphertext(ciphertext, state);
    return BoundCiphertext(std::move(ciphertext), std::move(state), impl_->binding);
}

BoundCiphertext HighPrecisionClientIO::BindFirstMult2Rcb(const lbcrypto::ConstCiphertext<DCRTPoly>& recombined,
                                                        const BoundCiphertext& leftFresh,
                                                        const BoundCiphertext& rightFresh) const {
    const auto& binding = *impl_->binding;
    CheckProfile(binding);
    CheckProfile(*leftFresh.binding_);
    CheckProfile(*rightFresh.binding_);
    const auto& left = leftFresh.state_;
    const auto& right = rightFresh.state_;
    for (const auto* parent : {&left, &right}) {
        Require(SameProfile(parent->contextProfile, binding.profile) &&
                parent->origin == ClientCiphertextOrigin::FreshClientEncoding && parent->level == 0 &&
                parent->slots == kSlots && parent->strideGap == kGap && SameBasis(parent->activeBasis, binding.fullBasis) &&
                parent->logicalScale.Numerator() == (ExactInteger(1) << 100) && parent->logicalScale.Denominator() == 1 &&
                !parent->firstMult2ScaleFactors.has_value(), "first binder requires matching fresh parents");
    }
    Require(left.keyTag == right.keyTag, "fresh parent key tags differ");
    CheckCiphertext(leftFresh.snapshot_, left);
    CheckCiphertext(rightFresh.snapshot_, right);
    const std::size_t count = left.activeBasis.moduliDecimal.size();
    const ExactInteger qDiv(left.activeBasis.moduliDecimal.back()), qL(left.activeBasis.moduliDecimal[count - 2]);
    Require(qDiv > 0 && qL > 0 && qDiv != qL && qDiv % 2 == 1 && qL % 2 == 1, "invalid first-operation divisors");
    const auto scale = PositiveRationalScale::FromPositive(
        left.logicalScale.Numerator() * right.logicalScale.Numerator(),
        left.logicalScale.Denominator() * right.logicalScale.Denominator() * qDiv * qL);
    const double base = binding.parameters->GetScalingFactorReal(0);
    const double fresh = FreshRecorded(binding);
    const double ready = fresh * fresh / base;
    const double divisor = binding.parameters->GetModReduceFactor(count - 2);
    const double recorded = ready / divisor;
    Require(std::isfinite(ready) && ready > 0 && std::isfinite(divisor) && divisor > 0 &&
            std::isfinite(recorded) && recorded > 0, "invalid first-operation recorded factor");
    auto state = FreshState(binding, left.keyTag);
    state.activeBasis.moduliDecimal.resize(count - 2);
    state.activeBasis.rootsOfUnityDecimal.resize(count - 2);
    state.level = 2;
    state.recordedScalingFactor = recorded;
    state.logicalScale = scale;
    state.origin = ClientCiphertextOrigin::FirstMult2Rcb;
    state.firstMult2ScaleFactors = FirstMult2ScaleFactors{qDiv, qL};
    CheckCiphertext(recombined, state);
    return BoundCiphertext(recombined->Clone(), std::move(state), impl_->binding);
}

DecodedSlots HighPrecisionClientIO::Decrypt(const lbcrypto::PrivateKey<DCRTPoly>& privateKey,
                                            const BoundCiphertext& input) const {
    const auto& binding = *impl_->binding;
    CheckProfile(binding);
    CheckProfile(*input.binding_);
    if (!privateKey || privateKey->GetCryptoContext().get() != binding.context.get() ||
        privateKey->GetKeyTag().empty() || privateKey->GetKeyTag() != input.state_.keyTag ||
        !MatchesElement(privateKey->GetPrivateElement(), binding.fullBasis))
        throw std::invalid_argument("HighPrecisionClientIO: malformed private key");
    Require(SameProfile(input.state_.contextProfile, binding.profile), "receipt belongs to another context");
    CheckCiphertext(input.snapshot_, input.state_);
    lbcrypto::ConstCiphertext<DCRTPoly> ciphertext = input.snapshot_;
    lbcrypto::Poly polynomial;
    // Public scheme Poly* route retains the configured CKKS polynomial-noise
    // policy. Never use DecryptCore or high-level Plaintext Decode here.
    // Official base-scheme.h:215-224, ckksrns-pke.cpp:71-95 at df495ba.
    const auto decrypted = binding.context->GetScheme()->Decrypt(ciphertext, privateKey, &polynomial);
    Require(decrypted.isValid, "official decryption returned invalid");
    const ExactInteger modulus = CompositeModulus(input.state_.activeBasis);
    Require(polynomial.GetParams() != nullptr && !polynomial.IsEmpty() && polynomial.GetFormat() == Format::COEFFICIENT &&
            polynomial.GetParams()->GetCyclotomicOrder() == kM && polynomial.GetParams()->GetRingDimension() == kN &&
            polynomial.GetParams()->GetModulus().ToString() == modulus.convert_to<std::string>() &&
            polynomial.GetValues().GetLength() == kN, "invalid decrypted polynomial");
    std::vector<ExactInteger> coefficients(kN);
    ExactInteger maximum = 0;
    for (std::size_t coefficient = 0; coefficient < kN; ++coefficient) {
        ExactInteger residue(polynomial.GetValues()[coefficient].ToString());
        if (residue < 0 || residue >= modulus)
            throw std::range_error("HighPrecisionClientIO: decrypted residue outside active modulus");
        if (residue > modulus / 2) residue -= modulus;
        coefficients[coefficient] = residue;
        maximum = std::max(maximum, Absolute(residue));
    }
    const auto primary = Forward(coefficients, input.state_.logicalScale, impl_->primary);
    const auto check = Forward(coefficients, input.state_.logicalScale, impl_->check);
    const CheckReal tolerance = NegativePowerOfTwo<CheckReal>(120);
    CheckReal maximumDisagreement = 0;
    std::vector<ClientComplex> values;
    values.reserve(kSlots);
    for (std::size_t slot = 0; slot < kSlots; ++slot) {
        const CheckReal real = Absolute(CheckReal(check[slot].real - CheckReal(primary[slot].real)));
        const CheckReal imag = Absolute(CheckReal(check[slot].imag - CheckReal(primary[slot].imag)));
        RequireFinite(real); RequireFinite(imag);
        maximumDisagreement = std::max(maximumDisagreement, std::max(real, imag));
        if (real > tolerance || imag > tolerance)
            throw std::range_error("HighPrecisionClientIO: decoding precision disagreement");
        values.push_back({ClientReal(check[slot].real), ClientReal(check[slot].imag)});
    }
    return {std::move(values), input.state_, {modulus, maximum, ExactInteger(modulus / 2 - maximum), ClientReal(maximumDisagreement)}};
}

}  // namespace openfhe_2023_1788::client_io
