#include "openfhe_2023_1788/paper_h128_client_keypair.h"
#include "scheme/ckksrns/ckksrns-scheme.h"
#include "math/nbtheory.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <typeinfo>
#include <string>
#include <utility>

#if NATIVEINT != 64 || MATHBACKEND != 4 || !defined(HAVE_INT128)
#error "Frozen h128 adapter requires NATIVEINT=64, MATHBACKEND=4 and HAVE_INT128"
#endif
static_assert(MAX_MODULUS_SIZE == 60, "Frozen h128 auxiliary-prime contract");

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

// Inspect actual public basis values, not just a product or a requested profile.
void ValidateBasis(const Basis& basis, std::uint32_t order, std::uint32_t n, const char* category) {
    Require(basis != nullptr && !basis->GetParams().empty() &&
                basis->GetCyclotomicOrder() == order && basis->GetRingDimension() == n, category);
    BigInteger product(1);
    for (std::size_t i = 0; i < basis->GetParams().size(); ++i) {
        const auto& tower = basis->GetParams()[i];
        Require(tower != nullptr && tower->GetCyclotomicOrder() == order && tower->GetRingDimension() == n,
                category);
        const auto& modulus = tower->GetModulus();
        const auto& root = tower->GetRootOfUnity();
        Require(modulus > NativeInteger(2) && modulus.GetMSB() <= MAX_MODULUS_SIZE &&
                    (modulus - NativeInteger(1)).Mod(NativeInteger(order)) == NativeInteger(0) &&
                    root > NativeInteger(1) && root < modulus && MillerRabinPrimalityTest(modulus) &&
                    root.ModExp(NativeInteger(n), modulus) == modulus - NativeInteger(1), category);
        for (std::size_t j = 0; j < i; ++j) {
            Require(modulus != basis->GetParams()[j]->GetModulus(), category);
        }
        product *= BigInteger(modulus);
    }
    Require(product == basis->GetModulus(), category);
}

void ValidateImplementations(const CryptoContext<DCRTPoly>& context) {
    const auto features = static_cast<std::uint32_t>(PKE | KEYSWITCH | LEVELEDSHE);
    Require((context->GetScheme()->GetEnabled() & features) == features, "features");
    // This pin has no public component getter. Its public SchemeBase stream
    // exposes the actual pointed-to RTTI (base-scheme.h:1538-1548), not mode labels.
    // Derive ABI spellings in this process; never hard-code compiler type names.
    std::ostringstream observed;
    observed << *context->GetScheme();
    const auto state = observed.str();
    Require(state.find(std::string(", PKE ") + typeid(PKECKKSRNS).name() + ", KeySwitch ") != std::string::npos &&
                state.find(std::string(", KeySwitch ") + typeid(KeySwitchHYBRID).name() + ", PRE ") != std::string::npos &&
                state.find(std::string(", LeveledSHE ") + typeid(LeveledSHECKKSRNS).name() + ", AdvancedSHE ") !=
                    std::string::npos, "implementation");
}

void ValidatePrecomputation(const std::shared_ptr<CryptoParametersCKKSRNS>& cp, const Basis& q) {
    const auto order = q->GetCyclotomicOrder(), n = q->GetRingDimension();
    const auto p = cp->GetParamsP(), qp = cp->GetParamsQP();
    ValidateBasis(p, order, n, "P basis");
    for (const auto& pt : p->GetParams()) {
        for (const auto& qt : q->GetParams()) {
            Require(pt->GetModulus() != qt->GetModulus(), "P basis");
        }
    }
    ValidateBasis(qp, order, n, "QP basis");
    const auto sizeQ = q->GetParams().size(), sizeP = p->GetParams().size();
    Require(qp->GetParams().size() == sizeQ + sizeP, "QP basis");
    for (std::size_t i = 0; i < sizeQ + sizeP; ++i) {
        const auto& expected = i < sizeQ ? q->GetParams()[i] : p->GetParams()[i - sizeQ];
        const auto& actual = qp->GetParams()[i];
        Require(actual->GetModulus() == expected->GetModulus() &&
                    actual->GetRootOfUnity() == expected->GetRootOfUnity(), "QP basis");
    }
    const auto digits = cp->GetNumPartQ(), auxBits = cp->GetAuxBits();
    Require(digits > 0 && digits <= sizeQ && auxBits > 0 && auxBits <= MAX_MODULUS_SIZE, "partitions");
    const auto width = sizeQ / digits + (sizeQ % digits != 0 ? 1 : 0);
    Require(cp->GetNumPerPartQ() == width && cp->GetNumberOfQPartitions() == digits &&
                (sizeQ - 1) / width + 1 == digits, "partitions");
    std::uint32_t maxBits = 0;
    for (std::uint32_t part = 0; part < digits; ++part) {
        // GetParamsPartQ is unchecked upstream: validate its exposed count first.
        const auto& subset = cp->GetParamsPartQ(part);
        ValidateBasis(subset, order, n, "partitions");
        const auto start = static_cast<std::size_t>(part) * width;
        const auto length = std::min(width, sizeQ - start);
        Require(subset->GetParams().size() == length, "partitions");
        for (std::size_t i = 0; i < length; ++i) {
            Require(subset->GetParams()[i]->GetModulus() == q->GetParams()[start + i]->GetModulus() &&
                        subset->GetParams()[i]->GetRootOfUnity() == q->GetParams()[start + i]->GetRootOfUnity(),
                    "partitions");
        }
        maxBits = std::max(maxBits, static_cast<std::uint32_t>(subset->GetModulus().GetLengthForBase(2)));
    }
    Require(sizeP == maxBits / auxBits + (maxBits % auxBits != 0 ? 1 : 0), "partitions");
    Require(cp->GetPModq().size() == sizeQ && cp->GetPInvModq().size() == sizeQ &&
                cp->GetPInvModqPrecon().size() == sizeQ && cp->GetPHatInvModp().size() == sizeP &&
                cp->GetPHatInvModpPrecon().size() == sizeP && cp->GetPHatModq().size() == sizeP &&
                cp->GetModqBarrettMu().size() == sizeQ, "HYBRID tables");
    for (const auto& row : cp->GetPHatModq()) {
        Require(row.size() == sizeQ, "HYBRID tables");
    }
    for (std::size_t i = 0; i < sizeQ; ++i) {
        const auto& qi = q->GetParams()[i]->GetModulus();
        const auto expected = NativeInteger(p->GetModulus().Mod(BigInteger(qi)).ConvertToInt());
        Require(cp->GetPModq()[i] == expected && cp->GetPInvModq()[i] < qi &&
                    expected.ModMul(cp->GetPInvModq()[i], qi) == NativeInteger(1), "HYBRID tables");
    }
    const double scale = cp->GetScalingFactorReal(0);  // Safe scalar for FIXEDMANUAL.
    Require(std::isfinite(scale) && scale > 0.0, "LEVELEDSHE tables");
    // Do not index hidden outer dimensions of complementary/rescale tables.
    // These checks are necessary public structural checks, not an all-table proof.
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
    Require(cp->GetEncryptionTechnique() == STANDARD, "encryption");
    Require(cp->GetScalingTechnique() == FIXEDMANUAL, "scaling");
    Require(cp->GetKeySwitchTechnique() == HYBRID, "key switching");
    Require(cp->GetPREMode() == NOT_SET, "PRE");
    Require(cp->GetNoiseScale() == 1, "noise scale");
    Require(cp->GetSecretKeyDist() == SPARSE_TERNARY, "secret distribution");
    Require(std::isfinite(cp->GetDistributionParameter()) && cp->GetDistributionParameter() > 0.0f, "sigma");
    ValidateImplementations(context);
    ValidateBasis(q, q->GetCyclotomicOrder(), n, "Q basis");
    Require(SameBasis(cp->GetParamsPK(), q), "public-key basis");
    ValidatePrecomputation(cp, q);
    return cp;
}

void ValidateFreshTag(const std::string& tag) {
    Require(!tag.empty() && CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys().count(tag) == 0 &&
                CryptoContextImpl<DCRTPoly>::GetAllEvalAutomorphismKeys().count(tag) == 0, "key tag");
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
    ValidateFreshTag(tag);  // Source-reviewed defensive branch; no injection/registry.
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
    ValidateFreshTag(tag);
    return KeyPair<DCRTPoly>(pk, sk);  // No key/ciphertext/eval-key escapes before this point.
}
}  // namespace openfhe_2023_1788
