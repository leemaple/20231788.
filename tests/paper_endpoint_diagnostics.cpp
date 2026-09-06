#include "paper_endpoint_diagnostics.h"
#include "paper_endpoint_exact_scalars_internal.h"
#include "paper_endpoint_checked_arithmetic.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace paper_endpoint_contract {
namespace {
using internal::RepresentedDyadic;
using paper_full_test::kAnchors;
using paper_full_test::kN;
using paper_full_test::kSlots;
constexpr int kCheckedBinaryExponentLimit = 400000;
// A scientific exponent with magnitude at most 119999 stays strictly inside
// the binary envelope because its significand is below 10 and log2(10) < 10/3.
constexpr int kProducerDecimalExponentLimit = 119999;
static_assert(std::numeric_limits<Binary768>::min_exponent < -kCheckedBinaryExponentLimit &&
              std::numeric_limits<Binary768>::max_exponent > kCheckedBinaryExponentLimit,
              "Binary768 backend must contain the checked transport exponent envelope");

[[noreturn]] void Stop(const char* reason, const std::string& detail) {
    FailEndpoint(reason, detail);
}

Int AbsInt(const Int& value) { return value < 0 ? Int(-value) : value; }
Rational Reduced(Int numerator, Int denominator) {
    if (denominator <= 0)
        throw std::invalid_argument("diagnostic rational denominator");
    const Int divisor = paper_full_test::Gcd(AbsInt(numerator), denominator);
    return {numerator / divisor, denominator / divisor};
}
Rational Add(const Rational& a, const Rational& b) {
    return Reduced(a.numerator * b.denominator + b.numerator * a.denominator,
                   a.denominator * b.denominator);
}
Rational Times(const Rational& a, const Rational& b) {
    return Reduced(a.numerator * b.numerator, a.denominator * b.denominator);
}
bool Greater(const Rational& a, const Rational& b) {
    return a.numerator * b.denominator > b.numerator * a.denominator;
}
Rational PowerTwo(int exponent) {
    if (exponent >= 0) return {Int(1) << static_cast<unsigned>(exponent), Int(1)};
    const auto shift = static_cast<unsigned>(-(static_cast<long long>(exponent)));
    return {Int(1), Int(1) << shift};
}
Rational NormBound(std::optional<int> exponent, int offset) {
    if (!exponent) return {Int(0), Int(1)};
    // Avoid overflowing the optional public exponent before the range check.
    const long long combined = static_cast<long long>(*exponent) + offset;
    if (combined < -400000 || combined > 400000)
        Stop("MODEL_UNSUPPORTED", "allowance exponent outside checked range");
    return PowerTwo(static_cast<int>(combined));
}
const Rational kCeiling = PowerTwo(-128);

void Finite(const Binary768& value) {
    if (!boost::math::isfinite(value)) Stop("NONFINITE", "represented component");
}
Rational AbsoluteValue(const Binary768& value) {
    Finite(value);
    auto result = RepresentedDyadic(value);
    result.numerator = AbsInt(result.numerator);
    return result;
}
template<unsigned Bits>
Rational MaximumOneNorm(const std::vector<Complex<Bits>>& values) {
    Rational maximum{Int(0), Int(1)};
    for (const auto& value : values) {
        const auto norm = Add(AbsoluteValue(Binary768(value.real)),
                              AbsoluteValue(Binary768(value.imag)));
        if (Greater(norm, maximum)) maximum = norm;
    }
    return maximum;
}
Rational MaximumOneNorm(const Observation& value) {
    const auto a = MaximumOneNorm(value.at512);
    const auto b = MaximumOneNorm(value.at768);
    return Greater(a, b) ? a : b;
}
struct Bounds final {
    Rational fresh, terminal, hornerFresh, hornerTerminal, power, propagated, subtraction;
    std::array<Rational, 4> residual;
    Rational identity;
};
Bounds Allowances(unsigned bits, const Observation& fresh, const Observation& terminal) {
    Bounds b;
    b.fresh = NormBound(fresh.scaledOneNormExponent, 12 - static_cast<int>(bits));
    b.terminal = NormBound(terminal.scaledOneNormExponent, 12 - static_cast<int>(bits));
    b.hornerFresh = NormBound(fresh.scaledOneNormExponent, 24 - static_cast<int>(bits));
    b.hornerTerminal = NormBound(terminal.scaledOneNormExponent, 24 - static_cast<int>(bits));
    b.power = PowerTwo(270 - static_cast<int>(bits));
    b.propagated = Add(b.power, Times(PowerTwo(263), b.fresh));
    b.subtraction = PowerTwo(264 - static_cast<int>(bits));
    b.residual = {{Add(b.fresh, b.subtraction),
                   Add(Add(b.terminal, b.power), b.subtraction),
                   Add(Add(b.propagated, b.power), b.subtraction),
                   Add(Add(b.terminal, b.propagated), b.subtraction)}};
    b.identity = Add(Add(Add(b.residual[1], b.residual[2]), b.residual[3]),
                     Times(Rational{Int(2), Int(1)}, b.subtraction));
    return b;
}

template<unsigned LeftBits, unsigned RightBits>
ComparisonReceipt Compare(const std::string& id,
                          const std::vector<Complex<LeftBits>>& left,
                          const std::vector<Complex<RightBits>>& right,
                          const Rational& allowance, Comparison kind,
                          bool supported = true, const char* unsupportedReason = "MODEL_UNSUPPORTED",
                          bool anchorsOnly = false,
                          const std::vector<std::array<bool, 2>>* rightAvailable = nullptr) {
    const auto expected = anchorsOnly ? kAnchors.size() : kSlots;
    if (left.size() != kSlots || right.size() != expected ||
        (rightAvailable != nullptr &&
         (kind != Comparison::Producer || rightAvailable->size() != expected)))
        Stop("INTEGRITY", "comparison shape " + id);
    ComparisonReceipt receipt{id, {Int(0), Int(1)}, allowance, 0, false};
    for (std::size_t index = 0; index < expected; ++index) {
        const auto slot = anchorsOnly ? kAnchors[index] : index;
        const std::array<Binary768, 2> a{{Binary768(left[slot].real), Binary768(left[slot].imag)}};
        const std::array<Binary768, 2> b{{Binary768(right[index].real), Binary768(right[index].imag)}};
        for (std::size_t component = 0; component < 2; ++component) {
            Finite(a[component]);
            // Unavailable finite-source transport has no distance to fabricate.
            // Still inspect every available component so a raw finite excess wins.
            if (rightAvailable != nullptr && !(*rightAvailable)[index][component]) {
                supported = false;
                continue;
            }
            Finite(b[component]);
            const auto distance = ExactAbsoluteDifference(a[component], b[component]);
            if (Greater(distance, receipt.distance)) {
                receipt.distance = distance;
                receipt.slot = slot;
                receipt.imaginary = component == 1;
            }
        }
    }
    const auto decision = AssessDifference(receipt.distance, allowance, kind, supported);
    if (decision == Decision::Fail) Stop("INTEGRITY", "comparison " + id);
    if (decision == Decision::Unresolved) {
        if (!supported) Stop(unsupportedReason, "comparison " + id);
        if (Greater(allowance, kCeiling)) Stop("ESTIMATOR_CEILING", "comparison " + id);
        Stop("MODEL_UNSUPPORTED", "threshold overlap " + id);
    }
    return receipt;
}

std::size_t IntegerBitLength(const Int& value) {
    if (value == 0) return 0;
    return static_cast<std::size_t>(boost::multiprecision::msb(AbsInt(value))) + 1;
}
bool Binary512ExponentSupported(const paper_full_test::Real& value) {
    if (value == 0) return true;
    int exponent = 0;
    (void)boost::multiprecision::frexp(value, &exponent);
    return exponent >= -kCheckedBinaryExponentLimit &&
           exponent <= kCheckedBinaryExponentLimit;
}
bool HornerConversionsSupportedImpl(const IntegerPolynomial& polynomial, const Scale& scale) {
    const auto supported = [](const Int& exact) {
        // Observe the unchanged decimal-string R(Int) path, not a replacement cast.
        if (IntegerBitLength(exact) >
            static_cast<std::size_t>(kCheckedBinaryExponentLimit)) return false;
        const paper_full_test::Real source = paper_full_test::R(exact);
        if (!boost::math::isfinite(source) || !Binary512ExponentSupported(source))
            return false;
        const auto actual = internal::WidenRepresented512(source);
        const auto represented = RepresentedDyadic(actual);
        const Int difference = AbsInt(represented.numerator - exact * represented.denominator);
        return (difference << 512) <= AbsInt(exact) * represented.denominator;
    };
    bool result = supported(scale.numerator) && supported(scale.denominator);
    for (const auto& coefficient : polynomial.coefficients)
        if (!supported(coefficient)) result = false;
    return result;
}
std::vector<Complex<768>> WidenAnchors(const std::array<paper_full_test::Complex, 10>& values) {
    std::vector<Complex<768>> result;
    result.reserve(values.size());
    for (const auto& value : values)
        result.push_back({internal::WidenRepresented512(value.real),
                          internal::WidenRepresented512(value.imag)});
    return result;
}
bool DecimalExponentWithinTransportEnvelope(const std::string& text) {
    const auto marker = text.find_first_of("eE");
    if (marker == std::string::npos || marker + 2 >= text.size())
        throw std::runtime_error("producer scientific conversion has no exponent");
    std::size_t index = marker + 1;
    if (text[index] != '+' && text[index] != '-')
        throw std::runtime_error("producer scientific conversion has no exponent sign");
    ++index;
    int magnitude = 0;
    bool outside = false;
    for (; index < text.size(); ++index) {
        if (text[index] < '0' || text[index] > '9')
            throw std::runtime_error("producer scientific conversion has malformed exponent");
        if (!outside) {
            const int digit = text[index] - '0';
            if (magnitude > (kProducerDecimalExponentLimit - digit) / 10)
                outside = true;
            else
                magnitude = magnitude * 10 + digit;
        }
    }
    return !outside;
}

internal::ProducerTransport TransportProducerImpl(
    const std::vector<paper_full_test::io::ClientComplex>& values) {
    internal::ProducerTransport result;
    result.values.reserve(values.size());
    result.available.reserve(values.size());
    result.supported = true;
    for (const auto& value : values) {
        if (!boost::math::isfinite(value.real) || !boost::math::isfinite(value.imag))
            Stop("NONFINITE", "producer component");
        if (value.real < -2 || value.real > 2 || value.imag < -2 || value.imag > 2)
            result.supported = false;

        Complex<768> converted{Binary768(0), Binary768(0)};
        std::array<bool, 2> available{{true, true}};
        const std::array<const paper_full_test::io::ClientReal*, 2> source{{&value.real,
                                                                            &value.imag}};
        std::array<Binary768*, 2> destination{{&converted.real, &converted.imag}};
        for (std::size_t component = 0; component < 2; ++component) {
            const std::string text = source[component]->str(100, std::ios_base::scientific);
            if (!DecimalExponentWithinTransportEnvelope(text)) {
                result.supported = false;
                available[component] = false;
                continue;
            }
            // The explicit decimal bound and Binary768 static_assert establish
            // exponent capacity. Any exception here is unexpected and is not caught.
            *destination[component] = Binary768(text);
            if (!boost::math::isfinite(*destination[component])) {
                result.supported = false;
                available[component] = false;
                continue;
            }
            if (*source[component] != 0 && *destination[component] == 0) {
                result.supported = false;
                available[component] = false;
                continue;
            }
            if (*destination[component] != 0) {
                int exponent = 0;
                (void)boost::multiprecision::frexp(*destination[component], &exponent);
                if (exponent < -kCheckedBinaryExponentLimit ||
                    exponent > kCheckedBinaryExponentLimit) {
                    result.supported = false;
                    available[component] = false;
                    continue;
                }
            }
            if (*destination[component] < -2 || *destination[component] > 2)
                result.supported = false;
        }
        result.values.push_back(std::move(converted));
        result.available.push_back(available);
    }
    return result;
}

template<unsigned Bits>
Complex<Bits> Difference(const Complex<Bits>& left, const Complex<Bits>& right) {
    return {internal::CheckedSubtract(left.real, right.real, "residual real subtraction"),
            internal::CheckedSubtract(left.imag, right.imag, "residual imaginary subtraction")};
}
template<unsigned Bits>
Complex<Bits> EighthSquare(Complex<Bits> value) {
    for (unsigned square = 0; square < 8; ++square) {
        const Binary<Bits> rr = internal::CheckedMultiply(value.real, value.real, "square rr");
        const Binary<Bits> ii = internal::CheckedMultiply(value.imag, value.imag, "square ii");
        const Binary<Bits> ri = internal::CheckedMultiply(value.real, value.imag, "square ri");
        const Binary<Bits> ir = internal::CheckedMultiply(value.imag, value.real, "square ir");
        value = {internal::CheckedSubtract(rr, ii, "square real subtraction"),
                 internal::CheckedAdd(ri, ir, "square imaginary addition")};
        Finite(Binary768(value.real)); Finite(Binary768(value.imag));
    }
    return value;
}
template<unsigned Bits>
Complex<Bits> ExactInput(std::size_t slot) {
    const auto t = slot / 2;
    const Int a = (Int(1015) << 65) - (Int(t % 16) << 59) + slot;
    Int b = Int(1 + (t / 16) % 8) << 65;
    if ((t / 512) % 2) b = -b;
    Int real, imag;
    switch ((t / 128) % 4) {
        case 0: real = a; imag = b; break;
        case 1: real = -b; imag = a; break;
        case 2: real = -a; imag = -b; break;
        default: real = b; imag = -a;
    }
    const auto convert = [](const Int& integer) {
        const Binary<Bits> value = boost::multiprecision::ldexp(
            Binary<Bits>(integer.convert_to<std::string>()), -75);
        const auto exact = RepresentedDyadic(Binary768(value));
        if ((exact.numerator << 75) != integer * exact.denominator)
            Stop("MODEL_UNSUPPORTED", "exact dyadic input conversion");
        return value;
    };
    if (4 * (AbsInt(real) + AbsInt(imag)) > 5 * (Int(1) << 75))
        Stop("CONDITIONING", "exact input radius");
    return {convert(real), convert(imag)};
}
template<unsigned Bits>
using Residuals = std::array<std::vector<Complex<Bits>>, 4>;
template<unsigned Bits>
Residuals<Bits> ComputeResiduals(const std::vector<Complex<Bits>>& fresh,
                               const std::vector<Complex<Bits>>& terminal) {
    Residuals<Bits> result;
    for (auto& values : result) values.reserve(kSlots);
    for (std::size_t slot = 0; slot < kSlots; ++slot) {
        const auto input = ExactInput<Bits>(slot);
        const auto idealPower = EighthSquare(input);
        const auto freshPower = EighthSquare(fresh[slot]);
        result[0].push_back(Difference(fresh[slot], input));
        result[1].push_back(Difference(terminal[slot], idealPower));
        result[2].push_back(Difference(freshPower, idealPower));
        result[3].push_back(Difference(terminal[slot], freshPower));
    }
    return result;
}
template<unsigned Bits>
std::vector<Complex<Bits>> Identity(const Residuals<Bits>& values) {
    std::vector<Complex<Bits>> result;
    result.reserve(kSlots);
    for (std::size_t slot = 0; slot < kSlots; ++slot)
        result.push_back(Difference(Difference(values[1][slot], values[2][slot]), values[3][slot]));
    return result;
}

const std::array<const char*, 4> kResidualNames{{"E0", "E8", "I8", "A8"}};
std::array<ResidualMaximum, 4> Maxima(const Residuals<768>& values, const Bounds& bounds) {
    std::array<ResidualMaximum, 4> result;
    for (std::size_t residual = 0; residual < result.size(); ++residual) {
        auto& maximum = result[residual];
        maximum.id = kResidualNames[residual];
        maximum.magnitude = 0;
        maximum.errorAllowance = bounds.residual[residual];
        maximum.slot = 0;
        maximum.imaginary = false;
        for (std::size_t slot = 0; slot < kSlots; ++slot) {
            const std::array<Binary768, 2> components{{values[residual][slot].real,
                                                      values[residual][slot].imag}};
            for (std::size_t component = 0; component < 2; ++component) {
                Finite(components[component]);
                const Binary768 magnitude = components[component] < 0
                    ? -components[component] : components[component];
                if (magnitude > maximum.magnitude) {
                    maximum.magnitude = magnitude;
                    maximum.slot = slot;
                    maximum.imaginary = component == 1;
                }
            }
        }
        for (std::size_t r = 0; r < 4; ++r) maximum.tuple[r] = values[r][maximum.slot];
    }
    return result;
}

void ValidateCaptureInputs(
    const IntegerPolynomial& fresh, const IntegerPolynomial& terminal,
    const Scale& freshScale, const Scale& terminalScale,
    const std::vector<paper_full_test::io::ClientComplex>& freshProducer,
    const std::vector<paper_full_test::io::ClientComplex>& terminalProducer) {
    if (fresh.coefficients.size() != kN || terminal.coefficients.size() != kN ||
        freshProducer.size() != kSlots || terminalProducer.size() != kSlots)
        throw std::invalid_argument("endpoint capture requires complete endpoint shapes");
    (void)ScaledOneNormExponent(Int(0), freshScale);
    (void)ScaledOneNormExponent(Int(0), terminalScale);
    for (const auto* polynomial : {&fresh, &terminal}) {
        if (polynomial->modulus <= 0 || (polynomial->modulus & 1) == 0)
            throw std::invalid_argument("endpoint capture requires positive odd modulus");
        for (const auto& coefficient : polynomial->coefficients)
            if (2 * AbsInt(coefficient) >= polynomial->modulus)
                throw std::invalid_argument("endpoint capture requires centered coefficients");
    }
}
} // namespace

namespace internal {
Binary768 WidenRepresented512(const paper_full_test::Real& value) {
    if (!boost::math::isfinite(value))
        Stop("NONFINITE", "fixed binary512 value");
    if (value == 0) return Binary768(0);
    if (!Binary512ExponentSupported(value))
        Stop("MODEL_UNSUPPORTED", "fixed binary512 exponent outside checked range");

    constexpr int sourceBits = std::numeric_limits<paper_full_test::Real>::digits;
    static_assert(sourceBits == 512 &&
                  sourceBits <= std::numeric_limits<Binary768>::digits,
                  "exact widening requires destination precision at least binary512");
    int exponent = 0;
    const paper_full_test::Real fraction =
        boost::multiprecision::frexp(value, &exponent);
    const paper_full_test::Real scaled =
        boost::multiprecision::ldexp(fraction, sourceBits);
    const Int significand = scaled.convert_to<Int>();
    const std::string significandText = significand.convert_to<std::string>();

    // Scaling exposes the complete represented significand as an integer. Check
    // that extraction in the source type before constructing the wider type.
    if (paper_full_test::Real(significandText) != scaled)
        Stop("MODEL_UNSUPPORTED", "fixed binary512 significand extraction");
    const Binary768 integerValue(significandText);
    if (integerValue.convert_to<Int>() != significand)
        Stop("MODEL_UNSUPPORTED", "binary768 integer reconstruction");
    Binary768 result = integerValue;
    result = boost::multiprecision::ldexp(result, exponent - sourceBits);
    if (!boost::math::isfinite(result) || result == 0 ||
        boost::multiprecision::ldexp(result, sourceBits - exponent) != integerValue)
        Stop("MODEL_UNSUPPORTED", "fixed binary512 widening range");
    return result;
}

bool HornerConversionsSupported(const IntegerPolynomial& polynomial, const Scale& scale) {
    return HornerConversionsSupportedImpl(polynomial, scale);
}

ProducerTransport TransportProducerForDiagnostics(
    const std::vector<paper_full_test::io::ClientComplex>& values) {
    return TransportProducerImpl(values);
}
} // namespace internal

EndpointEvidence CaptureEndpointEvidence(
    const IntegerPolynomial& fresh, const IntegerPolynomial& terminal,
    const Scale& freshScale, const Scale& terminalScale,
    const std::array<paper_full_test::Complex, 10>& freshHorner,
    const std::array<paper_full_test::Complex, 10>& terminalHorner,
    const std::vector<paper_full_test::io::ClientComplex>& freshProducer,
    const std::vector<paper_full_test::io::ClientComplex>& terminalProducer) {
    ValidateCaptureInputs(fresh, terminal, freshScale, terminalScale,
                          freshProducer, terminalProducer);
    return internal::CaptureEndpointEvidenceWithHornerSupport(
        fresh, terminal, freshScale, terminalScale, freshHorner, terminalHorner,
        freshProducer, terminalProducer,
        internal::HornerConversionsSupported(fresh, freshScale),
        internal::HornerConversionsSupported(terminal, terminalScale));
}

EndpointEvidence internal::CaptureEndpointEvidenceWithHornerSupport(
    const IntegerPolynomial& fresh, const IntegerPolynomial& terminal,
    const Scale& freshScale, const Scale& terminalScale,
    const std::array<paper_full_test::Complex, 10>& freshHorner,
    const std::array<paper_full_test::Complex, 10>& terminalHorner,
    const std::vector<paper_full_test::io::ClientComplex>& freshProducer,
    const std::vector<paper_full_test::io::ClientComplex>& terminalProducer,
    bool freshHornerSupported, bool terminalHornerSupported) {
    // Validate cheap input invariants before constructing any expensive roots.
    ValidateCaptureInputs(fresh, terminal, freshScale, terminalScale,
                          freshProducer, terminalProducer);
    const auto p0 = internal::TransportProducerForDiagnostics(freshProducer);
    const auto p8 = internal::TransportProducerForDiagnostics(terminalProducer);
    EndpointEvidence output;
    output.freshScale = freshScale;
    output.terminalScale = terminalScale;
    output.checks.reserve(24);

    struct Control final { const char* name; std::vector<SparseTerm> terms; int exponent; };
    const std::array<Control, 4> controls{{
        {"constant", {{0, Int(1)}}, 0},
        {"x", {{1, Int(1)}}, 0},
        {"xNminus1", {{kN - 1, Int(1)}}, 0},
        {"sparse", {{0, Int(3)}, {1, Int(-2)}, {17, Int(1)}, {kN - 1, Int(-1)}}, 3}
    }};
    for (const auto& control : controls) {
        IntegerPolynomial polynomial{std::vector<Int>(kN, Int(0)), Int(17)};
        for (const auto& term : control.terms) polynomial.coefficients[term.degree] = term.coefficient;
        const Scale unit{Int(1), Int(1)};
        const auto observed = Observe(polynomial, unit);
        const auto reference = DirectSparseReference768(control.terms, unit);
        const auto prefix = std::string("control.") + control.name;
        const auto directBound = PowerTwo(control.exponent + 10 - 768);
        output.checks.push_back(Compare(prefix + ".512", observed.at512, reference,
            Add(PowerTwo(control.exponent + 12 - 512), directBound), Comparison::TwoBoundedPaths));
        output.checks.push_back(Compare(prefix + ".768", observed.at768, reference,
            Add(PowerTwo(control.exponent + 12 - 768), directBound), Comparison::TwoBoundedPaths));
    }
    const auto x0 = Observe(fresh, freshScale);
    const auto x8 = Observe(terminal, terminalScale);
    output.freshCoefficientOneNorm = x0.coefficientOneNorm;
    output.terminalCoefficientOneNorm = x8.coefficientOneNorm;
    output.freshMaximumOneNorm = MaximumOneNorm(x0);
    output.terminalMaximumOneNorm = MaximumOneNorm(x8);
    const Rational radius{Int(5), Int(4)};
    const bool conditioned = !Greater(output.freshMaximumOneNorm, radius) &&
                             !Greater(output.terminalMaximumOneNorm, radius);
    const auto b512 = Allowances(512, x0, x8);
    const auto b768 = Allowances(768, x0, x8);
    output.checks.push_back(Compare("fresh.cross", x0.at512, x0.at768,
        Add(b512.fresh, b768.fresh), Comparison::TwoBoundedPaths, conditioned, "CONDITIONING"));
    output.checks.push_back(Compare("terminal.cross", x8.at512, x8.at768,
        Add(b512.terminal, b768.terminal), Comparison::TwoBoundedPaths, conditioned, "CONDITIONING"));

    const auto h0 = WidenAnchors(freshHorner);
    const auto h8 = WidenAnchors(terminalHorner);
    output.checks.push_back(Compare("fresh.horner.512", x0.at512, h0,
        Add(b512.fresh, b512.hornerFresh), Comparison::TwoBoundedPaths, freshHornerSupported,
        "MODEL_UNSUPPORTED", true));
    output.checks.push_back(Compare("fresh.horner.768", x0.at768, h0,
        Add(b768.fresh, b512.hornerFresh), Comparison::TwoBoundedPaths, freshHornerSupported,
        "MODEL_UNSUPPORTED", true));
    output.checks.push_back(Compare("terminal.horner.512", x8.at512, h8,
        Add(b512.terminal, b512.hornerTerminal), Comparison::TwoBoundedPaths, terminalHornerSupported,
        "MODEL_UNSUPPORTED", true));
    output.checks.push_back(Compare("terminal.horner.768", x8.at768, h8,
        Add(b768.terminal, b512.hornerTerminal), Comparison::TwoBoundedPaths, terminalHornerSupported,
        "MODEL_UNSUPPORTED", true));

    const auto transport = PowerTwo(-300);
    output.checks.push_back(Compare("fresh.producer.512", x0.at512, p0.values,
        Add(b512.fresh, transport), Comparison::Producer, p0.supported,
        "MODEL_UNSUPPORTED", false, &p0.available));
    output.checks.push_back(Compare("fresh.producer.768", x0.at768, p0.values,
        Add(b768.fresh, transport), Comparison::Producer, p0.supported,
        "MODEL_UNSUPPORTED", false, &p0.available));
    output.checks.push_back(Compare("terminal.producer.512", x8.at512, p8.values,
        Add(b512.terminal, transport), Comparison::Producer, p8.supported,
        "MODEL_UNSUPPORTED", false, &p8.available));
    output.checks.push_back(Compare("terminal.producer.768", x8.at768, p8.values,
        Add(b768.terminal, transport), Comparison::Producer, p8.supported,
        "MODEL_UNSUPPORTED", false, &p8.available));

    const auto r512 = ComputeResiduals(x0.at512, x8.at512);
    auto r768 = ComputeResiduals(x0.at768, x8.at768);
    for (std::size_t r = 0; r < 4; ++r)
        output.checks.push_back(Compare(std::string("residual.") + kResidualNames[r] + ".cross",
            r512[r], r768[r], Add(b512.residual[r], b768.residual[r]), Comparison::TwoBoundedPaths));
    const std::vector<Complex<768>> zero(kSlots, {Binary768(0), Binary768(0)});
    output.checks.push_back(Compare("identity.512", Identity(r512), zero,
        b512.identity, Comparison::TwoBoundedPaths));
    output.checks.push_back(Compare("identity.768", Identity(r768), zero,
        b768.identity, Comparison::TwoBoundedPaths));
    output.maxima = Maxima(r768, b768);
    output.freshErrors = std::move(r768[0]);
    output.terminalErrors = std::move(r768[1]);
    return output;
}
} // namespace paper_endpoint_contract
