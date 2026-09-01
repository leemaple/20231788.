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

CryptoContext<DCRTPoly> MakeContext(std::uint32_t multiplicativeDepth = 3,
                                    std::uint32_t batchSize = 8) {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multiplicativeDepth);
    parameters.SetScalingModSize(30);
    parameters.SetFirstModSize(35);
    parameters.SetScalingTechnique(lbcrypto::FIXEDMANUAL);
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
