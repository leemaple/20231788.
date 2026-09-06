#include "paper_endpoint_exact_scalars_internal.h"
#include "paper_endpoint_failure.h"

#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

#include <stdexcept>
#include <string>
#include <utility>

namespace paper_endpoint_contract {
namespace {

constexpr int kSignificantDigits = 110;
constexpr int kFractionalDigits = kSignificantDigits - 1;
constexpr int kCanonicalBytes = 119;
constexpr int kMaximumDecimalExponent = 99999;
constexpr int kExtractionBits = 768;
constexpr int kEarlyBinaryExponentLimit = 400000;

[[noreturn]] void InvalidArgument(const char* message) {
    throw std::invalid_argument(message);
}

void RequireArgument(bool condition, const char* message) {
    if (!condition) InvalidArgument(message);
}

Int Absolute(Int value) {
    return value < 0 ? -value : value;
}

Int GreatestCommonDivisor(Int left, Int right) {
    left = Absolute(left);
    right = Absolute(right);
    while (right != 0) {
        const Int remainder = left % right;
        left = right;
        right = remainder;
    }
    return left;
}

Rational Reduced(Int numerator, Int denominator) {
    RequireArgument(denominator > 0, "rational denominator must be positive");
    if (numerator == 0) return {Int(0), Int(1)};
    const Int divisor = GreatestCommonDivisor(numerator, denominator);
    return {numerator / divisor, denominator / divisor};
}

bool IsReducedNonnegative(const Rational& value) {
    return value.numerator >= 0 && value.denominator > 0 &&
           GreatestCommonDivisor(value.numerator, value.denominator) == 1;
}

Int PowerOfTen(unsigned exponent) {
    Int result = 1;
    Int factor = 10;
    while (exponent != 0) {
        if ((exponent & 1U) != 0) result *= factor;
        exponent >>= 1U;
        if (exponent != 0) factor *= factor;
    }
    return result;
}

int Compare(const Int& left, const Int& right) {
    if (left < right) return -1;
    if (left > right) return 1;
    return 0;
}

int CompareMagnitudeToPowerOfTen(const Int& numerator, const Int& denominator,
                                 int exponent) {
    RequireArgument(numerator > 0 && denominator > 0,
                    "positive magnitude required");
    if (exponent >= 0)
        return Compare(numerator, denominator * PowerOfTen(static_cast<unsigned>(exponent)));
    return Compare(numerator * PowerOfTen(static_cast<unsigned>(-exponent)), denominator);
}

long long FloorDivide(long long numerator, long long denominator) {
    const long long quotient = numerator / denominator;
    const long long remainder = numerator % denominator;
    return quotient - (remainder != 0 && numerator < 0 ? 1 : 0);
}

bool ExceedsDyadicThreshold(const Rational& value, unsigned denominatorExponent) {
    return (value.numerator << denominatorExponent) > value.denominator;
}

const std::string& CanonicalZero() {
    static const std::string zero = "+0." + std::string(kFractionalDigits, '0') +
                                    "e+00000";
    return zero;
}

} // namespace

namespace internal {

Rational RepresentedDyadic(const Binary768& value) {
    RequireArgument(boost::math::isfinite(value), "represented dyadic must be finite");
    if (value == 0) return {Int(0), Int(1)};

    const bool negative = value < 0;
    const Binary768 magnitude = negative ? Binary768(-value) : value;
    int binaryExponent = 0;
    const Binary768 fraction = boost::multiprecision::frexp(magnitude, &binaryExponent);
    // The observer/evidence model has an explicit checked exponent envelope.
    // Refuse unsupported values before subtracting the precision or allocating
    // an enormous exact rational. All legal canonical decimals lie inside it.
    if (binaryExponent < -kEarlyBinaryExponentLimit ||
        binaryExponent > kEarlyBinaryExponentLimit)
        FailEndpoint("MODEL_UNSUPPORTED", "represented dyadic exponent outside checked range");
    const Binary768 scaled = boost::multiprecision::ldexp(fraction, kExtractionBits);
    Int significand = scaled.convert_to<Int>();

    // The conversion is allowed only after scaling the complete represented
    // significand to an integer, and is checked by an exact binary round trip.
    const Binary768 roundTrip(significand.convert_to<std::string>());
    if (roundTrip != scaled)
        throw std::runtime_error("represented dyadic integer conversion was not exact");

    int binaryShift = binaryExponent - kExtractionBits;
    while ((significand & 1) == 0) {
        significand >>= 1;
        ++binaryShift;
    }

    Int numerator = significand;
    Int denominator = 1;
    if (binaryShift >= 0)
        numerator <<= static_cast<unsigned>(binaryShift);
    else
        denominator <<= static_cast<unsigned>(-binaryShift);
    if (negative) numerator = -numerator;
    return {std::move(numerator), std::move(denominator)};
}

} // namespace internal

Rational ExactAbsoluteDifference(const Binary768& left, const Binary768& right) {
    const Rational exactLeft = internal::RepresentedDyadic(left);
    const Rational exactRight = internal::RepresentedDyadic(right);
    Int numerator = exactLeft.numerator * exactRight.denominator -
                    exactRight.numerator * exactLeft.denominator;
    if (numerator < 0) numerator = -numerator;
    return Reduced(std::move(numerator),
                   exactLeft.denominator * exactRight.denominator);
}

Decision AssessDifference(const Rational& distance, const Rational& allowance,
                          Comparison comparison, bool modelSupported) {
    if (!IsReducedNonnegative(distance) || !IsReducedNonnegative(allowance) ||
        (comparison != Comparison::TwoBoundedPaths && comparison != Comparison::Producer))
        return Decision::Fail;

    if (ExceedsDyadicThreshold(distance, 120)) return Decision::Fail;
    if (!modelSupported || ExceedsDyadicThreshold(allowance, 128))
        return Decision::Unresolved;
    if (comparison == Comparison::TwoBoundedPaths &&
        distance.numerator * allowance.denominator >
            allowance.numerator * distance.denominator)
        return Decision::Fail;

    const Int combinedNumerator =
        distance.numerator * allowance.denominator +
        allowance.numerator * distance.denominator;
    const Int combinedDenominator = distance.denominator * allowance.denominator;
    return (combinedNumerator << 120) <= combinedDenominator ?
               Decision::Pass : Decision::Unresolved;
}

std::string CanonicalDecimal(const Binary768& value) {
    RequireArgument(boost::math::isfinite(value), "canonical decimal input must be finite");
    if (value == 0) return CanonicalZero();

    const Binary768 magnitude = value < 0 ? Binary768(-value) : value;
    int binaryExponent = 0;
    (void)boost::multiprecision::frexp(magnitude, &binaryExponent);
    // Any value capable of a legal five-digit decimal exponent is safely inside
    // this wider exact-binary guard (2^4 > 10). Reject before a giant dyadic shift.
    RequireArgument(binaryExponent <= kEarlyBinaryExponentLimit &&
                        binaryExponent >= -kEarlyBinaryExponentLimit,
                    "canonical decimal exponent overflow");

    Rational exact = internal::RepresentedDyadic(value);
    const bool negative = exact.numerator < 0;
    Int numerator = Absolute(std::move(exact.numerator));
    const Int& denominator = exact.denominator;

    const long long binaryFloor = static_cast<long long>(binaryExponent) - 1;
    int exponent = static_cast<int>(FloorDivide(binaryFloor * 1233, 4096));

    // The estimate only chooses a nearby starting decade. Exact integer
    // comparisons decide the exponent and reject distant overflow before rounding.
    if (exponent > kMaximumDecimalExponent) {
        RequireArgument(CompareMagnitudeToPowerOfTen(
                            numerator, denominator, kMaximumDecimalExponent + 1) < 0,
                        "canonical decimal exponent overflow");
        exponent = kMaximumDecimalExponent;
    }
    else if (exponent < -kMaximumDecimalExponent - 1) {
        RequireArgument(CompareMagnitudeToPowerOfTen(
                            numerator, denominator, -kMaximumDecimalExponent - 1) >= 0,
                        "canonical decimal exponent overflow");
        exponent = -kMaximumDecimalExponent - 1;
    }
    while (CompareMagnitudeToPowerOfTen(numerator, denominator, exponent) < 0)
        --exponent;
    while (CompareMagnitudeToPowerOfTen(numerator, denominator, exponent + 1) >= 0)
        ++exponent;

    RequireArgument(exponent >= -kMaximumDecimalExponent - 1 &&
                        exponent <= kMaximumDecimalExponent,
                    "canonical decimal exponent overflow");
    const int decimalShift = kFractionalDigits - exponent;
    if (decimalShift >= 0)
        numerator *= PowerOfTen(static_cast<unsigned>(decimalShift));
    else
        exact.denominator *= PowerOfTen(static_cast<unsigned>(-decimalShift));

    Int significand = numerator / exact.denominator;
    const Int remainder = numerator % exact.denominator;
    const Int doubledRemainder = 2 * remainder;
    if (doubledRemainder > exact.denominator ||
        (doubledRemainder == exact.denominator && (significand & 1) != 0))
        ++significand;

    const Int carry = PowerOfTen(kSignificantDigits);
    if (significand == carry) {
        significand /= 10;
        ++exponent;
    }
    RequireArgument(exponent >= -kMaximumDecimalExponent &&
                        exponent <= kMaximumDecimalExponent,
                    "canonical decimal exponent overflow");

    const std::string digits = significand.convert_to<std::string>();
    if (digits.size() != static_cast<std::size_t>(kSignificantDigits) || digits[0] == '0')
        throw std::runtime_error("canonical decimal significand invariant");

    const std::string exponentDigits = std::to_string(exponent < 0 ? -exponent : exponent);
    std::string result;
    result.reserve(kCanonicalBytes);
    result.push_back(negative ? '-' : '+');
    result.push_back(digits[0]);
    result.push_back('.');
    result.append(digits, 1, std::string::npos);
    result.push_back('e');
    result.push_back(exponent < 0 ? '-' : '+');
    result.append(5 - exponentDigits.size(), '0');
    result += exponentDigits;
    if (!IsCanonicalDecimal(result))
        throw std::runtime_error("canonical decimal formatting invariant");
    return result;
}

bool IsCanonicalDecimal(std::string_view text) {
    if (text.size() != static_cast<std::size_t>(kCanonicalBytes)) return false;
    if (text == std::string_view(CanonicalZero())) return true;
    if ((text[0] != '+' && text[0] != '-') || text[1] < '1' || text[1] > '9' ||
        text[2] != '.' || text[112] != 'e' ||
        (text[113] != '+' && text[113] != '-'))
        return false;
    for (std::size_t index = 3; index < 112; ++index)
        if (text[index] < '0' || text[index] > '9') return false;
    bool zeroExponent = true;
    for (std::size_t index = 114; index < text.size(); ++index) {
        if (text[index] < '0' || text[index] > '9') return false;
        zeroExponent = zeroExponent && text[index] == '0';
    }
    return !(text[113] == '-' && zeroExponent);
}

} // namespace paper_endpoint_contract
