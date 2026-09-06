#ifndef PAPER_ENDPOINT_EXACT_SCALARS_INTERNAL_H
#define PAPER_ENDPOINT_EXACT_SCALARS_INTERNAL_H

#include "paper_endpoint_observer_contract.h"

namespace paper_endpoint_contract::internal {

// Exact signed value represented by a finite Binary768. The result is reduced,
// its denominator is positive, and zero is returned only as 0/1. Nonfinite
// input is rejected with std::invalid_argument by the definition.
Rational RepresentedDyadic(const Binary768& value);

} // namespace paper_endpoint_contract::internal

#endif
