// Frozen before implementation. See tests/data/paper_h128_profile.json.
#include "openfhe_2023_1788/paper_h128_client_keypair.h"
#include "scheme/ckksrns/ckksrns-scheme.h"

#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#if NATIVEINT != 64 || MATHBACKEND != 4 || !defined(HAVE_INT128)
#error "Frozen h128 diagnostic requires NATIVEINT=64, MATHBACKEND=4 and HAVE_INT128"
#endif
static_assert(MAX_MODULUS_SIZE == 60, "Frozen auxiliary-prime contract");

namespace {
using namespace lbcrypto;
using Context = CryptoContext<DCRTPoly>;
using ContextImpl = CryptoContextImpl<DCRTPoly>;
using Params = CryptoParametersCKKSRNS;
using Basis = std::shared_ptr<DCRTPoly::Params>;
using Pair = KeyPair<DCRTPoly>;
using openfhe_2023_1788::CreateFixedQH128ClientKeyPair;

constexpr std::size_t kN = 256;
constexpr std::size_t kSlots = 128;
constexpr double kRoundtripTolerance = 1e-5;
constexpr double kSquareTolerance = 1e-5;
// Independently derived from the pin, never populated from a runtime context.
constexpr std::array<std::uint64_t, 3> kQ{{1125899906826241ULL, 1099511603713ULL, 1099511630849ULL}};
constexpr std::array<std::uint64_t, 3> kQRoots{{5834101087838ULL, 694658335ULL, 322807922ULL}};
constexpr std::array<std::uint64_t, 1> kP{{1152921504606844417ULL}};
constexpr std::array<std::uint64_t, 1> kPRoots{{11821407031913010ULL}};
const std::array<std::complex<double>, 8> kInput{{
    {0.0, 0.0}, {0.25, 0.0}, {-0.5, 0.0}, {0.0, 0.25},
    {0.25, 0.125}, {-0.25, 0.25}, {0.125, -0.25}, {-0.125, -0.125}}};
const std::array<std::complex<double>, 8> kSquare{{
    {0.0, 0.0}, {0.0625, 0.0}, {0.25, 0.0}, {-0.0625, 0.0},
    {0.046875, 0.0625}, {0.0, -0.125}, {-0.046875, -0.0625}, {0.0, 0.03125}}};

void Check(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);  // Messages contain no key material.
    }
}

Context MakeContext() {
    CCParams<CryptoContextCKKSRNS> p;
    p.SetRingDim(256);
    p.SetBatchSize(128);
    p.SetSecurityLevel(HEStd_NotSet);
    p.SetStandardDeviation(3.19f);
    p.SetSecretKeyDist(SPARSE_TERNARY);
    p.SetDigitSize(0);
    p.SetMaxRelinSkDeg(2);
    p.SetScalingModSize(40);
    p.SetFirstModSize(50);
    p.SetMultiplicativeDepth(2);
    p.SetNumLargeDigits(3);
    p.SetScalingTechnique(FIXEDMANUAL);
    p.SetKeySwitchTechnique(HYBRID);
    p.SetPREMode(NOT_SET);
    p.SetExecutionMode(EXEC_EVALUATION);
    p.SetDecryptionNoiseMode(FIXED_NOISE_DECRYPT);
    p.SetStatisticalSecurity(30);
    p.SetNumAdversarialQueries(1);
    p.SetCompositeDegree(1);
    p.SetRegisterWordSize(64);
    p.SetCKKSDataType(COMPLEX);
    p.SetNoiseEstimate(0.0);
    p.SetInteractiveBootCompressionLevel(SLACK);
    // CKKS disables setters for these defaults; assert rather than bypass them.
    Check(p.GetEncryptionTechnique() == STANDARD && p.GetMultiplicationTechnique() == HPS &&
              p.GetMultipartyMode() == FIXED_NOISE_MULTIPARTY && p.GetThresholdNumOfParties() == 1,
          "frozen CCParams defaults");
    auto ep = std::make_shared<EncodingParamsImpl>(p.GetScalingModSize(), p.GetBatchSize());
    auto cp = std::make_shared<Params>(std::make_shared<DCRTPoly::Params>(), ep,
        p.GetStandardDeviation(), 36.0f, p.GetSecurityLevel(), p.GetDigitSize(),
        p.GetSecretKeyDist(), p.GetMaxRelinSkDeg(), p.GetKeySwitchTechnique(),
        p.GetScalingTechnique(), p.GetEncryptionTechnique(), p.GetMultiplicationTechnique(),
        p.GetPREMode(), p.GetMultipartyMode(), p.GetExecutionMode(), p.GetDecryptionNoiseMode(),
        1, p.GetStatisticalSecurity(), p.GetNumAdversarialQueries(), p.GetThresholdNumOfParties(),
        p.GetInteractiveBootCompressionLevel(), p.GetCompositeDegree(), p.GetRegisterWordSize(),
        p.GetCKKSDataType());
    cp->SetNoiseScale(1);
    cp->SetFloodingDistributionParameter(0.0);
    cp->SetMultiplicativeDepth(2);
    cp->SetNoiseEstimate(0.0);
    auto scheme = std::make_shared<SchemeCKKSRNS>();
    scheme->SetKeySwitchingTechnique(HYBRID);
    // Explicit public parameter generator: no uninspected ComputeNumLargeDigits helper.
    Check(scheme->ParamsGenCKKSRNS(cp, 512, 3, 40, 50, 3, SLACK), "parameter generation");
    auto cc = CryptoContextFactory<DCRTPoly>::GetContext(cp, scheme, CKKSRNS_SCHEME);
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);
    return cc;  // All construction/precomputation is finished before the adapter call.
}

void CheckBasis(const Basis& basis, const std::vector<std::uint64_t>& moduli,
                const std::vector<std::uint64_t>& roots) {
    Check(basis != nullptr && basis->GetCyclotomicOrder() == 512 &&
              basis->GetRingDimension() == kN && basis->GetParams().size() == moduli.size(),
          "frozen basis envelope");
    BigInteger product(1);
    for (std::size_t i = 0; i < moduli.size(); ++i) {
        const auto& t = basis->GetParams()[i];
        Check(t != nullptr && t->GetCyclotomicOrder() == 512 && t->GetRingDimension() == kN &&
                  t->GetModulus() == NativeInteger(moduli[i]) &&
                  t->GetRootOfUnity() == NativeInteger(roots[i]), "frozen modulus/root order");
        product *= BigInteger(moduli[i]);
    }
    Check(product == basis->GetModulus(), "frozen composite modulus");
}

void CheckProfile(const Context& cc) {
    const auto cp = std::dynamic_pointer_cast<Params>(cc->GetCryptoParameters());
    Check(cp != nullptr && cc->getSchemeId() == CKKSRNS_SCHEME &&
              std::dynamic_pointer_cast<SchemeCKKSRNS>(cc->GetScheme()) != nullptr, "CKKS types");
    const std::vector<std::uint64_t> q(kQ.begin(), kQ.end()), qr(kQRoots.begin(), kQRoots.end());
    CheckBasis(cp->GetElementParams(), q, qr);
    CheckBasis(cp->GetParamsPK(), q, qr);
    CheckBasis(cp->GetParamsP(), {kP[0]}, {kPRoots[0]});
    CheckBasis(cp->GetParamsQP(), {kQ[0], kQ[1], kQ[2], kP[0]},
               {kQRoots[0], kQRoots[1], kQRoots[2], kPRoots[0]});
    Check(cp->GetNumPartQ() == 3 && cp->GetNumPerPartQ() == 1 &&
              cp->GetNumberOfQPartitions() == 3, "frozen partition shape");
    for (std::uint32_t i = 0; i < 3; ++i) {
        CheckBasis(cp->GetParamsPartQ(i), {kQ[i]}, {kQRoots[i]});
    }
    Check(cp->GetDistributionParameter() == 3.19f && cp->GetNoiseScale() == 1 &&
              cp->GetSecretKeyDist() == SPARSE_TERNARY && cp->GetScalingTechnique() == FIXEDMANUAL &&
              cp->GetKeySwitchTechnique() == HYBRID && cp->GetEncryptionTechnique() == STANDARD &&
              cp->GetPREMode() == NOT_SET && cp->GetDecryptionNoiseMode() == FIXED_NOISE_DECRYPT &&
              cp->GetExecutionMode() == EXEC_EVALUATION && cp->GetCKKSDataType() == COMPLEX &&
              cp->GetDigitSize() == 0 && cp->GetMaxRelinSkDeg() == 2 && cp->GetAuxBits() == 60 &&
              cp->GetExtraBits() == 0 && cp->GetCompositeDegree() == 1 &&
              cp->GetRegisterWordSize() == 64 && cp->GetScalingFactorReal(0) == 1099511627776.0 &&
              cc->GetEncodingParams()->GetBatchSize() == kSlots, "frozen profile metadata");
    const auto features = static_cast<std::uint32_t>(PKE | KEYSWITCH | LEVELEDSHE);
    Check(cc->GetScheme()->GetEnabled() == features, "frozen enabled features");
}

// Value snapshots, not shared_ptr-only comparisons. These contain public parameters only.
void AppendBasis(std::ostream& out, const Basis& p) {
    Check(p != nullptr, "snapshot basis");
    out << p.get() << ':' << p->GetCyclotomicOrder() << ':' << p->GetModulus() << ';';
    for (const auto& t : p->GetParams()) {
        out << t.get() << ':' << t->GetCyclotomicOrder() << ':' << t->GetModulus()
            << ':' << t->GetRootOfUnity() << ';';
    }
}
void AppendValues(std::ostream& out, const std::vector<NativeInteger>& values) {
    out << values.size() << ':';
    for (const auto& x : values) { out << x << ','; }
    out << ';';
}
std::string Snapshot(const Context& cc) {
    auto cp = std::dynamic_pointer_cast<Params>(cc->GetCryptoParameters());
    std::ostringstream s;
    s.precision(17);
    s << cc.get() << ':' << cp.get() << ':' << cc->GetScheme().get() << ':'
      << cc->getSchemeId() << ':' << cc->GetScheme()->GetEnabled() << ':'
      << cp->GetDistributionParameter() << ':' << cp->GetNoiseScale() << ':'
      << cp->GetSecretKeyDist() << ':' << cp->GetPREMode() << ':'
      << cp->GetScalingTechnique() << ':' << cp->GetKeySwitchTechnique() << ':'
      << cp->GetEncryptionTechnique() << ':' << cp->GetScalingFactorReal(0) << ':'
      << cp->GetNumPartQ() << ':' << cp->GetNumPerPartQ() << ':';
    AppendBasis(s, cp->GetElementParams()); AppendBasis(s, cp->GetParamsPK());
    AppendBasis(s, cp->GetParamsP()); AppendBasis(s, cp->GetParamsQP());
    AppendValues(s, cp->GetPModq()); AppendValues(s, cp->GetPInvModq());
    AppendValues(s, cp->GetPInvModqPrecon()); AppendValues(s, cp->GetPHatInvModp());
    AppendValues(s, cp->GetPHatInvModpPrecon());
    for (const auto& row : cp->GetPHatModq()) { AppendValues(s, row); }
    // Called only for the independently checked, fully generated frozen context.
    for (std::uint32_t part = 0; part < 3; ++part) {
        AppendBasis(s, cp->GetParamsPartQ(part));
        AppendValues(s, cp->GetPartQlHatInvModq(part, 0));
        AppendValues(s, cp->GetPartQlHatInvModqPrecon(part, 0));
        AppendBasis(s, cp->GetParamsComplPartQ(2, part));
        for (const auto& row : cp->GetPartQlHatModp(2, part)) { AppendValues(s, row); }
    }
    for (std::uint32_t i = 0; i < 2; ++i) {
        AppendValues(s, cp->GetQlQlInvModqlDivqlModq(i));
        AppendValues(s, cp->GetQlQlInvModqlDivqlModqPrecon(i));
        AppendValues(s, cp->GetqlInvModq(i)); AppendValues(s, cp->GetqlInvModqPrecon(i));
    }
    return s.str();
}

void CheckElement(const DCRTPoly& element) {
    Check(!element.IsEmpty() && element.GetFormat() == Format::EVALUATION &&
              element.GetAllElements().size() == 3, "returned full-Q evaluation element");
    CheckBasis(element.GetParams(), std::vector<std::uint64_t>(kQ.begin(), kQ.end()),
               std::vector<std::uint64_t>(kQRoots.begin(), kQRoots.end()));
    for (std::size_t j = 0; j < 3; ++j) {
        const auto& t = element.GetElementAtIndex(j);
        Check(t.GetLength() == kN && t.GetFormat() == Format::EVALUATION &&
                  t.GetModulus() == NativeInteger(kQ[j]) &&
                  t.GetParams()->GetRootOfUnity() == NativeInteger(kQRoots[j]), "returned tower");
    }
}
void CheckPair(const Context& cc, const Pair& keys) {
    Check(keys.good() && keys.publicKey->GetCryptoContext().get() == cc.get() &&
              keys.secretKey->GetCryptoContext().get() == cc.get() &&
              !keys.secretKey->GetKeyTag().empty() &&
              keys.secretKey->GetKeyTag() == keys.publicKey->GetKeyTag(), "pair identity");
    CheckElement(keys.secretKey->GetPrivateElement());
    const auto& pk = keys.publicKey->GetPublicElements();
    Check(pk.size() == 2, "public vector length");
    CheckElement(pk[0]); CheckElement(pk[1]);
    auto coefficientCopy = keys.secretKey->GetPrivateElement();
    coefficientCopy.SetFormat(Format::COEFFICIENT);
    std::vector<int> signedSupport(kN);
    for (std::size_t tower = 0; tower < 3; ++tower) {
        std::size_t nonzero = 0, plus = 0;
        const auto& values = coefficientCopy.GetElementAtIndex(tower).GetValues();
        for (std::size_t i = 0; i < kN; ++i) {
            int sign = 0;
            if (values[i] == NativeInteger(1)) { sign = 1; ++plus; }
            else if (values[i] == NativeInteger(kQ[tower] - 1)) { sign = -1; }
            else { Check(values[i] == NativeInteger(0), "secret is signed ternary"); }
            if (sign != 0) { ++nonzero; }
            if (tower == 0) { signedSupport[i] = sign; }
            else { Check(signedSupport[i] == sign, "common signed support across Q"); }
        }
        Check(nonzero == 128 && plus >= 63 && plus <= 65, "exact h128 and sign balance");
    }
}

std::vector<std::complex<double>> Input() {
    std::vector<std::complex<double>> input(kSlots);
    for (std::size_t i = 0; i < kSlots; ++i) { input[i] = kInput[i % kInput.size()]; }
    return input;
}
void CheckDecoded(const Context& cc, const PrivateKey<DCRTPoly>& sk,
                  const Ciphertext<DCRTPoly>& ciphertext, bool squared) {
    Plaintext decoded;
    Check(cc->Decrypt(sk, ciphertext, &decoded).isValid && decoded != nullptr, "official decryption");
    decoded->SetLength(kSlots);
    const auto& actual = decoded->GetCKKSPackedValue();
    Check(actual.size() == kSlots, "decoded full-packed length");
    for (std::size_t i = 0; i < kSlots; ++i) {
        const auto expected = squared ? kSquare[i % kSquare.size()] : kInput[i % kInput.size()];
        Check(std::isfinite(actual[i].real()) && std::isfinite(actual[i].imag()) &&
                  std::abs(actual[i] - expected) <= (squared ? kSquareTolerance : kRoundtripTolerance),
              "frozen key-consistency smoke tolerance");
    }
}

void ValidPath() {
    const auto cc = MakeContext();
    CheckProfile(cc);
    const auto before = Snapshot(cc);
    const auto cacheBefore = ContextImpl::GetAllEvalMultKeys();
    const auto existingPrivate = std::make_shared<PrivateKeyImpl<DCRTPoly>>(cc);
    const auto existingPublic = std::make_shared<PublicKeyImpl<DCRTPoly>>(cc, existingPrivate->GetKeyTag());
    const auto existingTag = existingPrivate->GetKeyTag();
    const auto keys = CreateFixedQH128ClientKeyPair(cc);
    Check(keys.secretKey.get() != existingPrivate.get() && keys.publicKey.get() != existingPublic.get() &&
              keys.secretKey->GetKeyTag() != existingTag && existingPrivate->GetKeyTag() == existingTag &&
              existingPublic->GetKeyTag() == existingTag, "fresh objects and no existing tag replacement");
    CheckPair(cc, keys);
    Check(ContextImpl::GetAllEvalMultKeys() == cacheBefore, "adapter did not insert an eval key");
    const auto skBefore = keys.secretKey->GetPrivateElement();
    const auto pkBefore = keys.publicKey->GetPublicElements();
    const auto plaintext = cc->MakeCKKSPackedPlaintext(Input(), 1, 0, nullptr, 128);
    const auto ciphertext = cc->Encrypt(keys.publicKey, plaintext);
    Check(ciphertext != nullptr && ciphertext->GetKeyTag() == keys.secretKey->GetKeyTag(), "official encryption");
    CheckDecoded(cc, keys.secretKey, ciphertext, false);
    cc->EvalMultKeyGen(keys.secretKey);
    Check(ContextImpl::GetAllEvalMultKeys().count(keys.secretKey->GetKeyTag()) == 1,
          "returned secret owns its eval key");
    const auto square = cc->EvalMult(ciphertext, ciphertext);
    Check(square != nullptr && square->GetElements().size() == 2 &&
              square->GetKeyTag() == keys.secretKey->GetKeyTag(), "official relinearized square");
    CheckDecoded(cc, keys.secretKey, square, true);
    Check(keys.secretKey->GetPrivateElement() == skBefore &&
              keys.publicKey->GetPublicElements() == pkBefore, "caller key immutability");
    Check(Snapshot(cc) == before, "caller context/bases/tables immutability");
    ContextImpl::ClearEvalMultKeys(keys.secretKey->GetKeyTag());
    Check(ContextImpl::GetAllEvalMultKeys() == cacheBefore, "only owned eval key cleanup");
    std::cout << "valid-path assertions passed (diagnostic, not paper evidence)\n";
}
}  // namespace

int main() {
    ValidPath();
    return 0;
}
