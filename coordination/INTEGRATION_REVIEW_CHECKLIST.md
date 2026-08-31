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

## OpenFHE 1.5.0 facts to verify in every candidate

- `LeveledSHEBase::EvalMultCore` forms the raw tensor and updates scale/noise
  metadata, but it is protected. A project-side implementation must either
  construct the three `DCRTPoly` components explicitly or present a minimal,
  separately reviewed upstream hook/patch.
- Public `Relinearize` can consume a crafted multi-component ciphertext, but
  its evaluation keys, element parameters, active RNS basis, component format,
  and key tag must all match. A successful API call alone does not prove the
  `Relin2` modulus semantics.
- CKKS `ModReduceInternalInPlace` calls `DropLastElementAndScale`, assumes the
  crypto-parameter tower ordering, and mutates level, noise-scale degree, and
  scaling factor. It is valid only when the exact required prime is the next
  configured tower to drop.
- `LevelReduceInternalInPlace` only drops towers and increments level. It does
  not implement quotient rounding or a centered remainder.
- Public scalar `EvalMult` may encode a CKKS scalar, alter scale metadata, and
  automatically reduce a level depending on scaling mode. It must not be used
  as a silent substitute for exact ring multiplication by the integer
  `q_div` unless an independent test proves the exact coefficient/RNS action
  and unchanged metadata required at that point.
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
6. Codex, Windows ZCode/Zima, and ChatGPT Pro review the same exact commit. Only
   a concrete unresolved disagreement is escalated to terminal-only Fable5.
