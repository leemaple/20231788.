#include "openfhe.h"
#include "openfhe_2023_1788/double_ckks.h"

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using lbcrypto::CryptoContext;
using lbcrypto::DCRTPoly;
using openfhe_2023_1788::DoubleCKKS;
using openfhe_2023_1788::PairLifecycle;

constexpr std::uint32_t kRingDimension = 64;
constexpr std::uint32_t kBatchSize = 16;
constexpr std::uint32_t kScalingModSize = 30;
constexpr std::uint32_t kFirstModSize = 35;
constexpr std::uint32_t kMultiplicativeDepth = 7;

// Frozen in the behavior-red patch, before any candidate Mult2 implementation
// is exercised. This is a functional decoded-slot threshold, not a precision-bit
// or production-security claim.
constexpr double kLogicalDecodedAbsoluteTolerance = 1.0e-3;

class TestFailure final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void Check(bool condition, const std::string& message) {
    if (!condition) {
        throw TestFailure(message);
    }
}

CryptoContext<DCRTPoly> MakeContext() {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(kMultiplicativeDepth);
    parameters.SetScalingModSize(kScalingModSize);
    parameters.SetFirstModSize(kFirstModSize);
    parameters.SetScalingTechnique(lbcrypto::FIXEDMANUAL);
    parameters.SetKeySwitchTechnique(lbcrypto::HYBRID);
    parameters.SetSecretKeyDist(lbcrypto::UNIFORM_TERNARY);
    parameters.SetCKKSDataType(lbcrypto::REAL);
    parameters.SetSecurityLevel(lbcrypto::HEStd_NotSet);
    parameters.SetRingDim(kRingDimension);
    parameters.SetBatchSize(kBatchSize);

    auto context = lbcrypto::GenCryptoContext(parameters);
    context->Enable(lbcrypto::PKE);
    context->Enable(lbcrypto::KEYSWITCH);
    context->Enable(lbcrypto::LEVELEDSHE);
    return context;
}

void CheckCompositionContract() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    context->EvalMultKeyGen(keys.secretKey);

    const std::vector<double> leftValues{0.0, std::ldexp(1.0, -20), -std::ldexp(1.0, -20),
                                         0.125, -0.25, 0.5, -0.375, 0.0625};
    const std::vector<double> rightValues{-0.5, 0.25, -0.125, 0.0,
                                          0.125, -0.25, std::ldexp(1.0, -18), 0.75};
    Check(leftValues.size() == rightValues.size(), "host-vector lengths differ");
    Check(leftValues.size() <= kBatchSize, "host-vector length exceeds selected batch size");

    const auto leftPlaintext = context->MakeCKKSPackedPlaintext(leftValues, 2, 0);
    const auto rightPlaintext = context->MakeCKKSPackedPlaintext(rightValues, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);
    const auto leftInputBefore = leftInput->Clone();
    const auto rightInputBefore = rightInput->Clone();

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto leftHighBefore = left.GetHigh()->Clone();
    const auto leftLowBefore = left.GetLow()->Clone();
    const auto rightHighBefore = right.GetHigh()->Clone();
    const auto rightLowBefore = right.GetLow()->Clone();
    const auto inputBasis = left.GetOrderedModuli();
    Check(inputBasis.size() >= 3, "Mult2 fixture did not retain enough active Q_l towers");
    const auto qDiv = left.GetDivisor();
    const auto qL = inputBasis.back();

    const auto result = module.Mult2(left, right);

    Check(result.GetLifecycle() == PairLifecycle::RefreshRequired,
          "Mult2 did not end at RefreshRequired");
    Check(result.GetContextIdentity() == context.get(), "Mult2 context identity changed");
    Check(result.GetDivisor() == qDiv, "Mult2 divisor changed");
    Check(result.GetLevel() == 2, "Mult2 output level is not two");
    Check(result.GetNoiseScaleDegree() == 2, "Mult2 output noise-scale degree is not two");
    Check(result.GetComponentCount() == 2, "Mult2 output pair does not contain two components per member");
    Check(result.GetFormat() == Format::EVALUATION, "Mult2 output pair format is not evaluation");
    Check(result.GetKeyTag() == left.GetKeyTag(), "Mult2 key tag changed");
    Check(result.GetSlots() == left.GetSlots(), "Mult2 slots changed");
    Check(result.GetOrderedModuli().size() + 1 == inputBasis.size(),
          "Mult2 output did not drop exactly q_l");
    for (std::size_t index = 0; index < result.GetOrderedModuli().size(); ++index) {
        Check(result.GetOrderedModuli()[index] == inputBasis[index],
              "Mult2 output basis is not the exact input prefix");
    }

    Check(*leftInput == *leftInputBefore, "Mult2 mutated the left encrypted input");
    Check(*rightInput == *rightInputBefore, "Mult2 mutated the right encrypted input");
    Check(*left.GetHigh() == *leftHighBefore && *left.GetLow() == *leftLowBefore,
          "Mult2 mutated the left pair");
    Check(*right.GetHigh() == *rightHighBefore && *right.GetLow() == *rightLowBefore,
          "Mult2 mutated the right pair");

    auto recombined = module.RCB(result);
    lbcrypto::ConstCiphertext<DCRTPoly> recombinedConst = recombined;
    lbcrypto::Plaintext decodedPlaintext;
    const auto decryptResult = context->Decrypt(keys.secretKey, recombinedConst, &decodedPlaintext);
    Check(decryptResult.isValid, "OpenFHE rejected the Mult2 decryption");
    decodedPlaintext->SetLength(leftValues.size());
    const auto recordedScaleValues = decodedPlaintext->GetCKKSPackedValue();
    Check(recordedScaleValues.size() == leftValues.size(), "decoded vector length changed");

    const long double deltaSquared = std::ldexp(1.0L, 2 * static_cast<int>(kScalingModSize));
    const long double logicalToRecordedRatio =
        deltaSquared /
        (static_cast<long double>(qDiv.ConvertToInt()) * static_cast<long double>(qL.ConvertToInt()));
    Check(logicalToRecordedRatio > 0.0L && std::isfinite(logicalToRecordedRatio),
          "logical/recorded scale ratio is invalid");

    double maximumLogicalError = 0.0;
    for (std::size_t index = 0; index < leftValues.size(); ++index) {
        const std::complex<double> expected(leftValues[index] * rightValues[index], 0.0);
        const auto logicalValue = recordedScaleValues[index] /
                                  static_cast<double>(logicalToRecordedRatio);
        maximumLogicalError = std::max(maximumLogicalError, std::abs(logicalValue - expected));
    }
    Check(maximumLogicalError <= kLogicalDecodedAbsoluteTolerance,
          "logical-scale decoded slot error exceeded the frozen threshold");

    std::cout << std::setprecision(18)
              << "security_level=HEStd_NotSet(functional-only)"
              << " ring_dimension=" << kRingDimension
              << " batch_size=" << kBatchSize
              << " scaling_mod_size=" << kScalingModSize
              << " first_mod_size=" << kFirstModSize
              << " multiplicative_depth=" << kMultiplicativeDepth
              << " key_switch=HYBRID"
              << " secret_key_dist=UNIFORM_TERNARY"
              << " ckks_data_type=REAL"
              << " active_Q_l_towers=" << inputBasis.size()
              << " q_div=" << qDiv
              << " q_l=" << qL
              << " ratio_2^(2p)/(q_div*q_l)=" << logicalToRecordedRatio
              << " frozen_decoded_abs_tolerance=" << kLogicalDecodedAbsoluteTolerance
              << " measured_max_logical_slot_error=" << maximumLogicalError << '\n';
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2 || std::string(argv[1]) != "composition_contract") {
        std::cerr << "usage: mult2_test composition_contract\n";
        return 2;
    }

    try {
        CheckCompositionContract();
        return 0;
    }
    catch (const std::exception& exception) {
        std::cerr << "mult2_test failure: " << exception.what() << '\n';
        return 1;
    }
}
