#include "openfhe_2023_1788/double_ckks.h"

#include <cmath>
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
        Invalid(std::string(label) + " level does not match its pair state");
    }
    if (ciphertext->NumberCiphertextElements() != 2) {
        Invalid(std::string(label) + " must contain exactly two RLWE components");
    }
    if (ciphertext->GetNoiseScaleDeg() != noiseScaleDegree) {
        Invalid(std::string(label) + " noise-scale degree does not match its pair state");
    }
    if (!std::isfinite(ciphertext->GetScalingFactor()) || ciphertext->GetScalingFactor() <= 0.0 ||
        ciphertext->GetScalingFactor() != recordedScalingFactor) {
        Invalid(std::string(label) + " recorded scaling factor does not match its pair state");
    }
    if (ciphertext->GetKeyTag().empty() || ciphertext->GetKeyTag() != keyTag) {
        Invalid(std::string(label) + " key tag does not match its pair state");
    }
    if (ciphertext->GetSlots() != slots) {
        Invalid(std::string(label) + " slots do not match its pair state");
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
                       ciphertext->GetSlots(), "DCP input");
}

CiphertextPair DoubleCKKS::DCP(const ReadOnlyCiphertext& ciphertext) const {
    ValidateDcpInput(ciphertext);

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

    const double recordedScalingFactor = ciphertext->GetScalingFactor();
    PaperScaleDescriptor paperScale{
        recordedScalingFactor,
        divisor_,
        static_cast<long double>(recordedScalingFactor) /
            static_cast<long double>(divisor_.ConvertToInt()),
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
    if (pair.level_ == 0 || pair.level_ >= fullModuli_.size()) {
        Invalid("pair level is outside the supported context basis");
    }

    std::vector<lbcrypto::NativeInteger> expectedModuli(fullModuli_.begin(), fullModuli_.end() - pair.level_);
    if (!SameOrderedModuli(pair.orderedModuli_, expectedModuli)) {
        Invalid("pair ordered RNS basis does not match its level");
    }
    if (!std::isfinite(pair.recordedScalingFactor_) ||
        pair.recordedScalingFactor_ != expectedInputScalingFactor_) {
        Invalid("pair scale metadata is invalid");
    }
    const long double expectedLogicalScalingFactor =
        static_cast<long double>(pair.recordedScalingFactor_) /
        static_cast<long double>(divisor_.ConvertToInt());
    if (pair.paperScale_.inputRecordedScalingFactor != pair.recordedScalingFactor_ ||
        pair.paperScale_.divisor != divisor_ ||
        !std::isfinite(pair.paperScale_.approximateLogicalScalingFactor) ||
        pair.paperScale_.approximateLogicalScalingFactor != expectedLogicalScalingFactor) {
        Invalid("pair paper-scale descriptor is inconsistent");
    }
    if (pair.keyTag_.empty()) {
        Invalid("pair key tag is empty");
    }

    ValidateCiphertext(pair.high_, pair.orderedModuli_, pair.level_, pair.noiseScaleDegree_,
                       pair.recordedScalingFactor_, pair.keyTag_, pair.slots_, "pair high");
    ValidateCiphertext(pair.low_, pair.orderedModuli_, pair.level_, pair.noiseScaleDegree_,
                       pair.recordedScalingFactor_, pair.keyTag_, pair.slots_, "pair low");
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

void DoubleCKKS::ValidateTensorCiphertext(const ReadOnlyCiphertext& ciphertext,
                                          const std::vector<lbcrypto::NativeInteger>& orderedModuli,
                                          std::size_t level,
                                          std::size_t noiseScaleDegree,
                                          double recordedScalingFactor,
                                          const std::string& keyTag,
                                          std::uint32_t slots,
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
        Invalid(std::string(label) + " level does not match its Tensor2 state");
    }
    if (ciphertext->NumberCiphertextElements() != 3) {
        Invalid(std::string(label) + " must contain exactly three RLWE components");
    }
    if (ciphertext->GetNoiseScaleDeg() != noiseScaleDegree) {
        Invalid(std::string(label) + " noise-scale degree does not match its Tensor2 state");
    }
    if (!std::isfinite(ciphertext->GetScalingFactor()) || ciphertext->GetScalingFactor() <= 0.0 ||
        ciphertext->GetScalingFactor() != recordedScalingFactor) {
        Invalid(std::string(label) + " recorded scaling factor does not match its Tensor2 state");
    }
    if (ciphertext->GetKeyTag().empty() || ciphertext->GetKeyTag() != keyTag) {
        Invalid(std::string(label) + " key tag does not match its Tensor2 state");
    }
    if (ciphertext->GetSlots() != slots) {
        Invalid(std::string(label) + " slots do not match its Tensor2 state");
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

    ValidateTensorCiphertext(pair.high_, pair.orderedModuli_, pair.level_, pair.noiseScaleDegree_,
                             pair.recordedScalingFactor_, pair.keyTag_, pair.slots_, "Tensor2 high");
    ValidateTensorCiphertext(pair.low_, pair.orderedModuli_, pair.level_, pair.noiseScaleDegree_,
                             pair.recordedScalingFactor_, pair.keyTag_, pair.slots_, "Tensor2 low");
}

TensorCiphertextPair DoubleCKKS::Tensor2(const CiphertextPair& left, const CiphertextPair& right) const {
    ValidatePair(left);
    ValidatePair(right);
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
        static_cast<long double>(left.paperScale_.inputRecordedScalingFactor) *
            static_cast<long double>(right.paperScale_.inputRecordedScalingFactor) /
            static_cast<long double>(divisor_.ConvertToInt()),
    };

    TensorCiphertextPair result(std::move(high3), std::move(low3), context_.get(), divisor_, left.orderedModuli_,
                                left.level_, tensorScale, normalizedRecordedScalingFactor,
                                normalizedNoiseScaleDegree, left.keyTag_, left.slots_, Format::EVALUATION, 3);
    ValidateTensorResult(result);
    return result;
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
