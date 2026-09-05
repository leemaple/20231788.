#include "repeated_mult2_semantic_oracle.h"
#include <iostream>
#include <set>
#include <utility>

namespace {
using namespace repeated_mult2_test;
using namespace openfhe_2023_1788;
constexpr char kName[]="repeated_mult2_semantic_two_square_contract";

struct OwnedUnrelatedTag final {
    std::string tag;
    ~OwnedUnrelatedTag() { lbcrypto::CryptoContextImpl<DCRTPoly>::ClearEvalMultKeys(tag); }
};
struct ClientOracle final {
    lbcrypto::PrivateKey<DCRTPoly> rootSecret;
    std::shared_ptr<OwnedUnrelatedTag> unrelated;
};
// The evaluator's reachable project-owned graph contains only public inputs,
// immutable plans/receipts, ciphertexts and public evaluation-key rows.
struct EvaluatorInputs final {
    std::shared_ptr<const RepeatedMult2Plan> plan;
    ReadOnlyCiphertext left, right;
};
struct Prepared final { ClientOracle client; EvaluatorInputs evaluator; };
struct Evaluation final { CiphertextPair x,y,z,w; NativeInteger d,m1,m2; };

Prepared PrepareClient() {
    auto setup=CreateRepeatedMult2DiagnosticSetup();
    Check(setup.plan && setup.publicKey && setup.rootSecret,"client setup outputs");
    const auto context=setup.plan->GetFamilyContext(0);
    const auto p=std::dynamic_pointer_cast<Parameters>(context->GetCryptoParameters());
    Check(setup.publicKey->GetCryptoContext()==context && setup.rootSecret->GetCryptoContext()==context,
          "client keys use root family");
    Check(setup.publicKey->GetKeyTag()==setup.plan->GetFamilyKeyTag(0) &&
          setup.rootSecret->GetKeyTag()==setup.plan->GetFamilyKeyTag(0),"client key tags");
    Check(setup.publicKey->GetPublicElements().size()==2,"public-key arity");
    for(const auto& poly:setup.publicKey->GetPublicElements()) {
        Check(Basis(poly.GetParams())==Basis(p->GetElementParams()) && poly.GetFormat()==Format::EVALUATION,
              "public key must be in actual Q, never QP");
    }
    const auto sentinelTag=setup.plan->GetFamilyKeyTag(0)+"-unrelated-semantic-witness";
    Check(lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys().count(sentinelTag)==0,
          "unrelated sentinel tag must not overwrite a row");
    auto sentinel=std::make_shared<OwnedUnrelatedTag>();
    sentinel->tag=sentinelTag;
    {
        auto secret=std::make_shared<lbcrypto::PrivateKeyImpl<DCRTPoly>>(context);
        secret->SetPrivateElement(setup.rootSecret->GetPrivateElement());
        secret->SetKeyTag(sentinel->tag);
        context->EvalMultKeyGen(secret);
    }
    using namespace repeated_mult2_exact_test;
    // Test-owned DCRT adapter only; do not read or serialize its stale cache.
    const auto x=precision_dcp_rcb_test::MakePrecisionPlaintext(context,Values(kX),100);
    const auto y=precision_dcp_rcb_test::MakePrecisionPlaintext(context,Values(kY),100);
    auto cx=context->Encrypt(setup.publicKey,x),cy=context->Encrypt(setup.publicKey,y);
    Check(cx && cy,"input encryption");
    return {{std::move(setup.rootSecret),std::move(sentinel)},
            {std::move(setup.plan),std::move(cx),std::move(cy)}};
}

template<class F> void Reject(F action,const std::string& label) {
    bool rejected=false;
    try { action(); }
    catch(const std::invalid_argument&) { rejected=true; }
    Check(rejected,"expected invalid_argument: "+label);
}
void ExactReceipt(const std::shared_ptr<const RepeatedMult2Receipt>& receipt,
                  std::size_t family,std::size_t operation,RepeatedPhase phase,
                  Int numerator,Int denominator,bool terminal=false) {
    Check(static_cast<bool>(receipt),"receipt missing");
    const Int gcd=Gcd(numerator,denominator); numerator/=gcd; denominator/=gcd;
    Check(receipt->GetFamilyIndex()==family && receipt->GetOperationIndex()==operation &&
          receipt->GetPhase()==phase && receipt->IsTerminal()==terminal,"receipt phase/family/operation");
    Check(receipt->GetExactScale().GetNumerator()==numerator &&
          receipt->GetExactScale().GetDenominator()==denominator,"independently derived exact scale");
    Check(Gcd(receipt->GetExactScale().GetNumerator(),receipt->GetExactScale().GetDenominator())==1,
          "receipt must be canonical reduced rational");
}
void FamilyChecks(const std::shared_ptr<const RepeatedMult2Plan>& plan) {
    Check(plan->GetFamilyCount()==2,"exactly two nonempty operation families");
    std::array<std::shared_ptr<Parameters>,2> params;
    for(std::size_t f=0;f<2;++f) {
        const auto c=plan->GetFamilyContext(f);
        auto p=std::dynamic_pointer_cast<Parameters>(c->GetCryptoParameters()); params[f]=p;
        Check(static_cast<bool>(p),"actual returned CKKS parameters");
        Check(c->getSchemeId()==lbcrypto::SCHEME::CKKSRNS_SCHEME && c->GetRingDimension()==64,"scheme and N");
        Check(p->GetElementParams()->GetParams().size()==10-f && p->GetMultiplicativeDepth()==9-f,
              "family full Q count/depth");
        Check(p->GetNumPartQ()==10-f && p->GetNumPerPartQ()==1,"HYBRID alpha=1");
        Check(p->GetKeySwitchTechnique()==lbcrypto::HYBRID && p->GetScalingTechnique()==lbcrypto::FIXEDMANUAL &&
              p->GetEncryptionTechnique()==lbcrypto::STANDARD && p->GetMultiplicationTechnique()==lbcrypto::HPS &&
              p->GetPREMode()==lbcrypto::NOT_SET && p->GetCKKSDataType()==lbcrypto::COMPLEX,"explicit modes");
        Check(p->GetSecretKeyDist()==lbcrypto::UNIFORM_TERNARY && p->GetStdLevel()==lbcrypto::HEStd_NotSet &&
              p->GetMultipartyMode()==lbcrypto::FIXED_NOISE_MULTIPARTY &&
              p->GetExecutionMode()==lbcrypto::EXEC_EVALUATION &&
              p->GetDecryptionNoiseMode()==lbcrypto::FIXED_NOISE_DECRYPT,"diagnostic/noise modes");
        Check(p->GetNoiseScale()==1 && p->GetDigitSize()==0 && p->GetMaxRelinSkDeg()==2 &&
              p->GetDistributionParameter()==3.19F && p->GetAssuranceMeasure()==36.0F &&
              p->GetStatisticalSecurity()==30 && p->GetNumAdversarialQueries()==1 &&
              p->GetThresholdNumOfParties()==1 && p->GetNoiseEstimate()==0 &&
              p->GetFloodingDistributionParameter()==0,"noise profile");
        Check(p->GetEncodingParams()->GetPlaintextModulus()==50 &&
              p->GetEncodingParams()->GetBatchSize()==16 && p->GetScalingFactorReal(0)==std::ldexp(1.0,50),
              "encoding/scaling profile");
        const auto& q=p->GetElementParams()->GetParams();
        const auto& auxiliary=p->GetParamsP()->GetParams();
        const auto& qp=p->GetParamsQP()->GetParams();
        Check(!auxiliary.empty() && qp.size()==q.size()+auxiliary.size(),"nonempty P and QP");
        // 50/55 are requested parameter bit settings, not a claim that every
        // prime straddling a power of two has that exact integer bit length.
        Check(q.front()->GetModulus()>q.back()->GetModulus() && q.back()->GetModulus()>NativeInteger(2),
              "actual first/Div prime ordering");
        std::set<NativeInteger> unique;
        for(std::size_t j=0;j<qp.size();++j) {
            const auto expected=j<q.size()?q[j]:auxiliary[j-q.size()];
            Check(qp[j]->GetModulus()==expected->GetModulus() &&
                  qp[j]->GetRootOfUnity()==expected->GetRootOfUnity() &&
                  qp[j]->GetCyclotomicOrder()==128 && unique.insert(qp[j]->GetModulus()).second,
                  "actual QP order/modulus/root/phi uniqueness");
        }
        Check(Basis(p->GetParamsPK())==Basis(p->GetElementParams()),"PRE NOT_SET selects Q public key basis");
        const auto& rows=lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
        const auto found=rows.find(plan->GetFamilyKeyTag(f));
        Check(!plan->GetFamilyKeyTag(f).empty() && found!=rows.end() && found->second.size()==1,"owned local row");
        const auto key=std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(found->second[0]);
        Check(key && key->GetCryptoContext()==c && key->GetKeyTag()==plan->GetFamilyKeyTag(f),"local row context/tag");
        Check(key->GetAVector().size()==q.size() && key->GetBVector().size()==q.size(),"alpha-one A/B row lengths");
        for(const auto* polynomials:{&key->GetAVector(),&key->GetBVector()}) for(const auto& poly:*polynomials) {
            Check(Basis(poly.GetParams())==Basis(p->GetParamsQP()) && poly.GetFormat()==Format::EVALUATION,
                  "family-local QP key entry");
            for(const auto& tower:poly.GetAllElements()) Check(tower.GetFormat()==Format::EVALUATION,"key native format");
        }
    }
    Check(plan->GetFamilyContext(0)!=plan->GetFamilyContext(1) && params[0]!=params[1] &&
          plan->GetFamilyContext(0)->GetScheme()!=plan->GetFamilyContext(1)->GetScheme() &&
          params[0]->GetElementParams()!=params[1]->GetElementParams() &&
          params[0]->GetParamsP()!=params[1]->GetParamsP() && params[0]->GetParamsQP()!=params[1]->GetParamsQP() &&
          plan->GetFamilyKeyTag(0)!=plan->GetFamilyKeyTag(1),"different family objects must not alias");
    const auto& q0=params[0]->GetElementParams()->GetParams();
    const auto& q1=params[1]->GetElementParams()->GetParams();
    for(std::size_t j=0;j<q1.size();++j) {
        const auto source=j+1==q1.size()?q0.back():q0[j];
        Check(q1[j]->GetModulus()==source->GetModulus() && q1[j]->GetRootOfUnity()==source->GetRootOfUnity() &&
              q1[j]->GetCyclotomicOrder()==source->GetCyclotomicOrder(),"B1 removes actual m1, retains exact d");
    }
}
void PairPhase(const CiphertextPair& p,const std::shared_ptr<const RepeatedMult2Plan>& plan,
               std::size_t family,std::size_t level,std::size_t noise,PairLifecycle lifecycle) {
    Check(p.GetContextIdentity()==plan->GetFamilyContext(family).get() &&
          p.GetKeyTag()==plan->GetFamilyKeyTag(family) && p.GetDivisor()==plan->GetDivisor(),"pair family/tag/divisor");
    Check(p.GetLevel()==level && p.GetNoiseScaleDegree()==noise && p.GetLifecycle()==lifecycle &&
          p.GetSlots()==16 && p.GetComponentCount()==2 && p.GetFormat()==Format::EVALUATION,"pair public state");
    const auto params=std::dynamic_pointer_cast<Parameters>(plan->GetFamilyContext(family)->GetCryptoParameters());
    const auto& q=params->GetElementParams()->GetParams();
    Check(p.GetOrderedModuli().size()==q.size()-level,"active basis length");
    for(const auto& c:{p.GetHigh(),p.GetLow()}) {
        Check(c->GetCryptoContext()==plan->GetFamilyContext(family) && c->GetKeyTag()==p.GetKeyTag() &&
              c->GetLevel()==level && c->GetNoiseScaleDeg()==noise && c->GetScalingFactor()==p.GetRecordedScalingFactor() &&
              c->GetSlots()==16 && c->GetEncodingType()==lbcrypto::CKKS_PACKED_ENCODING && c->GetElements().size()==2,
              "component metadata");
        for(const auto& poly:c->GetElements()) {
            Check(poly.GetFormat()==Format::EVALUATION && poly.GetNumOfElements()==q.size()-level,"component basis/format");
            for(std::size_t j=0;j<poly.GetNumOfElements();++j) {
                const auto& t=poly.GetElementAtIndex(j);
                Check(t.GetModulus()==q[j]->GetModulus() && t.GetParams()->GetRootOfUnity()==q[j]->GetRootOfUnity() &&
                      t.GetParams()->GetCyclotomicOrder()==128 && t.GetFormat()==Format::EVALUATION &&
                      p.GetOrderedModuli()[j]==t.GetModulus(),"active ordered prime/root/format");
            }
        }
    }
}
void SameValuesAfterReentry(const CiphertextPair& old,const CiphertextPair& next) {
    Check(old.GetHigh().get()!=next.GetHigh().get() && old.GetLow().get()!=next.GetLow().get(),"new family wrappers");
    for(const auto& pair:{std::make_pair(old.GetHigh(),next.GetHigh()),std::make_pair(old.GetLow(),next.GetLow())}) {
        const auto& a=pair.first; const auto& b=pair.second;
        Check(a->GetElements().size()==b->GetElements().size(),"re-entry arity");
        for(std::size_t j=0;j<a->GetElements().size();++j)
            Check(Polynomial(a->GetElements()[j])==Polynomial(b->GetElements()[j]),"re-entry coefficient identity");
        Check(a->GetScalingFactor()==b->GetScalingFactor() && a->GetScalingFactorInt()==b->GetScalingFactorInt() &&
              a->GetNoiseScaleDeg()==b->GetNoiseScaleDeg() && a->GetHopLevel()==b->GetHopLevel() &&
              a->GetSlots()==b->GetSlots() && a->GetEncodingType()==b->GetEncodingType(),"re-entry CKKS metadata identity");
    }
    Check(old.GetPaperScale().approximateLogicalScalingFactor==next.GetPaperScale().approximateLogicalScalingFactor &&
          old.GetPaperScale().approximateRecombinedLogicalScalingFactor==next.GetPaperScale().approximateRecombinedLogicalScalingFactor,
          "re-entry compatibility normalization unchanged");
}

// No private-key parameter/member/capture, and no client call anywhere in here.
Evaluation Evaluate(const EvaluatorInputs& input) {
    FamilyChecks(input.plan);
    const auto context0=Context(input.plan->GetFamilyContext(0)),context1=Context(input.plan->GetFamilyContext(1));
    const auto keys=AllKeyRows(),cx=Cipher(input.left),cy=Cipher(input.right);
    DoubleCKKS evaluator(input.plan);
    const auto X=evaluator.DCP(input.left),Y=evaluator.DCP(input.right);
    const auto xBefore=Pair(X),yBefore=Pair(Y);
    const auto d=X.GetDivisor(),m1=X.GetOrderedModuli().back();
    const auto T1=evaluator.Tensor2(X,Y); const auto R1=evaluator.Relin2(T1); const auto RS1=evaluator.RS2(R1);
    const auto rsBefore=Pair(RS1);
    const auto Z=evaluator.Mult2(X,Y);
    const auto zBefore=Pair(Z);
    const auto W=evaluator.Mult2(Z,Z);
    const auto m2=Z.GetOrderedModuli().back();
    // Public staged observations are wiring checks, not the semantic oracle.
    const auto T2=evaluator.Tensor2(Z,Z); const auto R2=evaluator.Relin2(T2); const auto RS2=evaluator.RS2(R2);
    Check(Cipher(W.GetHigh(),false)==Cipher(RS2.GetHigh(),false) &&
          Cipher(W.GetLow(),false)==Cipher(RS2.GetLow(),false),"stage-two public wiring");
    SameValuesAfterReentry(RS1,Z);
    Check(d!=m1 && d!=m2 && m1!=m2 && input.plan->GetDivisor()==d,"actual distinct d,m1,m2");
    PairPhase(X,input.plan,0,1,2,PairLifecycle::ReadyForFirstMult);
    PairPhase(Y,input.plan,0,1,2,PairLifecycle::ReadyForFirstMult);
    PairPhase(R1,input.plan,0,1,3,PairLifecycle::ReadyForRS2);
    PairPhase(RS1,input.plan,0,2,2,PairLifecycle::RefreshRequired);
    PairPhase(Z,input.plan,1,1,2,PairLifecycle::ReadyForRepeatedMult);
    PairPhase(R2,input.plan,1,1,3,PairLifecycle::ReadyForRS2);
    PairPhase(W,input.plan,1,2,2,PairLifecycle::RefreshRequired);
    const Int di=Integer(d),mi1=Integer(m1),mi2=Integer(m2);
    ExactReceipt(X.GetRepeatedReceipt(),0,0,RepeatedPhase::Input,Int(1)<<100,1);
    Check(X.GetRepeatedReceipt()==Y.GetRepeatedReceipt(),"same input normalization receipt");
    ExactReceipt(T1.GetRepeatedReceipt(),0,1,RepeatedPhase::Tensor,Int(1)<<200,di);
    ExactReceipt(R1.GetRepeatedReceipt(),0,1,RepeatedPhase::Relinearized,Int(1)<<200,di);
    ExactReceipt(RS1.GetRepeatedReceipt(),0,1,RepeatedPhase::Rescaled,Int(1)<<200,di*mi1);
    ExactReceipt(Z.GetRepeatedReceipt(),1,1,RepeatedPhase::Reentry,Int(1)<<200,di*mi1);
    ExactReceipt(T2.GetRepeatedReceipt(),1,2,RepeatedPhase::Tensor,Int(1)<<400,di*di*di*mi1*mi1);
    ExactReceipt(R2.GetRepeatedReceipt(),1,2,RepeatedPhase::Relinearized,Int(1)<<400,di*di*di*mi1*mi1);
    ExactReceipt(W.GetRepeatedReceipt(),1,2,RepeatedPhase::Rescaled,Int(1)<<400,di*di*di*mi1*mi1*mi2,true);
    Check(!X.GetRepeatedReceipt()->GetParent() && T1.GetRepeatedReceipt()->GetParent()==X.GetRepeatedReceipt() &&
          R1.GetRepeatedReceipt()->GetParent()==T1.GetRepeatedReceipt() &&
          RS1.GetRepeatedReceipt()->GetParent()==R1.GetRepeatedReceipt() &&
          Z.GetRepeatedReceipt()->GetParent()==RS1.GetRepeatedReceipt() &&
          T2.GetRepeatedReceipt()->GetParent()==Z.GetRepeatedReceipt() &&
          R2.GetRepeatedReceipt()->GetParent()==T2.GetRepeatedReceipt() &&
          W.GetRepeatedReceipt()->GetParent()==R2.GetRepeatedReceipt(),"immutable parent-derived receipt chain");
    for(const auto& tensor:{T1,T2}) {
        Check(tensor.GetComponentCount()==3 && tensor.GetLevel()==1 && tensor.GetNoiseScaleDegree()==3 &&
              tensor.GetSlots()==16 && tensor.GetFormat()==Format::EVALUATION,"Tensor2 shape");
        Check(tensor.GetHigh()->GetElements().size()==3 && tensor.GetLow()->GetElements().size()==3,"Tensor2 arity three");
    }
    Check(evaluator.Add(Z,Z).GetRepeatedReceipt()==Z.GetRepeatedReceipt() &&
          evaluator.Sub(Z,Z).GetRepeatedReceipt()==Z.GetRepeatedReceipt(),"Add/Sub retain exact same receipt");
    Reject([&]{ (void)evaluator.Add(X,R1); },"different exact phase/scale");
    Reject([&]{ (void)evaluator.Sub(X,Z); },"different family");
    Reject([&]{ (void)evaluator.Mult2(W,W); },"terminal plan receipt");
    DoubleCKKS legacy(input.plan->GetFamilyContext(0));
    const auto legacyPair=legacy.DCP(input.left);
    Reject([&]{ (void)evaluator.Mult2(legacyPair,legacyPair); },"no plan-issued receipt");
    Reject([&]{ (void)legacy.Mult2(RS1,RS1); },"plan receipt cannot be laundered through legacy constructor");
    auto mutableHigh=std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(Z.GetHigh());
    const auto originalTag=mutableHigh->GetKeyTag(); mutableHigh->SetKeyTag(originalTag+"-bad");
    Reject([&]{ (void)evaluator.RCB(Z); },"changed component tag"); mutableHigh->SetKeyTag(originalTag);
    mutableHigh->SetLevel(2); Reject([&]{ (void)evaluator.RCB(Z); },"changed local level"); mutableHigh->SetLevel(1);
    mutableHigh->SetNoiseScaleDeg(3); Reject([&]{ (void)evaluator.RCB(Z); },"changed noise degree"); mutableHigh->SetNoiseScaleDeg(2);
    const double recorded=mutableHigh->GetScalingFactor(); mutableHigh->SetScalingFactor(recorded*2);
    Reject([&]{ (void)evaluator.RCB(Z); },"changed recorded scale"); mutableHigh->SetScalingFactor(recorded);
    mutableHigh->SetSlots(8); Reject([&]{ (void)evaluator.RCB(Z); },"changed slots"); mutableHigh->SetSlots(16);
    Check(Pair(X)==xBefore && Pair(Y)==yBefore && Pair(Z)==zBefore && Pair(RS1)==rsBefore,"pair/source immutability");
    Check(Cipher(input.left)==cx && Cipher(input.right)==cy,"original encrypted input immutability");
    Check(Context(input.plan->GetFamilyContext(0))==context0 && Context(input.plan->GetFamilyContext(1))==context1,
          "actual context/profile/CRT-table immutability");
    Check(AllKeyRows()==keys,"owned and unrelated public key-row immutability");
    return {X,Y,Z,W,d,m1,m2};
}

void FinalClientOracle(const ClientOracle& client,const Evaluation& evaluated,std::size_t trial) {
    using namespace repeated_mult2_exact_test;
    const auto& secret=client.rootSecret->GetPrivateElement();
    const auto x=DecryptPair(evaluated.x,secret),y=DecryptPair(evaluated.y,secret);
    const auto z=DecryptPair(evaluated.z,secret),w=DecryptPair(evaluated.w,secret);
    const Int d=Integer(evaluated.d),m1=Integer(evaluated.m1),m2=Integer(evaluated.m2);
    for(std::size_t stage=1;stage<=2;++stage) {
        const Int n=Int(1)<<(stage==1?200:400);
        const Int denominator=stage==1?Int(d*m1):Int(d*d*d*m1*m1*m2);
        const Int gcd=Gcd(n,denominator),num=n/gcd,den=denominator/gcd;
        const auto actual=Canonical(stage==1?z:w,num,den);
        const auto expected=Values(stage==1?kZ:kW);
        Real maximum=0;
        for(std::size_t i=0;i<16;++i) { const Real error=Norm(Minus(actual[i],expected[i])); if(error>maximum) maximum=error; }
        const Real delta=Norm(Minus(Minus(actual[0],actual[1]),Float(Load(stage==1?kZDelta:kWDelta))));
        const auto& output=stage==1?evaluated.z:evaluated.w;
        const auto& input=stage==1?evaluated.x:evaluated.z;
        const std::size_t headroom=ObservedHeadroom(stage==1?x:z,stage==1?y:z,BasisProduct(input.GetHigh()->GetElements()[0]));
        std::cout << std::setprecision(50)
          << "{\"test\":\"" << kName << "\",\"scope\":\"low-N-two-operation-diagnostic\",\"trial\":" << trial
          << ",\"stage\":" << stage << ",\"N\":64,\"batch\":16,\"depth\":9,\"scaling_bits\":50,\"first_bits\":55"
          << ",\"input_scale_bits\":100,\"scaling\":\"FIXEDMANUAL\",\"key_switch\":\"HYBRID\",\"data_type\":\"COMPLEX\""
          << ",\"secret_distribution\":\"UNIFORM_TERNARY\",\"security\":\"HEStd_NotSet\",\"evaluation_family\":" << stage-1
          << ",\"result_family\":" << output.GetRepeatedReceipt()->GetFamilyIndex() << ",\"tag\":\"" << output.GetKeyTag()
          << "\",\"d\":\"" << d << "\",\"m\":\"" << (stage==1?m1:m2) << "\",\"m1\":\"" << m1 << "\",\"m2\":\"" << m2
          << "\",\"scale_numerator\":\"" << num << "\",\"scale_denominator\":\"" << den
          << "\",\"max_slot_error\":\"" << maximum << "\",\"delta_error\":\"" << delta
          << "\",\"observed_product_headroom_bits\":" << headroom << "}" << std::endl;
        Check(maximum<=Real(1)/Pow2(80),"all 16 complex slots must satisfy 2^-80");
        Check(delta<=Real(1)/Pow2(80),"distinguishing complex delta must satisfy 2^-80");
    }
}
} // namespace

int main() {
    VerifyFrozenArithmetic(); CheckCanonicalWitnesses();
    std::set<std::string> rootTags;
    for(std::size_t trial=0;trial<4;++trial) {
        auto prepared=PrepareClient();
        const auto rootTag=prepared.evaluator.plan->GetFamilyKeyTag(0);
        const auto nextTag=prepared.evaluator.plan->GetFamilyKeyTag(1);
        Check(rootTags.insert(rootTag).second,"four fresh root keypairs/tags per invocation");
        const auto result=Evaluate(prepared.evaluator);
        FinalClientOracle(prepared.client,result,trial);
        prepared.evaluator.plan.reset();
        const auto& rows=lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
        Check(rows.count(rootTag)==0 && rows.count(nextTag)==0 && rows.count(prepared.client.unrelated->tag)==1,
              "plan cleanup removes only its owned rows, preserves unrelated row");
    }
    return 0;
}
