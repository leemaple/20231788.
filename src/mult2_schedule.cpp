#include "mult2_schedule.h"

#include <cstdint>
#include <stdexcept>

namespace openfhe_2023_1788::detail {
namespace {
using Integer = lbcrypto::NativeInteger;
using Poly = lbcrypto::NativePoly;
using DCRT = lbcrypto::DCRTPoly;
using Signed = std::int64_t;

// The extension is explicitly confined to platforms that advertise it. It is
// not required by the digit or staged implementations.
#if defined(__SIZEOF_INT128__) && !defined(OPENFHE_2023_1788_DISABLE_WIDE_MULT2)
__extension__ typedef __int128 Wide;
#endif

#if NATIVEINT == 64
bool IsPrime(const Integer& modulus) {
    const auto n = static_cast<std::uint64_t>(modulus.ConvertToInt());
    if (n < 3 || n % 2 == 0) {
        return false;
    }
    std::uint64_t odd = n - 1;
    unsigned int shifts = 0;
    while (odd % 2 == 0) {
        odd /= 2;
        ++shifts;
    }
    // Deterministic Miller-Rabin basis set for all unsigned 64-bit integers.
    for (const std::uint64_t base : {2ULL, 325ULL, 9375ULL, 28178ULL,
                                     450775ULL, 9780504ULL, 1795265022ULL}) {
        if (base % n == 0) {
            continue;
        }
        auto x = Integer(base % n).ModExp(Integer(odd), modulus);
        if (x == Integer(1) || x == Integer(n - 1)) {
            continue;
        }
        bool minusOne = false;
        for (unsigned int j = 1; j < shifts; ++j) {
            x = x.ModMul(x, modulus);
            if (x == Integer(n - 1)) {
                minusOne = true;
                break;
            }
        }
        if (!minusOne) {
            return false;
        }
    }
    return true;
}

#endif

Poly Canonical(const Poly& input) {
    auto result = input;
    result.SetValues(input.GetValues().Mod(input.GetModulus()), input.GetFormat());
    return result;
}

Poly Coefficients(const Poly& input) {
    auto result = Canonical(input);
    result.SetFormat(Format::COEFFICIENT);
    return Canonical(result);
}

void Forward(Poly& value) {
    value.SetFormat(Format::EVALUATION);
    value = Canonical(value);
}

Signed Center(const Integer& canonical, const Integer& modulus) {
    const auto x = static_cast<std::uint64_t>(canonical.ConvertToInt());
    const auto p = static_cast<std::uint64_t>(modulus.ConvertToInt());
    return x > p / 2 ? -static_cast<Signed>(p - x) : static_cast<Signed>(x);
}

Integer Project(Signed value, const Integer& modulus) {
    // |value| < 2^59, so negation is representable in signed 64 bits.
    const auto magnitude = static_cast<std::uint64_t>(value < 0 ? -value : value);
    auto residue = Integer(magnitude).Mod(modulus);
    return value < 0 && residue != Integer(0) ? modulus - residue : residue;
}

std::vector<Signed> Centered(const Poly& coefficients) {
    std::vector<Signed> result;
    result.reserve(coefficients.GetLength());
    for (std::size_t j = 0; j < coefficients.GetLength(); ++j) {
        result.push_back(Center(coefficients.GetValues()[j], coefficients.GetModulus()));
    }
    return result;
}

Poly Projection(const Poly& prototype, const std::vector<Signed>& coefficients) {
    Poly result(prototype.GetParams(), Format::COEFFICIENT, true);
    lbcrypto::NativeVector values(coefficients.size(), prototype.GetModulus());
    for (std::size_t j = 0; j < coefficients.size(); ++j) {
        values[j] = Project(coefficients[j], prototype.GetModulus());
    }
    result.SetValues(std::move(values), Format::COEFFICIENT);
    return result;
}

struct Prepared final {
    Integer d, q, dInvQ;
    std::vector<Integer> dInv, qInv, dMod, alpha;
    Prepared(const DCRT& high, Mult2Backend backend)
        : d(high.GetAllElements().back().GetModulus()),
          q(high.GetAllElements().at(high.GetNumOfElements() - 2).GetModulus()),
          dInvQ(d.ModInverse(q)) {
        const auto& towers = high.GetAllElements();
        for (std::size_t i = 0; i + 1 < towers.size(); ++i) {
            const auto& p = towers[i].GetModulus();
            dInv.push_back(d.ModInverse(p));
            dMod.push_back(d.Mod(p));
            if (i + 2 < towers.size()) {
                qInv.push_back(q.ModInverse(p));
                if (backend != Mult2Backend::SharedStaged) {
                    alpha.push_back(dInv.back().ModMul(qInv.back(), p));
                }
            }
        }
    }
};

std::pair<DCRT, DCRT> Staged(const DCRT& inputA, const DCRT& inputB,
                            const Prepared& prep) {
    const auto& aTowers = inputA.GetAllElements();
    const auto& bTowers = inputB.GetAllElements();
    const auto count = bTowers.size();
    const auto a = Centered(Coefficients(aTowers.back()));
    // A_q+B_q == d*h_q+l_q mod q: no extra coefficient modular multiply.
    auto sumQ = Canonical(aTowers[count - 1]);
    sumQ += Canonical(bTowers.back());
    const auto c = Centered(Coefficients(sumQ));
    std::vector<Poly> h, low;
    h.reserve(count);
    low.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        auto correction = Projection(aTowers[i], a);
        Forward(correction);
        auto hi = Canonical(aTowers[i]);
        hi -= correction;
        hi *= prep.dInv[i];
        auto li = Canonical(bTowers[i]);
        li += correction;
        h.push_back(std::move(hi));
        low.push_back(std::move(li));
    }
    // These are the complete intermediate h/l polynomials in R*q.
    const auto u = Centered(Coefficients(h.back()));
    std::vector<Poly> outH, outL;
    outH.reserve(count - 1);
    outL.reserve(count - 1);
    for (std::size_t i = 0; i + 1 < count; ++i) {
        auto uEval = Projection(h[i], u);
        auto cEval = Projection(h[i], c);
        Forward(uEval);
        Forward(cEval);
        h[i] -= uEval;
        h[i] *= prep.qInv[i];
        uEval *= prep.dMod[i];
        low[i] += uEval;
        low[i] -= cEval;
        low[i] *= prep.qInv[i];
        outH.push_back(std::move(h[i]));
        outL.push_back(std::move(low[i]));
    }
    return {DCRT(outH), DCRT(outL)};
}

std::pair<DCRT, DCRT> Tail(const DCRT& inputA, const DCRT& inputB,
                          const Prepared& prep, Mult2Backend backend) {
    const auto& aTowers = inputA.GetAllElements();
    const auto& bTowers = inputB.GetAllElements();
    const auto count = bTowers.size();
    const auto a = Centered(Coefficients(aTowers.back()));
    auto aq = Coefficients(aTowers[count - 1]);
    aq -= Projection(aq, a);
    aq *= prep.dInvQ;
    const auto u = Centered(aq);
    auto sumQ = Canonical(aTowers[count - 1]);
    sumQ += Canonical(bTowers.back());
    const auto c = Centered(Coefficients(sumQ));
#if defined(__SIZEOF_INT128__) && !defined(OPENFHE_2023_1788_DISABLE_WIDE_MULT2)
    std::vector<Wide> wide;
    if (backend == Mult2Backend::TailWide) {
        wide.reserve(a.size());
        const Wide d = static_cast<Wide>(prep.d.ConvertToInt());
        for (std::size_t j = 0; j < a.size(); ++j) {
            // Both operands promoted before multiplication: |r| < 2^119.
            wide.push_back(static_cast<Wide>(a[j]) + d * static_cast<Wide>(u[j]));
        }
    }
#else
    if (backend == Mult2Backend::TailWide) {
        throw std::logic_error("unsupported wide schedule reached internal kernel");
    }
#endif
    std::vector<Poly> outH, outL;
    outH.reserve(count - 1);
    outL.reserve(count - 1);
    for (std::size_t i = 0; i + 1 < count; ++i) {
        Poly r(aTowers[i].GetParams(), Format::COEFFICIENT, true);
        if (backend == Mult2Backend::TailDigit) {
            r = Projection(aTowers[i], a);
            auto digit = Projection(aTowers[i], u);
            digit *= prep.dMod[i];
            r += digit;
        }
#if defined(__SIZEOF_INT128__) && !defined(OPENFHE_2023_1788_DISABLE_WIDE_MULT2)
        else {
            const auto& p = aTowers[i].GetModulus();
            lbcrypto::NativeVector values(a.size(), p);
            const Wide modulus = static_cast<Wide>(p.ConvertToInt());
            for (std::size_t j = 0; j < a.size(); ++j) {
                const Wide magnitude = wide[j] < 0 ? -wide[j] : wide[j];
                const auto rem = static_cast<std::uint64_t>(magnitude % modulus);
                values[j] = wide[j] < 0 && rem != 0 ? p - Integer(rem) : Integer(rem);
            }
            r.SetValues(std::move(values), Format::COEFFICIENT);
        }
#endif
        auto e = r;
        e -= Projection(aTowers[i], c);
        Forward(r);
        Forward(e);
        auto hi = Canonical(aTowers[i]);
        auto li = Canonical(bTowers[i]);
        hi -= r;
        hi *= prep.alpha[i];
        li += e;
        li *= prep.qInv[i];
        outH.push_back(std::move(hi));
        outL.push_back(std::move(li));
    }
    return {DCRT(outH), DCRT(outL)};
}
}  // namespace

Mult2Fallback Mult2ScheduleSupport(
    const std::shared_ptr<lbcrypto::ILDCRTParams<lbcrypto::BigInteger>>& basis,
    Mult2Backend backend) {
#if NATIVEINT != 64
    (void)basis;
    (void)backend;
    return Mult2Fallback::NonNative64;
#else
#if !defined(__SIZEOF_INT128__) || defined(OPENFHE_2023_1788_DISABLE_WIDE_MULT2)
    if (backend == Mult2Backend::TailWide) {
        return Mult2Fallback::WideUnavailable;
    }
#else
    (void)backend;
#endif
    const auto& towers = basis->GetParams();
    const auto n = basis->GetRingDimension();
    if (n == 0 || (n & (n - 1)) != 0) {
        return Mult2Fallback::InvalidRoot;
    }
    for (std::size_t i = 0; i < towers.size(); ++i) {
        const auto& p = towers[i]->GetModulus();
        if (p < Integer(3) || p >= Integer(std::uint64_t{1} << 60) ||
            p.Mod(Integer(2)) != Integer(1)) {
            return Mult2Fallback::ModulusRange;
        }
        if (!IsPrime(p)) {
            return Mult2Fallback::NonPrimeModulus;
        }
        for (std::size_t j = 0; j < i; ++j) {
            if (p == towers[j]->GetModulus()) {
                return Mult2Fallback::DuplicateModulus;
            }
        }
        const auto root = towers[i]->GetRootOfUnity().Mod(p);
        // For power-of-two N this proves exact order 2N.
        if (root.ModExp(Integer(n), p) != p - Integer(1)) {
            return Mult2Fallback::InvalidRoot;
        }
    }
    return Mult2Fallback::None;
#endif
}

std::pair<std::vector<DCRT>, std::vector<DCRT>> ComputeMult2Schedule(
    const std::vector<DCRT>& high, const std::vector<DCRT>& low, Mult2Backend backend) {
    if (backend != Mult2Backend::SharedStaged && backend != Mult2Backend::TailDigit &&
        backend != Mult2Backend::TailWide) {
        throw std::invalid_argument("internal Mult2 schedule requires a candidate backend");
    }
    const Prepared prep(high.at(0), backend);
    std::vector<DCRT> h, l;
    h.reserve(high.size());
    l.reserve(high.size());
    for (std::size_t component = 0; component < high.size(); ++component) {
        auto result = backend == Mult2Backend::SharedStaged
                          ? Staged(high[component], low.at(component), prep)
                          : Tail(high[component], low.at(component), prep, backend);
        h.push_back(std::move(result.first));
        l.push_back(std::move(result.second));
    }
    return {std::move(h), std::move(l)};
}
}  // namespace openfhe_2023_1788::detail
