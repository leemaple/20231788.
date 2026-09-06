#ifndef PAPER_ENDPOINT_EXACT_SCALARS_TEST_H
#define PAPER_ENDPOINT_EXACT_SCALARS_TEST_H

#include "paper_endpoint_observer_contract.h"
#include "paper_endpoint_failure.h"

#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include <limits>
#include <stdexcept>
#include <string>

namespace paper_endpoint_contract::synthetic {
namespace exact_scalar_test {

inline Int PowerOfTen(unsigned exponent) {
    Int result = 1;
    Int factor = 10;
    while (exponent != 0) {
        if ((exponent & 1U) != 0) result *= factor;
        exponent >>= 1U;
        if (exponent != 0) factor *= factor;
    }
    return result;
}

template <typename Callable>
inline void RequireInvalidArgument(Callable&& callable, const std::string& label) {
    try {
        callable();
    }
    catch (const std::invalid_argument&) {
        return;
    }
    paper_full_test::Require(false, label);
}

inline void CheckCanonicalLowerExponentCarry() {
    const Int binaryDenominator = Int(1) << 332955;
    const Int decimalDenominator = PowerOfTen(99999);
    const Int numerator = binaryDenominator / decimalDenominator;
    const Int distanceNumerator = binaryDenominator - numerator * decimalDenominator;
    paper_full_test::Require(boost::multiprecision::msb(numerator) + 1 <= 768,
                            "canonical lower-bound witness precision");
    paper_full_test::Require(distanceNumerator > 0,
                            "canonical lower-bound witness is below minimum");
    paper_full_test::Require(2 * distanceNumerator * PowerOfTen(110) < binaryDenominator,
                            "canonical lower-bound witness rounds across decade");

    const Binary768 magnitude = boost::multiprecision::ldexp(
        Binary768(numerator.convert_to<std::string>()), -332955);
    const std::string digits(109, '0');
    paper_full_test::Require(CanonicalDecimal(magnitude) ==
                                "+1." + digits + "e-99999",
                            "canonical lower-bound positive carry");
    paper_full_test::Require(CanonicalDecimal(-magnitude) ==
                                "-1." + digits + "e-99999",
                            "canonical lower-bound negative carry");

    RequireInvalidArgument(
        [] { (void)CanonicalDecimal(boost::multiprecision::ldexp(Binary768(1), -332192)); },
        "canonical non-carrying lower exponent accepted");
}

inline void CheckSignedDyadicAndMalformedInputs() {
    const Rational signedDistance = ExactAbsoluteDifference(
        boost::multiprecision::ldexp(Binary768(-3), -5),
        boost::multiprecision::ldexp(Binary768(1), -4));
    paper_full_test::Require(signedDistance.numerator == 5 && signedDistance.denominator == 32,
                            "exact distance preserves opposite signs");
    const Rational zero = ExactAbsoluteDifference(Binary768(-3), Binary768(-3));
    paper_full_test::Require(zero.numerator == 0 && zero.denominator == 1,
                            "canonical represented dyadic zero");

    const Rational validZero{Int(0), Int(1)};
    const Rational malformed[]{{Int(-1), Int(1)}, {Int(1), Int(0)},
                               {Int(1), Int(-1)}, {Int(2), Int(2)}};
    for (const auto& value : malformed) {
        paper_full_test::Require(
            AssessDifference(value, validZero, Comparison::Producer, true) == Decision::Fail &&
                AssessDifference(validZero, value, Comparison::Producer, true) == Decision::Fail,
            "malformed rational classification");
    }
    paper_full_test::Require(
        AssessDifference(validZero, validZero, static_cast<Comparison>(99), true) ==
            Decision::Fail,
        "invalid comparison classification");

    const Binary768 infinity = std::numeric_limits<Binary768>::infinity();
    const Binary768 nan = std::numeric_limits<Binary768>::quiet_NaN();
    const Binary768 nonfinite[] = {infinity, Binary768(-infinity), nan};
    for (const auto& value : nonfinite) {
        RequireInvalidArgument([&] { (void)CanonicalDecimal(value); },
                               "nonfinite canonical decimal accepted");
        RequireInvalidArgument([&] { (void)ExactAbsoluteDifference(value, Binary768(0)); },
                               "nonfinite exact difference accepted");
    }
    for (const auto exponent : {-400002, 400002}) {
        bool unsupported = false;
        try {
            (void)ExactAbsoluteDifference(
                boost::multiprecision::ldexp(Binary768(1), exponent), Binary768(0));
        } catch (const EndpointFailure& error) {
            unsupported = error.Reason() == "MODEL_UNSUPPORTED";
        }
        paper_full_test::Require(unsupported,
                                "exact comparison rejects out-of-range allocation as unsupported");
    }
}

inline void RunExactScalarBoundaryTests() {
    CheckCanonicalLowerExponentCarry();
    CheckSignedDyadicAndMalformedInputs();
}

} // namespace exact_scalar_test
} // namespace paper_endpoint_contract::synthetic

#endif
