#ifndef OPENFHE_2023_1788_DOUBLE_CKKS_H
#define OPENFHE_2023_1788_DOUBLE_CKKS_H

#include "openfhe.h"
#include "scheme/ckksrns/ckksrns-cryptoparameters.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace openfhe_2023_1788 {

using ReadOnlyCiphertext = std::shared_ptr<const lbcrypto::CiphertextImpl<lbcrypto::DCRTPoly>>;

enum class PairLifecycle : std::uint8_t {
    ReadyForFirstMult,
    ReadyForRS2,
    RefreshRequired,
    ReadyForRepeatedMult,
};

struct PaperScaleDescriptor final {
    double inputRecordedScalingFactor;
    lbcrypto::NativeInteger divisor;
    long double approximateLogicalScalingFactor;
    long double approximateRecombinedLogicalScalingFactor;
};

struct TensorScaleDescriptor final {
    long double approximateHighLogicalScalingFactor;
    long double approximateRecombinedLogicalScalingFactor;
};

class DoubleCKKS;
class RepeatedMult2Plan;
class RepeatedMult2Receipt;

class CiphertextPair final {
public:
    ReadOnlyCiphertext GetHigh() const noexcept;
    ReadOnlyCiphertext GetLow() const noexcept;

    const lbcrypto::CryptoContextImpl<lbcrypto::DCRTPoly>* GetContextIdentity() const noexcept;
    const lbcrypto::NativeInteger& GetDivisor() const noexcept;
    const std::vector<lbcrypto::NativeInteger>& GetOrderedModuli() const noexcept;
    std::size_t GetLevel() const noexcept;
    const PaperScaleDescriptor& GetPaperScale() const noexcept;
    double GetRecordedScalingFactor() const noexcept;
    std::size_t GetNoiseScaleDegree() const noexcept;
    PairLifecycle GetLifecycle() const noexcept;
    const std::string& GetKeyTag() const noexcept;
    std::uint32_t GetSlots() const noexcept;
    Format GetFormat() const noexcept;
    std::size_t GetComponentCount() const noexcept;
    const std::shared_ptr<const RepeatedMult2Receipt>& GetRepeatedReceipt() const noexcept { return receipt_; }

private:
    friend class DoubleCKKS;

    CiphertextPair(lbcrypto::Ciphertext<lbcrypto::DCRTPoly> high,
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
                   std::size_t componentCount);

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> high_;
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> low_;
    const lbcrypto::CryptoContextImpl<lbcrypto::DCRTPoly>* contextIdentity_;
    lbcrypto::NativeInteger divisor_;
    std::vector<lbcrypto::NativeInteger> orderedModuli_;
    std::size_t level_;
    PaperScaleDescriptor paperScale_;
    double recordedScalingFactor_;
    std::size_t noiseScaleDegree_;
    PairLifecycle lifecycle_;
    std::string keyTag_;
    std::uint32_t slots_;
    Format format_;
    std::size_t componentCount_;
    std::shared_ptr<const RepeatedMult2Receipt> receipt_;
};

class TensorCiphertextPair final {
public:
    ReadOnlyCiphertext GetHigh() const noexcept;
    ReadOnlyCiphertext GetLow() const noexcept;

    const lbcrypto::CryptoContextImpl<lbcrypto::DCRTPoly>* GetContextIdentity() const noexcept;
    const lbcrypto::NativeInteger& GetDivisor() const noexcept;
    const std::vector<lbcrypto::NativeInteger>& GetOrderedModuli() const noexcept;
    std::size_t GetLevel() const noexcept;
    const TensorScaleDescriptor& GetTensorScale() const noexcept;
    double GetRecordedScalingFactor() const noexcept;
    std::size_t GetNoiseScaleDegree() const noexcept;
    const std::string& GetKeyTag() const noexcept;
    std::uint32_t GetSlots() const noexcept;
    Format GetFormat() const noexcept;
    std::size_t GetComponentCount() const noexcept;
    const std::shared_ptr<const RepeatedMult2Receipt>& GetRepeatedReceipt() const noexcept { return receipt_; }

private:
    friend class DoubleCKKS;

    TensorCiphertextPair(lbcrypto::Ciphertext<lbcrypto::DCRTPoly> high,
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
                         std::size_t componentCount);

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> high_;
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> low_;
    const lbcrypto::CryptoContextImpl<lbcrypto::DCRTPoly>* contextIdentity_;
    lbcrypto::NativeInteger divisor_;
    std::vector<lbcrypto::NativeInteger> orderedModuli_;
    std::size_t level_;
    TensorScaleDescriptor tensorScale_;
    double recordedScalingFactor_;
    std::size_t noiseScaleDegree_;
    std::string keyTag_;
    std::uint32_t slots_;
    Format format_;
    std::size_t componentCount_;
    std::shared_ptr<const RepeatedMult2Receipt> receipt_;
};

class DoubleCKKS final {
public:
    explicit DoubleCKKS(lbcrypto::CryptoContext<lbcrypto::DCRTPoly> context);
    explicit DoubleCKKS(std::shared_ptr<const RepeatedMult2Plan> plan);
    // Keep the original null-context diagnostic unambiguous with the overload.
    explicit DoubleCKKS(std::nullptr_t) : DoubleCKKS(lbcrypto::CryptoContext<lbcrypto::DCRTPoly>{}) {}

    CiphertextPair DCP(const ReadOnlyCiphertext& ciphertext) const;
    CiphertextPair Add(const CiphertextPair& left, const CiphertextPair& right) const;
    CiphertextPair Sub(const CiphertextPair& left, const CiphertextPair& right) const;
    TensorCiphertextPair Tensor2(const CiphertextPair& left, const CiphertextPair& right) const;
    CiphertextPair Relin2(const TensorCiphertextPair& tensor) const;
    CiphertextPair RS2(const CiphertextPair& relinearized) const;
    CiphertextPair Mult2(const CiphertextPair& left, const CiphertextPair& right) const;
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> RCB(const CiphertextPair& pair) const;

private:
    DoubleCKKS(std::shared_ptr<const RepeatedMult2Plan> plan,std::size_t family);
    void ValidatePlannedPair(const CiphertextPair& pair) const;
    void ValidatePlannedTensor(const TensorCiphertextPair& pair) const;
    CiphertextPair Reenter(const CiphertextPair& pair) const;
    void AttachReceipt(CiphertextPair& pair,const std::shared_ptr<const RepeatedMult2Receipt>& receipt) const;
    void AttachReceipt(TensorCiphertextPair& pair,const std::shared_ptr<const RepeatedMult2Receipt>& receipt) const;
    void ValidateDcpInput(const ReadOnlyCiphertext& ciphertext) const;
    void ValidatePair(const CiphertextPair& pair) const;
    void ValidatePairCompatibility(const CiphertextPair& left, const CiphertextPair& right,
                                   const char* operationName) const;
    void ValidateTensorCompatibility(const CiphertextPair& left, const CiphertextPair& right) const;
    void ValidateTensorResult(const TensorCiphertextPair& pair) const;
    std::pair<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>, lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>
    DecomposeValidatedCiphertext(const ReadOnlyCiphertext& ciphertext) const;
    void ValidateCiphertext(const ReadOnlyCiphertext& ciphertext,
                            const std::vector<lbcrypto::NativeInteger>& orderedModuli,
                            std::size_t level,
                            std::size_t noiseScaleDegree,
                            double recordedScalingFactor,
                            const std::string& keyTag,
                            std::uint32_t slots,
                            std::size_t componentCount,
                            const char* stateLabel,
                            const char* label) const;

    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> context_;
    std::shared_ptr<lbcrypto::CryptoParametersCKKSRNS> parameters_;
    std::vector<lbcrypto::NativeInteger> fullModuli_;
    std::vector<lbcrypto::NativeInteger> firstPairModuli_;
    lbcrypto::NativeInteger divisor_;
    double expectedInputScalingFactor_;
    std::shared_ptr<const RepeatedMult2Plan> plan_;
    std::size_t familyIndex_=0;
};

}  // namespace openfhe_2023_1788

#endif  // OPENFHE_2023_1788_DOUBLE_CKKS_H
