# Hosted commands and expected registration

## Preconditions

- Project source is the exact selected snapshot `c9ee28d0370eeee1ec7a1965402ed0b5e91f425e`, whose active code/tests/workflow match tested `bd141806bd1e0b1dad80c7ad47bfd92fc334db55`.
- OpenFHE is pristine official 1.5.0 at `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Apply `patches/0001-add-first-mult2-high-precision-contract.patch` from the project root.
- Runners: Linux GCC and Windows MinGW64 only. No Mac crypto execution.
- Maximum build parallelism: 2.

## Exact commands

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=<pristine-install>

cmake --build build --parallel 2

cmake --build build --target relin2_api_contract_test --parallel 2
cmake --build build --target rs2_api_contract_test --parallel 2
cmake --build build --target mult2_api_contract_test --parallel 2
cmake --build build --target add_api_contract_test --parallel 2
cmake --build build --target sub_api_contract_test --parallel 2

cmake --build build --target precision_first_mult2_contract_test --parallel 2

ctest --test-dir build \
  -R '^precision_first_mult2_high_precision_contract$' \
  --verbose --output-on-failure

ctest --test-dir build --verbose --output-on-failure
```

The project CMake assigns `/W4 /WX` under MSVC and `-Wall -Wextra -Wpedantic -Werror` otherwise to the new target, matching the existing warning-as-error policy.

## Registration continuity

Current bindings were parsed from the exact supplied `project/CMakeLists.txt`:

```text
54 existing registrations
SHA-256 evidence/CURRENT_54_CTEST_BINDINGS.tsv =
56f93ef463e04b9858ce4ae8006698006382d346bc9344c93c2288218797ef65
```

The candidate has:

```text
55 registrations
one addition:
precision_first_mult2_high_precision_contract
  → precision_first_mult2_contract_test

SHA-256 evidence/CANDIDATE_55_CTEST_BINDINGS.tsv =
3fbd7d2b3441a638463da0f80b18ff84a4827ad53f9d6f0da5c388a8d6d3c27c
```

`evidence/CTEST_CONTINUITY.txt` verifies that every original name/command pair and their relative sequence are unchanged, with zero removals. The new registration is inserted immediately after the accepted DCP→RCB precision contract. Existing API targets and the workflow are not edited.

## Outcome recording

For each host, retain:

1. source commit and exact patch hash;
2. pristine OpenFHE commit;
3. compiler and CMake versions;
4. configure and warning-as-error build logs;
5. five explicit existing API-target build logs plus the new focused target build;
6. focused CTest output, including all four trial lines;
7. complete CTest output and final count;
8. exact values printed for `q_div`, `q_l`, final rational denominator, headroom, coefficient residual, product-delta error, and max-slot error.

Interpretation is fixed:

- A first pass is **first-observed GREEN**, not proof of prior red for this new test.
- A first failure is retained as RED. Do not alter literals, expected products, exact normalization, `2^-80`, trial count, or oracle after observing it.
- The existing two-host 54/54 logs remain precursor evidence. They do not count as execution of this 55th test.
- Inherited BV evidence is reported independently. This patch does not suppress, rewrite, or relabel BV claims.
