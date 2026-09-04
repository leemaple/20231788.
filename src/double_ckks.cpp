#include "openfhe_2023_1788/double_ckks.h"

#include <cmath>
#include <memory>
#include <stdexcept>
#include <utility>

namespace openfhe_2023_1788 {

namespace {

[[noreturn]] void Invalid(const std::string& message) {
    throw std::invalid_argument("DoubleCKKS: " + message);
}

std::vector<lbcrypto::NativeInteger> OrderedModuli(const lbcrypto::DCRTPoly& polynomial) {
    std::vector<lbcrypto::NativeInteger> moduli;
    moduli.reserve(polynomial.GetAllElements().size());
    for (const auto& tower : polynomial.GetAllElements()) {
        moduli.push_back(tower.GetModulus());
    }
    return moduli;
}

bool SameOrderedModuli(const std::vector<lbcrypto::NativeInteger>& left,
                       const std::vector<lbcrypto::NativeInteger>& right) {
    if (left.size() != right.size()) {
        return false;
    }
    for (std::size_t index = 0; index < left.size(); ++index) {
        if (left[index] != right[index]) {
            return false;
        }
    }
    return true;
}

bool HasCompleteOrderedBasis(
    const lbcrypto::DCRTPoly& polynomial,
    const std::shared_ptr<lbcrypto::ILDCRTParams<lbcrypto::BigInteger>>& expectedBasis) {
    const auto& actualBasis = polynomial.GetParams();
    if (!actualBasis || !expectedBasis || !(*actualBasis == *expectedBasis)) {
        return false;
    }

    const auto& actualTowers = polynomial.GetAllElements();
    const auto& expectedTowers = expectedBasis->GetParams();
    if (actualTowers.size() != expectedTowers.size()) {
        return false;
    }
    for (std::size_t index = 0; index < actualTowers.size(); ++index) {
        const auto& actualTowerBasis = actualTowers[index].GetParams();
        if (!actualTowerBasis || !expectedTowers[index] ||
            !(*actualTowerBasis == *expectedTowers[index])) {
            return false;
        }
    }
    return true;
}

bool IsInEvaluationFormat(const lbcrypto::DCRTPoly& polynomial) {
    if (polynomial.GetFormat() != Format::EVALUATION) {
        return false;
    }
    for (const auto& tower : polynomial.GetAllElements()) {
        if (tower.GetFormat() != Format::EVALUATION) {
            return false;
        }
    }
    return true;
}

}  // namespace

CiphertextPair::CiphertextPair(lbcrypto::Ciphertext<lbcrypto::DCRTPoly> high,
                               lbcrypto::Ciphertext<lbcrypto::DCRTPoly> low,
                               const lbcrypto::CryptoContextImpl<lbcrypto::DCRTPoly>* contextIdentity,
                               lbcrypto::NativeInteger divisor,
                               std::vector<lbcrypto::NativeInteger> orderedModuli,
                               std::size_t level,
                               PaperScaleDescriptor paperScale,
                               double recordedScalingFactor,
                               std::size_t noiseScaleDegree,
                               PairLifecycle lifecycle,
                               std::string keyTag,
                               std::uint32_t slots,
                               Format format,
                               std::size_t componentCount)
    : high_(std::move(high)),
      low_(std::move(low)),
      contextIdentity_(contextIdentity),
      divisor_(std::move(divisor)),
      orderedModuli_(std::move(orderedModuli)),
      level_(level),
      paperScale_(paperScale),
      recordedScalingFactor_(recordedScalingFactor),
      noiseScaleDegree_(noiseScaleDegree),
      lifecycle_(lifecycle),
      keyTag_(std::move(keyTag)),
      slots_(slots),
      format_(format),
      componentCount_(componentCount) {}

ReadOnlyCiphertext CiphertextPair::GetHigh() const noexcept {
    return high_;
}

ReadOnlyCiphertext CiphertextPair::GetLow() const noexcept {
    return low_;
}

const lbcrypto::CryptoContextImpl<lbcrypto::DCRTPoly>* CiphertextPair::GetContextIdentity() const noexcept {
    return contextIdentity_;
}

const lbcrypto::NativeInteger& CiphertextPair::GetDivisor() const noexcept {
    return divisor_;
}

const std::vector<lbcrypto::NativeInteger>& CiphertextPair::GetOrderedModuli() const noexcept {
    return orderedModuli_;
}

std::size_t CiphertextPair::GetLevel() const noexcept {
    return level_;
}

const PaperScaleDescriptor& CiphertextPair::GetPaperScale() const noexcept {
    return paperScale_;
}

double CiphertextPair::GetRecordedScalingFactor() const noexcept {
    return recordedScalingFactor_;
}

std::size_t CiphertextPair::GetNoiseScaleDegree() const noexcept {
    return noiseScaleDegree_;
}

PairLifecycle CiphertextPair::GetLifecycle() const noexcept {
    return lifecycle_;
}

const std::string& CiphertextPair::GetKeyTag() const noexcept {
    return keyTag_;
}

std::uint32_t CiphertextPair::GetSlots() const noexcept {
    return slots_;
}

Format CiphertextPair::GetFormat() const noexcept {
    return format_;
}

std::size_t CiphertextPair::GetComponentCount() const noexcept {
    return componentCount_;
}

TensorCiphertextPair::TensorCiphertextPair(
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> high,
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> low,
    const lbcrypto::CryptoContextImpl<lbcrypto::DCRTPoly>* contextIdentity,
    lbcrypto::NativeInteger divisor,
    std::vector<lbcrypto::NativeInteger> orderedModuli,
    std::size_t level,
    TensorScaleDescriptor tensorScale,
    double recordedScalingFactor,
    std::size_t noiseScaleDegree,
    std::string keyTag,
    std::uint32_t slots,
    Format format,
    std::size_t componentCount)
    : high_(std::move(high)),
      low_(std::move(low)),
      contextIdentity_(contextIdentity),
      divisor_(std::move(divisor)),
      orderedModuli_(std::move(orderedModuli)),
      level_(level),
      tensorScale_(tensorScale),
      recordedScalingFactor_(recordedScalingFactor),
      noiseScaleDegree_(noiseScaleDegree),
      keyTag_(std::move(keyTag)),
      slots_(slots),
      format_(format),
      componentCount_(componentCount) {}

ReadOnlyCiphertext TensorCiphertextPair::GetHigh() const noexcept {
    return high_;
}

ReadOnlyCiphertext TensorCiphertextPair::GetLow() const noexcept {
    return low_;
}

const lbcrypto::CryptoContextImpl<lbcrypto::DCRTPoly>* TensorCiphertextPair::GetContextIdentity() const noexcept {
    return contextIdentity_;
}

const lbcrypto::NativeInteger& TensorCiphertextPair::GetDivisor() const noexcept {
    return divisor_;
}

const std::vector<lbcrypto::NativeInteger>& TensorCiphertextPair::GetOrderedModuli() const noexcept {
    return orderedModuli_;
}

std::size_t TensorCiphertextPair::GetLevel() const noexcept {
    return level_;
}

const TensorScaleDescriptor& TensorCiphertextPair::GetTensorScale() const noexcept {
    return tensorScale_;
}

double TensorCiphertextPair::GetRecordedScalingFactor() const noexcept {
    return recordedScalingFactor_;
}

std::size_t TensorCiphertextPair::GetNoiseScaleDegree() const noexcept {
    return noiseScaleDegree_;
}

const std::string& TensorCiphertextPair::GetKeyTag() const noexcept {
    return keyTag_;
}

std::uint32_t TensorCiphertextPair::GetSlots() const noexcept {
    return slots_;
}

Format TensorCiphertextPair::GetFormat() const noexcept {
    return format_;
}

std::size_t TensorCiphertextPair::GetComponentCount() const noexcept {
    return componentCount_;
}

DoubleCKKS::DoubleCKKS(lbcrypto::CryptoContext<lbcrypto::DCRTPoly> context)
    : context_(std::move(context)), expectedInputScalingFactor_(0.0) {
    if (!context_) {
        Invalid("context is null");
    }

    parameters_ = std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context_->GetCryptoParameters());
    if (!parameters_) {
        Invalid("context is not CKKS-RNS");
    }
    if (parameters_->GetScalingTechnique() != lbcrypto::FIXEDMANUAL) {
        Invalid("only FIXEDMANUAL scaling is supported");
    }

    const auto elementParameters = parameters_->GetElementParams();
    if (!elementParameters || elementParameters->GetParams().size() < 3) {
        Invalid("the first Mult2 lifecycle requires at least three ordered Q towers");
    }

    fullModuli_.reserve(elementParameters->GetParams().size());
    for (const auto& towerParameters : elementParameters->GetParams()) {
        if (!towerParameters) {
            Invalid("context contains null tower parameters");
        }
        fullModuli_.push_back(towerParameters->GetModulus());
    }
    divisor_ = fullModuli_.back();
    if (divisor_.Mod(lbcrypto::NativeInteger(2)) != lbcrypto::NativeInteger(1)) {
        Invalid("the last Q tower used as q_div must be odd");
    }
    firstPairModuli_.assign(fullModuli_.begin(), fullModuli_.end() - 1);

    const double baseScalingFactor = parameters_->GetScalingFactorReal(0);
    expectedInputScalingFactor_    = baseScalingFactor * baseScalingFactor;
    if (!std::isfinite(baseScalingFactor) || baseScalingFactor <= 0.0 ||
        !std::isfinite(expectedInputScalingFactor_) || expectedInputScalingFactor_ <= 0.0) {
        Invalid("the FIXEDMANUAL scaling factor is invalid");
    }
}

void DoubleCKKS::ValidateCiphertext(const ReadOnlyCiphertext& ciphertext,
                                    const std::vector<lbcrypto::NativeInteger>& orderedModuli,
                                    std::size_t level,
                                    std::size_t noiseScaleDegree,
                                    double recordedScalingFactor,
                                    const std::string& keyTag,
                                    std::uint32_t slots,
                                    std::size_t componentCount,
                                    const char* stateLabel,
                                    const char* label) const {
    if (!ciphertext) {
        Invalid(std::string(label) + " is null");
    }
    if (ciphertext->GetCryptoContext().get() != context_.get()) {
        Invalid(std::string(label) + " belongs to a different context");
    }
    if (ciphertext->GetEncodingType() != lbcrypto::CKKS_PACKED_ENCODING) {
        Invalid(std::string(label) + " must use CKKS packed encoding metadata");
    }
    if (ciphertext->GetLevel() != level) {
        Invalid(std::string(label) + " level does not match its " + stateLabel + " state");
    }
    if (ciphertext->NumberCiphertextElements() != componentCount) {
        const std::string componentCountText =
            componentCount == 2 ? "two" : (componentCount == 3 ? "three" : std::to_string(componentCount));
        Invalid(std::string(label) + " must contain exactly " + componentCountText + " RLWE components");
    }
    if (ciphertext->GetNoiseScaleDeg() != noiseScaleDegree) {
        Invalid(std::string(label) + " noise-scale degree does not match its " + stateLabel + " state");
    }
    if (!std::isfinite(ciphertext->GetScalingFactor()) || ciphertext->GetScalingFactor() <= 0.0 ||
        ciphertext->GetScalingFactor() != recordedScalingFactor) {
        Invalid(std::string(label) + " recorded scaling factor does not match its " + stateLabel + " state");
    }
    if (ciphertext->GetKeyTag().empty() || ciphertext->GetKeyTag() != keyTag) {
        Invalid(std::string(label) + " key tag does not match its " + stateLabel + " state");
    }
    if (ciphertext->GetSlots() != slots) {
        Invalid(std::string(label) + " slots do not match its " + stateLabel + " state");
    }

    const auto& expectedTowerParameters = parameters_->GetElementParams()->GetParams();
    if (level > fullModuli_.size() || orderedModuli.size() != fullModuli_.size() - level) {
        Invalid(std::string(label) + " level and active-basis size disagree");
    }

    for (const auto& element : ciphertext->GetElements()) {
        if (element.GetFormat() != Format::EVALUATION) {
            Invalid(std::string(label) + " must be in evaluation format");
        }
        if (!SameOrderedModuli(OrderedModuli(element), orderedModuli)) {
            Invalid(std::string(label) + " ordered RNS basis mismatch");
        }
        const auto& towers = element.GetAllElements();
        for (std::size_t index = 0; index < towers.size(); ++index) {
            if (towers[index].GetRootOfUnity() != expectedTowerParameters[index]->GetRootOfUnity() ||
                towers[index].GetCyclotomicOrder() != expectedTowerParameters[index]->GetCyclotomicOrder()) {
                Invalid(std::string(label) + " tower parameters do not match the bound context");
            }
        }
    }
}

void DoubleCKKS::ValidateDcpInput(const ReadOnlyCiphertext& ciphertext) const {
    if (!ciphertext) {
        Invalid("DCP input is null");
    }
    if (ciphertext->GetEncodingType() != lbcrypto::CKKS_PACKED_ENCODING) {
        Invalid("DCP input must use CKKS packed encoding metadata");
    }
    if (ciphertext->GetLevel() != 0) {
        Invalid("DCP input must be at level zero");
    }
    if (ciphertext->GetNoiseScaleDeg() != 2) {
        Invalid("DCP input must have noise-scale degree two");
    }
    if (!std::isfinite(ciphertext->GetScalingFactor()) ||
        ciphertext->GetScalingFactor() != expectedInputScalingFactor_) {
        Invalid("DCP input must have the exact fresh degree-two FIXEDMANUAL scaling factor");
    }
    ValidateCiphertext(ciphertext, fullModuli_, 0, 2, expectedInputScalingFactor_, ciphertext->GetKeyTag(),
                       ciphertext->GetSlots(), 2, "pair", "DCP input");
}

std::pair<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>, lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>
DoubleCKKS::DecomposeValidatedCiphertext(const ReadOnlyCiphertext& ciphertext) const {
    std::vector<lbcrypto::NativeInteger> quotientFactors;
    std::vector<lbcrypto::NativeInteger> divisorInverses;
    quotientFactors.reserve(firstPairModuli_.size());
    divisorInverses.reserve(firstPairModuli_.size());
    for (const auto& modulus : firstPairModuli_) {
        const auto divisorInverse = divisor_.ModInverse(modulus);
        divisorInverses.push_back(divisorInverse);
        quotientFactors.push_back(modulus - divisorInverse);
    }

    std::vector<lbcrypto::DCRTPoly> highElements;
    std::vector<lbcrypto::DCRTPoly> lowElements;
    highElements.reserve(ciphertext->GetElements().size());
    lowElements.reserve(ciphertext->GetElements().size());

    for (const auto& source : ciphertext->GetElements()) {
        auto high = source;
        high.DropLastElementAndScale(quotientFactors, divisorInverses);

        auto sourcePrefix = source;
        sourcePrefix.DropLastElement();
        auto scaledHigh = high;
        scaledHigh *= divisor_;
        sourcePrefix -= scaledHigh;

        highElements.push_back(std::move(high));
        lowElements.push_back(std::move(sourcePrefix));
    }

    auto highCiphertext = ciphertext->Clone();
    highCiphertext->SetElements(std::move(highElements));
    highCiphertext->SetLevel(1);

    auto lowCiphertext = ciphertext->Clone();
    lowCiphertext->SetElements(std::move(lowElements));
    lowCiphertext->SetLevel(1);

    return {std::move(highCiphertext), std::move(lowCiphertext)};
}

CiphertextPair DoubleCKKS::DCP(const ReadOnlyCiphertext& ciphertext) const {
    ValidateDcpInput(ciphertext);

    auto [highCiphertext, lowCiphertext] = DecomposeValidatedCiphertext(ciphertext);

    const double recordedScalingFactor = ciphertext->GetScalingFactor();
    PaperScaleDescriptor paperScale{
        recordedScalingFactor,
        divisor_,
        static_cast<long double>(recordedScalingFactor) /
            static_cast<long double>(divisor_.ConvertToInt()),
        static_cast<long double>(recordedScalingFactor),
    };

    CiphertextPair pair(std::move(highCiphertext), std::move(lowCiphertext), context_.get(), divisor_,
                        firstPairModuli_, 1, paperScale, recordedScalingFactor, 2,
                        PairLifecycle::ReadyForFirstMult, ciphertext->GetKeyTag(), ciphertext->GetSlots(),
                        Format::EVALUATION, 2);
    ValidatePair(pair);
    return pair;
}

void DoubleCKKS::ValidatePair(const CiphertextPair& pair) const {
    if (pair.contextIdentity_ != context_.get()) {
        Invalid("pair belongs to a different context");
    }
    if (pair.divisor_ != divisor_) {
        Invalid("pair divisor does not match the bound context");
    }
    if (pair.componentCount_ != 2 || pair.format_ != Format::EVALUATION) {
        Invalid("pair shape or format is invalid");
    }

    std::vector<lbcrypto::NativeInteger> refreshModuli;
    if (pair.lifecycle_ == PairLifecycle::RefreshRequired) {
        if (pair.level_ != 2) {
            Invalid("pair level is outside the supported context basis");
        }
        refreshModuli.assign(firstPairModuli_.begin(), firstPairModuli_.end() - 1);
        if (!SameOrderedModuli(pair.orderedModuli_, refreshModuli)) {
            Invalid("pair ordered RNS basis does not match its level");
        }
    }
    else {
        if (pair.level_ != 1) {
            Invalid("pair level is outside the supported context basis");
        }
        if (!SameOrderedModuli(pair.orderedModuli_, firstPairModuli_)) {
            Invalid("pair ordered RNS basis does not match its level");
        }
    }

    double expectedRecordedScalingFactor = 0.0;
    std::size_t expectedNoiseScaleDegree = 0;
    long double expectedLogicalScalingFactor = 0.0L;
    long double expectedRecombinedLogicalScalingFactor = 0.0L;
    switch (pair.lifecycle_) {
        case PairLifecycle::ReadyForFirstMult:
            expectedRecordedScalingFactor = expectedInputScalingFactor_;
            expectedNoiseScaleDegree = 2;
            expectedLogicalScalingFactor =
                static_cast<long double>(expectedInputScalingFactor_) /
                static_cast<long double>(divisor_.ConvertToInt());
            expectedRecombinedLogicalScalingFactor =
                static_cast<long double>(expectedInputScalingFactor_);
            break;
        case PairLifecycle::ReadyForRS2: {
            const double baseScalingFactor = parameters_->GetScalingFactorReal(0);
            expectedRecordedScalingFactor =
                expectedInputScalingFactor_ * expectedInputScalingFactor_ / baseScalingFactor;
            expectedNoiseScaleDegree = 3;
            const long double divisor = static_cast<long double>(divisor_.ConvertToInt());
            const long double inputHighScalingFactor =
                static_cast<long double>(expectedInputScalingFactor_) / divisor;
            expectedLogicalScalingFactor = inputHighScalingFactor * inputHighScalingFactor;
            expectedRecombinedLogicalScalingFactor =
                static_cast<long double>(expectedInputScalingFactor_) *
                static_cast<long double>(expectedInputScalingFactor_) / divisor;
            break;
        }
        case PairLifecycle::RefreshRequired: {
            const lbcrypto::NativeInteger qL = firstPairModuli_.back();
            if (qL.Mod(lbcrypto::NativeInteger(2)) != lbcrypto::NativeInteger(1)) {
                Invalid("the last active Q tower used as q_l must be odd");
            }
            if (qL == divisor_) {
                Invalid("q_l must remain distinct from q_div");
            }

            const std::size_t modReduceFactorIndex = firstPairModuli_.size() - 1;
            const auto& fullTowerParameters = parameters_->GetElementParams()->GetParams();
            if (modReduceFactorIndex >= fullTowerParameters.size() ||
                !fullTowerParameters[modReduceFactorIndex] ||
                fullTowerParameters[modReduceFactorIndex]->GetModulus() != qL) {
                Invalid("q_l does not match the OpenFHE mod-reduce factor index");
            }
            if (fullTowerParameters.empty() || !fullTowerParameters.back() ||
                fullTowerParameters.back()->GetModulus() != divisor_) {
                Invalid("q_div does not match the final full-basis tower");
            }
            const double recordedFactorDivisor = parameters_->GetModReduceFactor(modReduceFactorIndex);
            if (!std::isfinite(recordedFactorDivisor) || recordedFactorDivisor <= 0.0) {
                Invalid("the OpenFHE mod-reduce factor is invalid");
            }

            const double baseScalingFactor = parameters_->GetScalingFactorReal(0);
            const double readyForRs2RecordedScalingFactor =
                expectedInputScalingFactor_ * expectedInputScalingFactor_ / baseScalingFactor;
            expectedRecordedScalingFactor =
                readyForRs2RecordedScalingFactor / recordedFactorDivisor;
            expectedNoiseScaleDegree = 2;

            const long double divisor = static_cast<long double>(divisor_.ConvertToInt());
            const long double qLAsLongDouble = static_cast<long double>(qL.ConvertToInt());
            const long double inputHighScalingFactor =
                static_cast<long double>(expectedInputScalingFactor_) / divisor;
            expectedLogicalScalingFactor =
                inputHighScalingFactor * inputHighScalingFactor / qLAsLongDouble;
            expectedRecombinedLogicalScalingFactor =
                static_cast<long double>(expectedInputScalingFactor_) *
                static_cast<long double>(expectedInputScalingFactor_) /
                divisor / qLAsLongDouble;
            break;
        }
        default:
            Invalid("pair lifecycle is invalid");
    }

    if (!std::isfinite(pair.recordedScalingFactor_) ||
        pair.recordedScalingFactor_ != expectedRecordedScalingFactor) {
        Invalid("pair scale metadata is invalid");
    }
    if (pair.noiseScaleDegree_ != expectedNoiseScaleDegree) {
        Invalid("pair noise-scale degree is invalid");
    }
    if (pair.paperScale_.inputRecordedScalingFactor != pair.recordedScalingFactor_ ||
        pair.paperScale_.divisor != divisor_ ||
        !std::isfinite(pair.paperScale_.approximateLogicalScalingFactor) ||
        pair.paperScale_.approximateLogicalScalingFactor != expectedLogicalScalingFactor) {
        Invalid("pair paper-scale descriptor is inconsistent");
    }
    if (!std::isfinite(pair.paperScale_.approximateRecombinedLogicalScalingFactor) ||
        pair.paperScale_.approximateRecombinedLogicalScalingFactor !=
            expectedRecombinedLogicalScalingFactor) {
        Invalid("pair recombined logical scale is inconsistent");
    }
    if (pair.keyTag_.empty()) {
        Invalid("pair key tag is empty");
    }

    ValidateCiphertext(pair.high_, pair.orderedModuli_, pair.level_, expectedNoiseScaleDegree,
                       pair.recordedScalingFactor_, pair.keyTag_, pair.slots_, 2, "pair", "pair high");
    ValidateCiphertext(pair.low_, pair.orderedModuli_, pair.level_, expectedNoiseScaleDegree,
                       pair.recordedScalingFactor_, pair.keyTag_, pair.slots_, 2, "pair", "pair low");
}

void DoubleCKKS::ValidateTensorCompatibility(const CiphertextPair& left, const CiphertextPair& right) const {
    if (left.contextIdentity_ != right.contextIdentity_) {
        Invalid("Tensor2 input contexts do not match");
    }
    if (left.divisor_ != right.divisor_) {
        Invalid("Tensor2 input divisors do not match");
    }
    if (!SameOrderedModuli(left.orderedModuli_, right.orderedModuli_)) {
        Invalid("Tensor2 input ordered RNS bases do not match");
    }
    if (left.level_ != right.level_) {
        Invalid("Tensor2 input levels do not match");
    }
    if (left.recordedScalingFactor_ != right.recordedScalingFactor_) {
        Invalid("Tensor2 input recorded scaling factors do not match");
    }
    if (left.noiseScaleDegree_ != right.noiseScaleDegree_) {
        Invalid("Tensor2 input noise-scale degrees do not match");
    }
    if (left.lifecycle_ != right.lifecycle_) {
        Invalid("Tensor2 input lifecycles do not match");
    }
    if (left.keyTag_ != right.keyTag_) {
        Invalid("Tensor2 input key tags do not match");
    }
    if (left.slots_ != right.slots_) {
        Invalid("Tensor2 input slots do not match");
    }
    if (left.format_ != right.format_) {
        Invalid("Tensor2 input formats do not match");
    }
    if (left.componentCount_ != right.componentCount_) {
        Invalid("Tensor2 input component counts do not match");
    }
}

void DoubleCKKS::ValidateTensorResult(const TensorCiphertextPair& pair) const {
    if (pair.contextIdentity_ != context_.get()) {
        Invalid("Tensor2 result belongs to a different context");
    }
    if (pair.divisor_ != divisor_) {
        Invalid("Tensor2 result divisor does not match the bound context");
    }
    if (pair.componentCount_ != 3 || pair.format_ != Format::EVALUATION) {
        Invalid("Tensor2 result shape or format is invalid");
    }
    if (pair.level_ != 1 || !SameOrderedModuli(pair.orderedModuli_, firstPairModuli_)) {
        Invalid("Tensor2 result basis or level is invalid");
    }

    const double baseScalingFactor = parameters_->GetScalingFactorReal(0);
    const double expectedRecordedScalingFactor =
        expectedInputScalingFactor_ * expectedInputScalingFactor_ / baseScalingFactor;
    if (!std::isfinite(pair.recordedScalingFactor_) || pair.recordedScalingFactor_ <= 0.0 ||
        pair.recordedScalingFactor_ != expectedRecordedScalingFactor) {
        Invalid("Tensor2 result scale metadata is invalid");
    }
    if (pair.noiseScaleDegree_ != 3) {
        Invalid("Tensor2 result noise-scale degree is invalid");
    }
    if (pair.keyTag_.empty()) {
        Invalid("Tensor2 result key tag is empty");
    }

    const long double divisor = static_cast<long double>(divisor_.ConvertToInt());
    const long double inputHighScale = static_cast<long double>(expectedInputScalingFactor_) / divisor;
    const long double expectedHighScale = inputHighScale * inputHighScale;
    const long double expectedRecombinedScale =
        static_cast<long double>(expectedInputScalingFactor_) *
        static_cast<long double>(expectedInputScalingFactor_) / divisor;
    if (!std::isfinite(pair.tensorScale_.approximateHighLogicalScalingFactor) ||
        !std::isfinite(pair.tensorScale_.approximateRecombinedLogicalScalingFactor) ||
        pair.tensorScale_.approximateHighLogicalScalingFactor != expectedHighScale ||
        pair.tensorScale_.approximateRecombinedLogicalScalingFactor != expectedRecombinedScale) {
        Invalid("Tensor2 result paper-scale descriptor is inconsistent");
    }

    ValidateCiphertext(pair.high_, pair.orderedModuli_, pair.level_, pair.noiseScaleDegree_,
                       pair.recordedScalingFactor_, pair.keyTag_, pair.slots_, 3, "Tensor2", "Tensor2 high");
    ValidateCiphertext(pair.low_, pair.orderedModuli_, pair.level_, pair.noiseScaleDegree_,
                       pair.recordedScalingFactor_, pair.keyTag_, pair.slots_, 3, "Tensor2", "Tensor2 low");
}

TensorCiphertextPair DoubleCKKS::Tensor2(const CiphertextPair& left, const CiphertextPair& right) const {
    ValidatePair(left);
    ValidatePair(right);
    if (left.lifecycle_ != PairLifecycle::ReadyForFirstMult ||
        right.lifecycle_ != PairLifecycle::ReadyForFirstMult) {
        Invalid("Tensor2 requires ReadyForFirstMult inputs");
    }
    ValidateTensorCompatibility(left, right);

    lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly> leftHigh = left.high_;
    lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly> leftLow = left.low_;
    lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly> rightHigh = right.high_;
    lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly> rightLow = right.low_;

    auto high3 = context_->EvalMultNoRelin(leftHigh, rightHigh);
    auto cross12 = context_->EvalMultNoRelin(leftHigh, rightLow);
    auto cross21 = context_->EvalMultNoRelin(leftLow, rightHigh);
    lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly> cross12Const = cross12;
    lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly> cross21Const = cross21;
    auto low3 = context_->EvalAdd(cross12Const, cross21Const);

    const double baseScalingFactor = parameters_->GetScalingFactorReal(0);
    const double rawRecordedScalingFactor = left.recordedScalingFactor_ * right.recordedScalingFactor_;
    const double normalizedRecordedScalingFactor = rawRecordedScalingFactor / baseScalingFactor;
    const std::size_t rawNoiseScaleDegree = left.noiseScaleDegree_ + right.noiseScaleDegree_;
    const std::size_t normalizedNoiseScaleDegree = rawNoiseScaleDegree - 1;

    if (high3->GetNoiseScaleDeg() != rawNoiseScaleDegree || low3->GetNoiseScaleDeg() != rawNoiseScaleDegree ||
        high3->GetScalingFactor() != rawRecordedScalingFactor || low3->GetScalingFactor() != rawRecordedScalingFactor) {
        Invalid("Tensor2 OpenFHE raw multiplication metadata is unexpected");
    }

    high3->SetNoiseScaleDeg(normalizedNoiseScaleDegree);
    high3->SetScalingFactor(normalizedRecordedScalingFactor);
    low3->SetNoiseScaleDeg(normalizedNoiseScaleDegree);
    low3->SetScalingFactor(normalizedRecordedScalingFactor);

    TensorScaleDescriptor tensorScale{
        left.paperScale_.approximateLogicalScalingFactor * right.paperScale_.approximateLogicalScalingFactor,
        left.paperScale_.approximateRecombinedLogicalScalingFactor *
            right.paperScale_.approximateRecombinedLogicalScalingFactor /
            static_cast<long double>(divisor_.ConvertToInt()),
    };

    TensorCiphertextPair result(std::move(high3), std::move(low3), context_.get(), divisor_, left.orderedModuli_,
                                left.level_, tensorScale, normalizedRecordedScalingFactor,
                                normalizedNoiseScaleDegree, left.keyTag_, left.slots_, Format::EVALUATION, 3);
    ValidateTensorResult(result);
    return result;
}

CiphertextPair DoubleCKKS::Relin2(const TensorCiphertextPair& tensor) const {
    ValidateTensorResult(tensor);
    if (tensor.GetOrderedModuli().size() < tensor.GetNoiseScaleDegree()) {
        Invalid("Relin2 requires at least as many active Q_l towers as the Tensor noise-scale degree");
    }
    const auto& evaluationKeys = lbcrypto::CryptoContextImpl<lbcrypto::DCRTPoly>::GetAllEvalMultKeys();
    const auto evaluationKey = evaluationKeys.find(tensor.GetKeyTag());
    if (evaluationKey == evaluationKeys.end()) {
        Invalid("Relin2 evaluation key is missing for the Tensor key tag");
    }
    if (evaluationKey->second.empty()) {
        Invalid("Relin2 evaluation-key vector is empty");
    }
    const auto& firstEvaluationKey = evaluationKey->second.front();
    if (!firstEvaluationKey) {
        Invalid("Relin2 first evaluation key is null");
    }
    if (firstEvaluationKey->GetCryptoContext().get() != context_.get()) {
        Invalid("Relin2 first evaluation key belongs to a different context");
    }
    if (firstEvaluationKey->GetKeyTag() != tensor.GetKeyTag()) {
        Invalid("Relin2 first evaluation key tag does not match the Tensor key tag");
    }
    const auto relinearizationKey =
        std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<lbcrypto::DCRTPoly>>(firstEvaluationKey);
    if (!relinearizationKey) {
        Invalid("Relin2 first evaluation key has the wrong concrete subtype");
    }
    const auto keySwitchTechnique = parameters_->GetKeySwitchTechnique();
    if (keySwitchTechnique != lbcrypto::BV && keySwitchTechnique != lbcrypto::HYBRID) {
        Invalid("Relin2 key-switch technique is unsupported");
    }
    if (keySwitchTechnique == lbcrypto::BV && parameters_->GetDigitSize() == 0 &&
        relinearizationKey->GetAVector().size() != parameters_->GetElementParams()->GetParams().size()) {
        Invalid("Relin2 evaluation key BV A vector length mismatch");
    }
    if (keySwitchTechnique == lbcrypto::BV && parameters_->GetDigitSize() == 0 &&
        relinearizationKey->GetBVector().size() != parameters_->GetElementParams()->GetParams().size()) {
        Invalid("Relin2 evaluation key BV B vector length mismatch");
    }
    if (keySwitchTechnique == lbcrypto::BV) {
        const auto expectedBasis = parameters_->GetElementParams();
        for (const auto& entry : relinearizationKey->GetAVector()) {
            if (!HasCompleteOrderedBasis(entry, expectedBasis)) {
                Invalid("Relin2 evaluation key BV entry basis mismatch");
            }
        }
        for (const auto& entry : relinearizationKey->GetBVector()) {
            if (!HasCompleteOrderedBasis(entry, expectedBasis)) {
                Invalid("Relin2 evaluation key BV entry basis mismatch");
            }
        }
    }
    if (keySwitchTechnique == lbcrypto::BV && parameters_->GetDigitSize() != 0) {
        const auto digitSize = parameters_->GetDigitSize();
        std::size_t expectedVectorSize = 0;
        for (const auto& towerParameters : parameters_->GetElementParams()->GetParams()) {
            const auto towerBits = towerParameters->GetModulus().GetMSB();
            expectedVectorSize += static_cast<std::size_t>((towerBits + digitSize - 1) / digitSize);
        }
        if (relinearizationKey->GetAVector().size() != expectedVectorSize) {
            Invalid("Relin2 evaluation key BV A vector length mismatch");
        }
        if (relinearizationKey->GetBVector().size() != expectedVectorSize) {
            Invalid("Relin2 evaluation key BV B vector length mismatch");
        }
    }
    if (keySwitchTechnique == lbcrypto::BV) {
        for (const auto& entry : relinearizationKey->GetAVector()) {
            if (!IsInEvaluationFormat(entry)) {
                Invalid("Relin2 evaluation key BV entry must be in evaluation format");
            }
        }
        for (const auto& entry : relinearizationKey->GetBVector()) {
            if (!IsInEvaluationFormat(entry)) {
                Invalid("Relin2 evaluation key BV entry must be in evaluation format");
            }
        }
    }
    if (keySwitchTechnique == lbcrypto::HYBRID &&
        relinearizationKey->GetAVector().size() != static_cast<std::size_t>(parameters_->GetNumPartQ())) {
        Invalid("Relin2 evaluation key HYBRID A vector length mismatch");
    }
    if (keySwitchTechnique == lbcrypto::HYBRID &&
        relinearizationKey->GetBVector().size() != static_cast<std::size_t>(parameters_->GetNumPartQ())) {
        Invalid("Relin2 evaluation key HYBRID B vector length mismatch");
    }
    if (keySwitchTechnique == lbcrypto::HYBRID) {
        const auto expectedBasis = parameters_->GetParamsQP();
        for (const auto& entry : relinearizationKey->GetAVector()) {
            if (!HasCompleteOrderedBasis(entry, expectedBasis)) {
                Invalid("Relin2 evaluation key HYBRID entry basis mismatch");
            }
        }
        for (const auto& entry : relinearizationKey->GetBVector()) {
            if (!HasCompleteOrderedBasis(entry, expectedBasis)) {
                Invalid("Relin2 evaluation key HYBRID entry basis mismatch");
            }
        }
        for (const auto& entry : relinearizationKey->GetAVector()) {
            if (!IsInEvaluationFormat(entry)) {
                Invalid("Relin2 evaluation key HYBRID entry must be in evaluation format");
            }
        }
        for (const auto& entry : relinearizationKey->GetBVector()) {
            if (!IsInEvaluationFormat(entry)) {
                Invalid("Relin2 evaluation key HYBRID entry must be in evaluation format");
            }
        }
    }

    auto raisedHigh = tensor.high_->Clone();
    auto raisedHighElements = raisedHigh->GetElements();
    const auto& fullTowerParameters = parameters_->GetElementParams()->GetParams();
    for (auto& element : raisedHighElements) {
        auto towers = element.GetAllElements();
        for (auto& tower : towers) {
            tower *= divisor_;
        }
        towers.emplace_back(fullTowerParameters.back(), Format::EVALUATION, true);
        element = lbcrypto::DCRTPoly(towers);
    }
    raisedHigh->SetElements(std::move(raisedHighElements));
    raisedHigh->SetLevel(0);

    ReadOnlyCiphertext raisedHighReadOnly = raisedHigh;
    ValidateCiphertext(raisedHighReadOnly, fullModuli_, 0, tensor.noiseScaleDegree_,
                       tensor.recordedScalingFactor_, tensor.keyTag_, tensor.slots_, 3,
                       "Relin2 raised-high", "Relin2 raised high");

    lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly> raisedHighConst = raisedHigh;
    auto relinearizedHigh = context_->Relinearize(raisedHighConst);
    lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly> lowConst = tensor.low_;
    auto relinearizedLow = context_->Relinearize(lowConst);

    ReadOnlyCiphertext relinearizedHighReadOnly = relinearizedHigh;
    ValidateCiphertext(relinearizedHighReadOnly, fullModuli_, 0, tensor.noiseScaleDegree_,
                       tensor.recordedScalingFactor_, tensor.keyTag_, tensor.slots_, 2,
                       "Relin2 relinearized-high", "Relin2 relinearized high");

    auto [high, remainder] = DecomposeValidatedCiphertext(relinearizedHighReadOnly);
    ReadOnlyCiphertext highReadOnly = high;
    ReadOnlyCiphertext remainderReadOnly = remainder;
    ReadOnlyCiphertext relinearizedLowReadOnly = relinearizedLow;
    ValidateCiphertext(highReadOnly, tensor.orderedModuli_, tensor.level_, tensor.noiseScaleDegree_,
                       tensor.recordedScalingFactor_, tensor.keyTag_, tensor.slots_, 2,
                       "Relin2 private-DCP", "Relin2 private-DCP quotient");
    ValidateCiphertext(remainderReadOnly, tensor.orderedModuli_, tensor.level_, tensor.noiseScaleDegree_,
                       tensor.recordedScalingFactor_, tensor.keyTag_, tensor.slots_, 2,
                       "Relin2 private-DCP", "Relin2 private-DCP remainder");
    ValidateCiphertext(relinearizedLowReadOnly, tensor.orderedModuli_, tensor.level_, tensor.noiseScaleDegree_,
                       tensor.recordedScalingFactor_, tensor.keyTag_, tensor.slots_, 2,
                       "Relin2 relinearized-low", "Relin2 relinearized low");

    lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly> remainderConst = remainder;
    lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly> relinearizedLowConst = relinearizedLow;
    auto low = context_->EvalAdd(remainderConst, relinearizedLowConst);
    ReadOnlyCiphertext lowReadOnly = low;
    ValidateCiphertext(lowReadOnly, tensor.orderedModuli_, tensor.level_, tensor.noiseScaleDegree_,
                       tensor.recordedScalingFactor_, tensor.keyTag_, tensor.slots_, 2,
                       "Relin2 result", "Relin2 result low");

    PaperScaleDescriptor paperScale{
        tensor.recordedScalingFactor_,
        divisor_,
        tensor.tensorScale_.approximateHighLogicalScalingFactor,
        tensor.tensorScale_.approximateRecombinedLogicalScalingFactor,
    };
    CiphertextPair result(std::move(high), std::move(low), context_.get(), divisor_,
                          tensor.orderedModuli_, tensor.level_, paperScale,
                          tensor.recordedScalingFactor_, tensor.noiseScaleDegree_,
                          PairLifecycle::ReadyForRS2, tensor.keyTag_, tensor.slots_,
                          Format::EVALUATION, 2);
    ValidatePair(result);
    return result;
}

CiphertextPair DoubleCKKS::RS2(const CiphertextPair& relinearized) const {
    ValidatePair(relinearized);
    if (relinearized.lifecycle_ != PairLifecycle::ReadyForRS2) {
        Invalid("RS2 requires ReadyForRS2 input");
    }

    const lbcrypto::NativeInteger qDiv = relinearized.divisor_;
    const lbcrypto::NativeInteger qL = relinearized.orderedModuli_.back();
    if (qL.Mod(lbcrypto::NativeInteger(2)) != lbcrypto::NativeInteger(1)) {
        Invalid("the last active Q tower used as q_l must be odd");
    }
    if (qL == qDiv) {
        Invalid("q_l must remain distinct from q_div");
    }

    const std::size_t modReduceFactorIndex = relinearized.orderedModuli_.size() - 1;
    const auto& fullTowerParameters = parameters_->GetElementParams()->GetParams();
    if (modReduceFactorIndex >= fullTowerParameters.size() ||
        !fullTowerParameters[modReduceFactorIndex] ||
        fullTowerParameters[modReduceFactorIndex]->GetModulus() != qL) {
        Invalid("q_l does not match the OpenFHE mod-reduce factor index");
    }
    if (fullTowerParameters.empty() || !fullTowerParameters.back() ||
        fullTowerParameters.back()->GetModulus() != qDiv) {
        Invalid("q_div does not match the final full-basis tower");
    }
    const double recordedFactorDivisor = parameters_->GetModReduceFactor(modReduceFactorIndex);
    if (!std::isfinite(recordedFactorDivisor) || recordedFactorDivisor <= 0.0) {
        Invalid("the OpenFHE mod-reduce factor is invalid");
    }

    std::vector<lbcrypto::NativeInteger> outputModuli(relinearized.orderedModuli_.begin(),
                                                      relinearized.orderedModuli_.end() - 1);
    const std::size_t outputLevel = relinearized.level_ + 1;
    const std::size_t outputNoiseScaleDegree = relinearized.noiseScaleDegree_ - 1;
    const double outputRecordedScalingFactor =
        relinearized.recordedScalingFactor_ / recordedFactorDivisor;
    if (!std::isfinite(outputRecordedScalingFactor) || outputRecordedScalingFactor <= 0.0) {
        Invalid("the RS2 output recorded scaling factor is invalid");
    }

    const long double qLAsLongDouble = static_cast<long double>(qL.ConvertToInt());
    const long double outputHighLogicalScalingFactor =
        relinearized.paperScale_.approximateLogicalScalingFactor / qLAsLongDouble;
    const long double outputRecombinedLogicalScalingFactor =
        relinearized.paperScale_.approximateRecombinedLogicalScalingFactor / qLAsLongDouble;
    if (!std::isfinite(outputHighLogicalScalingFactor) ||
        !std::isfinite(outputRecombinedLogicalScalingFactor)) {
        Invalid("the RS2 output logical scaling factors are invalid");
    }

    auto highInput = relinearized.high_->Clone();
    ReadOnlyCiphertext highInputReadOnly = highInput;
    ValidateCiphertext(highInputReadOnly, relinearized.orderedModuli_, relinearized.level_,
                       relinearized.noiseScaleDegree_, relinearized.recordedScalingFactor_,
                       relinearized.keyTag_, relinearized.slots_, 2, "RS2 input", "RS2 high input clone");

    auto recombinedInput = RCB(relinearized);
    ReadOnlyCiphertext recombinedInputReadOnly = recombinedInput;
    ValidateCiphertext(recombinedInputReadOnly, relinearized.orderedModuli_, relinearized.level_,
                       relinearized.noiseScaleDegree_, relinearized.recordedScalingFactor_,
                       relinearized.keyTag_, relinearized.slots_, 2, "RS2 input",
                       "RS2 recombined input");

    lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly> highInputConst = highInput;
    lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly> recombinedInputConst = recombinedInput;
    // Definition 4.5 requires these two independently rounded rescale calls.
    auto rescaledHigh = context_->Rescale(highInputConst);
    auto rescaledRecombined = context_->Rescale(recombinedInputConst);

    ReadOnlyCiphertext rescaledHighReadOnly = rescaledHigh;
    ReadOnlyCiphertext rescaledRecombinedReadOnly = rescaledRecombined;
    ValidateCiphertext(rescaledHighReadOnly, outputModuli, outputLevel, outputNoiseScaleDegree,
                       outputRecordedScalingFactor, relinearized.keyTag_, relinearized.slots_, 2,
                       "RS2 output", "RS2 rescaled high");
    ValidateCiphertext(rescaledRecombinedReadOnly, outputModuli, outputLevel,
                       outputNoiseScaleDegree, outputRecordedScalingFactor,
                       relinearized.keyTag_, relinearized.slots_, 2, "RS2 output",
                       "RS2 rescaled recombined");

    lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly> rescaledHighConst = rescaledHigh;
    auto scaledRescaledHigh = context_->EvalMultNoCheck(rescaledHighConst, qDiv);
    ReadOnlyCiphertext scaledRescaledHighReadOnly = scaledRescaledHigh;
    ValidateCiphertext(scaledRescaledHighReadOnly, outputModuli, outputLevel,
                       outputNoiseScaleDegree, outputRecordedScalingFactor,
                       relinearized.keyTag_, relinearized.slots_, 2, "RS2 output",
                       "RS2 q_div-scaled high");

    // Both operands were validated in the identical post-rescale state, so a
    // direct component subtraction cannot trigger hidden level/degree adjustment.
    auto newLow = rescaledRecombined->Clone();
    auto& newLowElements = newLow->GetElements();
    const auto& scaledHighElements = scaledRescaledHigh->GetElements();
    if (newLowElements.size() != scaledHighElements.size()) {
        Invalid("RS2 post-rescale component counts do not match");
    }
    for (std::size_t index = 0; index < newLowElements.size(); ++index) {
        const auto& recombinedBasis = newLowElements[index].GetParams();
        const auto& scaledHighBasis = scaledHighElements[index].GetParams();
        if (!recombinedBasis || !scaledHighBasis || !(*recombinedBasis == *scaledHighBasis)) {
            Invalid("RS2 post-rescale subtraction bases do not match");
        }
        newLowElements[index] -= scaledHighElements[index];
    }
    ReadOnlyCiphertext newLowReadOnly = newLow;
    ValidateCiphertext(newLowReadOnly, outputModuli, outputLevel, outputNoiseScaleDegree,
                       outputRecordedScalingFactor, relinearized.keyTag_, relinearized.slots_, 2,
                       "RS2 output", "RS2 new low");

    PaperScaleDescriptor paperScale{
        outputRecordedScalingFactor,
        qDiv,
        outputHighLogicalScalingFactor,
        outputRecombinedLogicalScalingFactor,
    };
    CiphertextPair result(std::move(rescaledHigh), std::move(newLow), context_.get(), qDiv,
                          std::move(outputModuli), outputLevel, paperScale,
                          outputRecordedScalingFactor, outputNoiseScaleDegree,
                          PairLifecycle::RefreshRequired, relinearized.keyTag_,
                          relinearized.slots_, Format::EVALUATION, 2);
    ValidatePair(result);
    return result;
}

CiphertextPair DoubleCKKS::Mult2(const CiphertextPair& left, const CiphertextPair& right) const {
    return RS2(Relin2(Tensor2(left, right)));
}

lbcrypto::Ciphertext<lbcrypto::DCRTPoly> DoubleCKKS::RCB(const CiphertextPair& pair) const {
    ValidatePair(pair);

    auto result = pair.high_->Clone();
    auto& resultElements = result->GetElements();
    const auto& lowElements = pair.low_->GetElements();
    for (std::size_t index = 0; index < resultElements.size(); ++index) {
        resultElements[index] *= divisor_;
        resultElements[index] += lowElements[index];
    }
    return result;
}

}  // namespace openfhe_2023_1788
