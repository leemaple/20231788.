#ifndef OPENFHE_2023_1788_HIGH_PRECISION_CLIENT_IO_H
#define OPENFHE_2023_1788_HIGH_PRECISION_CLIENT_IO_H

#include "openfhe.h"

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace openfhe_2023_1788::client_io {

using ClientReal = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100>>;
using ExactInteger = boost::multiprecision::cpp_int;

struct ClientComplex final {
    ClientReal real;
    ClientReal imag;
};

class PositiveRationalScale final {
public:
    static PositiveRationalScale FromPositive(ExactInteger numerator, ExactInteger denominator);
    const ExactInteger& Numerator() const noexcept;
    const ExactInteger& Denominator() const noexcept;

private:
    PositiveRationalScale(ExactInteger numerator, ExactInteger denominator);
    ExactInteger numerator_;
    ExactInteger denominator_;
};

enum class CanonicalProjection { OpenFhePackedStride };
enum class ClientCiphertextOrigin { FreshClientEncoding, FirstMult2Rcb };

struct OrderedDcrtBasis final {
    std::uint32_t cyclotomicOrder;
    std::uint32_t ringDimension;
    std::vector<std::string> moduliDecimal;
    std::vector<std::string> rootsOfUnityDecimal;
};

struct ClientContextProfile final {
    const void* contextIdentity;
    const void* cryptoParamsIdentity;
    std::uint32_t requiredFeatureMask;
    std::uint32_t enabledFeatureMaskObserved;
    lbcrypto::ScalingTechnique scalingTechnique;
    lbcrypto::KeySwitchTechnique keySwitchTechnique;
    lbcrypto::ExecutionMode executionMode;
    lbcrypto::DecryptionNoiseMode decryptionNoiseMode;
    lbcrypto::CKKSDataType ckksDataType;
};

struct FreshEncodingSpec final {
    std::uint32_t slots;
    PositiveRationalScale logicalScale;
};

struct FirstMult2ScaleFactors final {
    ExactInteger qDiv;
    ExactInteger qL;
};

struct ClientCiphertextState final {
    ClientContextProfile contextProfile;
    std::string keyTag;
    OrderedDcrtBasis activeBasis;
    lbcrypto::PlaintextEncodings encodingType;
    Format componentFormat;
    std::uint32_t slots;
    std::uint32_t strideGap;
    std::uint32_t level;
    std::size_t componentCount;
    bool metadataMapEmpty;
    std::size_t noiseScaleDegree;
    double recordedScalingFactor;  // Compatibility only; never slot normalization.
    lbcrypto::NativeInteger scalingFactorInt;
    PositiveRationalScale logicalScale;
    CanonicalProjection projection;
    ClientCiphertextOrigin origin;
    std::optional<FirstMult2ScaleFactors> firstMult2ScaleFactors;
};

class HighPrecisionClientIO;

namespace detail {
struct ClientContextBinding;
}

class BoundCiphertext final {
public:
    // Separate coefficients, scalar state and empty metadata map. OpenFHE
    // Params/context remain shared: callers must not mutate those objects.
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> CloneForEvaluation() const;
    const ClientCiphertextState& State() const noexcept;

private:
    friend class HighPrecisionClientIO;
    BoundCiphertext(lbcrypto::Ciphertext<lbcrypto::DCRTPoly> snapshot, ClientCiphertextState state,
                    std::shared_ptr<const detail::ClientContextBinding> binding);
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> snapshot_;
    ClientCiphertextState state_;
    std::shared_ptr<const detail::ClientContextBinding> binding_;
};

struct DecodeDiagnostics final {
    ExactInteger activeCompositeModulus;
    ExactInteger maximumCenteredAbsoluteCoefficient;
    ExactInteger centeredHeadroom;
    ClientReal maximumCrossPrecisionDisagreement;
};

struct DecodedSlots final {
    std::vector<ClientComplex> values;
    ClientCiphertextState state;
    DecodeDiagnostics diagnostics;
};

// Bounded N64/S16 first-operation client boundary. It never enables features,
// creates keys, evaluates ciphertexts, or exposes an OpenFHE Plaintext.
class HighPrecisionClientIO final {
public:
    explicit HighPrecisionClientIO(lbcrypto::CryptoContext<lbcrypto::DCRTPoly> context);
    BoundCiphertext Encrypt(const lbcrypto::PublicKey<lbcrypto::DCRTPoly>& publicKey,
                            const std::vector<ClientComplex>& values,
                            const FreshEncodingSpec& spec) const;
    // Immediate first DCP->Mult2->RCB adoption, not cryptographic lineage proof
    // or a repeated-family receipt. A future evaluator receipt owns that seam.
    BoundCiphertext BindFirstMult2Rcb(const lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly>& recombined,
                                     const BoundCiphertext& leftFresh,
                                     const BoundCiphertext& rightFresh) const;
    DecodedSlots Decrypt(const lbcrypto::PrivateKey<lbcrypto::DCRTPoly>& privateKey,
                        const BoundCiphertext& input) const;

private:
    struct Impl;
    std::shared_ptr<const Impl> impl_;
};

}  // namespace openfhe_2023_1788::client_io

#endif  // OPENFHE_2023_1788_HIGH_PRECISION_CLIENT_IO_H
