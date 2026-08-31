#include "openfhe.h"
#include "openfhe_2023_1788/double_ckks.h"

#include <boost/multiprecision/cpp_int.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

using BigInt = boost::multiprecision::cpp_int;
using lbcrypto::Ciphertext;
using lbcrypto::CryptoContext;
using lbcrypto::DCRTPoly;
using lbcrypto::NativeInteger;
using openfhe_2023_1788::CiphertextPair;
using openfhe_2023_1788::DoubleCKKS;
using openfhe_2023_1788::PaperScaleDescriptor;
using openfhe_2023_1788::TensorCiphertextPair;

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

class ImmutabilityProbeMetadata final : public lbcrypto::Metadata {
public:
    explicit ImmutabilityProbeMetadata(std::string value) : value_(std::move(value)) {}

    std::shared_ptr<lbcrypto::Metadata> Clone() const override {
        return std::make_shared<ImmutabilityProbeMetadata>(value_);
    }

    bool operator==(const lbcrypto::Metadata& metadata) const override {
        const auto* other = dynamic_cast<const ImmutabilityProbeMetadata*>(&metadata);
        return other != nullptr && value_ == other->value_;
    }

private:
    std::string value_;
};

struct MetadataSnapshotEntry {
    std::string key;
    std::shared_ptr<lbcrypto::Metadata> value;
};

using MetadataSnapshot = std::vector<MetadataSnapshotEntry>;

MetadataSnapshot SnapshotMetadata(lbcrypto::ConstCiphertext<DCRTPoly> ciphertext, const std::string& label) {
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");

    MetadataSnapshot snapshot;
    snapshot.reserve(metadata->size());
    for (const auto& [key, value] : *metadata) {
        Check(value != nullptr, label + " metadata value is null");
        auto cloned = value->Clone();
        Check(cloned != nullptr, label + " metadata clone is null");
        snapshot.push_back({key, std::move(cloned)});
    }
    return snapshot;
}

void CheckMetadataUnchanged(lbcrypto::ConstCiphertext<DCRTPoly> ciphertext, const MetadataSnapshot& before,
                            const std::string& label) {
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");
    Check(metadata->size() == before.size(), label + " metadata map size changed");

    auto current = metadata->begin();
    for (const auto& expected : before) {
        Check(current != metadata->end(), label + " metadata entry disappeared");
        Check(current->first == expected.key, label + " metadata key changed");
        Check(current->second != nullptr, label + " metadata value became null");
        Check(*(current->second) == *(expected.value), label + " metadata value changed");
        ++current;
    }
}

BigInt PositiveMod(BigInt value, const BigInt& modulus) {
    value %= modulus;
    if (value < 0) {
        value += modulus;
    }
    return value;
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

CryptoContext<DCRTPoly> MakeContext() {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(3);
    parameters.SetScalingModSize(30);
    parameters.SetFirstModSize(35);
    parameters.SetScalingTechnique(lbcrypto::FIXEDMANUAL);
    parameters.SetSecurityLevel(lbcrypto::HEStd_NotSet);
    parameters.SetRingDim(32);
    parameters.SetBatchSize(8);

    auto context = lbcrypto::GenCryptoContext(parameters);
    context->Enable(lbcrypto::PKE);
    context->Enable(lbcrypto::KEYSWITCH);
    context->Enable(lbcrypto::LEVELEDSHE);
    return context;
}

std::vector<BigInt> ComposeSource(const BigInt& divisor, const std::vector<BigInt>& high,
                                  const std::vector<BigInt>& low) {
    Check(high.size() == low.size(), "source high/low fixture size mismatch");
    std::vector<BigInt> source(high.size());
    for (std::size_t i = 0; i < high.size(); ++i) {
        source[i] = divisor * high[i] + low[i];
    }
    return source;
}

struct TensorFixture {
    CryptoContext<DCRTPoly> context;
    lbcrypto::KeyPair<DCRTPoly> keys;
    Ciphertext<DCRTPoly> leftInput;
    Ciphertext<DCRTPoly> rightInput;
};

TensorFixture MakeTensorFixture() {
    TensorFixture fixture;
    fixture.context = MakeContext();
    fixture.keys = fixture.context->KeyGen();

    auto plaintext = fixture.context->MakeCKKSPackedPlaintext(std::vector<double>{0.0}, 2, 0);
    fixture.leftInput = fixture.context->Encrypt(plaintext, fixture.keys.publicKey);
    fixture.rightInput = fixture.context->Encrypt(plaintext, fixture.keys.publicKey);

    const auto params = fixture.leftInput->GetElements().front().GetParams();
    const std::size_t n = params->GetRingDimension();
    const auto fullModuli = GetModuli(fixture.leftInput->GetElements().front());
    Check(fullModuli.size() >= 3, "Tensor2 fixture requires at least three full-basis towers");
    const BigInt divisor = fullModuli.back();

    std::vector<BigInt> leftHigh0(n, 0);
    std::vector<BigInt> leftLow0(n, 0);
    std::vector<BigInt> leftHigh1(n, 0);
    std::vector<BigInt> leftLow1(n, 0);
    std::vector<BigInt> rightHigh0(n, 0);
    std::vector<BigInt> rightLow0(n, 0);
    std::vector<BigInt> rightHigh1(n, 0);
    std::vector<BigInt> rightLow1(n, 0);

    // Explicit negacyclic witness: X^(N-1) * X = -1.
    leftHigh0[n - 1] = 1;
    rightHigh0[1] = 1;

    // Explicit signed product large enough to cross an active ~30-bit tower modulus.
    leftHigh1[0] = -1000003;
    rightHigh1[0] = 1000033;

    // Explicit omitted low-low witness: 17 * (-19) = -323 at coefficient 5.
    leftLow0[2] = 17;
    rightLow0[3] = -19;
    leftLow1[4] = -23;
    rightLow1[5] = 29;

    fixture.leftInput->SetElements({
        MakePolynomial(params, ComposeSource(divisor, leftHigh0, leftLow0)),
        MakePolynomial(params, ComposeSource(divisor, leftHigh1, leftLow1)),
    });
    fixture.rightInput->SetElements({
        MakePolynomial(params, ComposeSource(divisor, rightHigh0, rightLow0)),
        MakePolynomial(params, ComposeSource(divisor, rightHigh1, rightLow1)),
    });

    Check(fixture.leftInput->GetLevel() == 0 && fixture.rightInput->GetLevel() == 0,
          "Tensor2 fixtures must start at level zero");
    Check(fixture.leftInput->GetNoiseScaleDeg() == 2 && fixture.rightInput->GetNoiseScaleDeg() == 2,
          "Tensor2 fixtures must start at noise-scale degree two");
    Check(fixture.leftInput->GetKeyTag() == fixture.rightInput->GetKeyTag(),
          "Tensor2 fixtures must use one key tag");
    Check(fixture.leftInput->GetSlots() == fixture.rightInput->GetSlots(),
          "Tensor2 fixtures must start with matching slots");
    return fixture;
}

std::vector<BigInt> NegacyclicConvolutionMod(const std::vector<BigInt>& left,
                                             const std::vector<BigInt>& right,
                                             const BigInt& modulus) {
    Check(left.size() == right.size(), "negacyclic oracle input size mismatch");
    const std::size_t n = left.size();
    std::vector<BigInt> result(n, 0);
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            std::size_t coefficient = i + j;
            BigInt term = left[i] * right[j];
            if (coefficient >= n) {
                coefficient -= n;
                term = -term;
            }
            result[coefficient] = PositiveMod(result[coefficient] + term, modulus);
        }
    }
    return result;
}

struct TensorOracleResult {
    std::vector<BigInt> moduli;
    std::vector<std::vector<std::vector<BigInt>>> values;  // component, tower, coefficient
};

TensorOracleResult TensorOracle(lbcrypto::ConstCiphertext<DCRTPoly> left,
                                lbcrypto::ConstCiphertext<DCRTPoly> right) {
    Check(left != nullptr && right != nullptr, "tensor oracle received null ciphertext");
    Check(left->NumberCiphertextElements() == 2 && right->NumberCiphertextElements() == 2,
          "tensor oracle requires two-component inputs");

    std::vector<DCRTPoly> leftCoefficient;
    std::vector<DCRTPoly> rightCoefficient;
    for (const auto& element : left->GetElements()) {
        leftCoefficient.push_back(ToCoefficient(element));
    }
    for (const auto& element : right->GetElements()) {
        rightCoefficient.push_back(ToCoefficient(element));
    }

    TensorOracleResult result;
    result.moduli = GetModuli(leftCoefficient.front());
    Check(result.moduli == GetModuli(rightCoefficient.front()), "tensor oracle basis mismatch");
    const std::size_t towerCount = result.moduli.size();
    const std::size_t n = leftCoefficient.front().GetParams()->GetRingDimension();
    result.values.assign(3, std::vector<std::vector<BigInt>>(towerCount, std::vector<BigInt>(n, 0)));

    for (std::size_t leftComponent = 0; leftComponent < 2; ++leftComponent) {
        for (std::size_t rightComponent = 0; rightComponent < 2; ++rightComponent) {
            const std::size_t outputComponent = leftComponent + rightComponent;
            for (std::size_t tower = 0; tower < towerCount; ++tower) {
                std::vector<BigInt> leftResidues(n);
                std::vector<BigInt> rightResidues(n);
                for (std::size_t coefficient = 0; coefficient < n; ++coefficient) {
                    leftResidues[coefficient] =
                        CoefficientResidue(leftCoefficient[leftComponent], tower, coefficient);
                    rightResidues[coefficient] =
                        CoefficientResidue(rightCoefficient[rightComponent], tower, coefficient);
                }
                const auto product = NegacyclicConvolutionMod(leftResidues, rightResidues, result.moduli[tower]);
                for (std::size_t coefficient = 0; coefficient < n; ++coefficient) {
                    result.values[outputComponent][tower][coefficient] =
                        PositiveMod(result.values[outputComponent][tower][coefficient] + product[coefficient],
                                    result.moduli[tower]);
                }
            }
        }
    }
    return result;
}

TensorOracleResult AddOracleResults(const TensorOracleResult& left, const TensorOracleResult& right) {
    Check(left.moduli == right.moduli, "oracle-add basis mismatch");
    Check(left.values.size() == right.values.size(), "oracle-add component mismatch");
    TensorOracleResult result = left;
    for (std::size_t component = 0; component < result.values.size(); ++component) {
        for (std::size_t tower = 0; tower < result.moduli.size(); ++tower) {
            for (std::size_t coefficient = 0; coefficient < result.values[component][tower].size(); ++coefficient) {
                result.values[component][tower][coefficient] =
                    PositiveMod(left.values[component][tower][coefficient] +
                                    right.values[component][tower][coefficient],
                                result.moduli[tower]);
            }
        }
    }
    return result;
}

void CheckAgainstOracle(lbcrypto::ConstCiphertext<DCRTPoly> actual, const TensorOracleResult& expected,
                        const std::string& label) {
    Check(actual != nullptr, label + " is null");
    Check(actual->NumberCiphertextElements() == 3, label + " must contain exactly three RLWE components");
    Check(actual->GetElements().size() == expected.values.size(), label + " oracle component count mismatch");

    for (std::size_t component = 0; component < expected.values.size(); ++component) {
        const auto coefficientPolynomial = ToCoefficient(actual->GetElements().at(component));
        Check(GetModuli(coefficientPolynomial) == expected.moduli, label + " oracle basis mismatch");
        for (std::size_t tower = 0; tower < expected.moduli.size(); ++tower) {
            for (std::size_t coefficient = 0; coefficient < expected.values[component][tower].size(); ++coefficient) {
                const BigInt residue = CoefficientResidue(coefficientPolynomial, tower, coefficient);
                Check(residue == expected.values[component][tower][coefficient],
                      label + " mismatch at component " + std::to_string(component) + ", tower " +
                          std::to_string(tower) + ", coefficient " + std::to_string(coefficient));
            }
        }
    }
}

struct CiphertextSnapshot {
    lbcrypto::Ciphertext<DCRTPoly> clone;
    MetadataSnapshot metadata;
};

struct PairSnapshot {
    CiphertextSnapshot high;
    CiphertextSnapshot low;
    const lbcrypto::CryptoContextImpl<DCRTPoly>* contextIdentity;
    NativeInteger divisor;
    std::vector<NativeInteger> orderedModuli;
    std::size_t level;
    PaperScaleDescriptor paperScale;
    double recordedScalingFactor;
    std::size_t noiseScaleDegree;
    openfhe_2023_1788::PairLifecycle lifecycle;
    std::string keyTag;
    std::uint32_t slots;
    Format format;
    std::size_t componentCount;
};

PairSnapshot SnapshotPair(const CiphertextPair& pair, const std::string& label) {
    return {
        {pair.GetHigh()->Clone(), SnapshotMetadata(pair.GetHigh(), label + " high")},
        {pair.GetLow()->Clone(), SnapshotMetadata(pair.GetLow(), label + " low")},
        pair.GetContextIdentity(),
        pair.GetDivisor(),
        pair.GetOrderedModuli(),
        pair.GetLevel(),
        pair.GetPaperScale(),
        pair.GetRecordedScalingFactor(),
        pair.GetNoiseScaleDegree(),
        pair.GetLifecycle(),
        pair.GetKeyTag(),
        pair.GetSlots(),
        pair.GetFormat(),
        pair.GetComponentCount(),
    };
}

void CheckPairUnchanged(const CiphertextPair& pair, const PairSnapshot& before, const std::string& label) {
    Check(*pair.GetHigh() == *before.high.clone, label + " high observable ciphertext state changed");
    Check(*pair.GetLow() == *before.low.clone, label + " low observable ciphertext state changed");
    CheckMetadataUnchanged(pair.GetHigh(), before.high.metadata, label + " high");
    CheckMetadataUnchanged(pair.GetLow(), before.low.metadata, label + " low");
    Check(pair.GetContextIdentity() == before.contextIdentity, label + " context manifest changed");
    Check(pair.GetDivisor() == before.divisor, label + " divisor manifest changed");
    Check(pair.GetOrderedModuli() == before.orderedModuli, label + " basis manifest changed");
    Check(pair.GetLevel() == before.level, label + " level manifest changed");
    Check(pair.GetPaperScale().inputRecordedScalingFactor == before.paperScale.inputRecordedScalingFactor,
          label + " paper input scale changed");
    Check(pair.GetPaperScale().divisor == before.paperScale.divisor, label + " paper divisor changed");
    Check(pair.GetPaperScale().approximateLogicalScalingFactor ==
              before.paperScale.approximateLogicalScalingFactor,
          label + " paper high scale changed");
    Check(pair.GetRecordedScalingFactor() == before.recordedScalingFactor,
          label + " recorded scaling factor manifest changed");
    Check(pair.GetNoiseScaleDegree() == before.noiseScaleDegree, label + " noise-scale degree manifest changed");
    Check(pair.GetLifecycle() == before.lifecycle, label + " lifecycle manifest changed");
    Check(pair.GetKeyTag() == before.keyTag, label + " key-tag manifest changed");
    Check(pair.GetSlots() == before.slots, label + " slots manifest changed");
    Check(pair.GetFormat() == before.format, label + " format manifest changed");
    Check(pair.GetComponentCount() == before.componentCount, label + " component-count manifest changed");
}

void CheckOrderedModuli(const std::vector<NativeInteger>& actual, const std::vector<NativeInteger>& expected,
                        const std::string& label) {
    Check(actual.size() == expected.size(), label + " modulus count mismatch");
    for (std::size_t index = 0; index < expected.size(); ++index) {
        Check(actual[index] == expected[index],
              label + " ordered modulus mismatch at tower " + std::to_string(index));
    }
}

void CheckTensorMemberMetadata(lbcrypto::ConstCiphertext<DCRTPoly> ciphertext,
                               const CryptoContext<DCRTPoly>& expectedContext,
                               const std::vector<NativeInteger>& expectedModuli, std::size_t expectedLevel,
                               std::size_t expectedNoiseScaleDegree, double expectedScalingFactor,
                               const std::string& expectedKeyTag, std::uint32_t expectedSlots,
                               const std::string& label) {
    Check(ciphertext != nullptr, label + " is null");
    Check(ciphertext->GetCryptoContext().get() == expectedContext.get(), label + " context identity mismatch");
    Check(ciphertext->GetEncodingType() == lbcrypto::CKKS_PACKED_ENCODING, label + " encoding metadata mismatch");
    Check(ciphertext->NumberCiphertextElements() == 3, label + " component count mismatch");
    Check(ciphertext->GetLevel() == expectedLevel, label + " level mismatch");
    Check(ciphertext->GetNoiseScaleDeg() == expectedNoiseScaleDegree, label + " noise-scale degree mismatch");
    Check(ciphertext->GetScalingFactor() == expectedScalingFactor, label + " recorded scaling factor mismatch");
    Check(ciphertext->GetKeyTag() == expectedKeyTag, label + " key tag mismatch");
    Check(ciphertext->GetSlots() == expectedSlots, label + " slot-count metadata mismatch");
    for (const auto& element : ciphertext->GetElements()) {
        Check(element.GetFormat() == Format::EVALUATION, label + " is not in evaluation format");
        CheckOrderedModuli(GetNativeModuli(element), expectedModuli, label);
    }
}

std::pair<CiphertextPair, CiphertextPair> MakePairs(TensorFixture& fixture, DoubleCKKS& module) {
    fixture.leftInput->SetMetadataByKey("tensor2-immutability-left",
                                       std::make_shared<ImmutabilityProbeMetadata>("left-unchanged"));
    fixture.rightInput->SetMetadataByKey("tensor2-immutability-right",
                                        std::make_shared<ImmutabilityProbeMetadata>("right-unchanged"));
    return {module.DCP(fixture.leftInput), module.DCP(fixture.rightInput)};
}

void TestValidArithmeticAndImmutability() {
    auto fixture = MakeTensorFixture();
    DoubleCKKS module(fixture.context);
    auto [left, right] = MakePairs(fixture, module);

    const auto leftBefore = SnapshotPair(left, "left pair");
    const auto rightBefore = SnapshotPair(right, "right pair");

    const auto expectedHigh = TensorOracle(left.GetHigh(), right.GetHigh());
    const auto expectedCrossA = TensorOracle(left.GetHigh(), right.GetLow());
    const auto expectedCrossB = TensorOracle(left.GetLow(), right.GetHigh());
    const auto expectedLow = AddOracleResults(expectedCrossA, expectedCrossB);
    const auto omittedLowLow = TensorOracle(left.GetLow(), right.GetLow());

    const std::size_t n = left.GetHigh()->GetElements().front().GetParams()->GetRingDimension();
    Check(n == 32, "fixture ring dimension changed unexpectedly");

    const auto leftHigh0 = ToCoefficient(left.GetHigh()->GetElements().at(0));
    const auto rightHigh0 = ToCoefficient(right.GetHigh()->GetElements().at(0));
    Check(CoefficientResidue(leftHigh0, 0, n - 1) == 1, "X^(N-1) witness missing from left high");
    Check(CoefficientResidue(rightHigh0, 0, 1) == 1, "X witness missing from right high");
    Check(expectedHigh.values.at(0).at(0).at(0) == expectedHigh.moduli.at(0) - 1,
          "negacyclic X^(N-1)*X witness did not produce -1 at component 0/tower 0/coefficient 0");

    Check(expectedHigh.moduli.size() > 1, "signed modular-wrap witness requires active tower 1");
    const BigInt signedProduct = BigInt(-1000003) * BigInt(1000033);
    Check(-signedProduct > expectedHigh.moduli.at(1),
          "signed product no longer crosses named active tower 1 modulus");
    Check(expectedHigh.values.at(2).at(1).at(0) == PositiveMod(signedProduct, expectedHigh.moduli.at(1)),
          "signed modular-wrap witness mismatch at component 2/tower 1/coefficient 0");

    Check(omittedLowLow.values.at(0).at(0).at(5) == PositiveMod(BigInt(-323), omittedLowLow.moduli.at(0)),
          "low-low omission witness is not -323 at component 0/tower 0/coefficient 5");
    Check(omittedLowLow.values.at(0).at(0).at(5) != 0,
          "low-low omission witness is zero modulo named active tower 0");

    const TensorCiphertextPair result = module.Tensor2(left, right);
    CheckAgainstOracle(result.GetHigh(), expectedHigh, "Tensor2 high");
    CheckAgainstOracle(result.GetLow(), expectedLow, "Tensor2 low");

    const auto actualLow0 = ToCoefficient(result.GetLow()->GetElements().at(0));
    const BigInt actualWitness = CoefficientResidue(actualLow0, 0, 5);
    const BigInt crossWitness = expectedLow.values.at(0).at(0).at(5);
    const BigInt crossPlusLowLow =
        PositiveMod(crossWitness + omittedLowLow.values.at(0).at(0).at(5), omittedLowLow.moduli.at(0));
    Check(actualWitness == crossWitness,
          "Tensor2 low is not the independent cross term at the omission witness");
    Check(actualWitness != crossPlusLowLow,
          "Tensor2 low incorrectly includes the independently nonzero low-low term");

    CheckPairUnchanged(left, leftBefore, "left pair");
    CheckPairUnchanged(right, rightBefore, "right pair");
}

void TestResultAndScaleContract() {
    auto fixture = MakeTensorFixture();
    DoubleCKKS module(fixture.context);
    auto [left, right] = MakePairs(fixture, module);

    const long double h1 = left.GetPaperScale().approximateLogicalScalingFactor;
    const long double h2 = right.GetPaperScale().approximateLogicalScalingFactor;
    const long double r1 = static_cast<long double>(left.GetPaperScale().inputRecordedScalingFactor);
    const long double r2 = static_cast<long double>(right.GetPaperScale().inputRecordedScalingFactor);
    const long double divisor = static_cast<long double>(left.GetDivisor().ConvertToInt());
    const long double expectedHighLogicalScale = h1 * h2;
    const long double expectedRecombinedLogicalScale = r1 * r2 / divisor;

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(fixture.context->GetCryptoParameters());
    Check(parameters != nullptr, "fixture must expose CKKS-RNS parameters");
    const double baseScalingFactor = parameters->GetScalingFactorReal(0);
    const double expectedRecordedScalingFactor =
        left.GetRecordedScalingFactor() * right.GetRecordedScalingFactor() / baseScalingFactor;
    const std::size_t expectedNoiseScaleDegree =
        left.GetNoiseScaleDegree() + right.GetNoiseScaleDegree() - 1;

    Check(divisor != static_cast<long double>(baseScalingFactor),
          "test fixture unexpectedly equates the actual q_div prime with OpenFHE baseSF");

    const TensorCiphertextPair result = module.Tensor2(left, right);
    static_assert(std::is_same_v<std::decay_t<decltype(result)>, TensorCiphertextPair>,
                  "Tensor2 must return the distinct three-component result type");

    Check(result.GetContextIdentity() == fixture.context.get(), "Tensor2 result context identity mismatch");
    Check(result.GetDivisor() == left.GetDivisor(), "Tensor2 result divisor mismatch");
    Check(result.GetOrderedModuli() == left.GetOrderedModuli(), "Tensor2 result ordered basis mismatch");
    Check(result.GetLevel() == left.GetLevel(), "Tensor2 must not consume a tower or change level");
    Check(result.GetRecordedScalingFactor() == expectedRecordedScalingFactor,
          "Tensor2 normalized OpenFHE recorded scaling factor mismatch");
    Check(result.GetNoiseScaleDegree() == expectedNoiseScaleDegree,
          "Tensor2 normalized OpenFHE noise-scale degree mismatch");
    Check(result.GetNoiseScaleDegree() == 3, "fresh first Tensor2 result must have noise-scale degree three");
    Check(result.GetKeyTag() == left.GetKeyTag(), "Tensor2 result key tag mismatch");
    Check(result.GetSlots() == left.GetSlots(), "Tensor2 result slots mismatch");
    Check(result.GetFormat() == Format::EVALUATION, "Tensor2 result format mismatch");
    Check(result.GetComponentCount() == 3, "Tensor2 result manifest must record three components");

    const auto& tensorScale = result.GetTensorScale();
    Check(tensorScale.approximateHighLogicalScalingFactor == expectedHighLogicalScale,
          "Tensor2 H_out paper-scale transition mismatch");
    Check(tensorScale.approximateRecombinedLogicalScalingFactor == expectedRecombinedLogicalScale,
          "Tensor2 R_out paper-scale transition mismatch");

    CheckTensorMemberMetadata(result.GetHigh(), fixture.context, left.GetOrderedModuli(), left.GetLevel(),
                              expectedNoiseScaleDegree, expectedRecordedScalingFactor, left.GetKeyTag(),
                              left.GetSlots(), "Tensor2 high metadata");
    CheckTensorMemberMetadata(result.GetLow(), fixture.context, left.GetOrderedModuli(), left.GetLevel(),
                              expectedNoiseScaleDegree, expectedRecordedScalingFactor, left.GetKeyTag(),
                              left.GetSlots(), "Tensor2 low metadata");
}

void TestRightInputValidation() {
    auto fixture = MakeTensorFixture();
    DoubleCKKS module(fixture.context);
    auto [left, right] = MakePairs(fixture, module);

    auto& corruptedScale = const_cast<PaperScaleDescriptor&>(right.GetPaperScale());
    corruptedScale.approximateLogicalScalingFactor *= 2.0L;

    CheckThrowsInvalidArgument([&] { (void)module.Tensor2(left, right); },
                               "pair paper-scale descriptor is inconsistent",
                               "Tensor2 right-input manifest validation");
}

void TestMutualCompatibility() {
    auto fixture = MakeTensorFixture();
    DoubleCKKS module(fixture.context);

    const std::uint32_t leftSlots = fixture.leftInput->GetSlots();
    Check(leftSlots > 1, "fixture needs more than one slot for compatibility test");
    fixture.rightInput->SetSlots(leftSlots - 1);

    const auto left = module.DCP(fixture.leftInput);
    const auto right = module.DCP(fixture.rightInput);

    // These public calls re-run the complete existing pair validator and prove each pair is individually valid.
    (void)module.RCB(left);
    (void)module.RCB(right);
    Check(left.GetSlots() != right.GetSlots(), "mutual-compatibility fixture failed to create unequal slots");

    CheckThrowsInvalidArgument([&] { (void)module.Tensor2(left, right); }, "Tensor2 input slots do not match",
                               "Tensor2 mutual slot compatibility");
}

void TestPreArithmeticKeyCompatibility() {
    auto fixture = MakeTensorFixture();
    DoubleCKKS module(fixture.context);

    auto secondKeys = fixture.context->KeyGen();
    auto secondPlaintext = fixture.context->MakeCKKSPackedPlaintext(std::vector<double>{0.0}, 2, 0);
    auto secondInput = fixture.context->Encrypt(secondPlaintext, secondKeys.publicKey);
    auto deterministicElements = fixture.rightInput->GetElements();
    secondInput->SetElements(std::move(deterministicElements));

    const auto left = module.DCP(fixture.leftInput);
    const auto right = module.DCP(secondInput);

    // Both pairs are individually valid and slots match. OpenFHE TypeCheck also
    // rejects different key tags, so the project-owned diagnostic proves that
    // Tensor2 checks mutual compatibility before invoking multiplication.
    (void)module.RCB(left);
    (void)module.RCB(right);
    Check(left.GetSlots() == right.GetSlots(), "key-compatibility fixture has unequal slots");
    Check(left.GetKeyTag() != right.GetKeyTag(), "key-compatibility fixture did not create distinct key tags");

    CheckThrowsInvalidArgument([&] { (void)module.Tensor2(left, right); }, "Tensor2 input key tags do not match",
                               "Tensor2 pre-arithmetic key compatibility");
}

using TestFunction = void (*)();

TestFunction ResolveTest(const std::string& name) {
    if (name == "valid_arithmetic_immutability") {
        return &TestValidArithmeticAndImmutability;
    }
    if (name == "result_scale_contract") {
        return &TestResultAndScaleContract;
    }
    if (name == "right_input_validation") {
        return &TestRightInputValidation;
    }
    if (name == "mutual_compatibility") {
        return &TestMutualCompatibility;
    }
    if (name == "prearithmetic_key_compatibility") {
        return &TestPreArithmeticKeyCompatibility;
    }
    throw TestFailure("unknown Tensor2 test case: " + name);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Tensor2 test failure: expected exactly one case name\n";
        return 2;
    }

    try {
        ResolveTest(argv[1])();
        lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
        std::cout << "Tensor2 case passed: " << argv[1] << '\n';
        return 0;
    }
    catch (const TestFailure& failure) {
        std::cerr << "Tensor2 test failure: " << failure.what() << '\n';
        return 1;
    }
    catch (const std::exception& exception) {
        std::cerr << "Tensor2 unexpected exception: " << exception.what() << '\n';
        return 1;
    }
}
