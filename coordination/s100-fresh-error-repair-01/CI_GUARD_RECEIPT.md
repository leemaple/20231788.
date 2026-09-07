# CI diagnostic branch exclusion, 2026-09-07

Baseline155b40fbb312d3f000e0bc6d72685c11185720b2. Owner /root/s100_ci_guard, requested gpt-5.6-sol/high, backend requested-unverified; root independently inspected the entire checker and eight-line workflow diff and reran GREEN. No provider-diversity or cryptographic review claim.

Changed only the conditions for Build paper full eight-square contract, Run and finalize paper endpoint once, Select exact endpoint upload and Upload exact endpoint evidence on both hosts. Appended exact exclusions for codex/s100-fresh-error-repair-20260907 and codex/s100-fresh-error-red-20260907. Push allowlist, original branch behavior, previous-status/always() and endpoint-select outcome logic preserved. No new diagnostic target activation yet.

Agent's retained tool result before workflow edit: `python3 coordination/s100-fresh-error-repair-01/check_ci_ref_gates.py`, exit1; Ran4tests in0.007s, FAILED(failures=3). Failing: all_eight_steps_append_both_exact_ref_exclusions, each_single_gate_omission_is_detected, new_refs_skip_all_eight_steps_for_every_status_combination. Existing-ref/status equivalence control passed. Wall-clock timestamp not captured.

After edit agent GREEN4/4. Root independently ran `/Users/lifeng/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/bin/python3 -B -I coordination/s100-fresh-error-repair-01/check_ci_ref_gates.py`:4/4 PASS, unittest0.010s, exit0. `git diff --check`:exit0. Checker uses Ruby/Psych safe YAML parsing and a bounded conjunction evaluator without eval; all8required steps, bothrefs and status combinations, and16single-exclusion omission mutants are covered. It requires the pinned baseline Git object and Ruby; it is a local source-condition check, not an installed CI job or general GitHub expression interpreter.

No CMake build, CTest, OpenFHE operation, GitHub dispatch, full-chain rerun or numerical observation occurred. Original S100 FAIL remains. This patch removes accidental legacy experiment execution while preparing the next focused remote test; it does not establish numerical correctness or complete reproduction.
