#include "openfhe_2023_1788/double_ckks.h"

#include <array>
#include <chrono>
#include <complex>
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {
using openfhe_2023_1788::CiphertextPair;
using openfhe_2023_1788::DoubleCKKS;
using openfhe_2023_1788::ReadOnlyCiphertext;
using openfhe_2023_1788::Mult2Backend;
using openfhe_2023_1788::Mult2BackendTrace;
using openfhe_2023_1788::Mult2Fallback;
using lbcrypto::DCRTPoly;
using Clock = std::chrono::steady_clock;
constexpr std::array<Mult2Backend, 4> backends{Mult2Backend::Reference,
    Mult2Backend::SharedStaged, Mult2Backend::TailDigit, Mult2Backend::TailWide};
constexpr std::array<const char*, 4> names{"Reference", "SharedStaged", "TailDigit", "TailWide"};
// Each backend occupies every position twice. All routes share each input/key.
constexpr std::array<std::array<unsigned, 4>, 8> orders{{
    {{0, 1, 2, 3}}, {{1, 2, 3, 0}}, {{2, 3, 0, 1}}, {{3, 0, 1, 2}},
    {{3, 2, 1, 0}}, {{2, 1, 0, 3}}, {{1, 0, 3, 2}}, {{0, 3, 2, 1}}}};

void Require(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}

void CloseChecked(std::ofstream& stream, const char* message) {
    stream.flush();
    stream.close();
    Require(static_cast<bool>(stream), message);
}

bool SameCiphertextValues(const ReadOnlyCiphertext& a, const ReadOnlyCiphertext& b) {
    return a->GetCryptoContext().get() == b->GetCryptoContext().get() &&
           a->GetKeyTag() == b->GetKeyTag() && a->GetSlots() == b->GetSlots() &&
           a->GetLevel() == b->GetLevel() && a->GetHopLevel() == b->GetHopLevel() &&
           a->GetNoiseScaleDeg() == b->GetNoiseScaleDeg() &&
           a->GetScalingFactor() == b->GetScalingFactor() &&
           a->GetScalingFactorInt() == b->GetScalingFactorInt() &&
           a->GetEncodingType() == b->GetEncodingType() &&
           a->GetMetadataMap() && b->GetMetadataMap() &&
           a->GetMetadataMap()->empty() && b->GetMetadataMap()->empty() &&
           a->GetElements() == b->GetElements();
}

bool SameCiphertext(const ReadOnlyCiphertext& a, const ReadOnlyCiphertext& b) {
    if (!SameCiphertextValues(a, b)) return false;
    for (std::size_t i = 0; i < a->GetElements().size(); ++i) {
        const auto& x = a->GetElements()[i]; const auto& y = b->GetElements()[i];
        if (x.GetFormat() != y.GetFormat() || !(*x.GetParams() == *y.GetParams()) ||
            x.GetNumOfElements() != y.GetNumOfElements()) return false;
        for (std::size_t j = 0; j < x.GetNumOfElements(); ++j) {
            const auto& u = x.GetAllElements()[j]; const auto& v = y.GetAllElements()[j];
            if (u.GetFormat() != v.GetFormat() || !(*u.GetParams() == *v.GetParams()) ||
                u.GetValues().GetModulus() != v.GetValues().GetModulus()) return false;
        }
    }
    return true;
}

void SamePair(const CiphertextPair& a, const CiphertextPair& b) {
    const auto& x = a.GetPaperScale();
    const auto& y = b.GetPaperScale();
    Require(SameCiphertext(a.GetHigh(), b.GetHigh()) && SameCiphertext(a.GetLow(), b.GetLow()) &&
            a.GetOrderedModuli() == b.GetOrderedModuli() && a.GetDivisor() == b.GetDivisor() &&
            a.GetLifecycle() == b.GetLifecycle() && a.GetFormat() == b.GetFormat() &&
            a.GetComponentCount() == b.GetComponentCount() && a.GetLevel() == b.GetLevel() &&
            a.GetNoiseScaleDegree() == b.GetNoiseScaleDegree() &&
            a.GetRecordedScalingFactor() == b.GetRecordedScalingFactor() &&
            a.GetContextIdentity() == b.GetContextIdentity() && a.GetKeyTag() == b.GetKeyTag() &&
            a.GetSlots() == b.GetSlots() && !a.GetRepeatedReceipt() && !b.GetRepeatedReceipt() &&
            x.inputRecordedScalingFactor == y.inputRecordedScalingFactor && x.divisor == y.divisor &&
            x.approximateLogicalScalingFactor == y.approximateLogicalScalingFactor &&
            x.approximateRecombinedLogicalScalingFactor == y.approximateRecombinedLogicalScalingFactor,
            "backend output differs from Reference in full coefficients or state");
}

void CheckComparator(const CiphertextPair& reference) {
    auto corrupted = reference.GetHigh()->Clone();
    auto& tower = corrupted->GetElements().front().GetAllElements().front();
    auto values = tower.GetValues();
    values[0].ModAddEq(lbcrypto::NativeInteger(1), tower.GetModulus());
    tower.SetValues(std::move(values), Format::EVALUATION);
    Require(!SameCiphertext(corrupted, reference.GetHigh()), "coefficient comparator ignored corruption");
    corrupted = reference.GetHigh()->Clone();
    corrupted->SetSlots(reference.GetSlots() + 1);
    Require(!SameCiphertext(corrupted, reference.GetHigh()), "state comparator ignored corruption");
}

// Public ciphertext coefficients only: no secret key or evaluation key is saved.
void SavePublicPair(std::ofstream& binary, const CiphertextPair& pair) {
    for (const auto& member : {pair.GetHigh(), pair.GetLow()}) {
        for (const auto& poly : member->GetElements()) {
            for (const auto& tower : poly.GetAllElements()) {
                for (std::size_t i = 0; i < tower.GetLength(); ++i) {
                    const std::uint64_t value = tower.GetValues()[i].ConvertToInt();
                    // Fixed little-endian representation, independent of host byte order.
                    for (unsigned j = 0; j < 8; ++j) binary.put(static_cast<char>((value >> (8 * j)) & 255));
                }
            }
        }
    }
    Require(static_cast<bool>(binary), "public input write failed");
}

// Copy scalar/parameter values into text independently of upstream shared Params.
// Coefficients are separately copied into plain uint64 values below.
std::string StateSnapshot(const ReadOnlyCiphertext& c) {
    std::ostringstream out;
    out.precision(24);
    out << c.get() << ':' << c->GetCryptoContext().get() << ':' << c->GetKeyTag() << ':'
        << c->GetEncodingType() << ':' << c->GetSlots() << ':' << c->GetLevel() << ':'
        << c->GetNoiseScaleDeg() << ':' << c->GetScalingFactor() << ':'
        << c->GetScalingFactorInt() << ':' << c->GetHopLevel() << ':' << c->GetMetadataMap().get();
    Require(c->GetMetadataMap() && c->GetMetadataMap()->empty(), "legacy pilot requires empty metadata");
    for (const auto& poly : c->GetElements()) {
        out << '|' << poly.GetFormat() << ':' << poly.GetParams()->GetCyclotomicOrder()
            << ':' << poly.GetParams()->GetModulus();
        for (const auto& params : poly.GetParams()->GetParams())
            out << ':' << params->GetModulus() << ':' << params->GetRootOfUnity()
                << ':' << params->GetCyclotomicOrder();
        for (const auto& tower : poly.GetAllElements())
            out << '/' << tower.GetFormat() << ':' << tower.GetModulus() << ':'
                << tower.GetRootOfUnity() << ':' << tower.GetParams()->GetCyclotomicOrder()
                << ':' << tower.GetValues().GetModulus() << ':' << tower.GetLength();
    }
    return out.str();
}

std::vector<std::uint64_t> CoefficientSnapshot(const ReadOnlyCiphertext& c) {
    std::vector<std::uint64_t> values;
    for (const auto& poly : c->GetElements())
        for (const auto& tower : poly.GetAllElements())
            for (std::size_t i = 0; i < tower.GetLength(); ++i)
                values.push_back(tower.GetValues()[i].ConvertToInt());
    return values;
}

bool SameCoefficients(const ReadOnlyCiphertext& c, const std::vector<std::uint64_t>& saved) {
    std::size_t index = 0;
    for (const auto& poly : c->GetElements())
        for (const auto& tower : poly.GetAllElements())
            for (std::size_t i = 0; i < tower.GetLength(); ++i) {
                if (index >= saved.size() || tower.GetValues()[i].ConvertToInt() != saved[index]) return false;
                ++index;
            }
    return index == saved.size();
}

void RequireRoute(const Mult2BackendTrace& trace, unsigned backend) {
    Require(trace.completed && trace.requested == backends[backend] &&
            trace.executed == backends[backend] && trace.fallback == Mult2Fallback::None,
            "pilot route did not execute the requested backend without fallback");
}

std::size_t RunFixture(const char* id, std::uint32_t n, std::uint32_t depth, bool measure,
                       const std::filesystem::path& directory, std::ofstream& records) {
    const auto setupStart = Clock::now();
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(depth);
    parameters.SetScalingModSize(measure ? 58 : 30);
    parameters.SetFirstModSize(measure ? 60 : 35);
    parameters.SetScalingTechnique(lbcrypto::FIXEDMANUAL);
    parameters.SetSecurityLevel(measure ? lbcrypto::HEStd_128_classic : lbcrypto::HEStd_NotSet);
    parameters.SetRingDim(n);
    parameters.SetBatchSize(n / 2);
    parameters.SetKeySwitchTechnique(lbcrypto::HYBRID);
    parameters.SetDigitSize(0);
    auto context = lbcrypto::GenCryptoContext(parameters);
    Require(context->GetRingDimension() == n, "returned ring dimension differs");
    context->Enable(lbcrypto::PKE);
    context->Enable(lbcrypto::KEYSWITCH);
    context->Enable(lbcrypto::LEVELEDSHE);
    const auto keys = context->KeyGen();
    Require(keys.good(), "key generation failed");
    context->EvalMultKeyGen(keys.secretKey);
    std::vector<std::complex<double>> lhs(n / 2), rhs(n / 2);
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        lhs[i] = {static_cast<double>(static_cast<int>(i % 9) - 4) / 32.0,
                  static_cast<double>(static_cast<int>(i % 5) - 2) / 64.0};
        rhs[i] = {static_cast<double>(static_cast<int>(i % 7) - 3) / 32.0,
                  static_cast<double>(static_cast<int>(i % 3) - 1) / 64.0};
    }
    DoubleCKKS module(context);
    const auto left = module.DCP(context->Encrypt(context->MakeCKKSPackedPlaintext(lhs, 2, 0), keys.publicKey));
    const auto right = module.DCP(context->Encrypt(context->MakeCKKSPackedPlaintext(rhs, 2, 0), keys.publicKey));
    const std::array<ReadOnlyCiphertext, 4> inputs{
        left.GetHigh(), left.GetLow(), right.GetHigh(), right.GetLow()};
    std::array<std::vector<std::uint64_t>, 4> coefficients;
    std::array<std::string, 4> states;
    for (std::size_t i = 0; i < inputs.size(); ++i) {
        coefficients[i] = CoefficientSnapshot(inputs[i]);
        states[i] = StateSnapshot(inputs[i]);
    }
    const auto unchanged = [&]() {
        for (std::size_t i = 0; i < inputs.size(); ++i)
            Require(SameCoefficients(inputs[i], coefficients[i]) && StateSnapshot(inputs[i]) == states[i],
                    "operation mutated a pilot input");
    };

    const auto expected = module.Mult2(left, right);
    CheckComparator(expected);
    unchanged();

    const auto prefix = directory / id;
    std::ofstream publicInputs(prefix.string() + "-public-inputs.bin", std::ios::binary);
    SavePublicPair(publicInputs, left); SavePublicPair(publicInputs, right);
    CloseChecked(publicInputs, "public input close failed");
    const auto crypto = std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(
        context->GetCryptoParameters());
    Require(crypto && crypto->GetParamsP(), "missing actual HYBRID parameter basis");
    std::ofstream shape(prefix.string() + "-shape.json");
    shape << "{\"id\":\"" << id << "\",\"N\":" << n << ",\"depth\":" << depth
          << ",\"slots\":" << n / 2 << ",\"surviving_towers\":" << expected.GetOrderedModuli().size()
          << ",\"qDiv\":\"" << left.GetDivisor() << "\",\"full_Q\":[";
    const auto writeBasis = [&](const auto& basis) {
        for (std::size_t i = 0; i < basis->GetParams().size(); ++i) {
            if (i) shape << ',';
            const auto& p = basis->GetParams()[i];
            shape << "{\"modulus\":\"" << p->GetModulus() << "\",\"root\":\""
                  << p->GetRootOfUnity() << "\",\"cyclotomic_order\":" << p->GetCyclotomicOrder() << '}';
        }
    };
    writeBasis(crypto->GetElementParams()); shape << "],\"P\":[";
    writeBasis(crypto->GetParamsP()); shape << "],\"pair_moduli\":[";
    for (std::size_t i = 0; i < left.GetOrderedModuli().size(); ++i) {
        if (i) shape << ',';
        shape << '"' << left.GetOrderedModuli()[i] << '"';
    }
    shape << "],\"components_per_member\":2,\"input_pairs\":[\"left\",\"right\"],"
             "\"byte_order\":\"little\",\"coefficient_format\":\"NTT evaluation\","
             "\"key_switch\":\"HYBRID\",\"secret_key_distribution\":\"default uniform ternary\","
             "\"security_requested\":\"" << (measure ? "HEStd_128_classic" : "HEStd_NotSet")
          << "\",\"independent_security_certification\":false,\"repeated_receipt\":false,"
             "\"timing_boundary\":\"public Mult2WithBackend entrance to return\"}\n";
    CloseChecked(shape, "shape close failed");

    for (unsigned backend = 0; backend < backends.size(); ++backend) {
        Mult2BackendTrace trace;
        const auto result = module.Mult2WithBackend(left, right, backends[backend], &trace);
        SamePair(result, expected); RequireRoute(trace, backend); unchanged();
        records << "{\"kind\":\"check\",\"profile\":\"" << id
                << "\",\"backend\":\"" << names[backend] << "\",\"executed\":\""
                << names[backend] << "\",\"fallback\":\"None\",\"exact_output_and_input_state\":true}\n";
    }
    std::size_t sampleRows = 0;
    if (measure) {
        records << "{\"kind\":\"setup_and_checks\",\"profile\":\"" << id << "\",\"wall_ns\":"
                << std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - setupStart).count()
                << "}\n";
        for (unsigned block = 0; block < orders.size(); ++block) {
            for (unsigned iteration = 0; iteration < 3; ++iteration) {
                for (const auto backend : orders[block]) {
                    Mult2BackendTrace trace;
                    const auto cpuStart = std::clock();
                    const auto begin = Clock::now();
                    const auto result = module.Mult2WithBackend(left, right, backends[backend], &trace);
                    const auto end = Clock::now();
                    const auto cpuEnd = std::clock();
                    Require(cpuStart != std::clock_t(-1) && cpuEnd != std::clock_t(-1), "CPU clock unavailable");
                    // Result stays alive until comparison/logging completes. Every route has
                    // the same lifetime; comparison and destruction are outside every timer.
                    SamePair(result, expected); RequireRoute(trace, backend); unchanged();
                    records << "{\"kind\":\"sample\",\"profile\":\"" << id << "\",\"block\":" << block
                            << ",\"iteration\":" << iteration << ",\"stage\":\"full_public_legacy\","
                            << "\"backend\":\"" << names[backend] << "\",\"executed\":\"" << names[backend]
                            << "\",\"fallback\":\"None\",\"wall_ns\":"
                            << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()
                            << ",\"cpu_ns\":" << static_cast<long long>(
                                static_cast<long double>(cpuEnd - cpuStart) * 1000000000.0L / CLOCKS_PER_SEC)
                            << "}\n";
                    Require(static_cast<bool>(records), "sample write failed");
                    ++sampleRows;
                }
            }
            records.flush();
        }
    }
    unchanged();
    lbcrypto::CryptoContextImpl<DCRTPoly>::ClearEvalMultKeys(keys.secretKey->GetKeyTag());
    lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
    return sampleRows;
}
}  // namespace

int main(int argc, char** argv) {
    try {
        Require(argc == 3 || argc == 4, "usage: c9_cost_probe --check|--study output_dir [admission_sha256]");
        Require(NATIVEINT == 64, "pilot requires the witnessed Native64 configuration");
        const std::string mode(argv[1]);
        const bool measure = mode == "--study";
        Require((mode == "--check" && argc == 3) || (measure && argc == 4), "invalid mode/arguments");
        const std::string admission = measure ? argv[3] : "none";
        Require(!measure || (admission.size() == 64 && admission.find_first_not_of("0123456789abcdef") == std::string::npos),
                "study requires its frozen admission SHA256");
        const std::filesystem::path directory(argv[2]);
        Require(!std::filesystem::exists(directory), "output path exists; no overwrite or silent rerun");
        std::filesystem::create_directories(directory);
        std::ofstream started(directory / "started.json");
        started << "{\"source_commit\":\"" << C9_COST_SOURCE_COMMIT << "\",\"mode\":\"" << mode
                << "\",\"admission_sha256\":\"" << admission << "\"}\n";
        CloseChecked(started, "started record close failed");
        std::ofstream records(directory / "records.jsonl");
        std::size_t samples = 0;
        if (measure) {
            samples += RunFixture("N16384_D3", 16384, 3, true, directory, records);
            samples += RunFixture("N32768_D3", 32768, 3, true, directory, records);
            samples += RunFixture("N32768_D5", 32768, 5, true, directory, records);
        }
        else samples += RunFixture("check_N64_D3", 64, 3, false, directory, records);
        Require(samples == (measure ? 288U : 0U), "unexpected sample count");
        CloseChecked(records, "measurement record close failed");
        std::ofstream completed(directory / "completed.json");
        completed << "{\"source_commit\":\"" << C9_COST_SOURCE_COMMIT << "\",\"mode\":\"" << mode
                  << "\",\"passed\":true,\"sample_rows\":" << samples
                  << ",\"check_rows\":" << (measure ? 12 : 4)
                  << ",\"comparator_negative_controls\":" << (measure ? 6 : 2)
                  << ",\"classification\":\"" << (measure ? "diagnostic only" : "engineering only")
                  << "\",\"repeated_performance\":false}\n";
        CloseChecked(completed, "completion record close failed");
        std::cout << "C9_COST_" << (measure ? "DIAGNOSTIC" : "CHECK") << "_PASS\n";
        return 0;
    }
    catch (const std::exception& error) {
        std::cerr << "C9 cost probe failed: " << error.what() << '\n';
        return 1;
    }
}
