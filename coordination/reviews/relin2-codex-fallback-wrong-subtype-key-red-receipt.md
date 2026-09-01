# Relin2 Codex fallback wrong-concrete-subtype red coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **valid-row/nonnull/same-context/same-tag but wrong concrete key
subtype red accepted; production remains byte-identical to the preceding
wrong-tag green**.

## Implementation and hosted boundary

- implementation branch/final red commit:
  `agent/codex-relin2-01` /
  `fafe3850d7de4b6e725eed707f32865ff0d2de7d`;
- parent/test-introduction commit:
  `143b62489a3bf1dc29075a3ce16071b822bce350`;
- previous accepted green:
  `4bfe4fcf5d2c095eb99cdfd9f1df59f261b0b72d`;
- final red tree: `51168b6f4759326cbf66f75c949b54f742da7a43`;
- run/job: `33570184376` / Linux `100062228391`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33570184376`.

The cumulative boundary changes only CMake and the Relin2 test. The fourteenth
CTest first generates a valid real `EvalKeyRelinImpl` positive control, permits
the current exact scaffold or a future normal return, and executes complete
Tensor/deep-metadata plus map/vector/key context/tag/A/B invariance checks. It
then inserts one exact public base `EvalKeyImpl` with a valid map row, nonnull
pointer, bound context, and expected actual tag. The negative control never
calls the base type's throwing A/B getters. Two RAII scopes independently
restore the initially empty cache.

Initial commit `143b624` and run `33569890990`, Linux job `100061330826`, were
an unintended compile red: `-Werror=address` rejected a direct `what()` pointer
comparison with a string literal. That is not the behavioral red and makes no
CTest claim. Commit `fafe385` changed only that comparison to `std::string`.
The accepted run then built warning-clean, compiled the public API contract,
and produced exact `13/14` in 0.20 seconds with inherited 13/13 green. The real
key positive control first reached and accepted the old scaffold; only the
negative control treated that scaffold as its failing observation. CTest exit
was `8`.
Three read-only reviews returned `PASS`. Windows was cancelled during official
toolchain installation and makes no project-test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-fafe385` /
  `eea9f1a7571cb5ec375599468a7f8d31c903d82b` /
  `bb8bf21299a0e27d2fe842854b5f3863e932a9b3`;
- 22-entry manifest SHA-256:
  `698c0a9154e44b465b81fef0a32e9e095491e60fbc5b306cf2c4e25f27fb7c39`;
- receipt / complete-logs ZIP / accepted Linux log / pre-red compile log
  SHA-256:
  `b092b2cd2d90cd8675712bcb192de6c9ffebbdcf48fe0cfd2133a33596f2c25e` /
  `b0358235e8b80375192d63fe7154d78d15f7cb7d8fcf12edf831af7bef37e6cb` /
  `011f7406367470bf25d764258420ce4a5b159f072b230c50a9b79899f98e41f8` /
  `9c52dfa49ef1c1eafec762ec151428218762152bb04bc1e39e1852f7f5345fe0`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans.
