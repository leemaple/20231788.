#include "openfhe_2023_1788/double_ckks.h"

#include <type_traits>

namespace {

using openfhe_2023_1788::CiphertextPair;
using openfhe_2023_1788::DoubleCKKS;
using openfhe_2023_1788::PairLifecycle;

using RS2Signature = CiphertextPair (DoubleCKKS::*)(const CiphertextPair&) const;

static_assert(PairLifecycle::RefreshRequired != PairLifecycle::ReadyForRS2,
              "RS2 must end at the explicit RefreshRequired boundary");
static_assert(std::is_same_v<decltype(&DoubleCKKS::RS2), RS2Signature>,
              "DoubleCKKS::RS2 public signature changed");

}  // namespace

int main() {
    return 0;
}
