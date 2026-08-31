# Minimal public API proposal

Recorded: 2026-08-31

Status: Codex proposal for external-agent review before the first implementation slice. The user has already confirmed the public behavioral seams DCP, RCB, pair Add/Sub, Tensor2, Relin2, RS2, and Mult2.

## Module boundary

Use one project-owned `DoubleCKKS` object bound to one exact `CryptoContext<DCRTPoly>`. It owns validation of the fixed-manual prime manifest and is the only constructor of valid pair values.

```cpp
namespace openfhe_2023_1788 {

class CiphertextPair;
class TensorCiphertextPair;

class DoubleCKKS final {
public:
    explicit DoubleCKKS(lbcrypto::CryptoContext<lbcrypto::DCRTPoly> context);

    CiphertextPair DCP(lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly>& ciphertext) const;
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> RCB(const CiphertextPair& pair) const;

    CiphertextPair Add(const CiphertextPair& left, const CiphertextPair& right) const;
    CiphertextPair Sub(const CiphertextPair& left, const CiphertextPair& right) const;

    TensorCiphertextPair Tensor2(const CiphertextPair& left, const CiphertextPair& right) const;
    CiphertextPair Relin2(const TensorCiphertextPair& tensor) const;
    CiphertextPair RS2(const CiphertextPair& relinearized) const;
    CiphertextPair Mult2(const CiphertextPair& left, const CiphertextPair& right) const;
};

}  // namespace openfhe_2023_1788
```

`CiphertextPair` exposes read-only high/low ciphertext access plus read-only manifest/state facts needed by callers and tests: divisor, ordered active moduli, OpenFHE level, paper-scale transition descriptor, recorded scaling factor, noise-scale degree, and lifecycle state. `TensorCiphertextPair` exposes the same facts but guarantees that both members have exactly three RLWE components. Their constructors and mutable fields remain private to `DoubleCKKS`.

## Type and lifecycle states

- `DCP` accepts only a fresh, two-component, evaluation-format, level-0 ciphertext on the exact full basis `[q0, ..., q_l, q_div]` and returns `CiphertextPair::ReadyForFirstMult` on its exact prefix.
- `Tensor2` accepts only compatible `ReadyForFirstMult` pairs and returns `TensorCiphertextPair`; the distinct type prevents accidentally passing a two-component pair to Relin2.
- `Relin2` accepts only `TensorCiphertextPair` and returns `CiphertextPair::ReadyForRS2` with two RLWE components in each member.
- `RS2` accepts only `ReadyForRS2` and returns `CiphertextPair::RefreshRequired` after dropping `q_l`.
- `Mult2` composes those three calls and accepts only `ReadyForFirstMult`.
- `RCB` accepts every valid two-component pair lifecycle state.
- Add/Sub require identical lifecycle state and all compatibility facts; they preserve that state.
- A second Mult2 on `RefreshRequired` fails before tensoring or key-switch table access.

The lifecycle enum is a validation fact, not a substitute for checking the actual context pointer, key tag, ordered moduli, level, format, component counts, and dual scale metadata on every public entry.

## Explicit dependency choices

- `DoubleCKKS` keeps the exact context pointer; a deeply equal but different context is rejected.
- Relin2 uses the evaluation-key vector registered for the input key tag, and validates it before modulus raising. The first slice does not introduce its own global key store.
- Exact multiplication by `q_div` uses the upstream native-integer operation only after module validation.
- Tensor2 uses the public raw-tensor operation only after module validation.
- DCP/RS2 use a private last-tower rescale adapter; zero-tower modulus extension and metadata normalization remain private.

## Deliberate exclusions

No public constructor for malformed pairs, generic tuple length, arbitrary basis extension, user-settable metadata, automatic refresh, second multiplication, bootstrapping, serialization, compatibility wrapper, or custom OpenFHE fork belongs in the first slice.

Tests remain at the operations above. They may inspect read-only invariant facts and ciphertext elements, but they do not call the private rescale, CRT, tensor, modulus-extension, or metadata adapters.
