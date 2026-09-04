# Repeated-Mult2 handoff

**Boundary:** This file records the handshake only. The separate repeated-Mult2 task was live and unavailable to this review. It was not contacted, stopped, refreshed, polled, or inferred. The frozen task and supplied primary-source notes were read as evidence.

## Client decisions safe to adopt independently

The repeated design does not need to reopen these client-boundary decisions unless its returned evidence directly contradicts an upstream-source premise:

1. **Slot value type:** fixed `cpp_dec_float<100>` complex values; no binary64 public input/output.
2. **Exact scale semantics:** positive, denominator-positive, gcd-reduced arbitrary-size rational value.
3. **No ordinary Plaintext seam:** public client calls return immutable ciphertext/state or owned decoded values; no stale packed-value cache.
4. **Official input cryptography:** exact large-Poly/DCRT construction followed by public scheme Encrypt; no copied encryption algorithm.
5. **Official undecoded output:** public scheme `Decrypt(..., Poly*)`, check validity, retain configured polynomial flooding; never call `DecryptCore` or change decryption-noise mode merely to pass a precision gate. The first tracer explicitly asserts `FIXED_NOISE_DECRYPT` to exclude configured flooding from its narrow accuracy gate; cryptographic randomness remains; flooding mode needs a separate accuracy contract.
6. **Initial output policy:** `COMPLEX` and `EXEC_EVALUATION`; REAL is rejected because ordinary Decode's additional postprocessing is omitted.
7. **Canonical order:** powers of five modulo `2N`, with a multiprecision transcription of OpenFHE's special transforms.
8. **Packing semantics:** return `OpenFhePackedStride`, with `gap=N/(2S)`; keep all-coefficient canonical evaluation as separately labelled evidence.
9. **Exact conversion/centering:** decimal-string BigInteger bridge, exact CRT interpolation result, actual final centered representative diagnostics. V1 is build-gated to the compile-time-confirmed inspected `bigintdyn` implementation; it does not presume equivalent string behavior for every `MATHBACKEND`.
10. **Ownership:** client may decrypt at output; evaluator never receives secret key and never decrypts/re-encrypts.
11. **State integrity:** process-local context/crypto-parameter identity, required/observed feature masks, configuration modes, key tag, ordered moduli and roots, level, slots, format, component count, encoding/data type, physical metadata, and a present empty ciphertext metadata map are receipt invariants; evaluator access is through an OpenFHE clone, never the private snapshot. The empty-map restriction avoids aliasing of cloned `shared_ptr<Metadata>` values.
12. **No broad subsystem:** no serialization, context/key factory, plugin codec, rotations, bootstrapping, or multiparty features.

## Exact items that must be reconciled after the repeated return

| Dependency | Required returned fact/decision | Why the client cannot guess |
|---|---|---|
| Per-stage exact scale | Exact numerator/denominator transition for high, low, pair-recombined, and public RCB state after every Tensor2/Relin2/RS2/re-entry | Current `double`/`long double` fields and nominal `2^p` are not exact normalization. |
| Removed/introduced factors | Which actual primes divide/multiply each logical scale, in exact order | FIXEDMANUAL metadata uses nominal bit-scale observations while actual `q_l`, `q_div`, and generated primes differ. |
| Basis family | Exact ordered modulus **and root** list for every original, reduced, raised, auxiliary-Q, and P-extended state | A raised state can be non-prefix; level/count alone is ambiguous. |
| Context identity | Whether one context remains authoritative or immutable basis-specific contexts are introduced | Current validation uses pointer identity; a new context cannot be silently treated as equivalent. |
| Key identity/family | Which public/secret/evaluation key is valid on each basis/context and how key tags are bound | Dropping or replacing keys by shape is not cryptographic equivalence. |
| HYBRID/BV tables | Active digit partition count and exact table/key-row basis for each repeated Relin2 | Tables are positional/basis-dependent; `dnum=11` against a smaller active basis requires an explicit rule. |
| P basis | Exact generated P moduli/roots and whether they are shared or regenerated across basis families | Parameter-generation equality is not automatic merely because public construction APIs exist. |
| Sparse secret h=128 | Official supported generation/import path and evidence that all required keys share the intended secret | Current ordinary sparse route does not by itself establish paper h=128. |
| Lifecycle | State accepted after current `RefreshRequired`, and exact transition allowing the next Mult2 without section 6.2 refresh | Current first-operation implementation stops at a lifecycle boundary. |
| Metadata authority | Who issues level/noise-scale degree/recorded-factor/exact-scale receipt after each operation | Two mutable ledgers would become incoherent. |
| Operation lineage | Whether an evaluator-owned operation wrapper/receipt binds parent state to output, or whether orchestration remains an explicitly trusted immediate-adoption boundary | A raw OpenFHE ciphertext does not carry unforgeable parent lineage; structural state checks cannot detect deliberate substitution by another same-state ciphertext. |
| Public client binder | Whether `BindFirstMult2Rcb` is replaced by an evaluator-issued receipt, extended with a reviewed repeated transition, or retained only for the first tracer | The present binder is deliberately not a generic repeated architecture. |
| Paper experiment | Full `N=32768,h=128,dnum=11` configuration and eight-squaring observation plan | The N64 first-Mult2 client slice does not satisfy these obligations. |

## Required merge rule for exact scale

There must be exactly one semantic authority for an evaluated ciphertext's exact scale.

### Preferred outcome

The repeated evaluator returns an immutable public state receipt containing:

```text
parent state identity
operation/lifecycle
exact ordered basis/root identity
actual removed/introduced divisors
exact reduced rational scale
level / noise-scale degree / recorded OpenFHE factor
metadata-map ownership/content policy
slots / format / components / key tag / context identity
```

The client validates and consumes that receipt. Its local `PositiveRationalScale` is then merely a value representation of the evaluator-issued exact scale, not a competing computation.

### Acceptable fallback

If the repeated design deliberately leaves exact state with the client orchestrator, add only operation-specific binders whose scales are calculated from immutable parent receipts and actual operation factors. Do not add a generic `Bind(ciphertext, arbitraryScale)`.

### Rejected merge outcomes

- Maintaining one exact scale in the client and a different exact/approximate scale as independent evaluator authority.
- Recovering scale from `ciphertext->GetScalingFactor()`, `level`, or bit sizes alone.
- Assuming a post-RS2/raised basis is the original prefix because tower counts match.
- Reusing a key/evaluation-key table solely because key tags or vector lengths look compatible.
- Changing context/key/security settings inside client I/O to make repeated tests pass.

## First-tracer state before reconciliation

The proposed v1 client knows only two origins:

```text
FreshClientEncoding
FirstMult2Rcb
```

For the latter it derives `q_div` and `q_l` from the identical immutable fresh parent bases, verifies the exact drop-two output modulus/root prefix, and computes:

```text
scale_out = scale_left * scale_right / (q_div*q_l)
```

It also derives the physical factor from the pinned context in the evaluator's exact order—`fresh=base*base`, `ready=fresh*fresh/base`, `out=ready/GetModReduceFactor(fullBasisSize-2)`—rather than accepting a caller transition record. The validated output is cloned into the receipt under the empty-metadata-map rule. It admits no second Mult2. That bounded bridge may be implemented only after user confirmation; it must not be generalized until the returned repeated design closes the table above.

## Handoff acceptance checklist

Before a repeated client-I/O code slice is authorized, the coordinator should be able to answer all of the following with source/operation evidence:

- What exact basis, roots, context pointer, key tag, level, components, format, slots, degree, and recorded factor does each repeated public ciphertext have?
- What exact rational scale belongs to high, low, and recombined observations at that state?
- Which actual modulus factors produced that scale transition?
- Which public/secret/evaluation keys are valid for that exact basis and secret distribution?
- Which operation creates the next immutable receipt, and can the client validate it without decrypting inside the evaluator?
- Does the route preserve configured decryption protection and keep REAL unsupported unless separately designed?
- Does the route reach the paper's eight squarings under the paper-directed configuration without silently invoking section 6.2 refresh?

Until those answers exist, this package makes no claim about repeated execution.
