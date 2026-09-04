# Exact CMake test-name/command closure

Tested baseline source: `d73824c2d382013c3aadbd7cb29c57008e839714`

These counts and bindings were parsed statically from the supplied and proposed
CMake files. They are not observed CTest results for the candidate.

## Count closure

| Stage | Static registration count | CMake SHA-256 |
|---|---:|---|
| Supplied baseline | 53 | `99b27c4bd0f6374e510fe9011b4cf6b8bebf9c09dca8661600a24614d3b4ea09` |
| After patch 0001 | 54 | `05bacc8ce51ab6514da67f17a94dce68cdbc4db91613da1b58c654d36adce115` |
| After patch 0002 | 55 | `bf27cc6e2514049019671bbc8bc2461e65e0b63431dddd3a1b0194fa020180fa` |

All baseline entries remain identical and in the same order after patch 0001:
**true**.

All baseline entries remain identical and in the same order after patch 0002:
**true**.

## Exact 53 supplied bindings

| # | CTest name | Command |
|---:|---|---|
| 1 | `dcp_rcb` | `dcp_rcb_test` |
| 2 | `pair_add_runtime_behavior` | `pair_add_test` |
| 3 | `pair_sub_runtime_behavior` | `pair_sub_test` |
| 4 | `pair_arithmetic_controlled_oracle` | `pair_arithmetic_test controlled_oracle` |
| 5 | `pair_arithmetic_public_lifecycles_keyless` | `pair_arithmetic_test public_lifecycles_keyless` |
| 6 | `pair_arithmetic_compatibility_rejections` | `pair_arithmetic_test compatibility_rejections` |
| 7 | `tensor2_valid_arithmetic_immutability` | `tensor2_test valid_arithmetic_immutability` |
| 8 | `tensor2_result_scale_contract` | `tensor2_test result_scale_contract` |
| 9 | `tensor2_right_input_validation` | `tensor2_test right_input_validation` |
| 10 | `tensor2_mutual_compatibility` | `tensor2_test mutual_compatibility` |
| 11 | `tensor2_prearithmetic_key_compatibility` | `tensor2_test prearithmetic_key_compatibility` |
| 12 | `relin2_tensor_validation_order` | `relin2_test tensor_validation_order` |
| 13 | `relin2_insufficient_active_basis` | `relin2_test insufficient_active_basis` |
| 14 | `relin2_missing_eval_key` | `relin2_test missing_eval_key` |
| 15 | `relin2_key_empty` | `relin2_test key_empty` |
| 16 | `relin2_key_null_first` | `relin2_test key_null_first` |
| 17 | `relin2_key_wrong_context` | `relin2_test key_wrong_context` |
| 18 | `relin2_key_wrong_tag` | `relin2_test key_wrong_tag` |
| 19 | `relin2_key_wrong_subtype` | `relin2_test key_wrong_subtype` |
| 20 | `relin2_key_hybrid_a_length` | `relin2_test key_hybrid_a_length` |
| 21 | `relin2_key_hybrid_b_length` | `relin2_test key_hybrid_b_length` |
| 22 | `relin2_key_hybrid_entry_basis` | `relin2_test key_hybrid_entry_basis` |
| 23 | `relin2_key_hybrid_entry_format` | `relin2_test key_hybrid_entry_format` |
| 24 | `relin2_key_bv_zero_digit_a_length` | `relin2_test key_bv_zero_digit_a_length` |
| 25 | `relin2_key_bv_zero_digit_b_length` | `relin2_test key_bv_zero_digit_b_length` |
| 26 | `relin2_key_bv_nonzero_digit_a_length` | `relin2_test key_bv_nonzero_digit_a_length` |
| 27 | `relin2_key_bv_nonzero_digit_b_length` | `relin2_test key_bv_nonzero_digit_b_length` |
| 28 | `relin2_key_bv_zero_digit_entry_basis` | `relin2_test key_bv_zero_digit_entry_basis` |
| 29 | `relin2_key_bv_nonzero_digit_entry_basis` | `relin2_test key_bv_nonzero_digit_entry_basis` |
| 30 | `relin2_key_bv_zero_digit_entry_format` | `relin2_test key_bv_zero_digit_entry_format` |
| 31 | `relin2_key_bv_nonzero_digit_entry_format` | `relin2_test key_bv_nonzero_digit_entry_format` |
| 32 | `relin2_valid_arithmetic_state_immutability` | `relin2_test valid_arithmetic_state_immutability` |
| 33 | `relin2_controlled_witnesses_and_boundaries` | `relin2_test controlled_witnesses_and_boundaries` |
| 34 | `relin2_representative_public_input` | `relin2_test representative_public_input` |
| 35 | `relin2_key_extra_later_valid` | `relin2_test key_extra_later_valid` |
| 36 | `relin2_key_malformed_later_ignored` | `relin2_test key_malformed_later_ignored` |
| 37 | `relin2_hybrid_valid_shapes` | `relin2_test hybrid_valid_shapes` |
| 38 | `relin2_bv_zero_digit_valid_shapes` | `relin2_test bv_zero_digit_valid_shapes` |
| 39 | `relin2_bv_nonzero_digit_valid_shapes` | `relin2_test bv_nonzero_digit_valid_shapes` |
| 40 | `relin2_first_recombined_rcb_validation` | `relin2_test first_recombined_rcb_validation` |
| 41 | `relin2_first_recombined_tensor2_validation` | `relin2_test first_recombined_tensor2_validation` |
| 42 | `relin2_tensor2_requires_first_lifecycle` | `relin2_test tensor2_requires_first_lifecycle` |
| 43 | `rs2_wrong_lifecycle` | `rs2_test wrong_lifecycle` |
| 44 | `rs2_valid_arithmetic_state_immutability` | `rs2_test valid_arithmetic_state_immutability` |
| 45 | `rs2_mixed_tower_format` | `rs2_test mixed_tower_format` |
| 46 | `rs2_untouched_public_pipeline` | `rs2_test untouched_public_pipeline` |
| 47 | `rs2_declared_basis_mismatch` | `rs2_test declared_basis_mismatch` |
| 48 | `rs2_terminal_rejections` | `rs2_test terminal_rejections` |
| 49 | `mult2_composition_contract` | `mult2_test composition_contract` |
| 50 | `mult2_e2e_hybrid_real` | `mult2_e2e_oracle_test hybrid_real` |
| 51 | `mult2_e2e_hybrid_complex` | `mult2_e2e_oracle_test hybrid_complex` |
| 52 | `mult2_e2e_bv_real` | `mult2_e2e_oracle_test bv_real` |
| 53 | `mult2_e2e_bv_complex` | `mult2_e2e_oracle_test bv_complex` |

## Proposed additions

| Stage | CTest name | Command |
|---|---|---|
| `patch 0001` | `mult2_pair_add_input_hybrid_complex` | `mult2_e2e_oracle_test pair_add_input_hybrid_complex` |
| `patch 0002` | `mult2_pair_sub_input_hybrid_complex` | `mult2_e2e_oracle_test pair_sub_input_hybrid_complex` |

## Target closure

Declared executable targets: **15**.

```text
dcp_rcb_test
tensor2_api_contract_test
relin2_api_contract_test
rs2_api_contract_test
mult2_api_contract_test
add_api_contract_test
sub_api_contract_test
pair_add_test
pair_sub_test
pair_arithmetic_test
tensor2_test
relin2_test
rs2_test
mult2_test
mult2_e2e_oracle_test
```

Every command in the final 55-registration candidate resolves to one of those
targets: **true**.

The complete machine-readable parse is `CMAKE_TEST_CLOSURE.json`.
