#include "openfhe.h"
#include "openfhe_2023_1788/double_ckks.h"

#include <cmath>
#include <complex>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using lbcrypto::CryptoContext;
using lbcrypto::DCRTPoly;
using openfhe_2023_1788::DoubleCKKS;

// Frozen before Sub arithmetic exists. This is a functional host-difference
// check, not a precision-bit or security claim.
constexpr double kDecodedAbsoluteTolerance = 1.0e-6;

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
    parameters.SetMultiplicativeDepth(3);
    parameters.SetScalingModSize(30);
    parameters.SetFirstModSize(35);
    parameters.SetScalingTechnique(lbcrypto::FIXEDMANUAL);
    parameters.SetSecurityLevel(lbcrypto::HEStd_NotSet);
    parameters.SetRingDim(32);
    parameters.SetBatchSize(8);

    auto context = lbcrypto::GenCryptoContext(parameters);
    context->Enable(lbcrypto::PKE);
    context->Enable(lbcrypto::KEYSWITCH);
    context->Enable(lbcrypto::LEVELEDSHE);
    return context;
}

void TestRuntimeBehavior() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    const auto leftPlaintext =
        context->MakeCKKSPackedPlaintext(std::vector<double>{2.25, -1.5, 4.0}, 2, 0);
    const auto rightPlaintext =
        context->MakeCKKSPackedPlaintext(std::vector<double>{0.75, 2.0, -3.5}, 2, 0);
    const auto leftCiphertext = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightCiphertext = context->Encrypt(rightPlaintext, keys.publicKey);

    DoubleCKKS module(context);
    const auto left = module.DCP(leftCiphertext);
    const auto right = module.DCP(rightCiphertext);
    const auto difference = module.Sub(left, right);
    Check(difference.GetHigh().get() != left.GetHigh().get(), "Sub result high aliases the left input");
    Check(difference.GetLow().get() != left.GetLow().get(), "Sub result low aliases the left input");
    Check(difference.GetHigh().get() != right.GetHigh().get(), "Sub result high aliases the right input");
    Check(difference.GetLow().get() != right.GetLow().get(), "Sub result low aliases the right input");
    Check(difference.GetLifecycle() == left.GetLifecycle(), "Sub did not preserve the pair lifecycle");
    Check(difference.GetOrderedModuli() == left.GetOrderedModuli(), "Sub did not preserve the ordered basis");
    Check(difference.GetRecordedScalingFactor() == left.GetRecordedScalingFactor(),
          "Sub did not preserve the recorded scaling factor");
    const auto recombined = module.RCB(difference);
    Check(recombined->GetElements().size() == 2, "Sub result cannot be recombined as a pair");

    lbcrypto::ConstCiphertext<DCRTPoly> recombinedConst = recombined;
    lbcrypto::Plaintext decoded;
    const auto decryption = context->Decrypt(keys.secretKey, recombinedConst, &decoded);
    Check(decryption.isValid, "Sub result decryption was rejected");
    const std::vector<std::complex<double>> expected{{1.5, 0.0}, {-3.5, 0.0}, {7.5, 0.0}};
    decoded->SetLength(expected.size());
    const auto actual = decoded->GetCKKSPackedValue();
    Check(actual.size() == expected.size(), "Sub decoded slot count changed");
    for (std::size_t index = 0; index < expected.size(); ++index) {
        Check(std::isfinite(actual[index].real()) && std::isfinite(actual[index].imag()),
              "Sub decoded slot is not finite");
        Check(std::abs(actual[index] - expected[index]) <= kDecodedAbsoluteTolerance,
              "Sub decoded slot disagrees with the independent literal host difference");
    }
}

}  // namespace

int main() {
    try {
        TestRuntimeBehavior();
        lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
        std::cout << "pair Sub runtime behavior passed\n";
        return 0;
    }
    catch (const TestFailure& failure) {
        std::cerr << "pair Sub test failure: " << failure.what() << '\n';
        return 1;
    }
    catch (const std::exception& exception) {
        std::cerr << "pair Sub unexpected exception: " << exception.what() << '\n';
        return 1;
    }
}
