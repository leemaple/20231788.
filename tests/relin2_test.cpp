#include "openfhe.h"
#include "openfhe_2023_1788/double_ckks.h"

#include <boost/multiprecision/cpp_int.hpp>

#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
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
                                    std::uint32_t digitSize = 0,
                                    std::uint32_t maxRelinSkDeg = 2) {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multiplicativeDepth);
    parameters.SetScalingModSize(30);
    parameters.SetFirstModSize(35);
    parameters.SetScalingTechnique(lbcrypto::FIXEDMANUAL);
    parameters.SetKeySwitchTechnique(keySwitchTechnique);
    parameters.SetDigitSize(digitSize);
    parameters.SetMaxRelinSkDeg(maxRelinSkDeg);
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

void TestBVEvaluationKeyNonzeroDigitEntryFormat() {
    auto context = MakeContext(3, 8, lbcrypto::BV, 10);
    const auto keys = context->KeyGen();
    auto leftPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.25, -0.5}, 2, 0);
    auto rightPlaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{-0.75, 0.125}, 2, 0);
    const auto leftInput = context->Encrypt(leftPlaintext, keys.publicKey);
    const auto rightInput = context->Encrypt(rightPlaintext, keys.publicKey);

    leftInput->SetMetadataByKey("relin2-bv-nonzero-digit-entry-format-immutability-probe",
                                std::make_shared<ImmutabilityProbeMetadata>("unchanged"));

    Check(leftInput->GetElements().front().GetAllElements().size() == 4,
          "Relin2 BV nonzero-digit entry-format fixture must have exactly four full-Q towers");

    DoubleCKKS module(context);
    const auto left = module.DCP(leftInput);
    const auto right = module.DCP(rightInput);
    const auto tensor = module.Tensor2(left, right);

    Check(tensor.GetOrderedModuli().size() == 3,
          "Relin2 BV nonzero-digit entry-format fixture must have exactly three active Q_l towers");
    Check(tensor.GetNoiseScaleDegree() == 3,
          "Relin2 BV nonzero-digit entry-format fixture must have noise-scale degree three");
    Check(!tensor.GetKeyTag().empty(),
          "Relin2 BV nonzero-digit entry-format fixture must have a nonempty key tag");
    Check(keys.secretKey->GetKeyTag() == tensor.GetKeyTag(),
          "Relin2 BV nonzero-digit entry-format fixture secret key must match the Tensor tag");
    const auto highMetadata = tensor.GetHigh()->GetMetadataMap();
    const auto lowMetadata = tensor.GetLow()->GetMetadataMap();
    Check(highMetadata != nullptr && highMetadata->size() == 1 &&
              highMetadata->find("relin2-bv-nonzero-digit-entry-format-immutability-probe") !=
                  highMetadata->end(),
          "Relin2 BV nonzero-digit entry-format fixture high ciphertext lost its metadata probe");
    Check(lowMetadata != nullptr && lowMetadata->size() == 1 &&
              lowMetadata->find("relin2-bv-nonzero-digit-entry-format-immutability-probe") !=
                  lowMetadata->end(),
          "Relin2 BV nonzero-digit entry-format fixture low ciphertext lost its metadata probe");

    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr,
          "Relin2 BV nonzero-digit entry-format fixture must expose CKKS-RNS parameters");
    Check(parameters->GetKeySwitchTechnique() == lbcrypto::BV,
          "Relin2 BV nonzero-digit entry-format fixture must use BV key switching");
    const auto digitSize = parameters->GetDigitSize();
    Check(digitSize == 10,
          "Relin2 BV nonzero-digit entry-format fixture must use digit size ten");
    const auto expectedBasis = parameters->GetElementParams();
    Check(expectedBasis != nullptr && expectedBasis->GetParams().size() == 4,
          "Relin2 BV nonzero-digit entry-format fixture must expose the complete four-tower Q basis");
    std::size_t expectedBVKeyLength = 0;
    for (const auto& towerParameters : expectedBasis->GetParams()) {
        Check(towerParameters != nullptr,
              "Relin2 BV nonzero-digit entry-format fixture has null full-Q tower parameters");
        const auto towerBits = towerParameters->GetModulus().GetMSB();
        expectedBVKeyLength +=
            static_cast<std::size_t>((towerBits + digitSize - 1) / digitSize);
    }
    Check(expectedBVKeyLength == 15 && expectedBVKeyLength > expectedBasis->GetParams().size(),
          "Relin2 BV nonzero-digit entry-format fixture must have fifteen decomposed digits");

    auto& evaluationKeys = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(evaluationKeys.empty(),
          "Relin2 BV nonzero-digit entry-format fixture must start with an empty evaluation-key cache");
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeys);
        context->EvalMultKeyGen(keys.secretKey);

        const auto generatedRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(evaluationKeys.size() == 1 && generatedRow != evaluationKeys.end() &&
                  generatedRow->second.size() == 1 && generatedRow->second.front() != nullptr,
              "Relin2 BV nonzero-digit entry-format fixture must generate exactly one evaluation key");
        const auto bvKey =
            std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(generatedRow->second.front());
        Check(bvKey != nullptr,
              "Relin2 BV nonzero-digit entry-format fixture must use the relinearization-key subtype");
        Check(bvKey->GetCryptoContext().get() == context.get(),
              "Relin2 BV nonzero-digit entry-format fixture key must keep the bound context");
        Check(bvKey->GetKeyTag() == tensor.GetKeyTag(),
              "Relin2 BV nonzero-digit entry-format fixture key must match the Tensor tag");

        const auto originalA = bvKey->GetAVector();
        const auto originalB = bvKey->GetBVector();
        Check(originalA.size() == expectedBVKeyLength && originalB.size() == expectedBVKeyLength,
              "Relin2 BV nonzero-digit entry-format fixture must start with exact generated A/B lengths");
        for (std::size_t index = 0; index < originalA.size(); ++index) {
            Check(originalA[index].GetFormat() == Format::EVALUATION,
                  "Relin2 BV nonzero-digit entry-format fixture generated a non-Evaluation A entry");
            CheckKeyPolynomialBasis(originalA[index], expectedBasis,
                                    "Relin2 BV nonzero-digit entry-format generated A entry " +
                                        std::to_string(index));
            for (const auto& tower : originalA[index].GetAllElements()) {
                Check(tower.GetFormat() == Format::EVALUATION,
                      "Relin2 BV nonzero-digit entry-format fixture generated a non-Evaluation A tower");
            }
        }
        for (std::size_t index = 0; index < originalB.size(); ++index) {
            Check(originalB[index].GetFormat() == Format::EVALUATION,
                  "Relin2 BV nonzero-digit entry-format fixture generated a non-Evaluation B entry");
            CheckKeyPolynomialBasis(originalB[index], expectedBasis,
                                    "Relin2 BV nonzero-digit entry-format generated B entry " +
                                        std::to_string(index));
            for (const auto& tower : originalB[index].GetAllElements()) {
                Check(tower.GetFormat() == Format::EVALUATION,
                      "Relin2 BV nonzero-digit entry-format fixture generated a non-Evaluation B tower");
            }
        }

        const auto* cacheIdentityBeforePositive = &evaluationKeys;
        const auto* vectorIdentityBeforePositive = &generatedRow->second;
        const auto keyIdentityBeforePositive = generatedRow->second.front();
        const auto keyContextBeforePositive = bvKey->GetCryptoContext();
        const auto keyTagBeforePositive = bvKey->GetKeyTag();
        const auto tensorBeforePositive = SnapshotTensor(tensor);
        const auto keyABeforePositive =
            SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV nonzero-digit entry-format positive A");
        const auto keyBBeforePositive =
            SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV nonzero-digit entry-format positive B");
        CheckPassesCurrentScaffoldOrCompletes(
            [&] { (void)module.Relin2(tensor); },
            "Relin2 BV nonzero-digit valid entry format");
        CheckTensorUnchanged(tensor, tensorBeforePositive,
                             "Relin2 BV nonzero-digit entry-format positive control");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyABeforePositive,
                                "Relin2 BV nonzero-digit entry-format positive A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBBeforePositive,
                                "Relin2 BV nonzero-digit entry-format positive B");
        const auto currentPositiveRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBeforePositive && evaluationKeys.size() == 1 &&
                  currentPositiveRow != evaluationKeys.end() &&
                  &currentPositiveRow->second == vectorIdentityBeforePositive &&
                  currentPositiveRow->second.size() == 1 &&
                  currentPositiveRow->second.front().get() == keyIdentityBeforePositive.get(),
              "Relin2 BV nonzero-digit entry-format positive control mutated the cache shape or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextBeforePositive.get(),
              "Relin2 BV nonzero-digit entry-format positive control mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagBeforePositive,
              "Relin2 BV nonzero-digit entry-format positive control mutated the key tag");

        auto malformedA = originalA;
        const auto targetBasisIdentity = malformedA.back().GetParams();
        malformedA.back().SetFormat(Format::COEFFICIENT);
        Check(malformedA.size() == expectedBVKeyLength &&
                  malformedA.back().GetFormat() == Format::COEFFICIENT &&
                  malformedA.back().GetParams().get() == targetBasisIdentity.get(),
              "Relin2 BV nonzero-digit entry-format negative control must change only the last A representation");
        CheckKeyPolynomialBasis(malformedA.back(), expectedBasis,
                                "Relin2 BV nonzero-digit Coefficient-format negative control");
        for (std::size_t index = 0; index < malformedA.back().GetAllElements().size(); ++index) {
            Check(malformedA.back().GetAllElements()[index].GetFormat() == Format::COEFFICIENT,
                  "Relin2 BV nonzero-digit entry-format negative control left an Evaluation tower");
            Check(malformedA.back().GetAllElements()[index].GetParams().get() ==
                      originalA.back().GetAllElements()[index].GetParams().get(),
                  "Relin2 BV nonzero-digit entry-format negative control changed a tower basis identity");
        }
        auto roundTrippedTarget = malformedA.back();
        roundTrippedTarget.SetFormat(Format::EVALUATION);
        Check(roundTrippedTarget == originalA.back() &&
                  roundTrippedTarget.GetFormat() == Format::EVALUATION,
              "Relin2 BV nonzero-digit entry-format negative control changed the represented polynomial");
        for (std::size_t index = 0; index + 1 < malformedA.size(); ++index) {
            Check(malformedA[index] == originalA[index] &&
                      malformedA[index].GetFormat() == Format::EVALUATION,
                  "Relin2 BV nonzero-digit entry-format negative control changed another A entry");
        }
        bvKey->SetAVector(std::move(malformedA));
        Check(bvKey->GetBVector() == originalB &&
                  bvKey->GetAVector().size() == expectedBVKeyLength &&
                  bvKey->GetAVector().back().GetFormat() == Format::COEFFICIENT &&
                  bvKey->GetAVector().back().GetParams().get() == targetBasisIdentity.get(),
              "Relin2 BV nonzero-digit entry-format negative control changed key shape, basis, or B entries");

        const auto* cacheIdentityBeforeNegative = &evaluationKeys;
        const auto* vectorIdentityBeforeNegative = &generatedRow->second;
        const auto keyIdentityBeforeNegative = generatedRow->second.front();
        const auto keyContextBeforeNegative = bvKey->GetCryptoContext();
        const auto keyTagBeforeNegative = bvKey->GetKeyTag();
        const auto tensorBeforeNegative = SnapshotTensor(tensor);
        const auto keyABeforeNegative =
            SnapshotKeyVector(bvKey->GetAVector(), "Relin2 BV nonzero-digit entry-format negative A");
        const auto keyBBeforeNegative =
            SnapshotKeyVector(bvKey->GetBVector(), "Relin2 BV nonzero-digit entry-format negative B");
        CheckThrowsExactInvalidArgument(
            [&] { (void)module.Relin2(tensor); },
            "DoubleCKKS: Relin2 evaluation key BV entry must be in evaluation format",
            "Relin2 BV nonzero-digit evaluation-key entry format");
        CheckTensorUnchanged(tensor, tensorBeforeNegative,
                             "Relin2 BV nonzero-digit entry-format rejection");
        CheckKeyVectorUnchanged(bvKey->GetAVector(), keyABeforeNegative,
                                "Relin2 BV nonzero-digit entry-format rejection A");
        CheckKeyVectorUnchanged(bvKey->GetBVector(), keyBBeforeNegative,
                                "Relin2 BV nonzero-digit entry-format rejection B");
        const auto currentNegativeRow = evaluationKeys.find(tensor.GetKeyTag());
        Check(&evaluationKeys == cacheIdentityBeforeNegative && evaluationKeys.size() == 1 &&
                  currentNegativeRow != evaluationKeys.end() &&
                  &currentNegativeRow->second == vectorIdentityBeforeNegative &&
                  currentNegativeRow->second.size() == 1 &&
                  currentNegativeRow->second.front().get() == keyIdentityBeforeNegative.get(),
              "Relin2 BV nonzero-digit entry-format rejection mutated the cache shape or identity");
        Check(bvKey->GetCryptoContext().get() == keyContextBeforeNegative.get(),
              "Relin2 BV nonzero-digit entry-format rejection mutated the key context");
        Check(bvKey->GetKeyTag() == keyTagBeforeNegative,
              "Relin2 BV nonzero-digit entry-format rejection mutated the key tag");
    }
    Check(evaluationKeys.empty(),
          "Relin2 BV nonzero-digit entry-format fixture failed to restore the initially empty cache");
}

namespace core_red {

using BigInt = boost::multiprecision::cpp_int;
using lbcrypto::Ciphertext;
using lbcrypto::NativeInteger;
using openfhe_2023_1788::CiphertextPair;
using openfhe_2023_1788::PaperScaleDescriptor;
using openfhe_2023_1788::PairLifecycle;

class ProbeMetadata final : public lbcrypto::Metadata {
public:
    explicit ProbeMetadata(std::string value) : value_(std::move(value)) {}

    std::shared_ptr<lbcrypto::Metadata> Clone() const override {
        return std::make_shared<ProbeMetadata>(value_);
    }

    bool operator==(const lbcrypto::Metadata& metadata) const override {
        const auto* other = dynamic_cast<const ProbeMetadata*>(&metadata);
        return other != nullptr && value_ == other->value_;
    }

private:
    std::string value_;
};

struct MetadataSnapshotEntry {
    std::string key;
    bool isNull = true;
    std::shared_ptr<lbcrypto::Metadata> pointerIdentity;
    std::shared_ptr<lbcrypto::Metadata> deepValue;
};

struct MetadataSnapshot {
    lbcrypto::MetadataMap outerMapIdentity;
    std::vector<MetadataSnapshotEntry> entries;
};

MetadataSnapshot SnapshotMetadata(lbcrypto::ConstCiphertext<DCRTPoly> ciphertext, const std::string& label) {
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");
    MetadataSnapshot snapshot{metadata, {}};
    snapshot.entries.reserve(metadata->size());
    for (const auto& [key, value] : *metadata) {
        MetadataSnapshotEntry entry;
        entry.key = key;
        entry.isNull = (value == nullptr);
        entry.pointerIdentity = value;
        if (value) {
            entry.deepValue = value->Clone();
            Check(entry.deepValue != nullptr, label + " metadata clone is null");
        }
        snapshot.entries.push_back(std::move(entry));
    }
    return snapshot;
}

void CheckMetadataUnchanged(lbcrypto::ConstCiphertext<DCRTPoly> ciphertext, const MetadataSnapshot& expected,
                            const std::string& label) {
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");
    Check(metadata.get() == expected.outerMapIdentity.get(), label + " metadata outer-map identity changed");
    Check(metadata->size() == expected.entries.size(), label + " metadata map size changed");
    auto current = metadata->begin();
    for (const auto& item : expected.entries) {
        Check(current != metadata->end(), label + " metadata entry missing");
        Check(current->first == item.key, label + " metadata key/order changed");
        Check((current->second == nullptr) == item.isNull, label + " metadata nullness changed");
        if (!item.isNull) {
            Check(current->second.get() == item.pointerIdentity.get(), label + " metadata value-pointer identity changed");
            Check(item.deepValue != nullptr && *(current->second) == *(item.deepValue),
                  label + " metadata deep value changed");
        }
        ++current;
    }
    Check(current == metadata->end(), label + " metadata gained trailing entries");
}

void CheckMetadataValueEquivalent(lbcrypto::ConstCiphertext<DCRTPoly> ciphertext, const MetadataSnapshot& expected,
                                  const std::string& label) {
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");
    Check(metadata->size() == expected.entries.size(), label + " metadata map size mismatch");
    auto current = metadata->begin();
    for (const auto& item : expected.entries) {
        Check(current != metadata->end(), label + " metadata entry missing");
        Check(current->first == item.key, label + " metadata key/order mismatch");
        Check((current->second == nullptr) == item.isNull, label + " metadata nullness mismatch");
        if (!item.isNull) {
            Check(item.deepValue != nullptr && *(current->second) == *(item.deepValue),
                  label + " metadata deep value mismatch");
        }
        ++current;
    }
    Check(current == metadata->end(), label + " metadata gained trailing entries");
}

void CheckMetadataShallowAliasExact(lbcrypto::ConstCiphertext<DCRTPoly> ciphertext,
                                    const MetadataSnapshot& source,
                                    const std::string& label) {
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");
    Check(metadata.get() != source.outerMapIdentity.get(), label + " metadata outer map aliases its source");
    Check(metadata->size() == source.entries.size(), label + " metadata map size mismatch");
    auto current = metadata->begin();
    for (const auto& item : source.entries) {
        Check(current != metadata->end(), label + " metadata entry missing");
        Check(current->first == item.key, label + " metadata key/order mismatch");
        Check((current->second == nullptr) == item.isNull, label + " metadata nullness mismatch");
        if (!item.isNull) {
            Check(current->second.get() == item.pointerIdentity.get(), label + " metadata value did not shallow-alias source");
            Check(item.deepValue != nullptr && *(current->second) == *(item.deepValue),
                  label + " metadata deep value mismatch");
        }
        ++current;
    }
    Check(current == metadata->end(), label + " metadata gained trailing entries");
}

void CheckMetadataShallowAlias(lbcrypto::ConstCiphertext<DCRTPoly> ciphertext, const MetadataSnapshot& source,
                               const std::string& forbiddenKey, const std::string& label) {
    CheckMetadataShallowAliasExact(ciphertext, source, label);
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata->find(forbiddenKey) == metadata->end(), label + " unexpectedly contains low-only metadata");
}

BigInt PositiveMod(BigInt value, const BigInt& modulus) {
    value %= modulus;
    if (value < 0) {
        value += modulus;
    }
    return value;
}

BigInt ExtendedGcd(BigInt a, BigInt b, BigInt& x, BigInt& y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    BigInt nextX;
    BigInt nextY;
    const BigInt gcd = ExtendedGcd(b, a % b, nextX, nextY);
    x = nextY;
    y = nextX - (a / b) * nextY;
    return gcd;
}

BigInt ModInverse(const BigInt& value, const BigInt& modulus) {
    BigInt x;
    BigInt y;
    const BigInt gcd = ExtendedGcd(PositiveMod(value, modulus), modulus, x, y);
    Check(gcd == 1, "CRT oracle received non-coprime moduli");
    return PositiveMod(x, modulus);
}

BigInt Product(const std::vector<BigInt>& values) {
    BigInt result = 1;
    for (const auto& value : values) {
        result *= value;
    }
    return result;
}

BigInt Center(const BigInt& residue, const BigInt& modulus) {
    BigInt centered = PositiveMod(residue, modulus);
    if (centered > modulus / 2) {
        centered -= modulus;
    }
    return centered;
}

BigInt ReconstructCentered(const std::vector<BigInt>& residues, const std::vector<BigInt>& moduli) {
    const BigInt modulus = Product(moduli);
    BigInt result = 0;
    for (std::size_t i = 0; i < moduli.size(); ++i) {
        const BigInt partial = modulus / moduli[i];
        result += PositiveMod(residues[i], moduli[i]) * partial * ModInverse(partial, moduli[i]);
    }
    return Center(result, modulus);
}

std::vector<BigInt> GetModuli(const DCRTPoly& polynomial) {
    std::vector<BigInt> result;
    for (const auto& tower : polynomial.GetAllElements()) {
        result.emplace_back(tower.GetModulus().ConvertToInt());
    }
    return result;
}

std::vector<NativeInteger> GetNativeModuli(const DCRTPoly& polynomial) {
    std::vector<NativeInteger> result;
    for (const auto& tower : polynomial.GetAllElements()) {
        result.push_back(tower.GetModulus());
    }
    return result;
}

DCRTPoly ToCoefficient(const DCRTPoly& polynomial) {
    DCRTPoly result(polynomial);
    result.SetFormat(Format::COEFFICIENT);
    return result;
}

BigInt CoefficientResidue(const DCRTPoly& coefficientPolynomial, std::size_t tower, std::size_t coefficient) {
    return BigInt(coefficientPolynomial.GetAllElements().at(tower).GetValues().at(coefficient).ConvertToInt());
}

BigInt ReconstructCoefficient(const DCRTPoly& polynomial, std::size_t coefficient) {
    const auto coeff = ToCoefficient(polynomial);
    const auto moduli = GetModuli(coeff);
    std::vector<BigInt> residues;
    for (std::size_t tower = 0; tower < moduli.size(); ++tower) {
        residues.push_back(CoefficientResidue(coeff, tower, coefficient));
    }
    return ReconstructCentered(residues, moduli);
}

std::pair<BigInt, BigInt> DecomposeCentered(const BigInt& value, const BigInt& divisor) {
    BigInt remainder = PositiveMod(value, divisor);
    if (remainder > divisor / 2) {
        remainder -= divisor;
    }
    const BigInt quotient = (value - remainder) / divisor;
    Check(value == divisor * quotient + remainder, "centered DCP identity failed");
    return {quotient, remainder};
}

DCRTPoly MakePolynomial(const std::shared_ptr<DCRTPoly::Params>& params, const std::vector<BigInt>& coefficients) {
    DCRTPoly result(params, Format::COEFFICIENT, true);
    const std::size_t n = params->GetRingDimension();
    Check(coefficients.size() == n, "coefficient fixture length mismatch");
    for (auto& tower : result.GetAllElements()) {
        const BigInt modulus(tower.GetModulus().ConvertToInt());
        lbcrypto::NativeVector values(n, tower.GetModulus());
        for (std::size_t i = 0; i < n; ++i) {
            values[i] = NativeInteger(PositiveMod(coefficients[i], modulus).convert_to<std::uint64_t>());
        }
        tower.SetValues(std::move(values), Format::COEFFICIENT);
    }
    result.SetFormat(Format::EVALUATION);
    return result;
}

DCRTPoly MakeTowerPolynomial(const std::shared_ptr<DCRTPoly::Params>& params,
                             const std::vector<std::vector<BigInt>>& coefficients) {
    DCRTPoly result(params, Format::COEFFICIENT, true);
    auto& towers = result.GetAllElements();
    Check(coefficients.size() == towers.size(), "tower fixture count mismatch");
    const std::size_t n = params->GetRingDimension();
    for (std::size_t towerIndex = 0; towerIndex < towers.size(); ++towerIndex) {
        Check(coefficients[towerIndex].size() == n, "tower coefficient fixture length mismatch");
        const BigInt modulus(towers[towerIndex].GetModulus().ConvertToInt());
        lbcrypto::NativeVector values(n, towers[towerIndex].GetModulus());
        for (std::size_t i = 0; i < n; ++i) {
            values[i] = NativeInteger(PositiveMod(coefficients[towerIndex][i], modulus).convert_to<std::uint64_t>());
        }
        towers[towerIndex].SetValues(std::move(values), Format::COEFFICIENT);
    }
    result.SetFormat(Format::EVALUATION);
    return result;
}

std::vector<BigInt> ComposeSource(const BigInt& divisor, const std::vector<BigInt>& high,
                                  const std::vector<BigInt>& low) {
    Check(high.size() == low.size(), "source fixture length mismatch");
    std::vector<BigInt> result(high.size());
    for (std::size_t i = 0; i < high.size(); ++i) {
        result[i] = divisor * high[i] + low[i];
    }
    return result;
}


CryptoContext<DCRTPoly> MakeRelinContext(
    lbcrypto::KeySwitchTechnique technique = lbcrypto::HYBRID,
    std::uint32_t digitSize = 0,
    std::uint32_t depth = 3,
    std::uint32_t maxRelinSkDeg = 2) {
    return MakeContext(depth, 8, technique, digitSize, maxRelinSkDeg);
}

struct TensorFixture {
    CryptoContext<DCRTPoly> context;
    lbcrypto::KeyPair<DCRTPoly> keys;
    Ciphertext<DCRTPoly> leftInput;
    Ciphertext<DCRTPoly> rightInput;
};

TensorFixture MakeExactTensorFixture(const CryptoContext<DCRTPoly>& context) {
    TensorFixture fixture;
    fixture.context = context;
    fixture.keys = context->KeyGen();
    const auto plaintext = context->MakeCKKSPackedPlaintext(std::vector<double>{0.0}, 2, 0);
    fixture.leftInput = context->Encrypt(plaintext, fixture.keys.publicKey);
    fixture.rightInput = context->Encrypt(plaintext, fixture.keys.publicKey);
    const auto params = fixture.leftInput->GetElements().front().GetParams();
    const std::size_t n = params->GetRingDimension();
    const auto moduli = GetModuli(fixture.leftInput->GetElements().front());
    Check(moduli.size() >= 3, "Relin2 exact fixture requires at least three full-basis towers");
    const BigInt divisor = moduli.back();

    std::vector<BigInt> lh0(n, 0), ll0(n, 0), lh1(n, 0), ll1(n, 0);
    std::vector<BigInt> rh0(n, 0), rl0(n, 0), rh1(n, 0), rl1(n, 0);
    lh0[n - 1] = 1;
    rh0[1] = 1;
    lh1[0] = -1000003;
    rh1[0] = 1000033;
    ll0[2] = 17;
    rl0[3] = -19;
    ll1[4] = -23;
    rl1[5] = 29;
    fixture.leftInput->SetElements({MakePolynomial(params, ComposeSource(divisor, lh0, ll0)),
                                    MakePolynomial(params, ComposeSource(divisor, lh1, ll1))});
    fixture.rightInput->SetElements({MakePolynomial(params, ComposeSource(divisor, rh0, rl0)),
                                     MakePolynomial(params, ComposeSource(divisor, rh1, rl1))});
    return fixture;
}

TensorFixture MakeRepresentativePublicFixture(const CryptoContext<DCRTPoly>& context) {
    TensorFixture fixture;
    fixture.context = context;
    fixture.keys = context->KeyGen();
    const std::vector<std::complex<double>> leftValues{{1.25, -0.5}, {-2.0, 0.75}, {1.0e-8, -2.0e-8}, {12.5, 3.25}};
    const std::vector<std::complex<double>> rightValues{{-0.75, 1.0}, {3.5, -1.25}, {-3.0e-8, 1.0e-8}, {7.0, -2.5}};
    const auto leftPlain = context->MakeCKKSPackedPlaintext(leftValues, 2, 0);
    const auto rightPlain = context->MakeCKKSPackedPlaintext(rightValues, 2, 0);
    fixture.leftInput = context->Encrypt(leftPlain, fixture.keys.publicKey);
    fixture.rightInput = context->Encrypt(rightPlain, fixture.keys.publicKey);
    return fixture;
}

std::pair<CiphertextPair, CiphertextPair> MakePairs(TensorFixture& fixture, DoubleCKKS& module) {
    fixture.leftInput->SetMetadataByKey("relin2-left-input", std::make_shared<ProbeMetadata>("left-input"));
    fixture.rightInput->SetMetadataByKey("relin2-right-input", std::make_shared<ProbeMetadata>("right-input"));
    return {module.DCP(fixture.leftInput), module.DCP(fixture.rightInput)};
}

void TagTensorMetadata(const TensorCiphertextPair& tensor) {
    auto high = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(tensor.GetHigh());
    auto low = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(tensor.GetLow());
    high->SetMetadataByKey("relin2-tensor-high", std::make_shared<ProbeMetadata>("high-source"));
    low->SetMetadataByKey("relin2-tensor-low", std::make_shared<ProbeMetadata>("low-source"));
    const auto highMap = high->GetMetadataMap();
    const auto lowMap = low->GetMetadataMap();
    Check(highMap != nullptr && lowMap != nullptr, "Tensor metadata provenance fixture has a null map");
    Check(highMap.get() != lowMap.get(), "Tensor high/low metadata maps unexpectedly alias");
    Check(highMap->find("relin2-tensor-high") != highMap->end(), "Tensor high provenance metadata missing");
    Check(highMap->find("relin2-tensor-low") == highMap->end(), "Tensor high unexpectedly contains low-only metadata");
    Check(lowMap->find("relin2-tensor-low") != lowMap->end(), "Tensor low provenance metadata missing");
    Check(lowMap->find("relin2-tensor-high") == lowMap->end(), "Tensor low unexpectedly contains high-only metadata");
}

bool HasNonzeroElement(const DCRTPoly& polynomial) {
    for (const auto& tower : polynomial.GetAllElements()) {
        for (std::size_t index = 0; index < tower.GetValues().GetLength(); ++index) {
            if (tower.GetValues().at(index).ConvertToInt() != 0) {
                return true;
            }
        }
    }
    return false;
}

void CheckNonzeroThreeComponentTensor(const TensorCiphertextPair& tensor,
                                      const std::string& label) {
    Check(tensor.GetComponentCount() == 3 &&
              tensor.GetHigh()->GetElements().size() == 3 &&
              tensor.GetLow()->GetElements().size() == 3,
          label + " is not a three-component Tensor");
    for (std::size_t component = 0; component < 3; ++component) {
        Check(HasNonzeroElement(tensor.GetHigh()->GetElements()[component]),
              label + " high component " + std::to_string(component) + " is zero");
        Check(HasNonzeroElement(tensor.GetLow()->GetElements()[component]),
              label + " low component " + std::to_string(component) + " is zero");
    }
}

struct CiphertextSnapshot {
    ReadOnlyCiphertext identity;
    Ciphertext<DCRTPoly> clone;
    MetadataSnapshot metadata;
    KeyVectorSnapshot elements;
};

void CheckCiphertextDeepUnchanged(lbcrypto::ConstCiphertext<DCRTPoly> ciphertext, const CiphertextSnapshot& before,
                                  const std::string& label) {
    Check(ciphertext != nullptr && before.identity != nullptr && before.clone != nullptr,
          label + " ciphertext or snapshot is null");
    const auto& actualElements = ciphertext->GetElements();
    const auto& beforeElements = before.clone->GetElements();
    Check(ciphertext.get() == before.identity.get(), label + " ciphertext identity changed");
    Check(actualElements.size() == beforeElements.size(), label + " component count changed");
    for (std::size_t component = 0; component < actualElements.size(); ++component) {
        Check(actualElements[component].GetFormat() == beforeElements[component].GetFormat(),
              label + " aggregate DCRT format changed");
        const auto& actualTowers = actualElements[component].GetAllElements();
        const auto& beforeTowers = beforeElements[component].GetAllElements();
        Check(actualTowers.size() == beforeTowers.size(), label + " tower count changed");
        for (std::size_t tower = 0; tower < actualTowers.size(); ++tower) {
            Check(actualTowers[tower].GetFormat() == beforeTowers[tower].GetFormat(),
                  label + " NativePoly tower format changed");
        }
    }
    Check(*ciphertext == *before.clone, label + " ciphertext changed");
    CheckKeyVectorUnchanged(actualElements, before.elements, label + " elements");
    CheckMetadataUnchanged(ciphertext, before.metadata, label);
}

struct TensorSnapshot {
    CiphertextSnapshot high;
    CiphertextSnapshot low;
    const lbcrypto::CryptoContextImpl<DCRTPoly>* contextIdentity;
    NativeInteger divisor;
    std::vector<NativeInteger> orderedModuli;
    std::size_t level;
    TensorScaleDescriptor tensorScale;
    double recordedScalingFactor;
    std::size_t noiseScaleDegree;
    std::string keyTag;
    std::uint32_t slots;
    Format format;
    std::size_t componentCount;
};

TensorSnapshot SnapshotTensor(const TensorCiphertextPair& tensor, const std::string& label) {
    return {{tensor.GetHigh(), tensor.GetHigh()->Clone(), SnapshotMetadata(tensor.GetHigh(), label + " high"),
             SnapshotKeyVector(tensor.GetHigh()->GetElements(), label + " high elements")},
            {tensor.GetLow(), tensor.GetLow()->Clone(), SnapshotMetadata(tensor.GetLow(), label + " low"),
             SnapshotKeyVector(tensor.GetLow()->GetElements(), label + " low elements")},
            tensor.GetContextIdentity(), tensor.GetDivisor(), tensor.GetOrderedModuli(), tensor.GetLevel(),
            tensor.GetTensorScale(), tensor.GetRecordedScalingFactor(), tensor.GetNoiseScaleDegree(),
            tensor.GetKeyTag(), tensor.GetSlots(), tensor.GetFormat(), tensor.GetComponentCount()};
}

void CheckTensorUnchanged(const TensorCiphertextPair& tensor, const TensorSnapshot& before, const std::string& label) {
    CheckCiphertextDeepUnchanged(tensor.GetHigh(), before.high, label + " high");
    CheckCiphertextDeepUnchanged(tensor.GetLow(), before.low, label + " low");
    Check(tensor.GetContextIdentity() == before.contextIdentity, label + " context manifest changed");
    Check(tensor.GetDivisor() == before.divisor, label + " divisor manifest changed");
    Check(tensor.GetOrderedModuli() == before.orderedModuli, label + " basis manifest changed");
    Check(tensor.GetLevel() == before.level, label + " level manifest changed");
    Check(tensor.GetTensorScale().approximateHighLogicalScalingFactor ==
              before.tensorScale.approximateHighLogicalScalingFactor,
          label + " high logical scale changed");
    Check(tensor.GetTensorScale().approximateRecombinedLogicalScalingFactor ==
              before.tensorScale.approximateRecombinedLogicalScalingFactor,
          label + " recombined logical scale changed");
    Check(tensor.GetRecordedScalingFactor() == before.recordedScalingFactor, label + " recorded factor changed");
    Check(tensor.GetNoiseScaleDegree() == before.noiseScaleDegree, label + " degree changed");
    Check(tensor.GetKeyTag() == before.keyTag, label + " key tag changed");
    Check(tensor.GetSlots() == before.slots, label + " slots changed");
    Check(tensor.GetFormat() == before.format, label + " format changed");
    Check(tensor.GetComponentCount() == before.componentCount, label + " component count changed");
}

struct PairSnapshot {
    CiphertextSnapshot high;
    CiphertextSnapshot low;
    const lbcrypto::CryptoContextImpl<DCRTPoly>* contextIdentity;
    NativeInteger divisor;
    std::vector<NativeInteger> orderedModuli;
    std::size_t level;
    PaperScaleDescriptor paperScale;
    double recordedScalingFactor;
    std::size_t noiseScaleDegree;
    PairLifecycle lifecycle;
    std::string keyTag;
    std::uint32_t slots;
    Format format;
    std::size_t componentCount;
};

PairSnapshot SnapshotPair(const CiphertextPair& pair, const std::string& label) {
    return {{pair.GetHigh(), pair.GetHigh()->Clone(), SnapshotMetadata(pair.GetHigh(), label + " high"),
             SnapshotKeyVector(pair.GetHigh()->GetElements(), label + " high elements")},
            {pair.GetLow(), pair.GetLow()->Clone(), SnapshotMetadata(pair.GetLow(), label + " low"),
             SnapshotKeyVector(pair.GetLow()->GetElements(), label + " low elements")},
            pair.GetContextIdentity(), pair.GetDivisor(), pair.GetOrderedModuli(), pair.GetLevel(),
            pair.GetPaperScale(), pair.GetRecordedScalingFactor(), pair.GetNoiseScaleDegree(), pair.GetLifecycle(),
            pair.GetKeyTag(), pair.GetSlots(), pair.GetFormat(), pair.GetComponentCount()};
}

void CheckPairUnchanged(const CiphertextPair& pair, const PairSnapshot& before, const std::string& label) {
    CheckCiphertextDeepUnchanged(pair.GetHigh(), before.high, label + " high");
    CheckCiphertextDeepUnchanged(pair.GetLow(), before.low, label + " low");
    Check(pair.GetContextIdentity() == before.contextIdentity, label + " context manifest changed");
    Check(pair.GetDivisor() == before.divisor, label + " divisor manifest changed");
    Check(pair.GetOrderedModuli() == before.orderedModuli, label + " basis manifest changed");
    Check(pair.GetLevel() == before.level, label + " level manifest changed");
    Check(pair.GetPaperScale().inputRecordedScalingFactor == before.paperScale.inputRecordedScalingFactor,
          label + " input scale changed");
    Check(pair.GetPaperScale().divisor == before.paperScale.divisor, label + " paper divisor changed");
    Check(pair.GetPaperScale().approximateLogicalScalingFactor == before.paperScale.approximateLogicalScalingFactor,
          label + " high logical scale changed");
    Check(pair.GetPaperScale().approximateRecombinedLogicalScalingFactor ==
              before.paperScale.approximateRecombinedLogicalScalingFactor,
          label + " recombined logical scale changed");
    Check(pair.GetRecordedScalingFactor() == before.recordedScalingFactor, label + " recorded factor changed");
    Check(pair.GetNoiseScaleDegree() == before.noiseScaleDegree, label + " degree changed");
    Check(pair.GetLifecycle() == before.lifecycle, label + " lifecycle changed");
    Check(pair.GetKeyTag() == before.keyTag, label + " key tag changed");
    Check(pair.GetSlots() == before.slots, label + " slots changed");
    Check(pair.GetFormat() == before.format, label + " format changed");
    Check(pair.GetComponentCount() == before.componentCount, label + " component count changed");
}

struct TowerSnapshot {
    NativeInteger modulus;
    NativeInteger root;
    std::uint32_t cyclotomicOrder;
    Format format;
    std::vector<std::uint64_t> values;
};

struct DcrtSnapshot {
    Format format;
    std::vector<TowerSnapshot> towers;
};

DcrtSnapshot SnapshotDcrt(const DCRTPoly& polynomial) {
    DcrtSnapshot snapshot{polynomial.GetFormat(), {}};
    for (const auto& tower : polynomial.GetAllElements()) {
        TowerSnapshot entry{tower.GetModulus(), tower.GetRootOfUnity(), tower.GetCyclotomicOrder(), tower.GetFormat(), {}};
        entry.values.reserve(tower.GetValues().GetLength());
        for (std::size_t i = 0; i < tower.GetValues().GetLength(); ++i) {
            entry.values.push_back(tower.GetValues().at(i).ConvertToInt());
        }
        snapshot.towers.push_back(std::move(entry));
    }
    return snapshot;
}

bool SameDcrtSnapshot(const DcrtSnapshot& left, const DcrtSnapshot& right) {
    if (left.format != right.format || left.towers.size() != right.towers.size()) {
        return false;
    }
    for (std::size_t i = 0; i < left.towers.size(); ++i) {
        const auto& a = left.towers[i];
        const auto& b = right.towers[i];
        if (a.modulus != b.modulus || a.root != b.root || a.cyclotomicOrder != b.cyclotomicOrder ||
            a.format != b.format || a.values != b.values) {
            return false;
        }
    }
    return true;
}

struct KeyEntrySnapshot {
    bool isNull = true;
    const void* pointerIdentity = nullptr;
    const lbcrypto::CryptoContextImpl<DCRTPoly>* contextIdentity = nullptr;
    std::string actualTag;
    std::string concreteSubtype;
    bool isRelin = false;
    KeyVectorSnapshot a;
    KeyVectorSnapshot b;
};

using DeepKeyRows = std::map<std::string, std::vector<KeyEntrySnapshot>>;

struct DeepKeyCacheSnapshot {
    const EvalMultKeyMap* mapIdentity;
    std::map<std::string, const std::vector<lbcrypto::EvalKey<DCRTPoly>>*> rowIdentities;
    DeepKeyRows rows;
};

DeepKeyCacheSnapshot SnapshotDeepKeyCache() {
    const auto& cache = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    DeepKeyCacheSnapshot result{&cache, {}, {}};
    for (const auto& [mapTag, row] : cache) {
        result.rowIdentities[mapTag] = &row;
        auto& output = result.rows[mapTag];
        for (const auto& key : row) {
            KeyEntrySnapshot entry;
            if (!key) {
                output.push_back(std::move(entry));
                continue;
            }
            entry.isNull = false;
            entry.pointerIdentity = key.get();
            entry.contextIdentity = key->GetCryptoContext().get();
            entry.actualTag = key->GetKeyTag();
            entry.concreteSubtype = typeid(*key).name();
            const auto relin = std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(key);
            if (relin) {
                entry.isRelin = true;
                entry.a = SnapshotKeyVector(relin->GetAVector(), "evaluation-key cache A");
                entry.b = SnapshotKeyVector(relin->GetBVector(), "evaluation-key cache B");
            }
            output.push_back(std::move(entry));
        }
    }
    return result;
}

void CheckDeepKeyCacheMatches(const DeepKeyCacheSnapshot& expected, const std::string& label) {
    const auto& actual = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(&actual == expected.mapIdentity, label + " cache map identity changed");
    Check(actual.size() == expected.rows.size(), label + " cache row count changed");
    auto ai = actual.begin();
    auto ei = expected.rows.begin();
    for (; ei != expected.rows.end(); ++ei, ++ai) {
        Check(ai != actual.end() && ai->first == ei->first, label + " cache row tag changed");
        Check(&ai->second == expected.rowIdentities.at(ei->first),
              label + " cache row-vector identity changed");
        Check(ai->second.size() == ei->second.size(), label + " cache row length changed");
        for (std::size_t j = 0; j < ei->second.size(); ++j) {
            const auto& e = ei->second[j];
            const auto& key = ai->second[j];
            Check((key == nullptr) == e.isNull, label + " cache nullness changed");
            if (e.isNull) {
                continue;
            }
            Check(key.get() == e.pointerIdentity, label + " cache pointer identity changed");
            Check(key->GetCryptoContext().get() == e.contextIdentity, label + " key context identity changed");
            Check(key->GetKeyTag() == e.actualTag, label + " actual key tag changed");
            Check(typeid(*key).name() == e.concreteSubtype, label + " concrete key subtype changed");
            const auto relin = std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(key);
            Check((relin != nullptr) == e.isRelin, label + " relin subtype classification changed");
            if (relin) {
                CheckKeyVectorUnchanged(relin->GetAVector(), e.a, label + " A-vector");
                CheckKeyVectorUnchanged(relin->GetBVector(), e.b, label + " B-vector");
            }
        }
    }
    Check(ai == actual.end(), label + " cache gained trailing rows");
}

DCRTPoly RaiseElement(const DCRTPoly& source, const NativeInteger& divisor,
                      const std::shared_ptr<DCRTPoly::Params>& fullParams) {
    auto towers = source.GetAllElements();
    for (auto& tower : towers) {
        tower *= divisor;
    }
    lbcrypto::NativePoly zero(fullParams->GetParams().back(), Format::EVALUATION, true);
    towers.push_back(std::move(zero));
    return DCRTPoly(towers);
}

Ciphertext<DCRTPoly> RaiseHighReference(const TensorCiphertextPair& tensor,
                                        const CryptoContext<DCRTPoly>& context) {
    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr, "reference context is not CKKS-RNS");
    auto raised = tensor.GetHigh()->Clone();
    auto elements = raised->GetElements();
    for (auto& element : elements) {
        element = RaiseElement(element, tensor.GetDivisor(), parameters->GetElementParams());
    }
    raised->SetElements(std::move(elements));
    raised->SetLevel(0);
    return raised;
}

struct ReferenceRelinPaths {
    Ciphertext<DCRTPoly> raisedHigh;
    Ciphertext<DCRTPoly> relinearizedHigh;
    Ciphertext<DCRTPoly> relinearizedLow;
};

ReferenceRelinPaths BuildReferenceRelinPaths(const TensorCiphertextPair& tensor,
                                              const CryptoContext<DCRTPoly>& context) {
    ReferenceRelinPaths result;
    result.raisedHigh = RaiseHighReference(tensor, context);
    lbcrypto::ConstCiphertext<DCRTPoly> raisedConst = result.raisedHigh;
    result.relinearizedHigh = context->Relinearize(raisedConst);
    lbcrypto::ConstCiphertext<DCRTPoly> lowConst = tensor.GetLow();
    result.relinearizedLow = context->Relinearize(lowConst);
    return result;
}

void CheckMemberState(lbcrypto::ConstCiphertext<DCRTPoly> member, const TensorCiphertextPair& tensor,
                      const CryptoContext<DCRTPoly>& context, const MetadataSnapshot& expectedHighMetadata,
                      const std::string& label) {
    Check(member != nullptr, label + " is null");
    Check(member->GetCryptoContext().get() == context.get(), label + " context identity mismatch");
    Check(member->GetEncodingType() == lbcrypto::CKKS_PACKED_ENCODING, label + " encoding mismatch");
    Check(member->GetLevel() == 1, label + " level mismatch");
    Check(member->NumberCiphertextElements() == 2, label + " component count mismatch");
    Check(member->GetNoiseScaleDeg() == 3, label + " noise-scale degree mismatch");
    Check(member->GetScalingFactor() == tensor.GetRecordedScalingFactor(), label + " scaling factor mismatch");
    Check(member->GetKeyTag() == tensor.GetKeyTag(), label + " key tag mismatch");
    Check(member->GetSlots() == tensor.GetSlots(), label + " slots mismatch");
    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr, label + " missing CKKS parameters");
    const auto& expectedTowers = parameters->GetElementParams()->GetParams();
    const auto expectedBasis = tensor.GetLow()->GetElements().front().GetParams();
    for (std::size_t component = 0; component < member->GetElements().size(); ++component) {
        const auto& element = member->GetElements()[component];
        CheckKeyPolynomialBasis(element, expectedBasis,
                                label + " component " + std::to_string(component));
        Check(element.GetFormat() == Format::EVALUATION, label + " element format mismatch");
        Check(GetNativeModuli(element) == tensor.GetOrderedModuli(), label + " ordered basis mismatch");
        const auto& towers = element.GetAllElements();
        Check(towers.size() == tensor.GetOrderedModuli().size(), label + " tower count mismatch");
        for (std::size_t i = 0; i < towers.size(); ++i) {
            Check(towers[i].GetFormat() == Format::EVALUATION, label + " NativePoly tower format mismatch");
            Check(towers[i].GetModulus() == expectedTowers[i]->GetModulus(), label + " tower modulus mismatch");
            Check(towers[i].GetRootOfUnity() == expectedTowers[i]->GetRootOfUnity(), label + " tower root mismatch");
            Check(towers[i].GetCyclotomicOrder() == expectedTowers[i]->GetCyclotomicOrder(),
                  label + " tower cyclotomic order mismatch");
        }
    }
    CheckMetadataShallowAlias(member, expectedHighMetadata, "relin2-tensor-low", label);
}

void CheckResultState(const CiphertextPair& result, const TensorCiphertextPair& tensor,
                      const CryptoContext<DCRTPoly>& context, const MetadataSnapshot& expectedHighMetadata,
                      const MetadataSnapshot& expectedLowMetadata) {
    static_assert(std::is_same_v<decltype(result), const CiphertextPair&>);
    Check(result.GetLifecycle() == PairLifecycle::ReadyForRS2, "Relin2 lifecycle mismatch");
    Check(result.GetContextIdentity() == context.get(), "Relin2 context identity mismatch");
    Check(result.GetDivisor() == tensor.GetDivisor(), "Relin2 divisor mismatch");
    Check(result.GetKeyTag() == tensor.GetKeyTag(), "Relin2 actual key tag mismatch");
    Check(result.GetSlots() == tensor.GetSlots(), "Relin2 slots mismatch");
    Check(result.GetFormat() == Format::EVALUATION, "Relin2 pair format mismatch");
    Check(result.GetLevel() == 1, "Relin2 pair level mismatch");
    Check(result.GetComponentCount() == 2, "Relin2 pair component-count manifest mismatch");
    Check(result.GetNoiseScaleDegree() == 3, "Relin2 pair degree mismatch");
    Check(result.GetRecordedScalingFactor() == tensor.GetRecordedScalingFactor(), "Relin2 SF_T mismatch");
    Check(result.GetOrderedModuli() == tensor.GetOrderedModuli(), "Relin2 ordered Q_l manifest mismatch");
    Check(result.GetPaperScale().inputRecordedScalingFactor == tensor.GetRecordedScalingFactor(),
          "Relin2 paper recorded factor mismatch");
    Check(result.GetPaperScale().divisor == tensor.GetDivisor(), "Relin2 paper divisor mismatch");
    Check(result.GetPaperScale().approximateLogicalScalingFactor ==
              tensor.GetTensorScale().approximateHighLogicalScalingFactor,
          "Relin2 copied high logical scale mismatch");
    Check(result.GetPaperScale().approximateRecombinedLogicalScalingFactor ==
              tensor.GetTensorScale().approximateRecombinedLogicalScalingFactor,
          "Relin2 copied recombined logical scale mismatch");
    Check(result.GetHigh() != nullptr && result.GetLow() != nullptr,
          "Relin2 result contains a null ciphertext member");

    const auto resultHighMap = result.GetHigh()->GetMetadataMap();
    const auto resultLowMap = result.GetLow()->GetMetadataMap();
    Check(resultHighMap != nullptr && resultLowMap != nullptr, "Relin2 result metadata map is null");
    Check(resultHighMap.get() != resultLowMap.get(), "Relin2 high/low metadata outer maps alias");
    Check(resultHighMap.get() != expectedHighMetadata.outerMapIdentity.get(),
          "Relin2 high metadata outer map aliases Tensor high");
    Check(resultLowMap.get() != expectedHighMetadata.outerMapIdentity.get(),
          "Relin2 low metadata outer map aliases Tensor high");
    Check(resultHighMap.get() != expectedLowMetadata.outerMapIdentity.get(),
          "Relin2 high metadata outer map aliases Tensor low");
    Check(resultLowMap.get() != expectedLowMetadata.outerMapIdentity.get(),
          "Relin2 low metadata outer map aliases Tensor low");

    CheckMemberState(result.GetHigh(), tensor, context, expectedHighMetadata, "Relin2 high");
    CheckMemberState(result.GetLow(), tensor, context, expectedHighMetadata, "Relin2 low");
}

void CheckExactRelin2Oracle(const TensorCiphertextPair& tensor, const CiphertextPair& actual,
                            const ReferenceRelinPaths& reference) {
    Check(actual.GetHigh() != nullptr && actual.GetLow() != nullptr,
          "Relin2 exact oracle received a null result member");
    const BigInt divisor(tensor.GetDivisor().ConvertToInt());
    const std::size_t n = reference.relinearizedHigh->GetElements().front().GetParams()->GetRingDimension();
    for (std::size_t component = 0; component < 2; ++component) {
        const auto actualHigh = ToCoefficient(actual.GetHigh()->GetElements().at(component));
        const auto actualLow = ToCoefficient(actual.GetLow()->GetElements().at(component));
        const auto relinLow = ToCoefficient(reference.relinearizedLow->GetElements().at(component));
        const auto moduli = GetModuli(actualHigh);
        for (std::size_t coefficient = 0; coefficient < n; ++coefficient) {
            const BigInt source = ReconstructCoefficient(reference.relinearizedHigh->GetElements().at(component),
                                                         coefficient);
            const auto decomposition = DecomposeCentered(source, divisor);
            for (std::size_t tower = 0; tower < moduli.size(); ++tower) {
                const BigInt w = CoefficientResidue(relinLow, tower, coefficient);
                Check(CoefficientResidue(actualHigh, tower, coefficient) ==
                          PositiveMod(decomposition.first, moduli[tower]),
                      "Relin2 quotient mismatch at component=" + std::to_string(component) +
                          ",tower=" + std::to_string(tower) + ",coefficient=" + std::to_string(coefficient));
                Check(CoefficientResidue(actualLow, tower, coefficient) ==
                          PositiveMod(decomposition.second + w, moduli[tower]),
                      "Relin2 v+w mismatch at component=" + std::to_string(component) +
                          ",tower=" + std::to_string(tower) + ",coefficient=" + std::to_string(coefficient));
            }
        }
    }
}

Ciphertext<DCRTPoly> BuildRcbReference(const ReferenceRelinPaths& reference) {
    auto expected = reference.relinearizedHigh->Clone();
    auto elements = expected->GetElements();
    for (auto& element : elements) {
        element.DropLastElement();
    }
    const auto& low = reference.relinearizedLow->GetElements();
    Check(elements.size() == low.size(), "RCB reference component mismatch");
    for (std::size_t i = 0; i < elements.size(); ++i) {
        elements[i] += low[i];
    }
    expected->SetElements(std::move(elements));
    expected->SetLevel(1);
    return expected;
}

void CheckCiphertextExactly(lbcrypto::ConstCiphertext<DCRTPoly> actual, lbcrypto::ConstCiphertext<DCRTPoly> expected,
                            const std::string& label) {
    Check(actual->GetCryptoContext().get() == expected->GetCryptoContext().get(), label + " context mismatch");
    Check(actual->GetEncodingType() == expected->GetEncodingType(), label + " encoding mismatch");
    Check(actual->GetLevel() == expected->GetLevel(), label + " level mismatch");
    Check(actual->GetNoiseScaleDeg() == expected->GetNoiseScaleDeg(), label + " degree mismatch");
    Check(actual->GetScalingFactor() == expected->GetScalingFactor(), label + " factor mismatch");
    Check(actual->GetKeyTag() == expected->GetKeyTag(), label + " tag mismatch");
    Check(actual->GetSlots() == expected->GetSlots(), label + " slots mismatch");
    Check(actual->NumberCiphertextElements() == expected->NumberCiphertextElements(), label + " component mismatch");
    const auto expectedMetadata = SnapshotMetadata(expected, label + " expected");
    CheckMetadataValueEquivalent(actual, expectedMetadata, label + " metadata");
    for (std::size_t component = 0; component < actual->GetElements().size(); ++component) {
        CheckKeyPolynomialBasis(actual->GetElements().at(component),
                                expected->GetElements().at(component).GetParams(),
                                label + " component " + std::to_string(component));
        const auto a = SnapshotDcrt(actual->GetElements().at(component));
        const auto e = SnapshotDcrt(expected->GetElements().at(component));
        Check(SameDcrtSnapshot(a, e), label + " DCRT component mismatch at " + std::to_string(component));
    }
}

void CheckPublicRcbReturn(DoubleCKKS& module, const CiphertextPair& pair, const ReferenceRelinPaths& reference) {
    Check(pair.GetHigh() != nullptr && pair.GetLow() != nullptr,
          "public RCB oracle received a null pair member");
    const auto before = SnapshotPair(pair, "ReadyForRS2 before RCB");
    const auto pairHighMetadata = SnapshotMetadata(pair.GetHigh(), "ReadyForRS2 RCB source high");
    const auto pairLowMetadata = SnapshotMetadata(pair.GetLow(), "ReadyForRS2 RCB source low");
    const auto actual = module.RCB(pair);
    CheckPairUnchanged(pair, before, "ReadyForRS2 after RCB");
    Check(actual != nullptr, "public RCB return is null");
    const auto actualMap = actual->GetMetadataMap();
    Check(actualMap != nullptr, "public RCB return metadata map is null");
    Check(actualMap.get() != pairHighMetadata.outerMapIdentity.get(),
          "public RCB return metadata outer map aliases pair high");
    Check(actualMap.get() != pairLowMetadata.outerMapIdentity.get(),
          "public RCB return metadata outer map aliases pair low");
    CheckMetadataShallowAlias(actual, pairHighMetadata, "relin2-tensor-low", "public RCB return metadata");
    const auto expected = BuildRcbReference(reference);
    CheckCiphertextExactly(actual, expected, "public RCB return");
}


template <class Result>
struct CallObservation {
    std::unique_ptr<Result> result;
    std::exception_ptr exception;
};

template <class Function>
auto ObserveCall(Function&& function)
    -> CallObservation<std::decay_t<std::invoke_result_t<Function>>> {
    using Result = std::decay_t<std::invoke_result_t<Function>>;
    try {
        return {std::make_unique<Result>(
                    std::invoke(std::forward<Function>(function))),
                nullptr};
    }
    catch (...) {
        return {nullptr, std::current_exception()};
    }
}

template <class Result>
Result RequireNormalCompletion(CallObservation<Result>&& observation,
                               const std::string& label) {
    if (observation.result) {
        return std::move(*observation.result);
    }
    Check(observation.exception != nullptr,
          label + " recorded neither a result nor an exception");
    try {
        std::rethrow_exception(observation.exception);
    }
    catch (const std::logic_error& exception) {
        if (typeid(exception) == typeid(std::logic_error) &&
            std::string(exception.what()) == "DoubleCKKS: Relin2 is not implemented") {
            throw TestFailure(label + " reached the exact terminal Relin2 scaffold");
        }
        throw TestFailure(label + " threw an unexpected logic error: " + exception.what());
    }
    catch (const std::exception& exception) {
        throw TestFailure(label + " threw instead of returning normally: " + exception.what());
    }
    catch (...) {
        throw TestFailure(label + " threw a non-standard exception instead of returning normally");
    }
}

template <class Result>
void RequireExactInvalidArgument(const CallObservation<Result>& observation,
                                 const std::string& expectedMessage,
                                 const std::string& label) {
    if (observation.result) {
        throw TestFailure(label + " did not fail fast");
    }
    Check(observation.exception != nullptr,
          label + " recorded neither a result nor an exception");
    try {
        std::rethrow_exception(observation.exception);
    }
    catch (const std::invalid_argument& exception) {
        Check(typeid(exception) == typeid(std::invalid_argument),
              label + " threw a derived invalid-argument type: " + exception.what());
        Check(std::string(exception.what()) == expectedMessage,
              label + " reported an unexpected diagnostic: " + exception.what());
    }
    catch (const std::exception& exception) {
        throw TestFailure(label + " threw the wrong exception type: " + exception.what());
    }
    catch (...) {
        throw TestFailure(label + " threw a non-standard exception");
    }
}

template <class Function>
void CaptureBlockFailure(std::vector<std::string>& failures, const std::string& label,
                         Function&& function) {
    try {
        std::invoke(std::forward<Function>(function));
    }
    catch (const std::exception& exception) {
        failures.push_back(label + ": " + exception.what());
    }
    catch (...) {
        failures.push_back(label + ": non-standard exception");
    }
}

void RequireNoBlockFailures(const std::vector<std::string>& failures, const std::string& label) {
    if (failures.empty()) {
        return;
    }
    std::string message = label + " failed blocks";
    for (const auto& failure : failures) {
        message += " | " + failure;
    }
    throw TestFailure(message);
}

void CheckImmediateInputAndCacheInvariance(const TensorCiphertextPair& tensor,
                                           const TensorSnapshot& tensorBefore,
                                           const DeepKeyCacheSnapshot& cacheBefore,
                                           const std::string& label) {
    std::vector<std::string> failures;
    CaptureBlockFailure(failures, "Tensor", [&] {
        CheckTensorUnchanged(tensor, tensorBefore, label + " Tensor");
    });
    CaptureBlockFailure(failures, "evaluation-key cache", [&] {
        CheckDeepKeyCacheMatches(cacheBefore, label + " evaluation-key cache");
    });
    RequireNoBlockFailures(failures, label + " immediate invariance");
}

template <class Function>
void WithRestoredEvaluationKeyCache(const std::string& label, Function&& function) {
    auto& evaluationKeyCache = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    const auto initialCache = SnapshotDeepKeyCache();
    Check(initialCache.rows.empty(), label + " requires an initially empty evaluation-key cache");

    std::exception_ptr pending;
    {
        ScopedEvalMultKeyMapRestore restore(evaluationKeyCache);
        try {
            std::invoke(std::forward<Function>(function));
        }
        catch (...) {
            pending = std::current_exception();
        }
    }

    CheckDeepKeyCacheMatches(initialCache, label + " fixture-scope restoration");
    if (pending) {
        std::rethrow_exception(pending);
    }
}

template <class ArithmeticOracle>
void CheckIndependentResultOracles(
    const TensorCiphertextPair& tensor,
    const CiphertextPair& result,
    const CryptoContext<DCRTPoly>& context,
    const MetadataSnapshot& expectedHighMetadata,
    const MetadataSnapshot& expectedLowMetadata,
    DoubleCKKS& module,
    const std::string& label,
    ArithmeticOracle&& arithmeticOracle) {
    std::vector<std::string> failures;
    CaptureBlockFailure(failures, "exact cpp_int (u,v+w) oracle", [&] {
        const auto reference = BuildReferenceRelinPaths(tensor, context);
        std::invoke(std::forward<ArithmeticOracle>(arithmeticOracle), reference);
    });
    CaptureBlockFailure(failures, "complete ReadyForRS2 state/scale/metadata oracle", [&] {
        CheckResultState(result, tensor, context, expectedHighMetadata, expectedLowMetadata);
    });
    CaptureBlockFailure(failures, "public RCB exactness/non-mutation oracle", [&] {
        const auto reference = BuildReferenceRelinPaths(tensor, context);
        CheckPublicRcbReturn(module, result, reference);
    });
    RequireNoBlockFailures(failures, label);
}

void CheckPublicRelinearizationStage(
    lbcrypto::ConstCiphertext<DCRTPoly> actual,
    lbcrypto::ConstCiphertext<DCRTPoly> source,
    const std::shared_ptr<DCRTPoly::Params>& expectedBasis,
    const TensorCiphertextPair& tensor,
    std::size_t expectedLevel,
    const std::string& label) {
    Check(actual != nullptr && source != nullptr, label + " ciphertext is null");
    Check(actual.get() != source.get(), label + " ciphertext aliases its source");
    Check(actual->GetCryptoContext().get() == tensor.GetContextIdentity(),
          label + " context identity mismatch");
    Check(actual->GetEncodingType() == source->GetEncodingType(),
          label + " encoding mismatch");
    Check(actual->GetLevel() == expectedLevel, label + " level mismatch");
    Check(actual->NumberCiphertextElements() == 2, label + " component count mismatch");
    Check(actual->GetNoiseScaleDeg() == tensor.GetNoiseScaleDegree(),
          label + " noise-scale degree mismatch");
    Check(actual->GetScalingFactor() == tensor.GetRecordedScalingFactor(),
          label + " recorded scaling factor mismatch");
    Check(actual->GetKeyTag() == tensor.GetKeyTag(), label + " key tag mismatch");
    Check(actual->GetSlots() == tensor.GetSlots(), label + " slots mismatch");
    Check(expectedBasis != nullptr, label + " expected basis is null");
    for (std::size_t component = 0; component < actual->GetElements().size(); ++component) {
        const auto& element = actual->GetElements()[component];
        CheckKeyPolynomialBasis(element, expectedBasis,
                                label + " component " + std::to_string(component));
        Check(element.GetFormat() == Format::EVALUATION,
              label + " aggregate format is not Evaluation");
        for (const auto& tower : element.GetAllElements()) {
            Check(tower.GetFormat() == Format::EVALUATION,
                  label + " tower format is not Evaluation");
        }
    }
    const auto sourceMetadata = SnapshotMetadata(source, label + " source metadata");
    CheckMetadataShallowAliasExact(actual, sourceMetadata, label);
}

void CheckPublicRelinearizationShapes(const ReferenceRelinPaths& reference,
                                      const TensorCiphertextPair& tensor,
                                      const std::string& label) {
    const auto fullBasis = reference.raisedHigh->GetElements().front().GetParams();
    const auto activePrefixBasis = tensor.GetLow()->GetElements().front().GetParams();
    CheckPublicRelinearizationStage(reference.relinearizedHigh, reference.raisedHigh,
                                    fullBasis, tensor, 0,
                                    label + " full-basis public Relinearize");
    CheckPublicRelinearizationStage(reference.relinearizedLow, tensor.GetLow(),
                                    activePrefixBasis, tensor, 1,
                                    label + " active-prefix public Relinearize");
    Check(reference.relinearizedHigh->GetMetadataMap().get() !=
              reference.relinearizedLow->GetMetadataMap().get(),
          label + " public Relinearize outputs alias metadata outer maps");
}

void InstallGeneratedEvalKey(TensorFixture& fixture, const std::string& tag) {
    auto& cache = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    cache.erase(tag);
    fixture.context->EvalMultKeyGen(fixture.keys.secretKey);
    const auto found = cache.find(tag);
    Check(found != cache.end() && found->second.size() == 1 && found->second.front() != nullptr,
          "EvalMultKeyGen fixture did not install exactly one index-zero key");
}

void TestValidArithmeticStateImmutability() {
    WithRestoredEvaluationKeyCache("valid Relin2", [&] {
        auto fixture = MakeExactTensorFixture(MakeRelinContext());
        DoubleCKKS module(fixture.context);
        auto [left, right] = MakePairs(fixture, module);
        auto tensor = module.Tensor2(left, right);
        TagTensorMetadata(tensor);
        CheckNonzeroThreeComponentTensor(tensor, "valid Relin2 ordinary fixture");
        InstallGeneratedEvalKey(fixture, tensor.GetKeyTag());
        const auto expectedHighMetadata = SnapshotMetadata(tensor.GetHigh(), "valid expected high metadata");
        const auto expectedLowMetadata = SnapshotMetadata(tensor.GetLow(), "valid expected low metadata");
        const auto tensorBefore = SnapshotTensor(tensor, "valid Tensor");
        const auto cacheBefore = SnapshotDeepKeyCache();

        auto observation = ObserveCall([&] { return module.Relin2(tensor); });
        CheckImmediateInputAndCacheInvariance(tensor, tensorBefore, cacheBefore, "valid Relin2");
        const auto result = RequireNormalCompletion(
            std::move(observation), "valid Relin2 arithmetic/state fixture");

        CheckIndependentResultOracles(
            tensor, result, fixture.context, expectedHighMetadata, expectedLowMetadata,
            module, "valid Relin2", [&](const ReferenceRelinPaths& reference) {
            CheckExactRelin2Oracle(tensor, result, reference);
        });
    });
}

void InstallControlledBv0Key(const TensorCiphertextPair& tensor,
                             const CryptoContext<DCRTPoly>& context) {
    const auto parameters =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(context->GetCryptoParameters());
    Check(parameters != nullptr && parameters->GetKeySwitchTechnique() == lbcrypto::BV &&
              parameters->GetDigitSize() == 0,
          "controlled key fixture requires BV digitSize=0");
    const auto params = parameters->GetElementParams();
    const std::size_t count = params->GetParams().size();
    std::vector<DCRTPoly> a(count, DCRTPoly(params, Format::EVALUATION, true));
    std::vector<DCRTPoly> b(count, DCRTPoly(params, Format::EVALUATION, true));
    std::vector<BigInt> one(params->GetRingDimension(), 0);
    one[0] = 1;
    a[0] = MakePolynomial(params, one);
    b[0] = MakePolynomial(params, one);
    auto key = std::make_shared<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(context);
    key->SetKeyTag(tensor.GetKeyTag());
    key->SetAVector(std::move(a));
    key->SetBVector(std::move(b));
    auto& cache = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    cache[tensor.GetKeyTag()] = {key};
}

void InstallControlledTensorValues(const TensorCiphertextPair& tensor) {
    auto high = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(tensor.GetHigh());
    auto low = std::const_pointer_cast<lbcrypto::CiphertextImpl<DCRTPoly>>(tensor.GetLow());
    const auto params = high->GetElements().front().GetParams();
    const std::size_t n = params->GetRingDimension();
    const BigInt divisor(tensor.GetDivisor().ConvertToInt());
    const BigInt half = divisor / 2;
    const BigInt q0(params->GetParams().front()->GetModulus().ConvertToInt());
    const BigInt inverse = ModInverse(divisor, q0);

    DCRTPoly zero(params, Format::EVALUATION, true);
    std::vector<std::vector<BigInt>> c2Coefficients(
        params->GetParams().size(), std::vector<BigInt>(n, 0));
    c2Coefficients[0][0] = PositiveMod(half * inverse, q0);
    c2Coefficients[0][1] = PositiveMod((half + 1) * inverse, q0);
    auto c2 = MakeTowerPolynomial(params, c2Coefficients);
    high->SetElements({zero, zero, c2});

    std::vector<BigInt> low0(n, 0);
    low0[0] = 7;
    low0[1] = 11;
    low->SetElements({MakePolynomial(params, low0), zero, zero});
}

void CheckControlledWitnesses(const TensorCiphertextPair& tensor,
                              const CiphertextPair& result,
                              const ReferenceRelinPaths& reference) {
    const BigInt divisor(tensor.GetDivisor().ConvertToInt());
    const BigInt half = divisor / 2;
    const auto k0 = ToCoefficient(reference.relinearizedHigh->GetElements().at(0) -
                                  reference.raisedHigh->GetElements().at(0));
    const auto k1 = ToCoefficient(reference.relinearizedHigh->GetElements().at(1) -
                                  reference.raisedHigh->GetElements().at(1));
    const BigInt q0(k0.GetAllElements().front().GetModulus().ConvertToInt());
    Check(CoefficientResidue(k0, 0, 0) == PositiveMod(half, q0),
          "fixed K0 witness residue mismatch at component=0,tower=0,coefficient=0");
    Check(CoefficientResidue(k0, 0, 1) == PositiveMod(half + 1, q0),
          "fixed K0 carry witness residue mismatch at component=0,tower=0,coefficient=1");
    Check(CoefficientResidue(k1, 0, 0) == PositiveMod(half, q0),
          "fixed K1 witness residue mismatch at component=1,tower=0,coefficient=0");
    Check(CoefficientResidue(k1, 0, 1) == PositiveMod(half + 1, q0),
          "fixed K1 carry witness residue mismatch at component=1,tower=0,coefficient=1");

    const BigInt source0 = ReconstructCoefficient(reference.relinearizedHigh->GetElements().at(0), 0);
    const BigInt source1 = ReconstructCoefficient(reference.relinearizedHigh->GetElements().at(0), 1);
    const auto d0 = DecomposeCentered(source0, divisor);
    const auto d1 = DecomposeCentered(source1, divisor);
    Check(d0.first == 0 && d0.second == half,
          "fixed +half witness mismatch at component=0,coefficient=0");
    Check(d1.first == 1 && d1.second == -half,
          "fixed -half/carry witness mismatch at component=0,coefficient=1");

    const auto w = ToCoefficient(reference.relinearizedLow->GetElements().at(0));
    Check(CoefficientResidue(w, 0, 0) == BigInt(7),
          "fixed w witness mismatch at component=0,tower=0,coefficient=0");
    Check(PositiveMod(d0.second, q0) != 0 && CoefficientResidue(w, 0, 0) != 0,
          "fixed common nonzero v/w witness is not nonzero");

    const auto actualHigh = ToCoefficient(result.GetHigh()->GetElements().at(0));
    const auto actualLow = ToCoefficient(result.GetLow()->GetElements().at(0));
    Check(CoefficientResidue(actualHigh, 0, 0) == BigInt(0),
          "production +half quotient residue mismatch at component=0,tower=0,coefficient=0");
    Check(CoefficientResidue(actualLow, 0, 0) == PositiveMod(half + 7, q0),
          "production +half v+w residue mismatch at component=0,tower=0,coefficient=0");
    Check(CoefficientResidue(actualHigh, 0, 1) == BigInt(1),
          "production carry quotient residue mismatch at component=0,tower=0,coefficient=1");
    Check(CoefficientResidue(actualLow, 0, 1) == PositiveMod(-half + 11, q0),
          "production -half/carry v+w residue mismatch at component=0,tower=0,coefficient=1");
}

void TestControlledWitnessesAndBoundaries() {
    WithRestoredEvaluationKeyCache("controlled Relin2", [&] {
        auto fixture = MakeExactTensorFixture(MakeRelinContext(lbcrypto::BV, 0));
        DoubleCKKS module(fixture.context);
        auto [left, right] = MakePairs(fixture, module);
        auto tensor = module.Tensor2(left, right);
        TagTensorMetadata(tensor);
        InstallControlledTensorValues(tensor);
        InstallControlledBv0Key(tensor, fixture.context);
        const auto expectedHighMetadata = SnapshotMetadata(tensor.GetHigh(), "controlled expected high metadata");
        const auto expectedLowMetadata = SnapshotMetadata(tensor.GetLow(), "controlled expected low metadata");
        const auto tensorBefore = SnapshotTensor(tensor, "controlled Tensor");
        const auto cacheBefore = SnapshotDeepKeyCache();

        auto observation = ObserveCall([&] { return module.Relin2(tensor); });
        CheckImmediateInputAndCacheInvariance(tensor, tensorBefore, cacheBefore, "controlled Relin2");
        const auto result = RequireNormalCompletion(
            std::move(observation), "controlled Relin2 witnesses fixture");

        CheckIndependentResultOracles(
            tensor, result, fixture.context, expectedHighMetadata, expectedLowMetadata,
            module, "controlled Relin2", [&](const ReferenceRelinPaths& reference) {
                CheckControlledWitnesses(tensor, result, reference);
                CheckExactRelin2Oracle(tensor, result, reference);
            });
    });
}

void TestRepresentativePublicInput() {
    WithRestoredEvaluationKeyCache("representative Relin2", [&] {
        auto fixture = MakeRepresentativePublicFixture(MakeRelinContext());
        DoubleCKKS module(fixture.context);
        auto [left, right] = MakePairs(fixture, module);
        auto tensor = module.Tensor2(left, right);
        TagTensorMetadata(tensor);
        InstallGeneratedEvalKey(fixture, tensor.GetKeyTag());
        const auto expectedHighMetadata = SnapshotMetadata(tensor.GetHigh(), "representative expected high metadata");
        const auto expectedLowMetadata = SnapshotMetadata(tensor.GetLow(), "representative expected low metadata");
        const auto tensorBefore = SnapshotTensor(tensor, "representative Tensor");
        const auto cacheBefore = SnapshotDeepKeyCache();

        auto observation = ObserveCall([&] { return module.Relin2(tensor); });
        CheckImmediateInputAndCacheInvariance(tensor, tensorBefore, cacheBefore, "representative Relin2");
        const auto result = RequireNormalCompletion(
            std::move(observation), "representative public Relin2 fixture");

        CheckIndependentResultOracles(
            tensor, result, fixture.context, expectedHighMetadata, expectedLowMetadata,
            module, "representative Relin2", [&](const ReferenceRelinPaths& reference) {
                CheckExactRelin2Oracle(tensor, result, reference);
            });
    });
}

using RelinKey = std::shared_ptr<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>;

void CheckEvaluationFormat(const DCRTPoly& polynomial, const std::string& label) {
    Check(polynomial.GetFormat() == Format::EVALUATION, label + " aggregate format is not Evaluation");
    for (const auto& tower : polynomial.GetAllElements()) {
        Check(tower.GetFormat() == Format::EVALUATION, label + " tower format is not Evaluation");
    }
}

std::vector<RelinKey> GenerateAndValidateRealTwoKeyRow(TensorFixture& fixture,
                                                       const TensorCiphertextPair& tensor) {
    const auto parameters = std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(
        fixture.context->GetCryptoParameters());
    Check(parameters != nullptr, "real two-key fixture is not CKKS-RNS");
    Check(parameters->GetMaxRelinSkDeg() == 3,
          "real two-key fixture did not preserve maxRelinSkDeg=3");
    Check(parameters->GetKeySwitchTechnique() == lbcrypto::HYBRID,
          "real two-key fixture is not HYBRID");

    auto& cache = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    Check(cache.find(tensor.GetKeyTag()) == cache.end(),
          "real two-key fixture tag was not empty before EvalMultKeysGen");
    fixture.context->EvalMultKeysGen(fixture.keys.secretKey);
    const auto row = cache.find(tensor.GetKeyTag());
    Check(row != cache.end() && row->second.size() == 2,
          "EvalMultKeysGen did not produce the real ordered two-key row");
    Check(row->second[0] != nullptr && row->second[1] != nullptr &&
              row->second[0].get() != row->second[1].get(),
          "real two-key row entries are null or alias each other");

    std::vector<RelinKey> keys;
    keys.reserve(2);
    const auto expectedBasis = parameters->GetParamsQP();
    const auto expectedLength = static_cast<std::size_t>(parameters->GetNumPartQ());
    for (std::size_t keyIndex = 0; keyIndex < row->second.size(); ++keyIndex) {
        const auto key = std::dynamic_pointer_cast<lbcrypto::EvalKeyRelinImpl<DCRTPoly>>(
            row->second[keyIndex]);
        Check(key != nullptr, "real two-key row entry has the wrong concrete subtype");
        Check(key->GetCryptoContext().get() == fixture.context.get(),
              "real two-key row entry belongs to a different context");
        Check(key->GetKeyTag() == tensor.GetKeyTag(),
              "real two-key row entry tag does not match the Tensor tag");
        Check(key->GetAVector().size() == expectedLength &&
                  key->GetBVector().size() == expectedLength,
              "real two-key row entry has an invalid HYBRID A/B length");
        for (std::size_t entry = 0; entry < key->GetAVector().size(); ++entry) {
            CheckKeyPolynomialBasis(key->GetAVector()[entry], expectedBasis,
                                    "real two-key A entry " + std::to_string(keyIndex) + ":" +
                                        std::to_string(entry));
            CheckEvaluationFormat(key->GetAVector()[entry],
                                  "real two-key A entry " + std::to_string(keyIndex) + ":" +
                                      std::to_string(entry));
        }
        for (std::size_t entry = 0; entry < key->GetBVector().size(); ++entry) {
            CheckKeyPolynomialBasis(key->GetBVector()[entry], expectedBasis,
                                    "real two-key B entry " + std::to_string(keyIndex) + ":" +
                                        std::to_string(entry));
            CheckEvaluationFormat(key->GetBVector()[entry],
                                  "real two-key B entry " + std::to_string(keyIndex) + ":" +
                                      std::to_string(entry));
        }
        keys.push_back(key);
    }
    return keys;
}

void CheckRealTwoKeyRowIdentity(
    const std::string& tag,
    const EvalMultKeyMap* cacheIdentity,
    const std::vector<lbcrypto::EvalKey<DCRTPoly>>* rowIdentity,
    const std::vector<const void*>& expectedPointers,
    const CryptoContext<DCRTPoly>& context,
    const std::string& label) {
    auto& cache = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
    const auto row = cache.find(tag);
    Check(&cache == cacheIdentity && cache.size() == 1 && row != cache.end() &&
              &row->second == rowIdentity && row->second.size() == expectedPointers.size(),
          label + " cache map/vector shape or identity changed");
    for (std::size_t index = 0; index < expectedPointers.size(); ++index) {
        Check(row->second[index].get() == expectedPointers[index],
              label + " key pointer/null identity changed at index " + std::to_string(index));
        if (row->second[index]) {
            Check(row->second[index]->GetCryptoContext().get() == context.get(),
                  label + " key context changed at index " + std::to_string(index));
            Check(row->second[index]->GetKeyTag() == tag,
                  label + " key tag changed at index " + std::to_string(index));
        }
    }
}

void TestExtraLaterValid() {
    WithRestoredEvaluationKeyCache("extra-later Relin2", [&] {
        auto fixture = MakeExactTensorFixture(MakeRelinContext(lbcrypto::HYBRID, 0, 3, 3));
        DoubleCKKS module(fixture.context);
        auto [left, right] = MakePairs(fixture, module);
        auto tensor = module.Tensor2(left, right);
        TagTensorMetadata(tensor);
        const auto keys = GenerateAndValidateRealTwoKeyRow(fixture, tensor);
        auto& cache = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
        const auto row = cache.find(tensor.GetKeyTag());
        const auto* cacheIdentity = &cache;
        const auto* rowIdentity = &row->second;
        const std::vector<const void*> pointerIdentities{row->second[0].get(), row->second[1].get()};
        const auto key0ABefore = SnapshotKeyVector(keys[0]->GetAVector(), "real later-key zero A");
        const auto key0BBefore = SnapshotKeyVector(keys[0]->GetBVector(), "real later-key zero B");
        const auto key1ABefore = SnapshotKeyVector(keys[1]->GetAVector(), "real later-key one A");
        const auto key1BBefore = SnapshotKeyVector(keys[1]->GetBVector(), "real later-key one B");
        const auto expectedHighMetadata = SnapshotMetadata(tensor.GetHigh(), "extra-later expected high metadata");
        const auto expectedLowMetadata = SnapshotMetadata(tensor.GetLow(), "extra-later expected low metadata");
        const auto tensorBefore = SnapshotTensor(tensor, "extra-later Tensor");
        const auto cacheBefore = SnapshotDeepKeyCache();

        auto observation = ObserveCall([&] { return module.Relin2(tensor); });
        CheckImmediateInputAndCacheInvariance(tensor, tensorBefore, cacheBefore,
                                              "Relin2 real extra-later-key");
        CheckKeyVectorUnchanged(keys[0]->GetAVector(), key0ABefore, "real later-key zero A");
        CheckKeyVectorUnchanged(keys[0]->GetBVector(), key0BBefore, "real later-key zero B");
        CheckKeyVectorUnchanged(keys[1]->GetAVector(), key1ABefore, "real later-key one A");
        CheckKeyVectorUnchanged(keys[1]->GetBVector(), key1BBefore, "real later-key one B");
        CheckRealTwoKeyRowIdentity(tensor.GetKeyTag(), cacheIdentity, rowIdentity, pointerIdentities,
                                   fixture.context, "Relin2 real extra-later-key");
        const auto result = RequireNormalCompletion(
            std::move(observation), "Relin2 real extra-later-key fixture");
        CheckIndependentResultOracles(
            tensor, result, fixture.context, expectedHighMetadata, expectedLowMetadata,
            module, "Relin2 real extra-later-key", [&](const ReferenceRelinPaths& reference) {
                CheckExactRelin2Oracle(tensor, result, reference);
            });
    });
}

void TestMalformedLaterIgnored() {
    WithRestoredEvaluationKeyCache("malformed-later Relin2", [&] {
        auto fixture = MakeExactTensorFixture(MakeRelinContext(lbcrypto::HYBRID, 0, 3, 3));
        DoubleCKKS module(fixture.context);
        auto [left, right] = MakePairs(fixture, module);
        auto tensor = module.Tensor2(left, right);
        TagTensorMetadata(tensor);
        const auto keys = GenerateAndValidateRealTwoKeyRow(fixture, tensor);
        auto& cache = lbcrypto::CryptoContextImpl<DCRTPoly>::GetAllEvalMultKeys();
        auto row = cache.find(tensor.GetKeyTag());
        const auto firstPointer = row->second.front().get();
        row->second.back() = nullptr;
        Check(row->second.size() == 2 && row->second.front().get() == firstPointer &&
                  row->second.back() == nullptr,
              "malformed later-key fixture changed more than index one nullness");
        const auto* cacheIdentity = &cache;
        const auto* rowIdentity = &row->second;
        const std::vector<const void*> pointerIdentities{firstPointer, nullptr};
        const auto key0ABefore = SnapshotKeyVector(keys[0]->GetAVector(), "malformed later-key zero A");
        const auto key0BBefore = SnapshotKeyVector(keys[0]->GetBVector(), "malformed later-key zero B");
        const auto expectedHighMetadata = SnapshotMetadata(tensor.GetHigh(), "malformed-later expected high metadata");
        const auto expectedLowMetadata = SnapshotMetadata(tensor.GetLow(), "malformed-later expected low metadata");
        const auto tensorBefore = SnapshotTensor(tensor, "malformed-later Tensor");
        const auto cacheBefore = SnapshotDeepKeyCache();

        auto observation = ObserveCall([&] { return module.Relin2(tensor); });
        CheckImmediateInputAndCacheInvariance(tensor, tensorBefore, cacheBefore,
                                              "Relin2 malformed-later-key ignored");
        CheckKeyVectorUnchanged(keys[0]->GetAVector(), key0ABefore, "malformed later-key zero A");
        CheckKeyVectorUnchanged(keys[0]->GetBVector(), key0BBefore, "malformed later-key zero B");
        CheckRealTwoKeyRowIdentity(tensor.GetKeyTag(), cacheIdentity, rowIdentity, pointerIdentities,
                                   fixture.context, "Relin2 malformed-later-key ignored");
        const auto result = RequireNormalCompletion(
            std::move(observation), "Relin2 malformed-later-key ignored fixture");
        CheckIndependentResultOracles(
            tensor, result, fixture.context, expectedHighMetadata, expectedLowMetadata,
            module, "Relin2 malformed-later-key ignored", [&](const ReferenceRelinPaths& reference) {
                CheckExactRelin2Oracle(tensor, result, reference);
            });
    });
}

void TestValidTechnique(lbcrypto::KeySwitchTechnique technique, std::uint32_t digitSize,
                        const std::string& label) {
    WithRestoredEvaluationKeyCache(label, [&] {
        auto fixture = MakeExactTensorFixture(MakeRelinContext(technique, digitSize));
        DoubleCKKS module(fixture.context);
        auto [left, right] = MakePairs(fixture, module);
        auto tensor = module.Tensor2(left, right);
        TagTensorMetadata(tensor);
        InstallGeneratedEvalKey(fixture, tensor.GetKeyTag());
        const auto expectedHighMetadata = SnapshotMetadata(tensor.GetHigh(), label + " expected high metadata");
        const auto expectedLowMetadata = SnapshotMetadata(tensor.GetLow(), label + " expected low metadata");
        const auto tensorBefore = SnapshotTensor(tensor, label + " Tensor");
        const auto cacheBefore = SnapshotDeepKeyCache();

        auto observation = ObserveCall([&] { return module.Relin2(tensor); });
        CheckImmediateInputAndCacheInvariance(tensor, tensorBefore, cacheBefore, label);
        const auto result = RequireNormalCompletion(
            std::move(observation), label + " production Relin2");
        CheckIndependentResultOracles(
            tensor, result, fixture.context, expectedHighMetadata, expectedLowMetadata,
            module, label, [&](const ReferenceRelinPaths& reference) {
                CheckPublicRelinearizationShapes(reference, tensor, label);
                CheckExactRelin2Oracle(tensor, result, reference);
            });
    });
}

void TestHybridValidShapes() {
    TestValidTechnique(lbcrypto::HYBRID, 0, "Relin2 HYBRID valid shapes");
}

void TestBvZeroDigitValidShapes() {
    TestValidTechnique(lbcrypto::BV, 0, "Relin2 BV zero-digit valid shapes");
}

void TestBvNonzeroDigitValidShapes() {
    TestValidTechnique(lbcrypto::BV, 10, "Relin2 BV nonzero-digit valid shapes");
}

void TestFirstRecombinedRcbValidation() {
    auto fixture = MakeExactTensorFixture(MakeRelinContext());
    DoubleCKKS module(fixture.context);
    auto pair = module.DCP(fixture.leftInput);
    Check(pair.GetPaperScale().approximateRecombinedLogicalScalingFactor ==
              static_cast<long double>(fixture.leftInput->GetScalingFactor()),
          "DCP recombined logical scale propagation mismatch");
    auto& scale = const_cast<PaperScaleDescriptor&>(pair.GetPaperScale());
    scale.approximateRecombinedLogicalScalingFactor *= 2.0L;
    const auto before = SnapshotPair(pair, "corrupt recombined RCB pair");
    const auto observation = ObserveCall([&] { return module.RCB(pair); });
    CheckPairUnchanged(pair, before, "corrupt recombined RCB pair after failure");
    RequireExactInvalidArgument(
        observation,
        "DoubleCKKS: pair recombined logical scale is inconsistent",
        "ReadyForFirstMult RCB recombined validation");
}

void TestFirstRecombinedTensor2Validation() {
    auto fixture = MakeExactTensorFixture(MakeRelinContext());
    DoubleCKKS module(fixture.context);
    auto left = module.DCP(fixture.leftInput);
    auto right = module.DCP(fixture.rightInput);
    Check(right.GetPaperScale().approximateRecombinedLogicalScalingFactor ==
              static_cast<long double>(fixture.rightInput->GetScalingFactor()),
          "Tensor2 recombined-field fixture did not start from the propagated value");
    auto& scale = const_cast<PaperScaleDescriptor&>(right.GetPaperScale());
    scale.approximateRecombinedLogicalScalingFactor *= 2.0L;
    const auto leftBefore = SnapshotPair(left, "Tensor2 left before recombined failure");
    const auto rightBefore = SnapshotPair(right, "Tensor2 right before recombined failure");
    const auto observation = ObserveCall([&] { return module.Tensor2(left, right); });
    CheckPairUnchanged(left, leftBefore, "Tensor2 left after recombined failure");
    CheckPairUnchanged(right, rightBefore, "Tensor2 right after recombined failure");
    RequireExactInvalidArgument(
        observation,
        "DoubleCKKS: pair recombined logical scale is inconsistent",
        "Tensor2 recombined field validation");
}

void TestTensor2RequiresFirstLifecycle() {
    WithRestoredEvaluationKeyCache("Tensor2 lifecycle Relin2 fixture", [&] {
        auto fixture = MakeExactTensorFixture(MakeRelinContext());
        DoubleCKKS module(fixture.context);
        auto [firstLeft, firstRight] = MakePairs(fixture, module);
        auto tensor = module.Tensor2(firstLeft, firstRight);
        InstallGeneratedEvalKey(fixture, tensor.GetKeyTag());
        const auto readyForRs2 = module.Relin2(tensor);
        Check(readyForRs2.GetLifecycle() == PairLifecycle::ReadyForRS2,
              "Tensor2 lifecycle fixture did not construct ReadyForRS2");
        const auto cacheBefore = SnapshotDeepKeyCache();

        auto checkRejected = [&](const CiphertextPair& left, const CiphertextPair& right,
                                 const std::string& label) {
            const auto leftBefore = SnapshotPair(left, label + " left");
            const auto rightBefore = SnapshotPair(right, label + " right");
            const auto observation = ObserveCall([&] { return module.Tensor2(left, right); });
            CheckPairUnchanged(left, leftBefore, label + " left after rejection");
            CheckPairUnchanged(right, rightBefore, label + " right after rejection");
            CheckDeepKeyCacheMatches(cacheBefore, label + " evaluation-key cache");
            RequireExactInvalidArgument(
                observation,
                "DoubleCKKS: Tensor2 requires ReadyForFirstMult inputs",
                label);
        };

        std::vector<std::string> failures;
        CaptureBlockFailure(failures, "ReadyForRS2 left operand", [&] {
            checkRejected(readyForRs2, firstRight, "Tensor2 ReadyForRS2 left operand");
        });
        CaptureBlockFailure(failures, "ReadyForRS2 right operand", [&] {
            checkRejected(firstLeft, readyForRs2, "Tensor2 ReadyForRS2 right operand");
        });
        CaptureBlockFailure(failures, "ReadyForRS2 both operands", [&] {
            checkRejected(readyForRs2, readyForRs2, "Tensor2 ReadyForRS2 both operands");
        });
        RequireNoBlockFailures(failures, "Tensor2 ReadyForFirstMult lifecycle guard");
    });
}

}  // namespace core_red


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
    if (name == "key_bv_nonzero_digit_entry_format") {
        return &TestBVEvaluationKeyNonzeroDigitEntryFormat;
    }
    if (name == "valid_arithmetic_state_immutability") {
        return &core_red::TestValidArithmeticStateImmutability;
    }
    if (name == "controlled_witnesses_and_boundaries") {
        return &core_red::TestControlledWitnessesAndBoundaries;
    }
    if (name == "representative_public_input") {
        return &core_red::TestRepresentativePublicInput;
    }
    if (name == "key_extra_later_valid") {
        return &core_red::TestExtraLaterValid;
    }
    if (name == "key_malformed_later_ignored") {
        return &core_red::TestMalformedLaterIgnored;
    }
    if (name == "hybrid_valid_shapes") {
        return &core_red::TestHybridValidShapes;
    }
    if (name == "bv_zero_digit_valid_shapes") {
        return &core_red::TestBvZeroDigitValidShapes;
    }
    if (name == "bv_nonzero_digit_valid_shapes") {
        return &core_red::TestBvNonzeroDigitValidShapes;
    }
    if (name == "first_recombined_rcb_validation") {
        return &core_red::TestFirstRecombinedRcbValidation;
    }
    if (name == "first_recombined_tensor2_validation") {
        return &core_red::TestFirstRecombinedTensor2Validation;
    }
    if (name == "tensor2_requires_first_lifecycle") {
        return &core_red::TestTensor2RequiresFirstLifecycle;
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
