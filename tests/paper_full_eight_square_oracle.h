#ifndef PAPER_FULL_EIGHT_SQUARE_ORACLE_H
#define PAPER_FULL_EIGHT_SQUARE_ORACLE_H

// Independent client oracle for the frozen paper profile. The only shared
// transform primitive is official inverse NTT (SetFormat on a native tower).
// No production codec, FFT, Decrypt, DecryptCore, or dense ring convolution.
#include "openfhe_2023_1788/high_precision_client_io.h"
#include "openfhe_2023_1788/repeated_mult2.h"
#include <boost/math/constants/constants.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace paper_full_test {
using Int = boost::multiprecision::cpp_int;
using Real = boost::multiprecision::number<
    boost::multiprecision::backends::cpp_bin_float<512, boost::multiprecision::backends::digit_base_2>>;
using Poly = lbcrypto::DCRTPoly;
using Cipher = openfhe_2023_1788::ReadOnlyCiphertext;
namespace io = openfhe_2023_1788::client_io;
constexpr std::size_t kN = 32768;
constexpr std::size_t kSlots = 16384;
constexpr std::uint32_t kM = 65536;
constexpr std::uint64_t kDiv = 1099510054913ULL;
constexpr std::uint64_t kP = 1152921504606584833ULL;
constexpr std::uint64_t kPRoot = 4443670208963ULL;
constexpr std::array<std::uint64_t, 11> kQ{{
    1125899904679937ULL, 1125899903827969ULL,
    1152921504598720513ULL, 1152921504597016577ULL,
    1152921504595968001ULL, 1152921504595640321ULL,
    1152921504593412097ULL, 1152921504592822273ULL,
    1152921504592429057ULL, 1152921504589938689ULL, kDiv}};
constexpr std::array<std::uint64_t, 11> kRoots{{
    26113207984ULL, 150640639383ULL, 100545759574150ULL,
    31693996050849ULL, 88651361085495ULL, 9679305630873ULL,
    24428769072221ULL, 18776242964106ULL, 5821397352863ULL,
    33888991361320ULL, 121567553ULL}};
constexpr std::array<std::size_t, 10> kAnchors{{
    0, 1, 256, 257, 512, 513, 768, 769, 1023, 16383}};

inline void Require(bool condition, const std::string& label) {
    if (!condition) throw std::runtime_error("paper contract: " + label);
}
inline Real R(const Int& x) { return Real(x.convert_to<std::string>()); }
inline Real Abs(const Real& x) { return x < 0 ? Real(-x) : x; }
inline Real Pow2(int e) { return boost::multiprecision::ldexp(Real(1), e); }
inline void Finite(const Real& x, const std::string& label) {
    Require(boost::math::isfinite(x), label + " nonfinite");
}
inline Int Mod(Int x, const Int& q) { x %= q; if (x < 0) x += q; return x; }
inline Int Center(Int x, const Int& q) { x = Mod(x, q); if (2*x > q) x -= q; return x; }
inline Int Gcd(Int a, Int b) {
    while (b != 0) { const Int r = a % b; a = b; b = r; }
    return a;
}
inline Int Inverse(Int a, const Int& q) {
    Int r0=q, r1=Mod(a,q), t0=0, t1=1;
    while (r1 != 0) {
        const Int quotient=r0/r1, r=r0-quotient*r1, t=t0-quotient*t1;
        r0=r1; r1=r; t0=t1; t1=t;
    }
    Require(r0==1, "CRT coprime moduli");
    return Mod(t0,q);
}
struct Scale final { Int numerator, denominator; };
inline Scale Reduced(Int n, Int d) {
    Require(n>0 && d>0, "positive test scale");
    const Int g=Gcd(n,d); return {n/g,d/g};
}
inline std::array<Scale,9> Scales() {
    std::array<Scale,9> out;
    out[0]={Int(1)<<100,1};
    // Closed product form, independent of the production recursive receipt.
    for (std::size_t i=1;i<=8;++i) {
        Int den=1;
        for (std::size_t j=1;j<=i;++j) {
            Int factor=Int(kDiv)*kQ[10-j];
            for (std::size_t n=0;n<i-j;++n) factor*=factor;
            den*=factor;
        }
        out[i]=Reduced(Int(1)<<(100*(std::size_t(1)<<i)),den);
    }
    return out;
}
struct Complex final { Real real, imag; };
inline Complex Multiply(const Complex& a,const Complex& b) {
    return {a.real*b.real-a.imag*b.imag,a.real*b.imag+a.imag*b.real};
}
inline Complex Input(std::size_t s) {
    const std::size_t t=s/2;
    const Real a=Real(1015)/1024-Real(t%16)/65536+Real(s)*Pow2(-75);
    Real b=Real(1+(t/16)%8)/1024;
    if ((t/512)%2) b=-b;
    switch ((t/128)%4) {
        case 0: return {a,b};
        case 1: return {-b,a};
        case 2: return {-a,-b};
        default: return {b,-a};
    }
}
inline std::vector<Complex> Inputs() {
    std::vector<Complex> result; result.reserve(kSlots);
    for (std::size_t s=0;s<kSlots;++s) result.push_back(Input(s));
    return result;
}
inline std::vector<io::ClientComplex> ClientInputs(const std::vector<Complex>& input) {
    std::vector<io::ClientComplex> out; out.reserve(input.size());
    for (const auto& z:input)
        out.push_back({io::ClientReal(z.real.str(100,std::ios_base::scientific)),
                       io::ClientReal(z.imag.str(100,std::ios_base::scientific))});
    return out;
}
inline Complex FromClient(const io::ClientComplex& z) {
    return {Real(z.real.str(100,std::ios_base::scientific)),
            Real(z.imag.str(100,std::ios_base::scientific))};
}
inline Real Error(const Complex& a,const Complex& b) {
    const Real re=Abs(a.real-b.real), im=Abs(a.imag-b.imag);
    Finite(re,"real error"); Finite(im,"imag error"); return re>im?re:im;
}
inline void Emit(const std::string& field,const Real& value) {
    Finite(value,field);
    std::cout << std::scientific << std::setprecision(45)
              << "OBS field=" << field << " value=" << value << '\n' << std::flush;
}
// Accumulate finite acceptance misses only. Shape, nonfinite, codec/oracle
// agreement and nonwrap checks still throw; no invalid-state continuation.
inline void PrecisionGate(bool condition,const std::string& label,std::size_t& failures) {
    if (!condition) {
        ++failures;
        std::cout << "OBS numeric_gate=FAIL label=" << label << '\n' << std::flush;
    }
}
inline Complex Difference(const Complex& a,const Complex& b) {
    return {a.real-b.real,a.imag-b.imag};
}
inline void EmitSigned(const std::string& field,const Complex& z) {
    Finite(z.real,field+".real"); Finite(z.imag,field+".imag");
    // Enough digits to preserve the signed sub-binary64 perturbation in w0.
    // All arithmetic remains the original binary512 Real, not these strings.
    std::cout << std::scientific << std::setprecision(100)
              << "OBS field=" << field << ".real value=" << z.real << '\n'
              << "OBS field=" << field << ".imag value=" << z.imag << '\n' << std::flush;
}
inline void ObserveResiduals(const std::array<Complex,10>& actual,
                             const std::array<Complex,10>& previous,
                             const std::array<Complex,10>& freshPower,
                             const std::vector<Complex>& expected,const std::string& label) {
    Real inheritedMax=0, addedMax=0, localMax=0;
    for (std::size_t a=0;a<kAnchors.size();++a) {
        const auto& z=expected.at(kAnchors[a]);
        const auto inherited=Difference(freshPower[a],z);
        const auto added=Difference(actual[a],freshPower[a]);
        const auto local=Difference(actual[a],Multiply(previous[a],previous[a]));
        const auto field="diag."+label+".anchor_"+std::to_string(kAnchors[a]);
        EmitSigned(field+".E",Difference(actual[a],z));
        EmitSigned(field+".I",inherited);
        EmitSigned(field+".A",added);
        EmitSigned(field+".L",local);
        const Complex zero{Real(0),Real(0)};
        const Real inheritedError=Error(inherited,zero);
        const Real addedError=Error(added,zero);
        const Real localError=Error(local,zero);
        if (inheritedError>inheritedMax) inheritedMax=inheritedError;
        if (addedError>addedMax) addedMax=addedError;
        if (localError>localMax) localMax=localError;
    }
    // Diagnostics, never substitute acceptance against freshPower for truth.
    Emit("diag."+label+".I_anchor_max_component",inheritedMax);
    Emit("diag."+label+".A_anchor_max_component",addedMax);
    Emit("diag."+label+".L_anchor_max_component",localMax);
}
inline Real CheckFull(const io::DecodedSlots& decoded,const std::vector<Complex>& expected,
                      const std::string& label,std::size_t& failures) {
    Require(decoded.values.size()==kSlots && expected.size()==kSlots,"full-slot count");
    Real maximum=0;
    for (std::size_t i=0;i<kSlots;++i) {
        const Real error=Error(FromClient(decoded.values[i]),expected[i]);
        if (error>maximum) maximum=error;
    }
    Emit(label+".full_max_component_error",maximum);
    PrecisionGate(maximum<=Pow2(-80),label+" full-slot 2^-80 gate",failures);
    const Real cross(decoded.diagnostics.maximumCrossPrecisionDisagreement.str(100,std::ios_base::scientific));
    Emit(label+".codec_cross_precision",cross);
    Require(cross>=0 && cross<=Pow2(-120),label+" codec 2^-120 gate");
    Require(decoded.diagnostics.centeredHeadroom>0,"positive actual centered headroom");
    return maximum;
}

// Extract a signed root secret once, independently verify all eleven residues.
using SparseSecret = std::vector<std::pair<std::size_t,int>>;
inline SparseSecret ReadSecret(const lbcrypto::PrivateKey<Poly>& key) {
    Require(static_cast<bool>(key),"root secret present");
    const auto& secret=key->GetPrivateElement();
    Require(secret.GetNumOfElements()==kQ.size() && secret.GetFormat()==Format::EVALUATION,"root secret full Q/evaluation");
    std::vector<int> signs(kN,0);
    for (std::size_t tower=0;tower<kQ.size();++tower) {
        auto native=secret.GetElementAtIndex(tower);
        Require(native.GetModulus().ConvertToInt()==kQ[tower] &&
                native.GetParams()->GetRootOfUnity().ConvertToInt()==kRoots[tower] &&
                native.GetParams()->GetCyclotomicOrder()==kM,"root secret basis");
        native.SetFormat(Format::COEFFICIENT);
        Require(native.GetLength()==kN,"root secret degree");
        for (std::size_t i=0;i<kN;++i) {
            const std::uint64_t v=native[i].ConvertToInt();
            Require(v==0 || v==1 || v==kQ[tower]-1,"signed ternary secret");
            const int sign=v==0?0:(v==1?1:-1);
            if (tower==0) signs[i]=sign;
            else Require(signs[i]==sign,"same signed secret across root towers");
        }
    }
    SparseSecret out;
    for (std::size_t i=0;i<kN;++i) if (signs[i]) out.emplace_back(i,signs[i]);
    Require(out.size()==128,"actual signed h128");
    return out;
}
struct IntegerPolynomial final { std::vector<Int> coefficients; Int modulus; };

inline IntegerPolynomial SparseDecrypt(const Cipher& ciphertext,const SparseSecret& secret) {
    Require(ciphertext && ciphertext->GetElements().size()==2,"oracle needs two ciphertext components");
    const auto& c=ciphertext->GetElements();
    const std::size_t towers=c[0].GetNumOfElements();
    Require(towers>=2 && towers<=kQ.size() && c[1].GetNumOfElements()==towers,"oracle active tower count");
    IntegerPolynomial result{std::vector<Int>(kN,0),Int(1)};
    for (std::size_t j=0;j<towers;++j) result.modulus*=kQ[j];
    for (std::size_t j=0;j<towers;++j) {
        auto c0=c[0].GetElementAtIndex(j), c1=c[1].GetElementAtIndex(j);
        for (const auto* t:{&c0,&c1})
            Require(t->GetModulus().ConvertToInt()==kQ[j] &&
                    t->GetParams()->GetRootOfUnity().ConvertToInt()==kRoots[j] &&
                    t->GetParams()->GetCyclotomicOrder()==kM,"oracle actual prefix basis/root");
        c0.SetFormat(Format::COEFFICIENT); c1.SetFormat(Format::COEFFICIENT);
        Require(c0.GetLength()==kN && c1.GetLength()==kN,"oracle coefficient count");
        const std::uint64_t q=kQ[j];
        std::vector<std::uint64_t> accum(kN), operand(kN);
        for (std::size_t n=0;n<kN;++n) {
            accum[n]=c0[n].ConvertToInt(); operand[n]=c1[n].ConvertToInt();
            Require(accum[n]<q && operand[n]<q,"canonical tower residues");
        }
        // q<2^60, so every unsigned add/sub below is <2^61: portable
        // on Windows without unsigned __int128 or compiler intrinsics.
        for (const auto& term:secret) for (std::size_t n=0;n<kN;++n) {
            const std::size_t degree=n+term.first, index=degree%kN;
            const bool positive=(term.second>0)==(degree<kN);
            const std::uint64_t v=operand[n], old=accum[index];
            if (positive) { const auto sum=old+v; accum[index]=sum>=q?sum-q:sum; }
            else accum[index]=old>=v?old-v:q-(v-old);
        }
        const Int partial=result.modulus/q;
        const Int weight=partial*Inverse(partial,Int(q));
        for (std::size_t n=0;n<kN;++n) result.coefficients[n]+=weight*accum[n];
    }
    for (auto& coefficient:result.coefficients) coefficient=Center(coefficient,result.modulus);
    return result;
}
inline void ObserveCoefficientScale(const IntegerPolynomial& polynomial,const Scale& scale,
                                    const std::string& label) {
    Int maximum=0;
    for (const auto& c:polynomial.coefficients) {
        const Int magnitude=c<0?Int(-c):c;
        if (magnitude>maximum) maximum=magnitude;
    }
    Emit("diag."+label+".coefficient_max_over_scale",
         R(maximum)*R(scale.denominator)/R(scale.numerator));
}
inline IntegerPolynomial RecombinedPolynomial(const openfhe_2023_1788::CiphertextPair& pair,
                                              const SparseSecret& secret) {
    auto high=SparseDecrypt(pair.GetHigh(),secret);
    const auto low=SparseDecrypt(pair.GetLow(),secret);
    Require(high.modulus==low.modulus,"oracle pair common modulus");
    for (std::size_t n=0;n<kN;++n)
        high.coefficients[n]=Center(Int(kDiv)*high.coefficients[n]+low.coefficients[n],high.modulus);
    return high;
}
inline std::array<Complex,10> AnchorRoots() {
    std::array<Complex,10> roots;
    // Avoid the fixed-storage 512 -> 1536 trig-reduction conversion that
    // triggers GCC's array-bounds diagnostic in Boost 1.83. Storage only:
    // precision, exponent range, angles and returned binary512 Real stay fixed.
    using RootReal=boost::multiprecision::number<boost::multiprecision::backends::cpp_bin_float<
        512,boost::multiprecision::backends::digit_base_2,
        std::allocator<boost::multiprecision::limb_type>>,boost::multiprecision::et_off>;
    const RootReal pi=boost::math::constants::pi<RootReal>();
    for (std::size_t i=0;i<kAnchors.size();++i) {
        std::uint32_t exponent=1;
        for (std::size_t s=0;s<kAnchors[i];++s) exponent=(exponent*5)%kM;
        const RootReal angle=RootReal(2)*pi*exponent/kM;
        const RootReal re=boost::multiprecision::cos(angle), im=boost::multiprecision::sin(angle);
        roots[i]={Real(re),Real(im)};
    }
    return roots;
}
inline std::array<Complex,10> Horner(const IntegerPolynomial& polynomial,const Scale& scale,
                                    const std::array<Complex,10>& roots) {
    std::array<Complex,10> values{};
    // Convert each integer once for all ten anchors. No transform table/FFT.
    for (auto it=polynomial.coefficients.rbegin();it!=polynomial.coefficients.rend();++it) {
        const Real coefficient=R(*it);
        for (std::size_t a=0;a<values.size();++a) {
            values[a]=Multiply(values[a],roots[a]); values[a].real+=coefficient;
        }
    }
    const Real inverseScale=R(scale.denominator)/R(scale.numerator);
    for (auto& z:values) { z.real*=inverseScale; z.imag*=inverseScale; }
    return values;
}
inline void CheckAnchors(const std::array<Complex,10>& actual,const std::vector<Complex>& expected,
                         const std::string& label,std::size_t& failures,
                         const io::DecodedSlots* production=nullptr) {
    Real maximum=0, disagreement=0;
    for (std::size_t a=0;a<kAnchors.size();++a) {
        const Real error=Error(actual[a],expected[kAnchors[a]]);
        if (error>maximum) maximum=error;
        Emit(label+".anchor_"+std::to_string(kAnchors[a])+"_error",error);
        if (production) {
            const Real diff=Error(actual[a],FromClient(production->values.at(kAnchors[a])));
            if (diff>disagreement) disagreement=diff;
        }
    }
    Emit(label+".anchor_max_component_error",maximum);
    PrecisionGate(maximum<=Pow2(-80),label+" independent anchor 2^-80 gate",failures);
    if (production) {
        Emit(label+".anchor_vs_production",disagreement);
        Require(disagreement<=Pow2(-80),label+" anchor vs production 2^-80 gate");
    }
}
}  // namespace paper_full_test
#endif
