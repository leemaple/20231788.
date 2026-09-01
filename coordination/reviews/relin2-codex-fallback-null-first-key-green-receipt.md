# Relin2 Codex fallback null-first-key-green coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **present-vector/null-first-evaluation-key guard green on Linux;
first-key context/tag/subtype/shape validation and Relin2 arithmetic remain
unimplemented**.

## Implementation and hosted boundary

- implementation branch/commit: `agent/codex-relin2-01` /
  `37d1758b515a77e3a6f880f182462e223b2d2bd5`;
- parent/tree: `66d28156a698df78ecdfdd71ce8ddeb513308a7a` /
  `fe314423147bf028b1075070908f47ee8bbcbebc`;
- run/job: `33562728326` / Linux `100038781719`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33562728326`.

Only `src/double_ckks.cpp` changed, adding four lines after the accepted
nonempty-vector guard: bind a const reference to the first shared pointer,
reject null with the stable diagnostic, and otherwise continue to the exact old
scaffold. It does not dereference or clone the key, inspect context/tag/subtype
or A/B vectors, change basis or format, perform arithmetic, mutate cache/Tensor
state, or add production `try`/`catch`. Three read-only reviews returned
`PASS`. Linux built warning-clean, compiled the public API contract, and passed
exactly `11/11` tests, including the immediate Tensor/cache/deep-metadata
invariance checks. Windows was cancelled during pristine OpenFHE build and
makes no project-test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-37d1758` /
  `16db9cf5772b14c66333cee384f44b92b7566032` /
  `81b5bff77538b3b5f510f08069cc3f07b69e631b`;
- 19-entry manifest SHA-256:
  `531e935d1162a438f1fd740ad3dc183a9901bd5a29d10abaa4facd2ef825441f`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `3b70705276fb89ab811dd3691966b450ca33d35be877461f171e4dff806e8fae` /
  `f977452d6a088c349c491ab5c3752f7943ec609def20341269fffcc84136b50d` /
  `f973be1a72b68fad105e734947bcb9af4685a0ccc446256392f14b8c5558cdac`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans. The
next boundary must begin with a nonnull first key from a different
CryptoContext.
