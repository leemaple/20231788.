# Relin2 Codex fallback BV nonzero-digit entry-basis TDD closure receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV `digitSize > 0` evaluation-key full ordered-Q entry-basis
guard is green on Linux and Windows; BV entry format and all Relin2 arithmetic
remain unimplemented**.

## Red boundary

- source branch/commit/tree: `agent/codex-relin2-01` /
  `9c40f801208f27d013d285e18a38fc8c65e41611` /
  `6ca3fd4f0eeff8f8925f3c53a178529ffbcf6c44`;
- parent: `ed6bdb77f893bb89409059a5e4179686ddbd66d4`;
- source diff: only `CMakeLists.txt` and `tests/relin2_test.cpp`, with 248
  insertions and no deletion;
- run/jobs: `33604728646` / Linux `100166014843` / Windows
  `100166014520`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33604728646`;
- result: both warning-clean project builds and Relin2 public-API builds
  passed; exactly the new twenty-fourth test failed, producing `23/24` on each
  platform with the old scaffold exception;
- evidence branch/commit/tree: `evidence/relin2-hosted-9c40f80` /
  `ea40c98b546578811346fcb5da87b09e8424dc90` /
  `466b2d8253b20da95df4a6d6526d36952b6dab34`;
- 19-entry manifest SHA-256:
  `f32c7596e7fb87ee2e7c61d29fe0a00e79aa9f2afbbe0e81800779cfa5cf76da`.

The fixture uses BV key switching with `digitSize == 10`. Pristine OpenFHE
1.5.0's `KeySwitchBV::KeySwitchGenInternal` binds `ep` to
`sNew.GetParams()` and constructs every decomposed A/B digit over that basis
in Evaluation format. The fixture independently observes the complete ordered
four-tower Q bit-width manifest `{35, 31, 30, 31}` and the exact 15-entry A/B
vectors.

A durable positive control first replaces only the final A polynomial with an
independently allocated aggregate basis and independently allocated per-tower
parameters that are semantically equal to Q. It reaches the accepted scaffold,
which prevents a pointer-identity or blanket-BV rejection from satisfying the
test. The negative control resets from the generated A vector, swaps only its
final entry's first two complete towers, and preserves vector length, format,
cyclotomic order, all unswapped towers, all earlier A entries, B, context, tag,
and subtype. The exact required diagnostic is
`DoubleCKKS: Relin2 evaluation key BV entry basis mismatch`.

Both platforms observed the same wrong-exception red:
`DoubleCKKS: Relin2 is not implemented`. Tests 1 through 23 remained green;
Linux reported 0.18 seconds and Windows 0.48 seconds.

## Green boundary

- source commit/parent/tree: `07a7d734b6922f2810218e091b80648cf88dd0c2` /
  `9c40f801208f27d013d285e18a38fc8c65e41611` /
  `29fa87d51bbcf79a715abd7ede4b30c6ba71faf3`;
- source diff: only `src/double_ckks.cpp`, one line replaced;
- run/jobs: `33606983286` / Linux `100173107106` / Windows
  `100173106804`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33606983286`;
- result: warning-clean project build, Relin2 public-API build, and exactly
  `24/24` runtime tests passed on both platforms in 0.24/0.45 seconds;
- evidence branch/commit/tree: `evidence/relin2-hosted-07a7d73` /
  `14b5f22596ed9018082f03b19db70bbfaad1e2be` /
  `b0e01e9826ace60db5772bf102a8c949c738cc83`;
- 19-entry manifest SHA-256:
  `3a5cd28e964fb35a587201a6b1883cb9ca1e99eaf4499e13bcecc92d09da528f`.

The production change broadens only the already accepted BV full ordered-Q
basis guard: its condition changes from BV plus `digitSize == 0` to all BV.
Every A and B entry is checked value-semantically against
`parameters_->GetElementParams()`, with the same exact mismatch diagnostic.
The separate zero- and nonzero-digit vector-length guards are unchanged.

The change adds no helper, catch block, test, public header, format validation,
ciphertext raising, metadata operation, cache mutation, public `Relinearize`
call, or Relin2 arithmetic.

The green run executed the independent-parameter positive control and the
malformed-basis negative control. Immediately after each production `Relin2`
call, the test completed Tensor, A/B polynomial,
cache-map/vector/key-pointer, context, and tag invariance checks. All previous
HYBRID and BV guards remained green, and the final RAII check restored the
initially empty global evaluation-key cache.

## Integrity and decision

For both red and green, the implementation branch, local remote-tracking ref,
and live remote ref were verified equal after non-force pushes. Each evidence
directory contains 20 files: exactly 19 non-manifest files plus one
`MANIFEST.sha256`; every manifest row verifies.

The red complete-logs ZIP has 30 unique, unencrypted, path-safe members; the
green ZIP has 31. Both pass `unzip -t`, and their aggregate Linux/Windows logs
are byte-identical to the separately retained logs. Both evidence sets report
zero uploaded GitHub artifacts and passed Gitleaks 8.30.1 plus independent
targeted credential filename/content scans without retaining matching secret
text.

Three independent read-only source reviews passed before each source commit.
After each hosted run, independent Spec, TDD, and Delivery/evidence reviews
returned `PASS` against frozen evidence bytes. No unresolved algorithm or API
question remained, so this boundary did not require Fable 5.1 escalation.

The BV nonzero-digit full ordered-Q entry-basis red/green boundary is accepted
and closed. The next isolated TDD boundary is BV evaluation-key entry format.
It must begin from `07a7d734b6922f2810218e091b80648cf88dd0c2`, retain
durable positive controls for the accepted basis and format, introduce a
separate red before production changes, and must not combine ciphertext
raising, metadata changes, or Relin2 arithmetic.
