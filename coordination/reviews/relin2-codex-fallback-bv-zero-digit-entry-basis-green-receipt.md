# Relin2 Codex fallback BV zero-digit entry-basis TDD closure receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV `digitSize == 0` evaluation-key full ordered-Q entry-basis
guard is green on Linux and Windows; BV nonzero-digit entry basis, BV entry
format, and all Relin2 arithmetic remain unimplemented**.

## Red boundary

- source branch/commit/tree: `agent/codex-relin2-01` /
  `319b85e8f68e0b5f844fff0feac36066606e4e4c` /
  `c56f4563e35f63a7f720cde0c34c9a7d767f2425`;
- parent: `3015f3f14e4bfc457bb702d5f1050a99b7697d90`;
- source diff: only `CMakeLists.txt` and `tests/relin2_test.cpp`, with 237
  insertions and 3 message-only deletions in total;
- run/jobs: `33599676266` / Linux `100150374720` / Windows
  `100150374465`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33599676266`;
- result: both warning-clean project builds and Relin2 public-API builds
  passed; exactly the new twenty-third test failed, producing `22/23` on each
  platform with the old scaffold exception;
- evidence branch/commit/tree: `evidence/relin2-hosted-319b85e` /
  `a2edcfec639292a80c58b2bf38eb0584cea8591e` /
  `057c089cc95aed2d646f2516753e88307c277d45`;
- 19-entry manifest SHA-256:
  `8debb119effb980a6c6a3e1ea97d78a971cf7f539d900dfc1d5b45dd7843c673`.

The fixture uses BV key switching with `digitSize == 0` and binds the expected
basis to `CryptoParametersCKKSRNS::GetElementParams()`: the complete ordered
four-tower Q basis. It proves that every generated A/B entry starts in
Evaluation format on that complete basis.

A durable positive control first replaces only the final B polynomial with an
independently allocated aggregate basis and independently allocated per-tower
parameters that are semantically equal to Q. It reaches the accepted scaffold,
which prevents a pointer-identity implementation from satisfying the test. The
negative control resets from the generated B vector, swaps only its final
entry's first two complete towers, and preserves vector length, format,
cyclotomic order, all unswapped towers, all earlier B entries, A, context, tag,
and subtype. The exact required diagnostic is
`DoubleCKKS: Relin2 evaluation key BV entry basis mismatch`.

Both platforms observed the same wrong-exception red:
`DoubleCKKS: Relin2 is not implemented`. Tests 1 through 22 remained green;
Linux reported 0.25 seconds and Windows 0.32 seconds.

## Green boundary

- source commit/parent/tree: `ed6bdb77f893bb89409059a5e4179686ddbd66d4` /
  `319b85e8f68e0b5f844fff0feac36066606e4e4c` /
  `2372d5aa2a93c7f025e7c1c2ae1df21c341432e7`;
- source diff: only `src/double_ckks.cpp`, 13 insertions;
- run/jobs: `33601661404` / Linux `100156481823` / Windows
  `100156482101`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33601661404`;
- result: warning-clean project build, Relin2 public-API build, and exactly
  `23/23` runtime tests passed on both platforms in 0.15/0.45 seconds;
- evidence branch/commit/tree: `evidence/relin2-hosted-ed6bdb7` /
  `4ee3246cc8f69f0310e1d60fd5882d1c3311383e` /
  `51357c2fff47180226031ca7f7a628836e96470f`;
- 19-entry manifest SHA-256:
  `a00986e5e07c09ef006a3a4d372c115f5f99d103ef8076478e90bc06d5a1a9dc`.

The production change runs only after the accepted BV zero-digit A/B length
checks. For BV `digitSize == 0`, it applies the existing value-semantic
`HasCompleteOrderedBasis` helper to every A and B entry against full Q. Either
vector uses the same exact diagnostic. The guard accepts distinct parameter
objects with equal values and rejects the isolated ordered-basis mutation.

The change deliberately excludes BV nonzero-digit keys, entry format,
ciphertext raising, metadata operations, cache mutation, public
`Relinearize`, and Relin2 arithmetic. It changes no test, public header, or
CMake file and adds no catch block or new abstraction.

The green run executed the independent-parameter positive control and the
malformed-basis negative control. Immediately after each production `Relin2`
call, the test completed Tensor, deep-metadata, A/B polynomial,
cache-map/vector/key-pointer, context, and tag invariance checks. The final
RAII check restored the initially empty global evaluation-key cache.

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

Three independent read-only source reviews passed before the green commit.
After each hosted run, independent Spec, TDD, and Delivery/evidence reviews
returned `PASS` against the frozen evidence bytes.

The BV zero-digit full ordered-Q entry-basis red/green boundary is accepted and
closed. The next isolated TDD boundary is BV nonzero-digit evaluation-key entry
basis. It must begin from
`ed6bdb77f893bb89409059a5e4179686ddbd66d4`, retain a durable
independent-parameter positive control, introduce a separate red before
broadening production, and must not combine entry format, ciphertext raising,
or Relin2 arithmetic.
