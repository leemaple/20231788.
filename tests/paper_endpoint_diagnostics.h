#ifndef PAPER_ENDPOINT_DIAGNOSTICS_H
#define PAPER_ENDPOINT_DIAGNOSTICS_H

#include "paper_endpoint_observer_contract.h"
#include "paper_endpoint_failure.h"

#include <array>

namespace paper_endpoint_contract {
struct ComparisonReceipt final {
    std::string id;
    Rational distance;
    Rational allowance;
    std::size_t slot;
    bool imaginary;
};

struct ResidualMaximum final {
    std::string id;
    Binary768 magnitude;
    Rational errorAllowance;
    std::size_t slot;
    bool imaginary;
    std::array<Complex<768>, 4> tuple; // E0, E8, I8, A8 at the measured maximizer.
};

// Pure values only: no OpenFHE owner, key, context, ciphertext or plan escapes.
struct EndpointEvidence final {
    Scale freshScale;
    Scale terminalScale;
    Int freshCoefficientOneNorm;
    Int terminalCoefficientOneNorm;
    Rational freshMaximumOneNorm;
    Rational terminalMaximumOneNorm;
    std::vector<ComparisonReceipt> checks;
    std::vector<Complex<768>> freshErrors;
    std::vector<Complex<768>> terminalErrors;
    std::array<ResidualMaximum, 4> maxima;
};

namespace internal {
struct ProducerTransport final {
    std::vector<Complex<768>> values;
    std::vector<std::array<bool, 2>> available;
    bool supported;
};

bool HornerConversionsSupported(const IntegerPolynomial& polynomial, const Scale& scale);
ProducerTransport TransportProducerForDiagnostics(
    const std::vector<paper_full_test::io::ClientComplex>& values);
EndpointEvidence CaptureEndpointEvidenceWithHornerSupport(
    const IntegerPolynomial& fresh, const IntegerPolynomial& terminal,
    const Scale& freshScale, const Scale& terminalScale,
    const std::array<paper_full_test::Complex, 10>& freshHorner,
    const std::array<paper_full_test::Complex, 10>& terminalHorner,
    const std::vector<paper_full_test::io::ClientComplex>& freshProducer,
    const std::vector<paper_full_test::io::ClientComplex>& terminalProducer,
    bool freshHornerSupported, bool terminalHornerSupported);
} // namespace internal

EndpointEvidence CaptureEndpointEvidence(
    const IntegerPolynomial& fresh, const IntegerPolynomial& terminal,
    const Scale& freshScale, const Scale& terminalScale,
    const std::array<paper_full_test::Complex, 10>& freshHorner,
    const std::array<paper_full_test::Complex, 10>& terminalHorner,
    const std::vector<paper_full_test::io::ClientComplex>& freshProducer,
    const std::vector<paper_full_test::io::ClientComplex>& terminalProducer);
} // namespace paper_endpoint_contract
#endif
