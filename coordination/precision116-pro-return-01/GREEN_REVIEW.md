# Independent GREEN source review

## Findings

No actionable source-level defect was found in the four allowed production-file changes.

This is a conditional **source-review acceptance**, not implementation or precision acceptance. The GREEN patch is suitable to apply only after the separately owned RED has been accepted. It remains uncompiled and unexecuted in this review; Linux/Windows compilation, construction of the pinned OpenFHE contexts and keys, and the one-operation experimental test remain runtime-pending.

## Review identity and scope

- Reviewer: `/root/endpoint_interop_workflow`, an independent Codex agent context (GPT-5 family). This review does not claim provider diversity.
- Authoritative source at review dispatch: `2745ab44f185b9743d670f8acdf668f69a061980` on `codex/precision116-profile-seam-20260907`, descended from engineering base `dbbbee0d20d8a7ae3c138e42f633414db621a173`.
- During review, root committed the immutable return intake at `4647c5bb23f6b0f84900b3009e7d1f52c2f59ddf`. The intervening diff adds only the return/intake material under `coordination/precision116-pro-return-01/`; it does not modify active production, test, CMake, or workflow files.
- Authoritative task SHA-256: `e55068b18b1bdf7e3714d085598d80ec62ff6ca558e90a225c018709f8e4f26f`.
- Verified input packet SHA-256: `75c29c9f1305c44fc64679827dcfc1e8b3ba475b48ab46900f71849c255f9a7b`.
- GREEN patch SHA-256: `9273110e06fec7ed26788e7ef7f125d07bd28120e1a0887bd8906ec3e41c0dc1`.
- Full-copy SHA-256 values:
  - `include/openfhe_2023_1788/repeated_mult2.h`: `f95acc03b0feecd78a4763c9f776cbb3e0f6429860d71e81cc6ceb75e47f7cf3`
  - `src/double_ckks.cpp`: `c530b53d600aeb0c295477aebb84bddff2c25b4a139004cdc888edd5dba425eb`
  - `src/high_precision_client_io.cpp`: `57bbd6a7e0312dca4eb6adb90e236d3ff0334938c1da14c142213b2e871a8933`
  - `src/repeated_mult2.cpp`: `6781d01ff9b4012e7c6d14d16f482def0821c54f4e81e2de80bd8db9d1aea41b`
- The patch applies cleanly in a read-only `git apply --check`. Its new-side Git blob IDs exactly match the four supplied full copies. The bounded delta is 88 insertions and 32 deletions across only the four allowed production files.
- Independence order: I read the task, GREEN patch and four GREEN full copies before reading the author's `DESIGN.md`, `EXECUTION_LEDGER.md`, or `TEST_PLAN.md`. I did not read or execute the author's static checker before the independent first pass.

## Source review

### Profile identity and isolation

The patch adds one named public factory and only the private access needed by the existing client binding; it does not add a caller-configurable profile framework (`GREEN/include/openfhe_2023_1788/repeated_mult2.h:58-89`). The immutable descriptor preserves the original profile at metadata exponent 50 and defines the experimental profile at exponent 58 with exactly these changed entries:

- Base0: `(288230191468118017, 43136605093011213)`
- Base1: `(288230165698314241, 82872750907637397)`
- Div: `(72057589742960641, 50608680790172261)`

The middle eight ordered Q entries are references to `kPaperQ[2]` through `kPaperQ[9]`, and P remains exactly `kPaperP` (`GREEN/src/repeated_mult2.cpp:22-51`). The profile pointer is restricted to the two immutable named descriptors; family count and transition checks retain the eight-family, delete-second-last rule and never delete Div (`GREEN/src/repeated_mult2.cpp:257-277`, `443-470`).

The original factory still selects `kPaperProfile` with exponent 50 (`GREEN/src/repeated_mult2.cpp:40`, `465-466`). The context-only client path still defaults to 50 and exact `2^100`, and the first-operation context-only binder continues to require `2^100` (`GREEN/src/high_precision_client_io.cpp:457-470`, `613-656`). Therefore the new profile is opt-in and does not silently change the original paper or context-only behavior.

### Exact and recorded scale state

For a plan-bound client, the metadata exponent now comes from the issuing immutable profile, while the exact fresh scale comes from its issued Input receipt (`GREEN/src/high_precision_client_io.cpp:176-245`, `457-502`). Encryption validates that exact rational against the supplied encoding specification (`GREEN/src/high_precision_client_io.cpp:544-610`). Thus the experimental factory requires exact fresh `2^116`; it does not accept the original `2^100` state by accident.

The receipt construction preserves the exact recurrence:

`Tensor = S_r^2 / d`, then `Rescaled = Tensor / m_r`.

It uses exact integer/rational arithmetic for the logical scale and derives recorded factors from the profile metadata factor (`GREEN/src/repeated_mult2.cpp:279-311`). With exponent 58, fresh/Rescaled recorded state is `2^116` and Tensor/Relinearized recorded state is `2^174`; with exponent 50, the existing `2^100`/`2^150` behavior remains. Terminal root validation now asks the issuing plan for its expected recorded factor, while the native `d*high+low` recombination and wrapper mechanics remain unchanged (`GREEN/src/double_ckks.cpp:1246-1282`). The only `double_ckks.cpp` patch hunk is that profile-derived terminal expectation; DCP, tensor, relinearization, RS, encryption, decryption, and refresh arithmetic are not altered by this patch.

### OpenFHE compatibility and ownership

The supplied pristine OpenFHE sources support the static API shape used here:

- `references/official-full/src/pke/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp:46-49` delegates CRT precomputation, and `:171-175` defines FIXEDMANUAL's approximate factor as `2^plaintextModulus`.
- `references/official-full/src/pke/include/schemerns/rns-cryptoparameters.h:603-619` exposes the corresponding FIXEDMANUAL scaling factor, and `:637-647` exposes the mod-reduce factor.
- `references/official-full/src/pke/lib/schemerns/rns-cryptoparameters.cpp:127-166` constructs HYBRID P using the maximum partition size and 60-bit limbs. All proposed Q entries remain at most 60 bits and are distinct from the retained P, so the source-level one-P/alpha-one assumptions are coherent. Actual generated P/QP identity is still a runtime obligation.
- `references/official-full/src/pke/include/scheme/ckksrns/ckksrns-cryptoparameters.h:70-88` matches the unchanged CKKS-RNS parameter-constructor call shape.

Those citations are members of the verified input packet named above. The new private plan accessors are reachable through the pre-existing `ClientContextBinding` friendship, and the new named factory is explicitly friended so it can construct the private plan data (`GREEN/include/openfhe_2023_1788/repeated_mult2.h:76-92`). No source-level declaration/definition mismatch was found. This is a static compatibility conclusion only, not a compiler result.

The setup still creates one root h128 client key, projects that secret into each exact modulus/root/phi family only within client setup, destroys each local projected private key at the end of its scope, stores no secret in the plan, refuses key-row overwrite, seals every generated row, and clears only its owned tags on destruction (`GREEN/src/repeated_mult2.cpp:245-255`, `347-399`, `443-463`). The experimental profile reuses this existing ownership boundary rather than introducing a key adapter or cleanup path.

## Preserved limitations and required next evidence

- The source explicitly labels the experimental profile security unresolved and E80 not tested (`GREEN/src/repeated_mult2.cpp:41-44`; public factory warning at `GREEN/include/openfhe_2023_1788/repeated_mult2.h:61-62`). This patch provides no security qualification.
- The proposed change is a profile seam plus one-operation target. It does not prove intermediate non-wrap, the eight-operation/full-slot E80 target, or projected-family-key equivalence beyond existing structural validation.
- No compiler, OpenFHE construction, key generation, encryption, crypto operation, numerical replay, CTest, or hosted CI was run by this reviewer. Those are deliberately pending the accepted RED followed by the hosted GREEN path.

Disposition: **no actionable GREEN source finding; conditionally source-accepted for application after the actual RED is accepted. Runtime and scientific acceptance remain open.**
