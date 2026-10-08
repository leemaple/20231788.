#include "openfhe_2023_1788/double_ckks.h"
#include "openfhe_2023_1788/repeated_mult2.h"
#include "precision_dcp_rcb_fixture.h"
#include "../src/mult2_schedule.h"

#include <boost/multiprecision/cpp_int.hpp>
#include <array>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {
using namespace openfhe_2023_1788;
using Int = boost::multiprecision::cpp_int;
using DCRT = lbcrypto::DCRTPoly;
using Native = lbcrypto::NativeInteger;
using Context = lbcrypto::CryptoContext<DCRT>;
constexpr std::array<Mult2Backend, 4> kBackends{Mult2Backend::Reference,
    Mult2Backend::SharedStaged, Mult2Backend::TailDigit, Mult2Backend::TailWide};

void Check(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}
Int Mod(Int a, const Int& q) { a %= q; return a < 0 ? a + q : a; }
Int Inverse(Int a, const Int& q) {
    a = Mod(a, q);
    Int r = q, next = a, t = 0, nt = 1;
    while (next != 0) {
        const Int quotient = r / next;
        const Int rr = r - quotient * next, tt = t - quotient * nt;
        r = next; next = rr; t = nt; nt = tt;
    }
    Check(r == 1, "oracle inverse");
    return Mod(t, q);
}
Int Floor(Int n, const Int& d) {
    Int quotient = n / d;
    if (n < 0 && n % d != 0) --quotient;
    return quotient;
}
Int Round(const Int& n, const Int& d) { return Floor(2 * n + d, 2 * d); }
Int Big(const Native& value) { return Int(value.ConvertToInt()); }

const char* Name(Mult2Backend backend) {
    switch (backend) {
        case Mult2Backend::Reference: return "Reference";
        case Mult2Backend::SharedStaged: return "SharedStaged";
        case Mult2Backend::TailDigit: return "TailDigit";
        case Mult2Backend::TailWide: return "TailWide";
    }
    throw std::runtime_error("unknown backend in report");
}
const char* Name(Mult2Fallback fallback) {
    switch (fallback) {
        case Mult2Fallback::None: return "None";
        case Mult2Fallback::NonNative64: return "NonNative64";
        case Mult2Fallback::WideUnavailable: return "WideUnavailable";
        case Mult2Fallback::ModulusRange: return "ModulusRange";
        case Mult2Fallback::NonPrimeModulus: return "NonPrimeModulus";
        case Mult2Fallback::DuplicateModulus: return "DuplicateModulus";
        case Mult2Fallback::InvalidRoot: return "InvalidRoot";
    }
    throw std::runtime_error("unknown fallback in report");
}
Mult2Fallback ExpectedFallback(Mult2Backend backend) {
    if (backend == Mult2Backend::Reference) return Mult2Fallback::None;
#if NATIVEINT != 64
    return Mult2Fallback::NonNative64;
#elif !defined(__SIZEOF_INT128__) || defined(OPENFHE_2023_1788_DISABLE_WIDE_MULT2)
    return backend == Mult2Backend::TailWide ? Mult2Fallback::WideUnavailable : Mult2Fallback::None;
#else
    return Mult2Fallback::None;
#endif
}
void Trace(const Mult2BackendTrace& trace, Mult2Backend requested, const char* label) {
    const auto fallback = ExpectedFallback(requested);
    Check(trace.completed && trace.requested == requested && trace.fallback == fallback &&
              trace.executed == (fallback == Mult2Fallback::None ? requested : Mult2Backend::Reference),
          "unexpected actual backend/fallback");
    std::cout << "route," << label << ',' << Name(trace.requested) << ','
              << Name(trace.executed) << ',' << Name(trace.fallback) << '\n';
}

class Probe final : public lbcrypto::Metadata {
public:
    explicit Probe(int v) : value(v) {}
    std::shared_ptr<lbcrypto::Metadata> Clone() const override { return std::make_shared<Probe>(value); }
    bool operator==(const lbcrypto::Metadata& other) const override {
        const auto* p = dynamic_cast<const Probe*>(&other);
        return p && p->value == value;
    }
    int value;
};

// Values copied into text: shared Params and metadata payloads cannot hide an
// in-place mutation by changing both an input and a shallow Clone snapshot.
std::string Snapshot(const ReadOnlyCiphertext& c) {
    std::ostringstream s;
    s.precision(24);
    s << c.get() << ':' << c->GetCryptoContext().get() << ':' << c->GetKeyTag() << ':'
      << c->GetEncodingType() << ':' << c->GetSlots() << ':' << c->GetLevel() << ':'
      << c->GetNoiseScaleDeg() << ':' << c->GetScalingFactor() << ':'
      << c->GetScalingFactorInt() << ':' << c->GetHopLevel() << ':' << c->GetMetadataMap().get();
    for (const auto& kv : *c->GetMetadataMap()) {
        const auto* probe = dynamic_cast<const Probe*>(kv.second.get());
        Check(probe != nullptr, "unexpected metadata payload in fixture");
        s << ':' << kv.first << ':' << kv.second.get() << ':' << probe->value;
    }
    for (const auto& poly : c->GetElements()) {
        s << '|' << poly.GetFormat() << ':' << poly.GetParams()->GetCyclotomicOrder()
          << ':' << poly.GetParams()->GetModulus();
        for (const auto& params : poly.GetParams()->GetParams()) {
            s << ':' << params->GetModulus() << ':' << params->GetRootOfUnity()
              << ':' << params->GetCyclotomicOrder();
        }
        for (const auto& tower : poly.GetAllElements()) {
            s << '/' << tower.GetFormat() << ':' << tower.GetModulus() << ':'
              << tower.GetRootOfUnity() << ':' << tower.GetParams()->GetCyclotomicOrder()
              << ':' << tower.GetValues().GetModulus() << ':' << tower.GetLength();
            for (std::size_t j = 0; j < tower.GetLength(); ++j) s << ',' << tower.GetValues()[j];
        }
    }
    return s.str();
}
void SameCiphertext(const ReadOnlyCiphertext& a, const ReadOnlyCiphertext& b) {
    Check(*a == *b, "ciphertext coefficients/metadata differ");
    Check(a->GetMetadataMap()->size() == b->GetMetadataMap()->size(), "metadata size");
    auto j = b->GetMetadataMap()->begin();
    for (const auto& kv : *a->GetMetadataMap()) {
        Check(j != b->GetMetadataMap()->end() && kv.first == j->first && kv.second == j->second,
              "metadata keys or shallow payload identity differ");
        ++j;
    }
    for (std::size_t i = 0; i < a->GetElements().size(); ++i) {
        Check(*a->GetElements()[i].GetParams() == *b->GetElements()[i].GetParams(), "DCRT Params differ");
    }
}
void SamePair(const CiphertextPair& a, const CiphertextPair& b) {
    SameCiphertext(a.GetHigh(), b.GetHigh());
    SameCiphertext(a.GetLow(), b.GetLow());
    const auto& x = a.GetPaperScale(); const auto& y = b.GetPaperScale();
    Check(a.GetContextIdentity() == b.GetContextIdentity() && a.GetDivisor() == b.GetDivisor() &&
          a.GetOrderedModuli() == b.GetOrderedModuli() && a.GetLevel() == b.GetLevel() &&
          a.GetRecordedScalingFactor() == b.GetRecordedScalingFactor() &&
          a.GetNoiseScaleDegree() == b.GetNoiseScaleDegree() && a.GetLifecycle() == b.GetLifecycle() &&
          a.GetKeyTag() == b.GetKeyTag() && a.GetSlots() == b.GetSlots() &&
          a.GetFormat() == b.GetFormat() && a.GetComponentCount() == b.GetComponentCount() &&
          a.GetRepeatedReceipt() == b.GetRepeatedReceipt() &&
          x.inputRecordedScalingFactor == y.inputRecordedScalingFactor && x.divisor == y.divisor &&
          x.approximateLogicalScalingFactor == y.approximateLogicalScalingFactor &&
          x.approximateRecombinedLogicalScalingFactor == y.approximateRecombinedLogicalScalingFactor,
          "pair state differs");
    Check(a.GetHigh()->GetMetadataMap() != a.GetLow()->GetMetadataMap(), "output map alias");
}

std::vector<Int> Reconstruct(const DCRT& input) {
    auto poly = input; poly.SetFormat(Format::COEFFICIENT);
    Int product = 1;
    for (const auto& tower : poly.GetAllElements()) product *= Big(tower.GetModulus());
    std::vector<Int> result(poly.GetLength());
    for (const auto& tower : poly.GetAllElements()) {
        const Int p = Big(tower.GetModulus()), partial = product / p;
        const Int weight = partial * Inverse(partial, p);
        for (std::size_t j = 0; j < result.size(); ++j) result[j] += Big(tower.GetValues()[j]) * weight;
    }
    for (auto& value : result) value = Mod(value, product);
    return result;
}
void KernelOracle(const std::vector<DCRT>& a, const std::vector<DCRT>& b, const char* label) {
    const Int d = Big(a[0].GetAllElements().back().GetModulus());
    const Int q = Big(b[0].GetAllElements().back().GetModulus());
    const auto beforeA = a, beforeB = b;
    for (const auto backend : kBackends) {
        if (backend == Mult2Backend::Reference || ExpectedFallback(backend) != Mult2Fallback::None) continue;
        Check(detail::Mult2ScheduleSupport(a[0].GetParams(), backend) == Mult2Fallback::None,
              "oracle fixture unsupported");
        auto output = detail::ComputeMult2Schedule(a, b, backend);
        Check(output.first.size() == a.size() && output.second.size() == a.size(), "kernel arity");
        for (std::size_t component = 0; component < a.size(); ++component) {
            const auto av = Reconstruct(a[component]), bv = Reconstruct(b[component]);
            auto h = output.first[component], low = output.second[component];
            Check(h.GetNumOfElements() + 2 == a[component].GetNumOfElements() &&
                  low.GetNumOfElements() == h.GetNumOfElements(), "kernel basis size");
            h.SetFormat(Format::COEFFICIENT); low.SetFormat(Format::COEFFICIENT);
            for (std::size_t i = 0; i < h.GetNumOfElements(); ++i) {
                const Int p = Big(h.GetAllElements()[i].GetModulus());
                for (std::size_t j = 0; j < av.size(); ++j) {
                    const Int firstHigh = Round(av[j], d);
                    const Int firstLow = av[j] - d * firstHigh + bv[j];
                    const Int expectedHigh = Round(firstHigh, q);
                    const Int expectedLow = Round(d * firstHigh + firstLow, q) - d * expectedHigh;
                    Check(Big(h.GetAllElements()[i].GetValues()[j]) == Mod(expectedHigh, p) &&
                          Big(low.GetAllElements()[i].GetValues()[j]) == Mod(expectedLow, p),
                          "independent full-integer oracle mismatch");
                }
            }
        }
        Check(a == beforeA && b == beforeB, "kernel input mutation");
        std::cout << "kernel," << label << ',' << Name(backend) << ",all_coefficients_pass\n";
    }
}

void ControlledOrders(const Context& context) {
    const auto params = std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(
        context->GetCryptoParameters());
    auto basis = params->GetElementParams()->GetParams();
    for (int order = 0; order < 2; ++order) {
        const Int d = Big(basis.back()->GetModulus());
        const Int q = Big(basis[basis.size() - 2]->GetModulus());
        const std::array<Int, 5> da{Int(0), Int(d / 2), Int(-d / 2), Int(1), Int(-1)};
        const std::array<Int, 5> qu{Int(0), Int(q / 2), Int(-q / 2), Int(1), Int(-1)};
        std::vector<DCRT> a, b;
        for (std::size_t component = 0; component < 2; ++component) {
            std::vector<lbcrypto::NativePoly> at, bt;
            for (std::size_t i = 0; i < basis.size(); ++i) {
                lbcrypto::NativePoly ap(basis[i], Format::COEFFICIENT, true);
                lbcrypto::NativeVector av(context->GetRingDimension(), basis[i]->GetModulus());
                lbcrypto::NativeVector bv(context->GetRingDimension(), basis[i]->GetModulus());
                const Int p = Big(basis[i]->GetModulus());
                for (std::size_t j = 0; j < av.GetLength(); ++j) {
                    const Int aa = da[(j + component) % 5] + d * qu[(j / 5 + component) % 5]
                                   + (Int(j) - 64) * d * q;
                    const Int bb = -aa + qu[(j / 25 + component) % 5] + Int(j * j) * q;
                    av[j] = Native(Mod(aa, p).convert_to<std::uint64_t>());
                    bv[j] = Native(Mod(bb, p).convert_to<std::uint64_t>());
                }
                ap.SetValues(std::move(av), Format::COEFFICIENT); ap.SetFormat(Format::EVALUATION);
                at.push_back(std::move(ap));
                if (i + 1 < basis.size()) {
                    lbcrypto::NativePoly bp(basis[i], Format::COEFFICIENT, true);
                    bp.SetValues(std::move(bv), Format::COEFFICIENT); bp.SetFormat(Format::EVALUATION);
                    bt.push_back(std::move(bp));
                }
            }
            a.emplace_back(at); b.emplace_back(bt);
        }
        // Reordering test-owned polynomials is not a reordered HE context.
        KernelOracle(a, b, order == 0 ? "controlled_original_basis" : "controlled_swapped_tails");
        std::cout << "controlled_order," << order << ",d=" << d << ",q=" << q << '\n';
        std::swap(basis[basis.size() - 1], basis[basis.size() - 2]);
    }
}

Context MakeContext(std::uint32_t n, std::uint32_t depth, std::uint32_t bits,
                    lbcrypto::KeySwitchTechnique technique, std::uint32_t digit) {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> p;
    p.SetMultiplicativeDepth(depth); p.SetScalingModSize(bits); p.SetFirstModSize(bits);
    p.SetScalingTechnique(lbcrypto::FIXEDMANUAL); p.SetKeySwitchTechnique(technique);
    p.SetDigitSize(digit); p.SetSecurityLevel(lbcrypto::HEStd_NotSet);
    p.SetRingDim(n); p.SetBatchSize(8);
    auto c = lbcrypto::GenCryptoContext(p);
    c->Enable(lbcrypto::PKE); c->Enable(lbcrypto::KEYSWITCH); c->Enable(lbcrypto::LEVELEDSHE);
    return c;
}
template <class F> void Reject(F action, const char* label) {
    bool rejected = false;
    try { action(); } catch (const std::invalid_argument&) { rejected = true; }
    Check(rejected, std::string("expected rejection: ") + label);
}
void RejectEvery(const DoubleCKKS& module, const CiphertextPair& left,
                 const CiphertextPair& right, const char* label) {
    for (auto backend : kBackends) {
        Mult2BackendTrace trace;
        Reject([&] { module.Mult2WithBackend(left, right, backend, &trace); }, label);
        Check(!trace.completed, "failed call reported completed");
    }
    std::cout << "reject," << label << ",all_backends\n";
}

void Legacy(std::uint32_t n, std::uint32_t depth, std::uint32_t bits,
            lbcrypto::KeySwitchTechnique technique, std::uint32_t digit) {
    auto c = MakeContext(n, depth, bits, technique, digit);
    const auto keys = c->KeyGen(); c->EvalMultKeyGen(keys.secretKey);
    auto input = c->Encrypt(keys.publicKey, c->MakeCKKSPackedPlaintext(
        std::vector<double>{0.0, 0.125, -0.25, 0.0625}, 2, 0));
    input->SetMetadataByKey("origin", std::make_shared<Probe>(17));
    input->SetScalingFactorInt(Native(7)); input->SetHopLevel(3);
    DoubleCKKS module(c);
    const auto pair = module.DCP(input);
    const auto beforeInput = Snapshot(input), beforeH = Snapshot(pair.GetHigh()), beforeL = Snapshot(pair.GetLow());
    const auto reference = module.Mult2(pair, pair);
    const auto tensor = module.Tensor2(pair, pair);
    const auto params = std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(c->GetCryptoParameters());
    // Test-owned construction, followed by the same two ACTUAL key switches.
    // The kernel comparisons below all receive these exact saved A/B objects.
    auto raised = tensor.GetHigh()->Clone();
    auto elements = raised->GetElements();
    for (auto& element : elements) {
        auto towers = element.GetAllElements();
        for (auto& tower : towers) tower *= pair.GetDivisor();
        towers.emplace_back(params->GetElementParams()->GetParams().back(), Format::EVALUATION, true);
        element = DCRT(towers);
    }
    raised->SetElements(std::move(elements)); raised->SetLevel(0);
    lbcrypto::ConstCiphertext<DCRT> raisedConst = raised, lowConst = tensor.GetLow();
    const auto a = c->Relinearize(raisedConst), b = c->Relinearize(lowConst);
    KernelOracle(a->GetElements(), b->GetElements(), "actual_relinearize_outputs");
    if (bits == 59) ControlledOrders(c);
    std::cout << "profile,N=" << n << ",m=" << reference.GetOrderedModuli().size()
              << ",technique=" << technique << ",digit=" << digit << ",Q=";
    for (const auto& p : params->GetElementParams()->GetParams())
        std::cout << p->GetModulus() << ':' << p->GetRootOfUnity() << ';';
    std::cout << '\n';
    Mult2BackendTrace trace;
    for (auto backend : kBackends) {
        const auto actual = module.Mult2WithBackend(pair, pair, backend, &trace);
        SamePair(actual, reference); Trace(trace, backend, "legacy");
        Check(actual.GetHigh()->GetMetadataMap() != pair.GetHigh()->GetMetadataMap() &&
              actual.GetLow()->GetMetadataMap() != pair.GetHigh()->GetMetadataMap(), "input/output map alias");
        Check(Snapshot(input) == beforeInput && Snapshot(pair.GetHigh()) == beforeH &&
                  Snapshot(pair.GetLow()) == beforeL, "legacy input mutation");
    }
    Check(trace.completed, "trace reuse fixture must start with a successful call");
    Reject([&] { module.Mult2WithBackend(pair, pair, static_cast<Mult2Backend>(999), &trace); },
           "unknown_backend_reused_trace");
    Check(!trace.completed && trace.requested == static_cast<Mult2Backend>(999) &&
              trace.executed == Mult2Backend::Reference && trace.fallback == Mult2Fallback::None,
          "unknown backend retained a previous successful trace");
    RejectEvery(module, reference, reference, "terminal_legacy_lifecycle");
    auto low = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRT>>(pair.GetLow());
    const auto scale = low->GetScalingFactor(); low->SetScalingFactor(scale * 2);
    RejectEvery(module, pair, pair, "recorded_scale"); low->SetScalingFactor(scale);
    const auto noise = low->GetNoiseScaleDeg(); low->SetNoiseScaleDeg(noise + 1);
    RejectEvery(module, pair, pair, "noise_degree"); low->SetNoiseScaleDeg(noise);
    const auto level = low->GetLevel(); low->SetLevel(0);
    RejectEvery(module, pair, pair, "level"); low->SetLevel(level);
    const auto slots = low->GetSlots(); low->SetSlots(slots + 1);
    RejectEvery(module, pair, pair, "slots"); low->SetSlots(slots);
    const auto tag = low->GetKeyTag(); low->SetKeyTag("foreign-c9-key");
    RejectEvery(module, pair, pair, "key_tag"); low->SetKeyTag(tag);
    auto savedElements = low->GetElements(); low->GetElements().pop_back();
    RejectEvery(module, pair, pair, "arity"); low->SetElements(savedElements);
    low->GetElements()[0].SetFormat(Format::COEFFICIENT);
    RejectEvery(module, pair, pair, "coefficient_format"); low->SetElements(savedElements);
    std::swap(low->GetElements()[0].GetAllElements()[0], low->GetElements()[0].GetAllElements()[1]);
    RejectEvery(module, pair, pair, "ordered_basis"); low->SetElements(savedElements);
    low->GetElements()[0].GetAllElements()[0] = lbcrypto::NativePoly();
    RejectEvery(module, pair, pair, "missing_tower_storage"); low->SetElements(savedElements);
    DoubleCKKS otherContext(MakeContext(2 * n, depth, bits, technique, digit));
    RejectEvery(otherContext, pair, pair, "context_identity");
    auto& cache = lbcrypto::CryptoContextImpl<DCRT>::GetAllEvalMultKeys();
    auto savedKeys = cache.at(tag); cache.erase(tag);
    RejectEvery(module, pair, pair, "missing_eval_key"); cache[tag] = savedKeys;
    cache[tag].clear(); RejectEvery(module, pair, pair, "empty_eval_key"); cache[tag] = savedKeys;
    cache[tag][0] = nullptr; RejectEvery(module, pair, pair, "null_eval_key"); cache[tag] = savedKeys;
    Check(Snapshot(input) == beforeInput && Snapshot(pair.GetHigh()) == beforeH &&
              Snapshot(pair.GetLow()) == beforeL, "negative controls did not restore fixture");
    lbcrypto::CryptoContextImpl<DCRT>::ClearEvalMultKeys(tag);
}

void Repeated() {
    const auto setup = CreateRepeatedMult2DiagnosticSetup();
    const auto c = setup.plan->GetFamilyContext(0);
    std::vector<precision_dcp_rcb_test::MpComplex> values(
        16, {precision_dcp_rcb_test::BigFloat(0), precision_dcp_rcb_test::BigFloat(0)});
    values[0].real = precision_dcp_rcb_test::BigFloat("0.125");
    values[1].real = precision_dcp_rcb_test::BigFloat("-0.0625");
    const auto plaintext = precision_dcp_rcb_test::MakePrecisionPlaintext(
        c, values, 100);
    const auto input = c->Encrypt(setup.publicKey, plaintext);
    DoubleCKKS module(setup.plan);
    const auto pair = module.DCP(input);
    const auto beforeH = Snapshot(pair.GetHigh()), beforeL = Snapshot(pair.GetLow());
    const auto first = module.Mult2(pair, pair), second = module.Mult2(first, first);
    Check(first.GetRepeatedReceipt()->GetPhase() == RepeatedPhase::Reentry &&
          first.GetRepeatedReceipt()->GetFamilyIndex() == 1 &&
          second.GetRepeatedReceipt()->GetPhase() == RepeatedPhase::Rescaled &&
          second.GetRepeatedReceipt()->IsTerminal(), "repeated fixture phase");
    for (auto backend : kBackends) {
        Mult2BackendTrace t1, t2;
        const auto a = module.Mult2WithBackend(pair, pair, backend, &t1);
        const auto b = module.Mult2WithBackend(a, a, backend, &t2);
        SamePair(a, first); SamePair(b, second);
        Trace(t1, backend, "repeated_nonterminal"); Trace(t2, backend, "repeated_terminal");
        const auto actual = module.RCBWithReceipt(b), expected = module.RCBWithReceipt(second);
        SameCiphertext(actual.GetCiphertext(), expected.GetCiphertext());
        Check(actual.GetReceipt() == expected.GetReceipt(), "terminal RCB receipt");
        RejectEvery(module, b, b, "planned_terminal_input");
    }
    RejectEvery(module, pair, first, "planned_cross_family");
    DoubleCKKS legacy(c); RejectEvery(legacy, pair, pair, "foreign_receipt_for_legacy");
    DoubleCKKS other(CreateRepeatedMult2DiagnosticSetup().plan);
    RejectEvery(other, pair, pair, "foreign_plan_receipt");
    auto low = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRT>>(pair.GetLow());
    low->SetScalingFactorInt(Native(7));
    RejectEvery(module, pair, pair, "planned_integer_factor"); low->SetScalingFactorInt(Native(1));
    low->SetMetadataByKey("unexpected", std::make_shared<Probe>(1));
    RejectEvery(module, pair, pair, "planned_metadata"); low->GetMetadataMap()->erase("unexpected");
    Check(Snapshot(pair.GetHigh()) == beforeH && Snapshot(pair.GetLow()) == beforeL, "planned input mutation");
}
}  // namespace

int main() {
    try {
        // All are correctness fixtures with HEStd_NotSet, not security profiles.
        Legacy(64, 3, 30, lbcrypto::HYBRID, 0);  // true minimum surviving m=2
        Legacy(128, 5, 59, lbcrypto::HYBRID, 0);
        Legacy(64, 4, 50, lbcrypto::BV, 0);
        Legacy(128, 4, 50, lbcrypto::BV, 20);
        Repeated();
        std::cout << "C9_NATIVE_FUNCTIONAL_PASS\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "C9_NATIVE_FUNCTIONAL_FAIL: " << e.what() << '\n';
        return 1;
    }
}
