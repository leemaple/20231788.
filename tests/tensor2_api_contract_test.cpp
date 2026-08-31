#include "openfhe_2023_1788/double_ckks.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace {

using openfhe_2023_1788::CiphertextPair;
using openfhe_2023_1788::DoubleCKKS;
using openfhe_2023_1788::ReadOnlyCiphertext;
using openfhe_2023_1788::TensorCiphertextPair;
using openfhe_2023_1788::TensorScaleDescriptor;

using Tensor2Signature = TensorCiphertextPair (DoubleCKKS::*)(const CiphertextPair&, const CiphertextPair&) const;

static_assert(std::is_same_v<decltype(&DoubleCKKS::Tensor2), Tensor2Signature>,
              "DoubleCKKS::Tensor2 public signature changed");
static_assert(std::is_same_v<decltype(std::declval<const TensorCiphertextPair&>().GetHigh()), ReadOnlyCiphertext>);
static_assert(std::is_same_v<decltype(std::declval<const TensorCiphertextPair&>().GetLow()), ReadOnlyCiphertext>);
static_assert(std::is_same_v<decltype(std::declval<const TensorCiphertextPair&>().GetTensorScale()),
                             const TensorScaleDescriptor&>);
static_assert(std::is_same_v<decltype(std::declval<const TensorCiphertextPair&>().GetComponentCount()), std::size_t>);

}  // namespace

int main() {
    return 0;
}
