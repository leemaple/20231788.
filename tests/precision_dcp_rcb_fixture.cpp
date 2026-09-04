#include "precision_dcp_rcb_fixture.h"

#include <complex>
#include <vector>

namespace precision_dcp_rcb_test {

lbcrypto::Plaintext MakePrecisionPlaintext(
    const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
    const std::vector<MpComplex>& inputValues,
    std::uint32_t scaleBits) {
    (void)scaleBits;
    std::vector<std::complex<double>> binary64Values;
    binary64Values.reserve(inputValues.size());
    for (const auto& value : inputValues) {
        binary64Values.emplace_back(value.real.convert_to<double>(),
                                    value.imag.convert_to<double>());
    }

    // Intentionally incomplete RED fixture.  It discards the selected sub-ULP
    // source information before Encrypt, DCP, or RCB is called.  A future red
    // therefore diagnoses this fixture only; it is not evidence of an upstream
    // encoder, DCP, or RCB defect.
    return context->MakeCKKSPackedPlaintext(binary64Values, 2, 0);
}

}  // namespace precision_dcp_rcb_test
