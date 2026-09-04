#include "openfhe.h"
#include "openfhe_2023_1788/double_ckks.h"

#include <boost/multiprecision/cpp_int.hpp>

#include <algorithm>
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

struct MetadataSnapshot {
    lbcrypto::MetadataMap mapIdentity;
    std::vector<MetadataEntrySnapshot> entries;
};

MetadataSnapshot SnapshotMetadata(const ReadOnlyCiphertext& ciphertext, const std::string& label) {
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");
    MetadataSnapshot snapshot{metadata, {}};
    snapshot.entries.reserve(metadata->size());
    for (const auto& [key, value] : *metadata) {
        Check(value != nullptr, label + " metadata value is null");
        auto deepValue = value->Clone();
        Check(deepValue != nullptr, label + " metadata clone is null");
        snapshot.entries.push_back({key, value, std::move(deepValue)});
    }
    return snapshot;
}

void CheckMetadataUnchanged(const ReadOnlyCiphertext& ciphertext,
                            const MetadataSnapshot& expected,
                            const std::string& label) {
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");
    Check(metadata.get() == expected.mapIdentity.get(), label + " metadata map identity changed");
    Check(metadata->size() == expected.entries.size(), label + " metadata map size changed");
    auto actual = metadata->begin();
    for (const auto& entry : expected.entries) {
        Check(actual != metadata->end(), label + " metadata entry disappeared");
        Check(actual->first == entry.key, label + " metadata key or order changed");
        Check(actual->second.get() == entry.identity.get(), label + " metadata value identity changed");
        Check(*(actual->second) == *(entry.deepValue), label + " metadata deep value changed");
        ++actual;
    }
    Check(actual == metadata->end(), label + " metadata gained trailing entries");
}

void CheckMetadataDerivedFrom(const ReadOnlyCiphertext& ciphertext,
                              const MetadataSnapshot& source,
                              const std::string& label) {
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");
    Check(metadata.get() != source.mapIdentity.get(), label + " metadata map aliases its source");
    Check(metadata->size() == source.entries.size(), label + " metadata map size mismatch");
    auto actual = metadata->begin();
    for (const auto& entry : source.entries) {
        Check(actual != metadata->end(), label + " metadata entry disappeared");
        Check(actual->first == entry.key, label + " metadata key or order mismatch");
        Check(actual->second.get() == entry.identity.get(),
              label + " metadata value did not preserve left-member provenance");
        Check(*(actual->second) == *(entry.deepValue), label + " metadata deep value mismatch");
        ++actual;
    }
    Check(actual == metadata->end(), label + " metadata gained trailing entries");
}

struct TowerSnapshot {
    const void* paramsIdentity;
    NativeInteger modulus;
    NativeInteger root;
    std::uint32_t cyclotomicOrder;
    Format format;
    std::vector<std::uint64_t> values;
};

struct DcrtSnapshot {
    const void* paramsIdentity;
    Format format;
    std::vector<TowerSnapshot> towers;
};

DcrtSnapshot SnapshotDcrt(const DCRTPoly& polynomial) {
    DcrtSnapshot snapshot{polynomial.GetParams().get(), polynomial.GetFormat(), {}};
    snapshot.towers.reserve(polynomial.GetAllElements().size());
    for (const auto& tower : polynomial.GetAllElements()) {
        TowerSnapshot towerSnapshot{tower.GetParams().get(), tower.GetModulus(), tower.GetRootOfUnity(),
                                    tower.GetCyclotomicOrder(), tower.GetFormat(), {}};
        towerSnapshot.values.reserve(tower.GetValues().GetLength());
        for (std::size_t index = 0; index < tower.GetValues().GetLength(); ++index) {
            towerSnapshot.values.push_back(tower.GetValues().at(index).ConvertToInt());
        }
        snapshot.towers.push_back(std::move(towerSnapshot));
    }
    return snapshot;
}

bool SameDcrtValueAndParameters(const DcrtSnapshot& left, const DcrtSnapshot& right) {
    if (left.paramsIdentity != right.paramsIdentity || left.format != right.format ||
        left.towers.size() != right.towers.size()) {
        return false;
    }
    for (std::size_t index = 0; index < left.towers.size(); ++index) {
        const auto& a = left.towers[index];
        const auto& b = right.towers[index];
        if (a.paramsIdentity != b.paramsIdentity || a.modulus != b.modulus || a.root != b.root ||
            a.cyclotomicOrder != b.cyclotomicOrder || a.format != b.format || a.values != b.values) {
            return false;
        }
    }
    return true;
}

struct CiphertextSnapshot {
    ReadOnlyCiphertext identity;
    Ciphertext<DCRTPoly> valueClone;
    const lbcrypto::CryptoContextImpl<DCRTPoly>* contextIdentity;
    lbcrypto::PlaintextEncodings encoding;
    std::size_t level;
    std::size_t noiseScaleDegree;
    double scalingFactor;
    std::string keyTag;
    std::uint32_t slots;
    MetadataSnapshot metadata;
    std::vector<DcrtSnapshot> elements;
};

CiphertextSnapshot SnapshotCiphertext(const ReadOnlyCiphertext& ciphertext,
                                      const std::string& label) {
    Check(ciphertext != nullptr, label + " is null");
    CiphertextSnapshot snapshot{ciphertext,
                                ciphertext->Clone(),
                                ciphertext->GetCryptoContext().get(),
                                ciphertext->GetEncodingType(),
                                ciphertext->GetLevel(),
                                ciphertext->GetNoiseScaleDeg(),
                                ciphertext->GetScalingFactor(),
                                ciphertext->GetKeyTag(),
                                ciphertext->GetSlots(),
                                SnapshotMetadata(ciphertext, label),
                                {}};
    snapshot.elements.reserve(ciphertext->GetElements().size());
    for (const auto& element : ciphertext->GetElements()) {
        snapshot.elements.push_back(SnapshotDcrt(element));
    }
    return snapshot;
}

void CheckCiphertextUnchanged(const ReadOnlyCiphertext& ciphertext,
                              const CiphertextSnapshot& expected,
                              const std::string& label) {
    Check(ciphertext != nullptr && expected.identity != nullptr && expected.valueClone != nullptr,
          label + " ciphertext or snapshot is null");
    Check(ciphertext.get() == expected.identity.get(), label + " ciphertext identity changed");
    Check(ciphertext->GetCryptoContext().get() == expected.contextIdentity,
          label + " context identity changed");
    Check(ciphertext->GetEncodingType() == expected.encoding, label + " encoding changed");
    Check(ciphertext->GetLevel() == expected.level, label + " level changed");
    Check(ciphertext->GetNoiseScaleDeg() == expected.noiseScaleDegree,
          label + " noise-scale degree changed");
    Check(ciphertext->GetScalingFactor() == expected.scalingFactor,
          label + " recorded scaling factor changed");
    Check(ciphertext->GetKeyTag() == expected.keyTag, label + " key tag changed");
    Check(ciphertext->GetSlots() == expected.slots, label + " slots changed");
    Check(*ciphertext == *expected.valueClone, label + " ciphertext semantic value changed");
    Check(ciphertext->GetElements().size() == expected.elements.size(),
          label + " component count changed");
    for (std::size_t component = 0; component < expected.elements.size(); ++component) {
        Check(SameDcrtValueAndParameters(SnapshotDcrt(ciphertext->GetElements().at(component)),
                                         expected.elements[component]),
              label + " DCRT component or pointed-to parameters changed at component=" +
                  std::to_string(component));
    }
    CheckMetadataUnchanged(ciphertext, expected.metadata, label);
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
                        const PairSnapshot& expected,
                        const std::string& label) {
    CheckCiphertextUnchanged(pair.GetHigh(), expected.high, label + " high");
    CheckCiphertextUnchanged(pair.GetLow(), expected.low, label + " low");
    Check(pair.GetContextIdentity() == expected.contextIdentity, label + " context manifest changed");
    Check(pair.GetDivisor() == expected.divisor, label + " divisor manifest changed");
    Check(pair.GetOrderedModuli() == expected.orderedModuli, label + " basis manifest changed");
    Check(pair.GetLevel() == expected.level, label + " level manifest changed");
    Check(pair.GetPaperScale().inputRecordedScalingFactor ==
              expected.paperScale.inputRecordedScalingFactor,
          label + " paper recorded factor changed");
    Check(pair.GetPaperScale().divisor == expected.paperScale.divisor,
          label + " paper divisor changed");
    Check(pair.GetPaperScale().approximateLogicalScalingFactor ==
              expected.paperScale.approximateLogicalScalingFactor,
          label + " logical scale changed");
    Check(pair.GetPaperScale().approximateRecombinedLogicalScalingFactor ==
              expected.paperScale.approximateRecombinedLogicalScalingFactor,
          label + " recombined logical scale changed");
    Check(pair.GetRecordedScalingFactor() == expected.recordedScalingFactor,
          label + " recorded scaling factor changed");
    Check(pair.GetNoiseScaleDegree() == expected.noiseScaleDegree,
          label + " noise-scale degree changed");
    Check(pair.GetLifecycle() == expected.lifecycle, label + " lifecycle changed");
    Check(pair.GetKeyTag() == expected.keyTag, label + " key tag changed");
    Check(pair.GetSlots() == expected.slots, label + " slots changed");
    Check(pair.GetFormat() == expected.format, label + " format changed");
    Check(pair.GetComponentCount() == expected.componentCount,
          label + " component count changed");
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
    Check(gcd == 1, "pair-arithmetic CRT oracle received non-coprime moduli");
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
          "pair-arithmetic CRT oracle residue/modulus shape mismatch");
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
    return BigInt(
        coefficientPolynomial.GetAllElements().at(tower).GetValues().at(coefficient).ConvertToInt());
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

DCRTPoly MakePolynomial(const std::shared_ptr<DCRTPoly::Params>& params,
                        const std::vector<BigInt>& coefficients) {
    DCRTPoly result(params, Format::COEFFICIENT, true);
    const std::size_t ringDimension = params->GetRingDimension();
    Check(coefficients.size() == ringDimension,
          "pair-arithmetic controlled coefficient vector has wrong ring dimension");
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

CryptoContext<DCRTPoly> MakeContext(
    lbcrypto::KeySwitchTechnique keySwitchTechnique = lbcrypto::HYBRID,
    std::uint32_t batchSize = 8) {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(3);
    parameters.SetScalingModSize(30);
    parameters.SetFirstModSize(35);
    parameters.SetScalingTechnique(lbcrypto::FIXEDMANUAL);
    parameters.SetKeySwitchTechnique(keySwitchTechnique);
    parameters.SetDigitSize(0);
    parameters.SetSecurityLevel(lbcrypto::HEStd_NotSet);
    parameters.SetRingDim(32);
    parameters.SetBatchSize(batchSize);

    auto context = lbcrypto::GenCryptoContext(parameters);
    context->Enable(lbcrypto::PKE);
    context->Enable(lbcrypto::KEYSWITCH);
    context->Enable(lbcrypto::LEVELEDSHE);
    return context;
}

struct SignedWitness {
    BigInt left;
    BigInt right;
};

std::vector<SignedWitness> BoundaryWitnesses(const BigInt& activeModulus) {
    const BigInt half = activeModulus / 2;
    return {
        {0, 0},
        {1, 1},
        {-1, -1},
        {half - 1, 1},
        {half, 1},
        {half, -1},
        {-half + 1, -1},
        {-half, -1},
        {-half, 1},
        {half - 2, 3},
        {-half + 2, -3},
        {2, -3},
        {-5, 7},
    };
}

std::vector<BigInt> SelectWitnessSide(const std::vector<SignedWitness>& witnesses,
                                      bool selectLeft,
                                      std::size_t ringDimension,
                                      std::size_t phase) {
    Check(ringDimension >= witnesses.size(),
          "pair-arithmetic ring dimension is too small for boundary witnesses");
    std::vector<BigInt> values(ringDimension);
    for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
        const auto& witness = witnesses[(coefficient + phase) % witnesses.size()];
        values[coefficient] = selectLeft ? witness.left : witness.right;
    }
    return values;
}

void InstallControlledPair(const CiphertextPair& pair,
                           bool selectLeft,
                           std::size_t phaseBase,
                           const std::string& prefix) {
    auto high = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetHigh());
    auto low = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetLow());
    Check(high != nullptr && low != nullptr, prefix + " contains a null pair member");
    const auto params = high->GetElements().front().GetParams();
    Check(params != nullptr && *params == *low->GetElements().front().GetParams(),
          prefix + " high/low bases differ");
    const auto moduli = GetModuli(high->GetElements().front());
    const auto witnesses = BoundaryWitnesses(Product(moduli));
    const std::size_t ringDimension = params->GetRingDimension();

    std::vector<DCRTPoly> highElements;
    std::vector<DCRTPoly> lowElements;
    highElements.reserve(2);
    lowElements.reserve(2);
    for (std::size_t component = 0; component < 2; ++component) {
        highElements.push_back(MakePolynomial(
            params, SelectWitnessSide(witnesses, selectLeft, ringDimension,
                                      phaseBase + component * 3)));
        lowElements.push_back(MakePolynomial(
            params, SelectWitnessSide(witnesses, selectLeft, ringDimension,
                                      phaseBase + component * 5 + 7)));
    }
    high->SetElements(std::move(highElements));
    low->SetElements(std::move(lowElements));
    high->SetMetadataByKey(prefix + "-high", std::make_shared<ProbeMetadata>(prefix + "-high-value"));
    low->SetMetadataByKey(prefix + "-low", std::make_shared<ProbeMetadata>(prefix + "-low-value"));
}

enum class ArithmeticKind {
    Add,
    Sub,
};

BigInt ExpectedArithmetic(const BigInt& left, const BigInt& right, ArithmeticKind kind) {
    if (kind == ArithmeticKind::Add) {
        return BigInt(left + right);
    }
    return BigInt(left - right);
}

void CheckMemberOracle(const ReadOnlyCiphertext& left,
                       const ReadOnlyCiphertext& right,
                       const ReadOnlyCiphertext& actual,
                       ArithmeticKind kind,
                       const std::string& label) {
    Check(left != nullptr && right != nullptr && actual != nullptr, label + " has a null ciphertext");
    Check(left->GetElements().size() == 2 && right->GetElements().size() == 2 &&
              actual->GetElements().size() == 2,
          label + " does not have exactly two RLWE components");
    for (std::size_t component = 0; component < 2; ++component) {
        const auto leftCoefficient = ToCoefficient(left->GetElements().at(component));
        const auto rightCoefficient = ToCoefficient(right->GetElements().at(component));
        const auto actualCoefficient = ToCoefficient(actual->GetElements().at(component));
        const auto moduli = GetModuli(leftCoefficient);
        Check(moduli == GetModuli(rightCoefficient) && moduli == GetModuli(actualCoefficient),
              label + " basis mismatch in independent oracle");
        const std::size_t ringDimension = leftCoefficient.GetParams()->GetRingDimension();
        for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
            const BigInt leftSigned = ReconstructCoefficient(left->GetElements().at(component), coefficient);
            const BigInt rightSigned = ReconstructCoefficient(right->GetElements().at(component), coefficient);
            const BigInt expected = ExpectedArithmetic(leftSigned, rightSigned, kind);
            for (std::size_t tower = 0; tower < moduli.size(); ++tower) {
                const BigInt actualResidue =
                    CoefficientResidue(actualCoefficient, tower, coefficient);
                const BigInt expectedResidue = PositiveMod(expected, moduli[tower]);
                Check(actualResidue == expectedResidue,
                      label + " residue mismatch at component=" + std::to_string(component) +
                          ",tower=" + std::to_string(tower) +
                          ",coefficient=" + std::to_string(coefficient));
            }
        }
    }
}

void CheckRcbOracle(DoubleCKKS& module,
                    const CiphertextPair& left,
                    const CiphertextPair& right,
                    const CiphertextPair& actual,
                    ArithmeticKind kind,
                    const std::string& label) {
    const auto recombined = module.RCB(actual);
    Check(recombined != nullptr && recombined->GetElements().size() == 2,
          label + " public RCB returned an invalid ciphertext");
    const BigInt divisor(actual.GetDivisor().ConvertToInt());
    for (std::size_t component = 0; component < 2; ++component) {
        const auto actualCoefficient = ToCoefficient(recombined->GetElements().at(component));
        const auto moduli = GetModuli(actualCoefficient);
        const std::size_t ringDimension = actualCoefficient.GetParams()->GetRingDimension();
        for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
            const BigInt leftHigh = ReconstructCoefficient(left.GetHigh()->GetElements().at(component), coefficient);
            const BigInt rightHigh =
                ReconstructCoefficient(right.GetHigh()->GetElements().at(component), coefficient);
            const BigInt leftLow = ReconstructCoefficient(left.GetLow()->GetElements().at(component), coefficient);
            const BigInt rightLow = ReconstructCoefficient(right.GetLow()->GetElements().at(component), coefficient);
            const BigInt expected =
                divisor * ExpectedArithmetic(leftHigh, rightHigh, kind) +
                ExpectedArithmetic(leftLow, rightLow, kind);
            for (std::size_t tower = 0; tower < moduli.size(); ++tower) {
                Check(CoefficientResidue(actualCoefficient, tower, coefficient) ==
                          PositiveMod(expected, moduli[tower]),
                      label + " RCB mismatch at component=" + std::to_string(component) +
                          ",tower=" + std::to_string(tower) +
                          ",coefficient=" + std::to_string(coefficient));
            }
        }
    }
}

void CheckResultManifestAndProvenance(const CiphertextPair& result,
                                      const CiphertextPair& left,
                                      const PairSnapshot& leftBefore,
                                      const PairSnapshot& rightBefore,
                                      const std::string& label) {
    Check(result.GetContextIdentity() == leftBefore.contextIdentity, label + " context changed");
    Check(result.GetDivisor() == leftBefore.divisor, label + " divisor changed");
    Check(result.GetOrderedModuli() == leftBefore.orderedModuli, label + " basis changed");
    Check(result.GetLevel() == leftBefore.level, label + " level changed");
    Check(result.GetPaperScale().inputRecordedScalingFactor ==
              leftBefore.paperScale.inputRecordedScalingFactor,
          label + " paper recorded factor changed");
    Check(result.GetPaperScale().divisor == leftBefore.paperScale.divisor,
          label + " paper divisor changed");
    Check(result.GetPaperScale().approximateLogicalScalingFactor ==
              leftBefore.paperScale.approximateLogicalScalingFactor,
          label + " logical scale changed");
    Check(result.GetPaperScale().approximateRecombinedLogicalScalingFactor ==
              leftBefore.paperScale.approximateRecombinedLogicalScalingFactor,
          label + " recombined logical scale changed");
    Check(result.GetRecordedScalingFactor() == leftBefore.recordedScalingFactor,
          label + " recorded scaling factor changed");
    Check(result.GetNoiseScaleDegree() == leftBefore.noiseScaleDegree,
          label + " noise-scale degree changed");
    Check(result.GetLifecycle() == leftBefore.lifecycle, label + " lifecycle changed");
    Check(result.GetKeyTag() == leftBefore.keyTag, label + " key tag changed");
    Check(result.GetSlots() == leftBefore.slots, label + " slots changed");
    Check(result.GetFormat() == leftBefore.format, label + " format changed");
    Check(result.GetComponentCount() == leftBefore.componentCount,
          label + " component count changed");

    Check(result.GetHigh().get() != left.GetHigh().get(), label + " high aliases left high");
    Check(result.GetLow().get() != left.GetLow().get(), label + " low aliases left low");
    Check(result.GetHigh().get() != rightBefore.high.identity.get(), label + " high aliases right high");
    Check(result.GetLow().get() != rightBefore.low.identity.get(), label + " low aliases right low");
    Check(result.GetHigh().get() != result.GetLow().get(), label + " high and low outputs alias");
    CheckMetadataDerivedFrom(result.GetHigh(), leftBefore.high.metadata, label + " high");
    CheckMetadataDerivedFrom(result.GetLow(), leftBefore.low.metadata, label + " low");
}

bool SameCiphertextElements(const ReadOnlyCiphertext& left, const ReadOnlyCiphertext& right) {
    if (left == nullptr || right == nullptr ||
        left->GetElements().size() != right->GetElements().size()) {
        return false;
    }
    for (std::size_t component = 0; component < left->GetElements().size(); ++component) {
        if (!(left->GetElements().at(component) == right->GetElements().at(component))) {
            return false;
        }
    }
    return true;
}

void CheckZeroCiphertext(const ReadOnlyCiphertext& ciphertext, const std::string& label) {
    Check(ciphertext != nullptr, label + " is null");
    for (std::size_t component = 0; component < ciphertext->GetElements().size(); ++component) {
        const auto coefficient = ToCoefficient(ciphertext->GetElements().at(component));
        for (std::size_t tower = 0; tower < coefficient.GetAllElements().size(); ++tower) {
            for (std::size_t index = 0;
                 index < coefficient.GetAllElements().at(tower).GetValues().GetLength(); ++index) {
                Check(CoefficientResidue(coefficient, tower, index) == 0,
                      label + " is nonzero at component=" + std::to_string(component) +
                          ",tower=" + std::to_string(tower) +
                          ",coefficient=" + std::to_string(index));
            }
        }
    }
}

bool HasNonzeroValue(const ReadOnlyCiphertext& ciphertext) {
    if (ciphertext == nullptr) {
        return false;
    }
    for (const auto& element : ciphertext->GetElements()) {
        for (const auto& tower : element.GetAllElements()) {
            for (std::size_t index = 0; index < tower.GetValues().GetLength(); ++index) {
                if (tower.GetValues().at(index) != NativeInteger(0)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Test-owned controlled coefficients check exact modular arithmetic, not slot precision.
void TestControlledOracleAndAliases() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    const auto zero = context->MakeCKKSPackedPlaintext(std::vector<double>{0.0}, 2, 0);
    const auto leftInput = context->Encrypt(zero, keys.publicKey);
    const auto rightInput = context->Encrypt(zero, keys.publicKey);
    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    InstallControlledPair(left, true, 0, "controlled-left");
    InstallControlledPair(right, false, 0, "controlled-right");
    Check(HasNonzeroValue(left.GetHigh()) && HasNonzeroValue(left.GetLow()) &&
              HasNonzeroValue(right.GetHigh()) && HasNonzeroValue(right.GetLow()),
          "controlled boundary fixture contains a zero pair member");
    Check(!SameCiphertextElements(left.GetHigh(), left.GetLow()) &&
              !SameCiphertextElements(right.GetHigh(), right.GetLow()),
          "controlled boundary fixture did not distinguish high from low members");

    const auto leftBefore = SnapshotPair(left, "controlled left");
    const auto rightBefore = SnapshotPair(right, "controlled right");

    const auto sum = module.Add(left, right);
    CheckMemberOracle(left.GetHigh(), right.GetHigh(), sum.GetHigh(), ArithmeticKind::Add,
                      "controlled Add high");
    CheckMemberOracle(left.GetLow(), right.GetLow(), sum.GetLow(), ArithmeticKind::Add,
                      "controlled Add low");
    CheckRcbOracle(module, left, right, sum, ArithmeticKind::Add, "controlled Add");
    CheckResultManifestAndProvenance(sum, left, leftBefore, rightBefore, "controlled Add");

    const auto reverseSum = module.Add(right, left);
    CheckMemberOracle(right.GetHigh(), left.GetHigh(), reverseSum.GetHigh(), ArithmeticKind::Add,
                      "reverse Add high");
    CheckMemberOracle(right.GetLow(), left.GetLow(), reverseSum.GetLow(), ArithmeticKind::Add,
                      "reverse Add low");
    CheckRcbOracle(module, right, left, reverseSum, ArithmeticKind::Add, "reverse Add");
    Check(SameCiphertextElements(sum.GetHigh(), reverseSum.GetHigh()) &&
              SameCiphertextElements(sum.GetLow(), reverseSum.GetLow()),
          "Add is not commutative at the pair-value seam");

    const auto selfSum = module.Add(left, left);
    CheckMemberOracle(left.GetHigh(), left.GetHigh(), selfSum.GetHigh(), ArithmeticKind::Add,
                      "self Add high");
    CheckMemberOracle(left.GetLow(), left.GetLow(), selfSum.GetLow(), ArithmeticKind::Add,
                      "self Add low");
    CheckRcbOracle(module, left, left, selfSum, ArithmeticKind::Add, "self Add");
    CheckResultManifestAndProvenance(selfSum, left, leftBefore, leftBefore, "self Add alias");

    const auto difference = module.Sub(left, right);
    CheckMemberOracle(left.GetHigh(), right.GetHigh(), difference.GetHigh(), ArithmeticKind::Sub,
                      "controlled Sub high");
    CheckMemberOracle(left.GetLow(), right.GetLow(), difference.GetLow(), ArithmeticKind::Sub,
                      "controlled Sub low");
    CheckRcbOracle(module, left, right, difference, ArithmeticKind::Sub, "controlled Sub");
    CheckResultManifestAndProvenance(difference, left, leftBefore, rightBefore, "controlled Sub");

    const auto reverseDifference = module.Sub(right, left);
    CheckMemberOracle(right.GetHigh(), left.GetHigh(), reverseDifference.GetHigh(), ArithmeticKind::Sub,
                      "reverse Sub high");
    CheckMemberOracle(right.GetLow(), left.GetLow(), reverseDifference.GetLow(), ArithmeticKind::Sub,
                      "reverse Sub low");
    CheckRcbOracle(module, right, left, reverseDifference, ArithmeticKind::Sub, "reverse Sub");
    Check(!SameCiphertextElements(difference.GetHigh(), reverseDifference.GetHigh()) ||
              !SameCiphertextElements(difference.GetLow(), reverseDifference.GetLow()),
          "Sub order was not distinguished by the controlled witness");

    const auto selfDifference = module.Sub(left, left);
    CheckMemberOracle(left.GetHigh(), left.GetHigh(), selfDifference.GetHigh(), ArithmeticKind::Sub,
                      "self Sub high");
    CheckMemberOracle(left.GetLow(), left.GetLow(), selfDifference.GetLow(), ArithmeticKind::Sub,
                      "self Sub low");
    CheckRcbOracle(module, left, left, selfDifference, ArithmeticKind::Sub, "self Sub");
    CheckZeroCiphertext(selfDifference.GetHigh(), "self Sub high");
    CheckZeroCiphertext(selfDifference.GetLow(), "self Sub low");
    CheckResultManifestAndProvenance(selfDifference, left, leftBefore, leftBefore,
                                     "self Sub alias");

    CheckPairUnchanged(left, leftBefore, "controlled left after all arithmetic");
    CheckPairUnchanged(right, rightBefore, "controlled right after all arithmetic");
}

using TestFunction = void (*)();

TestFunction ResolveTest(const std::string& name) {
    if (name == "controlled_oracle") {
        return &TestControlledOracleAndAliases;
    }
    throw TestFailure("unknown pair-arithmetic test case: " + name);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "pair-arithmetic test failure: expected exactly one case name\n";
        return 2;
    }
    try {
        ResolveTest(argv[1])();
        lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
        std::cout << "pair-arithmetic case passed: " << argv[1] << '\n';
        return 0;
    }
    catch (const TestFailure& failure) {
        std::cerr << "pair-arithmetic test failure: " << failure.what() << '\n';
        return 1;
    }
    catch (const std::exception& exception) {
        std::cerr << "pair-arithmetic unexpected exception: " << exception.what() << '\n';
        return 1;
    }
}
