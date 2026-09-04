#include "precision_dcp_rcb_fixture.h"

#include <boost/math/constants/constants.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace precision_dcp_rcb_test {
namespace {

using BigInt = boost::multiprecision::cpp_int;

constexpr std::size_t kExpectedRingDimension = 64;
constexpr std::size_t kExpectedSlots = 16;
constexpr std::uint32_t kExpectedScaleBits = 100;

class FixtureFailure final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void Check(bool condition, const std::string& message) {
    if (!condition) {
        throw FixtureFailure(message);
    }
}

MpComplex Add(const MpComplex& left, const MpComplex& right) {
    return {left.real + right.real, left.imag + right.imag};
}

MpComplex Subtract(const MpComplex& left, const MpComplex& right) {
    return {left.real - right.real, left.imag - right.imag};
}

MpComplex Multiply(const MpComplex& left, const MpComplex& right) {
    return {left.real * right.real - left.imag * right.imag,
            left.real * right.imag + left.imag * right.real};
}

BigFloat Pow2Float(std::uint32_t bits) {
    BigFloat result = 1;
    for (std::uint32_t bit = 0; bit < bits; ++bit) {
        result *= 2;
    }
    return result;
}

BigInt RoundHalfAwayFromZero(const BigFloat& value) {
    BigFloat rounded;
    if (value >= 0) {
        rounded = boost::multiprecision::floor(value + BigFloat("0.5"));
    }
    else {
        rounded = boost::multiprecision::ceil(value - BigFloat("0.5"));
    }
    return rounded.convert_to<BigInt>();
}

BigInt Abs(const BigInt& value) {
    return value < 0 ? -value : value;
}

BigInt PositiveMod(BigInt value, const BigInt& modulus) {
    value %= modulus;
    if (value < 0) {
        value += modulus;
    }
    return value;
}

BigInt Product(const std::vector<BigInt>& values) {
    BigInt result = 1;
    for (const auto& value : values) {
        result *= value;
    }
    return result;
}

std::vector<BigInt> GetModuli(const lbcrypto::DCRTPoly& polynomial) {
    std::vector<BigInt> result;
    result.reserve(polynomial.GetAllElements().size());
    for (const auto& tower : polynomial.GetAllElements()) {
        result.emplace_back(tower.GetModulus().ConvertToInt());
    }
    return result;
}

std::vector<std::uint32_t> MakeRotationGroup(std::size_t size,
                                             std::uint32_t cyclotomicOrder) {
    std::vector<std::uint32_t> result(size);
    std::uint64_t power = 1;
    for (std::size_t index = 0; index < size; ++index) {
        result[index] = static_cast<std::uint32_t>(power);
        power = (power * 5U) % cyclotomicOrder;
    }
    return result;
}

std::vector<MpComplex> MakeRoots(std::uint32_t cyclotomicOrder) {
    std::vector<MpComplex> result(static_cast<std::size_t>(cyclotomicOrder) + 1U);
    const BigFloat pi = boost::math::constants::pi<BigFloat>();
    const BigFloat order(cyclotomicOrder);
    using boost::multiprecision::cos;
    using boost::multiprecision::sin;
    for (std::uint32_t index = 0; index < cyclotomicOrder; ++index) {
        const BigFloat angle = BigFloat(2) * pi * BigFloat(index) / order;
        result[index] = {cos(angle), sin(angle)};
    }
    result[cyclotomicOrder] = result[0];
    return result;
}

void BitReverse(std::vector<MpComplex>& values) {
    const std::size_t size = values.size();
    Check(size != 0 && (size & (size - 1U)) == 0,
          "special inverse transform size must be a nonzero power of two");
    for (std::size_t index = 1, reverse = 0; index < size; ++index) {
        std::size_t bit = size >> 1U;
        while ((reverse & bit) != 0U) {
            reverse ^= bit;
            bit >>= 1U;
        }
        reverse ^= bit;
        if (index < reverse) {
            std::swap(values[index], values[reverse]);
        }
    }
}

void FftSpecialInverse(std::vector<MpComplex>& values,
                       std::uint32_t cyclotomicOrder) {
    const auto rotationGroup = MakeRotationGroup(values.size(), cyclotomicOrder);
    const auto roots = MakeRoots(cyclotomicOrder);
    const std::size_t size = values.size();

    for (std::size_t length = size; length >= 1U; length >>= 1U) {
        const std::size_t halfLength = length >> 1U;
        const std::size_t quarterLength = length << 2U;
        Check(quarterLength != 0U && cyclotomicOrder % quarterLength == 0U,
              "special inverse transform/order geometry mismatch");
        const std::size_t gap =
            static_cast<std::size_t>(cyclotomicOrder) / quarterLength;
        for (std::size_t offset = 0; offset < size; offset += length) {
            for (std::size_t index = 0; index < halfLength; ++index) {
                const std::size_t rootIndex =
                    (quarterLength - (rotationGroup[index] % quarterLength)) * gap;
                const MpComplex sum =
                    Add(values[offset + index], values[offset + index + halfLength]);
                const MpComplex difference = Multiply(
                    Subtract(values[offset + index], values[offset + index + halfLength]),
                    roots[rootIndex]);
                values[offset + index] = sum;
                values[offset + index + halfLength] = difference;
            }
        }
        if (length == 1U) {
            break;
        }
    }

    BitReverse(values);
    for (auto& value : values) {
        value.real /= BigFloat(size);
        value.imag /= BigFloat(size);
    }
}

lbcrypto::DCRTPoly MakePolynomial(
    const std::shared_ptr<lbcrypto::DCRTPoly::Params>& params,
    const std::vector<BigInt>& coefficients) {
    Check(params != nullptr, "precision fixture received null DCRT parameters");
    Check(coefficients.size() == params->GetRingDimension(),
          "precision coefficient vector has wrong ring dimension");

    lbcrypto::DCRTPoly result(params, lbcrypto::Format::COEFFICIENT, true);
    auto& towers = result.GetAllElements();
    for (std::size_t towerIndex = 0; towerIndex < towers.size(); ++towerIndex) {
        const auto modulusNative = towers[towerIndex].GetModulus();
        const BigInt modulus(modulusNative.ConvertToInt());
        lbcrypto::NativeVector residues(params->GetRingDimension(), modulusNative);
        for (std::size_t coefficient = 0; coefficient < coefficients.size(); ++coefficient) {
            const auto residue =
                PositiveMod(coefficients[coefficient], modulus).convert_to<std::uint64_t>();
            residues[coefficient] = lbcrypto::NativeInteger(residue);
        }
        towers[towerIndex].SetValues(std::move(residues),
                                     lbcrypto::Format::COEFFICIENT);
    }
    result.SetFormat(lbcrypto::Format::EVALUATION);
    return result;
}

}  // namespace

lbcrypto::Plaintext MakePrecisionPlaintext(
    const lbcrypto::CryptoContext<lbcrypto::DCRTPoly>& context,
    const std::vector<MpComplex>& inputValues,
    std::uint32_t scaleBits) {
    Check(context != nullptr, "precision fixture received null context");
    Check(scaleBits == kExpectedScaleBits,
          "precision fixture is bound to the frozen 2^100 diagnostic scale");
    Check(inputValues.size() == kExpectedSlots,
          "precision fixture requires the complete frozen 16-slot vector");

    auto plaintext = context->MakeCKKSPackedPlaintext(
        std::vector<std::complex<double>>{{0.0, 0.0}}, 2, 0);
    Check(plaintext != nullptr, "placeholder plaintext creation failed");
    Check(plaintext->GetEncodingType() == lbcrypto::CKKS_PACKED_ENCODING,
          "placeholder plaintext encoding metadata mismatch");
    Check(plaintext->GetLevel() == 0, "placeholder plaintext level mismatch");
    Check(plaintext->GetNoiseScaleDeg() == 2,
          "placeholder plaintext scale-degree mismatch");
    Check(plaintext->GetSlots() == kExpectedSlots,
          "placeholder plaintext slot count mismatch");
    Check(plaintext->GetScalingFactor() ==
              std::ldexp(1.0, static_cast<int>(scaleBits)),
          "placeholder plaintext recorded scale mismatch");

    const auto params = plaintext->GetElement<lbcrypto::DCRTPoly>().GetParams();
    Check(params != nullptr, "placeholder plaintext has null DCRT parameters");
    const std::size_t ringDimension = params->GetRingDimension();
    Check(ringDimension == kExpectedRingDimension,
          "placeholder plaintext ring dimension mismatch");
    const std::size_t packedLength = 2U * kExpectedSlots;
    Check(ringDimension % packedLength == 0U,
          "ring dimension is not divisible by twice the slot count");

    std::vector<MpComplex> inverse(inputValues);
    FftSpecialInverse(inverse, static_cast<std::uint32_t>(2U * ringDimension));
    const BigFloat scale = Pow2Float(scaleBits);
    const std::size_t gap = ringDimension / packedLength;
    std::vector<BigInt> coefficients(ringDimension, 0);
    for (std::size_t index = 0; index < kExpectedSlots; ++index) {
        coefficients[gap * index] =
            RoundHalfAwayFromZero(inverse[index].real * scale);
        coefficients[gap * (index + kExpectedSlots)] =
            RoundHalfAwayFromZero(inverse[index].imag * scale);
    }

    const BigInt fullModulus =
        Product(GetModuli(plaintext->GetElement<lbcrypto::DCRTPoly>()));
    Check(fullModulus > 0, "placeholder plaintext has empty CRT modulus");
    for (const auto& coefficient : coefficients) {
        Check((Abs(coefficient) << 1U) < fullModulus,
              "precision fixture coefficient would wrap the plaintext CRT modulus");
    }

    // Test-only adapter.  Encrypt consumes this public DCRT element.  The
    // plaintext's private complex<double> cache remains the zero placeholder,
    // so this object must never be observed through packed-value getters,
    // production Decrypt, serialization, or a shipping codec contract.
    plaintext->GetElement<lbcrypto::DCRTPoly>() = MakePolynomial(params, coefficients);
    return plaintext;
}

}  // namespace precision_dcp_rcb_test
