// S100-FRESH-ERROR-REPAIR-01: opt-in, one fresh public encryption, no chain.
// RED deliberately calls an absent member through auto; there is no stub,
// feature-detection fallback, or dependency on the new result type's name.
#include "paper_full_eight_square_oracle.h"
#include "paper_endpoint_observer_contract.h"
#include "paper_endpoint_failure.h"

#include <boost/version.hpp>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#ifdef __APPLE__
#error "This diagnostic must be built and run on the authorized remote Linux/Windows host, not macOS"
#endif
#ifndef S100_FRESH_ERROR_SOURCE_COMMIT
#error "Configure the opt-in CMake target so the source revision is recorded"
#endif

namespace {
namespace pf = paper_full_test;
namespace observer = paper_endpoint_contract;
namespace io = openfhe_2023_1788::client_io;
using pf::Int;
using pf::Real;
using pf::Complex;
using pf::IntegerPolynomial;
using pf::Scale;
using Poly = lbcrypto::DCRTPoly;
using Context = lbcrypto::CryptoContext<Poly>;

static_assert(NATIVEINT == 64 && MATHBACKEND == 4, "frozen native64/backend4 profile");
static_assert(std::numeric_limits<Real>::digits == 512, "independent binary512 arithmetic");
static_assert(std::is_same_v<io::ExactInteger, Int>, "inspection must retain exact integers");
constexpr int kAgreementBits = 300;
constexpr unsigned kEnvelopeBits = 64;
const char* phase = "arguments";

class Invalid final : public std::runtime_error {
public:
    explicit Invalid(const char* reason) : std::runtime_error(reason) {}
};
void Check(bool condition, const char* reason) {
    if (!condition) throw Invalid(reason);
}
Int Magnitude(const Int& value) { return value < 0 ? Int(-value) : value; }
Real Tolerance() { return pf::Pow2(-kAgreementBits); }
void Finite(const Real& value) {
    Check(boost::math::isfinite(value), "NONFINITE");
}
void InEnvelope(const Real& value) {
    Finite(value);
    Check(pf::Abs(value) <= pf::Pow2(static_cast<int>(kEnvelopeBits)), "MODEL_ENVELOPE");
}
bool Near(const Real& left, const Real& right) {
    Finite(left); Finite(right);
    return pf::Abs(left - right) <= Tolerance();
}

// Preserve the produced decimal value, including backend guard digits, rather
// than first rounding through double or using the production forward transform.
Real Bridge(const io::ClientReal& value) {
    Check(boost::math::isfinite(value), "NONFINITE_CLIENT_VALUE");
    const std::string text = value.str(170, std::ios_base::scientific);
    Check(io::ClientReal(text) == value, "CLIENT_DECIMAL_ROUNDTRIP");
    const Real result(text);
    InEnvelope(result);
    return result;
}
Complex Bridge(const io::ClientComplex& value) {
    return {Bridge(value.real), Bridge(value.imag)};
}
Real Part(const Complex& value, std::size_t component) {
    Check(component < 2, "BAD_COMPONENT");
    return component == 0 ? value.real : value.imag;
}
Complex Value512(const observer::Observation& value, std::size_t slot) {
    return {Real(value.at512.at(slot).real), Real(value.at512.at(slot).imag)};
}

void Stage(const char* label) {
    phase = label;
    std::cout << "S100 stage=" << label << '\n' << std::flush;
}
void Scalar(const char* name, const Real& value) {
    Finite(value);
    std::cout << "S100 scalar=" << name << " value=" << value << '\n';
}

template <class Exception, class Call>
void Reject(const char* expected, Call call) {
    bool rejected = false;
    try { call(); }
    catch (const Exception& error) {
        Check(std::string(error.what()) == expected, "WRONG_REJECTION_REASON");
        rejected = true;
    }
    Check(rejected, "EXPECTED_REJECTION_MISSING");
}

Context SmallContext() {
    // Same supported public N64/S16 setup as the existing client contract.
    // No KeyGen, secret, public key, ciphertext, or random fixture is needed.
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> p;
    p.SetMultiplicativeDepth(7); p.SetScalingModSize(50); p.SetFirstModSize(55);
    p.SetScalingTechnique(lbcrypto::FIXEDMANUAL); p.SetKeySwitchTechnique(lbcrypto::HYBRID);
    p.SetDigitSize(0); p.SetMaxRelinSkDeg(2); p.SetNumLargeDigits(0);
    p.SetSecretKeyDist(lbcrypto::UNIFORM_TERNARY); p.SetSecurityLevel(lbcrypto::HEStd_NotSet);
    p.SetRingDim(64); p.SetBatchSize(16); p.SetCKKSDataType(lbcrypto::COMPLEX);
    p.SetPREMode(lbcrypto::NOT_SET); p.SetExecutionMode(lbcrypto::EXEC_EVALUATION);
    p.SetDecryptionNoiseMode(lbcrypto::FIXED_NOISE_DECRYPT);
    p.SetStandardDeviation(3.19f); p.SetNoiseEstimate(0.0); p.SetDesiredPrecision(25.0);
    p.SetStatisticalSecurity(30); p.SetNumAdversarialQueries(1);
    p.SetInteractiveBootCompressionLevel(lbcrypto::SLACK);
    p.SetCompositeDegree(1); p.SetRegisterWordSize(64);
    auto context = lbcrypto::GenCryptoContext(p);
    Check(context != nullptr, "NULL_SMALL_CONTEXT");
    context->Enable(lbcrypto::PKE);
    context->Enable(lbcrypto::KEYSWITCH);
    context->Enable(lbcrypto::LEVELEDSHE);
    return context;
}

template <class Inspection>
void Metadata(const Inspection& value, const Context& context,
              std::uint32_t slots, std::uint32_t gap, const Int& scale) {
    static_assert(std::is_same_v<std::decay_t<decltype(value.signedCoefficients)>,
                                 std::vector<Int>>, "owned exact coefficient vector");
    const auto params = context->GetCryptoParameters()->GetElementParams();
    Check(params != nullptr, "NULL_ELEMENT_PARAMETERS");
    const auto m = params->GetCyclotomicOrder();
    Check(value.basis.cyclotomicOrder == m && value.basis.ringDimension == m / 2 &&
          value.signedCoefficients.size() == m / 2, "INSPECTION_GEOMETRY");
    Check(value.slots == slots && value.strideGap == gap &&
          value.projection == io::CanonicalProjection::OpenFhePackedStride, "INSPECTION_PACKING");
    Check(value.logicalScale.Numerator() == scale && value.logicalScale.Denominator() == 1,
          "INSPECTION_EXACT_SCALE");
    const auto& towers = params->GetParams();
    Check(value.basis.moduliDecimal.size() == towers.size() &&
          value.basis.rootsOfUnityDecimal.size() == towers.size(), "INSPECTION_BASIS_COUNT");
    for (std::size_t i = 0; i < towers.size(); ++i) {
        Check(value.basis.moduliDecimal[i] == towers[i]->GetModulus().ToString() &&
              value.basis.rootsOfUnityDecimal[i] == towers[i]->GetRootOfUnity().ToString(),
              "INSPECTION_ORDERED_BASIS");
    }
}

struct Difference final {
    IntegerPolynomial polynomial;
    Int minimumLiftHeadroom;
};
Difference RawDifference(const IntegerPolynomial& p, const IntegerPolynomial& m) {
    Check(p.modulus == m.modulus && m.modulus > 0 && (m.modulus & 1) != 0,
          "RAW_DIFFERENCE_MODULUS");
    Check(!m.coefficients.empty() && p.coefficients.size() == m.coefficients.size(),
          "RAW_DIFFERENCE_SIZE");
    Difference result{{std::vector<Int>(m.coefficients.size(), Int(0)), m.modulus}, m.modulus};
    for (std::size_t j = 0; j < m.coefficients.size(); ++j) {
        Check(2 * Magnitude(m.coefficients[j]) < m.modulus &&
              2 * Magnitude(p.coefficients[j]) < m.modulus, "UNCENTERED_INPUT_LIFT");
        // This subtraction is over Z. NEVER use Center/Mod/% on this difference.
        const Int delta = p.coefficients[j] - m.coefficients[j];
        Check(2 * Magnitude(delta) < m.modulus, "RAW_DIFFERENCE_NOT_CENTERED");
        const Int headroom = m.modulus - 2 * (Magnitude(m.coefficients[j]) + Magnitude(delta));
        // Sufficient, deliberately conservative lift check, not a sampler bound.
        Check(headroom > 0, "INSUFFICIENT_LIFT_HEADROOM");
        Check(m.coefficients[j] + delta == p.coefficients[j], "INTEGER_RECONSTRUCTION");
        if (headroom < result.minimumLiftHeadroom) result.minimumLiftHeadroom = headroom;
        result.polynomial.coefficients[j] = delta;
    }
    return result;
}

void LiftControls() {
    const IntegerPolynomial m{{Int(3), Int(-4)}, Int(101)};
    const IntegerPolynomial p{{Int(5), Int(-7)}, Int(101)};
    const auto d = RawDifference(p, m);
    Check(d.polynomial.coefficients == std::vector<Int>({Int(2), Int(-3)}) &&
          d.minimumLiftHeadroom == 87, "RAW_DIFFERENCE_EXACT_CONTROL");
    // Center(-98 mod 101) would be +3. The raw -98 MUST instead be rejected.
    Reject<Invalid>("RAW_DIFFERENCE_NOT_CENTERED", [] {
        (void)RawDifference(IntegerPolynomial{{Int(-49)}, Int(101)},
                            IntegerPolynomial{{Int(49)}, Int(101)});
    });
    Reject<Invalid>("RAW_DIFFERENCE_NOT_CENTERED", [] {
        (void)RawDifference(IntegerPolynomial{{Int(49)}, Int(101)},
                            IntegerPolynomial{{Int(-49)}, Int(101)});
    });
    Reject<Invalid>("INSUFFICIENT_LIFT_HEADROOM", [] {
        (void)RawDifference(IntegerPolynomial{{Int(0)}, Int(101)},
                            IntegerPolynomial{{Int(49)}, Int(101)});
    });
    Reject<Invalid>("RAW_DIFFERENCE_MODULUS", [] {
        (void)RawDifference(IntegerPolynomial{{Int(0)}, Int(103)},
                            IntegerPolynomial{{Int(0)}, Int(101)});
    });
    Reject<Invalid>("UNCENTERED_INPUT_LIFT", [] {
        (void)RawDifference(IntegerPolynomial{{Int(51)}, Int(101)},
                            IntegerPolynomial{{Int(0)}, Int(101)});
    });
    Check(Near(Real(0), Tolerance()) && !Near(Real(0), Real(4) * Tolerance()),
          "AGREEMENT_GATE_CONTROL");
}

// An independent *forward* polynomial evaluation at exp(2*pi*i*5^s/128).
// These small controls do not duplicate the production inverse FFT/rounder.
std::vector<io::ClientComplex> SmallPolynomialInputs(const std::vector<Int>& coefficients,
                                                   const Int& scale) {
    Check(coefficients.size() == 64, "SMALL_CONTROL_DEGREE");
    using B = observer::Binary512;
    const B pi = boost::math::constants::pi<B>();
    const B normalization = B(1) / B(scale.convert_to<std::string>());
    std::vector<io::ClientComplex> result;
    std::uint32_t exponent = 1;
    for (std::size_t s = 0; s < 16; ++s) {
        const B angle = B(2) * pi * exponent / 128;
        const B rootRe = boost::multiprecision::cos(angle);
        const B rootIm = boost::multiprecision::sin(angle);
        B re = 0, im = 0;
        for (auto it = coefficients.rbegin(); it != coefficients.rend(); ++it) {
            const B nextRe = re * rootRe - im * rootIm + B(it->convert_to<std::string>());
            const B nextIm = re * rootIm + im * rootRe;
            re = nextRe; im = nextIm;
        }
        re *= normalization; im *= normalization;
        result.push_back({io::ClientReal(re.str(100, std::ios_base::scientific)),
                          io::ClientReal(im.str(100, std::ios_base::scientific))});
        exponent = (5U * exponent) % 128U;
    }
    return result;
}

void DecompositionControls();
void ObserverOrderControls();

void Controls() {
    Stage("keyless_controls");
    LiftControls();
    DecompositionControls();
    const Context context = SmallContext();
    const io::HighPrecisionClientIO client(context);
    const Int scale = Int(1) << 100;
    const io::FreshEncodingSpec spec{16, io::PositiveRationalScale::FromPositive(scale, 1)};
    const io::ClientReal denominator(scale.convert_to<std::string>());

    // Hand-derived constant coefficients, including an exact small 2^-75 value.
    const std::vector<Int> constants{Int(0), scale, Int(-scale), Int(3) * (scale >> 3),
                                     Int(-3) * (scale >> 3), Int(1) << 25};
    for (const auto& coefficient : constants) {
        const io::ClientReal c = io::ClientReal(coefficient.convert_to<std::string>()) / denominator;
        const std::vector<io::ClientComplex> values(16, io::ClientComplex{c, io::ClientReal(0)});
        const auto encoded = client.InspectEncoding(values, spec); // intentional RED seam
        static_assert(!std::is_reference_v<decltype(client.InspectEncoding(values, spec))>,
                      "inspection returns an owned value, not a view");
        Metadata(encoded, context, 16, 2, scale);
        std::vector<Int> expected(64, Int(0));
        expected[0] = coefficient;
        Check(encoded.signedCoefficients == expected, "EXACT_REAL_CONSTANT");
    }
    {
        const std::vector<io::ClientComplex> values(16, io::ClientComplex{io::ClientReal(0), io::ClientReal(-1)});
        const auto encoded = client.InspectEncoding(values, spec);
        std::vector<Int> expected(64, Int(0)); expected[32] = -scale;
        Check(encoded.signedCoefficients == expected, "EXACT_IMAGINARY_CONSTANT");
    }
    // Nearest rounding away from ambiguous halves, for both signs. No new tie policy.
    const std::array<std::pair<int, int>, 8> rounded{{
        {1, 0}, {3, 1}, {5, 1}, {7, 2}, {-1, 0}, {-3, -1}, {-5, -1}, {-7, -2}}};
    for (const auto& control : rounded) {
        const io::ClientReal c = io::ClientReal(control.first) / (4 * denominator);
        const auto encoded = client.InspectEncoding(
            std::vector<io::ClientComplex>(16, io::ClientComplex{c, io::ClientReal(0)}), spec);
        std::vector<Int> expected(64, Int(0)); expected[0] = control.second;
        Check(encoded.signedCoefficients == expected, "SIGNED_NEAREST_ROUNDING");
    }
    for (int numerator : {-3, -1, 1, 3}) {
        const io::ClientReal c = io::ClientReal(numerator) / (2 * denominator);
        Reject<std::range_error>("HighPrecisionClientIO: ambiguous encoding precision", [&] {
            (void)client.InspectEncoding(
                std::vector<io::ClientComplex>(16, io::ClientComplex{c, io::ClientReal(0)}), spec);
        });
    }

    // Degree two tests powers-of-five order, sign and stride. The mixed control
    // additionally tests the imaginary half and preservation of the 2^-75 term.
    std::vector<Int> monomial(64, Int(0)); monomial[2] = scale >> 3;
    std::vector<Int> mixed = monomial;
    mixed[0] = 3 * (scale >> 3); mixed[6] = -(scale >> 4);
    mixed[32] = -(scale >> 3); mixed[34] = Int(1) << 25;
    for (const auto& expected : {monomial, mixed}) {
        const auto values = SmallPolynomialInputs(expected, scale);
        const auto encoded = client.InspectEncoding(values, spec);
        Check(encoded.signedCoefficients == expected, "DIRECT_POLYNOMIAL_SIGN_ORDER_SCALE");
    }
    // Mutation sentinels demonstrate these controls distinguish plausible errors.
    const auto monomialValues = SmallPolynomialInputs(monomial, scale);
    auto wrongOrder = monomialValues; std::swap(wrongOrder[0], wrongOrder[1]);
    auto wrongSign = monomialValues;
    auto wrongScale = monomialValues;
    for (auto& z : wrongSign) z.imag = -z.imag;
    for (auto& z : wrongScale) { z.real *= 2; z.imag *= 2; }
    for (const auto& wrong : {wrongOrder, wrongSign, wrongScale}) {
        Check(client.InspectEncoding(wrong, spec).signedCoefficients != monomial,
              "MUTATION_SENTINEL_INEFFECTIVE");
    }

    const std::vector<io::ClientComplex> zeros(16, io::ClientComplex{io::ClientReal(0), io::ClientReal(0)});
    for (std::size_t count : {std::size_t(0), std::size_t(15), std::size_t(17)}) {
        Reject<std::invalid_argument>("HighPrecisionClientIO: expected exactly 16 slots", [&] {
            (void)client.InspectEncoding(std::vector<io::ClientComplex>(count), spec);
        });
    }
    Reject<std::invalid_argument>("HighPrecisionClientIO: expected exactly 16 slots", [&] {
        (void)client.InspectEncoding(zeros, io::FreshEncodingSpec{15, spec.logicalScale});
    });
    for (const auto& badScale : {
             io::PositiveRationalScale::FromPositive(Int(1) << 99, 1),
             io::PositiveRationalScale::FromPositive(scale, 3)}) {
        Reject<std::domain_error>("HighPrecisionClientIO: unsupported fresh exact scale", [&] {
            (void)client.InspectEncoding(zeros, io::FreshEncodingSpec{16, badScale});
        });
    }
    const std::array<io::ClientReal, 3> nonfinite{{
        std::numeric_limits<io::ClientReal>::infinity(),
        io::ClientReal(-std::numeric_limits<io::ClientReal>::infinity()),
        std::numeric_limits<io::ClientReal>::quiet_NaN()}};
    for (const io::ClientReal& bad : nonfinite) {
        for (std::size_t component = 0; component < 2; ++component) {
            auto values = zeros;
            if (component == 0) values[3].real = bad; else values[3].imag = bad;
            Reject<std::range_error>("HighPrecisionClientIO: nonfinite numerical value", [&] {
                (void)client.InspectEncoding(values, spec);
            });
        }
    }
    Reject<std::range_error>("HighPrecisionClientIO: encoding supported range exceeded", [&] {
        const io::ClientReal large((Int(1) << 100).convert_to<std::string>());
        (void)client.InspectEncoding(
            std::vector<io::ClientComplex>(16, io::ClientComplex{large, io::ClientReal(0)}), spec);
    });
    // Validation order: slots before scale; scale before nonfinite values.
    Reject<std::invalid_argument>("HighPrecisionClientIO: expected exactly 16 slots", [&] {
        (void)client.InspectEncoding({}, io::FreshEncodingSpec{15,
            io::PositiveRationalScale::FromPositive(1, 1)});
    });
    Reject<std::domain_error>("HighPrecisionClientIO: unsupported fresh exact scale", [&] {
        auto values = zeros; values[0].real = std::numeric_limits<io::ClientReal>::quiet_NaN();
        (void)client.InspectEncoding(values, io::FreshEncodingSpec{16,
            io::PositiveRationalScale::FromPositive(1, 1)});
    });

    auto values = SmallPolynomialInputs(mixed, scale);
    auto mutableSpec = spec;
    const auto before = values;
    auto first = client.InspectEncoding(values, mutableSpec);
    const auto separateCopy = first;
    for (std::size_t i = 0; i < values.size(); ++i)
        Check(values[i].real == before[i].real && values[i].imag == before[i].imag,
              "INSPECTION_MUTATED_INPUT");
    values[0] = {io::ClientReal(7), io::ClientReal(9)};
    mutableSpec.slots = 1;
    mutableSpec.logicalScale = io::PositiveRationalScale::FromPositive(1, 1);
    Check(first.signedCoefficients == mixed, "INSPECTION_ALIASES_INPUT");
    Metadata(first, context, 16, 2, scale);
    first.signedCoefficients[0] = 17;
    first.basis.moduliDecimal[0] = "3"; first.basis.rootsOfUnityDecimal[0] = "1";
    first.basis.cyclotomicOrder = 4; first.basis.ringDimension = 2;
    first.slots = 1; first.strideGap = 17;
    first.logicalScale = io::PositiveRationalScale::FromPositive(1, 1);
    Check(separateCopy.signedCoefficients == mixed, "INSPECTION_COPY_ALIASES_RESULT");
    Metadata(separateCopy, context, 16, 2, scale);
    const auto later = client.InspectEncoding(before, spec);
    Check(later.signedCoefficients == mixed, "RESULT_MUTATION_CHANGED_CLIENT");
    Metadata(later, context, 16, 2, scale);
    ObserverOrderControls();
    std::cout << "S100 status=COMPLETE mode=controls setup=keyless precision_claim=NONE\n";
}

Int CheckedOneNorm(const IntegerPolynomial& polynomial, const Scale& scale) {
    Check(polynomial.coefficients.size() == pf::kN && polynomial.modulus > 0 &&
          (polynomial.modulus & 1) != 0, "ORACLE_POLYNOMIAL_SHAPE");
    Check(scale.numerator == (Int(1) << 100) && scale.denominator == 1, "ORACLE_S100_SCALE");
    Int norm = 0;
    for (const auto& c : polynomial.coefficients) {
        Check(2 * Magnitude(c) < polynomial.modulus, "ORACLE_UNCENTERED_COEFFICIENT");
        norm += Magnitude(c);
    }
    Check(norm <= (scale.numerator << kEnvelopeBits), "MODEL_ENVELOPE");
    return norm;
}

// No transform is accepted solely because two FFT precisions agree. The
// existing ten direct binary512 Horner roots/evaluations are an additional path.
void CheckObservation(const IntegerPolynomial& polynomial, const Scale& scale,
                      const observer::Observation& observed,
                      const std::array<Complex, 10>& roots, const char* name) {
    Check(observed.at512.size() == pf::kSlots && observed.at768.size() == pf::kSlots,
          "OBSERVER_SLOT_COUNT");
    Check(CheckedOneNorm(polynomial, scale) == observed.coefficientOneNorm,
          "OBSERVER_EXACT_ONE_NORM");
    const auto anchors = pf::Horner(polynomial, scale, roots);
    observer::Binary768 cross = 0;
    for (std::size_t s = 0; s < pf::kSlots; ++s) {
        const auto& a = observed.at512[s];
        const auto& b = observed.at768[s];
        for (const auto& distance : {
                 boost::multiprecision::abs(observer::Binary768(a.real) - b.real),
                 boost::multiprecision::abs(observer::Binary768(a.imag) - b.imag)}) {
            Check(boost::math::isfinite(distance), "NONFINITE_OBSERVER");
            if (distance > cross) cross = distance;
        }
        InEnvelope(Real(a.real)); InEnvelope(Real(a.imag));
        Check(boost::math::isfinite(b.real) && boost::math::isfinite(b.imag), "NONFINITE_OBSERVER");
    }
    // Build the exact dyadic bound in its destination type: Boost 1.83's
    // fixed-512 to dynamic-768 copy path triggers GCC's array-bounds diagnostic.
    const observer::Binary768 crossTolerance =
        boost::multiprecision::ldexp(observer::Binary768(1), -kAgreementBits);
    Check(cross <= crossTolerance, "OBSERVER_512_768_DISAGREEMENT");
    Real hornerMax = 0;
    for (std::size_t a = 0; a < pf::kAnchors.size(); ++a) {
        const Real error = pf::Error(anchors[a], Value512(observed, pf::kAnchors[a]));
        if (error > hornerMax) hornerMax = error;
    }
    Check(hornerMax <= Tolerance(), "OBSERVER_HORNER_DISAGREEMENT");
    std::cout << "S100 oracle=" << name << " slots=" << pf::kSlots
              << " horner_anchors=" << pf::kAnchors.size()
              << " cross512_768=" << cross << " horner_max=" << hornerMax << '\n';
}

void ObserverOrderControls() {
    Stage("observer_order_controls");
    const Int delta = Int(1) << 100;
    const Scale scale{delta, Int(1)};
    IntegerPolynomial polynomial{std::vector<Int>(pf::kN, Int(0)), (Int(1) << 102) + 1};
    polynomial.coefficients[1] = delta; // X after dividing by the exact scale.
    const auto roots = pf::AnchorRoots();
    const auto observed = observer::Observe(polynomial, scale);
    const auto reference = observer::DirectSparseReference768({{1, delta}}, scale);
    Check(reference.size() == pf::kSlots, "OBSERVER_REFERENCE_SLOT_COUNT");
    const observer::Binary768 tolerance =
        boost::multiprecision::ldexp(observer::Binary768(1), -kAgreementBits);
    const auto validate = [&](const observer::Observation& value) {
        CheckObservation(polynomial, scale, value, roots, "monomial_x_order_control");
        for (std::size_t s = 0; s < pf::kSlots; ++s) {
            const std::array<observer::Binary768, 2> at512{{
                observer::Binary768(value.at512[s].real),
                observer::Binary768(value.at512[s].imag)}};
            const std::array<observer::Binary768, 2> at768{{
                value.at768[s].real, value.at768[s].imag}};
            const std::array<observer::Binary768, 2> expected{{
                reference[s].real, reference[s].imag}};
            for (std::size_t component = 0; component < 2; ++component) {
                Check(boost::math::isfinite(expected[component]), "NONFINITE_REFERENCE");
                Check(boost::multiprecision::abs(at512[component] - expected[component]) <= tolerance &&
                      boost::multiprecision::abs(at768[component] - expected[component]) <= tolerance,
                      "OBSERVER_SLOT_ORDER");
            }
        }
    };
    validate(observed);
    auto permuted = observed;
    std::swap(permuted.at512[2], permuted.at512[3]);
    std::swap(permuted.at768[2], permuted.at768[3]);
    // Neither 2 nor 3 is a Horner anchor; the all-slot direct control must reject this.
    Reject<Invalid>("OBSERVER_SLOT_ORDER", [&] { validate(permuted); });
    std::cout << "S100 observer_order_control=COMPLETE slots=" << pf::kSlots
              << " rejected_shared_nonanchor_permutation=1 public_encryptions=0\n";
}

struct Tuple final { Real encoding, encryption, decoding, total, residual; };
Tuple Decompose(const Real& z, const Real& m, const Real& d, const Real& p, const Real& production) {
    for (const auto& value : {z, m, d, p, production}) InEnvelope(value);
    const Real encoding = m - z, encryption = d, decoding = production - p;
    const Real total = production - z;
    const Real residual = ((encoding + encryption) + decoding) - total;
    Finite(residual);
    return {encoding, encryption, decoding, total, residual};
}
struct Maximum final { Real magnitude = 0; std::size_t slot = 0, component = 0; };
void Update(Maximum& maximum, const Real& value, std::size_t slot, std::size_t component) {
    Finite(value);
    const Real absolute = pf::Abs(value);
    if (absolute > maximum.magnitude) maximum = {absolute, slot, component};
    // Stable ties: first slot, then real before imaginary, including all-zero.
}
void DecompositionControls() {
    // Four extrema intentionally occur at four different slot/components;
    // cancellation makes the total maximum smaller than each term maximum.
    const std::array<std::array<Real, 3>, 4> terms{{
        {{Real(8), Real(-7), Real(0)}},
        {{Real(-7), Real(9), Real(-1)}},
        {{Real(-5), Real(-5), Real(10)}},
        {{Real("2.5"), Real("2.5"), Real("2.5")}}
    }};
    std::array<Maximum, 4> maxima{};
    for (std::size_t i = 0; i < terms.size(); ++i) {
        const auto& e = terms[i];
        const Tuple t = Decompose(Real(0), e[0], e[1], Real(e[0] + e[1]),
                                  Real((e[0] + e[1]) + e[2]));
        Check(t.encoding == e[0] && t.encryption == e[1] && t.decoding == e[2] &&
              t.total == (e[0] + e[1]) + e[2] && t.residual == 0,
              "SIGNED_DECOMPOSITION_CONTROL");
        const std::array<Real, 4> values{{t.encoding, t.encryption, t.decoding, t.total}};
        for (std::size_t term = 0; term < values.size(); ++term)
            Update(maxima[term], values[term], i / 2, i % 2);
    }
    const std::array<Real, 4> expected{{Real(8), Real(9), Real(10), Real("7.5")}};
    for (std::size_t i = 0; i < maxima.size(); ++i)
        Check(maxima[i].magnitude == expected[i] && maxima[i].slot == i / 2 &&
              maxima[i].component == i % 2, "DISTINCT_EXTREMA_CONTROL");
    Maximum tie{};
    Update(tie, Real(-1), 0, 0); Update(tie, Real(1), 0, 1);
    Check(tie.slot == 0 && tie.component == 0, "DETERMINISTIC_TIE_CONTROL");
    const Tuple wrongSign = Decompose(Real(0), Real(8), Real(7), Real(1), Real(1));
    Check(!Near(wrongSign.residual, Real(0)), "SIGNED_RECONSTRUCTION_MUTATION_UNDETECTED");
}

void EmitTuple(const char* selector, std::size_t slot, std::size_t component,
               const Real& z, const Tuple& t) {
    std::cout << "S100 tuple=" << selector << " slot=" << slot
              << " component=" << (component == 0 ? "real" : "imag")
              << " z=" << z << " encoding=" << t.encoding << " encryption=" << t.encryption
              << " decoding=" << t.decoding << " E0=" << t.total
              << " reconstruction_residual=" << t.residual << '\n';
}

void Fresh() {
    Stage("paper_s100_setup");
    LiftControls();
    DecompositionControls();
    auto setup = openfhe_2023_1788::CreatePaperRepeatedMult2Setup();
    Check(setup.plan && setup.publicKey && setup.rootSecret, "MISSING_PAPER_SETUP");
    const Context context = setup.plan->GetFamilyContext(0);
    Check(setup.publicKey->GetCryptoContext() == context && setup.rootSecret->GetCryptoContext() == context &&
          setup.publicKey->GetKeyTag() == setup.plan->GetFamilyKeyTag(0) &&
          setup.rootSecret->GetKeyTag() == setup.plan->GetFamilyKeyTag(0), "PAPER_KEY_IDENTITY");
    const auto publicBefore = setup.publicKey->GetPublicElements();
    const auto secretBefore = setup.rootSecret->GetPrivateElement();
    const io::HighPrecisionClientIO client(setup.plan);
    const Scale scale{Int(1) << 100, Int(1)};
    const io::FreshEncodingSpec spec{static_cast<std::uint32_t>(pf::kSlots),
        io::PositiveRationalScale::FromPositive(scale.numerator, scale.denominator)};
    const auto z = pf::Inputs(); // unchanged fixed dyadic family, no RNG/fixture replacement
    const auto values = pf::ClientInputs(z);
    Check(z.size() == pf::kSlots && values.size() == pf::kSlots, "FROZEN_INPUT_COUNT");
    for (std::size_t s = 0; s < pf::kSlots; ++s) {
        const auto represented = Bridge(values[s]);
        Check(represented.real == z[s].real && represented.imag == z[s].imag,
              "FROZEN_INPUT_DECIMAL_EXACTNESS");
    }
    Stage("shared_encoding_inspection");
    const auto inspected = client.InspectEncoding(values, spec);
    Metadata(inspected, context, static_cast<std::uint32_t>(pf::kSlots), 1, scale.numerator);
    Check(inspected.basis.moduliDecimal.size() == pf::kQ.size(), "S100_Q_COUNT");
    Int modulus = 1;
    for (std::size_t j = 0; j < pf::kQ.size(); ++j) {
        Check(inspected.basis.moduliDecimal[j] == std::to_string(pf::kQ[j]) &&
              inspected.basis.rootsOfUnityDecimal[j] == std::to_string(pf::kRoots[j]), "FROZEN_Q_ROOTS");
        modulus *= pf::kQ[j];
    }
    const IntegerPolynomial m{inspected.signedCoefficients, modulus};
    Stage("single_public_encryption");
    const auto fresh = client.Encrypt(setup.publicKey, values, spec); // the ONE actual public encryption
    const auto& state = fresh.State();
    Check(state.origin == io::ClientCiphertextOrigin::FreshClientEncoding &&
          state.level == 0 && state.componentCount == 2 && state.noiseScaleDegree == 2 &&
          state.logicalScale.Numerator() == scale.numerator && state.logicalScale.Denominator() == 1 &&
          state.slots == pf::kSlots && state.strideGap == 1 &&
          state.activeBasis.moduliDecimal == inspected.basis.moduliDecimal &&
          state.activeBasis.rootsOfUnityDecimal == inspected.basis.rootsOfUnityDecimal,
          "FRESH_STATE_IDENTITY");
    const auto source = fresh.CloneForEvaluation(); // owned copy for independent CRT, NOT evaluation
    Stage("independent_sparse_crt");
    const auto secret = pf::ReadSecret(setup.rootSecret); // h128 checked in memory, never emitted
    const auto p = pf::SparseDecrypt(source, secret);
    Check(p.modulus == modulus && p.coefficients.size() == pf::kN, "DECRYPTED_POLYNOMIAL_IDENTITY");
    const auto difference = RawDifference(p, m); // no re-centering of the raw p-m
    Stage("production_decrypt");
    const auto decoded = client.Decrypt(setup.rootSecret, fresh);
    Check(decoded.values.size() == pf::kSlots && decoded.diagnostics.activeCompositeModulus == modulus &&
          decoded.diagnostics.centeredHeadroom > 0, "PRODUCTION_DECODE_STATE");
    const Real codecCross = Bridge(decoded.diagnostics.maximumCrossPrecisionDisagreement);
    Check(codecCross >= 0 && codecCross <= pf::Pow2(-120), "PRODUCTION_INTERNAL_CROSS_PRECISION");
    // No total/encoding/encryption/decoding error gate is imposed here.
    const auto repeatedInspection = client.InspectEncoding(values, spec);
    Check(repeatedInspection.signedCoefficients == m.coefficients, "SHARED_ENCODING_NOT_DETERMINISTIC");
    Metadata(repeatedInspection, context, static_cast<std::uint32_t>(pf::kSlots), 1, scale.numerator);
    Check(setup.publicKey->GetPublicElements() == publicBefore &&
          setup.rootSecret->GetPrivateElement() == secretBefore, "CLIENT_MUTATED_KEY_VALUES");
    for (std::size_t s = 0; s < pf::kSlots; ++s) {
        const auto represented = Bridge(values[s]);
        Check(represented.real == z[s].real && represented.imag == z[s].imag, "CLIENT_MUTATED_INPUT_VALUES");
    }

    Stage("independent_embeddings_and_horner");
    (void)CheckedOneNorm(m, scale);
    (void)CheckedOneNorm(p, scale);
    (void)CheckedOneNorm(difference.polynomial, scale);
    const auto roots = pf::AnchorRoots();
    // Observe all three integer polynomials directly, including the raw delta.
    // O(p-m) is NOT obtained by subtracting two decoded/FFT outputs.
    const auto om = observer::Observe(m, scale);
    CheckObservation(m, scale, om, roots, "m");
    const auto op = observer::Observe(p, scale);
    CheckObservation(p, scale, op, roots, "p");
    const auto od = observer::Observe(difference.polynomial, scale);
    CheckObservation(difference.polynomial, scale, od, roots, "raw_p_minus_m");

    Stage("componentwise_reconstruction");
    std::array<Maximum, 4> maxima{};
    Maximum reconstruction{}, linearity{}, legacyBridge{};
    const auto tupleAt = [&](std::size_t s, std::size_t component) {
        return Decompose(Part(z[s], component), Part(Value512(om, s), component),
                         Part(Value512(od, s), component), Part(Value512(op, s), component),
                         Part(Bridge(decoded.values[s]), component));
    };
    for (std::size_t s = 0; s < pf::kSlots; ++s) {
        const auto oldBridge = pf::FromClient(decoded.values[s]);
        const auto production = Bridge(decoded.values[s]);
        for (std::size_t c = 0; c < 2; ++c) {
            const Tuple t = tupleAt(s, c);
            const std::array<Real, 4> terms{{t.encoding, t.encryption, t.decoding, t.total}};
            for (std::size_t term = 0; term < terms.size(); ++term) Update(maxima[term], terms[term], s, c);
            Update(reconstruction, t.residual, s, c);
            const Real linear = (Part(Value512(op, s), c) - Part(Value512(om, s), c)) -
                                Part(Value512(od, s), c);
            Update(linearity, linear, s, c);
            Update(legacyBridge, Part(production, c) - Part(oldBridge, c), s, c);
        }
    }
    Check(reconstruction.magnitude <= Tolerance(), "THREE_TERM_RECONSTRUCTION");
    Check(linearity.magnitude <= Tolerance(), "INDEPENDENT_EMBEDDING_LINEARITY");
    Check(legacyBridge.magnitude <= Tolerance(), "LEGACY_E0_BRIDGE_DISAGREEMENT");
    const std::array<const char*, 4> names{{"encoding", "encryption", "decoding", "E0"}};
    for (std::size_t term = 0; term < maxima.size(); ++term) {
        const auto& location = maxima[term];
        std::cout << "S100 maximum=" << names[term] << " value=" << location.magnitude
                  << " slot=" << location.slot << " component=" << (location.component == 0 ? "real" : "imag")
                  << '\n';
        EmitTuple(names[term], location.slot, location.component,
                  Part(z[location.slot], location.component), tupleAt(location.slot, location.component));
    }
    for (std::size_t s : pf::kAnchors) for (std::size_t c = 0; c < 2; ++c)
        EmitTuple("anchor", s, c, Part(z[s], c), tupleAt(s, c));
    Scalar("same_component_reconstruction_max", reconstruction.magnitude);
    Scalar("independent_linearity_max", linearity.magnitude);
    Scalar("legacy_decimal100_bridge_max", legacyBridge.magnitude);
    Scalar("production_internal_cross_precision", codecCross);
    Scalar("agreement_absolute_tolerance", Tolerance());
    const Real conditionalBound = Real(pf::kN) / (Real(2) * pf::R(scale.numerator));
    Check(conditionalBound == pf::Pow2(-86), "CONDITIONAL_ROUNDING_BOUND_ARITHMETIC");
    Scalar("conditional_nearest_coefficient_rounding_component_bound", conditionalBound);
    std::cout << "S100 rounding_bound_assumptions=exact_canonical_inverse_and_correct_nearest_rounding"
              << " established_by_this_run=NO\n";
    std::cout << "S100 lift_check=RAW_INTEGER_DIFFERENCE_AND_SUFFICIENT_HEADROOM"
              << " positive=" << (difference.minimumLiftHeadroom > 0 ? 1 : 0)
              << " hidden_sampler_wrap_claim=NONE\n";
    Check(std::cout.good(), "OUTPUT_STREAM_FAILURE");
    std::cout << "S100 status=COMPLETE mode=fresh public_encryptions=1 slots=" << pf::kSlots
              << " components=" << 2 * pf::kSlots << " horner_anchors_per_polynomial=" << pf::kAnchors.size()
              << " original_S100_E80=NOT_RERUN prior_FAIL=RETAINED precision_claim=NONE\n" << std::flush;
}
} // namespace

int main(int argc, char** argv) {
    try {
        Check(argc == 2, "EXPECTED_CONTROLS_OR_FRESH_ARGUMENT");
        const std::string mode(argv[1]);
        Check(mode == "--controls" || mode == "--fresh", "UNKNOWN_MODE");
        const char* threads = std::getenv("OMP_NUM_THREADS");
        Check(threads != nullptr && std::string(threads) == "2", "REQUIRE_OMP_NUM_THREADS_2");
        std::cout << std::scientific << std::setprecision(170);
        std::cout << "S100 schema=1 task=S100-FRESH-ERROR-REPAIR-01 mode=" << mode
                  << " source=" << S100_FRESH_ERROR_SOURCE_COMMIT
                  << " expected_openfhe_source=df495ba2e91739a6dc8f1de254fc5a41155ce504"
                  << " boost=" << BOOST_LIB_VERSION << " omp_num_threads=2\n";
#if defined(_MSC_VER)
        std::cout << "S100 compiler=msvc version=" << _MSC_VER << '\n';
#elif defined(__clang__)
        std::cout << "S100 compiler=clang version=" << __clang_major__ << '.' << __clang_minor__ << '\n';
#elif defined(__GNUC__)
        std::cout << "S100 compiler=gcc version=" << __GNUC__ << '.' << __GNUC_MINOR__ << '\n';
#endif
        if (mode == "--controls") Controls(); else Fresh();
        return 0; // valid diagnostic ONLY, not original S100 precision acceptance
    }
    catch (const Invalid& error) {
        std::cerr << "S100 status=INVALID stage=" << phase << " reason=" << error.what() << '\n';
    }
    catch (const observer::EndpointFailure& error) {
        std::cerr << "S100 status=INVALID stage=" << phase << " reason=" << error.Reason() << '\n';
    }
    catch (const std::exception&) {
        // Do not print arbitrary upstream exception payloads containing key data.
        std::cerr << "S100 status=INVALID stage=" << phase << " reason=EXTERNAL_EXCEPTION\n";
    }
    catch (...) {
        std::cerr << "S100 status=INVALID stage=" << phase << " reason=UNKNOWN_EXCEPTION\n";
    }
    return 2;
}
