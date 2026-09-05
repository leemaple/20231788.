// Source-mapping probe for the paper Table 3 candidate. This is not a
// production profile, a cryptographic test, or evidence of paper correctness.

#include "openfhe.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#if !defined(NATIVEINT) || NATIVEINT != 64
#error "table3_profile_probe requires NATIVEINT=64"
#endif

#if !defined(MATHBACKEND) || MATHBACKEND != 4
#error "table3_profile_probe requires MATHBACKEND=4"
#endif

namespace {

constexpr uint32_t kCyclotomicOrder = 65536;
constexpr uint32_t kRingDimension   = 32768;
constexpr uint32_t kBatchSize       = 16384;
constexpr uint32_t kAuxBits         = 60;

struct PrimeSpec {
    const char* role;
    uint64_t modulus;
    uint64_t root;
};

// PAPER_TABLE3_RESERVED_P_CANDIDATE_01.json, in storage order.
constexpr std::array<PrimeSpec, 11> kFullQ{{
    {"Base0", 1125899904679937ULL, 26113207984ULL},
    {"Base1", 1125899903827969ULL, 150640639383ULL},
    {"Mult0", 1152921504598720513ULL, 100545759574150ULL},
    {"Mult1", 1152921504597016577ULL, 31693996050849ULL},
    {"Mult2", 1152921504595968001ULL, 88651361085495ULL},
    {"Mult3", 1152921504595640321ULL, 9679305630873ULL},
    {"Mult4", 1152921504593412097ULL, 24428769072221ULL},
    {"Mult5", 1152921504592822273ULL, 18776242964106ULL},
    {"Mult6", 1152921504592429057ULL, 5821397352863ULL},
    {"Mult7", 1152921504589938689ULL, 33888991361320ULL},
    {"Div", 1099510054913ULL, 121567553ULL},
}};

constexpr PrimeSpec kReservedP{
    "P", 1152921504606584833ULL, 4443670208963ULL};

// Source-derived collision oracle: if kReservedP replaces Mult0 in full Q,
// the downward P search must skip kReservedP and accept the absent Mult0.
constexpr PrimeSpec kCollisionNextP{
    "P-after-collision", 1152921504598720513ULL, 100545759574150ULL};

using DCRTParams = lbcrypto::ILDCRTParams<lbcrypto::BigInteger>;

bool CheckSize(const std::string& field, size_t expected, size_t actual) {
    if (actual == expected)
        return true;
    std::cerr << "FAIL field=" << field << " expected=" << expected
              << " actual=" << actual << '\n';
    return false;
}

bool CheckNative(const std::string& field, const lbcrypto::NativeInteger& expected,
                 const lbcrypto::NativeInteger& actual) {
    if (actual == expected)
        return true;
    std::cerr << "FAIL field=" << field << " expected=" << expected
              << " actual=" << actual << '\n';
    return false;
}

lbcrypto::NativeInteger IndependentRoot(const PrimeSpec& expected) {
    static std::map<uint64_t, lbcrypto::NativeInteger> cache;
    const auto found = cache.find(expected.modulus);
    if (found != cache.end())
        return found->second;

    const lbcrypto::NativeInteger modulus(expected.modulus);
    const auto root =
        lbcrypto::RootOfUnity<lbcrypto::NativeInteger>(kCyclotomicOrder, modulus);
    cache.emplace(expected.modulus, root);
    std::cout << "OBS source=RootOfUnity role=" << expected.role
              << " modulus=" << modulus << " root=" << root << '\n';
    return root;
}

bool CheckPrime(const std::string& label, const PrimeSpec& expected,
                const std::shared_ptr<lbcrypto::ILNativeParams>& actual) {
    if (!actual) {
        std::cerr << "FAIL field=" << label << " reason=null-tower\n";
        return false;
    }

    const lbcrypto::NativeInteger expectedModulus(expected.modulus);
    const lbcrypto::NativeInteger literalRoot(expected.root);
    const auto independentRoot = IndependentRoot(expected);

    if (!CheckNative(label + ".literal-root-vs-RootOfUnity", literalRoot, independentRoot))
        return false;
    if (!CheckNative(label + ".modulus", expectedModulus, actual->GetModulus()))
        return false;
    return CheckNative(label + ".root", independentRoot, actual->GetRootOfUnity());
}

bool CheckBasis(const std::string& label, const std::vector<PrimeSpec>& expected,
                const std::shared_ptr<DCRTParams>& actual) {
    if (!actual) {
        std::cerr << "FAIL field=" << label << " reason=null-basis\n";
        return false;
    }

    const auto& towers = actual->GetParams();
    if (!CheckSize(label + ".size", expected.size(), towers.size()))
        return false;
    for (size_t i = 0; i < expected.size(); ++i) {
        if (!CheckPrime(label + "[" + std::to_string(i) + "]." + expected[i].role,
                        expected[i], towers[i]))
            return false;
    }
    return true;
}

void PrintBasis(const std::string& family, const char* getter,
                const std::shared_ptr<DCRTParams>& actual) {
    const auto& towers = actual->GetParams();
    std::cout << "OBS family=" << family << " getter=" << getter
              << " size=" << towers.size() << " values=";
    for (size_t i = 0; i < towers.size(); ++i) {
        if (i != 0)
            std::cout << ',';
        std::cout << towers[i]->GetModulus() << ':'
                  << towers[i]->GetRootOfUnity();
    }
    std::cout << '\n';
}

std::vector<PrimeSpec> MakeFamily(size_t index, bool includeDiv) {
    std::vector<PrimeSpec> q{kFullQ[0], kFullQ[1]};
    const size_t multCount = 8 - index;
    for (size_t i = 0; i < multCount; ++i)
        q.push_back(kFullQ[2 + i]);
    if (includeDiv)
        q.push_back(kFullQ[10]);
    return q;
}

std::shared_ptr<lbcrypto::CryptoParametersCKKSRNS> MakeParameters(
    const std::vector<PrimeSpec>& q) {
    std::vector<lbcrypto::NativeInteger> moduli;
    std::vector<lbcrypto::NativeInteger> roots;
    moduli.reserve(q.size());
    roots.reserve(q.size());
    for (const auto& prime : q) {
        moduli.emplace_back(prime.modulus);
        roots.emplace_back(prime.root);
    }

    auto elementParams =
        std::make_shared<DCRTParams>(kCyclotomicOrder, moduli, roots);
    lbcrypto::EncodingParams encodingParams =
        std::make_shared<lbcrypto::EncodingParamsImpl>(100, kBatchSize);
    auto parameters = std::make_shared<lbcrypto::CryptoParametersCKKSRNS>(
        elementParams, encodingParams, 3.19f, 36.0f, lbcrypto::HEStd_NotSet, 0,
        lbcrypto::SPARSE_TERNARY, 2, lbcrypto::HYBRID, lbcrypto::FIXEDMANUAL,
        lbcrypto::STANDARD, lbcrypto::HPS, lbcrypto::NOT_SET,
        lbcrypto::FIXED_NOISE_MULTIPARTY, lbcrypto::EXEC_EVALUATION,
        lbcrypto::FIXED_NOISE_DECRYPT, 1, 30, 1, 1,
        lbcrypto::CompressionLevel::SLACK, lbcrypto::BASE_NUM_LEVELS_TO_DROP,
        NATIVEINT, lbcrypto::COMPLEX);

    parameters->PrecomputeCRTTables(
        lbcrypto::HYBRID, lbcrypto::FIXEDMANUAL, lbcrypto::STANDARD,
        lbcrypto::HPS, static_cast<uint32_t>(q.size()), kAuxBits, 0);
    return parameters;
}

bool CheckPartitions(const std::string& label,
                     const std::vector<PrimeSpec>& expectedQ,
                     const std::shared_ptr<lbcrypto::CryptoParametersCKKSRNS>& parameters) {
    const size_t expectedL = expectedQ.size();
    if (!CheckSize(label + ".numPartQ", expectedL, parameters->GetNumPartQ()))
        return false;
    if (!CheckSize(label + ".alpha", 1, parameters->GetNumPerPartQ()))
        return false;
    if (!CheckSize(label + ".partition-count", expectedL,
                   parameters->GetNumberOfQPartitions()))
        return false;

    for (size_t i = 0; i < expectedL; ++i) {
        const std::vector<PrimeSpec> onePrime{expectedQ[i]};
        if (!CheckBasis(label + ".partQ[" + std::to_string(i) + "]",
                        onePrime, parameters->GetParamsPartQ(i)))
            return false;
    }
    return true;
}

bool CheckFamily(const std::string& label, const std::vector<PrimeSpec>& expectedQ,
                 const PrimeSpec& expectedP) {
    auto requestedParameters = MakeParameters(expectedQ);
    auto scheme = std::make_shared<lbcrypto::SchemeCKKSRNS>();
    scheme->SetKeySwitchingTechnique(lbcrypto::HYBRID);
    auto context = std::make_shared<lbcrypto::CryptoContextImpl<lbcrypto::DCRTPoly>>(
        requestedParameters, scheme, lbcrypto::SCHEME::CKKSRNS_SCHEME);
    if (!context) {
        std::cerr << "FAIL field=" << label << ".context reason=null-context\n";
        return false;
    }

    auto actualParameters = std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(
        context->GetCryptoParameters());
    if (!actualParameters) {
        std::cerr << "FAIL field=" << label
                  << ".context.parameters reason=wrong-parameter-type\n";
        return false;
    }

    if (!CheckSize(label + ".ring-dimension", kRingDimension,
                   context->GetRingDimension()))
        return false;
    if (!CheckBasis(label + ".Q", expectedQ, actualParameters->GetElementParams()))
        return false;
    if (!CheckPartitions(label, expectedQ, actualParameters))
        return false;

    const std::vector<PrimeSpec> expectedPBasis{expectedP};
    if (!CheckBasis(label + ".P", expectedPBasis, actualParameters->GetParamsP()))
        return false;

    std::vector<PrimeSpec> expectedQP = expectedQ;
    expectedQP.push_back(expectedP);
    if (!CheckBasis(label + ".QP", expectedQP, actualParameters->GetParamsQP()))
        return false;

    PrintBasis(label, "Q", actualParameters->GetElementParams());
    PrintBasis(label, "P", actualParameters->GetParamsP());
    PrintBasis(label, "QP", actualParameters->GetParamsQP());
    std::cout << "PASS family=" << label << " L=" << expectedQ.size()
              << " numPartQ=" << actualParameters->GetNumPartQ()
              << " alpha=" << actualParameters->GetNumPerPartQ()
              << " partition-count=" << actualParameters->GetNumberOfQPartitions()
              << '\n';
    return true;
}

std::vector<PrimeSpec> MakeCollisionQ() {
    std::vector<PrimeSpec> q(kFullQ.begin(), kFullQ.end());
    q[2] = kReservedP;
    return q;
}

}  // namespace

int main() {
    std::cout << "TABLE3_PROFILE_PROBE candidate=01 openfhe=df495ba2e91739a6dc8f1de254fc5a41155ce504\n";
    std::cout << "CONFIG NATIVEINT=" << NATIVEINT
              << " MATHBACKEND=" << MATHBACKEND << '\n';

    for (size_t i = 0; i <= 8; ++i) {
        if (!CheckFamily("B" + std::to_string(i), MakeFamily(i, true), kReservedP))
            return 1;
    }
    for (size_t i = 0; i <= 8; ++i) {
        if (!CheckFamily("A" + std::to_string(i), MakeFamily(i, false), kReservedP))
            return 1;
    }

    if (!CheckFamily("P-collision-Mult0-replaced", MakeCollisionQ(), kCollisionNextP))
        return 1;

    std::cout << "PASS families=18 collision-control=exact no-keygen no-ciphertext\n";
    return 0;
}
