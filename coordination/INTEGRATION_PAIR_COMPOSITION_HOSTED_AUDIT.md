# Integration Pair composition: independent hosted audit

Source `7c982519dfedacf5505dbd0f1ca6579ee91da2fd`; [run 33882911345](https://github.com/leemaple/20231788./actions/runs/33882911345), attempt **1**, `push`, `codex/integration-01`: **completed/success**, updated `2026-09-04T14:26:44Z`. Both job metadata and retained in-band `PROJECT_SOURCE_COMMIT / GITHUB_RUN_ID / GITHUB_RUN_ATTEMPT` match exactly. OpenFHE pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

## New-run result

| Host / actual job | Precision focused | Pair focused | Full suite | Default warning-clean build + five API targets |
| --- | --- | --- | --- | --- |
| Linux / 101055635135 | 1/1; 0.27 s | 2/2; 0.17 s | 57/57; 1.19 s | Passed |
| Windows / 101055635468 | 1/1; 0.26 s | 2/2; 0.20 s | 57/57; 2.38 s | Passed |

Matched every actual CTest name, selector COMMAND and index/order to frozen-source `CMakeLists.txt`: focused index 3, focused indices 56–57, then all 57 bindings in order (60 invocations per host). Verified successful default build and actual Relin2, RS2, Mult2, Add and Sub API build commands/completion markers. Frozen CMake applies `-Wall -Wextra -Wpedantic -Werror` to 18 targets; no compiler warning/error found in retained project logs. Workflow, CMake, ordinary oracle and four precision/fixture files matched the frozen commit byte-for-byte; no local compilation was used.

The two JSON files independently classify **16 first-Mult2** records (four focused + four full per host), **eight precursor** records (excluded from that count), **eight Pair** certificates (four per host), and **four BV** certificates. Scientific-decimal observations were parsed directly into BigInt rationals: all 16 first-Mult2 slot and product-delta errors are at most `1/1208925819614629174706176 = 2^-80`. Global worst slot: Windows focused trial 0, `1.0207019984968550582096576801729991728015e-27`; global worst delta: Linux focused trial 2, `5.6718120115719861766604087388765154494645e-28`. First-Mult2 context/scale/headroom fields also match the frozen N=64 diagnostic contract.

All eight Pair records retain `key_switch=HYBRID`, `fixed_key_bv_bound_available=false`, `PER_PATH_CONDITIONAL` and `UNPROVED`. Per-path triangle, execution nonwrap, coefficient-error and bound arithmetic were recomputed exactly; recorded bias and logical-slot errors are below exact `1/1000`. These are N=64, p=30 functional composition cases, not the p=50 high-precision experiment.

All four fixed-key BV arithmetic checks pass. Using the already accepted ordered active moduli, recomputed `Bpath = N * sum(floor(q_i/2) * row_norm_i)`, `Bpair = 2*Bpath`, residual domination, raw integer-lift nonwrap, conservative coefficient bound and final integer-lift inequality. Per-path bounds are Linux real/complex `3813930635264 / 2748778663936`, Windows real/complex `2714419122176 / 3882650148864`. All four preserve the conditional fixed-key status, zero raised-high digit and passed centered-digit probe; observed combined additivity is **false** in all four and remains recorded. This is not an all-key or unconditional Gaussian-key theorem.

## Retained evidence

Paths below are relative to the repository root; sizes are bytes, digests SHA-256.

| File | Bytes | SHA-256 |
| --- | ---: | --- |
| `artifacts/tdd/integration-pair-composition/linux.txt` | 79488 | `bdef68e5fa702f2a5146e3ae63a82c46f8b1f9a9dcbdb6f744c8c97a8e8888bd` |
| `artifacts/tdd/integration-pair-composition/windows.txt` | 83783 | `7f02e518c654aa7e5ab29a92ee254d724d7501bae8a7e25dd2db78c51461ad0d` |
| `coordination/evidence/integration-pair-composition/linux-7c98251.json` | 72816 | `d16a5d0b05f5f487032e9dec498a77c8389fe0fe2ad8342a645c1aa9332d4017` |
| `coordination/evidence/integration-pair-composition/windows-7c98251.json` | 71040 | `403e4017ef439c4b91f2e1f6057b125fe536dad5d9a6123ebaed334bcfceac6d` |

Read-only commands: `gh run watch 33882911345 --repo leemaple/20231788. --interval 50 --exit-status`; GET `gh api repos/leemaple/20231788./actions/runs/33882911345`, `.../actions/jobs/101055635135`, `.../actions/jobs/101055635468`, and each job's `/logs`; `git show 7c982519dfedacf5505dbd0f1ca6579ee91da2fd:<path>`; bounded Node BigInt/log inspection; final SHA-256/byte checks. Observation began `14:19:37Z`, deadline `14:34:37Z`; success observed `14:32:13Z`, within 15 minutes. No dispatch, restart, cancel, source edit, stage, commit or push.

Logs start at the group containing the project configure command and end before Linux Post job cleanup / Windows orphan cleanup. ANSI, CR and trailing horizontal whitespace were normalized; no internal lines were dropped. Linux's nonfatal Node 20 deprecation advisory is preserved; Windows has no such advisory. Metadata omits unnecessary account/runner identity. This is static verification of new hosted observations, not local cryptographic execution or a re-execution of canonical evaluation. It does not replace older branch/integration evidence or establish paper parameters, repeated multiplication, public I/O, security or performance claims.
