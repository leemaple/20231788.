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
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {
using openfhe_2023_1788::CiphertextPair;
using openfhe_2023_1788::DoubleCKKS;
using openfhe_2023_1788::ReadOnlyCiphertext;
using openfhe_2023_1788::RS2Backend;
using lbcrypto::DCRTPoly;
using Clock = std::chrono::steady_clock;
constexpr std::array<RS2Backend, 3> backends{
    RS2Backend::Reference, RS2Backend::Reordered, RS2Backend::Fused};
constexpr std::array<const char*, 3> names{"reference", "reordered", "fused"};
constexpr std::array<std::array<unsigned, 3>, 6> orders{{
    {{0, 1, 2}}, {{1, 2, 0}}, {{2, 0, 1}}, {{2, 1, 0}}, {{1, 0, 2}}, {{0, 2, 1}}}};

void Require(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}

bool SameCiphertext(const ReadOnlyCiphertext& a, const ReadOnlyCiphertext& b) {
    return a->GetCryptoContext().get() == b->GetCryptoContext().get() &&
           a->GetKeyTag() == b->GetKeyTag() && a->GetSlots() == b->GetSlots() &&
           a->GetLevel() == b->GetLevel() && a->GetNoiseScaleDeg() == b->GetNoiseScaleDeg() &&
           a->GetScalingFactor() == b->GetScalingFactor() &&
           a->GetScalingFactorInt() == b->GetScalingFactorInt() &&
           a->GetEncodingType() == b->GetEncodingType() && a->GetElements() == b->GetElements();
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

void RunFixture(std::uint32_t n, bool measure, const std::filesystem::path& directory,
                std::ofstream& records) {
    const auto setupStart = Clock::now();
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(3);
    parameters.SetScalingModSize(measure ? 58 : 30);
    parameters.SetFirstModSize(measure ? 60 : 35);
    parameters.SetScalingTechnique(lbcrypto::FIXEDMANUAL);
    parameters.SetSecurityLevel(measure ? lbcrypto::HEStd_128_classic : lbcrypto::HEStd_NotSet);
    parameters.SetRingDim(n);
    parameters.SetBatchSize(n / 2);
    parameters.SetKeySwitchTechnique(lbcrypto::HYBRID);
    parameters.SetDigitSize(0);
    auto context = lbcrypto::GenCryptoContext(parameters);
    context->Enable(lbcrypto::PKE);
    context->Enable(lbcrypto::KEYSWITCH);
    context->Enable(lbcrypto::LEVELEDSHE);
    const auto keys = context->KeyGen();
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
    const auto relin = module.Relin2(module.Tensor2(left, right));
    const auto expected = module.Mult2(left, right);
    SamePair(module.RS2(relin), expected);
    CheckComparator(expected);
    const auto leftHigh = left.GetHigh()->Clone();
    const auto leftLow = left.GetLow()->Clone();
    const auto rightHigh = right.GetHigh()->Clone();
    const auto rightLow = right.GetLow()->Clone();
    const auto relinHigh = relin.GetHigh()->Clone();
    const auto relinLow = relin.GetLow()->Clone();

    const auto prefix = directory / ("N" + std::to_string(n));
    std::ofstream inputs(prefix.string() + "-public-inputs.bin", std::ios::binary);
    SavePublicPair(inputs, left); SavePublicPair(inputs, right); SavePublicPair(inputs, relin);
    inputs.close();
    std::ofstream shape(prefix.string() + "-shape.json");
    shape << "{\"N\":" << n << ",\"slots\":" << n / 2
          << ",\"qDiv\":\"" << relin.GetDivisor() << "\",\"moduli\":[";
    for (std::size_t i = 0; i < relin.GetOrderedModuli().size(); ++i) {
        if (i) shape << ',';
        shape << '"' << relin.GetOrderedModuli()[i] << '"';
    }
    shape << "],\"components_per_member\":2,\"input_pairs\":[\"left\",\"right\",\"relinearized\"],"
             "\"byte_order\":\"little\",\"coefficient_format\":\"NTT evaluation\","
             "\"secret_key_distribution\":\"OpenFHE default uniform ternary\","
             "\"paper_h128_sparse_key_claim\":false}\n";
    Require(static_cast<bool>(shape), "shape record write failed");
    const auto operation = [&](unsigned stage, unsigned backend) {
        if (stage == 0) return module.RS2WithBackend(relin, backends[backend]);
        return module.RS2WithBackend(module.Relin2(module.Tensor2(left, right)), backends[backend]);
    };
    // Untimed validation and warmup. Both stages must match the public Mult2 result.
    for (unsigned stage = 0; stage < 2; ++stage)
        for (unsigned backend = 0; backend < 3; ++backend) SamePair(operation(stage, backend), expected);

    if (measure) {
        records << "{\"kind\":\"setup\",\"N\":" << n << ",\"wall_ns\":"
                << std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - setupStart).count()
                << "}\n";
        for (unsigned block = 0; block < orders.size(); ++block) {
            for (unsigned s = 0; s < 2; ++s) {
                const unsigned stage = (s + block) % 2;
                for (const auto backend : orders[block]) {
                    for (unsigned iteration = 0; iteration < 4; ++iteration) {
                        const auto cpuStart = std::clock();
                        const auto begin = Clock::now();
                        const auto result = operation(stage, backend);
                        const auto end = Clock::now();
                        const auto cpuEnd = std::clock();
                        Require(cpuStart != std::clock_t(-1) && cpuEnd != std::clock_t(-1), "CPU clock unavailable");
                        // Outside the timer; all coefficients and state are compared, every call.
                        SamePair(result, expected);
                        records << "{\"kind\":\"sample\",\"N\":" << n << ",\"block\":" << block
                                << ",\"iteration\":" << iteration << ",\"stage\":\""
                                << (stage == 0 ? "RS2_validated" : "Tensor2_Relin2_RS2")
                                << "\",\"backend\":\"" << names[backend] << "\",\"wall_ns\":"
                                << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()
                                << ",\"cpu_ns\":" << static_cast<long long>(
                                    static_cast<long double>(cpuEnd - cpuStart) * 1000000000.0L / CLOCKS_PER_SEC)
                                << "}\n";
                        Require(static_cast<bool>(records), "measurement record write failed");
                    }
                }
            }
            records.flush();
        }
    }
    Require(SameCiphertext(left.GetHigh(), leftHigh) && SameCiphertext(left.GetLow(), leftLow) &&
            SameCiphertext(right.GetHigh(), rightHigh) && SameCiphertext(right.GetLow(), rightLow) &&
            SameCiphertext(relin.GetHigh(), relinHigh) && SameCiphertext(relin.GetLow(), relinLow),
            "timing or checks mutated a public input");
    lbcrypto::CryptoContextImpl<DCRTPoly>::ClearEvalMultKeys(keys.secretKey->GetKeyTag());
    lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
}
}  // namespace

int main(int argc, char** argv) {
    try {
        Require(argc == 3 || argc == 4, "usage: c7_cost_probe --check|--study output_dir [admission_sha256]");
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
        started << "{\"source_commit\":\"" << C7_SOURCE_COMMIT << "\",\"mode\":\"" << mode
                << "\",\"admission_sha256\":\"" << admission << "\"}\n";
        started.close();
        std::ofstream records(directory / "records.jsonl");
        if (measure) {
            RunFixture(16384, true, directory, records);
            RunFixture(32768, true, directory, records);
        }
        else RunFixture(32, false, directory, records);
        records.close();
        std::ofstream completed(directory / "completed.json");
        completed << "{\"source_commit\":\"" << C7_SOURCE_COMMIT << "\",\"mode\":\"" << mode
                  << "\",\"passed\":true,\"sample_rows\":" << (measure ? 288 : 0)
                  << ",\"functional_comparator_negative_controls\":true}\n";
        Require(static_cast<bool>(completed), "completion record write failed");
        std::cout << "C7 " << mode << " completed; performance publication admission is separate\n";
        return 0;
    }
    catch (const std::exception& error) {
        std::cerr << "C7 probe failed: " << error.what() << '\n';
        return 1;
    }
}
