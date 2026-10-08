#ifndef OPENFHE_2023_1788_MULT2_SCHEDULE_H
#define OPENFHE_2023_1788_MULT2_SCHEDULE_H

#include "openfhe_2023_1788/double_ckks.h"

namespace openfhe_2023_1788::detail {

// Internal arithmetic seam, not a validated ciphertext/pair constructor.
Mult2Fallback Mult2ScheduleSupport(
    const std::shared_ptr<lbcrypto::ILDCRTParams<lbcrypto::BigInteger>>& basis,
    Mult2Backend backend);

// Caller has validated A in R*q*d, B in R*q, arity two and EVALUATION.
// Support must have succeeded for this exact ordered basis. No input mutation.
std::pair<std::vector<lbcrypto::DCRTPoly>, std::vector<lbcrypto::DCRTPoly>>
ComputeMult2Schedule(const std::vector<lbcrypto::DCRTPoly>& high,
                     const std::vector<lbcrypto::DCRTPoly>& low,
                     Mult2Backend backend);

}  // namespace openfhe_2023_1788::detail
#endif
