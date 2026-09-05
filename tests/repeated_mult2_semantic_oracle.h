#ifndef OPENFHE_2023_1788_REPEATED_MULT2_SEMANTIC_ORACLE_H
#define OPENFHE_2023_1788_REPEATED_MULT2_SEMANTIC_ORACLE_H

#include "precision_dcp_rcb_fixture.h"
#include "repeated_mult2_exact_vectors.h"
#include "openfhe_2023_1788/repeated_mult2.h"
#include <boost/math/constants/constants.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace repeated_mult2_test {
using Int = boost::multiprecision::cpp_int;
using Real = precision_dcp_rcb_test::BigFloat;
using Complex = precision_dcp_rcb_test::MpComplex;
using lbcrypto::DCRTPoly;
using lbcrypto::NativeInteger;
using openfhe_2023_1788::CiphertextPair;
using openfhe_2023_1788::ReadOnlyCiphertext;

inline void Check(bool value, const std::string& why) {
    if (!value) throw std::runtime_error("repeated_mult2_semantic_two_square_contract: " + why);
}
inline Int Abs(const Int& n) { return n < 0 ? Int(-n) : n; }
inline Int Mod(const Int& n, const Int& q) { Int r = n % q; return r < 0 ? Int(r + q) : r; }
inline Int Center(const Int& n, const Int& q) { Int r = Mod(n, q); return r > q / 2 ? Int(r - q) : r; }
inline Int Gcd(Int a, Int b) {
    while (b != 0) { Int r = a % b; a = b; b = r; }
    return Abs(a);
}
inline Real Float(const Int& n) { return Real(n.convert_to<std::string>()); }
inline Int Integer(const NativeInteger& n) { return Int(n.ConvertToInt()); }
inline Real Pow2(unsigned n) { return Float(Int(1) << n); }
inline Complex Plus(const Complex& a, const Complex& b) { return {a.real+b.real, a.imag+b.imag}; }
inline Complex Minus(const Complex& a, const Complex& b) { return {a.real-b.real, a.imag-b.imag}; }
inline Complex Times(const Complex& a, const Complex& b) {
    return {a.real*b.real-a.imag*b.imag, a.real*b.imag+a.imag*b.real};
}
inline Real Norm(const Complex& a) { return sqrt(a.real*a.real+a.imag*a.imag); }
struct Dyadic {
    Int n;
    unsigned k;
    Dyadic(Int value, unsigned bits) : n(std::move(value)), k(bits) {
        if (n == 0) k = 0;
        while (k != 0 && n % 2 == 0) { n /= 2; --k; }
    }
};
inline Dyadic Load(const repeated_mult2_exact_test::RawDyadic& raw) {
    Dyadic d(Int(raw.numerator), raw.denominatorBits);
    Check(d.n.convert_to<std::string>() == raw.numerator && d.k == raw.denominatorBits,
          "noncanonical frozen dyadic");
    return d;
}
inline Dyadic Add(const Dyadic& a, const Dyadic& b) {
    const unsigned k = std::max(a.k, b.k);
    return Dyadic((a.n << (k-a.k)) + (b.n << (k-b.k)), k);
}
inline Dyadic Sub(const Dyadic& a, const Dyadic& b) { return Add(a, Dyadic(-b.n,b.k)); }
inline Dyadic Mul(const Dyadic& a, const Dyadic& b) { return Dyadic(a.n*b.n,a.k+b.k); }
inline bool Equal(const Dyadic& a, const Dyadic& b) { return a.n == b.n && a.k == b.k; }
struct ExactComplex { Dyadic r, i; };
inline ExactComplex Load(const repeated_mult2_exact_test::RawComplex& a) { return {Load(a.real),Load(a.imag)}; }
inline ExactComplex Mul(const ExactComplex& a, const ExactComplex& b) {
    return {Sub(Mul(a.r,b.r),Mul(a.i,b.i)),Add(Mul(a.r,b.i),Mul(a.i,b.r))};
}
inline ExactComplex Sub(const ExactComplex& a, const ExactComplex& b) { return {Sub(a.r,b.r),Sub(a.i,b.i)}; }
inline bool Equal(const ExactComplex& a,const ExactComplex& b) { return Equal(a.r,b.r) && Equal(a.i,b.i); }
inline Complex Float(const ExactComplex& a) { return {Float(a.r.n)/Pow2(a.r.k),Float(a.i.n)/Pow2(a.i.k)}; }
inline std::vector<Complex> Values(const std::array<repeated_mult2_exact_test::RawComplex,16>& a) {
    std::vector<Complex> out;
    for (const auto& v : a) out.push_back(Float(Load(v)));
    return out;
}
inline void VerifyFrozenArithmetic() {
    using namespace repeated_mult2_exact_test;
    for (std::size_t i=0;i<16;++i) {
        const auto z=Mul(Load(kX[i]),Load(kY[i]));
        Check(Equal(z,Load(kZ[i])),"frozen Z literal " + std::to_string(i));
        Check(Equal(Mul(z,z),Load(kW[i])),"frozen W literal " + std::to_string(i));
    }
    Check(Equal(Sub(Load(kZ[0]),Load(kZ[1])),Load(kZDelta)),"frozen Z delta");
    Check(Equal(Sub(Load(kW[0]),Load(kW[1])),Load(kWDelta)),"frozen W delta");
    Check(Norm(Float(Load(kZDelta))) > Real(4)/Pow2(80) &&
          Norm(Float(Load(kWDelta))) > Real(4)/Pow2(80),"delta distinguishes collapsed slots");
}

// This oracle never calls OpenFHE Decrypt, CRTInterpolate, a decoder, or the
// fixture's plaintext cache. Its only private input is supplied AFTER evaluation.
inline Int Inverse(Int a, const Int& q) {
    a=Mod(a,q); Int r=q, next=a, t=0, nt=1;
    while (next != 0) {
        Int quotient=r/next, rr=r-quotient*next, tt=t-quotient*nt;
        r=next; next=rr; t=nt; nt=tt;
    }
    Check(r==1,"CRT inverse does not exist"); return Mod(t,q);
}
inline std::vector<Int> Negacyclic(const std::vector<Int>& a,const std::vector<Int>& b) {
    Check(a.size()==64 && b.size()==64,"oracle ring dimension");
    std::vector<Int> c(64);
    for (std::size_t i=0;i<64;++i) for (std::size_t j=0;j<64;++j) {
        if(i+j<64) c[i+j]+=a[i]*b[j]; else c[i+j-64]-=a[i]*b[j];
    }
    return c;
}
inline bool SamePrime(const lbcrypto::NativePoly& a,const lbcrypto::NativePoly& b) {
    return a.GetParams()->GetCyclotomicOrder()==b.GetParams()->GetCyclotomicOrder() &&
           a.GetModulus()==b.GetModulus() &&
           a.GetParams()->GetRootOfUnity()==b.GetParams()->GetRootOfUnity();
}
inline Int BasisProduct(const DCRTPoly& p) {
    Int q=1; for(const auto& t:p.GetAllElements()) q*=Integer(t.GetModulus()); return q;
}
inline std::vector<Int> DecryptPolynomial(const ReadOnlyCiphertext& c, const DCRTPoly& rootSecret) {
    Check(c && c->GetElements().size()==2,"oracle expects arity two");
    auto secret=rootSecret; secret.SetFormat(Format::COEFFICIENT);
    auto c0=c->GetElements()[0], c1=c->GetElements()[1];
    c0.SetFormat(Format::COEFFICIENT); c1.SetFormat(Format::COEFFICIENT);
    Check(c0.GetNumOfElements()==c1.GetNumOfElements(),"oracle component basis count");
    const Int Q=BasisProduct(c0);
    std::vector<Int> result(64);
    for(std::size_t tower=0;tower<c0.GetNumOfElements();++tower) {
        const auto& a=c0.GetElementAtIndex(tower);
        const auto& b=c1.GetElementAtIndex(tower);
        Check(SamePrime(a,b),"oracle ciphertext prime/root identity");
        std::size_t matches=0, index=0;
        for(std::size_t j=0;j<secret.GetNumOfElements();++j) {
            if(SamePrime(a,secret.GetElementAtIndex(j))) { ++matches; index=j; }
        }
        Check(matches==1,"root secret projection must have exactly one prime/root match");
        const auto& s=secret.GetElementAtIndex(index);
        Check(a.GetLength()==64 && b.GetLength()==64 && s.GetLength()==64,"oracle coefficient length");
        std::vector<Int> bv(64),sv(64);
        for(std::size_t j=0;j<64;++j) { bv[j]=Integer(b[j]); sv[j]=Integer(s[j]); }
        auto product=Negacyclic(bv,sv);
        const Int q=Integer(a.GetModulus()), partial=Q/q, weight=partial*Inverse(partial,q);
        for(std::size_t j=0;j<64;++j) result[j]+=Mod(Integer(a[j])+product[j],q)*weight;
    }
    for(auto& n:result) n=Center(n,Q);
    return result;
}
inline std::vector<Int> DecryptPair(const CiphertextPair& pair,const DCRTPoly& rootSecret) {
    auto high=DecryptPolynomial(pair.GetHigh(),rootSecret);
    const auto low=DecryptPolynomial(pair.GetLow(),rootSecret);
    const Int d=Integer(pair.GetDivisor()),q=BasisProduct(pair.GetHigh()->GetElements()[0]);
    for(std::size_t i=0;i<64;++i) high[i]=Center(d*high[i]+low[i],q);
    return high;
}
inline std::vector<Complex> Canonical(const std::vector<Int>& coefficients,const Int& scaleN,const Int& scaleD) {
    Check(coefficients.size()==64 && scaleN>0 && scaleD>0,"canonical oracle input");
    // Fixed, independently enumerated subgroup orbit of 5 modulo 128.
    constexpr std::array<unsigned,16> exponents{{1,5,25,125,113,53,9,45,97,101,121,93,81,21,105,13}};
    std::vector<Complex> result;
    const Real pi=boost::math::constants::pi<Real>();
    for(unsigned exponent:exponents) {
        const Real angle=Real(2)*pi*exponent/128;
        const Complex root{cos(angle),sin(angle)};
        Complex value{0,0};
        for(std::size_t j=64;j-- >0;) value=Plus(Times(value,root),{Float(coefficients[j]),0});
        value.real*=Float(scaleD)/Float(scaleN); value.imag*=Float(scaleD)/Float(scaleN);
        result.push_back(value);
    }
    return result;
}
inline void CheckCanonicalWitnesses() {
    std::vector<Int> coefficients(64); coefficients[0]=1;
    for(const auto& z:Canonical(coefficients,1,1)) Check(Norm(Minus(z,{1,0}))<Real("1e-90"),"constant oracle witness");
    coefficients[0]=0; coefficients[32]=1;
    for(const auto& z:Canonical(coefficients,1,1)) Check(Norm(Minus(z,{0,1}))<Real("1e-90"),"i oracle witness");
    coefficients[32]=0; coefficients[2]=1;
    const auto actual=Canonical(coefficients,1,1);
    // X^2 table is independent of the root-enumeration/Horner routine above.
    const std::array<Complex,16> expected{{
        {Real("0.99518472667219688624483695310947992157547486872985706183361296578489016689458654"), Real("0.098017140329560601994195563888641845861136673167500567257264979809387302789087537")},
        {Real("0.88192126434835502971275686366038834950844262067472798063253861671206664704500350"), Real("0.47139673682599764855638762590525437765746031893248062140161403100883522166516175")},
        {Real("-0.77301045336273696081090660975846980097104129290080960935640289668795060530598730"), Real("0.63439328416364549821517161322549337067568709484172160643382471869668672612354839")},
        {Real("0.95694033573220886493579788698026996948284920563003726130120719988416014536816082"), Real("-0.29028467725446236763619237581739527469147627832415111142066711312539289829945745")},
        {Real("0.098017140329560601994195563888641845861136673167500567257264979809387302789087537"), Real("-0.99518472667219688624483695310947992157547486872985706183361296578489016689458654")},
        {Real("0.47139673682599764855638762590525437765746031893248062140161403100883522166516175"), Real("-0.88192126434835502971275686366038834950844262067472798063253861671206664704500350")},
        {Real("0.63439328416364549821517161322549337067568709484172160643382471869668672612354839"), Real("0.77301045336273696081090660975846980097104129290080960935640289668795060530598730")},
        {Real("-0.29028467725446236763619237581739527469147627832415111142066711312539289829945745"), Real("-0.95694033573220886493579788698026996948284920563003726130120719988416014536816082")},
        {Real("-0.99518472667219688624483695310947992157547486872985706183361296578489016689458654"), Real("-0.098017140329560601994195563888641845861136673167500567257264979809387302789087537")},
        {Real("-0.88192126434835502971275686366038834950844262067472798063253861671206664704500350"), Real("-0.47139673682599764855638762590525437765746031893248062140161403100883522166516175")},
        {Real("0.77301045336273696081090660975846980097104129290080960935640289668795060530598730"), Real("-0.63439328416364549821517161322549337067568709484172160643382471869668672612354839")},
        {Real("-0.95694033573220886493579788698026996948284920563003726130120719988416014536816082"), Real("0.29028467725446236763619237581739527469147627832415111142066711312539289829945745")},
        {Real("-0.098017140329560601994195563888641845861136673167500567257264979809387302789087537"), Real("0.99518472667219688624483695310947992157547486872985706183361296578489016689458654")},
        {Real("-0.47139673682599764855638762590525437765746031893248062140161403100883522166516175"), Real("0.88192126434835502971275686366038834950844262067472798063253861671206664704500350")},
        {Real("-0.63439328416364549821517161322549337067568709484172160643382471869668672612354839"), Real("-0.77301045336273696081090660975846980097104129290080960935640289668795060530598730")},
        {Real("0.29028467725446236763619237581739527469147627832415111142066711312539289829945745"), Real("0.95694033573220886493579788698026996948284920563003726130120719988416014536816082")},
    }};
    for(std::size_t i=0;i<16;++i) Check(Norm(Minus(actual[i],expected[i]))<Real("1e-75"),"X^2 oracle witness");
}

// Snapshots encode values, never object representation, secret keys, or plaintext
// caches. Separately recorded identities detect illicit replacement/aliasing.
template<class T> inline void Write(std::ostream& out,const T& value) { out << value << ';'; }
template<class T> inline void Write(std::ostream& out,const std::vector<T>& values) {
    out << '[' << values.size() << ':'; for(const auto& v:values) Write(out,v); out << ']';
}
template<class Params> inline std::string Basis(const std::shared_ptr<Params>& p) {
    Check(static_cast<bool>(p),"null basis"); std::ostringstream out;
    Write(out,p->GetCyclotomicOrder()); Write(out,p->GetModulus());
    for(const auto& q:p->GetParams()) {
        Check(static_cast<bool>(q),"null native params");
        Write(out,q->GetCyclotomicOrder()); Write(out,q->GetModulus()); Write(out,q->GetRootOfUnity());
    }
    return out.str();
}
inline std::string Polynomial(const DCRTPoly& p) {
    std::ostringstream out; out << Basis(p.GetParams()); Write(out,static_cast<int>(p.GetFormat()));
    for(const auto& t:p.GetAllElements()) {
        Write(out,static_cast<int>(t.GetFormat())); Write(out,t.GetParams()->GetCyclotomicOrder());
        Write(out,t.GetModulus()); Write(out,t.GetParams()->GetRootOfUnity());
        for(std::size_t j=0;j<t.GetLength();++j) Write(out,t[j]);
    }
    return out.str();
}
inline std::string Cipher(const ReadOnlyCiphertext& c,bool includeIdentity=true) {
    Check(static_cast<bool>(c),"null ciphertext snapshot"); std::ostringstream out;
    out << std::setprecision(30);
    if(includeIdentity) { Write(out,c.get()); Write(out,c->GetCryptoContext().get()); }
    Write(out,c->GetKeyTag()); Write(out,c->GetLevel()); Write(out,c->GetHopLevel());
    Write(out,c->GetNoiseScaleDeg()); Write(out,c->GetScalingFactor()); Write(out,c->GetScalingFactorInt());
    Write(out,c->GetSlots()); Write(out,static_cast<int>(c->GetEncodingType()));
    Check(c->GetMetadataMap() && c->GetMetadataMap()->empty(),"diagnostic custom metadata must be empty");
    for(const auto& p:c->GetElements()) out << Polynomial(p);
    return out.str();
}
inline std::string Pair(const CiphertextPair& p) {
    std::ostringstream out; out << std::setprecision(40);
    Write(out,p.GetContextIdentity()); Write(out,p.GetDivisor()); Write(out,p.GetOrderedModuli());
    Write(out,p.GetLevel()); Write(out,p.GetRecordedScalingFactor()); Write(out,p.GetNoiseScaleDegree());
    Write(out,static_cast<int>(p.GetLifecycle())); Write(out,p.GetKeyTag()); Write(out,p.GetSlots());
    Write(out,static_cast<int>(p.GetFormat())); Write(out,p.GetComponentCount());
    Write(out,p.GetPaperScale().inputRecordedScalingFactor); Write(out,p.GetPaperScale().divisor);
    Write(out,p.GetPaperScale().approximateLogicalScalingFactor);
    Write(out,p.GetPaperScale().approximateRecombinedLogicalScalingFactor);
    const auto r=p.GetRepeatedReceipt(); Write(out,r.get());
    if(r) {
        Write(out,r->GetFamilyIndex()); Write(out,r->GetOperationIndex()); Write(out,static_cast<int>(r->GetPhase()));
        Write(out,r->GetExactScale().GetNumerator()); Write(out,r->GetExactScale().GetDenominator());
        Write(out,r->GetParent().get()); Write(out,r->IsTerminal());
    }
    out << Cipher(p.GetHigh()) << Cipher(p.GetLow()); return out.str();
}
using Parameters=lbcrypto::CryptoParametersCKKSRNS;
using BarrettVector=std::decay_t<decltype(std::declval<const Parameters&>().GetModqBarrettMu())>;
struct ContextSnapshot {
    std::string values;
    std::vector<BarrettVector> barrett;
    bool operator==(const ContextSnapshot& other) const { return values==other.values && barrett==other.barrett; }
};
inline ContextSnapshot Context(const lbcrypto::CryptoContext<DCRTPoly>& c) {
    auto p=std::dynamic_pointer_cast<Parameters>(c->GetCryptoParameters());
    Check(static_cast<bool>(p),"CKKS actual params"); std::ostringstream out; out << std::setprecision(40);
    Write(out,c.get()); Write(out,p.get()); Write(out,c->GetScheme().get());
    Write(out,p->GetElementParams().get()); Write(out,p->GetParamsP().get()); Write(out,p->GetParamsQP().get());
    out << Basis(p->GetElementParams()) << Basis(p->GetParamsP()) << Basis(p->GetParamsQP()) << Basis(p->GetParamsPK());
    Write(out,p->GetNumPartQ()); Write(out,p->GetNumPerPartQ());
    Write(out,static_cast<int>(c->getSchemeId()));
    Write(out,static_cast<int>(p->GetKeySwitchTechnique())); Write(out,static_cast<int>(p->GetScalingTechnique()));
    Write(out,static_cast<int>(p->GetEncryptionTechnique())); Write(out,static_cast<int>(p->GetMultiplicationTechnique()));
    Write(out,static_cast<int>(p->GetPREMode())); Write(out,static_cast<int>(p->GetCKKSDataType()));
    Write(out,static_cast<int>(p->GetSecretKeyDist())); Write(out,static_cast<int>(p->GetStdLevel()));
    Write(out,static_cast<int>(p->GetMultipartyMode())); Write(out,static_cast<int>(p->GetExecutionMode()));
    Write(out,static_cast<int>(p->GetDecryptionNoiseMode())); Write(out,p->GetNoiseScale());
    Write(out,p->GetDistributionParameter()); Write(out,p->GetAssuranceMeasure()); Write(out,p->GetDigitSize());
    Write(out,p->GetMaxRelinSkDeg()); Write(out,p->GetMultiplicativeDepth());
    Write(out,p->GetStatisticalSecurity()); Write(out,p->GetNumAdversarialQueries()); Write(out,p->GetThresholdNumOfParties());
    Write(out,p->GetNoiseEstimate()); Write(out,p->GetFloodingDistributionParameter());
    Write(out,p->GetCompositeDegree()); Write(out,p->GetRegisterWordSize()); Write(out,static_cast<int>(p->GetMPIntBootCiphertextCompressionLevel()));
    const auto e=p->GetEncodingParams();
    Write(out,e->GetPlaintextModulus()); Write(out,e->GetPlaintextRootOfUnity());
    Write(out,e->GetPlaintextBigModulus()); Write(out,e->GetPlaintextBigRootOfUnity());
    Write(out,e->GetPlaintextGenerator()); Write(out,e->GetBatchSize());
    Write(out,p->GetPModq()); Write(out,p->GetPInvModq()); Write(out,p->GetPInvModqPrecon());
    Write(out,p->GetPHatInvModp()); Write(out,p->GetPHatInvModpPrecon()); Write(out,p->GetPHatModq());
    Write(out,p->GettInvModp()); Write(out,p->GettInvModpPrecon()); Write(out,p->GettModqPrecon());
    const auto n=static_cast<std::uint32_t>(p->GetElementParams()->GetParams().size());
    ContextSnapshot snapshot; snapshot.barrett.push_back(p->GetModqBarrettMu());
    for(std::uint32_t level=0;level<n;++level) {
        Write(out,p->GetScalingFactorReal(level)); Write(out,p->GetModReduceFactor(level));
        if(level+1<n) {
            Write(out,p->GetQlQlInvModqlDivqlModq(level)); Write(out,p->GetQlQlInvModqlDivqlModqPrecon(level));
            Write(out,p->GetqlInvModq(level)); Write(out,p->GetqlInvModqPrecon(level));
        }
        out << Basis(p->GetParamsPartQ(level));
        Write(out,p->GetPartQlHatInvModq(level,0)); Write(out,p->GetPartQlHatInvModqPrecon(level,0));
        for(std::uint32_t part=0;part<=level;++part) {
            out << Basis(p->GetParamsComplPartQ(level,part));
            Write(out,p->GetPartQlHatModp(level,part));
            snapshot.barrett.push_back(p->GetmodComplPartqBarrettMu(level,part));
        }
    }
    snapshot.values=out.str(); return snapshot;
}
inline std::string AllKeyRows() {
    std::ostringstream out;
    const auto& rows=lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    for(const auto& entry:rows) {
        Write(out,entry.first); Write(out,entry.second.size());
        for(const auto& key:entry.second) {
            Check(static_cast<bool>(key),"unexpected null cached key in isolated diagnostic");
            Write(out,key.get()); Write(out,key->GetCryptoContext().get()); Write(out,key->GetKeyTag());
            const auto relin=std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(key);
            Check(static_cast<bool>(relin),"unexpected cached key subtype");
            for(const auto& p:relin->GetAVector()) out << Polynomial(p);
            for(const auto& p:relin->GetBVector()) out << Polynomial(p);
        }
    }
    return out.str();
}
inline std::size_t ObservedHeadroom(const std::vector<Int>& a,const std::vector<Int>& b,const Int& q) {
    const auto product=Negacyclic(a,b); Int peak=1;
    for(const auto& n:product) { const Int magnitude=Abs(n); if(magnitude>peak) peak=magnitude; }
    Check(2*peak<q,"observed unreduced product wraps active ring");
    const Int ratio=q/(2*peak); return static_cast<std::size_t>(boost::multiprecision::msb(ratio));
}
} // namespace repeated_mult2_test
#endif
