#include "paper_full_eight_square_oracle.h"
#include <cmath>
#include <set>
#include <type_traits>

// Missing production declarations in the initial hosted build are API RED.
// Success requires executing the real chain below, not these shape assertions.
#ifndef PAPER_SOURCE_COMMIT
#error "CMake must supply the actual configured paper source SHA"
#endif
static_assert(NATIVEINT==64 && MATHBACKEND==4,"frozen native64/backend4");

namespace {
using namespace paper_full_test;
using namespace openfhe_2023_1788;
using Parameters=lbcrypto::CryptoParametersCKKSRNS;
using Plan=std::shared_ptr<const RepeatedMult2Plan>;
using Receipt=std::shared_ptr<const RepeatedMult2Receipt>;
constexpr char kPin[]="df495ba2e91739a6dc8f1de254fc5a41155ce504";
static_assert(!std::is_default_constructible_v<RepeatedMult2Result>,"only evaluator issues results");
static_assert(!std::is_aggregate_v<RepeatedMult2Result>,"result cannot be aggregate-forged");
static_assert(!std::is_constructible_v<RepeatedMult2Result,Plan,ReadOnlyCiphertext,Receipt>,
              "caller cannot supply plan/ciphertext/receipt");
static_assert(!std::is_constructible_v<RepeatedMult2Result,Plan,lbcrypto::Ciphertext<Poly>,Receipt>,
              "mutable ciphertext does not grant result construction");
static_assert(std::is_same_v<decltype(std::declval<const RepeatedMult2Result&>().GetCiphertext()),
                             ReadOnlyCiphertext>,"result getter must be read-only");
static_assert(std::is_same_v<decltype(std::declval<const RepeatedMult2Result&>().GetReceipt()),
                             const Receipt&>,"result owns immutable receipt");

template<class Action> void Reject(Action action,const std::string& label) {
    bool rejected=false;
    try { action(); }
    catch (const std::invalid_argument&) { rejected=true; }
    Require(rejected,"expected invalid_argument: "+label);
    std::cout << "OBS rejection=" << label << " result=PASS\n";
}
void CheckTower(const std::shared_ptr<lbcrypto::ILNativeParams>& p,std::size_t index) {
    Require(p && p->GetCyclotomicOrder()==kM && p->GetRingDimension()==kN &&
            p->GetModulus().ConvertToInt()==kQ[index] &&
            p->GetRootOfUnity().ConvertToInt()==kRoots[index],"frozen Q modulus/root/geometry");
}
void CheckFamilies(const Plan& plan) {
    Require(plan && plan->GetFamilyCount()==8 && plan->GetDivisor().ConvertToInt()==kDiv,"eight paper families");
    std::set<std::string> tags;
    for (std::size_t f=0;f<8;++f) {
        const auto c=plan->GetFamilyContext(f);
        Require(c && c->getSchemeId()==lbcrypto::CKKSRNS_SCHEME && c->GetRingDimension()==kN,"actual paper context");
        const auto p=std::dynamic_pointer_cast<Parameters>(c->GetCryptoParameters());
        Require(p && p->GetElementParams() && p->GetParamsP() && p->GetParamsQP(),"actual CKKS precomputations");
        const auto& q=p->GetElementParams()->GetParams();
        const auto& aux=p->GetParamsP()->GetParams();
        const auto& qp=p->GetParamsQP()->GetParams();
        Require(q.size()==11-f && p->GetMultiplicativeDepth()==10-f && aux.size()==1 &&
                qp.size()==q.size()+1,"family Q/P/QP counts/depth");
        Require(p->GetNumPartQ()==q.size() && p->GetNumPerPartQ()==1 &&
                p->GetNumberOfQPartitions()==q.size() && p->GetAuxBits()==60 &&
                p->GetExtraBits()==0,"actual HYBRID alpha1 profile");
        Require(p->GetScalingTechnique()==lbcrypto::FIXEDMANUAL && p->GetKeySwitchTechnique()==lbcrypto::HYBRID &&
                p->GetEncryptionTechnique()==lbcrypto::STANDARD && p->GetMultiplicationTechnique()==lbcrypto::HPS &&
                p->GetPREMode()==lbcrypto::NOT_SET && p->GetCKKSDataType()==lbcrypto::COMPLEX &&
                p->GetSecretKeyDist()==lbcrypto::SPARSE_TERNARY && p->GetStdLevel()==lbcrypto::HEStd_NotSet,
                "actual paper algorithms");
        Require(p->GetCompositeDegree()==1 && p->GetRegisterWordSize()==64 && p->GetNoiseScale()==1 &&
                p->GetDigitSize()==0 && p->GetMaxRelinSkDeg()==2 && p->GetDistributionParameter()==3.19F &&
                p->GetAssuranceMeasure()==36.0F && p->GetStatisticalSecurity()==30 &&
                p->GetNumAdversarialQueries()==1 && p->GetThresholdNumOfParties()==1 &&
                p->GetNoiseEstimate()==0 && p->GetFloodingDistributionParameter()==0 &&
                p->GetExecutionMode()==lbcrypto::EXEC_EVALUATION &&
                p->GetDecryptionNoiseMode()==lbcrypto::FIXED_NOISE_DECRYPT &&
                p->GetMultipartyMode()==lbcrypto::FIXED_NOISE_MULTIPARTY,"actual noise profile");
        Require(p->GetEncodingParams() && p->GetPlaintextModulus()==50 && p->GetBatchSize()==kSlots,
                "nominal50 full slots");
        const auto& pk=p->GetParamsPK()->GetParams();
        Require(pk.size()==q.size(),"public key uses Q");
        for (std::size_t j=0;j<q.size();++j) {
            const auto index=j+1==q.size()?10:j;
            CheckTower(q[j],index); CheckTower(qp[j],index); CheckTower(pk[j],index);
            const auto partition=p->GetParamsPartQ(static_cast<std::uint32_t>(j));
            Require(partition && partition->GetParams().size()==1,"one tower per partition");
            CheckTower(partition->GetParams()[0],index);
            Require(p->GetScalingFactorReal(static_cast<std::uint32_t>(j))==std::ldexp(1.0,50) &&
                    p->GetModReduceFactor(static_cast<std::uint32_t>(j))==std::ldexp(1.0,50),"nominal getters");
            std::cout << "PROFILE family=" << f << " tower=" << j << " q=" << kQ[index]
                      << " root=" << kRoots[index] << '\n';
        }
        for (const auto& a:{aux[0],qp.back()})
            Require(a && a->GetModulus().ConvertToInt()==kP &&
                    a->GetRootOfUnity().ConvertToInt()==kPRoot && a->GetCyclotomicOrder()==kM,"reserved P/root");
        Require(!plan->GetFamilyKeyTag(f).empty() && tags.insert(plan->GetFamilyKeyTag(f)).second,"distinct family tags");
        const auto& rows=lbcrypto::CryptoContextImpl<Poly>::GetAllEvalMultKeys();
        const auto found=rows.find(plan->GetFamilyKeyTag(f));
        Require(found!=rows.end() && found->second.size()==1,"owned evaluation row");
        const auto key=std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<Poly>>(found->second[0]);
        Require(key && key->GetCryptoContext()==c && key->GetKeyTag()==plan->GetFamilyKeyTag(f) &&
                key->GetAVector().size()==q.size() && key->GetBVector().size()==q.size(),"family-local eval key");
        for (const auto* row:{&key->GetAVector(),&key->GetBVector()}) for (const auto& poly:*row) {
            Require(poly.GetFormat()==Format::EVALUATION && poly.GetNumOfElements()==qp.size(),"eval row QP shape");
            for (std::size_t j=0;j<qp.size();++j) {
                const auto& tower=poly.GetElementAtIndex(j);
                Require(tower.GetModulus()==qp[j]->GetModulus() &&
                        tower.GetParams()->GetRootOfUnity()==qp[j]->GetRootOfUnity() &&
                        tower.GetParams()->GetCyclotomicOrder()==kM && tower.GetFormat()==Format::EVALUATION,
                        "eval row actual QP roots/order");
            }
        }
    }
}
void CheckCipher(const Cipher& c,const Plan& plan,std::size_t family,std::size_t level,std::size_t towers) {
    Require(c && c->GetCryptoContext()==plan->GetFamilyContext(family) &&
            c->GetKeyTag()==plan->GetFamilyKeyTag(family) && c->GetLevel()==level &&
            c->GetNoiseScaleDeg()==2 && c->GetScalingFactor()==std::ldexp(1.0,100) &&
            c->GetScalingFactorInt()==lbcrypto::NativeInteger(1) && c->GetSlots()==kSlots &&
            c->GetEncodingType()==lbcrypto::CKKS_PACKED_ENCODING && c->GetElements().size()==2,
            "ciphertext identity and degree2/nominal100 metadata");
    const auto metadata=c->GetMetadataMap();
    Require(!metadata || metadata->empty(),"empty metadata");
    for (const auto& poly:c->GetElements()) {
        Require(poly.GetNumOfElements()==towers && poly.GetFormat()==Format::EVALUATION,"active element shape");
        for (std::size_t j=0;j<towers;++j) {
            const auto& tower=poly.GetElementAtIndex(j);
            CheckTower(tower.GetParams(),j);
            Require(tower.GetFormat()==Format::EVALUATION,"active native format");
        }
    }
}
void CheckReceipt(const Receipt& r,std::size_t family,std::size_t op,RepeatedPhase phase,
                  const Scale& scale,bool terminal=false) {
    Require(r && r->GetFamilyIndex()==family && r->GetOperationIndex()==op &&
            r->GetPhase()==phase && r->IsTerminal()==terminal,"receipt public state");
    Require(r->GetExactScale().GetNumerator()==scale.numerator &&
            r->GetExactScale().GetDenominator()==scale.denominator,"independent exact rational scale");
}
void CheckPair(const CiphertextPair& pair,const Plan& plan,std::size_t round,const Scale& scale) {
    const bool terminal=round==8;
    const auto family=terminal?7:round;
    const std::size_t level=terminal?2:1;
    const auto lifecycle=round==0?PairLifecycle::ReadyForFirstMult:
        (terminal?PairLifecycle::RefreshRequired:PairLifecycle::ReadyForRepeatedMult);
    Require(pair.GetContextIdentity()==plan->GetFamilyContext(family).get() &&
            pair.GetKeyTag()==plan->GetFamilyKeyTag(family) && pair.GetDivisor().ConvertToInt()==kDiv &&
            pair.GetLevel()==level && pair.GetSlots()==kSlots && pair.GetNoiseScaleDegree()==2 &&
            pair.GetComponentCount()==2 && pair.GetFormat()==Format::EVALUATION &&
            pair.GetRecordedScalingFactor()==std::ldexp(1.0,100) && pair.GetLifecycle()==lifecycle,
            "returned pair observable metadata");
    Require(pair.GetOrderedModuli().size()==10-round,"returned pair tower count");
    for (std::size_t j=0;j<10-round;++j)
        Require(pair.GetOrderedModuli()[j].ConvertToInt()==kQ[j],"returned pair ordered prefix");
    CheckCipher(pair.GetHigh(),plan,family,level,10-round);
    CheckCipher(pair.GetLow(),plan,family,level,10-round);
    const auto phase=round==0?RepeatedPhase::Input:(terminal?RepeatedPhase::Rescaled:RepeatedPhase::Reentry);
    CheckReceipt(pair.GetRepeatedReceipt(),family,round,phase,scale,terminal);
    std::cout << "RECEIPT operation=" << round << " family=" << family << " local_level=" << level
              << " towers=" << 10-round << " recorded_exp2=100 degree=2 exact_n=" << scale.numerator
              << " exact_d=" << scale.denominator << " terminal=" << terminal << '\n' << std::flush;
}
void CheckReceiptChain(const CiphertextPair& pair,const Receipt& previous,std::size_t round,
                       const std::array<Scale,9>& scales) {
    auto rs=pair.GetRepeatedReceipt();
    if (round<8) rs=rs->GetParent();
    CheckReceipt(rs,round-1,round,RepeatedPhase::Rescaled,scales[round],round==8);
    const auto relin=rs->GetParent();
    const auto tensorScale=Reduced(scales[round-1].numerator*scales[round-1].numerator,
        scales[round-1].denominator*scales[round-1].denominator*kDiv);
    CheckReceipt(relin,round-1,round,RepeatedPhase::Relinearized,tensorScale);
    const auto tensor=relin->GetParent();
    CheckReceipt(tensor,round-1,round,RepeatedPhase::Tensor,tensorScale);
    Require(tensor->GetParent()==previous,"same-chain receipt parent");
}

struct Evaluation final {
    CiphertextPair initial;
    std::vector<CiphertextPair> stages;
    RepeatedMult2Result result;
};
// No secret/private-key parameter, capture, callback or client oracle here.
Evaluation Evaluate(const Plan& plan,const Cipher& input) {
    DoubleCKKS evaluator(plan);
    auto pair=evaluator.DCP(input);
    const auto initial=pair;
    Reject([&] { (void)evaluator.RCBWithReceipt(pair); },"nonterminal_input");
    std::vector<CiphertextPair> stages; stages.reserve(8);
    for (std::size_t round=1;round<=8;++round) {
        pair=evaluator.Mult2(pair,pair);
        stages.push_back(pair);
        if (round==1) Reject([&] { (void)evaluator.RCBWithReceipt(pair); },"nonterminal_first_square");
    }
    auto result=evaluator.RCBWithReceipt(pair);
    return {initial,std::move(stages),std::move(result)};
}
void ForeignRejections(const Evaluation& evaluation,const RepeatedMult2ClientSetup& foreign) {
    // Reuse the one small setup retained outside the paper owner's lifetime.
    DoubleCKKS foreignEvaluator(foreign.plan);
    Reject([&] { (void)foreignEvaluator.RCBWithReceipt(evaluation.stages.back()); },"foreign_issuer");
    // Existing supported N64/Q8 client context, with no additional KeyGen.
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> p;
    p.SetMultiplicativeDepth(7); p.SetScalingModSize(50); p.SetFirstModSize(55);
    p.SetScalingTechnique(lbcrypto::FIXEDMANUAL); p.SetKeySwitchTechnique(lbcrypto::HYBRID);
    p.SetDigitSize(0); p.SetMaxRelinSkDeg(2); p.SetNumLargeDigits(0);
    p.SetSecretKeyDist(lbcrypto::UNIFORM_TERNARY); p.SetSecurityLevel(lbcrypto::HEStd_NotSet);
    p.SetRingDim(64); p.SetBatchSize(16); p.SetCKKSDataType(lbcrypto::COMPLEX);
    p.SetPREMode(lbcrypto::NOT_SET); p.SetExecutionMode(lbcrypto::EXEC_EVALUATION);
    p.SetDecryptionNoiseMode(lbcrypto::FIXED_NOISE_DECRYPT);
    p.SetStandardDeviation(3.19f); p.SetNoiseEstimate(0.0); p.SetDesiredPrecision(25.0);
    p.SetStatisticalSecurity(30); p.SetNumAdversarialQueries(1);
    p.SetInteractiveBootCompressionLevel(lbcrypto::SLACK); p.SetCompositeDegree(1); p.SetRegisterWordSize(64);
    const auto context=lbcrypto::GenCryptoContext(p);
    Require(static_cast<bool>(context),"foreign supported Q8 context");
    context->Enable(lbcrypto::PKE); context->Enable(lbcrypto::KEYSWITCH); context->Enable(lbcrypto::LEVELEDSHE);
    const io::HighPrecisionClientIO foreignClient(context);
    Reject([&] { (void)foreignClient.BindRepeatedRcb(evaluation.result); },"foreign_client_binding");
}
void CheckBound(const io::BoundCiphertext& bound,const Plan& plan,const Scale& scale,bool final) {
    const auto& s=bound.State();
    Require(s.contextProfile.contextIdentity==plan->GetFamilyContext(0).get() &&
            s.keyTag==plan->GetFamilyKeyTag(0) && s.level==(final?9U:0U) && s.slots==kSlots &&
            s.strideGap==1 && s.componentCount==2 && s.noiseScaleDegree==2 &&
            s.recordedScalingFactor==std::ldexp(1.0,100) && s.scalingFactorInt==lbcrypto::NativeInteger(1) &&
            s.metadataMapEmpty && !s.firstMult2ScaleFactors.has_value() &&
            s.logicalScale.Numerator()==scale.numerator && s.logicalScale.Denominator()==scale.denominator &&
            s.origin==(final?io::ClientCiphertextOrigin::RepeatedMult2Rcb:
                             io::ClientCiphertextOrigin::FreshClientEncoding),"bound scale/state provenance");
    Require(s.activeBasis.cyclotomicOrder==kM && s.activeBasis.ringDimension==kN &&
            s.activeBasis.moduliDecimal.size()==(final?2U:11U) &&
            s.activeBasis.rootsOfUnityDecimal.size()==s.activeBasis.moduliDecimal.size(),"bound actual basis");
    for (std::size_t j=0;j<s.activeBasis.moduliDecimal.size();++j)
        Require(s.activeBasis.moduliDecimal[j]==std::to_string(kQ[j]) &&
                s.activeBasis.rootsOfUnityDecimal[j]==std::to_string(kRoots[j]),"bound prefix roots/order");
}
void Witness(const io::DecodedSlots& decoded,const std::vector<Complex>& expected,
             const std::string& label,std::size_t& failures) {
    const Real difference=expected[1].real-expected[0].real;
    const Real actual=FromClient(decoded.values[1]).real-FromClient(decoded.values[0]).real;
    Emit(label+".expected_witness_real_difference",difference);
    Emit(label+".actual_witness_real_difference",actual);
    PrecisionGate(actual>Pow2(-76) && Abs(actual-difference)<=2*Pow2(-80),
                  label+" retained sub-binary64 witness",failures);
}

// Only strings escape. All paper plan, I/O, result and bound owners die before
// the outer test checks cache cleanup; the independent small setup stays live.
std::vector<std::string> RunPaper(const RepeatedMult2ClientSetup& foreign,std::size_t& failures) {
    const auto scales=Scales();
    auto expected=Inputs();
    const auto roots=AnchorRoots();
    auto setup=CreatePaperRepeatedMult2Setup();
    CheckFamilies(setup.plan);
    std::vector<std::string> paperTags;
    for (std::size_t f=0;f<8;++f) paperTags.push_back(setup.plan->GetFamilyKeyTag(f));
    Require(setup.publicKey && setup.rootSecret &&
            setup.publicKey->GetCryptoContext()==setup.plan->GetFamilyContext(0) &&
            setup.rootSecret->GetCryptoContext()==setup.plan->GetFamilyContext(0) &&
            setup.publicKey->GetKeyTag()==setup.plan->GetFamilyKeyTag(0) &&
            setup.rootSecret->GetKeyTag()==setup.plan->GetFamilyKeyTag(0),"root key identity");
    Require(setup.publicKey->GetPublicElements().size()==2,"public-key arity");
    for (const auto& polynomial:setup.publicKey->GetPublicElements()) {
        Require(polynomial.GetNumOfElements()==11 && polynomial.GetFormat()==Format::EVALUATION,"public-key full Q/evaluation");
        for (std::size_t j=0;j<11;++j) CheckTower(polynomial.GetElementAtIndex(j).GetParams(),j);
    }
    const auto secret=ReadSecret(setup.rootSecret);
    const auto secretBefore=setup.rootSecret->GetPrivateElement();
    const auto publicBefore=setup.publicKey->GetPublicElements();
    const io::HighPrecisionClientIO client(setup.plan);
    const auto fresh=client.Encrypt(setup.publicKey,ClientInputs(expected),
        {static_cast<std::uint32_t>(kSlots),io::PositiveRationalScale::FromPositive(Int(1)<<100,1)});
    CheckBound(fresh,setup.plan,scales[0],false);
    const auto source=fresh.CloneForEvaluation();
    CheckCipher(source,setup.plan,0,0,11);
    const auto sourceBefore=source->Clone();
    const auto freshDecoded=client.Decrypt(setup.rootSecret,fresh);
    CheckFull(freshDecoded,expected,"fresh",failures); Witness(freshDecoded,expected,"fresh",failures);
    const auto freshPolynomial=SparseDecrypt(source,secret);
    ObserveCoefficientScale(freshPolynomial,scales[0],"fresh");
    const auto freshAnchors=Horner(freshPolynomial,scales[0],roots);
    CheckAnchors(freshAnchors,expected,"fresh",failures,&freshDecoded);
    for (std::size_t a=0;a<kAnchors.size();++a) {
        const auto field="diag.fresh.anchor_"+std::to_string(kAnchors[a]);
        EmitSigned(field+".w0",freshAnchors[a]);
        EmitSigned(field+".E",Difference(freshAnchors[a],expected.at(kAnchors[a])));
    }
    auto previousAnchors=freshAnchors;
    auto freshPowers=freshAnchors;

    const auto evaluation=Evaluate(setup.plan,source);
    CheckPair(evaluation.initial,setup.plan,0,scales[0]);
    Require(!evaluation.initial.GetRepeatedReceipt()->GetParent(),"fresh receipt has no parent");
    Require(evaluation.stages.size()==8,"eight returned stages");
    Receipt previous=evaluation.initial.GetRepeatedReceipt();
    for (std::size_t round=1;round<=8;++round) {
        const auto& pair=evaluation.stages[round-1];
        CheckPair(pair,setup.plan,round,scales[round]);
        CheckReceiptChain(pair,previous,round,scales); previous=pair.GetRepeatedReceipt();
        for (auto& z:expected) z=Multiply(z,z);
        const auto polynomial=RecombinedPolynomial(pair,secret);
        const auto label="round_"+std::to_string(round);
        ObserveCoefficientScale(polynomial,scales[round],label);
        const auto anchors=Horner(polynomial,scales[round],roots);
        for (auto& w:freshPowers) w=Multiply(w,w);
        ObserveResiduals(anchors,previousAnchors,freshPowers,expected,label);
        CheckAnchors(anchors,expected,label,failures);
        previousAnchors=anchors;
    }
    Require(source->GetElements()==sourceBefore->GetElements() &&
            source->GetLevel()==sourceBefore->GetLevel() &&
            source->GetScalingFactor()==sourceBefore->GetScalingFactor(),"evaluation preserves fresh input");
    Require(setup.rootSecret->GetPrivateElement()==secretBefore &&
            setup.publicKey->GetPublicElements()==publicBefore,"evaluation preserves root key values");
    CheckCipher(source,setup.plan,0,0,11);
    Require(evaluation.result.GetReceipt()==previous,"result owns terminal receipt");
    CheckCipher(evaluation.result.GetCiphertext(),setup.plan,0,9,2);
    const auto bound=client.BindRepeatedRcb(evaluation.result);
    CheckBound(bound,setup.plan,scales[8],true);
    auto escaped=bound.CloneForEvaluation();
    escaped->SetElements(std::vector<Poly>{});
    Require(bound.CloneForEvaluation()->GetElements()==evaluation.result.GetCiphertext()->GetElements(),
            "bound snapshot unaffected by mutable evaluation clone");
    const auto decoded=client.Decrypt(setup.rootSecret,bound);
    CheckFull(decoded,expected,"final",failures); Witness(decoded,expected,"final",failures);
    const auto finalPolynomial=SparseDecrypt(evaluation.result.GetCiphertext(),secret);
    Require(finalPolynomial.modulus==Int(kQ[0])*kQ[1] &&
            decoded.diagnostics.activeCompositeModulus==finalPolynomial.modulus,"terminal two-Base modulus");
    ObserveCoefficientScale(finalPolynomial,scales[8],"final");
    CheckAnchors(Horner(finalPolynomial,scales[8],roots),expected,"final",failures,&decoded);
    const auto wrong=Horner(finalPolynomial,scales[0],roots);
    const Real wrongError=Error(wrong[0],expected[0]);
    Emit("final.wrong_nominal100_error",wrongError);
    Require(wrongError>Pow2(-30),"wrong nominal normalization must fail meaningfully");
    const Real delta=expected[1].real-expected[0].real;
    Require(delta>Pow2(-71) && delta<Pow2(-70),"exact-scalar audit witness interval");
    // Independent published scalar anchors (truncation far below the gate).
    Require(Abs(expected[0].real-Real("0.10106701692533075452271390324763932267007358697064098"))<Pow2(-150) &&
            Abs(expected[0].imag-Real("0.02604542052026911640736001715600098685432818977191963"))<Pow2(-150),
            "scalar expected oracle agrees with audited rational anchor");
    for (std::size_t s=0;s<kSlots;++s) {
        const Real norm2=expected[s].real*expected[s].real+expected[s].imag*expected[s].imag;
        Require(norm2>Real(".098")*Real(".098") && norm2<Real(".106")*Real(".106"),"nonzero ideal output domain");
        const auto z=FromClient(decoded.values[s]);
        Require(z.real*z.real+z.imag*z.imag>Real(".09")*Real(".09"),"nonzero actual output");
    }
    ForeignRejections(evaluation,foreign);
    CheckFamilies(setup.plan);
    return paperTags;
}
void Run() {
    std::cout << "BEGIN test=paper_full_eight_square_contract source=" << PAPER_SOURCE_COMMIT
              << " openfhe_pin=" << kPin << " native=64 backend=4 N=32768 M=65536 slots=16384 gap=1"
              << " h=128 nominal=50 P=" << kP << " P_root=" << kPRoot << " chain_count=1\n" << std::flush;
    const auto foreign=CreateRepeatedMult2DiagnosticSetup();
    struct SavedRow final {
        std::string tag;
        lbcrypto::EvalKey<Poly> identity;
        std::vector<Poly> a,b;
    };
    std::vector<SavedRow> unrelated;
    Require(foreign.plan && foreign.plan->GetFamilyCount()==2,"nonvacuous foreign setup");
    for (std::size_t f=0;f<2;++f) {
        const auto tag=foreign.plan->GetFamilyKeyTag(f);
        const auto& rows=lbcrypto::CryptoContextImpl<Poly>::GetAllEvalMultKeys();
        const auto found=rows.find(tag);
        Require(found!=rows.end() && found->second.size()==1,"unrelated row initially present");
        const auto key=std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<Poly>>(found->second[0]);
        Require(static_cast<bool>(key),"unrelated relin row type");
        // Small N64 row copies only, never the paper profile's large key rows.
        unrelated.push_back({tag,found->second[0],key->GetAVector(),key->GetBVector()});
    }
    std::size_t numericFailures=0;
    const auto paperTags=RunPaper(foreign,numericFailures);
    const auto& rows=lbcrypto::CryptoContextImpl<Poly>::GetAllEvalMultKeys();
    Require(paperTags.size()==8,"eight released owner tags");
    for (const auto& tag:paperTags) Require(rows.count(tag)==0,"paper destructor removes owned row");
    for (const auto& saved:unrelated) {
        const auto found=rows.find(saved.tag);
        Require(found!=rows.end() && found->second.size()==1 && found->second[0]==saved.identity,
                "unrelated cache identity survives paper lifetime");
        const auto key=std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<Poly>>(found->second[0]);
        Require(key && key->GetAVector()==saved.a && key->GetBVector()==saved.b,
                "unrelated cache coefficient values survive paper lifetime");
    }
    std::cout << "OBS lifecycle=paper_owner_cleanup owned_absent=8 unrelated_unchanged=2 result=PASS\n";
    std::cout << "OBS numeric_gate_failures=" << numericFailures << '\n' << std::flush;
    Require(numericFailures==0,"accumulated numeric acceptance failures: "+std::to_string(numericFailures));
    std::cout << "COMPLETE test=paper_full_eight_square_contract result=PASS source=" << PAPER_SOURCE_COMMIT
              << " openfhe_pin=" << kPin << " chain_count=1 squares=8 full_slots=16384 anchors=10"
              << " error_gate=2^-80 codec_gate=2^-120 gaussian_global_guarantee=false\n" << std::flush;
}
}  // namespace
int main() {
    try { Run(); return 0; }
    catch (const std::exception& error) {
        std::cerr << "COMPLETE test=paper_full_eight_square_contract result=FAIL source=" << PAPER_SOURCE_COMMIT
                  << " openfhe_pin=" << kPin << " reason=" << error.what() << '\n';
        return 1;
    }
}
