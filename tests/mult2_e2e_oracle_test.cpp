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

struct FixedKeyBvBound {
    std::size_t ringDimension;
    std::size_t activeRows;
    std::vector<BigInt> activeModuli;
    std::vector<BigInt> rowResidualNorms;
    BigInt qLProduct;
    BigInt qDiv;
    BigInt noiseScale;
    BigInt perPathRawBound;
    BigInt pairRawBound;
};

void CheckBvCenteredDigitLiftBoundaryProbe(
    const lbcrypto::PrivateKey<DCRTPoly>& secretKey) {
    Check(secretKey != nullptr, "BV centered-digit probe received a null secret key");
    DCRTPoly probe = ToCoefficient(secretKey->GetPrivateElement());
    Check(probe.GetAllElements().size() >= 2,
          "BV centered-digit probe requires Q_l followed by q_div");
    probe.DropLastElement();

    auto& towers = probe.GetAllElements();
    const std::size_t ringDimension = probe.GetParams()->GetRingDimension();
    Check(ringDimension >= 4, "BV centered-digit probe requires four coefficients");
    for (auto& tower : towers) {
        const BigInt modulus(tower.GetModulus().ConvertToInt());
        Check(modulus % 2 == 1, "BV centered-digit probe requires odd RNS moduli");
        const BigInt half = modulus / 2;
        lbcrypto::NativeVector values(ringDimension, tower.GetModulus());
        for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
            values[coefficient] = NativeInteger(0);
        }
        values[0] = NativeInteger(0);
        values[1] = NativeInteger(half.convert_to<std::uint64_t>());
        values[2] = NativeInteger((half + 1).convert_to<std::uint64_t>());
        values[3] = NativeInteger((modulus - 1).convert_to<std::uint64_t>());
        tower.SetValues(std::move(values), Format::COEFFICIENT);
    }

    const auto digits = probe.CRTDecompose(0);
    Check(digits.size() == towers.size(),
          "BV digitSize=0 decomposition did not return one digit per active tower");
    for (std::size_t sourceTower = 0; sourceTower < digits.size(); ++sourceTower) {
        const BigInt sourceModulus(towers[sourceTower].GetModulus().ConvertToInt());
        const BigInt sourceHalf = sourceModulus / 2;
        std::vector<BigInt> expectedLift(ringDimension, 0);
        expectedLift[1] = sourceHalf;
        expectedLift[2] = -sourceHalf;
        expectedLift[3] = -1;

        const DCRTPoly digit = ToCoefficient(digits[sourceTower]);
        Check(digit.GetAllElements().size() == towers.size(),
              "BV centered-digit probe changed the active basis");
        for (std::size_t targetTower = 0; targetTower < towers.size(); ++targetTower) {
            const BigInt targetModulus(
                digit.GetAllElements().at(targetTower).GetModulus().ConvertToInt());
            const auto actual = TowerResidues(digit, targetTower);
            Check(actual.size() == ringDimension,
                  "BV centered-digit probe changed the ring dimension");
            for (std::size_t coefficient = 0; coefficient < ringDimension; ++coefficient) {
                Check(actual[coefficient] ==
                          PositiveMod(expectedLift[coefficient], targetModulus),
                      "BV digitSize=0 decomposition does not preserve the centered source-tower lift");
            }
        }
    }
}

FixedKeyBvBound BuildFixedKeyBvBound(
    const CryptoContext<DCRTPoly>& context,
    const lbcrypto::PrivateKey<DCRTPoly>& secretKey) {
    Check(context != nullptr && secretKey != nullptr,
          "fixed-key BV bound received a null context or key");
    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(
            context->GetCryptoParameters());
    Check(parameters != nullptr &&
              parameters->GetKeySwitchTechnique() == lbcrypto::BV &&
              parameters->GetDigitSize() == 0,
          "fixed-key BV bound requires the BV digitSize=0 backend");

    const auto fullParameters = parameters->GetElementParams();
    Check(fullParameters != nullptr && fullParameters->GetParams().size() >= 2,
          "fixed-key BV bound requires Q_l followed by q_div");
    const std::size_t fullRows = fullParameters->GetParams().size();
    const std::size_t activeRows = fullRows - 1;
    const DCRTPoly secret = ToCoefficient(secretKey->GetPrivateElement());
    Check(secret.GetAllElements().size() == fullRows,
          "fixed-key BV bound secret-key basis differs from the full basis");
    const std::size_t ringDimension = secret.GetParams()->GetRingDimension();

    std::vector<BigInt> activeModuli;
    activeModuli.reserve(activeRows);
    for (std::size_t tower = 0; tower < activeRows; ++tower) {
        activeModuli.emplace_back(
            secret.GetAllElements().at(tower).GetModulus().ConvertToInt());
    }
    const BigInt qLProduct = Product(activeModuli);
    const BigInt qDiv(
        secret.GetAllElements().at(activeRows).GetModulus().ConvertToInt());
    for (const auto& modulus : activeModuli) {
        BigInt inverseCoefficient;
        BigInt unusedCoefficient;
        Check(ExtendedGcd(qDiv, modulus, inverseCoefficient,
                          unusedCoefficient) == 1,
              "q_div is not a unit in an active BV source tower");
    }

    const auto& evaluationKeys =
        lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    const auto keyRow = evaluationKeys.find(secretKey->GetKeyTag());
    Check(keyRow != evaluationKeys.end() && keyRow->second.size() == 1 &&
              keyRow->second.front() != nullptr,
          "fixed-key BV bound requires exactly one evaluation-multiplication key");
    const auto relinearizationKey =
        std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(
            keyRow->second.front());
    Check(relinearizationKey != nullptr,
          "fixed-key BV bound received the wrong evaluation-key subtype");
    Check(relinearizationKey->GetAVector().size() == fullRows &&
              relinearizationKey->GetBVector().size() == fullRows,
          "fixed-key BV bound received the wrong number of BV rows");
    const auto keyABefore = relinearizationKey->GetAVector();
    const auto keyBBefore = relinearizationKey->GetBVector();

    std::vector<std::vector<BigInt>> secretResidues(activeRows);
    std::vector<std::vector<BigInt>> secretSquaredResidues(activeRows);
    for (std::size_t tower = 0; tower < activeRows; ++tower) {
        secretResidues[tower] = TowerResidues(secret, tower);
        Check(secretResidues[tower].size() == ringDimension,
              "fixed-key BV bound secret coefficient shape mismatch");
        secretSquaredResidues[tower] = NegacyclicProductMod(
            secretResidues[tower], secretResidues[tower], activeModuli[tower]);
    }

    std::vector<BigInt> rowResidualNorms;
    rowResidualNorms.reserve(activeRows);
    BigInt perPathRawBound = 0;
    for (std::size_t row = 0; row < activeRows; ++row) {
        DCRTPoly a = relinearizationKey->GetAVector().at(row);
        DCRTPoly b = relinearizationKey->GetBVector().at(row);
        Check(a.GetAllElements().size() == fullRows &&
                  b.GetAllElements().size() == fullRows,
              "fixed-key BV row is not on the full key basis");
        a.DropLastElements(fullRows - activeRows);
        b.DropLastElements(fullRows - activeRows);
        a = ToCoefficient(a);
        b = ToCoefficient(b);
        Check(GetModuli(a) == activeModuli && GetModuli(b) == activeModuli,
              "fixed-key BV row restriction did not produce Q_l");

        std::vector<std::vector<BigInt>> residuals(
            ringDimension, std::vector<BigInt>(activeRows, 0));
        for (std::size_t tower = 0; tower < activeRows; ++tower) {
            const auto aResidues = TowerResidues(a, tower);
            const auto bResidues = TowerResidues(b, tower);
            const auto aTimesSecret = NegacyclicProductMod(
                aResidues, secretResidues[tower], activeModuli[tower]);
            for (std::size_t coefficient = 0;
                 coefficient < ringDimension; ++coefficient) {
                const BigInt gadget = row == tower
                                          ? secretSquaredResidues[tower][coefficient]
                                          : BigInt(0);
                residuals[coefficient][tower] = PositiveMod(
                    bResidues[coefficient] + aTimesSecret[coefficient] - gadget,
                    activeModuli[tower]);
            }
        }

        BigInt rowNorm = 0;
        for (const auto& coefficientResidues : residuals) {
            rowNorm = Max(rowNorm,
                          Abs(ReconstructCentered(coefficientResidues, activeModuli)));
        }
        rowResidualNorms.push_back(rowNorm);
        Check(activeModuli[row] % 2 == 1,
              "fixed-key BV bound requires odd active RNS moduli");
        const BigInt centeredDigitBound = activeModuli[row] / 2;
        perPathRawBound += BigInt(ringDimension) * centeredDigitBound * rowNorm;
    }

    const BigInt pairRawBound = BigInt(2) * perPathRawBound;
    Check(relinearizationKey->GetAVector() == keyABefore &&
              relinearizationKey->GetBVector() == keyBBefore,
          "fixed-key BV oracle mutated the evaluation key");

    // KeySwitchGen gives b_i + a_i*s - G_i(s^2) = -ns*e_i modulo
    // the key basis. The measured row residuals above therefore already contain
    // the noise-scale factor; multiplying them by ns again would double count it.
    return {ringDimension,
            activeRows,
            std::move(activeModuli),
            std::move(rowResidualNorms),
            qLProduct,
            qDiv,
            BigInt(parameters->GetNoiseScale()),
            perPathRawBound,
            pairRawBound};
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

Ciphertext<DCRTPoly> RaiseTensorHighReference(
    const TensorCiphertextPair& tensor,
    const CryptoContext<DCRTPoly>& context) {
    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(
            context->GetCryptoParameters());
    Check(parameters != nullptr,
          "Relin2 path oracle context is not CKKS-RNS");
    const auto fullParameters = parameters->GetElementParams();
    Check(fullParameters != nullptr &&
              fullParameters->GetParams().size() ==
                  tensor.GetOrderedModuli().size() + 1 &&
              fullParameters->GetParams().back() != nullptr,
          "Relin2 path oracle full basis is not Q_l followed by q_div");
    Check(fullParameters->GetParams().back()->GetModulus() ==
              tensor.GetDivisor(),
          "Relin2 path oracle final full-basis tower is not q_div");

    auto raised = tensor.GetHigh()->Clone();
    auto elements = raised->GetElements();
    for (auto& element : elements) {
        auto towers = element.GetAllElements();
        Check(towers.size() == tensor.GetOrderedModuli().size(),
              "Relin2 path oracle high input is not on Q_l");
        for (auto& tower : towers) {
            tower *= tensor.GetDivisor();
        }
        lbcrypto::NativePoly zero(fullParameters->GetParams().back(),
                                  Format::EVALUATION, true);
        towers.push_back(std::move(zero));
        element = DCRTPoly(towers);
    }
    raised->SetElements(std::move(elements));
    raised->SetLevel(0);
    return raised;
}

Ciphertext<DCRTPoly> DropFinalTowerReference(
    lbcrypto::ConstCiphertext<DCRTPoly> source) {
    Check(source != nullptr,
          "Relin2 path oracle cannot restrict a null ciphertext");
    auto result = source->Clone();
    auto elements = result->GetElements();
    for (auto& element : elements) {
        Check(element.GetAllElements().size() >= 2,
              "Relin2 path oracle source has no removable final tower");
        element.DropLastElement();
    }
    result->SetElements(std::move(elements));
    result->SetLevel(source->GetLevel() + 1);
    return result;
}

void CheckBvRelin2DigitDomains(
    const TensorCiphertextPair& tensor,
    const CryptoContext<DCRTPoly>& context) {
    auto raisedHigh = RaiseTensorHighReference(tensor, context);
    Check(raisedHigh->GetElements().size() == 3 &&
              tensor.GetLow()->GetElements().size() == 3,
          "BV Relin2 digit-domain witness requires three-component paths");

    const auto highDigits = raisedHigh->GetElements().at(2).CRTDecompose(0);
    const auto lowDigits = tensor.GetLow()->GetElements().at(2).CRTDecompose(0);
    Check(highDigits.size() == tensor.GetOrderedModuli().size() + 1,
          "raised-high BV decomposition did not include the full basis");
    Check(lowDigits.size() == tensor.GetOrderedModuli().size(),
          "low BV decomposition did not use the Q_l prefix basis");

    const DCRTPoly finalHighDigit = ToCoefficient(highDigits.back());
    for (std::size_t tower = 0;
         tower < finalHighDigit.GetAllElements().size(); ++tower) {
        const auto residues = TowerResidues(finalHighDigit, tower);
        for (const auto& residue : residues) {
            Check(residue == 0,
                  "the appended zero q_div tower did not eliminate the final BV digit");
        }
    }
}

struct RelinPathExecutionCertificate {
    BigInt highPathError;
    BigInt lowPathError;
    BigInt triangleBound;
    std::vector<BigInt> predictedPairErrors;
};

RelinPathExecutionCertificate CheckRelin2PathIdentity(
    const TensorCiphertextPair& tensor,
    const PairOracleResult& tensorPlain,
    const PairOracleResult& actualPlain,
    const lbcrypto::PrivateKey<DCRTPoly>& secretKey,
    const CryptoContext<DCRTPoly>& context) {
    auto raisedHigh = RaiseTensorHighReference(tensor, context);
    lbcrypto::ConstCiphertext<DCRTPoly> raisedHighConst = raisedHigh;
    auto relinearizedHigh = context->Relinearize(raisedHighConst);
    lbcrypto::ConstCiphertext<DCRTPoly> tensorLowConst = tensor.GetLow();
    auto relinearizedLow = context->Relinearize(tensorLowConst);
    Check(relinearizedHigh != nullptr && relinearizedLow != nullptr,
          "Relin2 path oracle received a null public Relinearize result");

    lbcrypto::ConstCiphertext<DCRTPoly> relinearizedHighConst =
        relinearizedHigh;
    auto restrictedHigh = DropFinalTowerReference(relinearizedHighConst);
    ReadOnlyCiphertext restrictedHighReadOnly = restrictedHigh;
    ReadOnlyCiphertext relinearizedLowReadOnly = relinearizedLow;
    const auto highPlain =
        IndependentDecrypt(restrictedHighReadOnly, secretKey);
    const auto lowPlain = IndependentDecrypt(relinearizedLowReadOnly, secretKey);

    const BigInt qDiv(tensor.GetDivisor().ConvertToInt());
    const auto activeModuli = tensorPlain.high.moduli;
    const BigInt qLProduct = tensorPlain.high.modulus;
    const std::size_t ringDimension = tensorPlain.high.coefficients.size();
    Check(activeModuli == tensorPlain.low.moduli &&
              activeModuli == actualPlain.high.moduli &&
              activeModuli == actualPlain.low.moduli &&
              activeModuli == highPlain.moduli &&
              activeModuli == lowPlain.moduli,
          "Relin2 path oracle bases differ");
    Check(highPlain.modulus == qLProduct && lowPlain.modulus == qLProduct,
          "Relin2 path oracle decrypted public paths on the wrong modulus");
    Check(tensorPlain.low.coefficients.size() == ringDimension &&
              actualPlain.recombined.size() == ringDimension &&
              highPlain.coefficients.size() == ringDimension &&
              lowPlain.coefficients.size() == ringDimension,
          "Relin2 path oracle coefficient shapes differ");

    // Conditional public-seam identity for this exact execution:
    //   err(RCB(Relin2(T))) = err(Rel_{Q_l*q_div}(q_div*T_hat)|Q_l)
    //                         + err(Rel_{Q_l}(T_check)).
    // The expected errors come from two independently constructed ordinary
    // Relinearize paths. Production DCP and RCB are not used to construct them.
    BigInt highPathError = 0;
    BigInt lowPathError = 0;
    BigInt pairPathError = 0;
    std::vector<BigInt> predictedPairErrors(ringDimension);
    for (std::size_t coefficient = 0;
         coefficient < ringDimension; ++coefficient) {
        const BigInt highSource = Center(
            qDiv * tensorPlain.high.coefficients[coefficient], qLProduct);
        const BigInt highError = Center(
            highPlain.coefficients[coefficient] - highSource, qLProduct);
        const BigInt lowError = Center(
            lowPlain.coefficients[coefficient] -
                tensorPlain.low.coefficients[coefficient],
            qLProduct);
        const BigInt predictedPairError =
            Center(highError + lowError, qLProduct);
        const BigInt actualPairError = Center(
            actualPlain.recombined[coefficient] -
                tensorPlain.recombined[coefficient],
            qLProduct);
        Check(predictedPairError == actualPairError,
              "Relin2 pair error differs from independent high + low paths at coefficient " +
                  std::to_string(coefficient));
        highPathError = Max(highPathError, Abs(highError));
        lowPathError = Max(lowPathError, Abs(lowError));
        pairPathError = Max(pairPathError, Abs(predictedPairError));
        predictedPairErrors[coefficient] = predictedPairError;
    }

    const BigInt triangleBound = highPathError + lowPathError;
    Check(pairPathError <= triangleBound,
          "Relin2 execution path errors violate their triangle bound");
    return {highPathError, lowPathError, triangleBound,
            std::move(predictedPairErrors)};
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
    BigInt highPathRelinError;
    BigInt lowPathRelinError;
    BigInt executionRelin2TriangleBound;
    BigInt paperAdditivityResidual;
    bool paperAdditivityExecutionObserved;
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
    const auto pathCertificate = CheckRelin2PathIdentity(
        tensor, tensorPlain, relinPlain, secretKey, context);

    Check(tensorPlain.recombined.size() == relinPlain.recombined.size() &&
              tensorPlain.recombined.size() == standardRelinPlain.coefficients.size(),
          "Tensor/Relin plaintext coefficient shape mismatch");
    BigInt empiricalRelinError = 0;
    BigInt empiricalPairRelinError = 0;
    BigInt paperAdditivityResidual = 0;
    for (std::size_t coefficient = 0; coefficient < tensorPlain.recombined.size(); ++coefficient) {
        const BigInt standardDifference = Center(
            standardRelinPlain.coefficients[coefficient] -
                recombinedTensorPlain.coefficients[coefficient],
            qLProduct);
        empiricalRelinError = Max(empiricalRelinError, Abs(standardDifference));
        const BigInt pairDifference = Center(
            relinPlain.recombined[coefficient] - tensorPlain.recombined[coefficient],
            qLProduct);
        Check(pairDifference == pathCertificate.predictedPairErrors[coefficient],
              "Relin2 pair error changed after the independent path oracle");
        empiricalPairRelinError = Max(empiricalPairRelinError, Abs(pairDifference));
        paperAdditivityResidual = Max(
            paperAdditivityResidual,
            Abs(Center(pairDifference - standardDifference, qLProduct)));
    }

    const std::size_t ringDimension = leftPlain.recombined.size();
    Check(ringDimension > 0 && rightPlain.recombined.size() == ringDimension &&
              tensorPlain.recombined.size() == ringDimension &&
              relinPlain.recombined.size() == ringDimension,
          "independent plaintext ring dimensions differ");
    const BigInt n(ringDimension);
    const BigInt h(hammingWeight);
    std::cout << "[RELIN2-EXECUTION]"
              << " q_div=" << qDiv << " q_l=" << qL
              << " ordinary_combined_relin_execution_error=" << empiricalRelinError
              << " pair_relin_execution_error=" << empiricalPairRelinError
              << " high_path_relin_error=" << pathCertificate.highPathError
              << " low_path_relin_error=" << pathCertificate.lowPathError
              << " execution_relin2_triangle_bound=" << pathCertificate.triangleBound
              << " paper_additivity_residual=" << paperAdditivityResidual
              << " paper_additivity_execution_observed="
              << (paperAdditivityResidual <= h ? "true" : "false")
              << " secret_h=" << hammingWeight
              << " input_Q_l=" << qLProduct
              << " oracle_basis_agreement=true\n";
    // A single ordinary Relinearize execution on the recombined Tensor is
    // not a universal E_Relin and does not, without a backend-specific
    // near-additivity argument, bound the two different Relin2 paths. The
    // accepting execution certificate instead uses the independent
    // high/low public-path errors above. This remains conditional on this exact
    // key, ciphertexts, basis, and OpenFHE execution; it is not Theorem 4.8's
    // universal gate.
    Check(empiricalPairRelinError <= pathCertificate.triangleBound,
          "Relin2 pair error exceeded the independent per-path execution bound");
    const BigInt inputEnvelope = mHigh * qDiv + mLow;
    const BigInt nonWrapLeft =
        n * inputEnvelope * inputEnvelope + pathCertificate.triangleBound;
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

    // Execution-only analogue of the corrected Theorem 4.8 expression. The
    // Relin2 term is bounded by errors measured on the independently constructed
    // high and low ordinary-Relinearize paths, not by the result error being
    // accepted and not by assuming that the separate combined call bounds
    // those paths. The final (h+1)/2 term
    // remains the RS2 rounding term. No universal E_Relin is claimed.
    const BigInt empiricalBoundNumerator =
        BigInt(2) * n * mLow * mLow +
        BigInt(2) * qDiv * pathCertificate.triangleBound +
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
            pathCertificate.highPathError,
            pathCertificate.lowPathError,
            pathCertificate.triangleBound,
            paperAdditivityResidual,
            paperAdditivityResidual <= h,
            hammingWeight,
            nonWrapLeft,
            empiricalBoundNumerator,
            empiricalBoundDenominator,
            coefficientErrorNumerator,
            coefficientErrorDenominator};
}

struct FixedKeyBvApplication {
    bool available;
    BigInt conservativeNonWrapLeft;
    BigInt conservativeBoundNumerator;
    BigInt conservativeBoundDenominator;
    BigInt finalIntegerLiftNumerator;
    BigInt finalIntegerLiftRight;
};

FixedKeyBvApplication CheckFixedKeyBvApplication(
    const FixedKeyBvBound* fixedKeyBound,
    const CiphertextPair& input,
    const ArithmeticCertificate& arithmetic) {
    if (fixedKeyBound == nullptr) {
        return {false, 0, 0, 1, 0, 1};
    }

    Check(input.GetOrderedModuli().size() == fixedKeyBound->activeModuli.size(),
          "fixed-key BV bound/input basis length mismatch");
    for (std::size_t tower = 0;
         tower < fixedKeyBound->activeModuli.size(); ++tower) {
        Check(BigInt(input.GetOrderedModuli()[tower].ConvertToInt()) ==
                  fixedKeyBound->activeModuli[tower],
              "fixed-key BV bound/input ordered basis mismatch");
    }
    Check(arithmetic.ringDimension == fixedKeyBound->ringDimension &&
              arithmetic.qLProduct == fixedKeyBound->qLProduct &&
              arithmetic.qDiv == fixedKeyBound->qDiv,
          "fixed-key BV bound does not match the exercised ring or basis");
    Check(fixedKeyBound->perPathRawBound < arithmetic.qLProduct / 2,
          "fixed-key BV per-path bound is only the trivial centered modular bound");
    Check(fixedKeyBound->pairRawBound < arithmetic.qLProduct / 2,
          "fixed-key BV pair bound cannot establish a unique integer lift");
    Check(arithmetic.highPathRelinError <= fixedKeyBound->perPathRawBound,
          "raised-high BV error exceeded the fixed-key ciphertext-uniform bound");
    Check(arithmetic.lowPathRelinError <= fixedKeyBound->perPathRawBound,
          "low BV error exceeded the fixed-key ciphertext-uniform bound");
    Check(arithmetic.empiricalPairRelinError <= fixedKeyBound->pairRawBound,
          "Relin2 pair error exceeded the fixed-key ciphertext-uniform bound");

    const BigInt n(arithmetic.ringDimension);
    const BigInt h(arithmetic.secretHammingWeight);
    const BigInt inputEnvelope =
        arithmetic.mHigh * arithmetic.qDiv + arithmetic.mLow;
    const BigInt conservativeNonWrapLeft =
        n * inputEnvelope * inputEnvelope + fixedKeyBound->pairRawBound;
    Check(BigInt(2) * conservativeNonWrapLeft < arithmetic.qLProduct,
          "fixed-key BV conservative non-wrap witness failed");

    // Directly bounding the two actual BV key-switch paths bypasses Lemma 4.4's
    // three-rounding near-additivity residual, so no extra +h belongs in the
    // Relin2 term. The final (h+1)/2 is retained as the independent RS2 term.
    const BigInt conservativeBoundNumerator =
        BigInt(2) * n * arithmetic.mLow * arithmetic.mLow +
        BigInt(2) * arithmetic.qDiv * fixedKeyBound->pairRawBound +
        arithmetic.qDiv * arithmetic.qL * (h + 1);
    const BigInt conservativeBoundDenominator =
        BigInt(2) * arithmetic.qDiv * arithmetic.qL;
    Check(BigInt(2) * arithmetic.coefficientErrorNumerator <=
              conservativeBoundNumerator,
          "independent coefficient error exceeded the fixed-key BV conservative expression");

    // To compare centered output coefficients with an ordinary integer/rational
    // target after RS2, the target magnitude plus the conservative error must
    // remain below Q_{l-1}/2. With denominator 2*q_div*q_l this is exactly
    //   2*N*(M_high*q_div+M_low)^2 + bound_numerator < q_div*Q_l.
    const BigInt finalIntegerLiftNumerator =
        BigInt(2) * n * inputEnvelope * inputEnvelope +
        conservativeBoundNumerator;
    const BigInt finalIntegerLiftRight = arithmetic.qDiv * arithmetic.qLProduct;
    Check(finalIntegerLiftNumerator < finalIntegerLiftRight,
          "fixed-key BV final centered output lacks a unique intended integer lift");

    return {true,
            conservativeNonWrapLeft,
            conservativeBoundNumerator,
            conservativeBoundDenominator,
            finalIntegerLiftNumerator,
            finalIntegerLiftRight};
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
                      const FixedKeyBvBound* fixedKeyBound,
                      const FixedKeyBvApplication& fixedKeyApplication,
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
              << " ordinary_combined_relin_execution_error="
              << arithmetic.empiricalRelinError
              << " pair_relin_execution_error=" << arithmetic.empiricalPairRelinError
              << " high_path_relin_error=" << arithmetic.highPathRelinError
              << " low_path_relin_error=" << arithmetic.lowPathRelinError
              << " execution_relin2_triangle_bound="
              << arithmetic.executionRelin2TriangleBound
              << " paper_additivity_residual=" << arithmetic.paperAdditivityResidual
              << " paper_additivity_execution_observed="
              << (arithmetic.paperAdditivityExecutionObserved ? "true" : "false")
              << " execution_certificate=PER_PATH_CONDITIONAL"
              << " conservative_E_Relin_available=false"
              << " universal_theorem_gate=UNPROVED"
              << " execution_nonwrap_left=" << arithmetic.nonWrapLeft
              << " execution_nonwrap_right_Q_l_over_2=" << arithmetic.qLProduct / 2
              << " coefficient_error=" << arithmetic.coefficientErrorNumerator
              << '/' << arithmetic.coefficientErrorDenominator
              << " execution_bound=" << arithmetic.empiricalBoundNumerator
              << '/' << arithmetic.empiricalBoundDenominator
              << " fixed_key_bv_bound_available="
              << (fixedKeyApplication.available ? "true" : "false");
    if (fixedKeyApplication.available) {
        Check(fixedKeyBound != nullptr,
              "fixed-key BV application lost its bound descriptor");
        std::cout
            << " fixed_key_bv_bound_status="
               "CANDIDATE_FIXED_KEY_CIPHERTEXT_UNIFORM_CONDITIONAL_ON_CENTERED_DIGIT_LIFT"
            << " fixed_key_bv_noise_scale=" << fixedKeyBound->noiseScale
            << " fixed_key_bv_active_rows=" << fixedKeyBound->activeRows
            << " fixed_key_bv_raised_high_q_div_digit=ZERO"
            << " fixed_key_bv_row_residual_norms=";
        for (std::size_t row = 0;
             row < fixedKeyBound->rowResidualNorms.size(); ++row) {
            if (row != 0) {
                std::cout << ',';
            }
            std::cout << fixedKeyBound->rowResidualNorms[row];
        }
        std::cout
            << " fixed_key_bv_per_path_raw_bound="
            << fixedKeyBound->perPathRawBound
            << " fixed_key_bv_pair_raw_bound=" << fixedKeyBound->pairRawBound
            << " fixed_key_bv_integer_lift_nonwrap=true"
            << " fixed_key_bv_conservative_nonwrap_left="
            << fixedKeyApplication.conservativeNonWrapLeft
            << " fixed_key_bv_conservative_bound="
            << fixedKeyApplication.conservativeBoundNumerator << '/'
            << fixedKeyApplication.conservativeBoundDenominator
            << " fixed_key_bv_final_integer_lift="
            << fixedKeyApplication.finalIntegerLiftNumerator << '<'
            << fixedKeyApplication.finalIntegerLiftRight
            << " fixed_key_bv_unconditional_gaussian_key_bound=false"
            << " fixed_key_bv_universal_theorem_gate=UNPROVED"
            << " centered_digit_boundary_probe=PASSED";
    }
    std::cout << " frozen_decoded_abs_tolerance="
              << kLogicalDecodedAbsoluteTolerance
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

    std::unique_ptr<FixedKeyBvBound> fixedKeyBvBound;
    if (technique == lbcrypto::BV) {
        // This key-only certificate is frozen before plaintext construction,
        // encryption, Tensor2, Relin2, or observation of any path error.
        CheckBvCenteredDigitLiftBoundaryProbe(keys.secretKey);
        fixedKeyBvBound = std::make_unique<FixedKeyBvBound>(
            BuildFixedKeyBvBound(context, keys.secretKey));
    }

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
    if (fixedKeyBvBound != nullptr) {
        CheckBvRelin2DigitDomains(tensor, context);
    }
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
    const auto fixedKeyApplication = CheckFixedKeyBvApplication(
        fixedKeyBvBound.get(), left, arithmetic);
    const auto slots = CheckDecodedSlots(
        result, arithmetic.qL, expected, context, keys.secretKey, module);
    PrintCertificate(caseName, technique, dataType, expected.size(), left,
                     arithmetic, fixedKeyBvBound.get(), fixedKeyApplication,
                     slots);
}

enum class PairInputOperation {
    Add,
    Sub,
};

CiphertextPair ComposeFirstMultInput(const DoubleCKKS& module,
                                     const CiphertextPair& left,
                                     const CiphertextPair& right,
                                     PairInputOperation operation) {
    if (operation == PairInputOperation::Add) {
        return module.Add(left, right);
    }
    if (operation == PairInputOperation::Sub) {
        return module.Sub(left, right);
    }
    throw TestFailure("unsupported pair-input arithmetic operation");
}

BigInt ExpectedPairInputCoefficient(const BigInt& left,
                                    const BigInt& right,
                                    const BigInt& modulus,
                                    PairInputOperation operation) {
    if (operation == PairInputOperation::Add) {
        const BigInt materialized = BigInt(left + right);
        return Center(materialized, modulus);
    }
    if (operation == PairInputOperation::Sub) {
        const BigInt materialized = BigInt(left - right);
        return Center(materialized, modulus);
    }
    throw TestFailure("unsupported pair-input arithmetic oracle operation");
}

void CheckPairInputRecombination(const PairOracleResult& left,
                                 const PairOracleResult& right,
                                 const PairOracleResult& composed,
                                 PairInputOperation operation,
                                 const std::string& label) {
    Check(left.high.moduli == right.high.moduli &&
              left.high.moduli == composed.high.moduli &&
              left.high.modulus == right.high.modulus &&
              left.high.modulus == composed.high.modulus,
          label + " independent pair-input oracle basis mismatch");
    Check(left.recombined.size() == right.recombined.size() &&
              left.recombined.size() == composed.recombined.size(),
          label + " independent pair-input oracle shape mismatch");
    for (std::size_t coefficient = 0; coefficient < left.recombined.size(); ++coefficient) {
        const BigInt expected = ExpectedPairInputCoefficient(
            left.recombined[coefficient], right.recombined[coefficient],
            left.high.modulus, operation);
        Check(composed.recombined[coefficient] == expected,
              label + " recombined coefficient mismatch at coefficient=" +
                  std::to_string(coefficient));
    }
}

void CheckFrozenPairHostArithmetic(
    const std::vector<std::complex<double>>& left,
    const std::vector<std::complex<double>>& right,
    const std::vector<std::complex<double>>& multiplier,
    const std::vector<std::complex<double>>& expectedComposed,
    const std::vector<std::complex<double>>& expectedProduct,
    PairInputOperation operation,
    const std::string& label) {
    for (std::size_t slot = 0; slot < left.size(); ++slot) {
        std::complex<double> materialized;
        if (operation == PairInputOperation::Add) {
            materialized = left[slot] + right[slot];
        }
        else if (operation == PairInputOperation::Sub) {
            materialized = left[slot] - right[slot];
        }
        else {
            throw TestFailure("unsupported frozen host arithmetic operation");
        }
        Check(materialized == expectedComposed[slot],
              label + " frozen composed host literal mismatch at slot=" +
                  std::to_string(slot));
        Check(materialized * multiplier[slot] == expectedProduct[slot],
              label + " frozen product host literal mismatch at slot=" +
                  std::to_string(slot));
    }
}

struct CompositionPairSnapshot {
    PairSnapshot pair;
    double paperRecordedScalingFactor;
    NativeInteger paperDivisor;
    long double logicalScalingFactor;
    long double recombinedLogicalScalingFactor;
};

CompositionPairSnapshot SnapshotCompositionPair(const CiphertextPair& pair,
                                                const std::string& label) {
    const auto& scale = pair.GetPaperScale();
    return {SnapshotPair(pair, label),
            scale.inputRecordedScalingFactor,
            scale.divisor,
            scale.approximateLogicalScalingFactor,
            scale.approximateRecombinedLogicalScalingFactor};
}

void CheckCompositionPairUnchanged(const CiphertextPair& pair,
                                   const CompositionPairSnapshot& snapshot,
                                   const std::string& label) {
    CheckPairUnchanged(pair, snapshot.pair, label);
    const auto& scale = pair.GetPaperScale();
    Check(scale.inputRecordedScalingFactor == snapshot.paperRecordedScalingFactor,
          label + " paper recorded factor changed");
    Check(scale.divisor == snapshot.paperDivisor,
          label + " paper divisor changed");
    Check(scale.approximateLogicalScalingFactor == snapshot.logicalScalingFactor,
          label + " logical scale changed");
    Check(scale.approximateRecombinedLogicalScalingFactor ==
              snapshot.recombinedLogicalScalingFactor,
          label + " recombined logical scale changed");
}

void CheckReadyForFirstMultInputState(const CiphertextPair& input,
                                      const CiphertextPair& reference,
                                      const CryptoContext<DCRTPoly>& context,
                                      const std::string& label) {
    Check(reference.GetLifecycle() == PairLifecycle::ReadyForFirstMult,
          label + " reference pair is not ReadyForFirstMult");
    Check(input.GetLifecycle() == PairLifecycle::ReadyForFirstMult,
          label + " is not ReadyForFirstMult");
    Check(input.GetContextIdentity() == context.get() &&
              input.GetContextIdentity() == reference.GetContextIdentity(),
          label + " context identity changed");
    Check(input.GetDivisor() == reference.GetDivisor(),
          label + " divisor changed");
    Check(input.GetOrderedModuli() == reference.GetOrderedModuli(),
          label + " ordered RNS basis changed");
    Check(input.GetLevel() == 1 && input.GetLevel() == reference.GetLevel(),
          label + " level changed");
    Check(input.GetRecordedScalingFactor() == reference.GetRecordedScalingFactor(),
          label + " recorded scaling factor changed");
    Check(input.GetNoiseScaleDegree() == 2 &&
              input.GetNoiseScaleDegree() == reference.GetNoiseScaleDegree(),
          label + " noise-scale degree changed");
    Check(input.GetKeyTag() == reference.GetKeyTag(),
          label + " key tag changed");
    Check(input.GetSlots() == reference.GetSlots(),
          label + " slots changed");
    Check(input.GetFormat() == Format::EVALUATION &&
              input.GetFormat() == reference.GetFormat(),
          label + " format changed");
    Check(input.GetComponentCount() == 2 &&
              input.GetComponentCount() == reference.GetComponentCount(),
          label + " component count changed");
    const auto& inputScale = input.GetPaperScale();
    const auto& referenceScale = reference.GetPaperScale();
    Check(inputScale.inputRecordedScalingFactor ==
              referenceScale.inputRecordedScalingFactor,
          label + " paper recorded scaling factor changed");
    Check(inputScale.divisor == referenceScale.divisor,
          label + " paper divisor changed");
    Check(inputScale.approximateLogicalScalingFactor ==
              referenceScale.approximateLogicalScalingFactor,
          label + " logical scaling factor changed");
    Check(inputScale.approximateRecombinedLogicalScalingFactor ==
              referenceScale.approximateRecombinedLogicalScalingFactor,
          label + " recombined logical scaling factor changed");
    CheckPhysicalCiphertextState(
        input.GetHigh(), input.GetOrderedModuli(), input.GetLevel(),
        input.GetNoiseScaleDegree(), input.GetRecordedScalingFactor(),
        input.GetKeyTag(), input.GetSlots(), context, label + " high");
    CheckPhysicalCiphertextState(
        input.GetLow(), input.GetOrderedModuli(), input.GetLevel(),
        input.GetNoiseScaleDegree(), input.GetRecordedScalingFactor(),
        input.GetKeyTag(), input.GetSlots(), context, label + " low");
}

void RunPairArithmeticInputCase(
    const std::string& caseName,
    PairInputOperation operation,
    const std::vector<std::complex<double>>& leftValues,
    const std::vector<std::complex<double>>& rightValues,
    const std::vector<std::complex<double>>& multiplierValues,
    const std::vector<std::complex<double>>& expectedComposedValues,
    const std::vector<std::complex<double>>& expectedProductValues) {
    Check(leftValues.size() == rightValues.size() &&
              leftValues.size() == multiplierValues.size() &&
              leftValues.size() == expectedComposedValues.size() &&
              leftValues.size() == expectedProductValues.size(),
          caseName + " frozen host-vector lengths differ");
    CheckHostEnvelope(leftValues);
    CheckHostEnvelope(rightValues);
    CheckHostEnvelope(multiplierValues);
    CheckHostEnvelope(expectedComposedValues);
    CheckFrozenPairHostArithmetic(
        leftValues, rightValues, multiplierValues, expectedComposedValues,
        expectedProductValues, operation, caseName);

    auto context = MakeContext(lbcrypto::HYBRID, lbcrypto::COMPLEX);
    Check(context->GetCKKSDataType() == lbcrypto::COMPLEX,
          caseName + " context must preserve complex slots");
    const auto keys = context->KeyGen();
    Check(keys.good(), caseName + " key generation failed");
    context->EvalMultKeyGen(keys.secretKey);

    const auto leftPlaintext = context->MakeCKKSPackedPlaintext(
        leftValues, kEncodingNoiseScaleDegree, kEncodingLevel);
    const auto rightPlaintext = context->MakeCKKSPackedPlaintext(
        rightValues, kEncodingNoiseScaleDegree, kEncodingLevel);
    const auto multiplierPlaintext = context->MakeCKKSPackedPlaintext(
        multiplierValues, kEncodingNoiseScaleDegree, kEncodingLevel);
    const auto& leftCachedValues = leftPlaintext->GetCKKSPackedValue();
    const auto& rightCachedValues = rightPlaintext->GetCKKSPackedValue();
    const auto& multiplierCachedValues = multiplierPlaintext->GetCKKSPackedValue();
    Check(!leftCachedValues.empty() && leftCachedValues.front().imag() == -0.0625,
          caseName + " left plaintext discarded the literal imaginary witness");
    Check(!rightCachedValues.empty() && rightCachedValues.front().imag() == 0.03125,
          caseName + " right plaintext discarded the literal imaginary witness");
    Check(!multiplierCachedValues.empty() &&
              multiplierCachedValues.front().imag() == 0.125,
          caseName + " multiplier plaintext discarded the literal imaginary witness");

    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);
    const auto multiplierInput = context->Encrypt(multiplierPlaintext, keys.publicKey);

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto multiplier = module.DCP(multiplierInput);
    Check(left.GetOrderedModuli().size() >= 4,
          caseName + " retained too few active Q_l towers");
    CheckPreselectedModulusBudget(left);

    const auto leftInputBefore = SnapshotCiphertext(
        leftInput, caseName + " left encrypted input");
    const auto rightInputBefore = SnapshotCiphertext(
        rightInput, caseName + " right encrypted input");
    const auto multiplierInputBefore = SnapshotCiphertext(
        multiplierInput, caseName + " multiplier encrypted input");
    const auto leftBefore = SnapshotCompositionPair(left, caseName + " left pair");
    const auto rightBefore = SnapshotCompositionPair(right, caseName + " right pair");
    const auto multiplierBefore = SnapshotCompositionPair(
        multiplier, caseName + " multiplier pair");

    const auto leftPlain = IndependentDecryptPair(left, keys.secretKey);
    const auto rightPlain = IndependentDecryptPair(right, keys.secretKey);
    const auto composed = ComposeFirstMultInput(module, left, right, operation);
    CheckReadyForFirstMultInputState(
        composed, left, context, caseName + " composed pair");
    CheckReadyForFirstMultInputState(
        multiplier, left, context, caseName + " multiplier pair");
    CheckPreselectedModulusBudget(composed);
    const auto composedPlain = IndependentDecryptPair(composed, keys.secretKey);
    CheckPairInputRecombination(
        leftPlain, rightPlain, composedPlain, operation, caseName);
    const auto composedBefore = SnapshotCompositionPair(
        composed, caseName + " composed pair before Mult2");

    const auto& evalKeysBefore =
        lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evalKeysBefore.find(keys.secretKey->GetKeyTag()) != evalKeysBefore.end(),
          caseName + " fixture EvalMultKey row is absent before Mult2");

    // Equality with the staged public composition is wiring evidence only. The
    // coefficient and decoded-slot expectations below come from independent
    // source-pair CRT arithmetic and frozen host literals, respectively.
    const auto tensor = module.Tensor2(composed, multiplier);
    const auto relinearized = module.Relin2(tensor);
    const auto stagedResult = module.RS2(relinearized);
    const auto result = module.Mult2(composed, multiplier);

    CheckPublicResultState(result, composed, context);
    CheckPublicResultState(stagedResult, composed, context);
    Check(*result.GetHigh() == *stagedResult.GetHigh() &&
              *result.GetLow() == *stagedResult.GetLow(),
          caseName + " Mult2 differs from Tensor2->Relin2->RS2");

    const auto arithmetic = CheckIndependentArithmetic(
        composed, multiplier, tensor, relinearized, result, keys.secretKey, context);
    const auto slots = CheckDecodedSlots(
        result, arithmetic.qL, expectedProductValues, context, keys.secretKey, module);

    CheckCompositionPairUnchanged(left, leftBefore, caseName + " left pair");
    CheckCompositionPairUnchanged(right, rightBefore, caseName + " right pair");
    CheckCompositionPairUnchanged(
        multiplier, multiplierBefore, caseName + " multiplier pair");
    CheckCompositionPairUnchanged(
        composed, composedBefore, caseName + " composed pair after Mult2");
    CheckCiphertextUnchanged(
        leftInput, leftInputBefore, caseName + " left encrypted input");
    CheckCiphertextUnchanged(
        rightInput, rightInputBefore, caseName + " right encrypted input");
    CheckCiphertextUnchanged(
        multiplierInput, multiplierInputBefore,
        caseName + " multiplier encrypted input");
    const auto& evalKeysAfter =
        lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evalKeysAfter.find(keys.secretKey->GetKeyTag()) != evalKeysAfter.end(),
          caseName + " fixture EvalMultKey row disappeared during composition");

    const auto fixedKeyApplication =
        CheckFixedKeyBvApplication(nullptr, composed, arithmetic);
    PrintCertificate(caseName, lbcrypto::HYBRID, lbcrypto::COMPLEX,
                     expectedProductValues.size(), composed, arithmetic,
                     nullptr, fixedKeyApplication, slots);
}

std::vector<std::complex<double>> FrozenPairLeftValues() {
    return {{0.125, -0.0625},
            {-0.25, 0.125},
            {0.0, 0.0},
            {0.0625, 0.0625},
            {-0.125, -0.25},
            {std::ldexp(1.0, -20), -std::ldexp(1.0, -21)},
            {0.1875, 0.0},
            {0.0, 0.125}};
}

std::vector<std::complex<double>> FrozenPairRightValues() {
    return {{0.0625, 0.03125},
            {0.125, -0.0625},
            {0.0, 0.0625},
            {-0.03125, 0.0625},
            {0.0625, 0.125},
            {-std::ldexp(1.0, -20), std::ldexp(1.0, -22)},
            {-0.0625, 0.03125},
            {0.0625, -0.0625}};
}

std::vector<std::complex<double>> FrozenPairMultiplierValues() {
    return {{-0.25, 0.125},
            {0.125, 0.125},
            {-0.125, 0.0},
            {0.0, -0.25},
            {0.25, -0.125},
            {0.5, 0.25},
            {-0.125, 0.25},
            {0.25, 0.0}};
}

std::vector<std::complex<double>> FrozenPairSumValues() {
    return {{0.1875, -0.03125},
            {-0.125, 0.0625},
            {0.0, 0.0625},
            {0.03125, 0.125},
            {-0.0625, -0.125},
            {0.0, -std::ldexp(1.0, -22)},
            {0.125, 0.03125},
            {0.0625, 0.0625}};
}

std::vector<std::complex<double>> FrozenPairSumTimesMultiplierValues() {
    return {{-0.04296875, 0.03125},
            {-0.0234375, -0.0078125},
            {0.0, -0.0078125},
            {0.03125, -0.0078125},
            {-0.03125, -0.0234375},
            {std::ldexp(1.0, -24), -std::ldexp(1.0, -23)},
            {-0.0234375, 0.02734375},
            {0.015625, 0.015625}};
}

std::vector<std::complex<double>> FrozenPairDifferenceValues() {
    return {{0.0625, -0.09375},
            {-0.375, 0.1875},
            {0.0, -0.0625},
            {0.09375, 0.0},
            {-0.1875, -0.375},
            {std::ldexp(1.0, -19), -3.0 * std::ldexp(1.0, -22)},
            {0.25, -0.03125},
            {-0.0625, 0.1875}};
}

std::vector<std::complex<double>> FrozenPairDifferenceTimesMultiplierValues() {
    return {{-0.00390625, 0.03125},
            {-0.0703125, -0.0234375},
            {0.0, 0.0078125},
            {0.0, -0.0234375},
            {-0.09375, -0.0703125},
            {19.0 * std::ldexp(1.0, -24), std::ldexp(1.0, -23)},
            {-0.0234375, 0.06640625},
            {-0.015625, 0.046875}};
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
    if (name == "pair_add_input_hybrid_complex") {
        const auto left = FrozenPairLeftValues();
        const auto right = FrozenPairRightValues();
        const auto multiplier = FrozenPairMultiplierValues();
        const auto sum = FrozenPairSumValues();
        const auto product = FrozenPairSumTimesMultiplierValues();
        RunPairArithmeticInputCase(
            name, PairInputOperation::Add, left, right, multiplier, sum, product);
        return;
    }
    if (name == "pair_sub_input_hybrid_complex") {
        const auto left = FrozenPairLeftValues();
        const auto right = FrozenPairRightValues();
        const auto multiplier = FrozenPairMultiplierValues();
        const auto difference = FrozenPairDifferenceValues();
        const auto product = FrozenPairDifferenceTimesMultiplierValues();
        RunPairArithmeticInputCase(
            name, PairInputOperation::Sub, left, right, multiplier, difference, product);
        return;
    }
    throw TestFailure("unknown Mult2 end-to-end case: " + name);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: mult2_e2e_oracle_test "
                     "hybrid_real|hybrid_complex|bv_real|bv_complex|"
                     "pair_add_input_hybrid_complex|pair_sub_input_hybrid_complex\n";
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
