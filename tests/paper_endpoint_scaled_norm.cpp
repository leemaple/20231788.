#include "paper_endpoint_observer_contract.h"

#include <limits>
#include <stdexcept>

namespace paper_endpoint_contract {
std::optional<int> ScaledOneNormExponent(const Int& coefficientOneNorm, const Scale& scale) {
    if (coefficientOneNorm < 0 || scale.numerator <= 0 || scale.denominator <= 0)
        throw std::invalid_argument("endpoint norm requires nonnegative C and positive scale");
    if (paper_full_test::Gcd(scale.numerator, scale.denominator) != 1)
        throw std::invalid_argument("endpoint norm requires reduced scale");
    if (coefficientOneNorm == 0)
        return std::nullopt;

    const Int numerator = coefficientOneNorm * scale.denominator;
    const auto numeratorBit = boost::multiprecision::msb(numerator);
    const auto denominatorBit = boost::multiprecision::msb(scale.numerator);
    // Their highest-set-bit difference is either ceil(log2(C/S)) or one less.
    // Keep the signed difference exact until after its checked conversion.
    Int exponent = Int(numeratorBit) - denominatorBit;
    if (numeratorBit >= denominatorBit) {
        if (numerator > (scale.numerator << (numeratorBit - denominatorBit)))
            ++exponent;
    } else {
        if ((numerator << (denominatorBit - numeratorBit)) > scale.numerator)
            ++exponent;
    }
    if (exponent < std::numeric_limits<int>::min() ||
        exponent > std::numeric_limits<int>::max())
        throw std::overflow_error("endpoint norm exponent exceeds the public int range");
    return exponent.convert_to<int>();
}
} // namespace paper_endpoint_contract
