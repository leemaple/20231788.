#include "openfhe_2023_1788/double_ckks.h"

#include <type_traits>

namespace {

using openfhe_2023_1788::CiphertextPair;
using openfhe_2023_1788::DoubleCKKS;
using openfhe_2023_1788::PairLifecycle;
using openfhe_2023_1788::PaperScaleDescriptor;
using openfhe_2023_1788::TensorCiphertextPair;

using Relin2Signature = CiphertextPair (DoubleCKKS::*)(const TensorCiphertextPair&) const;

static_assert(PairLifecycle::ReadyForRS2 != PairLifecycle::ReadyForFirstMult,
              "Relin2 requires the ReadyForRS2 lifecycle");
static_assert(
    std::is_same_v<decltype(PaperScaleDescriptor::approximateRecombinedLogicalScalingFactor), long double>,
    "PaperScaleDescriptor requires an independent recombined logical scale");
static_assert(std::is_same_v<decltype(&DoubleCKKS::Relin2), Relin2Signature>,
              "DoubleCKKS::Relin2 public signature changed");

}  // namespace

int main() {
    return 0;
}
