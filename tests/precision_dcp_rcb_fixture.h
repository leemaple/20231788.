#ifndef OPENFHE_2023_1788_PRECISION_DCP_RCB_FIXTURE_H
#define OPENFHE_2023_1788_PRECISION_DCP_RCB_FIXTURE_H

#include "openfhe.h"

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <cstdint>
#include <vector>

namespace precision_dcp_rcb_test {

using BigFloat = boost::multiprecision::cpp_dec_float_100;

struct MpComplex final {
    BigFloat real;
    BigFloat imag;
};

// Test-owned fixture seam.  The red implementation is deliberately incomplete:
// it converts the lossless values to binary64 before standard CKKS encoding.
// The green implementation replaces only this function body with a fresh
// 2^scaleBits public-DCRT construction.  Neither implementation is a shipping
// codec or a production API.
lbcrypto::Plaintext MakePrecisionPlaintext(
    const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
    const std::vector<MpComplex>& inputValues,
    std::uint32_t scaleBits);

}  // namespace precision_dcp_rcb_test

#endif  // OPENFHE_2023_1788_PRECISION_DCP_RCB_FIXTURE_H
