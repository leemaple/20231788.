#include "openfhe.h"
#include "openfhe_2023_1788/double_ckks.h"

#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

namespace {

using lbcrypto::CryptoContext;
using lbcrypto::DCRTPoly;
using openfhe_2023_1788::DoubleCKKS;
using openfhe_2023_1788::ReadOnlyCiphertext;
using openfhe_2023_1788::TensorCiphertextPair;
using openfhe_2023_1788::TensorScaleDescriptor;

class TestFailure final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void Check(bool condition, const std::string& message) {
    if (!condition) {
        throw TestFailure(message);
    }
}

class ImmutabilityProbeMetadata final : public lbcrypto::Metadata {
public:
    explicit ImmutabilityProbeMetadata(std::string value) : value_(std::move(value)) {}

    std::shared_ptr<lbcrypto::Metadata> Clone() const override {
        return std::make_shared<ImmutabilityProbeMetadata>(value_);
    }

    bool operator==(const lbcrypto::Metadata& metadata) const override {
        const auto* other = dynamic_cast<const ImmutabilityProbeMetadata*>(&metadata);
        return other != nullptr && value_ == other->value_;
    }

private:
    std::string value_;
};

template <class Function>
void CheckThrowsExactInvalidArgument(Function&& function, const std::string& expectedMessage,
                                     const std::string& label) {
    bool threw = false;
    try {
        std::invoke(std::forward<Function>(function));
    }
    catch (const std::invalid_argument& exception) {
        Check(exception.what() == expectedMessage,
              label + " reported an unexpected diagnostic: " + exception.what());
        threw = true;
    }
    catch (const std::exception& exception) {
        throw TestFailure(label + " threw the wrong exception type: " + exception.what());
    }
    Check(threw, label + " did not fail fast");
}

template <class Function>
void CheckPassesCurrentScaffoldOrCompletes(Function&& function, const std::string& label) {
    try {
        std::invoke(std::forward<Function>(function));
    }
    catch (const std::logic_error& exception) {
        Check(typeid(exception) == typeid(std::logic_error),
              label + " threw a derived logic-error type: " + exception.what());
        Check(std::string(exception.what()) == "DoubleCKKS: Relin2 is not implemented",
              label + " reported an unexpected diagnostic: " + exception.what());
    }
    catch (const std::exception& exception) {
        throw TestFailure(label + " threw the wrong exception type: " + exception.what());
    }
}

using EvalMultKeyMap = std::map<std::string, std::vector<lbcrypto::EvalKey<DCRTPoly>>>;

class ScopedEvalMultKeyMapRestore final {
public:
    explicit ScopedEvalMultKeyMapRestore(EvalMultKeyMap& cache) : cache_(cache), snapshot_(cache) {}

    ~ScopedEvalMultKeyMapRestore() noexcept {
        cache_.swap(snapshot_);
    }

    ScopedEvalMultKeyMapRestore(const ScopedEvalMultKeyMapRestore&) = delete;
    ScopedEvalMultKeyMapRestore& operator=(const ScopedEvalMultKeyMapRestore&) = delete;

private:
    EvalMultKeyMap& cache_;
    EvalMultKeyMap snapshot_;
};

struct MetadataSnapshotEntry {
    std::string key;
    std::shared_ptr<lbcrypto::Metadata> identity;
    std::shared_ptr<lbcrypto::Metadata> value;
};

using MetadataSnapshot = std::vector<MetadataSnapshotEntry>;

MetadataSnapshot SnapshotMetadata(const ReadOnlyCiphertext& ciphertext, const std::string& label) {
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");

    MetadataSnapshot snapshot;
    snapshot.reserve(metadata->size());
    for (const auto& [key, value] : *metadata) {
        Check(value != nullptr, label + " metadata value is null");
        auto cloned = value->Clone();
        Check(cloned != nullptr, label + " metadata clone is null");
        snapshot.push_back({key, value, std::move(cloned)});
    }
    return snapshot;
}

void CheckMetadataUnchanged(const ReadOnlyCiphertext& ciphertext, const MetadataSnapshot& before,
                            const std::string& label) {
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");
    Check(metadata->size() == before.size(), label + " metadata map size changed");

    auto current = metadata->begin();
    for (const auto& expected : before) {
        Check(current != metadata->end(), label + " metadata entry disappeared");
        Check(current->first == expected.key, label + " metadata key changed");
        Check(current->second != nullptr, label + " metadata value became null");
        Check(current->second.get() == expected.identity.get(), label + " metadata value identity changed");
        Check(*(current->second) == *(expected.value), label + " metadata value changed");
        ++current;
    }
}

struct TensorSnapshot {
    ReadOnlyCiphertext highIdentity;
    ReadOnlyCiphertext lowIdentity;
    lbcrypto::Ciphertext<DCRTPoly> highClone;
    lbcrypto::Ciphertext<DCRTPoly> lowClone;
    lbcrypto::MetadataMap highMetadataIdentity;
    lbcrypto::MetadataMap lowMetadataIdentity;
    MetadataSnapshot highMetadata;
    MetadataSnapshot lowMetadata;
    const lbcrypto::CryptoContextImpl<DCRTPoly>* contextIdentity;
    lbcrypto::NativeInteger divisor;
    std::vector<lbcrypto::NativeInteger> orderedModuli;
    std::size_t level;
    TensorScaleDescriptor tensorScale;
    double recordedScalingFactor;
    std::size_t noiseScaleDegree;
    std::string keyTag;
    std::uint32_t slots;
    Format format;
    std::size_t componentCount;
};

TensorSnapshot SnapshotTensor(const TensorCiphertextPair& tensor) {
    return {
        tensor.GetHigh(),
        tensor.GetLow(),
        tensor.GetHigh()->Clone(),
        tensor.GetLow()->Clone(),
        tensor.GetHigh()->GetMetadataMap(),
        tensor.GetLow()->GetMetadataMap(),
        SnapshotMetadata(tensor.GetHigh(), "Relin2 Tensor high"),
        SnapshotMetadata(tensor.GetLow(), "Relin2 Tensor low"),
        tensor.GetContextIdentity(),
        tensor.GetDivisor(),
        tensor.GetOrderedModuli(),
        tensor.GetLevel(),
        tensor.GetTensorScale(),
        tensor.GetRecordedScalingFactor(),
        tensor.GetNoiseScaleDegree(),
        tensor.GetKeyTag(),
        tensor.GetSlots(),
        tensor.GetFormat(),
        tensor.GetComponentCount(),
    };
}

void CheckTensorUnchanged(const TensorCiphertextPair& tensor, const TensorSnapshot& before,
                          const std::string& label) {
    Check(tensor.GetHigh().get() == before.highIdentity.get(), label + " high ciphertext identity changed");
    Check(tensor.GetLow().get() == before.lowIdentity.get(), label + " low ciphertext identity changed");
    Check(*tensor.GetHigh() == *before.highClone, label + " high ciphertext state changed");
    Check(*tensor.GetLow() == *before.lowClone, label + " low ciphertext state changed");
    Check(tensor.GetHigh()->GetMetadataMap() == before.highMetadataIdentity,
          label + " high metadata-map identity changed");
    Check(tensor.GetLow()->GetMetadataMap() == before.lowMetadataIdentity,
          label + " low metadata-map identity changed");
    CheckMetadataUnchanged(tensor.GetHigh(), before.highMetadata, label + " high");
    CheckMetadataUnchanged(tensor.GetLow(), before.lowMetadata, label + " low");
    Check(tensor.GetContextIdentity() == before.contextIdentity, label + " context manifest changed");
    Check(tensor.GetDivisor() == before.divisor, label + " divisor manifest changed");
    Check(tensor.GetOrderedModuli() == before.orderedModuli, label + " basis manifest changed");
    Check(tensor.GetLevel() == before.level, label + " level manifest changed");
    Check(tensor.GetTensorScale().approximateHighLogicalScalingFactor ==
              before.tensorScale.approximateHighLogicalScalingFactor,
          label + " high logical-scale manifest changed");
    Check(tensor.GetTensorScale().approximateRecombinedLogicalScalingFactor ==
              before.tensorScale.approximateRecombinedLogicalScalingFactor,
          label + " recombined logical-scale manifest changed");
    Check(tensor.GetRecordedScalingFactor() == before.recordedScalingFactor,
          label + " recorded scaling-factor manifest changed");
    Check(tensor.GetNoiseScaleDegree() == before.noiseScaleDegree,
          label + " noise-scale degree manifest changed");
    Check(tensor.GetKeyTag() == before.keyTag, label + " key-tag manifest changed");
    Check(tensor.GetSlots() == before.slots, label + " slots manifest changed");
    Check(tensor.GetFormat() == before.format, label + " format manifest changed");
    Check(tensor.GetComponentCount() == before.componentCount, label + " component-count manifest changed");
}

struct KeyPolynomialSnapshot {
    DCRTPoly value;
    const void* paramsIdentity;
    std::uint32_t cyclotomicOrder;
    lbcrypto::BigInteger modulus;
    lbcrypto::BigInteger rootOfUnity;
    Format format;
    std::vector<const void*> declaredTowerParamIdentities;
    std::vector<std::uint32_t> declaredTowerCyclotomicOrders;
    std::vector<lbcrypto::NativeInteger> declaredTowerModuli;
    std::vector<lbcrypto::NativeInteger> declaredTowerRoots;
    std::vector<const void*> actualTowerParamIdentities;
    std::vector<std::uint32_t> actualTowerCyclotomicOrders;
    std::vector<lbcrypto::NativeInteger> actualTowerModuli;
    std::vector<lbcrypto::NativeInteger> actualTowerRoots;
    std::vector<Format> actualTowerFormats;
};

KeyPolynomialSnapshot SnapshotKeyPolynomial(const DCRTPoly& polynomial, const std::string& label) {
    const auto& params = polynomial.GetParams();
    Check(params != nullptr, label + " aggregate parameters are null");

    KeyPolynomialSnapshot snapshot{
        polynomial,
        params.get(),
        params->GetCyclotomicOrder(),
        params->GetModulus(),
        params->GetRootOfUnity(),
        polynomial.GetFormat(),
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
    };

    for (const auto& towerParams : params->GetParams()) {
        Check(towerParams != nullptr, label + " declared tower parameters are null");
        snapshot.declaredTowerParamIdentities.push_back(towerParams.get());
        snapshot.declaredTowerCyclotomicOrders.push_back(towerParams->GetCyclotomicOrder());
        snapshot.declaredTowerModuli.push_back(towerParams->GetModulus());
        snapshot.declaredTowerRoots.push_back(towerParams->GetRootOfUnity());
    }
    for (const auto& tower : polynomial.GetAllElements()) {
        const auto& towerParams = tower.GetParams();
        Check(towerParams != nullptr, label + " actual tower parameters are null");
        snapshot.actualTowerParamIdentities.push_back(towerParams.get());
        snapshot.actualTowerCyclotomicOrders.push_back(tower.GetCyclotomicOrder());
        snapshot.actualTowerModuli.push_back(tower.GetModulus());
        snapshot.actualTowerRoots.push_back(tower.GetRootOfUnity());
        snapshot.actualTowerFormats.push_back(tower.GetFormat());
    }
    return snapshot;
}

using KeyVectorSnapshot = std::vector<KeyPolynomialSnapshot>;

KeyVectorSnapshot SnapshotKeyVector(const std::vector<DCRTPoly>& entries, const std::string& label) {
    KeyVectorSnapshot snapshot;
    snapshot.reserve(entries.size());
    for (std::size_t index = 0; index < entries.size(); ++index) {
        snapshot.push_back(SnapshotKeyPolynomial(entries[index], label + " entry " + std::to_string(index)));
    }
    return snapshot;
}

void CheckKeyVectorUnchanged(const std::vector<DCRTPoly>& entries, const KeyVectorSnapshot& before,
                             const std::string& label) {
    Check(entries.size() == before.size(), label + " vector length changed");
    for (std::size_t index = 0; index < entries.size(); ++index) {
        const auto current = SnapshotKeyPolynomial(entries[index], label + " entry " + std::to_string(index));
        const auto& expected = before[index];
        Check(current.value == expected.value, label + " polynomial value changed");
        Check(current.paramsIdentity == expected.paramsIdentity, label + " aggregate-parameter identity changed");
        Check(current.cyclotomicOrder == expected.cyclotomicOrder,
              label + " aggregate cyclotomic order changed");
        Check(current.modulus == expected.modulus, label + " aggregate modulus changed");
        Check(current.rootOfUnity == expected.rootOfUnity, label + " aggregate root of unity changed");
        Check(current.format == expected.format, label + " aggregate format changed");
        Check(current.declaredTowerParamIdentities == expected.declaredTowerParamIdentities,
              label + " declared tower-parameter identities changed");
        Check(current.declaredTowerCyclotomicOrders == expected.declaredTowerCyclotomicOrders,
              label + " declared tower cyclotomic orders changed");
        Check(current.declaredTowerModuli == expected.declaredTowerModuli,
              label + " declared tower moduli changed");
        Check(current.declaredTowerRoots == expected.declaredTowerRoots,
              label + " declared tower roots changed");
        Check(current.actualTowerParamIdentities == expected.actualTowerParamIdentities,
              label + " actual tower-parameter identities changed");
        Check(current.actualTowerCyclotomicOrders == expected.actualTowerCyclotomicOrders,
              label + " actual tower cyclotomic orders changed");
        Check(current.actualTowerModuli == expected.actualTowerModuli,
              label + " actual tower moduli changed");
        Check(current.actualTowerRoots == expected.actualTowerRoots,
              label + " actual tower roots changed");
        Check(current.actualTowerFormats == expected.actualTowerFormats,
              label + " actual tower formats changed");
    }
}

void CheckKeyPolynomialBasis(
    const DCRTPoly& polynomial,
    const std::shared_ptr<lbcrypto::ILDCRTParams<lbcrypto::BigInteger>>& expectedBasis,
    const std::string& label) {
    Check(expectedBasis != nullptr, label + " expected basis is null");
    const auto& actualBasis = polynomial.GetParams();
    Check(actualBasis != nullptr && *actualBasis == *expectedBasis,
          label + " declared basis does not match the expected basis");
    const auto& expectedTowers = expectedBasis->GetParams();
    const auto& actualTowers = polynomial.GetAllElements();
    Check(actualTowers.size() == expectedTowers.size(),
          label + " actual tower count does not match the expected basis");
    for (std::size_t index = 0; index < actualTowers.size(); ++index) {
        Check(expectedTowers[index] != nullptr, label + " expected tower parameters are null");
        Check(actualTowers[index].GetCyclotomicOrder() == expectedTowers[index]->GetCyclotomicOrder() &&
                  actualTowers[index].GetModulus() == expectedTowers[index]->GetModulus() &&
                  actualTowers[index].GetRootOfUnity() == expectedTowers[index]->GetRootOfUnity(),
              label + " actual tower basis does not match the expected basis");
    }
}

DCRTPoly CloneKeyPolynomialWithIndependentParams(const DCRTPoly& source, const std::string& label) {
    std::vector<DCRTPoly::PolyType> clonedTowers;
    clonedTowers.reserve(source.GetAllElements().size());
    for (std::size_t index = 0; index < source.GetAllElements().size(); ++index) {
        const auto& sourceTower = source.GetAllElements()[index];
        const auto& sourceParams = sourceTower.GetParams();
        Check(sourceParams != nullptr, label + " source tower parameters are null");
        auto clonedParams = std::make_shared<DCRTPoly::PolyType::Params>(*sourceParams);
        Check(clonedParams.get() != sourceParams.get() && *clonedParams == *sourceParams,
              label + " tower parameter clone is not independent and semantically equal");
        DCRTPoly::PolyType clonedTower(clonedParams, sourceTower.GetFormat());
        clonedTower.SetValues(sourceTower.GetValues(), sourceTower.GetFormat());
        Check(clonedTower.GetParams().get() != sourceParams.get() &&
                  *clonedTower.GetParams() == *sourceParams &&
                  clonedTower.GetFormat() == sourceTower.GetFormat() &&
                  clonedTower.GetValues() == sourceTower.GetValues(),
              label + " tower clone changed parameters, format, or values");
        clonedTowers.push_back(std::move(clonedTower));
    }

    DCRTPoly clone(clonedTowers);
    Check(clone.GetParams().get() != source.GetParams().get() &&
              *clone.GetParams() == *source.GetParams() && clone == source,
          label + " aggregate clone is not independent and semantically equal");
    Check(clone.GetAllElements().size() == source.GetAllElements().size(),
          label + " aggregate clone changed the tower count");
    for (std::size_t index = 0; index < clone.GetAllElements().size(); ++index) {
        Check(clone.GetAllElements()[index].GetParams().get() !=
                  source.GetAllElements()[index].GetParams().get() &&
                  *clone.GetAllElements()[index].GetParams() ==
                  *source.GetAllElements()[index].GetParams() &&
                  clone.GetAllElements()[index].GetValues() ==
                  source.GetAllElements()[index].GetValues(),
              label + " returned tower clone reused parameters or changed values");
    }
    return clone;
}

CryptoContext<DCRTPoly> MakeContext(std::uint32_t multiplicativeDepth = 3,
                                    std::uint32_t batchSize = 8,
                                    lbcrypto::KeySwitchTechnique keySwitchTechnique = lbcrypto::HYBRID,
                                    std::uint32_t digitSize = 0) {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multiplicativeDepth);
    parameters.SetScalingModSize(30);
    parameters.SetFirstModSize(35);
    parameters.SetScalingTechnique(lbcrypto::FIXEDMANUAL);
    parameters.SetKeySwitchTechnique(keySwitchTechnique);
    parameters.SetDigitSize(digitSize);
    parameters.SetSecurityLevel(lbcrypto::HEStd_NotSet);
    parameters.SetRingDim(32);
    parameters.SetBatchSize(batchSize);

    auto context = lbcrypto::GenCryptoContext(parameters);
    context->Enable(lbcrypto::PKE);
    context->Enable(lbcrypto::KEYSWITCH);
    context->Enable(lbcrypto::LEVELEDSHE);
    return context;
}

void TestTensorValidationOrder() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    auto tensor = module.Tensor2(left, right);

    auto& tensorScale = const_cast<TensorScaleDescriptor&>(tensor.GetTensorScale());
    Check(std::isfinite(tensorScale.approximateHighLogicalScalingFactor) &&
              tensorScale.approximateHighLogicalScalingFactor > 0.0L,
          "Relin2 validation-order fixture has an invalid starting high logical scale");
    tensorScale.approximateHighLogicalScalingFactor *= 2.0L;

    CheckThrowsExactInvalidArgument(
        [&] { (void)module.Relin2(tensor); },
        "DoubleCKKS: Tensor2 result paper-scale descriptor is inconsistent",
        "Relin2 Tensor manifest validation before evaluation-key lookup");
}

void TestInsufficientActiveBasis() {
    auto context = MakeContext(2);
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    Check(leftInput->GetElements().front().GetAllElements().size() == 3,
          "Relin2 insufficient-basis fixture must have exactly three full-basis towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    (void)module.RCB(left);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 2,
          "Relin2 insufficient-basis fixture must have exactly two active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 insufficient-basis fixture must have noise-scale degree three");

    CheckThrowsExactInvalidArgument(
        [&] { (void)module.Relin2(tensor); },
        "DoubleCKKS: Relin2 requires at least as many active Q_l towers as the Tensor noise-scale degree",
        "Relin2 insufficient active basis");
}

void TestMissingEvaluationKey() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 missing-key fixture must have exactly four full-basis towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 missing-key fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 missing-key fixture must have noise-scale degree three");

    const std::string keyTag = tensor.GetKeyTag();
    const auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(), "Relin2 missing-key fixture must start with an empty evaluation-key cache");
    Check(evaluationKeys.find(keyTag) == evaluationKeys.end(),
          "Relin2 missing-key fixture unexpectedly contains an evaluation key");

    CheckThrowsExactInvalidArgument(
        [&] { (void)module.Relin2(tensor); },
        "DoubleCKKS: Relin2 evaluation key is missing for the Tensor key tag",
        "Relin2 missing evaluation key");

    Check(evaluationKeys.empty(), "Relin2 missing-key rejection mutated the evaluation-key cache");
}

void TestEmptyEvaluationKeyVector() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 empty-key fixture must have exactly four full-basis towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 empty-key fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 empty-key fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(), "Relin2 empty-key fixture must have a nonempty key tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-immutability-probe") != highMetadata->end(),
          "Relin2 empty-key fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-immutability-probe") != lowMetadata->end(),
          "Relin2 empty-key fixture low ciphertext lost its metadata probe");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(), "Relin2 empty-key fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        const auto insertion = evaluationKeys.emplace(
            tensor.GetKeyTag(), std::vector<lbcrypto::EvalKey<DCRTPoly>>{});
        Check(insertion.second, "Relin2 empty-key fixture failed to insert the expected cache row");
        Check(evaluationKeys.size() == 1 && insertion.first->second.empty(),
              "Relin2 empty-key fixture must contain exactly one empty evaluation-key vector");

        const auto cacheBefore = evaluationKeys;
        const auto tensorBefore = SnapshotTensor(tensor);
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 evaluation-key vector is empty",
            "Relin2 empty evaluation-key vector");
        CheckTensorUnchanged(tensor, tensorBefore, "Relin2 empty-key rejection");
        Check(evaluationKeys == cacheBefore, "Relin2 empty-key rejection mutated the evaluation-key cache");
    }
    Check(evaluationKeys.empty(), "Relin2 empty-key fixture failed to restore the evaluation-key cache");
}

void TestNullFirstEvaluationKey() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-null-first-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 null-first-key fixture must have exactly four full-basis towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 null-first-key fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 null-first-key fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(), "Relin2 null-first-key fixture must have a nonempty key tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-null-first-immutability-probe") != highMetadata->end(),
          "Relin2 null-first-key fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-null-first-immutability-probe") != lowMetadata->end(),
          "Relin2 null-first-key fixture low ciphertext lost its metadata probe");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(), "Relin2 null-first-key fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        const auto insertion = evaluationKeys.emplace(
            tensor.GetKeyTag(), std::vector<lbcrypto::EvalKey<DCRTPoly>>{nullptr});
        Check(insertion.second, "Relin2 null-first-key fixture failed to insert the expected cache row");
        Check(evaluationKeys.size() == 1 && insertion.first->second.size() == 1 &&
                  insertion.first->second.front() == nullptr,
              "Relin2 null-first-key fixture must contain exactly one null evaluation key");

        const auto cacheBefore = evaluationKeys;
        const auto tensorBefore = SnapshotTensor(tensor);
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 first evaluation key is null",
            "Relin2 null first evaluation key");
        CheckTensorUnchanged(tensor, tensorBefore, "Relin2 null-first-key rejection");
        Check(evaluationKeys == cacheBefore,
              "Relin2 null-first-key rejection mutated the evaluation-key cache");
    }
    Check(evaluationKeys.empty(), "Relin2 null-first-key fixture failed to restore the evaluation-key cache");
}

void TestWrongContextEvaluationKey() {
    auto context = MakeContext();
    auto foreignContext = MakeContext(3, 4);
    Check(foreignContext.get() != context.get(),
          "Relin2 wrong-context fixture must use a distinct CryptoContext");
    Check(*foreignContext->GetCryptoParameters()->GetElementParams() ==
              *context->GetCryptoParameters()->GetElementParams(),
          "Relin2 wrong-context fixture contexts must have identical element parameters");

    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-wrong-context-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 wrong-context fixture must have exactly four full-basis towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 wrong-context fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 wrong-context fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(), "Relin2 wrong-context fixture must have a nonempty key tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-wrong-context-immutability-probe") != highMetadata->end(),
          "Relin2 wrong-context fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-wrong-context-immutability-probe") != lowMetadata->end(),
          "Relin2 wrong-context fixture low ciphertext lost its metadata probe");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(), "Relin2 wrong-context fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        const auto foreignKeys = foreignContext->KeyGen();
        foreignKeys.secretKey->SetKeyTag(tensor.GetKeyTag());
        foreignContext->EvalMultKeyGen(foreignKeys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 wrong-context fixture must generate exactly one evaluation key");
        const auto wrongContextKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(wrongContextKey != nullptr,
              "Relin2 wrong-context fixture key must have the relinearization-key subtype");
        Check(wrongContextKey->GetCryptoContext().get() == foreignContext.get() &&
                  wrongContextKey->GetCryptoContext().get() != tensor.GetContextIdentity(),
              "Relin2 wrong-context fixture key lost its foreign context");
        Check(wrongContextKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 wrong-context fixture key must otherwise have the expected tag");
        Check(!wrongContextKey->GetAVector().empty() && !wrongContextKey->GetBVector().empty(),
              "Relin2 wrong-context fixture must contain generated key material");

        const auto* cacheIdentityBefore = &evaluationKeys;
        const auto* vectorIdentityBefore = &generatedRow->second;
        const auto keyIdentityBefore = generatedRow->second.front();
        const auto keyContextBefore = wrongContextKey->GetCryptoContext();
        const auto keyTagBefore = wrongContextKey->GetKeyTag();
        const auto keyABefore = wrongContextKey->GetAVector();
        const auto keyBBefore = wrongContextKey->GetBVector();
        const auto tensorBefore = SnapshotTensor(tensor);
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 first evaluation key belongs to a different context",
            "Relin2 wrong-context first evaluation key");
        CheckTensorUnchanged(tensor, tensorBefore, "Relin2 wrong-context rejection");
        const auto currentRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBefore && evaluationKeys.size() == 1 &&
                  currentRow != evaluationKeys.end() && &currentRow->second == vectorIdentityBefore &&
                  currentRow->second.size() == 1 &&
                  currentRow->second.front().get() == keyIdentityBefore.get(),
              "Relin2 wrong-context rejection mutated the evaluation-key cache shape or identity");
        Check(wrongContextKey->GetCryptoContext().get() == keyContextBefore.get(),
              "Relin2 wrong-context rejection mutated the evaluation-key context");
        Check(wrongContextKey->GetKeyTag() == keyTagBefore,
              "Relin2 wrong-context rejection mutated the evaluation-key tag");
        Check(wrongContextKey->GetAVector() == keyABefore && wrongContextKey->GetBVector() == keyBBefore,
              "Relin2 wrong-context rejection mutated the evaluation-key A/B vectors");
    }
    Check(evaluationKeys.empty(), "Relin2 wrong-context fixture failed to restore the evaluation-key cache");
}

void TestWrongTagEvaluationKey() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-wrong-tag-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 wrong-tag fixture must have exactly four full-basis towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 wrong-tag fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 wrong-tag fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(), "Relin2 wrong-tag fixture must have a nonempty key tag");
    Check(keys.secretKey->GetKeyTag() == tensor.GetKeyTag(),
          "Relin2 wrong-tag fixture secret key must initially match the Tensor tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-wrong-tag-immutability-probe") != highMetadata->end(),
          "Relin2 wrong-tag fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-wrong-tag-immutability-probe") != lowMetadata->end(),
          "Relin2 wrong-tag fixture low ciphertext lost its metadata probe");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(), "Relin2 wrong-tag fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        context->EvalMultKeyGen(keys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 wrong-tag fixture must generate exactly one evaluation key");
        const auto wrongTagKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(wrongTagKey != nullptr,
              "Relin2 wrong-tag fixture key must have the relinearization-key subtype");
        Check(wrongTagKey->GetCryptoContext().get() == context.get(),
              "Relin2 wrong-tag fixture key must keep the bound context");
        Check(wrongTagKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 wrong-tag fixture generated key must initially match the Tensor tag");
        Check(!wrongTagKey->GetAVector().empty() && !wrongTagKey->GetBVector().empty(),
              "Relin2 wrong-tag fixture must contain generated key material");

        const std::string wrongKeyTag = tensor.GetKeyTag() + "-wrong";
        wrongTagKey->SetKeyTag(wrongKeyTag);
        Check(generatedRow->first == tensor.GetKeyTag() && wrongTagKey->GetKeyTag() == wrongKeyTag &&
                  wrongTagKey->GetKeyTag() != tensor.GetKeyTag(),
              "Relin2 wrong-tag fixture must change only the evaluation-key pointee tag");

        const auto* cacheIdentityBefore = &evaluationKeys;
        const auto* vectorIdentityBefore = &generatedRow->second;
        const auto keyIdentityBefore = generatedRow->second.front();
        const auto keyContextBefore = wrongTagKey->GetCryptoContext();
        const auto keyTagBefore = wrongTagKey->GetKeyTag();
        const auto keyABefore = wrongTagKey->GetAVector();
        const auto keyBBefore = wrongTagKey->GetBVector();
        const auto tensorBefore = SnapshotTensor(tensor);
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 first evaluation key tag does not match the Tensor key tag",
            "Relin2 wrong-tag first evaluation key");
        CheckTensorUnchanged(tensor, tensorBefore, "Relin2 wrong-tag rejection");
        const auto currentRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBefore && evaluationKeys.size() == 1 &&
                  currentRow != evaluationKeys.end() && &currentRow->second == vectorIdentityBefore &&
                  currentRow->second.size() == 1 &&
                  currentRow->second.front().get() == keyIdentityBefore.get(),
              "Relin2 wrong-tag rejection mutated the evaluation-key cache shape or identity");
        Check(wrongTagKey->GetCryptoContext().get() == keyContextBefore.get(),
              "Relin2 wrong-tag rejection mutated the evaluation-key context");
        Check(wrongTagKey->GetKeyTag() == keyTagBefore,
              "Relin2 wrong-tag rejection mutated the evaluation-key tag");
        Check(wrongTagKey->GetAVector() == keyABefore && wrongTagKey->GetBVector() == keyBBefore,
              "Relin2 wrong-tag rejection mutated the evaluation-key A/B vectors");
    }
    Check(evaluationKeys.empty(), "Relin2 wrong-tag fixture failed to restore the evaluation-key cache");
}

void TestWrongEvaluationKeySubtype() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-wrong-subtype-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 wrong-subtype fixture must have exactly four full-basis towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 wrong-subtype fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 wrong-subtype fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(), "Relin2 wrong-subtype fixture must have a nonempty key tag");
    Check(keys.secretKey->GetKeyTag() == tensor.GetKeyTag(),
          "Relin2 wrong-subtype fixture secret key must match the Tensor tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-wrong-subtype-immutability-probe") != highMetadata->end(),
          "Relin2 wrong-subtype fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-wrong-subtype-immutability-probe") != lowMetadata->end(),
          "Relin2 wrong-subtype fixture low ciphertext lost its metadata probe");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(),
          "Relin2 wrong-subtype fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        context->EvalMultKeyGen(keys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 wrong-subtype positive control must generate exactly one evaluation key");
        const auto correctSubtypeKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(correctSubtypeKey != nullptr,
              "Relin2 wrong-subtype positive control must use the relinearization-key subtype");
        Check(correctSubtypeKey->GetCryptoContext().get() == context.get(),
              "Relin2 wrong-subtype positive control key must keep the bound context");
        Check(correctSubtypeKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 wrong-subtype positive control key must match the Tensor tag");
        Check(!correctSubtypeKey->GetAVector().empty() && !correctSubtypeKey->GetBVector().empty(),
              "Relin2 wrong-subtype positive control must contain generated key material");

        const auto* cacheIdentityBefore = &evaluationKeys;
        const auto* vectorIdentityBefore = &generatedRow->second;
        const auto keyIdentityBefore = generatedRow->second.front();
        const auto keyContextBefore = correctSubtypeKey->GetCryptoContext();
        const auto keyTagBefore = correctSubtypeKey->GetKeyTag();
        const auto keyABefore = correctSubtypeKey->GetAVector();
        const auto keyBBefore = correctSubtypeKey->GetBVector();
        const auto tensorBefore = SnapshotTensor(tensor);
        CheckPassesCurrentScaffoldOrCompletes(
            [&] { (void)module.Relin2(tensor); }, "Relin2 wrong-subtype positive control");
        CheckTensorUnchanged(tensor, tensorBefore, "Relin2 wrong-subtype positive control");
        const auto currentRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBefore && evaluationKeys.size() == 1 &&
                  currentRow != evaluationKeys.end() && &currentRow->second == vectorIdentityBefore &&
                  currentRow->second.size() == 1 &&
                  currentRow->second.front().get() == keyIdentityBefore.get(),
              "Relin2 wrong-subtype positive control mutated the cache shape or identity");
        Check(correctSubtypeKey->GetCryptoContext().get() == keyContextBefore.get(),
              "Relin2 wrong-subtype positive control mutated the key context");
        Check(correctSubtypeKey->GetKeyTag() == keyTagBefore,
              "Relin2 wrong-subtype positive control mutated the key tag");
        Check(correctSubtypeKey->GetAVector() == keyABefore && correctSubtypeKey->GetBVector() == keyBBefore,
              "Relin2 wrong-subtype positive control mutated the key A/B vectors");
    }
    Check(evaluationKeys.empty(),
          "Relin2 wrong-subtype positive control failed to restore the evaluation-key cache");

    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        const auto wrongSubtypeKey = std::make_shared<lbcrypto::EvalKeyImpl<DCRTPoly>>(context);
        wrongSubtypeKey->SetKeyTag(tensor.GetKeyTag());
        const auto insertion = evaluationKeys.emplace(
            tensor.GetKeyTag(), std::vector<lbcrypto::EvalKey<DCRTPoly>>{wrongSubtypeKey});

        Check(insertion.second && evaluationKeys.size() == 1 && insertion.first->second.size() == 1 &&
                  insertion.first->second.front().get() == wrongSubtypeKey.get(),
              "Relin2 wrong-subtype negative control must insert exactly one evaluation key");
        Check(wrongSubtypeKey->GetCryptoContext().get() == context.get(),
              "Relin2 wrong-subtype negative-control key must keep the bound context");
        Check(wrongSubtypeKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 wrong-subtype negative-control key must match the Tensor tag");
        Check(typeid(*wrongSubtypeKey) == typeid(lbcrypto::EvalKeyImpl<DCRTPoly>) &&
                  std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(wrongSubtypeKey) == nullptr,
              "Relin2 wrong-subtype negative control must use the exact base evaluation-key type");

        const auto* cacheIdentityBefore = &evaluationKeys;
        const auto* vectorIdentityBefore = &insertion.first->second;
        const auto keyIdentityBefore = insertion.first->second.front();
        const auto keyContextBefore = wrongSubtypeKey->GetCryptoContext();
        const auto keyTagBefore = wrongSubtypeKey->GetKeyTag();
        const auto tensorBefore = SnapshotTensor(tensor);
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 first evaluation key has the wrong concrete subtype",
            "Relin2 wrong-subtype first evaluation key");
        CheckTensorUnchanged(tensor, tensorBefore, "Relin2 wrong-subtype rejection");
        const auto currentRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBefore && evaluationKeys.size() == 1 &&
                  currentRow != evaluationKeys.end() && &currentRow->second == vectorIdentityBefore &&
                  currentRow->second.size() == 1 &&
                  currentRow->second.front().get() == keyIdentityBefore.get(),
              "Relin2 wrong-subtype rejection mutated the evaluation-key cache shape or identity");
        Check(wrongSubtypeKey->GetCryptoContext().get() == keyContextBefore.get(),
              "Relin2 wrong-subtype rejection mutated the evaluation-key context");
        Check(wrongSubtypeKey->GetKeyTag() == keyTagBefore,
              "Relin2 wrong-subtype rejection mutated the evaluation-key tag");
        Check(typeid(*wrongSubtypeKey) == typeid(lbcrypto::EvalKeyImpl<DCRTPoly>) &&
                  std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(wrongSubtypeKey) == nullptr,
              "Relin2 wrong-subtype rejection changed the concrete evaluation-key type");
    }
    Check(evaluationKeys.empty(),
          "Relin2 wrong-subtype fixture failed to restore the evaluation-key cache");
}

void TestHybridEvaluationKeyALength() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-hybrid-a-length-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 HYBRID A-length fixture must have exactly four full-basis towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 HYBRID A-length fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 HYBRID A-length fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(),
          "Relin2 HYBRID A-length fixture must have a nonempty key tag");
    Check(keys.secretKey->GetKeyTag() == tensor.GetKeyTag(),
          "Relin2 HYBRID A-length fixture secret key must match the Tensor tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-hybrid-a-length-immutability-probe") != highMetadata->end(),
          "Relin2 HYBRID A-length fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-hybrid-a-length-immutability-probe") != lowMetadata->end(),
          "Relin2 HYBRID A-length fixture low ciphertext lost its metadata probe");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr,
          "Relin2 HYBRID A-length fixture must expose CKKS-RNS parameters");
    Check(parameters->GetKeySwitchTechnique() == lbcrypto::HYBRID,
          "Relin2 HYBRID A-length fixture must use HYBRID key switching");
    const auto expectedHybridKeyLength = static_cast<std::size_t>(parameters->GetNumPartQ());
    Check(expectedHybridKeyLength == 2,
          "Relin2 HYBRID A-length fixture must have exactly two Q partitions");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(),
          "Relin2 HYBRID A-length fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        context->EvalMultKeyGen(keys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 HYBRID A-length fixture must generate exactly one evaluation key");
        const auto hybridKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(hybridKey != nullptr,
              "Relin2 HYBRID A-length fixture must use the relinearization-key subtype");
        Check(hybridKey->GetCryptoContext().get() == context.get(),
              "Relin2 HYBRID A-length fixture key must keep the bound context");
        Check(hybridKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 HYBRID A-length fixture key must match the Tensor tag");

        const auto originalA = hybridKey->GetAVector();
        const auto originalB = hybridKey->GetBVector();
        Check(originalA.size() == expectedHybridKeyLength &&
                  originalB.size() == expectedHybridKeyLength,
              "Relin2 HYBRID A-length fixture must start with exact generated A/B lengths");

        auto malformedA = originalA;
        malformedA.pop_back();
        hybridKey->SetAVector(std::move(malformedA));
        Check(hybridKey->GetAVector().size() == 1 &&
                  hybridKey->GetAVector().front() == originalA.front() &&
                  hybridKey->GetBVector() == originalB,
              "Relin2 HYBRID A-length fixture must shorten only the A vector");

        const auto* cacheIdentityBefore = &evaluationKeys;
        const auto* vectorIdentityBefore = &generatedRow->second;
        const auto keyIdentityBefore = generatedRow->second.front();
        const auto keyContextBefore = hybridKey->GetCryptoContext();
        const auto keyTagBefore = hybridKey->GetKeyTag();
        const auto keyABefore = hybridKey->GetAVector();
        const auto keyBBefore = hybridKey->GetBVector();
        const auto tensorBefore = SnapshotTensor(tensor);
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 evaluation key HYBRID A vector length mismatch",
            "Relin2 HYBRID A-vector length");
        CheckTensorUnchanged(tensor, tensorBefore, "Relin2 HYBRID A-length rejection");
        const auto currentRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBefore && evaluationKeys.size() == 1 &&
                  currentRow != evaluationKeys.end() && &currentRow->second == vectorIdentityBefore &&
                  currentRow->second.size() == 1 &&
                  currentRow->second.front().get() == keyIdentityBefore.get(),
              "Relin2 HYBRID A-length rejection mutated the cache shape or identity");
        const auto currentHybridKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(currentRow->second.front());
        Check(currentHybridKey.get() == hybridKey.get(),
              "Relin2 HYBRID A-length rejection changed the concrete key subtype or identity");
        Check(hybridKey->GetCryptoContext().get() == keyContextBefore.get(),
              "Relin2 HYBRID A-length rejection mutated the key context");
        Check(hybridKey->GetKeyTag() == keyTagBefore,
              "Relin2 HYBRID A-length rejection mutated the key tag");
        Check(hybridKey->GetAVector() == keyABefore && hybridKey->GetBVector() == keyBBefore,
              "Relin2 HYBRID A-length rejection mutated the malformed A or valid B vector");
    }
    Check(evaluationKeys.empty(),
          "Relin2 HYBRID A-length fixture failed to restore the initially empty cache");
}

void TestHybridEvaluationKeyBLength() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-hybrid-b-length-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 HYBRID B-length fixture must have exactly four full-basis towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 HYBRID B-length fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 HYBRID B-length fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(),
          "Relin2 HYBRID B-length fixture must have a nonempty key tag");
    Check(keys.secretKey->GetKeyTag() == tensor.GetKeyTag(),
          "Relin2 HYBRID B-length fixture secret key must match the Tensor tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-hybrid-b-length-immutability-probe") != highMetadata->end(),
          "Relin2 HYBRID B-length fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-hybrid-b-length-immutability-probe") != lowMetadata->end(),
          "Relin2 HYBRID B-length fixture low ciphertext lost its metadata probe");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr,
          "Relin2 HYBRID B-length fixture must expose CKKS-RNS parameters");
    Check(parameters->GetKeySwitchTechnique() == lbcrypto::HYBRID,
          "Relin2 HYBRID B-length fixture must use HYBRID key switching");
    const auto expectedHybridKeyLength = static_cast<std::size_t>(parameters->GetNumPartQ());
    Check(expectedHybridKeyLength == 2,
          "Relin2 HYBRID B-length fixture must have exactly two Q partitions");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(),
          "Relin2 HYBRID B-length fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        context->EvalMultKeyGen(keys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 HYBRID B-length fixture must generate exactly one evaluation key");
        const auto hybridKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(hybridKey != nullptr,
              "Relin2 HYBRID B-length fixture must use the relinearization-key subtype");
        Check(hybridKey->GetCryptoContext().get() == context.get(),
              "Relin2 HYBRID B-length fixture key must keep the bound context");
        Check(hybridKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 HYBRID B-length fixture key must match the Tensor tag");

        const auto originalA = hybridKey->GetAVector();
        const auto originalB = hybridKey->GetBVector();
        Check(originalA.size() == expectedHybridKeyLength &&
                  originalB.size() == expectedHybridKeyLength,
              "Relin2 HYBRID B-length fixture must start with exact generated A/B lengths");

        auto malformedB = originalB;
        malformedB.pop_back();
        hybridKey->SetBVector(std::move(malformedB));
        Check(hybridKey->GetAVector() == originalA &&
                  hybridKey->GetBVector().size() == 1 &&
                  hybridKey->GetBVector().front() == originalB.front(),
              "Relin2 HYBRID B-length fixture must shorten only the B vector");

        const auto* cacheIdentityBefore = &evaluationKeys;
        const auto* vectorIdentityBefore = &generatedRow->second;
        const auto keyIdentityBefore = generatedRow->second.front();
        const auto keyContextBefore = hybridKey->GetCryptoContext();
        const auto keyTagBefore = hybridKey->GetKeyTag();
        const auto keyABefore = hybridKey->GetAVector();
        const auto keyBBefore = hybridKey->GetBVector();
        const auto tensorBefore = SnapshotTensor(tensor);
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 evaluation key HYBRID B vector length mismatch",
            "Relin2 HYBRID B-vector length");
        CheckTensorUnchanged(tensor, tensorBefore, "Relin2 HYBRID B-length rejection");
        const auto currentRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBefore && evaluationKeys.size() == 1 &&
                  currentRow != evaluationKeys.end() && &currentRow->second == vectorIdentityBefore &&
                  currentRow->second.size() == 1 &&
                  currentRow->second.front().get() == keyIdentityBefore.get(),
              "Relin2 HYBRID B-length rejection mutated the cache shape or identity");
        const auto currentHybridKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(currentRow->second.front());
        Check(currentHybridKey.get() == hybridKey.get(),
              "Relin2 HYBRID B-length rejection changed the concrete key subtype or identity");
        Check(hybridKey->GetCryptoContext().get() == keyContextBefore.get(),
              "Relin2 HYBRID B-length rejection mutated the key context");
        Check(hybridKey->GetKeyTag() == keyTagBefore,
              "Relin2 HYBRID B-length rejection mutated the key tag");
        Check(hybridKey->GetAVector() == keyABefore && hybridKey->GetBVector() == keyBBefore,
              "Relin2 HYBRID B-length rejection mutated the valid A or malformed B vector");
    }
    Check(evaluationKeys.empty(),
          "Relin2 HYBRID B-length fixture failed to restore the initially empty cache");
}

void TestHybridEvaluationKeyEntryBasis() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-hybrid-entry-basis-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 HYBRID entry-basis fixture must have exactly four full-basis towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 HYBRID entry-basis fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 HYBRID entry-basis fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(),
          "Relin2 HYBRID entry-basis fixture must have a nonempty key tag");
    Check(keys.secretKey->GetKeyTag() == tensor.GetKeyTag(),
          "Relin2 HYBRID entry-basis fixture secret key must match the Tensor tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-hybrid-entry-basis-immutability-probe") != highMetadata->end(),
          "Relin2 HYBRID entry-basis fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-hybrid-entry-basis-immutability-probe") != lowMetadata->end(),
          "Relin2 HYBRID entry-basis fixture low ciphertext lost its metadata probe");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr,
          "Relin2 HYBRID entry-basis fixture must expose CKKS-RNS parameters");
    Check(parameters->GetKeySwitchTechnique() == lbcrypto::HYBRID,
          "Relin2 HYBRID entry-basis fixture must use HYBRID key switching");
    const auto expectedHybridKeyLength = static_cast<std::size_t>(parameters->GetNumPartQ());
    Check(expectedHybridKeyLength == 2,
          "Relin2 HYBRID entry-basis fixture must have exactly two Q partitions");
    const auto expectedBasis = parameters->GetParamsQP();
    Check(expectedBasis != nullptr && expectedBasis->GetParams().size() >= 2,
          "Relin2 HYBRID entry-basis fixture must expose a nontrivial complete ParamsQP basis");
    Check(expectedBasis->GetParams()[0] != nullptr && expectedBasis->GetParams()[1] != nullptr &&
              expectedBasis->GetParams()[0]->GetModulus() != expectedBasis->GetParams()[1]->GetModulus(),
          "Relin2 HYBRID entry-basis fixture must have distinguishable first two ParamsQP towers");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(),
          "Relin2 HYBRID entry-basis fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        context->EvalMultKeyGen(keys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 HYBRID entry-basis fixture must generate exactly one evaluation key");
        const auto hybridKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(hybridKey != nullptr,
              "Relin2 HYBRID entry-basis fixture must use the relinearization-key subtype");
        Check(hybridKey->GetCryptoContext().get() == context.get(),
              "Relin2 HYBRID entry-basis fixture key must keep the bound context");
        Check(hybridKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 HYBRID entry-basis fixture key must match the Tensor tag");

        const auto originalA = hybridKey->GetAVector();
        const auto originalB = hybridKey->GetBVector();
        Check(originalA.size() == expectedHybridKeyLength &&
                  originalB.size() == expectedHybridKeyLength,
              "Relin2 HYBRID entry-basis fixture must start with exact generated A/B lengths");
        for (std::size_t index = 0; index < originalA.size(); ++index) {
            Check(originalA[index].GetFormat() == Format::EVALUATION,
                  "Relin2 HYBRID entry-basis fixture generated a non-Evaluation A entry");
            CheckKeyPolynomialBasis(originalA[index], expectedBasis,
                                    "Relin2 HYBRID generated A entry " + std::to_string(index));
        }
        for (std::size_t index = 0; index < originalB.size(); ++index) {
            Check(originalB[index].GetFormat() == Format::EVALUATION,
                  "Relin2 HYBRID entry-basis fixture generated a non-Evaluation B entry");
            CheckKeyPolynomialBasis(originalB[index], expectedBasis,
                                    "Relin2 HYBRID generated B entry " + std::to_string(index));
        }

        auto equivalentB = originalB;
        equivalentB.back() = CloneKeyPolynomialWithIndependentParams(
            originalB.back(), "Relin2 HYBRID equivalent-pointer positive control");
        Check(equivalentB.back().GetParams().get() != originalB.back().GetParams().get() &&
                  equivalentB.back().GetParams().get() != expectedBasis.get(),
              "Relin2 HYBRID entry-basis positive control must use an independent parameter object");
        for (std::size_t index = 0; index < equivalentB.back().GetAllElements().size(); ++index) {
            Check(equivalentB.back().GetAllElements()[index].GetParams().get() !=
                      originalB.back().GetAllElements()[index].GetParams().get(),
                  "Relin2 HYBRID entry-basis positive control must use independent tower parameters");
        }
        Check(*equivalentB.back().GetParams() == *expectedBasis &&
                  equivalentB.back() == originalB.back() &&
                  equivalentB.back().GetFormat() == Format::EVALUATION,
              "Relin2 HYBRID entry-basis positive control must remain semantically equivalent");
        CheckKeyPolynomialBasis(equivalentB.back(), expectedBasis,
                                "Relin2 HYBRID equivalent-pointer positive control");
        hybridKey->SetBVector(std::move(equivalentB));

        const auto* cacheIdentityBeforePositive = &evaluationKeys;
        const auto* vectorIdentityBeforePositive = &generatedRow->second;
        const auto keyIdentityBeforePositive = generatedRow->second.front();
        const auto keyContextBeforePositive = hybridKey->GetCryptoContext();
        const auto keyTagBeforePositive = hybridKey->GetKeyTag();
        const auto tensorBeforePositive = SnapshotTensor(tensor);
        const auto keyABeforePositive = SnapshotKeyVector(hybridKey->GetAVector(), "Relin2 HYBRID positive A");
        const auto keyBBeforePositive = SnapshotKeyVector(hybridKey->GetBVector(), "Relin2 HYBRID positive B");
        CheckPassesCurrentScaffoldOrCompletes(
            [&] { (void)module.Relin2(tensor); },
            "Relin2 HYBRID equivalent-pointer entry basis");
        CheckTensorUnchanged(tensor, tensorBeforePositive,
                             "Relin2 HYBRID equivalent-pointer positive control");
        CheckKeyVectorUnchanged(hybridKey->GetAVector(), keyABeforePositive,
                                "Relin2 HYBRID equivalent-pointer positive A");
        CheckKeyVectorUnchanged(hybridKey->GetBVector(), keyBBeforePositive,
                                "Relin2 HYBRID equivalent-pointer positive B");
        const auto currentPositiveRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBeforePositive && evaluationKeys.size() == 1 &&
                  currentPositiveRow != evaluationKeys.end() &&
                  &currentPositiveRow->second == vectorIdentityBeforePositive &&
                  currentPositiveRow->second.size() == 1 &&
                  currentPositiveRow->second.front().get() == keyIdentityBeforePositive.get(),
              "Relin2 HYBRID equivalent-pointer positive control mutated the cache shape or identity");
        Check(hybridKey->GetCryptoContext().get() == keyContextBeforePositive.get(),
              "Relin2 HYBRID equivalent-pointer positive control mutated the key context");
        Check(hybridKey->GetKeyTag() == keyTagBeforePositive,
              "Relin2 HYBRID equivalent-pointer positive control mutated the key tag");

        auto malformedB = originalB;
        auto swappedTowers = malformedB.back().GetAllElements();
        Check(swappedTowers.size() == expectedBasis->GetParams().size() && swappedTowers.size() >= 2,
              "Relin2 HYBRID entry-basis negative control must start from complete ParamsQP towers");
        std::swap(swappedTowers[0], swappedTowers[1]);
        malformedB.back() = DCRTPoly(swappedTowers);
        const auto wrongBasis = malformedB.back().GetParams();
        Check(wrongBasis != nullptr && !(*wrongBasis == *expectedBasis) &&
                  malformedB.back().GetFormat() == Format::EVALUATION &&
                  malformedB.back().GetAllElements().size() == expectedBasis->GetParams().size() &&
                  malformedB.back().GetCyclotomicOrder() == expectedBasis->GetCyclotomicOrder(),
              "Relin2 HYBRID entry-basis negative control must preserve shape and change only tower order");
        Check(wrongBasis->GetParams()[0]->GetModulus() == expectedBasis->GetParams()[1]->GetModulus() &&
                  wrongBasis->GetParams()[0]->GetRootOfUnity() == expectedBasis->GetParams()[1]->GetRootOfUnity() &&
                  wrongBasis->GetParams()[1]->GetModulus() == expectedBasis->GetParams()[0]->GetModulus() &&
                  wrongBasis->GetParams()[1]->GetRootOfUnity() == expectedBasis->GetParams()[0]->GetRootOfUnity(),
              "Relin2 HYBRID entry-basis negative control must swap the first two declared towers exactly");
        Check(malformedB.back().GetAllElements()[0].GetModulus() ==
                  expectedBasis->GetParams()[1]->GetModulus() &&
                  malformedB.back().GetAllElements()[0].GetRootOfUnity() ==
                  expectedBasis->GetParams()[1]->GetRootOfUnity() &&
                  malformedB.back().GetAllElements()[1].GetModulus() ==
                  expectedBasis->GetParams()[0]->GetModulus() &&
                  malformedB.back().GetAllElements()[1].GetRootOfUnity() ==
                  expectedBasis->GetParams()[0]->GetRootOfUnity(),
              "Relin2 HYBRID entry-basis negative control must swap the first two actual towers exactly");
        for (std::size_t index = 2; index < expectedBasis->GetParams().size(); ++index) {
            Check(wrongBasis->GetParams()[index]->GetCyclotomicOrder() ==
                      expectedBasis->GetParams()[index]->GetCyclotomicOrder() &&
                      wrongBasis->GetParams()[index]->GetModulus() ==
                      expectedBasis->GetParams()[index]->GetModulus() &&
                      wrongBasis->GetParams()[index]->GetRootOfUnity() ==
                      expectedBasis->GetParams()[index]->GetRootOfUnity(),
                  "Relin2 HYBRID entry-basis negative control changed an unswapped declared tower");
            Check(malformedB.back().GetAllElements()[index].GetCyclotomicOrder() ==
                      originalB.back().GetAllElements()[index].GetCyclotomicOrder() &&
                      malformedB.back().GetAllElements()[index].GetModulus() ==
                      originalB.back().GetAllElements()[index].GetModulus() &&
                      malformedB.back().GetAllElements()[index].GetRootOfUnity() ==
                      originalB.back().GetAllElements()[index].GetRootOfUnity(),
                  "Relin2 HYBRID entry-basis negative control changed an unswapped actual tower");
        }
        hybridKey->SetBVector(std::move(malformedB));
        Check(hybridKey->GetAVector() == originalA &&
                  hybridKey->GetBVector().size() == originalB.size() &&
                  hybridKey->GetBVector().front() == originalB.front() &&
                  hybridKey->GetBVector().back().GetParams().get() == wrongBasis.get(),
              "Relin2 HYBRID entry-basis negative control must alter only the last B entry basis");

        const auto* cacheIdentityBeforeNegative = &evaluationKeys;
        const auto* vectorIdentityBeforeNegative = &generatedRow->second;
        const auto keyIdentityBeforeNegative = generatedRow->second.front();
        const auto keyContextBeforeNegative = hybridKey->GetCryptoContext();
        const auto keyTagBeforeNegative = hybridKey->GetKeyTag();
        const auto tensorBeforeNegative = SnapshotTensor(tensor);
        const auto keyABeforeNegative = SnapshotKeyVector(hybridKey->GetAVector(), "Relin2 HYBRID negative A");
        const auto keyBBeforeNegative = SnapshotKeyVector(hybridKey->GetBVector(), "Relin2 HYBRID negative B");
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 evaluation key HYBRID entry basis mismatch",
            "Relin2 HYBRID evaluation-key entry basis");
        CheckTensorUnchanged(tensor, tensorBeforeNegative,
                             "Relin2 HYBRID entry-basis rejection");
        CheckKeyVectorUnchanged(hybridKey->GetAVector(), keyABeforeNegative,
                                "Relin2 HYBRID entry-basis rejection A");
        CheckKeyVectorUnchanged(hybridKey->GetBVector(), keyBBeforeNegative,
                                "Relin2 HYBRID entry-basis rejection B");
        const auto currentNegativeRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBeforeNegative && evaluationKeys.size() == 1 &&
                  currentNegativeRow != evaluationKeys.end() &&
                  &currentNegativeRow->second == vectorIdentityBeforeNegative &&
                  currentNegativeRow->second.size() == 1 &&
                  currentNegativeRow->second.front().get() == keyIdentityBeforeNegative.get(),
              "Relin2 HYBRID entry-basis rejection mutated the cache shape or identity");
        Check(hybridKey->GetCryptoContext().get() == keyContextBeforeNegative.get(),
              "Relin2 HYBRID entry-basis rejection mutated the key context");
        Check(hybridKey->GetKeyTag() == keyTagBeforeNegative,
              "Relin2 HYBRID entry-basis rejection mutated the key tag");
    }
    Check(evaluationKeys.empty(),
          "Relin2 HYBRID entry-basis fixture failed to restore the initially empty cache");
}

void TestHybridEvaluationKeyEntryFormat() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-hybrid-entry-format-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 HYBRID entry-format fixture must have exactly four full-basis towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 HYBRID entry-format fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 HYBRID entry-format fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(),
          "Relin2 HYBRID entry-format fixture must have a nonempty key tag");
    Check(keys.secretKey->GetKeyTag() == tensor.GetKeyTag(),
          "Relin2 HYBRID entry-format fixture secret key must match the Tensor tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-hybrid-entry-format-immutability-probe") != highMetadata->end(),
          "Relin2 HYBRID entry-format fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-hybrid-entry-format-immutability-probe") != lowMetadata->end(),
          "Relin2 HYBRID entry-format fixture low ciphertext lost its metadata probe");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr,
          "Relin2 HYBRID entry-format fixture must expose CKKS-RNS parameters");
    Check(parameters->GetKeySwitchTechnique() == lbcrypto::HYBRID,
          "Relin2 HYBRID entry-format fixture must use HYBRID key switching");
    const auto expectedHybridKeyLength = static_cast<std::size_t>(parameters->GetNumPartQ());
    Check(expectedHybridKeyLength == 2,
          "Relin2 HYBRID entry-format fixture must have exactly two Q partitions");
    const auto expectedBasis = parameters->GetParamsQP();
    Check(expectedBasis != nullptr && !expectedBasis->GetParams().empty(),
          "Relin2 HYBRID entry-format fixture must expose the complete ParamsQP basis");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(),
          "Relin2 HYBRID entry-format fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        context->EvalMultKeyGen(keys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 HYBRID entry-format fixture must generate exactly one evaluation key");
        const auto hybridKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(hybridKey != nullptr,
              "Relin2 HYBRID entry-format fixture must use the relinearization-key subtype");
        Check(hybridKey->GetCryptoContext().get() == context.get(),
              "Relin2 HYBRID entry-format fixture key must keep the bound context");
        Check(hybridKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 HYBRID entry-format fixture key must match the Tensor tag");

        const auto originalA = hybridKey->GetAVector();
        const auto originalB = hybridKey->GetBVector();
        Check(originalA.size() == expectedHybridKeyLength &&
                  originalB.size() == expectedHybridKeyLength,
              "Relin2 HYBRID entry-format fixture must start with exact generated A/B lengths");
        for (std::size_t index = 0; index < originalA.size(); ++index) {
            Check(originalA[index].GetFormat() == Format::EVALUATION,
                  "Relin2 HYBRID entry-format fixture generated a non-Evaluation A entry");
            CheckKeyPolynomialBasis(originalA[index], expectedBasis,
                                    "Relin2 HYBRID entry-format generated A entry " +
                                        std::to_string(index));
            for (const auto& tower : originalA[index].GetAllElements()) {
                Check(tower.GetFormat() == Format::EVALUATION,
                      "Relin2 HYBRID entry-format fixture generated a non-Evaluation A tower");
            }
        }
        for (std::size_t index = 0; index < originalB.size(); ++index) {
            Check(originalB[index].GetFormat() == Format::EVALUATION,
                  "Relin2 HYBRID entry-format fixture generated a non-Evaluation B entry");
            CheckKeyPolynomialBasis(originalB[index], expectedBasis,
                                    "Relin2 HYBRID entry-format generated B entry " +
                                        std::to_string(index));
            for (const auto& tower : originalB[index].GetAllElements()) {
                Check(tower.GetFormat() == Format::EVALUATION,
                      "Relin2 HYBRID entry-format fixture generated a non-Evaluation B tower");
            }
        }

        const auto* cacheIdentityBeforePositive = &evaluationKeys;
        const auto* vectorIdentityBeforePositive = &generatedRow->second;
        const auto keyIdentityBeforePositive = generatedRow->second.front();
        const auto keyContextBeforePositive = hybridKey->GetCryptoContext();
        const auto keyTagBeforePositive = hybridKey->GetKeyTag();
        const auto tensorBeforePositive = SnapshotTensor(tensor);
        const auto keyABeforePositive =
            SnapshotKeyVector(hybridKey->GetAVector(), "Relin2 HYBRID entry-format positive A");
        const auto keyBBeforePositive =
            SnapshotKeyVector(hybridKey->GetBVector(), "Relin2 HYBRID entry-format positive B");
        CheckPassesCurrentScaffoldOrCompletes(
            [&] { (void)module.Relin2(tensor); },
            "Relin2 HYBRID valid entry format");
        CheckTensorUnchanged(tensor, tensorBeforePositive,
                             "Relin2 HYBRID entry-format positive control");
        CheckKeyVectorUnchanged(hybridKey->GetAVector(), keyABeforePositive,
                                "Relin2 HYBRID entry-format positive A");
        CheckKeyVectorUnchanged(hybridKey->GetBVector(), keyBBeforePositive,
                                "Relin2 HYBRID entry-format positive B");
        const auto currentPositiveRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBeforePositive && evaluationKeys.size() == 1 &&
                  currentPositiveRow != evaluationKeys.end() &&
                  &currentPositiveRow->second == vectorIdentityBeforePositive &&
                  currentPositiveRow->second.size() == 1 &&
                  currentPositiveRow->second.front().get() == keyIdentityBeforePositive.get(),
              "Relin2 HYBRID entry-format positive control mutated the cache shape or identity");
        Check(hybridKey->GetCryptoContext().get() == keyContextBeforePositive.get(),
              "Relin2 HYBRID entry-format positive control mutated the key context");
        Check(hybridKey->GetKeyTag() == keyTagBeforePositive,
              "Relin2 HYBRID entry-format positive control mutated the key tag");

        auto malformedA = originalA;
        malformedA.front().SetFormat(Format::COEFFICIENT);
        Check(malformedA.size() == expectedHybridKeyLength &&
                  malformedA.front().GetFormat() == Format::COEFFICIENT &&
                  malformedA.front().GetParams().get() == originalA.front().GetParams().get(),
              "Relin2 HYBRID entry-format negative control must change only the first A format");
        CheckKeyPolynomialBasis(malformedA.front(), expectedBasis,
                                "Relin2 HYBRID Coefficient-format negative control");
        for (const auto& tower : malformedA.front().GetAllElements()) {
            Check(tower.GetFormat() == Format::COEFFICIENT,
                  "Relin2 HYBRID entry-format negative control left an Evaluation tower");
        }
        for (std::size_t index = 1; index < malformedA.size(); ++index) {
            Check(malformedA[index] == originalA[index] &&
                      malformedA[index].GetFormat() == Format::EVALUATION,
                  "Relin2 HYBRID entry-format negative control changed another A entry");
        }
        hybridKey->SetAVector(std::move(malformedA));
        Check(hybridKey->GetAVector().size() == expectedHybridKeyLength &&
                  hybridKey->GetBVector() == originalB &&
                  hybridKey->GetAVector().front().GetFormat() == Format::COEFFICIENT,
              "Relin2 HYBRID entry-format negative control changed key shape or B entries");

        const auto* cacheIdentityBeforeNegative = &evaluationKeys;
        const auto* vectorIdentityBeforeNegative = &generatedRow->second;
        const auto keyIdentityBeforeNegative = generatedRow->second.front();
        const auto keyContextBeforeNegative = hybridKey->GetCryptoContext();
        const auto keyTagBeforeNegative = hybridKey->GetKeyTag();
        const auto tensorBeforeNegative = SnapshotTensor(tensor);
        const auto keyABeforeNegative =
            SnapshotKeyVector(hybridKey->GetAVector(), "Relin2 HYBRID entry-format negative A");
        const auto keyBBeforeNegative =
            SnapshotKeyVector(hybridKey->GetBVector(), "Relin2 HYBRID entry-format negative B");
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 evaluation key HYBRID entry must be in evaluation format",
            "Relin2 HYBRID evaluation-key entry format");
        CheckTensorUnchanged(tensor, tensorBeforeNegative,
                             "Relin2 HYBRID entry-format rejection");
        CheckKeyVectorUnchanged(hybridKey->GetAVector(), keyABeforeNegative,
                                "Relin2 HYBRID entry-format rejection A");
        CheckKeyVectorUnchanged(hybridKey->GetBVector(), keyBBeforeNegative,
                                "Relin2 HYBRID entry-format rejection B");
        const auto currentNegativeRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBeforeNegative && evaluationKeys.size() == 1 &&
                  currentNegativeRow != evaluationKeys.end() &&
                  &currentNegativeRow->second == vectorIdentityBeforeNegative &&
                  currentNegativeRow->second.size() == 1 &&
                  currentNegativeRow->second.front().get() == keyIdentityBeforeNegative.get(),
              "Relin2 HYBRID entry-format rejection mutated the cache shape or identity");
        Check(hybridKey->GetCryptoContext().get() == keyContextBeforeNegative.get(),
              "Relin2 HYBRID entry-format rejection mutated the key context");
        Check(hybridKey->GetKeyTag() == keyTagBeforeNegative,
              "Relin2 HYBRID entry-format rejection mutated the key tag");
    }
    Check(evaluationKeys.empty(),
          "Relin2 HYBRID entry-format fixture failed to restore the initially empty cache");
}

void TestBVEvaluationKeyZeroDigitALength() {
    auto context = MakeContext(3, 8, lbcrypto::BV, 0);
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-bv-zero-digit-a-length-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 BV zero-digit A-length fixture must have exactly four full-Q towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 BV zero-digit A-length fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 BV zero-digit A-length fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(),
          "Relin2 BV zero-digit A-length fixture must have a nonempty key tag");
    Check(keys.secretKey->GetKeyTag() == tensor.GetKeyTag(),
          "Relin2 BV zero-digit A-length fixture secret key must match the Tensor tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-bv-zero-digit-a-length-immutability-probe") != highMetadata->end(),
          "Relin2 BV zero-digit A-length fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-bv-zero-digit-a-length-immutability-probe") != lowMetadata->end(),
          "Relin2 BV zero-digit A-length fixture low ciphertext lost its metadata probe");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr,
          "Relin2 BV zero-digit A-length fixture must expose CKKS-RNS parameters");
    Check(parameters->GetKeySwitchTechnique() == lbcrypto::BV,
          "Relin2 BV zero-digit A-length fixture must use BV key switching");
    Check(parameters->GetDigitSize() == 0,
          "Relin2 BV zero-digit A-length fixture must use digit size zero");
    const auto expectedBVKeyLength = parameters->GetElementParams()->GetParams().size();
    Check(expectedBVKeyLength == 4,
          "Relin2 BV zero-digit A-length fixture must have four full-Q digits");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(),
          "Relin2 BV zero-digit A-length fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        context->EvalMultKeyGen(keys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 BV zero-digit A-length fixture must generate exactly one evaluation key");
        const auto bvKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(bvKey != nullptr,
              "Relin2 BV zero-digit A-length fixture must use the relinearization-key subtype");
        Check(bvKey->GetCryptoContext().get() == context.get(),
              "Relin2 BV zero-digit A-length fixture key must keep the bound context");
        Check(bvKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 BV zero-digit A-length fixture key must match the Tensor tag");

        const auto originalA = bvKey->GetAVector();
        const auto originalB = bvKey->GetBVector();
        Check(originalA.size() == expectedBVKeyLength && originalB.size() == expectedBVKeyLength,
              "Relin2 BV zero-digit A-length fixture must start with exact generated A/B lengths");

        const auto* cacheIdentityPositive = &evaluationKeys;
        const auto* vectorIdentityPositive = &generatedRow->second;
        const auto keyIdentityPositive = generatedRow->second.front();
        const auto keyContextPositive = bvKey->GetCryptoContext();
        const auto keyTagPositive = bvKey->GetKeyTag();
        const auto keyAPositive = SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV zero-digit positive A");
        const auto keyBPositive = SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV zero-digit positive B");
        const auto tensorPositive = SnapshotTensor(tensor);
        CheckPassesCurrentScaffoldOrCompletes(
            [&] { (void)module.Relin2(tensor); },
            "Relin2 BV zero-digit valid A/B lengths");
        CheckTensorUnchanged(tensor, tensorPositive,
                             "Relin2 BV zero-digit A-length positive control");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyAPositive,
                                "Relin2 BV zero-digit A-length positive A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBPositive,
                                "Relin2 BV zero-digit A-length positive B");
        const auto positiveRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityPositive && evaluationKeys.size() == 1 &&
                  positiveRow != evaluationKeys.end() && &positiveRow->second == vectorIdentityPositive &&
                  positiveRow->second.size() == 1 &&
                  positiveRow->second.front().get() == keyIdentityPositive.get(),
              "Relin2 BV zero-digit A-length positive control mutated the cache shape or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextPositive.get(),
              "Relin2 BV zero-digit A-length positive control mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagPositive,
              "Relin2 BV zero-digit A-length positive control mutated the key tag");

        auto malformedA = originalA;
        malformedA.pop_back();
        bvKey->SetAVector(std::move(malformedA));
        Check(bvKey->GetAVector().size() + 1 == expectedBVKeyLength &&
                  bvKey->GetAVector().front() == originalA.front() &&
                  bvKey->GetAVector().back() == originalA[expectedBVKeyLength - 2] &&
                  bvKey->GetBVector() == originalB,
              "Relin2 BV zero-digit A-length fixture must shorten only the A vector");

        const auto* cacheIdentityBefore = &evaluationKeys;
        const auto* vectorIdentityBefore = &generatedRow->second;
        const auto keyIdentityBefore = generatedRow->second.front();
        const auto keyContextBefore = bvKey->GetCryptoContext();
        const auto keyTagBefore = bvKey->GetKeyTag();
        const auto keyABefore = SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV zero-digit malformed A");
        const auto keyBBefore = SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV zero-digit valid B");
        const auto tensorBefore = SnapshotTensor(tensor);
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 evaluation key BV A vector length mismatch",
            "Relin2 BV zero-digit A-vector length");
        CheckTensorUnchanged(tensor, tensorBefore, "Relin2 BV zero-digit A-length rejection");
        const auto currentRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBefore && evaluationKeys.size() == 1 &&
                  currentRow != evaluationKeys.end() && &currentRow->second == vectorIdentityBefore &&
                  currentRow->second.size() == 1 &&
                  currentRow->second.front().get() == keyIdentityBefore.get(),
              "Relin2 BV zero-digit A-length rejection mutated the cache shape or identity");
        const auto currentBVKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(currentRow->second.front());
        Check(currentBVKey.get() == bvKey.get(),
              "Relin2 BV zero-digit A-length rejection changed the concrete key subtype or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextBefore.get(),
              "Relin2 BV zero-digit A-length rejection mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagBefore,
              "Relin2 BV zero-digit A-length rejection mutated the key tag");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyABefore,
                                "Relin2 BV zero-digit A-length rejection A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBBefore,
                                "Relin2 BV zero-digit A-length rejection B");
    }
    Check(evaluationKeys.empty(),
          "Relin2 BV zero-digit A-length fixture failed to restore the initially empty cache");
}

void TestBVEvaluationKeyZeroDigitBLength() {
    auto context = MakeContext(3, 8, lbcrypto::BV, 0);
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-bv-zero-digit-b-length-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 BV zero-digit B-length fixture must have exactly four full-Q towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 BV zero-digit B-length fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 BV zero-digit B-length fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(),
          "Relin2 BV zero-digit B-length fixture must have a nonempty key tag");
    Check(keys.secretKey->GetKeyTag() == tensor.GetKeyTag(),
          "Relin2 BV zero-digit B-length fixture secret key must match the Tensor tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-bv-zero-digit-b-length-immutability-probe") != highMetadata->end(),
          "Relin2 BV zero-digit B-length fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-bv-zero-digit-b-length-immutability-probe") != lowMetadata->end(),
          "Relin2 BV zero-digit B-length fixture low ciphertext lost its metadata probe");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr,
          "Relin2 BV zero-digit B-length fixture must expose CKKS-RNS parameters");
    Check(parameters->GetKeySwitchTechnique() == lbcrypto::BV,
          "Relin2 BV zero-digit B-length fixture must use BV key switching");
    Check(parameters->GetDigitSize() == 0,
          "Relin2 BV zero-digit B-length fixture must use digit size zero");
    const auto expectedBVKeyLength = parameters->GetElementParams()->GetParams().size();
    Check(expectedBVKeyLength == 4,
          "Relin2 BV zero-digit B-length fixture must have four full-Q digits");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(),
          "Relin2 BV zero-digit B-length fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        context->EvalMultKeyGen(keys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 BV zero-digit B-length fixture must generate exactly one evaluation key");
        const auto bvKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(bvKey != nullptr,
              "Relin2 BV zero-digit B-length fixture must use the relinearization-key subtype");
        Check(bvKey->GetCryptoContext().get() == context.get(),
              "Relin2 BV zero-digit B-length fixture key must keep the bound context");
        Check(bvKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 BV zero-digit B-length fixture key must match the Tensor tag");

        const auto originalA = bvKey->GetAVector();
        const auto originalB = bvKey->GetBVector();
        Check(originalA.size() == expectedBVKeyLength && originalB.size() == expectedBVKeyLength,
              "Relin2 BV zero-digit B-length fixture must start with exact generated A/B lengths");

        const auto* cacheIdentityPositive = &evaluationKeys;
        const auto* vectorIdentityPositive = &generatedRow->second;
        const auto keyIdentityPositive = generatedRow->second.front();
        const auto keyContextPositive = bvKey->GetCryptoContext();
        const auto keyTagPositive = bvKey->GetKeyTag();
        const auto keyAPositive = SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV zero-digit positive A");
        const auto keyBPositive = SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV zero-digit positive B");
        const auto tensorPositive = SnapshotTensor(tensor);
        CheckPassesCurrentScaffoldOrCompletes(
            [&] { (void)module.Relin2(tensor); },
            "Relin2 BV zero-digit valid A/B lengths");
        CheckTensorUnchanged(tensor, tensorPositive,
                             "Relin2 BV zero-digit B-length positive control");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyAPositive,
                                "Relin2 BV zero-digit B-length positive A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBPositive,
                                "Relin2 BV zero-digit B-length positive B");
        const auto positiveRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityPositive && evaluationKeys.size() == 1 &&
                  positiveRow != evaluationKeys.end() && &positiveRow->second == vectorIdentityPositive &&
                  positiveRow->second.size() == 1 &&
                  positiveRow->second.front().get() == keyIdentityPositive.get(),
              "Relin2 BV zero-digit B-length positive control mutated the cache shape or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextPositive.get(),
              "Relin2 BV zero-digit B-length positive control mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagPositive,
              "Relin2 BV zero-digit B-length positive control mutated the key tag");

        auto malformedB = originalB;
        malformedB.pop_back();
        bvKey->SetBVector(std::move(malformedB));
        Check(bvKey->GetBVector().size() + 1 == expectedBVKeyLength &&
                  bvKey->GetBVector().front() == originalB.front() &&
                  bvKey->GetBVector().back() == originalB[expectedBVKeyLength - 2] &&
                  bvKey->GetAVector() == originalA,
              "Relin2 BV zero-digit B-length fixture must shorten only the B vector");

        const auto* cacheIdentityBefore = &evaluationKeys;
        const auto* vectorIdentityBefore = &generatedRow->second;
        const auto keyIdentityBefore = generatedRow->second.front();
        const auto keyContextBefore = bvKey->GetCryptoContext();
        const auto keyTagBefore = bvKey->GetKeyTag();
        const auto keyABefore = SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV zero-digit valid A");
        const auto keyBBefore = SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV zero-digit malformed B");
        const auto tensorBefore = SnapshotTensor(tensor);
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 evaluation key BV B vector length mismatch",
            "Relin2 BV zero-digit B-vector length");
        CheckTensorUnchanged(tensor, tensorBefore, "Relin2 BV zero-digit B-length rejection");
        const auto currentRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBefore && evaluationKeys.size() == 1 &&
                  currentRow != evaluationKeys.end() && &currentRow->second == vectorIdentityBefore &&
                  currentRow->second.size() == 1 &&
                  currentRow->second.front().get() == keyIdentityBefore.get(),
              "Relin2 BV zero-digit B-length rejection mutated the cache shape or identity");
        const auto currentBVKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(currentRow->second.front());
        Check(currentBVKey.get() == bvKey.get(),
              "Relin2 BV zero-digit B-length rejection changed the concrete key subtype or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextBefore.get(),
              "Relin2 BV zero-digit B-length rejection mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagBefore,
              "Relin2 BV zero-digit B-length rejection mutated the key tag");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyABefore,
                                "Relin2 BV zero-digit B-length rejection A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBBefore,
                                "Relin2 BV zero-digit B-length rejection B");
    }
    Check(evaluationKeys.empty(),
          "Relin2 BV zero-digit B-length fixture failed to restore the initially empty cache");
}

void TestBVEvaluationKeyNonzeroDigitALength() {
    auto context = MakeContext(3, 8, lbcrypto::BV, 10);
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-bv-nonzero-digit-a-length-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 BV nonzero-digit A-length fixture must have exactly four full-Q towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 BV nonzero-digit A-length fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 BV nonzero-digit A-length fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(),
          "Relin2 BV nonzero-digit A-length fixture must have a nonempty key tag");
    Check(keys.secretKey->GetKeyTag() == tensor.GetKeyTag(),
          "Relin2 BV nonzero-digit A-length fixture secret key must match the Tensor tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-bv-nonzero-digit-a-length-immutability-probe") !=
                  highMetadata->end(),
          "Relin2 BV nonzero-digit A-length fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-bv-nonzero-digit-a-length-immutability-probe") !=
                  lowMetadata->end(),
          "Relin2 BV nonzero-digit A-length fixture low ciphertext lost its metadata probe");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr,
          "Relin2 BV nonzero-digit A-length fixture must expose CKKS-RNS parameters");
    Check(parameters->GetKeySwitchTechnique() == lbcrypto::BV,
          "Relin2 BV nonzero-digit A-length fixture must use BV key switching");
    const auto digitSize = parameters->GetDigitSize();
    Check(digitSize == 10,
          "Relin2 BV nonzero-digit A-length fixture must use digit size ten");
    const auto& fullQParameters = parameters->GetElementParams()->GetParams();
    Check(fullQParameters.size() == 4,
          "Relin2 BV nonzero-digit A-length fixture must have four full-Q towers");
    std::vector<std::uint32_t> fullQTowerBits;
    fullQTowerBits.reserve(fullQParameters.size());
    std::size_t expectedBVKeyLength = 0;
    for (const auto& towerParameters : fullQParameters) {
        Check(towerParameters != nullptr,
              "Relin2 BV nonzero-digit A-length fixture has null full-Q tower parameters");
        const auto towerBits = towerParameters->GetModulus().GetMSB();
        fullQTowerBits.push_back(towerBits);
        expectedBVKeyLength +=
            static_cast<std::size_t>((towerBits + digitSize - 1) / digitSize);
    }
    Check(fullQTowerBits == std::vector<std::uint32_t>{35, 31, 30, 31},
          "Relin2 BV nonzero-digit A-length fixture has an unexpected full-Q bit-width manifest");
    Check(expectedBVKeyLength == 15 && expectedBVKeyLength > fullQParameters.size(),
          "Relin2 BV nonzero-digit A-length fixture must have fifteen decomposed digits");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(),
          "Relin2 BV nonzero-digit A-length fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        context->EvalMultKeyGen(keys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 BV nonzero-digit A-length fixture must generate exactly one evaluation key");
        const auto bvKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(bvKey != nullptr,
              "Relin2 BV nonzero-digit A-length fixture must use the relinearization-key subtype");
        Check(bvKey->GetCryptoContext().get() == context.get(),
              "Relin2 BV nonzero-digit A-length fixture key must keep the bound context");
        Check(bvKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 BV nonzero-digit A-length fixture key must match the Tensor tag");

        const auto originalA = bvKey->GetAVector();
        const auto originalB = bvKey->GetBVector();
        Check(originalA.size() == expectedBVKeyLength && originalB.size() == expectedBVKeyLength,
              "Relin2 BV nonzero-digit A-length fixture must start with exact generated A/B lengths");

        const auto* cacheIdentityPositive = &evaluationKeys;
        const auto* vectorIdentityPositive = &generatedRow->second;
        const auto keyIdentityPositive = generatedRow->second.front();
        const auto keyContextPositive = bvKey->GetCryptoContext();
        const auto keyTagPositive = bvKey->GetKeyTag();
        const auto keyAPositive =
            SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV nonzero-digit positive A");
        const auto keyBPositive =
            SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV nonzero-digit positive B");
        const auto tensorPositive = SnapshotTensor(tensor);
        CheckPassesCurrentScaffoldOrCompletes(
            [&] { (void)module.Relin2(tensor); },
            "Relin2 BV nonzero-digit valid A/B lengths");
        CheckTensorUnchanged(tensor, tensorPositive,
                             "Relin2 BV nonzero-digit A-length positive control");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyAPositive,
                                "Relin2 BV nonzero-digit A-length positive A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBPositive,
                                "Relin2 BV nonzero-digit A-length positive B");
        const auto positiveRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityPositive && evaluationKeys.size() == 1 &&
                  positiveRow != evaluationKeys.end() && &positiveRow->second == vectorIdentityPositive &&
                  positiveRow->second.size() == 1 &&
                  positiveRow->second.front().get() == keyIdentityPositive.get(),
              "Relin2 BV nonzero-digit A-length positive control mutated the cache shape or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextPositive.get(),
              "Relin2 BV nonzero-digit A-length positive control mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagPositive,
              "Relin2 BV nonzero-digit A-length positive control mutated the key tag");

        auto malformedA = originalA;
        malformedA.pop_back();
        bvKey->SetAVector(std::move(malformedA));
        Check(bvKey->GetAVector().size() + 1 == expectedBVKeyLength &&
                  bvKey->GetAVector().front() == originalA.front() &&
                  bvKey->GetAVector().back() == originalA[expectedBVKeyLength - 2] &&
                  bvKey->GetBVector() == originalB,
              "Relin2 BV nonzero-digit A-length fixture must shorten only the A vector");

        const auto* cacheIdentityBefore = &evaluationKeys;
        const auto* vectorIdentityBefore = &generatedRow->second;
        const auto keyIdentityBefore = generatedRow->second.front();
        const auto keyContextBefore = bvKey->GetCryptoContext();
        const auto keyTagBefore = bvKey->GetKeyTag();
        const auto keyABefore =
            SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV nonzero-digit malformed A");
        const auto keyBBefore =
            SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV nonzero-digit valid B");
        const auto tensorBefore = SnapshotTensor(tensor);
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 evaluation key BV A vector length mismatch",
            "Relin2 BV nonzero-digit A-vector length");
        CheckTensorUnchanged(tensor, tensorBefore,
                             "Relin2 BV nonzero-digit A-length rejection");
        const auto currentRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBefore && evaluationKeys.size() == 1 &&
                  currentRow != evaluationKeys.end() && &currentRow->second == vectorIdentityBefore &&
                  currentRow->second.size() == 1 &&
                  currentRow->second.front().get() == keyIdentityBefore.get(),
              "Relin2 BV nonzero-digit A-length rejection mutated the cache shape or identity");
        const auto currentBVKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(currentRow->second.front());
        Check(currentBVKey.get() == bvKey.get(),
              "Relin2 BV nonzero-digit A-length rejection changed the concrete key subtype or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextBefore.get(),
              "Relin2 BV nonzero-digit A-length rejection mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagBefore,
              "Relin2 BV nonzero-digit A-length rejection mutated the key tag");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyABefore,
                                "Relin2 BV nonzero-digit A-length rejection A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBBefore,
                                "Relin2 BV nonzero-digit A-length rejection B");
    }
    Check(evaluationKeys.empty(),
          "Relin2 BV nonzero-digit A-length fixture failed to restore the initially empty cache");
}

void TestBVEvaluationKeyNonzeroDigitBLength() {
    auto context = MakeContext(3, 8, lbcrypto::BV, 10);
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-bv-nonzero-digit-b-length-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 BV nonzero-digit B-length fixture must have exactly four full-Q towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 BV nonzero-digit B-length fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 BV nonzero-digit B-length fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(),
          "Relin2 BV nonzero-digit B-length fixture must have a nonempty key tag");
    Check(keys.secretKey->GetKeyTag() == tensor.GetKeyTag(),
          "Relin2 BV nonzero-digit B-length fixture secret key must match the Tensor tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-bv-nonzero-digit-b-length-immutability-probe") !=
                  highMetadata->end(),
          "Relin2 BV nonzero-digit B-length fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-bv-nonzero-digit-b-length-immutability-probe") !=
                  lowMetadata->end(),
          "Relin2 BV nonzero-digit B-length fixture low ciphertext lost its metadata probe");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr,
          "Relin2 BV nonzero-digit B-length fixture must expose CKKS-RNS parameters");
    Check(parameters->GetKeySwitchTechnique() == lbcrypto::BV,
          "Relin2 BV nonzero-digit B-length fixture must use BV key switching");
    const auto digitSize = parameters->GetDigitSize();
    Check(digitSize == 10,
          "Relin2 BV nonzero-digit B-length fixture must use digit size ten");
    const auto& fullQParameters = parameters->GetElementParams()->GetParams();
    Check(fullQParameters.size() == 4,
          "Relin2 BV nonzero-digit B-length fixture must have four full-Q towers");
    std::vector<std::uint32_t> fullQTowerBits;
    fullQTowerBits.reserve(fullQParameters.size());
    std::size_t expectedBVKeyLength = 0;
    for (const auto& towerParameters : fullQParameters) {
        Check(towerParameters != nullptr,
              "Relin2 BV nonzero-digit B-length fixture has null full-Q tower parameters");
        const auto towerBits = towerParameters->GetModulus().GetMSB();
        fullQTowerBits.push_back(towerBits);
        expectedBVKeyLength +=
            static_cast<std::size_t>((towerBits + digitSize - 1) / digitSize);
    }
    Check(fullQTowerBits == std::vector<std::uint32_t>{35, 31, 30, 31},
          "Relin2 BV nonzero-digit B-length fixture has an unexpected full-Q bit-width manifest");
    Check(expectedBVKeyLength == 15 && expectedBVKeyLength > fullQParameters.size(),
          "Relin2 BV nonzero-digit B-length fixture must have fifteen decomposed digits");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(),
          "Relin2 BV nonzero-digit B-length fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        context->EvalMultKeyGen(keys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 BV nonzero-digit B-length fixture must generate exactly one evaluation key");
        const auto bvKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(bvKey != nullptr,
              "Relin2 BV nonzero-digit B-length fixture must use the relinearization-key subtype");
        Check(bvKey->GetCryptoContext().get() == context.get(),
              "Relin2 BV nonzero-digit B-length fixture key must keep the bound context");
        Check(bvKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 BV nonzero-digit B-length fixture key must match the Tensor tag");

        const auto originalA = bvKey->GetAVector();
        const auto originalB = bvKey->GetBVector();
        Check(originalA.size() == expectedBVKeyLength && originalB.size() == expectedBVKeyLength,
              "Relin2 BV nonzero-digit B-length fixture must start with exact generated A/B lengths");

        const auto* cacheIdentityPositive = &evaluationKeys;
        const auto* vectorIdentityPositive = &generatedRow->second;
        const auto keyIdentityPositive = generatedRow->second.front();
        const auto keyContextPositive = bvKey->GetCryptoContext();
        const auto keyTagPositive = bvKey->GetKeyTag();
        const auto keyAPositive =
            SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV nonzero-digit positive A");
        const auto keyBPositive =
            SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV nonzero-digit positive B");
        const auto tensorPositive = SnapshotTensor(tensor);
        CheckPassesCurrentScaffoldOrCompletes(
            [&] { (void)module.Relin2(tensor); },
            "Relin2 BV nonzero-digit valid A/B lengths");
        CheckTensorUnchanged(tensor, tensorPositive,
                             "Relin2 BV nonzero-digit B-length positive control");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyAPositive,
                                "Relin2 BV nonzero-digit B-length positive A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBPositive,
                                "Relin2 BV nonzero-digit B-length positive B");
        const auto positiveRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityPositive && evaluationKeys.size() == 1 &&
                  positiveRow != evaluationKeys.end() && &positiveRow->second == vectorIdentityPositive &&
                  positiveRow->second.size() == 1 &&
                  positiveRow->second.front().get() == keyIdentityPositive.get(),
              "Relin2 BV nonzero-digit B-length positive control mutated the cache shape or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextPositive.get(),
              "Relin2 BV nonzero-digit B-length positive control mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagPositive,
              "Relin2 BV nonzero-digit B-length positive control mutated the key tag");

        auto malformedB = originalB;
        malformedB.pop_back();
        bvKey->SetBVector(std::move(malformedB));
        Check(bvKey->GetBVector().size() + 1 == expectedBVKeyLength &&
                  bvKey->GetBVector().front() == originalB.front() &&
                  bvKey->GetBVector().back() == originalB[expectedBVKeyLength - 2] &&
                  bvKey->GetAVector() == originalA,
              "Relin2 BV nonzero-digit B-length fixture must shorten only the B vector");

        const auto* cacheIdentityBefore = &evaluationKeys;
        const auto* vectorIdentityBefore = &generatedRow->second;
        const auto keyIdentityBefore = generatedRow->second.front();
        const auto keyContextBefore = bvKey->GetCryptoContext();
        const auto keyTagBefore = bvKey->GetKeyTag();
        const auto keyABefore =
            SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV nonzero-digit valid A");
        const auto keyBBefore =
            SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV nonzero-digit malformed B");
        const auto tensorBefore = SnapshotTensor(tensor);
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 evaluation key BV B vector length mismatch",
            "Relin2 BV nonzero-digit B-vector length");
        CheckTensorUnchanged(tensor, tensorBefore,
                             "Relin2 BV nonzero-digit B-length rejection");
        const auto currentRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBefore && evaluationKeys.size() == 1 &&
                  currentRow != evaluationKeys.end() && &currentRow->second == vectorIdentityBefore &&
                  currentRow->second.size() == 1 &&
                  currentRow->second.front().get() == keyIdentityBefore.get(),
              "Relin2 BV nonzero-digit B-length rejection mutated the cache shape or identity");
        const auto currentBVKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(currentRow->second.front());
        Check(currentBVKey.get() == bvKey.get(),
              "Relin2 BV nonzero-digit B-length rejection changed the concrete key subtype or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextBefore.get(),
              "Relin2 BV nonzero-digit B-length rejection mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagBefore,
              "Relin2 BV nonzero-digit B-length rejection mutated the key tag");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyABefore,
                                "Relin2 BV nonzero-digit B-length rejection A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBBefore,
                                "Relin2 BV nonzero-digit B-length rejection B");
    }
    Check(evaluationKeys.empty(),
          "Relin2 BV nonzero-digit B-length fixture failed to restore the initially empty cache");
}

void TestBVEvaluationKeyZeroDigitEntryBasis() {
    auto context = MakeContext(3, 8, lbcrypto::BV, 0);
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-bv-zero-digit-entry-basis-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 BV zero-digit entry-basis fixture must have exactly four full-Q towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 BV zero-digit entry-basis fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 BV zero-digit entry-basis fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(),
          "Relin2 BV zero-digit entry-basis fixture must have a nonempty key tag");
    Check(keys.secretKey->GetKeyTag() == tensor.GetKeyTag(),
          "Relin2 BV zero-digit entry-basis fixture secret key must match the Tensor tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-bv-zero-digit-entry-basis-immutability-probe") !=
                  highMetadata->end(),
          "Relin2 BV zero-digit entry-basis fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-bv-zero-digit-entry-basis-immutability-probe") !=
                  lowMetadata->end(),
          "Relin2 BV zero-digit entry-basis fixture low ciphertext lost its metadata probe");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr,
          "Relin2 BV zero-digit entry-basis fixture must expose CKKS-RNS parameters");
    Check(parameters->GetKeySwitchTechnique() == lbcrypto::BV,
          "Relin2 BV zero-digit entry-basis fixture must use BV key switching");
    Check(parameters->GetDigitSize() == 0,
          "Relin2 BV zero-digit entry-basis fixture must use digit size zero");
    const auto expectedBasis = parameters->GetElementParams();
    Check(expectedBasis != nullptr && expectedBasis->GetParams().size() == 4,
          "Relin2 BV zero-digit entry-basis fixture must expose the complete four-tower Q basis");
    Check(expectedBasis->GetParams()[0] != nullptr && expectedBasis->GetParams()[1] != nullptr &&
              expectedBasis->GetParams()[0]->GetModulus() != expectedBasis->GetParams()[1]->GetModulus(),
          "Relin2 BV zero-digit entry-basis fixture must have distinguishable first two Q towers");
    const auto expectedBVKeyLength = expectedBasis->GetParams().size();

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(),
          "Relin2 BV zero-digit entry-basis fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        context->EvalMultKeyGen(keys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 BV zero-digit entry-basis fixture must generate exactly one evaluation key");
        const auto bvKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(bvKey != nullptr,
              "Relin2 BV zero-digit entry-basis fixture must use the relinearization-key subtype");
        Check(bvKey->GetCryptoContext().get() == context.get(),
              "Relin2 BV zero-digit entry-basis fixture key must keep the bound context");
        Check(bvKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 BV zero-digit entry-basis fixture key must match the Tensor tag");

        const auto originalA = bvKey->GetAVector();
        const auto originalB = bvKey->GetBVector();
        Check(originalA.size() == expectedBVKeyLength && originalB.size() == expectedBVKeyLength,
              "Relin2 BV zero-digit entry-basis fixture must start with exact generated A/B lengths");
        for (std::size_t index = 0; index < originalA.size(); ++index) {
            Check(originalA[index].GetFormat() == Format::EVALUATION,
                  "Relin2 BV zero-digit entry-basis fixture generated a non-Evaluation A entry");
            CheckKeyPolynomialBasis(originalA[index], expectedBasis,
                                    "Relin2 BV zero-digit generated A entry " + std::to_string(index));
        }
        for (std::size_t index = 0; index < originalB.size(); ++index) {
            Check(originalB[index].GetFormat() == Format::EVALUATION,
                  "Relin2 BV zero-digit entry-basis fixture generated a non-Evaluation B entry");
            CheckKeyPolynomialBasis(originalB[index], expectedBasis,
                                    "Relin2 BV zero-digit generated B entry " + std::to_string(index));
        }

        auto equivalentB = originalB;
        equivalentB.back() = CloneKeyPolynomialWithIndependentParams(
            originalB.back(), "Relin2 BV zero-digit equivalent-pointer positive control");
        Check(equivalentB.back().GetParams().get() != originalB.back().GetParams().get() &&
                  equivalentB.back().GetParams().get() != expectedBasis.get(),
              "Relin2 BV zero-digit entry-basis positive control must use independent aggregate parameters");
        for (std::size_t index = 0; index < equivalentB.back().GetAllElements().size(); ++index) {
            Check(equivalentB.back().GetAllElements()[index].GetParams().get() !=
                      originalB.back().GetAllElements()[index].GetParams().get(),
                  "Relin2 BV zero-digit entry-basis positive control must use independent tower parameters");
        }
        Check(*equivalentB.back().GetParams() == *expectedBasis &&
                  equivalentB.back() == originalB.back() &&
                  equivalentB.back().GetFormat() == Format::EVALUATION,
              "Relin2 BV zero-digit entry-basis positive control must remain semantically equivalent");
        CheckKeyPolynomialBasis(equivalentB.back(), expectedBasis,
                                "Relin2 BV zero-digit equivalent-pointer positive control");
        bvKey->SetBVector(std::move(equivalentB));

        const auto* cacheIdentityBeforePositive = &evaluationKeys;
        const auto* vectorIdentityBeforePositive = &generatedRow->second;
        const auto keyIdentityBeforePositive = generatedRow->second.front();
        const auto keyContextBeforePositive = bvKey->GetCryptoContext();
        const auto keyTagBeforePositive = bvKey->GetKeyTag();
        const auto tensorBeforePositive = SnapshotTensor(tensor);
        const auto keyABeforePositive =
            SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV zero-digit entry-basis positive A");
        const auto keyBBeforePositive =
            SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV zero-digit entry-basis positive B");
        CheckPassesCurrentScaffoldOrCompletes(
            [&] { (void)module.Relin2(tensor); },
            "Relin2 BV zero-digit equivalent-pointer entry basis");
        CheckTensorUnchanged(tensor, tensorBeforePositive,
                             "Relin2 BV zero-digit equivalent-pointer positive control");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyABeforePositive,
                                "Relin2 BV zero-digit entry-basis positive A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBBeforePositive,
                                "Relin2 BV zero-digit entry-basis positive B");
        const auto currentPositiveRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBeforePositive && evaluationKeys.size() == 1 &&
                  currentPositiveRow != evaluationKeys.end() &&
                  &currentPositiveRow->second == vectorIdentityBeforePositive &&
                  currentPositiveRow->second.size() == 1 &&
                  currentPositiveRow->second.front().get() == keyIdentityBeforePositive.get(),
              "Relin2 BV zero-digit entry-basis positive control mutated the cache shape or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextBeforePositive.get(),
              "Relin2 BV zero-digit entry-basis positive control mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagBeforePositive,
              "Relin2 BV zero-digit entry-basis positive control mutated the key tag");

        auto malformedB = originalB;
        auto swappedTowers = malformedB.back().GetAllElements();
        Check(swappedTowers.size() == expectedBasis->GetParams().size() && swappedTowers.size() >= 2,
              "Relin2 BV zero-digit entry-basis negative control must start from complete Q towers");
        std::swap(swappedTowers[0], swappedTowers[1]);
        malformedB.back() = DCRTPoly(swappedTowers);
        const auto wrongBasis = malformedB.back().GetParams();
        Check(wrongBasis != nullptr && !(*wrongBasis == *expectedBasis) &&
                  malformedB.back().GetFormat() == Format::EVALUATION &&
                  malformedB.back().GetAllElements().size() == expectedBasis->GetParams().size() &&
                  malformedB.back().GetCyclotomicOrder() == expectedBasis->GetCyclotomicOrder(),
              "Relin2 BV zero-digit entry-basis negative control must preserve shape and change only tower order");
        Check(wrongBasis->GetParams()[0]->GetModulus() == expectedBasis->GetParams()[1]->GetModulus() &&
                  wrongBasis->GetParams()[0]->GetRootOfUnity() == expectedBasis->GetParams()[1]->GetRootOfUnity() &&
                  wrongBasis->GetParams()[1]->GetModulus() == expectedBasis->GetParams()[0]->GetModulus() &&
                  wrongBasis->GetParams()[1]->GetRootOfUnity() == expectedBasis->GetParams()[0]->GetRootOfUnity(),
              "Relin2 BV zero-digit entry-basis negative control must swap the first two declared towers exactly");
        Check(malformedB.back().GetAllElements()[0].GetModulus() ==
                  expectedBasis->GetParams()[1]->GetModulus() &&
                  malformedB.back().GetAllElements()[0].GetRootOfUnity() ==
                  expectedBasis->GetParams()[1]->GetRootOfUnity() &&
                  malformedB.back().GetAllElements()[1].GetModulus() ==
                  expectedBasis->GetParams()[0]->GetModulus() &&
                  malformedB.back().GetAllElements()[1].GetRootOfUnity() ==
                  expectedBasis->GetParams()[0]->GetRootOfUnity(),
              "Relin2 BV zero-digit entry-basis negative control must swap the first two actual towers exactly");
        for (std::size_t index = 2; index < expectedBasis->GetParams().size(); ++index) {
            Check(wrongBasis->GetParams()[index]->GetCyclotomicOrder() ==
                      expectedBasis->GetParams()[index]->GetCyclotomicOrder() &&
                      wrongBasis->GetParams()[index]->GetModulus() ==
                      expectedBasis->GetParams()[index]->GetModulus() &&
                      wrongBasis->GetParams()[index]->GetRootOfUnity() ==
                      expectedBasis->GetParams()[index]->GetRootOfUnity(),
                  "Relin2 BV zero-digit entry-basis negative control changed an unswapped declared tower");
            Check(malformedB.back().GetAllElements()[index].GetCyclotomicOrder() ==
                      originalB.back().GetAllElements()[index].GetCyclotomicOrder() &&
                      malformedB.back().GetAllElements()[index].GetModulus() ==
                      originalB.back().GetAllElements()[index].GetModulus() &&
                      malformedB.back().GetAllElements()[index].GetRootOfUnity() ==
                      originalB.back().GetAllElements()[index].GetRootOfUnity(),
                  "Relin2 BV zero-digit entry-basis negative control changed an unswapped actual tower");
        }
        bvKey->SetBVector(std::move(malformedB));
        Check(bvKey->GetAVector() == originalA &&
                  bvKey->GetBVector().size() == originalB.size() &&
                  bvKey->GetBVector().back().GetParams().get() == wrongBasis.get(),
              "Relin2 BV zero-digit entry-basis negative control must alter only the last B entry basis");
        for (std::size_t index = 0; index + 1 < originalB.size(); ++index) {
            Check(bvKey->GetBVector()[index] == originalB[index],
                  "Relin2 BV zero-digit entry-basis negative control changed an earlier B entry");
        }

        const auto* cacheIdentityBeforeNegative = &evaluationKeys;
        const auto* vectorIdentityBeforeNegative = &generatedRow->second;
        const auto keyIdentityBeforeNegative = generatedRow->second.front();
        const auto keyContextBeforeNegative = bvKey->GetCryptoContext();
        const auto keyTagBeforeNegative = bvKey->GetKeyTag();
        const auto tensorBeforeNegative = SnapshotTensor(tensor);
        const auto keyABeforeNegative =
            SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV zero-digit entry-basis negative A");
        const auto keyBBeforeNegative =
            SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV zero-digit entry-basis negative B");
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 evaluation key BV entry basis mismatch",
            "Relin2 BV zero-digit evaluation-key entry basis");
        CheckTensorUnchanged(tensor, tensorBeforeNegative,
                             "Relin2 BV zero-digit entry-basis rejection");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyABeforeNegative,
                                "Relin2 BV zero-digit entry-basis rejection A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBBeforeNegative,
                                "Relin2 BV zero-digit entry-basis rejection B");
        const auto currentNegativeRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBeforeNegative && evaluationKeys.size() == 1 &&
                  currentNegativeRow != evaluationKeys.end() &&
                  &currentNegativeRow->second == vectorIdentityBeforeNegative &&
                  currentNegativeRow->second.size() == 1 &&
                  currentNegativeRow->second.front().get() == keyIdentityBeforeNegative.get(),
              "Relin2 BV zero-digit entry-basis rejection mutated the cache shape or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextBeforeNegative.get(),
              "Relin2 BV zero-digit entry-basis rejection mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagBeforeNegative,
              "Relin2 BV zero-digit entry-basis rejection mutated the key tag");
    }
    Check(evaluationKeys.empty(),
          "Relin2 BV zero-digit entry-basis fixture failed to restore the initially empty cache");
}

void TestBVEvaluationKeyNonzeroDigitEntryBasis() {
    auto context = MakeContext(3, 8, lbcrypto::BV, 10);
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-bv-nonzero-digit-entry-basis-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 BV nonzero-digit entry-basis fixture must have exactly four full-Q towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 BV nonzero-digit entry-basis fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 BV nonzero-digit entry-basis fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(),
          "Relin2 BV nonzero-digit entry-basis fixture must have a nonempty key tag");
    Check(keys.secretKey->GetKeyTag() == tensor.GetKeyTag(),
          "Relin2 BV nonzero-digit entry-basis fixture secret key must match the Tensor tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-bv-nonzero-digit-entry-basis-immutability-probe") !=
                  highMetadata->end(),
          "Relin2 BV nonzero-digit entry-basis fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-bv-nonzero-digit-entry-basis-immutability-probe") !=
                  lowMetadata->end(),
          "Relin2 BV nonzero-digit entry-basis fixture low ciphertext lost its metadata probe");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr,
          "Relin2 BV nonzero-digit entry-basis fixture must expose CKKS-RNS parameters");
    Check(parameters->GetKeySwitchTechnique() == lbcrypto::BV,
          "Relin2 BV nonzero-digit entry-basis fixture must use BV key switching");
    const auto digitSize = parameters->GetDigitSize();
    Check(digitSize == 10,
          "Relin2 BV nonzero-digit entry-basis fixture must use digit size ten");
    const auto expectedBasis = parameters->GetElementParams();
    Check(expectedBasis != nullptr && expectedBasis->GetParams().size() == 4,
          "Relin2 BV nonzero-digit entry-basis fixture must expose the complete four-tower Q basis");
    Check(expectedBasis->GetParams()[0] != nullptr && expectedBasis->GetParams()[1] != nullptr &&
              expectedBasis->GetParams()[0]->GetModulus() != expectedBasis->GetParams()[1]->GetModulus(),
          "Relin2 BV nonzero-digit entry-basis fixture must have distinguishable first two Q towers");
    std::vector<std::uint32_t> fullQTowerBits;
    fullQTowerBits.reserve(expectedBasis->GetParams().size());
    std::size_t expectedBVKeyLength = 0;
    for (const auto& towerParameters : expectedBasis->GetParams()) {
        Check(towerParameters != nullptr,
              "Relin2 BV nonzero-digit entry-basis fixture has null full-Q tower parameters");
        const auto towerBits = towerParameters->GetModulus().GetMSB();
        fullQTowerBits.push_back(towerBits);
        expectedBVKeyLength +=
            static_cast<std::size_t>((towerBits + digitSize - 1) / digitSize);
    }
    Check(fullQTowerBits == std::vector<std::uint32_t>{35, 31, 30, 31},
          "Relin2 BV nonzero-digit entry-basis fixture has an unexpected full-Q bit-width manifest");
    Check(expectedBVKeyLength == 15 && expectedBVKeyLength > expectedBasis->GetParams().size(),
          "Relin2 BV nonzero-digit entry-basis fixture must have fifteen decomposed digits");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(),
          "Relin2 BV nonzero-digit entry-basis fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        context->EvalMultKeyGen(keys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 BV nonzero-digit entry-basis fixture must generate exactly one evaluation key");
        const auto bvKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(bvKey != nullptr,
              "Relin2 BV nonzero-digit entry-basis fixture must use the relinearization-key subtype");
        Check(bvKey->GetCryptoContext().get() == context.get(),
              "Relin2 BV nonzero-digit entry-basis fixture key must keep the bound context");
        Check(bvKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 BV nonzero-digit entry-basis fixture key must match the Tensor tag");

        const auto originalA = bvKey->GetAVector();
        const auto originalB = bvKey->GetBVector();
        Check(originalA.size() == expectedBVKeyLength && originalB.size() == expectedBVKeyLength,
              "Relin2 BV nonzero-digit entry-basis fixture must start with exact generated A/B lengths");
        for (std::size_t index = 0; index < originalA.size(); ++index) {
            Check(originalA[index].GetFormat() == Format::EVALUATION,
                  "Relin2 BV nonzero-digit entry-basis fixture generated a non-Evaluation A entry");
            CheckKeyPolynomialBasis(originalA[index], expectedBasis,
                                    "Relin2 BV nonzero-digit generated A entry " + std::to_string(index));
        }
        for (std::size_t index = 0; index < originalB.size(); ++index) {
            Check(originalB[index].GetFormat() == Format::EVALUATION,
                  "Relin2 BV nonzero-digit entry-basis fixture generated a non-Evaluation B entry");
            CheckKeyPolynomialBasis(originalB[index], expectedBasis,
                                    "Relin2 BV nonzero-digit generated B entry " + std::to_string(index));
        }

        auto equivalentA = originalA;
        equivalentA.back() = CloneKeyPolynomialWithIndependentParams(
            originalA.back(), "Relin2 BV nonzero-digit equivalent-pointer positive control");
        Check(equivalentA.back().GetParams().get() != originalA.back().GetParams().get() &&
                  equivalentA.back().GetParams().get() != expectedBasis.get(),
              "Relin2 BV nonzero-digit entry-basis positive control must use independent aggregate parameters");
        for (std::size_t index = 0; index < equivalentA.back().GetAllElements().size(); ++index) {
            Check(equivalentA.back().GetAllElements()[index].GetParams().get() !=
                      originalA.back().GetAllElements()[index].GetParams().get(),
                  "Relin2 BV nonzero-digit entry-basis positive control must use independent tower parameters");
        }
        Check(*equivalentA.back().GetParams() == *expectedBasis &&
                  equivalentA.back() == originalA.back() &&
                  equivalentA.back().GetFormat() == Format::EVALUATION,
              "Relin2 BV nonzero-digit entry-basis positive control must remain semantically equivalent");
        CheckKeyPolynomialBasis(equivalentA.back(), expectedBasis,
                                "Relin2 BV nonzero-digit equivalent-pointer positive control");
        bvKey->SetAVector(std::move(equivalentA));

        const auto* cacheIdentityBeforePositive = &evaluationKeys;
        const auto* vectorIdentityBeforePositive = &generatedRow->second;
        const auto keyIdentityBeforePositive = generatedRow->second.front();
        const auto keyContextBeforePositive = bvKey->GetCryptoContext();
        const auto keyTagBeforePositive = bvKey->GetKeyTag();
        const auto tensorBeforePositive = SnapshotTensor(tensor);
        const auto keyABeforePositive =
            SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV nonzero-digit entry-basis positive A");
        const auto keyBBeforePositive =
            SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV nonzero-digit entry-basis positive B");
        CheckPassesCurrentScaffoldOrCompletes(
            [&] { (void)module.Relin2(tensor); },
            "Relin2 BV nonzero-digit equivalent-pointer entry basis");
        CheckTensorUnchanged(tensor, tensorBeforePositive,
                             "Relin2 BV nonzero-digit equivalent-pointer positive control");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyABeforePositive,
                                "Relin2 BV nonzero-digit entry-basis positive A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBBeforePositive,
                                "Relin2 BV nonzero-digit entry-basis positive B");
        const auto currentPositiveRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBeforePositive && evaluationKeys.size() == 1 &&
                  currentPositiveRow != evaluationKeys.end() &&
                  &currentPositiveRow->second == vectorIdentityBeforePositive &&
                  currentPositiveRow->second.size() == 1 &&
                  currentPositiveRow->second.front().get() == keyIdentityBeforePositive.get(),
              "Relin2 BV nonzero-digit entry-basis positive control mutated the cache shape or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextBeforePositive.get(),
              "Relin2 BV nonzero-digit entry-basis positive control mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagBeforePositive,
              "Relin2 BV nonzero-digit entry-basis positive control mutated the key tag");

        auto malformedA = originalA;
        auto swappedTowers = malformedA.back().GetAllElements();
        Check(swappedTowers.size() == expectedBasis->GetParams().size() && swappedTowers.size() >= 2,
              "Relin2 BV nonzero-digit entry-basis negative control must start from complete Q towers");
        std::swap(swappedTowers[0], swappedTowers[1]);
        malformedA.back() = DCRTPoly(swappedTowers);
        const auto wrongBasis = malformedA.back().GetParams();
        Check(wrongBasis != nullptr && !(*wrongBasis == *expectedBasis) &&
                  malformedA.back().GetFormat() == Format::EVALUATION &&
                  malformedA.back().GetAllElements().size() == expectedBasis->GetParams().size() &&
                  malformedA.back().GetCyclotomicOrder() == expectedBasis->GetCyclotomicOrder(),
              "Relin2 BV nonzero-digit entry-basis negative control must preserve shape and change only tower order");
        Check(wrongBasis->GetParams()[0]->GetModulus() == expectedBasis->GetParams()[1]->GetModulus() &&
                  wrongBasis->GetParams()[0]->GetRootOfUnity() == expectedBasis->GetParams()[1]->GetRootOfUnity() &&
                  wrongBasis->GetParams()[1]->GetModulus() == expectedBasis->GetParams()[0]->GetModulus() &&
                  wrongBasis->GetParams()[1]->GetRootOfUnity() == expectedBasis->GetParams()[0]->GetRootOfUnity(),
              "Relin2 BV nonzero-digit entry-basis negative control must swap the first two declared towers exactly");
        Check(malformedA.back().GetAllElements()[0].GetModulus() ==
                  expectedBasis->GetParams()[1]->GetModulus() &&
                  malformedA.back().GetAllElements()[0].GetRootOfUnity() ==
                  expectedBasis->GetParams()[1]->GetRootOfUnity() &&
                  malformedA.back().GetAllElements()[1].GetModulus() ==
                  expectedBasis->GetParams()[0]->GetModulus() &&
                  malformedA.back().GetAllElements()[1].GetRootOfUnity() ==
                  expectedBasis->GetParams()[0]->GetRootOfUnity(),
              "Relin2 BV nonzero-digit entry-basis negative control must swap the first two actual towers exactly");
        for (std::size_t index = 2; index < expectedBasis->GetParams().size(); ++index) {
            Check(wrongBasis->GetParams()[index]->GetCyclotomicOrder() ==
                      expectedBasis->GetParams()[index]->GetCyclotomicOrder() &&
                      wrongBasis->GetParams()[index]->GetModulus() ==
                      expectedBasis->GetParams()[index]->GetModulus() &&
                      wrongBasis->GetParams()[index]->GetRootOfUnity() ==
                      expectedBasis->GetParams()[index]->GetRootOfUnity(),
                  "Relin2 BV nonzero-digit entry-basis negative control changed an unswapped declared tower");
            Check(malformedA.back().GetAllElements()[index].GetCyclotomicOrder() ==
                      originalA.back().GetAllElements()[index].GetCyclotomicOrder() &&
                      malformedA.back().GetAllElements()[index].GetModulus() ==
                      originalA.back().GetAllElements()[index].GetModulus() &&
                      malformedA.back().GetAllElements()[index].GetRootOfUnity() ==
                      originalA.back().GetAllElements()[index].GetRootOfUnity(),
                  "Relin2 BV nonzero-digit entry-basis negative control changed an unswapped actual tower");
        }
        bvKey->SetAVector(std::move(malformedA));
        Check(bvKey->GetBVector() == originalB &&
                  bvKey->GetAVector().size() == originalA.size() &&
                  bvKey->GetAVector().back().GetParams().get() == wrongBasis.get(),
              "Relin2 BV nonzero-digit entry-basis negative control must alter only the last A entry basis");
        for (std::size_t index = 0; index + 1 < originalA.size(); ++index) {
            Check(bvKey->GetAVector()[index] == originalA[index],
                  "Relin2 BV nonzero-digit entry-basis negative control changed an earlier A entry");
        }

        const auto* cacheIdentityBeforeNegative = &evaluationKeys;
        const auto* vectorIdentityBeforeNegative = &generatedRow->second;
        const auto keyIdentityBeforeNegative = generatedRow->second.front();
        const auto keyContextBeforeNegative = bvKey->GetCryptoContext();
        const auto keyTagBeforeNegative = bvKey->GetKeyTag();
        const auto tensorBeforeNegative = SnapshotTensor(tensor);
        const auto keyABeforeNegative =
            SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV nonzero-digit entry-basis negative A");
        const auto keyBBeforeNegative =
            SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV nonzero-digit entry-basis negative B");
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 evaluation key BV entry basis mismatch",
            "Relin2 BV nonzero-digit evaluation-key entry basis");
        CheckTensorUnchanged(tensor, tensorBeforeNegative,
                             "Relin2 BV nonzero-digit entry-basis rejection");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyABeforeNegative,
                                "Relin2 BV nonzero-digit entry-basis rejection A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBBeforeNegative,
                                "Relin2 BV nonzero-digit entry-basis rejection B");
        const auto currentNegativeRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBeforeNegative && evaluationKeys.size() == 1 &&
                  currentNegativeRow != evaluationKeys.end() &&
                  &currentNegativeRow->second == vectorIdentityBeforeNegative &&
                  currentNegativeRow->second.size() == 1 &&
                  currentNegativeRow->second.front().get() == keyIdentityBeforeNegative.get(),
              "Relin2 BV nonzero-digit entry-basis rejection mutated the cache shape or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextBeforeNegative.get(),
              "Relin2 BV nonzero-digit entry-basis rejection mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagBeforeNegative,
              "Relin2 BV nonzero-digit entry-basis rejection mutated the key tag");
    }
    Check(evaluationKeys.empty(),
          "Relin2 BV nonzero-digit entry-basis fixture failed to restore the initially empty cache");
}

void TestBVEvaluationKeyZeroDigitEntryFormat() {
    auto context = MakeContext(3, 8, lbcrypto::BV, 0);
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-bv-zero-digit-entry-format-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 BV zero-digit entry-format fixture must have exactly four full-Q towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 BV zero-digit entry-format fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 BV zero-digit entry-format fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(),
          "Relin2 BV zero-digit entry-format fixture must have a nonempty key tag");
    Check(keys.secretKey->GetKeyTag() == tensor.GetKeyTag(),
          "Relin2 BV zero-digit entry-format fixture secret key must match the Tensor tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-bv-zero-digit-entry-format-immutability-probe") !=
                  highMetadata->end(),
          "Relin2 BV zero-digit entry-format fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-bv-zero-digit-entry-format-immutability-probe") !=
                  lowMetadata->end(),
          "Relin2 BV zero-digit entry-format fixture low ciphertext lost its metadata probe");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr,
          "Relin2 BV zero-digit entry-format fixture must expose CKKS-RNS parameters");
    Check(parameters->GetKeySwitchTechnique() == lbcrypto::BV,
          "Relin2 BV zero-digit entry-format fixture must use BV key switching");
    Check(parameters->GetDigitSize() == 0,
          "Relin2 BV zero-digit entry-format fixture must use digit size zero");
    const auto expectedBasis = parameters->GetElementParams();
    Check(expectedBasis != nullptr && expectedBasis->GetParams().size() == 4,
          "Relin2 BV zero-digit entry-format fixture must expose the complete four-tower Q basis");
    const auto expectedBVKeyLength = expectedBasis->GetParams().size();

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(),
          "Relin2 BV zero-digit entry-format fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        context->EvalMultKeyGen(keys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 BV zero-digit entry-format fixture must generate exactly one evaluation key");
        const auto bvKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(bvKey != nullptr,
              "Relin2 BV zero-digit entry-format fixture must use the relinearization-key subtype");
        Check(bvKey->GetCryptoContext().get() == context.get(),
              "Relin2 BV zero-digit entry-format fixture key must keep the bound context");
        Check(bvKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 BV zero-digit entry-format fixture key must match the Tensor tag");

        const auto originalA = bvKey->GetAVector();
        const auto originalB = bvKey->GetBVector();
        Check(originalA.size() == expectedBVKeyLength && originalB.size() == expectedBVKeyLength,
              "Relin2 BV zero-digit entry-format fixture must start with exact generated A/B lengths");
        for (std::size_t index = 0; index < originalA.size(); ++index) {
            Check(originalA[index].GetFormat() == Format::EVALUATION,
                  "Relin2 BV zero-digit entry-format fixture generated a non-Evaluation A entry");
            CheckKeyPolynomialBasis(originalA[index], expectedBasis,
                                    "Relin2 BV zero-digit entry-format generated A entry " +
                                        std::to_string(index));
            for (const auto& tower : originalA[index].GetAllElements()) {
                Check(tower.GetFormat() == Format::EVALUATION,
                      "Relin2 BV zero-digit entry-format fixture generated a non-Evaluation A tower");
            }
        }
        for (std::size_t index = 0; index < originalB.size(); ++index) {
            Check(originalB[index].GetFormat() == Format::EVALUATION,
                  "Relin2 BV zero-digit entry-format fixture generated a non-Evaluation B entry");
            CheckKeyPolynomialBasis(originalB[index], expectedBasis,
                                    "Relin2 BV zero-digit entry-format generated B entry " +
                                        std::to_string(index));
            for (const auto& tower : originalB[index].GetAllElements()) {
                Check(tower.GetFormat() == Format::EVALUATION,
                      "Relin2 BV zero-digit entry-format fixture generated a non-Evaluation B tower");
            }
        }

        const auto* cacheIdentityBeforePositive = &evaluationKeys;
        const auto* vectorIdentityBeforePositive = &generatedRow->second;
        const auto keyIdentityBeforePositive = generatedRow->second.front();
        const auto keyContextBeforePositive = bvKey->GetCryptoContext();
        const auto keyTagBeforePositive = bvKey->GetKeyTag();
        const auto tensorBeforePositive = SnapshotTensor(tensor);
        const auto keyABeforePositive =
            SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV zero-digit entry-format positive A");
        const auto keyBBeforePositive =
            SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV zero-digit entry-format positive B");
        CheckPassesCurrentScaffoldOrCompletes(
            [&] { (void)module.Relin2(tensor); },
            "Relin2 BV zero-digit valid entry format");
        CheckTensorUnchanged(tensor, tensorBeforePositive,
                             "Relin2 BV zero-digit entry-format positive control");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyABeforePositive,
                                "Relin2 BV zero-digit entry-format positive A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBBeforePositive,
                                "Relin2 BV zero-digit entry-format positive B");
        const auto currentPositiveRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBeforePositive && evaluationKeys.size() == 1 &&
                  currentPositiveRow != evaluationKeys.end() &&
                  &currentPositiveRow->second == vectorIdentityBeforePositive &&
                  currentPositiveRow->second.size() == 1 &&
                  currentPositiveRow->second.front().get() == keyIdentityBeforePositive.get(),
              "Relin2 BV zero-digit entry-format positive control mutated the cache shape or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextBeforePositive.get(),
              "Relin2 BV zero-digit entry-format positive control mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagBeforePositive,
              "Relin2 BV zero-digit entry-format positive control mutated the key tag");

        auto malformedB = originalB;
        const auto targetBasisIdentity = malformedB.back().GetParams();
        malformedB.back().SetFormat(Format::COEFFICIENT);
        Check(malformedB.size() == expectedBVKeyLength &&
                  malformedB.back().GetFormat() == Format::COEFFICIENT &&
                  malformedB.back().GetParams().get() == targetBasisIdentity.get(),
              "Relin2 BV zero-digit entry-format negative control must change only the last B representation");
        CheckKeyPolynomialBasis(malformedB.back(), expectedBasis,
                                "Relin2 BV zero-digit Coefficient-format negative control");
        for (std::size_t index = 0; index < malformedB.back().GetAllElements().size(); ++index) {
            Check(malformedB.back().GetAllElements()[index].GetFormat() == Format::COEFFICIENT,
                  "Relin2 BV zero-digit entry-format negative control left an Evaluation tower");
            Check(malformedB.back().GetAllElements()[index].GetParams().get() ==
                      originalB.back().GetAllElements()[index].GetParams().get(),
                  "Relin2 BV zero-digit entry-format negative control changed a tower basis identity");
        }
        auto roundTrippedTarget = malformedB.back();
        roundTrippedTarget.SetFormat(Format::EVALUATION);
        Check(roundTrippedTarget == originalB.back() &&
                  roundTrippedTarget.GetFormat() == Format::EVALUATION,
              "Relin2 BV zero-digit entry-format negative control changed the represented polynomial");
        for (std::size_t index = 0; index + 1 < malformedB.size(); ++index) {
            Check(malformedB[index] == originalB[index] &&
                      malformedB[index].GetFormat() == Format::EVALUATION,
                  "Relin2 BV zero-digit entry-format negative control changed another B entry");
        }
        bvKey->SetBVector(std::move(malformedB));
        Check(bvKey->GetAVector() == originalA &&
                  bvKey->GetBVector().size() == expectedBVKeyLength &&
                  bvKey->GetBVector().back().GetFormat() == Format::COEFFICIENT &&
                  bvKey->GetBVector().back().GetParams().get() == targetBasisIdentity.get(),
              "Relin2 BV zero-digit entry-format negative control changed key shape, basis, or A entries");

        const auto* cacheIdentityBeforeNegative = &evaluationKeys;
        const auto* vectorIdentityBeforeNegative = &generatedRow->second;
        const auto keyIdentityBeforeNegative = generatedRow->second.front();
        const auto keyContextBeforeNegative = bvKey->GetCryptoContext();
        const auto keyTagBeforeNegative = bvKey->GetKeyTag();
        const auto tensorBeforeNegative = SnapshotTensor(tensor);
        const auto keyABeforeNegative =
            SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV zero-digit entry-format negative A");
        const auto keyBBeforeNegative =
            SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV zero-digit entry-format negative B");
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 evaluation key BV entry must be in evaluation format",
            "Relin2 BV zero-digit evaluation-key entry format");
        CheckTensorUnchanged(tensor, tensorBeforeNegative,
                             "Relin2 BV zero-digit entry-format rejection");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyABeforeNegative,
                                "Relin2 BV zero-digit entry-format rejection A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBBeforeNegative,
                                "Relin2 BV zero-digit entry-format rejection B");
        const auto currentNegativeRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBeforeNegative && evaluationKeys.size() == 1 &&
                  currentNegativeRow != evaluationKeys.end() &&
                  &currentNegativeRow->second == vectorIdentityBeforeNegative &&
                  currentNegativeRow->second.size() == 1 &&
                  currentNegativeRow->second.front().get() == keyIdentityBeforeNegative.get(),
              "Relin2 BV zero-digit entry-format rejection mutated the cache shape or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextBeforeNegative.get(),
              "Relin2 BV zero-digit entry-format rejection mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagBeforeNegative,
              "Relin2 BV zero-digit entry-format rejection mutated the key tag");
    }
    Check(evaluationKeys.empty(),
          "Relin2 BV zero-digit entry-format fixture failed to restore the initially empty cache");
}

using TestFunction = void (*)();

TestFunction ResolveTest(const std::string& name) {
    if (name == "tensor_validation_order") {
        return &TestTensorValidationOrder;
    }
    if (name == "insufficient_active_basis") {
        return &TestInsufficientActiveBasis;
    }
    if (name == "missing_eval_key") {
        return &TestMissingEvaluationKey;
    }
    if (name == "key_empty") {
        return &TestEmptyEvaluationKeyVector;
    }
    if (name == "key_null_first") {
        return &TestNullFirstEvaluationKey;
    }
    if (name == "key_wrong_context") {
        return &TestWrongContextEvaluationKey;
    }
    if (name == "key_wrong_tag") {
        return &TestWrongTagEvaluationKey;
    }
    if (name == "key_wrong_subtype") {
        return &TestWrongEvaluationKeySubtype;
    }
    if (name == "key_hybrid_a_length") {
        return &TestHybridEvaluationKeyALength;
    }
    if (name == "key_hybrid_b_length") {
        return &TestHybridEvaluationKeyBLength;
    }
    if (name == "key_hybrid_entry_basis") {
        return &TestHybridEvaluationKeyEntryBasis;
    }
    if (name == "key_hybrid_entry_format") {
        return &TestHybridEvaluationKeyEntryFormat;
    }
    if (name == "key_bv_zero_digit_a_length") {
        return &TestBVEvaluationKeyZeroDigitALength;
    }
    if (name == "key_bv_zero_digit_b_length") {
        return &TestBVEvaluationKeyZeroDigitBLength;
    }
    if (name == "key_bv_nonzero_digit_a_length") {
        return &TestBVEvaluationKeyNonzeroDigitALength;
    }
    if (name == "key_bv_nonzero_digit_b_length") {
        return &TestBVEvaluationKeyNonzeroDigitBLength;
    }
    if (name == "key_bv_zero_digit_entry_basis") {
        return &TestBVEvaluationKeyZeroDigitEntryBasis;
    }
    if (name == "key_bv_nonzero_digit_entry_basis") {
        return &TestBVEvaluationKeyNonzeroDigitEntryBasis;
    }
    if (name == "key_bv_zero_digit_entry_format") {
        return &TestBVEvaluationKeyZeroDigitEntryFormat;
    }
    throw TestFailure("unknown Relin2 test case: " + name);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Relin2 test failure: expected exactly one case name\n";
        return 2;
    }

    try {
        ResolveTest(argv[1])();
        lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
        std::cout << "Relin2 case passed: " << argv[1] << '\n';
        return 0;
    }
    catch (const TestFailure& failure) {
        std::cerr << "Relin2 test failure: " << failure.what() << '\n';
        return 1;
    }
    catch (const std::exception& exception) {
        std::cerr << "Relin2 unexpected exception: " << exception.what() << '\n';
        return 1;
    }
}
