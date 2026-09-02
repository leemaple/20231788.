# Relin2 Codex fallback HYBRID B-vector-length green coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the malformed HYBRID B-vector-length guard is green on Linux; basis,
format, and Relin2 arithmetic remain unimplemented**.

## Implementation and hosted boundary

- implementation branch/commit: `agent/codex-relin2-01` /
  `0e240f37ca22fd683f50a108c40e3e4bfa62f593`;
- parent/tree: `0efab27ea1e2946db94b8621be53c2ecaaa282e8` /
  `cb8d846bb6c070e84aae69cc1e26c2e35353aa7a`;
- run/job: `33574998256` / Linux `100076899535`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33574998256`.

Only `src/double_ckks.cpp` changed. Four production lines after the accepted
HYBRID A-length guard compare the already cast key's public B-vector length
with `GetNumPartQ()` only for HYBRID and emit the stable diagnostic on mismatch.
The change does not read an A/B entry, inspect basis/format, clone or raise a
ciphertext, invoke key switching/relinearization, begin arithmetic, mutate the
cache, or add production `try`/`catch`; the old scaffold remains next. Three
source reviews and three hosted-evidence reviews returned `PASS`.

Linux built warning-clean, compiled the public API contract, and passed exactly
`16/16` in 0.14 seconds. The new negative case received its exact diagnostic
and completed all immediate Tensor/deep-metadata plus cache-map/vector/key-
pointer/context/tag/subtype/A/B invariance checks and RAII restoration. The
malformed-A case still received the A diagnostic, proving A-before-B priority;
the real-key positive control still reached the scaffold and completed its
postchecks. Windows was cancelled during official toolchain installation and
makes no project-test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-0e240f3` /
  `b8d5dc2fe1f07de95f488aee179e97bc40a0c432` /
  `0b7bf0dade67c37834ab47c815921b8b387544a6`;
- 19-entry manifest SHA-256:
  `41b503628ece66c58c5d7a7f97fa4719f89e3c82fd5987d43565c7bf2ae53fce`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `f7969ede5558a242093e76e472755707c8396e40bff153499d7ef9797cf52313` /
  `d3b1289751d8dfadf83b62546012141dad1ed78d0cdefc72fbd70e4656d43af4` /
  `0c76bf7f4cccdde2157f8969f4e686ea8ec5ae977e29b7c73bb3caac498a2a92`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans. The
next isolated boundary keeps both vector lengths and entry formats valid while
making one HYBRID entry's complete `ParamsQP` basis identity wrong.
