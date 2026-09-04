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

// Frozen before any Add runtime green; a functional host-sum check, not a
// precision-bit or security claim. Expected values below are independent literals.
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
        context->MakeCKKSPackedPlaintext(std::vector<double>{1.25, -2.5, 0.75}, 2, 0);
    const auto rightPlaintext =
        context->MakeCKKSPackedPlaintext(std::vector<double>{-0.5, 3.0, 1.5}, 2, 0);
    const auto leftCiphertext = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightCiphertext = context->Encrypt(rightPlaintext, keys.publicKey);

    DoubleCKKS module(context);
    const auto left = module.DCP(leftCiphertext);
    const auto right = module.DCP(rightCiphertext);

    try {
        const auto sum = module.Add(left, right);
        Check(sum.GetHigh().get() != left.GetHigh().get(), "Add result high aliases the left input");
        Check(sum.GetLow().get() != left.GetLow().get(), "Add result low aliases the left input");
        Check(sum.GetHigh().get() != right.GetHigh().get(), "Add result high aliases the right input");
        Check(sum.GetLow().get() != right.GetLow().get(), "Add result low aliases the right input");
        Check(sum.GetLifecycle() == left.GetLifecycle(), "Add did not preserve the pair lifecycle");
        Check(sum.GetOrderedModuli() == left.GetOrderedModuli(), "Add did not preserve the ordered basis");
        Check(sum.GetRecordedScalingFactor() == left.GetRecordedScalingFactor(),
              "Add did not preserve the recorded scaling factor");
        const auto recombined = module.RCB(sum);
        Check(recombined->GetElements().size() == 2, "Add result cannot be recombined as a pair");

        lbcrypto::ConstCiphertext<DCRTPoly> recombinedConst = recombined;
        lbcrypto::Plaintext decoded;
        const auto decryption = context->Decrypt(keys.secretKey, recombinedConst, &decoded);
        Check(decryption.isValid, "Add result decryption was rejected");
        const std::vector<std::complex<double>> expected{{0.75, 0.0}, {0.5, 0.0}, {2.25, 0.0}};
        decoded->SetLength(expected.size());
        const auto actual = decoded->GetCKKSPackedValue();
        Check(actual.size() == expected.size(), "Add decoded slot count changed");
        for (std::size_t index = 0; index < expected.size(); ++index) {
            Check(std::isfinite(actual[index].real()) && std::isfinite(actual[index].imag()),
                  "Add decoded slot is not finite");
            Check(std::abs(actual[index] - expected[index]) <= kDecodedAbsoluteTolerance,
                  "Add decoded slot disagrees with the independent literal host sum");
        }
    }
    catch (const std::invalid_argument& exception) {
        const std::string message(exception.what());
        if (message == "DoubleCKKS: Add arithmetic is not implemented") {
            throw TestFailure("Add behavior missing: throwing scaffold reached");
        }
        throw TestFailure("Add fixture or validation failed before arithmetic: " + message);
    }
}

}  // namespace

int main() {
    try {
        TestRuntimeBehavior();
        lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
        std::cout << "pair Add runtime behavior passed\n";
        return 0;
    }
    catch (const TestFailure& failure) {
        std::cerr << "pair Add test failure: " << failure.what() << '\n';
        return 1;
    }
    catch (const std::exception& exception) {
        std::cerr << "pair Add unexpected exception: " << exception.what() << '\n';
        return 1;
    }
}
