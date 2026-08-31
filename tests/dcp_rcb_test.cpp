#include "openfhe.h"
#include "openfhe_2023_1788/double_ckks.h"

#include <boost/multiprecision/cpp_int.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using BigInt = boost::multiprecision::cpp_int;
using lbcrypto::Ciphertext;
using lbcrypto::CryptoContext;
using lbcrypto::DCRTPoly;
using lbcrypto::NativeInteger;
using openfhe_2023_1788::DoubleCKKS;
using openfhe_2023_1788::PaperScaleDescriptor;
using openfhe_2023_1788::PairLifecycle;

class StrippedDcpPrecomputationParameters final : public lbcrypto::CryptoParametersCKKSRNS {
public:
    explicit StrippedDcpPrecomputationParameters(const lbcrypto::CryptoParametersCKKSRNS& source)
        : lbcrypto::CryptoParametersCKKSRNS(source) {
        m_approxSF = source.GetScalingFactorReal(0);
        m_QlQlInvModqlDivqlModq.clear();
        m_qlInvModq.clear();
    }
};

class TestFailure final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void Check(bool condition, const std::string& message) {
    if (!condition) {
        throw TestFailure(message);
    }
}

template <class Function>
void CheckThrowsInvalidArgument(Function&& function, const std::string& expectedMessage,
                                const std::string& label) {
    bool threw = false;
    try {
        std::invoke(std::forward<Function>(function));
    }
    catch (const std::invalid_argument& exception) {
        const std::string message(exception.what());
        Check(message.rfind("DoubleCKKS: ", 0) == 0, label + " did not fail in the DoubleCKKS module");
        Check(message.find(expectedMessage) != std::string::npos,
              label + " reported an unexpected diagnostic: " + message);
        threw = true;
    }
    catch (const std::exception& exception) {
        throw TestFailure(label + " threw the wrong exception type: " + exception.what());
    }
    Check(threw, label + " did not fail fast");
}

BigInt PositiveMod(BigInt value, const BigInt& modulus) {
    value %= modulus;
    if (value < 0) {
        value += modulus;
    }
    return value;
}

BigInt ExtendedGcd(BigInt a, BigInt b, BigInt& x, BigInt& y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    BigInt nextX;
    BigInt nextY;
    const BigInt gcd = ExtendedGcd(b, a % b, nextX, nextY);
    x                = nextY;
    y                = nextX - (a / b) * nextY;
    return gcd;
}

BigInt ModInverse(const BigInt& value, const BigInt& modulus) {
    BigInt x;
    BigInt y;
    const BigInt gcd = ExtendedGcd(PositiveMod(value, modulus), modulus, x, y);
    Check(gcd == 1, "CRT oracle received non-coprime moduli");
    return PositiveMod(x, modulus);
}

BigInt Product(const std::vector<BigInt>& values) {
    BigInt product = 1;
    for (const auto& value : values) {
        product *= value;
    }
    return product;
}

BigInt Center(const BigInt& residue, const BigInt& modulus) {
    BigInt centered = PositiveMod(residue, modulus);
    if (centered > modulus / 2) {
        centered -= modulus;
    }
    return centered;
}

BigInt ReconstructCentered(const std::vector<BigInt>& residues, const std::vector<BigInt>& moduli) {
    Check(!moduli.empty(), "CRT oracle requires at least one modulus");
    Check(residues.size() == moduli.size(), "CRT oracle residue/modulus size mismatch");

    const BigInt modulus = Product(moduli);
    BigInt result        = 0;
    for (std::size_t i = 0; i < moduli.size(); ++i) {
        const BigInt partial = modulus / moduli[i];
        result += PositiveMod(residues[i], moduli[i]) * partial * ModInverse(partial, moduli[i]);
    }
    return Center(result, modulus);
}

std::pair<BigInt, BigInt> DecomposeCentered(const BigInt& value, const BigInt& divisor) {
    BigInt remainder = PositiveMod(value, divisor);
    if (remainder > divisor / 2) {
        remainder -= divisor;
    }
    const BigInt quotient = (value - remainder) / divisor;
    Check(value == divisor * quotient + remainder, "independent DCP oracle identity failed");
    return {quotient, remainder};
}

std::vector<BigInt> GetModuli(const DCRTPoly& polynomial) {
    std::vector<BigInt> result;
    result.reserve(polynomial.GetAllElements().size());
    for (const auto& tower : polynomial.GetAllElements()) {
        result.emplace_back(tower.GetModulus().ConvertToInt());
    }
    return result;
}

std::vector<NativeInteger> GetNativeModuli(const DCRTPoly& polynomial) {
    std::vector<NativeInteger> result;
    result.reserve(polynomial.GetAllElements().size());
    for (const auto& tower : polynomial.GetAllElements()) {
        result.push_back(tower.GetModulus());
    }
    return result;
}

DCRTPoly ToCoefficient(const DCRTPoly& polynomial) {
    DCRTPoly result(polynomial);
    result.SetFormat(Format::COEFFICIENT);
    return result;
}

BigInt CoefficientResidue(const DCRTPoly& coefficientPolynomial, std::size_t tower, std::size_t coefficient) {
    return BigInt(coefficientPolynomial.GetAllElements().at(tower).GetValues().at(coefficient).ConvertToInt());
}

BigInt ReconstructCoefficient(const DCRTPoly& polynomial, std::size_t coefficient) {
    const auto coefficientPolynomial = ToCoefficient(polynomial);
    const auto moduli                = GetModuli(coefficientPolynomial);
    std::vector<BigInt> residues;
    residues.reserve(moduli.size());
    for (std::size_t tower = 0; tower < moduli.size(); ++tower) {
        residues.push_back(CoefficientResidue(coefficientPolynomial, tower, coefficient));
    }
    return ReconstructCentered(residues, moduli);
}

DCRTPoly MakePolynomial(const std::shared_ptr<DCRTPoly::Params>& params, const std::vector<BigInt>& coefficients) {
    DCRTPoly result(params, Format::COEFFICIENT, true);
    auto& towers = result.GetAllElements();
    const std::size_t ringDimension = params->GetRingDimension();
    Check(coefficients.size() == ringDimension, "test coefficient vector has wrong ring dimension");

    for (std::size_t towerIndex = 0; towerIndex < towers.size(); ++towerIndex) {
        const auto modulusNative = towers[towerIndex].GetModulus();
        const BigInt modulus(modulusNative.ConvertToInt());
        lbcrypto::NativeVector residues(ringDimension, modulusNative);
        for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
            const auto residue = PositiveMod(coefficients[coefficient], modulus).convert_to<std::uint64_t>();
            residues[coefficient] = NativeInteger(residue);
        }
        towers[towerIndex].SetValues(std::move(residues), Format::COEFFICIENT);
    }
    result.SetFormat(Format::EVALUATION);
    return result;
}

CryptoContext<DCRTPoly> MakeContext(lbcrypto::ScalingTechnique scalingTechnique = lbcrypto::FIXEDMANUAL,
                                    std::uint32_t firstModSize = 35,
                                    std::uint32_t multiplicativeDepth = 3) {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multiplicativeDepth);
    parameters.SetScalingModSize(30);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetScalingTechnique(scalingTechnique);
    parameters.SetSecurityLevel(lbcrypto::HEStd_NotSet);
    parameters.SetRingDim(32);
    parameters.SetBatchSize(8);

    auto context = lbcrypto::GenCryptoContext(parameters);
    context->Enable(lbcrypto::PKE);
    context->Enable(lbcrypto::KEYSWITCH);
    context->Enable(lbcrypto::LEVELEDSHE);
    return context;
}

std::vector<BigInt> BoundaryCoefficients(const std::vector<BigInt>& moduli, std::size_t ringDimension) {
    const BigInt fullModulus = Product(moduli);
    const BigInt divisor     = moduli.back();
    const BigInt halfDivisor = divisor / 2;
    const BigInt halfFull    = fullModulus / 2;

    std::vector<BigInt> values = {
        0,
        1,
        -1,
        halfDivisor,
        -halfDivisor,
        halfDivisor + 1,
        -(halfDivisor + 1),
        halfDivisor - 1,
        -(halfDivisor - 1),
        halfDivisor + 2,
        -(halfDivisor + 2),
        halfFull,
        halfFull - 1,
        halfFull - 2,
        -halfFull,
        -halfFull + 1,
        -halfFull + 2,
        divisor,
        -divisor,
        divisor + 1,
        -(divisor + 1),
    };
    values.resize(ringDimension, 0);
    return values;
}

struct Fixture {
    CryptoContext<DCRTPoly> context;
    lbcrypto::KeyPair<DCRTPoly> keys;
    Ciphertext<DCRTPoly> input;
    std::vector<BigInt> firstCoefficients;
    std::vector<BigInt> secondCoefficients;
};

Fixture MakeFixture(std::uint32_t firstModSize = 35, std::uint32_t multiplicativeDepth = 3) {
    Fixture fixture;
    fixture.context = MakeContext(lbcrypto::FIXEDMANUAL, firstModSize, multiplicativeDepth);
    fixture.keys    = fixture.context->KeyGen();

    auto plaintext = fixture.context->MakeCKKSPackedPlaintext(std::vector<double>{0.0}, 2, 0);
    fixture.input   = fixture.context->Encrypt(plaintext, fixture.keys.publicKey);

    const auto params = fixture.input->GetElements().front().GetParams();
    const auto moduli = GetModuli(fixture.input->GetElements().front());
    fixture.firstCoefficients = BoundaryCoefficients(moduli, params->GetRingDimension());
    fixture.secondCoefficients = fixture.firstCoefficients;
    std::reverse(fixture.secondCoefficients.begin(), fixture.secondCoefficients.end());
    for (std::size_t index = 1; index < fixture.secondCoefficients.size(); index += 2) {
        fixture.secondCoefficients[index] = -fixture.secondCoefficients[index];
    }

    fixture.input->SetElements({MakePolynomial(params, fixture.firstCoefficients),
                                MakePolynomial(params, fixture.secondCoefficients)});
    Check(fixture.input->GetLevel() == 0, "fixture must be level zero");
    Check(fixture.input->GetNoiseScaleDeg() == 2, "fixture must encode at double scale");
    return fixture;
}

void CheckOrderedModuli(const std::vector<NativeInteger>& actual, const std::vector<NativeInteger>& expected,
                        const std::string& label) {
    Check(actual.size() == expected.size(), label + " modulus count mismatch");
    for (std::size_t index = 0; index < expected.size(); ++index) {
        Check(actual[index] == expected[index], label + " ordered modulus mismatch at tower " +
                                                    std::to_string(index));
    }
}

void CheckCiphertextMetadata(lbcrypto::ConstCiphertext<DCRTPoly> ciphertext,
                             const std::vector<NativeInteger>& expectedModuli, std::size_t expectedLevel,
                             std::size_t expectedNoiseScaleDegree, double expectedScalingFactor,
                             const std::string& expectedKeyTag, const std::string& label) {
    Check(ciphertext != nullptr, label + " is null");
    Check(ciphertext->NumberCiphertextElements() == 2, label + " component count mismatch");
    Check(ciphertext->GetLevel() == expectedLevel, label + " level mismatch");
    Check(ciphertext->GetNoiseScaleDeg() == expectedNoiseScaleDegree, label + " noise-scale degree mismatch");
    Check(ciphertext->GetScalingFactor() == expectedScalingFactor, label + " recorded scaling factor mismatch");
    Check(ciphertext->GetKeyTag() == expectedKeyTag, label + " key tag mismatch");
    for (const auto& element : ciphertext->GetElements()) {
        Check(element.GetFormat() == Format::EVALUATION, label + " is not in evaluation format");
        CheckOrderedModuli(GetNativeModuli(element), expectedModuli, label);
    }
}

void CheckDcpCoefficients(const DCRTPoly& input, const DCRTPoly& high, const DCRTPoly& low,
                          const BigInt& divisor) {
    const auto highCoefficient = ToCoefficient(high);
    const auto lowCoefficient  = ToCoefficient(low);
    const auto prefixModuli    = GetModuli(highCoefficient);
    const std::size_t ringDimension = input.GetParams()->GetRingDimension();

    Check(highCoefficient.GetAllElements().size() + 1 == input.GetAllElements().size(),
          "DCP high did not consume exactly the divisor tower");
    Check(lowCoefficient.GetAllElements().size() == highCoefficient.GetAllElements().size(),
          "DCP pair bases differ");

    for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
        const BigInt source = ReconstructCoefficient(input, coefficient);
        const auto expected = DecomposeCentered(source, divisor);

        std::vector<BigInt> highResidues;
        std::vector<BigInt> lowResidues;
        for (std::size_t tower = 0; tower < prefixModuli.size(); ++tower) {
            const BigInt actualHigh = CoefficientResidue(highCoefficient, tower, coefficient);
            const BigInt actualLow  = CoefficientResidue(lowCoefficient, tower, coefficient);
            Check(actualHigh == PositiveMod(expected.first, prefixModuli[tower]),
                  "DCP quotient mismatch at coefficient " + std::to_string(coefficient) + ", tower " +
                      std::to_string(tower));
            Check(actualLow == PositiveMod(expected.second, prefixModuli[tower]),
                  "DCP remainder mismatch at coefficient " + std::to_string(coefficient) + ", tower " +
                      std::to_string(tower));
            highResidues.push_back(actualHigh);
            lowResidues.push_back(actualLow);
        }

        const BigInt reconstructedHigh = ReconstructCentered(highResidues, prefixModuli);
        const BigInt reconstructedLow  = ReconstructCentered(lowResidues, prefixModuli);
        Check(source == divisor * reconstructedHigh + reconstructedLow,
              "DCP exact integer identity mismatch at coefficient " + std::to_string(coefficient));
    }
}

void CheckRcbCoefficients(const DCRTPoly& input, const DCRTPoly& high, const DCRTPoly& low,
                          const DCRTPoly& recombined, const BigInt& divisor) {
    const auto highCoefficient       = ToCoefficient(high);
    const auto lowCoefficient        = ToCoefficient(low);
    const auto recombinedCoefficient = ToCoefficient(recombined);
    const auto prefixModuli          = GetModuli(recombinedCoefficient);
    const std::size_t ringDimension  = input.GetParams()->GetRingDimension();

    for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
        const BigInt source = ReconstructCoefficient(input, coefficient);
        const auto expected = DecomposeCentered(source, divisor);
        for (std::size_t tower = 0; tower < prefixModuli.size(); ++tower) {
            const BigInt actual = CoefficientResidue(recombinedCoefficient, tower, coefficient);
            const BigInt expectedResidue = PositiveMod(divisor * expected.first + expected.second,
                                                       prefixModuli[tower]);
            Check(actual == expectedResidue,
                  "RCB mismatch at coefficient " + std::to_string(coefficient) + ", tower " +
                      std::to_string(tower));

            const BigInt highResidue = CoefficientResidue(highCoefficient, tower, coefficient);
            const BigInt lowResidue  = CoefficientResidue(lowCoefficient, tower, coefficient);
            Check(actual == PositiveMod(divisor * highResidue + lowResidue, prefixModuli[tower]),
                  "RCB did not equal q_div*high+low at coefficient " + std::to_string(coefficient) +
                      ", tower " + std::to_string(tower));
        }
    }
}

void TestDcpAndRcbExactOracle() {
    auto fixture = MakeFixture();
    DoubleCKKS module(fixture.context);

    const auto inputBefore = fixture.input->Clone();
    const auto fullModuli  = GetNativeModuli(fixture.input->GetElements().front());
    const auto divisor     = fullModuli.back();
    const BigInt divisorBig(divisor.ConvertToInt());
    const double recordedScale = fixture.input->GetScalingFactor();
    const std::string keyTag   = fixture.input->GetKeyTag();

    const auto pair = module.DCP(fixture.input);
    Check(pair.GetLifecycle() == PairLifecycle::ReadyForFirstMult, "DCP lifecycle mismatch");
    Check(pair.GetContextIdentity() == fixture.context.get(), "DCP context identity mismatch");
    Check(pair.GetDivisor() == divisor, "DCP divisor mismatch");
    Check(pair.GetLevel() == 1, "DCP pair level mismatch");
    Check(pair.GetNoiseScaleDegree() == 2, "DCP pair noise-scale degree mismatch");
    Check(pair.GetRecordedScalingFactor() == recordedScale, "DCP pair recorded scaling factor mismatch");
    const auto& paperScale = pair.GetPaperScale();
    Check(paperScale.inputRecordedScalingFactor == recordedScale, "DCP paper input scale mismatch");
    Check(paperScale.divisor == divisor, "DCP paper divisor mismatch");
    Check(paperScale.approximateLogicalScalingFactor ==
              static_cast<long double>(recordedScale) / divisorBig.convert_to<long double>(),
          "DCP paper logical scale mismatch");
    Check(divisor.Mod(NativeInteger(2)) == NativeInteger(1), "DCP divisor must be odd");
    Check(pair.GetKeyTag() == keyTag, "DCP pair key tag mismatch");
    Check(pair.GetFormat() == Format::EVALUATION, "DCP pair format mismatch");
    Check(pair.GetComponentCount() == 2, "DCP pair component count mismatch");

    std::vector<NativeInteger> prefixModuli(fullModuli.begin(), fullModuli.end() - 1);
    CheckOrderedModuli(pair.GetOrderedModuli(), prefixModuli, "DCP pair");
    CheckCiphertextMetadata(pair.GetHigh(), prefixModuli, 1, 2, recordedScale, keyTag, "DCP high");
    CheckCiphertextMetadata(pair.GetLow(), prefixModuli, 1, 2, recordedScale, keyTag, "DCP low");

    for (std::size_t component = 0; component < fixture.input->GetElements().size(); ++component) {
        CheckDcpCoefficients(fixture.input->GetElements()[component], pair.GetHigh()->GetElements()[component],
                             pair.GetLow()->GetElements()[component], divisorBig);
    }

    Check(fixture.input->GetLevel() == inputBefore->GetLevel(), "DCP mutated input level");
    Check(fixture.input->GetElements() == inputBefore->GetElements(), "DCP mutated input elements");

    const auto recombined = module.RCB(pair);
    CheckCiphertextMetadata(recombined, prefixModuli, 1, 2, recordedScale, keyTag, "RCB result");
    for (std::size_t component = 0; component < fixture.input->GetElements().size(); ++component) {
        CheckRcbCoefficients(fixture.input->GetElements()[component], pair.GetHigh()->GetElements()[component],
                             pair.GetLow()->GetElements()[component], recombined->GetElements()[component],
                             divisorBig);
    }
}

void TestValidationBeforeRawAccess() {
    auto fixture = MakeFixture();
    DoubleCKKS module(fixture.context);

    lbcrypto::ConstCiphertext<DCRTPoly> nullCiphertext{};
    CheckThrowsInvalidArgument([&] { module.DCP(nullCiphertext); }, "DCP input is null", "null ciphertext");

    auto wrongLevel = fixture.input->Clone();
    wrongLevel->SetLevel(1);
    CheckThrowsInvalidArgument([&] { module.DCP(wrongLevel); }, "DCP input must be at level zero", "wrong level");

    auto wrongDegree = fixture.input->Clone();
    wrongDegree->SetNoiseScaleDeg(1);
    CheckThrowsInvalidArgument([&] { module.DCP(wrongDegree); }, "noise-scale degree",
                               "wrong noise-scale degree");

    auto nonFiniteScale = fixture.input->Clone();
    nonFiniteScale->SetScalingFactor(std::numeric_limits<double>::quiet_NaN());
    CheckThrowsInvalidArgument([&] { module.DCP(nonFiniteScale); },
                               "exact fresh degree-two FIXEDMANUAL scaling factor",
                               "non-finite scaling factor");

    auto wrongFiniteScale = fixture.input->Clone();
    wrongFiniteScale->SetScalingFactor(fixture.input->GetScalingFactor() * 2.0);
    CheckThrowsInvalidArgument([&] { module.DCP(wrongFiniteScale); },
                               "exact fresh degree-two FIXEDMANUAL scaling factor",
                               "wrong finite scaling factor");

    auto wrongComponents = fixture.input->Clone();
    auto componentVector = wrongComponents->GetElements();
    componentVector.push_back(componentVector.front());
    wrongComponents->SetElements(std::move(componentVector));
    CheckThrowsInvalidArgument([&] { module.DCP(wrongComponents); }, "exactly two RLWE components",
                               "wrong component count");

    auto coefficientFormat = fixture.input->Clone();
    auto coefficientElements = coefficientFormat->GetElements();
    for (auto& element : coefficientElements) {
        element.SetFormat(Format::COEFFICIENT);
    }
    coefficientFormat->SetElements(std::move(coefficientElements));
    CheckThrowsInvalidArgument([&] { module.DCP(coefficientFormat); }, "evaluation format",
                               "coefficient-format input");

    auto reorderedBasis = fixture.input->Clone();
    auto reorderedElements = reorderedBasis->GetElements();
    for (auto& element : reorderedElements) {
        auto towers = element.GetAllElements();
        std::swap(towers[towers.size() - 1], towers[towers.size() - 2]);
        element = DCRTPoly(towers);
    }
    reorderedBasis->SetElements(std::move(reorderedElements));
    CheckThrowsInvalidArgument([&] { module.DCP(reorderedBasis); }, "ordered RNS basis mismatch",
                               "reordered basis");

    auto otherFixture = MakeFixture(36);
    DoubleCKKS otherModule(otherFixture.context);
    CheckThrowsInvalidArgument([&] { module.DCP(otherFixture.input); }, "belongs to a different context",
                               "mismatched context");
    const auto otherPair = otherModule.DCP(otherFixture.input);
    CheckThrowsInvalidArgument([&] { module.RCB(otherPair); }, "pair belongs to a different context",
                               "mismatched pair context");

    auto automaticContext = MakeContext(lbcrypto::FIXEDAUTO, 35);
    CheckThrowsInvalidArgument([&] { DoubleCKKS invalidModule(automaticContext); },
                               "only FIXEDMANUAL scaling is supported", "non-FIXEDMANUAL context");
}

void TestMinimumFirstMultBasis() {
    auto fixture = MakeFixture(35, 2);
    const auto fullModuli = GetNativeModuli(fixture.input->GetElements().front());
    Check(fullModuli.size() == 3, "minimum first-Mult2 fixture must have exactly three towers");

    DoubleCKKS module(fixture.context);
    const auto pair       = module.DCP(fixture.input);
    const auto recombined = module.RCB(pair);
    const BigInt divisor(fullModuli.back().ConvertToInt());

    Check(pair.GetOrderedModuli().size() == 2,
          "minimum first-Mult2 DCP must retain q0 and q_l before RS2");
    for (std::size_t component = 0; component < fixture.input->GetElements().size(); ++component) {
        CheckDcpCoefficients(fixture.input->GetElements()[component], pair.GetHigh()->GetElements()[component],
                             pair.GetLow()->GetElements()[component], divisor);
        CheckRcbCoefficients(fixture.input->GetElements()[component], pair.GetHigh()->GetElements()[component],
                             pair.GetLow()->GetElements()[component], recombined->GetElements()[component], divisor);
    }
}

void TestDcpDoesNotIndexUncheckedPrecomputationRows() {
    auto fixture = MakeFixture();
    const auto generatedParameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(fixture.context->GetCryptoParameters());
    Check(generatedParameters != nullptr, "fixture must expose CKKS-RNS parameters");

    auto strippedParameters = std::make_shared<StrippedDcpPrecomputationParameters>(*generatedParameters);
    CryptoContext<DCRTPoly> strippedContext = std::make_shared<lbcrypto::CryptoContextImpl<DCRTPoly>>(
        strippedParameters, fixture.context->GetScheme(), fixture.context->getSchemeId());
    auto strippedInput = std::make_shared<lbcrypto::CiphertextImpl<DCRTPoly>>(
        strippedContext, fixture.input->GetKeyTag(), fixture.input->GetEncodingType());
    strippedInput->SetElements(fixture.input->GetElements());
    strippedInput->SetLevel(fixture.input->GetLevel());
    strippedInput->SetNoiseScaleDeg(fixture.input->GetNoiseScaleDeg());
    strippedInput->SetScalingFactor(fixture.input->GetScalingFactor());

    DoubleCKKS module(strippedContext);
    const auto pair       = module.DCP(strippedInput);
    const auto recombined = module.RCB(pair);
    const auto fullModuli = GetNativeModuli(strippedInput->GetElements().front());
    const BigInt divisor(fullModuli.back().ConvertToInt());

    for (std::size_t component = 0; component < strippedInput->GetElements().size(); ++component) {
        CheckDcpCoefficients(strippedInput->GetElements()[component], pair.GetHigh()->GetElements()[component],
                             pair.GetLow()->GetElements()[component], divisor);
        CheckRcbCoefficients(strippedInput->GetElements()[component], pair.GetHigh()->GetElements()[component],
                             pair.GetLow()->GetElements()[component], recombined->GetElements()[component], divisor);
    }
}

void TestRcbRejectsTamperedPairStorage() {
    auto fixture = MakeFixture();
    DoubleCKKS module(fixture.context);

    {
        auto pair = module.DCP(fixture.input);
        auto high = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetHigh());
        high->SetKeyTag("tampered-key-tag");
        CheckThrowsInvalidArgument([&] { module.RCB(pair); }, "key tag", "tampered pair key tag");
    }

    {
        auto pair = module.DCP(fixture.input);
        auto high = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetHigh());
        high->GetElements().push_back(high->GetElements().front());
        CheckThrowsInvalidArgument([&] { module.RCB(pair); }, "exactly two RLWE components",
                                   "tampered pair component count");
    }

    {
        auto pair = module.DCP(fixture.input);
        auto low  = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetLow());
        auto elements = low->GetElements();
        for (auto& element : elements) {
            auto towers = element.GetAllElements();
            std::swap(towers[0], towers[1]);
            element = DCRTPoly(towers);
        }
        low->SetElements(std::move(elements));
        CheckThrowsInvalidArgument([&] { module.RCB(pair); }, "ordered RNS basis mismatch",
                                   "tampered pair basis order");
    }

    {
        auto pair = module.DCP(fixture.input);
        auto low  = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetLow());
        for (auto& element : low->GetElements()) {
            element.SetFormat(Format::COEFFICIENT);
        }
        CheckThrowsInvalidArgument([&] { module.RCB(pair); }, "evaluation format", "tampered pair format");
    }

    {
        auto pair = module.DCP(fixture.input);
        auto low  = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetLow());
        low->SetScalingFactor(low->GetScalingFactor() * 2.0);
        CheckThrowsInvalidArgument([&] { module.RCB(pair); }, "recorded scaling factor",
                                   "tampered pair scaling factor");
    }

    {
        auto pair = module.DCP(fixture.input);
        auto high = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetHigh());
        high->SetLevel(0);
        CheckThrowsInvalidArgument([&] { module.RCB(pair); }, "level", "tampered pair level");
    }

    {
        auto pair = module.DCP(fixture.input);
        auto low  = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetLow());
        low->SetNoiseScaleDeg(1);
        CheckThrowsInvalidArgument([&] { module.RCB(pair); }, "noise-scale degree",
                                   "tampered pair noise-scale degree");
    }

    {
        auto pair = module.DCP(fixture.input);
        auto& paperScale = const_cast<PaperScaleDescriptor&>(pair.GetPaperScale());
        paperScale.approximateLogicalScalingFactor *= 2.0L;
        CheckThrowsInvalidArgument([&] { module.RCB(pair); }, "paper-scale descriptor",
                                   "tampered pair paper scale");
    }
}

}  // namespace

int main() {
    try {
        TestDcpAndRcbExactOracle();
        TestValidationBeforeRawAccess();
        TestMinimumFirstMultBasis();
        TestDcpDoesNotIndexUncheckedPrecomputationRows();
        TestRcbRejectsTamperedPairStorage();
        lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
        std::cout << "DCP/RCB independent-oracle tests passed\n";
        return 0;
    }
    catch (const TestFailure& failure) {
        std::cerr << "DCP/RCB test failure: " << failure.what() << '\n';
        return 1;
    }
}
