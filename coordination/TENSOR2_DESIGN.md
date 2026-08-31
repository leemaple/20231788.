# Tensor2 vertical-slice design

This document describes only the `t=2` pair-tensor seam implemented on the clean-room DCP/RCB base. It does not define Relin2, RS2, Mult2, pair Add/Sub, a second multiplication lifecycle, serialization, or `t>2` tuples.

## Public seam

`DoubleCKKS::Tensor2(const CiphertextPair& left, const CiphertextPair& right) const` accepts only the existing read-only DCP result type and returns a distinct `TensorCiphertextPair`.

`TensorCiphertextPair` exposes read-only high/low ciphertexts plus the invariant manifest needed to check context identity, divisor, ordered basis, level, OpenFHE recorded scaling factor, noise-scale degree, key tag, slots, evaluation format, and the fixed three-component result shape. Its constructor is private to `DoubleCKKS`.

The type has no lifecycle enum. Its distinct type is the state boundary that prevents a three-component Tensor2 result from entering DCP/RCB APIs that require two RLWE components.

## Arithmetic

For `left=(h1,l1)` and `right=(h2,l2)`, the implementation follows Definition 4.1 exactly:

```text
high = h1 tensor h2
low  = h1 tensor l2 + l1 tensor h2
```

The three tensor products are performed with public OpenFHE `EvalMultNoRelin`; only the two cross products are added. `l1 tensor l2` is never evaluated. Tensor2 does not relinearize, rescale coefficients, or change the active modulus basis.

## Validation order

Before any OpenFHE arithmetic or raw element access, Tensor2:

1. runs the existing complete `ValidatePair` path on the left operand;
2. runs the same `ValidatePair` path on the right operand; and
3. checks cross-input compatibility, including key tag and slot count.

Invariant failures are project-owned `std::invalid_argument` diagnostics with the stable `DoubleCKKS: ` prefix. OpenFHE `TypeCheck` is therefore not the validation boundary for supported inputs.

## Dual scale contract

The existing DCP `PaperScaleDescriptor` remains unchanged.

For each input pair `i`:

- `H_i` is `GetPaperScale().approximateLogicalScalingFactor`, the DCP high-component logical scale;
- `R_i` is `GetPaperScale().inputRecordedScalingFactor`, the scale of the value reconstructed by `q_div*high+low`.

The Tensor result's separate `TensorScaleDescriptor` records:

```text
H_out = H_1 * H_2
R_out = R_1 * R_2 / q_div
```

The actual integer `q_div` is used for these paper scales.

OpenFHE's FIXEDMANUAL metadata schedule is tracked separately. Raw `EvalMultNoRelin` produces degree 4 and recorded factor `SF_1*SF_2`; because the pair tensor already supplies the paper's one logical division by the actual `q_div` without consuming a tower, the public result receives only the corresponding bookkeeping normalization:

```text
noiseScaleDegree = 3
recordedSF       = SF_1 * SF_2 / baseSF
baseSF           = CryptoParametersCKKSRNS::GetScalingFactorReal(0)
```

This normalization changes only ciphertext metadata. It does not assume `q_div == baseSF`, alter a polynomial coefficient, remove a tower, or change level 1.

## Test contract

`tensor2_test` is split into four CTest entries so failures are independently reported:

- `tensor2_valid_arithmetic_immutability`;
- `tensor2_result_scale_contract`;
- `tensor2_right_input_validation`;
- `tensor2_mutual_compatibility`.

Arithmetic expectations come from a test-owned Boost `cpp_int` schoolbook negacyclic-convolution oracle over every active RNS tower and coefficient, not from OpenFHE multiplication. Fixtures include an `X^(N-1)*X=-1` wrap witness, a signed product crossing an active modulus, and an independently nonzero low-low witness proving omission rather than merely matching the cross term.

The compile-only `tensor2_api_contract_test` is intentionally not a CTest entry; it pins the public result types/getters and `DoubleCKKS::Tensor2` signature at build time.
