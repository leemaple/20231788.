#ifndef OPENFHE_2023_1788_REPEATED_MULT2_H
#define OPENFHE_2023_1788_REPEATED_MULT2_H

#include "openfhe_2023_1788/double_ckks.h"
#include <boost/multiprecision/cpp_int.hpp>

#ifdef DEBUG_KEY
#error "Repeated Mult2 requires OpenFHE without DEBUG_KEY private-key context storage"
#endif

namespace openfhe_2023_1788 {

class ExactScale final {
public:
    using Integer=boost::multiprecision::cpp_int;
    ExactScale(Integer numerator,Integer denominator);
    const Integer& GetNumerator() const noexcept { return numerator_; }
    const Integer& GetDenominator() const noexcept { return denominator_; }
private:
    Integer numerator_,denominator_;
};

enum class RepeatedPhase : std::uint8_t { Input,Tensor,Relinearized,Rescaled,Reentry };

// Normalization/state authority, not a claim of ciphertext lineage or an error
// bound. Only the owning plan can issue a receipt; pointer membership is checked.
class RepeatedMult2Receipt final {
public:
    std::size_t GetFamilyIndex() const noexcept { return family_; }
    std::size_t GetOperationIndex() const noexcept { return operation_; }
    RepeatedPhase GetPhase() const noexcept { return phase_; }
    const ExactScale& GetExactScale() const noexcept { return scale_; }
    const std::shared_ptr<const RepeatedMult2Receipt>& GetParent() const noexcept { return parent_; }
    bool IsTerminal() const noexcept { return terminal_; }
private:
    friend class RepeatedMult2Plan;
    friend class DoubleCKKS;
    RepeatedMult2Receipt(std::size_t family,std::size_t operation,RepeatedPhase phase,ExactScale scale,
                        std::shared_ptr<const RepeatedMult2Receipt> parent,bool terminal,
                        std::size_t level,std::size_t arity,std::size_t noise,PairLifecycle lifecycle,
                        double recorded,long double high,long double recombined);
    std::size_t family_,operation_;
    RepeatedPhase phase_;
    ExactScale scale_;
    std::shared_ptr<const RepeatedMult2Receipt> parent_;
    bool terminal_;
    std::size_t level_,arity_,noise_;
    PairLifecycle lifecycle_;
    double recorded_;
    long double high_,recombined_;
};

struct RepeatedMult2ClientSetup;
RepeatedMult2ClientSetup CreateRepeatedMult2DiagnosticSetup();

// No private-key member or private-key constructor parameter. OpenFHE handles
// expose mutable upstream types, so all actual returned family profiles and row
// ownership are sealed and revalidated before use. Do not mutate those handles.
class RepeatedMult2Plan final {
public:
    ~RepeatedMult2Plan();
    RepeatedMult2Plan(const RepeatedMult2Plan&)=delete;
    RepeatedMult2Plan& operator=(const RepeatedMult2Plan&)=delete;
    std::size_t GetFamilyCount() const noexcept;
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> GetFamilyContext(std::size_t family) const;
    const std::string& GetFamilyKeyTag(std::size_t family) const;
    const lbcrypto::NativeInteger& GetDivisor() const noexcept;
private:
    friend class DoubleCKKS;
    friend RepeatedMult2ClientSetup CreateRepeatedMult2DiagnosticSetup();
    struct Data;
    explicit RepeatedMult2Plan(std::unique_ptr<Data> data);
    void ValidateFamily(std::size_t family) const;
    std::size_t RequireReceipt(const std::shared_ptr<const RepeatedMult2Receipt>& receipt) const;
    std::shared_ptr<const RepeatedMult2Receipt> ReceiptFor(std::size_t family,RepeatedPhase phase) const;
    std::unique_ptr<Data> data_;
};

// Client-owned result. Only `plan` is passed to DoubleCKKS. The root secret is
// retained separately by the client; projected setup secrets are not returned.
struct RepeatedMult2ClientSetup final {
    std::shared_ptr<const RepeatedMult2Plan> plan;
    lbcrypto::PublicKey<lbcrypto::DCRTPoly> publicKey;
    lbcrypto::PrivateKey<lbcrypto::DCRTPoly> rootSecret;
};

} // namespace openfhe_2023_1788
#endif
