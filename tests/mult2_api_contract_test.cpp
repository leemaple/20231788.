#include "openfhe_2023_1788/double_ckks.h"

#include <type_traits>

namespace {

using openfhe_2023_1788::CiphertextPair;
using openfhe_2023_1788::DoubleCKKS;

using Mult2Signature = CiphertextPair (DoubleCKKS::*)(const CiphertextPair&,
                                                    const CiphertextPair&) const;

static_assert(std::is_same_v<decltype(&DoubleCKKS::Mult2), Mult2Signature>,
              "DoubleCKKS::Mult2 public signature changed");

}  // namespace

int main() {
    return 0;
}
