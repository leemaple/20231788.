#include "openfhe_2023_1788/paper_h128_client_keypair.h"
#include "scheme/ckksrns/ckksrns-scheme.h"

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace openfhe_2023_1788 {
namespace {
using namespace lbcrypto;
using Basis = std::shared_ptr<DCRTPoly::Params>;

void Require(bool condition, const char* category) {
    if (!condition) {
        throw std::invalid_argument(std::string("fixed-Q h128: ") + category);
    }
}

bool SameBasis(const Basis& a, const Basis& b) {
    if (!a || !b || a->GetCyclotomicOrder() != b->GetCyclotomicOrder() ||
        a->GetRingDimension() != b->GetRingDimension() || a->GetModulus() != b->GetModulus() ||
        a->GetParams().size() != b->GetParams().size()) {
        return false;
    }
    for (std::size_t i = 0; i < a->GetParams().size(); ++i) {
        const auto& x = a->GetParams()[i];
        const auto& y = b->GetParams()[i];
        if (!x || !y || x->GetCyclotomicOrder() != y->GetCyclotomicOrder() ||
            x->GetRingDimension() != y->GetRingDimension() || x->GetModulus() != y->GetModulus() ||
            x->GetRootOfUnity() != y->GetRootOfUnity()) {
            return false;
        }
    }
    return true;
}

bool FullQEvaluation(const DCRTPoly& element, const Basis& q) {
    if (element.IsEmpty() || element.GetFormat() != Format::EVALUATION ||
        !SameBasis(element.GetParams(), q) || element.GetAllElements().size() != q->GetParams().size()) {
        return false;
    }
    for (std::size_t i = 0; i < q->GetParams().size(); ++i) {
        const auto& tower = element.GetElementAtIndex(i);
        const auto& p = q->GetParams()[i];
        if (tower.GetLength() != q->GetRingDimension() || tower.GetFormat() != Format::EVALUATION ||
            !tower.GetParams() || tower.GetParams()->GetCyclotomicOrder() != q->GetCyclotomicOrder() ||
            tower.GetModulus() != p->GetModulus() || tower.GetParams()->GetRootOfUnity() != p->GetRootOfUnity()) {
            return false;
        }
    }
    return true;
}

std::shared_ptr<CryptoParametersCKKSRNS> ValidateContext(const CryptoContext<DCRTPoly>& context) {
    Require(context != nullptr, "context");
    const auto cp = std::dynamic_pointer_cast<CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Require(cp != nullptr && context->getSchemeId() == CKKSRNS_SCHEME &&
                std::dynamic_pointer_cast<SchemeCKKSRNS>(context->GetScheme()) != nullptr, "scheme");
    const auto q = cp->GetElementParams();
    Require(q != nullptr && !q->GetParams().empty(), "Q basis");
    const auto n = q->GetRingDimension();
    Require(n >= 128 && (n & (n - 1)) == 0 && q->GetCyclotomicOrder() / 2 == n &&
                q->GetCyclotomicOrder() % 2 == 0, "ring");
    return cp;
}
}  // namespace

lbcrypto::KeyPair<lbcrypto::DCRTPoly> CreateFixedQH128ClientKeyPair(
    const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context) {
    using namespace lbcrypto;
    const auto cp = ValidateContext(context);
    const auto q = cp->GetElementParams();
    const DCRTPoly::TugType sampler;
    DCRTPoly secret(sampler, q, Format::EVALUATION, 128);
    auto sk = std::make_shared<PrivateKeyImpl<DCRTPoly>>(context);
    sk->SetPrivateElement(std::move(secret));  // Exactly once, on a newly allocated key.
    const auto tag = sk->GetKeyTag();
    auto zero = context->GetScheme()->EncryptZeroCore(sk);  // Official full-Q zero encryption.
    if (!zero || zero->size() != 2 || !FullQEvaluation(sk->GetPrivateElement(), q) ||
        !FullQEvaluation((*zero)[0], q) || !FullQEvaluation((*zero)[1], q)) {
        throw std::logic_error("fixed-Q h128: invalid generated elements");
    }
    auto pk = std::make_shared<PublicKeyImpl<DCRTPoly>>(context, tag);
    pk->SetPublicElements(std::move(*zero));  // Keep the official (a*s+e, -a) order.
    if (tag.empty() || sk->GetCryptoContext().get() != context.get() ||
        pk->GetCryptoContext().get() != context.get() || sk->GetKeyTag() != tag ||
        pk->GetKeyTag() != tag || pk->GetPublicElements().size() != 2) {
        throw std::logic_error("fixed-Q h128: invalid generated key identity");
    }
    return KeyPair<DCRTPoly>(pk, sk);  // No key/ciphertext/eval-key escapes before this point.
}
}  // namespace openfhe_2023_1788
