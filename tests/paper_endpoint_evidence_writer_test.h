#ifndef PAPER_ENDPOINT_EVIDENCE_WRITER_TEST_H
#define PAPER_ENDPOINT_EVIDENCE_WRITER_TEST_H

#include "paper_endpoint_evidence_writer.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace paper_endpoint_contract::synthetic {
namespace evidence_writer_test {

inline Rational ReducedForFixture(Int numerator, Int denominator) {
    const Int divisor = paper_full_test::Gcd(numerator < 0 ? -numerator : numerator,
                                              denominator);
    return {numerator / divisor, denominator / divisor};
}

inline Rational AddForFixture(const Rational& left, const Rational& right) {
    return ReducedForFixture(left.numerator * right.denominator +
                                 right.numerator * left.denominator,
                             left.denominator * right.denominator);
}

inline Rational Pow2ForFixture(int exponent) {
    if (exponent >= 0)
        return {Int(1) << static_cast<unsigned>(exponent), Int(1)};
    return {Int(1), Int(1) << static_cast<unsigned>(-exponent)};
}

inline Int Pow10ForFixture(unsigned exponent) {
    Int result = 1;
    for (unsigned index = 0; index < exponent; ++index)
        result *= 10;
    return result;
}

struct BoundsForFixture final {
    Rational d;
    Rational h;
    Rational p;
    Rational q;
    Rational r;
    std::array<Rational, 4> residual;
    Rational identity;
};

inline BoundsForFixture ZeroNormBounds(unsigned bits) {
    BoundsForFixture result;
    result.d = {Int(0), Int(1)};
    result.h = {Int(0), Int(1)};
    result.p = Pow2ForFixture(270 - static_cast<int>(bits));
    result.q = result.p;
    result.r = Pow2ForFixture(264 - static_cast<int>(bits));
    result.residual = {{result.r,
                        AddForFixture(result.p, result.r),
                        AddForFixture(result.p, AddForFixture(result.p, result.r)),
                        AddForFixture(result.p, result.r)}};
    result.identity = AddForFixture(
        AddForFixture(AddForFixture(result.residual[1], result.residual[2]),
                      result.residual[3]),
        AddForFixture(result.r, result.r));
    return result;
}

inline EndpointEvidence CompleteZeroEvidence() {
    EndpointEvidence evidence;
    const auto scales = paper_full_test::Scales();
    evidence.freshScale = scales.front();
    evidence.terminalScale = scales.back();
    evidence.freshCoefficientOneNorm = 0;
    evidence.terminalCoefficientOneNorm = 0;
    evidence.freshMaximumOneNorm = {Int(0), Int(1)};
    evidence.terminalMaximumOneNorm = {Int(0), Int(1)};

    const auto b512 = ZeroNormBounds(512);
    const auto b768 = ZeroNormBounds(768);
    const std::array<const char*, 4> controls{{"constant", "x", "xNminus1", "sparse"}};
    const std::array<int, 4> controlExponent{{0, 0, 0, 3}};
    for (std::size_t index = 0; index < controls.size(); ++index) {
        const auto prefix = std::string("control.") + controls[index];
        const auto direct = Pow2ForFixture(controlExponent[index] + 10 - 768);
        evidence.checks.push_back({prefix + ".512", {Int(0), Int(1)},
            AddForFixture(Pow2ForFixture(controlExponent[index] + 12 - 512), direct), 0, false});
        evidence.checks.push_back({prefix + ".768", {Int(0), Int(1)},
            AddForFixture(Pow2ForFixture(controlExponent[index] + 12 - 768), direct), 0, false});
    }
    const auto addCheck = [&evidence](const char* id, const Rational& allowance) {
        evidence.checks.push_back({id, {Int(0), Int(1)}, allowance, 0, false});
    };
    addCheck("fresh.cross", {Int(0), Int(1)});
    addCheck("terminal.cross", {Int(0), Int(1)});
    addCheck("fresh.horner.512", {Int(0), Int(1)});
    addCheck("fresh.horner.768", {Int(0), Int(1)});
    addCheck("terminal.horner.512", {Int(0), Int(1)});
    addCheck("terminal.horner.768", {Int(0), Int(1)});
    addCheck("fresh.producer.512", Pow2ForFixture(-300));
    addCheck("fresh.producer.768", Pow2ForFixture(-300));
    addCheck("terminal.producer.512", Pow2ForFixture(-300));
    addCheck("terminal.producer.768", Pow2ForFixture(-300));
    const std::array<const char*, 4> residuals{{"E0", "E8", "I8", "A8"}};
    for (std::size_t index = 0; index < residuals.size(); ++index)
        addCheck((std::string("residual.") + residuals[index] + ".cross").c_str(),
                 AddForFixture(b512.residual[index], b768.residual[index]));
    addCheck("identity.512", b512.identity);
    addCheck("identity.768", b768.identity);

    const Complex<768> zero{Binary768(0), Binary768(0)};
    evidence.freshErrors.assign(paper_full_test::kSlots, zero);
    evidence.terminalErrors.assign(paper_full_test::kSlots, zero);
    for (std::size_t index = 0; index < evidence.maxima.size(); ++index) {
        evidence.maxima[index].id = residuals[index];
        evidence.maxima[index].magnitude = 0;
        evidence.maxima[index].errorAllowance = b768.residual[index];
        evidence.maxima[index].slot = 0;
        evidence.maxima[index].imaginary = false;
        evidence.maxima[index].tuple.fill(zero);
    }
    return evidence;
}

class DisposableDirectory final {
public:
    DisposableDirectory() {
        const auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
        for (unsigned attempt = 0; attempt < 100; ++attempt) {
            path_ = std::filesystem::temp_directory_path() /
                    ("fs-endpoint-synthetic-writer-" + std::to_string(seed) + "-" +
                     std::to_string(attempt));
            std::error_code error;
            if (std::filesystem::create_directory(path_, error)) {
                path_ = std::filesystem::canonical(path_);
                return;
            }
        }
        throw std::runtime_error("synthetic writer could not create disposable directory");
    }
    ~DisposableDirectory() {
        std::error_code ignored;
        std::filesystem::remove_all(path_, ignored);
    }
    const std::filesystem::path& Path() const { return path_; }
private:
    std::filesystem::path path_;
};

inline EndpointEvidenceIdentity SyntheticIdentity() {
    return {"synthetic", std::string(40, '1'), "linux", "1", "1", 108300};
}

inline std::vector<std::string> ReadLines(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input)
        throw std::runtime_error("synthetic writer could not open canonical fixture");
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(input, line))
        lines.push_back(line);
    if (!input.eof())
        throw std::runtime_error("synthetic writer could not read canonical fixture");
    return lines;
}

inline void WriteLines(const std::filesystem::path& path,
                       const std::vector<std::string>& lines) {
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output)
        throw std::runtime_error("synthetic writer could not create malformed fixture");
    for (const auto& line : lines)
        output << line << '\n';
    output.close();
    if (output.fail())
        throw std::runtime_error("synthetic writer could not close malformed fixture");
}

inline void RequireEndpointReason(const std::function<void()>& action,
                                  const std::string& expectedReason,
                                  const std::string& label) {
    bool rejected = false;
    try {
        action();
    }
    catch (const EndpointFailure& failure) {
        rejected = failure.Reason() == expectedReason;
    }
    paper_full_test::Require(rejected, label + " expected endpoint reason=" + expectedReason);
}

inline std::vector<std::string> SplitPrimaryLine(const std::string& line) {
    std::vector<std::string> fields;
    std::size_t begin = 0;
    for (;;) {
        const auto tab = line.find('\t', begin);
        if (tab == std::string::npos) {
            fields.push_back(line.substr(begin));
            return fields;
        }
        fields.push_back(line.substr(begin, tab - begin));
        begin = tab + 1;
    }
}

inline std::string CanonicalFixture(char sign, char digit, char exponentSign,
                                    const std::string& exponent) {
    return std::string(1, sign) + digit + "." + std::string(109, '0') +
           "e" + exponentSign + exponent;
}

inline EndpointEvidence NonzeroPrimaryEvidence() {
    auto evidence = CompleteZeroEvidence();
    const Binary768 half = boost::multiprecision::ldexp(Binary768(1), -1);
    evidence.freshErrors[2] = {half, Binary768(0)};
    evidence.terminalErrors[2] = {Binary768(1), -half};
    evidence.maxima[0].magnitude = half;
    evidence.maxima[0].slot = 2;
    evidence.maxima[0].tuple = {{
        evidence.freshErrors[2], evidence.terminalErrors[2],
        {Binary768(2), Binary768(-2)}, {Binary768(4), Binary768(-4)}}};
    evidence.maxima[1].magnitude = 1;
    evidence.maxima[1].slot = 2;
    evidence.maxima[1].tuple = evidence.maxima[0].tuple;

    Int ten110 = 1;
    for (unsigned index = 0; index < 110; ++index)
        ten110 *= 10;
    const Int halfEvenDown = ten110 + 5;
    evidence.maxima[2].magnitude = Binary768(halfEvenDown.convert_to<std::string>());
    evidence.maxima[2].slot = 4;
    evidence.maxima[2].tuple[2].real = evidence.maxima[2].magnitude;

    const Int carry = ten110 * 10 - 5;
    evidence.maxima[3].magnitude = Binary768(carry.convert_to<std::string>());
    evidence.maxima[3].slot = 5;
    evidence.maxima[3].imaginary = true;
    evidence.maxima[3].tuple[3].imag = -evidence.maxima[3].magnitude;
    return evidence;
}

inline void RunEndpointEvidenceWriterBoundaryTests() {
    DisposableDirectory directory;
    const auto evidence = CompleteZeroEvidence();
    const auto identity = SyntheticIdentity();
    const EndpointPublicationBoundary boundary{0, true};
    const auto result = WriteEndpointEvidence(
        evidence, identity, boundary, directory.Path());
    ValidateEndpointEvidenceFile(result.readyPath, evidence, identity, boundary);
    paper_full_test::Require(result.canonicalBytes > 0 &&
        result.readyPath.filename().string().find("fs-endpoint-synthetic-") == 0,
        "synthetic complete 16384-row writer candidate validates without a live filename");

    RequireEndpointReason([&] {
        (void)WriteEndpointEvidence(evidence, identity, boundary, directory.Path());
    }, "IDENTITY", "writer refuses reuse/overwrite of a claimed identity directory");

    for (const std::size_t residual : {std::size_t(2), std::size_t(3)}) {
        auto wrongZeroSlot = evidence;
        wrongZeroSlot.maxima[residual].slot = 1;
        DisposableDirectory zeroSlotDirectory;
        RequireEndpointReason([&] {
            (void)WriteEndpointEvidence(wrongZeroSlot, identity, boundary,
                                        zeroSlotDirectory.Path());
        }, "INTEGRITY", "zero I8/A8 maximum rejects a nonzero slot independently");

        auto wrongZeroComponent = evidence;
        wrongZeroComponent.maxima[residual].imaginary = true;
        DisposableDirectory zeroComponentDirectory;
        RequireEndpointReason([&] {
            (void)WriteEndpointEvidence(wrongZeroComponent, identity, boundary,
                                        zeroComponentDirectory.Path());
        }, "INTEGRITY", "zero I8/A8 maximum rejects imag independently at slot zero");
    }

    {
        auto nonfinite = evidence;
        const auto infinity = std::numeric_limits<Binary768>::infinity();
        nonfinite.freshErrors[0].real = infinity;
        nonfinite.maxima[0].magnitude = infinity;
        nonfinite.maxima[0].tuple[0] = nonfinite.freshErrors[0];
        DisposableDirectory nonfiniteDirectory;
        RequireEndpointReason([&] {
            (void)WriteEndpointEvidence(nonfinite, identity, boundary,
                                        nonfiniteDirectory.Path());
        }, "NONFINITE", "writer exposes typed coherent positive-infinity failure");
    }
    {
        auto negativeInfinity = evidence;
        negativeInfinity.maxima[2].magnitude =
            -std::numeric_limits<Binary768>::infinity();
        negativeInfinity.maxima[2].tuple[2].real = negativeInfinity.maxima[2].magnitude;
        DisposableDirectory nonfiniteDirectory;
        RequireEndpointReason([&] {
            (void)WriteEndpointEvidence(negativeInfinity, identity, boundary,
                                        nonfiniteDirectory.Path());
        }, "NONFINITE", "negative-infinity magnitude is nonfinite before sign classification");
    }
    {
        auto notANumber = evidence;
        notANumber.maxima[2].magnitude = std::numeric_limits<Binary768>::quiet_NaN();
        notANumber.maxima[2].tuple[2].real = notANumber.maxima[2].magnitude;
        DisposableDirectory nonfiniteDirectory;
        RequireEndpointReason([&] {
            (void)WriteEndpointEvidence(notANumber, identity, boundary,
                                        nonfiniteDirectory.Path());
        }, "NONFINITE", "NaN magnitude has the typed nonfinite reason");
    }
    {
        auto rawExcessWithCeiling = evidence;
        rawExcessWithCeiling.freshCoefficientOneNorm = Int(1) << 473;
        rawExcessWithCeiling.checks[8].allowance = AddForFixture(
            Pow2ForFixture(-127), Pow2ForFixture(-383));
        rawExcessWithCeiling.checks[8].distance = Pow2ForFixture(-119);
        DisposableDirectory rawExcessDirectory;
        RequireEndpointReason([&] {
            (void)WriteEndpointEvidence(rawExcessWithCeiling, identity, boundary,
                                        rawExcessDirectory.Path());
        }, "INTEGRITY",
        "raw finite distance above 2^-120 precedes a coherent over-ceiling model");
    }
    {
        auto unconditioned = evidence;
        unconditioned.freshMaximumOneNorm = {Int(3), Int(2)};
        DisposableDirectory conditioningDirectory;
        RequireEndpointReason([&] {
            (void)WriteEndpointEvidence(unconditioned, identity, boundary,
                                        conditioningDirectory.Path());
        }, "CONDITIONING", "writer exposes typed conditioning failure");
    }
    {
        DisposableDirectory cleanupDirectory;
        RequireEndpointReason([&] {
            (void)WriteEndpointEvidence(evidence, identity, {0, false},
                                        cleanupDirectory.Path());
        }, "INTEGRITY", "writer exposes typed premature-publication failure");
    }

    const auto original = ReadLines(result.readyPath);
    paper_full_test::Require(original.size() == 1 + 43 + 24 + 1 + paper_full_test::kSlots,
                            "synthetic canonical fixture has the complete line count");
    const auto rejectMutation = [&](std::vector<std::string> lines, const std::string& name) {
        const auto path = directory.Path() / ("fs-endpoint-synthetic-" + name + ".tsv");
        WriteLines(path, lines);
        const std::string reason = name == "wrong-identity" ? "IDENTITY" :
                                   ((name == "scale-identity" ||
                                     name == "row-count-metadata") ? "INTEGRITY" : "FORMAT");
        RequireEndpointReason([&] {
            ValidateEndpointEvidenceFile(path, evidence, identity, boundary);
        }, reason, "validator rejects synthetic " + name);
    };

    auto malformed = original;
    malformed[1 + 43 + 24 + 1].replace(
        malformed[1 + 43 + 24 + 1].find('\t') + 1, 119, "nan");
    rejectMutation(std::move(malformed), "malformed-decimal");

    auto wrongIdentity = original;
    wrongIdentity[2] = "meta\tsource_commit\t" + std::string(40, '2');
    rejectMutation(std::move(wrongIdentity), "wrong-identity");

    auto reorderedCheck = original;
    std::swap(reorderedCheck[1 + 43], reorderedCheck[1 + 43 + 1]);
    rejectMutation(std::move(reorderedCheck), "reordered-check");

    auto reorderedRows = original;
    std::swap(reorderedRows[1 + 43 + 24 + 1], reorderedRows[1 + 43 + 24 + 2]);
    rejectMutation(std::move(reorderedRows), "reordered-row");

    auto missingRow = original;
    missingRow.pop_back();
    rejectMutation(std::move(missingRow), "missing-row");

    auto duplicateRow = original;
    duplicateRow.back() = duplicateRow[duplicateRow.size() - 2];
    rejectMutation(std::move(duplicateRow), "duplicate-row");

    auto extraRow = original;
    extraRow.push_back(original.back());
    rejectMutation(std::move(extraRow), "extra-row");

    auto fourRows = original;
    fourRows.resize(1 + 43 + 24 + 1 + 4);
    rejectMutation(std::move(fourRows), "four-row-not-full-evidence");

    auto countMismatch = original;
    countMismatch[1 + 37] = "meta\trow_count\t4";
    rejectMutation(std::move(countMismatch), "row-count-metadata");

    auto scaleMismatch = original;
    scaleMismatch[1 + 18] = "meta\tscale0_numerator\t1";
    rejectMutation(std::move(scaleMismatch), "scale-identity");

    const auto oversized = directory.Path() / "fs-endpoint-synthetic-oversized.tsv";
    {
        std::ofstream output(oversized, std::ios::binary | std::ios::trunc);
        const std::string block(4096, 'x');
        for (std::size_t bytes = 0; bytes <= 16U * 1024U * 1024U; bytes += block.size())
            output.write(block.data(), static_cast<std::streamsize>(block.size()));
    }
    RequireEndpointReason([&] {
        ValidateEndpointEvidenceFile(oversized, evidence, identity, boundary);
    }, "FORMAT", "validator rejects oversized synthetic file before parsing integers");

    const auto oversizedLine = directory.Path() / "fs-endpoint-synthetic-oversized-line.tsv";
    {
        std::ofstream output(oversizedLine, std::ios::binary | std::ios::trunc);
        output << std::string(32768, 'x') << '\n';
    }
    RequireEndpointReason([&] {
        ValidateEndpointEvidenceFile(oversizedLine, evidence, identity, boundary);
    }, "FORMAT", "validator rejects a line over 32768 bytes before numeric allocation");

    RequireEndpointReason([&] {
        (void)WriteEndpointEvidence(evidence, identity, boundary,
            directory.Path() / "child" / "..");
    }, "IDENTITY", "writer rejects traversal/non-normalized publication parent");

    const auto symlink = directory.Path() / "fs-endpoint-synthetic-link.tsv";
    std::error_code symlinkError;
    std::filesystem::create_symlink(result.readyPath, symlink, symlinkError);
    if (!symlinkError)
        RequireEndpointReason([&] {
            ValidateEndpointEvidenceFile(symlink, evidence, identity, boundary);
        }, "IDENTITY",
        "validator rejects a synthetic leaf symlink when the platform permits its fixture");

    std::ostringstream primary;
    const EndpointPublicationBoundary failingE80{9, true};
    EmitEndpointEvidencePrimary(primary, evidence, identity, failingE80);
    const auto records = primary.str();
    const auto countRecords = [&records](const std::string& prefix) {
        std::size_t count = 0;
        for (std::size_t position = 0; (position = records.find(prefix, position)) != std::string::npos;
             position += prefix.size())
            ++count;
        return count;
    };
    paper_full_test::Require(
        countRecords("FS_ENDPOINT_SCALE\t") == 9 &&
        countRecords("FS_ENDPOINT_CHECK\t") == 24 &&
        countRecords("FS_ENDPOINT_MAX\t") == 4 &&
        records.find("numeric_gate_failures=9") != std::string::npos &&
        records.find("numeric_gate_failures=7") == std::string::npos &&
        records.find("E0.real_q_num=") != std::string::npos &&
        records.find("A8.imag_q_den=") != std::string::npos,
        "primary grammar carries nine scales, 24 checks, four signed tuples, exact quanta, and caller count");

    std::ostringstream nonzeroPrimary;
    const auto nonzero = NonzeroPrimaryEvidence();
    EmitEndpointEvidencePrimary(nonzeroPrimary, nonzero, identity, boundary);
    std::vector<std::vector<std::string>> maximumRecords;
    std::istringstream primaryLines(nonzeroPrimary.str());
    std::string primaryLine;
    while (std::getline(primaryLines, primaryLine))
        if (primaryLine.find("FS_ENDPOINT_MAX\t") == 0)
            maximumRecords.push_back(SplitPrimaryLine(primaryLine));
    paper_full_test::Require(maximumRecords.size() == 4,
                            "nonzero primary fixture has four maximum records");

    const auto& e0 = maximumRecords[0];
    const Int e0QuantumDenominator = Int(2) * evidence_writer_test::Pow10ForFixture(110);
    const Int intervalCenter = Int(1) << 503;
    const Int intervalLower = intervalCenter - 1;
    const Int intervalUpper = intervalCenter + 1;
    const Int intervalDenominator = Int(1) << 504;
    paper_full_test::Require(
        e0.size() == 39 && e0[1] == "id=E0" &&
        e0[2] == "magnitude=" + CanonicalFixture('+', '5', '-', "00001") &&
        e0[3] == "magnitude_exact_num=1" && e0[4] == "magnitude_exact_den=2" &&
        e0[5] == "magnitude_quantum_num=1" &&
        e0[6] == "magnitude_quantum_den=" + e0QuantumDenominator.convert_to<std::string>() &&
        e0[7] == "allowance_num=1" &&
        e0[8] == "allowance_den=" + intervalDenominator.convert_to<std::string>() &&
        e0[9] == "interval_lower_num=" + intervalLower.convert_to<std::string>() &&
        e0[10] == "interval_lower_den=" + intervalDenominator.convert_to<std::string>() &&
        e0[11] == "interval_upper_num=" + intervalUpper.convert_to<std::string>() &&
        e0[12] == "interval_upper_den=" + intervalDenominator.convert_to<std::string>() &&
        e0[13] == "argmax_slot=2" && e0[14] == "argmax_component=real",
        "nonzero E0 maximum has exact negative-exponent quantum and rational interval");

    const std::array<const char*, 8> tupleNames{{
        "E0.real", "E0.imag", "E8.real", "E8.imag",
        "I8.real", "I8.imag", "A8.real", "A8.imag"}};
    const std::array<std::string, 8> tupleValues{{
        CanonicalFixture('+', '5', '-', "00001"),
        CanonicalFixture('+', '0', '+', "00000"),
        CanonicalFixture('+', '1', '+', "00000"),
        CanonicalFixture('-', '5', '-', "00001"),
        CanonicalFixture('+', '2', '+', "00000"),
        CanonicalFixture('-', '2', '+', "00000"),
        CanonicalFixture('+', '4', '+', "00000"),
        CanonicalFixture('-', '4', '+', "00000")}};
    const Int unitQuantumDenominator = Int(2) * evidence_writer_test::Pow10ForFixture(109);
    const std::string halfQuantumDenominator = e0QuantumDenominator.convert_to<std::string>();
    const std::string unitQuantumDenominatorText =
        unitQuantumDenominator.convert_to<std::string>();
    for (std::size_t index = 0; index < tupleNames.size(); ++index) {
        const auto offset = 15 + 3 * index;
        const bool zero = index == 1;
        const bool halfQuantum = index == 0 || index == 3;
        paper_full_test::Require(
            e0[offset] == std::string(tupleNames[index]) + "=" + tupleValues[index] &&
            e0[offset + 1] == std::string(tupleNames[index]) + "_q_num=" +
                                (zero ? "0" : "1") &&
            e0[offset + 2] == std::string(tupleNames[index]) + "_q_den=" +
                                (zero ? "1" :
                                 (halfQuantum ? halfQuantumDenominator :
                                  unitQuantumDenominatorText)),
            "primary signed tuple field/quantum order " + std::to_string(index));
    }

    paper_full_test::Require(
        maximumRecords[2][2] == "magnitude=" + CanonicalFixture('+', '1', '+', "00110") &&
        maximumRecords[2][5] == "magnitude_quantum_num=5" &&
        maximumRecords[2][6] == "magnitude_quantum_den=1" &&
        maximumRecords[3][2] == "magnitude=" + CanonicalFixture('+', '1', '+', "00111") &&
        maximumRecords[3][5] == "magnitude_quantum_num=50" &&
        maximumRecords[3][6] == "magnitude_quantum_den=1" &&
        maximumRecords[3][15 + 3 * 7] ==
            "A8.imag=" + CanonicalFixture('-', '1', '+', "00111"),
        "primary fixture preserves half-even-down and carry-renormalized signed values");
}

} // namespace evidence_writer_test
} // namespace paper_endpoint_contract::synthetic
#endif
