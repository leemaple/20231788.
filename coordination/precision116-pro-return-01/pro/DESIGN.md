# EXPERIMENTAL-PRECISION116-PROFILE-SEAM-01 — draft design

**Disposition: complete RED/GREEN source drafts; implementation acceptance pending.** Project **NOT COMPILED / NOT RUN** here. Original E80 **FAIL** remains unchanged. Candidate E80 **NOT TESTED**, security **UNRESOLVED**, adoption **NOT ADOPTED**. No independent implementation review is claimed.

## Authority and patch bases

The input archive, all 198 regular members/CRCs, self-excluding manifest and all 197 payload size/SHA256 records were verified. The 110 supplied Git-blob identities were also recomputed. This verifies captured bytes, not live Git ancestry or the original scan environment.

Engineering base: `dbbbee0d20d8a7ae3c138e42f633414db621a173`. Documentation packaging commit: `21d94c23a7163c4770e8ec377219ea54d4428e2d`. Dependency: pristine OpenFHE 1.5.0, `df495ba2e91739a6dc8f1de254fc5a41155ce504`, native64/backend4. RED applies to the packet's `project/` directory; GREEN applies to that exact tree **after RED**. No patch path has a `project/` prefix. No workflow is changed.

Candidate authority is the unchanged `project/coordination/fs-precision-profile-feasibility-01/candidate.json` (SHA256 `94189f13c259778f5c48a0e18d485aceade79645a620c5e223cea6c96a498f3f`) and `STATIC_CERTIFICATE.json` (SHA256 `b7e8590a5ece57c187d01b1aabe2b9f685744f544c1f150d4fcba66acebd1b2d`). The top-level TASK overrides historical recommendations. In particular this is not the older report-contract proposal.

## Fixed descriptor and private access

`GREEN/src/repeated_mult2.cpp:22–51` keeps the original `kPaperQ` and `kPaperP` declaration block byte-identical. An internal `PaperGeometryProfile` contains only eleven ordered Q modulus/root pairs, one P pair and the base metadata exponent. Two `constexpr` objects exist: original metadata50 and experimental metadata58. The latter substitutes only Base0, Base1 and Div, referencing the original eight middle entries and P directly. The frozen witnesses 5, 7 and 11 are verified by the static checker and RED test; no new witness-driven runtime parameter-selection mechanism is introduced.

`Family` and private `Plan::Data` retain a pointer to their immutable issuing descriptor; null denotes the unchanged small diagnostic profile. The plan rejects any other descriptor identity. The original and new named factories call `Data::CreatePaperGeometrySetup` with different fixed descriptors. This private, implementation-only builder factors the old eight-family factory without exposing its descriptor or a generic public configuration API. Neither this static member nor the plan stores a private key: setup-local key variables remain inside the existing client-setup boundary.

The only public API addition is:

```cpp
RepeatedMult2ClientSetup CreateExperimentalPrecision116Setup();
```

Two private getters provide the narrowly necessary metadata authority: `BaseMetadataExponent()` and `ExpectedRecordedScalingFactor()`. Existing friends consume them; only the new factory friendship is added. There is no runtime k argument, public profile enum, alias redirecting the old factory, public I/O method, projected-secret hook or evaluator private-key member.

### Metadata is not logical normalization

For each descriptor, F is exactly `2^baseMetadataBits`; Input/RS recorded metadata is F² and Tensor metadata is F³. Experimental values are F=2^58, F²=2^116, F³=2^174. The plan's Input exact rational is `2^(2*baseMetadataBits)`. The existing receipt recurrence is untouched:

\[
S_r=\frac{S_{r-1}^2}{d\,m_r}.
\]

Every family still deletes the second-last Q tower, with Q counts 11 through 4 and Div retained. The unchanged Tensor/RS code derives metadata from actual FIXEDMANUAL factors, while exact receipts use the actual divisor and consumed modulus. This distinction is required by the pinned getters: `references/official-full/src/pke/include/schemerns/rns-cryptoparameters.h:601–649`; FIXEDMANUAL initialization is at `references/official-full/src/pke/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp:171–176`. The constructive Tensor division is described in `references/paper/PAPER-2023-1788.txt:608–660`; the existing complete scale composition is `project/src/repeated_mult2.cpp:254–285`.

`ClientContextBinding` obtains a plan's fresh logical scale from its issued Input receipt, not from arbitrary live context metadata. Plan-bound encoding, fresh-state and terminal-state checks use that authority. Without a plan, metadata50, exact S100, N64/S16 geometry and the stricter original diagnostic basis/partition acceptance remain. `BindFirstMult2Rcb`, both transforms/rounding, encryption arithmetic, decryption and terminal-only `BindRepeatedRcb` are not relaxed. `double_ckks.cpp` changes exactly one expression: the terminal root wrapper's recorded-scale expectation now comes from the issuing plan.

## Ownership and source-preserved invariants

The shared installer, signed-h128 validation and per-tag `Data` destructor are byte-preserved. The original factory body is preserved modulo its private descriptor factoring; the verifier reconstructs and compares it exactly. There is still one root h128 sampling call, projection by unique `(modulus, root, cyclotomic order)` matches, eight distinct family contexts/tags/rows, existing coefficient seals and per-owner cleanup. No family secret is retained after its evaluation row is generated. Original locators: `project/src/repeated_mult2.cpp:320–367,412–433`.

The generic fixed-Q adapter is unchanged. Its public structural checks already cover actual prime/root identity, sparse-secret shape, mode implementations, public-key Q and HYBRID table dimensions/values: `project/src/paper_h128_client_keypair.cpp:68–160,193–217`. The RED checks use only getters with public size guards; hidden complementary/rescale table dimensions are not speculatively indexed. Public P/P-inverse and single-P PHat values follow `references/official-full/src/pke/lib/schemerns/rns-cryptoparameters.cpp:186–214`.

Public encryption, sigma3.19, execution/noise modes and dense ternary ephemeral sampling are unchanged. The latter is distinct from the sparse root secret: `references/official-full/src/pke/lib/schemerns/rns-pke.cpp:148–193` and `references/official-full/src/core/include/math/ternaryuniformgenerator-impl.h:55–67`.

## RED behavior and boundaries

The test-local header adds no initializer with runtime side effects. Main gains one early argument dispatch; removing that insertion and its include reproduces the complete original CPP byte-for-byte. CMake only appends the new serial/OMP2/1200-second CTest; all 61 existing registrations, including the old full-chain entry, retain their names, commands, order and properties. The executable remains excluded from the default build.

The public checks cover candidate Q/P/QP/roots, geometry, metadata58, alpha-one tables, unique ordered root subsets, root signed h128 and family row/context/tag shapes. It then rejects context-only candidate I/O and a plan-bound S100 request, encrypts all frozen original inputs at S116, performs DCP and **one Mult2**, and checks:

```text
Input(family0,op0,S0)
  -> Tensor(family0,op1,S0²/d)
  -> Relinearized(family0,op1,S0²/d)
  -> Rescaled(family0,op1,S0²/(d*Mult7))
  -> Reentry(family1,op1,S0²/(d*Mult7))
```

Returned pair wrappers have local level1, nine active towers, noise degree2, recorded2^116 and family1's tag. The nonterminal result must still be rejected by `RCBWithReceipt`; this seam neither adopts it into terminal-only client I/O nor decrypts it. The receipt chain proves exposed state/normalization integration, not ciphertext lineage or correct evaluation-key mathematics.

Root keys and inputs are compared with bounded snapshots. A fresh clone revalidates the plan's already-owned all-family coefficient seals; the test does not copy all large evaluation rows. Weak handles check release of plan, keys, observed ciphertexts, receipts and rows. An independently owned N64 setup's small row coefficients/identities survive candidate cleanup; after that setup is released, the prior evaluation-key cache identities are restored. Upstream factory context interning is intentionally not blanket-cleared: it has only an all-context release API, which would interfere with other contexts (`references/official-full/src/pke/include/cryptocontextfactory.h:51–85`). This is evaluation-key ownership cleanup, not a new context-registry reclamation policy.

## Remaining uncertainties and the next gate

Static exact arithmetic confirms all nine certificate scales; S8/S0≈1.0000152026932967 and final Q/S8≈0.9999834266523009. These are not measured ciphertext errors. New encoding/noise realizations, eight-square original-input accuracy, intermediate Tensor/Relin nonwrap and all-key correctness remain unproved. A successful one-operation test would add bounded execution evidence, not prove the destroyed projected secret or every HYBRID table.

The paper's original N32768/h128/approximately680-bit exposure and empirical average are described at `references/paper/PAPER-2023-1788.txt:1562–1592`. Approximately712 QP bits in this candidate cannot inherit its security claim. The static fresh-error budget remains conditional; increasing d together with S0 does not imply a 2^-16 reduction in normalized omitted low×low error. The all-slot, both-component, every-recombined-stage premise is not established by ten anchors or by this test. No input/key selection, retry or criterion replacement is introduced.

**Next real gate:** root records an actual hosted missing-factory compile RED, then applies this unchanged GREEN, obtains warning-clean Linux/Windows builds, the new one-operation result and legacy checks, and commissions independent review. No full eight-square run is authorized by this draft. Original E80 failures and the incomplete full paper-reimplementation goal remain in force.
