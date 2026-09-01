# Clean-room integration review checklist

This checklist is derived only from the supplied 2023/1788 paper and the official
OpenFHE 1.5.0 source at commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. It is an acceptance aid, not an
implementation and not evidence that any test has passed.

## Paper identities that must survive the OpenFHE mapping

- `DCP_qdiv` starts with a two-component ciphertext over `Q_l * q_div` and
  returns `(Quo_qdiv(ct), Rem_qdiv(ct))`, both represented over `Q_l`. The
  remainder is coefficient-wise centered in `(-q_div/2, q_div/2]`; merely
  deleting a tower is not `Rem_qdiv`.
- `RCB_qdiv(high, low) = q_div * high + low` is evaluated over the current
  `Q_l`. After homomorphic pair operations, recombination is not generally a
  lift back to `Q_l * q_div`.
- `Tensor2((h1,l1),(h2,l2))` returns two three-component ciphertexts over the
  same `Q_l`: `(h1 tensor h2, h1 tensor l2 + l1 tensor h2)`. The `l1 tensor l2`
  term is deliberately discarded. No modulus is consumed at this step.
- `Relin2(high3, low3)` is
  `DCP_qdiv(Relin(q_div * high3)) + (0, Relin(low3))`. The raised high term must
  actually be defined and key-switched over the required extended modulus;
  independently relinearizing both inputs at `Q_l` is the naive construction
  rejected by the paper.
- `RS2(high, low)` is
  `(RS_ql(high), RS_ql(q_div * high + low) - q_div * RS_ql(high))`. Both output
  components must use the same dropped `q_l`, basis `Q_(l-1)`, level, scale,
  domain, and rounding convention.
- `Mult2 = RS2 o Relin2 o Tensor2`. One multiplication consumes `q_l`, not
  `q_div`; an initial decomposition consumes `q_div`. The intended scale
  relation is approximately `Delta = q_div * q_l`, subject to the concrete
  prime schedule and error bound.
- The pair has a logical scale attached to its **recombined** plaintext. If the
  inputs have logical scales `S1` and `S2`, then `DCP` preserves that logical
  scale, `Tensor2` produces `S1 * S2 / q_div`, `Relin2` preserves it, and
  `RS2` produces `S1 * S2 / (q_div * q_l)`. For equal input scale `Delta`, the
  last expression returns approximately `Delta`. Separately, FIXEDMANUAL
  OpenFHE metadata tracks powers of the approximate factor `2^p`, so the
  corresponding recorded transitions are `SF1*SF2/2^p` after Tensor2 and
  `SF1*SF2/2^(2p)` after RS2. A candidate must track/assert both views; it must
  not conceal their prime-versus-power-of-two ratio with a metadata-only
  correction.
- Theorem 4.8 requires
  `N * (M_high * q_div + M_low)^2 + E_relin + h < Q_l / 2` and bounds the
  recombined multiplication error by
  `(N * M_low^2 / q_div + E_relin + h) / q_l + (h + 1) / 2` in the paper's
  polynomial norm. A claimed paper-derived end-to-end threshold must state how
  these quantities are instantiated or conservatively related to the decoded
  slot error; the phrase "paper-derived" alone is not evidence.

## OpenFHE 1.5.0 facts to verify in every candidate

- `LeveledSHEBase::EvalMultCore` is protected, but public
  `CryptoContextImpl::EvalMultNoRelin` type-checks and delegates to the same
  raw tensor path. Under `FIXEDMANUAL` it only aligns operand levels before
  tensoring. A project-side Tensor2 may compose that public operation after
  strict pair validation, or construct the three `DCRTPoly` components
  explicitly; in either case it must replace ordinary product metadata with
  the paper-derived pair logical scale rather than inheriting the default
  `S1*S2` metadata.
- Public `Relinearize` can consume a crafted multi-component ciphertext, but
  its evaluation keys, element parameters, active RNS basis, component format,
  and key tag must all match. A successful API call alone does not prove the
  `Relin2` modulus semantics.
- In HYBRID key switching, OpenFHE 1.5.0 selects precomputed `PartQ` and
  complement tables from the current tower **count**, and indexes evaluation
  key towers using `delta = full_Q_size - current_Q_size`. Therefore an
  arbitrary basis with the same number of towers as a valid prefix can run
  while using the wrong tables/key residues. A lifted `Q_l * q_div` basis must
  prove ordered-prime identity with the evaluation-key context, not merely
  matching ring dimension or tower count. BV key switching has the analogous
  prefix assumption: it truncates evaluation-key polynomials with
  `DropLastElements(full_size - current_size)`.
- CKKS `ModReduceInternalInPlace` calls `DropLastElementAndScale`, assumes the
  crypto-parameter tower ordering, and mutates level, noise-scale degree, and
  scaling factor. It is valid only when the exact required prime is the next
  configured tower to drop. Blindly using this ciphertext-level operation for
  `DCP` also applies ordinary CKKS metadata transitions, which do not by
  themselves represent the pair's preserved logical scale.
- `LevelReduceInternalInPlace` only drops towers and increments level. It does
  not implement quotient rounding or a centered remainder.
- Public scalar `EvalMult` may encode a CKKS scalar, alter scale metadata, and
  automatically reduce a level depending on scaling mode. It must not be used
  as a silent substitute for exact ring multiplication by `q_div`.
  `CryptoContextImpl::EvalMultNoCheck(ciphertext, NativeInteger)` instead
  clones the ciphertext, multiplies every DCRT component by the exact ring
  integer, and leaves metadata unchanged; it is acceptable only after the
  project module has performed all context/basis/shape checks that the upstream
  `NoCheck` entry point deliberately omits.
- Every manual `DCRTPoly` operation must state whether its input and output are
  in coefficient or evaluation format. NTT conversion cannot be inferred from
  a decrypted end result.

## Required lifecycle table

For every public operation, record and assert:

| Field | Required evidence |
|---|---|
| Prime roles | Ordered manifest identifying base, each multiplication prime, `q_div`, and any key-switch auxiliary primes |
| Active basis | Exact ordered tower list before and after the operation |
| Ciphertext shape | Pair length and ring-component count of each member |
| Domain | Coefficient/evaluation format for every component |
| Level | OpenFHE level plus the mathematical `Q_l` it denotes |
| Scale | Numeric scale and noise-scale degree, with the paper-derived transition |
| Keys | Key tag, relinearization key availability, and the basis under which the key switch is executed |
| Rounding | Independent signed-integer/CRT oracle and tie convention |

The table must cover initial `DCP`, `Tensor2`, `Relin2`, `RS2`, recombination,
and either a second `Mult2` or an executable fail-fast refresh boundary.

## Immediate rejection conditions

- Any source or design is copied from or compared against the quarantined old
  implementation.
- `DCP` is implemented as tower deletion without independently constructing
  and checking the centered remainder.
- `Relin2` relinearizes the high term only at `Q_l`, or performs two ordinary
  independent relinearizations while claiming equivalence to the paper.
- A prime is removed or re-added without proving its position is compatible
  with OpenFHE's configured tower order and precomputed tables.
- Metadata is copied from a convenient operand after a basis/scale-changing
  operation without explicit derivation and assertions.
- Expected test values are produced by the implementation under test.
- A decrypt-only smoke test is presented as proof of DCP/RCB, RS2 rounding,
  next-level lifecycle, precision, or modulus saving.
- Build, test, precision, performance, or security claims lack exact observed
  commands and output from pristine OpenFHE 1.5.0.

## Evidence gate before integration

1. Retained red-first failures exist for every nontrivial primitive.
2. Deterministic big-integer/CRT oracles cover signed boundaries and rounding.
3. The implementation passes the metadata and lifecycle assertions above.
4. End-to-end fixed real/complex-slot tests use a threshold derived in the
   design, not adjusted after seeing the result.
5. Warning-enabled builds and tests run on Windows or GitHub Actions; Mac is
   not used for sustained compilation.
6. Use Codex, ChatGPT Pro, and either Windows ZCode/Zima or the current fallback
   reviewer on the same exact commit when they are available. Follow the
   current substitution, evidence, no-resend, and restoration rules in
   `coordination/REVIEW_ALLOCATION.md`. External-agent unavailability must be
   recorded accurately but does not override executable acceptance evidence or
   block the next safe TDD boundary.
