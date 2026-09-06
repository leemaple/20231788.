# Independent integration review

## Disposition and scope

One bounded correction is recommended before integration; no other C++/public-API, ownership, terminal-binding, cleanup, rejection, CMake, or patch-applicability blocker was found in this source-first review.

This review was performed by the separate Codex context `endpoint_interop_workflow` using GPT-5. It is independent context, not provider diversity and not a numerical-oracle review. The inspected repository was `/Users/lifeng/Documents/20231788-openfhe-precision116-eight-square-20260907` at HEAD `2a6be7a1ec4528718df47d7c5d6366b3904b3036`; `git diff --name-status 2759fa90840946ef42957c7ba71ebea47e0e4995 -- src include tests CMakeLists.txt` was empty, so the active production/test/CMake bytes were still the stated tested production source. I read the frozen `TASK.md` before the returned author review and design documents.

## Finding

### [P2] Keep the evaluator boundary and retained static claim literally aligned

The test describes `Evaluate(plan, input)` as having no oracle path, but it calls the independently defined `ScaleOracle()` and then performs candidate expected-scale/state/receipt checks inside the evaluator function (`experimental_precision116_eight_square_test.cpp:264-287`). The governing brief asks that secret/oracle observations remain outside evaluator execution, and the returned `checks/STATIC_CHECKS.txt` more broadly claims that the evaluator contains no oracle call. The function is genuinely secret-free, client-free, callback-free, and polynomial-oracle-free, so this does **not** contaminate ciphertext arithmetic or expose a key; the issue is the literal isolation/assurance claim.

Required minimal resolution: leave only DCP, the eight `Mult2` calls, operand/input immutability checks, required nonterminal rejection calls, and the terminal `RCBWithReceipt` call inside `Evaluate`. Perform `CheckReturned`, `InspectAncestry`, and `EmitReturned` after the returned `Evaluation` reaches the client-side caller. Those checks are already repeated there for the initial pair and all eight stages (`:504-536`), so this is a small move/removal rather than a new framework. Keep the governing boundary unchanged and update the retained static claim only after the source literally satisfies it.

## Source compatibility and semantics

- The draft uses only public test-facing APIs: `CreateExperimentalPrecision116Setup`, immutable `shared_ptr<const RepeatedMult2Plan>`, public pair/receipt getters, `DoubleCKKS::{DCP,Mult2,RCBWithReceipt}`, `HighPrecisionClientIO::{Encrypt,BindRepeatedRcb,Decrypt}`, and public OpenFHE key/ciphertext observations. It does not require a new friend, production seam, or mutable receipt constructor.
- `RepeatedMult2Result` is constructible only by the evaluator/client friends, retains the issuing plan, exposes a const-owned ciphertext snapshot and exact terminal receipt (`include/openfhe_2023_1788/repeated_mult2.h:95-114`). The aggregate `Evaluation` return is compatible with the declared copy/move constructors; no default construction or assignment of `RepeatedMult2Result` is required.
- Production `Mult2` dispatches by the receipt's issuing family and returns terminal Rescaled directly instead of re-entering after family 7 (`src/double_ckks.cpp:1213-1224`). `RCBWithReceipt` requires the plan-issued terminal Rescaled receipt and original root prefix before wrapping at absolute level 9 (`:1246-1283`). The draft's round-8 family7/local-level2 and root-wrapper/level9 checks match that API.
- `BindRepeatedRcb` rejects a different issuing plan, revalidates the live result, adopts the terminal receipt's exact rational scale, and checks the two-tower root wrapper (`src/high_precision_client_io.cpp:659-675`). The successful binding and foreign-client rejection in the draft therefore exercise the intended public boundary rather than a private shortcut.
- Exact receipt ancestry is coherent: Input plus seven Reentries plus eight Tensor/Relinearized/Rescaled triples yields the asserted 32-node chain. The round-8 returned receipt is correctly treated as terminal Rescaled itself; earlier returned Reentry receipts correctly use their parent Rescaled node (`experimental_precision116_eight_square_test.cpp:224-237,530-536`).
- The terminal binder is invoked once successfully after round 8. The other calls are the required negative Input, first-Reentry, and foreign-plan probes, all expected to throw `std::invalid_argument`; production's validation paths use that exception type.

## Ownership, immutability, and cleanup

- The evaluator signature receives only the const plan and read-only ciphertext. No secret, client, callback, decryption, refresh, bootstrap, or re-encryption enters the evaluated chain. Candidate secret extraction and sparse polynomial reconstruction occur in the client-side caller before/after `Evaluate` (`experimental_precision116_eight_square_test.cpp:468-559`).
- Every square snapshots both operands and checks their full ciphertext values and live frozen basis afterward; the original evaluation input is likewise checked after the chain (`:249-256,278-301`). Public/root-secret object identities, values, contexts, and tags are compared with pre-chain snapshots (`:562-570`). The terminal bound object is checked to survive mutation of a separate clone (`:538-545`).
- The candidate test retains only weak references to candidate plans, keys, evaluation rows, ciphertexts, and receipts. Candidate row content is not duplicated. Only the small N64 diagnostic rows are copied while that unrelated setup stays live (`:455-460,575-613`).
- Production owns row cleanup in `RepeatedMult2Plan::Data::~Data`, clearing only family tags acquired after an absence check (`src/repeated_mult2.cpp:245-255,351-377`). The test checks candidate weak-owner expiry, absence of all candidate tags, preservation of the live diagnostic rows/keys, subsequent diagnostic usability, and restoration of the original evaluation-key maps.
- The only paper-sized setup call is `CreateExperimentalPrecision116Setup()` in the executed test path. The included one-operation helper defines another inline runner but the new `main` does not call it. Foreign client rejection creates only the already-supported small context and no keys or encrypted chain.

## CMake and patch applicability

- The returned `CMakeLists.txt` differs from the baseline only by the final 24-line option-gated block. With `OPENFHE_2023_1788_ENABLE_EXPERIMENTAL_PRECISION116_EIGHT_SQUARE=OFF`, neither the target nor CTest is registered, preserving the old default build/registry. With it ON, the target remains `EXCLUDE_FROM_ALL`, is warning-as-error enabled, links only the existing project library, and registers one serial `OMP_NUM_THREADS=2`, `TIMEOUT 1200` CTest.
- Root-owned integration precondition: enabling the option registers the test even though the executable is excluded from `all`. Every legacy list/run selector must therefore exclude `^experimental_precision116_eight_square_contract$` before the target is explicitly built and selected once. The old endpoint runner/select/upload paths must remain disabled on the dedicated activation ref as already documented. This is pending workflow integration, not satisfied by the returned CMake alone.
- `git apply --check --whitespace=error coordination/precision116-eight-square-return-01/pro/patches/precision116-eight-square.patch` exited 0 at the stated HEAD. Patch postimage Git blobs match the returned full files: CMake `8ecbe4afdfff4c62f4e85e2c721c2aad80eb5fb1`, test `7d15de2e6618e4f14b31795f7409dd79e939e897`. The supplied SHA-256 identities also matched: CMake `9cba634cedde1e8d96aff8561f5b4ed545b52fac8f1ce523e481a7833e1936f8`, test `ff82f162b90a33793506de3bb883b4e12cbc95bea736adbd683d837cca31cea7`, patch `0993f66f451f0b25e630f2f1cc51a118c11e323cafb072ba453652f7048152f2`.

## Assurance boundary

Observed here: source/API consistency, exact active-source identity, patch applicability, postimage equality, and static ownership/terminal-flow reasoning. The old production/test/CMake surface remains unchanged until this patch is applied.

Not performed or established here: C++ compilation/linking, warning cleanliness on GCC/MinGW, CTest execution, cryptography, runtime cleanup, numerical oracle validity, E80 success/failure, full-slot replay, CI, or security. Those remain pending the root-owned reviewed integration and first retained Linux/Windows execution. Numeric-oracle adequacy and actual numerical classification belong to the separately assigned review.
