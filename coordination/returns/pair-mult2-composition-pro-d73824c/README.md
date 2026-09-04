# Pair Add/Sub result -> first Mult2 regression candidate

Status: **TEST-ONLY CANDIDATE READY FOR CODEX HOSTED EXECUTION**  
Reviewer/authoring model: **GPT-5.6 Pro**  
Exact tested baseline source: `d73824c2d382013c3aadbd7cb29c57008e839714`  
Documentation head supplied for this slice: `1610ee522a39949a3f50a08e08ef3a9a8bcc126c`  
Official OpenFHE pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`

This archive contains a bounded test-only continuation. It does not alter any
production/header/workflow file and does not claim that the proposed tests have
compiled or passed.

## Patch order

Apply from the exact selected `project/` bytes at source `d73824c`:

```bash
git apply --check patches/0001-add-input-first-mult2-regression.patch
git apply patches/0001-add-input-first-mult2-regression.patch

git apply --check patches/0002-sub-input-first-mult2-regression.patch
git apply patches/0002-sub-input-first-mult2-regression.patch
```

The first patch adds only:

- selector `pair_add_input_hybrid_complex`;
- CTest `mult2_pair_add_input_hybrid_complex`;
- the shared bounded fixture/oracle support needed for `(A+B)*C`.

The second patch preserves that Add case and adds only:

- the Sub branch of the narrow fixture/oracles;
- selector `pair_sub_input_hybrid_complex`;
- CTest `mult2_pair_sub_input_hybrid_complex`;
- frozen `A-B` and `(A-B)*C` literals.

`patches/candidate-final.patch` is the aggregate baseline-to-final equivalent.
Do not apply it after the two numbered patches.

## Expected static registration counts

- selected baseline: 53 registered tests;
- after patch 0001: 54 registered tests;
- after patch 0002: 55 registered tests.

These are parsed candidate counts, **not observed CTest results**.

## Changed files

Only these two project paths change:

```text
CMakeLists.txt
tests/mult2_e2e_oracle_test.cpp
```

Complete final versions are under `final-project/`.

## Evidence files

- `REVIEW_AND_DESIGN.md`: source review, oracle derivation, requirement map,
  discrimination and claim boundaries.
- `EXECUTION-LEDGER.md`: executed static checks, supplied CI evidence,
  inferences and NOT EXECUTED items.
- `CMAKE_TEST_CLOSURE.json` / `.md`: all 53 old name-command bindings and the
  two exact additions.
- `HOST_VECTOR_VERIFICATION.json`: exact rational verification of all frozen
  dyadic vectors and equality with the final C++ literals.
- `INPUT_OUTPUT_MANIFEST.json`: input identity and candidate patch/file hashes.
- `STATIC_CHECKS.json` / `.md`: machine-readable and human-readable static check ledger.
- `REQUIRED-MEMBERS.txt`, `OUTPUT-TREE.txt`, `FILE-INVENTORY.json` and
  `MANIFEST.sha256`: output payload closure.

## Hosted execution still required

Codex remains responsible for applying one stage at a time and obtaining Linux
GCC and Windows MinGW64 warning builds, focused CTests and full CTest results.
No OpenFHE/project configure, compilation, encryption, decryption, CTest,
benchmark or CI action was performed in this drafting seat.
