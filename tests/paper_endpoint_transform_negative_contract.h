#ifndef PAPER_ENDPOINT_TRANSFORM_NEGATIVE_CONTRACT_H
#define PAPER_ENDPOINT_TRANSFORM_NEGATIVE_CONTRACT_H

#include "paper_endpoint_failure.h"
#include "paper_endpoint_observer_contract.h"

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Test-local negative cases for the two transform seams. This header is kept
// separate from the frozen RED fixtures and is intentionally not wired here.
// The boundary cases below were authored before implementation and are NOT RUN.
namespace paper_endpoint_contract::synthetic {
namespace transform_negative {

inline void RequireRejected(const std::function<void()>& operation,
                            const std::string& label) {
    bool rejected = false;
    try {
        operation();
    }
    catch (const std::invalid_argument&) {
        rejected = true;
    }
    paper_full_test::Require(rejected, label);
}

inline void RequireEndpointFailure(const std::function<void()>& operation,
                                   const std::string& reason,
                                   const std::string& label) {
    bool rejected = false;
    try {
        operation();
    }
    catch (const EndpointFailure& failure) {
        paper_full_test::Require(failure.Reason() == reason,
                                 label + " returned the wrong endpoint reason");
        rejected = true;
    }
    paper_full_test::Require(rejected, label);
}

inline void Run() {
    const Scale unit{Int(1), Int(1)};
    const auto polynomial = [](std::size_t size, Int modulus) {
        return IntegerPolynomial{std::vector<Int>(size, Int(0)), std::move(modulus)};
    };

    RequireRejected([&] { Observe(polynomial(paper_full_test::kN - 1, Int(17)), unit); },
                    "observer rejects short polynomial");
    RequireRejected([&] { Observe(polynomial(paper_full_test::kN + 1, Int(17)), unit); },
                    "observer rejects long polynomial");
    for (const Int& modulus : {Int(0), Int(-17), Int(16)}) {
        RequireRejected([&] { Observe(polynomial(paper_full_test::kN, modulus), unit); },
                        "observer rejects nonpositive/even modulus");
    }
    for (const Int& coefficient : {Int(9), Int(-9)}) {
        auto outside = polynomial(paper_full_test::kN, Int(17));
        outside.coefficients[7] = coefficient;
        RequireRejected([&] { Observe(outside, unit); },
                        "observer rejects noncentered coefficient");
    }
    for (const Scale& scale : {Scale{Int(0), Int(1)}, Scale{Int(1), Int(0)},
                              Scale{Int(1), Int(-1)}, Scale{Int(2), Int(2)}}) {
        RequireRejected([&] { Observe(polynomial(paper_full_test::kN, Int(17)), scale); },
                        "observer rejects invalid scale");
    }

    RequireRejected([&] {
        DirectSparseReference768({SparseTerm{paper_full_test::kN, Int(1)}}, unit);
    }, "direct reference rejects out-of-range degree");
    RequireRejected([&] {
        DirectSparseReference768({SparseTerm{2, Int(1)}, SparseTerm{1, Int(1)}}, unit);
    }, "direct reference rejects unsorted degrees");
    RequireRejected([&] {
        DirectSparseReference768({SparseTerm{1, Int(1)}, SparseTerm{1, Int(-1)}}, unit);
    }, "direct reference rejects duplicate degrees");
    RequireRejected([&] {
        DirectSparseReference768({SparseTerm{0, Int(1)}}, Scale{Int(2), Int(2)});
    }, "direct reference rejects unreduced scale");

    auto underflowingTwist = polynomial(paper_full_test::kN, Int(3));
    underflowingTwist.coefficients[1] = Int(1);
    RequireEndpointFailure([&] {
        Observe(underflowingTwist, Scale{Int(1) << 399999, Int(1)});
    }, "MODEL_UNSUPPORTED", "observer rejects arithmetic below supported exponent envelope");

    RequireEndpointFailure([&] {
        DirectSparseReference768(
            {SparseTerm{0, Int(1)}}, Scale{Int(1), Int(1) << 400000});
    }, "MODEL_UNSUPPORTED", "direct reference rejects conversion outside exponent envelope");
}

}  // namespace transform_negative
}  // namespace paper_endpoint_contract::synthetic

#endif
