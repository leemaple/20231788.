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
constexpr std::uint32_t kProductScaleBits = 200;
constexpr std::uint32_t kAcceptanceBits = 80;
constexpr std::uint32_t kCyclotomicOrder = 128;

struct MpComplex final {
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

BigFloat Pow2Float(std::uint32_t bits) {
    BigFloat value = 1;
    for (std::uint32_t bit = 0; bit < bits; ++bit) {
        value *= 2;
    }
    return value;
}

BigInt Pow2Integer(std::uint32_t bits) {
    return BigInt(1) << bits;
}

BigFloat ToBigFloat(const BigInt& value) {
    return BigFloat(value.convert_to<std::string>());
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

BigFloat Magnitude(const MpComplex& value) {
    using boost::multiprecision::sqrt;
    return sqrt(value.real * value.real + value.imag * value.imag);
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

std::vector<MpComplex> Roots(std::uint32_t order) {
    std::vector<MpComplex> roots(static_cast<std::size_t>(order) + 1U);
    const BigFloat pi = boost::math::constants::pi<BigFloat>();
    using boost::multiprecision::cos;
    using boost::multiprecision::sin;
    for (std::uint32_t index = 0; index < order; ++index) {
        const BigFloat angle = BigFloat(2) * pi * BigFloat(index) / BigFloat(order);
        roots[index] = {cos(angle), sin(angle)};
    }
    roots[order] = roots[0];
    return roots;
}

std::vector<std::uint32_t> RotationGroup(std::size_t size, std::uint32_t order) {
    std::vector<std::uint32_t> result(size);
    std::uint64_t power = 1;
    for (std::size_t index = 0; index < size; ++index) {
        result[index] = static_cast<std::uint32_t>(power);
        power = (power * 5U) % order;
    }
    return result;
}

void BitReverse(std::vector<MpComplex>& values) {
    const std::size_t size = values.size();
    for (std::size_t index = 1, reverse = 0; index < size; ++index) {
        std::size_t bit = size >> 1U;
        for (; reverse >= bit; bit >>= 1U) {
            reverse -= bit;
        }
        reverse += bit;
        if (index < reverse) {
            std::swap(values[index], values[reverse]);
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
            for (std::size_t index = 0; index < half; ++index) {
                const std::size_t rootIndex =
                    (quarter - (rotation[index] % quarter)) * gap;
                const MpComplex sum =
                    Add(values[offset + index], values[offset + index + half]);
                const MpComplex difference = Multiply(
                    Subtract(values[offset + index], values[offset + index + half]),
                    roots[rootIndex]);
                values[offset + index] = sum;
                values[offset + index + half] = difference;
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

std::array<std::uint32_t, kSlots> CanonicalExponents() {
    return {1U, 5U, 25U, 125U, 113U, 53U, 9U, 45U,
            97U, 101U, 121U, 93U, 81U, 21U, 105U, 13U};
}

MpComplex RootForExponent(std::uint32_t exponent) {
    const BigFloat pi = boost::math::constants::pi<BigFloat>();
    const BigFloat angle =
        BigFloat(2) * pi * BigFloat(exponent) / BigFloat(kCyclotomicOrder);
    using boost::multiprecision::cos;
    using boost::multiprecision::sin;
    return {cos(angle), sin(angle)};
}

MpComplex DirectEvaluate(const std::vector<BigInt>& coefficients,
                         std::uint32_t exponent,
                         std::uint32_t scaleBits) {
    const MpComplex root = RootForExponent(exponent);
    MpComplex accumulator{BigFloat(0), BigFloat(0)};
    for (auto coefficient = coefficients.rbegin(); coefficient != coefficients.rend();
         ++coefficient) {
        accumulator = Add(Multiply(accumulator, root),
                          {ToBigFloat(*coefficient), BigFloat(0)});
    }
    const BigFloat scale = Pow2Float(scaleBits);
    accumulator.real /= scale;
    accumulator.imag /= scale;
    return accumulator;
}

std::vector<MpComplex> DirectCanonicalEvaluate(
    const std::vector<BigInt>& coefficients,
    std::uint32_t scaleBits) {
    std::vector<MpComplex> result;
    result.reserve(kSlots);
    for (const std::uint32_t exponent : CanonicalExponents()) {
        result.push_back(DirectEvaluate(coefficients, exponent, scaleBits));
    }
    return result;
}

std::vector<BigInt> EncodeCoefficients(const std::vector<MpComplex>& values) {
    Check(values.size() == kSlots, "wrong slot count");
    std::vector<MpComplex> inverse(values);
    FftSpecialInverse(inverse, kCyclotomicOrder);
    const BigFloat scale = Pow2Float(kScaleBits);
    const std::size_t gap = kRingDimension / (2U * kSlots);
    std::vector<BigInt> coefficients(kRingDimension, 0);
    for (std::size_t slot = 0; slot < kSlots; ++slot) {
        coefficients[gap * slot] =
            RoundHalfAwayFromZero(inverse[slot].real * scale);
        coefficients[gap * (slot + kSlots)] =
            RoundHalfAwayFromZero(inverse[slot].imag * scale);
    }
    return coefficients;
}

std::vector<BigInt> NegacyclicProduct(const std::vector<BigInt>& left,
                                      const std::vector<BigInt>& right) {
    Check(left.size() == right.size() && !left.empty(), "product shape mismatch");
    std::vector<BigInt> result(left.size(), 0);
    for (std::size_t i = 0; i < left.size(); ++i) {
        for (std::size_t j = 0; j < right.size(); ++j) {
            const BigInt term = left[i] * right[j];
            const std::size_t index = i + j;
            if (index < result.size()) {
                result[index] += term;
            }
            else {
                result[index - result.size()] -= term;
            }
        }
    }
    return result;
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
    Check(left.size() == right.size(), "input lengths differ");
    std::vector<MpComplex> result;
    result.reserve(left.size());
    for (std::size_t slot = 0; slot < left.size(); ++slot) {
        result.push_back(Multiply(left[slot], right[slot]));
    }
    return result;
}

BigFloat MaximumError(const std::vector<MpComplex>& actual,
                      const std::vector<MpComplex>& expected) {
    Check(actual.size() == expected.size(), "comparison lengths differ");
    BigFloat maximum = 0;
    for (std::size_t slot = 0; slot < actual.size(); ++slot) {
        maximum = std::max(maximum, Magnitude(Subtract(actual[slot], expected[slot])));
    }
    return maximum;
}

MpComplex FrozenProductDelta() {
    const BigFloat p71 = BigFloat(1) / Pow2Float(71);
    const BigFloat p72 = BigFloat(1) / Pow2Float(72);
    const BigFloat p74 = BigFloat(1) / Pow2Float(74);
    const BigFloat p75 = BigFloat(1) / Pow2Float(75);
    return {p71 + p75, -p72 + p74};
}

void Run() {
    const auto left = LeftValues();
    const auto right = RightValues();
    const auto expected = ElementwiseProduct(left, right);
    const BigFloat tolerance = BigFloat(1) / Pow2Float(kAcceptanceBits);

    Check(left[0].real != left[1].real && left[0].imag != left[1].imag,
          "multiprecision source delta vanished");
    const std::complex<double> left0(left[0].real.convert_to<double>(),
                                     left[0].imag.convert_to<double>());
    const std::complex<double> left1(left[1].real.convert_to<double>(),
                                     left[1].imag.convert_to<double>());
    Check(left0 == left1, "binary64 negative control did not collapse");

    const auto leftCoefficients = EncodeCoefficients(left);
    const auto rightCoefficients = EncodeCoefficients(right);
    const auto representedLeft = DirectCanonicalEvaluate(leftCoefficients, kScaleBits);
    const auto representedRight = DirectCanonicalEvaluate(rightCoefficients, kScaleBits);
    const BigFloat leftError = MaximumError(representedLeft, left);
    const BigFloat rightError = MaximumError(representedRight, right);

    const auto coefficientProduct =
        NegacyclicProduct(leftCoefficients, rightCoefficients);
    const auto representedProduct =
        DirectCanonicalEvaluate(coefficientProduct, kProductScaleBits);
    const BigFloat productError = MaximumError(representedProduct, expected);
    const MpComplex representedDelta =
        Subtract(representedProduct[1], representedProduct[0]);
    const BigFloat deltaError =
        Magnitude(Subtract(representedDelta, FrozenProductDelta()));
    const BigFloat signalMagnitude = Magnitude(FrozenProductDelta());
    const BigFloat signalToToleranceRatio = signalMagnitude / tolerance;

    Check(Magnitude(Subtract(expected[0],
                             {BigFloat("0.046875"), BigFloat("-0.0625")})) <
              BigFloat("1e-75"),
          "slot-zero product witness failed");
    Check(Magnitude(Subtract(expected[2],
                             {BigFloat("-0.09375"), BigFloat("-0.109375")})) <
              BigFloat("1e-75"),
          "slot-two product witness failed");
    Check(Magnitude(Subtract(expected[3],
        {BigFloat("0.040132641420774808949887288570205500563303788511522569043477"),
         BigFloat("0.102547257465954082897938387693155713378359871980684644218665")})) <
              BigFloat("1e-75"),
          "slot-three product witness failed");
    Check(Magnitude(expected[7]) < BigFloat("1e-75"),
          "zero product witness failed");
    Check(Magnitude(Subtract(Subtract(expected[1], expected[0]),
                             FrozenProductDelta())) < BigFloat("1e-75"),
          "frozen product-delta witness failed");
    Check(productError < tolerance,
          "fixture-only represented product exceeds frozen 2^-80");
    Check(deltaError < tolerance,
          "fixture-only represented delta exceeds frozen 2^-80");
    Check(Pow2Integer(kProductScaleBits) ==
              Pow2Integer(kScaleBits) * Pow2Integer(kScaleBits),
          "exact scale numerator derivation failed");

    std::cout << std::setprecision(50)
              << "standalone_scope=non-cryptographic-fixture-arithmetic\n"
              << "binary64_collapses_sub_ulp=1\n"
              << "input_scale_numerator=2^" << kScaleBits << "\n"
              << "product_scale_numerator=2^" << kProductScaleBits << "\n"
              << "frozen_tolerance=2^-" << kAcceptanceBits << "\n"
              << "left_representation_max_error="
              << leftError.str(50, std::ios_base::scientific) << "\n"
              << "right_representation_max_error="
              << rightError.str(50, std::ios_base::scientific) << "\n"
              << "product_representation_max_error="
              << productError.str(50, std::ios_base::scientific) << "\n"
              << "product_delta_representation_error="
              << deltaError.str(50, std::ios_base::scientific) << "\n"
              << "product_delta_magnitude="
              << signalMagnitude.str(50, std::ios_base::scientific) << "\n"
              << "signal_to_tolerance_ratio="
              << signalToToleranceRatio.str(50, std::ios_base::scientific)
              << "\n";
}

}  // namespace

int main() {
    try {
        Run();
        std::cout << "standalone_first_mult2_contract_math=PASS\n";
        return 0;
    }
    catch (const TestFailure& error) {
        std::cerr << "standalone_first_mult2_contract_math=FAIL: "
                  << error.what() << '\n';
        return 1;
    }
}
