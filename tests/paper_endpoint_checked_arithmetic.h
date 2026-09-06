#ifndef PAPER_ENDPOINT_CHECKED_ARITHMETIC_H
#define PAPER_ENDPOINT_CHECKED_ARITHMETIC_H

#include "paper_endpoint_failure.h"
#include "paper_endpoint_observer_contract.h"

#include <boost/math/special_functions/fpclassify.hpp>

#include <stdexcept>
#include <string>

namespace paper_endpoint_contract::internal {

// This resource envelope contains every five-digit canonical decimal exponent
// and the frozen endpoint scales while keeping adversarial integer fixtures small.
inline constexpr int kCheckedBinaryExponentLimit = 400000;

namespace checked_arithmetic_detail {

inline std::string Detail(const char* operation, const char* condition) {
    return std::string(operation) + " " + condition;
}

template <unsigned Bits>
void RequireSupportedFinite(const Binary<Bits>& value, const char* operation) {
    if (!boost::math::isfinite(value))
        FailEndpoint("NONFINITE", Detail(operation, "is nonfinite"));
    if (value == 0)
        return;

    int exponent = 0;
    (void)boost::multiprecision::frexp(value, &exponent);
    if (exponent < -kCheckedBinaryExponentLimit ||
        exponent > kCheckedBinaryExponentLimit) {
        FailEndpoint("MODEL_UNSUPPORTED",
                     Detail(operation, "is outside the checked binary exponent envelope"));
    }
}

template <unsigned Bits>
Binary<Bits> CheckResult(const Binary<Bits>& result, const char* operation) {
    RequireSupportedFinite(result, operation);
    return result;
}

}  // namespace checked_arithmetic_detail

// Checked operations preserve the specified one-operation real graph. They
// distinguish nonfinite arithmetic from a finite-source exponent/underflow gap.
template <unsigned Bits>
Binary<Bits> CheckedMultiply(const Binary<Bits>& left, const Binary<Bits>& right,
                             const char* operation) {
    checked_arithmetic_detail::RequireSupportedFinite(left, operation);
    checked_arithmetic_detail::RequireSupportedFinite(right, operation);
    const Binary<Bits> result = left * right;
    if (left != 0 && right != 0 && result == 0) {
        FailEndpoint("MODEL_UNSUPPORTED", checked_arithmetic_detail::Detail(
            operation, "underflowed a nonzero product to zero"));
    }
    return checked_arithmetic_detail::CheckResult(result, operation);
}

template <unsigned Bits>
Binary<Bits> CheckedAdd(const Binary<Bits>& left, const Binary<Bits>& right,
                        const char* operation) {
    checked_arithmetic_detail::RequireSupportedFinite(left, operation);
    checked_arithmetic_detail::RequireSupportedFinite(right, operation);
    const Binary<Bits> result = left + right;
    if (result == 0 && left != -right) {
        FailEndpoint("MODEL_UNSUPPORTED", checked_arithmetic_detail::Detail(
            operation, "underflowed a nonzero sum to zero"));
    }
    return checked_arithmetic_detail::CheckResult(result, operation);
}

template <unsigned Bits>
Binary<Bits> CheckedSubtract(const Binary<Bits>& left, const Binary<Bits>& right,
                             const char* operation) {
    checked_arithmetic_detail::RequireSupportedFinite(left, operation);
    checked_arithmetic_detail::RequireSupportedFinite(right, operation);
    const Binary<Bits> result = left - right;
    if (result == 0 && left != right) {
        FailEndpoint("MODEL_UNSUPPORTED", checked_arithmetic_detail::Detail(
            operation, "underflowed a nonzero difference to zero"));
    }
    return checked_arithmetic_detail::CheckResult(result, operation);
}

template <unsigned Bits>
Binary<Bits> CheckedDivide(const Binary<Bits>& numerator, const Binary<Bits>& denominator,
                           const char* operation) {
    checked_arithmetic_detail::RequireSupportedFinite(numerator, operation);
    checked_arithmetic_detail::RequireSupportedFinite(denominator, operation);
    if (denominator == 0)
        throw std::invalid_argument(checked_arithmetic_detail::Detail(operation, "has zero divisor"));
    const Binary<Bits> result = numerator / denominator;
    if (numerator != 0 && result == 0) {
        FailEndpoint("MODEL_UNSUPPORTED", checked_arithmetic_detail::Detail(
            operation, "underflowed a nonzero quotient to zero"));
    }
    return checked_arithmetic_detail::CheckResult(result, operation);
}

}  // namespace paper_endpoint_contract::internal

#endif
