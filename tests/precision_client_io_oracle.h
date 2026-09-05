#ifndef OPENFHE_2023_1788_PRECISION_CLIENT_IO_ORACLE_H
#define OPENFHE_2023_1788_PRECISION_CLIENT_IO_ORACLE_H

// Test-only arithmetic and schoolbook/CRT/Horner oracle, adapted solely from
// this clean-room baseline's precision_first_mult2_contract_test.cpp at
// 4ecbd972429884489918d9f82dfc3fe9f702ef4a. No Plaintext fixture, production
// client transform, or official RLWE decrypt implementation is used here.
// Literal vectors/products are frozen by PROPOSED_FIRST_TDD_CONTRACT.md.
#include "openfhe.h"
#include "openfhe_2023_1788/double_ckks.h"
#include <boost/math/constants/constants.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace lossless_client_io_test {
using BigFloat = boost::multiprecision::cpp_dec_float_100;
using BigInt = boost::multiprecision::cpp_int;
struct MpComplex final { BigFloat real; BigFloat imag; };
using lbcrypto::DCRTPoly;
using openfhe_2023_1788::ReadOnlyCiphertext;
constexpr std::uint32_t kRingDimension = 64;
constexpr std::uint32_t kBatchSize = 16;
constexpr std::uint32_t kCyclotomicOrder = 128;
class TestFailure final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void Check(bool condition, const std::string& message) {
    if (!condition) {
        throw TestFailure(message);
    }
}


BigFloat Pow2Float(std::uint32_t bits) {
    BigFloat result = 1;
    for (std::uint32_t bit = 0; bit < bits; ++bit) {
        result *= 2;
    }
    return result;
}


BigInt Pow2Integer(std::uint32_t bits) {
    return BigInt(1) << bits;
}


BigFloat ToBigFloat(const BigInt& value) {
    return BigFloat(value.convert_to<std::string>());
}


BigInt Abs(const BigInt& value) {
    return value < 0 ? -value : value;
}


BigInt Max(const BigInt& left, const BigInt& right) {
    return left < right ? right : left;
}


BigFloat AbsFloat(const BigFloat& value) {
    return value < 0 ? -value : value;
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


BigInt ExtendedGcd(BigInt left, BigInt right, BigInt& x, BigInt& y) {
    if (right == 0) {
        x = 1;
        y = 0;
        return left;
    }
    BigInt nextX;
    BigInt nextY;
    const BigInt gcd = ExtendedGcd(right, left % right, nextX, nextY);
    x = nextY;
    y = nextX - (left / right) * nextY;
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
    const std::size_t size = left.size();
    std::vector<BigInt> result(size, 0);
    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t j = 0; j < size; ++j) {
            const BigInt term = left[i] * right[j];
            const std::size_t rawIndex = i + j;
            if (rawIndex < size) {
                result[rawIndex] += term;
            }
            else {
                result[rawIndex - size] -= term;
            }
        }
    }
    for (auto& coefficient : result) {
        coefficient = PositiveMod(coefficient, modulus);
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


struct DecryptionOracleResult final {
    std::vector<BigInt> coefficients;
    std::vector<BigInt> moduli;
    BigInt modulus;
};

DecryptionOracleResult IndependentDecrypt(
    const ReadOnlyCiphertext& ciphertext,
    const lbcrypto::PrivateKey<DCRTPoly>& secretKey) {
    Check(ciphertext != nullptr, "independent decryption received a null ciphertext");
    Check(secretKey != nullptr, "independent decryption received a null secret key");
    Check(!ciphertext->GetElements().empty(),
          "independent decryption received no RLWE components");

    std::vector<DCRTPoly> components;
    components.reserve(ciphertext->GetElements().size());
    for (const auto& element : ciphertext->GetElements()) {
        components.push_back(ToCoefficient(element));
    }
    const DCRTPoly secret = ToCoefficient(secretKey->GetPrivateElement());
    const auto moduli = GetModuli(components.front());
    Check(!moduli.empty(), "independent decryption received an empty RNS basis");
    Check(secret.GetAllElements().size() >= moduli.size(),
          "secret-key basis is shorter than ciphertext basis");

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
        Check(BigInt(secret.GetAllElements().at(tower).GetModulus().ConvertToInt()) ==
                  moduli[tower],
              "secret-key tower does not match ciphertext tower");
        const auto secretResidues = TowerResidues(secret, tower);
        Check(secretResidues.size() == ringDimension,
              "secret-key coefficient vector has wrong length");

        std::vector<BigInt> decrypted = TowerResidues(components.front(), tower);
        Check(decrypted.size() == ringDimension,
              "ciphertext coefficient vector has wrong length");
        std::vector<BigInt> secretPower(ringDimension, 0);
        secretPower.front() = 1;
        for (std::size_t component = 1; component < components.size(); ++component) {
            secretPower = NegacyclicProductMod(secretPower, secretResidues, moduli[tower]);
            const auto componentResidues = TowerResidues(components[component], tower);
            const auto term =
                NegacyclicProductMod(componentResidues, secretPower, moduli[tower]);
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


MpComplex Add(const MpComplex& left, const MpComplex& right) {
    return {left.real + right.real, left.imag + right.imag};
}


MpComplex Subtract(const MpComplex& left, const MpComplex& right) {
    return {left.real - right.real, left.imag - right.imag};
}


MpComplex Multiply(const MpComplex& left, const MpComplex& right) {
    return {left.real * right.real - left.imag * right.imag,
            left.real * right.imag + left.imag * right.real};
}


MpComplex Scale(const MpComplex& value, const BigFloat& factor) {
    return {value.real * factor, value.imag * factor};
}


BigFloat Magnitude(const MpComplex& value) {
    using boost::multiprecision::sqrt;
    return sqrt(value.real * value.real + value.imag * value.imag);
}


MpComplex RootForExponent(std::uint32_t exponent) {
    const BigFloat pi = boost::math::constants::pi<BigFloat>();
    const BigFloat angle =
        BigFloat(2) * pi * BigFloat(exponent) / BigFloat(kCyclotomicOrder);
    using boost::multiprecision::cos;
    using boost::multiprecision::sin;
    return {cos(angle), sin(angle)};
}


MpComplex DirectEvaluateRational(const std::vector<BigInt>& coefficients,
                                 std::uint32_t exponent,
                                 const BigInt& scaleNumerator,
                                 const BigInt& scaleDenominator) {
    Check(scaleNumerator > 0 && scaleDenominator > 0,
          "direct evaluator received an invalid exact scale");
    const MpComplex root = RootForExponent(exponent);
    MpComplex accumulator{BigFloat(0), BigFloat(0)};
    for (auto coefficient = coefficients.rbegin(); coefficient != coefficients.rend();
         ++coefficient) {
        accumulator = Add(Multiply(accumulator, root),
                          {ToBigFloat(*coefficient), BigFloat(0)});
    }
    return Scale(accumulator,
                 ToBigFloat(scaleDenominator) / ToBigFloat(scaleNumerator));
}


std::array<std::uint32_t, kBatchSize> CanonicalExponents() {
    return {1U, 5U, 25U, 125U, 113U, 53U, 9U, 45U,
            97U, 101U, 121U, 93U, 81U, 21U, 105U, 13U};
}


std::vector<MpComplex> DirectCanonicalEvaluateRational(
    const std::vector<BigInt>& coefficients,
    const BigInt& scaleNumerator,
    const BigInt& scaleDenominator) {
    std::vector<MpComplex> result;
    result.reserve(kBatchSize);
    for (const std::uint32_t exponent : CanonicalExponents()) {
        result.push_back(DirectEvaluateRational(
            coefficients, exponent, scaleNumerator, scaleDenominator));
    }
    return result;
}


std::vector<MpComplex> X2WitnessTable() {
    return {
        {BigFloat("0.99518472667219688624483695310947992157547486872985706183361296578489016689458654"), BigFloat("0.098017140329560601994195563888641845861136673167500567257264979809387302789087537")},
        {BigFloat("0.88192126434835502971275686366038834950844262067472798063253861671206664704500350"), BigFloat("0.47139673682599764855638762590525437765746031893248062140161403100883522166516175")},
        {BigFloat("-0.77301045336273696081090660975846980097104129290080960935640289668795060530598730"), BigFloat("0.63439328416364549821517161322549337067568709484172160643382471869668672612354839")},
        {BigFloat("0.95694033573220886493579788698026996948284920563003726130120719988416014536816082"), BigFloat("-0.29028467725446236763619237581739527469147627832415111142066711312539289829945745")},
        {BigFloat("0.098017140329560601994195563888641845861136673167500567257264979809387302789087537"), BigFloat("-0.99518472667219688624483695310947992157547486872985706183361296578489016689458654")},
        {BigFloat("0.47139673682599764855638762590525437765746031893248062140161403100883522166516175"), BigFloat("-0.88192126434835502971275686366038834950844262067472798063253861671206664704500350")},
        {BigFloat("0.63439328416364549821517161322549337067568709484172160643382471869668672612354839"), BigFloat("0.77301045336273696081090660975846980097104129290080960935640289668795060530598730")},
        {BigFloat("-0.29028467725446236763619237581739527469147627832415111142066711312539289829945745"), BigFloat("-0.95694033573220886493579788698026996948284920563003726130120719988416014536816082")},
        {BigFloat("-0.99518472667219688624483695310947992157547486872985706183361296578489016689458654"), BigFloat("-0.098017140329560601994195563888641845861136673167500567257264979809387302789087537")},
        {BigFloat("-0.88192126434835502971275686366038834950844262067472798063253861671206664704500350"), BigFloat("-0.47139673682599764855638762590525437765746031893248062140161403100883522166516175")},
        {BigFloat("0.77301045336273696081090660975846980097104129290080960935640289668795060530598730"), BigFloat("-0.63439328416364549821517161322549337067568709484172160643382471869668672612354839")},
        {BigFloat("-0.95694033573220886493579788698026996948284920563003726130120719988416014536816082"), BigFloat("0.29028467725446236763619237581739527469147627832415111142066711312539289829945745")},
        {BigFloat("-0.098017140329560601994195563888641845861136673167500567257264979809387302789087537"), BigFloat("0.99518472667219688624483695310947992157547486872985706183361296578489016689458654")},
        {BigFloat("-0.47139673682599764855638762590525437765746031893248062140161403100883522166516175"), BigFloat("0.88192126434835502971275686366038834950844262067472798063253861671206664704500350")},
        {BigFloat("-0.63439328416364549821517161322549337067568709484172160643382471869668672612354839"), BigFloat("-0.77301045336273696081090660975846980097104129290080960935640289668795060530598730")},
        {BigFloat("0.29028467725446236763619237581739527469147627832415111142066711312539289829945745"), BigFloat("0.95694033573220886493579788698026996948284920563003726130120719988416014536816082")},
    };
}


std::vector<MpComplex> LeftValues() {
    const BigFloat p40 = BigFloat(1) / Pow2Float(40);
    const BigFloat p45 = BigFloat(1) / Pow2Float(45);
    const BigFloat p60 = BigFloat(1) / Pow2Float(60);
    const BigFloat p65 = BigFloat(1) / Pow2Float(65);
    const BigFloat p70 = BigFloat(1) / Pow2Float(70);
    const BigFloat p73 = BigFloat(1) / Pow2Float(73);
    const BigFloat p80 = BigFloat(1) / Pow2Float(80);
    return {
        {BigFloat("0.125"), BigFloat("-0.0625")},
        {BigFloat("0.125") + p70, BigFloat("-0.0625") + p73},
        {BigFloat("0.125"), BigFloat("0.25")},
        {BigFloat("0.123456789012345678901234567890"), BigFloat("-0.234567890123456789012345678901")},
        {BigFloat("-0.314159265358979323846264338327"), BigFloat("0.271828182845904523536028747135")},
        {-p60, p65},
        {BigFloat("1.234567890123456789e-19"), BigFloat("-9.876543210987654321e-19")},
        {BigFloat(0), BigFloat(0)},
        {BigFloat("-0.499999999999999999999999999999"), BigFloat("0.333333333333333333333333333333")},
        {p40, -p45},
        {BigFloat("-0.125") - p70, BigFloat("0.0625") - p73},
        {BigFloat("0.2"), BigFloat("-0.142857142857142857142857142857")},
        {BigFloat("1.23456789e-28"), BigFloat("-9.87654321e-28")},
        {BigFloat("0.375"), BigFloat("-0.4375")},
        {BigFloat("-0.0009765625"), BigFloat("0.001953125")},
        {BigFloat(0), p80},
    };
}


std::vector<MpComplex> RightValues() {
    const BigFloat p20 = BigFloat(1) / Pow2Float(20);
    const BigFloat p22 = BigFloat(1) / Pow2Float(22);
    return {
        {BigFloat("0.5"), BigFloat("-0.25")},
        {BigFloat("0.5"), BigFloat("-0.25")},
        {BigFloat("-0.5"), BigFloat("0.125")},
        {BigFloat("-0.271828182845904523536028747135"), BigFloat("0.314159265358979323846264338327")},
        {BigFloat("0.2"), BigFloat("-0.1")},
        {BigFloat("-0.25"), BigFloat("0.375")},
        {BigFloat("0.333333333333333333333333333333"), BigFloat("-0.2")},
        {BigFloat("0.411111111111111111111111111111"), BigFloat("-0.377777777777777777777777777777")},
        {BigFloat("-0.125"), BigFloat("-0.25")},
        {p20, -p22},
        {BigFloat("0.5"), BigFloat("0.25")},
        {BigFloat("-0.4"), BigFloat("0.3")},
        {BigFloat("0.25"), BigFloat("-0.125")},
        {BigFloat("-0.125"), BigFloat("0.0625")},
        {BigFloat("0.7"), BigFloat("-0.2")},
        {BigFloat("0.5"), BigFloat(0)},
    };
}


std::vector<MpComplex> ElementwiseProduct(const std::vector<MpComplex>& left,
                                          const std::vector<MpComplex>& right) {
    Check(left.size() == right.size(), "literal input-vector lengths differ");
    std::vector<MpComplex> result;
    result.reserve(left.size());
    for (std::size_t slot = 0; slot < left.size(); ++slot) {
        result.push_back(Multiply(left[slot], right[slot]));
    }
    return result;
}


std::vector<MpComplex> FrozenExpectedProducts() {
    return {
        {BigFloat("0.046875"), BigFloat("-0.0625")},
        {BigFloat("0.046875000000000000000449986253228847055130046328486059792339801788330078125"),
         BigFloat("-0.06250000000000000000015881867761018131357531046887743286788463592529296875")},
        {BigFloat("-0.09375"), BigFloat("-0.109375")},
        {BigFloat("0.040132641420774808949887288570205500563303788511522569043477"),
         BigFloat("0.102547257465954082897938387693155713378359871980684644218665")},
        {BigFloat("-0.0356490347872054124156499929519"),
         BigFloat("0.0857815631050788370918321832597")},
        {BigFloat("0.00000000000000000020667603913004928273267069016583263874053955078125"),
         BigFloat("-0.0000000000000000003320369153236857329147824202664196491241455078125")},
        {BigFloat("-0.0000000000000000001563786012156378601200000000000411522630041152263"),
         BigFloat("-0.0000000000000000003539094648353909464799999999996707818929670781893")},
        {BigFloat("0"), BigFloat("0")},
        {BigFloat("0.145833333333333333333333333333125"),
         BigFloat("0.083333333333333333333333333333125")},
        {BigFloat("0.0000000000000000008605854744103691444934156606905162334442138671875"),
         BigFloat("-0.00000000000000000024394548880923849765167688019573688507080078125")},
        {BigFloat("-0.078125000000000000000397046694025453283938276172193582169711589813232421875"),
         BigFloat("-0.00000000000000000000026469779601696885595885078146238811314105987548828125")},
        {BigFloat("-0.0371428571428571428571428571429"),
         BigFloat("0.1171428571428571428571428571428")},
        {BigFloat("-0.000000000000000000000000000092592592875"),
         BigFloat("-0.000000000000000000000000000262345678875")},
        {BigFloat("-0.01953125"), BigFloat("0.078125")},
        {BigFloat("-0.00029296875"), BigFloat("0.0015625")},
        {BigFloat("0"),
         BigFloat("0.000000000000000000000000413590306276513837435704346034981426782906055450439453125")},
    };
}


MpComplex FrozenProductDelta() {
    const BigFloat p71 = BigFloat(1) / Pow2Float(71);
    const BigFloat p72 = BigFloat(1) / Pow2Float(72);
    const BigFloat p74 = BigFloat(1) / Pow2Float(74);
    const BigFloat p75 = BigFloat(1) / Pow2Float(75);
    return {p71 + p75, -p72 + p74};
}


BigInt MaximumAbsolute(const std::vector<BigInt>& values) {
    BigInt maximum = 0;
    for (const auto& value : values) {
        maximum = Max(maximum, Abs(value));
    }
    return maximum;
}


BigFloat MaximumSlotError(const std::vector<MpComplex>& actual,
                          const std::vector<MpComplex>& expected) {
    Check(actual.size() == expected.size(), "actual/expected slot-count mismatch");
    BigFloat maximum = 0;
    for (std::size_t slot = 0; slot < expected.size(); ++slot) {
        const BigFloat error = Magnitude(Subtract(actual[slot], expected[slot]));
        if (error > maximum) {
            maximum = error;
        }
    }
    return maximum;
}


}  // namespace lossless_client_io_test
#endif  // OPENFHE_2023_1788_PRECISION_CLIENT_IO_ORACLE_H
