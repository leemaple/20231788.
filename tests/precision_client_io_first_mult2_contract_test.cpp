#include "openfhe.h"
#include "openfhe_2023_1788/high_precision_client_io.h"
#include "openfhe_2023_1788/double_ckks.h"
#include "precision_client_io_oracle.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace {
namespace io = openfhe_2023_1788::client_io;
using namespace lossless_client_io_test;
using lbcrypto::Ciphertext;
using lbcrypto::CryptoContext;
using lbcrypto::NativeInteger;
using lbcrypto::PrivateKey;
using lbcrypto::PublicKey;
using openfhe_2023_1788::DoubleCKKS;

constexpr char kTestName[] = "precision_client_io_first_mult2_contract";
constexpr char kPin[] = "df495ba2e91739a6dc8f1de254fc5a41155ce504";
constexpr std::uint32_t kFeatures = lbcrypto::PKE | lbcrypto::KEYSWITCH | lbcrypto::LEVELEDSHE;
static_assert(NATIVEINT == 64 && MATHBACKEND == 4, "frozen native64/backend4 diagnostic");
static_assert(std::is_same_v<lbcrypto::BigInteger, bigintdyn::BigInteger>,
              "the inspected official decimal bridge is bigintdyn only");
static_assert(std::is_same_v<io::ClientReal,
              boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100>>>,
              "ClientReal is the exact 100-digit number wrapper");
static_assert(std::is_same_v<io::ExactInteger, BigInt>);
static_assert(std::is_same_v<decltype(io::ClientComplex::real), io::ClientReal>);
static_assert(std::is_same_v<decltype(io::ClientComplex::imag), io::ClientReal>);
static_assert(std::is_same_v<decltype(std::declval<const io::BoundCiphertext&>().State()),
                           const io::ClientCiphertextState&>);
static_assert(noexcept(std::declval<const io::BoundCiphertext&>().State()));
static_assert(!std::is_default_constructible_v<io::BoundCiphertext>);
static_assert(std::is_same_v<decltype(io::DecodedSlots::values), std::vector<io::ClientComplex>>);

BigFloat Tolerance(std::uint32_t bits) { return BigFloat(1) / Pow2Float(bits); }

std::vector<io::ClientComplex> ClientValues(const std::vector<MpComplex>& values) {
    std::vector<io::ClientComplex> result;
    for (const auto& value : values) result.push_back({value.real, value.imag});
    return result;
}

std::vector<MpComplex> OracleValues(const std::vector<io::ClientComplex>& values) {
    std::vector<MpComplex> result;
    for (const auto& value : values) result.push_back({value.real, value.imag});
    return result;
}

// A narrow, diagnostic-specific rejection is the only exception recovery here.
// An upstream exception, a different project diagnostic, or no exception fails.
template <class Call>
void Reject(const std::string& diagnostic, Call call) {
    bool rejected = false;
    try { call(); }
    catch (const std::invalid_argument& error) {
        Check(error.what() == diagnostic, "wrong rejection diagnostic: " + std::string(error.what()));
        rejected = true;
    }
    Check(rejected, "required rejection was accepted: " + diagnostic);
}

void CheckRational() {
    for (const auto& pair : std::vector<std::pair<BigInt, BigInt>>{
             {0, 1}, {1, 0}, {-1, 1}, {1, -1}, {-1, -1}}) {
        Reject("PositiveRationalScale: numerator and denominator must be positive", [&] {
            (void)io::PositiveRationalScale::FromPositive(pair.first, pair.second);
        });
    }
    BigInt numerator = Pow2Integer(220) * 21;
    BigInt denominator = Pow2Integer(100) * 35;
    const auto rational = io::PositiveRationalScale::FromPositive(numerator, denominator);
    const auto copy = rational;
    numerator = 1;
    denominator = 1;
    Check(rational.Numerator() == Pow2Integer(120) * 3 && rational.Denominator() == 5,
          "positive rational is not an owned gcd-reduced arbitrary-size value");
    Check(copy.Numerator() == rational.Numerator() && copy.Denominator() == rational.Denominator(),
          "positive rational copy changed value");
}

std::vector<BigInt> Project(std::vector<BigInt> coefficients) {
    Check(coefficients.size() == kRingDimension, "projection geometry mismatch");
    for (std::size_t index = 1; index < coefficients.size(); index += 2) coefficients[index] = 0;
    return coefficients;
}

void CheckOracles(const std::vector<MpComplex>& left, const std::vector<MpComplex>& right,
                  const std::vector<MpComplex>& products) {
    Check(left.size() == 16 && right.size() == 16 && products.size() == 16, "frozen vector length");
    Check(MaximumSlotError(ElementwiseProduct(left, right), products) <= BigFloat("1e-75"),
          "independent arithmetic disagrees with frozen products");
    Check(Magnitude(Subtract(Subtract(left[1], left[0]),
          {Tolerance(70), Tolerance(73)})) <= BigFloat("1e-75"), "frozen input delta");
    Check(Magnitude(Subtract(Subtract(products[1], products[0]), FrozenProductDelta())) <=
          BigFloat("1e-75"), "frozen product delta");
    for (const auto& values : {left, right}) {
        for (const auto& value : values)
            Check(AbsFloat(value.real) + AbsFloat(value.imag) <= 1, "frozen input envelope");
    }

    // Literal controls do not call the client transform. Powers of five and
    // the X^2 table are independently frozen; full Horner and packed projection
    // are deliberately distinguished at gap 2.
    for (const std::size_t power : {0U, 32U, 2U, 1U}) {
        std::vector<BigInt> coefficients(64, 0);
        coefficients[power] = 1;
        const auto projected = DirectCanonicalEvaluateRational(Project(coefficients), 1, 1);
        const auto full = DirectCanonicalEvaluateRational(coefficients, 1, 1);
        const auto x2 = X2WitnessTable();
        for (std::size_t slot = 0; slot < 16; ++slot) {
            const MpComplex expected = power == 0 ? MpComplex{1, 0} :
                power == 32 ? MpComplex{0, 1} : power == 2 ? x2[slot] : MpComplex{0, 0};
            Check(Magnitude(Subtract(projected[slot], expected)) <= BigFloat("1e-70"),
                  "literal canonical/projection witness failed");
            if (power == 1) Check(Magnitude(full[slot]) > BigFloat("0.99"), "off-stride X disappeared");
        }
    }

    // Independent O(S^2) real interpolation, not the production butterfly
    // transform: include each selected root and its conjugate. Checking its
    // polynomial by direct Horner also fixes the inverse convention.
    const auto exponents = CanonicalExponents();
    const BigInt scale = Pow2Integer(200);
    std::vector<BigInt> interpolated(64, 0);
    for (std::uint32_t coefficient = 0; coefficient < 64; coefficient += 2) {
        BigFloat value = 0;
        for (std::size_t slot = 0; slot < 16; ++slot) {
            const auto root = RootForExponent((exponents[slot] * coefficient) % 128);
            value += left[slot].real * root.real + left[slot].imag * root.imag;
        }
        const BigFloat scaled = value * ToBigFloat(scale) / 16;
        const BigFloat floor = boost::multiprecision::floor(scaled);
        const BigFloat rounded = scaled - floor <= BigFloat("0.5") ? floor : BigFloat(floor + 1);
        interpolated[coefficient] = rounded.convert_to<BigInt>();
    }
    Check(MaximumSlotError(DirectCanonicalEvaluateRational(interpolated, scale, 1), left) <=
          Tolerance(180), "independent inverse/forward canonical convention mismatch");
}

using Params = lbcrypto::CryptoParametersCKKSRNS;

io::OrderedDcrtBasis Basis(const std::shared_ptr<DCRTPoly::Params>& params) {
    Check(params != nullptr, "observed null basis");
    io::OrderedDcrtBasis result{params->GetCyclotomicOrder(), params->GetRingDimension(), {}, {}};
    for (const auto& tower : params->GetParams()) {
        Check(tower != nullptr, "observed null tower parameters");
        Check(tower->GetCyclotomicOrder() == result.cyclotomicOrder &&
              tower->GetRingDimension() == result.ringDimension, "inconsistent tower geometry");
        result.moduliDecimal.push_back(tower->GetModulus().ToString());
        result.rootsOfUnityDecimal.push_back(tower->GetRootOfUnity().ToString());
    }
    return result;
}

bool SameBasis(const io::OrderedDcrtBasis& left, const io::OrderedDcrtBasis& right) {
    return left.cyclotomicOrder == right.cyclotomicOrder && left.ringDimension == right.ringDimension &&
           left.moduliDecimal == right.moduliDecimal && left.rootsOfUnityDecimal == right.rootsOfUnityDecimal;
}

bool SameState(const io::ClientCiphertextState& left, const io::ClientCiphertextState& right) {
    const auto& a = left.contextProfile;
    const auto& b = right.contextProfile;
    if (a.contextIdentity != b.contextIdentity || a.cryptoParamsIdentity != b.cryptoParamsIdentity ||
        a.requiredFeatureMask != b.requiredFeatureMask || a.enabledFeatureMaskObserved != b.enabledFeatureMaskObserved ||
        a.scalingTechnique != b.scalingTechnique || a.keySwitchTechnique != b.keySwitchTechnique ||
        a.executionMode != b.executionMode || a.decryptionNoiseMode != b.decryptionNoiseMode || a.ckksDataType != b.ckksDataType)
        return false;
    if (left.firstMult2ScaleFactors.has_value() != right.firstMult2ScaleFactors.has_value()) return false;
    if (left.firstMult2ScaleFactors &&
        (left.firstMult2ScaleFactors->qDiv != right.firstMult2ScaleFactors->qDiv ||
         left.firstMult2ScaleFactors->qL != right.firstMult2ScaleFactors->qL)) return false;
    return left.keyTag == right.keyTag && SameBasis(left.activeBasis, right.activeBasis) &&
        left.encodingType == right.encodingType && left.componentFormat == right.componentFormat &&
        left.slots == right.slots && left.strideGap == right.strideGap && left.level == right.level &&
        left.componentCount == right.componentCount && left.metadataMapEmpty == right.metadataMapEmpty &&
        left.noiseScaleDegree == right.noiseScaleDegree && left.recordedScalingFactor == right.recordedScalingFactor &&
        left.scalingFactorInt == right.scalingFactorInt && left.logicalScale.Numerator() == right.logicalScale.Numerator() &&
        left.logicalScale.Denominator() == right.logicalScale.Denominator() && left.projection == right.projection &&
        left.origin == right.origin;
}

std::string BasisObservation(const io::OrderedDcrtBasis& basis) {
    std::ostringstream out;
    out << basis.cyclotomicOrder << ':' << basis.ringDimension;
    for (std::size_t index = 0; index < basis.moduliDecimal.size(); ++index)
        out << ':' << basis.moduliDecimal[index] << '/' << basis.rootsOfUnityDecimal.at(index);
    return out.str();
}

// In-memory snapshots only; key coefficients are never printed or persisted.
// In particular this is null-safe for deliberately unset DCRT/NativePoly keys.
std::string ElementObservation(const DCRTPoly& element) {
    std::ostringstream out;
    out << element.GetParams().get() << ':' << element.GetFormat();
    if (element.GetParams()) out << ':' << BasisObservation(Basis(element.GetParams()));
    for (const auto& tower : element.GetAllElements()) {
        out << '|' << tower.GetParams().get() << ':' << tower.GetFormat();
        if (tower.GetParams()) out << ':' << tower.GetModulus() << ':' << tower.GetRootOfUnity();
        out << ':' << tower.IsEmpty();
        if (!tower.IsEmpty()) {
            const auto& values = tower.GetValues();
            for (std::size_t index = 0; index < values.GetLength(); ++index) out << ',' << values[index];
        }
    }
    return out.str();
}

std::string KeyObservation(const PublicKey<DCRTPoly>& key) {
    if (!key) return "null";
    std::ostringstream out;
    out << key->GetCryptoContext().get() << ':' << key->GetKeyTag();
    for (const auto& element : key->GetPublicElements()) out << '|' << ElementObservation(element);
    return out.str();
}

std::string KeyObservation(const PrivateKey<DCRTPoly>& key) {
    if (!key) return "null";
    std::ostringstream out;
    out << key->GetCryptoContext().get() << ':' << key->GetKeyTag() << '|' << ElementObservation(key->GetPrivateElement());
    return out.str();
}

std::string ContextObservation(const CryptoContext<DCRTPoly>& context) {
    const auto cp = std::dynamic_pointer_cast<Params>(context->GetCryptoParameters());
    Check(cp != nullptr, "actual factory result is not CKKS-RNS");
    Check(context->getSchemeId() == lbcrypto::CKKSRNS_SCHEME, "actual scheme ID");
    Check(context->GetScheme()->GetEnabled() == kFeatures, "actual feature mask");
    Check(cp->GetScalingTechnique() == lbcrypto::FIXEDMANUAL && cp->GetKeySwitchTechnique() == lbcrypto::HYBRID &&
          cp->GetEncryptionTechnique() == lbcrypto::STANDARD && cp->GetMultiplicationTechnique() == lbcrypto::HPS &&
          cp->GetPREMode() == lbcrypto::NOT_SET && cp->GetCKKSDataType() == lbcrypto::COMPLEX &&
          cp->GetExecutionMode() == lbcrypto::EXEC_EVALUATION &&
          cp->GetDecryptionNoiseMode() == lbcrypto::FIXED_NOISE_DECRYPT && cp->GetNoiseScale() == 1 &&
          cp->GetSecretKeyDist() == lbcrypto::UNIFORM_TERNARY && cp->GetStdLevel() == lbcrypto::HEStd_NotSet &&
          cp->GetDigitSize() == 0 && cp->GetMaxRelinSkDeg() == 2 && cp->GetMultiplicativeDepth() == 7 &&
          cp->GetMultipartyMode() == lbcrypto::FIXED_NOISE_MULTIPARTY && cp->GetThresholdNumOfParties() == 1,
          "actual frozen context profile");
    Check(cp->GetEncodingParams() != nullptr && cp->GetBatchSize() == 16 && cp->GetPlaintextModulus() == 50,
          "actual encoding parameters");
    const auto q = Basis(cp->GetElementParams());
    const auto p = Basis(cp->GetParamsP());
    const auto qp = Basis(cp->GetParamsQP());
    Check(q.ringDimension == 64 && q.cyclotomicOrder == 128 && q.moduliDecimal.size() == 8,
          "actual frozen Q geometry");
    Check(!p.moduliDecimal.empty() && SameBasis(Basis(cp->GetParamsPK()), q), "actual P/paramsPK=Q");
    auto concatenated = q;
    concatenated.moduliDecimal.insert(concatenated.moduliDecimal.end(), p.moduliDecimal.begin(), p.moduliDecimal.end());
    concatenated.rootsOfUnityDecimal.insert(concatenated.rootsOfUnityDecimal.end(), p.rootsOfUnityDecimal.begin(), p.rootsOfUnityDecimal.end());
    Check(SameBasis(qp, concatenated), "actual QP is not exact Q followed by P");
    Check(q.moduliDecimal.back() == "1125899906843009" && q.moduliDecimal[6] == "1125899906840833",
          "actual qDiv/qL differ from frozen diagnostic");
    Check(cp->GetNumPartQ() == 3 && cp->GetNumPerPartQ() == 3 && cp->GetNumberOfQPartitions() == 3,
          "actual HYBRID partition profile");
    std::ostringstream out;
    out << std::hexfloat << context.get() << ':' << cp.get() << ':' << context->GetScheme().get() << ':'
        << context->GetScheme()->GetEnabled() << ':' << BasisObservation(q) << ':' << BasisObservation(p)
        << ':' << BasisObservation(qp) << ':' << cp->GetEncodingParams().get();
    io::OrderedDcrtBasis partitions{128, 64, {}, {}};
    for (std::uint32_t part = 0; part < cp->GetNumPartQ(); ++part) {
        const auto basis = Basis(cp->GetParamsPartQ(part));
        partitions.moduliDecimal.insert(partitions.moduliDecimal.end(), basis.moduliDecimal.begin(), basis.moduliDecimal.end());
        partitions.rootsOfUnityDecimal.insert(partitions.rootsOfUnityDecimal.end(), basis.rootsOfUnityDecimal.begin(), basis.rootsOfUnityDecimal.end());
        out << ':' << BasisObservation(basis);
        for (std::uint32_t level = 0; level < basis.moduliDecimal.size(); ++level) {
            for (const auto& value : cp->GetPartQlHatInvModq(part, level)) out << ':' << value;
            for (const auto& value : cp->GetPartQlHatInvModqPrecon(part, level)) out << ':' << value;
        }
    }
    Check(SameBasis(partitions, q), "actual ordered HYBRID partitions");
    const BigInt pProduct = [&] { BigInt result = 1; for (const auto& modulus : p.moduliDecimal) result *= BigInt(modulus); return result; }();
    Check(cp->GetPModq().size() == q.moduliDecimal.size(), "HYBRID PModq table shape");
    for (std::size_t index = 0; index < q.moduliDecimal.size(); ++index) {
        Check(BigInt(cp->GetPModq()[index].ToString()) == pProduct % BigInt(q.moduliDecimal[index]), "HYBRID PModq table value");
        out << ':' << cp->GetPModq()[index] << ':' << cp->GetModReduceFactor(index);
    }
    out << ':' << cp->GetScalingFactorReal(0) << ':' << cp->GetDistributionParameter() << ':'
        << cp->GetNoiseEstimate() << ':' << cp->GetStatisticalSecurity() << ':' << cp->GetNumAdversarialQueries();
    return out.str();
}

CryptoContext<DCRTPoly> MakeContext(std::uint32_t firstModSize = 55) {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> p;
    p.SetMultiplicativeDepth(7); p.SetScalingModSize(50); p.SetFirstModSize(firstModSize);
    p.SetScalingTechnique(lbcrypto::FIXEDMANUAL); p.SetKeySwitchTechnique(lbcrypto::HYBRID);
    p.SetDigitSize(0); p.SetMaxRelinSkDeg(2); p.SetNumLargeDigits(0);
    p.SetSecretKeyDist(lbcrypto::UNIFORM_TERNARY); p.SetSecurityLevel(lbcrypto::HEStd_NotSet);
    p.SetRingDim(64); p.SetBatchSize(16); p.SetCKKSDataType(lbcrypto::COMPLEX);
    p.SetPREMode(lbcrypto::NOT_SET); p.SetExecutionMode(lbcrypto::EXEC_EVALUATION);
    p.SetDecryptionNoiseMode(lbcrypto::FIXED_NOISE_DECRYPT);
    p.SetStandardDeviation(3.19f); p.SetNoiseEstimate(0.0); p.SetDesiredPrecision(25.0);
    p.SetStatisticalSecurity(30); p.SetNumAdversarialQueries(1);
    p.SetInteractiveBootCompressionLevel(lbcrypto::SLACK); p.SetCompositeDegree(1); p.SetRegisterWordSize(64);
    // CKKS disables encryption/multiplication/multiparty/threshold setters;
    // validate their exact official defaults on the actual factory result.
    auto context = lbcrypto::GenCryptoContext(p);
    Check(context != nullptr, "context generation returned null");
    context->Enable(lbcrypto::PKE); context->Enable(lbcrypto::KEYSWITCH); context->Enable(lbcrypto::LEVELEDSHE);
    (void)ContextObservation(context);
    return context;
}

void CheckUnsupportedFirstModulus() {
    const auto context = MakeContext(56);
    const auto before = ContextObservation(context);
    const auto cp = std::dynamic_pointer_cast<Params>(context->GetCryptoParameters());
    const auto actualFirstBits =
        cp->GetElementParams()->GetParams().front()->GetModulus().GetMSB();
    Check(actualFirstBits == 56, "first-modulus fixture did not produce 56 bits");
    Check(cp->GetScalingFactorReal(0) == std::ldexp(1.0, 50),
          "first-modulus fixture changed base scaling factor");
    std::cout << "first_modulus_boundary_fixture ready=1 actual_first_bits="
              << actualFirstBits << " Q_towers=8 N=64 M=128 fixture_new_keypairs=0"
              << std::endl;
    const std::string diagnostic =
        "HighPrecisionClientIO: unsupported diagnostic Q basis";
    bool rejected = false;
    try { (void)io::HighPrecisionClientIO(context); }
    catch (const std::domain_error& error) {
        Check(error.what() == diagnostic,
              "wrong first-modulus diagnostic: " + std::string(error.what()));
        rejected = true;
    }
    Check(ContextObservation(context) == before,
          "first-modulus constructor changed context or tables");
    Check(rejected, "required rejection was accepted: " + diagnostic);
    std::cout << "first_modulus_boundary_rejection passed" << std::endl;
}

void CheckElement(const DCRTPoly& element, const io::OrderedDcrtBasis& expected) {
    Check(element.GetParams() != nullptr && SameBasis(Basis(element.GetParams()), expected), "element declared basis");
    Check(element.GetFormat() == Format::EVALUATION && element.GetAllElements().size() == expected.moduliDecimal.size(), "element shape/format");
    for (std::size_t index = 0; index < element.GetAllElements().size(); ++index) {
        const auto& tower = element.GetAllElements()[index];
        Check(tower.GetParams() != nullptr && !tower.IsEmpty(), "element empty tower");
        Check(tower.GetFormat() == Format::EVALUATION && tower.GetCyclotomicOrder() == 128 &&
              tower.GetModulus().ToString() == expected.moduliDecimal[index] &&
              tower.GetRootOfUnity().ToString() == expected.rootsOfUnityDecimal[index] && tower.GetValues().GetLength() == 64,
              "element actual tower identity");
    }
}

void CheckBound(const io::BoundCiphertext& bound, const CryptoContext<DCRTPoly>& context,
                const std::string& tag, std::uint32_t level, double recorded) {
    const auto cp = std::dynamic_pointer_cast<Params>(context->GetCryptoParameters());
    auto expected = Basis(cp->GetElementParams());
    expected.moduliDecimal.resize(expected.moduliDecimal.size() - level);
    expected.rootsOfUnityDecimal.resize(expected.rootsOfUnityDecimal.size() - level);
    const auto& state = bound.State();
    const auto& profile = state.contextProfile;
    Check(profile.contextIdentity == context.get() && profile.cryptoParamsIdentity == cp.get() &&
          profile.requiredFeatureMask == kFeatures && profile.enabledFeatureMaskObserved == context->GetScheme()->GetEnabled() &&
          profile.scalingTechnique == lbcrypto::FIXEDMANUAL && profile.keySwitchTechnique == lbcrypto::HYBRID &&
          profile.executionMode == lbcrypto::EXEC_EVALUATION && profile.decryptionNoiseMode == lbcrypto::FIXED_NOISE_DECRYPT &&
          profile.ckksDataType == lbcrypto::COMPLEX, "receipt context profile");
    Check(!tag.empty() && state.keyTag == tag && SameBasis(state.activeBasis, expected), "receipt tag/basis");
    Check(state.encodingType == lbcrypto::CKKS_PACKED_ENCODING && state.componentFormat == Format::EVALUATION &&
          state.slots == 16 && state.strideGap == 2 && state.level == level && state.componentCount == 2 &&
          state.metadataMapEmpty && state.noiseScaleDegree == 2 && state.recordedScalingFactor == recorded &&
          state.scalingFactorInt == NativeInteger(1) && state.projection == io::CanonicalProjection::OpenFhePackedStride,
          "receipt ciphertext state");
    Check(state.logicalScale.Numerator() == Pow2Integer(level == 0 ? 100 : 200) &&
          state.logicalScale.Denominator() == (level == 0 ? BigInt(1) : BigInt("1267650600226646386227681786497")),
          "receipt exact rational scale");
    Check(state.origin == (level == 0 ? io::ClientCiphertextOrigin::FreshClientEncoding : io::ClientCiphertextOrigin::FirstMult2Rcb), "receipt origin");
    Check(state.firstMult2ScaleFactors.has_value() == (level == 2), "receipt optional factors");
    if (level == 2) Check(state.firstMult2ScaleFactors->qDiv == BigInt("1125899906843009") &&
                         state.firstMult2ScaleFactors->qL == BigInt("1125899906840833"), "receipt factors");
    const auto ciphertext = bound.CloneForEvaluation();
    Check(ciphertext != nullptr && ciphertext->GetCryptoContext().get() == context.get() && ciphertext->GetKeyTag() == tag,
          "receipt actual ciphertext identity");
    Check(ciphertext->GetEncodingType() == state.encodingType && ciphertext->GetLevel() == level &&
          ciphertext->GetSlots() == 16 && ciphertext->GetNoiseScaleDeg() == 2 && ciphertext->GetScalingFactor() == recorded &&
          ciphertext->GetScalingFactorInt() == NativeInteger(1) && ciphertext->GetElements().size() == 2 &&
          ciphertext->GetMetadataMap() && ciphertext->GetMetadataMap()->empty(), "actual ciphertext state");
    for (const auto& element : ciphertext->GetElements()) CheckElement(element, expected);
}

// Value snapshots never escape this process. In particular, compare map keys
// explicitly: official Ciphertext equality compares metadata values, not keys.
std::string CiphertextObservation(const Ciphertext<DCRTPoly>& ciphertext) {
    Check(ciphertext != nullptr, "observed null ciphertext");
    std::ostringstream out;
    out << std::hexfloat << ciphertext->GetCryptoContext().get() << ':'
        << ciphertext->GetKeyTag() << ':' << ciphertext->GetEncodingType() << ':'
        << ciphertext->GetSlots() << ':' << ciphertext->GetLevel() << ':' << ciphertext->GetHopLevel() << ':'
        << ciphertext->GetNoiseScaleDeg() << ':' << ciphertext->GetScalingFactor()
        << ':' << ciphertext->GetScalingFactorInt();
    const auto metadata = ciphertext->GetMetadataMap();
    out << ':' << static_cast<bool>(metadata);
    if (metadata) {
        out << ':' << metadata->size();
        for (const auto& entry : *metadata) out << ':' << entry.first << ':' << static_cast<bool>(entry.second);
    }
    for (const auto& element : ciphertext->GetElements()) out << '|' << ElementObservation(element);
    return out.str();
}

void CheckCloneIsolation(const io::BoundCiphertext& bound, const CryptoContext<DCRTPoly>& context,
                         const std::string& tag, std::uint32_t level, double recorded) {
    const auto stateBefore = bound.State();
    const auto changed = bound.CloneForEvaluation(), sibling = bound.CloneForEvaluation();
    Check(changed.get() != sibling.get() && changed->GetMetadataMap() && sibling->GetMetadataMap() &&
          changed->GetMetadataMap().get() != sibling->GetMetadataMap().get() &&
          changed->GetMetadataMap()->empty() && sibling->GetMetadataMap()->empty(), "clones must own empty metadata maps");
    const auto before = CiphertextObservation(sibling);
    Check(CiphertextObservation(changed) == before, "initial evaluator clones differ");
    auto& residue = changed->GetElements().at(0).GetAllElements().at(0).at(0);
    const auto oldResidue = residue;
    const auto nextHopLevel = changed->GetHopLevel() + 1;
    residue = oldResidue == NativeInteger(0) ? NativeInteger(1) : NativeInteger(0);
    Check(residue != oldResidue, "clone coefficient-storage mutation did not occur");
    changed->SetLevel(level + 1);
    changed->SetHopLevel(nextHopLevel);
    changed->SetSlots(8);
    changed->SetNoiseScaleDeg(3);
    changed->SetScalingFactor(1.0);
    changed->SetScalingFactorInt(NativeInteger(2));
    changed->SetKeyTag(tag + ":clone-only");
    changed->SetEncodingType(lbcrypto::COEF_PACKED_ENCODING);
    Check(changed->GetLevel() == level + 1 && changed->GetHopLevel() == nextHopLevel &&
          changed->GetSlots() == 8 && changed->GetNoiseScaleDeg() == 3 &&
          changed->GetScalingFactor() == 1.0 && changed->GetScalingFactorInt() == NativeInteger(2) &&
          changed->GetKeyTag() == tag + ":clone-only" && changed->GetEncodingType() == lbcrypto::COEF_PACKED_ENCODING,
          "clone scalar mutations did not occur");
    changed->SetMetadataByKey("clone-only", std::make_shared<lbcrypto::Metadata>());
    Check(changed->GetMetadataMap()->size() == 1 && changed->GetMetadataMap()->count("clone-only") == 1 &&
          changed->GetMetadataMap()->at("clone-only") != nullptr, "clone metadata mutation did not occur");
    Check(CiphertextObservation(changed) != before, "clone mutation witness unchanged");
    Check(CiphertextObservation(sibling) == before &&
          CiphertextObservation(bound.CloneForEvaluation()) == before && SameState(bound.State(), stateBefore),
          "evaluator clone changed sibling or bound receipt coefficients/scalars/map");
    CheckBound(bound, context, tag, level, recorded);
}

struct Observation final {
    io::DecodedSlots decoded;
    BigFloat publicError;
    BigFloat oracleError;
    BigFloat publicDeltaError;
    BigFloat oracleDeltaError;
    BigFloat componentDisagreement;
};

Observation Observe(const io::HighPrecisionClientIO& client, const PrivateKey<DCRTPoly>& key,
                    const io::BoundCiphertext& bound, const std::vector<MpComplex>& expected,
                    const MpComplex& expectedDelta) {
    const auto clone = bound.CloneForEvaluation();
    const auto polynomial = IndependentDecrypt(clone, key);
    const auto& scale = bound.State().logicalScale;
    const auto oracle = DirectCanonicalEvaluateRational(Project(polynomial.coefficients), scale.Numerator(), scale.Denominator());
    auto decoded = client.Decrypt(key, bound);
    const auto values = OracleValues(decoded.values);
    Check(values.size() == 16 && oracle.size() == 16, "owned decoded/oracle slot count");
    // The frozen vectors are within the unit envelope. This explicit bounded
    // comparison also rejects NaN/Inf before a maximum reduction could hide it.
    for (const auto& slots : {values, oracle}) {
        for (const auto& value : slots)
            Check(AbsFloat(value.real) < 2 && AbsFloat(value.imag) < 2, "nonfinite/out-of-envelope observation");
    }
    const auto maxCoefficient = MaximumAbsolute(polynomial.coefficients);
    Check(decoded.diagnostics.activeCompositeModulus == polynomial.modulus &&
          decoded.diagnostics.maximumCenteredAbsoluteCoefficient == maxCoefficient &&
          decoded.diagnostics.centeredHeadroom == polynomial.modulus / 2 - maxCoefficient &&
          decoded.diagnostics.centeredHeadroom >= 0, "actual centered diagnostics");
    Check(decoded.diagnostics.maximumCrossPrecisionDisagreement >= 0 &&
          decoded.diagnostics.maximumCrossPrecisionDisagreement <= Tolerance(120), "public cross-precision guard");
    Check(SameState(decoded.state, bound.State()), "owned decoded state disagrees with receipt");
    const BigFloat publicError = MaximumSlotError(values, expected);
    const BigFloat oracleError = MaximumSlotError(oracle, expected);
    const BigFloat publicDelta = Magnitude(Subtract(Subtract(values[1], values[0]), expectedDelta));
    const BigFloat oracleDelta = Magnitude(Subtract(Subtract(oracle[1], oracle[0]), expectedDelta));
    Check(publicError <= Tolerance(80) && oracleError <= Tolerance(80), "frozen all-slot 2^-80 precision");
    Check(publicDelta <= Tolerance(80) && oracleDelta <= Tolerance(80), "frozen adjacent-delta 2^-80 precision");
    BigFloat disagreement = 0;
    for (std::size_t index = 0; index < 16; ++index) {
        const BigFloat real = AbsFloat(values[index].real - oracle[index].real);
        const BigFloat imag = AbsFloat(values[index].imag - oracle[index].imag);
        disagreement = std::max(disagreement, std::max(real, imag));
    }
    Check(disagreement <= Tolerance(120), "production forward transform versus independent projected Horner");
    // clone and all local coefficient/CRT/Horner buffers die on return. The
    // caller subsequently reads decoded.values; only owned values may escape.
    return {std::move(decoded), publicError, oracleError, publicDelta, oracleDelta, disagreement};
}

// Only owned replacement parameters are used for malformed-basis fixtures.
// No shared context/key Params are mutated; live drift belongs to Cycle B.
DCRTPoly WrongBasis(const std::shared_ptr<DCRTPoly::Params>& original, unsigned variant) {
    std::vector<NativeInteger> moduli, roots;
    for (const auto& tower : original->GetParams()) { moduli.push_back(tower->GetModulus()); roots.push_back(tower->GetRootOfUnity()); }
    if (variant == 0) { moduli.pop_back(); roots.pop_back(); }
    if (variant == 1) { std::swap(moduli[0], moduli[1]); std::swap(roots[0], roots[1]); }
    if (variant == 2) roots[0] = roots[0].ModInverse(moduli[0]);
    const auto owned = std::make_shared<DCRTPoly::Params>(128, moduli, roots);
    Check(owned.get() != original.get(), "malformed fixture must own its parameters");
    return DCRTPoly(owned, Format::EVALUATION, true);
}

template <class Unchanged>
std::size_t CheckMalformedKeys(const io::HighPrecisionClientIO& client, const PublicKey<DCRTPoly>& publicKey,
                              const PrivateKey<DCRTPoly>& secretKey, const std::vector<io::ClientComplex>& values,
                              const io::FreshEncodingSpec& spec, const io::BoundCiphertext& fresh, Unchanged unchanged) {
    std::size_t count = 0;
    const auto rejectPublic = [&](const PublicKey<DCRTPoly>& malformed) {
        const auto before = KeyObservation(malformed);
        Reject("HighPrecisionClientIO: malformed public key", [&] { (void)client.Encrypt(malformed, values, spec); });
        Check(KeyObservation(malformed) == before, "malformed public key changed"); unchanged(); ++count;
    };
    const auto rejectPrivate = [&](const PrivateKey<DCRTPoly>& malformed) {
        const auto before = KeyObservation(malformed);
        Reject("HighPrecisionClientIO: malformed private key", [&] { (void)client.Decrypt(malformed, fresh); });
        Check(KeyObservation(malformed) == before, "malformed private key changed"); unchanged(); ++count;
    };
    rejectPublic(nullptr); rejectPrivate(nullptr);
    const auto& original = publicKey->GetPublicElements();
    for (const std::size_t size : {0U, 1U, 3U}) {
        auto key = std::make_shared<lbcrypto::PublicKeyImpl<DCRTPoly>>(publicKey->GetCryptoContext(), publicKey->GetKeyTag());
        std::vector<DCRTPoly> elements(size, original[0]); key->SetPublicElements(std::move(elements)); rejectPublic(key);
    }
    std::vector<DCRTPoly> malformedElements{DCRTPoly{}};
    // The official default DCRT owns empty Params; moving it is the public,
    // non-dereferencing way to obtain the distinct null-Params shape. Never
    // call DCRTPoly(nullptr, ...) or operator== on this moved-from fixture.
    DCRTPoly nullParameters;
    const auto ownedDefault = std::move(nullParameters);
    Check(!nullParameters.GetParams() && ownedDefault.GetParams() != nullptr, "null-Params fixture construction");
    malformedElements.push_back(nullParameters);
    malformedElements.emplace_back(original[0].GetParams(), Format::EVALUATION, false);
    auto coefficient = original[0]; coefficient.SetFormat(Format::COEFFICIENT); malformedElements.push_back(coefficient);
    auto missingTower = original[0]; missingTower.GetAllElements()[1] = lbcrypto::NativePoly{}; malformedElements.push_back(missingTower);
    auto mixedFormat = original[0]; mixedFormat.GetAllElements()[1].SetFormat(Format::COEFFICIENT); malformedElements.push_back(mixedFormat);
    for (unsigned variant = 0; variant < 3; ++variant) malformedElements.push_back(WrongBasis(original[0].GetParams(), variant));
    for (const auto& element : malformedElements) {
        // Either public component must be checked, not merely pk[0].
        for (std::size_t index = 0; index < 2; ++index) {
            auto key = std::make_shared<lbcrypto::PublicKeyImpl<DCRTPoly>>(*publicKey);
            auto elements = original; elements[index] = element; key->SetPublicElements(std::move(elements)); rejectPublic(key);
        }
    }
    auto emptySecret = std::make_shared<lbcrypto::PrivateKeyImpl<DCRTPoly>>(secretKey->GetCryptoContext());
    emptySecret->SetKeyTag(secretKey->GetKeyTag()); rejectPrivate(emptySecret);
    // Shape-only fixtures do not claim a matching RLWE key relation.
    for (std::size_t index = 1; index < malformedElements.size(); ++index) {
        auto key = std::make_shared<lbcrypto::PrivateKeyImpl<DCRTPoly>>(*secretKey);
        key->SetPrivateElement(malformedElements[index]); rejectPrivate(key);
    }
    return count;
}

template <class Unchanged>
void CheckSharedParamsDrift(const CryptoContext<DCRTPoly>& originalContext,
                            double freshRecorded, double outputRecorded, Unchanged originalUnchanged) {
    using ContextImpl = lbcrypto::CryptoContextImpl<DCRTPoly>;
    using Factory = lbcrypto::CryptoContextFactory<DCRTPoly>;
    // Strong references prevent address reuse and keep every original context
    // alive when only the factory registry is cleared. Never clear eval maps.
    const auto registeredBefore = Factory::GetAllContexts();
    const auto multBefore = ContextImpl::GetAllEvalMultKeys();
    const auto autoBefore = ContextImpl::GetAllEvalAutomorphismKeys();
    std::vector<std::pair<std::string, std::map<std::uint32_t, lbcrypto::EvalKey<DCRTPoly>>>> autoRowsBefore;
    for (const auto& row : autoBefore) {
        Check(row.second != nullptr, "null original automorphism row");
        autoRowsBefore.emplace_back(row.first, *row.second);
    }
    Check(std::find(registeredBefore.begin(), registeredBefore.end(), originalContext) != registeredBefore.end(),
          "original context missing before disposable fixture");
    const auto originalParams = std::dynamic_pointer_cast<Params>(originalContext->GetCryptoParameters());
    Check(originalParams != nullptr, "original context is not CKKS-RNS");
    std::vector<const void*> originalBases, originalNativeParams;
    const auto rememberBasis = [&](const std::shared_ptr<DCRTPoly::Params>& basis) {
        Check(basis != nullptr, "null original fixture basis");
        originalBases.push_back(basis.get());
        for (const auto& tower : basis->GetParams()) {
            Check(tower != nullptr, "null original fixture native parameters");
            originalNativeParams.push_back(tower.get());
        }
    };
    for (const auto& old : registeredBefore) {
        const auto cp = std::dynamic_pointer_cast<Params>(old->GetCryptoParameters());
        Check(cp != nullptr && cp->GetNumPartQ() == 3 && cp->GetNumberOfQPartitions() == 3,
              "unexpected original factory context");
        rememberBasis(cp->GetElementParams()); rememberBasis(cp->GetParamsP()); rememberBasis(cp->GetParamsQP());
        for (std::uint32_t part = 0; part < 3; ++part) rememberBasis(cp->GetParamsPartQ(part));
    }
    Factory::ReleaseAllContexts();
    const auto context = MakeContext(55);
    const auto cp = std::dynamic_pointer_cast<Params>(context->GetCryptoParameters());
    Check(cp != nullptr && cp->GetElementParams()->GetParams().front()->GetModulus().GetMSB() == 55,
          "disposable fixture is not actual firstMod55");
    for (const auto& old : registeredBefore) {
        Check(context.get() != old.get() && cp.get() != old->GetCryptoParameters().get() &&
              context->GetEncodingParams().get() != old->GetEncodingParams().get(),
              "disposable context/crypto/encoding parameters alias an original context");
    }
    Check(*cp == *originalParams && *context->GetEncodingParams() == *originalContext->GetEncodingParams(),
          "disposable crypto/encoding values differ from original firstMod55");
    const auto isolatedBasis = [&](const std::shared_ptr<DCRTPoly::Params>& actual,
                                   const std::shared_ptr<DCRTPoly::Params>& original) {
        Check(actual != nullptr && SameBasis(Basis(actual), Basis(original)) &&
              actual->GetModulus() == original->GetModulus(), "disposable basis values differ from original firstMod55");
        Check(std::find(originalBases.begin(), originalBases.end(), actual.get()) == originalBases.end(),
              "disposable basis aliases original parameters");
        for (const auto& tower : actual->GetParams())
            Check(std::find(originalNativeParams.begin(), originalNativeParams.end(), tower.get()) == originalNativeParams.end(),
                  "disposable native parameters alias original parameters");
    };
    isolatedBasis(cp->GetElementParams(), originalParams->GetElementParams());
    isolatedBasis(cp->GetParamsP(), originalParams->GetParamsP());
    isolatedBasis(cp->GetParamsQP(), originalParams->GetParamsQP());
    for (std::uint32_t part = 0; part < 3; ++part)
        isolatedBasis(cp->GetParamsPartQ(part), originalParams->GetParamsPartQ(part));
    Check(ContextImpl::GetAllEvalMultKeys() == multBefore && ContextImpl::GetAllEvalAutomorphismKeys() == autoBefore,
          "factory registry release changed evaluation maps");
    originalUnchanged();
    std::cout << "shared_params_fixture allocation_plan additional_matching_keypairs=1 additional_eval_keygen_calls=1"
              << " actual_first_bits=55 isolated_context_and_bases=1" << std::endl;
    const auto keys = context->KeyGen();
    Check(keys.good() && !keys.publicKey->GetKeyTag().empty() && keys.publicKey->GetKeyTag() == keys.secretKey->GetKeyTag(),
          "disposable matching keypair");
    const auto tag = keys.secretKey->GetKeyTag();
    Check(multBefore.count(tag) == 0 && autoBefore.count(tag) == 0, "disposable key tag collides with an original cache entry");
    context->EvalMultKeyGen(keys.secretKey);
    auto expectedMult = multBefore;
    const auto& ownKeys = ContextImpl::GetAllEvalMultKeys().at(tag);
    Check(ownKeys.size() == 1 && ownKeys.front() != nullptr && ownKeys.front()->GetCryptoContext().get() == context.get() &&
          ownKeys.front()->GetKeyTag() == tag, "disposable evaluation key identity");
    expectedMult.emplace(tag, ownKeys);
    const auto mapsUnchanged = [&] {
        Check(ContextImpl::GetAllEvalMultKeys() == expectedMult && ContextImpl::GetAllEvalAutomorphismKeys() == autoBefore,
              "disposable fixture changed original evaluation maps");
        for (const auto& row : autoRowsBefore)
            Check(*ContextImpl::GetAllEvalAutomorphismKeys().at(row.first) == row.second,
                  "disposable fixture changed an original automorphism row");
    };
    const auto leftValues = ClientValues(LeftValues()), rightValues = ClientValues(RightValues());
    const auto contextBefore = ContextObservation(context), pkBefore = KeyObservation(keys.publicKey), skBefore = KeyObservation(keys.secretKey);
    const io::FreshEncodingSpec spec{16, io::PositiveRationalScale::FromPositive(Pow2Integer(100), 1)};
    const io::HighPrecisionClientIO client(context);
    const auto left = client.Encrypt(keys.publicKey, leftValues, spec);
    const auto right = client.Encrypt(keys.publicKey, rightValues, spec);
    CheckBound(left, context, tag, 0, freshRecorded); CheckBound(right, context, tag, 0, freshRecorded);
    const auto leftView = left.CloneForEvaluation(), rightView = right.CloneForEvaluation();
    const auto leftBefore = CiphertextObservation(leftView), rightBefore = CiphertextObservation(rightView);
    const DoubleCKKS evaluator(context);
    const auto product = evaluator.Mult2(evaluator.DCP(leftView), evaluator.DCP(rightView));
    Check(product.GetLifecycle() == openfhe_2023_1788::PairLifecycle::RefreshRequired, "disposable first Mult2 lifecycle");
    const auto rcb = evaluator.RCB(product);
    const auto result = client.BindFirstMult2Rcb(rcb, left, right);
    CheckBound(result, context, tag, 2, outputRecorded);
    const auto decoded = client.Decrypt(keys.secretKey, left);
    Check(MaximumSlotError(OracleValues(decoded.values), LeftValues()) <= Tolerance(80),
          "disposable pre-drift public decryption");
    Check(ContextObservation(context) == contextBefore && KeyObservation(keys.publicKey) == pkBefore &&
          KeyObservation(keys.secretKey) == skBefore && CiphertextObservation(leftView) == leftBefore &&
          CiphertextObservation(rightView) == rightBefore && CiphertextObservation(left.CloneForEvaluation()) == leftBefore &&
          CiphertextObservation(right.CloneForEvaluation()) == rightBefore, "disposable pre-drift operations changed inputs");
    const auto leftState = left.State(), rightState = right.State(), resultState = result.State();
    const auto sharedQ = cp->GetElementParams();
    const auto qBefore = Basis(sharedQ);
    const BigInt compositeBefore(sharedQ->GetModulus().ToString());
    Check(qBefore.moduliDecimal.size() == 8 && leftView->GetElements().at(0).GetParams().get() == sharedQ.get() &&
          rightView->GetElements().at(0).GetParams().get() == sharedQ.get(), "disposable shared Q fixture identity");
    mapsUnchanged(); originalUnchanged();
    std::cout << "shared_params_fixture ready=1 valid_clone_bind_decrypt=1 actual_first_bits=55"
              << " Q_towers=8 fixture_new_keypairs=1 fixture_eval_keygen_calls=1 crypto_complete_before_drift=1" << std::endl;
    // The sole intentional shared-state mutation; all crypto has finished.
    // The public PopLastParam requires a nonempty basis, proven above.
    sharedQ->PopLastParam();
    auto shortened = qBefore;
    shortened.moduliDecimal.pop_back(); shortened.rootsOfUnityDecimal.pop_back();
    const BigInt compositeAfter = compositeBefore / BigInt(qBefore.moduliDecimal.back());
    Check(SameBasis(Basis(sharedQ), shortened) && sharedQ->GetParams().size() == 7 &&
          BigInt(sharedQ->GetModulus().ToString()) == compositeAfter,
          "shared Q mutation did not produce the exact seven-tower prefix");
    const auto leftAfter = CiphertextObservation(leftView), rightAfter = CiphertextObservation(rightView);
    const auto rcbAfter = CiphertextObservation(rcb);
    const auto pkAfter = KeyObservation(keys.publicKey), skAfter = KeyObservation(keys.secretKey);
    const auto changedBasis = BasisObservation(Basis(sharedQ));
    const auto driftUnchanged = [&] {
        Check(SameState(left.State(), leftState) && SameState(right.State(), rightState) &&
              SameState(result.State(), resultState), "immutable receipt value changed after shared parameter drift");
        Check(BasisObservation(Basis(sharedQ)) == changedBasis &&
              CiphertextObservation(leftView) == leftAfter && CiphertextObservation(rightView) == rightAfter &&
              CiphertextObservation(rcb) == rcbAfter && KeyObservation(keys.publicKey) == pkAfter &&
              KeyObservation(keys.secretKey) == skAfter, "rejected operation further changed disposable fixture");
        mapsUnchanged(); originalUnchanged();
    };
    driftUnchanged();
    std::cout << "shared_params_mutation ready=1 Q_towers_before=8 Q_towers_after=7 immutable_receipt_towers=8"
              << " next_boundary=CloneForEvaluation" << std::endl;
    const std::string diagnostic = "HighPrecisionClientIO: shared context basis changed";
    const auto rejectDrift = [&](const char* boundary, auto call) {
        bool rejected = false;
        try { call(); }
        catch (const std::domain_error& error) {
            Check(error.what() == diagnostic, "wrong shared-Params diagnostic: " + std::string(error.what()));
            rejected = true;
        }
        driftUnchanged();
        Check(rejected, "required shared-Params rejection was accepted at " + std::string(boundary) + ": " + diagnostic);
        std::cout << "shared_params_boundary_rejection passed boundary=" << boundary << std::endl;
    };
    rejectDrift("CloneForEvaluation", [&] { (void)left.CloneForEvaluation(); });
    rejectDrift("BindFirstMult2Rcb", [&] { (void)client.BindFirstMult2Rcb(rcb, left, right); });
    rejectDrift("Decrypt", [&] { (void)client.Decrypt(keys.secretKey, left); });
    ContextImpl::ClearEvalMultKeys(tag);
    Check(ContextImpl::GetAllEvalMultKeys() == multBefore && ContextImpl::GetAllEvalAutomorphismKeys() == autoBefore,
          "owned evaluation-key cleanup changed original maps");
    for (const auto& row : autoRowsBefore)
        Check(*ContextImpl::GetAllEvalAutomorphismKeys().at(row.first) == row.second, "cleanup changed original automorphism row");
    originalUnchanged();
    std::cout << "shared_params_drift_contract passed boundaries=3 owned_eval_tag_cleanup=1" << std::endl;
}

void RunContract() {
    const auto left = LeftValues(), right = RightValues(), products = FrozenExpectedProducts();
    CheckRational(); CheckOracles(left, right, products);
    const auto leftValues = ClientValues(left), rightValues = ClientValues(right);
    const auto context = MakeContext();
    const auto keys = context->KeyGen();
    Check(keys.good() && !keys.publicKey->GetKeyTag().empty() && keys.publicKey->GetKeyTag() == keys.secretKey->GetKeyTag(), "one fresh matching keypair");
    context->EvalMultKeyGen(keys.secretKey);
    const auto cp = std::dynamic_pointer_cast<Params>(context->GetCryptoParameters());
    Check(keys.publicKey->GetPublicElements().size() == 2, "valid public key element count");
    for (const auto& element : keys.publicKey->GetPublicElements()) CheckElement(element, Basis(cp->GetElementParams()));
    CheckElement(keys.secretKey->GetPrivateElement(), Basis(cp->GetElementParams()));
    const auto contextBefore = ContextObservation(context), pkBefore = KeyObservation(keys.publicKey), skBefore = KeyObservation(keys.secretKey);
    const auto& evalKeys = context->GetEvalMultKeyVector(keys.secretKey->GetKeyTag());
    Check(!evalKeys.empty(), "matching evaluation key absent");
    const auto evalKey = std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(evalKeys.front());
    Check(evalKey != nullptr && evalKey->GetCryptoContext().get() == context.get() && evalKey->GetKeyTag() == keys.secretKey->GetKeyTag(), "matching evaluation key identity");
    const auto evalA = evalKey->GetAVector(), evalB = evalKey->GetBVector();
    Check(evalA.size() == cp->GetNumPartQ() && evalB.size() == cp->GetNumPartQ(), "matching HYBRID key partitions");
    for (const auto& element : evalA) CheckElement(element, Basis(cp->GetParamsQP()));
    for (const auto& element : evalB) CheckElement(element, Basis(cp->GetParamsQP()));
    const auto unchanged = [&] {
        Check(ContextObservation(context) == contextBefore, "client/evaluator changed context or tables");
        Check(KeyObservation(keys.publicKey) == pkBefore && KeyObservation(keys.secretKey) == skBefore, "client/evaluator changed valid keys");
        Check(evalKey->GetAVector() == evalA && evalKey->GetBVector() == evalB, "evaluation key changed");
        Check(leftValues.size() == left.size() && rightValues.size() == right.size(), "input vector sizes changed");
        for (std::size_t index = 0; index < left.size(); ++index) {
            Check(leftValues[index].real == left[index].real && leftValues[index].imag == left[index].imag &&
                  rightValues[index].real == right[index].real && rightValues[index].imag == right[index].imag,
                  "input values changed");
        }
    };
    const io::HighPrecisionClientIO client(context);
    unchanged();
    const io::FreshEncodingSpec spec{16, io::PositiveRationalScale::FromPositive(Pow2Integer(100), 1)};
    const auto leftBound = client.Encrypt(keys.publicKey, leftValues, spec);
    unchanged();
    const auto rightBound = client.Encrypt(keys.publicKey, rightValues, spec);
    const double base = cp->GetScalingFactorReal(0), freshRecorded = base * base;
    Check(base == std::ldexp(1.0, 50) && std::isfinite(freshRecorded) && freshRecorded > 0, "fresh physical metadata");
    CheckBound(leftBound, context, keys.publicKey->GetKeyTag(), 0, freshRecorded);
    CheckBound(rightBound, context, keys.publicKey->GetKeyTag(), 0, freshRecorded);
    const auto leftSnapshot = leftBound.CloneForEvaluation(), rightSnapshot = rightBound.CloneForEvaluation();
    const auto receiptsUnchanged = [&] {
        unchanged();
        CheckBound(leftBound, context, keys.publicKey->GetKeyTag(), 0, freshRecorded);
        CheckBound(rightBound, context, keys.publicKey->GetKeyTag(), 0, freshRecorded);
        Check(*leftBound.CloneForEvaluation() == *leftSnapshot && *rightBound.CloneForEvaluation() == *rightSnapshot, "fresh receipt changed");
    };
    const auto leftObservation = Observe(client, keys.secretKey, leftBound, left, {Tolerance(70), Tolerance(73)});
    const auto rightObservation = Observe(client, keys.secretKey, rightBound, right, {0, 0});
    receiptsUnchanged();
    const auto rejected = CheckMalformedKeys(client, keys.publicKey, keys.secretKey, leftValues, spec, leftBound, receiptsUnchanged);

    // The complete evaluator path has no secret parameter or client decryption.
    const DoubleCKKS evaluator(context);
    const auto leftEval = leftBound.CloneForEvaluation(), rightEval = rightBound.CloneForEvaluation();
    const auto leftPair = evaluator.DCP(leftEval), rightPair = evaluator.DCP(rightEval);
    Check(leftPair.GetLifecycle() == openfhe_2023_1788::PairLifecycle::ReadyForFirstMult &&
          rightPair.GetLifecycle() == openfhe_2023_1788::PairLifecycle::ReadyForFirstMult, "DCP lifecycle");
    const auto leftHigh = leftPair.GetHigh()->Clone(), leftLow = leftPair.GetLow()->Clone();
    const auto rightHigh = rightPair.GetHigh()->Clone(), rightLow = rightPair.GetLow()->Clone();
    const auto product = evaluator.Mult2(leftPair, rightPair);
    Check(product.GetLifecycle() == openfhe_2023_1788::PairLifecycle::RefreshRequired, "first Mult2 lifecycle");
    Check(*leftPair.GetHigh() == *leftHigh && *leftPair.GetLow() == *leftLow &&
          *rightPair.GetHigh() == *rightHigh && *rightPair.GetLow() == *rightLow, "evaluator changed input pairs");
    const auto productHigh = product.GetHigh()->Clone(), productLow = product.GetLow()->Clone();
    const auto rcb = evaluator.RCB(product);
    Check(*product.GetHigh() == *productHigh && *product.GetLow() == *productLow, "RCB changed result pair");
    const auto rcbSnapshot = rcb->Clone();
    const auto resultBound = client.BindFirstMult2Rcb(rcb, leftBound, rightBound);
    Check(*rcb == *rcbSnapshot, "binding changed immediate RCB input");
    const double ready = freshRecorded * freshRecorded / base;
    const double divisor = cp->GetModReduceFactor(cp->GetElementParams()->GetParams().size() - 2);
    const double recorded = ready / divisor;
    Check(std::isfinite(ready) && ready > 0 && std::isfinite(divisor) && divisor > 0 && std::isfinite(recorded) && recorded > 0,
          "physical factor operation-order intermediates");
    CheckBound(resultBound, context, keys.publicKey->GetKeyTag(), 2, recorded);
    const auto resultSnapshot = resultBound.CloneForEvaluation();
    const auto output = Observe(client, keys.secretKey, resultBound, products, FrozenProductDelta());
    receiptsUnchanged();
    CheckBound(resultBound, context, keys.publicKey->GetKeyTag(), 2, recorded);
    Check(*resultBound.CloneForEvaluation() == *resultSnapshot, "output receipt changed during Decrypt");
    // Observe's local ciphertext and all raw coefficient buffers are dead now.
    Check(MaximumSlotError(OracleValues(leftObservation.decoded.values), left) <= Tolerance(80) &&
          MaximumSlotError(OracleValues(rightObservation.decoded.values), right) <= Tolerance(80) &&
          MaximumSlotError(OracleValues(output.decoded.values), products) <= Tolerance(80), "decoded values do not own their lifetime");

    std::cout << std::setprecision(45) << "test=" << kTestName << " result=PASS source=" << LOSSLESS_IO_SOURCE_COMMIT
              << " openfhe_pin=" << kPin << " claim_scope=low-N-first-operation-production-io-diagnostic"
              << " N=64 S=16 gap=2 native=64 backend=4 depth=7 scaling_bits=50 first_bits=55"
              << " key_switch=HYBRID scaling=FIXEDMANUAL data=COMPLEX execution=EXEC_EVALUATION decrypt_noise=FIXED_NOISE_DECRYPT"
              << " encryption=STANDARD multiplication=HPS PRE=NOT_SET noise_scale=1 secret=UNIFORM_TERNARY security=HEStd_NotSet"
              << " fresh_keypairs=1 eval_keygen_calls=1 q_div=1125899906843009 q_l=1125899906840833"
              << " exact_scale_numerator=" << resultBound.State().logicalScale.Numerator()
              << " exact_scale_denominator=" << resultBound.State().logicalScale.Denominator()
              << " max_fresh_public_error=" << std::max(leftObservation.publicError, rightObservation.publicError)
              << " max_fresh_oracle_error=" << std::max(leftObservation.oracleError, rightObservation.oracleError)
              << " max_product_public_error=" << output.publicError << " max_product_oracle_error=" << output.oracleError
              << " input_public_delta_error=" << leftObservation.publicDeltaError << " input_oracle_delta_error=" << leftObservation.oracleDeltaError
              << " product_public_delta_error=" << output.publicDeltaError << " product_oracle_delta_error=" << output.oracleDeltaError
              << " max_horner_component_disagreement=" << std::max({leftObservation.componentDisagreement, rightObservation.componentDisagreement, output.componentDisagreement})
              << " cross_precision_error=" << std::max({leftObservation.decoded.diagnostics.maximumCrossPrecisionDisagreement,
                    rightObservation.decoded.diagnostics.maximumCrossPrecisionDisagreement, output.decoded.diagnostics.maximumCrossPrecisionDisagreement})
              << " centered_headroom=" << output.decoded.diagnostics.centeredHeadroom << " malformed_key_rejections=" << rejected << std::endl;
    // Keep the original contexts, keys, receipts and decoded results alive
    // through the last disposable fixture. The existing firstMod56 control
    // still executes exactly once and must pass before any shared-state drift.
    CheckUnsupportedFirstModulus();
    CheckCloneIsolation(leftBound, context, keys.publicKey->GetKeyTag(), 0, freshRecorded);
    CheckCloneIsolation(resultBound, context, keys.publicKey->GetKeyTag(), 2, recorded);
    const auto originalUnchanged = [&] {
        receiptsUnchanged();
        CheckBound(resultBound, context, keys.publicKey->GetKeyTag(), 2, recorded);
        Check(*resultBound.CloneForEvaluation() == *resultSnapshot, "original result changed during clone/drift checks");
    };
    originalUnchanged();
    std::cout << "clone_isolation_contract passed fresh_and_result=1 coefficients=1 scalars=1 present_empty_maps=1" << std::endl;
    CheckSharedParamsDrift(context, freshRecorded, recorded, originalUnchanged);
}
}  // namespace

int main() { RunContract(); }
