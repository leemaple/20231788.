#ifndef PAPER_ENDPOINT_INTEROP_TEST_H
#define PAPER_ENDPOINT_INTEROP_TEST_H

#include "paper_endpoint_evidence_writer.h"

#include <boost/version.hpp>

#include <array>
#include <filesystem>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#ifndef PAPER_SOURCE_COMMIT
#error "CMake must supply the actual configured paper source SHA"
#endif

namespace paper_endpoint_contract::synthetic::interop_test {
namespace detail {

inline Complex<768> FrozenInput(std::size_t slot) {
    const auto halfSlot = slot / 2;
    const Int a = (Int(1015) << 65) - (Int(halfSlot % 16) << 59) + slot;
    Int b = Int(1 + (halfSlot / 16) % 8) << 65;
    if ((halfSlot / 512) % 2 != 0)
        b = -b;
    Int real;
    Int imag;
    switch ((halfSlot / 128) % 4) {
        case 0: real = a; imag = b; break;
        case 1: real = -b; imag = a; break;
        case 2: real = -a; imag = -b; break;
        default: real = b; imag = -a; break;
    }
    const auto dyadic = [](const Int& value) {
        return boost::multiprecision::ldexp(
            Binary768(value.convert_to<std::string>()), -75);
    };
    return {dyadic(real), dyadic(imag)};
}

// Same backend and operation ordering as Capture's residual graph. This is a
// C++ consistency guard; the independent cross-language oracle is Python's
// Decimal256 replay of the emitted canonical rows.
inline Complex<768> SamePrecisionEighthSquare(Complex<768> value) {
    for (unsigned square = 0; square < 8; ++square) {
        const Binary768 rr = value.real * value.real;
        const Binary768 ii = value.imag * value.imag;
        const Binary768 ri = value.real * value.imag;
        const Binary768 ir = value.imag * value.real;
        value = {rr - ii, ri + ir};
    }
    return value;
}

inline bool Equal(const Complex<768>& left, const Complex<768>& right) {
    return left.real == right.real && left.imag == right.imag;
}

inline void CheckCapturedZeroEndpoints(const EndpointEvidence& evidence) {
    using paper_full_test::Require;
    const auto scales = paper_full_test::Scales();
    Require(evidence.freshScale.numerator == scales.front().numerator &&
                evidence.freshScale.denominator == scales.front().denominator &&
                evidence.terminalScale.numerator == scales.back().numerator &&
                evidence.terminalScale.denominator == scales.back().denominator,
            "interop capture retains literal paper S0/S8");
    Require(evidence.freshCoefficientOneNorm == 0 &&
                evidence.terminalCoefficientOneNorm == 0 &&
                evidence.freshMaximumOneNorm.numerator == 0 &&
                evidence.freshMaximumOneNorm.denominator == 1 &&
                evidence.terminalMaximumOneNorm.numerator == 0 &&
                evidence.terminalMaximumOneNorm.denominator == 1,
            "interop zero endpoint polynomials satisfy the conditioning model");
    Require(evidence.freshErrors.size() == paper_full_test::kSlots &&
                evidence.terminalErrors.size() == paper_full_test::kSlots,
            "interop capture retains every endpoint row");

    for (std::size_t slot = 0; slot < paper_full_test::kSlots; ++slot) {
        const auto z = FrozenInput(slot);
        const auto z256 = SamePrecisionEighthSquare(z);
        Require(Equal(evidence.freshErrors[slot], {-z.real, -z.imag}),
                "interop captured E0 is the negative frozen input");
        Require(Equal(evidence.terminalErrors[slot], {-z256.real, -z256.imag}),
                "interop captured E8 is the negative frozen 256th power");
    }

    const auto& e0 = evidence.maxima[0];
    const auto& e8 = evidence.maxima[1];
    const auto& i8 = evidence.maxima[2];
    const auto& a8 = evidence.maxima[3];
    Require(e0.id == "E0" && e0.slot == 16353 && e0.imaginary,
            "interop literal E0 maximum is slot 16353 imag");
    Require(e8.id == "E8" && e8.slot == 16289 && e8.imaginary &&
                i8.id == "I8" && i8.slot == 16289 && i8.imaginary &&
                e8.magnitude == i8.magnitude,
            "interop literal E8/I8 maximum is slot 16289 imag");
    Require(a8.id == "A8" && a8.magnitude == 0 && a8.slot == 0 &&
                !a8.imaginary,
            "interop A8 is identically zero with the least-slot tie policy");
    for (const auto& maximum : evidence.maxima) {
        Require(Equal(maximum.tuple[1], maximum.tuple[2]) &&
                    Equal(maximum.tuple[3], {Binary768(0), Binary768(0)}),
                "interop selected tuples retain E8=I8 and A8=0");
    }

    const Binary768 originalGate = boost::multiprecision::ldexp(Binary768(1), -80);
    Require(e0.magnitude > originalGate && e8.magnitude > originalGate,
            "interop E0 and E8 independently fail the unchanged 2^-80 gate");
}

} // namespace detail

// This is a test-only producer for the C++ -> Python interop seam. It performs
// no context construction, key generation, encryption, ciphertext operation, or
// call to the paper test's normal Run(). The returned path is the real writer's
// canonical TSV; `primary` receives the real endpoint-primary record block.
inline EndpointEvidenceFile ProduceCapturedEndpointEvidence(
    const std::filesystem::path& exclusiveTrustedParent,
    std::string host,
    std::string githubRunId,
    std::string githubRunAttempt,
    std::ostream& primary) {
    const IntegerPolynomial zeroPolynomial{
        std::vector<Int>(paper_full_test::kN, Int(0)), Int(17)};
    const auto scales = paper_full_test::Scales();
    const std::array<paper_full_test::Complex, 10> zeroHorner{};
    const std::vector<paper_full_test::io::ClientComplex> zeroProducer(
        paper_full_test::kSlots,
        {paper_full_test::io::ClientReal(0), paper_full_test::io::ClientReal(0)});

    const auto evidence = CaptureEndpointEvidence(
        zeroPolynomial, zeroPolynomial, scales.front(), scales.back(),
        zeroHorner, zeroHorner, zeroProducer, zeroProducer);
    detail::CheckCapturedZeroEndpoints(evidence);

    const EndpointEvidenceIdentity identity{
        "synthetic", PAPER_SOURCE_COMMIT, std::move(host),
        std::move(githubRunId), std::move(githubRunAttempt), BOOST_VERSION};
    // Exactly the two retained original endpoint errors, E0 and E8, exceed
    // 2^-80. I8 and A8 are diagnostic decomposition terms, not extra E80 gates.
    const EndpointPublicationBoundary boundary{2, true};
    const auto file = WriteEndpointEvidence(
        evidence, identity, boundary, exclusiveTrustedParent);
    const std::string stem = "fs-endpoint-synthetic-" + identity.sourceCommit + "." +
                             identity.host + "." + identity.githubRunId + "." +
                             identity.githubRunAttempt;
    paper_full_test::Require(
        file.readyPath == exclusiveTrustedParent / stem / (stem + ".tsv") &&
            file.canonicalBytes > 0,
        "interop writer retains the canonical synthetic identity stem");
    EmitEndpointEvidencePrimary(primary, evidence, identity, boundary);
    return file;
}

} // namespace paper_endpoint_contract::synthetic::interop_test

#endif
