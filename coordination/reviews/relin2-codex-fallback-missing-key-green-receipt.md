# Relin2 Codex fallback missing-key-green coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **missing-evaluation-key guard green on Linux; key-shape validation and
Relin2 arithmetic remain unimplemented**.

## Implementation and hosted boundary

- implementation branch/commit: `agent/codex-relin2-01` /
  `7c0e94de99c0d6a966f73d78bf50b8c590cfce8c`;
- parent/tree: `b0196dd5349e94a0df0cd0f0750fa7f6819a3498` /
  `a82bcc806b99b09e9b7a8199271a56cf3426b7d3`;
- run/job: `33557603825` / Linux `100022130903`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33557603825`.

Only `src/double_ckks.cpp` changed, adding four lines: bind the process eval-key
map as const, use `find` after complete Tensor/basis validation, and emit the
stable project error when the tag row is absent. There is no `operator[]`,
throwing OpenFHE getter, key/vector access, mutation, clone, or arithmetic; a
present row still reaches the old exact scaffold. Three read-only reviews
returned `PASS`. Linux built warning-clean, compiled the API contract, and
passed exactly `9/9` tests. Windows was cancelled during pristine OpenFHE build
near 10% and makes no project-test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-7c0e94d` /
  `2f986b9eb34bee7c4c888170d960f0cb5ede1ca7` /
  `71fc5dbf8e15a395c9a9238977f3db90bc2376dc`;
- 19-entry manifest SHA-256:
  `b3a685fd62d0b6d17f2cbccbbdc3b7b465ac805037db0fc5a924d17f8c22605e`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `d476140cf7a75731b660e8e69afddea3065290a75691b7236171ce6c7c1eb19c` /
  `1945d68c09e5a5dc64b81b73c6b25ed1d329333583fc5edd67afac5d15810507` /
  `330bd8a9002b35d5e3871a129ee51f8300afeed65a2deab8caa4afe45bd79ba4`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans. The
next boundary must begin with a present-but-empty eval-key-vector red.
