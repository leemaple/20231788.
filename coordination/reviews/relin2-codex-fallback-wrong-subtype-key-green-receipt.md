# Relin2 Codex fallback wrong-concrete-subtype green coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **valid-row/nonnull/same-context/same-tag but wrong concrete subtype
guard green on Linux; A/B length/basis/format and Relin2 arithmetic remain
unimplemented**.

## Implementation and hosted boundary

- implementation branch/commit: `agent/codex-relin2-01` /
  `331dd7d64d53e6a87d94555a00aab29b1eaa998e`;
- parent/tree: `fafe3850d7de4b6e725eed707f32865ff0d2de7d` /
  `421b0f6147698d0055e1477c56c63ca3e6e46041`;
- run/job: `33571134323` / Linux `100065111800`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33571134323`.

Only `src/double_ckks.cpp` changed, adding five lines after the accepted tag
guard: dynamically cast the validated first key to public
`EvalKeyRelinImpl<DCRTPoly>` and reject a null cast with the stable diagnostic.
It does not read A/B, inspect technique/basis/format, clone or raise a
ciphertext, invoke key switching, begin arithmetic, mutate the cache, or add
production `try`/`catch`; the old scaffold remains next. Three source reviews
and three evidence reviews returned `PASS`.

Linux built warning-clean, compiled the public API contract, and passed exactly
`14/14` in 0.11 seconds. The real-subtype positive control continued to the
scaffold and completed all immediate Tensor/deep-metadata and cache/key A/B
postchecks. The exact-base negative control received the new diagnostic and
completed all immediate Tensor/deep-metadata plus cache/vector/pointer/context/
tag/concrete-type checks without a base A/B getter. Both RAII scopes restored
the empty cache. Windows was cancelled during official toolchain installation
and makes no project-test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-331dd7d` /
  `783d4715a2afac8911d4ea44d0df3126ef928ed6` /
  `d2f4f735d9288bc0e2d29b26e8e41a9c5dada8a2`;
- 19-entry manifest SHA-256:
  `cac528a5d162707a4a1d54c4f0b3b45e382819cae8e3ef0a041bacd7c94f3c32`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `f3abd5efd5a33a02b3d137a52c71df8273c34af0f1335ce7941c9a21a7e51482` /
  `3a5f94a7e04f6ddc601ccc458f1ed94e5c98a60cc4571bd61241873d8a502092` /
  `b710a3f9f980e3800d09ca2c3343ce010555afc72c80e359a6704bbd98df6173`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans. The
next boundary is a valid HYBRID relinearization key whose A-vector length alone
is malformed, rejected before basis/format, raising, key switching, or
arithmetic.
