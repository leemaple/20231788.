#ifndef OPENFHE_2023_1788_PAPER_H128_CLIENT_KEYPAIR_H
#define OPENFHE_2023_1788_PAPER_H128_CLIENT_KEYPAIR_H

#include "openfhe.h"

namespace openfhe_2023_1788 {

// Client setup only: context must already be finalized, with CKKS-RNS full Q,
// STANDARD/FIXEDMANUAL/HYBRID, PRE NOT_SET, noiseScale=1, SPARSE_TERNARY,
// positive finite sigma, PKE/KEYSWITCH/LEVELEDSHE, and matching Q/P/QP partitions.
// Returns fresh full-Q EVALUATION keys; never changes the context or eval caches.
// Unsupported profiles: std::invalid_argument("fixed-Q h128: <category>").
// Unexpected official failures propagate; invalid generated output is a logic_error.
// As with OpenFHE's global key caches, callers must serialize concurrent mutation.
// This is not a basis-family, security, precision, or paper-completion guarantee.
lbcrypto::KeyPair<lbcrypto::DCRTPoly> CreateFixedQH128ClientKeyPair(
    const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context);

}  // namespace openfhe_2023_1788

#endif
