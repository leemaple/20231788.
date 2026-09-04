#include "openfhe_2023_1788/double_ckks.h"

#include <type_traits>

namespace {

using openfhe_2023_1788::CiphertextPair;
using openfhe_2023_1788::DoubleCKKS;

using SubSignature = CiphertextPair (DoubleCKKS::*)(const CiphertextPair&, const CiphertextPair&) const;

static_assert(std::is_same_v<decltype(&DoubleCKKS::Sub), SubSignature>,
              "DoubleCKKS::Sub public signature changed");

}  // namespace

int main() {
    return 0;
}
