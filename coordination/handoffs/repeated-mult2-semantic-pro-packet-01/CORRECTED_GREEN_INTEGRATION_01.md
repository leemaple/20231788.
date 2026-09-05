# Corrected Repeated Mult2 GREEN candidate integration

Observed 2026-09-05, Asia/Shanghai. Engineering source is `d09f15f535f0dbf22ef89b33255e947166cc392a` on `codex/repeated-mult2-semantic-01`, pushed once. Documentation HEADs are not tested source.

## Gate and exact change

The actual dual-host RED at `7399db55b799a166aee9b72b8f89bcded373b540`, run `33938285334`, was independently accepted before any GREEN application. See `HOSTED_RED_ACCEPTANCE_01.json`, `HOSTED_RED_RESULT_01.md`, and the immutable RED excerpts. The new explicit target failed for the missing production header after all legacy checks; no new semantic pass was claimed.

The original Pro GREEN patch was retained unchanged (SHA-256 `0c9f118fb1034dbdcd3cb01f8da802595c5e0f0eb54f003622eb6d5514b87a27`). It was applied using apply_patch, then only four unsupported high-level CKKS CCParams calls were removed: SetEncryptionTechnique, SetMultiplicationTechnique, SetMultipartyMode, SetThresholdNumOfParties. The required STANDARD/HPS/FIXED_NOISE_MULTIPARTY/1 values are the pinned defaults and remain checked on the actual returned family profiles. Explicit supported low-level construction, PrecomputeCRTTables, COMPLEX, and PRE NOT_SET remain. Two explanatory comment lines are the only other deviation from the original production return.

Only five engineering paths changed from RED: CMakeLists.txt, double_ckks.h, repeated_mult2.h, double_ckks.cpp, repeated_mult2.cpp. CMake adds only the original two target_sources wiring lines. All three frozen RED files and the workflow are byte-identical to RED; all 58 normalized CTest bindings are unchanged. Three other returned production/header files are byte-identical to the original complete return. Exact hashes are in `CORRECTED_GREEN_PREFLIGHT_01.json`.

## Executed static checks

Root checked exact source diff, original-return byte identity, five-path scope, three frozen tests/workflow, CTest binding preservation, and the four-call-only correction. Source git diff --check passed. Gitleaks 8.30.1 scanned src (84,067 bytes) and include (12,643 bytes), with zero findings. No production try/catch was added. No Mac build, cryptographic runtime, or experiment was run.

## Hosted execution identity

The single source push created [run 33940418513](https://github.com/leemaple/20231788./actions/runs/33940418513), attempt 1, at 2026-09-05T02:55:09Z. Linux job 101236605909 and Windows job 101236605855 are the only two jobs. The source is d09f15f535f0dbf22ef89b33255e947166cc392a; pristine OpenFHE pin is df495ba2e91739a6dc8f1de254fc5a41155ce504. No manual dispatch or rerun was performed. Terminal results are recorded separately, not retroactively substituted for this preflight.

## Claim boundary

This candidate implements the frozen N=64 two-operation diagnostic only. Even a dual-host GREEN will not establish N=32768 paper parameters, h=128 integration, production lossless I/O, eight no-refresh squares, 1,000-trial precision, security, or performance. Those remain separate reviewed and tested boundaries. No default-branch merge is authorized by this record alone.
