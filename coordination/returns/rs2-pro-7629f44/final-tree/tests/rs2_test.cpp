#include "openfhe.h"
#include "openfhe_2023_1788/double_ckks.h"

#include <boost/multiprecision/cpp_int.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>
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
using openfhe_2023_1788::PairLifecycle;
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
    std::shared_ptr<lbcrypto::Metadata> identity;
    std::shared_ptr<lbcrypto::Metadata> value;
};

using MetadataSnapshot = std::vector<MetadataSnapshotEntry>;

MetadataSnapshot SnapshotMetadata(const ReadOnlyCiphertext& ciphertext, const std::string& label) {
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");

    MetadataSnapshot snapshot;
    snapshot.reserve(metadata->size());
    for (const auto& [key, value] : *metadata) {
        Check(value != nullptr, label + " metadata value is null");
        auto clone = value->Clone();
        Check(clone != nullptr, label + " metadata clone is null");
        snapshot.push_back({key, value, std::move(clone)});
    }
    return snapshot;
}

void CheckMetadataUnchanged(const ReadOnlyCiphertext& ciphertext, const MetadataSnapshot& before,
                            const std::string& label) {
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");
    Check(metadata->size() == before.size(), label + " metadata map size changed");

    auto current = metadata->begin();
    for (const auto& expected : before) {
        Check(current != metadata->end(), label + " metadata entry disappeared");
        Check(current->first == expected.key, label + " metadata key changed");
        Check(current->second != nullptr, label + " metadata value became null");
        Check(current->second.get() == expected.identity.get(), label + " metadata value identity changed");
        Check(*(current->second) == *(expected.value), label + " metadata value changed");
        ++current;
    }
    Check(current == metadata->end(), label + " metadata map gained trailing entries");
}

struct PolynomialSnapshot {
    DCRTPoly value;
    const void* aggregateParamsIdentity;
    Format aggregateFormat;
    std::vector<const void*> towerParamsIdentities;
    std::vector<Format> towerFormats;
};

PolynomialSnapshot SnapshotPolynomial(const DCRTPoly& polynomial, const std::string& label) {
    const auto& params = polynomial.GetParams();
    Check(params != nullptr, label + " aggregate parameters are null");

    PolynomialSnapshot snapshot{polynomial, params.get(), polynomial.GetFormat(), {}, {}};
    snapshot.towerParamsIdentities.reserve(polynomial.GetAllElements().size());
    snapshot.towerFormats.reserve(polynomial.GetAllElements().size());
    for (const auto& tower : polynomial.GetAllElements()) {
        Check(tower.GetParams() != nullptr, label + " tower parameters are null");
        snapshot.towerParamsIdentities.push_back(tower.GetParams().get());
        snapshot.towerFormats.push_back(tower.GetFormat());
    }
    return snapshot;
}

void CheckPolynomialUnchanged(const DCRTPoly& polynomial, const PolynomialSnapshot& before,
                              const std::string& label) {
    Check(polynomial == before.value, label + " polynomial value changed");
    Check(polynomial.GetParams() != nullptr, label + " aggregate parameters became null");
    Check(polynomial.GetParams().get() == before.aggregateParamsIdentity,
          label + " aggregate-parameter identity changed");
    Check(polynomial.GetFormat() == before.aggregateFormat, label + " aggregate format changed");
    Check(polynomial.GetAllElements().size() == before.towerParamsIdentities.size(),
          label + " tower count changed");
    for (std::size_t tower = 0; tower < polynomial.GetAllElements().size(); ++tower) {
        Check(polynomial.GetAllElements()[tower].GetParams() != nullptr,
              label + " tower parameters became null");
        Check(polynomial.GetAllElements()[tower].GetParams().get() == before.towerParamsIdentities[tower],
              label + " tower-parameter identity changed");
        Check(polynomial.GetAllElements()[tower].GetFormat() == before.towerFormats[tower],
              label + " tower format changed");
    }
}

using PolynomialVectorSnapshot = std::vector<PolynomialSnapshot>;

PolynomialVectorSnapshot SnapshotPolynomialVector(const std::vector<DCRTPoly>& polynomials,
                                                   const std::string& label) {
    PolynomialVectorSnapshot snapshot;
    snapshot.reserve(polynomials.size());
    for (std::size_t index = 0; index < polynomials.size(); ++index) {
        snapshot.push_back(SnapshotPolynomial(polynomials[index], label + " entry " + std::to_string(index)));
    }
    return snapshot;
}

void CheckPolynomialVectorUnchanged(const std::vector<DCRTPoly>& polynomials,
                                    const PolynomialVectorSnapshot& before,
                                    const std::string& label) {
    Check(polynomials.size() == before.size(), label + " vector length changed");
    for (std::size_t index = 0; index < polynomials.size(); ++index) {
        CheckPolynomialUnchanged(polynomials[index], before[index], label + " entry " + std::to_string(index));
    }
}

struct CiphertextSnapshot {
    ReadOnlyCiphertext identity;
    Ciphertext<DCRTPoly> value;
    lbcrypto::MetadataMap metadataMapIdentity;
    MetadataSnapshot metadata;
    PolynomialVectorSnapshot elements;
};

CiphertextSnapshot SnapshotCiphertext(const ReadOnlyCiphertext& ciphertext, const std::string& label) {
    Check(ciphertext != nullptr, label + " is null");
    return {
        ciphertext,
        ciphertext->Clone(),
        ciphertext->GetMetadataMap(),
        SnapshotMetadata(ciphertext, label),
        SnapshotPolynomialVector(ciphertext->GetElements(), label + " elements"),
    };
}

void CheckCiphertextUnchanged(const ReadOnlyCiphertext& ciphertext, const CiphertextSnapshot& before,
                              const std::string& label) {
    Check(ciphertext != nullptr, label + " became null");
    Check(ciphertext.get() == before.identity.get(), label + " ciphertext identity changed");
    Check(*ciphertext == *before.value, label + " ciphertext state changed");
    Check(ciphertext->GetMetadataMap() == before.metadataMapIdentity,
          label + " metadata-map identity changed");
    CheckMetadataUnchanged(ciphertext, before.metadata, label);
    CheckPolynomialVectorUnchanged(ciphertext->GetElements(), before.elements, label + " elements");
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
    return {
        SnapshotCiphertext(pair.GetHigh(), label + " high"),
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
        pair.GetComponentCount(),
    };
}

void CheckPairUnchanged(const CiphertextPair& pair, const PairSnapshot& before, const std::string& label) {
    CheckCiphertextUnchanged(pair.GetHigh(), before.high, label + " high");
    CheckCiphertextUnchanged(pair.GetLow(), before.low, label + " low");
    Check(pair.GetContextIdentity() == before.contextIdentity, label + " context manifest changed");
    Check(pair.GetDivisor() == before.divisor, label + " divisor manifest changed");
    Check(pair.GetOrderedModuli() == before.orderedModuli, label + " basis manifest changed");
    Check(pair.GetLevel() == before.level, label + " level manifest changed");
    Check(pair.GetPaperScale().inputRecordedScalingFactor == before.paperScale.inputRecordedScalingFactor,
          label + " paper input-recorded scale changed");
    Check(pair.GetPaperScale().divisor == before.paperScale.divisor,
          label + " paper divisor changed");
    Check(pair.GetPaperScale().approximateLogicalScalingFactor ==
              before.paperScale.approximateLogicalScalingFactor,
          label + " high logical scale changed");
    Check(pair.GetPaperScale().approximateRecombinedLogicalScalingFactor ==
              before.paperScale.approximateRecombinedLogicalScalingFactor,
          label + " recombined logical scale changed");
    Check(pair.GetRecordedScalingFactor() == before.recordedScalingFactor,
          label + " recorded scaling-factor manifest changed");
    Check(pair.GetNoiseScaleDegree() == before.noiseScaleDegree,
          label + " noise-scale degree manifest changed");
    Check(pair.GetLifecycle() == before.lifecycle, label + " lifecycle manifest changed");
    Check(pair.GetKeyTag() == before.keyTag, label + " key-tag manifest changed");
    Check(pair.GetSlots() == before.slots, label + " slots manifest changed");
    Check(pair.GetFormat() == before.format, label + " format manifest changed");
    Check(pair.GetComponentCount() == before.componentCount,
          label + " component-count manifest changed");
}

template <class Function>
void CheckThrowsExactInvalidArgument(Function&& function, const std::string& expectedMessage,
                                     const std::string& label) {
    bool threw = false;
    try {
        std::invoke(std::forward<Function>(function));
    }
    catch (const std::invalid_argument& exception) {
        Check(exception.what() == expectedMessage,
              label + " reported an unexpected diagnostic: " + exception.what());
        threw = true;
    }
    catch (const std::exception& exception) {
        throw TestFailure(label + " threw the wrong exception type: " + exception.what());
    }
    Check(threw, label + " did not fail fast");
}

using EvalMultKeyMap = std::map<std::string, std::vector<lbcrypto::EvalKey<DCRTPoly>>>;

class ScopedEvalMultKeyMapRestore final {
public:
    explicit ScopedEvalMultKeyMapRestore(EvalMultKeyMap& cache) : cache_(cache), snapshot_(cache) {}

    ~ScopedEvalMultKeyMapRestore() noexcept {
        cache_.swap(snapshot_);
    }

    ScopedEvalMultKeyMapRestore(const ScopedEvalMultKeyMapRestore&) = delete;
    ScopedEvalMultKeyMapRestore& operator=(const ScopedEvalMultKeyMapRestore&) = delete;

private:
    EvalMultKeyMap& cache_;
    EvalMultKeyMap snapshot_;
};

struct EvalKeyEntrySnapshot {
    bool isNull;
    const void* pointerIdentity;
    const lbcrypto::CryptoContextImpl<DCRTPoly>* contextIdentity;
    std::string keyTag;
    std::string concreteSubtype;
    bool isRelin;
    PolynomialVectorSnapshot a;
    PolynomialVectorSnapshot b;
};

using EvalKeyRowsSnapshot = std::map<std::string, std::vector<EvalKeyEntrySnapshot>>;

struct EvalKeyCacheSnapshot {
    const EvalMultKeyMap* mapIdentity;
    std::map<std::string, const std::vector<lbcrypto::EvalKey<DCRTPoly>>*> rowIdentities;
    EvalKeyRowsSnapshot rows;
};

EvalKeyCacheSnapshot SnapshotEvalKeyCache() {
    const auto& cache = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    EvalKeyCacheSnapshot snapshot{&cache, {}, {}};
    for (const auto& [tag, row] : cache) {
        snapshot.rowIdentities[tag] = &row;
        auto& output = snapshot.rows[tag];
        output.reserve(row.size());
        for (const auto& key : row) {
            if (!key) {
                output.push_back({true, nullptr, nullptr, {}, {}, false, {}, {}});
                continue;
            }
            const auto relin = std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(key);
            output.push_back({
                false,
                key.get(),
                key->GetCryptoContext().get(),
                key->GetKeyTag(),
                typeid(*key).name(),
                relin != nullptr,
                relin ? SnapshotPolynomialVector(relin->GetAVector(), "RS2 evaluation-key A")
                      : PolynomialVectorSnapshot{},
                relin ? SnapshotPolynomialVector(relin->GetBVector(), "RS2 evaluation-key B")
                      : PolynomialVectorSnapshot{},
            });
        }
    }
    return snapshot;
}

void CheckEvalKeyCacheUnchanged(const EvalKeyCacheSnapshot& before, const std::string& label) {
    const auto& cache = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(&cache == before.mapIdentity, label + " map identity changed");
    Check(cache.size() == before.rows.size(), label + " row count changed");

    auto actualRow = cache.begin();
    auto expectedRow = before.rows.begin();
    for (; expectedRow != before.rows.end(); ++expectedRow, ++actualRow) {
        Check(actualRow != cache.end() && actualRow->first == expectedRow->first,
              label + " row tag changed");
        Check(&actualRow->second == before.rowIdentities.at(expectedRow->first),
              label + " row-vector identity changed");
        Check(actualRow->second.size() == expectedRow->second.size(), label + " row length changed");
        for (std::size_t index = 0; index < expectedRow->second.size(); ++index) {
            const auto& expected = expectedRow->second[index];
            const auto& key = actualRow->second[index];
            Check((key == nullptr) == expected.isNull, label + " key nullness changed");
            if (expected.isNull) {
                continue;
            }
            Check(key.get() == expected.pointerIdentity, label + " key pointer identity changed");
            Check(key->GetCryptoContext().get() == expected.contextIdentity,
                  label + " key context identity changed");
            Check(key->GetKeyTag() == expected.keyTag, label + " key tag changed");
            Check(typeid(*key).name() == expected.concreteSubtype, label + " concrete subtype changed");
            const auto relin = std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(key);
            Check((relin != nullptr) == expected.isRelin, label + " relinearization subtype changed");
            if (relin) {
                CheckPolynomialVectorUnchanged(relin->GetAVector(), expected.a, label + " A vector");
                CheckPolynomialVectorUnchanged(relin->GetBVector(), expected.b, label + " B vector");
            }
        }
    }
    Check(actualRow == cache.end(), label + " gained trailing rows");
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
    BigInt product = 1;
    for (const auto& value : values) {
        product *= value;
    }
    return product;
}

BigInt Center(const BigInt& value, const BigInt& modulus) {
    BigInt centered = PositiveMod(value, modulus);
    if (centered > modulus / 2) {
        centered -= modulus;
    }
    return centered;
}

BigInt ReconstructCentered(const std::vector<BigInt>& residues, const std::vector<BigInt>& moduli) {
    Check(!moduli.empty(), "RS2 CRT oracle requires at least one modulus");
    Check(residues.size() == moduli.size(), "RS2 CRT oracle residue/modulus size mismatch");

    const BigInt modulus = Product(moduli);
    BigInt result = 0;
    for (std::size_t index = 0; index < moduli.size(); ++index) {
        const BigInt partial = modulus / moduli[index];
        result += PositiveMod(residues[index], moduli[index]) * partial *
                  ModInverse(partial, moduli[index]);
    }
    return Center(result, modulus);
}

// Textbook signed divide-and-round: choose the unique centered remainder in
// (-divisor/2, divisor/2], then divide exactly. No OpenFHE rescale helper is used.
BigInt CenteredRescaleQuotient(const BigInt& value, const BigInt& divisor) {
    Check(divisor > 0 && divisor % 2 == 1, "RS2 oracle divisor must be a positive odd integer");
    const BigInt centeredRemainder = Center(value, divisor);
    const BigInt numerator = value - centeredRemainder;
    Check(numerator % divisor == 0, "RS2 oracle centered quotient is not integral");
    return numerator / divisor;
}

std::vector<BigInt> ToBigModuli(const std::vector<NativeInteger>& moduli) {
    std::vector<BigInt> result;
    result.reserve(moduli.size());
    for (const auto& modulus : moduli) {
        result.emplace_back(modulus.ConvertToInt());
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

BigInt CoefficientResidue(const DCRTPoly& coefficientPolynomial, std::size_t tower,
                          std::size_t coefficient) {
    return BigInt(coefficientPolynomial.GetAllElements().at(tower).GetValues().at(coefficient).ConvertToInt());
}

BigInt ReconstructCoefficient(const DCRTPoly& coefficientPolynomial,
                              const std::vector<BigInt>& moduli,
                              std::size_t coefficient) {
    std::vector<BigInt> residues;
    residues.reserve(moduli.size());
    for (std::size_t tower = 0; tower < moduli.size(); ++tower) {
        residues.push_back(CoefficientResidue(coefficientPolynomial, tower, coefficient));
    }
    return ReconstructCentered(residues, moduli);
}

BigInt ReconstructRecombinedCoefficient(const DCRTPoly& highCoefficient,
                                        const DCRTPoly& lowCoefficient,
                                        const std::vector<BigInt>& moduli,
                                        const BigInt& qDiv,
                                        std::size_t coefficient) {
    std::vector<BigInt> residues;
    residues.reserve(moduli.size());
    for (std::size_t tower = 0; tower < moduli.size(); ++tower) {
        const BigInt high = CoefficientResidue(highCoefficient, tower, coefficient);
        const BigInt low = CoefficientResidue(lowCoefficient, tower, coefficient);
        residues.push_back(PositiveMod(qDiv * high + low, moduli[tower]));
    }
    return ReconstructCentered(residues, moduli);
}

DCRTPoly MakePolynomial(const std::shared_ptr<DCRTPoly::Params>& params,
                        const std::vector<BigInt>& coefficients) {
    Check(params != nullptr, "RS2 witness polynomial parameters are null");
    DCRTPoly result(params, Format::COEFFICIENT, true);
    const std::size_t ringDimension = params->GetRingDimension();
    Check(coefficients.size() == ringDimension, "RS2 witness coefficient vector has wrong length");

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

CryptoContext<DCRTPoly> MakeContext() {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(3);
    parameters.SetScalingModSize(30);
    parameters.SetFirstModSize(35);
    parameters.SetScalingTechnique(lbcrypto::FIXEDMANUAL);
    parameters.SetKeySwitchTechnique(lbcrypto::HYBRID);
    parameters.SetMaxRelinSkDeg(2);
    parameters.SetSecurityLevel(lbcrypto::HEStd_NotSet);
    parameters.SetRingDim(32);
    parameters.SetBatchSize(8);

    auto context = lbcrypto::GenCryptoContext(parameters);
    context->Enable(lbcrypto::PKE);
    context->Enable(lbcrypto::KEYSWITCH);
    context->Enable(lbcrypto::LEVELEDSHE);
    return context;
}

void CheckOrderedModuli(const std::vector<NativeInteger>& actual,
                        const std::vector<NativeInteger>& expected,
                        const std::string& label) {
    Check(actual.size() == expected.size(), label + " modulus count mismatch");
    for (std::size_t tower = 0; tower < expected.size(); ++tower) {
        Check(actual[tower] == expected[tower],
              label + " ordered modulus mismatch at tower " + std::to_string(tower));
    }
}

void CheckCiphertextState(const ReadOnlyCiphertext& ciphertext,
                          const CryptoContext<DCRTPoly>& context,
                          const std::vector<NativeInteger>& expectedModuli,
                          std::size_t expectedLevel,
                          std::size_t expectedDegree,
                          double expectedRecordedScalingFactor,
                          const std::string& expectedKeyTag,
                          std::uint32_t expectedSlots,
                          const std::string& label) {
    Check(ciphertext != nullptr, label + " is null");
    Check(ciphertext->GetCryptoContext().get() == context.get(), label + " context identity mismatch");
    Check(ciphertext->GetEncodingType() == lbcrypto::CKKS_PACKED_ENCODING,
          label + " encoding metadata mismatch");
    Check(ciphertext->NumberCiphertextElements() == 2, label + " component count mismatch");
    Check(ciphertext->GetLevel() == expectedLevel, label + " level mismatch");
    Check(ciphertext->GetNoiseScaleDeg() == expectedDegree, label + " noise-scale degree mismatch");
    Check(ciphertext->GetScalingFactor() == expectedRecordedScalingFactor,
          label + " recorded scaling factor mismatch");
    Check(ciphertext->GetKeyTag() == expectedKeyTag, label + " key tag mismatch");
    Check(ciphertext->GetSlots() == expectedSlots, label + " slots mismatch");
    Check(ciphertext->GetMetadataMap() != nullptr, label + " metadata map is null");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr, label + " context is not CKKS-RNS");
    const auto& expectedTowerParameters = parameters->GetElementParams()->GetParams();
    Check(expectedModuli.size() <= expectedTowerParameters.size(),
          label + " expected basis is longer than the context basis");

    for (std::size_t component = 0; component < ciphertext->GetElements().size(); ++component) {
        const auto& element = ciphertext->GetElements()[component];
        Check(element.GetFormat() == Format::EVALUATION,
              label + " component " + std::to_string(component) + " is not in Evaluation format");
        CheckOrderedModuli(GetNativeModuli(element), expectedModuli,
                           label + " component " + std::to_string(component));

        const auto& aggregateParameters = element.GetParams();
        Check(aggregateParameters != nullptr, label + " aggregate parameters are null");
        const auto& declaredTowerParameters = aggregateParameters->GetParams();
        Check(declaredTowerParameters.size() == expectedModuli.size(),
              label + " declared basis length is not the exact context prefix");

        for (std::size_t tower = 0; tower < element.GetAllElements().size(); ++tower) {
            const auto& actualTower = element.GetAllElements()[tower];
            Check(expectedTowerParameters[tower] != nullptr &&
                      declaredTowerParameters[tower] != nullptr && actualTower.GetParams() != nullptr,
                  label + " contains null tower parameters");
            Check(actualTower.GetFormat() == Format::EVALUATION,
                  label + " contains a non-Evaluation tower");
            Check(declaredTowerParameters[tower]->GetModulus() ==
                          expectedTowerParameters[tower]->GetModulus() &&
                      declaredTowerParameters[tower]->GetRootOfUnity() ==
                          expectedTowerParameters[tower]->GetRootOfUnity() &&
                      declaredTowerParameters[tower]->GetCyclotomicOrder() ==
                          expectedTowerParameters[tower]->GetCyclotomicOrder() &&
                      actualTower.GetModulus() == expectedTowerParameters[tower]->GetModulus() &&
                      actualTower.GetRootOfUnity() == expectedTowerParameters[tower]->GetRootOfUnity() &&
                      actualTower.GetCyclotomicOrder() ==
                          expectedTowerParameters[tower]->GetCyclotomicOrder(),
                  label + " tower parameters are not the exact context prefix at tower " +
                      std::to_string(tower));
        }
    }
}

void CheckRs2State(const CiphertextPair& input,
                   const CiphertextPair& output,
                   const CryptoContext<DCRTPoly>& context,
                   const std::string& label) {
    Check(input.GetOrderedModuli().size() >= 2, label + " input has too few active towers");
    const NativeInteger qL = input.GetOrderedModuli().back();
    const NativeInteger qDiv = input.GetDivisor();
    Check(qL.Mod(NativeInteger(2)) == NativeInteger(1), label + " q_l is not odd");
    Check(qDiv.Mod(NativeInteger(2)) == NativeInteger(1), label + " q_div is not odd");
    Check(qL != qDiv, label + " fixture does not distinguish q_l from q_div");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr, label + " context is not CKKS-RNS");
    const std::size_t factorIndex = input.GetOrderedModuli().size() - 1;
    const auto& fullTowerParameters = parameters->GetElementParams()->GetParams();
    Check(factorIndex < fullTowerParameters.size(), label + " mod-reduce factor index is out of range");
    Check(fullTowerParameters[factorIndex] != nullptr,
          label + " q_l tower parameters are null");
    Check(fullTowerParameters[factorIndex]->GetModulus() == qL,
          label + " q_l does not occupy the official factor index");
    Check(fullTowerParameters.back() != nullptr && fullTowerParameters.back()->GetModulus() == qDiv,
          label + " q_div is not the separate final full-basis tower");

    const double recordedFactorDivisor = parameters->GetModReduceFactor(factorIndex);
    Check(std::isfinite(recordedFactorDivisor) && recordedFactorDivisor > 0.0,
          label + " official recorded-factor divisor is not finite and positive");
    const double expectedRecorded = input.GetRecordedScalingFactor() / recordedFactorDivisor;
    const long double qLAsLongDouble = static_cast<long double>(qL.ConvertToInt());
    const long double expectedHighLogical =
        input.GetPaperScale().approximateLogicalScalingFactor / qLAsLongDouble;
    const long double expectedRecombinedLogical =
        input.GetPaperScale().approximateRecombinedLogicalScalingFactor / qLAsLongDouble;
    Check(std::isfinite(expectedRecorded) && expectedRecorded > 0.0,
          label + " expected recorded scaling factor is not finite and positive");
    Check(std::isfinite(expectedHighLogical) && std::isfinite(expectedRecombinedLogical),
          label + " expected logical scaling factors are not finite");
    const std::vector<NativeInteger> outputModuli(input.GetOrderedModuli().begin(),
                                                   input.GetOrderedModuli().end() - 1);

    Check(output.GetLifecycle() == PairLifecycle::RefreshRequired,
          label + " lifecycle is not RefreshRequired");
    Check(output.GetContextIdentity() == input.GetContextIdentity(), label + " context manifest changed");
    Check(output.GetContextIdentity() == context.get(), label + " context manifest is not bound context");
    Check(output.GetHigh().get() != input.GetHigh().get() &&
              output.GetHigh().get() != input.GetLow().get() &&
              output.GetLow().get() != input.GetHigh().get() &&
              output.GetLow().get() != input.GetLow().get(),
          label + " output aliases an input ciphertext");
    Check(output.GetHigh().get() != output.GetLow().get(),
          label + " output pair members alias each other");
    Check(output.GetDivisor() == qDiv, label + " q_div manifest changed");
    CheckOrderedModuli(output.GetOrderedModuli(), outputModuli, label + " output manifest");
    Check(output.GetLevel() == input.GetLevel() + 1 && output.GetLevel() == 2,
          label + " level transition is not 1 -> 2");
    Check(output.GetNoiseScaleDegree() == input.GetNoiseScaleDegree() - 1 &&
              output.GetNoiseScaleDegree() == 2,
          label + " noise-scale degree transition is not 3 -> 2");
    Check(output.GetRecordedScalingFactor() == expectedRecorded,
          label + " recorded scaling-factor transition mismatch");
    Check(output.GetPaperScale().inputRecordedScalingFactor == expectedRecorded,
          label + " paper current-recorded factor mismatch");
    Check(output.GetPaperScale().divisor == qDiv, label + " paper q_div changed");
    Check(output.GetPaperScale().approximateLogicalScalingFactor == expectedHighLogical,
          label + " high logical-scale transition mismatch");
    Check(output.GetPaperScale().approximateRecombinedLogicalScalingFactor ==
              expectedRecombinedLogical,
          label + " recombined logical-scale transition mismatch");
    Check(output.GetKeyTag() == input.GetKeyTag(), label + " key-tag manifest changed");
    Check(output.GetSlots() == input.GetSlots(), label + " slots manifest changed");
    Check(output.GetFormat() == Format::EVALUATION, label + " pair format changed");
    Check(output.GetComponentCount() == 2, label + " pair component count changed");

    CheckCiphertextState(output.GetHigh(), context, outputModuli, output.GetLevel(),
                         output.GetNoiseScaleDegree(), expectedRecorded, output.GetKeyTag(),
                         output.GetSlots(), label + " high");
    CheckCiphertextState(output.GetLow(), context, outputModuli, output.GetLevel(),
                         output.GetNoiseScaleDegree(), expectedRecorded, output.GetKeyTag(),
                         output.GetSlots(), label + " low");

}

void CheckIndependentRs2Oracle(const CiphertextPair& input,
                               const CiphertextPair& output,
                               DoubleCKKS& module,
                               const CryptoContext<DCRTPoly>& context,
                               bool requireMutationWitnesses,
                               const std::string& label) {
    const auto inputModuli = ToBigModuli(input.GetOrderedModuli());
    const auto outputModuli = ToBigModuli(output.GetOrderedModuli());
    Check(inputModuli.size() == outputModuli.size() + 1,
          label + " oracle basis transition is not one tower");
    const BigInt qL = inputModuli.back();
    const BigInt qDiv(input.GetDivisor().ConvertToInt());
    const BigInt outputCompositeModulus = Product(outputModuli);

    const auto recombinedOutput = module.RCB(output);
    CheckCiphertextState(recombinedOutput, context, output.GetOrderedModuli(), output.GetLevel(),
                         output.GetNoiseScaleDegree(), output.GetRecordedScalingFactor(),
                         output.GetKeyTag(), output.GetSlots(), label + " recombined output");

    Check(input.GetHigh()->GetElements().size() == 2 && input.GetLow()->GetElements().size() == 2,
          label + " input does not have two RLWE components per member");
    Check(output.GetHigh()->GetElements().size() == 2 && output.GetLow()->GetElements().size() == 2 &&
              recombinedOutput->GetElements().size() == 2,
          label + " output does not have two RLWE components per member");

    bool qDivAsQlMutationIsDistinguished = false;
    bool oneRescaleShortcutIsDistinguished = false;

    for (std::size_t component = 0; component < 2; ++component) {
        const auto inputHigh = ToCoefficient(input.GetHigh()->GetElements()[component]);
        const auto inputLow = ToCoefficient(input.GetLow()->GetElements()[component]);
        const auto outputHigh = ToCoefficient(output.GetHigh()->GetElements()[component]);
        const auto outputLow = ToCoefficient(output.GetLow()->GetElements()[component]);
        const auto outputRecombined = ToCoefficient(recombinedOutput->GetElements()[component]);
        const std::size_t ringDimension = inputHigh.GetParams()->GetRingDimension();
        Check(inputLow.GetParams()->GetRingDimension() == ringDimension,
              label + " input member ring dimensions differ");

        for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
            const BigInt high = ReconstructCoefficient(inputHigh, inputModuli, coefficient);
            const BigInt low = ReconstructCoefficient(inputLow, inputModuli, coefficient);
            const BigInt recombined = ReconstructRecombinedCoefficient(
                inputHigh, inputLow, inputModuli, qDiv, coefficient);
            const BigInt expectedHigh = CenteredRescaleQuotient(high, qL);
            const BigInt expectedRecombined = CenteredRescaleQuotient(recombined, qL);
            const BigInt expectedLow = expectedRecombined - qDiv * expectedHigh;

            if (PositiveMod(expectedHigh - CenteredRescaleQuotient(high, qDiv),
                            outputCompositeModulus) != 0) {
                qDivAsQlMutationIsDistinguished = true;
            }
            if (PositiveMod(expectedLow - CenteredRescaleQuotient(low, qL),
                            outputCompositeModulus) != 0) {
                oneRescaleShortcutIsDistinguished = true;
            }

            for (std::size_t tower = 0; tower < outputModuli.size(); ++tower) {
                const BigInt actualHigh = CoefficientResidue(outputHigh, tower, coefficient);
                const BigInt actualLow = CoefficientResidue(outputLow, tower, coefficient);
                const BigInt actualRecombined = CoefficientResidue(outputRecombined, tower, coefficient);
                Check(actualHigh == PositiveMod(expectedHigh, outputModuli[tower]),
                      label + " high oracle mismatch at component " + std::to_string(component) +
                          ", coefficient " + std::to_string(coefficient) + ", tower " +
                          std::to_string(tower));
                Check(actualLow == PositiveMod(expectedLow, outputModuli[tower]),
                      label + " low oracle mismatch at component " + std::to_string(component) +
                          ", coefficient " + std::to_string(coefficient) + ", tower " +
                          std::to_string(tower));
                Check(actualRecombined == PositiveMod(expectedRecombined, outputModuli[tower]),
                      label + " RCB(RS2) identity mismatch at component " +
                          std::to_string(component) + ", coefficient " +
                          std::to_string(coefficient) + ", tower " + std::to_string(tower));
            }
        }
    }

    if (requireMutationWitnesses) {
        Check(qDivAsQlMutationIsDistinguished,
              label + " lacks a coefficient that distinguishes q_div from q_l");
        Check(oneRescaleShortcutIsDistinguished,
              label + " lacks a coefficient that distinguishes two RS calls from RS(low)");
    }
}

std::vector<BigInt> BoundaryValues(const BigInt& qL, const BigInt& compositeModulus,
                                   std::size_t ringDimension) {
    const BigInt halfQl = qL / 2;
    const BigInt halfComposite = compositeModulus / 2;
    std::vector<BigInt> values{
        0,
        1,
        -1,
        halfQl,
        -halfQl,
        halfQl + 1,
        -(halfQl + 1),
        halfQl - 1,
        -(halfQl - 1),
        halfComposite,
        halfComposite - 1,
        -halfComposite,
        -halfComposite + 1,
        halfComposite + 1,
        -(halfComposite + 1),
    };
    for (std::size_t index = values.size(); index < ringDimension; ++index) {
        const BigInt magnitude = BigInt(17 * index + 3);
        values.push_back(index % 2 == 0 ? magnitude : -magnitude);
    }
    values.resize(ringDimension);
    return values;
}

// The pair is first created through DCP -> Tensor2 -> Relin2. This test-only
// mutation replaces only public-observable ciphertext coefficients; it neither
// constructs a pair nor calls a private production helper.
void InstallControlledWitnesses(CiphertextPair& pair) {
    Check(pair.GetLifecycle() == PairLifecycle::ReadyForRS2,
          "controlled RS2 fixture is not ReadyForRS2");
    Check(pair.GetHigh()->GetElements().size() == 2 && pair.GetLow()->GetElements().size() == 2,
          "controlled RS2 fixture does not have two components");

    const auto moduli = ToBigModuli(pair.GetOrderedModuli());
    const BigInt compositeModulus = Product(moduli);
    const BigInt qL = moduli.back();
    const BigInt qDiv(pair.GetDivisor().ConvertToInt());
    Check(qL != qDiv, "controlled RS2 fixture does not distinguish q_l and q_div");

    const std::size_t ringDimension = pair.GetHigh()->GetElements().front().GetParams()->GetRingDimension();
    auto high0 = BoundaryValues(qL, compositeModulus, ringDimension);
    auto recombined0 = high0;
    std::reverse(recombined0.begin(), recombined0.end());

    const BigInt smallerDivisor = qL < qDiv ? qL : qDiv;
    const BigInt qDivDistinguishingWitness = (smallerDivisor + 1) / 2;
    const BigInt oneRescaleHighWitness = (qL + 1) / 2;
    Check(ringDimension > 17, "controlled RS2 fixture ring dimension is too small");
    high0[15] = qDivDistinguishingWitness;
    recombined0[15] = 1;
    high0[16] = oneRescaleHighWitness;
    recombined0[16] = 0;

    auto high1 = high0;
    auto recombined1 = recombined0;
    std::reverse(high1.begin(), high1.end());
    std::rotate(recombined1.begin(), recombined1.begin() + 7, recombined1.end());
    for (std::size_t index = 1; index < ringDimension; index += 2) {
        high1[index] = -high1[index];
    }
    for (std::size_t index = 0; index < ringDimension; index += 3) {
        recombined1[index] = -recombined1[index];
    }

    const std::vector<std::vector<BigInt>> highCoefficients{high0, high1};
    const std::vector<std::vector<BigInt>> recombinedCoefficients{recombined0, recombined1};
    std::vector<std::vector<BigInt>> lowCoefficients(2, std::vector<BigInt>(ringDimension));
    for (std::size_t component = 0; component < 2; ++component) {
        for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
            lowCoefficients[component][coefficient] =
                recombinedCoefficients[component][coefficient] -
                qDiv * highCoefficients[component][coefficient];
        }
    }

    auto high = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetHigh());
    auto low = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetLow());
    Check(high != nullptr && low != nullptr, "controlled RS2 fixture ciphertext cast failed");
    std::vector<DCRTPoly> highElements;
    std::vector<DCRTPoly> lowElements;
    highElements.reserve(2);
    lowElements.reserve(2);
    for (std::size_t component = 0; component < 2; ++component) {
        highElements.push_back(MakePolynomial(high->GetElements()[component].GetParams(),
                                              highCoefficients[component]));
        lowElements.push_back(MakePolynomial(low->GetElements()[component].GetParams(),
                                             lowCoefficients[component]));
    }
    high->SetElements(std::move(highElements));
    low->SetElements(std::move(lowElements));
}

void CheckRs2Case(const CiphertextPair& input,
                  DoubleCKKS& module,
                  const CryptoContext<DCRTPoly>& context,
                  bool requireMutationWitnesses,
                  const std::string& label) {
    const auto inputBefore = SnapshotPair(input, label + " input");
    const auto cacheBefore = SnapshotEvalKeyCache();

    const auto output = module.RS2(input);

    CheckPairUnchanged(input, inputBefore, label + " input after RS2");
    CheckEvalKeyCacheUnchanged(cacheBefore, label + " evaluation-key cache after RS2");
    CheckRs2State(input, output, context, label + " state");
    CheckIndependentRs2Oracle(input, output, module, context, requireMutationWitnesses,
                              label + " oracle");
    CheckPairUnchanged(input, inputBefore, label + " input after output/oracle checks");
    CheckEvalKeyCacheUnchanged(cacheBefore, label + " evaluation-key cache after output/oracle checks");
}

void TestWrongLifecycleImmutability() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    auto plaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto input = context->Encrypt(plaintext, keys.publicKey);
    input->SetMetadataByKey("rs2-wrong-lifecycle-immutability",
                            std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    DoubleCKKS module(context);
    const auto pair = module.DCP(input);
    Check(pair.GetLifecycle() == PairLifecycle::ReadyForFirstMult,
          "RS2 wrong-lifecycle fixture did not construct ReadyForFirstMult");

    const auto before = SnapshotPair(pair, "RS2 wrong-lifecycle input");
    CheckThrowsExactInvalidArgument(
        [&] { (void)module.RS2(pair); },
        "DoubleCKKS: RS2 requires ReadyForRS2 input",
        "RS2 wrong-lifecycle guard");
    CheckPairUnchanged(pair, before, "RS2 wrong-lifecycle input after rejection");
}

void TestValidArithmeticStateImmutability() {
    auto& evaluationKeyCache = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeyCache.empty(),
          "RS2 valid fixture requires an initially empty evaluation-key cache");
    ScopedEvalMultKeyMapRestore restore(evaluationKeyCache);

    auto context = MakeContext();
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(
        std::vector<double>{0.25, -0.5, 1.0, -1.25}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(
        std::vector<double>{-0.75, 0.125, 0.5, 1.5}, 2, 0);
    auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);
    leftInput->SetMetadataByKey("rs2-valid-left",
                                std::make_shared<ImmutabilityProbeMetadata>("left-unchanged"));
    rightInput->SetMetadataByKey("rs2-valid-right",
                                 std::make_shared<ImmutabilityProbeMetadata>("right-unchanged"));

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);
    context->EvalMultKeyGen(keys.secretKey);
    auto readyForRs2 = module.Relin2(tensor);
    Check(readyForRs2.GetLifecycle() == PairLifecycle::ReadyForRS2,
          "RS2 valid fixture did not construct ReadyForRS2 through DCP -> Tensor2 -> Relin2");
    Check(readyForRs2.GetLevel() == 1 && readyForRs2.GetNoiseScaleDegree() == 3,
          "RS2 valid fixture has the wrong pre-RS2 state");
    Check(readyForRs2.GetOrderedModuli().size() == 3,
          "RS2 valid fixture must expose three active pre-RS2 towers");
    Check(!evaluationKeyCache.empty(),
          "RS2 valid fixture did not install official OpenFHE evaluation keys");

    CheckRs2Case(readyForRs2, module, context, false, "genuine public-pipeline RS2");

    InstallControlledWitnesses(readyForRs2);
    CheckRs2Case(readyForRs2, module, context, true, "controlled-boundary RS2");
}

using TestFunction = void (*)();

TestFunction ResolveTest(const std::string& name) {
    if (name == "wrong_lifecycle_immutability") {
        return &TestWrongLifecycleImmutability;
    }
    if (name == "valid_arithmetic_state_immutability") {
        return &TestValidArithmeticStateImmutability;
    }
    throw TestFailure("unknown RS2 test case: " + name);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "RS2 test failure: expected exactly one case name\n";
        return 2;
    }

    try {
        ResolveTest(argv[1])();
        lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
        std::cout << "RS2 case passed: " << argv[1] << '\n';
        return 0;
    }
    catch (const TestFailure& failure) {
        std::cerr << "RS2 test failure: " << failure.what() << '\n';
        return 1;
    }
    catch (const std::exception& exception) {
        std::cerr << "RS2 unexpected exception: " << exception.what() << '\n';
        return 1;
    }
}
