#ifndef OPENFHE_2023_1788_PUBLIC_S100_ENCODING_PROBE_H
#define OPENFHE_2023_1788_PUBLIC_S100_ENCODING_PROBE_H
#include "openfhe_2023_1788/high_precision_client_io.h"
namespace openfhe_2023_1788::client_io::diagnostic {
// Diagnostic-only, no context/key creation. This calls the existing
// ComputeEncoding with the frozen S100 public geometry/basis/scale.
// It does NOT create a validated client, ciphertext or repeated-plan receipt.
// The standalone driver supplies the unchanged original public input formula.
EncodingInspection InspectFixedS100PublicEncoding(const std::vector<ClientComplex>& values);
}
#endif
