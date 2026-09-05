# Repeated Mult2: two-operation semantic candidate

**Status: SOURCE CANDIDATE DELIVERED. Compile, cryptographic runtime and hosted acceptance: NOT RUN.**

This is a new, locally authored RED/GREEN candidate from the complete recovery packet. No source edit or artifact from the failed execution session was accepted or used. The supplied previous Pro return was treated as reviewed evidence, not imported implementation.

## Exact identities

| Object | Identity |
| --- | --- |
| Implementation baseline | `80d771c52df10bce1c60992b5e0edb4e64f145ca`; 40 supplied project files |
| Intended branch | `codex/repeated-mult2-semantic-01` |
| Pristine OpenFHE 1.5.0 source pin | `df495ba2e91739a6dc8f1de254fc5a41155ce504` |
| Input ZIP | 1,299,850 bytes; SHA-256 `764baddb20d81c1168745ac31eb043d0d94cf1ba6b406d0194f9245a994196a2` |
| Current TASK | 21,814 bytes; SHA-256 `40839c3450028f91fd8dc6bb3509e9dc848ec4168d82f081e94d7d4997fafe48` |
| TASK overlay commit | `4a14245412baf58c40de2cd2d60cd36ab19dc10a`; documentation only, not an implementation-baseline blob |
| Frozen exact vectors | 13,593 bytes; SHA-256 `6a0dae07b55adf8552272407d4e8885b1e993808d9914fc250b508ecc8d772e6` |

## Delivered behavior and scope

The client creates two public basis/key families, retains its root secret separately, and supplies encrypted inputs. The evaluator has only a public immutable plan and ciphertexts. Its semantic evaluation is:

```cpp
DoubleCKKS evaluator(clientSetup.plan);
const auto X = evaluator.DCP(encryptedX);
const auto Y = evaluator.DCP(encryptedY);
const auto Z = evaluator.Mult2(X, Y);
const auto W = evaluator.Mult2(Z, Z);
```

The first result is re-entered internally with new wrappers in the next family; coefficients and logical normalization do not change at that boundary. The second result is terminal. No intermediate client secret, decryption, re-encryption, bootstrap, or Section 6.2 refresh is involved. This is a low-N two-operation diagnostic, not production lossless I/O, eight operations, h=128, a security result, or full paper reproduction.

The test freezes N=64, batch=16, depth=9, scaling bits=50, first bits=55, exact input scale 2^100, FIXEDMANUAL/HYBRID/COMPLEX, UNIFORM_TERNARY and HEStd_NotSet. It requests four fresh root keypairs and checks all 16 complex Z and W slots and both distinguishing deltas at absolute error <=2^-80. Those are executable assertions, **not observed cryptographic measurements** in this return.

## Apply in order

In a clean checkout/copy of the specified baseline, with an absolute `RETURN` path to this extracted package:

```sh
git apply --check "$RETURN/patches/0001-red-repeated-mult2-semantic-two-square.patch"
git apply "$RETURN/patches/0001-red-repeated-mult2-semantic-two-square.patch"
# RED build is for the independently authorized Linux/Windows runner; NOT RUN here.
git apply --check "$RETURN/patches/0002-green-repeated-mult2-semantic-two-square.patch"
git apply "$RETURN/patches/0002-green-repeated-mult2-semantic-two-square.patch"
```

RED adds only three test files, CMake and the minimum workflow binding. Its predicted build failure is the absent `openfhe_2023_1788/repeated_mult2.h`, which GREEN provides. GREEN adds/changes production headers/source and its CMake source binding, without changing any RED test. GREEN alone is rejected by an actual patch check. The exact replay/complete-file comparison passes.

`complete/project/` contains exactly the nine changed/new final files, **not a standalone full checkout**. The unchanged 36 final files come from the original 40-file baseline; the final selected tree contains 45 files. No legacy test file or assertion was edited. All 57 original CTest name/command/order bindings remain; the only appended entry is `repeated_mult2_semantic_two_square_contract`.

## Reproduce offline checks

Python 3.9+ and the Git command-line tool are sufficient. This command does not compile or execute cryptography and does not use the network:

```sh
python "$RETURN/verify_candidate.py" --package "$RETURN" \
  --input-archive "/path/to/repeated-mult2-semantic-implementation-01-80d771c.zip" \
  --input-sidecar "/path/to/repeated-mult2-semantic-implementation-01-80d771c.zip.sha256"
```

The uploaded ZIP basename may include `(1)`; the verifier checks content identity and the original filename recorded inside the sidecar. It validates packet closure, official source payloads, exact vectors and literal transcription, ordered patch replay, the nine complete files, unchanged tests, bindings, bounded source checks and this package manifest. It is not a substitute for a C++ compiler, cryptographic execution or independent review.

## Review navigation

`DESIGN_DECISION.md` explains the two-family state machine and ownership. `SOURCE_CLAIM_TEST_LEDGER.md` and `verification/SOURCE_LINE_INDEX.json` bind claims to actual source lines. `RED_GREEN_EXECUTION_LEDGER.md` separates observed checks from unexecuted gates. `EXPECTED_CTEST_BINDINGS.tsv` records all 58 bindings. `NEXT_GATES.md` keeps later work out of this slice. `PACKAGE_INTEGRITY.md` defines closure/provenance and the outer sidecar.

OpenFHE must be built without the optional `DEBUG_KEY` macro, which stores a private key in the context in the pinned source. A compile-time guard rejects that configuration. A normal CMake Debug build does not itself request `DEBUG_KEY`.
