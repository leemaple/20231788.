#include "paper_endpoint_observer_contract.h"
#include "paper_endpoint_checked_arithmetic.h"

#include <boost/math/constants/constants.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace paper_endpoint_contract {
namespace {

[[noreturn]] void Reject(const char* message) {
    throw std::invalid_argument(std::string("endpoint transform: ") + message);
}

Int Magnitude(const Int& value) {
    return value < 0 ? Int(-value) : value;
}

std::size_t IntegerBitLength(const Int& value) {
    if (value == 0)
        return 0;
    return static_cast<std::size_t>(boost::multiprecision::msb(Magnitude(value))) + 1;
}

void ValidateScale(const Scale& scale) {
    if (scale.numerator <= 0 || scale.denominator <= 0)
        Reject("scale must be positive");
    if (paper_full_test::Gcd(scale.numerator, scale.denominator) != 1)
        Reject("scale must be reduced");
}

Int ValidatePolynomial(const IntegerPolynomial& polynomial) {
    if (polynomial.coefficients.size() != paper_full_test::kN)
        Reject("polynomial must have exactly N coefficients");
    if (polynomial.modulus <= 0 || (polynomial.modulus & 1) == 0)
        Reject("polynomial modulus must be positive and odd");

    Int oneNorm = 0;
    for (const auto& coefficient : polynomial.coefficients) {
        const Int magnitude = Magnitude(coefficient);
        if (2 * magnitude >= polynomial.modulus)
            Reject("polynomial coefficient is not centered");
        oneNorm += magnitude;
    }
    return oneNorm;
}

void ValidateTerms(const std::vector<SparseTerm>& terms) {
    std::size_t previous = 0;
    bool first = true;
    for (const auto& term : terms) {
        if (term.degree >= paper_full_test::kN)
            Reject("sparse degree is outside [0,N)");
        if (!first && term.degree <= previous)
            Reject("sparse degrees must be sorted and unique");
        previous = term.degree;
        first = false;
    }
}

template <unsigned Bits>
void RequireFinite(const Binary<Bits>& value, const char* message) {
    if (!boost::math::isfinite(value))
        FailEndpoint("NONFINITE", message);
}

// Convert an integer without a decimal or binary64 bridge. If more than Bits
// significant bits are present, perform exact nearest/ties-to-even rounding,
// then assemble the at-most-Bits-bit significand from exactly represented
// 64-bit chunks. The final ldexp is an exact power-of-two scale.
template <unsigned Bits>
Binary<Bits> RoundedInteger(const Int& input) {
    if (input == 0)
        return Binary<Bits>(0);

    const bool negative = input < 0;
    const Int magnitude = Magnitude(input);
    const std::size_t sourceBits = IntegerBitLength(magnitude);
    if (sourceBits >
        static_cast<std::size_t>(internal::kCheckedBinaryExponentLimit)) {
        FailEndpoint("MODEL_UNSUPPORTED",
                     "integer conversion exceeds the checked binary exponent envelope");
    }
    std::size_t shift = sourceBits > Bits ? sourceBits - Bits : 0;
    Int significand = magnitude;
    if (shift != 0) {
        significand = magnitude >> shift;
        const Int remainder = magnitude - (significand << shift);
        const Int halfway = Int(1) << (shift - 1);
        if (remainder > halfway || (remainder == halfway && (significand & 1) != 0))
            ++significand;
        if (static_cast<std::size_t>(boost::multiprecision::msb(significand)) + 1 > Bits) {
            significand >>= 1;
            ++shift;
        }
    }
    const std::size_t roundedBits =
        static_cast<std::size_t>(boost::multiprecision::msb(significand)) + 1;
    if (shift > static_cast<std::size_t>(internal::kCheckedBinaryExponentLimit) - roundedBits) {
        FailEndpoint("MODEL_UNSUPPORTED",
                     "rounded integer exceeds the checked binary exponent envelope");
    }

    Binary<Bits> result = 0;
    constexpr unsigned kChunkBits = 64;
    const Int mask = (Int(1) << kChunkBits) - 1;
    unsigned offset = 0;
    while (significand != 0) {
        const std::uint64_t chunk = (significand & mask).convert_to<std::uint64_t>();
        result += boost::multiprecision::ldexp(Binary<Bits>(chunk), static_cast<int>(offset));
        significand >>= kChunkBits;
        offset += kChunkBits;
    }
    result = boost::multiprecision::ldexp(result, static_cast<int>(shift));
    if (negative)
        result = -result;
    if (!boost::math::isfinite(result))
        FailEndpoint("MODEL_UNSUPPORTED", "integer conversion overflowed its exponent range");
    internal::checked_arithmetic_detail::RequireSupportedFinite(
        result, "integer conversion result");
    return result;
}

template <unsigned Bits>
Binary<Bits> ScaledCoefficient(const Int& coefficient, const Scale& scale,
                               const Binary<Bits>& roundedScaleNumerator) {
    const std::size_t coefficientBits = IntegerBitLength(coefficient);
    const std::size_t denominatorBits = IntegerBitLength(scale.denominator);
    const std::size_t exponentLimit =
        static_cast<std::size_t>(internal::kCheckedBinaryExponentLimit);
    if (coefficientBits != 0 &&
        (coefficientBits > exponentLimit || denominatorBits > exponentLimit ||
         coefficientBits > exponentLimit + 1 - denominatorBits)) {
        FailEndpoint("MODEL_UNSUPPORTED",
                     "scaled integer exceeds the checked binary exponent envelope");
    }
    const Int scaledInteger = coefficient * scale.denominator;
    const Binary<Bits> numerator = RoundedInteger<Bits>(scaledInteger);
    return internal::CheckedDivide(
        numerator, roundedScaleNumerator, "scaled-coefficient division");
}

template <unsigned Bits>
Complex<Bits> Multiply(const Complex<Bits>& left, const Complex<Bits>& right) {
    const Binary<Bits> realLeft = internal::CheckedMultiply(
        left.real, right.real, "complex real-left multiplication");
    const Binary<Bits> realRight = internal::CheckedMultiply(
        left.imag, right.imag, "complex real-right multiplication");
    const Binary<Bits> imagLeft = internal::CheckedMultiply(
        left.real, right.imag, "complex imag-left multiplication");
    const Binary<Bits> imagRight = internal::CheckedMultiply(
        left.imag, right.real, "complex imag-right multiplication");
    return {internal::CheckedSubtract(realLeft, realRight, "complex real subtraction"),
            internal::CheckedAdd(imagLeft, imagRight, "complex imaginary addition")};
}

template <unsigned Bits>
Complex<Bits> TwistRoot(const Binary<Bits>& pi, std::size_t index) {
    const Binary<Bits> angle = internal::CheckedDivide(
        internal::CheckedMultiply(pi, Binary<Bits>(index), "twist angle multiplication"),
        Binary<Bits>(paper_full_test::kN), "twist angle power-of-two scaling");
    using boost::multiprecision::cos;
    using boost::multiprecision::sin;
    Complex<Bits> result{cos(angle), sin(angle)};
    RequireFinite(result.real, "nonfinite twist root");
    RequireFinite(result.imag, "nonfinite twist root");
    return result;
}

template <unsigned Bits>
Complex<Bits> OrdinaryRoot(const Binary<Bits>& pi, std::size_t index) {
    const Binary<Bits> angle = internal::CheckedDivide(
        internal::CheckedMultiply(
            internal::CheckedMultiply(Binary<Bits>(2), pi,
                                      "ordinary angle power-of-two multiplication"),
            Binary<Bits>(index), "ordinary angle index multiplication"),
        Binary<Bits>(paper_full_test::kN), "ordinary angle power-of-two scaling");
    using boost::multiprecision::cos;
    using boost::multiprecision::sin;
    Complex<Bits> result{cos(angle), sin(angle)};
    RequireFinite(result.real, "nonfinite ordinary root");
    RequireFinite(result.imag, "nonfinite ordinary root");
    return result;
}

template <unsigned Bits>
struct DftTables final {
    std::vector<Complex<Bits>> twists;
    std::vector<Complex<Bits>> ordinary;
};

template <unsigned Bits>
const DftTables<Bits>& Tables() {
    static const DftTables<Bits> tables = [] {
        DftTables<Bits> result;
        const Binary<Bits> pi = boost::math::constants::pi<Binary<Bits>>();
        RequireFinite(pi, "nonfinite observer pi");
        result.twists.reserve(paper_full_test::kN);
        result.ordinary.reserve(paper_full_test::kN / 2);
        for (std::size_t index = 0; index < paper_full_test::kN; ++index)
            result.twists.push_back(TwistRoot<Bits>(pi, index));
        for (std::size_t index = 0; index < paper_full_test::kN / 2; ++index)
            result.ordinary.push_back(OrdinaryRoot<Bits>(pi, index));
        return result;
    }();
    return tables;
}

template <unsigned Bits>
void BitReverse(std::vector<Complex<Bits>>& values) {
    for (std::size_t index = 1, reverse = 0; index < values.size(); ++index) {
        std::size_t bit = values.size() >> 1;
        while ((reverse & bit) != 0) {
            reverse ^= bit;
            bit >>= 1;
        }
        reverse ^= bit;
        if (index < reverse)
            std::swap(values[index], values[reverse]);
    }
}

template <unsigned Bits>
std::vector<Complex<Bits>> Transform(const IntegerPolynomial& polynomial,
                                     const Scale& scale) {
    const Binary<Bits> scaleNumerator = RoundedInteger<Bits>(scale.numerator);
    if (scaleNumerator <= 0)
        FailEndpoint("MODEL_UNSUPPORTED", "scale numerator conversion is not positive");

    std::vector<Binary<Bits>> coefficients;
    coefficients.reserve(paper_full_test::kN);
    for (std::size_t index = 0; index < paper_full_test::kN; ++index) {
        coefficients.push_back(
            ScaledCoefficient<Bits>(polynomial.coefficients[index], scale, scaleNumerator));
    }

    const auto& tables = Tables<Bits>();
    std::vector<Complex<Bits>> values(paper_full_test::kN);
    for (std::size_t index = 0; index < paper_full_test::kN; ++index) {
        values[index] = {
            internal::CheckedMultiply(coefficients[index], tables.twists[index].real,
                                      "twisted-input real multiplication"),
            internal::CheckedMultiply(coefficients[index], tables.twists[index].imag,
                                      "twisted-input imaginary multiplication")};
    }

    BitReverse(values);
    for (std::size_t length = 2; length <= paper_full_test::kN; length <<= 1) {
        const std::size_t half = length / 2;
        const std::size_t rootStride = paper_full_test::kN / length;
        for (std::size_t begin = 0; begin < paper_full_test::kN; begin += length) {
            for (std::size_t offset = 0; offset < half; ++offset) {
                const Complex<Bits> lower = values[begin + offset];
                const Complex<Bits> upper =
                    Multiply(values[begin + offset + half], tables.ordinary[offset * rootStride]);
                values[begin + offset] = {
                    internal::CheckedAdd(lower.real, upper.real,
                                         "butterfly upper real addition"),
                    internal::CheckedAdd(lower.imag, upper.imag,
                                         "butterfly upper imaginary addition")};
                values[begin + offset + half] = {
                    internal::CheckedSubtract(lower.real, upper.real,
                                              "butterfly lower real subtraction"),
                    internal::CheckedSubtract(lower.imag, upper.imag,
                                              "butterfly lower imaginary subtraction")};
            }
        }
    }

    std::vector<Complex<Bits>> slots;
    slots.reserve(paper_full_test::kSlots);
    std::uint32_t exponent = 1;
    for (std::size_t slot = 0; slot < paper_full_test::kSlots; ++slot) {
        const std::size_t bin = (exponent - 1U) / 2U;
        RequireFinite(values[bin].real, "nonfinite final observer real component");
        RequireFinite(values[bin].imag, "nonfinite final observer imaginary component");
        slots.push_back(values[bin]);
        exponent = static_cast<std::uint32_t>(
            (static_cast<std::uint64_t>(exponent) * 5U) % paper_full_test::kM);
    }
    return slots;
}

const Binary768& DirectPi768() {
    static const Binary768 pi = [] {
        Binary768 result = boost::math::constants::pi<Binary768>();
        RequireFinite(result, "nonfinite direct-reference pi");
        return result;
    }();
    return pi;
}

Complex<768> DirectRoot768(std::uint32_t exponent) {
    const Binary768& pi = DirectPi768();
    const Binary768 angle = internal::CheckedDivide(
        internal::CheckedMultiply(
            internal::CheckedMultiply(Binary768(2), pi,
                                      "direct angle power-of-two multiplication"),
            Binary768(exponent), "direct angle exponent multiplication"),
        Binary768(paper_full_test::kM), "direct angle power-of-two scaling");
    using boost::multiprecision::cos;
    using boost::multiprecision::sin;
    Complex<768> result{cos(angle), sin(angle)};
    RequireFinite(result.real, "nonfinite direct root");
    RequireFinite(result.imag, "nonfinite direct root");
    return result;
}

const std::vector<Complex<768>>& DirectOddRoots768() {
    static const std::vector<Complex<768>> roots = [] {
        std::vector<Complex<768>> result;
        result.reserve(paper_full_test::kM / 2);
        for (std::uint32_t index = 0; index < paper_full_test::kM / 2; ++index)
            result.push_back(DirectRoot768(2U * index + 1U));
        return result;
    }();
    return roots;
}

const Complex<768>& DirectOddRoot768(std::uint32_t exponent) {
    return DirectOddRoots768().at((exponent - 1U) / 2U);
}

}  // namespace

Observation Observe(const IntegerPolynomial& polynomial, const Scale& scale) {
    ValidateScale(scale);
    const Int oneNorm = ValidatePolynomial(polynomial);
    Observation result;
    result.coefficientOneNorm = oneNorm;
    result.scaledOneNormExponent = ScaledOneNormExponent(oneNorm, scale);
    result.at512 = Transform<512>(polynomial, scale);
    result.at768 = Transform<768>(polynomial, scale);
    return result;
}

std::vector<Complex<768>> DirectSparseReference768(
    const std::vector<SparseTerm>& terms, const Scale& scale) {
    ValidateScale(scale);
    ValidateTerms(terms);
    const Binary768 scaleNumerator = RoundedInteger<768>(scale.numerator);
    if (scaleNumerator <= 0)
        FailEndpoint("MODEL_UNSUPPORTED", "scale numerator conversion is not positive");

    struct PreparedTerm final {
        std::size_t degree;
        Binary768 coefficient;
    };
    std::vector<PreparedTerm> prepared;
    prepared.reserve(terms.size());
    for (const auto& term : terms) {
        prepared.push_back(
            {term.degree, ScaledCoefficient<768>(term.coefficient, scale, scaleNumerator)});
    }

    std::vector<Complex<768>> result;
    result.reserve(paper_full_test::kSlots);
    std::uint32_t slotExponent = 1;
    for (std::size_t slot = 0; slot < paper_full_test::kSlots; ++slot) {
        Complex<768> sum{Binary768(0), Binary768(0)};
        for (const auto& term : prepared) {
            const std::uint32_t exponent = static_cast<std::uint32_t>(
                (static_cast<std::uint64_t>(slotExponent) * term.degree) %
                paper_full_test::kM);
            if (exponent == 0) {
                sum.real = internal::CheckedAdd(
                    sum.real, term.coefficient, "direct-reference real addition");
            }
            else {
                const Complex<768> direct = (exponent & 1U) != 0
                    ? DirectOddRoot768(exponent)
                    : DirectRoot768(exponent);
                sum.real = internal::CheckedAdd(
                    sum.real,
                    internal::CheckedMultiply(
                        term.coefficient, direct.real,
                        "direct-reference real multiplication"),
                    "direct-reference real addition");
                sum.imag = internal::CheckedAdd(
                    sum.imag,
                    internal::CheckedMultiply(
                        term.coefficient, direct.imag,
                        "direct-reference imaginary multiplication"),
                    "direct-reference imaginary addition");
            }
        }
        RequireFinite(sum.real, "nonfinite direct sparse result");
        RequireFinite(sum.imag, "nonfinite direct sparse result");
        result.push_back(std::move(sum));
        slotExponent = static_cast<std::uint32_t>(
            (static_cast<std::uint64_t>(slotExponent) * 5U) % paper_full_test::kM);
    }
    return result;
}

}  // namespace paper_endpoint_contract
