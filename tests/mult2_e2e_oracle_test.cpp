#include "openfhe.h"
#include "openfhe_2023_1788/double_ckks.h"

#include <boost/multiprecision/cpp_int.hpp>

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
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
using openfhe_2023_1788::PairLifecycle;
using openfhe_2023_1788::ReadOnlyCiphertext;
using openfhe_2023_1788::TensorCiphertextPair;

constexpr std::uint32_t kRingDimension = 64;
constexpr std::uint32_t kBatchSize = 16;
constexpr std::uint32_t kScalingModSize = 30;
constexpr std::uint32_t kFirstModSize = 35;
constexpr std::uint32_t kMultiplicativeDepth = 7;
constexpr std::uint32_t kDigitSize = 0;
constexpr std::uint32_t kMaxRelinSkDeg = 2;
constexpr std::size_t kEncodingNoiseScaleDegree = 2;
constexpr std::uint32_t kEncodingLevel = 0;
constexpr std::uint32_t kSelectionReserveBits = 32;

// This threshold was frozen by the behavior-red patch before Mult2 was
// implemented. It is only a decoded-slot absolute-error threshold after the
// documented logical-scale correction; it is not a bit-precision or security
// claim.
constexpr double kLogicalDecodedAbsoluteTolerance = 1.0e-3;

class TestFailure final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void Check(bool condition, const std::string& message) {
    if (!condition) {
        throw TestFailure(message);
    }
}

BigInt Abs(const BigInt& value) {
    return value < 0 ? -value : value;
}

BigInt Max(const BigInt& left, const BigInt& right) {
    return left < right ? right : left;
}

BigInt PositiveMod(BigInt value, const BigInt& modulus) {
    value %= modulus;
    if (value < 0) {
        value += modulus;
    }
    return value;
}

BigInt Center(const BigInt& residue, const BigInt& modulus) {
    BigInt centered = PositiveMod(residue, modulus);
    if (centered > modulus / 2) {
        centered -= modulus;
    }
    return centered;
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
    Check(gcd == 1, "CRT oracle received non-coprime moduli");
    return PositiveMod(x, modulus);
}

BigInt Product(const std::vector<BigInt>& values) {
    BigInt result = 1;
    for (const auto& value : values) {
        result *= value;
    }
    return result;
}

std::size_t BitLength(const BigInt& value) {
    Check(value > 0, "bit-length input must be positive");
    return static_cast<std::size_t>(boost::multiprecision::msb(value)) + 1;
}

BigInt ReconstructCentered(const std::vector<BigInt>& residues,
                           const std::vector<BigInt>& moduli) {
    Check(!moduli.empty() && residues.size() == moduli.size(),
          "CRT oracle residue/modulus shape mismatch");
    const BigInt modulus = Product(moduli);
    BigInt result = 0;
    for (std::size_t index = 0; index < moduli.size(); ++index) {
        const BigInt partial = modulus / moduli[index];
        result += PositiveMod(residues[index], moduli[index]) * partial *
                  ModInverse(partial, moduli[index]);
    }
    return Center(result, modulus);
}

DCRTPoly ToCoefficient(const DCRTPoly& polynomial) {
    DCRTPoly result(polynomial);
    result.SetFormat(Format::COEFFICIENT);
    return result;
}

std::vector<BigInt> GetModuli(const DCRTPoly& polynomial) {
    std::vector<BigInt> result;
    result.reserve(polynomial.GetAllElements().size());
    for (const auto& tower : polynomial.GetAllElements()) {
        result.emplace_back(tower.GetModulus().ConvertToInt());
    }
    return result;
}

std::vector<BigInt> TowerResidues(const DCRTPoly& coefficientPolynomial,
                                  std::size_t towerIndex) {
    const auto& values = coefficientPolynomial.GetAllElements().at(towerIndex).GetValues();
    std::vector<BigInt> result;
    result.reserve(values.GetLength());
    for (std::size_t coefficient = 0; coefficient < values.GetLength(); ++coefficient) {
        result.emplace_back(values.at(coefficient).ConvertToInt());
    }
    return result;
}

std::vector<BigInt> NegacyclicProductMod(const std::vector<BigInt>& left,
                                         const std::vector<BigInt>& right,
                                         const BigInt& modulus) {
    Check(left.size() == right.size() && !left.empty(),
          "negacyclic modular product shape mismatch");
    const std::size_t n = left.size();
    std::vector<BigInt> result(n, 0);
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            const BigInt term = left[i] * right[j];
            const std::size_t rawIndex = i + j;
            if (rawIndex < n) {
                result[rawIndex] += term;
            }
            else {
                result[rawIndex - n] -= term;
            }
        }
    }
    for (auto& coefficient : result) {
        coefficient = PositiveMod(coefficient, modulus);
    }
    return result;
}

std::vector<BigInt> NegacyclicProductInteger(const std::vector<BigInt>& left,
                                             const std::vector<BigInt>& right) {
    Check(left.size() == right.size() && !left.empty(),
          "negacyclic integer product shape mismatch");
    const std::size_t n = left.size();
    std::vector<BigInt> result(n, 0);
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            const BigInt term = left[i] * right[j];
            const std::size_t rawIndex = i + j;
            if (rawIndex < n) {
                result[rawIndex] += term;
            }
            else {
                result[rawIndex - n] -= term;
            }
        }
    }
    return result;
}

void AddModInPlace(std::vector<BigInt>& accumulator,
                   const std::vector<BigInt>& addend,
                   const BigInt& modulus) {
    Check(accumulator.size() == addend.size(), "modular vector-add shape mismatch");
    for (std::size_t index = 0; index < accumulator.size(); ++index) {
        accumulator[index] = PositiveMod(accumulator[index] + addend[index], modulus);
    }
}

struct DecryptionOracleResult {
    std::vector<BigInt> coefficients;
    std::vector<BigInt> moduli;
    BigInt modulus;
};

DecryptionOracleResult IndependentDecrypt(const ReadOnlyCiphertext& ciphertext,
                                          const lbcrypto::PrivateKey<DCRTPoly>& secretKey) {
    Check(ciphertext != nullptr, "independent decryption received a null ciphertext");
    Check(secretKey != nullptr, "independent decryption received a null secret key");
    Check(!ciphertext->GetElements().empty(), "independent decryption received no RLWE components");

    std::vector<DCRTPoly> components;
    components.reserve(ciphertext->GetElements().size());
    for (const auto& element : ciphertext->GetElements()) {
        components.push_back(ToCoefficient(element));
    }
    const DCRTPoly secret = ToCoefficient(secretKey->GetPrivateElement());
    const auto moduli = GetModuli(components.front());
    Check(!moduli.empty(), "independent decryption received an empty RNS basis");
    Check(secret.GetAllElements().size() >= moduli.size(),
          "secret-key basis is shorter than the ciphertext basis");

    const std::size_t ringDimension = components.front().GetParams()->GetRingDimension();
    for (const auto& component : components) {
        Check(component.GetParams()->GetRingDimension() == ringDimension,
              "ciphertext components disagree on ring dimension");
        Check(GetModuli(component) == moduli,
              "ciphertext components disagree on ordered RNS basis");
    }

    std::vector<std::vector<BigInt>> coefficientResidues(
        ringDimension, std::vector<BigInt>(moduli.size(), 0));
    for (std::size_t tower = 0; tower < moduli.size(); ++tower) {
        Check(BigInt(secret.GetAllElements().at(tower).GetModulus().ConvertToInt()) == moduli[tower],
              "secret-key tower does not match ciphertext tower");
        const auto secretResidues = TowerResidues(secret, tower);
        Check(secretResidues.size() == ringDimension,
              "secret-key coefficient vector has the wrong length");

        std::vector<BigInt> decrypted = TowerResidues(components.front(), tower);
        Check(decrypted.size() == ringDimension,
              "ciphertext coefficient vector has the wrong length");
        std::vector<BigInt> secretPower(ringDimension, 0);
        secretPower.front() = 1;
        for (std::size_t component = 1; component < components.size(); ++component) {
            secretPower = NegacyclicProductMod(secretPower, secretResidues, moduli[tower]);
            const auto componentResidues = TowerResidues(components[component], tower);
            const auto term = NegacyclicProductMod(componentResidues, secretPower, moduli[tower]);
            AddModInPlace(decrypted, term, moduli[tower]);
        }
        for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
            coefficientResidues[coefficient][tower] = decrypted[coefficient];
        }
    }

    DecryptionOracleResult result{{}, moduli, Product(moduli)};
    result.coefficients.reserve(ringDimension);
    for (const auto& residues : coefficientResidues) {
        result.coefficients.push_back(ReconstructCentered(residues, moduli));
    }
    return result;
}

std::vector<BigInt> Recombine(const std::vector<BigInt>& high,
                              const std::vector<BigInt>& low,
                              const BigInt& divisor,
                              const BigInt& modulus) {
    Check(high.size() == low.size(), "pair plaintext coefficient shape mismatch");
    std::vector<BigInt> result(high.size());
    for (std::size_t coefficient = 0; coefficient < high.size(); ++coefficient) {
        result[coefficient] = Center(divisor * high[coefficient] + low[coefficient], modulus);
    }
    return result;
}

struct PairOracleResult {
    DecryptionOracleResult high;
    DecryptionOracleResult low;
    std::vector<BigInt> recombined;
};

PairOracleResult IndependentDecryptPair(const CiphertextPair& pair,
                                        const lbcrypto::PrivateKey<DCRTPoly>& secretKey) {
    const auto high = IndependentDecrypt(pair.GetHigh(), secretKey);
    const auto low = IndependentDecrypt(pair.GetLow(), secretKey);
    Check(high.moduli == low.moduli && high.modulus == low.modulus,
          "pair members decrypt over different RNS bases");
    const BigInt divisor(pair.GetDivisor().ConvertToInt());
    return {high, low, Recombine(high.coefficients, low.coefficients, divisor, high.modulus)};
}

PairOracleResult IndependentDecryptTensor(const TensorCiphertextPair& pair,
                                          const lbcrypto::PrivateKey<DCRTPoly>& secretKey) {
    const auto high = IndependentDecrypt(pair.GetHigh(), secretKey);
    const auto low = IndependentDecrypt(pair.GetLow(), secretKey);
    Check(high.moduli == low.moduli && high.modulus == low.modulus,
          "Tensor pair members decrypt over different RNS bases");
    const BigInt divisor(pair.GetDivisor().ConvertToInt());
    return {high, low, Recombine(high.coefficients, low.coefficients, divisor, high.modulus)};
}

Ciphertext<DCRTPoly> RecombineTensorCiphertext(const TensorCiphertextPair& pair) {
    Check(pair.GetHigh() != nullptr && pair.GetLow() != nullptr,
          "Tensor recombination received a null member");
    auto result = pair.GetHigh()->Clone();
    auto& resultElements = result->GetElements();
    const auto& lowElements = pair.GetLow()->GetElements();
    Check(resultElements.size() == lowElements.size(),
          "Tensor recombination component counts differ");
    for (std::size_t index = 0; index < resultElements.size(); ++index) {
        const auto& highParams = resultElements[index].GetParams();
        const auto& lowParams = lowElements[index].GetParams();
        Check(highParams != nullptr && lowParams != nullptr && *highParams == *lowParams,
              "Tensor recombination component bases differ");
        resultElements[index] *= pair.GetDivisor();
        resultElements[index] += lowElements[index];
    }
    return result;
}

BigInt MaximumAbsolute(const std::vector<BigInt>& values) {
    BigInt result = 0;
    for (const auto& value : values) {
        result = Max(result, Abs(value));
    }
    return result;
}

std::size_t SecretHammingWeight(const lbcrypto::PrivateKey<DCRTPoly>& secretKey) {
    Check(secretKey != nullptr, "secret Hamming-weight oracle received a null key");
    const DCRTPoly secret = ToCoefficient(secretKey->GetPrivateElement());
    const auto moduli = GetModuli(secret);
    Check(!moduli.empty(), "secret Hamming-weight oracle received an empty basis");
    const std::size_t ringDimension = secret.GetParams()->GetRingDimension();
    std::size_t hammingWeight = 0;
    for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
        std::vector<BigInt> residues;
        residues.reserve(moduli.size());
        for (std::size_t tower = 0; tower < moduli.size(); ++tower) {
            residues.emplace_back(
                secret.GetAllElements().at(tower).GetValues().at(coefficient).ConvertToInt());
        }
        const BigInt centered = ReconstructCentered(residues, moduli);
        Check(centered >= -1 && centered <= 1,
              "secret-key coefficient is outside the expected ternary support");
        if (centered != 0) {
            ++hammingWeight;
        }
    }
    return hammingWeight;
}

struct CiphertextSnapshot {
    ReadOnlyCiphertext identity;
    Ciphertext<DCRTPoly> clone;
};

CiphertextSnapshot SnapshotCiphertext(const ReadOnlyCiphertext& ciphertext,
                                      const std::string& label) {
    Check(ciphertext != nullptr, label + " is null");
    return {ciphertext, ciphertext->Clone()};
}

void CheckCiphertextUnchanged(const ReadOnlyCiphertext& ciphertext,
                              const CiphertextSnapshot& snapshot,
                              const std::string& label) {
    Check(ciphertext.get() == snapshot.identity.get(), label + " identity changed");
    Check(*ciphertext == *snapshot.clone, label + " value or metadata changed");
}

struct PairSnapshot {
    CiphertextSnapshot high;
    CiphertextSnapshot low;
    const lbcrypto::CryptoContextImpl<DCRTPoly>* contextIdentity;
    NativeInteger divisor;
    std::vector<NativeInteger> orderedModuli;
    std::size_t level;
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
            pair.GetRecordedScalingFactor(),
            pair.GetNoiseScaleDegree(),
            pair.GetLifecycle(),
            pair.GetKeyTag(),
            pair.GetSlots(),
            pair.GetFormat(),
            pair.GetComponentCount()};
}

void CheckPairUnchanged(const CiphertextPair& pair,
                        const PairSnapshot& snapshot,
                        const std::string& label) {
    CheckCiphertextUnchanged(pair.GetHigh(), snapshot.high, label + " high");
    CheckCiphertextUnchanged(pair.GetLow(), snapshot.low, label + " low");
    Check(pair.GetContextIdentity() == snapshot.contextIdentity, label + " context manifest changed");
    Check(pair.GetDivisor() == snapshot.divisor, label + " divisor manifest changed");
    Check(pair.GetOrderedModuli() == snapshot.orderedModuli, label + " basis manifest changed");
    Check(pair.GetLevel() == snapshot.level, label + " level manifest changed");
    Check(pair.GetRecordedScalingFactor() == snapshot.recordedScalingFactor,
          label + " recorded scale manifest changed");
    Check(pair.GetNoiseScaleDegree() == snapshot.noiseScaleDegree,
          label + " noise-scale degree manifest changed");
    Check(pair.GetLifecycle() == snapshot.lifecycle, label + " lifecycle manifest changed");
    Check(pair.GetKeyTag() == snapshot.keyTag, label + " key-tag manifest changed");
    Check(pair.GetSlots() == snapshot.slots, label + " slots manifest changed");
    Check(pair.GetFormat() == snapshot.format, label + " format manifest changed");
    Check(pair.GetComponentCount() == snapshot.componentCount,
          label + " component-count manifest changed");
}

CryptoContext<DCRTPoly> MakeContext(lbcrypto::KeySwitchTechnique keySwitchTechnique,
                                    lbcrypto::CKKSDataType dataType) {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(kMultiplicativeDepth);
    parameters.SetScalingModSize(kScalingModSize);
    parameters.SetFirstModSize(kFirstModSize);
    parameters.SetScalingTechnique(lbcrypto::FIXEDMANUAL);
    parameters.SetKeySwitchTechnique(keySwitchTechnique);
    parameters.SetDigitSize(kDigitSize);
    parameters.SetMaxRelinSkDeg(kMaxRelinSkDeg);
    parameters.SetSecretKeyDist(lbcrypto::UNIFORM_TERNARY);
    parameters.SetSecurityLevel(lbcrypto::HEStd_NotSet);
    parameters.SetRingDim(kRingDimension);
    parameters.SetBatchSize(kBatchSize);
    parameters.SetCKKSDataType(dataType);

    auto context = lbcrypto::GenCryptoContext(parameters);
    context->Enable(lbcrypto::PKE);
    context->Enable(lbcrypto::KEYSWITCH);
    context->Enable(lbcrypto::LEVELEDSHE);
    return context;
}

std::string KeySwitchName(lbcrypto::KeySwitchTechnique technique) {
    if (technique == lbcrypto::HYBRID) {
        return "HYBRID";
    }
    if (technique == lbcrypto::BV) {
        return "BV";
    }
    return "unsupported";
}

std::vector<std::complex<double>> ToComplex(const std::vector<double>& values) {
    std::vector<std::complex<double>> result;
    result.reserve(values.size());
    for (const double value : values) {
        result.emplace_back(value, 0.0);
    }
    return result;
}

std::vector<std::complex<double>> ElementwiseProduct(
    const std::vector<std::complex<double>>& left,
    const std::vector<std::complex<double>>& right) {
    Check(left.size() == right.size(), "host-vector lengths differ");
    std::vector<std::complex<double>> result(left.size());
    for (std::size_t index = 0; index < left.size(); ++index) {
        result[index] = left[index] * right[index];
    }
    return result;
}

void CheckHostEnvelope(const std::vector<std::complex<double>>& values) {
    Check(!values.empty() && values.size() <= kBatchSize,
          "host-vector length is outside the selected batch size");
    for (const auto& value : values) {
        Check(std::abs(value.real()) + std::abs(value.imag()) <= 1.0,
              "host-vector value exceeds the frozen unit L1 envelope");
    }
}

void CheckPreselectedModulusBudget(const CiphertextPair& pair) {
    // Frozen before exercising the candidate green: for N=64 and Delta=2^30,
    // E_select = 2*N*Delta^2 = 2^67 and
    // 2*N*E_select^2*2^32 = 2^173, so strict Q_l > required needs at
    // least 174 actual bits. Depth seven was selected up front; this runtime
    // check verifies the generated prime product. The 32-bit reserve is an
    // engineering preselection margin, not a conservative theorem proof.
    const auto& nativeModuli = pair.GetOrderedModuli();
    Check(pair.GetHigh() != nullptr && !pair.GetHigh()->GetElements().empty(),
          "preselection budget received an empty pair member");
    const std::size_t ringDimension =
        pair.GetHigh()->GetElements().front().GetParams()->GetRingDimension();
    Check(ringDimension == kRingDimension,
          "generated ring dimension differs from the selected fixture parameter");
    std::vector<BigInt> moduli;
    moduli.reserve(nativeModuli.size());
    for (const auto& modulus : nativeModuli) {
        moduli.emplace_back(modulus.ConvertToInt());
    }
    const BigInt qLProduct = Product(moduli);
    const BigInt delta = BigInt(1) << kScalingModSize;
    const BigInt inputCoefficientEnvelope =
        BigInt(2) * BigInt(ringDimension) * delta * delta;
    BigInt required = BigInt(2) * BigInt(ringDimension) *
                      inputCoefficientEnvelope * inputCoefficientEnvelope;
    required <<= kSelectionReserveBits;
    Check(qLProduct > required,
          "selected Q_l does not satisfy the pre-Mult2 host-envelope budget with reserve");
}

void CheckPhysicalCiphertextState(
    const ReadOnlyCiphertext& ciphertext,
    const std::vector<NativeInteger>& expectedModuli,
    std::size_t expectedLevel,
    std::size_t expectedNoiseScaleDegree,
    double expectedRecordedScalingFactor,
    const std::string& expectedKeyTag,
    std::uint32_t expectedSlots,
    const CryptoContext<DCRTPoly>& context,
    const std::string& label) {
    Check(ciphertext != nullptr, label + " is null");
    Check(ciphertext->GetCryptoContext().get() == context.get(),
          label + " context identity changed");
    Check(ciphertext->GetEncodingType() == lbcrypto::CKKS_PACKED_ENCODING,
          label + " encoding metadata is not CKKS packed");
    Check(ciphertext->GetLevel() == expectedLevel, label + " level changed");
    Check(ciphertext->GetNoiseScaleDeg() == expectedNoiseScaleDegree,
          label + " noise-scale degree changed");
    Check(ciphertext->GetScalingFactor() == expectedRecordedScalingFactor,
          label + " recorded scaling factor changed");
    Check(ciphertext->GetKeyTag() == expectedKeyTag, label + " key tag changed");
    Check(ciphertext->GetSlots() == expectedSlots, label + " slots changed");
    Check(ciphertext->NumberCiphertextElements() == 2,
          label + " does not contain exactly two RLWE components");
    for (const auto& element : ciphertext->GetElements()) {
        Check(element.GetFormat() == Format::EVALUATION,
              label + " component is not in evaluation format");
        const auto& towers = element.GetAllElements();
        Check(towers.size() == expectedModuli.size(),
              label + " active-basis size changed");
        for (std::size_t index = 0; index < towers.size(); ++index) {
            Check(towers[index].GetModulus() == expectedModuli[index],
                  label + " ordered RNS basis changed");
            Check(towers[index].GetFormat() == Format::EVALUATION,
                  label + " tower is not in evaluation format");
        }
    }
}

void CheckPublicResultState(const CiphertextPair& result,
                            const CiphertextPair& input,
                            const CryptoContext<DCRTPoly>& context) {
    Check(result.GetLifecycle() == PairLifecycle::RefreshRequired,
          "Mult2 did not end at RefreshRequired");
    Check(result.GetContextIdentity() == context.get(), "Mult2 context identity changed");
    Check(result.GetDivisor() == input.GetDivisor(), "Mult2 divisor changed");
    Check(result.GetLevel() == 2, "Mult2 output level is not two");
    Check(result.GetNoiseScaleDegree() == 2,
          "Mult2 output noise-scale degree is not two");
    Check(result.GetComponentCount() == 2,
          "Mult2 output members do not each contain two components");
    Check(result.GetFormat() == Format::EVALUATION,
          "Mult2 output pair format is not evaluation");
    Check(result.GetKeyTag() == input.GetKeyTag(), "Mult2 key tag changed");
    Check(result.GetSlots() == input.GetSlots(), "Mult2 slots changed");
    Check(result.GetOrderedModuli().size() + 1 == input.GetOrderedModuli().size(),
          "Mult2 output did not drop exactly q_l");
    for (std::size_t index = 0; index < result.GetOrderedModuli().size(); ++index) {
        Check(result.GetOrderedModuli()[index] == input.GetOrderedModuli()[index],
              "Mult2 output basis is not the exact input prefix");
    }
    CheckPhysicalCiphertextState(
        result.GetHigh(), result.GetOrderedModuli(), result.GetLevel(),
        result.GetNoiseScaleDegree(), result.GetRecordedScalingFactor(),
        result.GetKeyTag(), result.GetSlots(), context, "Mult2 high output");
    CheckPhysicalCiphertextState(
        result.GetLow(), result.GetOrderedModuli(), result.GetLevel(),
        result.GetNoiseScaleDegree(), result.GetRecordedScalingFactor(),
        result.GetKeyTag(), result.GetSlots(), context, "Mult2 low output");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr, "Mult2 fixture context is not CKKS-RNS");
    const std::size_t qLIndex = input.GetOrderedModuli().size() - 1;
    const double baseScalingFactor = parameters->GetScalingFactorReal(0);
    const double selectedBaseScalingFactor =
        std::ldexp(1.0, static_cast<int>(kScalingModSize));
    Check(baseScalingFactor == selectedBaseScalingFactor,
          "actual FIXEDMANUAL base scaling factor differs from selected 2^p");
    const double officialRescaleFactor = parameters->GetModReduceFactor(qLIndex);
    const double expectedRecordedScalingFactor =
        input.GetRecordedScalingFactor() * input.GetRecordedScalingFactor() /
        baseScalingFactor / officialRescaleFactor;
    Check(result.GetRecordedScalingFactor() == expectedRecordedScalingFactor,
          "Mult2 recorded scaling factor did not follow the official FIXEDMANUAL factors");

    const long double qDiv = static_cast<long double>(input.GetDivisor().ConvertToInt());
    const long double qL =
        static_cast<long double>(input.GetOrderedModuli().back().ConvertToInt());
    const long double inputScale = static_cast<long double>(input.GetRecordedScalingFactor());
    const long double expectedHighLogicalScale =
        (inputScale / qDiv) * (inputScale / qDiv) / qL;
    const long double expectedRecombinedLogicalScale =
        inputScale * inputScale / qDiv / qL;
    Check(result.GetPaperScale().inputRecordedScalingFactor == result.GetRecordedScalingFactor(),
          "Mult2 paper-scale recorded factor is inconsistent");
    Check(result.GetPaperScale().divisor == input.GetDivisor(),
          "Mult2 paper-scale divisor is inconsistent");
    Check(result.GetPaperScale().approximateLogicalScalingFactor == expectedHighLogicalScale,
          "Mult2 high logical scale is inconsistent");
    Check(result.GetPaperScale().approximateRecombinedLogicalScalingFactor ==
              expectedRecombinedLogicalScale,
          "Mult2 recombined logical scale is inconsistent");
}

struct ArithmeticCertificate {
    std::size_t ringDimension;
    BigInt qLProduct;
    BigInt qDiv;
    BigInt qL;
    BigInt mHigh;
    BigInt mLow;
    BigInt empiricalRelinError;
    BigInt empiricalPairRelinError;
    std::size_t secretHammingWeight;
    BigInt nonWrapLeft;
    BigInt empiricalBoundNumerator;
    BigInt empiricalBoundDenominator;
    BigInt coefficientErrorNumerator;
    BigInt coefficientErrorDenominator;
};

ArithmeticCertificate CheckIndependentArithmetic(
    const CiphertextPair& left,
    const CiphertextPair& right,
    const TensorCiphertextPair& tensor,
    const CiphertextPair& relinearized,
    const CiphertextPair& result,
    const lbcrypto::PrivateKey<DCRTPoly>& secretKey,
    const CryptoContext<DCRTPoly>& context) {
    const auto leftPlain = IndependentDecryptPair(left, secretKey);
    const auto rightPlain = IndependentDecryptPair(right, secretKey);
    const auto tensorPlain = IndependentDecryptTensor(tensor, secretKey);
    const auto relinPlain = IndependentDecryptPair(relinearized, secretKey);
    auto recombinedTensor = RecombineTensorCiphertext(tensor);
    ReadOnlyCiphertext recombinedTensorReadOnly = recombinedTensor;
    const auto recombinedTensorPlain = IndependentDecrypt(recombinedTensorReadOnly, secretKey);
    Check(recombinedTensorPlain.coefficients == tensorPlain.recombined,
          "independent Tensor pair/ciphertext recombinations disagree");
    lbcrypto::ConstCiphertext<DCRTPoly> recombinedTensorConst = recombinedTensor;
    auto standardRelinearized = context->Relinearize(recombinedTensorConst);
    ReadOnlyCiphertext standardRelinearizedReadOnly = standardRelinearized;
    const auto standardRelinPlain =
        IndependentDecrypt(standardRelinearizedReadOnly, secretKey);
    Check(leftPlain.high.moduli == rightPlain.high.moduli &&
              leftPlain.high.moduli == tensorPlain.high.moduli &&
              leftPlain.high.moduli == relinPlain.high.moduli &&
              leftPlain.high.moduli == recombinedTensorPlain.moduli &&
              leftPlain.high.moduli == standardRelinPlain.moduli,
          "pre-RS2 oracle bases differ");

    const BigInt qLProduct = leftPlain.high.modulus;
    const BigInt qDiv(left.GetDivisor().ConvertToInt());
    const BigInt qL(left.GetOrderedModuli().back().ConvertToInt());
    Check(qLProduct == Product(leftPlain.high.moduli), "Q_l product mismatch");
    Check(qLProduct % qL == 0, "q_l is not a factor of Q_l");

    const BigInt mHigh = Max(
        Max(MaximumAbsolute(leftPlain.high.coefficients),
            MaximumAbsolute(rightPlain.high.coefficients)),
        BigInt(0));
    const BigInt mLow = Max(
        Max(MaximumAbsolute(leftPlain.low.coefficients),
            MaximumAbsolute(rightPlain.low.coefficients)),
        BigInt(0));
    const std::size_t hammingWeight = SecretHammingWeight(secretKey);

    Check(tensorPlain.recombined.size() == relinPlain.recombined.size() &&
              tensorPlain.recombined.size() == standardRelinPlain.coefficients.size(),
          "Tensor/Relin plaintext coefficient shape mismatch");
    BigInt empiricalRelinError = 0;
    BigInt empiricalPairRelinError = 0;
    for (std::size_t coefficient = 0; coefficient < tensorPlain.recombined.size(); ++coefficient) {
        const BigInt standardDifference = Center(
            standardRelinPlain.coefficients[coefficient] -
                recombinedTensorPlain.coefficients[coefficient],
            qLProduct);
        empiricalRelinError = Max(empiricalRelinError, Abs(standardDifference));
        const BigInt pairDifference = Center(
            relinPlain.recombined[coefficient] - tensorPlain.recombined[coefficient],
            qLProduct);
        empiricalPairRelinError = Max(empiricalPairRelinError, Abs(pairDifference));
    }

    const std::size_t ringDimension = leftPlain.recombined.size();
    Check(ringDimension > 0 && rightPlain.recombined.size() == ringDimension &&
              tensorPlain.recombined.size() == ringDimension &&
              relinPlain.recombined.size() == ringDimension,
          "independent plaintext ring dimensions differ");
    const BigInt n(ringDimension);
    const BigInt h(hammingWeight);
    Check(empiricalPairRelinError <= empiricalRelinError + h,
          "pair relinearization error exceeded empirical E_Relin + h");
    const BigInt inputEnvelope = mHigh * qDiv + mLow;
    const BigInt nonWrapLeft =
        n * inputEnvelope * inputEnvelope + empiricalRelinError + h;
    Check(BigInt(2) * nonWrapLeft < qLProduct,
          "execution-specific non-wrap witness failed");

    // Decrypt and recombine the two public result members independently. The
    // coefficient oracle does not call production RCB; RCB is exercised only by
    // the separate public decoded-slot path below.
    const auto actualOutput = IndependentDecryptPair(result, secretKey);
    Check(actualOutput.high.modulus == qLProduct / qL,
          "Mult2 output modulus is not Q_l/q_l");

    const auto exactInputProduct =
        NegacyclicProductInteger(leftPlain.recombined, rightPlain.recombined);
    Check(exactInputProduct.size() == actualOutput.recombined.size(),
          "ideal/output coefficient shape mismatch");

    BigInt coefficientErrorNumerator = 0;
    for (std::size_t coefficient = 0; coefficient < exactInputProduct.size(); ++coefficient) {
        const BigInt numerator = Abs(
            actualOutput.recombined[coefficient] * qDiv * qL -
            exactInputProduct[coefficient]);
        coefficientErrorNumerator = Max(coefficientErrorNumerator, numerator);
    }
    const BigInt coefficientErrorDenominator = qDiv * qL;

    // This is the paper's expression with E_Relin instantiated by the measured
    // error of this exact staged execution. It is an empirical certificate only;
    // it is not a conservative universal E_Relin proof.
    const BigInt empiricalBoundNumerator =
        BigInt(2) * n * mLow * mLow +
        BigInt(2) * qDiv * (empiricalRelinError + h) +
        qDiv * qL * (h + 1);
    const BigInt empiricalBoundDenominator = BigInt(2) * qDiv * qL;
    Check(BigInt(2) * coefficientErrorNumerator <= empiricalBoundNumerator,
          "independent coefficient error exceeded the execution-specific theorem expression");

    return {ringDimension,
            qLProduct,
            qDiv,
            qL,
            mHigh,
            mLow,
            empiricalRelinError,
            empiricalPairRelinError,
            hammingWeight,
            nonWrapLeft,
            empiricalBoundNumerator,
            empiricalBoundDenominator,
            coefficientErrorNumerator,
            coefficientErrorDenominator};
}

struct SlotCertificate {
    long double ratio;
    double maximumRecordedBiasError;
    double maximumLogicalError;
};

SlotCertificate CheckDecodedSlots(
    const CiphertextPair& result,
    const BigInt& qL,
    const std::vector<std::complex<double>>& expected,
    const CryptoContext<DCRTPoly>& context,
    const lbcrypto::PrivateKey<DCRTPoly>& secretKey,
    const DoubleCKKS& module) {
    auto recombined = module.RCB(result);
    lbcrypto::ConstCiphertext<DCRTPoly> recombinedConst = recombined;
    lbcrypto::Plaintext decodedPlaintext;
    const auto decryptResult = context->Decrypt(secretKey, recombinedConst, &decodedPlaintext);
    Check(decryptResult.isValid, "OpenFHE rejected the Mult2 decryption");
    decodedPlaintext->SetLength(expected.size());
    const auto recordedScaleValues = decodedPlaintext->GetCKKSPackedValue();
    Check(recordedScaleValues.size() == expected.size(), "decoded vector length changed");

    const long double formulaRatio =
        std::ldexp(1.0L, 2 * static_cast<int>(kScalingModSize)) /
        (static_cast<long double>(result.GetDivisor().ConvertToInt()) *
         qL.convert_to<long double>());
    const long double descriptorRatio =
        result.GetPaperScale().approximateRecombinedLogicalScalingFactor /
        static_cast<long double>(result.GetRecordedScalingFactor());
    Check(formulaRatio > 0.0L && std::isfinite(formulaRatio),
          "2^(2p)/(q_div*q_l) is invalid");
    Check(descriptorRatio > 0.0L && std::isfinite(descriptorRatio),
          "logical/recorded scale ratio is invalid");
    const long double ratioDifference = std::abs(descriptorRatio - formulaRatio);
    const long double ratioTolerance =
        32.0L * std::numeric_limits<long double>::epsilon() *
        std::max(std::abs(descriptorRatio), std::abs(formulaRatio));
    Check(ratioDifference <= ratioTolerance,
          "descriptor ratio differs from 2^(2p)/(q_div*q_l)");

    double maximumRecordedBiasError = 0.0;
    double maximumLogicalError = 0.0;
    for (std::size_t index = 0; index < expected.size(); ++index) {
        Check(std::isfinite(recordedScaleValues[index].real()) &&
                  std::isfinite(recordedScaleValues[index].imag()),
              "decoded slot contains a non-finite component");
        const auto biasedExpected = expected[index] * static_cast<double>(descriptorRatio);
        maximumRecordedBiasError = std::max(
            maximumRecordedBiasError,
            std::abs(recordedScaleValues[index] - biasedExpected));
        const auto logicalValue =
            recordedScaleValues[index] / static_cast<double>(descriptorRatio);
        maximumLogicalError = std::max(maximumLogicalError,
                                       std::abs(logicalValue - expected[index]));
    }
    const double recordedBiasTolerance =
        kLogicalDecodedAbsoluteTolerance *
        std::max(1.0, std::abs(static_cast<double>(descriptorRatio)));
    Check(maximumRecordedBiasError <= recordedBiasTolerance,
          "recorded-scale decoder did not follow the predicted deterministic bias");
    Check(maximumLogicalError <= kLogicalDecodedAbsoluteTolerance,
          "logical-scale decoded slot error exceeded the frozen threshold");
    return {descriptorRatio, maximumRecordedBiasError, maximumLogicalError};
}

void PrintCertificate(const std::string& caseName,
                      lbcrypto::KeySwitchTechnique technique,
                      lbcrypto::CKKSDataType dataType,
                      std::size_t hostVectorLength,
                      const CiphertextPair& input,
                      const ArithmeticCertificate& arithmetic,
                      const SlotCertificate& slots) {
    const long double requestedRatio =
        std::ldexp(1.0L, 2 * static_cast<int>(kScalingModSize)) /
        (arithmetic.qDiv.convert_to<long double>() * arithmetic.qL.convert_to<long double>());
    const BigInt delta = BigInt(1) << kScalingModSize;
    const BigInt selectionEnvelope =
        BigInt(2) * BigInt(arithmetic.ringDimension) * delta * delta;
    BigInt selectionRequired = BigInt(2) * BigInt(arithmetic.ringDimension) *
                               selectionEnvelope * selectionEnvelope;
    selectionRequired <<= kSelectionReserveBits;

    std::cout << std::setprecision(18)
              << "case=" << caseName
              << " security_level=HEStd_NotSet(functional-only)"
              << " key_switch=" << KeySwitchName(technique)
              << " secret_key_dist=UNIFORM_TERNARY"
              << " ckks_data_type=" << (dataType == lbcrypto::COMPLEX ? "COMPLEX" : "REAL")
              << " selected_ring_dimension=" << kRingDimension
              << " actual_N=" << arithmetic.ringDimension
              << " batch_size=" << kBatchSize
              << " host_vector_length=" << hostVectorLength
              << " encoding_noise_scale_degree=" << kEncodingNoiseScaleDegree
              << " encoding_level=" << kEncodingLevel
              << " scaling_technique=FIXEDMANUAL"
              << " digit_size=" << kDigitSize
              << " max_relin_sk_deg=" << kMaxRelinSkDeg
              << " scaling_mod_size=" << kScalingModSize
              << " first_mod_size=" << kFirstModSize
              << " multiplicative_depth=" << kMultiplicativeDepth
              << " active_Q_l_towers=" << input.GetOrderedModuli().size()
              << " Q_l_bits=" << BitLength(arithmetic.qLProduct)
              << " preselection_required_bits=" << BitLength(selectionRequired)
              << " selection_reserve_bits=" << kSelectionReserveBits
              << " q_div=" << arithmetic.qDiv
              << " q_l=" << arithmetic.qL
              << " ratio_2^(2p)/(q_div*q_l)=" << requestedRatio
              << " descriptor_ratio=" << slots.ratio
              << " M_high=" << arithmetic.mHigh
              << " M_low=" << arithmetic.mLow
              << " secret_h=" << arithmetic.secretHammingWeight
              << " empirical_E_Relin=" << arithmetic.empiricalRelinError
              << " empirical_pair_relin_error=" << arithmetic.empiricalPairRelinError
              << " conservative_E_Relin_available=false"
              << " universal_theorem_gate=UNPROVED"
              << " execution_nonwrap_left=" << arithmetic.nonWrapLeft
              << " execution_nonwrap_right_Q_l_over_2=" << arithmetic.qLProduct / 2
              << " coefficient_error=" << arithmetic.coefficientErrorNumerator
              << '/' << arithmetic.coefficientErrorDenominator
              << " empirical_bound=" << arithmetic.empiricalBoundNumerator
              << '/' << arithmetic.empiricalBoundDenominator
              << " frozen_decoded_abs_tolerance=" << kLogicalDecodedAbsoluteTolerance
              << " measured_recorded_bias_error=" << slots.maximumRecordedBiasError
              << " measured_logical_slot_error=" << slots.maximumLogicalError
              << '\n';
}

template <class HostValue>
void RunCase(const std::string& caseName,
             lbcrypto::KeySwitchTechnique technique,
             const std::vector<HostValue>& leftValues,
             const std::vector<HostValue>& rightValues) {
    const auto leftComplex = [&] {
        if constexpr (std::is_same_v<HostValue, double>) {
            return ToComplex(leftValues);
        }
        else {
            return std::vector<std::complex<double>>(leftValues.begin(), leftValues.end());
        }
    }();
    const auto rightComplex = [&] {
        if constexpr (std::is_same_v<HostValue, double>) {
            return ToComplex(rightValues);
        }
        else {
            return std::vector<std::complex<double>>(rightValues.begin(), rightValues.end());
        }
    }();
    CheckHostEnvelope(leftComplex);
    CheckHostEnvelope(rightComplex);
    const auto expected = ElementwiseProduct(leftComplex, rightComplex);

    constexpr lbcrypto::CKKSDataType dataType =
        std::is_same_v<HostValue, double> ? lbcrypto::REAL : lbcrypto::COMPLEX;
    auto context = MakeContext(technique, dataType);
    const auto keys = context->KeyGen();
    Check(keys.good(), "key generation failed");
    context->EvalMultKeyGen(keys.secretKey);

    const auto leftPlaintext = context->MakeCKKSPackedPlaintext(
        leftValues, kEncodingNoiseScaleDegree, kEncodingLevel);
    const auto rightPlaintext = context->MakeCKKSPackedPlaintext(
        rightValues, kEncodingNoiseScaleDegree, kEncodingLevel);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);
    const auto leftInputBefore = SnapshotCiphertext(leftInput, caseName + " left encrypted input");
    const auto rightInputBefore = SnapshotCiphertext(rightInput, caseName + " right encrypted input");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    Check(left.GetOrderedModuli().size() >= 4,
          "Mult2 theorem fixture retained too few active Q_l towers");
    CheckPreselectedModulusBudget(left);
    const auto leftBefore = SnapshotPair(left, caseName + " left pair");
    const auto rightBefore = SnapshotPair(right, caseName + " right pair");

    // Public staged seams expose the intermediate plaintexts required by the
    // independent coefficient/theorem certificate. The public Mult2 call below
    // remains the candidate under test; neither staged ciphertext is used as an
    // arithmetic expected value.
    const auto tensor = module.Tensor2(left, right);
    const auto relinearized = module.Relin2(tensor);
    const auto stagedResult = module.RS2(relinearized);
    const auto result = module.Mult2(left, right);

    CheckPublicResultState(result, left, context);
    CheckPublicResultState(stagedResult, left, context);
    Check(*result.GetHigh() == *stagedResult.GetHigh() &&
              *result.GetLow() == *stagedResult.GetLow(),
          "Mult2 differs from the named Tensor2->Relin2->RS2 composition");
    CheckPairUnchanged(left, leftBefore, caseName + " left pair");
    CheckPairUnchanged(right, rightBefore, caseName + " right pair");
    CheckCiphertextUnchanged(leftInput, leftInputBefore, caseName + " left encrypted input");
    CheckCiphertextUnchanged(rightInput, rightInputBefore, caseName + " right encrypted input");

    const auto arithmetic = CheckIndependentArithmetic(
        left, right, tensor, relinearized, result, keys.secretKey, context);
    const auto slots = CheckDecodedSlots(
        result, arithmetic.qL, expected, context, keys.secretKey, module);
    PrintCertificate(
        caseName, technique, dataType, expected.size(), left, arithmetic, slots);
}

void RunSelectedCase(const std::string& name) {
    const std::vector<double> realLeft{
        0.0, std::ldexp(1.0, -20), -std::ldexp(1.0, -20), 0.125,
        -0.25, 0.5, -0.375, 0.0625};
    const std::vector<double> realRight{
        -0.5, 0.25, -0.125, 0.0,
        0.125, -0.25, std::ldexp(1.0, -18), 0.75};
    const std::vector<std::complex<double>> complexLeft{
        {0.0, 0.0},
        {std::ldexp(1.0, -20), -std::ldexp(1.0, -21)},
        {-0.125, 0.25},
        {0.375, -0.125},
        {-0.5, -0.25},
        {0.0625, 0.0},
        {0.0, 0.125},
        {-0.2, 0.3}};
    const std::vector<std::complex<double>> complexRight{
        {0.25, 0.0},
        {-0.5, 0.25},
        {0.125, -0.125},
        {-0.25, 0.5},
        {0.25, -0.25},
        {0.0, -0.5},
        {std::ldexp(1.0, -19), std::ldexp(1.0, -20)},
        {0.4, -0.1}};

    if (name == "hybrid_real") {
        RunCase(name, lbcrypto::HYBRID, realLeft, realRight);
        return;
    }
    if (name == "hybrid_complex") {
        RunCase(name, lbcrypto::HYBRID, complexLeft, complexRight);
        return;
    }
    if (name == "bv_real") {
        RunCase(name, lbcrypto::BV, realLeft, realRight);
        return;
    }
    if (name == "bv_complex") {
        RunCase(name, lbcrypto::BV, complexLeft, complexRight);
        return;
    }
    throw TestFailure("unknown Mult2 end-to-end case: " + name);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: mult2_e2e_oracle_test "
                     "hybrid_real|hybrid_complex|bv_real|bv_complex\n";
        return 2;
    }

    try {
        RunSelectedCase(argv[1]);
        lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
        return 0;
    }
    catch (const TestFailure& failure) {
        std::cerr << "mult2_e2e_oracle_test failure: " << failure.what() << '\n';
        return 1;
    }
    catch (const std::exception& exception) {
        std::cerr << "mult2_e2e_oracle_test unexpected exception: " << exception.what() << '\n';
        return 1;
    }
}

