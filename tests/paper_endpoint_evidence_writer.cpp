#include "paper_endpoint_evidence_writer.h"

#include <boost/math/special_functions/fpclassify.hpp>

#include <array>
#include <fstream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

namespace paper_endpoint_contract {
namespace {

constexpr std::uintmax_t kMaximumBytes = 16U * 1024U * 1024U;
constexpr std::size_t kMaximumLineBytes = 32768;
constexpr std::size_t kRowCount = 16384;
constexpr std::size_t kMetadataCount = 43;
constexpr std::size_t kCheckCount = 24;
constexpr std::size_t kMaximumIntegerBitsBeforeText = 110000;
constexpr char kHeader[] = "#fs-residual-endpoint-01.v1-r1";
constexpr char kRowHeader[] = "slot\tE0.real\tE0.imag\tE8.real\tE8.imag";
constexpr char kBaseline[] = "9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e";
constexpr char kProduction[] = "b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89";
constexpr char kOpenFhe[] = "df495ba2e91739a6dc8f1de254fc5a41155ce504";
constexpr char kAcceptedRed[] = "2fe655d493dcde5f05aa1515f41ca6823bba30bd";

constexpr std::array<const char*, kCheckCount> kCheckIds{{
    "control.constant.512", "control.constant.768", "control.x.512",
    "control.x.768", "control.xNminus1.512", "control.xNminus1.768",
    "control.sparse.512", "control.sparse.768", "fresh.cross", "terminal.cross",
    "fresh.horner.512", "fresh.horner.768", "terminal.horner.512",
    "terminal.horner.768", "fresh.producer.512", "fresh.producer.768",
    "terminal.producer.512", "terminal.producer.768", "residual.E0.cross",
    "residual.E8.cross", "residual.I8.cross", "residual.A8.cross",
    "identity.512", "identity.768"}};
constexpr std::array<const char*, 4> kResidualIds{{"E0", "E8", "I8", "A8"}};

[[noreturn]] void Stop(const char* reason, const std::string& detail) {
    FailEndpoint(reason, "writer " + detail);
}

[[noreturn]] void Integrity(const std::string& detail) {
    Stop("INTEGRITY", detail);
}

[[noreturn]] void Format(const std::string& detail) {
    Stop("FORMAT", detail);
}

[[noreturn]] void Identity(const std::string& detail) {
    Stop("IDENTITY", detail);
}

[[noreturn]] void Io(const std::string& detail) {
    Stop("IO_ERROR", detail);
}

[[noreturn]] void Nonfinite(const std::string& detail) {
    Stop("NONFINITE", detail);
}

[[noreturn]] void Conditioning(const std::string& detail) {
    Stop("CONDITIONING", detail);
}

[[noreturn]] void EstimatorCeiling(const std::string& detail) {
    Stop("ESTIMATOR_CEILING", detail);
}

Int AbsInt(const Int& value) {
    return value < 0 ? Int(-value) : value;
}

Rational Reduced(Int numerator, Int denominator) {
    if (denominator <= 0)
        Integrity("rational denominator must be positive");
    const Int divisor = paper_full_test::Gcd(AbsInt(numerator), denominator);
    return {numerator / divisor, denominator / divisor};
}

bool Equal(const Rational& left, const Rational& right) {
    return left.numerator == right.numerator && left.denominator == right.denominator;
}

bool Greater(const Rational& left, const Rational& right) {
    return left.numerator * right.denominator > right.numerator * left.denominator;
}

Rational Add(const Rational& left, const Rational& right) {
    return Reduced(left.numerator * right.denominator +
                       right.numerator * left.denominator,
                   left.denominator * right.denominator);
}

Rational Subtract(const Rational& left, const Rational& right) {
    return Reduced(left.numerator * right.denominator -
                       right.numerator * left.denominator,
                   left.denominator * right.denominator);
}

Rational Times(const Rational& left, const Rational& right) {
    return Reduced(left.numerator * right.numerator,
                   left.denominator * right.denominator);
}

Rational PowerTwo(int exponent) {
    if (exponent >= 0)
        return {Int(1) << static_cast<unsigned>(exponent), Int(1)};
    const auto shift = static_cast<unsigned>(-(static_cast<long long>(exponent)));
    return {Int(1), Int(1) << shift};
}

std::size_t BitLength(const Int& value) {
    return value == 0 ? 0U :
        static_cast<std::size_t>(boost::multiprecision::msb(AbsInt(value))) + 1U;
}

void ValidateIntegerTextBound(const Int& value, const char* field) {
    if (BitLength(value) > kMaximumIntegerBitsBeforeText)
        Format(std::string(field) + " exceeds the bounded textual envelope");
}

std::string UnsignedText(const Int& value, const char* field) {
    if (value < 0)
        Integrity(std::string(field) + " must be nonnegative");
    ValidateIntegerTextBound(value, field);
    return value.convert_to<std::string>();
}

void ValidateRational(const Rational& value, const char* field) {
    if (value.numerator < 0 || value.denominator <= 0)
        Integrity(std::string(field) + " must be nonnegative and have a positive denominator");
    ValidateIntegerTextBound(value.numerator, field);
    ValidateIntegerTextBound(value.denominator, field);
    if (paper_full_test::Gcd(value.numerator, value.denominator) != 1)
        Integrity(std::string(field) + " must be reduced");
}

void ValidateScale(const Scale& value, const char* field) {
    if (value.numerator <= 0 || value.denominator <= 0 ||
        paper_full_test::Gcd(value.numerator, value.denominator) != 1)
        Integrity(std::string(field) + " must be a positive reduced scale");
    ValidateIntegerTextBound(value.numerator, field);
    ValidateIntegerTextBound(value.denominator, field);
}

bool IsLowerHex40(std::string_view value) {
    if (value.size() != 40)
        return false;
    for (const char c : value)
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')))
            return false;
    return true;
}

bool IsPositiveCanonicalInteger(std::string_view value) {
    if (value.empty() || value.front() == '0')
        return false;
    for (const char c : value)
        if (c < '0' || c > '9')
            return false;
    return true;
}

void ValidateIdentity(const EndpointEvidenceIdentity& identity) {
    if (identity.scope != "live-single-chain" && identity.scope != "synthetic")
        Identity("scope must be live-single-chain or synthetic");
    if (!IsLowerHex40(identity.sourceCommit))
        Identity("source commit must be forty lowercase hexadecimal bytes");
    if (identity.scope == "live-single-chain" &&
        (identity.sourceCommit == kBaseline || identity.sourceCommit == kAcceptedRed))
        Identity("live source commit must be the actual new compiled source");
    if (identity.host != "linux" && identity.host != "windows")
        Identity("host must be linux or windows");
    if (!IsPositiveCanonicalInteger(identity.githubRunId) ||
        !IsPositiveCanonicalInteger(identity.githubRunAttempt))
        Identity("run id and attempt must be positive canonical decimals");
    if (identity.boostVersion == 0)
        Identity("BOOST_VERSION must be the actual positive value");
}

void ValidateBoundary(const EndpointPublicationBoundary& boundary) {
    if (!boundary.ownerCleanupConfirmed)
        Integrity("owner cleanup confirmation is required before publication");
}

std::array<Scale, 9> ExactScales() {
    const auto closed = paper_full_test::Scales();
    std::array<Scale, 9> recursive;
    recursive[0] = {Int(1) << 100, Int(1)};
    for (std::size_t index = 1; index < recursive.size(); ++index) {
        const Int denominator = recursive[index - 1].denominator *
                                recursive[index - 1].denominator *
                                paper_full_test::kDiv * paper_full_test::kQ[10 - index];
        const Int numerator = recursive[index - 1].numerator *
                              recursive[index - 1].numerator;
        const Int divisor = paper_full_test::Gcd(numerator, denominator);
        recursive[index] = {numerator / divisor, denominator / divisor};
        if (recursive[index].numerator != closed[index].numerator ||
            recursive[index].denominator != closed[index].denominator)
            Integrity("recursive and closed-form exact scales disagree");
    }
    return recursive;
}

Rational NormBound(const Int& coefficientOneNorm, const Scale& scale,
                   int offset) {
    const auto exponent = ScaledOneNormExponent(coefficientOneNorm, scale);
    if (!exponent)
        return {Int(0), Int(1)};
    const long long combined = static_cast<long long>(*exponent) + offset;
    if (combined < -400000 || combined > 400000)
        Stop("MODEL_UNSUPPORTED", "allowance exponent outside checked range");
    return PowerTwo(static_cast<int>(combined));
}

struct Bounds final {
    Rational fresh;
    Rational terminal;
    Rational hornerFresh;
    Rational hornerTerminal;
    Rational power;
    Rational propagated;
    Rational subtraction;
    std::array<Rational, 4> residual;
    Rational identity;
};

Bounds MakeBounds(unsigned bits, const EndpointEvidence& evidence) {
    Bounds result;
    result.fresh = NormBound(evidence.freshCoefficientOneNorm,
                             evidence.freshScale, 12 - static_cast<int>(bits));
    result.terminal = NormBound(evidence.terminalCoefficientOneNorm,
                                evidence.terminalScale, 12 - static_cast<int>(bits));
    result.hornerFresh = NormBound(evidence.freshCoefficientOneNorm,
                                   evidence.freshScale, 24 - static_cast<int>(bits));
    result.hornerTerminal = NormBound(evidence.terminalCoefficientOneNorm,
                                      evidence.terminalScale, 24 - static_cast<int>(bits));
    result.power = PowerTwo(270 - static_cast<int>(bits));
    result.propagated = Add(result.power, Times(PowerTwo(263), result.fresh));
    result.subtraction = PowerTwo(264 - static_cast<int>(bits));
    result.residual = {{Add(result.fresh, result.subtraction),
                        Add(Add(result.terminal, result.power), result.subtraction),
                        Add(Add(result.propagated, result.power), result.subtraction),
                        Add(Add(result.terminal, result.propagated), result.subtraction)}};
    result.identity = Add(
        Add(Add(result.residual[1], result.residual[2]), result.residual[3]),
        Add(result.subtraction, result.subtraction));
    return result;
}

std::array<Rational, kCheckCount> ExpectedAllowances(const EndpointEvidence& evidence) {
    std::array<Rational, kCheckCount> result;
    std::size_t position = 0;
    const std::array<int, 4> controlExponent{{0, 0, 0, 3}};
    for (const int exponent : controlExponent) {
        const auto direct = PowerTwo(exponent + 10 - 768);
        result[position++] = Add(PowerTwo(exponent + 12 - 512), direct);
        result[position++] = Add(PowerTwo(exponent + 12 - 768), direct);
    }
    const auto b512 = MakeBounds(512, evidence);
    const auto b768 = MakeBounds(768, evidence);
    result[position++] = Add(b512.fresh, b768.fresh);
    result[position++] = Add(b512.terminal, b768.terminal);
    result[position++] = Add(b512.fresh, b512.hornerFresh);
    result[position++] = Add(b768.fresh, b512.hornerFresh);
    result[position++] = Add(b512.terminal, b512.hornerTerminal);
    result[position++] = Add(b768.terminal, b512.hornerTerminal);
    const auto transport = PowerTwo(-300);
    result[position++] = Add(b512.fresh, transport);
    result[position++] = Add(b768.fresh, transport);
    result[position++] = Add(b512.terminal, transport);
    result[position++] = Add(b768.terminal, transport);
    for (std::size_t index = 0; index < 4; ++index)
        result[position++] = Add(b512.residual[index], b768.residual[index]);
    result[position++] = b512.identity;
    result[position++] = b768.identity;
    if (position != result.size())
        Integrity("internal check allowance count");
    return result;
}

bool EqualComplex(const Complex<768>& left, const Complex<768>& right) {
    return left.real == right.real && left.imag == right.imag;
}

Binary768 Absolute(const Binary768& value) {
    if (!boost::math::isfinite(value))
        Nonfinite("evidence contains a nonfinite component");
    return value < 0 ? Binary768(-value) : value;
}

std::string CanonicalForEvidence(const Binary768& value) {
    if (!boost::math::isfinite(value))
        Nonfinite("canonical evidence component");
    try {
        return CanonicalDecimal(value);
    }
    catch (const std::invalid_argument&) {
        Format("canonical evidence component exponent is outside the v1-r1 schema");
    }
}

std::pair<std::size_t, bool> ExactMaximumLocation(
    const std::vector<Complex<768>>& values, Binary768& magnitude) {
    magnitude = 0;
    std::pair<std::size_t, bool> location{0, false};
    for (std::size_t slot = 0; slot < values.size(); ++slot) {
        const std::array<Binary768, 2> components{{values[slot].real, values[slot].imag}};
        for (std::size_t component = 0; component < components.size(); ++component) {
            const auto candidate = Absolute(components[component]);
            if (candidate > magnitude) {
                magnitude = candidate;
                location = {slot, component == 1};
            }
        }
    }
    return location;
}

void ValidateEvidence(const EndpointEvidence& evidence,
                      const EndpointEvidenceIdentity& identity,
                      const EndpointPublicationBoundary& boundary) {
    ValidateIdentity(identity);
    ValidateBoundary(boundary);
    ValidateScale(evidence.freshScale, "fresh scale");
    ValidateScale(evidence.terminalScale, "terminal scale");
    const auto scales = ExactScales();
    if (evidence.freshScale.numerator != scales.front().numerator ||
        evidence.freshScale.denominator != scales.front().denominator ||
        evidence.terminalScale.numerator != scales.back().numerator ||
        evidence.terminalScale.denominator != scales.back().denominator)
        Integrity("endpoint evidence must carry exact frozen S0 and S8");
    if (evidence.freshCoefficientOneNorm < 0 || evidence.terminalCoefficientOneNorm < 0)
        Integrity("coefficient one-norms must be nonnegative");
    ValidateIntegerTextBound(evidence.freshCoefficientOneNorm, "fresh coefficient one-norm");
    ValidateIntegerTextBound(evidence.terminalCoefficientOneNorm, "terminal coefficient one-norm");
    ValidateRational(evidence.freshMaximumOneNorm, "fresh maximum one-norm");
    ValidateRational(evidence.terminalMaximumOneNorm, "terminal maximum one-norm");
    const Rational radius{Int(5), Int(4)};
    if (Greater(evidence.freshMaximumOneNorm, radius) ||
        Greater(evidence.terminalMaximumOneNorm, radius))
        Conditioning("endpoint represented one-norm exceeds 5/4");
    if (evidence.freshErrors.size() != kRowCount ||
        evidence.terminalErrors.size() != kRowCount)
        Integrity("endpoint evidence requires exactly 16384 E0 and E8 rows");

    const auto allowances = ExpectedAllowances(evidence);
    if (evidence.checks.size() != kCheckCount)
        Integrity("endpoint evidence requires exactly 24 checks");
    const Rational ceiling = PowerTwo(-128);
    for (std::size_t index = 0; index < evidence.checks.size(); ++index) {
        const auto& check = evidence.checks[index];
        if (check.id != kCheckIds[index])
            Integrity("comparison receipt identity/order mismatch");
        ValidateRational(check.distance, "comparison distance");
        ValidateRational(check.allowance, "comparison allowance");
        if (!Equal(check.allowance, allowances[index]))
            Integrity("comparison allowance does not match the exact declared model");
        const Comparison kind = check.id.find(".producer.") == std::string::npos ?
            Comparison::TwoBoundedPaths : Comparison::Producer;
        const auto decision = AssessDifference(check.distance, check.allowance, kind, true);
        if (decision == Decision::Fail)
            Integrity("comparison receipt is a declared-model failure");
        if (Greater(check.allowance, ceiling))
            EstimatorCeiling("comparison allowance exceeds 2^-128");
        if (decision == Decision::Unresolved)
            Stop("MODEL_UNSUPPORTED", "comparison receipt overlaps the observer threshold");
        if (check.slot >= kRowCount)
            Integrity("comparison argmax slot is outside the full row domain");
        if (check.distance.numerator == 0 && (check.slot != 0 || check.imaginary))
            Integrity("zero comparison must use the least slot and real component");
        if (check.id.find(".horner.") != std::string::npos) {
            bool found = false;
            for (const auto anchor : paper_full_test::kAnchors)
                found = found || check.slot == anchor;
            if (!found)
                Integrity("Horner comparison argmax is not a frozen anchor");
        }
    }

    const auto b768 = MakeBounds(768, evidence);
    for (std::size_t index = 0; index < evidence.maxima.size(); ++index) {
        const auto& maximum = evidence.maxima[index];
        if (maximum.id != kResidualIds[index] || maximum.slot >= kRowCount)
            Integrity("residual maximum identity/shape/value");
        if (!boost::math::isfinite(maximum.magnitude))
            Nonfinite("residual maximum magnitude");
        if (maximum.magnitude < 0)
            Integrity("residual maximum magnitude is negative");
        ValidateRational(maximum.errorAllowance, "residual maximum allowance");
        if (!Equal(maximum.errorAllowance, b768.residual[index]))
            Integrity("residual maximum allowance mismatch");
        if (Greater(maximum.errorAllowance, ceiling))
            EstimatorCeiling("residual maximum allowance exceeds 2^-128");
        for (const auto& value : maximum.tuple) {
            (void)Absolute(value.real);
            (void)Absolute(value.imag);
        }
        const auto& selected = maximum.imaginary ? maximum.tuple[index].imag :
                                                   maximum.tuple[index].real;
        if (Absolute(selected) != maximum.magnitude)
            Integrity("residual maximum does not match its signed tuple component");
        if (!EqualComplex(maximum.tuple[0], evidence.freshErrors[maximum.slot]) ||
            !EqualComplex(maximum.tuple[1], evidence.terminalErrors[maximum.slot]))
            Integrity("residual maximum tuple does not match retained E0/E8 rows");
        if (maximum.magnitude == 0 && (maximum.slot != 0 || maximum.imaginary))
            Integrity("zero residual maximum must use slot zero and real component");
        if (index < 2) {
            Binary768 recomputedMagnitude;
            const auto location = ExactMaximumLocation(
                index == 0 ? evidence.freshErrors : evidence.terminalErrors,
                recomputedMagnitude);
            if (recomputedMagnitude != maximum.magnitude ||
                location.first != maximum.slot || location.second != maximum.imaginary)
                Integrity("retained E0/E8 maximum or tie policy mismatch");
        }
    }
}

std::string Stem(const EndpointEvidenceIdentity& identity) {
    const std::string prefix = identity.scope == "synthetic" ?
        "fs-endpoint-synthetic-" : "fs-residual-endpoint-01.v1-r1.";
    return prefix + identity.sourceCommit + "." + identity.host + "." +
           identity.githubRunId + "." + identity.githubRunAttempt;
}

std::string RationalFields(const Rational& value) {
    return UnsignedText(value.numerator, "rational numerator") + "\t" +
           UnsignedText(value.denominator, "rational denominator");
}

using Metadata = std::array<std::pair<std::string, std::string>, kMetadataCount>;

Metadata ExpectedMetadata(const EndpointEvidence& evidence,
                          const EndpointEvidenceIdentity& identity,
                          const EndpointPublicationBoundary& boundary) {
    const auto scales = ExactScales();
    const bool e80Pass = boundary.numericGateFailures == 0;
    return {{{"scope", identity.scope},
             {"source_commit", identity.sourceCommit},
             {"baseline_tested_source", kBaseline},
             {"production_source", kProduction},
             {"openfhe_pin", kOpenFhe},
             {"host", identity.host},
             {"github_run_id", identity.githubRunId},
             {"github_run_attempt", identity.githubRunAttempt},
             {"test_name", "paper_full_eight_square_contract"},
             {"chain_count", "1"},
             {"n", "32768"},
             {"m", "65536"},
             {"slots", "16384"},
             {"gap", "1"},
             {"input_formula", "frozen-four-phase-exact-dyadic-v1"},
             {"primary_precision_bits", "768"},
             {"check_precision_bits", "512"},
             {"significant_digits", "110"},
             {"scale0_numerator", UnsignedText(scales.front().numerator, "S0 numerator")},
             {"scale0_denominator", UnsignedText(scales.front().denominator, "S0 denominator")},
             {"scale8_numerator", UnsignedText(scales.back().numerator, "S8 numerator")},
             {"scale8_denominator", UnsignedText(scales.back().denominator, "S8 denominator")},
             {"coefficient_l1_fresh", UnsignedText(evidence.freshCoefficientOneNorm, "fresh C")},
             {"coefficient_l1_terminal", UnsignedText(evidence.terminalCoefficientOneNorm, "terminal C")},
             {"fresh_max_l1_numerator", UnsignedText(evidence.freshMaximumOneNorm.numerator, "fresh norm numerator")},
             {"fresh_max_l1_denominator", UnsignedText(evidence.freshMaximumOneNorm.denominator, "fresh norm denominator")},
             {"terminal_max_l1_numerator", UnsignedText(evidence.terminalMaximumOneNorm.numerator, "terminal norm numerator")},
             {"terminal_max_l1_denominator", UnsignedText(evidence.terminalMaximumOneNorm.denominator, "terminal norm denominator")},
             {"model", "conditional-binary-nearest-direct-trig8u-v1"},
             {"assurance", "CONDITIONAL"},
             {"boost_version", std::to_string(identity.boostVersion)},
             {"root_policy", "direct-own-precision-v1"},
             {"norm", "max-real-imag-component"},
             {"observer_tolerance", "2^-120"},
             {"estimator_ceiling", "2^-128"},
             {"original_error_gate", "2^-80"},
             {"rounding", "decimal-nearest-ties-even"},
             {"row_count", "16384"},
             {"check_count", "24"},
             {"numeric_gate_failures", std::to_string(boundary.numericGateFailures)},
             {"E80_disposition", e80Pass ? "PASS" : "FAIL"},
             {"A_disposition", "NOT_ADOPTED"},
             {"observer_disposition", "PASS"}}};
}

class BoundedWriter final {
public:
    explicit BoundedWriter(const std::filesystem::path& path)
        : output_(path, std::ios::binary | std::ios::out | std::ios::trunc) {
        if (!output_)
            Io("cannot create private staging candidate");
    }

    void Line(const std::string& line) {
        const auto bytes = static_cast<std::uintmax_t>(line.size()) + 1U;
        if (bytes > kMaximumLineBytes)
            Format("line exceeds 32768 bytes including LF");
        if (total_ > kMaximumBytes - bytes)
            Format("canonical file exceeds 16 MiB");
        output_.write(line.data(), static_cast<std::streamsize>(line.size()));
        output_.put('\n');
        if (!output_)
            Io("failed while writing private staging candidate");
        total_ += bytes;
    }

    std::uintmax_t Close() {
        output_.close();
        if (output_.fail())
            Io("failed to close private staging candidate");
        return total_;
    }

private:
    std::ofstream output_;
    std::uintmax_t total_ = 0;
};

void WriteCanonical(const std::filesystem::path& path,
                    const EndpointEvidence& evidence,
                    const EndpointEvidenceIdentity& identity,
                    const EndpointPublicationBoundary& boundary,
                    std::uintmax_t& bytes) {
    BoundedWriter writer(path);
    writer.Line(kHeader);
    for (const auto& field : ExpectedMetadata(evidence, identity, boundary))
        writer.Line("meta\t" + field.first + "\t" + field.second);
    for (const auto& check : evidence.checks) {
        writer.Line("check\t" + check.id + "\tPASS\t" +
                    RationalFields(check.distance) + "\t" +
                    RationalFields(check.allowance) + "\t" +
                    std::to_string(check.slot) + "\t" +
                    (check.imaginary ? "imag" : "real"));
    }
    writer.Line(kRowHeader);
    for (std::size_t slot = 0; slot < kRowCount; ++slot) {
        writer.Line(std::to_string(slot) + "\t" +
                    CanonicalForEvidence(evidence.freshErrors[slot].real) + "\t" +
                    CanonicalForEvidence(evidence.freshErrors[slot].imag) + "\t" +
                    CanonicalForEvidence(evidence.terminalErrors[slot].real) + "\t" +
                    CanonicalForEvidence(evidence.terminalErrors[slot].imag));
    }
    bytes = writer.Close();
}

std::vector<std::string_view> SplitTabs(const std::string& line) {
    std::vector<std::string_view> fields;
    std::size_t begin = 0;
    for (;;) {
        const auto tab = line.find('\t', begin);
        if (tab == std::string::npos) {
            fields.emplace_back(line.data() + begin, line.size() - begin);
            return fields;
        }
        fields.emplace_back(line.data() + begin, tab - begin);
        begin = tab + 1;
    }
}

Int ParseUnsigned(std::string_view text) {
    if (text.empty() || (text.size() > 1 && text.front() == '0'))
        Format("noncanonical unsigned integer");
    Int result = 0;
    for (const char c : text) {
        if (c < '0' || c > '9')
            Format("noncanonical unsigned integer");
        result *= 10;
        result += c - '0';
    }
    ValidateIntegerTextBound(result, "parsed unsigned integer");
    return result;
}

Rational ParseRational(std::string_view numerator, std::string_view denominator) {
    const Rational result{ParseUnsigned(numerator), ParseUnsigned(denominator)};
    if (result.denominator <= 0 ||
        paper_full_test::Gcd(result.numerator, result.denominator) != 1)
        Format("parsed rational must be reduced with a positive denominator");
    return result;
}

class BoundedReader final {
public:
    explicit BoundedReader(const std::filesystem::path& path)
        : input_(path, std::ios::binary | std::ios::in) {
        if (!input_)
            Io("cannot reopen canonical candidate");
    }

    bool Line(std::string& line) {
        line.clear();
        char value = 0;
        while (input_.get(value)) {
            ++total_;
            if (total_ > kMaximumBytes)
                Format("canonical file exceeds 16 MiB");
            if (value == '\n')
                return true;
            const auto byte = static_cast<unsigned char>(value);
            if (value == '\r' || value == '\0' || byte > 0x7f)
                Format("canonical file must be ASCII LF without CR, NUL, or BOM");
            if (line.size() + 2U > kMaximumLineBytes)
                Format("line exceeds 32768 bytes including LF");
            line.push_back(value);
        }
        if (!input_.eof())
            Io("failed while reopening canonical candidate");
        if (!line.empty())
            Format("canonical file lacks final LF");
        return false;
    }

    std::uintmax_t Bytes() const { return total_; }

private:
    std::ifstream input_;
    std::uintmax_t total_ = 0;
};

void RequireLine(BoundedReader& reader, std::string& line, const char* detail) {
    if (!reader.Line(line) || line.empty())
        Format(detail);
}

bool IsIdentityMetadataKey(std::string_view key) {
    return key == "scope" || key == "source_commit" ||
           key == "baseline_tested_source" || key == "production_source" ||
           key == "openfhe_pin" || key == "host" || key == "github_run_id" ||
           key == "github_run_attempt" || key == "test_name" ||
           key == "boost_version";
}

void ValidateCanonicalInternal(
    const std::filesystem::path& path,
    const EndpointEvidence& evidence,
    const EndpointEvidenceIdentity& identity,
    const EndpointPublicationBoundary& boundary) {
    const auto status = std::filesystem::symlink_status(path);
    if (status.type() == std::filesystem::file_type::not_found)
        Io("canonical candidate is missing");
    if (std::filesystem::is_symlink(status) || !std::filesystem::is_regular_file(status))
        Identity("canonical candidate must be a regular non-symlink file");
    const auto physicalBytes = std::filesystem::file_size(path);
    if (physicalBytes == 0 || physicalBytes > kMaximumBytes)
        Format("canonical file byte limit");

    BoundedReader reader(path);
    std::string line;
    RequireLine(reader, line, "missing canonical header");
    if (line != kHeader)
        Format("canonical header mismatch");

    const auto metadata = ExpectedMetadata(evidence, identity, boundary);
    for (const auto& expected : metadata) {
        RequireLine(reader, line, "missing ordered metadata");
        const auto fields = SplitTabs(line);
        const std::string_view expectedKey(expected.first.data(), expected.first.size());
        const std::string_view expectedValue(expected.second.data(), expected.second.size());
        if (fields.size() != 3 || fields[0] != "meta" || fields[1] != expectedKey)
            Format("ordered metadata schema/value mismatch");
        if (fields[2] != expectedValue) {
            if (IsIdentityMetadataKey(expectedKey))
                Identity("canonical metadata identity mismatch: " + expected.first);
            Integrity("canonical metadata semantic mismatch: " + expected.first);
        }
    }

    const auto allowances = ExpectedAllowances(evidence);
    for (std::size_t index = 0; index < evidence.checks.size(); ++index) {
        RequireLine(reader, line, "missing ordered comparison receipt");
        const auto fields = SplitTabs(line);
        if (fields.size() != 9 || fields[0] != "check" || fields[1] != kCheckIds[index] ||
            fields[2] != "PASS")
            Format("ordered comparison schema mismatch");
        const auto distance = ParseRational(fields[3], fields[4]);
        const auto allowance = ParseRational(fields[5], fields[6]);
        const auto& expected = evidence.checks[index];
        if (!Equal(distance, expected.distance) || !Equal(allowance, expected.allowance) ||
            !Equal(allowance, allowances[index]))
            Integrity("comparison rational mismatch");
        const auto expectedSlot = std::to_string(expected.slot);
        if (fields[7] != std::string_view(expectedSlot.data(), expectedSlot.size()) ||
            fields[8] != (expected.imaginary ? "imag" : "real"))
            Integrity("comparison argmax mismatch");
    }

    RequireLine(reader, line, "missing slot header");
    if (line != kRowHeader)
        Format("slot header mismatch");
    for (std::size_t slot = 0; slot < kRowCount; ++slot) {
        RequireLine(reader, line, "missing canonical slot row");
        const auto fields = SplitTabs(line);
        const auto expectedSlot = std::to_string(slot);
        if (fields.size() != 5 ||
            fields[0] != std::string_view(expectedSlot.data(), expectedSlot.size()))
            Format("ordered slot row mismatch");
        const std::array<const Binary768*, 4> expected{{
            &evidence.freshErrors[slot].real, &evidence.freshErrors[slot].imag,
            &evidence.terminalErrors[slot].real, &evidence.terminalErrors[slot].imag}};
        for (std::size_t component = 0; component < expected.size(); ++component) {
            if (!IsCanonicalDecimal(fields[component + 1]))
                Format("noncanonical residual decimal");
            const auto expectedDecimal = CanonicalForEvidence(*expected[component]);
            if (fields[component + 1] !=
                std::string_view(expectedDecimal.data(), expectedDecimal.size()))
                Integrity("residual row does not bind to the supplied pure evidence value");
        }
    }
    if (reader.Line(line))
        Format("extra canonical line");
    if (reader.Bytes() != physicalBytes)
        Format("canonical file changed while validating");
}

bool HasForbiddenLexicalComponent(const std::filesystem::path& path) {
    for (const auto& component : path)
        if (component == "." || component == "..")
            return true;
    return false;
}

void ValidateTrustedParent(const std::filesystem::path& path) {
    if (path.empty() || !path.is_absolute() || path != path.lexically_normal() ||
        HasForbiddenLexicalComponent(path))
        Identity("exclusive trusted parent must be absolute and lexically normalized");
    const auto status = std::filesystem::symlink_status(path);
    if (std::filesystem::is_symlink(status) || !std::filesystem::is_directory(status))
        Identity("exclusive trusted parent must be an existing non-symlink directory");
    for (auto cursor = path; !cursor.empty();) {
        if (std::filesystem::is_symlink(std::filesystem::symlink_status(cursor)))
            Identity("exclusive trusted parent may not have symlink ancestors");
        const auto parent = cursor.parent_path();
        if (parent == cursor)
            break;
        cursor = parent;
    }
}

Int Pow10(unsigned exponent) {
    Int result = 1;
    Int factor = 10;
    while (exponent != 0) {
        if ((exponent & 1U) != 0)
            result *= factor;
        exponent >>= 1U;
        if (exponent != 0)
            factor *= factor;
    }
    return result;
}

Rational DecimalQuantum(const std::string& canonical) {
    if (!IsCanonicalDecimal(canonical))
        Integrity("cannot derive a quantum from a noncanonical decimal");
    if (canonical[1] == '0')
        return {Int(0), Int(1)};
    int exponent = 0;
    for (std::size_t index = 114; index < 119; ++index)
        exponent = exponent * 10 + (canonical[index] - '0');
    if (canonical[113] == '-')
        exponent = -exponent;
    const int power = exponent - 110;
    if (power < -32760 || power > 32760)
        Format("serialization quantum exceeds the bounded primary-record envelope");
    return power >= 0 ? Reduced(Int(5) * Pow10(static_cast<unsigned>(power)), Int(1)) :
                        Reduced(Int(5), Pow10(static_cast<unsigned>(-power)));
}

std::string NamedRational(const char* name, const Rational& value) {
    return std::string("\t") + name + "_num=" +
           UnsignedText(value.numerator, name) + "\t" + name + "_den=" +
           UnsignedText(value.denominator, name);
}

void CheckOutput(std::ostream& output) {
    if (!output)
        Io("primary diagnostic stream rejected a record");
}

} // namespace

EndpointEvidenceFile WriteEndpointEvidence(
    const EndpointEvidence& evidence,
    const EndpointEvidenceIdentity& identity,
    const EndpointPublicationBoundary& boundary,
    const std::filesystem::path& exclusiveTrustedParent) {
    ValidateEvidence(evidence, identity, boundary);
    try {
        ValidateTrustedParent(exclusiveTrustedParent);
        const auto stem = Stem(identity);
        const auto identityDirectory = exclusiveTrustedParent / stem;
        if (std::filesystem::exists(std::filesystem::symlink_status(identityDirectory)))
            Identity("identity publication directory already exists");
        if (!std::filesystem::create_directory(identityDirectory))
            Io("could not claim the previously absent identity directory");
        std::filesystem::permissions(identityDirectory,
            std::filesystem::perms::owner_all, std::filesystem::perm_options::replace);

        const auto stagingDirectory = identityDirectory / ".staging";
        if (!std::filesystem::create_directory(stagingDirectory))
            Io("could not create the private staging directory");
        std::filesystem::permissions(stagingDirectory,
            std::filesystem::perms::owner_all, std::filesystem::perm_options::replace);
        const auto candidate = stagingDirectory / ".candidate.tsv";
        const auto ready = identityDirectory / (stem + ".tsv");
        if (std::filesystem::exists(std::filesystem::symlink_status(candidate)) ||
            std::filesystem::exists(std::filesystem::symlink_status(ready)))
            Identity("candidate or ready destination unexpectedly exists");

        std::uintmax_t writtenBytes = 0;
        WriteCanonical(candidate, evidence, identity, boundary, writtenBytes);
        ValidateCanonicalInternal(candidate, evidence, identity, boundary);
        if (std::filesystem::exists(std::filesystem::symlink_status(ready)))
            Identity("ready destination appeared before rename");
        std::filesystem::rename(candidate, ready);
        if (!std::filesystem::remove(stagingDirectory))
            Io("private staging directory was not empty after rename");
        const auto readyBytes = std::filesystem::file_size(ready);
        if (readyBytes != writtenBytes)
            Integrity("ready byte count changed across rename");
        return {ready, readyBytes};
    }
    catch (const std::filesystem::filesystem_error& error) {
        Io(error.code().message());
    }
}

void ValidateEndpointEvidenceFile(
    const std::filesystem::path& path,
    const EndpointEvidence& expectedEvidence,
    const EndpointEvidenceIdentity& expectedIdentity,
    const EndpointPublicationBoundary& expectedBoundary) {
    ValidateEvidence(expectedEvidence, expectedIdentity, expectedBoundary);
    try {
        ValidateCanonicalInternal(path, expectedEvidence, expectedIdentity, expectedBoundary);
    }
    catch (const std::filesystem::filesystem_error& error) {
        Io(error.code().message());
    }
}

void EmitEndpointEvidencePrimary(
    std::ostream& output,
    const EndpointEvidence& evidence,
    const EndpointEvidenceIdentity& identity,
    const EndpointPublicationBoundary& boundary) {
    ValidateEvidence(evidence, identity, boundary);
    try {
    output << "FS_ENDPOINT_BEGIN\tschema=fs-residual-endpoint-primary-v1-r1"
           << "\tscope=" << identity.scope
           << "\tsource_commit=" << identity.sourceCommit
           << "\thost=" << identity.host
           << "\tgithub_run_id=" << identity.githubRunId
           << "\tgithub_run_attempt=" << identity.githubRunAttempt
           << "\tboost_version=" << identity.boostVersion << '\n';
    const auto scales = ExactScales();
    for (std::size_t index = 0; index < scales.size(); ++index)
        output << "FS_ENDPOINT_SCALE\tindex=" << index
               << "\tnumerator=" << UnsignedText(scales[index].numerator, "scale numerator")
               << "\tdenominator=" << UnsignedText(scales[index].denominator, "scale denominator")
               << '\n';
    for (const auto& check : evidence.checks)
        output << "FS_ENDPOINT_CHECK\tid=" << check.id << "\tresult=PASS"
               << NamedRational("distance", check.distance)
               << NamedRational("allowance", check.allowance)
               << "\targmax_slot=" << check.slot
               << "\targmax_component=" << (check.imaginary ? "imag" : "real") << '\n';

    for (const auto& maximum : evidence.maxima) {
        const auto magnitudeText = CanonicalForEvidence(maximum.magnitude);
        const auto magnitudeExact = ExactAbsoluteDifference(maximum.magnitude, Binary768(0));
        const auto magnitudeQuantum = DecimalQuantum(magnitudeText);
        const auto lower = Greater(maximum.errorAllowance, magnitudeExact) ?
            Rational{Int(0), Int(1)} : Subtract(magnitudeExact, maximum.errorAllowance);
        const auto upper = Add(magnitudeExact, maximum.errorAllowance);
        output << "FS_ENDPOINT_MAX\tid=" << maximum.id
               << "\tmagnitude=" << magnitudeText
               << NamedRational("magnitude_exact", magnitudeExact)
               << NamedRational("magnitude_quantum", magnitudeQuantum)
               << NamedRational("allowance", maximum.errorAllowance)
               << NamedRational("interval_lower", lower)
               << NamedRational("interval_upper", upper)
               << "\targmax_slot=" << maximum.slot
               << "\targmax_component=" << (maximum.imaginary ? "imag" : "real");
        for (std::size_t residual = 0; residual < maximum.tuple.size(); ++residual) {
            const std::array<const Binary768*, 2> components{{
                &maximum.tuple[residual].real, &maximum.tuple[residual].imag}};
            for (std::size_t component = 0; component < components.size(); ++component) {
                const std::string field = std::string(kResidualIds[residual]) + "." +
                                          (component == 0 ? "real" : "imag");
                const auto text = CanonicalForEvidence(*components[component]);
                const auto quantum = DecimalQuantum(text);
                output << "\t" << field << "=" << text
                       << "\t" << field << "_q_num="
                       << UnsignedText(quantum.numerator, "tuple quantum numerator")
                       << "\t" << field << "_q_den="
                       << UnsignedText(quantum.denominator, "tuple quantum denominator");
            }
        }
        output << '\n';
    }
    output << "FS_ENDPOINT_COMPLETE\tresult=PASS\tassurance=CONDITIONAL"
           << "\trow_count=16384\tcheck_count=24"
           << "\tnumeric_gate_failures=" << boundary.numericGateFailures
           << "\tE80_disposition=" << (boundary.numericGateFailures == 0 ? "PASS" : "FAIL")
           << "\tA_disposition=NOT_ADOPTED\towner_cleanup_confirmed=true\n";
    CheckOutput(output);
    }
    catch (const std::ios_base::failure& error) {
        Io(error.code().message());
    }
}

} // namespace paper_endpoint_contract
