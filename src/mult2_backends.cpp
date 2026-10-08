#include "openfhe_2023_1788/double_ckks.h"
#include "openfhe_2023_1788/repeated_mult2.h"
#include "mult2_schedule.h"

#include <cmath>
#include <stdexcept>

namespace openfhe_2023_1788 {
namespace {
void Require(bool condition, const char* message) {
    if (!condition) {
        throw std::invalid_argument(std::string("DoubleCKKS: ") + message);
    }
}
}  // namespace

void DoubleCKKS::ValidateMult2Stage(const TensorCiphertextPair& tensor) const {
    // This describes a virtual ReadyForRS2 stage. Tail routes do not claim to
    // have created or validated an intermediate CiphertextPair.
    Require(tensor.contextIdentity_ == context_.get() && tensor.divisor_ == divisor_ &&
                tensor.orderedModuli_ == firstPairModuli_ && tensor.level_ == 1 &&
                tensor.noiseScaleDegree_ == 3 && tensor.componentCount_ == 3 &&
                tensor.format_ == Format::EVALUATION,
            "Mult2 virtual stage basis/shape mismatch");
    if (plan_) {
        const auto receipt = plan_->ReceiptFor(familyIndex_, RepeatedPhase::Relinearized);
        const auto family = plan_->RequireReceipt(receipt);
        Require(family == familyIndex_ && receipt->phase_ == RepeatedPhase::Relinearized &&
                    receipt->parent_ == tensor.receipt_ && receipt->arity_ == 2 &&
                    receipt->level_ == tensor.level_ && receipt->noise_ == tensor.noiseScaleDegree_ &&
                    receipt->lifecycle_ == PairLifecycle::ReadyForRS2 &&
                    receipt->recorded_ == tensor.recordedScalingFactor_ &&
                    receipt->high_ == tensor.tensorScale_.approximateHighLogicalScalingFactor &&
                    receipt->recombined_ == tensor.tensorScale_.approximateRecombinedLogicalScalingFactor,
                "Mult2 virtual stage receipt/state mismatch");
    }
    else {
        const double recorded = expectedInputScalingFactor_ * expectedInputScalingFactor_ /
                                parameters_->GetScalingFactorReal(0);
        const long double d = static_cast<long double>(divisor_.ConvertToInt());
        const long double h = static_cast<long double>(expectedInputScalingFactor_) / d;
        const long double combined = static_cast<long double>(expectedInputScalingFactor_) *
                                     static_cast<long double>(expectedInputScalingFactor_) / d;
        Require(!tensor.receipt_ && tensor.recordedScalingFactor_ == recorded &&
                    std::isfinite(recorded) &&
                    tensor.tensorScale_.approximateHighLogicalScalingFactor == h * h &&
                    tensor.tensorScale_.approximateRecombinedLogicalScalingFactor == combined,
                "Mult2 virtual stage scale mismatch");
    }
}

CiphertextPair DoubleCKKS::FinishMult2FromRelin(const TensorCiphertextPair& tensor,
                                              const ReadOnlyCiphertext& high,
                                              const ReadOnlyCiphertext& low,
                                              Mult2Backend backend) const {
    // PrepareRelin2 has already validated the actual A. Validate actual B here;
    // the unchanged public Relin2 validates B after its DCP operations.
    ValidateCiphertext(low, tensor.orderedModuli_, tensor.level_, tensor.noiseScaleDegree_,
                       tensor.recordedScalingFactor_, tensor.keyTag_, tensor.slots_, 2,
                       "Relin2 relinearized-low", "Relin2 relinearized low");
    ValidateMult2Stage(tensor);

    const auto q = tensor.orderedModuli_.back();
    Require(q.Mod(lbcrypto::NativeInteger(2)) == lbcrypto::NativeInteger(1),
            "the last active Q tower used as q_l must be odd");
    Require(q != divisor_, "q_l must remain distinct from q_div");
    const auto index = tensor.orderedModuli_.size() - 1;
    const auto& fullTowers = parameters_->GetElementParams()->GetParams();
    Require(index < fullTowers.size() && fullTowers[index] &&
                fullTowers[index]->GetModulus() == q,
            "q_l does not match the OpenFHE mod-reduce factor index");
    Require(!fullTowers.empty() && fullTowers.back() &&
                fullTowers.back()->GetModulus() == divisor_,
            "q_div does not match the final full-basis tower");
    const double recordedDivisor = parameters_->GetModReduceFactor(index);
    Require(std::isfinite(recordedDivisor) && recordedDivisor > 0.0,
            "the OpenFHE mod-reduce factor is invalid");
    const double outputRecorded = tensor.recordedScalingFactor_ / recordedDivisor;
    Require(std::isfinite(outputRecorded) && outputRecorded > 0.0,
            "the RS2 output recorded scaling factor is invalid");
    const auto qLong = static_cast<long double>(q.ConvertToInt());
    const auto outputHigh = tensor.tensorScale_.approximateHighLogicalScalingFactor / qLong;
    const auto outputRecombined = tensor.tensorScale_.approximateRecombinedLogicalScalingFactor / qLong;
    Require(std::isfinite(outputHigh) && std::isfinite(outputRecombined),
            "the RS2 output logical scaling factors are invalid");

    auto elements = detail::ComputeMult2Schedule(high->GetElements(), low->GetElements(), backend);
    auto resultHigh = high->CloneEmpty();
    auto resultLow = high->CloneEmpty();
    resultHigh->SetElements(std::move(elements.first));
    resultLow->SetElements(std::move(elements.second));
    const auto level = tensor.level_ + 1;
    const auto noise = tensor.noiseScaleDegree_ - 1;
    for (auto& output : {resultHigh, resultLow}) {
        output->SetNoiseScaleDeg(noise);
        output->SetLevel(level);
        output->SetScalingFactor(outputRecorded);
    }
    std::vector<lbcrypto::NativeInteger> moduli(tensor.orderedModuli_.begin(),
                                               tensor.orderedModuli_.end() - 1);
    ValidateCiphertext(resultHigh, moduli, level, noise, outputRecorded, tensor.keyTag_,
                       tensor.slots_, 2, "RS2 output", "RS2 rescaled high");
    ValidateCiphertext(resultLow, moduli, level, noise, outputRecorded, tensor.keyTag_,
                       tensor.slots_, 2, "RS2 output", "RS2 new low");
    PaperScaleDescriptor scale{outputRecorded, divisor_, outputHigh, outputRecombined};
    CiphertextPair result(std::move(resultHigh), std::move(resultLow), context_.get(), divisor_,
                          std::move(moduli), level, scale, outputRecorded, noise,
                          PairLifecycle::RefreshRequired, tensor.keyTag_, tensor.slots_,
                          Format::EVALUATION, 2);
    if (plan_) {
        AttachReceipt(result, plan_->ReceiptFor(familyIndex_, RepeatedPhase::Rescaled));
    }
    ValidatePair(result);
    return result;
}

CiphertextPair DoubleCKKS::Mult2WithBackend(const CiphertextPair& left,
                                           const CiphertextPair& right,
                                           Mult2Backend backend) const {
    return Mult2WithBackend(left, right, backend, nullptr);
}

CiphertextPair DoubleCKKS::Mult2WithBackend(const CiphertextPair& left,
                                           const CiphertextPair& right,
                                           Mult2Backend backend,
                                           Mult2BackendTrace* trace) const {
    if (trace) {
        *trace = {backend, Mult2Backend::Reference, Mult2Fallback::None, false};
    }
    Require(backend == Mult2Backend::Reference || backend == Mult2Backend::SharedStaged ||
                backend == Mult2Backend::TailDigit || backend == Mult2Backend::TailWide,
            "unknown Mult2 backend");
    if (backend == Mult2Backend::Reference) {
        auto result = Mult2(left, right);
        if (trace) {
            trace->completed = true;
        }
        return result;
    }
    if (plan_) {
        const auto family = plan_->RequireReceipt(left.receipt_);
        if (family != familyIndex_) {
            return DoubleCKKS(plan_, family).Mult2WithBackend(left, right, backend, trace);
        }
    }
    const auto tensor = Tensor2(left, right);
    const auto fallback = detail::Mult2ScheduleSupport(parameters_->GetElementParams(), backend);
    auto result = [&]() {
        if (fallback != Mult2Fallback::None) {
            if (trace) {
                trace->fallback = fallback;
            }
            return RS2(Relin2(tensor));
        }
        const auto relin = PrepareRelin2(tensor);
        if (trace) {
            trace->executed = backend;
        }
        return FinishMult2FromRelin(tensor, relin.first, relin.second, backend);
    }();
    if (plan_ && !result.receipt_->IsTerminal()) {
        result = Reenter(result);
    }
    if (trace) {
        trace->completed = true;
    }
    return result;
}
}  // namespace openfhe_2023_1788
