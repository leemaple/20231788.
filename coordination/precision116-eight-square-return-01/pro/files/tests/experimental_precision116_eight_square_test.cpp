// EXPERIMENTAL-PRECISION116-EIGHT-SQUARE-01. One candidate keygen/chain.
// Test-only adaptation: original inputs, binary512 arithmetic, anchors and
// Horner are shared; original-profile Q/d/S100 decrypt/scale helpers are NOT.
// The seam supplies frozen candidate literals and applicable public checks,
// never its single-operation runner or its nonterminal pair/receipt checks.
#include "experimental_precision116_profile_seam.h"
#include <boost/version.hpp>
#include <limits>

#ifndef EXPERIMENTAL_PRECISION116_SOURCE_COMMIT
#error "Build this test through its explicitly enabled CMake target"
#endif

namespace precision116_eight {
namespace pf = paper_full_test;
namespace xp = experimental_precision116_test;
namespace io = openfhe_2023_1788::client_io;
using pf::Int;
using pf::Real;
using pf::Complex;
using pf::Scale;
using pf::IntegerPolynomial;
using pf::SparseSecret;
using Poly = lbcrypto::DCRTPoly;
using Native = lbcrypto::NativeInteger;
using Cipher = openfhe_2023_1788::ReadOnlyCiphertext;
using Pair = openfhe_2023_1788::CiphertextPair;
using Plan = std::shared_ptr<const openfhe_2023_1788::RepeatedMult2Plan>;
using Receipt = xp::Receipt;
using Phase = openfhe_2023_1788::RepeatedPhase;
using Lifecycle = openfhe_2023_1788::PairLifecycle;
using Setup = openfhe_2023_1788::RepeatedMult2ClientSetup;
constexpr const char* kLabel = "EXPERIMENTAL_PRECISION116_EIGHT_SQUARE";
constexpr const char* kProfile = "experimental-s116-d56-b58-v1";
constexpr const char* kBaseline = "2759fa90840946ef42957c7ba71ebea47e0e4995";
constexpr const char* kOpenFhe = "df495ba2e91739a6dc8f1de254fc5a41155ce504";
static_assert(pf::kN == 32768 && pf::kM == 65536 && pf::kSlots == 16384,
              "frozen original input geometry");
static_assert(std::numeric_limits<Real>::digits >= 512, "binary512-or-better oracle");

void Require(bool condition, const std::string& label) {
    if (!condition) throw std::runtime_error(std::string(kLabel) + ": " + label);
}
void Finite(const Real& value, const std::string& label) {
    Require(boost::math::isfinite(value), label + " nonfinite");
}
void Emit(const std::string& field, const Real& value) {
    Finite(value, field);
    std::cout << std::scientific << std::setprecision(100) << kLabel
              << " field=" << field << " value=" << value << '\n' << std::flush;
    Require(static_cast<bool>(std::cout), "observation output failed");
}
void Gate(bool condition, const std::string& label, std::size_t& failures) {
    if (!condition) {
        ++failures;
        std::cout << kLabel << " numeric_gate=FAIL gate=" << label << '\n' << std::flush;
        Require(static_cast<bool>(std::cout), "gate output failed");
    }
}
std::vector<xp::Prime> Prefix(std::size_t count) {
    Require(count >= 2 && count <= xp::kQ.size(), "candidate prefix count");
    return {xp::kQ.begin(), xp::kQ.begin() + static_cast<std::ptrdiff_t>(count)};
}
Int Modulus(std::size_t count) {
    Int result = 1;
    for (const auto& q : Prefix(count)) result *= q.modulus;
    return result;
}

// Closed product from frozen literals, not the production recursive receipts.
std::array<Scale, 9> ScaleOracle() {
    std::array<Scale, 9> result;
    result[0] = {Int(1) << 116, 1};
    for (std::size_t r = 1; r <= 8; ++r) {
        Int denominator = 1;
        for (std::size_t j = 1; j <= r; ++j) {
            Int factor = Int(xp::kQ[10].modulus) * xp::kQ[10 - j].modulus;
            for (std::size_t power = 0; power < r - j; ++power) factor *= factor;
            denominator *= factor;
        }
        result[r] = pf::Reduced(Int(1) << (116 * (std::size_t(1) << r)), denominator);
    }
    Require(result[8].numerator != result[0].numerator * result[8].denominator,
            "terminal rational scale differs from nominal S116");
    return result;
}
void CheckCandidateLiterals() {
    // Complete the Proth precondition for the three frozen witness checks in
    // CheckFamilies. Retained primes/P retain their pinned provenance.
    for (std::size_t j = 0; j < xp::kWitness.size(); ++j) {
        const std::uint64_t q = xp::kQ[j == 2 ? 10 : j].modulus;
        std::uint64_t odd = q - 1, power = 1;
        while ((odd & 1U) == 0) { odd >>= 1; power <<= 1; }
        Require(odd < power && (odd & 1U) != 0, "frozen new prime has Proth form");
        const Int witness = boost::multiprecision::powm(
            Int(xp::kWitness[j]), Int((q - 1) / 2), Int(q));
        Require(witness == q - 1, "independent integer Proth witness");
    }
}

// Client-only: recover the one signed h128 vector, checking every actual
// root modulus/root/format against the frozen candidate, not plan-derived Q.
SparseSecret CandidateSecret(const lbcrypto::PrivateKey<Poly>& key) {
    Require(static_cast<bool>(key), "client root secret exists");
    const auto& polynomial = key->GetPrivateElement();
    xp::CheckPolynomial(polynomial, Prefix(11));
    std::vector<int> signs(pf::kN, 0);
    for (std::size_t j = 0; j < xp::kQ.size(); ++j) {
        auto tower = polynomial.GetElementAtIndex(j);
        tower.SetFormat(Format::COEFFICIENT);  // Explicitly shared official inverse NTT.
        Require(tower.GetLength() == pf::kN, "root coefficient count");
        for (std::size_t i = 0; i < pf::kN; ++i) {
            const std::uint64_t value = tower[i].ConvertToInt();
            Require(value == 0 || value == 1 || value == xp::kQ[j].modulus - 1,
                    "signed ternary coefficient");
            const int sign = value == 0 ? 0 : (value == 1 ? 1 : -1);
            if (j == 0) signs[i] = sign;
            else Require(signs[i] == sign, "same signed root across all eleven towers");
        }
    }
    SparseSecret result;
    for (std::size_t i = 0; i < pf::kN; ++i)
        if (signs[i] != 0) result.emplace_back(i, signs[i]);
    Require(result.size() == 128, "exact root Hamming weight128");
    return result;
}

// No production Decrypt, FFT, DecryptCore, CRTInterpolate or ring multiply.
// expectedCount is established independently by the stage, never read from c.
IntegerPolynomial CandidatePolynomial(const Cipher& c, const SparseSecret& secret,
                                     std::size_t expectedCount) {
    Require(c && c->GetElements().size() == 2 && secret.size() == 128,
            "sparse oracle ciphertext/secret shape");
    std::set<std::size_t> support;
    for (const auto& term : secret)
        Require(term.first < pf::kN && (term.second == -1 || term.second == 1) &&
                support.insert(term.first).second, "signed sparse oracle support");
    const auto expected = Prefix(expectedCount);
    for (const auto& p : c->GetElements()) xp::CheckPolynomial(p, expected);
    IntegerPolynomial result{std::vector<Int>(pf::kN, 0), Modulus(expectedCount)};
    for (std::size_t j = 0; j < expectedCount; ++j) {
        auto c0 = c->GetElements()[0].GetElementAtIndex(j);
        auto c1 = c->GetElements()[1].GetElementAtIndex(j);
        c0.SetFormat(Format::COEFFICIENT);
        c1.SetFormat(Format::COEFFICIENT);
        Require(c0.GetLength() == pf::kN && c1.GetLength() == pf::kN, "oracle coefficient lengths");
        const std::uint64_t q = xp::kQ[j].modulus;
        std::vector<std::uint64_t> accum(pf::kN), operand(pf::kN);
        for (std::size_t n = 0; n < pf::kN; ++n) {
            accum[n] = c0[n].ConvertToInt(); operand[n] = c1[n].ConvertToInt();
            Require(accum[n] < q && operand[n] < q, "canonical inverse-NTT residues");
        }
        // Signed negacyclic convolution modulo X^N+1. q < 2^60, so additions
        // are < 2^61; native uint64_t suffices on both GCC and MinGW64.
        for (const auto& term : secret) for (std::size_t n = 0; n < pf::kN; ++n) {
            const std::size_t degree = n + term.first, index = degree % pf::kN;
            const bool positive = (term.second > 0) == (degree < pf::kN);
            const std::uint64_t value = operand[n], old = accum[index];
            if (positive) {
                const std::uint64_t sum = old + value;
                accum[index] = sum >= q ? sum - q : sum;
            }
            else accum[index] = old >= value ? old - value : q - (value - old);
        }
        const Int partial = result.modulus / q;
        const Int weight = partial * pf::Inverse(partial, Int(q));
        for (std::size_t n = 0; n < pf::kN; ++n) result.coefficients[n] += weight * accum[n];
    }
    for (auto& coefficient : result.coefficients) coefficient = pf::Center(coefficient, result.modulus);
    return result;
}
IntegerPolynomial CandidateRecombined(const Pair& pair, const SparseSecret& secret,
                                      std::size_t round) {
    Require(round <= 8, "recombined oracle round");
    auto high = CandidatePolynomial(pair.GetHigh(), secret, 10 - round);
    const auto low = CandidatePolynomial(pair.GetLow(), secret, 10 - round);
    Require(high.modulus == low.modulus, "candidate pair common modulus");
    for (std::size_t n = 0; n < pf::kN; ++n)
        high.coefficients[n] = pf::Center(Int(xp::kQ[10].modulus) * high.coefficients[n] +
                                          low.coefficients[n], high.modulus);
    return high;
}

void InspectReceipt(const Receipt& receipt, Phase phase, std::size_t family,
                    std::size_t operation, const Scale& expected, bool terminal) {
    Require(receipt && receipt->GetPhase() == phase && receipt->GetFamilyIndex() == family &&
            receipt->GetOperationIndex() == operation && receipt->IsTerminal() == terminal,
            "receipt phase/family/operation/terminal");
    const auto& actual = receipt->GetExactScale();
    // Equality of reduced integers, not only cross multiplication or doubles.
    Require(actual.GetNumerator() == expected.numerator && actual.GetDenominator() == expected.denominator,
            "receipt disagrees with independent closed-product exact scale");
}
void CheckReturned(const Pair& pair, const Plan& plan, std::size_t round, const Scale& scale) {
    Require(round <= 8, "returned round range");
    const bool terminal = round == 8;
    const std::size_t family = terminal ? 7 : round, level = terminal ? 2 : 1;
    const auto lifecycle = round == 0 ? Lifecycle::ReadyForFirstMult :
                           (terminal ? Lifecycle::RefreshRequired : Lifecycle::ReadyForRepeatedMult);
    const auto active = Prefix(10 - round);
    std::vector<Native> moduli;
    for (const auto& prime : active) moduli.emplace_back(prime.modulus);
    Require(pair.GetContextIdentity() == plan->GetFamilyContext(family).get() &&
            pair.GetKeyTag() == plan->GetFamilyKeyTag(family) && pair.GetDivisor() == Native(xp::kQ[10].modulus) &&
            pair.GetOrderedModuli() == moduli && pair.GetLevel() == level && pair.GetNoiseScaleDegree() == 2 &&
            pair.GetRecordedScalingFactor() == std::ldexp(1.0, 116) && pair.GetFormat() == Format::EVALUATION &&
            pair.GetSlots() == pf::kSlots && pair.GetComponentCount() == 2 && pair.GetLifecycle() == lifecycle,
            "returned physical family/basis/metadata/lifecycle");
    const auto& compatibility = pair.GetPaperScale();
    Require(compatibility.divisor == Native(xp::kQ[10].modulus) &&
            compatibility.inputRecordedScalingFactor == std::ldexp(1.0, 116) &&
            std::isfinite(compatibility.approximateLogicalScalingFactor) &&
            std::isfinite(compatibility.approximateRecombinedLogicalScalingFactor) &&
            compatibility.approximateLogicalScalingFactor > 0 &&
            compatibility.approximateRecombinedLogicalScalingFactor > 0,
            "returned compatibility metadata finite and positive");
    xp::CheckCipher(pair.GetHigh(), plan->GetFamilyContext(family), plan->GetFamilyKeyTag(family), active, level);
    xp::CheckCipher(pair.GetLow(), plan->GetFamilyContext(family), plan->GetFamilyKeyTag(family), active, level);
    Require(pair.GetHigh() != pair.GetLow() && pair.GetHigh()->GetMetadataMap() != pair.GetLow()->GetMetadataMap(),
            "separate pair wrappers/maps");
    const Phase phase = round == 0 ? Phase::Input : (terminal ? Phase::Rescaled : Phase::Reentry);
    InspectReceipt(pair.GetRepeatedReceipt(), phase, family, round, scale, terminal);
}
void InspectAncestry(const Pair& pair, const Receipt& previous, std::size_t round,
                     const std::array<Scale, 9>& scales) {
    Require(round >= 1 && round <= 8, "receipt ancestry round");
    const auto returned = pair.GetRepeatedReceipt();
    Require(static_cast<bool>(returned), "returned receipt exists");
    const auto rs = round == 8 ? returned : returned->GetParent();
    InspectReceipt(rs, Phase::Rescaled, round - 1, round, scales[round], round == 8);
    const Scale tensorScale = pf::Reduced(scales[round - 1].numerator * scales[round - 1].numerator,
        scales[round - 1].denominator * scales[round - 1].denominator * xp::kQ[10].modulus);
    const auto relin = rs->GetParent();
    InspectReceipt(relin, Phase::Relinearized, round - 1, round, tensorScale, false);
    const auto tensor = relin->GetParent();
    InspectReceipt(tensor, Phase::Tensor, round - 1, round, tensorScale, false);
    Require(tensor->GetParent() == previous, "exact same-chain parent identity");
}
void EmitReturned(const Pair& pair, std::size_t round) {
    const auto receipt = pair.GetRepeatedReceipt();
    std::cout << kLabel << " stage=returned round=" << round << " family=" << receipt->GetFamilyIndex()
              << " phase=" << (round == 0 ? "Input" : (round == 8 ? "Rescaled" : "Reentry"))
              << " local_level=" << pair.GetLevel() << " towers=" << pair.GetOrderedModuli().size()
              << " recorded_exp2=116 degree=2 terminal=" << receipt->IsTerminal()
              << " actual_scale_n=" << receipt->GetExactScale().GetNumerator()
              << " actual_scale_d=" << receipt->GetExactScale().GetDenominator() << '\n' << std::flush;
    Require(static_cast<bool>(std::cout), "receipt output failed");
}
void PreserveCipher(const Cipher& actual, const Cipher& snapshot, const std::string& label) {
    Require(actual && snapshot && actual->GetMetadataMap() && snapshot->GetMetadataMap() &&
            snapshot->GetElements().size() == 2, label + " preservation inputs");
    // Snapshot determines only the expected stage/identity for preservation;
    // actual q/root values are still checked against frozen candidate literals.
    xp::CheckCipher(actual, snapshot->GetCryptoContext(), snapshot->GetKeyTag(),
                    Prefix(snapshot->GetElements()[0].GetNumOfElements()), snapshot->GetLevel());
    Require(*actual == *snapshot && actual->GetMetadataMap()->empty(), label + " ciphertext preservation");
}

struct Evaluation final {
    Pair initial;
    std::vector<Pair> stages;
    openfhe_2023_1788::RepeatedMult2Result result;
};
// NO client, secret, capture, callback, oracle polynomial or decryption here.
// Only immutable public plan + ciphertext cross this evaluator call boundary.
Evaluation Evaluate(const Plan& plan, const Cipher& input) {
    const auto scales = ScaleOracle();
    openfhe_2023_1788::DoubleCKKS evaluator(plan);
    const auto inputSnapshot = input->Clone();
    auto pair = evaluator.DCP(input);  // Exactly one DCP in the evaluator path.
    const auto initial = pair;
    CheckReturned(pair, plan, 0, scales[0]);
    Require(!pair.GetRepeatedReceipt()->GetParent(), "Input receipt has no parent");
    EmitReturned(pair, 0);
    xp::Reject<std::invalid_argument>([&] { (void)evaluator.RCBWithReceipt(pair); },
                                     "nonterminal Input terminal-binder rejection");
    std::vector<Pair> stages; stages.reserve(8);
    for (std::size_t round = 1; round <= 8; ++round) {
        const auto highBefore = pair.GetHigh()->Clone(), lowBefore = pair.GetLow()->Clone();
        const auto previous = pair.GetRepeatedReceipt();
        auto next = evaluator.Mult2(pair, pair);  // One public square per round; no retries.
        PreserveCipher(pair.GetHigh(), highBefore, "square operand high");
        PreserveCipher(pair.GetLow(), lowBefore, "square operand low");
        Require(pair.GetRepeatedReceipt() == previous, "square preserves operand receipt");
        CheckReturned(pair, plan, round - 1, scales[round - 1]);
        CheckReturned(next, plan, round, scales[round]);
        InspectAncestry(next, previous, round, scales);
        EmitReturned(next, round);
        if (round == 1)
            xp::Reject<std::invalid_argument>([&] { (void)evaluator.RCBWithReceipt(next); },
                                             "nonterminal Reentry terminal-binder rejection");
        stages.push_back(next);
        pair = std::move(next);
    }
    PreserveCipher(input, inputSnapshot, "evaluator input");
    const auto highBefore = pair.GetHigh()->Clone(), lowBefore = pair.GetLow()->Clone();
    auto result = evaluator.RCBWithReceipt(pair);
    PreserveCipher(pair.GetHigh(), highBefore, "terminal RCB high input");
    PreserveCipher(pair.GetLow(), lowBefore, "terminal RCB low input");
    Require(result.GetReceipt() == pair.GetRepeatedReceipt(), "terminal result receipt identity");
    xp::CheckCipher(result.GetCiphertext(), plan->GetFamilyContext(0), plan->GetFamilyKeyTag(0), Prefix(2), 9);
    return {initial, std::move(stages), std::move(result)};
}

void CheckBoundState(const io::ClientCiphertextState& state, const Plan& plan,
                     const Scale& scale, bool final) {
    const auto root = plan->GetFamilyContext(0);
    const auto mask = static_cast<std::uint32_t>(lbcrypto::PKE | lbcrypto::KEYSWITCH | lbcrypto::LEVELEDSHE);
    const auto& profile = state.contextProfile;
    Require(profile.contextIdentity == root.get() && profile.cryptoParamsIdentity == root->GetCryptoParameters().get() &&
            profile.requiredFeatureMask == mask && (profile.enabledFeatureMaskObserved & mask) == mask &&
            profile.scalingTechnique == lbcrypto::FIXEDMANUAL && profile.keySwitchTechnique == lbcrypto::HYBRID &&
            profile.executionMode == lbcrypto::EXEC_EVALUATION && profile.decryptionNoiseMode == lbcrypto::FIXED_NOISE_DECRYPT &&
            profile.ckksDataType == lbcrypto::COMPLEX, "bound context profile");
    Require(state.keyTag == plan->GetFamilyKeyTag(0) && state.slots == pf::kSlots && state.strideGap == 1 &&
            state.level == (final ? 9U : 0U) && state.componentCount == 2 && state.metadataMapEmpty &&
            state.noiseScaleDegree == 2 && state.recordedScalingFactor == std::ldexp(1.0, 116) &&
            state.scalingFactorInt == Native(1) && state.logicalScale.Numerator() == scale.numerator &&
            state.logicalScale.Denominator() == scale.denominator &&
            state.encodingType == lbcrypto::CKKS_PACKED_ENCODING && state.componentFormat == Format::EVALUATION &&
            state.origin == (final ? io::ClientCiphertextOrigin::RepeatedMult2Rcb : io::ClientCiphertextOrigin::FreshClientEncoding) &&
            state.projection == io::CanonicalProjection::OpenFhePackedStride && !state.firstMult2ScaleFactors,
            "bound state including exact rational normalization");
    const std::size_t count = final ? 2 : 11;
    Require(state.activeBasis.cyclotomicOrder == pf::kM && state.activeBasis.ringDimension == pf::kN &&
            state.activeBasis.moduliDecimal.size() == count && state.activeBasis.rootsOfUnityDecimal.size() == count,
            "bound actual basis geometry");
    for (std::size_t j = 0; j < count; ++j)
        Require(state.activeBasis.moduliDecimal[j] == std::to_string(xp::kQ[j].modulus) &&
                state.activeBasis.rootsOfUnityDecimal[j] == std::to_string(xp::kQ[j].root),
                "bound candidate prefix q/root literals");
}
Real ObserveEndpoint(const io::DecodedSlots& decoded, const std::vector<Complex>& ideal,
                     const std::string& label, std::size_t& failures) {
    Require(decoded.values.size() == pf::kSlots && ideal.size() == pf::kSlots, "all 16384 slots present");
    Real maximum = 0;
    std::size_t maxSlot = 0;
    const char* maxComponent = "real";
    for (std::size_t s = 0; s < pf::kSlots; ++s) {
        const auto actual = pf::FromClient(decoded.values[s]);
        Finite(actual.real, label + ".actual.real"); Finite(actual.imag, label + ".actual.imag");
        Finite(ideal[s].real, label + ".ideal.real"); Finite(ideal[s].imag, label + ".ideal.imag");
        const Real re = pf::Abs(actual.real - ideal[s].real), im = pf::Abs(actual.imag - ideal[s].imag);
        if (re > maximum) { maximum = re; maxSlot = s; maxComponent = "real"; }
        if (im > maximum) { maximum = im; maxSlot = s; maxComponent = "imag"; }
    }
    Emit(label + ".full_max_component_error", maximum);
    std::cout << kLabel << " endpoint=" << label << " max_slot=" << maxSlot << " component=" << maxComponent << '\n';
    Gate(maximum <= pf::Pow2(-80), label + ".full_component_E80", failures);
    const Real cross(decoded.diagnostics.maximumCrossPrecisionDisagreement.str(100, std::ios_base::scientific));
    Emit(label + ".codec_disagreement", cross);
    Require(cross >= 0 && cross <= pf::Pow2(-120), label + " codec 2^-120 consistency");
    std::cout << kLabel << " endpoint=" << label << " centered_headroom=" << decoded.diagnostics.centeredHeadroom
              << " actual_Q=" << decoded.diagnostics.activeCompositeModulus << '\n' << std::flush;
    Require(decoded.diagnostics.centeredHeadroom > 0, label + " positive actual centered headroom");
    const Real expectedDelta = ideal[1].real - ideal[0].real;
    const Real actualDelta = pf::FromClient(decoded.values[1]).real - pf::FromClient(decoded.values[0]).real;
    Emit(label + ".expected_slot1_minus_slot0_real", expectedDelta);
    Emit(label + ".actual_slot1_minus_slot0_real", actualDelta);
    Emit(label + ".witness_delta_disagreement", pf::Abs(actualDelta - expectedDelta));
    Gate(actualDelta > pf::Pow2(-76), label + ".actual_delta_gt_2^-76", failures);
    Gate(pf::Abs(actualDelta - expectedDelta) <= 2 * pf::Pow2(-80), label + ".delta_disagreement_le_2*2^-80", failures);
    return maximum;
}
Real ObserveAnchors(const IntegerPolynomial& polynomial, const Scale& scale,
                    const std::array<Complex, 10>& roots, const std::vector<Complex>& ideal,
                    const std::string& label, std::size_t& failures,
                    const io::DecodedSlots* production = nullptr) {
    Require(polynomial.coefficients.size() == pf::kN && ideal.size() == pf::kSlots, "anchor oracle shapes");
    const auto actual = pf::Horner(polynomial, scale, roots);
    Real maximum = 0, agreement = 0;
    for (std::size_t a = 0; a < pf::kAnchors.size(); ++a) {
        const Real error = pf::Error(actual[a], ideal[pf::kAnchors[a]]);
        Emit(label + ".anchor_" + std::to_string(pf::kAnchors[a]) + ".component_error", error);
        if (error > maximum) maximum = error;
        if (production) {
            const Real diff = pf::Error(actual[a], pf::FromClient(production->values.at(pf::kAnchors[a])));
            if (diff > agreement) agreement = diff;
        }
    }
    Emit(label + ".anchor_max_component_error", maximum);
    Gate(maximum <= pf::Pow2(-80), label + ".anchor_E80", failures);
    if (production) {
        Emit(label + ".anchor_vs_production", agreement);
        Require(agreement <= pf::Pow2(-80), label + " independent anchor vs production 2^-80 consistency");
        Int maxCoefficient = 0;
        for (const auto& value : polynomial.coefficients) {
            const Int magnitude = value < 0 ? Int(-value) : value;
            if (magnitude > maxCoefficient) maxCoefficient = magnitude;
        }
        // FIXED_NOISE_DECRYPT adds no polynomial flooding in the pinned Poly*
        // route; independent CRT must reproduce the actual endpoint diagnostics.
        Require(production->diagnostics.activeCompositeModulus == polynomial.modulus &&
                production->diagnostics.maximumCenteredAbsoluteCoefficient == maxCoefficient &&
                production->diagnostics.centeredHeadroom == polynomial.modulus / 2 - maxCoefficient,
                label + " independent centered-CRT diagnostic agreement");
    }
    return maximum;
}
void CheckFinalIdeal(const std::vector<Complex>& ideal) {
    Require(ideal.size() == pf::kSlots, "final ideal full slots");
    const Real delta = ideal[1].real - ideal[0].real;
    Emit("final.expected_witness_delta", delta);
    Require(delta > pf::Pow2(-71) && delta < pf::Pow2(-70), "published expected witness interval");
    Require(pf::Abs(ideal[0].real - Real("0.10106701692533075452271390324763932267007358697064098")) < pf::Pow2(-150) &&
            pf::Abs(ideal[0].imag - Real("0.02604542052026911640736001715600098685432818977191963")) < pf::Pow2(-150),
            "published slot0 expected scalars within 2^-150");
    for (const auto& z : ideal) {
        Finite(z.real, "final ideal real"); Finite(z.imag, "final ideal imag");
        const Real norm2 = z.real * z.real + z.imag * z.imag;
        Require(norm2 > Real(".098") * Real(".098") && norm2 < Real(".106") * Real(".106"),
                "every ideal output squared magnitude in (.098^2,.106^2)");
    }
}
void CheckFinalActual(const io::DecodedSlots& decoded, std::size_t& failures) {
    Require(decoded.values.size() == pf::kSlots, "actual domain full slots");
    Real minimum = std::numeric_limits<Real>::max();
    std::size_t minimumSlot = 0, misses = 0;
    for (std::size_t s = 0; s < pf::kSlots; ++s) {
        const auto z = pf::FromClient(decoded.values[s]);
        const Real norm2 = z.real * z.real + z.imag * z.imag;
        Finite(norm2, "actual final squared magnitude");
        if (norm2 < minimum) { minimum = norm2; minimumSlot = s; }
        if (!(norm2 > Real(".09") * Real(".09"))) ++misses;
    }
    Emit("final.minimum_actual_squared_magnitude", minimum);
    std::cout << kLabel << " field=actual_domain minimum_slot=" << minimumSlot << " failing_slots=" << misses << '\n';
    Gate(misses == 0, "final.every_actual_squared_magnitude_gt_.09^2", failures);
}

void ForeignRejections(const Evaluation& evaluation, const Setup& diagnostic) {
    openfhe_2023_1788::DoubleCKKS foreignEvaluator(diagnostic.plan);
    xp::Reject<std::invalid_argument>([&] { (void)foreignEvaluator.RCBWithReceipt(evaluation.stages.back()); },
                                     "foreign plan accepted terminal pair");
    // Existing supported N64/Q8 I/O boundary. Context ONLY, no keygen/chain;
    // the concurrently live diagnostic setup is never mutated to make it fit.
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> p;
    p.SetMultiplicativeDepth(7); p.SetScalingModSize(50); p.SetFirstModSize(55);
    p.SetScalingTechnique(lbcrypto::FIXEDMANUAL); p.SetKeySwitchTechnique(lbcrypto::HYBRID);
    p.SetDigitSize(0); p.SetMaxRelinSkDeg(2); p.SetNumLargeDigits(0);
    p.SetSecretKeyDist(lbcrypto::UNIFORM_TERNARY); p.SetSecurityLevel(lbcrypto::HEStd_NotSet);
    p.SetRingDim(64); p.SetBatchSize(16); p.SetCKKSDataType(lbcrypto::COMPLEX);
    p.SetPREMode(lbcrypto::NOT_SET); p.SetExecutionMode(lbcrypto::EXEC_EVALUATION);
    p.SetDecryptionNoiseMode(lbcrypto::FIXED_NOISE_DECRYPT);
    p.SetStandardDeviation(3.19f); p.SetNoiseEstimate(0.0); p.SetDesiredPrecision(25.0);
    p.SetStatisticalSecurity(30); p.SetNumAdversarialQueries(1);
    p.SetInteractiveBootCompressionLevel(lbcrypto::SLACK); p.SetCompositeDegree(1); p.SetRegisterWordSize(64);
    const auto context = lbcrypto::GenCryptoContext(p);
    Require(static_cast<bool>(context), "foreign Q8 context exists");
    context->Enable(lbcrypto::PKE); context->Enable(lbcrypto::KEYSWITCH); context->Enable(lbcrypto::LEVELEDSHE);
    const io::HighPrecisionClientIO foreignClient(context);
    xp::Reject<std::invalid_argument>([&] { (void)foreignClient.BindRepeatedRcb(evaluation.result); },
                                     "foreign client accepted candidate terminal result");
}
void TrackOwners(const Setup& setup, xp::ReleasedOwners& owners) {
    owners.plan = setup.plan; owners.publicKey = setup.publicKey; owners.secret = setup.rootSecret;
    for (std::size_t f = 0; f < setup.plan->GetFamilyCount(); ++f) {
        const auto& tag = setup.plan->GetFamilyKeyTag(f);
        owners.tags.push_back(tag);
        owners.rows.push_back(lbcrypto::CryptoContextImpl<Poly>::GetAllEvalMultKeys().at(tag).at(0));
    }
}
struct Measurements final {
    Real freshMaximum = 0, finalMaximum = 0, freshAnchorMaximum = 0, finalAnchorMaximum = 0;
    std::array<Real, 8> roundAnchorMaximum{};
    Real wrongNominalError = 0;
};
// All secret/oracle observations are client-side, before/after Evaluate, not
// callbacks into evaluation. Only numeric values and weak owners leave here.
Measurements RunCandidate(const Setup& diagnostic, xp::ReleasedOwners& owners, std::size_t& failures) {
    const auto scales = ScaleOracle();
    auto ideal = pf::Inputs();
    const auto roots = pf::AnchorRoots();
    CheckCandidateLiterals();
    auto setup = openfhe_2023_1788::CreateExperimentalPrecision116Setup();
    TrackOwners(setup, owners);
    xp::CheckFamilies(setup.plan); xp::CheckRootKeys(setup);
    const auto secret = CandidateSecret(setup.rootSecret);
    const auto publicBefore = setup.publicKey->GetPublicElements();
    const auto secretBefore = setup.rootSecret->GetPrivateElement();
    const auto publicIdentity = setup.publicKey.get();
    const auto secretIdentity = setup.rootSecret.get();
    const auto rowsBefore = xp::CurrentRows();
    std::cout << kLabel << " exact_candidate=ESTABLISHED input=paper_full_test::Inputs"
                 " N=32768 M=65536 gap=1 h=128 families=8 base_metadata_exp2=58 recorded_exp2=116 expected_tensor_recorded_exp2=174\n";
    for (std::size_t j = 0; j < xp::kQ.size(); ++j)
        std::cout << kLabel << " frozen_Q_index=" << j << " q=" << xp::kQ[j].modulus << " root=" << xp::kQ[j].root << '\n';
    std::cout << kLabel << " frozen_P=" << xp::kP.modulus << " root=" << xp::kP.root << '\n' << std::flush;
    const io::HighPrecisionClientIO client(setup.plan);
    // Inputs are exact original dyadics; no binary64 conversion or resampling.
    const auto fresh = client.Encrypt(setup.publicKey, pf::ClientInputs(ideal),
        {16384, io::PositiveRationalScale::FromPositive(scales[0].numerator, scales[0].denominator)});
    CheckBoundState(fresh.State(), setup.plan, scales[0], false);
    const auto source = fresh.CloneForEvaluation(), sourceBefore = source->Clone();
    owners.ciphertexts.push_back(source);
    xp::CheckCipher(source, setup.plan->GetFamilyContext(0), setup.plan->GetFamilyKeyTag(0), Prefix(11), 0);
    const auto freshDecoded = client.Decrypt(setup.rootSecret, fresh);
    CheckBoundState(freshDecoded.state, setup.plan, scales[0], false);
    Measurements measured;
    measured.freshMaximum = ObserveEndpoint(freshDecoded, ideal, "fresh", failures);
    const auto freshPolynomial = CandidatePolynomial(source, secret, 11);
    measured.freshAnchorMaximum = ObserveAnchors(freshPolynomial, scales[0], roots, ideal, "fresh", failures, &freshDecoded);

    const auto evaluation = Evaluate(setup.plan, source);
    Require(evaluation.stages.size() == 8, "one complete eight-square chain returned");
    CheckReturned(evaluation.initial, setup.plan, 0, scales[0]);
    Require(!evaluation.initial.GetRepeatedReceipt()->GetParent(), "initial receipt root");
    // Round0 DCP recombination must exactly equal reduction of the fresh
    // polynomial to the independently expected Div-deleted modulus.
    const auto dcpPolynomial = CandidateRecombined(evaluation.initial, secret, 0);
    for (std::size_t n = 0; n < pf::kN; ++n)
        Require(dcpPolynomial.coefficients[n] == pf::Center(freshPolynomial.coefficients[n], dcpPolynomial.modulus),
                "DCP recombined polynomial equals fresh reduction");
    (void)ObserveAnchors(dcpPolynomial, scales[0], roots, ideal, "round_0", failures);
    owners.ciphertexts.push_back(evaluation.initial.GetHigh());
    owners.ciphertexts.push_back(evaluation.initial.GetLow());
    Receipt previous = evaluation.initial.GetRepeatedReceipt();
    IntegerPolynomial terminalPairPolynomial;
    for (std::size_t round = 1; round <= 8; ++round) {
        const auto& pair = evaluation.stages[round - 1];
        CheckReturned(pair, setup.plan, round, scales[round]);
        InspectAncestry(pair, previous, round, scales); previous = pair.GetRepeatedReceipt();
        owners.ciphertexts.push_back(pair.GetHigh()); owners.ciphertexts.push_back(pair.GetLow());
        for (auto& z : ideal) z = pf::Multiply(z, z);  // Independent binary512 z^(2^round).
        auto polynomial = CandidateRecombined(pair, secret, round);
        measured.roundAnchorMaximum[round - 1] = ObserveAnchors(
            polynomial, scales[round], roots, ideal, "round_" + std::to_string(round), failures);
        if (round == 8) terminalPairPolynomial = std::move(polynomial);
    }
    std::set<const void*> receiptIdentities;
    for (auto r = evaluation.result.GetReceipt(); r; r = r->GetParent()) {
        Require(receiptIdentities.insert(r.get()).second, "acyclic distinct receipt chain");
        owners.receipts.push_back(r);
    }
    Require(receiptIdentities.size() == 32 && evaluation.result.GetReceipt() == previous,
            "all 32 issued receipt nodes on the terminal chain");
    CheckFinalIdeal(ideal);
    const auto bound = client.BindRepeatedRcb(evaluation.result);
    CheckBoundState(bound.State(), setup.plan, scales[8], true);
    owners.ciphertexts.push_back(evaluation.result.GetCiphertext());
    auto escaped = bound.CloneForEvaluation();
    xp::CheckCipher(escaped, setup.plan->GetFamilyContext(0), setup.plan->GetFamilyKeyTag(0), Prefix(2), 9);
    escaped->SetElements(std::vector<Poly>{});
    Require(bound.CloneForEvaluation()->GetElements() == evaluation.result.GetCiphertext()->GetElements(),
            "bound terminal snapshot survives mutation of a separate clone");
    const auto finalDecoded = client.Decrypt(setup.rootSecret, bound);
    CheckBoundState(finalDecoded.state, setup.plan, scales[8], true);
    measured.finalMaximum = ObserveEndpoint(finalDecoded, ideal, "final", failures);
    CheckFinalActual(finalDecoded, failures);
    const auto finalPolynomial = CandidatePolynomial(evaluation.result.GetCiphertext(), secret, 2);
    Require(finalPolynomial.modulus == terminalPairPolynomial.modulus &&
            finalPolynomial.coefficients == terminalPairPolynomial.coefficients,
            "root wrapper terminal polynomial equals family7 recombination exactly");
    measured.finalAnchorMaximum = ObserveAnchors(finalPolynomial, scales[8], roots, ideal, "final", failures, &finalDecoded);
    const auto wrongNominal = pf::Horner(finalPolynomial, scales[0], roots);
    measured.wrongNominalError = pf::Error(wrongNominal[0], ideal[0]);
    Emit("final.wrong_nominal_2^116_anchor0_component_error", measured.wrongNominalError);
    Require(measured.wrongNominalError > pf::Pow2(-30), "wrong nominal normalization must err by >2^-30");
    ForeignRejections(evaluation, diagnostic);
    PreserveCipher(source, sourceBefore, "client original input");
    PreserveCipher(fresh.CloneForEvaluation(), sourceBefore, "client fresh bound snapshot");
    Require(setup.publicKey.get() == publicIdentity && setup.rootSecret.get() == secretIdentity &&
            setup.publicKey->GetPublicElements() == publicBefore && setup.rootSecret->GetPrivateElement() == secretBefore &&
            setup.publicKey->GetCryptoContext() == setup.plan->GetFamilyContext(0) &&
            setup.rootSecret->GetCryptoContext() == setup.plan->GetFamilyContext(0) &&
            setup.publicKey->GetKeyTag() == setup.plan->GetFamilyKeyTag(0) &&
            setup.rootSecret->GetKeyTag() == setup.plan->GetFamilyKeyTag(0),
            "root public/secret identities, context, tag and values unchanged");
    xp::CheckFamilies(setup.plan);
    Require(xp::CurrentRows() == rowsBefore, "candidate public evaluation-row identities preserved");
    std::cout << kLabel << " full_chain_numerical_predicates=REACHED structural_oracle_checks=VALID"
                 " cleanup=PENDING security=UNRESOLVED\n" << std::flush;
    return measured;
}
Measurements Run(std::size_t& failures) {
    const auto initialRows = xp::CurrentRows();
    const auto initialAutomorphisms = lbcrypto::CryptoContextImpl<Poly>::GetAllEvalAutomorphismKeys();
    xp::ReleasedOwners diagnosticOwners;
    Measurements measured;
    {
        const auto diagnostic = openfhe_2023_1788::CreateRepeatedMult2DiagnosticSetup();
        Require(diagnostic.plan && diagnostic.plan->GetFamilyCount() == 2 &&
                diagnostic.plan->GetFamilyContext(0)->GetRingDimension() == 64, "live small diagnostic guard");
        TrackOwners(diagnostic, diagnosticOwners);
        struct SmallRow final { std::string tag; lbcrypto::EvalKey<Poly> identity; std::vector<Poly> a, b; };
        std::vector<SmallRow> smallRows;
        for (const auto& tag : diagnosticOwners.tags) {
            const auto& row = lbcrypto::CryptoContextImpl<Poly>::GetAllEvalMultKeys().at(tag);
            Require(row.size() == 1 && row[0], "small diagnostic row present");
            smallRows.push_back({tag, row[0], row[0]->GetAVector(), row[0]->GetBVector()});
        }
        const auto smallPublic = diagnostic.publicKey->GetPublicElements();
        const auto smallSecret = diagnostic.rootSecret->GetPrivateElement();
        const auto withDiagnostic = xp::CurrentRows();
        xp::ReleasedOwners candidateOwners;
        measured = RunCandidate(diagnostic, candidateOwners, failures);
        Require(candidateOwners.tags.size() == 8, "eight candidate owned rows tracked");
        xp::CheckReleased(candidateOwners);
        Require(xp::CurrentRows() == withDiagnostic, "candidate cleanup preserves unrelated cache identities");
        for (const auto& saved : smallRows) {
            const auto& row = lbcrypto::CryptoContextImpl<Poly>::GetAllEvalMultKeys().at(saved.tag);
            Require(row.size() == 1 && row[0] == saved.identity && row[0]->GetAVector() == saved.a &&
                    row[0]->GetBVector() == saved.b, "live small diagnostic row identity/coefficients unchanged");
        }
        Require(diagnostic.publicKey->GetPublicElements() == smallPublic &&
                diagnostic.rootSecret->GetPrivateElement() == smallSecret, "live small diagnostic keys unchanged");
        const openfhe_2023_1788::DoubleCKKS stillUsable(diagnostic.plan);
        (void)stillUsable;  // Revalidate small plan only; no second encrypted chain.
    }
    xp::CheckReleased(diagnosticOwners);
    Require(xp::CurrentRows() == initialRows &&
            lbcrypto::CryptoContextImpl<Poly>::GetAllEvalAutomorphismKeys() == initialAutomorphisms,
            "owned-row cleanup restores global caches");
    return measured;
}
void Identity(const char* event) {
    std::cout << kLabel << " event=" << event << " source=" << EXPERIMENTAL_PRECISION116_SOURCE_COMMIT
              << " baseline_source=" << kBaseline << " profile=" << kProfile
              << " squares=8 full_slots=16384 requested_chain_count=1 security=UNRESOLVED"
              << " required_openfhe_pin=" << kOpenFhe << " native=64 backend=4 boost_version=" << BOOST_VERSION << '\n' << std::flush;
    Require(static_cast<bool>(std::cout), "identity output failed");
}
}  // namespace precision116_eight

int main(int argc, char**) {
    namespace t = precision116_eight;
    std::size_t failures = 0;
    try {
        t::Require(argc == 1, "this dedicated executable accepts no alternate modes");
        t::Identity("START");
        const auto measured = t::Run(failures);
        for (std::size_t r = 1; r <= 8; ++r)
            t::Emit("summary.round_" + std::to_string(r) + ".anchor_max_component_error", measured.roundAnchorMaximum[r - 1]);
        std::cout << t::kLabel << " boundary=ten_anchors_do_not_prove_all_slot_intermediate_accuracy_or_nonwrap"
                     " tensor_relin_lift_safety=NOT_PROVED original_profile_recorded_result=FAIL\n";
        std::cout << std::scientific << std::setprecision(100) << t::kLabel
                  << " event=COMPLETE source=" << EXPERIMENTAL_PRECISION116_SOURCE_COMMIT
                  << " baseline_source=" << t::kBaseline << " profile=" << t::kProfile
                  << " squares=8 full_slots=16384 chain_count=1 result=" << (failures == 0 ? "PASS" : "FAIL")
                  << " security=UNRESOLVED structural_oracle_checks=VALID cleanup=PASS"
                  << " error_gate=2^-80 codec_gate=2^-120 numeric_gate_failures=" << failures
                  << " fresh_max_component_error=" << measured.freshMaximum
                  << " final_max_component_error=" << measured.finalMaximum
                  << " fresh_anchor_max=" << measured.freshAnchorMaximum << " final_anchor_max=" << measured.finalAnchorMaximum
                  << " wrong_nominal_error=" << measured.wrongNominalError << '\n' << std::flush;
        t::Require(static_cast<bool>(std::cout), "final output failed");
        return failures == 0 ? 0 : 1;
    }
    catch (const std::exception& error) {
        // One process boundary only. No production catch-and-continue/retry.
        // Partial measurements above stay facts, never full-chain PASS/RED.
        std::cerr << t::kLabel << " event=ABORT source=" << EXPERIMENTAL_PRECISION116_SOURCE_COMMIT
                  << " baseline_source=" << t::kBaseline << " profile=" << t::kProfile
                  << " squares=8 full_slots=16384 result=INVALID_OR_INCOMPLETE numerical_result=NOT_ESTABLISHED"
                  << " security=UNRESOLVED prior_finite_gate_failures=" << failures << " reason=" << error.what() << '\n';
        return 2;
    }
}
