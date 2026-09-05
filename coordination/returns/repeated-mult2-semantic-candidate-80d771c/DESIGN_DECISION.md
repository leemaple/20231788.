# Design decision: two public families, exact receipts, no client re-entry

## Decision and boundary

Implement the approved evaluator seam rather than the old expected-rejection or shape-only probe. Keep the existing single-context constructor and its first-operation behavior. Add one client setup function, a public-data plan, one constructor overload, read-only receipts and a new lifecycle value. Do not create a generic backend, shipping codec, new cryptographic primitive or an eight-operation configuration.

The two successful semantic calls in the RED test remain the same through GREEN. Missing-API compilation is the predicted RED; no compiler or cryptographic runner was invoked. The implementation is a candidate for the frozen test, not a claim that it passed.

## Ownership and private-key separation

`CreateRepeatedMult2DiagnosticSetup()` is the sole production private-key boundary. It creates one native root keypair, validates Q public-key shape, and creates family-local evaluation keys. Family 1's secret is an exact projection of the root polynomial matched by cyclotomic order, modulus and root of unity. Its temporary `PrivateKeyImpl` is destroyed in the setup loop. No independent family secret is sampled.

`RepeatedMult2ClientSetup` separates `plan`, `publicKey` and `rootSecret`. Only `plan` reaches `DoubleCKKS`. `RepeatedMult2Plan::Data` contains contexts, actual returned parameter/scheme objects, ordered public basis seals, tags, evaluation-key rows with value seals, and immutable receipts. It has no private-key member, private-key constructor parameter, plaintext vector, client callback, decoder or refresh hook. The optional upstream `DEBUG_KEY` context-secret storage is explicitly prohibited by a compile-time guard.

The test separates `PrepareClient`, `Evaluate(const EvaluatorInputs&)` and `FinalClientOracle(const ClientOracle&, ...)`. `EvaluatorInputs` cannot reach the separately held root secret through a project-owned member. Both successful Mult2 calls finish before the final oracle accesses that secret. A read-only Z snapshot between calls is not a client operation. Public staged calls are additional wiring observations, not the semantic oracle. Rejection tests do not count as successful semantic operations.

The unrelated-row witness is generated during client setup from the same root under a separate tag. It does not add a fifth root KeyGen. Its small RAII guard owns only that preflight-absent nonempty tag. The plan similarly acquires each tag only after absence is checked and clears only its own tags on destruction, including a partial setup exception. No global cache clear is used.

## Ordered family plan

The diagnostic initial full Q basis B0 has ten primes from the frozen native parameter generation request. The final prime is the exact divisor d. A0 is B0 without d. Operation 1 consumes actual m1, the last prime in A0. Family B1 removes m1 from B0 while preserving the same d, including its root identity. A1 is B1 without d. Operation 2 consumes actual m2 from A1. There are exactly two nonempty families and a terminal receipt, not an unused third context or evaluation-key row.

Each family obtains its own actual returned context, parameters, scheme, Q/P/QP parameter objects, distinct key tag and evaluation-key row. Different-family object aliasing is rejected. Equivalent-family factory interning is allowed only after checking the returned profile and enabled scheme component types. The prime-generation seed is enabled before possible equivalent B0 interning; no family tables are overwritten to impersonate another basis.

HYBRID uses `numPartQ = |B_i|` with `numPerPartQ = 1`. Each partition is checked against its own Q prime. P is nonempty; QP is exactly that family's ordered Q followed by that family's P. P values may coincide across equivalent bit-sized families, but distinct family objects/tables and family-local key rows are required. The implementation neither requires equal P nor copies one family's P/QP tables into another.

Factory construction explicitly sets CKKSRNS_SCHEME, COMPLEX, PRE NOT_SET, HYBRID, FIXEDMANUAL, STANDARD, HPS and the diagnostic execution/noise/multiparty fields. Validation reads the parameters attached to the returned context, not merely the requested object. It checks full basis identities/products, algorithm modes, encoding, ring size, security declaration, secret distribution, partitioning and public-key basis selection before exposing setup keys/ciphertexts. Evaluation revalidates the actual family and owned row before use. This is not a general adversarial/concurrent OpenFHE-handle mutation guarantee; callers must not mutate shared upstream handles. Numeric before/after snapshots additionally check all exposed tables used by this diagnostic.

## Exact normalization and phases

`ExactScale` stores a positive coprime arbitrary-precision numerator and denominator. A receipt is immutable, has a private constructor, belongs by pointer membership to one plan, and has an immutable parent. It records family, operation, phase, local level, arity, noise degree, lifecycle and compatibility metadata. Receipt identity is normalization/phase authority, not a claim of ciphertext lineage, authenticity or a low-noise theorem.

For S0=2^100, actual d and consumed primes m1,m2:

```
T1 = 2^200 / d
S1 = 2^200 / (d*m1)
T2 = 2^400 / (d^3*m1^2)
S2 = 2^400 / (d^3*m1^2*m2)
```

Every ratio is reduced canonically. The same recurrence `T_i=S_(i-1)^2/d`, `S_i=T_i/m_i` generates the plan. Metadata doubles/long doubles never replace these exact rationals.

| Receipt | Family/local level | Arity high/low | Lifecycle | Exact normalization |
| --- | --- | --- | --- | --- |
| Input | 0/1 | 2/2 | ReadyForFirstMult | S0 |
| Tensor 1 | 0/1 | 3/3 | Tensor phase | T1 |
| Relin 1 | 0/1 | 2/2 | ReadyForRS2 | T1 |
| RS 1 | 0/2 | 2/2 | RefreshRequired | S1 |
| Re-entry | 1/1 | 2/2 | ReadyForRepeatedMult | S1 |
| Tensor 2 | 1/1 | 3/3 | Tensor phase | T2 |
| Relin 2 | 1/1 | 2/2 | ReadyForRS2 | T2 |
| RS 2 / W | 1/2 | 2/2 | RefreshRequired, terminal | S2 |

Compatibility metadata is propagated with the existing OpenFHE/project operations. For FIXEDMANUAL, recorded factors use the nominal 2^50 table factor; exact normalization uses the actual consumed prime m_i. The approximate high/recombined descriptor is carried across re-entry unchanged, not reset from a constructor's original first-operation assumptions.

Pair/tensor validation checks plan membership, phase/family, divisor, ordered basis/roots, wrapper/context/tag, level, arity, format, slots, noise degree and recorded/approximate metadata against the receipt. Exact scales cannot be substituted through the public receipt API. Add/Sub require the same receipt identity and family, retaining that receipt on the result. A phase receipt does not attest provenance of arbitrary caller-mutated coefficient data.

## Arithmetic and internal re-entry

The baseline coefficient kernels remain unchanged: Tensor2 omits low-low; Relin2 raises high with a zero d tower, relinearizes, decomposes internally and preserves the DCP remainder; RS2 uses `RS(d*high+low) - d*RS(high)` for low; RCB remains `d*high+low`. The offline verifier checks literal preservation of the bounded Tensor2/Relin2/RS2/RCB coefficient bodies after removing the new receipt attachments/routing.

After nonterminal RS2, `Reenter` creates new ciphertext wrappers with the next context and tag, copies existing DCRT elements without arithmetic, changes only family-local level and readiness, and preserves coefficient values, prime/root identity, slots, arity, format, noise/recorded-scale metadata and S1. It also copies the metadata map container; as in upstream clone semantics, metadata payload objects are shared, not deep-cloned. The diagnostic has no application metadata payload and does not mutate one. Source wrappers are not rebound in place.

The public evaluator routes by the plan-issued receipt to a private family worker. This keeps `evaluator.Mult2(Z,Z)` unchanged. Re-entry does not DCP, RCB, multiply/divide by d, rescale, key-switch, decrypt, encrypt, bootstrap or call the client. The second result is returned terminal without allocating a future family.

## Legacy boundary and minimal surface

The old constructor still derives and enforces its existing one-context lifecycle. A legacy RefreshRequired pair is not upgraded. Plan receipts are rejected on the legacy path; a legacy pair without a plan receipt is rejected on the repeated path. ReadyForRepeatedMult is appended after the old enum values. A nullptr constructor delegation preserves the pre-existing null-context diagnostic despite the new constructor overload.

The RED patch changes only CMake, the workflow and three new test files. The GREEN patch changes only CMake, the existing DoubleCKKS header/source and the new repeated header/source. Legacy tests and RED test bytes are untouched. CMake adds the new implementation translation unit immediately after the RED target binding, giving the ordered GREEN patch an actual dependency on RED. This is not a deliberate source defect.

## Oracle independence and remaining uncertainty

Frozen X/Y/Z/W and deltas are byte-preserved and independently recomputed using exact dyadic integers before setup. A separate Python Fraction check verifies every literal and its C++ transcription. The existing test fixture is only an input DCRT adapter; the new output oracle never reads or serializes its stale binary64 cache.

Final decryption explicitly computes c0+c1*s with negacyclic polynomial arithmetic at each actual prime/root, reconstructs CRT integers, recombines the pair and performs multiprecision canonical Horner evaluation at fixed independently enumerated roots. Constant, i and X^2 witnesses check canonical ordering. No library decrypt/decoder/CRT interpolation or staged/direct equality is used as the arithmetic reference. Every slot and the two distinguishing complex deltas must meet 2^-80. Observed product headroom is calculated only after both calls and is labeled observed, not a universal all-key bound.

These checks are encoded, not executed cryptography. Compilation/API integration, platform warnings, stochastic decryption error, factory behavior and all-key/no-wrap properties remain unestablished until independent authorized runs. A failure must remain a failure; do not weaken the frozen oracle or relabel a probe as success. The ordered-family/receipt design leaves the evaluator API stable for later slices, but the current client factory intentionally implements only this two-operation diagnostic.
