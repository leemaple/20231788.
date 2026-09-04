# Static check summary

These are executed non-cryptographic checks. Compilation and CTest remain NOT EXECUTED.

| Check | Result |
|---|---|
| `input_archive_exact_size_sha` | `True` |
| `outer_zip_safe_crc` | `True` |
| `outer_manifest_payloads_verified` | `10` |
| `nested_zip_exact_size_sha` | `True` |
| `nested_zip_safe_crc` | `True` |
| `nested_manifest_payloads_verified` | `70` |
| `frozen_dyadic_identities_exact` | `32` |
| `final_cpp_frozen_literals_match_all_7_json_vectors` | `True` |
| `baseline_ctest_names_unique` | `53` |
| `stage1_ctest_names_unique_static_expected` | `54` |
| `stage2_ctest_names_unique_static_expected` | `55` |
| `all_53_old_name_command_bindings_preserved_exactly_and_in_order` | `True` |
| `all_55_commands_resolve_to_15_declared_executable_targets` | `True` |
| `changed_paths_exactly` | `["CMakeLists.txt", "tests/mult2_e2e_oracle_test.cpp"]` |
| `patch1_paths_only` | `[["CMakeLists.txt", "CMakeLists.txt"], ["tests/mult2_e2e_oracle_test.cpp", "tests/mult2_e2e_oracle_test.cpp"]]` |
| `patch2_paths_only` | `[["CMakeLists.txt", "CMakeLists.txt"], ["tests/mult2_e2e_oracle_test.cpp", "tests/mult2_e2e_oracle_test.cpp"]]` |
| `aggregate_paths_only` | `[["CMakeLists.txt", "CMakeLists.txt"], ["tests/mult2_e2e_oracle_test.cpp", "tests/mult2_e2e_oracle_test.cpp"]]` |
| `patch1_deleted_old_test_lines` | `["                     \"hybrid_real|hybrid_complex|bv_real|bv_complex\\n\";"]` |
| `patch2_deleted_stage1_test_lines` | `["                     \"pair_add_input_hybrid_complex\\n\";"]` |
| `old_assertion_case_vector_threshold_body_deletions` | `0` |
| `git_diff_check_patch_generation` | `PASS (executed before this script)` |
| `numbered_replay_all_project_files_byte_equal` | `True` |
| `aggregate_replay_all_project_files_byte_equal` | `True` |
| `cpp_static_lexical_delimiter_balance` | `{"ok": true, "remaining": [], "terminal_state": "code"}` |
| `new_expected_path_forbidden_shipping_oracle_calls_absent` | `True` |
| `fixture_eval_mult_key_generation_present` | `True` |
| `fixture_eval_mult_key_clear_absent` | `True` |
| `first_mult2_only_one_direct_call_in_shared_new_runner` | `True` |
| `baseline_logs_contain_supplied_53_of_53_totals` | `True` |
| `baseline_workflow_contains_both_host_api_target_builds` | `True` |
| `baseline_mult2_e2e_target_warning_as_error_unchanged` | `True` |
| `compile_configure_ctest_crypto_benchmark` | `NOT EXECUTED` |
| `input_archive_post_review_size_sha_unchanged` | `True` |
| `delivery_zip_fresh_extract_crc_safe_member_manifest_inventory_checks` | `True` |

See `EXECUTION-LEDGER.md` for ownership and claim boundaries.
