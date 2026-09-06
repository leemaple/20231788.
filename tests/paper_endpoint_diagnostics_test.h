#ifndef PAPER_ENDPOINT_DIAGNOSTICS_TEST_H
#define PAPER_ENDPOINT_DIAGNOSTICS_TEST_H

#include "paper_endpoint_diagnostics.h"
#include <limits>
#include <utility>

namespace paper_endpoint_contract::synthetic {
inline void RunEndpointDiagnosticsBoundaryTests() {
    const Scale unit{Int(1), Int(1)};
    const IntegerPolynomial missing{{}, Int(17)};
    const std::array<paper_full_test::Complex, 10> anchors{};
    const std::vector<paper_full_test::io::ClientComplex> producer;
    bool rejected = false;
    try {
        (void)CaptureEndpointEvidence(missing, missing, unit, unit,
                                      anchors, anchors, producer, producer);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    paper_full_test::Require(rejected, "diagnostic capture rejects missing endpoint shape");

    // These invalid inputs must fail at capture, before any root tables/DFTs.
    const IntegerPolynomial valid{std::vector<Int>(paper_full_test::kN, Int(0)), Int(17)};
    const std::vector<paper_full_test::io::ClientComplex> zeros(
        paper_full_test::kSlots, {paper_full_test::io::ClientReal(0),
                                 paper_full_test::io::ClientReal(0)});
    for (const auto& invalid : {Scale{Int(0), Int(1)}, Scale{Int(1), Int(0)},
                                Scale{Int(2), Int(2)}}) {
        rejected = false;
        try {
            (void)CaptureEndpointEvidence(valid, valid, invalid, unit,
                                          anchors, anchors, zeros, zeros);
        } catch (const std::invalid_argument&) {
            rejected = true;
        }
        paper_full_test::Require(rejected, "capture rejects invalid exact scale");
    }
    auto nonfinite = zeros;
    nonfinite[0].real = std::numeric_limits<paper_full_test::io::ClientReal>::infinity();
    rejected = false;
    try {
        (void)CaptureEndpointEvidence(valid, valid, unit, unit,
                                      anchors, anchors, nonfinite, zeros);
    } catch (const std::runtime_error& error) {
        rejected = std::string(error.what()).find("endpoint NONFINITE:") == 0;
    }
    paper_full_test::Require(rejected, "capture rejects nonfinite producer source as NONFINITE");

    // The source significand has all 512 bits set. Widening must preserve the
    // already represented dyadic value, its sign, and supported exponents.
    const Int fullSignificand = (Int(1) << 512) - 1;
    const auto exact768 = [&](int exponent, bool negative) {
        Binary768 value(fullSignificand.convert_to<std::string>());
        value = boost::multiprecision::ldexp(value, exponent - 512);
        return negative ? Binary768(-value) : value;
    };
    const paper_full_test::Real full512(fullSignificand.convert_to<std::string>());
    for (const auto& fixture : {
             std::pair<int, bool>{400000, false},
             std::pair<int, bool>{-400000, true}}) {
        const paper_full_test::Real source = boost::multiprecision::ldexp(
            full512, fixture.first - 512);
        const Binary768 widened = internal::WidenRepresented512(
            fixture.second ? paper_full_test::Real(-source) : source);
        const auto difference = ExactAbsoluteDifference(
            widened, exact768(fixture.first, fixture.second));
        paper_full_test::Require(
            difference.numerator == 0,
            "fixed binary512 widening preserves every represented bit");
    }
    paper_full_test::Require(
        internal::WidenRepresented512(paper_full_test::Real(0)) == 0,
        "fixed binary512 widening preserves zero");

    const auto requireWidenFailure = [](const paper_full_test::Real& value,
                                        const std::string& reason,
                                        const std::string& label) {
        bool failed = false;
        try {
            (void)internal::WidenRepresented512(value);
        } catch (const EndpointFailure& failure) {
            failed = failure.Reason() == reason;
        }
        paper_full_test::Require(failed, label);
    };
    requireWidenFailure(
        std::numeric_limits<paper_full_test::Real>::infinity(), "NONFINITE",
        "fixed binary512 widening rejects infinity");
    requireWidenFailure(
        std::numeric_limits<paper_full_test::Real>::quiet_NaN(), "NONFINITE",
        "fixed binary512 widening rejects NaN");
    requireWidenFailure(
        boost::multiprecision::ldexp(paper_full_test::Real(1), 400000),
        "MODEL_UNSUPPORTED", "fixed binary512 widening rejects high exponent");
    requireWidenFailure(
        boost::multiprecision::ldexp(paper_full_test::Real(1), -400002),
        "MODEL_UNSUPPORTED", "fixed binary512 widening rejects low exponent");

    const IntegerPolynomial fullWidthHorner{{fullSignificand},
                                            (Int(1) << 513) + 1};
    paper_full_test::Require(
        internal::HornerConversionsSupported(fullWidthHorner, unit),
        "Horner applicability widens the original R(Int) result exactly");

    auto hornerOutsideRange = valid;
    hornerOutsideRange.coefficients[0] = Int(1) << 400000;
    hornerOutsideRange.modulus = (Int(1) << 400001) + 1;
    paper_full_test::Require(
        !internal::HornerConversionsSupported(hornerOutsideRange, unit),
        "Horner applicability rejects conversion range before R(Int)");

    const paper_full_test::io::ClientReal unavailable("1e+120000");
    paper_full_test::Require(boost::math::isfinite(unavailable),
                            "producer range fixture is a finite source value");
    const auto transported = internal::TransportProducerForDiagnostics(
        {{paper_full_test::io::ClientReal(1), unavailable}});
    paper_full_test::Require(
        !transported.supported && transported.values.size() == 1 &&
            transported.available.size() == 1 && transported.available[0][0] &&
            !transported.available[0][1] && transported.values[0].real == 1,
        "producer transport preserves an available component beside unavailable conversion");
    const auto rawDistance = ExactAbsoluteDifference(
        Binary768(0), transported.values[0].real);
    paper_full_test::Require(
        AssessDifference(rawDistance, Rational{Int(0), Int(1)},
                         Comparison::Producer, transported.supported) == Decision::Fail,
        "raw finite producer excess wins over another unavailable component");
}
} // namespace paper_endpoint_contract::synthetic
#endif
