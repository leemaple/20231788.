#ifndef PAPER_ENDPOINT_SCALED_NORM_TEST_H
#define PAPER_ENDPOINT_SCALED_NORM_TEST_H

#include "paper_endpoint_observer_contract.h"
#include <stdexcept>

namespace paper_endpoint_contract::synthetic {
inline void RunScaledNormBoundaryTests() {
    using paper_full_test::Require;
    // 1/(3/2)=2/3; 2/(3/2)=4/3; 3/(3/2)=2 exactly.
    const Scale fractional{Int(3), Int(2)};
    Require(ScaledOneNormExponent(Int(1), fractional) == std::optional<int>(0),
            "fractional scale below unit boundary");
    Require(ScaledOneNormExponent(Int(2), fractional) == std::optional<int>(1),
            "fractional scale above unit boundary");
    Require(ScaledOneNormExponent(Int(3), fractional) == std::optional<int>(1),
            "fractional scale exact power boundary");

    const Int boundary = Int(1) << 255;
    const Scale largeScale{boundary, Int(1)};
    Require(ScaledOneNormExponent(boundary - 1, largeScale) == std::optional<int>(0) &&
                ScaledOneNormExponent(boundary, largeScale) == std::optional<int>(0) &&
                ScaledOneNormExponent(boundary + 1, largeScale) == std::optional<int>(1),
            "exact large-integer power boundary");

    struct Invalid final { Int norm; Scale scale; };
    const Invalid invalid[] = {
        {Int(-1), {Int(1), Int(1)}},
        {Int(0), {Int(0), Int(1)}},
        {Int(0), {Int(-1), Int(1)}},
        {Int(0), {Int(1), Int(0)}},
        {Int(0), {Int(1), Int(-1)}},
        {Int(0), {Int(2), Int(4)}}
    };
    for (const auto& test : invalid) {
        bool rejected = false;
        try {
            (void)ScaledOneNormExponent(test.norm, test.scale);
        } catch (const std::invalid_argument&) {
            rejected = true; // Expected public input-rejection boundary only.
        }
        Require(rejected, "scaled norm validates inputs before zero sentinel");
    }
}
} // namespace paper_endpoint_contract::synthetic
#endif
