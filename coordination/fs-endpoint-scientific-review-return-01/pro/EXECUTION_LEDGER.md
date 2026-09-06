# Execution ledger

## Environment and boundaries

Review execution used a hosted working container, Python3.13.5, Linux x86_64; exact environment string is in `results/environment.json`. It was not the user's Mac and not either original CI runner. No C++ build, OpenFHE execution, encryption, key generation, NTT, FFT, CI launch/query, Git checkout/provenance reconstruction, repository write, third-party upload, credential access or secret scan was performed. No original input payload changed (`results/input_immutability.json`:217 unchanged).

The original hosted results are **retained evidence**, not executions by this reviewer: run34039088536/attempt1, Linux job101502439156 and Windows101502439304. The checked logs show all six regression groups [1,2,57,1,2,60] passed on each host, one real paper chain each, failed CTest exit8, 39 endpoint records, 24 controls and successful exact two-file uploads. Nine Linux and seven Windows numerical failures remain. The unprefixed failure replay is not a second encrypted chain.

## Actual executed commands and results

The initial working directory was `/mnt/data/fs_review_work`, with safely extracted packet root `input/`. Exact stdout/stderr artifacts listed below are retained in `results/`. Timing files record wall/user/system time; they do not measure encrypted-chain speed.

| Command / operation | Actual result | Retained evidence |
|---|---|---|
| `python3 -B verify_input.py /mnt/data/fs-endpoint-scientific-review-ed5fd192.zip --extract /mnt/data/fs_review_work/input` | Original size/hash, safe regular members, CRC, self-excluding manifest and217 payloads PASS;218 members extracted | `integrity.json` |
| `python3 -B verify_input.py /mnt/data/fs-endpoint-scientific-review-ed5fd192.zip` | PASS with final explicit, optimization-independent validation | `integrity_final.json` |
| `python3 -B -O verify_input.py /mnt/data/fs-endpoint-scientific-review-ed5fd192.zip` | PASS, output identical to unoptimized final verifier | `integrity_optimized.json` |
| From `input/`: `python3 -B project/coordination/fs-endpoint-live-run-01/verify_live_evidence.py linux` | exit0, EVIDENCE_AUDIT_PASS; E80FAIL preserved | `supplied_verify_linux.json`, `.time` |
| Same supplied command with `windows` | exit0, EVIDENCE_AUDIT_PASS; E80FAIL preserved | `supplied_verify_windows.json`, `.time` |
| `python3 -B run_supplied_full_replay.py input linux` | exit0; supplied Decimal256 replay of16,384 rows plus full reconciliation PASS | `supplied_full_replay_linux.json`, `.time` |
| Same driver with `windows` | exit0; supplied Decimal256 replay of16,384 rows plus full reconciliation PASS | `supplied_full_replay_windows.json`, `.time` |
| `python3 -B independent_endpoint_check.py --self-test` |78 exact arithmetic assertions PASS | `independent_self_tests.json` |
| `python3 -B independent_endpoint_check.py input linux` | exit0; independently authored integer-interval recurrence on16,384 rows PASS | `independent_linux.json`, `.time` |
| Same independent command with `windows` | exit0; all16,384 rows PASS | `independent_windows.json`, `.time` |
| `python3 -B -m unittest -v test_independent_endpoint_check` |5 tests PASS: interval arithmetic, canonical numbers, fail-closed source/dispositions, paper sign counterexample, scale compounding | `independent_unit_tests.log` |
| SHA-256 recheck of every extracted manifest payload |217/217 unchanged | `input_immutability.json` |

Final independent interval commands took11.34s wall/Linux data and12.41s/Windows data in the retained timing files. Supplied full replays took5.80s and5.78s wall, respectively. These are local scalar replay times only, not new cryptographic timings. Small revisions to the reviewer's own canonical validator/unit tests were followed by complete replay of the same frozen data; no new key or chain was sampled.

### Self-contained package reproduction

After copying the selected evidence into the return, the exact README commands were executed against `evidence/`, without using the full input directory. Both supplied receipt checks, both full supplied numerical replays, both full independent integer-interval replays and the unit suite passed. All scientific output fields matched the retained results after excluding only the documented elapsed-time fields. Exact argv, exits, stderr and comparison receipts are in `results/package_smoke_linux.json` and `results/package_smoke_windows.json`.

## Reviewer-tool failures and corrections

1. The first review-authored **supplied-replay driver** omitted four required `decode_status` identity arguments. It failed with TypeError before numerical replay. The driver, not any input code, was corrected; both full runs then passed. Retained: `driver_initial_failure.log`.
2. The first independent reader expected a space after FS_ENDPOINT tags and a `stage` key. Actual records use tabs and `index`. It failed the one-sequence check before its full-row loop. Only the reviewer's parser was corrected after inspecting the actual records. Retained: `independent_initial_failure.log` (0.81s). Both hosts then passed, and stricter canonical-number checks were subsequently unit-tested/replayed.
3. An initial combined package-smoke orchestration exceeded the tool's45s wall timeout before emitting its aggregate receipt. Its incomplete results are not counted. There were no surviving child processes. The orchestration was split by host; both independent host receipts now record successful self-contained execution. This was scalar verification of existing captures, not a failed encrypted run.
4. An exploratory `sed` read used the wrong short header path `project/include/double_ckks.h` and failed. The actual header under `project/include/openfhe_2023_1788/` was then located and read.

The initial input verifier used active Python assertions and was run without optimization. Before delivery, reviewer-only assertions were replaced with explicit exceptions; final normal and `-O` runs agreed. No supplied verifier, production source or historical failure was repaired/replaced.

## Reading and source work

The attached ZIP was not searchable through Files: two searches returned no parsed results. The already mounted ZIP was therefore processed directly. TASK.md was read in full after archive identity/safety checks. Source inspection used ordinary `sed`, `nl`, `grep`, file reads and hash comparison; relevant original ranges are enumerated in `FINDINGS.md`. This ledger is not an exhaustive terminal transcript of every read-only source-display command.

The PDF skill was read. The supplied paper PDF was rendered read-only with PyMuPDF. Mathematical pages4,5,6,7,8 and the experimental/table page13 were visually inspected; page11 was also inspected. The paper's publicly accessible ePrint PDF was opened as a primary-source cross-check; attempted web screenshots returned403, so visual adjudication used the exact supplied local PDF. No source/paper was modified or uploaded. The public fetch did not recreate Git or CI provenance.

## Deliberately not done

No independent all-key/security theorem, strict transcendental interval certification, new coefficient/NTT capture, intermediate full-slot residual experiment, statistically meaningful failure-rate estimate, or isolated encoding/encryption-noise decomposition was produced. None is misrepresented by the integer scalar enclosure result.

`PROPOSED_PRECISION_BOUNDARY_V1.json` and the integration RED/GREEN specification are **unadopted proposals**. No corresponding repository change or hosted TDD stage ran. No production patch was authored because no specific production defect was evidenced. CTest61 registration/no-argument behavior/timeout/serial/OMP2, all60 regressions, all five API targets, original E80/A dispositions, inputs/noise/keys/scales and all old artifacts remain unchanged.

## Completion status of this review

All mandatory review checks were completed: archive integrity; exact source/data binding within retained evidence; full16,384-row replay on each host; separately authored arithmetic; same-component signed attribution with allowances; preservation of9/7 misses and Linux witness failure; bounded source/paper adjudication; one explicit next decision; self-contained return verification. This is not a partial endpoint review and not an acceptance of the complete original E80 implementation.
