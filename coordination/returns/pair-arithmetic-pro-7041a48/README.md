# Pair Add/Sub clean-room delivery

**Status: `READY_FOR_CODEX_INTEGRATION`**

This status means the source candidate, ordered mail patches, static audits, and exact final selected-project tree are internally consistent and ready for hosted compilation/review. It does **not** mean that OpenFHE compilation, CTest, MinGW64, or GitHub Actions passed in this environment.

## 1. Bound identities

- Input archive: `pair-arithmetic-pro-7041a48.zip`
- Input bytes: `900942`
- Input SHA-256: `50269f2a0f5198d5f4aee312808097370e6153783f7586cb1e9c0446da133c38`
- Exact project source commit named by the packet: `7041a489ae1afa98b75322ec334543f29f10b738`
- Target branch: `codex/pair-arithmetic-01`
- Pristine OpenFHE 1.5.0 commit named by the packet: `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- Final selected-project Git tree produced by applying these patches to the supplied `project/` bytes: `32e9cf3a5e66c3e87018fe8a344593df24b8bb2a`

The local commit hashes in `COMMIT_ORDER.tsv` are content-review identifiers created over the packet's selected archive. Applying the mail patches to the actual repository commit may produce different commit hashes because the parent object and committer metadata differ; the file result is bound by the tree/file hashes instead.

## 2. What was implemented

The public API is exactly:

```cpp
CiphertextPair Add(const CiphertextPair& left, const CiphertextPair& right) const;
CiphertextPair Sub(const CiphertextPair& left, const CiphertextPair& right) const;
```

Both operations:

1. validate `left` independently;
2. validate `right` independently;
3. apply one narrow pair-compatibility check;
4. clone the corresponding left high and left low ciphertexts;
5. apply direct DCRT component arithmetic to the two validated RLWE components;
6. preserve the left manifest/lifecycle/scale facts;
7. validate the fresh result before returning it.

No `EvalAdd`, `EvalSub`, rescale, modulus reduction, normalization, relinearization, key generation, cache write, automatic alignment, tuple generalization, public mutable factory, or Mult2 change was added.

## 3. Required red—green patch order

Apply the patches strictly in lexical order.

| No. | Patch | Intended TDD state after application | Local execution status |
|---:|---|---|---|
| 01 | `0001-test-red-add-pair-add-public-api-compile-contract.patch` | Add compile-red: explicit target refers to the absent public member | Not compiled |
| 02 | `0002-feat-green-expose-throwing-pair-add-scaffold.patch` | Add API scaffold-green: exact const signature plus a throwing body | Not compiled |
| 03 | `0003-test-red-require-executable-pair-add-behavior.patch` | Add runtime-red: registered real call rejects the scaffold diagnostic | Not run |
| 04 | `0004-feat-green-implement-minimal-componentwise-pair-add.patch` | Minimal Add green source state | Not run |
| 05 | `0005-test-red-add-pair-sub-public-api-compile-contract.patch` | Sub compile-red: explicit target refers to the absent public member | Not compiled |
| 06 | `0006-feat-green-expose-throwing-pair-sub-scaffold.patch` | Sub API scaffold-green: exact const signature plus a throwing body | Not compiled |
| 07 | `0007-test-red-require-executable-pair-sub-behavior.patch` | Sub runtime-red: registered real call rejects the scaffold diagnostic | Not run |
| 08 | `0008-feat-green-implement-minimal-componentwise-pair-sub.patch` | Minimal Sub green source state | Not run |
| 09 | `0009-test-add-independent-crt-and-rcb-pair-arithmetic-oracle.patch` | Independent `cpp_int` coefficient/CRT and public-RCB oracle | Not run |
| 10 | `0010-test-cover-all-pair-lifecycles-and-evaluation-key-independence.patch` | Three public lifecycles, real/complex fixtures, targeted key removal | Not run |
| 11 | `0011-test-reject-incompatible-and-malformed-pair-operands-before-arithmetic.patch` | Compatibility and malformed-public-accessor rejection matrix | Not run |
| 12 | `0012-ci-build-pair-arithmetic-api-contracts-on-target-branch.patch` | Separate workflow branch/API-target wiring | Not dispatched |

The source-state transitions above were inspected commit by commit; see `checks/red-green-source-state.txt`. They are not substitutes for observing compiler/runtime red and green on pristine OpenFHE.

## 4. Applying to the exact repository baseline

From a clean checkout of the named source commit:

```bash
git switch --detach 7041a489ae1afa98b75322ec334543f29f10b738
git switch -c codex/pair-arithmetic-01
git am /absolute/path/to/patches/*.patch
```

Before applying, confirm that the real checkout is clean and that its relevant files match `INPUT_SOURCE_MANIFEST.json`. The full 12-patch sequence was locally replayed with `git am` against the exact supplied `project/` bytes and reproduced tree `32e9cf3a5e66c3e87018fe8a344593df24b8bb2a`; see `checks/patch-apply.txt`.

## 5. Required hosted Linux/GCC verification

Use a pristine install built from OpenFHE commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`:

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=<pristine-openfhe-1.5.0-install>

cmake --build build --parallel 2
cmake --build build --target pair_add_api_contract_test --parallel 2
cmake --build build --target pair_sub_api_contract_test --parallel 2

ctest --test-dir build -R '^pair_' --output-on-failure
ctest --test-dir build --output-on-failure
```

To preserve actual TDD evidence rather than only test the final tree, execute the relevant target/test after each of patches 01–08 and record the expected failure or success before moving to the next patch.

## 6. Required hosted Windows/MinGW64 verification

In an MSYS2 `MINGW64` shell, with the pristine OpenFHE prefix on disk:

```bash
prefix='<pristine-openfhe-1.5.0-install>'
build='build-mingw64'

cmake -S . -B "$build" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$prefix"

cmake --build "$build" --parallel 2
cmake --build "$build" --target pair_add_api_contract_test --parallel 2
cmake --build "$build" --target pair_sub_api_contract_test --parallel 2

export PATH="$prefix/bin:$prefix/lib:$PATH"
ctest --test-dir "$build" -R '^pair_' --output-on-failure
ctest --test-dir "$build" --output-on-failure
```

The supplied workflow now builds both API contract targets in the `linux-gcc` and `windows-mingw64` jobs and includes `codex/pair-arithmetic-01` once in the push branch list. That wiring was parsed and inspected locally; it was not dispatched here.

## 7. Package layout

- `patches/`: the 12 ordered `git format-patch` files.
- `final-project/`: complete final selected-project files, including unchanged baseline files.
- `final-changed-files/`: only the nine added/modified final files.
- `BASELINE_TO_FINAL.diff`: aggregate patch from supplied selected baseline to final candidate.
- `COMMIT_ORDER.tsv`: order, local content commit, and subject.
- `SOURCE_FACT_AUDIT.md`: paper/OpenFHE/design mapping and implementation boundary.
- `TEST_CLAIM_LEDGER.md`: claim-to-test map with `EXECUTED`, `SOURCE-INSPECTED`, and `NOT EXECUTED` states.
- `INPUT_TASK.md`: byte-identical copy of the governing task (`a026b4b2…334c8`).
- `INPUT_SOURCE_MANIFEST.json`: byte-identical copy of the verified source manifest.
- `INPUT_VERIFICATION.md`: outer hash, ZIP, manifest, and source identity verification.
- `EXECUTION_NOTES.md`: exact local execution boundary and hosted commands.
- `checks/`: raw static/replay/dependency-probe outputs.
- `PATCH_HASHES.sha256`: every mail patch hash.
- `FINAL_PROJECT_HASHES.sha256`: every file under `final-project/`.
- `CHANGED_FILE_HASHES.sha256`: every file under `final-changed-files/`.
- `FILE_HASHES.sha256`: every package file except `FILE_HASHES.sha256` itself.
- `DELIVERY_MANIFEST.json`: machine-readable identity, contents, and execution status.

## 8. Result boundary

Locally executed: input hash/manifest checks, PDF/source inspection, patch replay, Git tree/diff checks, YAML parsing, static source-state scans, complete package-hash verification, and a CMake dependency probe.

Not executed: project configure completion, compilation, warning build, API target build, focused CTest, full CTest, Windows/MinGW64 build, GitHub Actions, decoded numerical accuracy, performance, security, or Mult2 theorem-normalization evaluation.
