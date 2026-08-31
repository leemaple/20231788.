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
                               long double paperScale,
                               double recordedScalingFactor,
                               std::size_t noiseScaleDegree,
                               PairLifecycle lifecycle,
                               std::string keyTag,
                               lbcrypto::Format format,
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

long double CiphertextPair::GetPaperScale() const noexcept {
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

lbcrypto::Format CiphertextPair::GetFormat() const noexcept {
    return format_;
}

std::size_t CiphertextPair::GetComponentCount() const noexcept {
    return componentCount_;
}

DoubleCKKS::DoubleCKKS(lbcrypto::CryptoContext<lbcrypto::DCRTPoly> context) : context_(std::move(context)) {
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
    if (!elementParameters || elementParameters->GetParams().size() < 4) {
        Invalid("the first Mult2 lifecycle requires at least four ordered Q towers");
    }

    fullModuli_.reserve(elementParameters->GetParams().size());
    for (const auto& towerParameters : elementParameters->GetParams()) {
        if (!towerParameters) {
            Invalid("context contains null tower parameters");
        }
        fullModuli_.push_back(towerParameters->GetModulus());
    }
    divisor_ = fullModuli_.back();
    firstPairModuli_.assign(fullModuli_.begin(), fullModuli_.end() - 1);
}

void DoubleCKKS::ValidateCiphertext(const ReadOnlyCiphertext& ciphertext,
                                    const std::vector<lbcrypto::NativeInteger>& orderedModuli,
                                    std::size_t level,
                                    std::size_t noiseScaleDegree,
                                    double recordedScalingFactor,
                                    const std::string& keyTag,
                                    const char* label) const {
    if (!ciphertext) {
        Invalid(std::string(label) + " is null");
    }
    if (ciphertext->GetCryptoContext().get() != context_.get()) {
        Invalid(std::string(label) + " belongs to a different context");
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

    const auto& expectedTowerParameters = parameters_->GetElementParams()->GetParams();
    if (level > fullModuli_.size() || orderedModuli.size() != fullModuli_.size() - level) {
        Invalid(std::string(label) + " level and active-basis size disagree");
    }

    for (const auto& element : ciphertext->GetElements()) {
        if (element.GetFormat() != lbcrypto::Format::EVALUATION) {
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
    if (!std::isfinite(ciphertext->GetScalingFactor()) || ciphertext->GetScalingFactor() <= 0.0) {
        Invalid("DCP input must have a finite positive recorded scaling factor");
    }
    ValidateCiphertext(ciphertext, fullModuli_, 0, 2, ciphertext->GetScalingFactor(), ciphertext->GetKeyTag(),
                       "DCP input");
}

CiphertextPair DoubleCKKS::DCP(const ReadOnlyCiphertext& ciphertext) const {
    ValidateDcpInput(ciphertext);

    std::vector<lbcrypto::DCRTPoly> highElements;
    std::vector<lbcrypto::DCRTPoly> lowElements;
    highElements.reserve(ciphertext->GetElements().size());
    lowElements.reserve(ciphertext->GetElements().size());

    for (const auto& source : ciphertext->GetElements()) {
        auto high = source;
        high.DropLastElementAndScale(parameters_->GetQlQlInvModqlDivqlModq(0), parameters_->GetqlInvModq(0));

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
    const long double paperScale = static_cast<long double>(recordedScalingFactor) /
                                   static_cast<long double>(divisor_.ConvertToInt());

    CiphertextPair pair(std::move(highCiphertext), std::move(lowCiphertext), context_.get(), divisor_,
                        firstPairModuli_, 1, paperScale, recordedScalingFactor, 2,
                        PairLifecycle::ReadyForFirstMult, ciphertext->GetKeyTag(), lbcrypto::Format::EVALUATION, 2);
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
    if (pair.componentCount_ != 2 || pair.format_ != lbcrypto::Format::EVALUATION) {
        Invalid("pair shape or format is invalid");
    }
    if (pair.level_ == 0 || pair.level_ >= fullModuli_.size()) {
        Invalid("pair level is outside the supported context basis");
    }

    std::vector<lbcrypto::NativeInteger> expectedModuli(fullModuli_.begin(), fullModuli_.end() - pair.level_);
    if (!SameOrderedModuli(pair.orderedModuli_, expectedModuli)) {
        Invalid("pair ordered RNS basis does not match its level");
    }
    if (!std::isfinite(pair.recordedScalingFactor_) || pair.recordedScalingFactor_ <= 0.0 ||
        !std::isfinite(pair.paperScale_) || pair.paperScale_ <= 0.0L) {
        Invalid("pair scale metadata is invalid");
    }
    if (pair.keyTag_.empty()) {
        Invalid("pair key tag is empty");
    }

    ValidateCiphertext(pair.high_, pair.orderedModuli_, pair.level_, pair.noiseScaleDegree_,
                       pair.recordedScalingFactor_, pair.keyTag_, "pair high");
    ValidateCiphertext(pair.low_, pair.orderedModuli_, pair.level_, pair.noiseScaleDegree_,
                       pair.recordedScalingFactor_, pair.keyTag_, "pair low");
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
