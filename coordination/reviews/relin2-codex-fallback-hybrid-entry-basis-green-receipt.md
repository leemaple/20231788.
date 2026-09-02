# Relin2 Codex fallback HYBRID entry-basis TDD closure receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the complete ordered HYBRID evaluation-key entry-basis guard is
green on Linux and Windows; HYBRID format, BV key shape, and Relin2 arithmetic
remain unimplemented**.

## Red boundary

- source branch/commit/tree: `agent/codex-relin2-01` /
  `3bca03d118625b636dffb4491481dcda5995dccd` /
  `5caebeaa9887111ff5c409eb5dfb54fa88e8da65`;
- run/jobs: `33577480146` / Linux `100084514002` / Windows
  `100084513764`;
- result: warning-clean project build and Relin2 API-contract build passed on
  both platforms; exactly the new test failed, producing `16/17` on each;
- evidence branch/commit/tree: `evidence/relin2-hosted-3bca03d` /
  `06328f8dc975847d8734b613c51dd7007224c62c` /
  `3414406b6ee27caf005e2d79ebb32f55c4731d86`;
- 19-entry manifest SHA-256:
  `2245523b78d36af4394c9df63ba7b2f8341b9da5415589ef8d31703d59653330`.

The frozen seventeenth fixture generated a real public HYBRID
`EvalKeyRelinImpl` with exact two-entry A/B vectors. Its positive control
rebuilt the final B entry with independent aggregate and per-tower parameter
identities while preserving all semantic values; this still reached the old
scaffold. Its negative control swapped only the first two complete towers of
the final B entry, preserving length, Evaluation format, context, tag,
subtype, and all unswapped towers. Both platforms observed the exact old
not-implemented exception instead of the required project diagnostic.

## Green boundary

- source commit/parent/tree: `a90188d0a83c8f138c177ab5a0114eb66e735d8b` /
  `3bca03d118625b636dffb4491481dcda5995dccd` /
  `891792982def9b3c908a1f9e4dcc5bc7868b0b4a`;
- run/jobs: `33579691868` / Linux `100091129476` / Windows
  `100091129136`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33579691868`;
- result: warning-clean project build, Relin2 API-contract build, and exactly
  `17/17` runtime tests passed on both platforms;
- evidence branch/commit/tree: `evidence/relin2-hosted-a90188d` /
  `02ad1ee6d0a02f3e7612736c36d5067958dbd659` /
  `9f46d3beadcbc7feb2265e9c3ae107c85724b5d2`;
- 19-entry manifest SHA-256:
  `61c4612215393ae137b138be903cd8ef201ef11756e5a237e039db9e1b904fbd`.

Only `src/double_ckks.cpp` changed from red. The minimal guard obtains the
bound context's public `ParamsQP`, then checks every A entry and every B entry
for exact tower count plus semantic equality of the aggregate and every
same-index tower parameter. It accepts independently allocated equivalent
parameters and emits only
`DoubleCKKS: Relin2 evaluation key HYBRID entry basis mismatch` for the
malformed basis. It does not inspect format, BV shape, raise ciphertexts, call
key switching/relinearization, or begin arithmetic; valid input still reaches
the old scaffold.

Three read-only source reviews and three frozen evidence reviews returned
`PASS`. Both evidence sets passed ZIP integrity, Gitleaks 8.30.1, independent
targeted scans, manifests, source identity, and remote-ref verification. The
green run executed the positive and negative paths' immediate Tensor,
deep-metadata, A/B polynomial, cache-map/vector/key-pointer, context, tag, and
RAII restoration checks. GitHub action-runtime Node deprecation notices are
disclosed in the green receipt and are not C++ project-build warnings.

The next isolated TDD boundary changes only one otherwise valid HYBRID key
entry from Evaluation to Coefficient format and requires the exact diagnostic
`DoubleCKKS: Relin2 evaluation key HYBRID entry must be in evaluation format`.
