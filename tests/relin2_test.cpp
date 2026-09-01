#include "openfhe.h"
#include "openfhe_2023_1788/double_ckks.h"

#include <cmath>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using lbcrypto::CryptoContext;
using lbcrypto::DCRTPoly;
using openfhe_2023_1788::DoubleCKKS;
using openfhe_2023_1788::TensorScaleDescriptor;

class TestFailure final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void Check(bool condition, const std::string& message) {
    if (!condition) {
        throw TestFailure(message);
    }
}

template <class Function>
void CheckThrowsExactInvalidArgument(Function&& function, const std::string& expectedMessage,
                                     const std::string& label) {
    bool threw = false;
    try {
        std::invoke(std::forward<Function>(function));
    }
    catch (const std::invalid_argument& exception) {
        Check(exception.what() == expectedMessage,
              label + " reported an unexpected diagnostic: " + exception.what());
        threw = true;
    }
    catch (const std::exception& exception) {
        throw TestFailure(label + " threw the wrong exception type: " + exception.what());
    }
    Check(threw, label + " did not fail fast");
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

void TestTensorValidationOrder() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    auto tensor = module.Tensor2(left, right);

    auto& tensorScale = const_cast<TensorScaleDescriptor&>(tensor.GetTensorScale());
    Check(std::isfinite(tensorScale.approximateHighLogicalScalingFactor) &&
              tensorScale.approximateHighLogicalScalingFactor > 0.0L,
          "Relin2 validation-order fixture has an invalid starting high logical scale");
    tensorScale.approximateHighLogicalScalingFactor *= 2.0L;

    CheckThrowsExactInvalidArgument(
        [&] { (void)module.Relin2(tensor); },
        "DoubleCKKS: Tensor2 result paper-scale descriptor is inconsistent",
        "Relin2 Tensor manifest validation before evaluation-key lookup");
}

using TestFunction = void (*)();

TestFunction ResolveTest(const std::string& name) {
    if (name == "tensor_validation_order") {
        return &TestTensorValidationOrder;
    }
    throw TestFailure("unknown Relin2 test case: " + name);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Relin2 test failure: expected exactly one case name\n";
        return 2;
    }

    try {
        ResolveTest(argv[1])();
        lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
        std::cout << "Relin2 case passed: " << argv[1] << '\n';
        return 0;
    }
    catch (const TestFailure& failure) {
        std::cerr << "Relin2 test failure: " << failure.what() << '\n';
        return 1;
    }
    catch (const std::exception& exception) {
        std::cerr << "Relin2 unexpected exception: " << exception.what() << '\n';
        return 1;
    }
}
