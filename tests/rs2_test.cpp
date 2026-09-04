#include "openfhe.h"
#include "openfhe_2023_1788/double_ckks.h"

#include <boost/multiprecision/cpp_int.hpp>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
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
using openfhe_2023_1788::CiphertextPair;
using openfhe_2023_1788::DoubleCKKS;
using openfhe_2023_1788::PairLifecycle;
using openfhe_2023_1788::PaperScaleDescriptor;
using openfhe_2023_1788::ReadOnlyCiphertext;

class TestFailure final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void Check(bool condition, const std::string& message) {
    if (!condition) {
        throw TestFailure(message);
    }
}

class ProbeMetadata final : public lbcrypto::Metadata {
public:
    explicit ProbeMetadata(std::string value) : value_(std::move(value)) {}

    std::shared_ptr<lbcrypto::Metadata> Clone() const override {
        return std::make_shared<ProbeMetadata>(value_);
    }

    bool operator==(const lbcrypto::Metadata& metadata) const override {
        const auto* other = dynamic_cast<const ProbeMetadata*>(&metadata);
        return other != nullptr && value_ == other->value_;
    }

private:
    std::string value_;
};

struct MetadataEntrySnapshot {
    std::string key;
    std::shared_ptr<lbcrypto::Metadata> identity;
    std::shared_ptr<lbcrypto::Metadata> deepValue;
};

struct CiphertextSnapshot {
    ReadOnlyCiphertext identity;
    Ciphertext<DCRTPoly> clone;
    lbcrypto::MetadataMap metadataIdentity;
    std::vector<MetadataEntrySnapshot> metadata;
};

CiphertextSnapshot SnapshotCiphertext(const ReadOnlyCiphertext& ciphertext,
                                      const std::string& label) {
    Check(ciphertext != nullptr, label + " is null");
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");

    CiphertextSnapshot snapshot{ciphertext, ciphertext->Clone(), metadata, {}};
    snapshot.metadata.reserve(metadata->size());
    for (const auto& [key, value] : *metadata) {
        Check(value != nullptr, label + " metadata value is null");
        auto deepValue = value->Clone();
        Check(deepValue != nullptr, label + " metadata clone is null");
        snapshot.metadata.push_back({key, value, std::move(deepValue)});
    }
    return snapshot;
}

void CheckCiphertextUnchanged(const ReadOnlyCiphertext& ciphertext,
                              const CiphertextSnapshot& before,
                              const std::string& label) {
    Check(ciphertext != nullptr && before.identity != nullptr && before.clone != nullptr,
          label + " ciphertext or snapshot is null");
    Check(ciphertext.get() == before.identity.get(), label + " ciphertext identity changed");
    Check(*ciphertext == *before.clone, label + " ciphertext value changed");

    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");
    Check(metadata.get() == before.metadataIdentity.get(), label + " metadata map identity changed");
    Check(metadata->size() == before.metadata.size(), label + " metadata map size changed");
    auto current = metadata->begin();
    for (const auto& expected : before.metadata) {
        Check(current != metadata->end(), label + " metadata entry disappeared");
        Check(current->first == expected.key, label + " metadata key or order changed");
        Check(current->second.get() == expected.identity.get(), label + " metadata identity changed");
        Check(*(current->second) == *(expected.deepValue), label + " metadata value changed");
        ++current;
    }
    Check(current == metadata->end(), label + " metadata gained trailing entries");
}

void CheckMetadataDerivedFrom(const ReadOnlyCiphertext& ciphertext,
                              const CiphertextSnapshot& source,
                              const std::string& label) {
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");
    Check(metadata.get() != source.metadataIdentity.get(), label + " metadata map aliases its source");
    Check(metadata->size() == source.metadata.size(), label + " metadata map size mismatch");
    auto current = metadata->begin();
    for (const auto& expected : source.metadata) {
        Check(current != metadata->end(), label + " metadata entry disappeared");
        Check(current->first == expected.key, label + " metadata key or order mismatch");
        Check(current->second.get() == expected.identity.get(),
              label + " metadata value did not retain source provenance");
        Check(*(current->second) == *(expected.deepValue), label + " metadata value mismatch");
        ++current;
    }
    Check(current == metadata->end(), label + " metadata gained trailing entries");
}

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
    PairLifecycle lifecycle;
    std::string keyTag;
    std::uint32_t slots;
    Format format;
    std::size_t componentCount;
};

PairSnapshot SnapshotPair(const CiphertextPair& pair, const std::string& label) {
    return {SnapshotCiphertext(pair.GetHigh(), label + " high"),
            SnapshotCiphertext(pair.GetLow(), label + " low"),
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
            pair.GetComponentCount()};
}

void CheckPairUnchanged(const CiphertextPair& pair,
                        const PairSnapshot& before,
                        const std::string& label) {
    CheckCiphertextUnchanged(pair.GetHigh(), before.high, label + " high");
    CheckCiphertextUnchanged(pair.GetLow(), before.low, label + " low");
    Check(pair.GetContextIdentity() == before.contextIdentity, label + " context manifest changed");
    Check(pair.GetDivisor() == before.divisor, label + " divisor manifest changed");
    Check(pair.GetOrderedModuli() == before.orderedModuli, label + " basis manifest changed");
    Check(pair.GetLevel() == before.level, label + " level manifest changed");
    Check(pair.GetPaperScale().inputRecordedScalingFactor == before.paperScale.inputRecordedScalingFactor,
          label + " paper recorded scale changed");
    Check(pair.GetPaperScale().divisor == before.paperScale.divisor,
          label + " paper divisor changed");
    Check(pair.GetPaperScale().approximateLogicalScalingFactor ==
              before.paperScale.approximateLogicalScalingFactor,
          label + " high logical scale changed");
    Check(pair.GetPaperScale().approximateRecombinedLogicalScalingFactor ==
              before.paperScale.approximateRecombinedLogicalScalingFactor,
          label + " recombined logical scale changed");
    Check(pair.GetRecordedScalingFactor() == before.recordedScalingFactor,
          label + " recorded scale changed");
    Check(pair.GetNoiseScaleDegree() == before.noiseScaleDegree,
          label + " noise-scale degree changed");
    Check(pair.GetLifecycle() == before.lifecycle, label + " lifecycle changed");
    Check(pair.GetKeyTag() == before.keyTag, label + " key tag changed");
    Check(pair.GetSlots() == before.slots, label + " slots changed");
    Check(pair.GetFormat() == before.format, label + " format changed");
    Check(pair.GetComponentCount() == before.componentCount,
          label + " component count changed");
}

template <class Function>
void CheckThrowsInvalidArgument(Function&& function, const std::string& expectedMessage) {
    bool threw = false;
    try {
        std::invoke(std::forward<Function>(function));
    }
    catch (const std::invalid_argument& exception) {
        Check(exception.what() == expectedMessage,
              "RS2 wrong-lifecycle diagnostic mismatch: " + std::string(exception.what()));
        threw = true;
    }
    catch (const std::exception& exception) {
        throw TestFailure("RS2 wrong lifecycle threw the wrong exception type: " +
                          std::string(exception.what()));
    }
    Check(threw, "RS2 wrong lifecycle did not fail fast");
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
    x = nextY;
    y = nextX - (a / b) * nextY;
    return gcd;
}

BigInt ModInverse(const BigInt& value, const BigInt& modulus) {
    BigInt x;
    BigInt y;
    const BigInt gcd = ExtendedGcd(PositiveMod(value, modulus), modulus, x, y);
    Check(gcd == 1, "RS2 CRT oracle received non-coprime moduli");
    return PositiveMod(x, modulus);
}

BigInt Product(const std::vector<BigInt>& values) {
    BigInt result = 1;
    for (const auto& value : values) {
        result *= value;
    }
    return result;
}

BigInt Center(const BigInt& residue, const BigInt& modulus) {
    BigInt centered = PositiveMod(residue, modulus);
    if (centered > modulus / 2) {
        centered -= modulus;
    }
    return centered;
}

BigInt ReconstructCentered(const std::vector<BigInt>& residues,
                           const std::vector<BigInt>& moduli) {
    Check(!moduli.empty() && residues.size() == moduli.size(),
          "RS2 CRT oracle residue/modulus shape mismatch");
    const BigInt modulus = Product(moduli);
    BigInt result = 0;
    for (std::size_t index = 0; index < moduli.size(); ++index) {
        const BigInt partial = modulus / moduli[index];
        result += PositiveMod(residues[index], moduli[index]) * partial *
                  ModInverse(partial, moduli[index]);
    }
    return Center(result, modulus);
}

std::vector<BigInt> GetModuli(const DCRTPoly& polynomial) {
    std::vector<BigInt> result;
    result.reserve(polynomial.GetAllElements().size());
    for (const auto& tower : polynomial.GetAllElements()) {
        result.emplace_back(tower.GetModulus().ConvertToInt());
    }
    return result;
}

DCRTPoly ToCoefficient(const DCRTPoly& polynomial) {
    DCRTPoly result(polynomial);
    result.SetFormat(Format::COEFFICIENT);
    return result;
}

BigInt CoefficientResidue(const DCRTPoly& coefficientPolynomial,
                          std::size_t tower,
                          std::size_t coefficient) {
    return BigInt(coefficientPolynomial.GetAllElements().at(tower).GetValues().at(coefficient).ConvertToInt());
}

BigInt ReconstructCoefficient(const DCRTPoly& polynomial, std::size_t coefficient) {
    const auto coefficientPolynomial = ToCoefficient(polynomial);
    const auto moduli = GetModuli(coefficientPolynomial);
    std::vector<BigInt> residues;
    residues.reserve(moduli.size());
    for (std::size_t tower = 0; tower < moduli.size(); ++tower) {
        residues.push_back(CoefficientResidue(coefficientPolynomial, tower, coefficient));
    }
    return ReconstructCentered(residues, moduli);
}

BigInt RescaleCentered(const BigInt& value,
                       const BigInt& sourceModulus,
                       const BigInt& droppedModulus) {
    const BigInt centeredValue = Center(value, sourceModulus);
    const BigInt remainder = Center(centeredValue, droppedModulus);
    Check((centeredValue - remainder) % droppedModulus == 0,
          "RS2 quotient oracle lost exact divisibility");
    return (centeredValue - remainder) / droppedModulus;
}

DCRTPoly MakePolynomial(const std::shared_ptr<DCRTPoly::Params>& params,
                        const std::vector<BigInt>& coefficients) {
    DCRTPoly result(params, Format::COEFFICIENT, true);
    const std::size_t ringDimension = params->GetRingDimension();
    Check(coefficients.size() == ringDimension, "RS2 coefficient fixture length mismatch");
    for (auto& tower : result.GetAllElements()) {
        const BigInt modulus(tower.GetModulus().ConvertToInt());
        lbcrypto::NativeVector values(ringDimension, tower.GetModulus());
        for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
            values[coefficient] = NativeInteger(
                PositiveMod(coefficients[coefficient], modulus).convert_to<std::uint64_t>());
        }
        tower.SetValues(std::move(values), Format::COEFFICIENT);
    }
    result.SetFormat(Format::EVALUATION);
    return result;
}

std::vector<BigInt> BoundaryValues(const BigInt& droppedModulus,
                                   const BigInt& sourceModulus,
                                   std::size_t ringDimension,
                                   std::size_t phase) {
    const BigInt droppedHalf = droppedModulus / 2;
    const BigInt sourceHalf = sourceModulus / 2;
    const std::vector<BigInt> witnesses{
        0,
        1,
        -1,
        droppedHalf - 1,
        droppedHalf,
        droppedHalf + 1,
        -droppedHalf + 1,
        -droppedHalf,
        -droppedHalf - 1,
        droppedModulus - 1,
        droppedModulus,
        droppedModulus + 1,
        -droppedModulus + 1,
        -droppedModulus,
        -droppedModulus - 1,
        sourceHalf - 1,
        sourceHalf,
        sourceHalf + 1,
        -sourceHalf + 1,
        -sourceHalf,
        -sourceHalf - 1,
    };
    Check(ringDimension >= witnesses.size(),
          "RS2 ring dimension is too small for the fixed boundary witness set");
    std::vector<BigInt> result(ringDimension);
    for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
        result[coefficient] = witnesses[(coefficient + phase) % witnesses.size()];
    }
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

void InstallControlledValues(const CiphertextPair& pair) {
    auto high = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetHigh());
    auto low = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetLow());
    Check(high != nullptr && low != nullptr, "RS2 controlled fixture contains a null pair member");
    Check(high.get() != low.get(), "RS2 controlled fixture pair members alias");
    const auto params = high->GetElements().front().GetParams();
    Check(params != nullptr && *params == *low->GetElements().front().GetParams(),
          "RS2 controlled fixture pair bases do not match");
    const std::size_t ringDimension = params->GetRingDimension();
    const auto moduli = GetModuli(high->GetElements().front());
    Check(moduli.size() >= 2, "RS2 controlled fixture requires at least two active towers");
    const BigInt sourceModulus = Product(moduli);
    const BigInt droppedModulus = moduli.back();
    const BigInt divisor(pair.GetDivisor().ConvertToInt());

    std::vector<DCRTPoly> highElements;
    std::vector<DCRTPoly> lowElements;
    highElements.reserve(2);
    lowElements.reserve(2);
    for (std::size_t component = 0; component < 2; ++component) {
        const auto highValues = BoundaryValues(droppedModulus, sourceModulus, ringDimension,
                                               component * 5);
        const auto recombinedValues = BoundaryValues(droppedModulus, sourceModulus, ringDimension,
                                                     component * 7 + 3);
        std::vector<BigInt> lowValues(ringDimension);
        for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
            lowValues[coefficient] = recombinedValues[coefficient] - divisor * highValues[coefficient];
        }
        highElements.push_back(MakePolynomial(params, highValues));
        lowElements.push_back(MakePolynomial(params, lowValues));
    }
    high->SetElements(std::move(highElements));
    low->SetElements(std::move(lowElements));
    high->SetMetadataByKey("rs2-high-provenance", std::make_shared<ProbeMetadata>("high-source"));
    low->SetMetadataByKey("rs2-low-provenance", std::make_shared<ProbeMetadata>("low-source"));
    Check(high->GetMetadataMap().get() != low->GetMetadataMap().get(),
          "RS2 controlled fixture metadata maps alias");
}

void CheckMemberState(const ReadOnlyCiphertext& member,
                      const CiphertextPair& input,
                      const CryptoContext<DCRTPoly>& context,
                      const std::vector<NativeInteger>& expectedModuli,
                      double expectedRecordedScalingFactor,
                      const CiphertextSnapshot& expectedMetadata,
                      const std::string& label) {
    Check(member != nullptr, label + " is null");
    Check(member.get() != input.GetHigh().get() && member.get() != input.GetLow().get(),
          label + " aliases an input ciphertext");
    Check(member->GetCryptoContext().get() == context.get(), label + " context identity mismatch");
    Check(member->GetEncodingType() == lbcrypto::CKKS_PACKED_ENCODING, label + " encoding mismatch");
    Check(member->GetLevel() == input.GetLevel() + 1, label + " level mismatch");
    Check(member->NumberCiphertextElements() == 2, label + " component count mismatch");
    Check(member->GetNoiseScaleDeg() == input.GetNoiseScaleDegree() - 1,
          label + " noise-scale degree mismatch");
    Check(member->GetScalingFactor() == expectedRecordedScalingFactor,
          label + " recorded scaling factor mismatch");
    Check(member->GetKeyTag() == input.GetKeyTag(), label + " key tag mismatch");
    Check(member->GetSlots() == input.GetSlots(), label + " slots mismatch");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr, label + " CKKS-RNS parameters are missing");
    const auto& expectedTowerParameters = parameters->GetElementParams()->GetParams();
    for (std::size_t component = 0; component < member->GetElements().size(); ++component) {
        const auto& element = member->GetElements().at(component);
        Check(element.GetFormat() == Format::EVALUATION,
              label + " component is not in evaluation format");
        const auto& towers = element.GetAllElements();
        Check(towers.size() == expectedModuli.size(), label + " active tower count mismatch");
        for (std::size_t tower = 0; tower < towers.size(); ++tower) {
            Check(towers[tower].GetModulus() == expectedModuli[tower],
                  label + " ordered modulus mismatch");
            Check(towers[tower].GetRootOfUnity() == expectedTowerParameters[tower]->GetRootOfUnity(),
                  label + " tower root mismatch");
            Check(towers[tower].GetCyclotomicOrder() ==
                      expectedTowerParameters[tower]->GetCyclotomicOrder(),
                  label + " tower cyclotomic order mismatch");
            Check(towers[tower].GetFormat() == Format::EVALUATION,
                  label + " tower is not in evaluation format");
        }
    }
    CheckMetadataDerivedFrom(member, expectedMetadata, label);
}

void CheckResultState(const CiphertextPair& result,
                      const CiphertextPair& input,
                      const PairSnapshot& inputBefore,
                      const CryptoContext<DCRTPoly>& context) {
    auto expectedModuli = input.GetOrderedModuli();
    Check(expectedModuli.size() >= 2, "RS2 result-state oracle needs a droppable q_l tower");
    const NativeInteger droppedModulus = expectedModuli.back();
    expectedModuli.pop_back();

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr, "RS2 result-state oracle is missing CKKS-RNS parameters");
    const double modReduceFactor =
        parameters->GetModReduceFactor(static_cast<std::uint32_t>(input.GetOrderedModuli().size() - 1));
    Check(modReduceFactor == parameters->GetScalingFactorReal(0),
          "RS2 FIXEDMANUAL metadata divisor is not the configured 2^p factor");
    Check(static_cast<double>(droppedModulus.ConvertToInt()) != modReduceFactor,
          "RS2 fixture does not distinguish native q_l from the 2^p metadata divisor");
    const double expectedRecordedScalingFactor = input.GetRecordedScalingFactor() / modReduceFactor;
    const long double dropped = static_cast<long double>(droppedModulus.ConvertToInt());

    Check(result.GetLifecycle() == PairLifecycle::RefreshRequired, "RS2 lifecycle mismatch");
    Check(result.GetContextIdentity() == context.get(), "RS2 context identity mismatch");
    Check(result.GetDivisor() == input.GetDivisor(), "RS2 q_div manifest changed");
    Check(result.GetOrderedModuli() == expectedModuli, "RS2 ordered Q_(l-1) manifest mismatch");
    Check(result.GetLevel() == input.GetLevel() + 1, "RS2 pair level mismatch");
    Check(result.GetRecordedScalingFactor() == expectedRecordedScalingFactor,
          "RS2 recorded factor did not divide by the FIXEDMANUAL 2^p factor");
    Check(result.GetNoiseScaleDegree() == input.GetNoiseScaleDegree() - 1,
          "RS2 pair noise-scale degree mismatch");
    Check(result.GetPaperScale().inputRecordedScalingFactor == expectedRecordedScalingFactor,
          "RS2 paper recorded-factor field mismatch");
    Check(result.GetPaperScale().divisor == input.GetDivisor(), "RS2 paper q_div changed");
    Check(result.GetPaperScale().approximateLogicalScalingFactor ==
              input.GetPaperScale().approximateLogicalScalingFactor / dropped,
          "RS2 high logical scale did not divide by native q_l");
    Check(result.GetPaperScale().approximateRecombinedLogicalScalingFactor ==
              input.GetPaperScale().approximateRecombinedLogicalScalingFactor / dropped,
          "RS2 recombined logical scale did not divide by native q_l");
    Check(result.GetKeyTag() == input.GetKeyTag(), "RS2 key tag mismatch");
    Check(result.GetSlots() == input.GetSlots(), "RS2 slot count mismatch");
    Check(result.GetFormat() == Format::EVALUATION, "RS2 pair format mismatch");
    Check(result.GetComponentCount() == 2, "RS2 pair component-count manifest mismatch");
    Check(result.GetHigh().get() != result.GetLow().get(), "RS2 result pair members alias");

    CheckMemberState(result.GetHigh(), input, context, expectedModuli,
                     expectedRecordedScalingFactor, inputBefore.high, "RS2 result high");
    CheckMemberState(result.GetLow(), input, context, expectedModuli,
                     expectedRecordedScalingFactor, inputBefore.high, "RS2 result low");
    Check(result.GetHigh()->GetMetadataMap().get() != result.GetLow()->GetMetadataMap().get(),
          "RS2 result metadata outer maps alias");
    Check(result.GetHigh()->GetMetadataMap().get() != inputBefore.low.metadataIdentity.get() &&
              result.GetLow()->GetMetadataMap().get() != inputBefore.low.metadataIdentity.get(),
          "RS2 result metadata map aliases input low metadata");
    Check(result.GetHigh()->GetMetadataMap()->find("rs2-low-provenance") ==
              result.GetHigh()->GetMetadataMap()->end() &&
              result.GetLow()->GetMetadataMap()->find("rs2-low-provenance") ==
                  result.GetLow()->GetMetadataMap()->end(),
          "RS2 result retained low-only metadata after public recombination");
}

void CheckExactArithmeticOracle(DoubleCKKS& module,
                                const CiphertextPair& input,
                                const CiphertextPair& result) {
    const BigInt divisor(input.GetDivisor().ConvertToInt());
    const auto sourceModuli = GetModuli(input.GetHigh()->GetElements().front());
    const auto targetModuli = GetModuli(result.GetHigh()->GetElements().front());
    Check(sourceModuli.size() == targetModuli.size() + 1,
          "RS2 exact oracle expected one consumed tower");
    for (std::size_t tower = 0; tower < targetModuli.size(); ++tower) {
        Check(sourceModuli[tower] == targetModuli[tower],
              "RS2 exact oracle target is not the ordered source prefix");
    }
    const BigInt sourceModulus = Product(sourceModuli);
    const BigInt droppedModulus = sourceModuli.back();
    const std::size_t ringDimension =
        input.GetHigh()->GetElements().front().GetParams()->GetRingDimension();
    Check(result.GetHigh()->GetElements().size() == 2 && result.GetLow()->GetElements().size() == 2,
          "RS2 exact oracle requires two RLWE components per member");

    const PairSnapshot resultBeforeRcb = SnapshotPair(result, "RS2 result before RCB");
    const auto recombinedResult = module.RCB(result);
    CheckPairUnchanged(result, resultBeforeRcb, "RS2 result after RCB");
    Check(recombinedResult != nullptr, "RS2 public RCB returned null");
    Check(recombinedResult.get() != result.GetHigh().get() &&
              recombinedResult.get() != result.GetLow().get(),
          "RS2 public RCB aliases a result member");
    Check(recombinedResult->GetLevel() == result.GetLevel(), "RS2 public RCB level mismatch");
    Check(recombinedResult->GetNoiseScaleDeg() == result.GetNoiseScaleDegree(),
          "RS2 public RCB noise-scale degree mismatch");
    Check(recombinedResult->GetScalingFactor() == result.GetRecordedScalingFactor(),
          "RS2 public RCB recorded scaling factor mismatch");

    bool differsFromDirectLowRescale = false;
    for (std::size_t component = 0; component < 2; ++component) {
        const auto inputHigh = ToCoefficient(input.GetHigh()->GetElements().at(component));
        const auto inputLow = ToCoefficient(input.GetLow()->GetElements().at(component));
        const auto actualHigh = ToCoefficient(result.GetHigh()->GetElements().at(component));
        const auto actualLow = ToCoefficient(result.GetLow()->GetElements().at(component));
        const auto actualRecombined = ToCoefficient(recombinedResult->GetElements().at(component));

        for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
            const BigInt high = ReconstructCoefficient(inputHigh, coefficient);
            const BigInt low = ReconstructCoefficient(inputLow, coefficient);
            const BigInt recombined = Center(divisor * high + low, sourceModulus);
            const BigInt rescaledHigh =
                RescaleCentered(high, sourceModulus, droppedModulus);
            const BigInt rescaledRecombined =
                RescaleCentered(recombined, sourceModulus, droppedModulus);
            const BigInt expectedLow = rescaledRecombined - divisor * rescaledHigh;
            const BigInt directLowRescale =
                RescaleCentered(low, sourceModulus, droppedModulus);

            for (std::size_t tower = 0; tower < targetModuli.size(); ++tower) {
                const BigInt expectedHighResidue = PositiveMod(rescaledHigh, targetModuli[tower]);
                const BigInt expectedLowResidue = PositiveMod(expectedLow, targetModuli[tower]);
                const BigInt expectedRecombinedResidue =
                    PositiveMod(rescaledRecombined, targetModuli[tower]);
                const std::string location =
                    "component=" + std::to_string(component) + ",tower=" +
                    std::to_string(tower) + ",coefficient=" + std::to_string(coefficient);
                Check(CoefficientResidue(actualHigh, tower, coefficient) == expectedHighResidue,
                      "RS2 high quotient mismatch at " + location);
                Check(CoefficientResidue(actualLow, tower, coefficient) == expectedLowResidue,
                      "RS2 low correction mismatch at " + location);
                Check(CoefficientResidue(actualRecombined, tower, coefficient) ==
                          expectedRecombinedResidue,
                      "RCB(RS2(pair)) != RS(RCB(pair)) at " + location);
                if (expectedLowResidue != PositiveMod(directLowRescale, targetModuli[tower])) {
                    differsFromDirectLowRescale = true;
                }
            }
        }
    }
    Check(differsFromDirectLowRescale,
          "RS2 witnesses do not distinguish the required two-RS construction from RS(low)");
}

void TestWrongLifecycle() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    const auto plaintext =
        context->MakeCKKSPackedPlaintext(std::vector<double>{1.25, -2.0, 0.0}, 2, 0);
    auto input = context->Encrypt(plaintext, keys.publicKey);
    input->SetMetadataByKey("rs2-wrong-lifecycle", std::make_shared<ProbeMetadata>("unchanged"));

    DoubleCKKS module(context);
    const CiphertextPair pair = module.DCP(input);
    Check(pair.GetLifecycle() == PairLifecycle::ReadyForFirstMult,
          "RS2 wrong-lifecycle fixture is not ReadyForFirstMult");
    const PairSnapshot before = SnapshotPair(pair, "RS2 wrong-lifecycle input");

    CheckThrowsInvalidArgument(
        [&] {
            (void)module.RS2(pair);
        },
        "DoubleCKKS: RS2 requires ReadyForRS2 input");
    CheckPairUnchanged(pair, before, "RS2 wrong-lifecycle input");

    lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
}

void TestValidArithmeticStateImmutability() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    context->EvalMultKeyGen(keys.secretKey);
    const auto plaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.0}, 2, 0);
    const auto leftInput = context->Encrypt(plaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(plaintext, keys.publicKey);

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);
    const CiphertextPair relinearized = module.Relin2(tensor);
    Check(relinearized.GetLifecycle() == PairLifecycle::ReadyForRS2,
          "RS2 valid fixture did not reach ReadyForRS2 through the public pipeline");
    Check(relinearized.GetNoiseScaleDegree() == 3,
          "RS2 valid fixture must start at noise-scale degree three");
    Check(relinearized.GetOrderedModuli().size() == 3,
          "RS2 valid fixture must expose three active Q_l towers");

    InstallControlledValues(relinearized);
    const PairSnapshot before = SnapshotPair(relinearized, "RS2 valid input");
    const auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    const auto evaluationKeysBefore = evaluationKeys;

    const CiphertextPair result = module.RS2(relinearized);
    CheckPairUnchanged(relinearized, before, "RS2 valid input");
    Check(evaluationKeys == evaluationKeysBefore,
          "RS2 mutated the process-wide evaluation-key cache");
    CheckResultState(result, relinearized, before, context);
    CheckExactArithmeticOracle(module, relinearized, result);

    lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
}

void TestMixedTowerFormat() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    context->EvalMultKeyGen(keys.secretKey);
    const auto plaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    DoubleCKKS module(context);
    const auto left = module.DCP(context->Encrypt(plaintext, keys.publicKey));
    const auto right = module.DCP(context->Encrypt(plaintext, keys.publicKey));
    const auto tensor = module.Tensor2(left, right);

    for (const bool corruptHigh : {true, false}) {
        const auto input = module.Relin2(tensor);
        auto member = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(
            corruptHigh ? input.GetHigh() : input.GetLow());
        auto& element = member->GetElements().at(0);
        element.GetAllElements().at(0).SetFormat(Format::COEFFICIENT);
        Check(element.GetFormat() == Format::EVALUATION,
              "RS2 mixed-format fixture changed the aggregate format");
        Check(element.GetAllElements().at(0).GetFormat() == Format::COEFFICIENT,
              "RS2 mixed-format fixture did not corrupt an individual tower");
        const auto before = SnapshotPair(input, "RS2 mixed-format input");
        const std::string memberName = corruptHigh ? "pair high" : "pair low";
        CheckThrowsInvalidArgument(
            [&] { (void)module.RS2(input); },
            "DoubleCKKS: " + memberName + " tower must be in evaluation format");
        CheckPairUnchanged(input, before, "RS2 mixed-format input after rejection");
    }
    lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
}

}  // namespace

int main(int argc, char** argv) {
    try {
        if (argc != 2) {
            throw TestFailure("usage: rs2_test <case>");
        }
        const std::string name(argv[1]);
        if (name == "wrong_lifecycle") {
            TestWrongLifecycle();
        }
        else if (name == "valid_arithmetic_state_immutability") {
            TestValidArithmeticStateImmutability();
        }
        else if (name == "mixed_tower_format") {
            TestMixedTowerFormat();
        }
        else {
            throw TestFailure("unknown RS2 case: " + name);
        }
        std::cout << "RS2 case passed: " << name << '\n';
        return 0;
    }
    catch (const std::exception& exception) {
        std::cerr << "RS2 test failure: " << exception.what() << '\n';
        return 1;
    }
}
