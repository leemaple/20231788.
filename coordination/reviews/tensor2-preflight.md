# Codex Tensor2 preflight — paper/OpenFHE seam

Recorded: 2026-08-31 Asia/Shanghai

## Status and evidence boundary

This is a targeted source/design preflight for the next clean-room vertical
slice. It is not an implementation, build, CTest, precision, Windows, or
performance result. The implementation base remains exact branch
`agent/codex-tensor2-01`, commit
`02b34bac9cb87afc8acb9df275d5c0e137b554e7`. Sustained execution remains on
GitHub Actions/Windows rather than the Mac.

Sources inspected:

- the user-supplied IACR ePrint 2023/1788 paper text, Definition 4.1 and Lemma
  4.2;
- official pristine OpenFHE 1.5.0 commit
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`;
- only the current greenfield DCP/RCB public seam and tests.

No previous/local/private 2023/1788 implementation was read or used.

## Paper contract

Definition 4.1 gives, without changing modulus `Q_l`:

```text
Tensor2((h1,l1),(h2,l2)) =
    (h1 tensor h2, h1 tensor l2 + l1 tensor h2).
```

Both result members have three RLWE components. The `l1 tensor l2` term is
deliberately absent.

Lemma 4.2 gives the exact modular relationship:

```text
Tensor(ct1,ct2) decrypted under (1,s,s^2)
  = q_div * RCB(Tensor2(CT1,CT2)) decrypted under (1,s,s^2)
    + Dec(l1) * Dec(l2)                 (mod Q_l).
```

Subject to the lemma's centered-range condition, this means the recombined
Tensor2 output represents the ordinary product divided by `q_div`, plus the
bounded error caused by omitting low-low. Tensor2 therefore performs a logical
rescale by `q_div` while consuming no RNS tower.

## Observed OpenFHE 1.5.0 behavior

1. `CryptoContext::EvalMultNoRelin` performs `TypeCheck` and then calls the
   scheme's `EvalMult` (`src/pke/include/cryptocontext.h:1968-1972`).
2. Under `FIXEDMANUAL`, `LeveledSHERNS::EvalMult` clones both inputs,
   `AdjustForMultInPlace` only aligns their tower counts, and `EvalMultCore`
   performs the RLWE convolution (`src/pke/lib/schemerns/rns-leveledshe.cpp:
   182-191, 393-400, 479-485`). Equal-level inputs therefore retain the same
   ordered basis.
3. For two two-component ciphertexts, `EvalMultCore` constructs exactly the
   three standard convolution components. It clones empty metadata from the
   left operand, sets noise-scale degree to the sum of input degrees, and sets
   the floating scaling factor to the product of input factors
   (`src/pke/lib/schemebase/base-leveledshe.cpp:620-660`).
4. The two cross products have matching three-component shape, basis, and raw
   multiplication metadata. `FIXEDMANUAL` ciphertext-ciphertext addition aligns
   levels but returns directly for two non-plaintext ciphertexts; it does not
   introduce a tower drop (`src/pke/lib/schemerns/rns-leveledshe.cpp:402-477`).

For fresh DCP pair members, each OpenFHE ciphertext currently records noise-scale
degree 2 and `SF = baseSF^2`. Raw `EvalMultNoRelin` consequently records degree
4 and `SF^2 = baseSF^4`. Paper Tensor2, however, represents a product divided by
`q_div`; for the intended first multiplication where `q_div` approximates but
is not equal to `baseSF`, the OpenFHE-normalized result contract is degree 3 and
recorded factor `SF1 * SF2 / baseSF`, while the separate paper/recombined scale
is `S1 * S2 / q_div`. This conclusion is derived from the paper and observed
metadata transitions; the exact normalization design remains subject to
independent ChatGPT Pro review and coefficient-preservation tests.

## Minimal seam expected for review

- Validate both `CiphertextPair` inputs fully before multiplication, then reject
  mutual key-tag or slot-count incompatibility before raw arithmetic.
- Compute exactly three no-relinearization products: high-high, high-low, and
  low-high. Add only the two cross products.
- Normalize only result metadata needed by the dual OpenFHE/paper scale
  contract. Assert that normalization does not alter a polynomial coefficient,
  basis tower, or level.
- Return a distinct read-only three-component result type whose constructor is
  private to `DoubleCKKS`; do not let it enter DCP/RCB APIs that require two
  components.
- Preserve all four input ciphertext objects in their complete OpenFHE equality
  state.
- Do not add Relin2, RS2, Mult2, pair addition/subtraction, refresh, `t>2`, or a
  speculative generic tuple layer in this slice.

## Independent oracle and failure tests required

- A Boost `cpp_int` schoolbook negacyclic convolution in
  `Z_q[X]/(X^N+1)` must calculate all three components, every coefficient, and
  every active RNS tower without calling an OpenFHE/production multiplication
  helper for the expected value.
- Convert copied polynomials to coefficient form for oracle comparison; do not
  mutate production inputs to observe them.
- Prove omission rather than only matching the cross term: construct inputs for
  which independent `l1 tensor l2` is nonzero and show the actual low output is
  not cross-plus-low-low.
- Assert unchanged ordered basis, level 1, evaluation format, exact context,
  encoding, key tag, slots, three components, result lifecycle/type, recorded
  degree/factor, divisor, and both paper-scale quantities.
- Add fail-fast cases for each individual pair invariant plus at least mutual
  key-tag and slot-count incompatibility. Existing DCP/RCB tests must remain
  unchanged in strength.

## Open design question to resolve before acceptance

The current `PaperScaleDescriptor` has DCP-specific fields
`inputRecordedScalingFactor` and `approximateLogicalScalingFactor`. The latter
describes the high quotient component after DCP, while the pair as recombined
still carries the input logical scale. Tensor2 needs both the high-component and
recombined-pair views after a further division by `q_div`. The implementation
must either make the existing names unambiguous with the smallest coherent
descriptor correction, or prove that a separate minimal Tensor result
descriptor is clearer. It must not silently identify the actual RNS prime
`q_div` with OpenFHE's real `baseSF`.
