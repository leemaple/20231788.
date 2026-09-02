# Relin2 Codex fallback HYBRID A-vector-length green coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the malformed HYBRID A-vector-length guard is green on Linux;
HYBRID B length, basis, format, and Relin2 arithmetic remain unimplemented**.

## Implementation and hosted boundary

- implementation branch/commit: `agent/codex-relin2-01` /
  `3645c4e02d3fb1eca4496b1de9be1ebb5517bac9`;
- parent/tree: `b50448f561ba62f9a6271779f5c1eeb4903cac4e` /
  `b1d0f395c59d01b3a2e4e78e3d10856dd746e687`;
- run/job: `33573158385` / Linux `100071272960`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33573158385`.

Only `src/double_ckks.cpp` changed. Four production lines after the accepted
concrete-subtype guard compare the already cast key's public A-vector length
with `GetNumPartQ()` only for HYBRID and emit the stable diagnostic on mismatch.
The change does not read B or an A entry, inspect basis/format, clone or raise a
ciphertext, invoke key switching/relinearization, begin arithmetic, mutate the
cache, or add production `try`/`catch`; the old scaffold remains next. Three
source reviews and three hosted-evidence reviews returned `PASS`.

Linux built warning-clean, compiled the public API contract, and passed exactly
`15/15` in 0.16 seconds. The new negative case received its exact diagnostic
and completed all immediate Tensor/deep-metadata plus cache-map/vector/key-
pointer/context/tag/subtype/A/B invariance checks and RAII restoration. The
inherited real-key positive control still reached the scaffold and completed
its postchecks. Windows was cancelled during official toolchain installation
and makes no project-test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-3645c4e` /
  `aeb30643427e4a345b1e4584ba1016f1cf6b2ed7` /
  `a69b4bdfdfa76abf667f0c8199acdc5ba9db2148`;
- 19-entry manifest SHA-256:
  `1240ad932319d1d267a5a11f447d7222416f937904a87621673d4203ac6e9590`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `45ec5dbbfae00defdbff44739946e5d14f0b26a86a2ced07c15c6ad60c18afee` /
  `bfea66c8c22510a3eb7a0ed9a7b02b0e637671be50c1c831112dfa622457b09a` /
  `1c5e1b3b1b6e7f669809013fc236bb76d8cc2066bb667182a413348f3a8ba7c1`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans. The
next isolated boundary is a valid HYBRID key whose B-vector length alone is
malformed, rejected before basis/format, raising, key switching, or arithmetic.
