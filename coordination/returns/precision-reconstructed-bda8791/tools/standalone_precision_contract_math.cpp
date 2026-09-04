#include <boost/math/constants/constants.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using BigFloat = boost::multiprecision::cpp_dec_float_100;
using BigInt = boost::multiprecision::cpp_int;

constexpr std::size_t kRingDimension = 64;
constexpr std::size_t kSlots = 16;
constexpr std::uint32_t kScaleBits = 100;
constexpr std::uint32_t kAcceptanceBits = 80;
constexpr std::uint32_t kCyclotomicOrder = 128;

struct MpComplex {
    BigFloat real;
    BigFloat imag;
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

MpComplex Add(const MpComplex& a, const MpComplex& b) {
    return {a.real + b.real, a.imag + b.imag};
}
MpComplex Subtract(const MpComplex& a, const MpComplex& b) {
    return {a.real - b.real, a.imag - b.imag};
}
MpComplex Multiply(const MpComplex& a, const MpComplex& b) {
    return {a.real * b.real - a.imag * b.imag,
            a.real * b.imag + a.imag * b.real};
}
BigFloat Magnitude(const MpComplex& value) {
    using boost::multiprecision::sqrt;
    return sqrt(value.real * value.real + value.imag * value.imag);
}
BigFloat Pow2(std::uint32_t bits) {
    BigFloat result = 1;
    for (std::uint32_t i = 0; i < bits; ++i) {
        result *= 2;
    }
    return result;
}
BigInt RoundHalfAwayFromZero(const BigFloat& value) {
    BigFloat rounded;
    if (value >= 0) {
        rounded = boost::multiprecision::floor(value + BigFloat("0.5"));
    }
    else {
        rounded = boost::multiprecision::ceil(value - BigFloat("0.5"));
    }
    return rounded.convert_to<BigInt>();
}
BigFloat ToBigFloat(const BigInt& value) {
    return BigFloat(value.convert_to<std::string>());
}

std::array<std::uint32_t, kSlots> CanonicalExponents() {
    return {1U, 5U, 25U, 125U, 113U, 53U, 9U, 45U,
            97U, 101U, 121U, 93U, 81U, 21U, 105U, 13U};
}

std::vector<MpComplex> Roots(std::uint32_t order) {
    std::vector<MpComplex> roots(static_cast<std::size_t>(order) + 1U);
    const BigFloat pi = boost::math::constants::pi<BigFloat>();
    const BigFloat denominator(order);
    using boost::multiprecision::cos;
    using boost::multiprecision::sin;
    for (std::uint32_t i = 0; i < order; ++i) {
        const BigFloat angle = BigFloat(2) * pi * BigFloat(i) / denominator;
        roots[i] = {cos(angle), sin(angle)};
    }
    roots[order] = roots[0];
    return roots;
}

std::vector<std::uint32_t> RotationGroup(std::size_t size, std::uint32_t order) {
    std::vector<std::uint32_t> result(size);
    std::uint64_t power = 1;
    for (std::size_t i = 0; i < size; ++i) {
        result[i] = static_cast<std::uint32_t>(power);
        power = (power * 5U) % order;
    }
    return result;
}

void BitReverse(std::vector<MpComplex>& values) {
    const std::size_t size = values.size();
    for (std::size_t i = 1, j = 0; i < size; ++i) {
        std::size_t bit = size >> 1U;
        for (; j >= bit; bit >>= 1U) {
            j -= bit;
        }
        j += bit;
        if (i < j) {
            std::swap(values[i], values[j]);
        }
    }
}

void FftSpecialInverse(std::vector<MpComplex>& values, std::uint32_t order) {
    const auto rotation = RotationGroup(values.size(), order);
    const auto roots = Roots(order);
    const std::size_t size = values.size();
    for (std::size_t length = size; length >= 1; length >>= 1U) {
        const std::size_t half = length >> 1U;
        const std::size_t quarter = length << 2U;
        const std::size_t gap = static_cast<std::size_t>(order) / quarter;
        for (std::size_t offset = 0; offset < size; offset += length) {
            for (std::size_t j = 0; j < half; ++j) {
                const std::size_t rootIndex = (quarter - (rotation[j] % quarter)) * gap;
                const MpComplex sum = Add(values[offset + j], values[offset + j + half]);
                MpComplex difference = Subtract(values[offset + j], values[offset + j + half]);
                difference = Multiply(difference, roots[rootIndex]);
                values[offset + j] = sum;
                values[offset + j + half] = difference;
            }
        }
        if (length == 1) {
            break;
        }
    }
    BitReverse(values);
    for (auto& value : values) {
        value.real /= BigFloat(size);
        value.imag /= BigFloat(size);
    }
}

std::vector<MpComplex> ExpectedValues() {
    const BigFloat p40 = BigFloat(1) / Pow2(40);
    const BigFloat p45 = BigFloat(1) / Pow2(45);
    const BigFloat p60 = BigFloat(1) / Pow2(60);
    const BigFloat p65 = BigFloat(1) / Pow2(65);
    const BigFloat p70 = BigFloat(1) / Pow2(70);
    const BigFloat p73 = BigFloat(1) / Pow2(73);
    const BigFloat p80 = BigFloat(1) / Pow2(80);
    return {
        {BigFloat("0.125"), BigFloat("-0.0625")},
        {BigFloat("0.125") + p70, BigFloat("-0.0625") + p73},
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
        {BigFloat("0.411111111111111111111111111111"), BigFloat("-0.377777777777777777777777777777")},
        {BigFloat(0), p80},
    };
}

std::vector<BigInt> EncodeCoefficients(const std::vector<MpComplex>& values) {
    Check(values.size() == kSlots, "wrong slot count");
    std::vector<MpComplex> inverse(values);
    FftSpecialInverse(inverse, kCyclotomicOrder);
    const BigFloat scale = Pow2(kScaleBits);
    const std::size_t gap = kRingDimension / (2U * kSlots);
    std::vector<BigInt> coefficients(kRingDimension, 0);
    for (std::size_t i = 0; i < kSlots; ++i) {
        coefficients[gap * i] = RoundHalfAwayFromZero(inverse[i].real * scale);
        coefficients[gap * (i + kSlots)] = RoundHalfAwayFromZero(inverse[i].imag * scale);
    }
    return coefficients;
}

MpComplex RootForExponent(std::uint32_t exponent) {
    const BigFloat pi = boost::math::constants::pi<BigFloat>();
    const BigFloat angle = BigFloat(2) * pi * BigFloat(exponent) / BigFloat(kCyclotomicOrder);
    using boost::multiprecision::cos;
    using boost::multiprecision::sin;
    return {cos(angle), sin(angle)};
}

MpComplex DirectEvaluate(const std::vector<BigInt>& coefficients, std::uint32_t exponent,
                         std::uint32_t scaleBits) {
    const MpComplex root = RootForExponent(exponent);
    MpComplex accumulator{BigFloat(0), BigFloat(0)};
    for (auto it = coefficients.rbegin(); it != coefficients.rend(); ++it) {
        accumulator = Add(Multiply(accumulator, root), {ToBigFloat(*it), BigFloat(0)});
    }
    const BigFloat scale = Pow2(scaleBits);
    accumulator.real /= scale;
    accumulator.imag /= scale;
    return accumulator;
}

std::vector<MpComplex> DirectCanonicalEvaluate(const std::vector<BigInt>& coefficients,
                                               std::uint32_t scaleBits) {
    std::vector<MpComplex> result;
    result.reserve(kSlots);
    for (std::uint32_t exponent : CanonicalExponents()) {
        result.push_back(DirectEvaluate(coefficients, exponent, scaleBits));
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

void CheckCanonicalWitnesses() {
    const BigInt scale = BigInt(1) << 90U;
    const BigFloat tolerance("1e-70");
    std::vector<BigInt> coefficients(kRingDimension, 0);
    coefficients[0] = scale;
    auto actual = DirectCanonicalEvaluate(coefficients, 90U);
    for (const auto& value : actual) {
        Check(Magnitude(Subtract(value, {BigFloat(1), BigFloat(0)})) <= tolerance,
              "constant witness failed");
    }
    std::fill(coefficients.begin(), coefficients.end(), BigInt(0));
    coefficients[32] = scale;
    actual = DirectCanonicalEvaluate(coefficients, 90U);
    for (const auto& value : actual) {
        Check(Magnitude(Subtract(value, {BigFloat(0), BigFloat(1)})) <= tolerance,
              "X^32 witness failed");
    }
    std::fill(coefficients.begin(), coefficients.end(), BigInt(0));
    coefficients[2] = scale;
    actual = DirectCanonicalEvaluate(coefficients, 90U);
    const auto expected = X2WitnessTable();
    for (std::size_t i = 0; i < kSlots; ++i) {
        Check(Magnitude(Subtract(actual[i], expected[i])) <= tolerance,
              "X^2 witness failed at slot " + std::to_string(i));
    }
}

BigFloat MaxError(const std::vector<MpComplex>& expected, const std::vector<MpComplex>& actual) {
    Check(expected.size() == actual.size(), "size mismatch");
    BigFloat maximum = 0;
    for (std::size_t i = 0; i < expected.size(); ++i) {
        const BigFloat error = Magnitude(Subtract(actual[i], expected[i]));
        if (error > maximum) {
            maximum = error;
        }
    }
    return maximum;
}

}  // namespace

int main() {
    try {
        const auto expected = ExpectedValues();
        const std::complex<double> base(expected[0].real.convert_to<double>(), expected[0].imag.convert_to<double>());
        const std::complex<double> shifted(expected[1].real.convert_to<double>(), expected[1].imag.convert_to<double>());
        Check(base == shifted, "binary64 negative control failed");
        CheckCanonicalWitnesses();
        const auto coefficients = EncodeCoefficients(expected);
        const auto actual = DirectCanonicalEvaluate(coefficients, kScaleBits);
        const BigFloat maximumError = MaxError(expected, actual);
        const MpComplex actualDelta = Subtract(actual[1], actual[0]);
        const MpComplex expectedDelta{BigFloat(1) / Pow2(70), BigFloat(1) / Pow2(73)};
        const BigFloat deltaError = Magnitude(Subtract(actualDelta, expectedDelta));
        const BigFloat tolerance = BigFloat(1) / Pow2(kAcceptanceBits);
        std::cout << "binary64_collapses_sub_ulp=1\n";
        std::cout << "canonical_witnesses_pass=1\n";
        std::cout << "max_slot_error=" << maximumError.str(80, std::ios_base::scientific) << "\n";
        std::cout << "delta_error=" << deltaError.str(80, std::ios_base::scientific) << "\n";
        std::cout << "below_2^-80=" << ((maximumError <= tolerance && deltaError <= tolerance) ? 1 : 0) << "\n";
        Check(maximumError <= tolerance, "max error exceeds 2^-80");
        Check(deltaError <= tolerance, "delta error exceeds 2^-80");
        return 0;
    }
    catch (const TestFailure& error) {
        std::cerr << "standalone precision contract failure: " << error.what() << '\n';
        return 1;
    }
}
