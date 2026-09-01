# Relin2 Codex fallback wrong-actual-key-tag green coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **nonnull, same-context first-key/wrong-actual-tag guard green on
Linux; subtype, A/B shape/basis/format, and Relin2 arithmetic remain
unimplemented**.

## Implementation and hosted boundary

- implementation branch/commit: `agent/codex-relin2-01` /
  `4bfe4fcf5d2c095eb99cdfd9f1df59f261b0b72d`;
- parent/tree: `82c9fa972e528a181a38fda4f3d71b9210547357` /
  `7ced7f73590dcd698a55e49b8305be955805876f`;
- run/job: `33567914096` / Linux `100055212547`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33567914096`.

Only `src/double_ckks.cpp` changed, adding three lines immediately after the
accepted context guard: compare the first key pointee's public `GetKeyTag()`
value with the Tensor tag and reject mismatch with the stable diagnostic. It
does not inspect subtype/A/B material, clone or raise a ciphertext, invoke key
switching, begin arithmetic, or add production `try`/`catch`; the old scaffold
remains next. Three source reviews and three final evidence reviews returned
`PASS`. Linux built warning-clean, compiled the public API contract, and passed
exactly `13/13` tests in 0.13 seconds. The new passing case executed the
immediate Tensor/deep-metadata and map/vector/key identity, context, tag, A/B
invariance checks and proved cache restoration. Windows was cancelled while
installing the official toolchain and makes no project-test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-4bfe4fc` /
  `75aa5640c1349433169289877f070c2e36275c1d` /
  `48c61bd7a98096f50c41259b961b665d62cf6824`;
- 19-entry manifest SHA-256:
  `3ecf78402d8b5733dfdb99e5a44ece81052016722c5dfa470af2708f42a1245a`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `47ab12f1a9b31c045c364b9fed10a54e893ba9e7733ebc74605501e524d3c5ea` /
  `d34d8e1ae8465f946463bf6bfe30bf0e3ce6707136bba8180b17c9cbe8dd65c9` /
  `e857834688d70a59255b9716d8d524d56a7b4287a66d532097f3653f8e6ffe6b`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans. The
next boundary must keep map row, pointer, context, and actual tag valid and
reject the wrong concrete evaluation-key subtype before any A/B getter.
