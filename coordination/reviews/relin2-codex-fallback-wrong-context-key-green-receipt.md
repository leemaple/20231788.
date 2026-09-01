# Relin2 Codex fallback wrong-context-key-green coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **nonnull-first-key/wrong-context guard green on Linux; actual key tag,
subtype, A/B shape/basis/format, and Relin2 arithmetic remain unimplemented**.

## Implementation and hosted boundary

- implementation branch/commit: `agent/codex-relin2-01` /
  `ba4ca7b4c93fc4d750618418aeeaf1db6b1dd32d`;
- parent/tree: `0a8f84039e61c36548987df2b68153754c519442` /
  `87b4e8e09cdc8d319fec6b71da7f2ad66abd91e4`;
- run/job: `33566113002` / Linux `100049553404`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33566113002`.

Only `src/double_ckks.cpp` changed, adding three lines immediately after the
accepted null guard: compare the first key's public context identity with the
bound module context and reject mismatch with the stable diagnostic. It does
not inspect tag/subtype/A/B material, clone or raise a ciphertext, invoke key
switching, begin arithmetic, or add production `try`/`catch`; the old scaffold
remains next. Three read-only reviews returned `PASS`. Linux built
warning-clean, compiled the public API contract, and passed exactly `12/12`
tests in 0.12 seconds. The new passing case executed the immediate
Tensor/deep-metadata and map/vector/key identity, context, tag, A/B invariance
checks and proved cache restoration. Windows was cancelled while installing
the official toolchain and makes no project-test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-ba4ca7b` /
  `820e37676726c1c059bc0db5a56d0fa7888e8485` /
  `b4cc77982176c7abd3daf48fc4bcfa6112f18373`;
- 19-entry manifest SHA-256:
  `4755af1b6d1b996376c40b3008d075777967e2e6d48a52bb184ff16a90d09710`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `7183e39d27080a274a9afb445f4a6c457b80f80cc5bc2f864c07634b3e76b86a` /
  `227cf50314f2fe6a8b018d67617b446789940efdd613ccb1198ad6b7683ea230` /
  `45a888d8c2afc85e2acf993414b571b5f6f5c93ca74b4eb05f785ba608983fae`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans. The
next boundary must keep context valid and reject a mismatched actual key tag.
