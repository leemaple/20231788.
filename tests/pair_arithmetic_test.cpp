#include "openfhe.h"
#include "openfhe_2023_1788/double_ckks.h"

#include <boost/multiprecision/cpp_int.hpp>

#include <algorithm>
#include <complex>
#include <cstddef>
#include <cstdint>
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
    std::uint32_t batchSize = 8,
    lbcrypto::CKKSDataType dataType = lbcrypto::REAL) {
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
    parameters.SetCKKSDataType(dataType);

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

void CheckMemberStructureDerivedFrom(const ReadOnlyCiphertext& result,
                                     const CiphertextSnapshot& source,
                                     const std::string& label) {
    Check(result != nullptr, label + " result is null");
    Check(result->GetCryptoContext().get() == source.contextIdentity, label + " context changed");
    Check(result->GetEncodingType() == source.encoding, label + " encoding changed");
    Check(result->GetLevel() == source.level, label + " level changed");
    Check(result->GetNoiseScaleDeg() == source.noiseScaleDegree,
          label + " noise-scale degree changed");
    Check(result->GetScalingFactor() == source.scalingFactor,
          label + " recorded scaling factor changed");
    Check(result->GetKeyTag() == source.keyTag, label + " key tag changed");
    Check(result->GetSlots() == source.slots, label + " slots changed");
    Check(result->GetElements().size() == source.elements.size(),
          label + " component count changed");
    for (std::size_t component = 0; component < source.elements.size(); ++component) {
        const auto actual = SnapshotDcrt(result->GetElements().at(component));
        const auto& expected = source.elements[component];
        Check(actual.paramsIdentity == expected.paramsIdentity,
              label + " aggregate parameter provenance changed at component=" +
                  std::to_string(component));
        Check(actual.format == expected.format && actual.towers.size() == expected.towers.size(),
              label + " DCRT structure changed at component=" + std::to_string(component));
        for (std::size_t tower = 0; tower < expected.towers.size(); ++tower) {
            const auto& a = actual.towers[tower];
            const auto& e = expected.towers[tower];
            Check(a.paramsIdentity == e.paramsIdentity && a.modulus == e.modulus &&
                      a.root == e.root && a.cyclotomicOrder == e.cyclotomicOrder &&
                      a.format == e.format,
                  label + " tower parameter provenance changed at component=" +
                      std::to_string(component) + ",tower=" + std::to_string(tower));
        }
    }
    CheckMetadataDerivedFrom(result, source.metadata, label);
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
    CheckMemberStructureDerivedFrom(result.GetHigh(), leftBefore.high, label + " high");
    CheckMemberStructureDerivedFrom(result.GetLow(), leftBefore.low, label + " low");
    Check(result.GetHigh()->GetMetadataMap().get() != result.GetLow()->GetMetadataMap().get(),
          label + " high/low metadata maps alias");
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

void TagPairMembers(const CiphertextPair& pair, const std::string& prefix) {
    auto high = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetHigh());
    auto low = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(pair.GetLow());
    Check(high != nullptr && low != nullptr, prefix + " tagging encountered a null pair member");
    high->SetMetadataByKey(prefix + "-high", std::make_shared<ProbeMetadata>(prefix + "-high-value"));
    low->SetMetadataByKey(prefix + "-low", std::make_shared<ProbeMetadata>(prefix + "-low-value"));
}

struct ContextParameterSnapshot {
    const void* cryptoParametersIdentity;
    const void* elementParametersIdentity;
    std::vector<const void*> towerParameterIdentities;
    std::vector<NativeInteger> moduli;
    std::vector<NativeInteger> roots;
    std::vector<std::uint32_t> cyclotomicOrders;
};

ContextParameterSnapshot SnapshotContextParameters(const CryptoContext<DCRTPoly>& context,
                                                   const std::string& label) {
    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr, label + " context parameters are not CKKS-RNS");
    const auto elementParameters = parameters->GetElementParams();
    Check(elementParameters != nullptr, label + " element parameters are null");
    ContextParameterSnapshot snapshot{parameters.get(), elementParameters.get(), {}, {}, {}, {}};
    for (const auto& tower : elementParameters->GetParams()) {
        Check(tower != nullptr, label + " has a null tower parameter");
        snapshot.towerParameterIdentities.push_back(tower.get());
        snapshot.moduli.push_back(tower->GetModulus());
        snapshot.roots.push_back(tower->GetRootOfUnity());
        snapshot.cyclotomicOrders.push_back(tower->GetCyclotomicOrder());
    }
    return snapshot;
}

void CheckContextParametersUnchanged(const CryptoContext<DCRTPoly>& context,
                                     const ContextParameterSnapshot& expected,
                                     const std::string& label) {
    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr && parameters.get() == expected.cryptoParametersIdentity,
          label + " crypto-parameter identity changed");
    const auto elementParameters = parameters->GetElementParams();
    Check(elementParameters != nullptr && elementParameters.get() == expected.elementParametersIdentity,
          label + " element-parameter identity changed");
    Check(elementParameters->GetParams().size() == expected.moduli.size(),
          label + " tower-parameter count changed");
    for (std::size_t tower = 0; tower < expected.moduli.size(); ++tower) {
        const auto& actual = elementParameters->GetParams().at(tower);
        Check(actual != nullptr && actual.get() == expected.towerParameterIdentities[tower],
              label + " tower-parameter identity changed at tower=" + std::to_string(tower));
        Check(actual->GetModulus() == expected.moduli[tower] &&
                  actual->GetRootOfUnity() == expected.roots[tower] &&
                  actual->GetCyclotomicOrder() == expected.cyclotomicOrders[tower],
              label + " tower-parameter value changed at tower=" + std::to_string(tower));
    }
}

using EvalMultKeyMap = std::map<std::string, std::vector<lbcrypto::EvalKey<DCRTPoly>>>;

struct EvalKeyEntrySnapshot {
    bool isNull;
    const void* pointerIdentity;
    const lbcrypto::CryptoContextImpl<DCRTPoly>* contextIdentity;
    std::string keyTag;
    std::string concreteType;
    bool isRelinearizationKey;
    std::vector<DcrtSnapshot> a;
    std::vector<DcrtSnapshot> b;
};

struct EvalKeyCacheSnapshot {
    const EvalMultKeyMap* mapIdentity;
    std::map<std::string, const std::vector<lbcrypto::EvalKey<DCRTPoly>>*> rowIdentities;
    std::map<std::string, std::vector<EvalKeyEntrySnapshot>> rows;
};

std::vector<DcrtSnapshot> SnapshotDcrtVector(const std::vector<DCRTPoly>& values) {
    std::vector<DcrtSnapshot> result;
    result.reserve(values.size());
    for (const auto& value : values) {
        result.push_back(SnapshotDcrt(value));
    }
    return result;
}

EvalKeyCacheSnapshot SnapshotEvalKeyCache() {
    const auto& cache = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    EvalKeyCacheSnapshot snapshot{&cache, {}, {}};
    for (const auto& [tag, row] : cache) {
        snapshot.rowIdentities[tag] = &row;
        auto& entries = snapshot.rows[tag];
        entries.reserve(row.size());
        for (const auto& key : row) {
            if (!key) {
                entries.push_back({true, nullptr, nullptr, {}, {}, false, {}, {}});
                continue;
            }
            EvalKeyEntrySnapshot entry{false,
                                       key.get(),
                                       key->GetCryptoContext().get(),
                                       key->GetKeyTag(),
                                       typeid(*key).name(),
                                       false,
                                       {},
                                       {}};
            const auto relinearizationKey =
                std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(key);
            if (relinearizationKey) {
                entry.isRelinearizationKey = true;
                entry.a = SnapshotDcrtVector(relinearizationKey->GetAVector());
                entry.b = SnapshotDcrtVector(relinearizationKey->GetBVector());
            }
            entries.push_back(std::move(entry));
        }
    }
    return snapshot;
}

void CheckDcrtVectorUnchanged(const std::vector<DCRTPoly>& actual,
                              const std::vector<DcrtSnapshot>& expected,
                              const std::string& label) {
    Check(actual.size() == expected.size(), label + " vector length changed");
    for (std::size_t index = 0; index < expected.size(); ++index) {
        Check(SameDcrtValueAndParameters(SnapshotDcrt(actual[index]), expected[index]),
              label + " changed at index=" + std::to_string(index));
    }
}

void CheckEvalKeyCacheUnchanged(const EvalKeyCacheSnapshot& expected,
                                const std::string& label) {
    const auto& cache = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(&cache == expected.mapIdentity, label + " cache map identity changed");
    Check(cache.size() == expected.rows.size(), label + " cache row count changed");
    auto actualRow = cache.begin();
    auto expectedRow = expected.rows.begin();
    for (; expectedRow != expected.rows.end(); ++expectedRow, ++actualRow) {
        Check(actualRow != cache.end() && actualRow->first == expectedRow->first,
              label + " cache row tag changed");
        Check(&actualRow->second == expected.rowIdentities.at(expectedRow->first),
              label + " cache row-vector identity changed");
        Check(actualRow->second.size() == expectedRow->second.size(),
              label + " cache row length changed");
        for (std::size_t index = 0; index < expectedRow->second.size(); ++index) {
            const auto& actual = actualRow->second[index];
            const auto& entry = expectedRow->second[index];
            Check((actual == nullptr) == entry.isNull, label + " cache key nullness changed");
            if (entry.isNull) {
                continue;
            }
            Check(actual.get() == entry.pointerIdentity, label + " cache key identity changed");
            Check(actual->GetCryptoContext().get() == entry.contextIdentity,
                  label + " cache key context changed");
            Check(actual->GetKeyTag() == entry.keyTag, label + " cache key tag changed");
            Check(typeid(*actual).name() == entry.concreteType,
                  label + " cache concrete key type changed");
            const auto relinearizationKey =
                std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(actual);
            Check((relinearizationKey != nullptr) == entry.isRelinearizationKey,
                  label + " cache relinearization-key classification changed");
            if (relinearizationKey) {
                CheckDcrtVectorUnchanged(relinearizationKey->GetAVector(), entry.a,
                                         label + " cache A-vector");
                CheckDcrtVectorUnchanged(relinearizationKey->GetBVector(), entry.b,
                                         label + " cache B-vector");
            }
        }
    }
    Check(actualRow == cache.end(), label + " cache gained trailing rows");
}

void ExercisePublicLifecycle(DoubleCKKS& module,
                             const CiphertextPair& left,
                             const CiphertextPair& right,
                             PairLifecycle expectedLifecycle,
                             const EvalKeyCacheSnapshot& keyCache,
                             const std::string& label) {
    Check(left.GetLifecycle() == expectedLifecycle && right.GetLifecycle() == expectedLifecycle,
          label + " fixture lifecycle mismatch");
    Check(HasNonzeroValue(left.GetHigh()) && HasNonzeroValue(left.GetLow()) &&
              HasNonzeroValue(right.GetHigh()) && HasNonzeroValue(right.GetLow()),
          label + " untouched public fixture contains a zero pair member");
    const auto leftBefore = SnapshotPair(left, label + " left");
    const auto rightBefore = SnapshotPair(right, label + " right");

    const auto sum = module.Add(left, right);
    CheckMemberOracle(left.GetHigh(), right.GetHigh(), sum.GetHigh(), ArithmeticKind::Add,
                      label + " Add high");
    CheckMemberOracle(left.GetLow(), right.GetLow(), sum.GetLow(), ArithmeticKind::Add,
                      label + " Add low");
    CheckRcbOracle(module, left, right, sum, ArithmeticKind::Add, label + " Add");
    CheckResultManifestAndProvenance(sum, left, leftBefore, rightBefore, label + " Add");
    CheckEvalKeyCacheUnchanged(keyCache, label + " after Add and RCB");

    const auto difference = module.Sub(left, right);
    CheckMemberOracle(left.GetHigh(), right.GetHigh(), difference.GetHigh(), ArithmeticKind::Sub,
                      label + " Sub high");
    CheckMemberOracle(left.GetLow(), right.GetLow(), difference.GetLow(), ArithmeticKind::Sub,
                      label + " Sub low");
    CheckRcbOracle(module, left, right, difference, ArithmeticKind::Sub, label + " Sub");
    CheckResultManifestAndProvenance(difference, left, leftBefore, rightBefore, label + " Sub");
    CheckEvalKeyCacheUnchanged(keyCache, label + " after Sub and RCB");

    CheckPairUnchanged(left, leftBefore, label + " left after arithmetic");
    CheckPairUnchanged(right, rightBefore, label + " right after arithmetic");
}

void TestPublicLifecyclesAndKeyIndependence() {
    auto guardContext = MakeContext(lbcrypto::BV);
    const auto guardKeys = guardContext->KeyGen();
    guardContext->EvalMultKeyGen(guardKeys.secretKey);
    const std::string guardTag = guardKeys.secretKey->GetKeyTag();

    auto context = MakeContext(lbcrypto::HYBRID, 8, lbcrypto::COMPLEX);
    Check(context->GetCKKSDataType() == lbcrypto::COMPLEX,
          "public-pipeline context must preserve complex slots");
    const auto keys = context->KeyGen();
    context->EvalMultKeyGen(keys.secretKey);
    const std::string fixtureTag = keys.secretKey->GetKeyTag();
    Check(!guardTag.empty() && !fixtureTag.empty() && guardTag != fixtureTag,
          "evaluation-key fixture tags are empty or collide");

    const std::vector<double> leftValues{
        0.25, -0.125, 0.5, 0x1p-20, -0.75, 0.375, 0.25, -0x1p-18};
    const std::vector<std::complex<double>> rightValues{
        {-0.5, 0.125}, {0.25, -0.25}, {0.125, 0.0}, {-0x1p-19, 0x1p-20},
        {0.5, -0.375}, {-0.0625, 0.5}, {0.25, 0.0}, {0.0, -0.125}};
    const auto leftInput = context->Encrypt(
        context->MakeCKKSPackedPlaintext(leftValues, 2, 0), keys.publicKey);
    const auto rightPlaintext = context->MakeCKKSPackedPlaintext(rightValues, 2, 0);
    const auto& rightCachedValues = rightPlaintext->GetCKKSPackedValue();
    Check(!rightCachedValues.empty() && rightCachedValues.front().imag() == 0.125,
          "public-pipeline plaintext constructor discarded the imaginary witness");
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);
    const auto leftInputBefore = SnapshotCiphertext(leftInput, "public-pipeline left ciphertext");
    const auto rightInputBefore = SnapshotCiphertext(rightInput, "public-pipeline right ciphertext");

    DoubleCKKS module(context);
    const auto firstLeft = module.DCP(leftInput);
    const auto firstRight = module.DCP(rightInput);
    const auto readyLeft = module.Relin2(module.Tensor2(firstLeft, firstLeft));
    const auto readyRight = module.Relin2(module.Tensor2(firstRight, firstRight));
    const auto refreshLeft = module.RS2(readyLeft);
    const auto refreshRight = module.RS2(readyRight);

    TagPairMembers(firstLeft, "public-first-left");
    TagPairMembers(firstRight, "public-first-right");
    TagPairMembers(readyLeft, "public-ready-left");
    TagPairMembers(readyRight, "public-ready-right");
    TagPairMembers(refreshLeft, "public-refresh-left");
    TagPairMembers(refreshRight, "public-refresh-right");

    const auto& cacheBeforeClear = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(cacheBeforeClear.find(fixtureTag) != cacheBeforeClear.end(),
          "fixture evaluation-key row is absent before targeted removal");
    Check(cacheBeforeClear.find(guardTag) != cacheBeforeClear.end(),
          "unrelated guard evaluation-key row is absent before targeted removal");
    lbcrypto::CryptoContextImpl<DCRTPoly>::ClearEvalMultKeys(fixtureTag);
    const auto& cacheAfterClear = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(cacheAfterClear.find(fixtureTag) == cacheAfterClear.end(),
          "targeted fixture evaluation-key row was not removed");
    Check(cacheAfterClear.find(guardTag) != cacheAfterClear.end(),
          "targeted removal erased the unrelated guard evaluation-key row");
    const auto keyCache = SnapshotEvalKeyCache();
    // Start the context-parameter immutability window only after lifecycle
    // preparation and targeted key removal, so any failure is attributable to
    // Add/Sub/RCB rather than to the already-existing preparation pipeline.
    const auto contextBefore = SnapshotContextParameters(context, "public-pipeline");
    const auto guardContextBefore = SnapshotContextParameters(guardContext, "guard");

    ExercisePublicLifecycle(module, firstLeft, firstRight, PairLifecycle::ReadyForFirstMult,
                            keyCache, "ReadyForFirstMult");
    ExercisePublicLifecycle(module, readyLeft, readyRight, PairLifecycle::ReadyForRS2,
                            keyCache, "ReadyForRS2");
    ExercisePublicLifecycle(module, refreshLeft, refreshRight, PairLifecycle::RefreshRequired,
                            keyCache, "RefreshRequired");

    CheckCiphertextUnchanged(leftInput, leftInputBefore, "public-pipeline left ciphertext");
    CheckCiphertextUnchanged(rightInput, rightInputBefore, "public-pipeline right ciphertext");
    CheckContextParametersUnchanged(context, contextBefore, "public-pipeline context");
    CheckContextParametersUnchanged(guardContext, guardContextBefore, "guard context");
    CheckEvalKeyCacheUnchanged(keyCache, "final pair-arithmetic key cache");

    lbcrypto::CryptoContextImpl<DCRTPoly>::ClearEvalMultKeys(guardTag);
}

template <class Function>
void CheckExactInvalidArgument(Function&& function,
                               const std::string& expectedMessage,
                               const std::string& label) {
    bool threw = false;
    try {
        std::invoke(std::forward<Function>(function));
    }
    catch (const std::invalid_argument& exception) {
        Check(exception.what() == expectedMessage,
              label + " diagnostic mismatch: " + std::string(exception.what()));
        threw = true;
    }
    catch (const std::exception& exception) {
        throw TestFailure(label + " threw the wrong exception type: " + exception.what());
    }
    Check(threw, label + " did not reject");
}

void CheckBothRejectWithoutMutation(DoubleCKKS& module,
                                    const CiphertextPair& left,
                                    const CiphertextPair& right,
                                    const std::string& addMessage,
                                    const std::string& subMessage,
                                    const std::string& label) {
    const auto leftBefore = SnapshotPair(left, label + " left");
    const auto rightBefore = SnapshotPair(right, label + " right");
    CheckExactInvalidArgument([&] { (void)module.Add(left, right); }, addMessage,
                              label + " Add");
    CheckPairUnchanged(left, leftBefore, label + " left after Add rejection");
    CheckPairUnchanged(right, rightBefore, label + " right after Add rejection");
    CheckExactInvalidArgument([&] { (void)module.Sub(left, right); }, subMessage,
                              label + " Sub");
    CheckPairUnchanged(left, leftBefore, label + " left after Sub rejection");
    CheckPairUnchanged(right, rightBefore, label + " right after Sub rejection");
}

void TestCompatibilityAndMalformedRejections() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    context->EvalMultKeyGen(keys.secretKey);
    const auto plaintext =
        context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5, 0.75}, 2, 0);
    DoubleCKKS module(context);
    const auto makePair = [&]() {
        return module.DCP(context->Encrypt(plaintext, keys.publicKey));
    };

    const auto firstForLifecycle = makePair();
    const auto otherFirst = makePair();
    const auto readyForLifecycle =
        module.Relin2(module.Tensor2(otherFirst, otherFirst));
    (void)module.RCB(firstForLifecycle);
    (void)module.RCB(readyForLifecycle);

    auto secondKeys = context->KeyGen();
    const auto secondKeyPair =
        module.DCP(context->Encrypt(plaintext, secondKeys.publicKey));
    const auto primaryKeyPair = makePair();
    (void)module.RCB(primaryKeyPair);
    (void)module.RCB(secondKeyPair);
    Check(primaryKeyPair.GetKeyTag() != secondKeyPair.GetKeyTag(),
          "key-tag rejection fixture did not create distinct key tags");

    const auto slotLeftInput = context->Encrypt(plaintext, keys.publicKey);
    const auto fourSlotPlaintext = context->MakeCKKSPackedPlaintext(
        std::vector<double>{0.25, -0.5, 0.75}, 2, 0, nullptr, 4);
    const auto slotRightInput = context->Encrypt(fourSlotPlaintext, keys.publicKey);
    Check(slotLeftInput->GetSlots() == 8 && slotRightInput->GetSlots() == 4,
          "slot rejection fixture must use genuine eight-slot and four-slot encodings");
    const auto slotLeft = module.DCP(slotLeftInput);
    const auto slotRight = module.DCP(slotRightInput);
    (void)module.RCB(slotLeft);
    (void)module.RCB(slotRight);
    Check(slotLeft.GetSlots() != slotRight.GetSlots(),
          "slot rejection fixture did not create distinct slot counts");

    auto foreignContext = MakeContext(lbcrypto::HYBRID, 4);
    Check(foreignContext.get() != context.get(),
          "foreign-context rejection fixture must use a distinct CryptoContext identity");
    const auto foreignKeys = foreignContext->KeyGen();
    DoubleCKKS foreignModule(foreignContext);
    const auto foreignPair = foreignModule.DCP(
        foreignContext->Encrypt(
            foreignContext->MakeCKKSPackedPlaintext(std::vector<double>{-0.25, 0.5}, 2, 0),
            foreignKeys.publicKey));
    const auto localPairForForeign = makePair();

    const auto contextBefore = SnapshotContextParameters(context, "rejection context");
    const auto foreignContextBefore = SnapshotContextParameters(foreignContext, "foreign rejection context");
    const auto keyCache = SnapshotEvalKeyCache();

    CheckBothRejectWithoutMutation(
        module, firstForLifecycle, readyForLifecycle,
        "DoubleCKKS: Add input lifecycles do not match",
        "DoubleCKKS: Sub input lifecycles do not match", "mixed lifecycle");
    CheckBothRejectWithoutMutation(
        module, primaryKeyPair, secondKeyPair,
        "DoubleCKKS: Add input key tags do not match",
        "DoubleCKKS: Sub input key tags do not match", "genuine key-tag mismatch");
    CheckBothRejectWithoutMutation(
        module, slotLeft, slotRight,
        "DoubleCKKS: Add input slots do not match",
        "DoubleCKKS: Sub input slots do not match", "genuine slot mismatch");
    CheckBothRejectWithoutMutation(
        module, localPairForForeign, foreignPair,
        "DoubleCKKS: pair belongs to a different context",
        "DoubleCKKS: pair belongs to a different context", "foreign right context");

    {
        auto left = makePair();
        auto right = makePair();
        auto& divisor = const_cast<NativeInteger&>(right.GetDivisor());
        divisor += NativeInteger(2);
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair divisor does not match the bound context",
            "DoubleCKKS: pair divisor does not match the bound context",
            "right divisor manifest corruption");
    }
    {
        auto left = makePair();
        auto right = makePair();
        auto& moduli = const_cast<std::vector<NativeInteger>&>(right.GetOrderedModuli());
        moduli.front() += NativeInteger(2);
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair ordered RNS basis does not match its level",
            "DoubleCKKS: pair ordered RNS basis does not match its level",
            "right ordered-basis manifest corruption");
    }
    {
        auto left = makePair();
        auto right = makePair();
        auto& scale = const_cast<PaperScaleDescriptor&>(right.GetPaperScale());
        scale.inputRecordedScalingFactor *= 2.0;
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair paper-scale descriptor is inconsistent",
            "DoubleCKKS: pair paper-scale descriptor is inconsistent",
            "right paper-recorded scale corruption");
    }
    {
        auto left = makePair();
        auto right = makePair();
        auto& scale = const_cast<PaperScaleDescriptor&>(right.GetPaperScale());
        scale.divisor += NativeInteger(2);
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair paper-scale descriptor is inconsistent",
            "DoubleCKKS: pair paper-scale descriptor is inconsistent",
            "right paper divisor corruption");
    }
    {
        auto left = makePair();
        auto right = makePair();
        auto& scale = const_cast<PaperScaleDescriptor&>(right.GetPaperScale());
        scale.approximateLogicalScalingFactor *= 2.0L;
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair paper-scale descriptor is inconsistent",
            "DoubleCKKS: pair paper-scale descriptor is inconsistent",
            "right logical scale corruption");
    }
    {
        auto left = makePair();
        auto right = makePair();
        auto& scale = const_cast<PaperScaleDescriptor&>(right.GetPaperScale());
        scale.approximateRecombinedLogicalScalingFactor *= 2.0L;
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair recombined logical scale is inconsistent",
            "DoubleCKKS: pair recombined logical scale is inconsistent",
            "right recombined logical scale corruption");
    }
    {
        auto left = makePair();
        auto right = makePair();
        auto high = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(right.GetHigh());
        high->SetLevel(0);
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair high level does not match its pair state",
            "DoubleCKKS: pair high level does not match its pair state",
            "right member level corruption");
    }
    {
        auto left = makePair();
        auto right = makePair();
        auto low = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(right.GetLow());
        low->SetNoiseScaleDeg(1);
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair low noise-scale degree does not match its pair state",
            "DoubleCKKS: pair low noise-scale degree does not match its pair state",
            "right member degree corruption");
    }
    {
        auto left = makePair();
        auto right = makePair();
        auto low = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(right.GetLow());
        low->SetScalingFactor(low->GetScalingFactor() * 2.0);
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair low recorded scaling factor does not match its pair state",
            "DoubleCKKS: pair low recorded scaling factor does not match its pair state",
            "right member recorded-scale corruption");
    }
    {
        auto left = makePair();
        auto right = makePair();
        auto& keyTag = const_cast<std::string&>(right.GetKeyTag());
        keyTag = "corrupt-pair-manifest-tag";
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair high key tag does not match its pair state",
            "DoubleCKKS: pair high key tag does not match its pair state",
            "right key-tag manifest corruption");
    }
    {
        auto left = makePair();
        auto right = makePair();
        auto high = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(right.GetHigh());
        high->SetSlots(high->GetSlots() + 1);
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair high slots do not match its pair state",
            "DoubleCKKS: pair high slots do not match its pair state",
            "right member slots corruption");
    }
    {
        auto left = makePair();
        auto right = makePair();
        auto low = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(right.GetLow());
        low->SetEncodingType(lbcrypto::PACKED_ENCODING);
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair low must use CKKS packed encoding metadata",
            "DoubleCKKS: pair low must use CKKS packed encoding metadata",
            "right member encoding corruption");
    }
    {
        auto left = makePair();
        auto right = makePair();
        auto high = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(right.GetHigh());
        high->GetElements().push_back(high->GetElements().front());
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair high must contain exactly two RLWE components",
            "DoubleCKKS: pair high must contain exactly two RLWE components",
            "right member component-count corruption");
    }
    {
        auto left = makePair();
        auto right = makePair();
        auto low = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(right.GetLow());
        low->GetElements().at(0).GetAllElements().at(0).SetFormat(Format::COEFFICIENT);
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair low tower must be in evaluation format",
            "DoubleCKKS: pair low tower must be in evaluation format",
            "right member tower-format corruption");
    }
    {
        auto left = makePair();
        auto right = secondKeyPair;
        auto& scale = const_cast<PaperScaleDescriptor&>(right.GetPaperScale());
        scale.approximateLogicalScalingFactor *= 2.0L;
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair paper-scale descriptor is inconsistent",
            "DoubleCKKS: pair paper-scale descriptor is inconsistent",
            "right validation precedes mutual key-tag compatibility");
    }
    {
        auto left = makePair();
        auto right = makePair();
        auto& scale = const_cast<PaperScaleDescriptor&>(left.GetPaperScale());
        scale.approximateRecombinedLogicalScalingFactor *= 2.0L;
        auto& rightScale = const_cast<PaperScaleDescriptor&>(right.GetPaperScale());
        rightScale.approximateLogicalScalingFactor *= 2.0L;
        CheckBothRejectWithoutMutation(
            module, left, right,
            "DoubleCKKS: pair recombined logical scale is inconsistent",
            "DoubleCKKS: pair recombined logical scale is inconsistent",
            "left validation precedes right validation and compatibility");
    }

    CheckContextParametersUnchanged(context, contextBefore, "rejection context");
    CheckContextParametersUnchanged(foreignContext, foreignContextBefore,
                                    "foreign rejection context");
    CheckEvalKeyCacheUnchanged(keyCache, "rejection evaluation-key cache");
    lbcrypto::CryptoContextImpl<DCRTPoly>::ClearEvalMultKeys(keys.secretKey->GetKeyTag());
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
    if (name == "public_lifecycles_keyless") {
        return &TestPublicLifecyclesAndKeyIndependence;
    }
    if (name == "compatibility_rejections") {
        return &TestCompatibilityAndMalformedRejections;
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
