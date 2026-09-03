#include "openfhe.h"
#include "openfhe_2023_1788/double_ckks.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using lbcrypto::Ciphertext;
using lbcrypto::CryptoContext;
using lbcrypto::DCRTPoly;
using lbcrypto::NativeInteger;
using openfhe_2023_1788::CiphertextPair;
using openfhe_2023_1788::DoubleCKKS;
using openfhe_2023_1788::PairLifecycle;
using openfhe_2023_1788::PaperScaleDescriptor;
using openfhe_2023_1788::ReadOnlyCiphertext;

class TestFailure final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void Check(bool condition, const std::string& message) {
    if (!condition) {
        throw TestFailure(message);
    }
}

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

struct MetadataEntrySnapshot {
    std::string key;
    std::shared_ptr<lbcrypto::Metadata> identity;
    std::shared_ptr<lbcrypto::Metadata> deepValue;
};

struct CiphertextSnapshot {
    ReadOnlyCiphertext identity;
    Ciphertext<DCRTPoly> clone;
    lbcrypto::MetadataMap metadataIdentity;
    std::vector<MetadataEntrySnapshot> metadata;
};

CiphertextSnapshot SnapshotCiphertext(const ReadOnlyCiphertext& ciphertext,
                                      const std::string& label) {
    Check(ciphertext != nullptr, label + " is null");
    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");

    CiphertextSnapshot snapshot{ciphertext, ciphertext->Clone(), metadata, {}};
    snapshot.metadata.reserve(metadata->size());
    for (const auto& [key, value] : *metadata) {
        Check(value != nullptr, label + " metadata value is null");
        auto deepValue = value->Clone();
        Check(deepValue != nullptr, label + " metadata clone is null");
        snapshot.metadata.push_back({key, value, std::move(deepValue)});
    }
    return snapshot;
}

void CheckCiphertextUnchanged(const ReadOnlyCiphertext& ciphertext,
                              const CiphertextSnapshot& before,
                              const std::string& label) {
    Check(ciphertext != nullptr && before.identity != nullptr && before.clone != nullptr,
          label + " ciphertext or snapshot is null");
    Check(ciphertext.get() == before.identity.get(), label + " ciphertext identity changed");
    Check(*ciphertext == *before.clone, label + " ciphertext value changed");

    const auto metadata = ciphertext->GetMetadataMap();
    Check(metadata != nullptr, label + " metadata map is null");
    Check(metadata.get() == before.metadataIdentity.get(), label + " metadata map identity changed");
    Check(metadata->size() == before.metadata.size(), label + " metadata map size changed");
    auto current = metadata->begin();
    for (const auto& expected : before.metadata) {
        Check(current != metadata->end(), label + " metadata entry disappeared");
        Check(current->first == expected.key, label + " metadata key or order changed");
        Check(current->second.get() == expected.identity.get(), label + " metadata identity changed");
        Check(*(current->second) == *(expected.deepValue), label + " metadata value changed");
        ++current;
    }
    Check(current == metadata->end(), label + " metadata gained trailing entries");
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

PairSnapshot SnapshotPair(const CiphertextPair& pair) {
    return {SnapshotCiphertext(pair.GetHigh(), "RS2 input high"),
            SnapshotCiphertext(pair.GetLow(), "RS2 input low"),
            pair.GetContextIdentity(),
            pair.GetDivisor(),
            pair.GetOrderedModuli(),
            pair.GetLevel(),
            pair.GetPaperScale(),
            pair.GetRecordedScalingFactor(),
            pair.GetNoiseScaleDegree(),
            pair.GetLifecycle(),
            pair.GetKeyTag(),
            pair.GetSlots(),
            pair.GetFormat(),
            pair.GetComponentCount()};
}

void CheckPairUnchanged(const CiphertextPair& pair, const PairSnapshot& before) {
    CheckCiphertextUnchanged(pair.GetHigh(), before.high, "RS2 input high");
    CheckCiphertextUnchanged(pair.GetLow(), before.low, "RS2 input low");
    Check(pair.GetContextIdentity() == before.contextIdentity, "RS2 input context manifest changed");
    Check(pair.GetDivisor() == before.divisor, "RS2 input divisor manifest changed");
    Check(pair.GetOrderedModuli() == before.orderedModuli, "RS2 input basis manifest changed");
    Check(pair.GetLevel() == before.level, "RS2 input level manifest changed");
    Check(pair.GetPaperScale().inputRecordedScalingFactor == before.paperScale.inputRecordedScalingFactor,
          "RS2 input paper recorded scale changed");
    Check(pair.GetPaperScale().divisor == before.paperScale.divisor,
          "RS2 input paper divisor changed");
    Check(pair.GetPaperScale().approximateLogicalScalingFactor ==
              before.paperScale.approximateLogicalScalingFactor,
          "RS2 input high logical scale changed");
    Check(pair.GetPaperScale().approximateRecombinedLogicalScalingFactor ==
              before.paperScale.approximateRecombinedLogicalScalingFactor,
          "RS2 input recombined logical scale changed");
    Check(pair.GetRecordedScalingFactor() == before.recordedScalingFactor,
          "RS2 input recorded scale changed");
    Check(pair.GetNoiseScaleDegree() == before.noiseScaleDegree,
          "RS2 input noise-scale degree changed");
    Check(pair.GetLifecycle() == before.lifecycle, "RS2 input lifecycle changed");
    Check(pair.GetKeyTag() == before.keyTag, "RS2 input key tag changed");
    Check(pair.GetSlots() == before.slots, "RS2 input slots changed");
    Check(pair.GetFormat() == before.format, "RS2 input format changed");
    Check(pair.GetComponentCount() == before.componentCount,
          "RS2 input component count changed");
}

template <class Function>
void CheckThrowsInvalidArgument(Function&& function, const std::string& expectedMessage) {
    bool threw = false;
    try {
        std::invoke(std::forward<Function>(function));
    }
    catch (const std::invalid_argument& exception) {
        Check(exception.what() == expectedMessage,
              "RS2 wrong-lifecycle diagnostic mismatch: " + std::string(exception.what()));
        threw = true;
    }
    catch (const std::exception& exception) {
        throw TestFailure("RS2 wrong lifecycle threw the wrong exception type: " +
                          std::string(exception.what()));
    }
    Check(threw, "RS2 wrong lifecycle did not fail fast");
}

CryptoContext<DCRTPoly> MakeContext() {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(3);
    parameters.SetScalingModSize(30);
    parameters.SetFirstModSize(35);
    parameters.SetScalingTechnique(lbcrypto::FIXEDMANUAL);
    parameters.SetSecurityLevel(lbcrypto::HEStd_NotSet);
    parameters.SetRingDim(32);
    parameters.SetBatchSize(8);

    auto context = lbcrypto::GenCryptoContext(parameters);
    context->Enable(lbcrypto::PKE);
    context->Enable(lbcrypto::KEYSWITCH);
    context->Enable(lbcrypto::LEVELEDSHE);
    return context;
}

void TestWrongLifecycle() {
    auto context = MakeContext();
    const auto keys = context->KeyGen();
    const auto plaintext =
        context->MakeCKKSPackedPlaintext(std::vector<double>{1.25, -2.0, 0.0}, 2, 0);
    auto input = context->Encrypt(plaintext, keys.publicKey);
    input->SetMetadataByKey("rs2-wrong-lifecycle", std::make_shared<ProbeMetadata>("unchanged"));

    DoubleCKKS module(context);
    const CiphertextPair pair = module.DCP(input);
    Check(pair.GetLifecycle() == PairLifecycle::ReadyForFirstMult,
          "RS2 wrong-lifecycle fixture is not ReadyForFirstMult");
    const PairSnapshot before = SnapshotPair(pair);

    CheckThrowsInvalidArgument(
        [&] {
            (void)module.RS2(pair);
        },
        "DoubleCKKS: RS2 requires ReadyForRS2 input");
    CheckPairUnchanged(pair, before);

    lbcrypto::CryptoContextFactory<DCRTPoly>::ReleaseAllContexts();
}

}  // namespace

int main(int argc, char** argv) {
    try {
        if (argc != 2 || std::string(argv[1]) != "wrong_lifecycle") {
            throw TestFailure("usage: rs2_test wrong_lifecycle");
        }
        TestWrongLifecycle();
        std::cout << "RS2 case passed: wrong_lifecycle\n";
        return 0;
    }
    catch (const std::exception& exception) {
        std::cerr << "RS2 test failure: " << exception.what() << '\n';
        return 1;
    }
}
