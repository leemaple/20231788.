# Independent numerical/oracle review

## Disposition

**No numerical-oracle blocker found in the draft.** The proposed test is a coherent, independently discriminating measurement of the experimental profile's fresh and eight-square endpoint accuracy, with ten independent polynomial anchors at every returned stage. It preserves the original `2^-80` component gates and does not turn a future passing execution into proof of intermediate all-slot nonwrap, security, or correctness beyond the single sampled chain.

This is source-only acceptance of the test design, not a compile or runtime result. I did not run C++, cryptography, transforms, a full-slot replay, CI, or any Pro command. I formed the first-pass judgment below from the frozen task, draft source, existing oracle/test, and production source before reading any Pro-authored `INDEPENDENT_REVIEW.md`, `DESIGN_AND_ORACLE.md`, or verdict; those files were not needed and were not used for this review. Fable 5.1 remained quota-unavailable as stated in the task, so this is a separate Codex fallback context, not a new provider or provider-diverse confirmation.

## Exact reviewed scope and identity

- Repository/worktree baseline during review: `2a6be7a1ec4528718df47d7c5d6366b3904b3036` in `/Users/lifeng/Documents/20231788-openfhe-precision116-eight-square-20260907`.
- Implemented production boundary: `2759fa90840946ef42957c7ba71ebea47e0e4995`. `git diff 2759fa... -- include src tests` was empty at the reviewed baseline, so the inspected production/original tests are the implemented bytes.
- Frozen task: `coordination/precision116-eight-square-01/TASK.md`, SHA-256 `1fb9e2188ba8f4f90f3b4ac3d5fe7c4bdd248529363001366fe2e4cbfdf71a0d`.
- Reviewed draft: `coordination/precision116-eight-square-return-01/pro/files/tests/experimental_precision116_eight_square_test.cpp`, SHA-256 `ff82f162b90a33793506de3bb883b4e12cbc95bea736adbd683d837cca31cea7`.
- Relevant frozen helpers: `tests/paper_full_eight_square_oracle.h` SHA-256 `08d6c4dfe6761b84d893d90c120418a29d8a2b96aa05304e95728d72e4c3e203`; `tests/paper_full_eight_square_contract_test.cpp` `d3a852d1c2a2ef7db38b2303f52afffd0527ff886ed4655efbbb3ad5c0f4bd90`; `tests/experimental_precision116_profile_seam.h` `a9e1ab95b5d0e85b8ca635ec0aae7fe5b927a9972046f2fc1f995067f1f86fa5`.

The separately returned CMake/API integration and ownership/build compatibility are outside this numerical seat.

## Independent first-pass derivation

### Literal and exact-scale independence

The test never derives an expected modulus, root, divisor, or scale from a live plan, receipt, or ciphertext. Its `xp::kQ`, `xp::kP`, roots, and witnesses are frozen test literals in the already frozen one-operation header; `CheckFamilies` compares them against each live family and native Q/P/QP tables. `CheckCandidateLiterals` independently completes the Proth-witness condition for the three new primes, while the native basis checks establish exact root order 65536 by verifying `root^32768=-1` and `root^65536=1` (draft lines 60-99; seam lines 35-49, 64-197).

For round `r`, the draft constructs

`S_r = 2^(116*2^r) / product_(j=1..r) (d*m_j)^(2^(r-j))`,

with `d=kQ[10]` and `m_j=kQ[10-j]`, then reduces the exact integer fraction (draft lines 70-85). This is algebraically the closed solution of `S_0=2^116` and `S_r=S_(r-1)^2/(d*m_r)`, and it consumes Mult7 through Mult0 in the required order. Receipt numerators and denominators must equal these reduced integers, not merely agree as floating-point approximations (lines 184-238). The tensor receipt is independently checked against `S_(r-1)^2/d`.

### Sparse polynomial oracle

`CandidateSecret` inverse-transforms each native tower independently, accepts only residues `0,1,q-1`, requires the same signed coefficient in all eleven towers, and requires exactly 128 nonzeros (lines 101-126). It therefore obtains the actual sampled root secret without a family-secret retention hook.

`CandidatePolynomial` computes `c0+c1*s` directly in `Z_q[X]/(X^N+1)` for each expected literal tower (lines 128-170). Since the two degrees are below `N`, their sum wraps at most once; the expression `(secret_sign>0)==(degree<N)` applies exactly the product of the secret sign and the negacyclic wrap sign. Every modular add/sub remains below `2q<2^61`, so `uint64_t` is sufficient for the frozen sub-60-bit moduli. The CRT weight `(Q/q_j)*(Q/q_j)^(-1) mod q_j` reconstructs the unique residue modulo the literal composite Q, after which centering supplies the canonical coefficient representative. This path does not call production decrypt, its FFT, `DecryptCore`, CRT interpolation, or a production ring multiply; the only shared primitive is the explicitly allowed official inverse NTT.

For every pair, decrypting high and low separately and centering `d*high+low mod Q` implements the paper recombination over the independently expected active prefix (lines 172-181). The expected tower counts `10-r` match DCP's Div deletion followed by one Mult-prime deletion per square. Round 0 additionally requires this recombination to equal the fresh polynomial reduced to the Div-deleted modulus coefficient by coefficient (lines 501-514).

The binary512 Horner evaluator uses the original fixed canonical roots `zeta^(5^slot)` only at the ten frozen anchors and divides by the independently derived exact rational scale. It is arithmetically separate from production's decimal160/decimal220 full transform. Thus production decoding and independent Horner do not share a transform implementation, even though both necessarily inspect the same ciphertext and sampled secret at the client boundary (draft lines 365-397; original oracle lines 283-313).

### Chain and ideal

The only evaluator inputs are the immutable public plan and one cloned fresh ciphertext. `Evaluate` performs exactly one DCP and eight calls to `Mult2(pair,pair)`, with no decrypt, callback, refresh, retry, or alternate chain (draft lines 259-303). The ideal starts from the original dyadic `paper_full_test::Inputs()` and is squared once per returned round in binary512, yielding `z^(2^r)` and `z^256` at round 8 (lines 470-529). `ClientInputs` preserves these dyadics through a 100-digit decimal bridge and never converts them through binary64 (original oracle lines 97-123).

The receipt ancestry is complete. There is one Input node; rounds 1-7 each contribute Tensor, Relinearized, Rescaled, and Reentry; round 8 contributes Tensor, Relinearized, and terminal Rescaled. Hence `1 + 7*4 + 3 = 32`, exactly the number of distinct acyclic nodes required from the terminal parent walk (draft lines 184-238, 519-536). Physical family, active prefix, level, terminal lifecycle, exact scale, and parent object identity are checked at each stage.

## Predicate coverage and non-tautology

- Fresh and final production decryptions compare all 16,384 slots and both components to the independent ideal, accumulating a named finite miss when the maximum exceeds `2^-80` (draft lines 333-363, 497-502, 546-554).
- At both endpoints, the test retains codec disagreement `<=2^-120`, positive centered headroom, slot1-real minus slot0-real `>2^-76`, and disagreement from the independent expected delta `<=2*2^-80`. Final ideal slot 0 is checked against the frozen published real/imaginary literals within `2^-150`; its expected witness is required in `(2^-71,2^-70)` (lines 333-429).
- Every final ideal slot must have squared magnitude strictly between `.098^2` and `.106^2`; every actual final slot must be finite and strictly above `.09^2`. The test reports the actual minimum slot and miss count (lines 400-429).
- Fresh, DCP round 0, every returned round 1-8, and the root-wrapped final ciphertext are independently CRT-decrypted and Horner-evaluated at all ten anchors against the corresponding ideal with the original `2^-80` gate (lines 500-528, 550-554). At fresh/final, those anchors also must agree with production decoding, and the independent CRT modulus/max-centered-coefficient/headroom must equal the production diagnostics (lines 365-397).
- The wrong-scale falsifier evaluates the actual independent terminal polynomial at nominal `2^116`, compares anchor 0 with the ideal final value, emits the component error, and requires it to exceed `2^-30`; it does more than assert `S8!=S0` (lines 555-558).

These checks are not a shared-oracle tautology: the exact ideal is plaintext binary512 arithmetic; production full-slot decoding uses the production CRT/decrypt and decimal transforms; the anchor path independently performs signed sparse negacyclic decryption, exact CRT, and binary512 Horner. Literal constants are shared with a frozen test-only header, not populated from production state, and are compared to live production objects.

Round-8 pair Horner and final-wrapper Horner are deliberately **not** two independent numerical observations: the test first proves the root wrapper polynomial equals the family-7 recombination coefficient-for-coefficient (lines 550-553), so the second evaluation is consistency plus production-endpoint agreement. The independent numerical oracles remain the binary512 ideal and the separate sparse-CRT/Horner versus production-decode paths.

The failure classification is also coherent. Finite accuracy, witness, and actual-domain misses use `Gate`, retain measured values/locations, continue through cleanup, and end in result FAIL. Nonfinite data, malformed structure/receipts, nonpositive headroom, codec disagreement, production-versus-independent transform disagreement, and a non-discriminating wrong-scale oracle throw and become `INVALID_OR_INCOMPLETE`; the wrong-scale error is emitted before that check (lines 41-59, 333-429, 625-657). Treating the latter conditions as invalid is appropriate because they invalidate the observation or its oracle rather than establish a finite E80 miss.

## Limits that must remain attached to any future result

- No test was compiled or run in this review. A valid numerical result remains pending hosted execution on the exact integrated source.
- Ten anchors at intermediate stages cannot establish all-slot intermediate accuracy or rule out coefficient-lift aliasing/nonwrap. The draft prints that boundary explicitly (lines 632-635).
- A single randomized key/noise chain on each host, even if both pass, is correctness evidence for those draws, not a Gaussian tail theorem or a universal guarantee.
- The test neither runs nor overwrites the old endpoint protocol. It does not reverse the recorded original-profile E80 FAIL.
- The experimental profile's approximately 712-bit QP exposure remains `security=UNRESOLVED`; no 128-bit claim follows from this numerical slice.

Subject to the separate C++ API/build review and an actual frozen RED/observation run, the draft is suitable for the requested bounded eight-square numerical experiment without weakening the original oracle.

## Narrow active-copy delta closure

Follow-up source-only comparison at baseline HEAD `2a6be7a1ec4528718df47d7c5d6366b3904b3036`: active `tests/experimental_precision116_eight_square_test.cpp` has SHA-256 `478954a08580d3d342a47ad1e54900f4dce455af1964a3d3e2d231cf61b748d2`, versus archived reviewed draft SHA-256 `ff82f162b90a33793506de3bb883b4e12cbc95bea736adbd683d837cca31cea7`.

The complete diff only removes `ScaleOracle`, `CheckReturned`, `InspectAncestry`, `EmitReturned`, terminal receipt-identity, and terminal ciphertext checks from `Evaluate`, then performs the applicable checks after `Evaluate` returns on the client side. The active client path still checks the initial pair/root receipt, emits round 0, checks and emits every returned round 1-8 with the same exact closed-product scales and ancestry, checks the terminal root ciphertext, and later requires the terminal result receipt to equal the final stage receipt while counting the same 32-node chain.

The evaluator still receives only the public plan and ciphertext and still contains exactly one DCP followed by eight `Mult2(pair,pair)` calls and one terminal RCB. All independent input, binary512 ideal, sparse h128 negacyclic CRT, Horner, full-slot endpoint, E80, codec, witness, domain, headroom, and wrong-scale code is unchanged. Therefore this relocation does not alter the numerical/oracle verdict above and better enforces the literal client-versus-evaluator observation boundary.

If `Evaluate` aborts, fewer structural/receipt observations will have been emitted before the process reports `INVALID_OR_INCOMPLETE`; that is reduced partial diagnostics only, not a weakened valid-chain gate, because no numerical result is established unless evaluation returns and every relocated check succeeds. No compile, cryptographic execution, CI, or numerical replay was run for this delta. Reviewer context remains a separate Codex fallback with backend identity unattested, not provider-diverse evidence.
